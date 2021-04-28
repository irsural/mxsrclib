// @brief ����� lwipbuf �������������� ����� �����, ������������ ethernet �
// � �������� �������� ������.
//
// ����: 18.04.2021
// ���� ��������: 12.04.2021

#pragma once

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define USE_LCD

#include <irsdefs.h>
#include <irscpp.h>
#include <irsnetdefs.h>
#include <irsencode.h>
#include <irsstrconv.h>

#include <ethernet_h7.h>
#include <lwipopts_conf.h>

#include <stm32h7xx_hal.h>

#if LWIP_DHCP
#include "app_ethernet.h"
#include "lwip/dhcp.h"
#endif // LWIP_DHCP

#ifdef USE_LCD

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "lcd_trace.h"

#ifdef __cplusplus
}
#endif // __cplusplus
  
#endif // USE_LCD

#include "netif/etharp.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/err.h"

#include "lwip/stats.h"
#include "lwip/tcp.h"

#define _OVERRIDE_

namespace irs
{

/* ������ LWIP ������ �� ��������� */
#define IRSLIB_LWIPBUF_SIZE 15
  
template<typename char_type, typename traits_type = char_traits<char_type>>
class lwipbuf : public basic_streambuf<char_type, traits_type>
{
public:
  typedef typename traits_type::int_type int_type;
  
  /* @brief ����������� �� ���������. �������� ������������ ������ ��� ������,
   * ������ � ���� �������� ������������� ������� � ��� ������.
   * 
   * @param sizebuf - ������ ������. �� ��������� - IRSLIB_LWIPBUF_SIZE (128)
   * @param port - ����, �� ������� ����� ���������� ���������. �� ��������� -
   * IRSLIB_LOG_PORT (5008)
   */
  lwipbuf(size_t sizebuf = IRSLIB_LWIPBUF_SIZE, 
          u16_t port = IRSLIB_LOG_PORT);
  
  /* @brief ���������� �� ���������. ������������ ������������ ������, 
   * ���������� ��� ������.
   */
  virtual ~lwipbuf();
 
  /* @brief ������� "���". �� ���������� �������� � ����� ����� �� ������
   * ��������. ��� ������������ ��������� �������� ������� �� ������ ����� ethernet.
   *
   * @param ap_netif - ������ �� ��������� �������.
   */
  virtual void tick(struct netif* ap_netif);
  
  /* @brief ������� �������� ��������� ��������. ������ � ���� ������� ��������
   * ���������� ����������� ������� ���������; � ������ ������ ������� ������
   * ������� �� ������ ������� ������.
   *
   * @param ap_msg - ������������ ���������.
   * @param size_msg - ����������� ���������.
   */
  virtual void send(const void* ap_msg, size_t size_msg);

  /* @brief ������� ��������� ����� �����������. 
   *
   * @return u16_t - ������� ���� �����������, �� �������� ���������� ��������
   * ���������.
   */
  u16_t get_port() const;
  
  /* @brief ������� �������� ������� ��������, ������������ � ������.
   *
   * @return true - � ������, ���� ���� �������� ����������, false - � ���� ������.
   */
  bool is_any_connected() const;
  
  /* @brief �������, ������������ ���������� �������� ����������.
   *
   * @return size_t - ���������� �������� ����������.
   */
  size_t get_count_connected() const;

  /* @brief �������, ���������� ��� ������������ ������ � ������, ����� ��
   * ���������� ������ �����/������. ������ �-� ������� �����, ��������������
   * ���������� �������� ��������� ��������, ������� ���� �� ������� ������ �
   * ������; ����� ��������� ���������� ������ � �����, ������� �� ����.
   * 
   * @param c - ������, ������� �� ���� � �����. �� ��������� - traits_type::eof()
   * - �������� ������ ��������� ������ (� ������ ������� �����/������).
   *
   * @return 1 - � ������, ���� �������� c = traits_type::eof(), 0 - � ���� ������.
   *
   * @note ������ ������� ����������� �� basic_streambuf<char_type, traits_type>.
   * ��� ���������� �������� ��������� ������� overflow(), ������� ������������
   * ���� ��������� ����������.
   */
  virtual int_type overflow(int_type c = traits_type::eof()) _OVERRIDE_;
  
  /* @brief ������� ������������� �������. � ������ ������ ����� ����������
   * ���������� �������������. � ������, ���� � ������ ���� ������ ��������
   * overflow(), � ���� ������ ���������� 0.
   *
   * @return overflow() - � ������, ���� � ������ ���� ������, � ���� ������ - 0.
   *
   * @note ������ ������� ����������� �� basic_streambuf<char_type, traits_type>.
   */
  virtual int sync() _OVERRIDE_;
 
private:
  typedef vector<struct tcp_pcb*> arr_conn_type;
  
  /* @brief ������� ������������� �������. ������� ������ tcp ���������, � �����
   * callback-������� ������ �������� ���������� (accept-func). � ������, ����
   * ��������� #define USE_LCD � ������������� ��������� ��������, ������� ��
   * ������� ����� ��������������� ���������.
   */
  int tcp_init();
  
  /* @brief ������� ����������� ������ � m_temp_buffer � ����������� '\n', � 
   * ������ ������� ��������� ����� ������ '\n' ������ '\r'.
   *
   * @param ap_str - ������, ������� ���������� �����������.
   *
   * @return size_t - ������������ ������ ������ ������ � ������������ ���������.
   */
  size_t copy_str_to_buffer_with_correct_endls(const char* ap_str);
  
  /* @brief ���������� ������� overflow(), ��������� ��� ��������� �������������
   * �������� ������. ������ ������� ���������� ��� ������������ ������ � ������, 
   * � ������ ������������� ������� �����/������. ������ �-� ������� �����, 
   * �������������� ���������� �������� ��������� ��������, ������� ���� �� 
   * ������� ������ � ������; ����� ��������� ���������� ������ � �����, ������� 
   * �� ����.
   *
   * @param c - ������, ������� �� ���� � �����.
   * @param buffer - �����, �������� ������������ ���������.
   * @param sz - ������ ������������� ������.
   *
   * @return 1 - � ������, ���� �������� c = traits_type::eof(), 0 - � ���� ������.
   */
  int_type overflow(int_type c, const char* buffer, size_t sz);
  
  /* @brief callback-������� ��������� ����������� ������ ����������. 
   * ������������� ���������, � ����� ����������� callback-������� ��� �������� 
   * ����������. � ������, ���� ��������� #define USE_LCD, ������� 
   * ��������������� ��������� �� ������� �����.
   *
   * @param arg - ��������� ������, ���������� �������������, ������� ����� 
   * ������������ �� ��� callback-������� �������� ����������.
   * @param newpcb - ���������� ������ ����������.
   * @param err - �� ������������.
   */
  static err_t c_tcp_accept(void* arg, struct tcp_pcb* newpcb, err_t err);
  
  /* @brief callback-������� ��������� ������� ������ �� �������. 
   * ���������, �������� �� �� �����-���� ������ �� �������. � ������������� ������
   * ������� ���������� �������� ����������, �.�. ��������� ������� �� ����.
   *
   * @param arg - ��������� ������, ���������� �������������, ������� ����� 
   * ������������ �� ��� callback-������� �������� ����������.
   * @param tpcb - ���������� �������� ����������.
   * @param p - �����, � ������� ��������� ���������� ������ �� �������.
   * @param err - �� ������������.
   */
  static err_t c_tcp_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, 
                          err_t err);
  
  /* @brief callback-�������, ���������� � ������������ ��������. � ������ ������
   * �� ������������.
   *
   * @param arg - ��������� ������, ���������� �������������, ������� ����� 
   * ������������ �� ��� callback-������� �������� ����������.
   * @param tpcb - ���������� �������� ����������.
   */
  static err_t c_tcp_poll(void* arg, struct tcp_pcb* tpcb);
  
  /* @brief callback-�������, ��������� ���������� ����������, �.�. �����������
   * ������� �� �������.
   *
   * @param tpcb - ���������� ����������, �-�� ���������� �������.
   */
  static void c_tcp_connection_close(struct tcp_pcb* tpcb);
  
  /* ����, �� �-��� �������������� �������� ���������. */
  u16_t m_port;
  
  /* ��������� ���������� ����������. */
  struct tcp_pcb* mp_tcp_pcb;

  /* ����������� �������� ������, � ������� ���������� ���������� ������ � ������
   * ������������� ������� �����/������. 
   */
  size_t m_sizebuf;
  
  /* �����, ������������ ��� �������� ������ �� ������ ��������� � ������.
   * �� ��������� ����������� ������� ������ ��������� m_sizebuf*2. � ������
   * ������������� > UTF-8 ����������� �������� m_sizebuf * sizeof(char_type).
   */
  char* m_convert_buffer;
  
  /* �������� ������, � ������� ���������� ���������� ������ � ������
   * ������������� ������� �����/������. 
   */
  vector<char_type> m_buffer;
  
  /* ������������� �����, � ������� ���������� ����������� ������ �� ��������
   * ������, ������� ������� '\n' �� "\r\n" ��� ����������� �������� �������
   * ������� �� ����� ������.
   * �� ��������� ����������� ������� ������ ��������� m_sizebuf*2. � ������
   * ������������� > UTF-8 ����������� �������� m_sizebuf * sizeof(char_type).
   */
  vector<char> m_temp_buffer;
  
  /* ������, �������� ��� ����������� �������� ����������. */
  arr_conn_type m_connections;
};

template<typename char_type, typename traits_type>
lwipbuf<char_type, traits_type>::lwipbuf(size_t sizebuf, u16_t port)
  : m_sizebuf(sizebuf)
  , m_port(port)
{
  m_buffer.resize(m_sizebuf);
  m_temp_buffer.resize(m_sizebuf * sizeof(char_type));

  m_convert_buffer = new char[m_sizebuf * sizeof(char_type)]();

  /* ������ �����, � �-�� ����� ����������� ������, ������������ �����
   * ������ �����/������. */
  char_type* buf = m_buffer.data();
  this->setp(buf, buf + m_buffer.size());
  
  /* ������������� �������. */
  IRS_ASSERT(tcp_init() < 0);
}

template<typename char_type, typename traits_type>
lwipbuf<char_type, traits_type>::~lwipbuf()
{
  if (m_convert_buffer) { delete[] m_convert_buffer; } 
  if (mp_tcp_pcb) { memp_free(MEMP_TCP_PCB, mp_tcp_pcb); }
}

template<typename char_type, typename traits_type>
int lwipbuf<char_type, traits_type>::tcp_init()
{
  /* ������� ����� ���������� ����������. */
  mp_tcp_pcb = tcp_new();
  
  if (mp_tcp_pcb != NULL) {
    /* ������ ���������� �� ������������ ����. */
    err_t err = tcp_bind(mp_tcp_pcb, IP_ADDR_ANY, m_port);
    
    if (err == ERR_OK) {
      /* �������� ������� ������� ���� �� ����� ����������. */
      mp_tcp_pcb = tcp_listen(mp_tcp_pcb);
      
      /* ������ ���������������� ������, �-�� ���������� ���������� ��� ������
       * callback-�������.
       */
      tcp_arg(mp_tcp_pcb, static_cast<void*>(&m_connections));
        
      /* ������ callback-������� ��������� ����������� ������ ����������. */
      tcp_accept(mp_tcp_pcb, c_tcp_accept);
      
      return 0;
    } else {
#ifdef USE_LCD
      LCD_UsrTrace("  Error [tcpInit]: tcp_bind was failed\n");
#endif // USE_LCD
      
      /* ������������ ������, ���������� ��� ���������� ������ ����������. */
      memp_free(MEMP_TCP_PCB, mp_tcp_pcb);
      
      return -1;
    }
  }
  
  return -1;
}

template<typename char_type, typename traits_type>
void lwipbuf<char_type, traits_type>::tick(struct netif* ap_netif)
{
  /* �������� ������, ���������� �� �������� �� ������ ethernet. */
  ethernetif_input(ap_netif);
  
  /* �������-�������. */
  sys_check_timeouts();
    
#if LWIP_NETIF_LINK_CALLBACK
  Ethernet_Link_Periodic_Handle(ap_netif);
#endif // LWIP_NETIF_LINK_CALLBACK

#if LWIP_DHCP
  DHCP_Periodic_Handle(ap_netif, m_port);
#endif // LWIP_DHCP
}

template<typename char_type, typename traits_type>
void lwipbuf<char_type, traits_type>::send(const void* ap_msg, size_t size_msg)
{
#ifdef USE_LCD
  if (m_connections.size() == 0) {
    LCD_UsrTrace("  Warning: There is no active connection!\n");
  }
#endif // USE_LCD
  
  /* ������������ �������� ��������� ��� ���� ��������, ������������ �������. */
  for (int i = m_connections.size() - 1; i >= 0; i--) {
    /* ������������ ������ ������ � ����� LWIP. */
    err_t err = tcp_write(m_connections[i], 
                          ap_msg,
                          size_msg,
                          TCP_WRITE_FLAG_COPY);
    
    /* ���� ������ ������� ���������� � ����� LWIP, ������������ �� ��������. */
    if (err == ERR_OK) { 
      err = tcp_output(m_connections[i]);
    }
    
    /* ���� ��������� ������ ������/�������� ������, �� ��������� � ���������
     * ������ ������ LWIP, �� ��������� ������ ���������� � ��������.
     */
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
size_t 
lwipbuf<char_type, traits_type>::copy_str_to_buffer_with_correct_endls(const char* ap_str)
{
  if (ap_str) {
    size_t j = 0;
    
    for (size_t i = 0; i < strlen(ap_str); i++) {
      if (ap_str[i] == '\n') { m_temp_buffer[j++] = '\r'; }
      m_temp_buffer[j++] = ap_str[i];
    }
    
    return j;
  }
  
  return 0;
}

template<>
lwipbuf<wchar_t>::int_type lwipbuf<wchar_t>::overflow(int_type c) _OVERRIDE_
{
  /* ��������� ������ ��������� � ������. */
  ptrdiff_t sz = this->pptr() - this->pbase();
  
  /* ������������ ������� ���������. */
  wstring temp(L"", sz);
  for (size_t i = 0; i < sz; i++) { temp[i] = m_buffer[i]; }
  string converted_str = wstring_to_utf8(temp);

  size_t msg_size = copy_str_to_buffer_with_correct_endls(converted_str.c_str());
  
  /* ���������� ������ ��������. */
  send(&m_temp_buffer.front(), msg_size);
  
  /* ����������� ������� ������� ���������� ������ ��� ���������� ������ ������
   * � ������� ����� ��� ������������� ������� �����/������. 
   */
  this->pbump(-sz);
  
  if (c != traits_type::eof()) {
    m_buffer[0] = static_cast<wchar_t>(c);
    this->pbump(1);
  }
  
  return 0;
}

template<typename char_type, typename traits_type>
lwipbuf<char_type, traits_type>::int_type 
lwipbuf<char_type, traits_type>::overflow(int_type c) _OVERRIDE_
{
  /* ��������� ������ ��������� � ������. */
  ptrdiff_t sz = this->pptr() - this->pbase();
  
  /* ������������ ������� ���������. */
  cp1251_to_utf8(m_convert_buffer, sz, &m_buffer.front());
  
  size_t msg_size = copy_str_to_buffer_with_correct_endls(m_convert_buffer);
  
  /* ���������� ������ ��������. */
  send(&m_temp_buffer.front(), msg_size);
  
  /* ����������� ������� ������� ���������� ������ ��� ���������� ������ ������
   * � ������� ����� ��� ������������� ������� �����/������. 
   */
  this->pbump(-sz);
  
  if (c != traits_type::eof()) {
    m_buffer[0] = static_cast<char>(c);
    this->pbump(1);
  }
  
  /* ���������� ������ ��������. */
  return 0;
}

template<typename char_type, typename traits_type>
int lwipbuf<char_type, traits_type>::sync() _OVERRIDE_
{ return this->pptr() == this->pbase() ? 0 : overflow(); }

template<typename char_type, typename traits_type>
err_t lwipbuf<char_type, traits_type>::c_tcp_accept(void* arg, 
                                                    struct tcp_pcb* newpcb, 
                                                    err_t err)
{
  arr_conn_type* conns = static_cast<arr_conn_type*>(arg);
  err_t ret_err;
  
  LWIP_UNUSED_ARG(err);

  /* ������������� ��������� ��� ������ ����������. */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /* ������ ���������������� ������, �-�� ���������� ���������� �� ��� callback-
   * -������� ��� ������� ����������. 
   */
  tcp_arg(newpcb, arg);
  
   /* ������ callback-�������, ���������� ��� ��������� ����� ������� �� �������. */ 
  tcp_recv(newpcb, c_tcp_recv);
    
  /* ������ callback-�������, ���������� � ������������� ��������. */
//  tcp_poll(newpcb, c_tcp_poll, 0);
  
  /* ��������� ����� ���������� � ����� ������ ������������. */
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
  arr_conn_type* conns = static_cast<arr_conn_type*>(arg);
  
  /* ���� ������� ������� �� ���� �������� �� �������, ������ �� ����������.
   * � ���� ������ ������������ �������� ����������� ������� ����������. */
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
//  arr_conn_type* conns = reinterpret_cast<arr_conn_type*>(arg);
  err_t ret_err = ERR_OK;
  
  return ret_err;
}

template<typename char_type, typename traits_type>
void lwipbuf<char_type, traits_type>::c_tcp_connection_close(struct tcp_pcb* tpcb)
{
  /* ������� ��� callback-������� ��� ������� �����������. */
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  /* ��������� TCP ����������. */
  tcp_close(tpcb);
  
#ifdef USE_LCD
    LCD_UsrTrace("  State: A connection is closed!\n");
#endif // USE_LCD
}

} // namespace irs

#endif // TCP_SERVER_H