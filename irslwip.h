#ifndef LWIP_HARDFLOW_H
#define LWIP_HARDFLOW_H

#include <irsdefs.h>

#include <hardflowg.h>

#define USE_DHCP

#ifdef USE_LWIP 

extern "C" {

#pragma diag_suppress=Pa181
#include <lwip/init.h>
#include <lwip/ip_addr.h>
  
#ifndef IRS_STM32H7xx
#include <lwip/timers.h>
#else // defined(IRS_STM32H7xx)
#include <lwip/timeouts.h>
#endif // IRS_STM32H7xx
  
#include <lwip/tcp.h>
#include <lwip/udp.h>
#include <lwip/mem.h>
#include <lwip/memp.h>
#include <lwip/dhcp.h>
#include <netif/etharp.h>
#pragma diag_default=Pa181  

} // extern "C"

#endif // USE_LWIP

#if defined(IRS_STM32H7xx)// || defined(ARMxxx)
#define IRSLIB_USE_LWIP_CONTROL
#endif

#ifdef IRSLIB_USE_LWIP_CONTROL

#ifdef IRS_STM32H7xx
extern "C" {
#include "ethernet_h7.h"
}
#else
#error "Не подключен файл ethernet для текущего микроконтроллера"
#endif // include ethernet

#endif // IRSLIB_USE_LWIP_CONTROL

#include <irsfinal.h>

#ifdef USE_LWIP

#ifndef IRS_LIB_LWIP_ETH_DEBUG_TYPE
# define IRS_LIB_LWIP_ETH_DEBUG_TYPE IRS_LIB_DEBUG_NONE
#endif // IRS_LIB_LWIP_ETH_DEBUG_TYPE

# if (IRS_LIB_LWIP_ETH_DEBUG_TYPE == IRS_LIB_DEBUG_BASE)
# define IRS_LIB_LWIP_ETH_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
# define IRS_LIB_LWIP_ETH_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG(msg)
# define IRS_LIB_LWIP_ETH_DBG_MSG_SRC_BASE(msg) IRS_LIB_DBG_MSG_SRC(msg)
# define IRS_LIB_LWIP_ETH_DBG_RAW_MSG_DETAIL(msg)
# define IRS_LIB_LWIP_ETH_DBG_MSG_DETAIL(msg)
# define IRS_LIB_LWIP_ETH_DBG_RAW_MSG_BLOCK_BASE(msg) msg
# define IRS_LIB_LWIP_ETH_DBG_RAW_MSG_BLOCK_DETAIL(msg)
#elif (IRS_LIB_LWIP_ETH_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
# define IRS_LIB_LWIP_ETH_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
# define IRS_LIB_LWIP_ETH_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG(msg)
# define IRS_LIB_LWIP_ETH_DBG_MSG_SRC_BASE(msg) IRS_LIB_DBG_MSG_SRC(msg)
# define IRS_LIB_LWIP_ETH_DBG_RAW_MSG_BLOCK_BASE(msg)
# define IRS_LIB_LWIP_ETH_DBG_RAW_MSG_DETAIL(msg) IRS_LIB_DBG_RAW_MSG(msg)
# define IRS_LIB_LWIP_ETH_DBG_MSG_DETAIL(msg) IRS_LIB_DBG_MSG(msg)
# define IRS_LIB_LWIP_ETH_DBG_RAW_MSG_BLOCK_DETAIL(msg) msg
#else // IRS_LIB_LWIP_ETH_DEBUG_TYPE == IRS_LIB_DEBUG_NONE
# define IRS_LIB_LWIP_ETH_DBG_RAW_MSG_BASE(msg)
# define IRS_LIB_LWIP_ETH_DBG_MSG_BASE(msg)
# define IRS_LIB_LWIP_ETH_DBG_MSG_SRC_BASE(msg)
# define IRS_LIB_LWIP_ETH_DBG_RAW_MSG_BLOCK_BASE(msg)
# define IRS_LIB_LWIP_ETH_DBG_RAW_MSG_DETAIL(msg)
# define IRS_LIB_LWIP_ETH_DBG_MSG_DETAIL(msg)
# define IRS_LIB_LWIP_ETH_DBG_MSG_SRC_DETAIL(msg)
# define IRS_LIB_LWIP_ETH_DBG_RAW_MSG_BLOCK_DETAIL(msg)
#endif // IRS_LIB_LWIP_ETH_DEBUG_TYPE == IRS_LIB_DEBUG_NONE

namespace irs {

namespace lwip {

class ethernet_t
{
public:
  struct configuration_t
  {
    mxip_t ip;
    mxip_t netmask;
    mxip_t gateway;
    bool dhcp_enabled;
    configuration_t():
      ip(mxip_t::any_ip()),
      netmask(mxip_t::any_ip()),
      gateway(mxip_t::any_ip()),
      dhcp_enabled(false)
    {
    }
  };
  ethernet_t(simple_ethernet_t* ap_simple_ethernet,
    const configuration_t& a_configuration);
  ~ethernet_t();
  mxip_t get_ip();
  mxip_t get_netmask();
  mxip_t get_gateway();
  netif* get_netif();
  void tick();
private:
  void start_dhcp();
  void lwip_tick();
  static err_t low_level_output(struct netif *ap_netif, struct pbuf *p);
  err_t ethernetif_input();
  static struct pbuf * low_level_input();
  static err_t ethernetif_init(struct netif *ap_netif);
  static void low_level_init(struct netif *ap_netif);
  //static int accept_udp_port(u16_t a_port);
  static simple_ethernet_t* mp_simple_ethernet;
  netif m_netif;
  loop_timer_t m_sys_check_timeouts_loop_timer;
  loop_timer_t m_etharp_tmr_loop_timer;
  #ifdef USE_DHCP
  loop_timer_t m_dhcp_fine_tmr_loop_timer;
  loop_timer_t m_dhcp_coarse_tmr_loop_timer;
  #endif // USE_DHCP
};


#ifdef IRSLIB_USE_LWIP_CONTROL

//Переписанный из ethernet_t класс, который использует подключенный файл
//с реализацией ethernet
class lwip_control_t
{
public:
  struct config_t
  {
    mxip_t ip;
    mxip_t netmask;
    mxip_t gateway;
    bool dhcp_enabled;
    config_t():
      ip(mxip_t::any_ip()),
      netmask(mxip_t::any_ip()),
      gateway(mxip_t::any_ip()),
      dhcp_enabled(false)
    {
    }
  };
  lwip_control_t(const config_t& a_configuration);
  ~lwip_control_t();
  mxip_t get_ip();
  void get_dhcp_ip(uint8_t* a_iptxt);
  mxip_t get_netmask();
  mxip_t get_gateway();
  netif* get_netif();
  void tick();
  mxmac_t get_mac();
private:
  void start_dhcp();
  void lwip_tick();
  
  mxmac_t st_generate_mac(device_code_t a_device_code);
  //static err_t low_level_output(struct netif *ap_netif, struct pbuf *p);
  //err_t ethernetif_input();
  //static struct pbuf * low_level_input();
  //static err_t ethernetif_init(struct netif *ap_netif);
  //static void low_level_init(struct netif *ap_netif);
  mxmac_t m_mac;
  netif m_netif;
  loop_timer_t m_sys_check_timeouts_loop_timer;
  loop_timer_t m_etharp_tmr_loop_timer;
  loop_timer_t m_check_link_timer;
  #ifdef USE_DHCP
  loop_timer_t m_dhcp_fine_tmr_loop_timer;
  loop_timer_t m_dhcp_coarse_tmr_loop_timer;
  #endif // USE_DHCP
};

#endif // IRSLIB_USE_LWIP_CONTROL


} // namespace lwip

namespace hardflow {

namespace lwip {

class buffers_t
{
public:
  typedef size_t size_type;
  buffers_t(size_type a_buf_max_count = 2, size_type a_buf_max_size = 1500);
  size_type write(size_type a_channel_id, const irs_u8* ap_pbuf,
    size_type a_size);
  template <class DataReader>
  size_type write(size_type a_channel_id, DataReader a_data);
  size_type available_for_write(size_type a_channel_id) const;
  size_type available_for_write_to_new_channel() const;
  size_type read(size_type a_channel_id, irs_u8* ap_buf, size_type a_size);
  size_type available_for_read(size_type a_channel_id) const;
  void free_buffer(size_type a_channel_id);
  size_type channel_next_available_for_reading();
  void reserve_buffers();
private:
  irs::deque_data_t<irs_u8>* create_buf(size_type a_channel_id);
  void insert_to_id_buf_pos(size_type a_channel_id, size_type a_pos);
  void erase_from_id_buf_pos(size_type a_channel_id);
  void erase_from_id_buf_pos(map<size_type, size_type>::iterator a_it);
  size_type m_buf_max_count;
  size_type m_buf_max_size;
  vector<irs::deque_data_t<irs_u8> > m_buffers;
  map<size_type, size_type> m_id_buf_pos;
  vector<size_type> m_free_buffers;
  map<size_type, size_type>::iterator m_channel_for_read;
};

template <class DataReader>
buffers_t::size_type
buffers_t::write(size_type a_channel_id, DataReader a_data)
{
  if (a_data.size() == 0) {
    return 0;
  }

  if (a_data.size() > m_buf_max_size) {
    return 0;
  }

  irs::deque_data_t<irs_u8>* buf = create_buf(a_channel_id);
  if (!buf) {
    return 0;
  }

  IRS_LIB_ASSERT(buf->size() <= m_buf_max_size);
  const size_type available_size = (m_buf_max_size - buf->size());
  size_type size = min(available_size, a_data.size());

  #ifndef IRS_NOEXCEPTION
  try {
  #endif // IRS_NOEXCEPTION
    buf->reserve(buf->size() + a_data.size());
    for (const irs_u8* start = a_data.data(); start != NULL;
        start = a_data.next()) {
      const irs_u8* end = start + a_data.data_size();
      buf->push_back(start, end);
    }
    return size;
  #ifndef IRS_NOEXCEPTION
  } catch (std::bad_alloc&) {
    return 0;
  }
  #endif // IRS_NOEXCEPTION
}

class pbuf_reader_t
{
public:
  typedef size_t size_type;
  pbuf_reader_t(const pbuf* ap_pbuf):
    mp_pbuf(ap_pbuf)
  {
  }
  size_type size() const
  {
    return mp_pbuf->tot_len;
  }
  const irs_u8* next()
  {
    if (!mp_pbuf) {
      return NULL;
    }
    mp_pbuf = mp_pbuf->next;
    return reinterpret_cast<const irs_u8*>(mp_pbuf->payload);
  }
  const irs_u8* data() const
  {
    if (!mp_pbuf) {
      return NULL;
    }
    return reinterpret_cast<irs_u8*>(mp_pbuf->payload);
  }
  size_type data_size() const
  {
    if (!mp_pbuf) {
      return 0;
    }
    return mp_pbuf->len;
  }
private:
  pbuf_reader_t();
  const pbuf* mp_pbuf;
};

class tcp_server_t: public hardflow_t
{
public:
  typedef std::size_t size_type;
  struct configuration_t
  {
    //! \brief Максимальный размер буфера канала
    size_type channel_buffer_max_size;
    //! \brief Максимальное количество буферов каналов, может быть меньше либо
    //!   равно количеству каналов, задаваемому через параметр конструктора
    //!   a_channel_max_count
    size_type channel_buffer_max_count;
    //! \brief Резервировать память для буферов
    bool reserve_buffers;
    configuration_t():
      channel_buffer_max_size(1500),
      channel_buffer_max_count(2),
      reserve_buffers(true)
    {
    }
  };
  tcp_server_t(const mxip_t& a_local_ip, irs_u16 a_local_port,
    size_type a_channel_max_count = 3,
    const configuration_t& a_configuration = configuration_t());
  ~tcp_server_t();
  virtual string_type param(const string_type& a_param_name);
  virtual void set_param(const string_type& a_param_name,
    const string_type& a_value);
  virtual size_type read(size_type a_channel_ident, irs_u8* ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8* ap_buf,
    size_type a_size);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  virtual void set_channel_switching_mode(channel_switching_mode_t a_mode);
  virtual void tick();
  bool listen(const mxip_t& a_ip, irs_u16 a_port);
private:
  struct channel_t
  {
    tcp_server_t* server;
    size_type id;
    tcp_pcb* pcb;
    bool marked_for_erase;
    channel_t():
      server(NULL),
      id(0),
      pcb(NULL),
      marked_for_erase(false)
    {
    }
  };
  enum { pool_interval = 10 };
  bool close_connection(channel_t* ap_channel);
  channel_t* create_channel(tcp_pcb *ap_pcb);
  void erase_channel(channel_t* ap_channel);
  static err_t accept(void *arg, tcp_pcb *pcb, err_t err);
  static err_t recv(void *arg, tcp_pcb *pcb, pbuf *p, err_t err);
  static void conn_err(void *arg, err_t err);
  static err_t poll(void *arg, tcp_pcb *pcb);
  static err_t sent(void *arg, tcp_pcb *pcb, u16_t len);
  tcp_pcb* mp_pcb;
  std::deque<irs::handle_t<channel_t> > m_channels;
  std::map<size_type, channel_t*> m_channels_map;
  const size_type m_channel_max_count;
  buffers_t m_buffers;
  size_type m_current_processed_channel;
  size_type m_current_channel;
  channel_switching_mode_t m_mode;
  size_type m_last_id;
  static size_type m_read_channel;
};

class tcp_client_t: public hardflow_t
{
public:
  struct configuration_t
  {
    //! \brief Максимальный размер буфера
    size_type buffer_max_size;
    //! \brief Резервировать память для буфера
    bool reserve_buffer;
    configuration_t():
      buffer_max_size(1500),
      reserve_buffer(true)
    {
    }
  };
  tcp_client_t(const mxip_t& a_local_ip, irs_u16 a_local_port,
    const mxip_t& a_dest_ip, irs_u16 a_dest_port,
    const configuration_t& a_configuration = configuration_t());
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
  enum { pool_interval = 10 };
  bool connect();
  bool close_connection();
  static err_t connected(void* arg, tcp_pcb* tpcb, err_t err);
  static err_t recv(void *arg, tcp_pcb *pcb, pbuf *p, err_t err);
  static void conn_err(void *arg, err_t err);
  static err_t poll(void *arg, tcp_pcb *pcb);
  static err_t sent(void *arg, tcp_pcb *pcb, u16_t len);
  const mxip_t m_local_ip;
  const irs_u16 m_local_port;
  const mxip_t m_dest_ip;
  const irs_u16 m_dest_port;
  tcp_pcb* mp_pcb;
  bool m_marked_for_erase;
  size_type m_channel_id;
  irs::deque_data_t<irs_u8> m_buffer;
  size_type m_buffer_max_size;
  bool m_need_connect;
  bool m_connection_is_established;
  timer_t m_delay_before_connect;
};

#if LWIP_UDP

#ifndef __ICCAVR__
template <class address_t>
class udp_channels_t
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
    measure_time_t lifetime;
    measure_time_t downtime;
    channel_t():
      address(),
      lifetime(),
      downtime()
    {
    }
  };
  typedef map<address_type, id_type> map_address_id_type;
  typedef typename map<address_type, id_type>::iterator map_address_id_iterator;
  typedef typename map<address_type, id_type>::const_iterator
    map_address_id_const_iterator;
  typedef map<id_type, channel_t> map_id_channel_type;
  typedef typename map<id_type, channel_t>::iterator map_id_channel_iterator;
  typedef typename map<id_type, channel_t>::const_iterator
    map_id_channel_const_iterator;
  typedef deque<id_type> queue_id_type;

  enum { invalid_channel = hardflow_t::invalid_channel };

  udp_channels_t(
    const udp_limit_connections_mode_t a_mode = udplc_mode_queue,
    const size_type a_max_size = 1000,
    const size_type channel_buffer_max_count = 2,
    const size_type a_channel_buf_max_size = 1500,
    const bool reserve_buffers = false,
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
  size_type channel_buf_size_get(const address_type& a_address) const;
  size_type channel_buf_size_get(const id_type a_id) const;
  size_type channel_buf_max_size_get() const;
  //void channel_buf_max_size_set(size_type a_channel_buf_max_size);
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
  template <class DataReader>
  size_type write(const address_type& a_address, DataReader a_data);
  size_type available_for_write(const address_type& a_address) const;
  size_type read(size_type a_id, irs_u8 *ap_buf,
    size_type a_size);
  size_type channel_next();
  size_type channel_next_available_for_reading();
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
  buffers_t m_buffers;
  size_type m_buf_max_size;
  map_id_channel_iterator m_it_cur_channel;
  map_id_channel_iterator m_it_cur_channel_for_check;
  bool m_max_lifetime_enabled;
  bool m_max_downtime_enabled;
  counter_t m_max_lifetime;
  counter_t m_max_downtime;
};

template <class address_t>
udp_channels_t<address_t>::udp_channels_t(
  const udp_limit_connections_mode_t a_mode,
  const size_type a_max_size,
  const size_type channel_buffer_max_count,
  const size_type a_channel_buf_max_size,
  const bool reserve_buffers,
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
  m_buffers(channel_buffer_max_count, a_channel_buf_max_size),
  m_buf_max_size(a_channel_buf_max_size),
  m_it_cur_channel(m_map_id_channel.end()),
  m_it_cur_channel_for_check(m_map_id_channel.end()),
  m_max_lifetime_enabled(a_limit_lifetime_enabled),
  m_max_downtime_enabled(a_limit_downtime_enabled),
  m_max_lifetime(irs::make_cnt_s(a_max_lifetime_sec)),
  m_max_downtime(irs::make_cnt_s(a_max_downtime_sec))
{
  if (reserve_buffers) {
    m_buffers.reserve_buffers();
  }
}

template <class address_t>
void udp_channels_t<address_t>::insert(
  address_type a_address, id_type* ap_id, bool* ap_insert_success)
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
      #ifndef IRS_NOEXCEPTION
      try {
      #endif // IRS_NOEXCEPTION
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
      #ifndef IRS_NOEXCEPTION
      } catch (...) {
        if (m_map_id_channel.size() > channel_prev_count) {
          m_map_id_channel.erase(map_id_channel_res.first);
        }
        if (m_map_address_id.size() > channel_prev_count) {
          m_map_address_id.erase(map_address_id_res.first);
        }
        m_id_list.resize(channel_prev_count);
      }
      #endif // IRS_NOEXCEPTION
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
bool udp_channels_t<address_t>::id_get(
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
bool udp_channels_t<address_t>::address_get(id_type a_id,
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
void udp_channels_t<address_t>::erase(id_type a_id)
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

    m_buffers.free_buffer(it_map_address_id->second);
    m_map_id_channel.erase(it_erase_channel);
    m_map_address_id.erase(it_map_address_id);
    m_id_list.erase(it_id);
  } else {
    IRS_LIB_ASSERT_MSG("Канал отсутсвует");
  }
}

template <class address_t>
bool udp_channels_t<address_t>::is_channel_exists(const id_type a_id)
{
  return m_map_id_channel.find(a_id) != m_map_id_channel.end();
}

template <class address_t>
typename udp_channels_t<address_t>::size_type
udp_channels_t<address_t>::channel_buf_size_get(
  const address_type& a_address) const
{
  map_address_id_const_iterator it = m_map_address_id.find(a_address);
  if (it != m_map_address_id.end()) {
    m_buffers.available_for_read(it->second);
  }
  return 0;
}


template <class address_t>
typename udp_channels_t<address_t>::size_type
udp_channels_t<address_t>::channel_buf_size_get(const id_type a_id) const
{
  return m_buffers.available_for_read(a_id);
}

template <class address_t>
typename udp_channels_t<address_t>::size_type
udp_channels_t<address_t>::channel_buf_max_size_get() const
{
  return m_buf_max_size;
}

/*template <class address_t>
void udp_channels_t<address_t>::channel_buf_max_size_set(
  size_type a_channel_buf_max_size)
{
  m_buf_max_size = a_channel_buf_max_size;
  map_id_channel_iterator it_channel = m_map_id_channel.begin();
  while (it_channel != m_map_id_channel.end()) {
    it_channel->second.buffer.resize(m_buf_max_size);
  }
}*/

template <class address_t>
double udp_channels_t<address_t>::lifetime_get(
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
bool udp_channels_t<address_t>::limit_lifetime_enabled_get() const
{
  return m_max_lifetime_enabled;
}

template <class address_t>
double udp_channels_t<address_t>::max_lifetime_get() const
{
  return CNT_TO_DBLTIME(m_max_lifetime);
}

template <class address_t>
void udp_channels_t<address_t>::max_lifetime_set(
  double a_max_lifetime_sec)
{
  m_max_lifetime = irs::make_cnt_s(a_max_lifetime_sec);
}

template <class address_t>
void udp_channels_t<address_t>::limit_lifetime_enabled_set(
  bool a_limit_lifetime_enabled)
{
  m_max_lifetime_enabled = a_limit_lifetime_enabled;
}

template <class address_t>
void udp_channels_t<address_t>::downtime_timer_reset(
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
double udp_channels_t<address_t>::downtime_get(
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
bool udp_channels_t<address_t>::limit_downtime_enabled_get() const
{
  return m_max_downtime_enabled;
}

template <class address_t>
void udp_channels_t<address_t>::limit_downtime_enabled_set(
  bool a_limit_downtime_enabled)
{
  m_max_downtime_enabled = a_limit_downtime_enabled;
}

template <class address_t>
double udp_channels_t<address_t>::max_downtime_get() const
{
  return CNT_TO_DBLTIME(m_max_downtime);
}

template <class address_t>
void udp_channels_t<address_t>::max_downtime_set(
  double a_max_downtime_sec)
{
  m_max_downtime = irs::make_cnt_s(a_max_downtime_sec);
}

template <class address_t>
void udp_channels_t<address_t>::channel_address_get(
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
void udp_channels_t<address_t>::channel_address_set(
  const id_type a_channel_id, const address_type& a_address)
{
  map_id_channel_iterator it_channel = m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    map_address_id_iterator it_map_addr_id = m_map_address_id.find(
      it_channel->second.address);
    if (it_map_addr_id != m_map_address_id.end()) {
      it_channel->second.address = a_address;
      //it_channel->second.buffer.clear();
      const id_type id = it_map_addr_id->second;
      m_buffers.free_buffer(id);
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
void udp_channels_t<address_t>::clear()
{
  m_map_id_channel.clear();
  m_map_address_id.clear();
  m_id_list.clear();
  m_it_cur_channel = m_map_id_channel.end();
  m_it_cur_channel_for_check = m_map_id_channel.end();
}

template <class address_t>
typename udp_channels_t<address_t>::size_type
udp_channels_t<address_t>::size()
{
  return m_id_list.size();
}

template <class address_t>
void udp_channels_t<address_t>::mode_set(
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
typename udp_channels_t<address_t>::size_type
udp_channels_t<address_t>::max_size_get()
{
  return m_max_size;
}

template <class address_t>
void udp_channels_t<address_t>::max_size_set(
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
typename udp_channels_t<address_t>::size_type
udp_channels_t<address_t>::write(
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
      write_byte_count = m_buffers.write(
        it_map_id_channel->first, ap_buf, a_size);
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
template <class DataReader>
typename udp_channels_t<address_t>::size_type
udp_channels_t<address_t>::write(const address_type& a_address,
  DataReader a_data)
{
  size_type write_byte_count = 0;
  bool channel_exists = false;
  id_type id = 0;
  // Проверяем наличие канала и создаем, если его не существует
  insert(a_address, &id, &channel_exists);
  if (channel_exists) {
    map_id_channel_iterator it = m_map_id_channel.find(id);
    if (it != m_map_id_channel.end()) {
      write_byte_count = m_buffers.write(it->first, a_data);
      it->second.downtime.start();
    } else {
      IRS_LIB_ASSERT_MSG("Канал отсутсвует в списке");
    }
  } else {
    // Канала с таким адресом не существует и создать его не удалось
  }
  return write_byte_count;
}

template <class address_t>
udp_channels_t<address_t>::size_type
udp_channels_t<address_t>::available_for_write(
  const address_type& a_address) const
{
  // Ищем, есть ли уже такой адрес в списке
  map_address_id_const_iterator it = m_map_address_id.find(a_address);
  if (it != m_map_address_id.end()) {
    return m_buffers.available_for_write(it->second);
  }
  return m_buffers.available_for_write_to_new_channel();
}

template <class address_t>
typename udp_channels_t<address_t>::size_type
udp_channels_t<address_t>::read(size_type a_id,
  irs_u8 *ap_buf, size_type a_size)
{
  size_type read_byte_count = 0;
  map_id_channel_iterator it_map_id_channel =
    m_map_id_channel.find(a_id);
  if (it_map_id_channel != m_map_id_channel.end()) {

    read_byte_count = m_buffers.read(it_map_id_channel->first, ap_buf, a_size);

    if (read_byte_count > 0) {
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
typename udp_channels_t<address_t>::size_type
udp_channels_t<address_t>::channel_next()
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
typename udp_channels_t<address_t>::size_type
udp_channels_t<address_t>::channel_next_available_for_reading()
{
  return m_buffers.channel_next_available_for_reading();
}

template <class address_t>
typename udp_channels_t<address_t>::size_type
udp_channels_t<address_t>::cur_channel() const
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
void udp_channels_t<address_t>::tick()
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
bool udp_channels_t<address_t>::lifetime_exceeded(
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
bool udp_channels_t<address_t>::downtime_exceeded(
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
void udp_channels_t<address_t>::next_free_channel_id()
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
#endif //__ICCAVR__

template <class address_t>
class udp_channels_t;

class udp_t: public hardflow_t
{
public:
  enum mode_t {
    mode_stream,
    mode_datagram
  };
  struct configuration_t
  {
    mode_t mode;
    //! \brief Максимальный размер данных в посылаемом пакете. По умолчанию
    //!   MTU(1500 байт) - IP заголовок(20 байт) - UDP заголовок (8 байт) =
    //!   1472 байт
    size_type send_paket_data_max_size;
    //! \brief Максимальный размер данных в принимаемом пакете. По умолчанию
    //!   1500 байт
    size_type receive_paket_data_max_size;
    //! \brief Максимальный размер буфера канала
    size_type channel_buffer_max_size;
    //! \brief Максимальное количество буферов каналов, может быть меньше либо
    //!   равно количеству каналов, задаваемому через параметр конструктора
    //!   a_channel_max_count
    size_type channel_buffer_max_count;
    //! \brief Резервировать память для буферов
    bool reserve_buffers;
    udp_limit_connections_mode_t connections_mode;
    bool limit_lifetime_enabled;
    double max_lifetime_sec;
    bool limit_downtime_enabled;
    double max_downtime_sec;
    configuration_t():
      mode(mode_stream),
      send_paket_data_max_size(1472),
      receive_paket_data_max_size(1500),
      channel_buffer_max_size(1500),
      channel_buffer_max_count(2),
      reserve_buffers(true),
      connections_mode(udplc_mode_queue),
      limit_lifetime_enabled(false),
      max_lifetime_sec(24*60*60),
      limit_downtime_enabled(false),
      max_downtime_sec(60*60)
    {
    }
  };
  //! \param[in] a_local_ip - локальный адрес. Пример: "127.0.0.1".
  //!   Можно указать mxip_t::any_ip(), тогда он будет выбран автоматически.
  //! \param[in] a_local_port - локальный порт. Пример: "5005". Можно
  //!   указать 0 (рекомендуется для клиента),
  //!   тогда будет выбран случайный, свободный порт.
  //! \param[in] a_dest_ip - удаленный адрес. Пример: "127.0.0.1".
  //!   Можно указать mxip_t::any_ip(), если удаленный хост не задан
  //!   (например для сервера)
  //! \param[in] a_dest_port - удаленный порт. Пример: "5005". Можно
  //!   указать 0 (например для сервера).
  //! \param[in] a_channel_max_count - максимальное количество каналов
  //! \param[in] a_configuration - дополнительные параметры
  udp_t(const mxip_t& a_local_ip, irs_u16 a_local_port,
    const mxip_t& a_dest_ip, irs_u16 a_dest_port,
    const size_type a_channel_max_count = 3,
    configuration_t a_configuration = configuration_t());
  virtual ~udp_t();
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
  virtual void set_channel_switching_mode(channel_switching_mode_t a_mode);
private:
  struct address_t
  {
    mxip_t ip;
    irs_u16 port;
    address_t(const mxip_t& a_ip = mxip_t::any_ip(), irs_u16 a_port = 0):
      ip(a_ip),
      port(a_port)
    {
    }
    bool operator<(const address_t& a_address) const
    {
      if (ip.addr < a_address.ip.addr) {
        return true;
      } else if (ip.addr == a_address.ip.addr) {
        return port < a_address.port;
      } else {
        return false;
      }
    }
  };
  typedef address_t address_type;
  void create();
  #ifndef IRS_STM32H7xx
  static void recv(void *arg, udp_pcb *ap_upcb, pbuf *ap_buf, 
    ip_addr *ap_addr, u16_t a_port);
  #else 
  static void recv(void *arg, udp_pcb *ap_upcb, pbuf *ap_buf, 
    const ip_addr_t *ap_addr, u16_t a_port);
  #endif // IRS_STM32H7xx
    
  const mxip_t m_local_ip;
  const mxip_t m_dest_ip;
  const irs_u16 m_local_port;
  const irs_u16 m_dest_port;
  configuration_t m_configuration;
  const size_type m_channel_max_count;
  udp_pcb* mp_pcb;
  udp_channels_t<address_type> m_channel_list;
  channel_switching_mode_t m_mode;
};

#endif // LWIP_UDP

} // namespace lwip

} // namespace hardflow

} // namespace irs

#endif // USE_LWIP

#endif // LWIP_HARDFLOW_H
