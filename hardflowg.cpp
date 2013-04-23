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
  m_func_select_timeout.tv_sec = 0;
  m_func_select_timeout.tv_usec = 0;

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


void irs::hardflow::udp_flow_t::start()
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
          //int so = 1;
          //setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST,
          //  reinterpret_cast<char*>(&so), sizeof(so));
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
        // Ошибка получения списка адресов
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
  if (a_param_name == irst("locale_adress")) {
    param_value = m_local_host_name;
  } else if (a_param_name == irst("locale_port")) {
    param_value = m_local_host_port;
  } else if (a_param_name == irst("remote_adress")) {
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
        } else if (func_name == irst("remote_adress")) {
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
  if (a_param_name == irst("remote_adress")) {
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
    memset(&remote_host_adr, sizeof(sockaddr_in), 0);
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
  } else if (a_param_name == irst("remote_adress")) {
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
  } else if (a_param_name == irst("remote_adress")) {
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
      mxmac_t new_mac = mxmac_t::zero_mac();
      string_type mac_str = irst("0.0.") + a_value;
      cstr_to_mxmac(new_mac, IRS_SIMPLE_FROM_TYPE_STR(mac_str.c_str()));
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
  m_init_success(false),
  m_init_command(),
  m_init_count(0),
  m_init_channel_ident(0),
  m_transmit_data()
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
  m_init_command.push_back(irst("++addr ") + irs::string_t(a_address));
  m_init_command.push_back(irst("++auto 0"));
  m_init_command.push_back(irst("++mode 1"));
  m_init_command.push_back(irst("++eoi 1"));
  int timeout = bound(a_timeout_read_ms, 1, 3000);
  m_init_command.push_back(irst("++read_tmo_ms ") +
      irs::string_t(timeout));
  m_init_command.push_back(irst("++eos ") + irs::string_t(a_read_end_line));
  //m_init_command.push_back(irst("++eos 1"));
  m_init_command.push_back(irst("++eot_enable 0"));
  m_init_command.push_back(irst("++eot_char 0"));

  m_init_count = 0;
  m_init_mode = mode_start;
  m_init_channel_ident = channel_next();
}
irs::hardflow::prologix_flow_t::~prologix_flow_t()
{
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
      m_buffer = irst("++read eoi") + m_end_line_write;
      m_read_data = u8_from_str(m_buffer);
      m_channel_ident = a_channel_ident;
      m_read_mode = mode_start_read;
      m_is_read_wait = true;
    }
    read_size = 0;
  } else {
    read_size = min(a_size, m_read_data.size());
    irs::c_array_view_t<irs_u8> array_view(ap_buf, read_size);
    irs::mem_copy(m_read_data, 0, array_view, 0, read_size);
    m_is_read_wait = false;
    m_is_read = false;
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
        m_fixed_flow.write(m_channel_ident, m_read_data.data(),
          m_read_data.size());
        m_read_mode = mode_start_read_wait;
      } break;
      case mode_start_read_wait: {
        switch (m_fixed_flow.write_status()) {
          case irs::hardflow::fixed_flow_t::status_success: {
            m_read_mode = mode_read;
          } break;
          case irs::hardflow::fixed_flow_t::status_error: {
            m_read_mode = mode_start_read;
          } break;
          default: {
          } break;
        }
      } break;
      case mode_read: {
        m_read_data.clear();
        m_read_data.resize(m_read_chunk_size);
        m_read_mode = mode_read_wait;
      } break;
      case mode_read_wait: {
        irs_u8* p_data = m_read_data.data() + m_read_data.size() -
          m_read_chunk_size;
        size_t cur_read_size = mp_hardflow->read(
          m_channel_ident, p_data, m_read_chunk_size);
        m_read_data.resize(m_read_data.size() + cur_read_size);
        irs::raw_data_t<irs_u8> transmit_data_copy = m_read_data;
        transmit_data_copy.resize(m_read_data.size() - m_read_chunk_size);
        irs::string_t read_str = str_from_u8(transmit_data_copy);
        size_t pos_end_line = read_str.find(m_end_line_read);
        if (pos_end_line != irs::string_t::npos) {
          m_read_string = read_str.substr(0,
            pos_end_line + m_end_line_read.size());
          m_read_data = u8_from_str(m_read_string);
          m_read_mode = mode_free;
          m_is_read = true;
        } else {
          // Если не нашли конец строки повторяем чтение
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
    count = std::min(channel_buf.size(), a_size);
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
    count = std::min(m_buf_max_size - channel_buf.size(), a_size);
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
