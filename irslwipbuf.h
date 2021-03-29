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
    
/* typedef basic_streambuf<char, char_traits<char> > streambuf; */
class lwipbuf : public streambuf
{
public:
  /* Functions */
  lwipbuf(size_t sizebuf = IRSLIB_LWIPBUF_SIZE, 
          u16_t port = IRSLIB_LOG_PORT);
  virtual ~lwipbuf();
 
  virtual void tick(struct netif* ap_netif);
  virtual void send(const void* ap_msg, size_t size_msg);

  u16_t get_port() const;
  bool is_any_connected() const;
  size_t get_count_connected() const;
  
  virtual int overflow(int c = EOF) _OVERRIDE_;
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
  vector<char> m_buffer;
  vector<char> m_temp_buffer;
  arr_conn_type m_connections;
};

} // namespace irs

#endif // TCP_SERVER_H