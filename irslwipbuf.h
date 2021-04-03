#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define USE_LCD

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
    
inline void cp1251_to_utf8(char *out, const char *in);
  
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
  
  int_type overflow(int_type c, const char* buffer, size_t sz);
  
  /* LWIP callback functions */
  static err_t c_tcp_accept(void* arg, struct tcp_pcb* newpcb, err_t err);
  static err_t c_tcp_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, 
                          err_t err);
  static err_t c_tcp_poll(void* arg, struct tcp_pcb* tpcb);
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
{
  m_buffer.resize(m_sizebuf);
  m_temp_buffer.resize(m_sizebuf*2);
  
  m_convert_buffer = new char[m_sizebuf*2]();
//  m_convert_buffer = typeid(char_type) != typeid(char) 
//    ? new char[m_sizebuf]() : nullptr;

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
lwipbuf<char_type, traits_type>::overflow(int_type c,
                                          const char* buffer,
                                          size_t sz)
{
  size_t j = 0;
  for (size_t i = 0; i < sz; i++) {
    if (buffer[i] == '\n') { m_temp_buffer[j++] = '\r'; }
    m_temp_buffer[j++] = buffer[i];
  }
  
  send(&m_temp_buffer.front(), j);
  
  this->pbump(-sz);
  m_temp_buffer.clear();
  m_buffer.clear();

  if (c != traits_type::eof()) { m_buffer[0] = char(c); }
  
  return c == traits_type::eof();
}

template<>
lwipbuf<wchar_t>::int_type lwipbuf<wchar_t>::overflow(int_type c) _OVERRIDE_
{
  ptrdiff_t sz = this->pptr() - this->pbase();

  if (typeid(char_type) != typeid(char)) {
    wcstombs(m_convert_buffer, m_buffer.data(), sz); 
  }
  
  return this->overflow(c, m_convert_buffer, sz);
}

template<typename char_type, typename traits_type>
lwipbuf<char_type, traits_type>::int_type 
lwipbuf<char_type, traits_type>::overflow(int_type c) _OVERRIDE_
{
  ptrdiff_t sz = this->pptr() - this->pbase();
  
  cp1251_to_utf8(m_convert_buffer, &m_buffer.front());
  
  return this->overflow(c, m_convert_buffer, sz*2);
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

inline void cp1251_to_utf8(char *out, const char *in) {
    static const char table[128*3 + 1] = {                 
        "\320\202 \320\203 \342\200\232\321\223 \342\200\236\342\200\246\342\200\240\342\200\241"
        "\342\202\254\342\200\260\320\211 \342\200\271\320\212 \320\214 \320\213 \320\217 "      
        "\321\222 \342\200\230\342\200\231\342\200\234\342\200\235\342\200\242\342\200\223\342\200\224"
        "   \342\204\242\321\231 \342\200\272\321\232 \321\234 \321\233 \321\237 "                     
        "\302\240 \320\216 \321\236 \320\210 \302\244 \322\220 \302\246 \302\247 "                     
        "\320\201 \302\251 \320\204 \302\253 \302\254 \302\255 \302\256 \320\207 "                     
        "\302\260 \302\261 \320\206 \321\226 \322\221 \302\265 \302\266 \302\267 "
        "\321\221 \342\204\226\321\224 \302\273 \321\230 \320\205 \321\225 \321\227 "
        "\320\220 \320\221 \320\222 \320\223 \320\224 \320\225 \320\226 \320\227 "
        "\320\230 \320\231 \320\232 \320\233 \320\234 \320\235 \320\236 \320\237 "
        "\320\240 \320\241 \320\242 \320\243 \320\244 \320\245 \320\246 \320\247 "
        "\320\250 \320\251 \320\252 \320\253 \320\254 \320\255 \320\256 \320\257 "
        "\320\260 \320\261 \320\262 \320\263 \320\264 \320\265 \320\266 \320\267 "
        "\320\270 \320\271 \320\272 \320\273 \320\274 \320\275 \320\276 \320\277 "
        "\321\200 \321\201 \321\202 \321\203 \321\204 \321\205 \321\206 \321\207 "
        "\321\210 \321\211 \321\212 \321\213 \321\214 \321\215 \321\216 \321\217 "
    };
    
    while (*in) {
        if (*in & 0x80) {
            const char *p = &table[3 * (0x7f & *in++)];
            if (*p == ' ') { continue; }
            *out++ = *p++;
            *out++ = *p++;
            if (*p == ' ') { continue; }
            *out++ = *p++;
        }
        else {
            *out++ = *in++;
        }
    }

    *out = 0;
}

} // namespace irs

#endif // TCP_SERVER_H