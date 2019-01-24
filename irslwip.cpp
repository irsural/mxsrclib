#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irslwip.h>
#include <irsalg.h>

#ifdef USE_STDPERIPH_DRIVER
# if defined(IRS_STM32F2xx)
#   include <stm32f2x7_eth.h>
# elif defined(IRS_STM32F4xx)
#   include <stm32f4x7_eth.h>
# endif // defined(IRS_STM32F4xx)
#endif // USE_STDPERIPH_DRIVER

#include <irsfinal.h>


#ifdef USE_LWIP

namespace {

void copy_pbuf_to_deque_data_back(const pbuf* ap_pbuf,
  irs::deque_data_t<irs_u8>* ap_deque_data)
{
  ap_deque_data->reserve(ap_deque_data->size() + ap_pbuf->tot_len);
  for (const pbuf* q = ap_pbuf; q != NULL; q = q->next) {
    irs_u8* end = reinterpret_cast<irs_u8*>(q->payload) +
      static_cast<size_t>(q->len);
    ap_deque_data->push_back(reinterpret_cast<irs_u8*>(q->payload), end);
  }
}

void copy_pbuf_to_buffer(const pbuf* ap_pbuf, irs_u8* ap_buf)
{
  irs_u8* buffer = ap_buf;
  for (const pbuf* q = ap_pbuf; q != NULL; q = q->next) {
    memcpy(buffer, reinterpret_cast<irs_u8*>(q->payload), q->len);
    buffer += q->len;
  }
}

void copy_buffer_to_pbuf(const irs_u8* ap_buf, pbuf* ap_pbuf)
{
  const irs_u8* buf_src = ap_buf;
  for (pbuf* buf_dest = ap_pbuf; buf_dest != NULL; buf_dest = buf_dest->next) {
    memcpy(buf_dest->payload, buf_src, buf_dest->len);
    buf_src = buf_src + buf_dest->len;
  }
}

} // unnamed namespace

// Вместо него lwip_control_t
#ifndef IRS_STM32H7xx

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

  //set_lwip_accept_udp_port_fun(accept_udp_port);

  if (a_configuration.dhcp_enabled) {
    start_dhcp();
  }
}

irs::lwip::ethernet_t::~ethernet_t()
{
  netif_remove(&m_netif);
}

mxip_t irs::lwip::ethernet_t::get_ip()
{
  mxip_t ip;
  ip.addr = m_netif.ip_addr.addr;
  return ip;
}

mxip_t irs::lwip::ethernet_t::get_netmask()
{
  mxip_t netmask;
  netmask.addr = m_netif.netmask.addr;
  return netmask;
}

mxip_t irs::lwip::ethernet_t::get_gateway()
{
  mxip_t gw;
  gw.addr = m_netif.gw.addr;
  return gw;
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
    copy_buffer_to_pbuf(buffer, p);
    mp_simple_ethernet->set_recv_handled();
    IRS_LIB_LWIP_ETH_DBG_MSG_DETAIL("low_level_input: length = " << len);
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

#endif // IRS_STM32H7xx


#ifdef IRSLIB_USE_LWIP_CONTROL

// class lwip_control_t
irs::lwip::lwip_control_t::lwip_control_t(const config_t& a_config):
  m_mac(st_generate_mac(irs::device_code_upms_1v)),
  m_netif(irs::zero_struct_t<netif>::get()),
  m_sys_check_timeouts_loop_timer(make_cnt_ms(10)),
  m_etharp_tmr_loop_timer(make_cnt_ms(ARP_TMR_INTERVAL))
  #if LWIP_DHCP
  ,
  m_dhcp_fine_tmr_loop_timer(make_cnt_ms(DHCP_FINE_TIMER_MSECS)),
  m_dhcp_coarse_tmr_loop_timer(make_cnt_ms(DHCP_COARSE_TIMER_MSECS))
  #endif // LWIP_DHCP
{
  mem_init();
  memp_init();

  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gateway;

  IP4_ADDR(&ipaddr, a_config.ip.val[0], a_config.ip.val[1],
    a_config.ip.val[2], a_config.ip.val[3]);
  IP4_ADDR(&netmask, a_config.netmask.val[0],
    a_config.netmask.val[1], a_config.netmask.val[2],
    a_config.netmask.val[3]);
  IP4_ADDR(&gateway, a_config.gateway.val[0],
    a_config.gateway.val[1], a_config.gateway.val[2],
    a_config.gateway.val[3]);
  
  u8_t mac_to_eth[NETIF_MAX_HWADDR_LEN];
  mac_to_eth[0] = m_mac.val[0];
  mac_to_eth[1] = m_mac.val[1];
  mac_to_eth[2] = m_mac.val[2];
  mac_to_eth[3] = m_mac.val[3];
  mac_to_eth[4] = m_mac.val[4];
  mac_to_eth[5] = m_mac.val[5];
  
  eth_set_mac(mac_to_eth);

  netif_add(&m_netif, &ipaddr, &netmask, &gateway, NULL,
    &ethernetif_init, &ethernet_input);
  netif_set_default(&m_netif);
  netif_set_up(&m_netif);
  
  if (netif_is_up(&m_netif)) {
    IRS_LIB_LWIP_ETH_DBG_MSG_BASE("netif is up");
  } else {
    IRS_LIB_LWIP_ETH_DBG_MSG_BASE("netif start failed");
  }

  //set_lwip_accept_udp_port_fun(accept_udp_port);

  if (a_config.dhcp_enabled) {
    start_dhcp();
  }
}

irs::lwip::lwip_control_t::~lwip_control_t()
{
  netif_remove(&m_netif);
}

mxip_t irs::lwip::lwip_control_t::get_ip()
{
  mxip_t ip;
  ip.addr = m_netif.ip_addr.addr;
  return ip;
}

mxip_t irs::lwip::lwip_control_t::get_netmask()
{
  mxip_t netmask;
  netmask.addr = m_netif.netmask.addr;
  return netmask;
}

mxip_t irs::lwip::lwip_control_t::get_gateway()
{
  mxip_t gw;
  gw.addr = m_netif.gw.addr;
  return gw;
}

netif* irs::lwip::lwip_control_t::get_netif()
{
  return &m_netif;
}

void irs::lwip::lwip_control_t::tick()
{
  ethernetif_input(&m_netif);
  lwip_tick();
}

void irs::lwip::lwip_control_t::start_dhcp()
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

void irs::lwip::lwip_control_t::lwip_tick()
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
  /*
  if (dhcp_supplied_address(&m_netif)) {
    uint8_t iptxt[20];
    sprintf((char *)iptxt, "%s", ip4addr_ntoa(netif_ip4_addr(&m_netif)));
    irs::mlog() << "IP address assigned by a DHCP server: \n" << iptxt << endl;
  } else {
    struct dhcp *dhcp;
    dhcp = (struct dhcp *)netif_get_client_data(&m_netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
  }
  */
  
  #endif // LWIP_DHCP
}

mxmac_t irs::lwip::lwip_control_t::st_generate_mac(irs::device_code_t a_device_code)
{
  IRS_STATIC_ASSERT(sizeof(irs::device_code_t) == 1);
  const irs_u16 device_id = irs::crc16(UID_H7_BEGIN, UID_H7_SIZE);
  return generate_mac(a_device_code, device_id);
}

mxmac_t irs::lwip::lwip_control_t::get_mac()
{
  return m_mac;
}


/*
err_t irs::lwip::ethernet_t::low_level_output(
  struct netif* ap_netif, struct pbuf *p)
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
    copy_buffer_to_pbuf(buffer, p);
    mp_simple_ethernet->set_recv_handled();
    IRS_LIB_LWIP_ETH_DBG_MSG_DETAIL("low_level_input: length = " << len);
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

  ap_netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
    NETIF_FLAG_LINK_UP;
}
*/
#endif // IRSLIB_USE_LWIP_CONTROL


// class buffers_t
irs::hardflow::lwip::buffers_t::buffers_t(size_type a_buf_max_count,
  size_type a_buf_max_size
):
  m_buf_max_count(a_buf_max_count),
  m_buf_max_size(a_buf_max_size),
  m_buffers(),
  m_id_buf_pos(),
  m_free_buffers(),
  m_channel_for_read(m_id_buf_pos.end())
{
}

irs::hardflow::lwip::buffers_t::size_type
irs::hardflow::lwip::buffers_t::write(size_type a_channel_id,
  const irs_u8* ap_buf, size_type a_size)
{
  if (a_size == 0) {
    return 0;
  }

  if (a_size > m_buf_max_size) {
    return 0;
  }

  irs::deque_data_t<irs_u8>* buf = create_buf(a_channel_id);
  if (!buf) {
    return 0;
  }

  IRS_LIB_ASSERT(buf->size() <= m_buf_max_size);
  const size_type available_size = (m_buf_max_size - buf->size());
  size_type size = min(available_size, a_size);
  try {
    buf->push_back(ap_buf, ap_buf + size);
    return size;
  } catch (std::bad_alloc&) {
    return 0;
  }
}

irs::deque_data_t<irs_u8>* irs::hardflow::lwip::buffers_t::create_buf(
  size_type a_channel_id)
{
  map<size_type, size_type>::iterator it = m_id_buf_pos.find(a_channel_id);

  if (it == m_id_buf_pos.end()) {

    if (m_id_buf_pos.size() >= m_buf_max_count) {
      return NULL;
    }

    try {

      if (!m_free_buffers.empty()) {
        typedef pair<map<size_type, size_type>::iterator, bool> result_t;
        size_type pos = m_free_buffers.back();

        insert_to_id_buf_pos(a_channel_id, pos);

        m_free_buffers.pop_back();
        return &m_buffers[pos];
      } else if (m_buffers.size() < m_buf_max_count) {
        m_buffers.push_back(irs::deque_data_t<irs_u8>());

        try {
          insert_to_id_buf_pos(a_channel_id, m_buffers.size() - 1);
        } catch (std::bad_alloc&) {
          m_buffers.pop_back();
          throw;
        }

        try {
          // Чтобы гарантированно иметь возможность освободить буфер
          m_free_buffers.reserve(m_buffers.size());
        } catch (std::bad_alloc&) {
          erase_from_id_buf_pos(a_channel_id);
          m_buffers.pop_back();
          throw;
        }
        return &m_buffers[m_buffers.size() - 1];
      }

    } catch (std::bad_alloc&) {
      return NULL;
    }

  } else {
    return &m_buffers[it->second];
  }
  return NULL;
}

void irs::hardflow::lwip::buffers_t::insert_to_id_buf_pos(
  size_type a_channel_id, size_type a_pos)
{
  m_id_buf_pos.insert(make_pair(a_channel_id, a_pos));

  if (m_id_buf_pos.size() == 1) {
    m_channel_for_read = m_id_buf_pos.begin();
  }
}

void irs::hardflow::lwip::buffers_t::erase_from_id_buf_pos(
  size_type a_channel_id)
{
  map<size_type, size_type>::iterator it = m_id_buf_pos.find(a_channel_id);
  if (it == m_channel_for_read) {
    return;
  }
  erase_from_id_buf_pos(it);
}

void irs::hardflow::lwip::buffers_t::erase_from_id_buf_pos(
  map<size_type, size_type>::iterator a_it)
{
  if (m_channel_for_read == a_it) {
    ++m_channel_for_read;
    if (m_channel_for_read == m_id_buf_pos.end()) {
      m_channel_for_read = m_id_buf_pos.begin();
    }
  }

  m_id_buf_pos.erase(a_it);

  if (m_id_buf_pos.empty()) {
    m_channel_for_read = m_id_buf_pos.end();
  }
}

irs::hardflow::lwip::buffers_t::size_type
irs::hardflow::lwip::buffers_t::available_for_write(
  size_type a_channel_id) const
{
  map<size_type, size_type>::const_iterator it =
    m_id_buf_pos.find(a_channel_id);
  if (it == m_id_buf_pos.end()) {
    return available_for_write_to_new_channel();
  }
  return m_buf_max_size - m_buffers[it->second].size();
}

irs::hardflow::lwip::buffers_t::size_type
irs::hardflow::lwip::buffers_t::available_for_write_to_new_channel() const
{
  if (m_id_buf_pos.size() < m_buf_max_count) {
    return m_buf_max_size;
  } else {
    return 0;
  }
}

irs::hardflow::lwip::buffers_t::size_type
irs::hardflow::lwip::buffers_t::read(
  size_type a_channel_id, irs_u8* ap_buf, size_type a_size)
{
  map<size_type, size_type>::iterator it = m_id_buf_pos.find(a_channel_id);
  if (it == m_id_buf_pos.end()) {
    return 0;
  }
  irs::deque_data_t<irs_u8> *buf = &m_buffers[it->second];

  const size_type read_byte_count = min(buf->size(), a_size);
  if (read_byte_count > 0) {
    buf->copy_to(0, read_byte_count, ap_buf);
    buf->pop_front(read_byte_count);
    if (buf->empty()) {
      m_free_buffers.push_back(it->second);
      erase_from_id_buf_pos(it);
    }
  }

  return read_byte_count;
}

irs::hardflow::lwip::buffers_t::size_type
irs::hardflow::lwip::buffers_t::available_for_read(size_type a_channel_id) const
{
  map<size_type, size_type>::const_iterator it =
    m_id_buf_pos.find(a_channel_id);
  if (it == m_id_buf_pos.end()) {
    return 0;
  }
  return m_buffers[it->second].size();
}

void irs::hardflow::lwip::buffers_t::free_buffer(size_type a_channel_id)
{
  map<size_type, size_type>::iterator it = m_id_buf_pos.find(a_channel_id);
  if (it == m_id_buf_pos.end()) {
    return;
  }
  m_buffers[it->second].clear();
  m_free_buffers.push_back(it->second);
  m_id_buf_pos.erase(it);
}

irs::hardflow::lwip::buffers_t::size_type
irs::hardflow::lwip::buffers_t::channel_next_available_for_reading()
{
  if (m_id_buf_pos.empty()) {
    return irs::hardflow_t::invalid_channel;
  }

  ++m_channel_for_read;
  if (m_channel_for_read == m_id_buf_pos.end()) {
    m_channel_for_read = m_id_buf_pos.begin();
  }

  return m_channel_for_read->first;
}

void irs::hardflow::lwip::buffers_t::reserve_buffers()
{
  while (m_buffers.size() < m_buf_max_count) {
    try {
      m_buffers.push_back(irs::deque_data_t<irs_u8>());
      m_buffers.back().reserve(m_buf_max_size);
    } catch (std::bad_alloc&) {
      m_buffers.pop_back();
      throw;
    }

    try {
      // Чтобы гарантированно иметь возможность освободить буфер
      m_free_buffers.reserve(m_buffers.size());
    } catch (std::bad_alloc&) {
      m_buffers.pop_back();
      throw;
    }
    m_free_buffers.push_back(m_buffers.size() - 1);
  }
}

#ifndef IRS_STM32H7xx
// class tcp_server_t
irs::hardflow::lwip::tcp_server_t::size_type
irs::hardflow::lwip::tcp_server_t::m_read_channel =
  irs::hardflow::lwip::tcp_server_t::invalid_channel;

irs::hardflow::lwip::tcp_server_t::tcp_server_t(
  const mxip_t& a_local_ip,
  irs_u16 a_local_port,
  size_type a_channel_max_count,
  const configuration_t& a_configuration
):
  mp_pcb(),
  m_channels(),
  m_channels_map(),
  m_channel_max_count(a_channel_max_count),
  m_buffers(a_configuration.channel_buffer_max_count,
    a_configuration.channel_buffer_max_size),
  m_current_processed_channel(0),
  m_current_channel(invalid_channel),
  m_mode(csm_any),
  m_last_id(invalid_channel + 1)
{
  if (a_configuration.reserve_buffers) {
    m_buffers.reserve_buffers();
  }
  listen(a_local_ip, a_local_port);
}

irs::hardflow::lwip::tcp_server_t::~tcp_server_t()
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

irs::hardflow::lwip::tcp_server_t::string_type
irs::hardflow::lwip::tcp_server_t::param(const string_type& /*a_param_name*/)
{
  return string_type();
}

void irs::hardflow::lwip::tcp_server_t::set_param(
  const string_type& /*a_param_name*/, const string_type& /*a_value*/)
{
}

irs::hardflow::lwip::tcp_server_t::size_type
irs::hardflow::lwip::tcp_server_t::read(size_type a_channel_ident,
  irs_u8* ap_buf, size_type a_size)
{
  return m_buffers.read(a_channel_ident, ap_buf, a_size);
}

irs::hardflow::lwip::tcp_server_t::size_type
irs::hardflow::lwip::tcp_server_t::write(
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
        //IRS_LIB_HARDFLOWG_DBG_MSG_BASE("write: записано " << write_data_size);
      } else {
        write_data_size = 0;
        IRS_LIB_HARDFLOWG_DBG_MSG_BASE("write: запись не удалась");
      }
    }
  }
  return write_data_size;
}

irs::hardflow::lwip::tcp_server_t::size_type
irs::hardflow::lwip::tcp_server_t::channel_next()
{
  if (m_mode == csm_any) {
    if (m_channels.empty()) {
      return invalid_channel;
    } else {
      m_current_channel++;
      if (m_current_channel >= m_channels.size()) {
        m_current_channel = 0;
      }
      return m_channels[m_current_channel]->id;
    }
  } else if (m_mode == csm_ready_for_reading) {
    return m_buffers.channel_next_available_for_reading();
  }
  return invalid_channel;
}

bool irs::hardflow::lwip::tcp_server_t::is_channel_exists(
  size_type a_channel_ident)
{
  return m_channels_map.find(a_channel_ident) != m_channels_map.end();
}

void irs::hardflow::lwip::tcp_server_t::set_channel_switching_mode(
  channel_switching_mode_t a_mode)
{
  m_mode = a_mode;
}

void irs::hardflow::lwip::tcp_server_t::tick()
{
}

bool irs::hardflow::lwip::tcp_server_t::listen(
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

err_t irs::hardflow::lwip::tcp_server_t::accept(
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

bool irs::hardflow::lwip::tcp_server_t::close_connection(channel_t* ap_channel)
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
irs::hardflow::lwip::tcp_server_t::channel_t*
irs::hardflow::lwip::tcp_server_t::create_channel(
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

void irs::hardflow::lwip::tcp_server_t::erase_channel(channel_t* ap_channel)
{
  IRS_LIB_ASSERT(m_channels_map.find(ap_channel->id) != m_channels_map.end());
  std::deque<irs::handle_t<channel_t> >::iterator it =
    m_channels.begin();
  while (it != m_channels.end()) {
    if (it->get() == ap_channel) {
      m_buffers.free_buffer((*it)->id);
      m_channels_map.erase(ap_channel->id);
      m_channels.erase(it);
      break;
    }
    ++it;
  }
}

err_t irs::hardflow::lwip::tcp_server_t::recv(void *arg, tcp_pcb *pcb, pbuf *p,
  err_t err)
{
  if ((err == ERR_OK) && (p != NULL)) {
    channel_t* channel = reinterpret_cast<channel_t*>(arg);

    size_type len = p->tot_len;
    if (len > 0) {
      if (channel->server->m_buffers.available_for_write(channel->id) >= len) {
        channel->server->m_buffers.write(channel->id, pbuf_reader_t(p));
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Получено: " << len << " байт");
        tcp_recved(pcb, len);
        pbuf_free(p);
      } else {
        IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Данные не приняты, так как не удалось "
          "выделить память");
        return ERR_MEM;
      }
    }
  } else if ((err == ERR_OK) && (p == NULL)) {
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Удаленный хост разорвал соединение");
    channel_t* channel = reinterpret_cast<channel_t*>(arg);
    channel->server->close_connection(channel);
  }
  return ERR_OK;
}

void irs::hardflow::lwip::tcp_server_t::conn_err(void *arg, err_t /*a_err*/)
{
  channel_t* channel = reinterpret_cast<channel_t*>(arg);
  tcp_server_t* server = channel->server;
  std::deque<irs::handle_t<channel_t> >::iterator it =
    server->m_channels.begin();
  while (it != server->m_channels.end()) {
    if (it->get() == channel) {
      server->m_buffers.free_buffer((*it)->id);
      server->m_channels_map.erase(channel->id);
      server->m_channels.erase(it);
      break;
    }
    ++it;
  }
  IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("conn_err");
}

err_t irs::hardflow::lwip::tcp_server_t::poll(void* /*a_arg*/,
  tcp_pcb* /*ap_pcb*/)
{
  return ERR_OK;
}

err_t irs::hardflow::lwip::tcp_server_t::sent(
  void* /*ap_arg*/, tcp_pcb* /*ap_pcb*/, u16_t /*a_len*/)
{
  return ERR_OK;
}

// class tcp_client_t
irs::hardflow::lwip::tcp_client_t::tcp_client_t(
  const mxip_t& a_local_ip,
  irs_u16 a_local_port,
  const mxip_t &a_dest_ip,
  irs_u16 a_dest_port,
  const configuration_t &a_configuration
):
  m_local_ip(a_local_ip),
  m_local_port(a_local_port),
  m_dest_ip(a_dest_ip),
  m_dest_port(a_dest_port),
  mp_pcb(),
  m_marked_for_erase(false),
  m_channel_id(invalid_channel + 1),
  m_buffer(),
  m_buffer_max_size(a_configuration.buffer_max_size),
  m_need_connect(false),
  m_connection_is_established(false),
  m_delay_before_connect(make_cnt_s(1))
{
  if (a_configuration.reserve_buffer) {
    m_buffer.reserve(m_buffer_max_size);
  }
  connect();
}

irs::hardflow::lwip::tcp_client_t::~tcp_client_t()
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

bool irs::hardflow::lwip::tcp_client_t::connect()
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

bool irs::hardflow::lwip::tcp_client_t::close_connection()
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

err_t irs::hardflow::lwip::tcp_client_t::connected(
  void* arg, tcp_pcb* /*ap_tpcb*/, err_t /*a_err*/)
{
  IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Соединение установлено");
  tcp_client_t* client = reinterpret_cast<tcp_client_t*>(arg);
  client->m_connection_is_established = true;
  return ERR_OK;
}

err_t irs::hardflow::lwip::tcp_client_t::recv(void *arg, tcp_pcb *pcb,
  pbuf *p, err_t err)
{
  if ((err == ERR_OK) && (p != NULL)) {
    tcp_client_t* client = reinterpret_cast<tcp_client_t*>(arg);
    size_type len = p->tot_len;
    if (len > 0) {
      const size_type available = client->m_buffer_max_size -
        client->m_buffer.size();
      if (available >= len) {
        try {
          copy_pbuf_to_deque_data_back(p, &client->m_buffer);
          tcp_recved(pcb, len);
          pbuf_free(p);
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Получено: " << len << " байт");
        } catch (std::bad_alloc&) {
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Данные не приняты, так как не "
            "удалось выделить память");
          return ERR_MEM;
        }
      } else {
        return ERR_MEM;
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Данные не приняты, так как "
          "буфер заполнен");
      }
    }
  } else if ((err == ERR_OK) && (p == NULL)) {
    tcp_client_t* client = reinterpret_cast<tcp_client_t*>(arg);
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Удаленный хост разорвал соединение");
    client->close_connection();
  }
  return ERR_OK;
}

void irs::hardflow::lwip::tcp_client_t::conn_err(void *arg, err_t /*a_err*/)
{
  tcp_client_t* client = reinterpret_cast<tcp_client_t*>(arg);
  IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("conn_err");
  client->mp_pcb = NULL;
  client->m_marked_for_erase = false;
  client->m_need_connect = true;
  client->m_delay_before_connect.start();
}

err_t irs::hardflow::lwip::tcp_client_t::poll(void* /*ap_arg*/,
  tcp_pcb* /*ap_pcb*/)
{
  return ERR_OK;
}

err_t irs::hardflow::lwip::tcp_client_t::sent(
  void* /*ap_arg*/, tcp_pcb* /*ap_pcb*/, u16_t /*a_len*/)
{
  return ERR_OK;
}

irs::hardflow::lwip::tcp_client_t::size_type
irs::hardflow::lwip::tcp_client_t::read(
  size_type /*a_channel_ident*/, irs_u8 *ap_buf, size_type a_size)
{
  const size_type read_byte_count = min(m_buffer.size(), a_size);
  if (read_byte_count > 0) {
    m_buffer.copy_to(0, read_byte_count, ap_buf);
    m_buffer.pop_front(read_byte_count);
  }

  return read_byte_count;
}

irs::hardflow::lwip::tcp_client_t::size_type
irs::hardflow::lwip::tcp_client_t::write(
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

void irs::hardflow::lwip::tcp_client_t::tick()
{
  if (m_marked_for_erase) {
    close_connection();
  }
  if (m_need_connect && m_delay_before_connect.check()) {
    connect();
  }
}

irs::hardflow::lwip::tcp_client_t::string_type
irs::hardflow::lwip::tcp_client_t::param(const string_type& /*a_param_name*/)
{
  return string_type();
}

void irs::hardflow::lwip::tcp_client_t::set_param(
  const string_type& /*a_param_name*/,
  const string_type& /*a_param_value*/)
{
}

irs::hardflow::lwip::tcp_client_t::size_type
irs::hardflow::lwip::tcp_client_t::channel_next()
{
  return m_channel_id;
}

bool irs::hardflow::lwip::tcp_client_t::is_channel_exists(
  size_type a_channel_ident)
{
  return m_channel_id == a_channel_ident;
}

#endif // IRS_STM32H7xx


#if LWIP_UDP

// class udp_t
irs::hardflow::lwip::udp_t::udp_t(
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
    m_configuration.channel_buffer_max_count,
    m_configuration.channel_buffer_max_size,
    m_configuration.reserve_buffers,
    m_configuration.limit_lifetime_enabled,
    m_configuration.max_lifetime_sec,
    m_configuration.limit_downtime_enabled,
    m_configuration.max_downtime_sec),
  m_mode(csm_any)
{
  create();
}

void irs::hardflow::lwip::udp_t::create()
{
  mp_pcb = udp_new();
  if (!mp_pcb) {
    return;
  }

  #ifndef IRS_STM32H7xx
  ip_addr local_ip;
  #else 
  ip_addr_t local_ip;
  #endif // IRS_STM32H7xx
  
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
  if (m_dest_ip != mxip_t::any_ip()) {
    address_type remote_host_address(m_dest_ip, m_dest_port);
    m_channel_list.insert(
      remote_host_address, &remote_host_id, &insert_success);
  }
}

#ifndef IRS_STM32H7xx
void irs::hardflow::lwip::udp_t::recv(void *arg, udp_pcb* /*ap_upcb*/,
  pbuf *ap_buf, ip_addr *ap_addr, u16_t a_port)
#else 
void irs::hardflow::lwip::udp_t::recv(void *arg, udp_pcb* /*ap_upcb*/,
  pbuf *ap_buf, const ip_addr_t *ap_addr, u16_t a_port)
#endif
{
  udp_t* udp = reinterpret_cast<udp_t*>(arg);

  size_type len = ap_buf->tot_len;
  if ((len > 0) &&
      (len <= udp->m_configuration.receive_paket_data_max_size)) {
    address_type sender_address;
    sender_address.ip.addr = ap_addr->addr;
    sender_address.port = a_port;

    bool no_place = false;
    if (udp->m_configuration.mode == mode_datagram) {
      if (udp->m_channel_list.channel_buf_size_get(sender_address) != 0 ) {
        no_place = true;
      }
    } else {
      if (udp->m_channel_list.available_for_write(sender_address) < len) {
        no_place = true;
      }
    }
    if (!no_place) {
      // Оптимизировать !!!
      /*vector<irs_u8> buf(len);
      copy_pbuf_to_buffer(ap_buf, irs::vector_data(buf));
      size_type size = udp->m_channel_list.write(sender_address,
        irs::vector_data(buf), buf.size());
      */
      size_type size = udp->m_channel_list.write(sender_address,
        pbuf_reader_t(ap_buf));

      if (size == 0) {
        IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Данные отброшены, так как нет "
          "места");
      }
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Данные отброшены, так как нет "
        "места");
    }
  } else if (len > udp->m_configuration.receive_paket_data_max_size) {
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Данные отброшены, так как их размер "
      "выше допустимого");
  }
  pbuf_free(ap_buf);
}

irs::hardflow::lwip::udp_t::~udp_t()
{
  if (mp_pcb) {
    udp_remove(mp_pcb);
  }
}

irs::hardflow::lwip::udp_t::size_type
irs::hardflow::lwip::udp_t::read(size_type a_channel_ident, irs_u8 *ap_buf,
  size_type a_size)
{
  if (m_configuration.mode == mode_datagram) {
    if (m_channel_list.channel_buf_size_get(a_channel_ident) > a_size) {
      return 0;
    }
  }
  return m_channel_list.read(a_channel_ident, ap_buf, a_size);
}

irs::hardflow::lwip::udp_t::size_type
irs::hardflow::lwip::udp_t::write(size_type a_channel_ident,
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
  address_type remote_host_address;
  
  #ifndef IRS_STM32H7xx
  ip_addr dst_ip;
  #else 
  ip_addr_t dst_ip;
  #endif // IRS_STM32H7xx
  
  if (m_channel_list.address_get(a_channel_ident, &remote_host_address)) {
    dst_ip.addr = remote_host_address.ip.addr;
  } else {
    pbuf_free(pkt_buf);
    IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Канала с указанным идентификатором не "
      "существует");
    return 0;
  }
  copy_buffer_to_pbuf(ap_buf, pkt_buf);
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

void irs::hardflow::lwip::udp_t::tick()
{
  m_channel_list.tick();
}

irs::hardflow::lwip::udp_t::string_type
irs::hardflow::lwip::udp_t::param(const string_type& /*a_param_name*/)
{
  return string_type();
}

void irs::hardflow::lwip::udp_t::set_param(const string_type& /*a_param_name*/,
  const string_type& /*a_param_value*/)
{
}

irs::hardflow::lwip::udp_t::size_type
irs::hardflow::lwip::udp_t::channel_next()
{
  if (m_mode == csm_any) {
    return m_channel_list.channel_next();
  } else if (m_mode == csm_ready_for_reading) {
    return m_channel_list.channel_next_available_for_reading();

  }
  // Возможно не хватает условия для csm_ready_for_writing
  return invalid_channel;
}

bool irs::hardflow::lwip::udp_t::is_channel_exists(size_type a_channel_ident)
{
  return m_channel_list.is_channel_exists(a_channel_ident);
}

void irs::hardflow::lwip::udp_t::set_channel_switching_mode(
  channel_switching_mode_t a_mode)
{
  m_mode = a_mode;
}

#endif // LWIP_UDP

#endif // USE_LWIP

