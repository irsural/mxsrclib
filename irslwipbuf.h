#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define USE_LCD

#include <codecvt>

#include <irsdefs.h>
#include <irscpp.h>
#include <irsnetdefs.h>

#include <ethernet_h7.h>
#include <lwipopts_conf.h>

#include "netif/etharp.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/err.h"

#include "lwip/stats.h"
#include "lwip/tcp.h"

#if LWIP_DHCP
#include "app_ethernet.h"
#include "lwip/dhcp.h"
#endif

#ifdef USE_LCD

#ifdef __cplusplus
extern "C" {
#endif

#include "lcd_trace.h"

#ifdef __cplusplus
}
#endif
  
#endif

#define _OVERRIDE_

namespace irs
{

#define IRSLIB_LWIPBUF_SIZE 128
    
template<typename char_type, typename traits_type = char_traits<char_type>>
class lwipbuf : public basic_streambuf<char_type, traits_type>
{
public:
  typedef typename traits_type::int_type int_type;
  
  /* Functions */
  lwipbuf(size_t sizebuf = IRSLIB_LWIPBUF_SIZE, 
          u16_t port = IRSLIB_LOG_PORT);
  virtual ~lwipbuf();
 
  virtual void tick(struct netif* ap_netif);
  virtual void send(const void* ap_msg, size_t size_msg);

  u16_t get_port() const;
  bool is_any_connected() const;
  size_t get_count_connected() const;

  virtual int_type overflow(int_type c = traits_type::eof()) _OVERRIDE_;
  virtual int sync() _OVERRIDE_;
 
private:
  typedef vector<struct tcp_pcb*> arr_conn_type; 
  
  /* Functions */
  int init();
  
  /**
  * @brief  This function is the implementation of tcp_accept LwIP callback
  * @param  arg: not used
  * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
  * @param  err: not used 
  * @retval err_t: error status
  */
  static err_t c_tcp_accept(void* arg, struct tcp_pcb* newpcb, err_t err);
 
  /**
  * @brief  This function is the implementation for tcp_recv LwIP callback
  * @param  arg: pointer on a argument for the tcp_pcb connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  pbuf: pointer on the received pbuf
  * @param  err: error information regarding the reveived pbuf
  * @retval err_t: error code
  */
  static err_t c_tcp_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, 
                          err_t err);
  
  /**
  * @brief  This function implements the tcp_poll LwIP callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: pointer on the tcp_pcb for the current tcp connection
  * @retval err_t: error code
  */
  static err_t c_tcp_poll(void* arg, struct tcp_pcb* tpcb);
  
  /**
  * @brief  This functions closes the tcp connection
  * @param  tcp_pcb: pointer on the tcp connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
  static void c_tcp_connection_close(struct tcp_pcb* tpcb);
  
  /* Fields */
  u16_t m_port;
  struct tcp_pcb* mp_tcp_pcb;

  size_t m_sizebuf;
  char* m_convert_buffer;
  vector<char_type> m_buffer;
  vector<char> m_temp_buffer;
  arr_conn_type m_connections;
};

template<typename char_type, typename traits_type>
lwipbuf<char_type, traits_type>::lwipbuf(size_t sizebuf, u16_t port)
  : m_sizebuf(sizebuf)
  , m_port(port)
  , m_buffer(m_sizebuf)
  , m_temp_buffer(m_sizebuf*2)
{
  m_convert_buffer = typeid(char_type) != typeid(char) 
    ? new char[m_sizebuf]() : nullptr;

  char_type* buf = m_buffer.data();
  this->setp(buf, buf + m_buffer.size());
  
  if (init() < 0) { throw "Cannot init lwipbuf!"; } 
}

template<typename char_type, typename traits_type>
lwipbuf<char_type, traits_type>::~lwipbuf()
{
  if (m_convert_buffer) { delete[] m_convert_buffer; } 
  if (mp_tcp_pcb) { memp_free(MEMP_TCP_PCB, mp_tcp_pcb); }
}

template<typename char_type, typename traits_type>
int lwipbuf<char_type, traits_type>::init()
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

template<typename char_type, typename traits_type>
void lwipbuf<char_type, traits_type>::tick(struct netif* ap_netif)
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

template<typename char_type, typename traits_type>
void lwipbuf<char_type, traits_type>::send(const void* ap_msg, size_t size_msg)
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

template<typename char_type, typename traits_type>
u16_t lwipbuf<char_type, traits_type>::get_port() const
{ return m_port; }

template<typename char_type, typename traits_type>
bool lwipbuf<char_type, traits_type>::is_any_connected() const
{ return m_connections.size() > 0; }

template<typename char_type, typename traits_type>
size_t lwipbuf<char_type, traits_type>::get_count_connected() const
{ return m_connections.size(); }

template<typename char_type, typename traits_type>
lwipbuf<char_type, traits_type>::int_type
lwipbuf<char_type, traits_type>::overflow(int_type c) _OVERRIDE_
{
  ptrdiff_t sz = this->pptr() - this->pbase();

  if (typeid(char_type) != typeid(char)) {
    wcstombs(m_convert_buffer, m_buffer.data(), sz); 
  }
  
  for (size_t i = 0; i < sz; i++) {
    if (typeid(char_type) == typeid(char)) {
      if (m_buffer[i] == '\n') { m_temp_buffer.push_back('\r'); }
      m_temp_buffer.push_back(m_buffer[i]);
    } else {
      if (m_convert_buffer[i] == '\n') { m_temp_buffer.push_back('\r'); }
      m_temp_buffer.push_back(m_convert_buffer[i]);
    }
  }    
  
  send(&m_temp_buffer.front(), m_temp_buffer.size());

  this->pbump(-sz);
  m_temp_buffer.clear();
  
  if (c != traits_type::eof()) { m_buffer.push_back(char(c)); }
  
  return c == traits_type::eof();
}

template<>
lwipbuf<char>::int_type lwipbuf<char>::overflow(int_type c) _OVERRIDE_
{
  ptrdiff_t sz = this->pptr() - this->pbase();

  for (size_t i = 0; i < sz; i++) {
      if (m_buffer[i] == '\n') { m_temp_buffer.push_back('\r'); }
      m_temp_buffer.push_back(m_buffer[i]);
  }    
  
  send(&m_temp_buffer.front(), m_temp_buffer.size());

  this->pbump(-sz);
  m_temp_buffer.clear();
  
  if (c != traits_type::eof()) { m_buffer.push_back(char(c)); }
  
  return c == traits_type::eof();
}

template<typename char_type, typename traits_type>
int lwipbuf<char_type, traits_type>::sync() _OVERRIDE_
{ return this->pptr() == this->pbase() ? 0 : overflow(); }

template<typename char_type, typename traits_type>
err_t lwipbuf<char_type, traits_type>::c_tcp_accept(void* arg, 
                                                    struct tcp_pcb* newpcb, 
                                                    err_t err)
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

template<typename char_type, typename traits_type>
err_t lwipbuf<char_type, traits_type>::c_tcp_recv(void* arg, 
                                                  struct tcp_pcb* tpcb, 
                                                  struct pbuf* p, 
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

template<typename char_type, typename traits_type>
err_t lwipbuf<char_type, traits_type>::c_tcp_poll(void* arg, 
                                                  struct tcp_pcb* tpcb)
{
  arr_conn_type* conns = reinterpret_cast<arr_conn_type*>(arg);
  err_t ret_err = ERR_OK;
  
  return ret_err;
}

template<typename char_type, typename traits_type>
void lwipbuf<char_type, traits_type>::c_tcp_connection_close(struct tcp_pcb* tpcb)
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

#endif // TCP_SERVER_H