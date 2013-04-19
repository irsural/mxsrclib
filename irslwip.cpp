#include <irspch.h>

#include <irslwip.h>

#if defined(IRS_STM32F2xx)
# include <stm32f2x7_eth.h>
#elif defined(IRS_STM32F4xx)
# include <stm32f4x7_eth.h>
#endif // defined(IRS_STM32F4xx)

#include <irsfinal.h>

#ifdef USE_LWIP

// class ethernet_t
irs::simple_ethernet_t* irs::lwip::ethernet_t::mp_simple_ethernet = NULL;

irs::lwip::ethernet_t::ethernet_t(simple_ethernet_t* ap_simple_ethernet,
  const configuration_t& a_configuration
):
  m_netif(),
  m_sys_check_timeouts_loop_timer(make_cnt_ms(10)),
  m_etharp_tmr_loop_timer(make_cnt_ms(ARP_TMR_INTERVAL))
  #if LWIP_DHCP
  ,
  m_dhcp_fine_tmr_loop_timer(make_cnt_ms(DHCP_FINE_TIMER_MSECS)),
  m_dhcp_coarse_tmr_loop_timer(make_cnt_ms(DHCP_COARSE_TIMER_MSECS))
  #endif // LWIP_DHCP
{
  mp_simple_ethernet = ap_simple_ethernet;
  mem_init();
  memp_init();

  ip_addr ipaddr;
  ip_addr netmask;
  ip_addr gateway;

  IP4_ADDR(&ipaddr, a_configuration.ip.val[0], a_configuration.ip.val[1],
    a_configuration.ip.val[2], a_configuration.ip.val[3]);
  IP4_ADDR(&netmask, a_configuration.netmask.val[0],
    a_configuration.netmask.val[1], a_configuration.netmask.val[2],
    a_configuration.netmask.val[3]);
  IP4_ADDR(&gateway, a_configuration.gateway.val[0],
    a_configuration.gateway.val[1], a_configuration.gateway.val[2],
    a_configuration.gateway.val[3]);

  netif_add(&m_netif, &ipaddr, &netmask, &gateway, NULL,
    &ethernet_t::ethernetif_init, &ethernet_input);
  netif_set_default(&m_netif);
  netif_set_up(&m_netif);

  if (a_configuration.dhcp_enabled) {
    start_dhcp();
  }
}

netif* irs::lwip::ethernet_t::get_netif()
{
  return &m_netif;
}

void irs::lwip::ethernet_t::tick()
{
  ethernetif_input();
  lwip_tick();
}

void irs::lwip::ethernet_t::start_dhcp()
{
  #if LWIP_DHCP
  const err_t err = dhcp_start(&m_netif);
  if (err == ERR_MEM) {
    IRS_LIB_LWIP_ETH_DBG_MSG_BASE("Не хватает памяти для запуска DHCP-клиента");
  } else if (err != ERR_OK) {
    IRS_LIB_LWIP_ETH_DBG_MSG_BASE("Не удалось запустить DHCP-клиента. "
      "Код ошибки " << static_cast<int>(err));
  }
  #endif // LWIP_DHCP
}

void irs::lwip::ethernet_t::lwip_tick()
{
  if (m_sys_check_timeouts_loop_timer.check()) {
    sys_check_timeouts();
  }
  if (m_etharp_tmr_loop_timer.check()) {
    etharp_tmr();
  }
  #if LWIP_DHCP
  if (m_dhcp_fine_tmr_loop_timer.check()) {
    dhcp_fine_tmr();
  }
  if (m_dhcp_coarse_tmr_loop_timer.check()) {
    dhcp_coarse_tmr();
  }
  #endif // LWIP_DHCP
}

err_t irs::lwip::ethernet_t::low_level_output(
  struct netif* /*ap_netif*/, struct pbuf *p)
{
  if (!mp_simple_ethernet) {
    IRS_LIB_ASSERT_MSG("Драйвер Ethernet не задан");
    return ERR_MEM;
  }
  if (!mp_simple_ethernet->is_send_buf_empty()) {
    IRS_LIB_LWIP_ETH_DBG_MSG_DETAIL("Нет места в очереди отправляемых пакетов");
    return ERR_MEM;
  }

  int framelength = 0;
  irs_u8* buf = mp_simple_ethernet->get_send_buf();
  for(pbuf* q = p; q != NULL; q = q->next) {
    if (q->len <= (mp_simple_ethernet->send_buf_max_size() - framelength)) {
      memcpy(buf, q->payload, q->len);
      buf += q->len;
      framelength += q->len;
    } else {
      IRS_LIB_LWIP_ETH_DBG_MSG_BASE("Попытка послать слишком большой пакет");
      return ERR_MEM;
    }
  }
  if (framelength > 0) {
    IRS_LIB_LWIP_ETH_DBG_MSG_DETAIL("low_level_output: length = " <<
      framelength);
    mp_simple_ethernet->send_packet(framelength);
  }
  return ERR_OK;
}

err_t irs::lwip::ethernet_t::ethernetif_input()
{
  err_t err = ERR_MEM;
  pbuf* p = low_level_input();
  if (p == NULL) {
    return ERR_MEM;
  }
  err = m_netif.input(p, &m_netif);
  if (err != ERR_OK) {
    IRS_LIB_LWIP_ETH_DBG_MSG_BASE("ethernetif_input: IP input error\n");
    pbuf_free(p);
    p = NULL;
  }
  return err;
}

struct pbuf* irs::lwip::ethernet_t::low_level_input()
{
  if (!mp_simple_ethernet->is_recv_buf_filled()) {
    return NULL;
  }

  irs_u16 len = static_cast<irs_u16>(mp_simple_ethernet->recv_buf_size());
  irs_u8* buffer = mp_simple_ethernet->get_recv_buf();
  pbuf* p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

  if (p != NULL) {
    for (pbuf* q = p; q != NULL; q = q->next) {
      memcpy(reinterpret_cast<irs_u8*>(q->payload), buffer, q->len);
      buffer += q->len;
    }
    mp_simple_ethernet->set_recv_handled();
  }
  return p;
}

err_t irs::lwip::ethernet_t::ethernetif_init(struct netif* ap_netif)
{
  IRS_LIB_ASSERT(ap_netif);

  #if LWIP_NETIF_HOSTNAME
  ap_netif->hostname = "lwip";
  #endif // LWIP_NETIF_HOSTNAME

  ap_netif->name[0] = 's';
  ap_netif->name[1] = 't';
  ap_netif->output = etharp_output;
  ap_netif->linkoutput = low_level_output;

  low_level_init(ap_netif);

  return ERR_OK;
}


void irs::lwip::ethernet_t::low_level_init(struct netif *ap_netif)
{
  IRS_LIB_ASSERT(mp_simple_ethernet);

  IRS_LIB_ASSERT(mac_length <= NETIF_MAX_HWADDR_LEN);
  ap_netif->hwaddr_len = mac_length;
  const mxmac_t mac = mp_simple_ethernet->get_local_mac();
  ap_netif->hwaddr[0] =  mac.val[0];
  ap_netif->hwaddr[1] =  mac.val[1];
  ap_netif->hwaddr[2] =  mac.val[2];
  ap_netif->hwaddr[3] =  mac.val[3];
  ap_netif->hwaddr[4] =  mac.val[4];
  ap_netif->hwaddr[5] =  mac.val[5];

  ap_netif->mtu = mp_simple_ethernet->send_buf_max_size() -
    irs::simple_ethernet_t::ethernet_header_size;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  ap_netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
    NETIF_FLAG_LINK_UP;
}

// class tcp_server_t
irs::lwip::hardflow::tcp_server_t::size_type
irs::lwip::hardflow::tcp_server_t::m_read_channel =
  irs::lwip::hardflow::tcp_server_t::invalid_channel;

class read_buffer_t
{
public:
  typedef std::size_t size_type;
  bool empty()
  {
    return m_buf.empty();
  }
  bool owned(void* ap_candidate, size_type a_channel)
  {
    return (mp_owner == ap_candidate) && (m_channel == a_channel);
  }
  size_type channel(void* ap_owner)
  {
    if (mp_owner == ap_owner) {
      return m_channel;
    }
    return irs::hardflow_t::invalid_channel;
  }
  bool write(void* ap_owner, size_type a_channel,
    const irs_u8* ap_buf, size_type a_size)
  {
    bool status = false;
    if (a_size == 0) {
      return status;
    }
    size_type pos = 0;
    if (m_buf.empty()) {
      mp_owner = ap_owner;
      m_channel = a_channel;
    } else {
      IRS_LIB_ASSERT(mp_owner == ap_owner);
      IRS_LIB_ASSERT(m_channel == a_channel);
      pos = m_buf.size();
    }
    try {
      m_buf.resize(m_buf.size() + a_size);
      irs::memcpyex(irs::vector_data(m_buf, pos), ap_buf, a_size);
      status = true;
    } catch (std::bad_alloc&) {
      status = false;
      if (m_buf.empty()) {
        mp_owner = NULL;
        m_channel = irs::hardflow_t::invalid_channel;
      }
    }
    return status;
  }
  size_type read(const void* ap_owner, size_type a_channel,
    irs_u8* ap_buf, size_type a_size)
  {
    size_type size = 0;
    if ((mp_owner == ap_owner) &&
      (m_channel == a_channel)) {
      size = std::min(m_buf.size(), a_size);
      irs::memcpyex(ap_buf, irs::vector_data(m_buf), size);
      m_buf.erase(m_buf.begin(), m_buf.begin() + size);
      if (m_buf.empty()) {
        mp_owner = NULL;
        m_channel = irs::hardflow_t::invalid_channel;
      }
    }
    return size;
  }
  size_type size()
  {
    return m_buf.size();
  }
private:
  void* mp_owner;
  std::vector<irs_u8> m_buf;
  irs::hardflow_t::size_type m_channel;
};

read_buffer_t* tcp_read_buffer()
{
  static read_buffer_t read_buffer;
  return &read_buffer;
}

read_buffer_t* udp_read_buffer()
{
  static read_buffer_t read_buffer;
  return &read_buffer;
}

irs::lwip::hardflow::tcp_server_t::tcp_server_t(
  const mxip_t& a_local_ip,
  irs_u16 a_local_port,
  size_type a_channel_max_count
):
  mp_pcb(),
  m_channels(),
  m_channels_map(),
  m_channel_max_count(a_channel_max_count),
  m_current_processed_channel(0),
  m_current_channel(invalid_channel),
  m_mode(csm_any),
  m_last_id(invalid_channel + 1)
{
  listen(a_local_ip, a_local_port);
}

irs::lwip::hardflow::tcp_server_t::~tcp_server_t()
{
  std::deque<irs::handle_t<channel_t> >::iterator it =
    m_channels.begin();
  while (it != m_channels.end()) {
    tcp_pcb* pcb = (*it)->pcb;
    tcp_arg(pcb, NULL);
    tcp_recv(pcb, NULL);
    tcp_err(pcb, NULL);
    tcp_poll(pcb, NULL, 0);
    tcp_sent(pcb, NULL);
    tcp_abort(pcb);
    ++it;
  }
  if (mp_pcb) {
    tcp_close(mp_pcb);
  }
}

irs::lwip::hardflow::tcp_server_t::string_type
irs::lwip::hardflow::tcp_server_t::param(const string_type& /*a_param_name*/)
{
  return string_type();
}

void irs::lwip::hardflow::tcp_server_t::set_param(
  const string_type& /*a_param_name*/, const string_type& /*a_value*/)
{
}

irs::lwip::hardflow::tcp_server_t::size_type
irs::lwip::hardflow::tcp_server_t::read(size_type a_channel_ident,
  irs_u8* ap_buf, size_type a_size)
{
  return tcp_read_buffer()->read(this, a_channel_ident, ap_buf, a_size);
}

irs::lwip::hardflow::tcp_server_t::size_type
irs::lwip::hardflow::tcp_server_t::write(
  size_type a_channel_ident, const irs_u8* ap_buf,
  size_type a_size)
{
  size_type write_data_size = 0;
  if (is_channel_exists(a_channel_ident)) {
    channel_t* channel = m_channels_map.find(a_channel_ident)->second;
    size_type available = static_cast<size_type>(tcp_sndbuf(channel->pcb));
    write_data_size = std::min(a_size, available);
    if (write_data_size > 0) {
      err_t err =
        tcp_write(channel->pcb, ap_buf, write_data_size, TCP_WRITE_FLAG_COPY);
      if (err == ERR_OK) {
        IRS_LIB_HARDFLOWG_DBG_MSG_BASE("write: записано " << write_data_size);
      } else {
        write_data_size = 0;
        IRS_LIB_HARDFLOWG_DBG_MSG_BASE("write: запись не удалась");
      }
    }
  }
  return write_data_size;
}

irs::lwip::hardflow::tcp_server_t::size_type
irs::lwip::hardflow::tcp_server_t::channel_next()
{
  if (m_mode == csm_any) {
    if (m_channels.empty()) {
      return invalid_channel;
    } else {
      m_current_channel++;
      if (m_current_channel >= m_channels.size()) {
        m_current_channel = 0;
      }
      m_channels[m_current_channel]->id;
    }
  } else if (m_mode == csm_ready_for_reading) {
    return m_read_channel;
  }
  return invalid_channel;
}

bool irs::lwip::hardflow::tcp_server_t::is_channel_exists(
  size_type a_channel_ident)
{
  return m_channels_map.find(a_channel_ident) != m_channels_map.end();
}

void irs::lwip::hardflow::tcp_server_t::set_channel_switching_mode(
  channel_switching_mode_t a_mode)
{
  m_mode = a_mode;
}

void irs::lwip::hardflow::tcp_server_t::tick()
{
}

bool irs::lwip::hardflow::tcp_server_t::listen(
  const mxip_t& a_local_ip, irs_u16 a_port)
{
  mp_pcb = tcp_new();
  if (!mp_pcb) {
    return false;
  }
  ip_addr ip;
  ip.addr = a_local_ip.addr;
  err_t err = tcp_bind(mp_pcb, &ip, a_port);
  if (err == ERR_USE) {
    IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Указанный порт уже используется");
    if (ERR_OK == tcp_close(mp_pcb)) {
      mp_pcb = NULL;
    }
    return false;
  }
  tcp_pcb* pcb = tcp_listen(mp_pcb);
  if (!pcb) {
    if (ERR_OK == tcp_close(mp_pcb)) {
      mp_pcb = NULL;
    }
    return false;
  }
  mp_pcb = pcb;
  tcp_arg(mp_pcb, this);
  tcp_accept(mp_pcb, tcp_server_t::accept);
  return true;
}

err_t irs::lwip::hardflow::tcp_server_t::accept(
  void *arg, tcp_pcb *pcb, err_t /*err*/)
{
  tcp_server_t* server = reinterpret_cast<tcp_server_t*>(arg);
  IRS_LIB_ASSERT(server);
  tcp_accepted(server->mp_pcb);
  if (server->m_channel_max_count > 0) {
    std::deque<irs::handle_t<channel_t> >* channels = &server->m_channels;
    if (channels->size() >= server->m_channel_max_count) {
      channel_t* channel = channels->front().get();
      if (!server->close_connection(channel)) {
        return ERR_MEM;
      }
    }
    channel_t* channel = server->create_channel(pcb);
    if (channel) {
      return ERR_OK;
    }
  }
  return ERR_MEM;
}

bool irs::lwip::hardflow::tcp_server_t::close_connection(channel_t* ap_channel)
{
  tcp_arg(ap_channel->pcb, NULL);
  tcp_recv(ap_channel->pcb, NULL);
  tcp_err(ap_channel->pcb, NULL);
  tcp_poll(ap_channel->pcb, NULL, 0);
  tcp_sent(ap_channel->pcb, NULL);

  const err_t err = tcp_close(ap_channel->pcb);
  if (err == ERR_OK) {
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Удален канал " << ap_channel->id);
    erase_channel(ap_channel);
    return true;
  } else {
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Закрыть соединение не удалось");
    ap_channel->marked_for_erase = true;
    tcp_arg(ap_channel->pcb, ap_channel);
    tcp_recv(ap_channel->pcb, tcp_server_t::recv);
    tcp_err(ap_channel->pcb, tcp_server_t::conn_err);
    tcp_poll(ap_channel->pcb, tcp_server_t::poll, pool_interval);
    tcp_sent(ap_channel->pcb, tcp_server_t::sent);
    return false;
  }
}
irs::lwip::hardflow::tcp_server_t::channel_t*
irs::lwip::hardflow::tcp_server_t::create_channel(
  tcp_pcb *ap_pcb)
{
  irs::handle_t<channel_t> channel;
  const size_type prev_channel_count = m_channels.size();
  try {
    channel.reset(new channel_t());
    if (m_last_id == invalid_channel) {
      m_last_id++;
    }
    channel->server = this;
    channel->id = m_last_id;
    channel->pcb = ap_pcb;
    m_channels.push_back(channel);
    m_channels_map.insert(make_pair(channel->id, channel.get()));
    tcp_arg(ap_pcb, channel.get());
    tcp_recv(ap_pcb, tcp_server_t::recv);
    tcp_err(ap_pcb, tcp_server_t::conn_err);
    tcp_poll(ap_pcb, tcp_server_t::poll, pool_interval);
    tcp_sent(ap_pcb, tcp_server_t::sent);
    m_last_id++;
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Добавлен канал " << channel->id);
  } catch (...) {
    channel.reset();
    m_channels.resize(prev_channel_count);
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Создать канал не удалось");
  }
  return channel.get();
}

void irs::lwip::hardflow::tcp_server_t::erase_channel(channel_t* ap_channel)
{
  IRS_LIB_ASSERT(m_channels_map.find(ap_channel->id) != m_channels_map.end());
  std::deque<irs::handle_t<channel_t> >::iterator it =
    m_channels.begin();
  while (it != m_channels.end()) {
    if (it->get() == ap_channel) {
      m_channels_map.erase(ap_channel->id);
      m_channels.erase(it);
      break;
    }
    ++it;
  }
}

err_t irs::lwip::hardflow::tcp_server_t::recv(void *arg, tcp_pcb *pcb, pbuf *p,
  err_t err)
{
  if ((err == ERR_OK) && (p != NULL)) {
    if (tcp_read_buffer()->empty()) {
      channel_t* channel = reinterpret_cast<channel_t*>(arg);
      irs_u8* data = reinterpret_cast<irs_u8*>(p->payload);
      size_type len = p->tot_len;
      if (len > 0) {
        if (tcp_read_buffer()->write(channel->server, channel->id, data, len)) {
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Получено: " << len << " байт");
          tcp_recved(pcb, len);
          pbuf_free(p);
        } else {
          IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Данные не приняты, так как не удалось "
            "выделить память");
          return ERR_MEM;
        }
      }
    } else {
      return ERR_MEM;
    }
  } else if ((err == ERR_OK) && (p == NULL)) {
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Удаленный хост разорвал соединение");
    channel_t* channel = reinterpret_cast<channel_t*>(arg);
    channel->server->close_connection(channel);
  }
  return ERR_OK;
}

void irs::lwip::hardflow::tcp_server_t::conn_err(void *arg, err_t /*a_err*/)
{
  channel_t* channel = reinterpret_cast<channel_t*>(arg);
  tcp_server_t* server = channel->server;
  std::deque<irs::handle_t<channel_t> >::iterator it =
    server->m_channels.begin();
  while (it != server->m_channels.end()) {
    if (it->get() == channel) {
      server->m_channels_map.erase(channel->id);
      server->m_channels.erase(it);
      break;
    }
    ++it;
  }
  IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("conn_err");
}

err_t irs::lwip::hardflow::tcp_server_t::poll(void* /*a_arg*/,
  tcp_pcb* /*ap_pcb*/)
{
  return ERR_OK;
}

err_t irs::lwip::hardflow::tcp_server_t::sent(
  void* /*ap_arg*/, tcp_pcb* /*ap_pcb*/, u16_t /*a_len*/)
{
  return ERR_OK;
}

// class tcp_client_t
irs::lwip::hardflow::tcp_client_t::tcp_client_t(
  const mxip_t& a_local_ip,
  irs_u16 a_local_port,
  const mxip_t &a_dest_ip,
  irs_u16 a_dest_port
):
  m_local_ip(a_local_ip),
  m_local_port(a_local_port),
  m_dest_ip(a_dest_ip),
  m_dest_port(a_dest_port),
  mp_pcb(),
  m_marked_for_erase(false),
  m_channel_id(invalid_channel + 1),
  m_need_connect(false),
  m_connection_is_established(false),
  m_delay_before_connect(make_cnt_s(1))
{
  connect();
}

irs::lwip::hardflow::tcp_client_t::~tcp_client_t()
{
  if (mp_pcb) {
    tcp_arg(mp_pcb, this);
    tcp_recv(mp_pcb, NULL);
    tcp_err(mp_pcb, NULL);
    tcp_poll(mp_pcb, NULL, 0);
    tcp_sent(mp_pcb, NULL);
    tcp_abort(mp_pcb);
  }
}

bool irs::lwip::hardflow::tcp_client_t::connect()
{
  mp_pcb = tcp_new();
  if (!mp_pcb) {
    return false;
  }
  tcp_arg(mp_pcb, this);
  tcp_recv(mp_pcb, tcp_client_t::recv);
  tcp_err(mp_pcb, tcp_client_t::conn_err);
  tcp_poll(mp_pcb, tcp_client_t::poll, pool_interval);
  tcp_sent(mp_pcb, tcp_client_t::sent);
  ip_addr local_ip;
  local_ip.addr = m_local_ip.addr;
  err_t err = tcp_bind(mp_pcb, &local_ip, m_local_port);
  if (err == ERR_USE) {
    IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Указанный порт уже используется");
    if (ERR_OK == tcp_close(mp_pcb)) {
      mp_pcb = NULL;
    }
    return false;
  }
  ip_addr remote_ip;
  remote_ip.addr = m_dest_ip.addr;
  err_t connect_err = tcp_connect(mp_pcb, &remote_ip,
    m_dest_port, tcp_client_t::connected);
  if (connect_err == ERR_VAL) {
    IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Указаны недопустимые аргументы");
    if (ERR_OK == tcp_close(mp_pcb)) {
      mp_pcb = NULL;
    }
    return false;
  }
  //m_connect_timeout.start();
  m_need_connect = false;
  return true;
}

bool irs::lwip::hardflow::tcp_client_t::close_connection()
{
  m_connection_is_established = false;
  tcp_arg(mp_pcb, NULL);
  tcp_recv(mp_pcb, NULL);
  tcp_err(mp_pcb, NULL);
  tcp_poll(mp_pcb, NULL, 0);
  tcp_sent(mp_pcb, NULL);

  const err_t err = tcp_close(mp_pcb);
  if (err == ERR_OK) {
    mp_pcb = NULL;
    m_marked_for_erase = false;
    m_need_connect = true;
    m_delay_before_connect.start();
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Соединение успешно закрыто");
    return true;
  } else {
    IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Закрыть соединение не удалось");
    m_marked_for_erase = true;
    tcp_arg(mp_pcb, this);
    tcp_recv(mp_pcb, tcp_client_t::recv);
    tcp_err(mp_pcb, tcp_client_t::conn_err);
    tcp_poll(mp_pcb, tcp_client_t::poll, pool_interval);
    tcp_sent(mp_pcb, tcp_client_t::sent);
    return false;
  }
}

err_t irs::lwip::hardflow::tcp_client_t::connected(
  void* arg, tcp_pcb* /*ap_tpcb*/, err_t /*a_err*/)
{
  IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Соединение установлено");
  tcp_client_t* client = reinterpret_cast<tcp_client_t*>(arg);
  client->m_connection_is_established = true;
  return ERR_OK;
}

err_t irs::lwip::hardflow::tcp_client_t::recv(void *arg, tcp_pcb *pcb,
  pbuf *p, err_t err)
{
  if ((err == ERR_OK) && (p != NULL)) {
    if (tcp_read_buffer()->empty()) {
      tcp_client_t* client = reinterpret_cast<tcp_client_t*>(arg);
      irs_u8* data = reinterpret_cast<irs_u8*>(p->payload);
      size_type len = p->tot_len;
      if (len > 0) {
        if (tcp_read_buffer()->write(client, client->m_channel_id, data, len)) {
          tcp_recved(pcb, len);
          pbuf_free(p);
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Получено: " << len << " байт");
        } else {
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Данные не приняты, так как не "
            "удалось выделить память");
          return ERR_MEM;
        }
      }
    } else {
      return ERR_MEM;
    }
  } else if ((err == ERR_OK) && (p == NULL)) {
    tcp_client_t* client = reinterpret_cast<tcp_client_t*>(arg);
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Удаленный хост разорвал соединение");
    client->close_connection();
  }
  return ERR_OK;
}

void irs::lwip::hardflow::tcp_client_t::conn_err(void *arg, err_t /*a_err*/)
{
  tcp_client_t* client = reinterpret_cast<tcp_client_t*>(arg);
  IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("conn_err");
  client->mp_pcb = NULL;
  client->m_marked_for_erase = false;
  client->m_need_connect = true;
  client->m_delay_before_connect.start();
}

err_t irs::lwip::hardflow::tcp_client_t::poll(void* /*ap_arg*/,
  tcp_pcb* /*ap_pcb*/)
{
  return ERR_OK;
}

err_t irs::lwip::hardflow::tcp_client_t::sent(
  void* /*ap_arg*/, tcp_pcb* /*ap_pcb*/, u16_t /*a_len*/)
{
  return ERR_OK;
}

irs::lwip::hardflow::tcp_client_t::size_type
irs::lwip::hardflow::tcp_client_t::read(
  size_type a_channel_ident, irs_u8 *ap_buf, size_type a_size)
{
  return tcp_read_buffer()->read(this, a_channel_ident, ap_buf, a_size);
}

irs::lwip::hardflow::tcp_client_t::size_type
irs::lwip::hardflow::tcp_client_t::write(
  size_type a_channel_ident, const irs_u8 *ap_buf, size_type a_size)
{
  size_type write_data_size = 0;
  if (m_channel_id == a_channel_ident) {
    size_type available = static_cast<size_type>(tcp_sndbuf(mp_pcb));
    write_data_size = std::min(a_size, available);
    if (write_data_size > 0) {
      err_t err =
        tcp_write(mp_pcb, ap_buf, write_data_size, TCP_WRITE_FLAG_COPY);
      if (err == ERR_OK) {
        IRS_LIB_HARDFLOWG_DBG_MSG_BASE("write: Записано " << write_data_size);
      } else {
        write_data_size = 0;
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("write: Запись не удалась");
      }
    }
  }
  return write_data_size;
}

void irs::lwip::hardflow::tcp_client_t::tick()
{
  if (m_marked_for_erase) {
    close_connection();
  }
  if (m_need_connect && m_delay_before_connect.check()) {
    connect();
  }
}

irs::lwip::hardflow::tcp_client_t::string_type
irs::lwip::hardflow::tcp_client_t::param(const string_type& /*a_param_name*/)
{
  return string_type();
}

void irs::lwip::hardflow::tcp_client_t::set_param(
  const string_type& /*a_param_name*/,
  const string_type& /*a_param_value*/)
{
}

irs::lwip::hardflow::tcp_client_t::size_type
irs::lwip::hardflow::tcp_client_t::channel_next()
{
  return m_channel_id;
}

bool irs::lwip::hardflow::tcp_client_t::is_channel_exists(
  size_type a_channel_ident)
{
  return m_channel_id == a_channel_ident;
}

//class udp_channel_list_t
irs::lwip::hardflow::udp_channel_list_t::udp_channel_list_t(
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

void irs::lwip::hardflow::udp_channel_list_t::insert(address_type a_address,
  id_type* ap_id, bool* ap_insert_success)
{
  *ap_insert_success = false;
  // Ищем, есть ли уже такой адрес в списке
  map_address_id_type::iterator it_map_addr_id = m_map_address_id.find(a_address);
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

bool irs::lwip::hardflow::udp_channel_list_t::id_get(address_type a_address,
  id_type* ap_id)
{
  bool find_success = false;
  map_address_id_type::iterator it_map_addr_id = m_map_address_id.end();
  it_map_addr_id = m_map_address_id.find(a_address);
  if (it_map_addr_id != m_map_address_id.end()) {
    *ap_id = it_map_addr_id->second;
    find_success = true;
  } else {
    find_success = false;
  }
  return find_success;
}

bool irs::lwip::hardflow::udp_channel_list_t::address_get(id_type a_id,
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

void irs::lwip::hardflow::udp_channel_list_t::erase(id_type a_id)
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
    map_address_id_type::iterator it_map_address_id =
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

bool irs::lwip::hardflow::udp_channel_list_t::is_channel_exists(const id_type a_id)
{
  return m_map_id_channel.find(a_id) != m_map_id_channel.end();
}

irs::lwip::hardflow::udp_channel_list_t::size_type
irs::lwip::hardflow::udp_channel_list_t::channel_buf_max_size_get() const
{
  return m_buf_max_size;
}

void irs::lwip::hardflow::udp_channel_list_t::channel_buf_max_size_set(
  size_type a_channel_buf_max_size)
{
  m_buf_max_size = a_channel_buf_max_size;
  map_id_channel_iterator it_channel = m_map_id_channel.begin();
  while (it_channel != m_map_id_channel.end()) {
    it_channel->second.buffer.resize(m_buf_max_size);
  }
}

double irs::lwip::hardflow::udp_channel_list_t::lifetime_get(
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

bool irs::lwip::hardflow::udp_channel_list_t::limit_lifetime_enabled_get() const
{
  return m_max_lifetime_enabled;
}

double irs::lwip::hardflow::udp_channel_list_t::max_lifetime_get() const
{
  return CNT_TO_DBLTIME(m_max_lifetime);
}

void irs::lwip::hardflow::udp_channel_list_t::max_lifetime_set(
  double a_max_lifetime_sec)
{
  m_max_lifetime = irs::make_cnt_s(a_max_lifetime_sec);
}

void irs::lwip::hardflow::udp_channel_list_t::limit_lifetime_enabled_set(
  bool a_limit_lifetime_enabled)
{
  m_max_lifetime_enabled = a_limit_lifetime_enabled;
}

void irs::lwip::hardflow::udp_channel_list_t::downtime_timer_reset(
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

double irs::lwip::hardflow::udp_channel_list_t::downtime_get(
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

bool irs::lwip::hardflow::udp_channel_list_t::limit_downtime_enabled_get() const
{
  return m_max_downtime_enabled;
}

void irs::lwip::hardflow::udp_channel_list_t::limit_downtime_enabled_set(
  bool a_limit_downtime_enabled)
{
  m_max_downtime_enabled = a_limit_downtime_enabled;
}

double irs::lwip::hardflow::udp_channel_list_t::max_downtime_get() const
{
  return CNT_TO_DBLTIME(m_max_downtime);
}

void irs::lwip::hardflow::udp_channel_list_t::max_downtime_set(
  double a_max_downtime_sec)
{
  m_max_downtime = irs::make_cnt_s(a_max_downtime_sec);
}

void irs::lwip::hardflow::udp_channel_list_t::channel_address_get(
  const id_type a_channel_id, address_type* ap_address)
{
  map_id_channel_iterator it_channel = m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    *ap_address = it_channel->second.address;
  } else {
    // Канал не найден
  }
}

void irs::lwip::hardflow::udp_channel_list_t::channel_address_set(
  const id_type a_channel_id, const address_type& a_address)
{
  map_id_channel_iterator it_channel = m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    map_address_id_type::iterator it_map_addr_id = m_map_address_id.find(
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

void irs::lwip::hardflow::udp_channel_list_t::clear()
{
  m_map_id_channel.clear();
  m_map_address_id.clear();
  m_id_list.clear();
  m_it_cur_channel = m_map_id_channel.end();
  m_it_cur_channel_for_check = m_map_id_channel.end();
}

irs::lwip::hardflow::udp_channel_list_t::size_type
irs::lwip::hardflow::udp_channel_list_t::size()
{
  return m_id_list.size();
}

void irs::lwip::hardflow::udp_channel_list_t::mode_set(
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

irs::lwip::hardflow::udp_channel_list_t::size_type
irs::lwip::hardflow::udp_channel_list_t::max_size_get()
{
  return m_max_size;
}

void irs::lwip::hardflow::udp_channel_list_t::max_size_set(size_type a_max_size)
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

irs::lwip::hardflow::udp_channel_list_t::size_type
irs::lwip::hardflow::udp_channel_list_t::write(const address_type& a_address,
  const irs_u8 *ap_buf, size_type a_size)
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

irs::lwip::hardflow::udp_channel_list_t::size_type
irs::lwip::hardflow::udp_channel_list_t::read(size_type a_id,
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

irs::lwip::hardflow::udp_channel_list_t::size_type
irs::lwip::hardflow::udp_channel_list_t::channel_next()
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

irs::lwip::hardflow::udp_channel_list_t::size_type
irs::lwip::hardflow::udp_channel_list_t::cur_channel() const
{
  id_type channel_id = invalid_channel;
  if (m_it_cur_channel != m_map_id_channel.end()) {
    channel_id = m_it_cur_channel->first;
  } else {
    // Текущий канал не установлен
  }
  return channel_id;
}

void irs::lwip::hardflow::udp_channel_list_t::tick()
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

bool irs::lwip::hardflow::udp_channel_list_t::lifetime_exceeded(
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

bool irs::lwip::hardflow::udp_channel_list_t::downtime_exceeded(
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

void irs::lwip::hardflow::udp_channel_list_t::next_free_channel_id()
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

// class udp_t
irs::lwip::hardflow::udp_t::udp_t(
  const mxip_t& a_local_ip,
  irs_u16 a_local_port,
  const mxip_t& a_dest_ip,
  irs_u16 a_dest_port,
  const size_type a_channel_max_count,
  configuration_t a_configuration
):
  m_local_ip(a_local_ip),
  m_local_port(a_local_port),
  m_dest_ip(a_dest_ip),
  m_dest_port(a_dest_port),
  m_configuration(a_configuration),
  m_channel_max_count(a_channel_max_count),
  mp_pcb(NULL),
  m_channel_list(m_configuration.connections_mode,
    m_channel_max_count,
    0,
    m_configuration.limit_lifetime_enabled,
    m_configuration.max_lifetime_sec,
    m_configuration.limit_downtime_enabled,
    m_configuration.max_downtime_sec),
  m_mode(csm_any)
{
  create();
}

void irs::lwip::hardflow::udp_t::create()
{
  mp_pcb = udp_new();
  if (!mp_pcb) {
    return;
  }

  ip_addr local_ip;
  local_ip.addr = m_local_ip.addr;
  err_t err = udp_bind(mp_pcb, &local_ip, m_local_port);
  if (err == ERR_USE) {
    IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Указанный порт уже используется");
    udp_remove(mp_pcb);
    mp_pcb = NULL;
    return;
  }
  udp_recv(mp_pcb, udp_t::recv, this);
  bool insert_success = false;
  size_type remote_host_id = 0;
  udp_channel_list_t::address_type remote_host_address(m_dest_ip, m_dest_port);
  m_channel_list.insert(remote_host_address, &remote_host_id, &insert_success);
}

void irs::lwip::hardflow::udp_t::recv(void *arg, udp_pcb* /*ap_upcb*/,
  pbuf* ap_buf, ip_addr* ap_addr, u16_t a_port)
{
  if (udp_read_buffer()->empty()) {
    udp_t* udp = reinterpret_cast<udp_t*>(arg);
    irs_u8* data = reinterpret_cast<irs_u8*>(ap_buf->payload);
    size_type len = ap_buf->tot_len;
    if ((len > 0) &&
      (len <= udp->m_configuration.receive_paket_data_max_size)) {
      udp_channel_list_t::address_type sender_address;
      sender_address.ip.addr = ap_addr->addr;
      sender_address.port = a_port;
      size_type id = 0;
      bool insert_success = false;
      udp->m_channel_list.insert(sender_address, &id, &insert_success);
      if (insert_success) {
        if (!udp_read_buffer()->write(udp, id, data, len)) {
          IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Данные отброшены, так как не "
          "удалось выделить память");
        }
      }
    } else if (len > udp->m_configuration.receive_paket_data_max_size) {
      IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Данные отброшены, так как их размер "
        "выше допустимого");
    }
  } else {
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Данные отброшены, так как буфер занят");
  }
  pbuf_free(ap_buf);
}

irs::lwip::hardflow::udp_t::~udp_t()
{
  if (mp_pcb) {
    udp_remove(mp_pcb);
  }
}

irs::lwip::hardflow::udp_t::size_type
irs::lwip::hardflow::udp_t::read(size_type a_channel_ident, irs_u8 *ap_buf,
  size_type a_size)
{
  if (m_configuration.mode == mode_datagram) {
    if (udp_read_buffer()->size() <= a_size) {
      return udp_read_buffer()->read(this, a_channel_ident, ap_buf, a_size);
    } else {
      return 0;
    }
  }
  return udp_read_buffer()->read(this, a_channel_ident, ap_buf, a_size);
}

irs::lwip::hardflow::udp_t::size_type
irs::lwip::hardflow::udp_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  if (!mp_pcb) {
    return 0;
  }
  const size_type size = std::min(a_size,
    m_configuration.send_paket_data_max_size);
  pbuf* pkt_buf = pbuf_alloc(PBUF_TRANSPORT, size, PBUF_POOL);
  if (!pkt_buf) {
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Не хватает памяти для отправки пакета");
    return 0;
  }
  udp_channel_list_t::address_type remote_host_address;
  ip_addr dst_ip;
  if (m_channel_list.address_get(a_channel_ident, &remote_host_address)) {
    dst_ip.addr = remote_host_address.ip.addr;
  } else {
    IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Канала с указанным идентификатором не "
      "существует");
    return 0;
  }
  const irs_u8* buf_src = ap_buf;
  for (pbuf* buf_dest = pkt_buf; buf_dest != NULL; buf_dest = buf_dest->next) {
    memcpy(buf_dest->payload, buf_src, buf_dest->len);
    buf_src = buf_src + buf_dest->len;
  }
  err_t err = udp_sendto(mp_pcb, pkt_buf, &dst_ip, remote_host_address.port);
  pbuf_free(pkt_buf);
  m_channel_list.downtime_timer_reset(a_channel_ident);
  if (err != ERR_OK) {
    if (err == ERR_MEM) {
      IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Не хватает памяти для отправки пакета");
      return 0;
    } else if (err == ERR_RTE) {
      IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Не удается найти маршрут к адресу");
    }
    return 0;
  }
  return size;
}

void irs::lwip::hardflow::udp_t::tick()
{
  m_channel_list.tick();
}

irs::lwip::hardflow::udp_t::string_type
irs::lwip::hardflow::udp_t::param(const string_type& /*a_param_name*/)
{
  return string_type();
}

void irs::lwip::hardflow::udp_t::set_param(const string_type& /*a_param_name*/,
  const string_type& /*a_param_value*/)
{
}

irs::lwip::hardflow::udp_t::size_type
irs::lwip::hardflow::udp_t::channel_next()
{
  if (m_mode == csm_any) {
    return m_channel_list.channel_next();
  } else if (m_mode == csm_ready_for_reading) {
    return udp_read_buffer()->channel(this);
  }
  return invalid_channel;


}

bool irs::lwip::hardflow::udp_t::is_channel_exists(size_type a_channel_ident)
{
  return m_channel_list.is_channel_exists(a_channel_ident);
}

void irs::lwip::hardflow::udp_t::set_channel_switching_mode(
  channel_switching_mode_t a_mode)
{
  m_mode = a_mode;
}

#endif // USE_LWIP
