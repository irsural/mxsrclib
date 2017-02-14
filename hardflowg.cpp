//! \file
//! \ingroup in_out_group
//! \ingroup network_in_out_group
//! \brief Коммуникационные потоки
//!
//! Дата: 21.05.2011\n
//! Дата создания: 8.09.2009

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <hardflowg.h>
#include <irscpp.h>
#include <mxdata.h>

#include <irsfinal.h>

#if defined(IRS_WIN32) || defined(IRS_LINUX)

#if defined(IRS_WIN32)
bool irs::hardflow::lib_socket_load(WSADATA* ap_wsadata, int a_version_major,
  int a_version_minor)
{
  bool lib_load_success = true;
  WORD version_requested = 0;
  IRS_LOBYTE(version_requested) = static_cast<irs_u8>(a_version_major);
  IRS_HIBYTE(version_requested) = static_cast<irs_u8>(a_version_minor);
  if (WSAStartup(version_requested, ap_wsadata) == 0) {
    // Библиотека удачно загружена
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Библиотека сокетов удачно загружена");
  } else {
    // Ошибка при загрузке библиотеки
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
      error_str(error_sock_t().get_last_error()));
    lib_load_success = false;
  }
  return lib_load_success;
}
#endif // defined(IRS_WIN32)

irs::hardflow::error_sock_t::errcode_type
irs::hardflow::error_sock_t::get_last_error()
{
  errcode_type error_code = 0;
  #if defined(IRS_WIN32)
  error_code = WSAGetLastError();
  #elif defined(IRS_LINUX)
  error_code = errno;
  #endif // IRS_WINDOWS IRS_LINUX
  return error_code;
}

// class udp_flow_t
irs::hardflow::udp_flow_t::udp_flow_t(const configuration_t& a_configuration):
  m_error_sock(),
  m_state_info(),
  #if defined(IRS_WIN32)
  m_wsd(),
  #endif // IRS_WIN32
  m_sock(),
  m_broadcast_allowed(a_configuration.broadcast_allowed),
  m_local_host_name(a_configuration.local_host_name),
  m_local_host_port(a_configuration.local_host_port),
  m_func_select_timeout(),
  m_s_kit(),
  m_send_msg_max_size(65000),
  m_channel_list(
    a_configuration.connections_mode,
    a_configuration.channel_max_count,
    a_configuration.channel_buf_max_size,
    a_configuration.limit_lifetime_enabled,
    a_configuration.max_lifetime_sec,
    a_configuration.limit_downtime_enabled,
    a_configuration.max_downtime_sec
  ),
  m_read_buf(a_configuration.channel_buf_max_size)
{
  init(a_configuration.remote_host_name, a_configuration.remote_host_port);
}

// class udp_flow_t
irs::hardflow::udp_flow_t::udp_flow_t(
  const string_type& a_local_host_name,
  const string_type& a_local_host_port,
  const string_type& a_remote_host_name,
  const string_type& a_remote_host_port,
  const udp_limit_connections_mode_t a_mode,
  const size_type a_channel_max_count,
  const size_type a_channel_buf_max_size,
  const bool a_limit_lifetime_enabled,
  const double a_max_lifetime_sec,
  const bool a_limit_downtime_enabled,
  const double a_max_downtime_sec
):
  m_error_sock(),
  m_state_info(),
  #if defined(IRS_WIN32)
  m_wsd(),
  #endif // IRS_WIN32
  m_sock(),
  m_broadcast_allowed(false),
  m_local_host_name(a_local_host_name),
  m_local_host_port(a_local_host_port),
  m_func_select_timeout(),
  m_s_kit(),
  m_send_msg_max_size(65000),
  m_channel_list(
    a_mode,
    a_channel_max_count,
    a_channel_buf_max_size,
    a_limit_lifetime_enabled,
    a_max_lifetime_sec,
    a_limit_downtime_enabled,
    a_max_downtime_sec
  ),
  m_read_buf(a_channel_buf_max_size)
{
  init(a_remote_host_name, a_remote_host_port);
}

void irs::hardflow::udp_flow_t::init(const string_type& a_remote_host_name,
  const string_type& a_remote_host_port)
{
  m_func_select_timeout.tv_sec = 0;
  m_func_select_timeout.tv_usec = 0;

  load_lib_socket();

  sockaddr_in remote_host_addr;
  memsetex(&remote_host_addr, 1);
  bool init_success = true;
  addr_init(a_remote_host_name, a_remote_host_port,
    &remote_host_addr, &init_success);
  if (init_success) {
    bool insert_success = false;
    size_type remote_host_id = 0;
    m_channel_list.insert(
      remote_host_addr, &remote_host_id, &insert_success);
    IRS_LIB_ASSERT(insert_success);
  } else {
    // Адресс не задан или задан неправильно
  }
}

irs::hardflow::udp_flow_t::~udp_flow_t()
{
  if (m_state_info.sock_created) {
    close_socket(m_sock);
  } else {
    // Сокет не создан
  }
}

const irs::hardflow::udp_flow_t::char_type*
  irs::hardflow::udp_flow_t::empty_cstr()
{
  return irst("");
}
double irs::hardflow::udp_flow_t::def_max_lifetime_sec()
{
  return 24.*60.*60.;
}
double irs::hardflow::udp_flow_t::def_max_downtime_sec()
{
  return 60.*60.;
}

void irs::hardflow::udp_flow_t::load_lib_socket()
{
  if (!m_state_info.lib_socket_loaded) {
    // Загружаем библиотеку сокетов версии 2.2
    #if defined(IRS_WIN32)
    if (lib_socket_load(&m_wsd, 2, 2)) {
      m_state_info.lib_socket_loaded = true;
    } else {
      // Загрузить библиотеку не удалось
    }
    #elif defined(IRS_LINUX)
    m_state_info.lib_socket_loaded = true;
    #endif // IRS_WINDOWS IRS_LINUX
    m_state_info.sock_created = false;
  } else {
    // Библиотека уже загружена
  }
}

void irs::hardflow::udp_flow_t::start()
{
  if (m_state_info.lib_socket_loaded) {
    if (!m_state_info.sock_created) {
      #if defined(IRS_WIN32)
      m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      #elif defined(IRS_LINUX)
      m_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
      #endif // IRS_WINDOWS IRS_LINUX
      if (m_sock != IRS_INVALID_SOCKET) {
        m_state_info.sock_created = true;
        m_state_info.set_io_mode_sock_success = false;
      } else {
        // Сокет создать не удалось
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
          error_str(m_error_sock.get_last_error()));
      }
    } else {
      // Сокет уже создан
    }
  } else {
    // Библиотека сокетов не загружена
  }
  if (m_state_info.sock_created) {
    if (!m_state_info.set_io_mode_sock_success) {
      m_state_info.set_io_mode_sock_success = true;
      #ifdef NOP
      // для включения неблокирующего режима переменная ulblock должна иметь
      // ненулевое значение
      unsigned long ulblock = 1;
      #if defined(IRS_WIN32)
      if (ioctlsocket(m_sock, FIONBIO, &ulblock) == IRS_SOCKET_ERROR) {
        // функция завершилась неудачей
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
          error_str(m_error_sock.get_last_error()));
      } else {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.bind_sock_and_ladr_success = false;
      }
      #elif defined(IRS_LINUX)
      if (fcntl(m_sock, F_SETFL, O_NONBLOCK) == IRS_SOCKET_ERROR) {
        // функция завершилась неудачей
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
          error_str(m_error_sock.get_last_error()));
      } else {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.bind_sock_and_ladr_success = false;
      }
      #endif // IRS_WINDOWS IRS_LINUX
      #endif // NOP
    } else {
      // Сокет уже переведен в неблокирующий режим
    }
  } else {
    // Сокет не создан
  }
  if (m_state_info.set_io_mode_sock_success) {
    if (!m_state_info.bind_sock_and_ladr_success) {
      sockaddr_in local_addr;
      memsetex(&local_addr, 1);
      bool init_success = true;
      local_addr_init(&local_addr, &init_success);
      if (init_success) {
        if (::bind(m_sock, (sockaddr *)&local_addr,
          sizeof(local_addr)) != IRS_SOCKET_ERROR)
        {
          if (m_broadcast_allowed) {
            int so = 1;
            if (setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST,
              reinterpret_cast<char*>(&so), sizeof(so))) {
              error_str(m_error_sock.get_last_error());
            }
          }
          m_state_info.bind_sock_and_ladr_success = true;
        } else {
          // Ошибка при связывании сокета с локальным адресом
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
            error_str(m_error_sock.get_last_error()));
        }
      } else {
        // Ошибка инициализации
      }
    } else {
      // Сокет уже привязан к локальному адресу
    }
  } else {
    // Перевод сокета в неблокирующий режим завершился неудачей
  }
}

void irs::hardflow::udp_flow_t::sock_close()
{
  close_socket(m_sock);
  m_state_info.sock_created = false;
  m_state_info.set_io_mode_sock_success = false;
  m_state_info.bind_sock_and_ladr_success = false;
}

void irs::hardflow::udp_flow_t::local_addr_init(
  sockaddr_in* ap_sockaddr, bool* ap_init_success)
{
  *ap_init_success = true;
  if (m_local_host_name != irst("")) {
    if (!adress_str_to_adress_binary(m_local_host_name,
      &ap_sockaddr->sin_addr.s_addr))
    {
      // Преобразовать адрес не удалось
      *ap_init_success = false;
    } else {
      // Адрес успешно получен
    }
  } else {
    // Адрес не задан
    ap_sockaddr->sin_addr.s_addr = htonl(INADDR_ANY);
  }
  if (*ap_init_success) {
    ap_sockaddr->sin_family = AF_INET;
    if (m_local_host_port != irst("")) {
      unsigned short port = 0;
      if (m_local_host_port.to_number(port)) {
        ap_sockaddr->sin_port = htons(port);
      } else {
        // Преобразовать строку в число не удалось
        *ap_init_success = false;
      }
    } else {
      // Порт не указан, выбераем любой порт
      ap_sockaddr->sin_port = htons(0);
    }
  } else {
    // Значение адреса получить не удалось
  }
}

void irs::hardflow::udp_flow_t::addr_init(
  const string_type& a_host_name,
  const string_type& a_host_port,
  sockaddr_in* ap_sockaddr,
  bool* ap_init_success)
{
  *ap_init_success = adress_str_to_adress_binary(
    a_host_name, &ap_sockaddr->sin_addr.s_addr);
  ap_sockaddr->sin_family = AF_INET;
  if (*ap_init_success) {
    unsigned short port = 0;
    if (a_host_port.to_number(port)) {
      ap_sockaddr->sin_port = htons(port);
    } else {
      // Преобразовать в число не удалось
      *ap_init_success = false;
    }
  } else {
    // Произошла ошибка
  }
}

bool irs::hardflow::udp_flow_t::adress_str_to_adress_binary(
  const string_type& a_adress_str, in_addr_type* ap_adress_binary)
{
  bool adress_convert_success = false;
  if (a_adress_str != irst("")) {
    irs_u32 adress = inet_addr(IRS_SIMPLE_FROM_TYPE_STR(a_adress_str.c_str()));
    if (adress == INADDR_NONE) {
      hostent* p_host =
        gethostbyname(IRS_SIMPLE_FROM_TYPE_STR(a_adress_str.c_str()));
      if (p_host != NULL) {
        // Берем первый адрес в списке
        memcpy(ap_adress_binary, p_host->h_addr_list[0], p_host->h_length);
        adress_convert_success = true;
      } else {
        IRS_LIB_HARDFLOWG_DBG_MSG_BASE(
          error_str(m_error_sock.get_last_error()));
      }
    } else {
      *ap_adress_binary = adress;
      adress_convert_success = true;
    }
  } else {
    // Адрес не задан
  }
  return adress_convert_success;
}

bool irs::hardflow::udp_flow_t::detect_func_single_arg(
  string_type a_param, string_type* ap_func_name, string_type* ap_arg) const
{
  bool detect_success = false;
  const size_type func_name_begin_pos = a_param.find_first_not_of(irst(" "));
  const size_type func_name_end_pos =
    a_param.find_first_of(irst(" ("), func_name_begin_pos + 1);
  const size_type left_square_bracket_pos = a_param.find(irst("("));
  const size_type right_square_bracket_pos = a_param.find(irst(")"));
  if ((func_name_begin_pos != string_type::npos) &&
      (func_name_end_pos != string_type::npos) &&
      (left_square_bracket_pos != string_type::npos) &&
      (right_square_bracket_pos !=string_type::npos) &&
      (func_name_begin_pos < func_name_end_pos) &&
      (func_name_end_pos <= left_square_bracket_pos) &&
      (left_square_bracket_pos < right_square_bracket_pos))
  {
    *ap_func_name = a_param.substr(func_name_begin_pos,
      func_name_end_pos - func_name_begin_pos);
    *ap_arg = a_param.substr(left_square_bracket_pos + 1,
      right_square_bracket_pos - (left_square_bracket_pos + 1));
    detect_success = true;
  } else {
  }
  return detect_success;
}

irs::hardflow::udp_flow_t::string_type irs::hardflow::udp_flow_t::param(
  const string_type &a_param_name)
{
  string_type param_value;
  if (a_param_name == irst("local_address")) {
    param_value = m_local_host_name;
  } else if (a_param_name == irst("local_port")) {
    param_value = m_local_host_port;
  } else if (a_param_name == irst("remote_address")) {
    address_type adress;
    m_channel_list.channel_address_get(m_channel_list.cur_channel(), &adress);
    param_value = inet_ntoa(adress.sin_addr);
  } else if (a_param_name == irst("remote_port")) {
    address_type adress;
    m_channel_list.channel_address_get(m_channel_list.cur_channel(), &adress);
    irs_u16 port =  ntohs(adress.sin_port);
    number_to_string(port, &param_value);
  } else if (a_param_name == irst("read_buf_max_size")) {
    number_to_string(m_channel_list.channel_buf_max_size_get(), &param_value);
  } else if (a_param_name == irst("limit_lifetime_enabled")) {
    number_to_string(m_channel_list.limit_lifetime_enabled_get(), &param_value);
  } else if (a_param_name == irst("max_lifetime")) {
    number_to_string(m_channel_list.max_lifetime_get(), &param_value);
  } else if (a_param_name == irst("limit_downtime_enabled")) {
    number_to_string(m_channel_list.limit_downtime_enabled_get(), &param_value);
  } else if (a_param_name == irst("max_downtime")) {
    number_to_string(m_channel_list.max_downtime_get(), &param_value);
  } else if (a_param_name == irst("channel_max_count")) {
    number_to_string(m_channel_list.max_size_get(), &param_value);
  } else {
    string_type func_name;
    string_type arg;
    if (detect_func_single_arg(a_param_name, &func_name, &arg)) {
      size_type channel_id = 0;
      if (string_to_number(arg, &channel_id)) {
        if (func_name == irst("lifetime")) {
          number_to_string(m_channel_list.lifetime_get(channel_id),
            &param_value);
        } else if (func_name == irst("downtime")) {
          number_to_string(m_channel_list.downtime_get(channel_id),
            &param_value);
        } else if (func_name == irst("remote_address")) {
          address_type adress;
          m_channel_list.channel_address_get(channel_id, &adress);
          param_value = inet_ntoa(adress.sin_addr);
        } else if (func_name == irst("remote_port")) {
          address_type adress;
          m_channel_list.channel_address_get(channel_id, &adress);
          irs_u16 port =  ntohs(adress.sin_port);
          number_to_string(port, &param_value);
        } else {
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Неверный формат параметра " <<
            "\"" << str_conv<std::string>(a_param_name) << "\"");
        }
      } else {
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Неверный формат параметра" <<
          "\"" << str_conv<std::string>(a_param_name) << "\"");
      }
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Неизвестный параметр " <<
        "\"" << str_conv<std::string>(a_param_name) << "\"");
    }
  }
  return param_value;
}

void irs::hardflow::udp_flow_t::set_param(const string_type &a_param_name,
  const string_type &a_value)
{
  if (a_param_name == irst("remote_address")) {
    in_addr_type adress_binary = 0;
    if (adress_str_to_adress_binary(a_value, &adress_binary)) {
      address_type adress;
      m_channel_list.channel_address_get(m_channel_list.cur_channel(), &adress);
      adress.sin_addr.s_addr = adress_binary;
      m_channel_list.channel_address_set(m_channel_list.cur_channel(), adress);
    } else {
      // Преобразовать адрес не удалось
    }
  } else if (a_param_name == irst("remote_port")) {
    irs_u16 port = 0;
    if (string_to_number(a_value, &port)) {
      address_type adress;
      m_channel_list.channel_address_get(m_channel_list.cur_channel(), &adress);
      adress.sin_port = htons(port);
      m_channel_list.channel_address_set(m_channel_list.cur_channel(), adress);
    } else {
      // Преобразовать значение не удалось
    }
  } else if (a_param_name == irst("read_buf_max_size")) {
    size_type read_buf_max_size = 0;
    if (string_to_number(a_value, &read_buf_max_size)) {
      m_channel_list.channel_buf_max_size_set(read_buf_max_size);
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Недопустимое значение параметра. " <<
        "read_buf_max_size == " << str_conv<std::string>(a_value));
    }
  } else if (a_param_name == irst("limit_lifetime_enabled")) {
    bool limit_lifetime_enabled = false;
    if (string_to_number(a_value, &limit_lifetime_enabled)) {
      m_channel_list.limit_lifetime_enabled_set(limit_lifetime_enabled);
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Недопустимое значение параметра. " <<
        "limit_lifetime_enabled == " << str_conv<std::string>(a_value));
    }
  } else if (a_param_name == irst("max_lifetime")) {
    double max_lifetime = 0.;
    if (string_to_number(a_value, &max_lifetime)) {
      m_channel_list.max_lifetime_set(max_lifetime);
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Недопустимое значение параметра. " <<
        "max_lifetime == " << str_conv<std::string>(a_value));
    }
  } else if (a_param_name == irst("limit_downtime_enabled")) {
    bool limit_downtime_enabled = false;
    if (string_to_number(a_value, &limit_downtime_enabled)) {
      m_channel_list.limit_downtime_enabled_set(limit_downtime_enabled);
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Недопустимое значение параметра. " <<
        "limit_downtime_enabled == " << str_conv<std::string>(a_value));
    }
  } else if (a_param_name == irst("max_downtime")) {
    double max_downtime = 0.;
    if (string_to_number(a_value, &max_downtime)) {
      m_channel_list.max_downtime_set(max_downtime);
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Недопустимое значение параметра. " <<
        "max_downtime == " << str_conv<std::string>(a_value));
    }
  } else if (a_param_name == irst("channel_max_count")) {
    size_type channel_max_count = 0;
    if (string_to_number(a_value, &channel_max_count)) {
      m_channel_list.max_size_set(channel_max_count);
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Недопустимое значение параметра. " <<
        "channel_max_count == " << str_conv<std::string>(a_value));
    }
  } else {
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Неизвестный параметр");
  }
}

irs::hardflow::udp_flow_t::size_type irs::hardflow::udp_flow_t::read(
  size_type a_channel_ident, irs_u8 *ap_buf, size_type a_size)
{
  size_type size_rd = 0;
  size_rd = m_channel_list.read(a_channel_ident, ap_buf, a_size);
  return size_rd;
}

irs::hardflow::udp_flow_t::size_type irs::hardflow::udp_flow_t::write(
  size_type a_channel_ident, const irs_u8 *ap_buf, size_type a_size)
{
  size_type size_wr = 0;
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    sockaddr_in remote_host_adr;
    memset(&remote_host_adr, 0, sizeof(sockaddr_in));
    if (m_channel_list.address_get(a_channel_ident, &remote_host_adr)) {
      size_type msg_size = min(a_size, m_send_msg_max_size);
      const int ret = sendto(m_sock, reinterpret_cast<const char*>(ap_buf),
        msg_size, 0, reinterpret_cast<sockaddr*>(&remote_host_adr),
          sizeof(remote_host_adr));
      if (ret != IRS_SOCKET_ERROR) {
        size_wr = static_cast<size_type>(ret);
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Записано " <<
          str_conv<std::string>(num_to_str(size_wr)) << " байт");
      } else {
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Запись: " <<
          error_str(m_error_sock.get_last_error()));
      }
      m_channel_list.downtime_timer_reset(a_channel_ident);
    } else {
      // Адресс с таким идентификатором отсутсвует в списке
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("Неизвестный идентификатор");
    }
  } else {
    // Запуск не произошел
  }
  return size_wr;
}

irs::hardflow::udp_flow_t::size_type irs::hardflow::udp_flow_t::channel_next()
{
  return m_channel_list.channel_next();
}

bool irs::hardflow::udp_flow_t::is_channel_exists(size_type a_channel_ident)
{
  return m_channel_list.is_channel_exists(a_channel_ident);
}

void irs::hardflow::udp_flow_t::tick()
{
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    // Клиент успешно запущен
    FD_ZERO(&m_s_kit);
    FD_SET(m_sock, &m_s_kit);
    int ready_read_sock_count = select(m_sock + 1, &m_s_kit, NULL, NULL,
      &m_func_select_timeout);
    if ((ready_read_sock_count != IRS_SOCKET_ERROR) &&
      (ready_read_sock_count != 0))
    {
      if (FD_ISSET(m_sock, &m_s_kit)) {
        sockaddr_in sender_addr;
        socklen_type sender_addr_size = sizeof(sender_addr);
        int ret = recvfrom(m_sock, reinterpret_cast<char*>(m_read_buf.data()),
          m_read_buf.size(), 0, (sockaddr*)&sender_addr, &sender_addr_size);
        if (ret != IRS_SOCKET_ERROR) {
          m_channel_list.write(sender_addr, m_read_buf.data(), ret);
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Прочитано " <<
            str_conv<std::string>(num_to_str(ret)) << " байт");
        } else {
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Чтение: " <<
            error_str(m_error_sock.get_last_error()));
        }
      } else {
        // Нет сокетов для чтения
      }
    } else if (ready_read_sock_count == IRS_SOCKET_ERROR) {
      IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
        error_str(m_error_sock.get_last_error()));
    } else {
      // Нет сокетов для чтения
    }
  } else {
    start();
  }
  m_channel_list.tick();
}

// Создание серевера udp_flow_t
irs::handle_t<irs::hardflow_t> irs::hardflow::make_udp_flow_server(
  const udp_flow_t::string_type& a_local_port,
  const double a_max_downtime_sec,
  const udp_flow_t::size_type a_channel_max_count
)
{
  udp_flow_t::string_type local_host_name = udp_flow_t::empty_cstr();
  udp_flow_t::string_type local_host_port = a_local_port;
  udp_flow_t::string_type remote_host_name = udp_flow_t::empty_cstr();
  udp_flow_t::string_type remote_host_port = udp_flow_t::empty_cstr();
  udp_limit_connections_mode_t mode = udplc_mode_queue;
  udp_flow_t::size_type channel_max_count = a_channel_max_count;
  udp_flow_t::size_type channel_buf_max_size =
    udp_flow_t::def_channel_buf_max_size;
  bool limit_lifetime_enabled = false;
  double max_lifetime_sec = udp_flow_t::def_max_lifetime_sec();
  bool limit_downtime_enabled = true;
  double max_downtime_sec = a_max_downtime_sec;
  return new udp_flow_t(local_host_name, local_host_port, remote_host_name,
    remote_host_port, mode, channel_max_count, channel_buf_max_size,
    limit_lifetime_enabled, max_lifetime_sec, limit_downtime_enabled,
    max_downtime_sec);
}
// Создание клиента udp_flow_t
irs::handle_t<irs::hardflow_t> irs::hardflow::make_udp_flow_client(
  const udp_flow_t::string_type& a_remote_address,
  const udp_flow_t::string_type& a_remote_port
)
{
  udp_flow_t::string_type local_host_name = udp_flow_t::empty_cstr();
  udp_flow_t::string_type local_host_port = udp_flow_t::empty_cstr();
  udp_flow_t::string_type remote_host_name = a_remote_address;
  udp_flow_t::string_type remote_host_port = a_remote_port;
  udp_limit_connections_mode_t mode = udplc_mode_limited;
  udp_flow_t::size_type channel_max_count = 1;
  udp_flow_t::size_type channel_buf_max_size =
    udp_flow_t::def_channel_buf_max_size;
  bool limit_lifetime_enabled = false;
  double max_lifetime_sec = udp_flow_t::def_max_lifetime_sec();
  bool limit_downtime_enabled = false;
  double max_downtime_sec = udp_flow_t::def_max_downtime_sec();
  return new udp_flow_t(local_host_name, local_host_port, remote_host_name,
    remote_host_port, mode, channel_max_count, channel_buf_max_size,
    limit_lifetime_enabled, max_lifetime_sec, limit_downtime_enabled,
    max_downtime_sec);
}

irs::hardflow::tcp_server_t::tcp_server_t(
  irs_u16 local_port
):
  #if defined(IRS_WIN32)
  m_wsd(),
  #endif // IRS_WIN32
  m_addr(zero_struct_t<sockaddr_in>::get()),
  m_serv_select_timeout(zero_struct_t<timeval>::get()),
  m_read_fds(zero_struct_t<fd_set>::get()),
  m_write_fds(zero_struct_t<fd_set>::get()),
  m_is_open(true),
  m_local_port(local_port),
  m_server_sock(0),
  m_map_channel_sock(),
  mp_map_channel_sock_it(m_map_channel_sock.begin()),
  m_channel(invalid_channel + 1),
  m_channel_id_overflow(false),
  m_channel_max_count(static_cast<size_type>(-1))
{
  start_server();
}

irs::hardflow::tcp_server_t::~tcp_server_t()
{
  stop_server();
}

void irs::hardflow::tcp_server_t::start_server()
{
  m_is_open = true;
  bool lib_load_success = true;
  #if defined(IRS_WIN32)
  if (!lib_socket_load(&m_wsd, 2, 2)) {
    lib_load_success = false;
  } else {
    // Библиотека удачно загружена
  }
  #endif // defined(IRS_WIN32)
  if (lib_load_success) {
    m_server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(m_server_sock != IRS_INVALID_SOCKET) {
      m_addr.sin_family = AF_INET;
      m_addr.sin_port = htons(m_local_port);
      m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      if(::bind(m_server_sock, (struct sockaddr *)&m_addr,
        sizeof(m_addr)) >= 0)
      {
        int queue_lenght = 300; //длина очереди
        listen(m_server_sock, queue_lenght);
      } else {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("bind: " <<
          last_error_str() << endl);
        m_is_open = false;
        close_socket(m_server_sock);
      }
    } else {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("socket: " <<
        last_error_str() << endl);
      m_is_open = false;
    }
  } else {
    // Библиотеку загрузить не удалось
    m_is_open = true;
  }
}

irs::hardflow::tcp_server_t::size_type
  irs::hardflow::tcp_server_t::channel_next()
{
  size_type channel = invalid_channel;
  if (!m_map_channel_sock.empty()) {
    IRS_LIB_ASSERT(mp_map_channel_sock_it != m_map_channel_sock.end());
    mp_map_channel_sock_it++;
    if(mp_map_channel_sock_it == m_map_channel_sock.end())
    {
      mp_map_channel_sock_it = m_map_channel_sock.begin();
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("Reach the end" << endl);
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("Go to first socket" << endl);
    }
    channel = mp_map_channel_sock_it->first;
  } else {
    channel = invalid_channel;
  }
  return channel;
}

bool irs::hardflow::tcp_server_t::is_channel_exists(size_type a_channel_ident)
{
  return m_map_channel_sock.find(a_channel_ident) != m_map_channel_sock.end();
}

void irs::hardflow::tcp_server_t::new_channel()
{
  if (!m_channel_id_overflow) {
    m_channel++;
    if (m_channel == invalid_channel) {
      m_channel_id_overflow = true;
    } else {
      // Переполнение не произошло
      IRS_LIB_ASSERT(m_map_channel_sock.find(m_channel) ==
        m_map_channel_sock.end());
    }
  } else {
    // Уже было переполнение счетчика
  }
  if (m_channel_id_overflow)  {
    if (m_map_channel_sock.size() < m_channel_max_count) {
      if(m_channel == invalid_channel) {
        m_channel++;
      }
      map<size_type, int>::iterator it_prev =
        m_map_channel_sock.find(m_channel);
      map<size_type, int>::iterator it_cur = it_prev;
      if(it_cur != m_map_channel_sock.end()) {
        while(true) {
          it_cur++;
          if(it_cur == m_map_channel_sock.end()) {
            m_channel = it_prev->first + 1;
            if(m_channel == invalid_channel) {
              m_channel++;
            }
            it_prev = m_map_channel_sock.find(m_channel);
            it_cur = it_prev;
            if(it_cur == m_map_channel_sock.end()) {
              break;
            }
          } else if((it_cur->first - it_prev->first) > 1) {
            m_channel = it_prev->first + 1;
            break;
          }
          it_prev = it_cur;
        }
      }
    } else {
      //Нет свободных мест под новый канал
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(
        "No place for add new channel" << endl);
      m_channel = invalid_channel;
    }
  } else {
    // Переполнения счетчика не было
  }
}

irs::hardflow::tcp_server_t::size_type
  irs::hardflow::tcp_server_t::read(size_type a_channel_ident,
  irs_u8 *ap_buf, size_type a_size)
{
  size_type rd_data_size = 0;
  if (m_is_open && is_channel_exists(a_channel_ident)) {
    socket_type sock_rd = m_map_channel_sock[a_channel_ident];
    FD_SET(sock_rd, &m_read_fds);
    int sock_ready = select(sock_rd + 1, &m_read_fds,
      NULL, NULL, &m_serv_select_timeout);
    if((sock_ready != IRS_SOCKET_ERROR) && (sock_ready != 0)) {
      if(FD_ISSET(sock_rd, &m_read_fds)) {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("recv start" << endl);
        int server_read = recv(sock_rd, reinterpret_cast<char*>(ap_buf),
          a_size, 0);
        if(server_read > 0){
          rd_data_size =
            static_cast<irs::hardflow::tcp_client_t::size_type>(server_read);
        }
        else {
          if(server_read < 0) {
            IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("recv: " << last_error_str());
          }
          map<size_type, int>::iterator channel_erase_it =
            m_map_channel_sock.find(a_channel_ident);
          if(channel_erase_it == mp_map_channel_sock_it) {
            if (mp_map_channel_sock_it == m_map_channel_sock.begin()) {
              mp_map_channel_sock_it = m_map_channel_sock.end();
              if (m_map_channel_sock.size() > 1) {
                mp_map_channel_sock_it--;
              }
            } else {
              mp_map_channel_sock_it--;
            }
            m_map_channel_sock.erase(a_channel_ident);
          } else {
            m_map_channel_sock.erase(a_channel_ident);
          }
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_DETAIL(
            irs::mlog() << "Delete channel " << (int)a_channel_ident
              << " by read command" << endl;
            irs::mlog() << "-------------------------------" << endl;
            for(map<size_type, int>::iterator it = m_map_channel_sock.begin();
              it != m_map_channel_sock.end(); it++) {
              irs::mlog() << "m_channel: " << (int)it->first <<
                " socket: " << it->second << endl;
            }
            irs::mlog() << "-------------------------------" << endl;
          );
          close_socket(sock_rd);
        }
      }
    } else if(sock_ready == IRS_SOCKET_ERROR) {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("select: " << last_error_str())
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("read" << endl);
      close_socket(sock_rd);
    } else {
      // если select вернула 0, то событие не сработало
    }
    FD_CLR(sock_rd, &m_read_fds);
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("FD_CLR" << endl);
  }
  return rd_data_size;
}

irs::hardflow::tcp_server_t::size_type
  irs::hardflow::tcp_server_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  size_type wr_data_size = 0;
  if (m_is_open && is_channel_exists(a_channel_ident)) {
    socket_type sock_wr = m_map_channel_sock[a_channel_ident];
    FD_SET(sock_wr, &m_write_fds);
    int sock_ready = select(sock_wr + 1, NULL,
      &m_write_fds, NULL, &m_serv_select_timeout);
    if((sock_ready != IRS_SOCKET_ERROR) && (sock_ready != 0)) {
      if(FD_ISSET(sock_wr, &m_write_fds)) {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("send start" << endl);
        int server_write = send(sock_wr, reinterpret_cast<const char*>(ap_buf),
          a_size, 0);
        if(server_write > 0){
          wr_data_size =
            static_cast<irs::hardflow::tcp_client_t::size_type>(server_write);
        }
        else{
          if(server_write < 0) {
            IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("send: " << last_error_str());
          }
          map<size_type, int>::iterator channel_erase_it =
            m_map_channel_sock.find(a_channel_ident);
          if(channel_erase_it == mp_map_channel_sock_it) {
            if (mp_map_channel_sock_it == m_map_channel_sock.begin()) {
              mp_map_channel_sock_it = m_map_channel_sock.end();
              if (m_map_channel_sock.size() > 1) {
                mp_map_channel_sock_it--;
              }
            } else {
              mp_map_channel_sock_it--;
            }
            m_map_channel_sock.erase(a_channel_ident);
          } else {
            m_map_channel_sock.erase(a_channel_ident);
          }

          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_DETAIL(
            irs::mlog() << "Delete channel " << (int)a_channel_ident
              << " by write command" << endl;
            irs::mlog() << "-------------------------------" << endl;
            for(map<size_type, int>::iterator it = m_map_channel_sock.begin();
              it != m_map_channel_sock.end(); it++) {
              irs::mlog() << "m_channel: " << (int)it->first <<
                " socket: " << it->second << endl;
            }
            irs::mlog() << "-------------------------------" << endl;
          );
          close_socket(sock_wr);
        }
      }
    } else if(sock_ready == IRS_SOCKET_ERROR) {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("select: " <<
        last_error_str() << endl;);
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("write" << endl);
      close_socket(sock_wr);
    } else {
      // если select вернула 0, то событие не сработало
    }
    FD_CLR(sock_wr, &m_write_fds);
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("FD_CLR" << endl);
  }
  return wr_data_size;
}

void irs::hardflow::tcp_server_t::tick()
{
  if(m_is_open) {
    FD_SET(m_server_sock, &m_read_fds);
    int se_select = select(m_server_sock + 1, &m_read_fds,
      NULL, NULL, &m_serv_select_timeout);
    if((se_select != IRS_SOCKET_ERROR) && (se_select != 0)) {
      if(FD_ISSET(m_server_sock, &m_read_fds)) {
        int new_sock = accept(m_server_sock, NULL, NULL);
        if(new_sock >= 0) {
          new_channel();
          if (m_channel != invalid_channel) {
            #if (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
            pair<map<size_type, int>::iterator, bool> insert_channel =
            #endif // (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
            m_map_channel_sock.insert(make_pair(m_channel, new_sock));
            if(m_map_channel_sock.size() == 1)
              mp_map_channel_sock_it = m_map_channel_sock.begin();
            IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_DETAIL(
              if(insert_channel.second) {
                irs::mlog() << "New channel added: " << (int)m_channel << endl;
                irs::mlog() << "-------------------------------" << endl;
                for(map<size_type,int>::iterator it=m_map_channel_sock.begin();
                  it != m_map_channel_sock.end(); it++)
                {
                  irs::mlog() << "m_channel: " << (int)it->first <<
                    " socket: " << it->second << endl;
                }
                irs::mlog() << "-------------------------------" << endl;
              }
              else {
                IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(
                  "Channel already exist" << endl);
              }
            );
          }
          else {
            IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("Invalid channel" << endl);
            close_socket(new_sock);
          }
        } else {
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("accept: " <<
            last_error_str() << endl);
        }
      }
    } else if (se_select == IRS_SOCKET_ERROR) {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("select: " << last_error_str());
    } else {
      // если select вернула 0, то событие не сработало
    }
    FD_CLR(m_server_sock, &m_read_fds);
  } else {
    start_server();
  }
}

void elem_map_close_sock(pair<const size_t, int>& a_map_item)
{
  irs::hardflow::close_socket(a_map_item.second);
}

void irs::hardflow::tcp_server_t::stop_server()
{
  close_socket(m_server_sock);
  for_each(m_map_channel_sock.begin(), m_map_channel_sock.end(),
    elem_map_close_sock);
  m_map_channel_sock.clear();
  m_is_open = false;
}

irs::hardflow::tcp_server_t::string_type
  irs::hardflow::tcp_server_t::param(const string_type &/*a_name*/)
{
  string_type param;
  return param;
}

void irs::hardflow::tcp_server_t::set_param(const string_type &/*a_name*/,
  const string_type &/*a_value*/)
{
}

irs::hardflow::tcp_client_t::tcp_client_t(
  mxip_t dest_ip,
  irs_u16 dest_port
):
  m_error_sock(),
  m_state_info(),
  #if defined(IRS_WIN32)
  m_wsd(),
  #endif // IRS_WIN32
  m_addr(zero_struct_t<sockaddr_in>::get()),
  m_client_select_timeout(zero_struct_t<timeval>::get()),
  m_read_fds(zero_struct_t<fd_set>::get()),
  m_write_fds(zero_struct_t<fd_set>::get()),
  m_client_sock(0),
  m_is_open(false),
  m_dest_ip(dest_ip),
  m_dest_port(dest_port),
  m_channel(1)
{
  //start_client();
}

irs::hardflow::tcp_client_t::~tcp_client_t()
{
  if (m_state_info.sock_created) {
    close_socket(m_client_sock);
  } else {
    // Сокет не создан
  }
}

void irs::hardflow::tcp_client_t::start_client()
{
  if (!m_state_info.lib_socket_loaded) {
    // Загружаем библиотеку сокетов версии 2.2
    #if defined(IRS_WIN32)
    if (lib_socket_load(&m_wsd, 2, 2)) {
      m_state_info.lib_socket_loaded = true;
    } else {
      // Загрузить библиотеку не удалось
    }
    #elif defined(IRS_LINUX)
    m_state_info.lib_socket_loaded = true;
    #endif // IRS_WINDOWS IRS_LINUX
    m_state_info.sock_created = false;
  } else {
    // Библиотека уже загружена
  }
  if (m_state_info.lib_socket_loaded) {
    if (!m_state_info.sock_created) {
      #if defined(IRS_WIN32)
      m_client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      #elif defined(IRS_LINUX)
      m_client_sock = socket(PF_INET, SOCK_STREAM, 0);
      #endif // IRS_WINDOWS IRS_LINUX
      if (m_client_sock != IRS_INVALID_SOCKET) {
        m_state_info.sock_created = true;
        m_state_info.set_io_mode_sock_success = false;
      } else {
        // Сокет создать не удалось
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
          error_str(m_error_sock.get_last_error()));
      }
    } else {
      // Сокет уже создан
    }
  } else {
    // Библиотека сокетов не загружена
  }
  if (m_state_info.sock_created) {
    if (!m_state_info.set_io_mode_sock_success) {
      // для включения неблокирующего режима переменная ulblock должна иметь
      // ненулевое значение
      #if defined(IRS_WIN32)
      unsigned long ulblock = 1;
      if (ioctlsocket(m_client_sock, FIONBIO, &ulblock) != IRS_SOCKET_ERROR) {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.connect_sock_success = false;
      } else {
        // функция завершилась неудачей
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
          error_str(m_error_sock.get_last_error()));
      }
      #elif defined(IRS_LINUX)
      if (fcntl(m_client_sock, F_SETFL, O_NONBLOCK) != IRS_SOCKET_ERROR) {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.connect_sock_success = false;
      } else {
        // функция завершилась неудачей
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
          error_str(m_error_sock.get_last_error()));
      }
      #endif // IRS_WINDOWS IRS_LINUX
    } else {
      // Сокет уже переведен в неблокирующий режим
    }
  } else {
    // Сокет не создан
  }
  if (m_state_info.set_io_mode_sock_success) {
    if (!m_state_info.connect_sock_success) {
      m_addr.sin_family = AF_INET;
      m_addr.sin_port = htons(m_dest_port);
      m_addr.sin_addr.s_addr = reinterpret_cast<unsigned long&>(m_dest_ip);
      if (connect(m_client_sock, (struct sockaddr *)&m_addr,
        sizeof(m_addr)) != IRS_SOCKET_ERROR)
      {
        m_state_info.connect_sock_success = true;
      } else {
        const errcode_type error = m_error_sock.get_last_error();
        if (error == error_sock_t::eisconn) {
          // Сокет уже соединен с сервером
          m_state_info.connect_sock_success = true;
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(
            "Произошло подключение к серверу");
        } else {
          // Ошибка при устоновлении соединения
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
            error_str(m_error_sock.get_last_error()));
        }
      }
    } else {
      // Соединение уже установлено
    }
  } else {
    // Перевод сокета в неблокирующий режим завершился неудачей
  }
}

void irs::hardflow::tcp_client_t::stop_client()
{
  close_socket(m_client_sock);
  m_is_open = false;
  FD_ZERO(&m_read_fds);
  FD_ZERO(&m_write_fds);
  IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("Close client" << endl);
}

irs::hardflow::tcp_client_t::size_type
  irs::hardflow::tcp_client_t::channel_next()
{
  size_type channel = 1;
  return channel;
}

bool irs::hardflow::tcp_client_t::is_channel_exists(
  size_type /*a_channel_ident*/)
{
  return true;
}


irs::hardflow::tcp_client_t::size_type
  irs::hardflow::tcp_client_t::read(size_type /*a_channel_ident*/,
  irs_u8 *ap_buf, size_type a_size)
{
  size_type rd_data_size = 0;
  if (m_state_info.get_state_start()) {
    //a_channel_ident = m_channel;
    FD_SET(m_client_sock, &m_read_fds);
    int sock_ready = select(m_client_sock + 1, &m_read_fds,
      NULL, NULL, &m_client_select_timeout);
    if((sock_ready != IRS_SOCKET_ERROR) && (sock_ready != 0)) {
      if(FD_ISSET(m_client_sock, &m_read_fds)) {
        int client_read = recv(m_client_sock, reinterpret_cast<char*>(ap_buf),
          a_size, 0);
        if(client_read > 0) {
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_DETAIL(
            std::string s(reinterpret_cast<const char*>(ap_buf), client_read);
            IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irs::stime <<
              " recv data: " << s << std::endl);
          );
          rd_data_size = static_cast<size_type>(client_read);
        } else {
          if(client_read < 0) {
            IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("recv: " << last_error_str());
          }
          close_socket(m_client_sock);
          m_state_info.sock_created = false;
        }
      }
    } else if(sock_ready == IRS_SOCKET_ERROR) {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("select: " << last_error_str());
    } else {
      // если select вернула 0, то событие не сработало
    }
    FD_CLR(m_client_sock,&m_read_fds);
  }
  return rd_data_size;
}

irs::hardflow::tcp_client_t::size_type
  irs::hardflow::tcp_client_t::write(size_type /*a_channel_ident*/,
  const irs_u8 *ap_buf, size_type a_size)
{
  size_type wr_data_size = 0;
  if (m_state_info.get_state_start()) {
    socket_type sock_wr = m_client_sock;
    FD_SET(sock_wr, &m_write_fds);
    int sock_ready = select(sock_wr + 1, NULL,
      &m_write_fds, NULL, &m_client_select_timeout);
    if((sock_ready != IRS_SOCKET_ERROR) && (sock_ready != 0)) {
      if(FD_ISSET(sock_wr, &m_write_fds)) {
        int client_write = send(sock_wr, reinterpret_cast<const char*>(ap_buf),
          a_size, 0);
        if (client_write > 0) {
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_DETAIL(
            std::string s(reinterpret_cast<const char*>(ap_buf), client_write);
            IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irs::stime <<
              " send data: " << s << std::endl);
          );
          wr_data_size =
            static_cast<size_type>(client_write);
        }
        else{
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("send: " << last_error_str());
          close_socket(m_client_sock);
          m_state_info.sock_created = false;
        }
      }
    } else if(sock_ready == IRS_SOCKET_ERROR) {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("select: " << last_error_str());
    }
    else {
      // если select вернула 0, то событие не сработало
    }
    FD_CLR(sock_wr, &m_write_fds);
  }
  return wr_data_size;
}

irs::hardflow::tcp_client_t::string_type
  irs::hardflow::tcp_client_t::param(const string_type &a_param_name)
{
  string_type param_value;
  if (a_param_name == irst("remote_port")) {
    number_to_string(m_dest_port, &param_value);
  } else if (a_param_name == irst("remote_address")) {
    char ip_str[IP_STR_LEN];
    memsetex(ip_str, IP_STR_LEN);
    mxip_to_cstr(ip_str, m_dest_ip);
    param_value = ip_str;
  }
  return param_value;
}

void irs::hardflow::tcp_client_t::set_param(const string_type &a_param_name,
  const string_type &a_param_value)
{
  if (a_param_name == irst("remote_port")) {
    irs_u16 new_dest_port = 0;
    str_to_num(a_param_value, &new_dest_port);
    if (m_dest_port != new_dest_port) {
      m_dest_port = new_dest_port;
      m_state_info.connect_sock_success = false;
    }
  } else if (a_param_name == irst("remote_address")) {
    mxip_t new_dest_ip = mxip_t::zero_ip();
    new_dest_ip = make_mxip(a_param_value.c_str());
    if (m_dest_ip != new_dest_ip) {
      m_dest_ip = new_dest_ip;
      m_state_info.connect_sock_success = false;
    }
  }
}

void irs::hardflow::tcp_client_t::tick()
{
  if(!m_state_info.get_state_start()) {
    start_client();
  }
}
#endif //defined(IRS_WIN32) || defined(IRS_LINUX)

irs::hardflow::fixed_flow_t::fixed_flow_t(
  hardflow_t* ap_hardflow
):
  mp_hardflow(ap_hardflow),
  m_read_channel(0),
  mp_read_buf_cur(IRS_NULL),
  mp_read_buf(IRS_NULL),
  m_read_size_need(0),
  m_read_size_rest(0),
  m_read_status(status_success),
  m_read_timeout(make_cnt_s(2)),
  #if (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
  m_channel_not_exists(false),
  #endif // IRS_LIB_DEBUG_DETAIL
  m_write_channel(0),
  mp_write_buf_cur(IRS_NULL),
  mp_write_buf(IRS_NULL),
  m_write_size_need(0),
  m_write_size_rest(0),
  m_write_status(status_success),
  m_write_timeout(make_cnt_s(2))
{
}

void irs::hardflow::fixed_flow_t::connect(hardflow_t* ap_hardflow)
{
  mp_hardflow = ap_hardflow;
}

irs::hardflow::fixed_flow_t::size_type
  irs::hardflow::fixed_flow_t::read_byte_count() const
{
  return mp_read_buf_cur - mp_read_buf;
}

irs::hardflow::fixed_flow_t::size_type
  irs::hardflow::fixed_flow_t::write_byte_count() const
{
  return mp_write_buf_cur - mp_write_buf;
}

void irs::hardflow::fixed_flow_t::read_timeout(counter_t a_read_timeout)
{
  m_read_timeout.set(a_read_timeout);
}

counter_t irs::hardflow::fixed_flow_t::read_timeout() const
{
  return m_read_timeout.get();
}

void irs::hardflow::fixed_flow_t::write_timeout(counter_t a_write_timeout)
{
  m_write_timeout.set(a_write_timeout);
}

counter_t irs::hardflow::fixed_flow_t::write_timeout() const
{
  return m_write_timeout.get();
}

void irs::hardflow::fixed_flow_t::read(size_type a_channel_ident,
  irs_u8* ap_buf, size_type a_size)
{
  if (read_status() != status_wait) {
    m_read_status = status_wait;
    m_read_channel = a_channel_ident;
    mp_read_buf_cur = ap_buf;
    mp_read_buf = ap_buf;
    m_read_size_need = a_size;
    m_read_size_rest = a_size;
    m_read_timeout.stop();
  } else {
    IRS_LIB_ERROR(ec_standard, "irs::hardflow::fixed_flow_t::read - "
      "Попытка чтения пока предыдущая операция чтения не завершена");
  }
}

irs::hardflow::fixed_flow_t::status_t
  irs::hardflow::fixed_flow_t::read_status() const
{
  return m_read_status;
}

irs::hardflow::fixed_flow_t::size_type
  irs::hardflow::fixed_flow_t::read_abort()
{
  IRS_LIB_ASSERT(m_read_size_need >= m_read_size_rest);
  size_type m_size_retr = m_read_size_need - m_read_size_rest;
  m_read_status = status_success;
  mp_read_buf_cur = IRS_NULL;
  m_read_size_rest = 0;
  m_read_size_need = 0;
  return m_size_retr;
}

void irs::hardflow::fixed_flow_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  if (write_status() != status_wait) {

    #ifdef NOP
    if (debug_param_check_ptr(irst("prologix_hardflow"), mp_hardflow)) {
      mlog() << "";
    }
    #endif //NOP

    m_write_status = status_wait;
    m_write_channel = a_channel_ident;
    mp_write_buf_cur = ap_buf;
    mp_write_buf = ap_buf;
    m_write_size_need = a_size;
    m_write_size_rest = a_size;
    m_write_timeout.stop();
  } else {
    IRS_LIB_ERROR(ec_standard, "irs::hardflow::fixed_flow_t::write - "
      "Попытка записи пока предыдущая операция записи не завершена");
  }
}

irs::hardflow::fixed_flow_t::status_t
  irs::hardflow::fixed_flow_t::write_status() const
{
  return m_write_status;
}

irs::hardflow::fixed_flow_t::size_type
  irs::hardflow::fixed_flow_t::write_abort()
{
  IRS_LIB_ASSERT(m_write_size_need >= m_write_size_rest);
  size_type m_size_rec = m_write_size_need - m_write_size_rest;
  m_write_status = status_success;
  mp_write_buf_cur = IRS_NULL;
  m_write_size_rest = 0;
  m_write_size_need = 0;
  return m_size_rec;
}

void irs::hardflow::fixed_flow_t::tick()
{
  //mp_hardflow->tick();
  if(read_status() == status_wait) {
    if (mp_hardflow->is_channel_exists(m_read_channel)) {
      #if (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
      m_channel_not_exists = false;
      #endif // IRS_LIB_DEBUG_DETAIL
      size_type read_size = mp_hardflow->read(m_read_channel, mp_read_buf_cur,
        m_read_size_rest);
      mp_read_buf_cur += read_size;
      m_read_size_rest -= read_size;
      if (mp_read_buf_cur >= (mp_read_buf + m_read_size_need)) {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("read end") << endl);
        m_read_status = status_success;
      }
      if (read_size == 0) {
        if (m_read_timeout.stopped()) {
          m_read_timeout.start();
        }
      } else {
        m_read_timeout.stop();
      }
      if (m_read_timeout.check()) {
        m_read_status = status_error;
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
          irsm("read abort by timeout") << endl);
      }
    } else {
      m_read_status = status_error;
      #if (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
      if (!m_channel_not_exists) {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(
          irsm("read abort by channel is absent") << endl);
        m_channel_not_exists = true;
      }
      #endif // IRS_LIB_DEBUG_DETAIL
    }
  }
  if (write_status() == status_wait) {
    if (mp_hardflow->is_channel_exists(m_write_channel)) {
      #if (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
      m_channel_not_exists = false;
      #endif // IRS_LIB_DEBUG_DETAIL

      #ifdef NOP
      if (debug_param_check_ptr(irst("prologix_hardflow"), mp_hardflow)) {
        mlog() << "";
      }
      #endif //NOP

      size_type write_size = mp_hardflow->write(m_write_channel,
        mp_write_buf_cur, m_write_size_rest);
      mp_write_buf_cur += write_size;
      m_write_size_rest -= write_size;
      if (mp_write_buf_cur >= (mp_write_buf + m_write_size_need) ) {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("write end") << endl);
        m_write_status = status_success;
      }
      if (write_size == 0) {
        if (m_write_timeout.stopped()) {
          m_write_timeout.start();
        }
      } else {
        m_write_timeout.stop();
      }
      if (m_write_timeout.check()) {

        #ifdef NOP
        if (debug_param_check_ptr(irst("prologix_hardflow"), mp_hardflow)) {
          mlog() << "";
        }
        #endif //NOP

        m_write_status = status_error;
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(
          irsm("write abort by timeout") << endl);
      }
    } else {
      m_write_status = status_error;
      #if (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
      if (!m_channel_not_exists) {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(
          irsm("write abort by channel is absent") << endl);
        m_channel_not_exists = true;
      }
      #endif // IRS_LIB_DEBUG_DETAIL
    }
  }
}

irs::hardflow::simple_udp_flow_t::simple_udp_flow_t(
  simple_tcpip_t* ap_simple_udp,
  mxip_t a_local_ip,
  irs_u16 a_local_port,
  mxip_t a_dest_ip,
  irs_u16 a_dest_port,
  size_type a_channel_max_count
):
  mp_simple_udp(ap_simple_udp),
  m_local_ip(a_local_ip),
  m_local_port(a_local_port),
  m_dest_ip(a_dest_ip),
  m_dest_port(a_dest_port),
  m_channel(invalid_channel),
  m_cur_channel(invalid_channel),
  m_channel_max_count(a_channel_max_count),
  m_channel_list(m_channel_max_count, udp_channel_t())
{
  mp_simple_udp->open_udp();
  mp_simple_udp->open_port(m_local_port);
  mp_recv_buf = mp_simple_udp->get_recv_buf();
  mp_recv_buf_cur = mp_simple_udp->get_recv_buf();
  mp_send_buf = mp_simple_udp->get_send_buf();
}

irs::hardflow::simple_udp_flow_t::~simple_udp_flow_t()
{
  mp_simple_udp->close_udp();
  mp_simple_udp->close_port(m_local_port);
}

void irs::hardflow::simple_udp_flow_t::view_channel_list()
{
  IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_DETAIL(
    mlog() << irsm("Channel List updated: m_channel = ") <<
      int(m_channel) << endl;
    for (size_t list_idx = 0; list_idx < m_channel_list.size(); list_idx++) {
      mlog() << irsm("Decue index ") << list_idx << irsm(" ip: ") <<
        m_channel_list[list_idx].ip << irsm(" port: ") <<
        m_channel_list[list_idx].port << endl;
    }
  );
}

void irs::hardflow::simple_udp_flow_t::new_channel(mxip_t a_ip, irs_u16 a_port)
{
  #ifdef NOP
  static udp_channel_t channel_content(mxip_t::zero_ip(), 0);
  if (a_ip != mxip_t::zero_ip()) {
    if ((channel_content.ip != a_ip) && ((channel_content.port != a_port))) {
      mlog() << irsm("check ip: ") << a_ip << irsm(" check port: ") <<
        a_port << endl;
    }
    channel_content = udp_channel_t(a_ip, a_port);
  }
  #endif // NOP

  if (find_if(m_channel_list.begin(), m_channel_list.end(),
    channel_equal_t(a_ip, a_port)) == m_channel_list.end())
  {
    m_channel++;
    if (m_channel == invalid_channel) {
      m_channel_list.clear();
      //m_channel_list.resize(m_channel_max_count, udp_channel_t());
      irs_container_resize(&m_channel_list, m_channel_max_count);
      m_channel++;
      m_channel_list.pop_back();
      m_channel_list.push_front(udp_channel_t(a_ip, a_port));
    } else {
      if (m_channel <= m_channel_max_count) {
        m_channel_list[m_channel - 1] = udp_channel_t(a_ip, a_port);
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("Add channel: ") <<
          int(m_channel) << endl);
      } else {
        m_channel_list.pop_front();
        m_channel_list.push_back(udp_channel_t(a_ip, a_port));
      }
    }
    view_channel_list();
  }
}

void irs::hardflow::simple_udp_flow_t::delete_channels_by_downtime()
{
}

irs::hardflow::simple_udp_flow_t::size_type
  irs::hardflow::simple_udp_flow_t::channel_next()
{
  mxip_t dest_ip_cur = mxip_t::zero_ip();
  irs_u16 dest_port = 0;
  irs_u16 local_port_of_recieved_packet = 0;
  if (mp_simple_udp->read(&dest_ip_cur, &dest_port,
    &local_port_of_recieved_packet) &&
    (local_port_of_recieved_packet == m_local_port))
  {
    deque<udp_channel_t>::iterator channel_list_cur_it =
      find_if(m_channel_list.begin(), m_channel_list.end(),
      channel_equal_t(dest_ip_cur, dest_port));
    if (channel_list_cur_it != m_channel_list.end()) {
      // В deque Watcom нет операции минус на итераторах
      //size_type list_index = channel_list_cur_it - m_channel_list.begin();
      size_type list_index = irs_deque_distance(m_channel_list.begin(),
        channel_list_cur_it);
      m_cur_channel = channel_ident_from_index(list_index);
      IRS_LIB_ASSERT(channel_list_index(m_cur_channel) == list_index);
    } else {
      m_channel++;
      if (m_channel != invalid_channel) {
        if (m_channel <= m_channel_max_count) {
          m_channel_list[m_channel - 1] =
            udp_channel_t(dest_ip_cur, dest_port);
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("Add channel: ") <<
            int(m_channel) << endl);
        } else {
          m_channel_list.pop_front();
          m_channel_list.push_back(udp_channel_t(dest_ip_cur, dest_port));
        }
      } else {
        m_channel_list.clear();
        //m_channel_list.resize(m_channel_max_count, udp_channel_t());
        irs_container_resize(&m_channel_list, m_channel_max_count);
        m_channel++;
        m_channel_list.pop_back();
        m_channel_list.push_front(udp_channel_t(dest_ip_cur, dest_port));
      }
      view_channel_list();
      m_cur_channel = m_channel;
    }
  } else {
    return invalid_channel;
  }
  return m_cur_channel;
}

bool irs::hardflow::simple_udp_flow_t::is_channel_exists(
  size_type a_channel_ident)
{
  //mxip_t dest_ip_cur = mxip_t::zero_ip();
  //irs_u16 dest_port = 0;
  //#ifdef NOP
  //if (channel_next() != invalid_channel) {
  if (m_channel >= m_channel_max_count) {
    return ((a_channel_ident > (m_channel - m_channel_max_count)) &&
      (a_channel_ident <= m_channel));
  } else {
    return ((a_channel_ident <= m_channel) &&
      (a_channel_ident != invalid_channel));
  }
  /*} else {
    return false;
  }*/
  //#endif // NOP
  //return (a_channel_ident != invalid_channel);
}

irs::hardflow::simple_udp_flow_t::size_type
  irs::hardflow::simple_udp_flow_t::channel_list_index(
  size_type a_channel_ident)
{
  size_type deque_index = 0;
  if (m_channel >= m_channel_max_count) {
    deque_index = m_channel_max_count -
      (m_channel - a_channel_ident) - 1;
  } else {
    deque_index = a_channel_ident - 1;
  }
  return deque_index;
}

irs::hardflow::simple_udp_flow_t::size_type
  irs::hardflow::simple_udp_flow_t::channel_ident_from_index(
  size_type a_list_index)
{
  size_type channel_ident = invalid_channel;
  if (m_channel >= m_channel_max_count) {
    channel_ident = a_list_index + 1 - m_channel_max_count + m_channel;
  } else {
    channel_ident = a_list_index + 1;
  }
  return channel_ident;
}

irs::hardflow::simple_udp_flow_t::size_type
  irs::hardflow::simple_udp_flow_t::read(size_type a_channel_ident,
  irs_u8 *ap_buf, size_type a_size)
{
  #ifdef NOP
  IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(irsm("hardflow read begin") << endl);
  size_type read_data_size = 0;
  if (is_channel_exists(a_channel_ident)) {
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(irsm("read channel cur = ") <<
      a_channel_ident << endl);
    size_type deque_index = channel_list_index(a_channel_ident);
    IRS_LIB_ASSERT(deque_index < m_channel_max_count);
    mxip_t dest_ip = mxip_t::zero_ip();
    irs_u16 dest_port = 0;
    read_data_size =
      mp_simple_udp->read(&dest_ip, &dest_port, &m_local_port);
    if (read_data_size >= a_size) {
      if (udp_channel_t(dest_ip, dest_port) == m_channel_list[deque_index])
      {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_DETAIL(
          mlog() << irsm("read ip: ") << m_channel_list[deque_index].ip <<
            irsm(" port: ") << m_channel_list[deque_index].port << endl;
        )
        memcpyex(ap_buf, mp_recv_buf_cur, a_size);
        if (static_cast<irs_size_t>(mp_recv_buf_cur + a_size - mp_recv_buf) <
          (mp_simple_udp->recv_data_size()))
        {
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
            irsm("считан кусок пакета размером = ") << a_size << endl);
          mp_recv_buf_cur += a_size;
        } else {
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
            irsm("считан остаток пакета размером = ") << a_size << endl);
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
            irsm("пакет считан полностью") << endl);
          mp_recv_buf_cur = mp_recv_buf;
          mp_simple_udp->read_complete();
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("Пакет полностью"
            " прочитан") << endl);
        }
      } else {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
          irsm("hardflow read end, return 0") << endl);
        return 0;
      }
    }
  } else {
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("Channel don't exist, "
      "read data not posible ") << endl);
  }
  IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
    irsm("hardflow read end, return value not zero") << endl);
  return read_data_size;
  #else // NOP
  size_type read_data_size = 0;
  if (is_channel_exists(a_channel_ident)) {
    const size_type deque_index = channel_list_index(a_channel_ident);
    mxip_t dest_ip = mxip_t::zero_ip();
    irs_u16 dest_port = 0;
    const size_type recv_data_size =
      mp_simple_udp->read(&dest_ip, &dest_port, &m_local_port);
    if ((recv_data_size > 0) &&
      (dest_ip == m_channel_list[deque_index].ip) &&
      (dest_port == m_channel_list[deque_index].port)) {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(irsm("read channel cur = ") <<
        a_channel_ident << endl);
      //size_type deque_index = channel_list_index(a_channel_ident);
      IRS_LIB_ASSERT(channel_list_index(a_channel_ident) < m_channel_max_count);
      size_t byte_count =  mp_simple_udp->recv_data_size() -
        (mp_recv_buf_cur -  mp_recv_buf);
      const size_t size = min(a_size, byte_count);
      memcpyex(ap_buf, mp_recv_buf_cur, size);
      mp_recv_buf_cur += size;
      byte_count -= size;
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
        irsm("Считан кусок пакета размером = ") << size << endl);
      if (byte_count == 0) {
        mp_recv_buf_cur = mp_recv_buf;
        mp_simple_udp->read_complete();
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
          irsm("Пакет считан полностью") << endl);
      }
      /*if (static_cast<irs_size_t>(mp_recv_buf_cur + a_size - mp_recv_buf) <
        mp_simple_udp->recv_data_size())
      {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
          irsm("считан кусок пакета размером = ") << a_size << endl);
        mp_recv_buf_cur += size;
      } else {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
          irsm("считан остаток пакета размером = ") << a_size << endl);
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
          irsm("пакет считан полностью") << endl);
        mp_recv_buf_cur = mp_recv_buf;
        mp_simple_udp->read_complete();
      }*/
      read_data_size = size;
    }
  } else {
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("Channel don't exist, "
      "read data not posible ") << endl);
  }
  return read_data_size;
  #endif // NOP
}

irs::hardflow::simple_udp_flow_t::size_type
  irs::hardflow::simple_udp_flow_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  if (is_channel_exists(a_channel_ident))
  {
    size_type deque_index = channel_list_index(a_channel_ident);
    IRS_LIB_ASSERT(deque_index < m_channel_max_count);
    if (a_size > mp_simple_udp->send_data_size_max()) {
      a_size = mp_simple_udp->send_data_size_max();
    }
    if (mp_simple_udp->is_write_complete()) {
      mp_simple_udp->write(m_channel_list[deque_index].ip,
        m_channel_list[deque_index].port, m_local_port, a_size);
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BLOCK_DETAIL(
        if (m_channel_list[deque_index].ip != mxip_t::zero_ip()) {
          mlog() << irsm("dest ip: ") << m_channel_list[deque_index].ip <<
            irsm(" port: ") << m_channel_list[deque_index].port << endl;
        }
      );
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
        irsm("memcpyex in hardflow write") << endl);
      memcpyex(mp_send_buf, ap_buf, a_size);
    } else {
      return 0;
    }
  } else {
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("Channel don't exist,"
      "write data not posible ") << endl);
  }
  return a_size;
}

irs::hardflow::simple_udp_flow_t::string_type
  irs::hardflow::simple_udp_flow_t::param(const string_type& /*a_name*/)
{
  return string_type();
}

void irs::hardflow::simple_udp_flow_t::set_param(const string_type &a_name,
  const string_type& a_value)
{
  if (a_name == irst("local_addr")) {
    if(a_value != irst("")) {
      mxip_t new_ip = mxip_t::zero_ip();
      cstr_to_mxip(new_ip, IRS_SIMPLE_FROM_TYPE_STR(a_value.c_str()));
      mp_simple_udp->set_ip(new_ip);
      mxmac_t new_mac = make_mxmac(0, 0, new_ip.val[0], new_ip.val[1],
        new_ip.val[2], new_ip.val[3]);
      mp_simple_udp->set_mac(new_mac);
    }
  }
}

void irs::hardflow::simple_udp_flow_t::tick()
{
  mp_simple_udp->tick();
  delete_channels_by_downtime();
}

irs::hardflow::simple_tcp_flow_t::simple_tcp_flow_t(
  simple_tcpip_t* ap_simple_tcp,
  mxip_t a_local_ip,
  irs_u16 a_local_port,
  mxip_t a_dest_ip,
  irs_u16 a_dest_port
):
  mp_simple_tcp(ap_simple_tcp),
  m_local_ip(a_local_ip),
  m_local_port(a_local_port),
  m_dest_ip(a_dest_ip),
  m_dest_port(a_dest_port),
  m_cur_channel(invalid_channel),
  m_cur_dest_ip(mxip_t::zero_ip()),
  m_cur_dest_port(0)
{
  //mp_simple_tcp->tcp_init();
  //mp_simple_tcp->tcp_listen(1 /*channel*/, m_dest_port);
  mp_simple_tcp->passive_open_tcp();
  mp_simple_tcp->open_port(m_local_port);
  mp_recv_buf = mp_simple_tcp->get_recv_buf();
  mp_recv_buf_cur = mp_simple_tcp->get_recv_buf();
  mp_send_buf = mp_simple_tcp->get_send_buf();
}

irs::hardflow::simple_tcp_flow_t::~simple_tcp_flow_t()
{

}

irs::hardflow::simple_tcp_flow_t::size_type
  irs::hardflow::simple_tcp_flow_t::channel_next()
{
  if (mp_simple_tcp->tcp_cur_channel(&m_cur_channel, &m_cur_dest_ip,
    &m_cur_dest_port))
  {
    return m_cur_channel;
  } else {
    return invalid_channel;
  }
}

bool irs::hardflow::simple_tcp_flow_t::is_channel_exists(
  size_type /*a_channel_ident*/)
{
  return (m_cur_channel != invalid_channel);
}

irs::hardflow::simple_tcp_flow_t::size_type
  irs::hardflow::simple_tcp_flow_t::read(size_type a_channel_ident,
  irs_u8* ap_buf, size_type a_size)
{
  size_type read_data_size = 0;
  if (is_channel_exists(a_channel_ident)) {
    static size_type cur_channel = 0;
    if (cur_channel != a_channel_ident) {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(irsm("channel = ") <<
        int(a_channel_ident) << endl);
      cur_channel = a_channel_ident;
    }
    mxip_t dest_ip = mxip_t::zero_ip();
    irs_u16 dest_port = 0;
    read_data_size =
      mp_simple_tcp->read(&dest_ip, &dest_port, &m_local_port);
    if (read_data_size >= a_size) {
      memcpyex(ap_buf, mp_recv_buf_cur, a_size);
      if (static_cast<irs_size_t>(mp_recv_buf_cur + a_size - mp_recv_buf) <
        (mp_simple_tcp->recv_data_size()))
      {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
          irsm("считан кусок пакета размером = ") << a_size << endl);
        mp_recv_buf_cur += a_size;
      } else {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
          irsm("считан остаток пакета размером = ") << a_size << endl);
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
          irsm("пакет считан полностью") << endl);
        mp_recv_buf_cur = mp_recv_buf;
        mp_simple_tcp->read_complete();
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("Пакет полностью"
          " прочитан") << endl);
      }
    }
  } else {
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("Channel don't exist, "
      "read data not posible ") << endl);
  }
  return read_data_size;
}

irs::hardflow::simple_tcp_flow_t::size_type
  irs::hardflow::simple_tcp_flow_t::write(size_type a_channel_ident,
  const irs_u8* ap_buf, size_type a_size)
{
  size_t size = 0;
  if (is_channel_exists(a_channel_ident)) {
    if (mp_simple_tcp->is_write_complete()) {
      size = min(a_size, mp_simple_tcp->send_data_size_max());
      mp_simple_tcp->write(m_cur_dest_ip, m_cur_dest_port,
        m_local_port, size);
      memcpyex(mp_send_buf, ap_buf, size);
    }
  } else {
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("Channel don't exist,"
      "write data not posible ") << endl);
  }
  return size;
}

irs::hardflow::simple_tcp_flow_t::string_type
  irs::hardflow::simple_tcp_flow_t::param(const string_type& /*a_name*/)
{
  return string_type();
}

void irs::hardflow::simple_tcp_flow_t::set_param(const string_type &/*a_name*/,
  const string_type& /*a_value*/)
{
}

void irs::hardflow::simple_tcp_flow_t::tick()
{
  mp_simple_tcp->tick();
}

irs::hardflow::prologix_flow_t::prologix_flow_t(irs::hardflow_t* ap_hardflow,
  int a_address, end_line_t a_read_end_line, end_line_t a_write_end_line,
  int a_timeout_read_ms):
  mp_hardflow(ap_hardflow),
  m_buffer(),
  m_fixed_flow(mp_hardflow.get()),
  m_is_write(false),
  m_is_write_wait(false),
  m_is_read(false),
  m_is_read_wait(false),
  m_write_mode(mode_free),
  m_read_mode(mode_free),
  m_init_mode(mode_free),
  m_channel_ident(0),
  m_write_data(),
  m_read_chunk_size(100),
  m_end_line_write(irst("")),
  m_end_line_read(irst("")),
  m_read_string(irst("")),
  m_read_data(),
  m_read_command_data(),
  m_init_success(false),
  m_init_command(),
  m_init_count(0),
  m_init_channel_ident(0),
  m_transmit_data(),
  m_timeout(irs::make_cnt_s(0))
{
  switch (a_read_end_line) {
    case cr_lf: {
      m_end_line_read = irst("\r\n");
    } break;
    case cr: {
      m_end_line_read = irst("\r");
    } break;
    case lf: {
      m_end_line_read = irst("\n");
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("a_read_end_line не должен быть default или none");
    } break;
  }
  switch (a_write_end_line) {
    case cr_lf: {
      m_end_line_write = irst("\r\n");
    } break;
    case cr: {
      m_end_line_write = irst("\r");
    } break;
    case lf: {
      m_end_line_write = irst("\n");
    } break;
    case none: {
      m_end_line_write = irst("");
    } break;
    default: {
    } break;
  }

  m_read_command_data = u8_from_str(
    string_type(irst("++read eoi") + m_end_line_write));

  const int timeout = bound(a_timeout_read_ms, 1, 3000);
  m_timeout.set(irs::make_cnt_ms(timeout));

  m_init_command.push_back(irst("++addr ") + irs::string_t(a_address));
  m_init_command.push_back(irst("++auto 0"));
  m_init_command.push_back(irst("++mode 1"));
  m_init_command.push_back(irst("++eoi 1"));
  m_init_command.push_back(irst("++read_tmo_ms ") +
      irs::string_t(timeout));
  m_init_command.push_back(irst("++eos ") + irs::string_t(a_read_end_line));
  //m_init_command.push_back(irst("++eos 1"));
  m_init_command.push_back(irst("++eot_enable 0"));
  m_init_command.push_back(irst("++eot_char 0"));
  m_init_command.push_back(irst("++clr"));

  m_init_count = 0;
  m_init_mode = mode_start;
  m_init_channel_ident = channel_next();
}
irs::hardflow::prologix_flow_t::~prologix_flow_t()
{
  // Завершение сеанса с включением локального управления.
  // На Agilent 3458A занимает несколько секунд, поэтому код отключен
  #ifdef NOP
  timer_t timeout(make_cnt_s(5));
  timeout.start();
  while ((m_fixed_flow.write_status() ==
    irs::hardflow::fixed_flow_t::status_wait) && !timeout.stopped()) {
    m_fixed_flow.tick();
    for (int i = 0; i < 5; i++) {
      mp_hardflow->tick();
    }
    timeout.check();
  }
  string_type buffer = irst("++loc") + m_end_line_write;
  m_write_data = u8_from_str(buffer);
  m_fixed_flow.write(m_channel_ident, m_write_data.data(),
    m_write_data.size());
  while ((m_fixed_flow.write_status() ==
      irs::hardflow::fixed_flow_t::status_wait) && !timeout.stopped()) {
    m_fixed_flow.tick();
    for (int i = 0; i < 5; i++) {
      mp_hardflow->tick();
      timeout.check();
    }
  }
  #endif // NOP
}
irs::hardflow_t::string_type
  irs::hardflow::prologix_flow_t::param(const string_type &a_name)
{
  return mp_hardflow->param(a_name);
}
void irs::hardflow::prologix_flow_t::set_param(const string_type &a_name,
  const string_type &a_value)
{
  mp_hardflow->set_param(a_name, a_value);
}
irs::hardflow_t::size_type
  irs::hardflow::prologix_flow_t::read(size_type a_channel_ident,
  irs_u8 *ap_buf, size_type a_size)
{
  size_type read_size = 0;
  if (!m_is_read) {
    if (!m_is_read_wait) {
      m_channel_ident = a_channel_ident;
      m_read_mode = mode_start_read;
      m_is_read_wait = true;
    }
    read_size = 0;
  } else {
    read_size = min(a_size, m_read_data.size());
    irs::c_array_view_t<irs_u8> array_view(ap_buf, read_size);
    irs::mem_copy(m_read_data, 0, array_view, 0, read_size);
    if (read_size == m_read_data.size()) {
      m_read_data.clear();
      m_is_read_wait = false;
      m_is_read = false;
    } else if (read_size > 0) {
      irs::raw_data_t<irs_u8> read_data(m_read_data.data(), read_size);
      m_read_data = read_data;
    }
  }
  return read_size;
}
irs::hardflow_t::size_type
  irs::hardflow::prologix_flow_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  size_type write_size = 0;
  if (!m_is_write) {
    if (!m_is_write_wait) {
      //m_buffer.assign(reinterpret_cast<const char*>(ap_buf), a_size);
      irs::raw_data_t<irs_u8> data(ap_buf, a_size);
      m_buffer = str_from_u8(data);
      //m_buffer = m_buffer.substr(0, m_buffer.find(irst('\n')));
      m_buffer += m_end_line_write;
      m_write_data = u8_from_str(m_buffer);
      m_channel_ident = a_channel_ident;
      m_write_mode = mode_write;
      m_is_write_wait = true;
    }
    write_size = 0;
  } else {
    write_size = a_size;
    m_is_write_wait = false;
    m_is_write = false;
  }
  return write_size;
}
irs::hardflow_t::size_type
  irs::hardflow::prologix_flow_t::channel_next()
{
  return mp_hardflow->channel_next();
}
bool irs::hardflow::prologix_flow_t::is_channel_exists(size_type a_channel_ident)
{
  return mp_hardflow->is_channel_exists(a_channel_ident);
}
void irs::hardflow::prologix_flow_t::tick()
{
  mp_hardflow->tick();
  m_fixed_flow.tick();
  switch (m_init_mode) {
    case mode_free: {
    } break;
    case mode_start: {
      if (m_init_count < m_init_command.size()) {
        irs::string_t init_comm = m_init_command[m_init_count];
        init_comm += m_end_line_write;
        m_transmit_data = u8_from_str(init_comm);
        m_fixed_flow.write(m_init_channel_ident, m_transmit_data.data(),
          m_transmit_data.size());
        m_init_mode = mode_wait;
      } else {
        m_init_mode = mode_free;
        m_init_success = true;
      }
    } break;
    case mode_wait: {
      switch (m_fixed_flow.write_status()) {
        case irs::hardflow::fixed_flow_t::status_success: {
          m_init_count++;
        }
        case irs::hardflow::fixed_flow_t::status_error: {
          m_init_mode = mode_start;
        } break;
        default: {
        } break;
      }
    } break;
    default: {
    } break;
  }
  if (m_init_success) {
    switch (m_write_mode) {
      case mode_free: {
      } break;
      case mode_write: {
        m_fixed_flow.write(m_channel_ident, m_write_data.data(),
          m_write_data.size());
        m_write_mode = mode_write_wait;
      } break;
      case mode_write_wait: {
        switch (m_fixed_flow.write_status()) {
          case irs::hardflow::fixed_flow_t::status_success: {
            m_is_write = true;
            m_write_mode = mode_free;
          } break;
          case irs::hardflow::fixed_flow_t::status_error: {
            m_write_mode = mode_write;
          } break;
          default: {
          } break;
        }
      } break;
      default: {
      } break;
    }
    switch (m_read_mode) {
      case mode_free: {
      } break;
      case mode_start_read: {
        m_fixed_flow.write(m_channel_ident, m_read_command_data.data(),
          m_read_command_data.size());
        m_read_mode = mode_start_read_wait;
      } break;
      case mode_start_read_wait: {
        switch (m_fixed_flow.write_status()) {
          case irs::hardflow::fixed_flow_t::status_success: {
            m_timeout.start();
            m_read_mode = mode_read_wait;
          } break;
          case irs::hardflow::fixed_flow_t::status_error: {
            m_read_mode = mode_start_read;
          } break;
          default: {
          } break;
        }
      } break;
      case mode_read_wait: {
        const size_t prev_read_data_size = m_read_data.size();
        size_t size = m_read_chunk_size - prev_read_data_size;
        if (size > 0) {
          vector<irs_u8> read_buf(size);
          irs_u8* p_data = &read_buf.front();
          const size_t cur_read_size = mp_hardflow->read(
            m_channel_ident, p_data, size);
          if (cur_read_size > 0) {
            m_read_data.resize(m_read_data.size() + cur_read_size);
            memcpyex(m_read_data.data() + prev_read_data_size, p_data,
              cur_read_size);
            irs::raw_data_t<irs_u8> transmit_data_copy = m_read_data;
            irs::string_t read_str = str_from_u8(transmit_data_copy);
            size_t pos_end_line = read_str.find(m_end_line_read);
            if (pos_end_line != irs::string_t::npos) {
              m_read_string = read_str.substr(0,
                pos_end_line + m_end_line_read.size());
              m_read_data = u8_from_str(m_read_string);
              m_read_mode = mode_free;
              m_is_read = true;
            }
          }
        }
        if (!m_is_read)  {
          // Если не нашли конец строки повторяем чтение
          if (m_timeout.check()) {
            m_read_mode = mode_start_read;
          }
        }
      } break;
      default: {
      } break;
    }
  }
}
irs::raw_data_t<irs_u8> irs::hardflow::prologix_flow_t::u8_from_str(
  const irs::string_t& a_string)
{
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  irs::irs_string_t str_simple = irs::str_conv<irs::irs_string_t>(a_string);
  #else // !IRS_FULL_STDCPPLIB_SUPPORT
  irs::irs_string_t str_simple = IRS_SIMPLE_FROM_TYPE_STR(a_string.c_str());
  #endif // !IRS_FULL_STDCPPLIB_SUPPORT
  const irs_u8* p_str_u8 = reinterpret_cast<const irs_u8*>(str_simple.c_str());
  irs::raw_data_t<irs_u8> data_u8(p_str_u8, str_simple.size());
  return data_u8;
}
irs::string_t irs::hardflow::prologix_flow_t::str_from_u8(
  const irs::raw_data_t<irs_u8>& a_data)
{
  const char* p_data = reinterpret_cast<const char*>(a_data.data());
  irs::irs_string_t str_simple(p_data, a_data.size());
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  irs::string_t str_res = irs::str_conv<irs::string_t>(str_simple);
  #else // !IRS_FULL_STDCPPLIB_SUPPORT
  irs::string_t str_res = IRS_TYPE_FROM_SIMPLE_STR(str_simple.c_str());
  #endif // !IRS_FULL_STDCPPLIB_SUPPORT
  return str_res;
}

#ifdef IRS_WIN32

// class com_flow_t
irs::hardflow::com_flow_t::com_flow_t(
  const string_type& a_portname,
  const irs_u32 a_baud_rate,
  const irs_u32 a_f_parity,
  const irs_u8 a_parity,
  const irs_u8 a_byte_size,
  const irs_u8 a_stop_bits,
  const irs_u32 a_f_dtr_control,
  const irs_u32 a_f_rts_control
):
  m_port_status(PS_PREFECT),
  m_max_size_write(256),
  m_com(IRS_NULL),
  mp_error_trans(irs::error_trans()),
  m_com_param(),
  m_com_param_str(),
  m_on_resource_free(false)
{
  BOOL fsuccess = TRUE;
  COMMTIMEOUTS time_outs;
  m_com = CreateFile(
    (irst("\\\\.\\") + a_portname).c_str(),
    GENERIC_READ|GENERIC_WRITE,
    0,
    NULL,
    OPEN_EXISTING,
    0,
    NULL);
  if (m_com == INVALID_HANDLE_VALUE) {
    fsuccess = FALSE;
    IRS_LIB_ERROR(ec_standard,
      ("Ошибка открытия COM-порта. " + last_error_str()).c_str());
  }
  if (fsuccess) {
    // Настраиваем
    // устанавливаем размер внутренних буферов приема-передачи в драйвере порта
    fsuccess = SetupComm(m_com,m_max_size_write*2, m_max_size_write*2);
    if(!fsuccess){IRS_LIB_SEND_LAST_ERROR();}
  }
  if (fsuccess) {
    get_param_dbc();
    m_com_param.baud_rate = a_baud_rate;
    m_com_param.f_parity = a_f_parity;
    m_com_param.parity = a_parity;
    m_com_param.byte_size = a_byte_size;
    m_com_param.stop_bits = a_stop_bits;
    m_com_param.f_dtr_control = a_f_dtr_control;
    m_com_param.f_rts_control = a_f_rts_control;

    set_and_get_param_dbc();
    fsuccess = GetCommTimeouts(m_com,&time_outs);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
    }
  }
  if (fsuccess) {
    time_outs.ReadIntervalTimeout = 10;
    time_outs.ReadTotalTimeoutMultiplier = 0;
    time_outs.ReadTotalTimeoutConstant = 10;
    time_outs.WriteTotalTimeoutMultiplier = 0;
    time_outs.WriteTotalTimeoutConstant = 10;
    fsuccess = SetCommTimeouts(m_com,&time_outs);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
    }
  }
  if (fsuccess) {
    // Сброс порта
    fsuccess = PurgeComm(
      m_com, PURGE_RXABORT|PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
    }
  }
  if (!fsuccess) {
    m_port_status = PS_DEFECT;
    resource_free();
  }
}

irs::hardflow::com_flow_t::~com_flow_t()
{
  resource_free();
}

irs::hardflow::com_flow_t::string_type
irs::hardflow::com_flow_t::param(const string_type &a_name)
{
  string_type param_value_str = irst("");
  if (m_port_status == PS_PREFECT) {
    if (a_name == m_com_param_str.baud_rate) {
      param_value_str = m_com_param.baud_rate;
    } else if(a_name == m_com_param_str.f_parity) {
      param_value_str = m_com_param.f_parity;
    } else if(a_name == m_com_param_str.f_outx_cts_flow) {
      param_value_str = m_com_param.f_outx_cts_flow;
    } else if(a_name == m_com_param_str.f_outx_dsr_flow) {
      param_value_str = m_com_param.f_outx_dsr_flow;
    } else if(a_name == m_com_param_str.f_dtr_control) {
      param_value_str = m_com_param.f_dtr_control;
    } else if(a_name == m_com_param_str.f_dsr_sensitivity) {
      param_value_str = m_com_param.f_dsr_sensitivity;
    } else if(a_name == m_com_param_str.f_tx_continue_on_xoff) {
      param_value_str = m_com_param.f_tx_continue_on_xoff;
    } else if(a_name == m_com_param_str.f_out_x) {
      param_value_str = m_com_param.f_out_x;
    } else if(a_name == m_com_param_str.f_in_x) {
      param_value_str = m_com_param.f_in_x;
    } else if(a_name == m_com_param_str.f_error_char) {
      param_value_str = m_com_param.f_error_char;
    } else if(a_name == m_com_param_str.f_null) {
      param_value_str = m_com_param.f_null;
    } else if (a_name == m_com_param_str.f_rts_control) {
      param_value_str = m_com_param.f_rts_control;
    } else if(a_name == m_com_param_str.f_abort_on_error) {
      param_value_str = m_com_param.f_abort_on_error;
    } else if(a_name == m_com_param_str.xon_lim) {
      param_value_str = m_com_param.xon_lim;
    } else if(a_name == m_com_param_str.xoff_lim) {
      param_value_str = m_com_param.xoff_lim;
    } else if(a_name == m_com_param_str.byte_size) {
      param_value_str = m_com_param.byte_size;
    } else if(a_name == m_com_param_str.parity) {
      param_value_str = m_com_param.parity;
    } else if(a_name == m_com_param_str.stop_bits) {
      param_value_str = m_com_param.stop_bits;
    } else if(a_name == m_com_param_str.xon_char) {
      param_value_str = m_com_param.xon_char;
    } else if(a_name == m_com_param_str.xoff_char) {
      param_value_str = m_com_param.xoff_char;
    } else if(a_name == m_com_param_str.error_char) {
      param_value_str = m_com_param.error_char;
    } else if(a_name == m_com_param_str.eof_char) {
      param_value_str = m_com_param.eof_char;
    } else if(a_name == m_com_param_str.evt_char) {
      param_value_str = m_com_param.evt_char;
    }
  }
  return param_value_str;
}

void irs::hardflow::com_flow_t::set_param(const string_type &a_name,
  const string_type &a_value)
{
  bool fsuccess = true;
  if (m_port_status == PS_PREFECT) {
    irs_u32 value = 0;
    fsuccess = a_value.to_number(value);
    if (a_name == m_com_param_str.baud_rate) {
      if (fsuccess) {
        m_com_param.baud_rate = value;
      }
    } else if(a_name == m_com_param_str.f_parity) {
      if (fsuccess) {
        m_com_param.f_parity = value;
      }
    } else if(a_name == m_com_param_str.f_outx_cts_flow) {
      if(fsuccess){m_com_param.f_outx_cts_flow = value;}
    } else if(a_name == m_com_param_str.f_outx_dsr_flow) {
      if (fsuccess) {
        m_com_param.f_outx_dsr_flow = value;
      }
    } else if (a_name == m_com_param_str.f_dtr_control) {
      if (fsuccess) {
        m_com_param.f_dtr_control = value;
      }
    } else if (a_name == m_com_param_str.f_dsr_sensitivity) {
      if (fsuccess) {
        m_com_param.f_dsr_sensitivity = value;
      }
    } else if (a_name == m_com_param_str.f_tx_continue_on_xoff) {
      if (fsuccess) {
        m_com_param.f_tx_continue_on_xoff = value;
      }
    } else if (a_name == m_com_param_str.f_out_x) {
      if (fsuccess) {
        m_com_param.f_out_x = value;
      }
    } else if (a_name == m_com_param_str.f_in_x) {
      if (fsuccess) { m_com_param.f_in_x = value;
      }
    } else if (a_name == m_com_param_str.f_error_char) {
      if (fsuccess) {
        m_com_param.f_error_char = value;
      }
    } else if (a_name == m_com_param_str.f_null) {
      if (fsuccess) {
        m_com_param.f_null = value;
      }
    } else if (a_name == m_com_param_str.f_rts_control) {
      if (fsuccess) {
        m_com_param.f_rts_control = value;
      }
    } else if (a_name == m_com_param_str.f_abort_on_error) {
      if (fsuccess) {
        m_com_param.f_abort_on_error = value; }
    } else if (a_name == m_com_param_str.xon_lim) {
      if (fsuccess) {
        m_com_param.xon_lim = static_cast<irs_i8>(value);
      }
    } else if (a_name == m_com_param_str.xoff_lim) {
      if (fsuccess) {
        m_com_param.xoff_lim = static_cast<irs_i8>(value);
      }
    } else if (a_name == m_com_param_str.byte_size) {
      if (fsuccess) {
        m_com_param.byte_size = static_cast<irs_u8>(value);
      }
    } else if (a_name == m_com_param_str.parity) {
      if (fsuccess) {
        m_com_param.parity = static_cast<irs_u8>(value);
      }
    } else if (a_name == m_com_param_str.stop_bits) {
      if (fsuccess) {
        m_com_param.stop_bits = static_cast<irs_u8>(value);
      }
    } else if (a_name == m_com_param_str.xon_char) {
      if (fsuccess) {
        m_com_param.xon_char = static_cast<irs_i8>(value);
      }
    } else if (a_name == m_com_param_str.xoff_char) {
      if (fsuccess) {
        m_com_param.xoff_char = static_cast<irs_i8>(value);
      }
    } else if (a_name == m_com_param_str.error_char) {
      if (fsuccess) {
        m_com_param.error_char = static_cast<irs_i8>(value);
      }
    } else if(a_name == m_com_param_str.eof_char) {
      if (fsuccess) {
        m_com_param.eof_char = static_cast<irs_i8>(value);
      }
    } else if (a_name == m_com_param_str.evt_char) {
      if (fsuccess) {
        m_com_param.evt_char = static_cast<irs_i8>(value);
      }
    }
    set_and_get_param_dbc();
  }
  IRS_ASSERT(fsuccess);
}

#ifdef IRS_HARDFLOW_LOG_COM_PORT
namespace irs {
void com_flow_log(const irs_u8* ap_buf, size_t a_size, DWORD result)
{
  irs_string_t buf_str(reinterpret_cast<const char*>(ap_buf), a_size);
  buf_str = conv_notprintable_to_hex(buf_str);
  stringstream slog;
  slog << "com_flow_t::write(\"" << buf_str << "\", " << a_size;
  slog << ") == " << result;
  slog << endl;
  static irs_string_t prev_log_line = "";
  static int counter = 0;
  fstream flog("com_log.txt", ios::app | ios::out);
  if (slog.str() != prev_log_line) {
    if (counter) {
      flog << "Предыдущая строка повторялась ";
      flog << counter << " раз" << endl;
      counter = 0;
    }
    flog << slog.str();
  } else {
    counter++;
  }
  prev_log_line = slog.str();
}
} //namespace irs
#endif //IRS_HARDFLOW_LOG_COM_PORT

irs::hardflow::com_flow_t::size_type
irs::hardflow::com_flow_t::read(size_type a_channel_ident,
  irs_u8 *ap_buf, size_type a_size)
{
  DWORD fsuccess = 1;
  if (a_channel_ident != m_channel_id) {
    fsuccess = 0;
  } else {
    // Указан существующий канал
  }
  COMSTAT com_stat;
  memset((void*)&com_stat, 0, sizeof(COMSTAT));
  if (m_port_status == PS_DEFECT) {
    fsuccess = 0;
  }
  if (fsuccess) {
    DWORD errors = 0;
    fsuccess = ClearCommError(m_com, &errors, &com_stat);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
      return 0;
    }
  }
  size_type size_rd = 0;
  if (fsuccess) {
    size_type num_bytes_buf_rd = com_stat.cbInQue;
    if (num_bytes_buf_rd > 0) {
      DWORD num_of_bytes_read = 0;
      size_rd = min(a_size, num_bytes_buf_rd);
      fsuccess = ReadFile(m_com, ap_buf, size_rd, &num_of_bytes_read, NULL);

      #ifdef IRS_HARDFLOW_LOG_COM_PORT
      com_flow_log(ap_buf, num_of_bytes_read, fsuccess);
      #endif //IRS_HARDFLOW_LOG_COM_PORT

      if (fsuccess) {
        size_rd = num_of_bytes_read;
      } else {
        size_rd = 0;
        IRS_LIB_SEND_LAST_ERROR();
      }
    }
  } else {
    m_port_status = PS_DEFECT;
    resource_free();
  }
  return size_rd;
}

irs::hardflow::com_flow_t::size_type
irs::hardflow::com_flow_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  DWORD fsuccess = 1;
  if (a_channel_ident != m_channel_id) {
    fsuccess = 0;
  } else {
    // Указан существующий канал
  }
  COMSTAT com_stat;
  memset((void*)&com_stat, 0, sizeof(COMSTAT));
  if (m_port_status == PS_DEFECT) {
    fsuccess = 0;
  }
  if (fsuccess) {
    DWORD errors = 0;
    fsuccess = ClearCommError(m_com, &errors, &com_stat);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
      return 0;
    }
  }
  irs_u32 size_wr = 0;
  if (fsuccess) {
    irs_uarc num_bytes_buf_wr = com_stat.cbOutQue;
    if(num_bytes_buf_wr == 0){
      DWORD num_of_bytes_written = 0;
      size_wr = min(a_size, m_max_size_write);
      fsuccess = WriteFile(m_com, ap_buf, size_wr,
        &num_of_bytes_written, NULL);

      #ifdef IRS_HARDFLOW_LOG_COM_PORT
      com_flow_log(ap_buf, num_of_bytes_written, fsuccess);
      #endif //IRS_HARDFLOW_LOG_COM_PORT

      if (fsuccess) {
        size_wr = num_of_bytes_written;
      } else {
        size_wr = 0;
        IRS_LIB_SEND_LAST_ERROR();
      }
    }
  } else {
    m_port_status = PS_DEFECT;
    resource_free();
  }
  return size_wr;
}

irs::hardflow::com_flow_t::size_type
irs::hardflow::com_flow_t::channel_next()
{
  return m_channel_id;
}

bool irs::hardflow::com_flow_t::is_channel_exists(size_type a_channel_ident)
{
  return (m_channel_id == a_channel_ident);
}

void irs::hardflow::com_flow_t::tick()
{}

void irs::hardflow::com_flow_t::set_and_get_param_dbc()
{
  DWORD fsuccess = 1;
  DCB dcb;
  if (m_port_status == PS_DEFECT) {
    fsuccess = 0;
  } else {
    fsuccess = GetCommState(m_com, &dcb);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
    }
  }
  if(fsuccess){
    dcb.BaudRate = m_com_param.baud_rate;
    dcb.fParity = m_com_param.f_parity;
    dcb.fOutxCtsFlow = m_com_param.f_outx_cts_flow;
    dcb.fOutxDsrFlow = m_com_param.f_outx_dsr_flow;
    dcb.fDtrControl = m_com_param.f_dtr_control;
    dcb.fDsrSensitivity = m_com_param.f_dsr_sensitivity;
    dcb.fTXContinueOnXoff = m_com_param.f_tx_continue_on_xoff;
    dcb.fOutX = m_com_param.f_out_x;
    dcb.fInX = m_com_param.f_in_x;
    dcb.fErrorChar = m_com_param.f_error_char;
    dcb.fNull = m_com_param.f_null;
    dcb.fRtsControl = m_com_param.f_rts_control;
    dcb.fAbortOnError = m_com_param.f_abort_on_error;
    dcb.XonLim = m_com_param.xon_lim;
    dcb.XoffLim = m_com_param.xoff_lim;
    dcb.ByteSize = m_com_param.byte_size;
    dcb.Parity = m_com_param.parity;
    dcb.StopBits = m_com_param.stop_bits;
    dcb.XonChar = m_com_param.xon_char;
    dcb.XoffChar = m_com_param.xoff_char;
    dcb.ErrorChar = m_com_param.error_char;
    dcb.EofChar = m_com_param.eof_char;
    dcb.EvtChar = m_com_param.evt_char;
    fsuccess = SetCommState(m_com, &dcb);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
      return;
    }
    get_param_dbc();
  } else {
    m_port_status = PS_DEFECT;
    resource_free();
  }
}
void irs::hardflow::com_flow_t::get_param_dbc()
{
  DWORD fsuccess = 0;
  DCB dcb;
  if (m_port_status == PS_DEFECT) {
    fsuccess = 0;
  } else {
    fsuccess = GetCommState(m_com, &dcb);
    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
    }
  }
  if (fsuccess) {
    m_com_param.baud_rate = dcb.BaudRate;
    m_com_param.f_parity = dcb.fParity;
    m_com_param.f_outx_cts_flow = dcb.fOutxCtsFlow;
    m_com_param.f_outx_dsr_flow = dcb.fOutxDsrFlow;
    m_com_param.f_dtr_control = dcb.fDtrControl;
    m_com_param.f_dsr_sensitivity = dcb.fDsrSensitivity;
    m_com_param.f_tx_continue_on_xoff = dcb.fTXContinueOnXoff;
    m_com_param.f_out_x = dcb.fOutX;
    m_com_param.f_in_x = dcb.fInX;
    m_com_param.f_error_char = dcb.fErrorChar;
    m_com_param.f_null = dcb.fNull;
    m_com_param.f_rts_control = dcb.fRtsControl;
    m_com_param.f_abort_on_error = dcb.fAbortOnError;
    m_com_param.xon_lim = dcb.XonLim;
    m_com_param.xoff_lim = dcb.XoffLim;
    m_com_param.byte_size = dcb.ByteSize;
    m_com_param.parity = dcb.Parity;
    m_com_param.stop_bits = dcb.StopBits;
    m_com_param.xon_char = dcb.XonChar;
    m_com_param.xoff_char = dcb.XoffChar;
    m_com_param.error_char = dcb.ErrorChar;
    m_com_param.eof_char = dcb.EofChar;
    m_com_param.evt_char = dcb.EvtChar;

    if (!fsuccess) {
      IRS_LIB_SEND_LAST_ERROR();
    }
  } else {
    m_port_status = PS_DEFECT;
    resource_free();
  }
}
void irs::hardflow::com_flow_t::resource_free()
{
  if (!m_on_resource_free) {
    m_on_resource_free = true;
    PurgeComm(m_com, PURGE_RXABORT|PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
    CloseHandle(m_com);
  }
}

// class usb_hid_t
irs::hardflow::usb_hid_t::usb_hid_t(const string_type& a_device_path,
  size_type a_channel_start_index,
  size_type a_channel_count,
  size_type a_report_size
):
  m_device_path(a_device_path),
  m_channel_start_index(a_channel_start_index),
  m_channel_end_index(m_channel_start_index + a_channel_count - 1),
  m_channel_count(a_channel_count),
  m_report_size(a_report_size),
  m_packet_size(m_report_size + sizeof(report_id_field_type)),
  m_data_max_size(m_packet_size - header_size),
  m_buffer_max_size(a_report_size*report_max_count),
  m_hid_handle(NULL),
  m_read_thread(NULL),
  m_read_thread_id(0),
  m_write_thread(NULL),
  m_write_thread_id(0),
  m_hid_read_overlapped(),
  m_hid_write_overlapped(),
  m_close_read_thread_event(NULL),
  m_close_write_thread_event(NULL),
  m_read_buffer_mutex(NULL),
  m_write_buffer_mutex(NULL),
  m_error_string_mutex(NULL),
  m_read_packet(),
  m_write_packet(),
  m_read_buffers(m_channel_count),
  m_write_buffers(m_channel_count),
  m_user_read_buffers(m_channel_count),
  m_user_write_buffers(m_channel_count),
  m_error_string(),
  m_user_error_string(),
  m_error(false),
  m_sync_read_buffers_loop_timer(irs::make_cnt_ms(5)),
  m_sync_write_buffers_loop_timer(irs::make_cnt_ms(5)),
  m_write_buf_index(0),
  m_channel(invalid_channel + 1)
{
  memset(&m_hid_read_overlapped, 0, sizeof(m_hid_read_overlapped));
  memset(&m_hid_write_overlapped, 0, sizeof(m_hid_write_overlapped));

  for (size_type i = 0; i < m_user_read_buffers.size(); i++) {
    m_read_buffers[i].reserve(m_buffer_max_size);
    m_user_read_buffers[i].reserve(m_buffer_max_size);
  }

  try {
    if ((m_channel_start_index < 1) ||
      (m_channel_start_index > static_cast<size_type>(
      std::numeric_limits<channel_field_type>::max() + 1))) {
      throw std::logic_error("Стартовый индекс должен быть от 1 до 256");
    }
    if ((a_channel_count < 1) || (a_channel_count > static_cast<size_type>(
      std::numeric_limits<channel_field_type>::max()))) {
      throw std::logic_error("Количество каналов должно быть от 1 до 255");
    }
    if ((m_report_size < 1) || (m_report_size > report_max_size)) {
      std::ostringstream msg;
      msg << "Значение a_report_size должно быть от 1 до " << report_max_size;
      throw std::logic_error(msg.str());
    }
    m_hid_handle = CreateFile(
      a_device_path.c_str(),
      GENERIC_READ|GENERIC_WRITE,
      FILE_SHARE_READ|FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_OVERLAPPED,
      NULL);

    if (m_hid_handle == INVALID_HANDLE_VALUE) {
      throw std::runtime_error(
        ("Ошибка открытия hid-устройства. " + irs::last_error_str()).c_str());
    }

    m_hid_read_overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hid_write_overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    m_read_buffer_mutex = CreateMutex(NULL, FALSE, NULL);
    if (!m_read_buffer_mutex) {
      throw std::runtime_error(("Ошибка создания mutex буфера чтения. " +
        irs::last_error_str()).c_str());
    }
    BOOL manual_reset = TRUE;
    BOOL initial_state = FALSE;
    m_close_read_thread_event = CreateEvent(NULL, manual_reset, initial_state,
      NULL);
    if (!m_close_read_thread_event) {
      throw std::runtime_error(
        ("Ошибка создания события. " + irs::last_error_str()).c_str());
    }
    m_close_write_thread_event = CreateEvent(NULL, manual_reset, initial_state,
      NULL);
    if (!m_close_write_thread_event) {
      throw std::runtime_error(
        ("Ошибка создания события. " + irs::last_error_str()).c_str());
    }

    m_read_thread = CreateThread(NULL,
      0,
      read_report,
      this,
      0,
      &m_read_thread_id);
    if (!m_read_thread) {
      throw std::runtime_error(
        ("Ошибка создания потока чтения. " + irs::last_error_str()).c_str());
    }
    m_write_buffer_mutex = CreateMutex(NULL, FALSE, NULL);
    if (!m_write_buffer_mutex) {
      throw std::runtime_error(("Ошибка создания mutex буфера записи. " +
        irs::last_error_str()).c_str());
    }
    m_error_string_mutex = CreateMutex(NULL, FALSE, NULL);
    if (!m_error_string_mutex) {
      throw std::runtime_error(("Ошибка создания mutex буфера ошибки. " +
        irs::last_error_str()).c_str());
    }

    m_write_thread = CreateThread(NULL,
      0,
      write_report,
      this,
      0,
      &m_write_thread_id);
    if (!m_write_thread) {
      throw std::runtime_error(
        ("Ошибка создания потока записи. " + irs::last_error_str()).c_str());
    }
  } catch (...) {
    release_resources();
    throw;
  }
}

irs::hardflow::usb_hid_t::~usb_hid_t()
{
  release_resources();
}

void irs::hardflow::usb_hid_t::release_resources()
{
  if (m_write_thread) {
    SetEvent(m_close_write_thread_event);
    WaitForSingleObject(m_write_thread, INFINITE);
  }
  CloseHandle(m_write_thread);
  m_write_thread = NULL;
  CloseHandle(m_close_write_thread_event);
  m_close_write_thread_event = NULL;

  if (m_read_thread) {
    SetEvent(m_close_read_thread_event);
    WaitForSingleObject(m_read_thread, INFINITE);
  }
  CloseHandle(m_read_thread);
  m_read_thread = IRS_NULL;
  CloseHandle(m_close_read_thread_event);
  m_close_read_thread_event = NULL;

  CloseHandle(m_error_string_mutex);
  m_error_string_mutex = NULL;
  CloseHandle(m_write_buffer_mutex);
  m_write_buffer_mutex = NULL;
  CloseHandle(m_read_buffer_mutex);
  m_read_buffer_mutex = NULL;

  CloseHandle(m_hid_write_overlapped.hEvent);
  m_hid_write_overlapped.hEvent = NULL;
  CloseHandle(m_hid_read_overlapped.hEvent);
  m_hid_read_overlapped.hEvent = NULL;

  CloseHandle(m_hid_handle);
  m_hid_handle = NULL;
}

irs::hardflow::usb_hid_t::string_type
irs::hardflow::usb_hid_t::param(const string_type& a_param_name)
{
  string_type param_value;
  if (a_param_name == irst("error_string")) {
    param_value = m_user_error_string;
  }
  return param_value;
}

void irs::hardflow::usb_hid_t::set_param(const string_type& /*a_name*/,
  const string_type& /*a_value*/)
{
}

irs::hardflow::usb_hid_t::size_type
irs::hardflow::usb_hid_t::read(size_type a_channel_ident, irs_u8 *ap_buf,
  size_type a_size)
{
  size_type read_size = 0;
  const size_type buf_index = channel_id_to_buf_index(a_channel_ident);
  if (buf_index < m_channel_count) {
    buffers_iterator buf_it = m_user_read_buffers.begin() +
      channel_id_to_buf_index(a_channel_ident);
    read_size = read_from_buffer(&(*buf_it), ap_buf, a_size);
  }
  return read_size;
}

irs::hardflow::usb_hid_t::size_type
irs::hardflow::usb_hid_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  size_type write_size = 0;
  const size_type buf_index = channel_id_to_buf_index(a_channel_ident);
  if (buf_index < m_channel_count) {
    buffers_iterator buf_it = m_user_write_buffers.begin() + buf_index;
    write_size = write_to_buffer(&(*buf_it), ap_buf, a_size);
  }
  return write_size;
}

irs::hardflow::usb_hid_t::size_type
irs::hardflow::usb_hid_t::channel_next()
{
  size_type channel = m_channel;
  m_channel++;
  if (m_channel > m_channel_end_index) {
    m_channel = m_channel_start_index;
  }
  return channel;
}

bool irs::hardflow::usb_hid_t::is_channel_exists(size_type a_channel_ident)
{
  if ((a_channel_ident >= m_channel_start_index) &&
    (a_channel_ident <= m_channel_start_index)) {
    return true;
  }
  return false;
}

void irs::hardflow::usb_hid_t::tick()
{
  if (!m_error) {
    if (m_sync_read_buffers_loop_timer.check()) {
      if (WaitForSingleObject(m_read_buffer_mutex, INFINITE) ==
        WAIT_OBJECT_0) {
        for (size_type i = 0; i < m_channel_count; i++) {
          transfer(&m_read_buffers[i], &m_user_read_buffers[i]);
        }
        ReleaseMutex(m_read_buffer_mutex);
      }
    }

    if (m_sync_write_buffers_loop_timer.check()) {
      if (WaitForSingleObject(m_write_buffer_mutex, INFINITE) ==
        WAIT_OBJECT_0) {
        for (size_type i = 0; i < m_channel_count; i++) {
          transfer(&m_user_write_buffers[i], &m_write_buffers[i]);
        }
        ReleaseMutex(m_write_buffer_mutex);
      }
    }
    if (WaitForSingleObject(m_error_string_mutex, 0) == WAIT_OBJECT_0) {
      m_error = !m_error_string.empty();
      if (m_error) {
        m_user_error_string = m_error_string;
        release_resources();
      }
      ReleaseMutex(m_error_string_mutex);
    }
  }
}

void irs::hardflow::usb_hid_t::transfer(vector<irs_u8>* ap_buffer_src,
  vector<irs_u8>* ap_buffer_dest)
{
  const size_type size = std::min(ap_buffer_src->size(),
    m_buffer_max_size - ap_buffer_dest->size());
  if (size > 0) {
    size_type pos = ap_buffer_dest->size();
    ap_buffer_dest->resize(ap_buffer_dest->size() + size);
    irs_u8* dest = vector_data(*ap_buffer_dest, pos);
    irs_u8* src = vector_data(*ap_buffer_src);
    memcpy(dest, src, size);
    ap_buffer_src->erase(ap_buffer_src->begin(), ap_buffer_src->begin() + size);
  }
}

irs::hardflow::usb_hid_t::size_type
irs::hardflow::usb_hid_t::read_from_buffer(vector<irs_u8>* ap_buffer,
  irs_u8 *ap_buf, size_type a_size)
{
  const size_type size = std::min(a_size, ap_buffer->size());
  if (size > 0) {
    memcpyex(ap_buf, vector_data(*ap_buffer), size);
    ap_buffer->erase(ap_buffer->begin(), ap_buffer->begin() + size);
  }
  return size;
}

irs::hardflow::usb_hid_t::size_type
irs::hardflow::usb_hid_t::write_to_buffer(
  vector<irs_u8>* ap_buffer, const irs_u8 *ap_buf, size_type a_size)
{
  const size_type size = std::min(a_size,
    m_buffer_max_size - ap_buffer->size());
  if (size > 0) {
    size_type pos = ap_buffer->size();
    ap_buffer->resize(ap_buffer->size() + size);
    irs_u8* dest = vector_data(*ap_buffer, pos);
    memcpyex(dest, ap_buf, size);
  }
  return size;
}

DWORD WINAPI irs::hardflow::usb_hid_t::read_report(void* ap_params)
{
  usb_hid_t* owner = static_cast<usb_hid_t*>(ap_params);
  packet_t* packet =&owner->m_read_packet;
  int global_count = 0;
  while (true) {
    bool read_success = false;
    const ULONG packet_length = owner->m_report_size + 1;
    ULONG byte_read = 0;
    IRS_LIB_ASSERT(packet_length >= sizeof(*packet));
    if (ReadFile(owner->m_hid_handle,
      packet,
      packet_length, &byte_read,
      &owner->m_hid_read_overlapped)) {
      read_success = true;
    } else {
      while (true) {
        if (GetOverlappedResult(owner->m_hid_handle,
          &owner->m_hid_read_overlapped, &byte_read, false)) {
          read_success = true;
          break;
        } else {
          bool close_thread = false;
          if (owner->check_error()) {
            close_thread = true;
          }
          if (WaitForSingleObject(owner->m_close_read_thread_event, 0) ==
              WAIT_OBJECT_0) {
            close_thread = true;
          }
          if (close_thread) {
            CancelIo(owner->m_hid_handle);
            return 0;
          }
          //CancelIo(owner->m_hid_handle);
          Sleep(1);
        }
      }
    }
    if (read_success && (byte_read == packet_length)) {
      if (packet->report_id != report_id) {
        continue;
      }
      if (static_cast<size_type>(packet->data_size) > owner->m_data_max_size) {
        continue;
      }
      global_count += owner->m_read_packet.data_size;
      const size_type buf_index =
        owner->packet_channel_id_to_buf_index(packet->channel_id);
      while (true) {
        if (WaitForSingleObject(owner->m_close_read_thread_event, 0) ==
          WAIT_OBJECT_0) {
          return 0;
        }
        if (buf_index < owner->m_channel_count) {
          if (WaitForSingleObject(owner->m_read_buffer_mutex, INFINITE) ==
            WAIT_OBJECT_0) {
            buffers_iterator it = owner->m_read_buffers.begin() + buf_index;
            if ((owner->m_buffer_max_size - it->size()) >=
              static_cast<size_type>(packet->data_size)) {
              const size_type write_size = owner->write_to_buffer(&(*it),
                packet->data, packet->data_size);
              IRS_LIB_ASSERT(write_size ==
                static_cast<size_type>(packet->data_size));
              ReleaseMutex(owner->m_read_buffer_mutex);
              break;
            } else {
              ReleaseMutex(owner->m_read_buffer_mutex);
              Sleep(1);
            }
          }
        } else {
          break;
        }
      }
    }
  }
  return 0;
}

DWORD WINAPI irs::hardflow::usb_hid_t::write_report(void* ap_params)
{
  usb_hid_t* owner = static_cast<usb_hid_t*>(ap_params);
  packet_t* packet =&owner->m_write_packet;
  bool write_success = true;
  while (true) {
    if (write_success) {
      while (true) {
        if (WaitForSingleObject(owner->m_close_write_thread_event, 0) ==
          WAIT_OBJECT_0) {
          return 0;
        }
        if (WaitForSingleObject(owner->m_write_buffer_mutex, INFINITE) ==
          WAIT_OBJECT_0) {
          size_type start_index = owner->m_write_buf_index;
          do {
            if (!owner->m_write_buffers[owner->m_write_buf_index].empty()) {
              break;
            }
            owner->m_write_buf_index++;
            if (owner->m_write_buf_index >= owner->m_channel_count) {
              owner->m_write_buf_index = 0;
            }
          } while (owner->m_write_buf_index != start_index);

          if (!owner->m_write_buffers[owner->m_write_buf_index].empty()) {
            packet->report_id = report_id;
            packet->channel_id = owner->buf_index_to_packet_channel_id(
              owner->m_write_buf_index);
            const size_type size = std::min<size_type>(
              owner->m_write_buffers[owner->m_write_buf_index].size(),
              owner->m_data_max_size);
            packet->data_size = static_cast<size_field_type>(size);
            const size_type read_count = owner->read_from_buffer(
              &owner->m_write_buffers[owner->m_write_buf_index],
              packet->data, size);
            IRS_LIB_ASSERT(read_count == size);
            ReleaseMutex(owner->m_write_buffer_mutex);
            write_success = false;
            break;
          } else {
            ReleaseMutex(owner->m_write_buffer_mutex);
            Sleep(1);
          }
          owner->m_write_buf_index++;
          if (owner->m_write_buf_index >= owner->m_channel_count) {
            owner->m_write_buf_index = 0;
          }
        }
      }
    }
    const ULONG packet_length = owner->m_packet_size;
    ULONG byte_write = 0;
    if (WriteFile
      (owner->m_hid_handle,
      packet,
      packet_length,
      &byte_write,
      (LPOVERLAPPED) &owner->m_hid_write_overlapped)) {
      write_success = true;
    } else {
      while (true) {
        if (WaitForSingleObject(owner->m_close_write_thread_event, 0) ==
          WAIT_OBJECT_0) {
          CancelIo(owner->m_hid_handle);
          return 0;
        }
        const DWORD wait_result =
          WaitForSingleObject(owner->m_hid_write_overlapped.hEvent, 3);
        if (wait_result == WAIT_OBJECT_0) {
          if (GetOverlappedResult(owner->m_hid_handle,
            &owner->m_hid_write_overlapped, &byte_write, true)) {
            write_success = true;
            break;
          } else {
            Sleep(1);
          }
        } else {
          if (owner->check_error()) {
            CancelIo(owner->m_hid_handle);
            return 0;
          }
          Sleep(1);
        }
      }
    }
  }
  return 0;
}

bool irs::hardflow::usb_hid_t::check_error()
{
  const DWORD error_code = GetLastError();

  if ((error_code == ERROR_IO_INCOMPLETE) ||
      (error_code == ERROR_IO_PENDING) /*ERROR_DEVICE_NOT_CONNECTED*/) {
    return false;
  }

  if (WaitForSingleObject(m_error_string_mutex, INFINITE) ==
    WAIT_OBJECT_0) {
    m_error_string = irs::str_conv<string_type>(
      irs::error_str(error_code));
    ReleaseMutex(m_error_string_mutex);
  }
  return true;
}

#endif // IRS_WIN32

#ifdef USE_STDPERIPH_DRIVER
#ifdef IRS_STM32F_2_AND_4
// class st_com_flow_t
irs::hardflow::arm::st_com_flow_t::st_com_flow_t(
  int a_com_index,
  gpio_channel_t a_rx,
  gpio_channel_t a_tx,
  size_type a_inbuf_size,
  size_type a_outbuf_size,
  irs_u32 a_baud_rate
):
  m_event(this),
  m_rx(a_rx),
  m_tx(a_tx),
  m_inbuf_max_size(a_inbuf_size),
  m_outbuf_max_size(a_outbuf_size),
  m_inbuf(),
  m_inbuf_overflow(false),
  m_outbuf(),
  m_usart(0),
  m_usart_typedef(0),
  m_baud_rate(a_baud_rate)
{
  m_inbuf.reserve(m_inbuf_max_size);
  m_outbuf.reserve(m_outbuf_max_size);

  IRS_LIB_ASSERT((a_rx != PNONE) || (a_tx != PNONE));

  m_usart = get_usart(a_com_index);
  irs_u32 usart_address = reinterpret_cast<irs_u32>(m_usart);
  m_usart_typedef = reinterpret_cast<USART_TypeDef*>(usart_address);
  irs::clock_enable(reinterpret_cast<size_t>(m_usart));

  if (a_tx != PNONE) {
    irs::clock_enable(a_tx);
  }
  if (a_rx != PNONE) {
    irs::clock_enable(a_rx);
  }

  if (a_rx != PNONE) {
    gpio_moder_alternate_function_enable(a_rx);
    gpio_usart_alternate_function_select(a_rx, a_com_index);
  }
  if (a_tx != PNONE) {
    gpio_moder_alternate_function_enable(a_tx);
    gpio_usart_alternate_function_select(a_tx, a_com_index);
  }

  connect_event(a_com_index);
  if (a_rx != PNONE) {
    USART_ITConfig(m_usart_typedef, USART_IT_RXNE, ENABLE);
  }
  if (a_tx != PNONE) {
    USART_ITConfig(m_usart_typedef, USART_IT_TXE, ENABLE);
  }

  set_usart_options(a_com_index);
  if (a_rx != PNONE) {
    m_usart->USART_CR1_bit.RE = 1;
  }
  if (a_tx != PNONE) {
    m_usart->USART_CR1_bit.TE = 1; // 1: Transmitter is enabled
  }
}

void irs::hardflow::arm::st_com_flow_t::set_usart_options(int a_com_index)
{
  m_usart->USART_CR1_bit.UE = 1;
  #ifdef IRS_STM32F7xx
  m_usart->USART_CR1_bit.M0 = 0; // 8 Data bits
  #else // F2 F4
  m_usart->USART_CR1_bit.M = 0; // 8 Data bits
  #endif // F2 F4
  m_usart->USART_CR1_bit.PCE = 0; // Parity control disabled
  m_usart->USART_CR1_bit.PS = 0; // Even parity
  m_usart->USART_CR2_bit.STOP = 0; // 1 stop bit
  m_usart->USART_CR1_bit.OVER8 = 0;
  volatile irs::cpu_traits_t::frequency_type periphery_frequency =
    irs::cpu_traits_t::periphery_frequency_first();
  if ((a_com_index == 1) || (a_com_index == 6)) {
    periphery_frequency = irs::cpu_traits_t::periphery_frequency_second();
  }
  m_usart->USART_BRR_bit.DIV_Mantissa = periphery_frequency/(16*m_baud_rate);
  m_usart->USART_BRR_bit.DIV_Fraction =
    16*(periphery_frequency%(16*m_baud_rate))/(16*m_baud_rate);
}

void irs::hardflow::arm::st_com_flow_t::connect_event(int a_com_index)
{
  switch (a_com_index) {
    case 1: {
      irs::interrupt_array()->int_gen_events(irs::arm::usart1_int)->
        push_back(&m_event);
    } break;
    case 2: {
      irs::interrupt_array()->int_gen_events(irs::arm::usart2_int)->
        push_back(&m_event);
    } break;
    case 3: {
      irs::interrupt_array()->int_gen_events(irs::arm::usart3_int)->
        push_back(&m_event);
    } break;
    case 4: {
      irs::interrupt_array()->int_gen_events(irs::arm::usart4_int)->
        push_back(&m_event);
    } break;
    case 5: {
      irs::interrupt_array()->int_gen_events(irs::arm::usart5_int)->
        push_back(&m_event);
    } break;
    case 6: {
      irs::interrupt_array()->int_gen_events(irs::arm::usart6_int)->
        push_back(&m_event);
    } break;
  }
}

irs::hardflow::arm::st_com_flow_t::~st_com_flow_t()
{
}

irs::hardflow::arm::st_com_flow_t::string_type
irs::hardflow::arm::st_com_flow_t::param(const string_type& /*a_name*/)
{
  return string_type();
}

void irs::hardflow::arm::st_com_flow_t::set_param(const string_type& /*a_name*/,
  const string_type& /*a_value*/)
{
}

irs::hardflow::arm::st_com_flow_t::size_type
irs::hardflow::arm::st_com_flow_t::read(
  size_type a_channel_ident, irs_u8 *ap_buf, size_type a_size)
{
  if (a_channel_ident != m_channel_id) {
    return 0;
  }
  if (m_rx == PNONE) {
    return 0;
  }
  USART_ITConfig(m_usart_typedef, USART_IT_RXNE, DISABLE);
  if (m_inbuf.empty()) {
    USART_ITConfig(m_usart_typedef, USART_IT_RXNE, ENABLE);
    return 0;
  }
  const size_type size = min(m_inbuf.size(), a_size);
  m_inbuf.copy_to(0, size, ap_buf);
  m_inbuf.pop_front(size);
  USART_ITConfig(m_usart_typedef, USART_IT_RXNE, ENABLE);
  return size;
}

irs::hardflow::arm::st_com_flow_t::size_type
irs::hardflow::arm::st_com_flow_t::write(
  size_type a_channel_ident, const irs_u8 *ap_buf,
  size_type a_size)
{
  if (a_channel_ident != m_channel_id) {
    return 0;
  }
  if (m_tx == PNONE) {
    return 0;
  }
  USART_ITConfig(m_usart_typedef, USART_IT_TXE, DISABLE);
  if (m_outbuf.size() >= m_outbuf_max_size) {
    USART_ITConfig(m_usart_typedef, USART_IT_TXE, ENABLE);
    return 0;
  }
  const size_type size = min(a_size, m_outbuf_max_size - m_outbuf.size());
  m_outbuf.push_back(ap_buf, ap_buf + size);
  USART_ITConfig(m_usart_typedef, USART_IT_TXE, ENABLE);
  return size;
}

irs::hardflow::arm::st_com_flow_t::size_type
irs::hardflow::arm::st_com_flow_t::channel_next()
{
  return m_channel_id;
}

bool irs::hardflow::arm::st_com_flow_t::is_channel_exists(
  size_type a_channel_ident)
{
  return (m_channel_id == a_channel_ident);
}

void irs::hardflow::arm::st_com_flow_t::tick()
{
}

bool irs::hardflow::arm::st_com_flow_t::check_input_buffer_overflow()
{
  bool status = m_inbuf_overflow;
  m_inbuf_overflow = false;
  return status;
}

irs::hardflow::arm::st_com_flow_t::usart_event_t::usart_event_t(
  st_com_flow_t* ap_st_com_flow
):
  mp_st_com_flow(ap_st_com_flow)
{
}

void irs::hardflow::arm::st_com_flow_t::usart_event_t::exec()
{
  USART_TypeDef* usart_typedef = mp_st_com_flow->m_usart_typedef;
  // Прерывание по приему данных
  if (USART_GetITStatus(usart_typedef, USART_IT_RXNE) == SET) {
    // Проверяем нет ли ошибок
    if ((usart_typedef->SR &
        (USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE|USART_FLAG_ORE)) == 0) {
      irs_u8 data = static_cast<irs_u8>(
        (USART_ReceiveData(usart_typedef)& 0xFF));
      if (mp_st_com_flow->m_inbuf.size() < mp_st_com_flow->m_inbuf_max_size) {
        mp_st_com_flow->m_inbuf.push_back(data);
      } else {
        mp_st_com_flow->m_inbuf_overflow = 1;
      }
    } else {
      USART_ReceiveData(usart_typedef);
      mp_st_com_flow->m_inbuf_overflow = 1;
    }
  }

  // Прерывание по переполнению буфера
  if (USART_GetITStatus(usart_typedef, USART_IT_ORE_RX) == SET) {
    USART_ReceiveData(usart_typedef);
    mp_st_com_flow->m_inbuf_overflow = 1;
  }

  // Прерывание, когда данные переданы
  if(USART_GetITStatus(usart_typedef, USART_IT_TXE) == SET) {
    if (!mp_st_com_flow->m_outbuf.empty()) {
      irs_u8 data = mp_st_com_flow->m_outbuf.front();
      mp_st_com_flow->m_outbuf.pop_front();
      USART_SendData(usart_typedef, data);
    } else {
      USART_ITConfig(usart_typedef, USART_IT_TXE, DISABLE);
    }
  }
}
#endif // IRS_STM32F_2_AND_4
#endif // USE_STDPERIPH_DRIVER

// class echo_t
irs::hardflow::echo_t::echo_t(size_type a_channel_count,
  size_type a_channel_buf_max_size):
  m_channels(a_channel_count),
  m_buf_max_size(a_channel_buf_max_size),
  m_channel(invalid_channel)
{
  if (!m_channels.empty()) {
    m_channel = 0;
  }
}

irs::hardflow::echo_t::string_type
irs::hardflow::echo_t::param(const string_type& /*a_name*/)
{
  return string_type();
}

void irs::hardflow::echo_t::set_param(const string_type& /*a_name*/,
  const string_type& /*a_value*/)
{
}

irs::hardflow::echo_t::size_type
irs::hardflow::echo_t::read(size_type a_channel_ident, irs_u8 *ap_buf,
  size_type a_size)
{
  size_type count = 0;
  if (a_channel_ident < m_channels.size()) {
    channel_buf_type& channel_buf = m_channels[a_channel_ident];
    count = min(channel_buf.size(), a_size);
    memcpyex(ap_buf, vector_data(channel_buf), count);
    channel_buf.erase(channel_buf.begin(), channel_buf.begin() + count);
  }
  return count;
}

irs::hardflow::echo_t::size_type
irs::hardflow::echo_t::write(size_type a_channel_ident, const irs_u8 *ap_buf,
  size_type a_size)
{
  size_type count = 0;
  if (a_channel_ident < m_channels.size()) {
    channel_buf_type& channel_buf = m_channels[a_channel_ident];
    count = min(m_buf_max_size - channel_buf.size(), a_size);
    const size_type pos = channel_buf.size();
    channel_buf.resize(channel_buf.size() + count);
    memcpyex(vector_data(channel_buf, pos), ap_buf, count);
  }
  return count;
}

irs::hardflow::echo_t::size_type irs::hardflow::echo_t::channel_next()
{
  if (m_channels.empty()) {
    return invalid_channel;
  }
  m_channel++;
  if (m_channel >= m_channels.size()) {
    m_channel = 0;
  }
  return m_channel;
}

bool irs::hardflow::echo_t::is_channel_exists(size_type a_channel_ident)
{
  return a_channel_ident < m_channels.size();
}

void irs::hardflow::echo_t::tick()
{
}

irs::hardflow::echo_t::size_type
irs::hardflow::echo_t::channel_data_size(size_type a_channel_ident)
{
  if (a_channel_ident < m_channels.size()) {
    return m_channels[a_channel_ident].size();
  } else {
    return 0;
  }
}

irs_status_t irs::to_irs_status(hardflow::fixed_flow_t::status_t a_status)
{
  switch (a_status) {
    case hardflow::fixed_flow_t::status_wait: {
      return irs_st_busy;
    }
    case hardflow::fixed_flow_t::status_success: {
      return irs_st_ready;
    }
    case hardflow::fixed_flow_t::status_error: {
      return irs_st_error;
    }
    default: {
      IRS_LIB_ASSERT_MSG("Неучтенный статус");
    }
  }
  return irs_st_error;
}

#ifndef __ICCAVR__
// class memory_flow_t
irs::hardflow::memory_flow_t::memory_flow_t(
  size_type a_channel_receive_buffer_max_size
):
  m_channels(),
  m_channel_buffers(),
  m_receive_buf_max_size(a_channel_receive_buffer_max_size),
  m_channel(m_channel_buffers.end())
{
}

irs::hardflow::memory_flow_t::~memory_flow_t()
{
  multimap<size_type, channel_t>::iterator it = m_channels.begin();
  while (it != m_channels.end()) {
    size_type local_channel_ident = it->first;
    size_type remote_channel_ident = it->second.remote_channel_ident;
    memory_flow_t* memory_flow = it->second.memory_flow;
    ++it;
    delete_connection(memory_flow, local_channel_ident, remote_channel_ident);
  }
}

irs::hardflow::memory_flow_t::string_type
irs::hardflow::memory_flow_t::param(const string_type& /*a_name*/)
{
  return string_type();
}

void irs::hardflow::memory_flow_t::set_param(const string_type& /*a_name*/,
  const string_type& /*a_value*/)
{
}

irs::hardflow::memory_flow_t::size_type
irs::hardflow::memory_flow_t::read(size_type a_channel_ident, irs_u8* ap_buf,
  size_type a_size)
{
  map<size_type, vector<irs_u8> >::iterator it =
    m_channel_buffers.find(a_channel_ident);
  if (it != m_channel_buffers.end()) {
    const size_type count = min(a_size, it->second.size());
    irs::memcpyex(ap_buf, vector_data(it->second), count);
    it->second.erase(it->second.begin(), it->second.begin() + count);
    return count;
  }
  return 0;
}

irs::hardflow::memory_flow_t::size_type
irs::hardflow::memory_flow_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  pair<multimap<size_type, channel_t>::iterator,
    multimap<size_type, channel_t>::iterator> ret =
    m_channels.equal_range(a_channel_ident);
  multimap<size_type, channel_t>::iterator it = ret.first;

  if (ret.first != ret.second) {
    size_type count = a_size;
    while (it != ret.second) {
      size_type channel_ident = it->second.remote_channel_ident;
      map<size_type, vector<irs_u8> >::const_iterator buffer_it =
        it->second.memory_flow->m_channel_buffers.find(channel_ident);
      const size_type buffer_size = buffer_it->second.size();
      count = min(count, it->second.memory_flow->m_receive_buf_max_size -
        buffer_size);
      ++it;
    }
    it = ret.first;
    while (it != ret.second) {
      memory_flow_t* memory_flow = it->second.memory_flow;
      size_type channel_ident = it->second.remote_channel_ident;
      #ifdef IRS_LIB_DEBUG
      size_type write_count =
      #endif //IRS_LIB_DEBUG
        memory_flow->write_to_local_buffer(channel_ident, ap_buf, count);
      IRS_LIB_ASSERT(write_count == count);
      ++it;
    }
    return count;
  }
  return 0;
}

irs::hardflow::memory_flow_t::size_type
irs::hardflow::memory_flow_t::write_to_local_buffer(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  map<size_type, vector<irs_u8> >::iterator it =
    m_channel_buffers.find(a_channel_ident);
  if (it != m_channel_buffers.end()) {
    IRS_LIB_ASSERT(it->second.size() <= m_receive_buf_max_size);
    const size_type count = min(a_size,
      it->second.size() - m_receive_buf_max_size);
    const size_type pos = it->second.size();
    it->second.resize(it->second.size() + count);
    irs::memcpyex(vector_data(it->second, pos), ap_buf, count);
    return count;
  }
  return 0;
}

irs::hardflow::memory_flow_t::size_type
irs::hardflow::memory_flow_t::channel_next()
{
  if (m_channel != m_channel_buffers.end()) {
    ++m_channel;
  }
  if (m_channel == m_channel_buffers.end()) {
    m_channel = m_channel_buffers.begin();
  }
  if (m_channel != m_channel_buffers.end()) {
    return m_channel->first;
  }
  return invalid_channel;
}

bool irs::hardflow::memory_flow_t::is_channel_exists(size_type a_channel_ident)
{
  return m_channels.find(a_channel_ident) != m_channels.end();
}

void irs::hardflow::memory_flow_t::tick()
{
}

void irs::hardflow::memory_flow_t::add_connection(
  memory_flow_t* ap_memory_flow, size_type a_local_channel_ident,
  size_type a_remote_channel_ident)
{
  IRS_LIB_ASSERT(a_local_channel_ident != invalid_channel);
  IRS_LIB_ASSERT(a_remote_channel_ident != invalid_channel);
  add_part_connection(ap_memory_flow, a_local_channel_ident,
    a_remote_channel_ident);
  ap_memory_flow->add_part_connection(this, a_remote_channel_ident,
    a_local_channel_ident);
}

void irs::hardflow::memory_flow_t::add_part_connection(
  memory_flow_t* ap_memory_flow,
  size_type a_local_channel_ident, size_type a_remote_channel_ident)
{
  multimap<size_type, channel_t>::iterator it =
    find_channel(ap_memory_flow, a_local_channel_ident,
    a_remote_channel_ident);
  if (it == m_channels.end()) {
    channel_t channel(ap_memory_flow, a_remote_channel_ident);
    m_channels.insert(make_pair(a_local_channel_ident, channel));
    m_channel_buffers.insert(make_pair(a_local_channel_ident,
      vector<irs_u8>()));
  }
}

irs::hardflow::memory_flow_t::channels_type::iterator
irs::hardflow::memory_flow_t::find_channel(
  memory_flow_t* ap_memory_flow, size_type a_local_channel_ident,
  size_type a_remote_channel_ident)
{
  pair<multimap<size_type, channel_t>::iterator,
    multimap<size_type, channel_t>::iterator> ret =
    m_channels.equal_range(a_local_channel_ident);
  channel_t channel(ap_memory_flow, a_remote_channel_ident);
  multimap<size_type, channel_t>::iterator it = irs::find_value(
    ret.first, ret.second, channel);
  return it;
}

void irs::hardflow::memory_flow_t::delete_connection(
  memory_flow_t* ap_memory_flow, size_type a_local_channel_ident,
  size_type a_remote_channel_ident)
{
  delete_part_connection(ap_memory_flow, a_local_channel_ident,
    a_remote_channel_ident);
  ap_memory_flow->delete_part_connection(this, a_remote_channel_ident,
    a_local_channel_ident);
}

void irs::hardflow::memory_flow_t::delete_part_connection(
  memory_flow_t* ap_memory_flow, size_type a_local_channel_ident,
  size_type a_remote_channel_ident)
{
  multimap<size_type, channel_t>::iterator it =
    find_channel(ap_memory_flow, a_local_channel_ident, a_remote_channel_ident);
  if (it != m_channels.end()) {
    m_channels.erase(it);
    if (m_channels.find(a_local_channel_ident) == m_channels.end()) {
      map<size_type, vector<irs_u8> >::iterator buffer_it =
        m_channel_buffers.find(a_local_channel_ident);
      if (m_channel == buffer_it) {
        ++m_channel;
      }
      m_channel_buffers.erase(buffer_it);
    }
  }
}

irs::hardflow::memory_flow_t::size_type
irs::hardflow::memory_flow_t::received_data_size(size_type a_channel_ident)
{
  map<size_type, vector<irs_u8> >::const_iterator it =
    m_channel_buffers.find(a_channel_ident);
  if (it != m_channel_buffers.end()) {
    return it->second.size();
  }
  return 0;
}
#endif //__ICCAVR__
