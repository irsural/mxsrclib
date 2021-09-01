// @brief класс basic_lwipbuf представляющий собой буфер, использующий ethernet
// в качестве передачи данных.
//
// Дата создания: 12.04.2021
// Реализация: Галимзянов
// Некоторые исправления: Крашенинников

#ifndef LWIP_BUF_H
#define LWIP_BUF_H

// Крашенинников 25.08.2021
// В настройки проекта следует добавить define-константу IRS_USE_UTF8_CPP=1

// Крашенинников Галимзянову: Напиши в описании класса в документе Word
// обо всех define-константах, которые нужно добавить в проект для правильного
// функционирования lwipbuf. Также это написать здесь.

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

/* Размер LWIP буфера по умолчанию */
#define IRSLIB_LWIPBUF_SIZE 128

template<typename char_type, typename traits_type = char_traits<char_type>>
class basic_lwipbuf: public basic_streambuf<char_type, traits_type>
{
public:
  typedef typename traits_type::int_type int_type;

  typedef vector<char> buffer_type;

  /**
   * @brief конструктор по умолчанию. Выделяет динамическую память под буферы,
   * вместе с этим проводит инициализацию сервера и его запуск.
   *
   * @param a_sizebuf - размер буфера. По умолчанию - IRSLIB_LWIPBUF_SIZE (128)
   * @param a_port - порт, на который будем отправлять сообщения. По умолчанию -
   * IRSLIB_LOG_PORT (5008)
   * @param ap_ip - статический ip адрес на случай, если мы хотим обрабатывать
   * определенный адрес. По умолчанию IP_ADDR_ANY
   */
  basic_lwipbuf(size_t a_sizebuf = IRSLIB_LWIPBUF_SIZE,
    u16_t a_port = IRSLIB_LOG_PORT, const ip_addr_t* ap_ip = IP_ADDR_ANY);

  /**
   * @brief деструктор по умолчанию. Высвобождает динамическую память,
   * выделенную под буферы.
   */
  virtual ~basic_lwipbuf();

  /**
   * @brief функция "тик". Ее необходимо вызывать в вашем цикле на каждой
   * итерации. Данная функция обновляет информацию о наличии активных
   * подключений.
   */
  virtual void tick();

  /**
   * @brief функция отправки сообщения клиентам. Вместе с этим функция проверят
   * корректное отправление клиенту сообщения; в плохом случае удаляет сессию
   * клиента из общего массива сессий.
   *
   * @param ap_msg - отправляемое сообщение.
   * @param size_msg - размерность сообщения.
   */
  virtual void send(const void* ap_msg, size_t a_size_msg);

  /**
   * @brief функция получения порта сооединения.
   *
   * @return u16_t - текущий порт сооединения, по которому происходит отправка
   * сообещний.
   */
  u16_t get_port() const;

  /**
   * @brief функция получения IP адреса сервера.
   *
   * @return ip_addr_t - текущий IP адрес сервера.
   */
  ip_addr_t get_ip() const;

  /**
   * @brief функция проверки наличия клиентов, подключенных к северу.
   *
   * @return true - в случае, если есть активные соединения, false - в ином
   * случае.
   */
  bool is_any_connected() const;

  /**
   * @brief функция, возвращающая количество активных соединений.
   *
   * @return size_t - количество активных соединений.
   */
  size_t get_count_connected() const;

  /**
   * @brief функция, вызываемая при переполнении буфера в случае, когда мы
   * используем потоки ввода/вывода. Данная Ф-я очищает буфер, предварительно
   * осуществив отправку сообщения клиентам, которое было на текущий момент в
   * буфере; затем добавляет остаточный символ в буфер, который не влез.
   *
   * @param c - символ, который не влез в буфер.
   * По умолчанию - traits_type::eof() - означает символ окончания строки
   * (в случае потоков ввода/вывода).
   *
   * @return 1 - в случае, если параметр c = traits_type::eof(), 0 - в ином
   * случае.
   *
   * @note данная функция наследуется от basic_streambuf<char_type, traits_type>.
   * При реализации вызывает приватную функцию overflow(), которая осуществляет
   * выше сказанный функционал.
   */
  _OVERRIDE_ virtual int_type overflow(int_type c = traits_type::eof());

  /**
   * @brief функция синхронизации потоков. В данном случае может вызываться
   * специально пользователем. В случае, если в буфере есть данные вызывает
   * overflow(), в ином случае возвращает 0.
   *
   * @return overflow() - в случае, если в буфере есть данные, в ином случае - 0.
   *
   * @note данная функция наследуется от basic_streambuf<char_type, traits_type>.
   */
  _OVERRIDE_ virtual int sync();

private:
  typedef vector<struct tcp_pcb*> arr_conn_type;

  /**
   * @brief функция инициализации сервера. Функция задает tcp слушателя, а также
   * callback-функции приема текущего соединения (accept-func). В случае, если
   * определен #define USE_LCD и инициализация произошла неудачно, выведет на
   * дисплей платы соответствующее сообщение.
   */
  int tcp_init();

  /**
   * @brief функция добавления символов '\r' перед символами '\n' в буфере.
   * Для того, чтобы не осуществлять сдвиг данных, данные копируются в конец
   * буфера.
   *
   * @param a_buffer - буффер, в котором осуществляем добавление символов.
   * @param a_size_str - размерность сообщения в буфере.
   *
   * @return size_t - индекс, с которого начинается преобразованное сообщение
   * в буфере.
   */
  size_t copy_with_r_symbols(vector<char>& a_buffer,
    const size_t a_symbols_length);

  /**
   * @brief callback-функция получения дескриптора нового соединения.
   * Устанавливает приоритет, а также специальные callback-функции для текущего
   * соединения. В случае, если определен #define USE_LCD, выведет
   * соответствующее сообщение на дисплей платы.
   *
   * @param arg - некоторые данные, задаваемые пользователем, которые будут
   * передаваться во все callback-функции текущего соединения.
   * @param newpcb - дескриптор нового соединения.
   * @param err - не используется.
   */
  static err_t c_tcp_accept(void* arg, struct tcp_pcb* newpcb, err_t err);

  /**
   * @brief callback-функция получения пакетов данных от клиента.
   * Проверяет, получили ли мы какие-либо пакеты от клиента. В отрицательном
   * случае удаляет дескриптор текущего соединения, т.е. отключает клиента от
   * себя.
   *
   * @param arg - некоторые данные, задаваемые пользователем, которые будут
   * передаваться во все callback-функции текущего соединения.
   * @param tpcb - дескриптор текущего соединения.
   * @param p - буфер, в котором находятся полученные данные от клиента.
   * @param err - не используется.
   */
  static err_t c_tcp_recv(void* arg, struct tcp_pcb* tpcb, struct pbuf* p,
    err_t err);

  /**
   * @brief callback-функция, вызываемая с определенным периодом. В данный момент
   * не используется.
   *
   * @param arg - некоторые данные, задаваемые пользователем, которые будут
   * передаваться во все callback-функции текущего соединения.
   * @param tpcb - дескриптор текущего соединения.
   */
  static err_t c_tcp_poll(void* arg, struct tcp_pcb* tpcb);

  /**
   * @brief callback-функция, удаляющая дескриптор соединения, т.е. отключающая
   * клиента от сервера.
   *
   * @param tpcb - дескриптор соединения, к-ый необходимо удалить.
   */
  static void c_tcp_connection_close(struct tcp_pcb* tpcb);

  /* Начальный дескриптор соединения. */
  struct tcp_pcb* mp_tcp_pcb;

  /**
   * Размерность глваного буфера, в который происходит заполнение данных в случае
   * использования потоков ввода/вывода.
   */
  size_t m_sizebuf;

  /* Порт, по к-ому осуществляется отправка сообщений. */
  u16_t m_port;

  /* Статический ip адрес на случай, если мы не используем DHCP. */
  const ip_addr_t* mp_ip;

  /* Буффер, в который записываются данные. */
  buffer_type m_unified_buffer;

  /* Массив, хранящий все дескрипторы активных соединений. */
  arr_conn_type m_connections;

  /**
   * Переменная для хранения данных об активном подключении,
   * для корректного отображения сообщения об отсутствии подключений.
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
   * Задаем буфер, в к-ые будут сохраняться данные, передаваемые через
   * потоки ввода/вывода.
   */
  char_type* ref_buffer = reinterpret_cast<char_type*>(m_unified_buffer.data());
  char_type* buf_end = ref_buffer + m_unified_buffer.size()/sizeof(char_type);
  char_type* buf_begin = buf_end - m_sizebuf;
  this->setp(buf_begin, buf_end);

  /* Инициализация сервера. */
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
  /* Создаем новый дескриптор соединения. */
  mp_tcp_pcb = tcp_new();

  if (mp_tcp_pcb != IRS_NULL) {
    if (!mp_ip) {
#ifndef IRS_NOEXCEPTION
      throw runtime_error("mp_ip cannot be IRS_NULL");
#else // IRS_NOEXCEPTION
      return -1;
#endif // IRS_NOEXCEPTION
    }

    /* Биндим дескриптор на определенный порт. */
    err_t err = tcp_bind(mp_tcp_pcb, const_cast<ip_addr_t*>(mp_ip), m_port);
    if (err == ERR_OK) {
      /* Начинаем слушать текущий порт на новые соединения. */
      mp_tcp_pcb = tcp_listen(mp_tcp_pcb);

      /**
       * Задаем пользовательские данные, к-ые необходимо передавать при вызове
       * callback-функций.
       */
      tcp_arg(mp_tcp_pcb, static_cast<void*>(&m_connections));

      /* Задаем callback-функцию получения дескриптора нового соединения. */
      tcp_accept(mp_tcp_pcb, c_tcp_accept);

      return 0;
    } else {
      /* Высвобождаем память, выделенную под дескриптор нового соединения. */
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

  /* Осуществляем отправку сообщений для всех клиентов, подключенных серверу. */
  for (int i = m_connections.size() - 1; i >= 0; i--) {
    /* Осуществляем запись данных в буфер LWIP. */
    err_t err = tcp_write(m_connections[i], ap_msg, a_size_msg,
      TCP_WRITE_FLAG_COPY);

    /* Если данные успешно записались в буфер LWIP, осуществляем их отправку. */
    if (err == ERR_OK) {
      err = tcp_output(m_connections[i]);
    }

    /**
     * Если произошла ошибка записи/отправки данных, не связанная с нехваткой
     * памяти буфера LWIP, то закрываем данное соединение с клиентом.
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
  /* Вычисляем размер сообщения в буфере. */
  ptrdiff_t sz = pptr() - pbase();

  /**
   * Осуществляем перекодировку сообщения.
   * На выходе получаем размерность полученного сообщения.
   */
  size_t utf8_size =
    lwipbuf_to_utf8(pbase(), pptr(), m_unified_buffer.begin());

  /**
   * Добавляем к каждому символу '/n' символ '/r'.
   * На выходе получаем индекс, с которого начинается преобразованное сообщение.
   */
  size_t msg_start_index = copy_with_r_symbols(m_unified_buffer, utf8_size);

  /* Отправляем данные клиентам. */
  char_type* data_rn = m_unified_buffer.data() + msg_start_index;
  size_t size_rn = m_unified_buffer.size() - msg_start_index;
  send(data_rn, size_rn);

  /**
   * Устанавлием позицию каретки заполнения данных для корректной записи данных
   * в главный буфер при использовании потоков ввода/вывода.
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

  /* Устанавливаем приоритет для нового соединения. */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /**
   * Задаем пользовательские данные, к-ые необходимо передавать во все callback-
   * -функции для данного соединения.
   */
  tcp_arg(newpcb, arg);

   /**
    * Задаем callback-функцию, вызываемую при получении новых пакетов от клиента.
    */
  tcp_recv(newpcb, c_tcp_recv);

  /* Задаем callback-функцию, вызываемую с опеределенным периодом. */
  // tcp_poll(newpcb, c_tcp_poll, 0);

  /* Добавляем новый дескриптор в общий массив дескрипторов. */
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
   * Если никаких пакетов не было получено от клиента, значит он отключился.
   * В этом случае осуществляем удаление дескриптора данного соединения.
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
  /* Удаляем все callback-функции для данного дескриптора. */
  tcp_arg(tpcb, IRS_NULL);
  tcp_sent(tpcb, IRS_NULL);
  tcp_recv(tpcb, IRS_NULL);
  tcp_poll(tpcb, IRS_NULL, 0);

  /* Закрываем TCP соединение. */
  tcp_close(tpcb);

#ifdef USE_LCD
    LCD_UsrTrace("  [State] A connection is closed!\n");
#endif // USE_LCD
}

typedef basic_lwipbuf<char, char_traits<char>> lwipbuf;
typedef basic_lwipbuf<wchar_t, char_traits<wchar_t>> wlwipbuf;

} // namespace irs

#endif // LWIP_BUF_H
