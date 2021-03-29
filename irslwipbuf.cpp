#include <irspch.h> 

#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irslwipbuf.h>

namespace irs
{

lwipbuf::lwipbuf(size_t sizebuf, u16_t port)
  : m_sizebuf(sizebuf)
  , m_port(port)
  , m_buffer(m_sizebuf)
  , m_temp_buffer(m_sizebuf*2)
{
  char_type* buf = m_buffer.data();
  setp(buf, buf + m_buffer.size());
  
  if (init() < 0) { throw "Cannot init lwipbuf!"; } 
}

lwipbuf::~lwipbuf()
{}

int lwipbuf::init()
{
    /* Create new tcp pcb */
  mp_tcp_pcb = tcp_new();
  
  if (mp_tcp_pcb != NULL) {
    /* Bind echo_pcb to port (ECHO protocol) */
    err_t err = tcp_bind(mp_tcp_pcb, IP_ADDR_ANY, m_port);
    
    if (err == ERR_OK) {
      /* Start tcp listening for echo_pcb */
      mp_tcp_pcb = tcp_listen(mp_tcp_pcb);
      
      /* Set buffer up as arg in order to use it in callback functions */
      tcp_arg(mp_tcp_pcb, reinterpret_cast<void*>(&m_connections));
        
      /* Initialize LwIP tcp_accept callback function */
      tcp_accept(mp_tcp_pcb, c_tcp_accept);
      
      return 0;
    } else {
#ifdef USE_LCD
      LCD_UsrTrace("  Error [tcpInit]: tcp_bind was failed\n");
#endif // USE_LCD
      
      /* deallocate the pcb */
      memp_free(MEMP_TCP_PCB, mp_tcp_pcb);
      
      return -1;
    }
  }
  
  return -1;
}

void lwipbuf::tick(struct netif* ap_netif)
{
    ethernetif_input(ap_netif);
    
    /* Handle timeouts */
    sys_check_timeouts();
    
#if LWIP_NETIF_LINK_CALLBACK
    Ethernet_Link_Periodic_Handle(ap_netif);
#endif

#if LWIP_DHCP
    DHCP_Periodic_Handle(ap_netif, m_port);
#endif
}

void lwipbuf::send(const void* ap_msg, size_t size_msg)
{
#ifdef USE_LCD
  if (m_connections.size() == 0) {
    LCD_UsrTrace("  Warning: There is no active connection!\n");
  }
#endif // USE_LCD
  
  for (int i = m_connections.size() - 1; i >= 0; i--) {
    err_t err = tcp_write(m_connections[i], 
                    ap_msg,
                    size_msg,
                    TCP_WRITE_FLAG_COPY);
    
    if (err == ERR_OK) { 
      err = tcp_output(m_connections[i]);
    }
    
    if (err != ERR_OK && err != ERR_MEM) {
      c_tcp_connection_close(m_connections[i]);
      m_connections.erase(m_connections.begin() + i);
    }
  }
}  

u16_t lwipbuf::get_port() const
{ return m_port; }

bool lwipbuf::is_any_connected() const
{ return m_connections.size() > 0; }

size_t lwipbuf::get_count_connected() const
{ return m_connections.size(); }

int lwipbuf::overflow(int c) _OVERRIDE_
{
  ptrdiff_t sz = pptr() - pbase();

  for (size_t i = 0; i < sz; i++) {
    if (m_buffer[i] == '\n') { m_temp_buffer.push_back('\r'); }
    m_temp_buffer.push_back(m_buffer[i]);
  }
  
  send(&m_temp_buffer.front(), m_temp_buffer.size());

  pbump(-sz);
  m_temp_buffer.clear();
  
  if (c != EOF) { m_buffer.push_back(char(c)); }
  
  return c == EOF;
}

int lwipbuf::sync() _OVERRIDE_
{ return pptr() == pbase() ? 0 : overflow(); }

err_t lwipbuf::c_tcp_accept(void* arg, struct tcp_pcb* newpcb, err_t err)
{
  arr_conn_type* conns = reinterpret_cast<arr_conn_type*>(arg);
  err_t ret_err;
  
  LWIP_UNUSED_ARG(err);

  /* Set priority for the newly accepted tcp connection newpcb */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /* Pass newly allocated es structure as argument to newpcb */
  tcp_arg(newpcb, arg);
  
   /* initialize lwip tcp_recv callback function for newpcb  */ 
  tcp_recv(newpcb, c_tcp_recv);
    
  /* Initialize lwip tcp_poll callback function for newpcb */
//  tcp_poll(newpcb, c_tcp_poll, 0);
  
  conns->push_back(newpcb);
    
#ifdef USE_LCD
    LCD_UsrTrace("  State: A new connection has been set up!\n");
#endif // USE_LCD
    
  ret_err = ERR_OK;
  
  return ret_err; 
}

err_t lwipbuf::c_tcp_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, 
                          err_t err)
{
  arr_conn_type* conns = reinterpret_cast<arr_conn_type*>(arg);
  
  /* If a received package is NULL then the client disconnected.
   * So close connection session
   */
  if (p == NULL) {
    c_tcp_connection_close(tpcb);
    
    arr_conn_type::iterator iter = find(conns->begin(), conns->end(), tpcb);
    if (iter != conns->end()) { conns->erase(iter); }
  }
  
  return ERR_OK;
}

err_t lwipbuf::c_tcp_poll(void* arg, struct tcp_pcb* tpcb)
{
  arr_conn_type* conns = reinterpret_cast<arr_conn_type*>(arg);
  err_t ret_err = ERR_OK;
  
  return ret_err;
}

void lwipbuf::c_tcp_connection_close(struct tcp_pcb* tpcb)
{
  /* Remove all callbacks */
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  /* Close tcp connection */
  tcp_close(tpcb);
  
#ifdef USE_LCD
    LCD_UsrTrace("  State: A connection is closed!\n");
#endif // USE_LCD
}

} // namespace irs
