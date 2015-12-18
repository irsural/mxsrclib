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

} // namespace lwip

namespace hardflow {

namespace lwip {

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

#if LWIP_UDP

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
  //! \param[in] a_local_ip - локальный адрес. ѕример: "127.0.0.1".
  //!   ћожно указать mxip_t::any_ip(), тогда он будет выбран автоматически.
  //! \param[in] a_local_port - локальный порт. ѕример: "5005". ћожно
  //!   указать 0 (рекомендуетс€ дл€ клиента),
  //!   тогда будет выбран случайный, свободный порт.
  //! \param[in] a_dest_ip - удаленный адрес. ѕример: "127.0.0.1".
  //!   ћожно указать mxip_t::any_ip(), если удаленный хост не задан
  //!   (например дл€ сервера)
  //! \param[in] a_dest_port - удаленный порт. ѕример: "5005". ћожно
  //!   указать 0 (например дл€ сервера).
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
  static void recv(void *arg, udp_pcb *ap_upcb,
    pbuf *ap_buf, ip_addr *ap_addr, u16_t a_port);
  const mxip_t m_local_ip;
  const irs_u16 m_local_port;
  const mxip_t m_dest_ip;
  const irs_u16 m_dest_port;
  configuration_t m_configuration;
  const size_type m_channel_max_count;
  udp_pcb* mp_pcb;
  udp_channel_list_t<address_type> m_channel_list;
  channel_switching_mode_t m_mode;
  loop_timer_t m_check_buffer_timer;
};

#endif // LWIP_UDP

} // namespace lwip

} // namespace hardflow

} // namespace irs

#endif // USE_LWIP

#endif // LWIP_HARDFLOW_H
