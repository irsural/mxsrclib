// @brief ����� basic_lwipbuf �������������� ����� �����, ������������ ethernet
// � �������� �������� ������.
//
// ���� ��������: 12.04.2021
// ����������: ����������
// ��������� �����������: �������������

#ifndef LWIP_BUF_H
#define LWIP_BUF_H

// ������������� 25.08.2021
// � ��������� ������� ������� �������� define-��������� IRS_USE_UTF8_CPP=1

// ������������� �����������: ������ � �������� ������ � ��������� Word
// ��� ���� define-����������, ������� ����� �������� � ������ ��� �����������
// ���������������� lwipbuf. ����� ��� �������� �����.

#include <irsdefs.h>
#include <irscpp.h>
#include <irsnetdefs.h>
#include <irsencode.h>
#include <irsstrconv.h>

#ifdef USE_LCD
extern "C" {
#include "lcd_trace.h"
}
#endif // USE_LCD

#pragma diag_suppress=Pa181

#include "lwipopts.h"

#include "netif/etharp.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/netif.h"

#ifdef IRS_STM32F4xx
#include "lwip/timers.h"
#else //IRS_STM32F4xx
#include "lwip/timeouts.h"
#endif //IRS_STM32F4xx
#include "lwip/err.h"

#include "lwip/stats.h"
#include "lwip/tcp.h"

#pragma diag_default=Pa181

#define _OVERRIDE_

namespace irs
{

/* ������ LWIP ������ �� ��������� */
#define IRSLIB_LWIPBUF_SIZE 128

template<typename char_type, typename traits_type = char_traits<char_type>>
class basic_lwipbuf: public basic_streambuf<char_type, traits_type>
{
public:
  typedef typename traits_type::int_type int_type;

  typedef vector<char> buffer_type;

  /**
   * @brief ����������� �� ���������. �������� ������������ ������ ��� ������,
   * ������ � ���� �������� ������������� ������� � ��� ������.
   *
   * @param a_sizebuf - ������ ������. �� ��������� - IRSLIB_LWIPBUF_SIZE (128)
   * @param a_port - ����, �� ������� ����� ���������� ���������. �� ��������� -
   * IRSLIB_LOG_PORT (5008)
   * @param ap_ip - ����������� ip ����� �� ������, ���� �� ����� ������������
   * ������������ �����. �� ��������� IP_ADDR_ANY
   */
  basic_lwipbuf(size_t a_sizebuf = IRSLIB_LWIPBUF_SIZE,
    u16_t a_port = IRSLIB_LOG_PORT, const ip_addr_t* ap_ip = IP_ADDR_ANY);

  /**
   * @brief ���������� �� ���������. ������������ ������������ ������,
   * ���������� ��� ������.
   */
  virtual ~basic_lwipbuf();

  /**
   * @brief ������� "���". �� ���������� �������� � ����� ����� �� ������
   * ��������. ������ ������� ��������� ���������� � ������� ��������
   * �����������.
   */
  virtual void tick();

  /**
   * @brief ������� �������� ��������� ��������. ������ � ���� ������� ��������
   * ���������� ����������� ������� ���������; � ������ ������ ������� ������
   * ������� �� ������ ������� ������.
   *
   * @param ap_msg - ������������ ���������.
   * @param size_msg - ����������� ���������.
   */
  virtual void send(const void* ap_msg, size_t a_size_msg);

  /**
   * @brief ������� ��������� ����� �����������.
   *
   * @return u16_t - ������� ���� �����������, �� �������� ���������� ��������
   * ���������.
   */
  u16_t get_port() const;

  /**
   * @brief ������� ��������� IP ������ �������.
   *
   * @return ip_addr_t - ������� IP ����� �������.
   */
  ip_addr_t get_ip() const;

  /**
   * @brief ������� �������� ������� ��������, ������������ � ������.
   *
   * @return true - � ������, ���� ���� �������� ����������, false - � ����
   * ������.
   */
  bool is_any_connected() const;

  /**
   * @brief �������, ������������ ���������� �������� ����������.
   *
   * @return size_t - ���������� �������� ����������.
   */
  size_t get_count_connected() const;

  /**
   * @brief �������, ���������� ��� ������������ ������ � ������, ����� ��
   * ���������� ������ �����/������. ������ �-� ������� �����, ��������������
   * ���������� �������� ��������� ��������, ������� ���� �� ������� ������ �
   * ������; ����� ��������� ���������� ������ � �����, ������� �� ����.
   *
   * @param c - ������, ������� �� ���� � �����.
   * �� ��������� - traits_type::eof() - �������� ������ ��������� ������
   * (� ������ ������� �����/������).
   *
   * @return 1 - � ������, ���� �������� c = traits_type::eof(), 0 - � ����
   * ������.
   *
   * @note ������ ������� ����������� �� basic_streambuf<char_type, traits_type>.
   * ��� ���������� �������� ��������� ������� overflow(), ������� ������������
   * ���� ��������� ����������.
   */
  _OVERRIDE_ virtual int_type overflow(int_type c = traits_type::eof());

  /**
   * @brief ������� ������������� �������. � ������ ������ ����� ����������
   * ���������� �������������. � ������, ���� � ������ ���� ������ ��������
   * overflow(), � ���� ������ ���������� 0.
   *
   * @return overflow() - � ������, ���� � ������ ���� ������, � ���� ������ - 0.
   *
   * @note ������ ������� ����������� �� basic_streambuf<char_type, traits_type>.
   */
  _OVERRIDE_ virtual int sync();

private:
  typedef vector<struct tcp_pcb*> arr_conn_type;

  /**
   * @brief ������� ������������� �������. ������� ������ tcp ���������, � �����
   * callback-������� ������ �������� ���������� (accept-func). � ������, ����
   * ��������� #define USE_LCD � ������������� ��������� ��������, ������� ��
   * ������� ����� ��������������� ���������.
   */
  int tcp_init();

  /**
   * @brief ������� ���������� �������� '\r' ����� ��������� '\n' � ������.
   * ��� ����, ����� �� ������������ ����� ������, ������ ���������� � �����
   * ������.
   *
   * @param a_buffer - ������, � ������� ������������ ���������� ��������.
   * @param a_size_str - ����������� ��������� � ������.
   *
   * @return size_t - ������, � �������� ���������� ��������������� ���������
   * � ������.
   */
  size_t copy_with_r_symbols(vector<char>& a_buffer,
    const size_t a_symbols_length);

  /**
   * @brief callback-������� ��������� ����������� ������ ����������.
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

  /**
   * @brief callback-������� ��������� ������� ������ �� �������.
   * ���������, �������� �� �� �����-���� ������ �� �������. � �������������
   * ������ ������� ���������� �������� ����������, �.�. ��������� ������� ��
   * ����.
   *
   * @param arg - ��������� ������, ���������� �������������, ������� �����
   * ������������ �� ��� callback-������� �������� ����������.
   * @param tpcb - ���������� �������� ����������.
   * @param p - �����, � ������� ��������� ���������� ������ �� �������.
   * @param err - �� ������������.
   */
  static err_t c_tcp_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p,
    err_t err);

  /**
   * @brief callback-�������, ���������� � ������������ ��������. � ������ ������
   * �� ������������.
   *
   * @param arg - ��������� ������, ���������� �������������, ������� �����
   * ������������ �� ��� callback-������� �������� ����������.
   * @param tpcb - ���������� �������� ����������.
   */
  static err_t c_tcp_poll(void* arg, struct tcp_pcb* tpcb);

  /**
   * @brief callback-�������, ��������� ���������� ����������, �.�. �����������
   * ������� �� �������.
   *
   * @param tpcb - ���������� ����������, �-�� ���������� �������.
   */
  static void c_tcp_connection_close(struct tcp_pcb* tpcb);

  /* ��������� ���������� ����������. */
  struct tcp_pcb* mp_tcp_pcb;

  /**
   * ����������� �������� ������, � ������� ���������� ���������� ������ � ������
   * ������������� ������� �����/������.
   */
  size_t m_sizebuf;

  /* ����, �� �-��� �������������� �������� ���������. */
  u16_t m_port;

  /* ����������� ip ����� �� ������, ���� �� �� ���������� DHCP. */
  const ip_addr_t* mp_ip;

  /* ������, � ������� ������������ ������. */
  buffer_type m_unified_buffer;

  /* ������, �������� ��� ����������� �������� ����������. */
  arr_conn_type m_connections;

  /**
   * ���������� ��� �������� ������ �� �������� �����������,
   * ��� ����������� ����������� ��������� �� ���������� �����������.
   */
  bool m_connected;
};

template<typename char_type, typename traits_type>
basic_lwipbuf<char_type, traits_type>::basic_lwipbuf(size_t a_sizebuf,
  u16_t a_port, const ip_addr_t* ap_ip
)
  : m_sizebuf(a_sizebuf)
  , m_port(a_port)
  , mp_ip(ap_ip)
  , m_unified_buffer(m_sizebuf * (sizeof(char_type) == 1 ? 3 : 4) + 4)
  , m_connected(false)
{
  /**
   * ������ �����, � �-�� ����� ����������� ������, ������������ �����
   * ������ �����/������.
   */
  char_type* ref_buffer = reinterpret_cast<char_type*>(m_unified_buffer.data());
  char_type* buf_end = ref_buffer + m_unified_buffer.size()/sizeof(char_type);
  char_type* buf_begin = buf_end - m_sizebuf;
  this->setp(buf_begin, buf_end);

  /* ������������� �������. */
  if (tcp_init() < 0) {
#ifdef USE_LCD
    LCD_UsrTrace("  [Error] tcp_init: tcp_bind was failed\n");
#endif // USE_LCD
  } else {
#ifdef USE_LCD
    LCD_UsrTrace("  [State] tcp_init: successful initialization\n");
#endif // USE_LCD
  }
}

template<typename char_type, typename traits_type>
basic_lwipbuf<char_type, traits_type>::~basic_lwipbuf()
{
  if (mp_tcp_pcb) { memp_free(MEMP_TCP_PCB, mp_tcp_pcb); }
}

template<typename char_type, typename traits_type>
int basic_lwipbuf<char_type, traits_type>::tcp_init()
{
  /* ������� ����� ���������� ����������. */
  mp_tcp_pcb = tcp_new();

  if (mp_tcp_pcb != IRS_NULL) {
    if (!mp_ip) {
#ifndef IRS_NOEXCEPTION
      throw runtime_error("mp_ip cannot be IRS_NULL");
#else // IRS_NOEXCEPTION
      return -1;
#endif // IRS_NOEXCEPTION
    }

    /* ������ ���������� �� ������������ ����. */
    err_t err = tcp_bind(mp_tcp_pcb, const_cast<ip_addr_t*>(mp_ip), m_port);
    if (err == ERR_OK) {
      /* �������� ������� ������� ���� �� ����� ����������. */
      mp_tcp_pcb = tcp_listen(mp_tcp_pcb);

      /**
       * ������ ���������������� ������, �-�� ���������� ���������� ��� ������
       * callback-�������.
       */
      tcp_arg(mp_tcp_pcb, static_cast<void*>(&m_connections));

      /* ������ callback-������� ��������� ����������� ������ ����������. */
      tcp_accept(mp_tcp_pcb, c_tcp_accept);

      return 0;
    } else {
      /* ������������ ������, ���������� ��� ���������� ������ ����������. */
      memp_free(MEMP_TCP_PCB, mp_tcp_pcb);
      return -1;
    }
  }

  return -1;
}

template<typename char_type, typename traits_type>
void basic_lwipbuf<char_type, traits_type>::tick()
{
  m_connected = !m_connections.empty();
}

template<typename char_type, typename traits_type>
void
basic_lwipbuf<char_type, traits_type>::send(
  const void* ap_msg, size_t a_size_msg
)
{
#ifdef USE_LCD
  if (m_connections.empty() && m_connected) {
    LCD_UsrTrace("  [State] There are not activate connections!\n");
    m_connected = false;
  }
#endif // USE_LCD

  /* ������������ �������� ��������� ��� ���� ��������, ������������ �������. */
  for (int i = m_connections.size() - 1; i >= 0; i--) {
    /* ������������ ������ ������ � ����� LWIP. */
    err_t err = tcp_write(m_connections[i], ap_msg, a_size_msg,
      TCP_WRITE_FLAG_COPY);

    /* ���� ������ ������� ���������� � ����� LWIP, ������������ �� ��������. */
    if (err == ERR_OK) {
      err = tcp_output(m_connections[i]);
    }

    /**
     * ���� ��������� ������ ������/�������� ������, �� ��������� � ���������
     * ������ ������ LWIP, �� ��������� ������ ���������� � ��������.
     */
    if (err != ERR_OK && err != ERR_MEM) {
      c_tcp_connection_close(m_connections[i]);
      m_connections.erase(m_connections.begin() + i);
    }
  }
}

template<typename char_type, typename traits_type>
u16_t basic_lwipbuf<char_type, traits_type>::get_port() const
{ return m_port; }

template<typename char_type, typename traits_type>
ip_addr_t basic_lwipbuf<char_type, traits_type>::get_ip() const
{ return *mp_ip; }

template<typename char_type, typename traits_type>
bool basic_lwipbuf<char_type, traits_type>::is_any_connected() const
{ return m_connected; }

template<typename char_type, typename traits_type>
size_t basic_lwipbuf<char_type, traits_type>::get_count_connected() const
{ return m_connections.size(); }

template<typename char_type, typename traits_type>
size_t
basic_lwipbuf<char_type, traits_type>::copy_with_r_symbols(
  vector<char>& a_buffer, const size_t a_symbols_length
)
{
  size_t j = a_buffer.size() - 1;

  for (size_t i = a_symbols_length - 1; static_cast<int>(i) >= 0; i--) {
    a_buffer.at(j--) = a_buffer.at(i);
    if (a_buffer.at(i) == '\n') {
      a_buffer.at(j--) = '\r';
    }
  }

  return (j + 1);
}

template<typename char_type, typename traits_type>
_OVERRIDE_ basic_lwipbuf<char_type, traits_type>::int_type
basic_lwipbuf<char_type, traits_type>::overflow(int_type c)
{
  /* ��������� ������ ��������� � ������. */
  ptrdiff_t sz = pptr() - pbase();

  /**
   * ������������ ������������� ���������.
   * �� ������ �������� ����������� ����������� ���������.
   */
  size_t utf8_size =
    lwipbuf_to_utf8(pbase(), pptr(), m_unified_buffer.begin());

  /**
   * ��������� � ������� ������� '/n' ������ '/r'.
   * �� ������ �������� ������, � �������� ���������� ��������������� ���������.
   */
  size_t msg_start_index = copy_with_r_symbols(m_unified_buffer, utf8_size);

  /* ���������� ������ ��������. */
  char_type* data_rn = m_unified_buffer.data() + msg_start_index;
  size_t size_rn = m_unified_buffer.size() - msg_start_index;
  send(data_rn, size_rn);

  /**
   * ����������� ������� ������� ���������� ������ ��� ���������� ������ ������
   * � ������� ����� ��� ������������� ������� �����/������.
   */
  this->pbump(-sz);

  if (c != traits_type::eof()) {
    pptr() = static_cast<char_type>(c);
    this->pbump(1);
  }

  return 0;
}

template<typename char_type, typename traits_type>
_OVERRIDE_ int basic_lwipbuf<char_type, traits_type>::sync()
{
  return overflow();
}

template<typename char_type, typename traits_type>
err_t basic_lwipbuf<char_type, traits_type>::c_tcp_accept(void* arg,
  struct tcp_pcb* newpcb, err_t err
)
{
  LWIP_UNUSED_ARG(err);

  arr_conn_type* conns = static_cast<arr_conn_type*>(arg);
  err_t ret_err;

  /* ������������� ��������� ��� ������ ����������. */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /**
   * ������ ���������������� ������, �-�� ���������� ���������� �� ��� callback-
   * -������� ��� ������� ����������.
   */
  tcp_arg(newpcb, arg);

   /**
    * ������ callback-�������, ���������� ��� ��������� ����� ������� �� �������.
    */
  tcp_recv(newpcb, c_tcp_recv);

  /* ������ callback-�������, ���������� � ������������� ��������. */
  // tcp_poll(newpcb, c_tcp_poll, 0);

  /* ��������� ����� ���������� � ����� ������ ������������. */
  conns->push_back(newpcb);

#ifdef USE_LCD
    LCD_UsrTrace("  [State] A new connection was established\n");
#endif // USE_LCD

  ret_err = ERR_OK;

  return ret_err;
}

template<typename char_type, typename traits_type>
err_t basic_lwipbuf<char_type, traits_type>::c_tcp_recv(void* arg,
  struct tcp_pcb* tpcb, struct pbuf* p, err_t err
)
{
  arr_conn_type* conns = static_cast<arr_conn_type*>(arg);

  /**
   * ���� ������� ������� �� ���� �������� �� �������, ������ �� ����������.
   * � ���� ������ ������������ �������� ����������� ������� ����������.
   */
  if (p == IRS_NULL) {
    c_tcp_connection_close(tpcb);

    arr_conn_type::iterator iter = find(conns->begin(), conns->end(), tpcb);
    if (iter != conns->end()) { conns->erase(iter); }
  }

  return ERR_OK;
}

template<typename char_type, typename traits_type>
err_t basic_lwipbuf<char_type, traits_type>::c_tcp_poll(void* arg,
  struct tcp_pcb* tpcb
)
{
  LWIP_UNUSED_ARG(arg);
  err_t ret_err = ERR_OK;

  return ret_err;
}

template<typename char_type, typename traits_type>
void
basic_lwipbuf<char_type, traits_type>::c_tcp_connection_close(
  struct tcp_pcb* tpcb
)
{
  /* ������� ��� callback-������� ��� ������� �����������. */
  tcp_arg(tpcb, IRS_NULL);
  tcp_sent(tpcb, IRS_NULL);
  tcp_recv(tpcb, IRS_NULL);
  tcp_poll(tpcb, IRS_NULL, 0);

  /* ��������� TCP ����������. */
  tcp_close(tpcb);

#ifdef USE_LCD
    LCD_UsrTrace("  [State] A connection is closed!\n");
#endif // USE_LCD
}

typedef basic_lwipbuf<char, char_traits<char>> lwipbuf;
typedef basic_lwipbuf<wchar_t, char_traits<wchar_t>> wlwipbuf;

} // namespace irs

#endif // LWIP_BUF_H
