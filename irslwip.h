#ifndef LWIP_HARDFLOW_H
#define LWIP_HARDFLOW_H

#include <irsdefs.h>

#include <hardflowg.h>

#define USE_DHCP

#ifdef USE_LWIP

extern "C" {

#include <lwip/ip_addr.h>
#include <lwip/timers.h>
#include <lwip/tcp.h>
#include <lwip/udp.h>
#include <lwip/mem.h>
#include <lwip/memp.h>
#include <lwip/dhcp.h>
#ifdef IRS_STM32F_2_AND_4
#endif // IRS_STM32F_2_AND_4
#include <netif/etharp.h>
#ifdef IRS_STM32F_2_AND_4
#endif // IRS_STM32F_2_AND_4

} // extern "C"

#endif // USE_LWIP

#include <irsfinal.h>

#ifdef USE_LWIP

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
  static simple_ethernet_t* mp_simple_ethernet;
  netif m_netif;
  loop_timer_t m_sys_check_timeouts_loop_timer;
  loop_timer_t m_etharp_tmr_loop_timer;
  #ifdef USE_DHCP
  loop_timer_t m_dhcp_fine_tmr_loop_timer;
  loop_timer_t m_dhcp_coarse_tmr_loop_timer;
  #endif // USE_DHCP
};

namespace hardflow {

class tcp_server_t: public hardflow_t
{
public:
  typedef std::size_t size_type;
  tcp_server_t(const mxip_t& a_local_ip, irs_u16 a_local_port,
    size_type a_channel_max_count = 3);
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
  size_type m_current_processed_channel;
  size_type m_current_channel;
  channel_switching_mode_t m_mode;
  size_type m_last_id;
  static size_type m_read_channel;
};

class tcp_client_t: public hardflow_t
{
public:
  tcp_client_t(const mxip_t& a_local_ip, irs_u16 a_local_port,
    const mxip_t& a_dest_ip, irs_u16 a_dest_port);
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
  bool m_need_connect;
  bool m_connection_is_established;
  timer_t m_delay_before_connect;
};

struct lwip_address_t
{
  ip_addr ip;
  u16_t port;
  bool operator<(const lwip_address_t& a_address)
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

enum udp_limit_connections_mode_t {
  udplc_mode_invalid,      //!< \brief Ќедопустимый режим
  udplc_mode_queue,        //!< \brief ”читываетс€ переменна€ m_max_size
  udplc_mode_limited,      //!< \brief ”читываетс€ переменна€ m_max_size
  udplc_mode_unlimited     //!< \brief ѕеременна€ m_max_size не учитываетс€
};

class udp_channel_list_t
{
public:
  class less_t;
  class udp_flow_t;
  typedef hardflow_t::size_type size_type;
  typedef hardflow_t::string_type string_type;
  typedef size_type id_type;
  typedef irs::deque_data_t<irs_u8> buffer_type;
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
  typedef map<address_type, id_type>::iterator map_address_id_iterator;
  typedef map<id_type, channel_t> map_id_channel_type;
  typedef map<id_type, channel_t>::iterator map_id_channel_iterator;
  typedef map<id_type, channel_t>::const_iterator map_id_channel_const_iterator;
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
  // ¬озвращает false, если достигнут лимит, и true, если адрес успешно
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
    //! \brief ћаксимальный размер данных в посылаемом пакете. ѕо умолчанию
    //!   MTU(1500 байт) - IP заголовок(20 байт) - UDP заголовок (8 байт) =
    //!   1472 байт
    size_type send_paket_data_max_size;
    //! \brief ћаксимальный размер данных в принимаемом пакете. ѕо умолчанию
    //!   9000 байт
    size_type receive_paket_data_max_size;
    udp_limit_connections_mode_t connections_mode;
    bool limit_lifetime_enabled;
    double max_lifetime_sec;
    bool limit_downtime_enabled;
    double max_downtime_sec;
    configuration_t():
      mode(mode_stream),
      send_paket_data_max_size(1472),
      receive_paket_data_max_size(9000),
      connections_mode(udplc_mode_queue),
      limit_lifetime_enabled(false),
      max_lifetime_sec(24*60*60),
      limit_downtime_enabled(false),
      max_downtime_sec(60*60)
    {
    }
  };
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
  void create();
  static void recv(void *arg, udp_pcb *ap_upcb,
    pbuf *ap_buf, ip_addr *ap_addr, u16_t a_port);
  const mxip_t m_local_ip;
  const irs_u16 m_local_port;
  const mxip_t m_dest_ip;
  const irs_u16 m_dest_port;
  configuration_t m_configuration;
  const size_type m_channel_max_count;
  udp_pcb* mp_pcb;
  udp_channel_list_t m_channel_list;
  channel_switching_mode_t m_mode;
};

} // namespace hardflow

} // namespace lwip

} // namespace irs

#endif // USE_LWIP

#endif // LWIP_HARDFLOW_H
