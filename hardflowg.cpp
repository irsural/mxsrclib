// Коммуникационные потоки
// Дата: 8.09.2009

#include <hardflowg.h>

#if defined(IRS_WIN32) //|| defined(IRS_LINUX)

//class host_list_t
irs::hardflow::host_list_t::host_list_t(
  mode_t a_mode,
  size_type a_max_size
):
  m_mode(a_mode),
  m_max_size(a_max_size),
  m_counter(0),
  m_map_id_adress(),
  m_map_adress_id(),
  m_id_list()
{
}

void irs::hardflow::host_list_t::insert(adress_type a_adress,
  id_type* ap_id, bool* ap_insert_success)
{
  *ap_insert_success = false;
  // Ищем, есть ли уже такой адрес в списке
  map_adress_id_type::iterator it_map_addr_id =
    m_map_adress_id.end();
  it_map_addr_id = m_map_adress_id.find(a_adress);
  if (it_map_addr_id == m_map_adress_id.end()) {
    bool allow_add = false;
    switch (m_mode) {
      case mode_queue: {
        IRS_ASSERT(m_id_list.size() < m_max_size);
        if (m_id_list.size() < m_max_size+1) {
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
      case mode_limited_vector: {
        IRS_ASSERT(m_id_list.size() < m_max_size);
        if (m_id_list.size() < m_max_size+1) {
          allow_add = true;
        } else {
          // Достигнут лимит количества записей
        }
      } break;
      case mode_unlimited_vector: {
        allow_add = true;
      } break;
      default : {
        IRS_ASSERT_MSG("Неизвестный вариант рабочего режима");
      }
    }
    if (allow_add) {
      m_map_id_adress.insert(make_pair(m_counter, a_adress));
      m_map_adress_id.insert(make_pair(a_adress, m_counter));
      m_id_list.push_back(m_counter);
      *ap_id = m_counter;
      m_counter++;
      *ap_insert_success = true;
    } else {
      // Добавление не разрешено
    }
  } else {
    // Элемент уже присутсвует в списке
    *ap_id = it_map_addr_id->second;
    *ap_insert_success = true;
  }
}

bool irs::hardflow::host_list_t::id_get(adress_type a_adress,
  id_type* ap_id)
{
  bool find_success = false;
  map_adress_id_type::iterator it_map_addr_id = m_map_adress_id.end();
  it_map_addr_id = m_map_adress_id.find(a_adress);
  if (it_map_addr_id != m_map_adress_id.end()) {
    *ap_id = it_map_addr_id->second;
    find_success = true;
  } else {
    find_success = false;
  }
  return find_success;
}

bool irs::hardflow::host_list_t::adress_get(id_type a_id,
  adress_type* ap_adress)
{
  bool find_success = true;
  map_id_adress_type::iterator it_map_id_addr = m_map_id_adress.end();
  it_map_id_addr = m_map_id_adress.find(a_id);
  if (it_map_id_addr != m_map_id_adress.end()) {
    *ap_adress = it_map_id_addr->second;
    find_success = true;
  } else {
    find_success = false;
  }
  return find_success;
}

void irs::hardflow::host_list_t::erase(id_type a_id)
{
  map_id_adress_type::iterator it_map_id_adress =
    m_map_id_adress.find(a_id);
  map_adress_id_type::iterator it_map_adress_id =
    m_map_adress_id.find(it_map_id_adress->second);
  m_map_id_adress.erase(it_map_id_adress);
  m_map_adress_id.erase(it_map_adress_id);
  queue_id_type::iterator it_id =
    find(m_id_list.begin(), m_id_list.end(), a_id);
  m_id_list.erase(it_id);
}

void irs::hardflow::host_list_t::clear()
{
  m_map_id_adress.clear();
  m_map_adress_id.clear();
  m_id_list.clear();
}

irs::hardflow::host_list_t::size_type
irs::hardflow::host_list_t::size()
{
  return m_id_list.size();
}

void irs::hardflow::host_list_t::mode_set(const mode_t a_mode)
{
  m_mode = a_mode;
  switch (m_mode) {
    case mode_queue: {
      if (m_id_list.size() > m_max_size) {
        size_type adress_count_need_delete = m_id_list.size() - m_max_size;
        for (size_type id_i = 0; id_i < adress_count_need_delete; id_i++) {
          erase(m_id_list.front());
        }
      } else {
        // Удаление объектов не требуется
      }
    } break;
    case mode_limited_vector: {
      if (m_id_list.size() > m_max_size) {
        size_type adress_count_need_delete = m_id_list.size() - m_max_size;
        for (size_type id_i = 0; id_i < adress_count_need_delete; id_i++) {
          erase(m_id_list.back());
        }
      } else {
        // Удаление объектов не требуется
      }
    } break;
    case mode_unlimited_vector: {
      // Удаление не требуется
    } break;
    default : {
      IRS_ASSERT_MSG("Неизвестный тип рабочего режима");
    }
  }
}

irs::hardflow::host_list_t::size_type
irs::hardflow::host_list_t::max_size_get()
{
  return m_max_size;
}

void irs::hardflow::host_list_t::max_size_set(size_type a_max_size)
{
  m_max_size = a_max_size;
}

// class error_sock_t
irs::hardflow::error_sock_t::error_sock_t()
{ }

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
  const string_type& a_remote_host_port
):
  /*#ifdef __BORLANDC__
  mp_error_trans(irs::error_trans()),
  #endif // __BORLANDC__*/
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
  m_send_msg_max_size(1024),
  mesh_point_list()
  //m_recv_msg_max_size(1024)
{
  m_func_select_timeout.tv_sec = 0;
  m_func_select_timeout.tv_usec = 0;

  sockaddr_in remote_host_addr;
  bool init_success = true;
  addr_init(a_remote_host_name, a_remote_host_port,
    &remote_host_addr, &init_success);
  if (init_success) {
    bool insert_success = false;
    irs_uarc remote_host_id = 0;
    mesh_point_list.insert(remote_host_addr, &remote_host_id, &insert_success);
    IRS_LIB_ASSERT(insert_success);
  } else {
    // Адресс не задан или задан неправильно
  }
}

irs::hardflow::udp_flow_t::~udp_flow_t()
{
  if (m_state_info.sock_created) {    
    #if defined(IRS_WIN32)
    closesocket(m_sock);
    #elif defined(IRS_LINUX)
    close(m_sock);
    #endif // IRS_WINDOWS IRS_LINUX
  } else {
    // Сокет не создан
  }
}

void irs::hardflow::udp_flow_t::start()
{
  if (m_state_info.lib_socket_loaded) {
    // Библиотека уже загружена
  } else {
    #if defined(IRS_WIN32)
    // Загружаем библиотеку сокетов версии 2.2
    WORD version_requested = 0;
    IRS_LOBYTE(version_requested) = 2;
    IRS_HIBYTE(version_requested) = 2;
    if (WSAStartup(version_requested, &m_wsd) != 0) {
      // Ошибка при загрузке библиотеки
      SEND_MESSAGE_ERR(m_error_sock.get_last_error());
    } else {
      // библиотека удачно загружена
      m_state_info.lib_socket_loaded = true;
      m_state_info.sock_created = false;
      IRS_LIB_SOCK_DBG_MSG("Библиотека сокетов удачно загружена");
    }
    #elif defined(IRS_LINUX)
    m_state_info.lib_socket_loaded = true;
    #endif // IRS_WINDOWS IRS_LINUX
    m_state_info.sock_created = false;
  }
  if (m_state_info.lib_socket_loaded) {
    if (m_state_info.sock_created) {
      // Сокет уже создан
    } else {
      m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (m_sock == static_cast<socket_type>(m_invalid_socket)) {
        // Сокет создать не удалось
        SEND_MESSAGE_ERR(m_error_sock.get_last_error());
      } else {
        m_state_info.sock_created = true;
        m_state_info.set_io_mode_sock_success = false;
        // Обновляем переменную, указывающую максимальный размер сообщения
        int optval;
        socklen_type optval_size = sizeof(optval);
        int optname = SO_SNDBUF;
        int res = getsockopt(m_sock, SOL_SOCKET, optname,
          reinterpret_cast<char*>(&optval),  &optval_size);
        if (res == m_socket_error) {
          SEND_MESSAGE_ERR(m_error_sock.get_last_error());
        } else {
          m_send_msg_max_size = optval;
        }
        /*optval_size = sizeof(optval);
        optname = SO_RCVBUF;
        res = getsockopt(m_sock, SOL_SOCKET, optname,
          reinterpret_cast<char*>(&optval),  &optval_size);
        if (res == m_socket_error) {
          SEND_MESSAGE_ERR(m_error_sock.get_last_error());
        } else {
          m_recv_msg_max_size = optval;
        }*/
      }
    }
  } else {
    // Библиотека сокетов не загружена
  }
  if (m_state_info.sock_created) {
    if (m_state_info.set_io_mode_sock_success) {
      // Сокет уже переведен в неблокирующий режим
    } else {     
      // для включения неблокирующего режима переменная ulblock должна иметь
      // ненулевое значение
      unsigned long ulblock = 1;
      #if defined(IRS_WIN32)
      if (ioctlsocket(m_sock, FIONBIO, &ulblock) == m_socket_error) {
        // функция завершилась неудачей
        SEND_MESSAGE_ERR(m_error_sock.get_last_error());
      } else {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.bind_sock_and_ladr_success = false;
      }
      #elif defined(IRS_LINUX)
      if (fcntl(m_sock, F_SETFL, O_NONBLOCK) == m_socket_error) {
        // функция завершилась неудачей
        SEND_MESSAGE_ERR(m_error_sock.get_last_error());
      } else {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.bind_sock_and_ladr_success = false;
      }
      #endif // IRS_WINDOWS IRS_LINUX
    }
  } else {
    // Сокет не создан
  }
  if (m_state_info.set_io_mode_sock_success) {
    if (m_state_info.bind_sock_and_ladr_success) {
      // Сокет уже привязан к локальному адресу
    } else {
      sockaddr_in local_addr;
      bool init_success = true;
      local_addr_init(&local_addr, &init_success);
      if (init_success) { m_state_info.bind_sock_and_ladr_success = true;
        if (bind(m_sock, (sockaddr *)&local_addr,
          sizeof(local_addr)) == m_socket_error)
        {
          SEND_MESSAGE_ERR(m_error_sock.get_last_error());
          // Ошибка при связывании сокета с локальным адресом
        } else {
          m_state_info.bind_sock_and_ladr_success = true;
        }
      } else {
        // Ошибка инициализации
      }
    }
  } else {
    // Перевод сокета в неблокирующий режим завершился неудачей
  }
}

void irs::hardflow::udp_flow_t::sock_close()
{
  #if defined(IRS_WIN32)
  closesocket(m_sock);
  #elif defined(IRS_LINUX)
  close(m_sock);
  #endif // IRS_WINDOWS IRS_LINUX
  m_state_info.sock_created = false;
  m_state_info.set_io_mode_sock_success = false;
  m_state_info.bind_sock_and_ladr_success = false;
}

void irs::hardflow::udp_flow_t::local_addr_init(
  sockaddr_in* ap_sockaddr, bool* ap_init_success)
{
  *ap_init_success = true;
  if (m_local_host_name == "") {
    ap_sockaddr->sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    unsigned long addr = inet_addr(m_local_host_name.c_str());
    if (addr == INADDR_NONE) {
      hostent* p_host = gethostbyname(m_local_host_name.c_str());
      if (p_host == NULL) {
        // Ошибка получения адреса
        *ap_init_success = false;
      } else {
        // Берем первый адрес в списке
        memcpy(&ap_sockaddr->sin_addr,
          p_host->h_addr_list[0], p_host->h_length);
      }
    } else {
      ap_sockaddr->sin_addr.s_addr = addr;
    }
  }
  ap_sockaddr->sin_family = AF_INET;
  unsigned short port = 0;
  if (m_local_host_port.to_number(port)) {
    ap_sockaddr->sin_port = htons(port);
  } else {
    *ap_init_success = false;
  }  
}

void irs::hardflow::udp_flow_t::addr_init(
  const string_type& a_remote_host_name,
  const string_type& a_remote_host_port,
  sockaddr_in* ap_sockaddr,
  bool* ap_init_success)
{
  *ap_init_success = true;
  unsigned long addr = inet_addr(a_remote_host_name.c_str());
  if (addr == INADDR_NONE) {
    hostent* p_host = gethostbyname(a_remote_host_name.c_str());
    if (p_host == NULL) {
      // Ошибка получения адреса
      *ap_init_success = false;
    } else {
      // Берем первый адрес в списке
      memcpy(&(ap_sockaddr->sin_addr), p_host->h_addr_list[0],
        p_host->h_length);
    }
  } else {
    ap_sockaddr->sin_addr.s_addr = addr;
  }
  ap_sockaddr->sin_family = AF_INET;
  unsigned short port = 0;
  if (a_remote_host_port.to_number(port)) {
    ap_sockaddr->sin_port = htons(port);
  } else {
    *ap_init_success = false;
  }  
}

irs::string irs::hardflow::udp_flow_t::param(const irs::string &a_name)
{
  irs::string param;
  return param;
}

void irs::hardflow::udp_flow_t::set_param(const irs::string &a_name,
  const irs::string &a_value)
{
}

irs_uarc irs::hardflow::udp_flow_t::read(
  irs_uarc &a_channel_ident, irs_u8 *ap_buf, irs_uarc a_size)
{ 
  irs_uarc size_rd = 0;
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    FD_ZERO(&m_s_kit);
    FD_SET(m_sock, &m_s_kit);
    int ready_sock_count = select(NULL, &m_s_kit, NULL, NULL,
      &m_func_select_timeout);
    if (ready_sock_count != m_socket_error) {
      if (FD_ISSET(m_sock, &m_s_kit)) {
        sockaddr_in sender_addr;
        socklen_type sender_addr_size = sizeof(sender_addr);
        int ret = recvfrom(m_sock, reinterpret_cast<char*>(ap_buf), a_size, 0,
          (sockaddr*)&sender_addr, &sender_addr_size);
        if (ret != m_socket_error) {
          bool insert_success = false;
          mesh_point_list.insert(sender_addr, &a_channel_ident,
            &insert_success);
          if (insert_success) {
            size_rd = static_cast<irs_uarc>(ret);
            IRS_LIB_SOCK_DBG_MSG("Прочитано " <<
              static_cast<string_type>(size_rd) << " байт");
          } else {
            // Добавление отклонено
            IRS_LIB_SOCK_DBG_MSG("Добавление в список адресов отклонено");
            IRS_LIB_SOCK_DBG_MSG("В списке " <<
              static_cast<string_type>(mesh_point_list.size()) << "адресов");
          }
        } else {
          SEND_MESSAGE_ERR(m_error_sock.get_last_error());
        }
      } else {
        // Нет сокетов для чтения
      }
    } else if (ready_sock_count > 0) {
      SEND_MESSAGE_ERR(m_error_sock.get_last_error());

    } else {
      // Сокет не готов для чтения
    }    
  } else {
    // Запуск не произошел
  }
  return size_rd;
}

irs_uarc irs::hardflow::udp_flow_t::write(
  irs_uarc a_channel_ident, const irs_u8 *ap_buf, irs_uarc a_size)
{
  irs_uarc size_wr = 0;
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    sockaddr_in remote_host_adr;
    memset(&remote_host_adr, sizeof(sockaddr_in), 0);
    if (mesh_point_list.adress_get(a_channel_ident, &remote_host_adr)) {
      size_type msg_size = min(a_size, m_send_msg_max_size);
      const int ret = sendto(m_sock, reinterpret_cast<const char*>(ap_buf),
        msg_size, 0, reinterpret_cast<sockaddr*>(&remote_host_adr),
          sizeof(remote_host_adr));
      if (ret != m_socket_error) {
        size_wr = static_cast<irs_uarc>(ret);
        IRS_LIB_SOCK_DBG_MSG("Записано " <<
          static_cast<string_type>(size_wr) << " байт");
      } else {
        SEND_MESSAGE_ERR(m_error_sock.get_last_error());
      }
    } else {
      // Адресс с таким идентификатором отсутсвует в списке
      IRS_LIB_SOCK_DBG_MSG("Неизвестный идентификатор");
    }
  } else {
    // Запуск не произошел
  }
  return size_wr;
}

void irs::hardflow::udp_flow_t::tick()
{
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    // Клиент успешно запущен
  } else {
    start();
  }
}

#endif //defined(IRS_WIN32) || defined(IRS_LINUX)

