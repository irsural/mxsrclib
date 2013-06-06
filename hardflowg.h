//! \file
//! \ingroup in_out_group
//! \ingroup network_in_out_group
//! \brief Коммуникационные потоки
//!
//! Дата: 21.05.2011\n
//! Дата создания: 27.08.2009

#ifndef hardflowgH
#define hardflowgH

#include <irsdefs.h>

#if defined(IRS_WIN32)
// Standart Windows headers
#include <winsock2.h>
#elif defined(IRS_LINUX)
// Standart Linux headers
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif // IRS_WINDOWS IRS_LINUX

// Standart C++ headers
#include <string.h>

// mxsrclib headers
#include <irsstd.h>
#include <timer.h>
#include <irserror.h>
#include <mxifar.h>
#include <mxdata.h>
#include <irscpp.h>
#include <irstcpip.h>
#include <irsnetdefs.h>
#include <irssysutils.h>
#include <irsdsp.h>

#include <irsfinal.h>

#ifndef IRS_LIB_HARDFLOWG_DEBUG_TYPE
# define IRS_LIB_HARDFLOWG_DEBUG_TYPE IRS_LIB_DEBUG_NONE
#endif // !IRS_LIB_HARDFLOWG_DEBUG_TYPE

# if (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_BASE)
# define IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
# define IRS_LIB_HARDFLOWG_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG(msg)
# define IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE(msg) IRS_LIB_DBG_MSG_SRC(msg)
# define IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(msg)
# define IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(msg)
# define IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_BASE(msg) msg
# define IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_DETAIL(msg)
#elif (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
# define IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
# define IRS_LIB_HARDFLOWG_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG(msg)
# define IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE(msg) IRS_LIB_DBG_MSG_SRC(msg)
# define IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_BASE(msg)
# define IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(msg) IRS_LIB_DBG_RAW_MSG(msg)
# define IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(msg) IRS_LIB_DBG_MSG(msg)
# define IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_DETAIL(msg) msg
#else // IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_NONE
# define IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(msg)
# define IRS_LIB_HARDFLOWG_DBG_MSG_BASE(msg)
# define IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE(msg)
# define IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_BASE(msg)
# define IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(msg)
# define IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(msg)
# define IRS_LIB_HARDFLOWG_DBG_MSG_SRC_DETAIL(msg)
# define IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_DETAIL(msg)
#endif // IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_NONE

#if defined(IRS_WIN32) || defined(IRS_LINUX)
inline unsigned long extract_ip(const sockaddr_in& a_address)
{
  #if defined(IRS_WIN32)
  return a_address.sin_addr.S_un.S_addr;
  #elif defined (IRS_LINUX)
  return a_address.sin_addr.s_addr;
  #else //arch
  #error Current architecture not supported. Supported only IRS_WIN32 \
    and IRS_LINUX
  return 0;
  #endif //arch
}

inline bool operator<(const sockaddr_in& a_first_adr,
  const sockaddr_in& a_second_adr)
{
  bool first_less_second = false;
  if (extract_ip(a_first_adr) < extract_ip(a_second_adr))
  {
    first_less_second = true;
  } else if (extract_ip(a_first_adr) == extract_ip(a_second_adr)) {
    first_less_second = (a_first_adr.sin_port < a_second_adr.sin_port);
  } else {
    // Первый больше второго
  }
  return first_less_second;
};
#endif // defined(IRS_WIN32) || defined(IRS_LINUX)

namespace irs {

//! \brief Абстрактный интерфейс для классов, реализующих обмен данными по
//!   различным протоколам.
//! \ingroup in_out_group
//! \author Krasheninnikov Maxim
class hardflow_t {
public:
  typedef size_t size_type;
  typedef string_t string_type;
  typedef char_t char_type;

  enum {
    invalid_channel = 0
  };
  enum channel_switching_mode_t {
    csm_any,
    csm_ready_for_reading,
    csm_ready_for_writing
  };
  //! \brief Деструктор
  virtual ~hardflow_t() {}
  //! \brief Чтение специфического параметра
  //!
  //! \param[in] a_name - имя читаемого параметра
  virtual string_type param(const string_type &a_name) = 0;
  //! \brief Установка специфического параметра
  //!
  //! \param[in] a_name - имя устанавливаемого параметра
  //! \param[in] a_value - новое значение параметра
  virtual void set_param(const string_type &a_name,
    const string_type &a_value) = 0;
  //! \brief Чтение данных
  //!
  //! \param[in] a_channel_ident – канал, из которого читаются данные;
  //! \param[out] ap_buf – буфер, в который считываются данные;
  //! \param[in] a_size – размер считываемых данных.
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size) = 0;
  //! \brief Запись данных
  //!
  //! \param[in] a_channel_ident – канал, в который пишутся данные;
  //! \param[in] ap_buf – указатель на начало буфера с данными;
  //! \param[in] a_size – размер передаваемых данных.
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size) = 0;
  //! \brief Переход к следующему каналу
  virtual size_type channel_next() = 0;
  //! \brief Переключение режима перехода к следующему существующему каналу
  virtual void set_channel_switching_mode(channel_switching_mode_t /*a_mode*/)
  {
  }
  //! \brief Проверка на наличие канала
  //!
  //! \param[in] a_channel_ident - проверяемый канал
  virtual bool is_channel_exists(size_type a_channel_ident) = 0;
  //! \brief Элементарная операция
  virtual void tick() = 0;
};

namespace hardflow {

//! \addtogroup network_in_out_group
//! @{

typedef hardflow_t::string_type stringns_t;

enum udp_limit_connections_mode_t {
  udplc_mode_invalid,      //!< \brief Недопустимый режим
  udplc_mode_queue,        //!< \brief Учитывается переменная m_max_size
  udplc_mode_limited,      //!< \brief Учитывается переменная m_max_size
  udplc_mode_unlimited     //!< \brief Переменная m_max_size не учитывается
};

template <class address_t>
class udp_channel_list_t
{
public:
  class less_t;
  class udp_flow_t;
  typedef hardflow_t::size_type size_type;
  typedef hardflow_t::string_type string_type;
  typedef size_type id_type;
  typedef irs::deque_data_t<irs_u8> buffer_type;

  typedef address_t address_type;
  struct channel_t
  {
    address_type address;
    buffer_type buffer;
    measure_time_t lifetime;
    measure_time_t downtime;
    channel_t():
      address(),
      buffer(),
      lifetime(),
      downtime()
    {
    }
  };
  typedef map<address_type, id_type> map_address_id_type;
  typedef typename map<address_type, id_type>::iterator map_address_id_iterator;
  typedef map<id_type, channel_t> map_id_channel_type;
  typedef typename map<id_type, channel_t>::iterator map_id_channel_iterator;
  typedef typename map<id_type, channel_t>::const_iterator
    map_id_channel_const_iterator;
  typedef deque<id_type> queue_id_type;

  enum { invalid_channel = hardflow_t::invalid_channel };
  udp_channel_list_t(
    const udp_limit_connections_mode_t a_mode = udplc_mode_queue,
    const size_type a_max_size = 1000,
    const size_type a_channel_buf_max_size = 32768,
    const bool a_limit_lifetime_enabled = false,
    const double a_max_lifetime_sec = 24*60*60,
    const bool a_limit_downtime_enabled = false,
    const double a_max_downtime_sec = 60*60
  );
  bool id_get(address_type a_address, id_type* ap_id);
  bool address_get(id_type a_id, address_type* ap_address);
  // Возвращает false, если достигнут лимит, и true, если адрес успешно
  // добавлен в список или такой адрес уже есть
  void insert(address_type a_address, id_type* ap_id, bool* ap_insert_success);
  void erase(const id_type a_id);
  bool is_channel_exists(const id_type a_id);
  size_type channel_buf_max_size_get() const;
  void channel_buf_max_size_set(size_type a_channel_buf_max_size);
  double lifetime_get(const id_type a_channel_id) const;
  bool limit_lifetime_enabled_get() const;
  void limit_lifetime_enabled_set(bool a_limit_lifetime_enabled);
  double max_lifetime_get() const;
  void max_lifetime_set(double a_max_lifetime_sec);
  void downtime_timer_reset(const id_type a_channel_id);
  double downtime_get(const id_type a_channel_id) const;
  bool limit_downtime_enabled_get() const;
  void limit_downtime_enabled_set(bool a_limit_downtime_enabled);
  double max_downtime_get() const;
  void max_downtime_set(double a_max_downtime_sec);
  void channel_address_get(const id_type a_channel_id,
    address_type* ap_address) ;
  void channel_address_set(const id_type a_channel_id,
    const address_type& a_address);
  void clear();
  size_type size();
  void mode_set(const udp_limit_connections_mode_t a_mode);
  size_type max_size_get();
  void max_size_set(size_type a_max_size);
  size_type write(const address_type& a_address, const irs_u8 *ap_buf,
    size_type a_size);
  size_type read(size_type a_id, irs_u8 *ap_buf,
    size_type a_size);
  size_type channel_next();
  size_type cur_channel() const;
  void tick();
private:
  bool lifetime_exceeded(const map_id_channel_iterator);
  bool downtime_exceeded(const map_id_channel_iterator);
  void next_free_channel_id();
  udp_limit_connections_mode_t m_mode;
  size_type m_max_size;
  size_type m_channel_id;
  bool m_channel_id_overflow;
  const size_type m_channel_max_count;
  map_id_channel_type m_map_id_channel;
  map_address_id_type m_map_address_id;
  queue_id_type m_id_list;
  size_type m_buf_max_size;
  map_id_channel_iterator m_it_cur_channel;
  map_id_channel_iterator m_it_cur_channel_for_check;
  bool m_max_lifetime_enabled;
  bool m_max_downtime_enabled;
  counter_t m_max_lifetime;
  counter_t m_max_downtime;
};

template <class address_t>
irs::hardflow::udp_channel_list_t<address_t>::udp_channel_list_t(
  const udp_limit_connections_mode_t a_mode,
  const size_type a_max_size,
  const size_type a_channel_buf_max_size,
  const bool a_limit_lifetime_enabled,
  const double a_max_lifetime_sec,
  const bool a_limit_downtime_enabled,
  const double a_max_downtime_sec
):
  m_mode(a_mode),
  m_max_size(min(a_max_size, static_cast<size_type>(-1))),
  m_channel_id(invalid_channel + 1),
  m_channel_id_overflow(false),
  m_channel_max_count(static_cast<size_type>(-1)),
  m_map_id_channel(),
  //m_map_id_address(),
  m_map_address_id(),
  m_id_list(),
  m_buf_max_size(a_channel_buf_max_size),
  m_it_cur_channel(m_map_id_channel.end()),
  m_it_cur_channel_for_check(m_map_id_channel.end()),
  m_max_lifetime_enabled(a_limit_lifetime_enabled),
  m_max_downtime_enabled(a_limit_downtime_enabled),
  m_max_lifetime(irs::make_cnt_s(a_max_lifetime_sec)),
  m_max_downtime(irs::make_cnt_s(a_max_downtime_sec))
{
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::insert(address_type a_address,
  id_type* ap_id, bool* ap_insert_success)
{
  *ap_insert_success = false;
  // Ищем, есть ли уже такой адрес в списке
  map_address_id_iterator it_map_addr_id = m_map_address_id.find(a_address);
  if (it_map_addr_id == m_map_address_id.end()) {
    bool allow_add = false;
    switch (m_mode) {
      case udplc_mode_queue: {
        IRS_ASSERT(m_id_list.size() <= m_max_size);
        if (m_id_list.size() < m_max_size) {
          allow_add = true;
        } else {
          if (m_id_list.size() > 0) {
            erase(m_id_list.front());
            allow_add = true;
          } else {
            // Максимальный размер установлен в ноль
          }
        }
      } break;
      case udplc_mode_limited: {
        IRS_ASSERT(m_id_list.size() <= m_max_size);
        if (m_id_list.size() < m_max_size) {
          allow_add = true;
        } else {
          // Достигнут лимит количества записей
        }
      } break;
      case udplc_mode_unlimited: {
        if (m_id_list.size() <= m_channel_max_count) {
          allow_add = true;
        } else {
          // Достигнут лимит количества записей
        }
      } break;
      default : {
        IRS_ASSERT_MSG("Неизвестный вариант рабочего режима");
      }
    }
    if (allow_add) {
      next_free_channel_id();
      IRS_LIB_ASSERT(m_channel_id != invalid_channel);
      channel_t channel;
      channel.address = a_address;
      channel.lifetime.start();
      channel.downtime.start();
      const size_type channel_prev_count = m_id_list.size();
      std::pair<map_id_channel_iterator, bool> map_id_channel_res;
      std::pair<map_address_id_iterator, bool> map_address_id_res;
      try {
        map_id_channel_res =
          m_map_id_channel.insert(make_pair(m_channel_id, channel));
        map_address_id_res =
          m_map_address_id.insert(make_pair(a_address, m_channel_id));
        m_id_list.push_back(m_channel_id);
        *ap_id = m_channel_id;
        if (m_it_cur_channel == m_map_id_channel.end()) {
          m_it_cur_channel = m_map_id_channel.begin();
        } else {
          // Текущий канал уже установлен
        }
        if (m_it_cur_channel_for_check == m_map_id_channel.end()) {
          m_it_cur_channel_for_check = m_map_id_channel.begin();
        } else {
          // Текущий канал для проверки уже установлен
        }
        *ap_insert_success = true;
      } catch (...) {
        if (m_map_id_channel.size() > channel_prev_count) {
          m_map_id_channel.erase(map_id_channel_res.first);
        }
        if (m_map_address_id.size() > channel_prev_count) {
          m_map_address_id.erase(map_address_id_res.first);
        }
        m_id_list.resize(channel_prev_count);
      }
    } else {
      // Добавление не разрешено
    }
  } else {
    // Элемент уже присутсвует в списке
    *ap_id = it_map_addr_id->second;
    *ap_insert_success = true;
  }
}

template <class address_t>
bool irs::hardflow::udp_channel_list_t<address_t>::id_get(
  address_type a_address, id_type* ap_id)
{
  bool find_success = false;
  map_address_id_iterator it_map_addr_id = m_map_address_id.end();
  it_map_addr_id = m_map_address_id.find(a_address);
  if (it_map_addr_id != m_map_address_id.end()) {
    *ap_id = it_map_addr_id->second;
    find_success = true;
  } else {
    find_success = false;
  }
  return find_success;
}

template <class address_t>
bool irs::hardflow::udp_channel_list_t<address_t>::address_get(id_type a_id,
  address_type* ap_address)
{
  bool find_success = true;
  map_id_channel_iterator it_map_id_channel = m_map_id_channel.find(a_id);
  if (it_map_id_channel != m_map_id_channel.end()) {
    *ap_address = it_map_id_channel->second.address;
    find_success = true;
  } else {
    find_success = false;
  }
  return find_success;
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::erase(id_type a_id)
{
  map_id_channel_iterator it_erase_channel =
    m_map_id_channel.find(a_id);
  if (it_erase_channel != m_map_id_channel.end()) {
    if (m_it_cur_channel == it_erase_channel) {
      if (m_it_cur_channel != m_map_id_channel.begin()) {
        m_it_cur_channel--;
      } else {
        m_it_cur_channel = m_map_id_channel.end();
        if (m_map_id_channel.size() > 1) {
          m_it_cur_channel--;
        } else {
          // Текущий канал оставляем неопределенным,
          // так как удаляется последний канал
        }
      }
    } else {
      // Удаляемый канал не совпадает с текущим
    }
    if (m_it_cur_channel_for_check == it_erase_channel) {
      if (m_it_cur_channel_for_check != m_map_id_channel.begin()) {
        m_it_cur_channel_for_check--;
      } else {
        m_it_cur_channel_for_check = m_map_id_channel.end();
        if (m_map_id_channel.size() > 1) {
          m_it_cur_channel_for_check--;
        } else {
          // Текущий канал оставляем неопределенным,
          // так как удаляется последний канал
        }
      }
    } else {
      // Удаляемый канал не совпадает с текущим
    }
    map_address_id_iterator it_map_address_id =
      m_map_address_id.find(it_erase_channel->second.address);
    queue_id_type::iterator it_id =
      find(m_id_list.begin(), m_id_list.end(), a_id);
    m_map_id_channel.erase(it_erase_channel);
    m_map_address_id.erase(it_map_address_id);
    m_id_list.erase(it_id);
  } else {
    IRS_LIB_ASSERT_MSG("Канал отсутсвует");
  }
}

template <class address_t>
bool irs::hardflow::udp_channel_list_t<address_t>::is_channel_exists(const id_type a_id)
{
  return m_map_id_channel.find(a_id) != m_map_id_channel.end();
}

template <class address_t>
typename irs::hardflow::udp_channel_list_t<address_t>::size_type
irs::hardflow::udp_channel_list_t<address_t>::channel_buf_max_size_get() const
{
  return m_buf_max_size;
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::channel_buf_max_size_set(
  size_type a_channel_buf_max_size)
{
  m_buf_max_size = a_channel_buf_max_size;
  map_id_channel_iterator it_channel = m_map_id_channel.begin();
  while (it_channel != m_map_id_channel.end()) {
    it_channel->second.buffer.resize(m_buf_max_size);
  }
}

template <class address_t>
double irs::hardflow::udp_channel_list_t<address_t>::lifetime_get(
  const id_type a_channel_id) const
{
  double channel_lifetime = 0.;
  map_id_channel_const_iterator it_channel =
    m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    channel_lifetime = it_channel->second.lifetime.get();
  } else {
    // Канал отсутсвует
  }
  return channel_lifetime;
}

template <class address_t>
bool irs::hardflow::udp_channel_list_t<address_t>::limit_lifetime_enabled_get() const
{
  return m_max_lifetime_enabled;
}

template <class address_t>
double irs::hardflow::udp_channel_list_t<address_t>::max_lifetime_get() const
{
  return CNT_TO_DBLTIME(m_max_lifetime);
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::max_lifetime_set(
  double a_max_lifetime_sec)
{
  m_max_lifetime = irs::make_cnt_s(a_max_lifetime_sec);
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::limit_lifetime_enabled_set(
  bool a_limit_lifetime_enabled)
{
  m_max_lifetime_enabled = a_limit_lifetime_enabled;
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::downtime_timer_reset(
  const id_type a_channel_id)
{
  map_id_channel_iterator it_channel =
    m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    it_channel->second.downtime.start();
  } else {
    // Канал отсутсвует
  }
}

template <class address_t>
double irs::hardflow::udp_channel_list_t<address_t>::downtime_get(
  const id_type a_channel_id) const
{
  double channel_downtime = 0.;
  map_id_channel_const_iterator it_channel =
    m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    channel_downtime = it_channel->second.downtime.get();
  } else {
    // Канал отсутсвует
  }
  return channel_downtime;
}

template <class address_t>
bool irs::hardflow::udp_channel_list_t<address_t>::limit_downtime_enabled_get() const
{
  return m_max_downtime_enabled;
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::limit_downtime_enabled_set(
  bool a_limit_downtime_enabled)
{
  m_max_downtime_enabled = a_limit_downtime_enabled;
}

template <class address_t>
double irs::hardflow::udp_channel_list_t<address_t>::max_downtime_get() const
{
  return CNT_TO_DBLTIME(m_max_downtime);
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::max_downtime_set(
  double a_max_downtime_sec)
{
  m_max_downtime = irs::make_cnt_s(a_max_downtime_sec);
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::channel_address_get(
  const id_type a_channel_id, address_type* ap_address)
{
  map_id_channel_iterator it_channel = m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    *ap_address = it_channel->second.address;
  } else {
    // Канал не найден
  }
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::channel_address_set(
  const id_type a_channel_id, const address_type& a_address)
{
  map_id_channel_iterator it_channel = m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    map_address_id_iterator it_map_addr_id = m_map_address_id.find(
      it_channel->second.address);
    if (it_map_addr_id != m_map_address_id.end()) {
      it_channel->second.address = a_address;
      it_channel->second.buffer.clear();
      const id_type id = it_map_addr_id->second;
      m_map_address_id.erase(it_map_addr_id);
      m_map_address_id.insert(make_pair(a_address, id));
    } else {
      IRS_LIB_ASSERT_MSG("Канал с таким адресом должен существовать в "
        "обоих списках");
    }
  } else {
    // Канал не найден
  }
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::clear()
{
  m_map_id_channel.clear();
  m_map_address_id.clear();
  m_id_list.clear();
  m_it_cur_channel = m_map_id_channel.end();
  m_it_cur_channel_for_check = m_map_id_channel.end();
}

template <class address_t>
typename irs::hardflow::udp_channel_list_t<address_t>::size_type
irs::hardflow::udp_channel_list_t<address_t>::size()
{
  return m_id_list.size();
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::mode_set(
  const udp_limit_connections_mode_t a_mode)
{
  m_mode = a_mode;
  switch (m_mode) {
    case udplc_mode_queue: {
      if (m_id_list.size() > m_max_size) {
        size_type address_count_need_delete = m_id_list.size() - m_max_size;
        for (size_type id_i = 0; id_i < address_count_need_delete; id_i++) {
          erase(m_id_list.front());
        }
      } else {
        // Удаление объектов не требуется
      }
    } break;
    case udplc_mode_limited: {
      if (m_id_list.size() > m_max_size) {
        size_type address_count_need_delete = m_id_list.size() - m_max_size;
        for (size_type id_i = 0; id_i < address_count_need_delete; id_i++) {
          erase(m_id_list.back());
        }
      } else {
        // Удаление объектов не требуется
      }
    } break;
    case udplc_mode_unlimited: {
      // Удаление не требуется
    } break;
    default : {
      IRS_ASSERT_MSG("Неизвестный тип рабочего режима");
    }
  }
}

template <class address_t>
typename irs::hardflow::udp_channel_list_t<address_t>::size_type
irs::hardflow::udp_channel_list_t<address_t>::max_size_get()
{
  return m_max_size;
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::max_size_set(
  size_type a_max_size)
{
  m_max_size = min(a_max_size, m_channel_max_count);
  switch (m_mode) {
    case udplc_mode_queue: {
      if (m_id_list.size() > m_max_size) {
        const size_type channel_erase_count = m_id_list.size() - m_max_size;
        for (size_type channel_i = 0; channel_i < channel_erase_count;
          channel_i++)
        {
          erase(m_id_list.front());
          m_id_list.pop_front();
        }
      } else {
        // Удаление лишних каналов не требуется
      }
    } break;
    case udplc_mode_limited: {
      const size_type channel_erase_count = m_id_list.size() - m_max_size;
      for (size_type channel_i = 0; channel_i < channel_erase_count;
        channel_i++)
      {
        erase(m_id_list.back());
        m_id_list.pop_back();
      }
    } break;
    case udplc_mode_unlimited: {
      // Удаление каналов не требуется
    } break;
    default : {
      IRS_ASSERT_MSG("Неизвестный вариант рабочего режима");
    }
  }
}

template <class address_t>
typename irs::hardflow::udp_channel_list_t<address_t>::size_type
irs::hardflow::udp_channel_list_t<address_t>::write(
  const address_type& a_address, const irs_u8 *ap_buf, size_type a_size)
{
  size_type write_byte_count = 0;
  bool channel_exists = false;
  id_type id = 0;
  // Проверяем наличие канала и создаем, если его не существует
  insert(a_address, &id, &channel_exists);
  if (channel_exists) {
    map_id_channel_iterator it_map_id_channel =
      m_map_id_channel.find(id);
    if (it_map_id_channel != m_map_id_channel.end()) {
      buffer_type& buf = it_map_id_channel->second.buffer;
      IRS_LIB_ASSERT(buf.size() <= m_buf_max_size);
      write_byte_count = min(a_size, m_buf_max_size - buf.size());
      buf.push_back(ap_buf, ap_buf + write_byte_count);
      it_map_id_channel->second.downtime.start();
    } else {
      IRS_LIB_ASSERT_MSG("Канал отсутсвует в списке");
    }
  } else {
    // Канала с таким адресом не существует и создать его не удалось
  }
  return write_byte_count;
}

template <class address_t>
typename irs::hardflow::udp_channel_list_t<address_t>::size_type
irs::hardflow::udp_channel_list_t<address_t>::read(size_type a_id,
  irs_u8 *ap_buf, size_type a_size)
{
  size_type read_byte_count = 0;
  map_id_channel_iterator it_map_id_channel =
    m_map_id_channel.find(a_id);
  if (it_map_id_channel != m_map_id_channel.end()) {
    buffer_type& buf = it_map_id_channel->second.buffer;
    read_byte_count = min(buf.size(), a_size);
    if (read_byte_count > 0) {
      buf.copy_to(0, read_byte_count, ap_buf);
      buf.pop_front(read_byte_count);
      it_map_id_channel->second.downtime.start();
    } else {
      // Нет данных в буфере
    }
  } else {
    // Этого канала не существует
  }
  return read_byte_count;
}

template <class address_t>
typename irs::hardflow::udp_channel_list_t<address_t>::size_type
irs::hardflow::udp_channel_list_t<address_t>::channel_next()
{
  size_type cur_channel_id = invalid_channel;
  if (m_it_cur_channel != m_map_id_channel.end()) {
    m_it_cur_channel++;
  } else {
    // Текущий канал не установлен
  }
  if (m_it_cur_channel != m_map_id_channel.end()) {
    cur_channel_id = m_it_cur_channel->first;
  } else {
    m_it_cur_channel = m_map_id_channel.begin();
    if (m_it_cur_channel != m_map_id_channel.end()) {
      cur_channel_id = m_it_cur_channel->first;
    } else {
      // Нет ни одного канала
    }
  }
  return cur_channel_id;
}

template <class address_t>
typename irs::hardflow::udp_channel_list_t<address_t>::size_type
irs::hardflow::udp_channel_list_t<address_t>::cur_channel() const
{
  id_type channel_id = invalid_channel;
  if (m_it_cur_channel != m_map_id_channel.end()) {
    channel_id = m_it_cur_channel->first;
  } else {
    // Текущий канал не установлен
  }
  return channel_id;
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::tick()
{
  if (m_max_lifetime_enabled || m_max_downtime_enabled) {
    if (m_it_cur_channel_for_check == m_map_id_channel.end()) {
      m_it_cur_channel_for_check = m_map_id_channel.begin();
    } else {
      // Текущий канал правильный
    }
    if (m_it_cur_channel_for_check != m_map_id_channel.end()) {
      bool channel_need_destroy = false;
      if (m_max_lifetime_enabled) {
        if (lifetime_exceeded(m_it_cur_channel_for_check)) {
          channel_need_destroy = true;
        } else {
          // Максимальное время жизни не превышено
        }
      } else {
        // Проверка времени жизни отключена
      }
      if (m_max_downtime_enabled) {
        if (downtime_exceeded(m_it_cur_channel_for_check)) {
          channel_need_destroy = true;
        } else {
          // Максимальное время бездействие не превышено
        }
      } else {
        // Проверка времени бездействия отключена
      }
      if (channel_need_destroy) {
        erase(m_it_cur_channel_for_check->first);
      } else {
        // Время жизни и время бездействия не превышено
        m_it_cur_channel_for_check++;
      }
    } else {
      // Нет ни одного канала
      IRS_LIB_ASSERT(m_map_id_channel.empty());
    }
  } else {
    // Проверка времени жизни и времени бездействия отключена
  }
}

template <class address_t>
bool irs::hardflow::udp_channel_list_t<address_t>::lifetime_exceeded(
  const map_id_channel_iterator /*a_it_cur_channel*/)
{
  bool lifetime_exceeded_status = false;
  if (m_it_cur_channel_for_check != m_map_id_channel.end()) {
    if (m_it_cur_channel_for_check->second.lifetime.get_cnt() > m_max_lifetime)
    {
      lifetime_exceeded_status = true;
    } else {
      // Максимальное время жизни не превышено
    }
  } else {
    // Недопустимый канал
  }
  return lifetime_exceeded_status;
}

template <class address_t>
bool irs::hardflow::udp_channel_list_t<address_t>::downtime_exceeded(
  const map_id_channel_iterator /*a_it_cur_channel*/)
{
  bool downtime_exceeded_status = false;
  if (m_it_cur_channel_for_check != m_map_id_channel.end()) {
    if (m_it_cur_channel_for_check->second.downtime.get_cnt() > m_max_downtime)
    {
      downtime_exceeded_status = true;
    } else {
      // Максимальное время бездействия не превышено
    }
  } else {
    // Недопустимый канал
  }
  return downtime_exceeded_status;
}

template <class address_t>
void irs::hardflow::udp_channel_list_t<address_t>::next_free_channel_id()
{
  if (!m_channel_id_overflow) {
    m_channel_id++;
    if (m_channel_id == invalid_channel) {
      m_channel_id_overflow = true;
    } else {
      // Переполнение не произошло
    }
  } else {
    // Уже было переполнение счетчика
  }
  if (m_channel_id_overflow) {
    if (m_map_id_channel.size() < m_channel_max_count) {
      if(m_channel_id == invalid_channel) {
        m_channel_id++;
      }
      map_id_channel_iterator it_prev =
        m_map_id_channel.find(m_channel_id);
      map_id_channel_iterator it_cur = it_prev;
      if(it_cur != m_map_id_channel.end()) {
        while(true) {
          it_cur++;
          if(it_cur == m_map_id_channel.end()) {
            m_channel_id = it_prev->first + 1;
            if(m_channel_id == invalid_channel) {
              m_channel_id++;
            }
            it_prev = m_map_id_channel.find(m_channel_id);
            it_cur = it_prev;
            if(it_cur == m_map_id_channel.end()) {
              break;
            }
          } else if((it_cur->first - it_prev->first) > 1) {
            m_channel_id = it_prev->first + 1;
            break;
          }
          it_prev = it_cur;
        }
      } else {
        // Текущее значение идентификатора является уникальным
      }
    } else {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(
        "Нет свободных мест для нового канала" << endl);
      m_channel_id = invalid_channel;
    }
  } else {
    // Переполнения счетчика не было
  }
}

#if defined(IRS_WIN32) || defined(IRS_LINUX)

#ifdef IRS_LIB_DEBUG
#define HARDFLOW_DBG_ERR(error_code)\
  irs::send_format_msg(error_code,__FILE__,__LINE__)
#define HARDFLOW_DBG_MSG(error_code)\
  irs::send_format_msg(error_code,__FILE__,__LINE__)
#else // IRS_LIB_DEBUG
#define HARDFLOW_DBG_ERR(error_code)
#define HARDFLOW_DBG_MSG(error_code)
#endif // IRS_LIB_DEBUG

//! \brief Класс для получения последней ошибки
//! \author Lyashchov Maxim
class error_sock_t
{
public:
  #if defined(IRS_WIN32)
  enum error_t {
    eacess = WSAEACCES,                   //!< \brief Только для Windows
    eaddrenuse = WSAEADDRINUSE,
    eaddrnotavail = WSAEADDRNOTAVAIL,
    eafnosupport = WSAEAFNOSUPPORT,
    econnaborned = WSAECONNABORTED,
    econnrefused = WSAECONNREFUSED,
    econnreset = WSAECONNRESET,
    edestaddrreq = WSAEDESTADDRREQ,
    efault = WSAEFAULT,                   //!< \brief Только для Windows
    ehostdown = WSAEHOSTDOWN,
    ehostunread = WSAEHOSTUNREACH,
    einprogress = WSAEINPROGRESS,         //!< \brief Только для Windows
    eintr = WSAEINTR,                     //!< \brief Только для Windows
    eisconn = WSAEISCONN,
    einval = WSAEINVAL,                   //!< \brief Только для Windows
    emfile = WSAEMFILE,                   //!< \brief Только для Windows
    emsgsize = WSAEMSGSIZE,
    enetdown = WSAENETDOWN,
    enetreset = WSAENETRESET,
    enetunreach = WSAENETUNREACH,
    enobufs = WSAENOBUFS,
    enoprotoopt = WSAENOPROTOOPT,
    enotconn = WSAENOTCONN,
    enotsock = WSAENOTSOCK,
    eopnotsupp = WSAEOPNOTSUPP,
    epfnosupport = WSAEPFNOSUPPORT,
    eproclim = WSAEPROCLIM,               //!< \brief Только для Windows
    eprotonosupport = WSAEPROTONOSUPPORT,
    eprototype = WSAEPROTOTYPE,
    esocktnosupport = WSAESOCKTNOSUPPORT,
    eshutdown = WSAESHUTDOWN,             //!< \brief Только для Windows
    etimedout = WSAETIMEDOUT,
    ewouldblock = WSAEWOULDBLOCK,
    notinitialised = WSANOTINITIALISED,   //!< \brief Только для Windows
    sysnotready = WSASYSNOTREADY,         //!< \brief Только для Windows
    vernotsupported = WSAVERNOTSUPPORTED  //!< \brief Только для Windows
  };
  #elif defined(IRS_LINUX)
  enum error_t {
    eaddrenuse = EADDRINUSE,
    eaddrnotavail = EADDRNOTAVAIL,
    eafnosupport = EAFNOSUPPORT,
    econnaborned = ECONNABORTED,
    econnrefused = ECONNREFUSED,
    econnreset = ECONNRESET,
    edestaddrreq = EDESTADDRREQ,
    ehostdown = EHOSTDOWN,
    ehostunread = EHOSTUNREACH,
    eisconn = EISCONN,
    emsgsize = EMSGSIZE,
    enetdown = ENETDOWN,
    enetreset = ENETRESET,
    enetunreach = ENETUNREACH,
    enobufs = ENOBUFS,
    enoprotoopt = ENOPROTOOPT,
    enotconn = ENOTCONN,
    enotsock = ENOTSOCK,
    epfnosupport = EPFNOSUPPORT,
    eprotonosupport = EPROTONOSUPPORT,
    eprototype = EPROTOTYPE,
    esocktnosupport = ESOCKTNOSUPPORT,
    etimedout = ETIMEDOUT,
    ewouldblock = EWOULDBLOCK
  };
  #endif // IRS_WINDOWS IRS_LINUX
  typedef int errcode_type;
private:
public:
  errcode_type get_last_error();
};

#if defined(IRS_WIN32)
typedef SOCKET socketns_t;
#elif defined(IRS_LINUX)
typedef int socketns_t;
#endif // IRS_WINDOWS IRS_LINUX

//! \brief Закрытие сокета. Работает в Windows и Linux
inline void close_socket(socketns_t a_socket)
{
  #if defined(IRS_WIN32)
  closesocket(a_socket);
  #elif defined(IRS_LINUX)
  close(a_socket);
  #endif // IRS_WINDOWS IRS_LINUX
}

#if defined(IRS_WIN32)
bool lib_socket_load(WSADATA* ap_wsadata, int a_version_major,
  int a_version_minor);
#endif // defined(IRS_WIN32)

//! \brief Реализует обмен данными по UDP протоколу
//! \author Lyashchov Maxim
class udp_flow_t: public hardflow_t
{
public:
  //typedef hardflow_t::size_type size_type;
  //typedef string_t string_type;
  typedef sockaddr_in address_type;
  //! \brief Режим работы с каналами
  enum mode_t {
    invalid_mode,             //!< \brief Недопустимый режим
    mode_queue,               //!< \brief Учитывается переменная m_max_size
    mode_limited_vector,      //!< \brief Учитывается переменная m_max_size
    mode_unlimited_vector};   //!< \brief Переменная m_max_size не учитывается
  typedef int errcode_type;
  #if defined(IRS_WIN32)
  typedef SOCKET socket_type;
  typedef int socklen_type;
  typedef unsigned long in_addr_type;
  #elif defined(IRS_LINUX)
  typedef int socket_type;
  typedef socklen_t socklen_type;
  typedef in_addr_t in_addr_type;
  #endif // IRS_WINDOWS IRS_LINUX

  enum {
    def_channel_max_count = 1000,
    def_channel_buf_max_size = 0xFFFF
  };
  struct configuration_t
  {
    string_type local_host_name;
    string_type local_host_port;
    string_type remote_host_name;
    string_type remote_host_port;
    udp_limit_connections_mode_t connections_mode;
    size_type channel_max_count;
    size_type channel_buf_max_size;
    bool limit_lifetime_enabled;
    double max_lifetime_sec;
    bool limit_downtime_enabled;
    double max_downtime_sec;
    bool broadcast_allowed;
    configuration_t():
      local_host_name(empty_cstr()),
      local_host_port(empty_cstr()),
      remote_host_name(empty_cstr()),
      remote_host_port(empty_cstr()),
      connections_mode(udplc_mode_queue),
      channel_max_count(def_channel_max_count),
      channel_buf_max_size(def_channel_buf_max_size),
      limit_lifetime_enabled(false),
      max_lifetime_sec(def_max_lifetime_sec()),
      limit_downtime_enabled(false),
      max_downtime_sec(def_max_downtime_sec()),
      broadcast_allowed(false)
    {
    }
  };
private:
  //enum { m_socket_error = socket_error };
  //enum { m_invalid_socket = invalid_socket };
  struct state_info_t
  {
    bool lib_socket_loaded;
    bool sock_created;
    bool set_io_mode_sock_success;
    bool bind_sock_and_ladr_success;
    state_info_t(
    ):
      lib_socket_loaded(false),
      sock_created(false),
      set_io_mode_sock_success(false),
      bind_sock_and_ladr_success(false)
    { }
    bool get_state_start()
    {
      bool start_success =
        lib_socket_loaded &&
        sock_created &&
        set_io_mode_sock_success &&
        bind_sock_and_ladr_success;
      return start_success;
    }
  };
  enum parameter_t{
    param_invalid,
    param_adress,
    param_port,
    param_read_buf_max_size,
    param_limit_lifetime_enabled,
    param_limit_lifetime,
    param_limit_downtime_enabled,
    param_limit_downtime,
    param_channel_max_count
  };

  error_sock_t m_error_sock;
  state_info_t m_state_info;
  #if defined(IRS_WIN32)
  WSADATA m_wsd;
  #endif // IRS_WIN32
  socket_type m_sock;
  bool m_broadcast_allowed;
  string_type m_local_host_name;
  string_type m_local_host_port;
  timeval m_func_select_timeout;
  fd_set m_s_kit;
  size_type m_send_msg_max_size;
  udp_channel_list_t<address_type> m_channel_list;
  irs::raw_data_t<irs_u8> m_read_buf;
public:
  udp_flow_t(const configuration_t& a_configuration);
  //! \param[in] a_local_host_name - локальный адрес. Пример: "127.0.0.1".
  //!   Обычно можно передавать пустую строку, тогда он будет установлен
  //!   автоматически.
  //! \param[in] a_local_host_port - локальный порт. Пример: "5005". Можно
  //!   указать пустую строку (рекомендуется для клиента),
  //!   тогда будет выбран случайный, свободный порт.
  //! \param[in] a_remote_host_name - удаленный адрес. Пример: "127.0.0.1".
  //!   Можно указать пустую строку (необходимо для сервера).
  //! \param[in] a_remote_host_port - удаленный порт. Пример: "5005". Можно
  //!   указать пустую строку (необходимо для сервера).
  //! \param[in] a_mode - режим работы с каналами. Для сервера рекомендуется
  //!   устанавливать режим udplc_mode_queue, для клиента udplc_mode_limited.
  //! \param[in] a_channel_max_count - максимальное количество каналов.
  //! \param[in] a_channel_buf_max_size - максимальный размер буфера каждого
  //!   канала.
  //! \param[in] a_limit_lifetime_enabled - включает ограничение по времени
  //!   жизни каналов.
  //! \param[in] a_max_lifetime_sec - время жизни каналов в секундах.
  //! \param[in] a_limit_downtime_enabled - включает ограничение по времени
  //!   бездействия каналов. При превышении заданного порога, канал удаляется.
  //! \param[in] a_max_downtime_sec - максимальное время бездействия каналов в
  //!   секундах.
  //!
  //! Пример конфигурации сервера:
  //! \code
  //!   irs::hardflow::udp_flow_t udp_flow(irst(""), irst("5005"), irst(""),
  //!      irst(""), irs::hardflow::udplc_mode_queue, 1000, 0xFFFF,
  //!     false, 24*60*60, true, 10);
  //! \endcode
  //! Пример конфигурации клиента:
  //! \code
  //! irs::hardflow::udp_flow_t udp_flow(irst(""), irst(""), irst("127.0.0.1"),
  //!   irst("5005"), irs::hardflow::udplc_mode_limited, 1, 0xFFFF,
  //!   false, 24*60*60, false, 10);
  //! \endcode
  udp_flow_t(
    const string_type& a_local_host_name = empty_cstr(),
    const string_type& a_local_host_port = empty_cstr(),
    const string_type& a_remote_host_name = empty_cstr(),
    const string_type& a_remote_host_port = empty_cstr(),
    const udp_limit_connections_mode_t a_mode = udplc_mode_queue,
    const size_type a_channel_max_count = def_channel_max_count,
    const size_type a_channel_buf_max_size = def_channel_buf_max_size,
    const bool a_limit_lifetime_enabled = false,
    const double a_max_lifetime_sec = def_max_lifetime_sec(),
    const bool a_limit_downtime_enabled = false,
    const double a_max_downtime_sec = def_max_downtime_sec()
  );
  virtual ~udp_flow_t();
private:
  void init(const string_type& a_remote_host_name,
    const string_type& a_remote_host_port);
  void load_lib_socket();
  void start();
  void sock_close();
  void local_addr_init(sockaddr_in* ap_sockaddr, bool* ap_init_success);
  void addr_init(const string_type& a_remote_host_name,
    const string_type& a_remote_host_port,
    sockaddr_in* ap_sockaddr,
    bool* ap_init_success);
  bool adress_str_to_adress_binary(
    const string_type& a_adress_str, in_addr_type* ap_adress_binary);
  bool detect_func_single_arg(string_type a_param, string_type* ap_func_name,
    string_type* ap_arg) const;
public:
  virtual string_type param(const string_type &a_param_name);
  virtual void set_param(const string_type &a_param_name,
    const string_type &a_value);
  //! \details В случае если функция read возвращает size_type == 0, буфер
  //!   ap_buf может быть испорчен.
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  virtual void tick();

  static const char_type* empty_cstr();
  static double def_max_lifetime_sec();
  static double def_max_downtime_sec();
};

enum {
  //! \brief Константа означает, что для локального порта
  //! берется любой свободный
  make_udp_flow_port_none = 0
};
//! \brief Создание серевера udp_flow_t
handle_t<hardflow_t> make_udp_flow_server(
  const udp_flow_t::string_type& a_local_port,
  const double a_max_downtime_sec = 10.,
  const udp_flow_t::size_type a_channel_max_count =
    udp_flow_t::def_channel_max_count
);
//! \brief Создание клиента udp_flow_t
handle_t<hardflow_t> make_udp_flow_client(
  const udp_flow_t::string_type& a_remote_address = udp_flow_t::empty_cstr(),
  const udp_flow_t::string_type& a_remote_port = udp_flow_t::empty_cstr()
);

//! \brief Сервер для передачи данных по TCP протоколу
//! \authors
//! - Sergeev Sergey
//!     Реализация под Linux
//! - Lyashchov Maksim
//!     Реализация под Windows
//!
//! Создаем сокет сервера(socket()) и присваиваем ему локальный порт и
//!   адрес (bind()), при этом адреса для входящих соединений могут быть
//!   произвольными. Создаем очередь запросов на соединение (listen()),
//!   при этом сокет переводится в режим ожидания запросов со стороны клиентов.
//!   Если очередь заполнена, то последующие запросы будут игнорироваться.
//!   Когда сервер готов обслужить очередной запрос, он создает сокет для
//!   входящего соединения (accept()) и присваивает ему незанятый номер
//!   канала (new_channel()). После завершения работы с клиентом, выделенный
//!   для соединения сокет и связанный с ним канал удаляются
//!   (close_socket(), m_map_channel_sock.erase()).
class tcp_server_t: public hardflow_t
{
public:
  //typedef hardflow_t::size_type size_type;
  typedef socketns_t socket_type;

  tcp_server_t(irs_u16 local_port);
  virtual ~tcp_server_t();
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual void tick();
  virtual string_type param(const string_type &a_name);
  virtual void set_param(const string_type &a_name,
    const string_type &a_value);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);

private:
  //enum { m_socket_error = socket_error };
  //enum { m_invalid_socket = invalid_socket };
  #if defined(IRS_WIN32)
  WSADATA m_wsd;
  #endif // IRS_WIN32
  struct sockaddr_in m_addr;
  struct timeval m_serv_select_timeout;
  fd_set m_read_fds;
  fd_set m_write_fds;
  bool m_is_open;
  irs_u16 m_local_port;
  socket_type m_server_sock;
  map<size_type, int> m_map_channel_sock;
  map<size_type, int>::iterator mp_map_channel_sock_it;
  size_type m_channel;
  bool m_channel_id_overflow;
  const size_type m_channel_max_count;

  //! \brief Запуск сервера
  void start_server();
  //! \brief Остановка сервера
  void stop_server();
  //! \brief Создание нового канала
  void new_channel();
};

//! \brief Клиент для передачи данных по TCP протоколу
//! \authors
//! - Sergeev Sergey
//!     Реализация под Linux
//! - Lyashchov Maksim
//!     Реализация под Windows
//!
//! \details Создаем сокет клиента (socket()) и переводим его в неблокирующий
//! режим (fcntl()). Устанавливаем соединение с сервером (connect())
//! по заданному порту и адресу. Теперь клиент готов к выполнению операций
//! чтения и записи. В случае разрыва соединения сокет клиента
//! закрывается (close_socket()).
class tcp_client_t: public hardflow_t
{
public:
  //typedef hardflow_t::size_type size_type;
  typedef socketns_t socket_type;

  tcp_client_t (mxip_t dest_ip, irs_u16 dest_port);
  virtual ~tcp_client_t();
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual void tick();
  virtual string_type param(const string_type &a_param_name);
  virtual void set_param(const string_type &a_param_name,
    const string_type &a_param_value);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);

private:
  typedef int errcode_type;

  error_sock_t m_error_sock;
  struct state_info_t
  {
    bool lib_socket_loaded;
    bool sock_created;
    // Статус установки сокета в неблокирующий режим
    bool set_io_mode_sock_success;
    bool connect_sock_success;
    state_info_t(
    ):
      lib_socket_loaded(false),
      sock_created(false),
      set_io_mode_sock_success(false),
      connect_sock_success(false)
    { }
    bool get_state_start()
    {
      bool start_success =
        lib_socket_loaded &&
        sock_created &&
        set_io_mode_sock_success &&
        connect_sock_success;
      return start_success;
    }
  } m_state_info;
  #if defined(IRS_WIN32)
  WSADATA m_wsd;
  #endif // IRS_WIN32
  struct sockaddr_in m_addr;
  timeval m_client_select_timeout;
  fd_set m_read_fds;
  fd_set m_write_fds;
  socket_type m_client_sock;
  bool m_is_open;
  mxip_t m_dest_ip;
  irs_u16 m_dest_port;
  size_type m_channel;
  //! \brief Запуск работы клиента
  void start_client();
  //! \brief Запуск работы сервера
  void stop_client();
};
#endif //defined(IRS_WIN32) || defined(IRS_LINUX)

//! \brief Прием/передача фиксированных объемов данных с
//!   использованием hardflow_t
//! \author Sergeev Sergey
//!
//! Позволяет устанавливать независимый тайм-аут на прием и передачу
class fixed_flow_t
{
public:
  typedef hardflow_t::size_type size_type;

  //! \brief Статус операции чтения или записи
  enum status_t {
    //! \brief Операция выполняется
    status_wait,
    //! \brief Операция завершилась успешно
    status_success,
    //! \brief Операция завершилась с ошибкой
    //!
    //! Ошибка может произойти по причине тайм-аута или
    //! по причине разрыва соединения
    status_error
  };

  fixed_flow_t(hardflow_t* ap_hardflow = IRS_NULL);
  void read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  void write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  //! \brief Возвращает статус операции чтения
  status_t read_status() const;
  //! \brief Обрывает чтение и возвращает количество прочитанных байт
  size_type read_abort();
  //! \brief Возвращает статус операции записи
  status_t write_status() const;
  //! \brief Обрывает запись и возвращает количество записанных байт
  size_type write_abort();
  //! \brief Элементарная операция
  void tick();
  //! \brief Устанавливает указатель на интерфейс hardflow_t объекта,
  //! реализующего обмен данными
  void connect(hardflow_t* ap_hardflow);
  //! \brief Возвращает количество прочитанных данных на текущий момент
  size_type read_byte_count() const;
  //! \brief Возвращает колечество записанных данных на текущий момент
  size_type write_byte_count() const;
  //! \brief Устанавливает тайм-аут чтения
  void read_timeout(counter_t a_read_timeout);
  //! \brief Возвращает тайм-аут записи
  counter_t read_timeout() const;
  //! \brief Устанавливает тайм-аут чтения
  void write_timeout(counter_t a_write_timeout);
  //! \brief Возвращает тайм-аут записи
  counter_t write_timeout() const;
private:
  hardflow_t* mp_hardflow;
  size_type m_read_channel;
  irs_u8* mp_read_buf_cur;
  irs_u8* mp_read_buf;
  size_type m_read_size_need;
  size_type m_read_size_rest;
  status_t m_read_status;
  timer_t m_read_timeout;

  #if (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
  bool m_channel_not_exists;
  #endif // IRS_LIB_DEBUG_DETAIL

  size_type m_write_channel;
  const irs_u8* mp_write_buf_cur;
  const irs_u8* mp_write_buf;
  size_type m_write_size_need;
  size_type m_write_size_rest;
  status_t  m_write_status;
  timer_t m_write_timeout;
};

//! \brief Поток для передачи данных по протоколу UDP
//! \author Sergeev Sergey
class simple_udp_flow_t: public hardflow_t
{
public:
  //typedef hardflow_t::size_type size_type;
  /*********************************

          Сделать режим клиента

  *********************************/
  simple_udp_flow_t(
    simple_tcpip_t* ap_simple_udp,
    mxip_t a_local_ip,
    irs_u16 a_local_port,
    mxip_t a_dest_ip,
    irs_u16 a_dest_port,
    size_type a_channel_max_count = 3
  );
  virtual ~simple_udp_flow_t();
  virtual string_type param(const string_type &a_name);
  virtual void set_param(const string_type &a_name,
    const string_type &a_value);
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  virtual void tick();

private:
  struct udp_channel_t {
    mxip_t ip;
    irs_u16 port;
    udp_channel_t():
      ip(mxip_t::zero_ip()),
      port(0)
    {
    }
    udp_channel_t(mxip_t a_ip, irs_u16 a_port):
      ip(a_ip),
      port(a_port)
    {
    }
    bool operator==(udp_channel_t a_udp_channel)
    {
      return ((ip == a_udp_channel.ip) && (port == a_udp_channel.port));
    }
  };
  class channel_equal_t
  {
  public:
    channel_equal_t(mxip_t a_ip, irs_u16 a_port):
      m_ip(a_ip),
      m_port(a_port)
    {
    }
    bool operator()(udp_channel_t a_udp_channel)
    {
      if (a_udp_channel.port == m_port) {
        if (a_udp_channel.ip == m_ip) {
          return true;
        }
      }
      return false;
    }
  private:
    mxip_t m_ip;
    irs_u16 m_port;
  };

  simple_tcpip_t* mp_simple_udp;
  mxip_t m_local_ip;
  irs_u16 m_local_port;
  mxip_t m_dest_ip;
  irs_u16 m_dest_port;
  size_type m_channel;
  size_type m_cur_channel;
  irs_u8* mp_recv_buf;
  irs_u8* mp_recv_buf_cur;
  irs_u8* mp_send_buf;
  const size_type m_channel_max_count;
  deque<udp_channel_t> m_channel_list;
  //deque<udp_channel_t>::iterator m_channel_list_it;

  void new_channel(mxip_t a_ip, irs_u16 a_port);
  void delete_channels_by_downtime();
  void view_channel_list();
  size_type channel_list_index(size_type a_channel_ident);
  size_type channel_ident_from_index(size_type a_list_index);
};

//! \brief Поток для передачи данных по протоколу TCP
//! \author Sergeev Sergey
class simple_tcp_flow_t: public hardflow_t
{
public:
  simple_tcp_flow_t(
    simple_tcpip_t* ap_simple_tcp,
    mxip_t a_local_ip,
    irs_u16 a_local_port,
    mxip_t a_dest_ip,
    irs_u16 a_dest_port
  );
  virtual ~simple_tcp_flow_t();
  virtual string_type param(const string_type& a_name);
  virtual void set_param(const string_type& a_name,
    const string_type& a_value);
  virtual size_type read(size_type a_channel_ident, irs_u8* ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8* ap_buf,
    size_type a_size);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  virtual void tick();
private:
  simple_tcpip_t* mp_simple_tcp;
  mxip_t m_local_ip;
  irs_u16 m_local_port;
  mxip_t m_dest_ip;
  irs_u16 m_dest_port;
  size_type m_cur_channel;
  irs_u8* mp_recv_buf;
  irs_u8* mp_recv_buf_cur;
  irs_u8* mp_send_buf;
  mxip_t m_cur_dest_ip;
  irs_u16 m_cur_dest_port;
};
//! @}

//! \brief Прослойка для работы с prologix-gpib
//! \author
class prologix_flow_t: public irs::hardflow_t
{
  irs::handle_t<irs::hardflow_t> mp_hardflow;
  irs::string_t m_buffer;
  irs::hardflow::fixed_flow_t m_fixed_flow;
  bool m_is_write;
  bool m_is_write_wait;
  bool m_is_read;
  bool m_is_read_wait;
  enum mode_t {
    mode_free,
    mode_start,
    mode_wait,
    mode_error,
    mode_write,
    mode_write_wait,
    mode_read,
    mode_read_wait,
    mode_start_read,
    mode_start_read_wait
  };
  mode_t m_write_mode;
  mode_t m_read_mode;
  mode_t m_init_mode;
  size_type m_channel_ident;
  irs::raw_data_t<irs_u8> m_write_data;
  const size_t m_read_chunk_size;
  irs::string_t m_end_line_write;
  irs::string_t m_end_line_read;
  irs::string_t m_read_string;
  irs::raw_data_t<irs_u8> m_read_data;
  bool m_init_success;
  vector<irs::string_t> m_init_command;
  unsigned int m_init_count;
  size_type m_init_channel_ident;
  irs::raw_data_t<irs_u8> m_transmit_data;
  static irs::raw_data_t<irs_u8> u8_from_str(const irs::string_t& a_string);
  static irs::string_t str_from_u8(const irs::raw_data_t<irs_u8>& a_data);

public:
  enum end_line_t {
    cr_lf = 0,
    cr = 1,
    lf = 2,
    none = 3
  };
  prologix_flow_t(irs::hardflow_t* ap_hardflow, int a_address,
    end_line_t a_read_end_line = cr_lf, end_line_t a_write_end_line = cr_lf,
    int a_timeout_read_ms = 1);
  virtual ~prologix_flow_t();
  virtual string_type param(const string_type& a_name);
  virtual void set_param(const string_type& a_name,
    const string_type& a_value);
  virtual size_type read(size_type a_channel_ident, irs_u8* ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8* ap_buf,
    size_type a_size);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  virtual void tick();
};

class connector_t: public hardflow_t
{
public:
  connector_t(hardflow_t* ap_hardflow):
    mp_hardflow(ap_hardflow)
  {
  }
  virtual string_type param(const string_type& a_name)
  {
    return mp_hardflow->param(a_name);
  }
  virtual void set_param(const string_type& a_name,
    const string_type &a_value)
  {
    mp_hardflow->set_param(a_name, a_value);
  }
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size)
  {
    return mp_hardflow->read(a_channel_ident, ap_buf, a_size);
  }
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size)
  {
    return mp_hardflow->write(a_channel_ident, ap_buf, a_size);
  }
  virtual size_type channel_next()
  {
    return mp_hardflow->channel_next();
  }
  virtual void set_channel_switching_mode(channel_switching_mode_t a_mode)
  {
    mp_hardflow->set_channel_switching_mode(a_mode);
  }
  virtual bool is_channel_exists(size_type a_channel_ident)
  {
    return mp_hardflow->is_channel_exists(a_channel_ident);
  }
  virtual void tick()
  {
    return mp_hardflow->tick();
  }
  void hardflow(hardflow_t* ap_hardflow)
  {
    mp_hardflow = ap_hardflow;
  }
private:
  hardflow_t* mp_hardflow;
};

class echo_t: public hardflow_t
{
public:
  echo_t(size_type a_channel_count = 1,
    size_type a_channel_buf_max_size = 16384);
  virtual string_type param(const string_type &a_name);
  virtual void set_param(const string_type &a_name,
    const string_type &a_value);
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  virtual void tick();
  size_type channel_data_size(size_type a_channel_ident);
private:
  typedef vector<irs_u8> channel_buf_type;
  vector<vector<irs_u8> > m_channels;
  size_type m_buf_max_size;
  size_type m_channel;
};

class memory_flow_t: public hardflow_t
{
public:
  memory_flow_t(size_type a_channel_receive_buffer_max_size = 1024);
  virtual ~memory_flow_t();
  virtual string_type param(const string_type &a_name);
  virtual void set_param(const string_type &a_name,
    const string_type &a_value);
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  virtual void tick();
  void add_connection(memory_flow_t* ap_memory_flow,
    size_type a_local_channel_ident, size_type a_remote_channel_ident);
  void delete_connection(memory_flow_t* ap_memory_flow,
    size_type a_local_channel_ident, size_type a_remote_channel_ident);
  size_type received_data_size(size_type a_channel_ident);
private:
  struct channel_t
  {
    memory_flow_t* memory_flow;
    size_type remote_channel_ident;
    channel_t(memory_flow_t* ap_memory_flow,
      size_type a_remote_channel_ident):
      memory_flow(ap_memory_flow),
      remote_channel_ident(a_remote_channel_ident)
    {
    }
    bool operator==(const channel_t& a_channel) const
    {
      return (memory_flow == a_channel.memory_flow) &&
        (remote_channel_ident == a_channel.remote_channel_ident);
    }
  };
  typedef std::multimap<size_type, channel_t> channels_type;

  void add_part_connection(memory_flow_t* ap_memory_flow,
    size_type a_local_channel_ident, size_type a_remote_channel_ident);
  void delete_part_connection(memory_flow_t* ap_memory_flow,
    size_type a_local_channel_ident, size_type a_remote_channel_ident);
  size_type write_to_local_buffer(size_type a_channel_ident,
    const irs_u8 *ap_buf, size_type a_size);
  std::multimap<size_type, channel_t>::iterator find_channel(
    memory_flow_t* ap_memory_flow, size_type a_local_channel_ident,
    size_type a_remote_channel_ident);

  std::multimap<size_type, channel_t> m_channels;
  std::map<size_type, vector<irs_u8> > m_channel_buffers;
  size_type m_receive_buf_max_size;
  std::map<size_type, vector<irs_u8> >::const_iterator m_channel;
};

//! @}

} // namespace hardflow

irs_status_t to_irs_status(hardflow::fixed_flow_t::status_t a_status);

} // namespace irs

#endif //hardflowgH
