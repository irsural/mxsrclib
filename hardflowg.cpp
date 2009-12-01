// ���������������� ������
// ���� ��������: 8.09.2009
// ���� ���������� ���������: 25.11.2009

#include <hardflowg.h>
#include <irscpp.h>
#include <mxdata.h>

//#define IRS_LIB_SOCK_DEBUG
#define IRS_LIB_SOCK_DEBUG_BASE

#ifdef IRS_LIB_SOCK_DEBUG
#define IRS_TCP_DBG_MSG(msg) {msg}
#else
#define IRS_TCP_DBG_MSG(msg) ;
#endif

#ifdef IRS_LIB_SOCK_DEBUG_BASE
#define IRS_TCP_DBG_MSG_BASE(msg) {msg}
#else
#define IRS_TCP_DBG_MSG_BASE(msg) ;
#endif

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
  // ����, ���� �� ��� ����� ����� � ������
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
            // ������������ ������ ���������� � ����
          }
        }
      } break;
      case mode_limited_vector: {
        IRS_ASSERT(m_id_list.size() < m_max_size);
        if (m_id_list.size() < m_max_size+1) {
          allow_add = true;
        } else {
          // ��������� ����� ���������� �������
        }
      } break;
      case mode_unlimited_vector: {
        allow_add = true;
      } break;
      default : {
        IRS_ASSERT_MSG("����������� ������� �������� ������");
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
      // ���������� �� ���������
    }
  } else {
    // ������� ��� ����������� � ������
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
        // �������� �������� �� ���������
      }
    } break;
    case mode_limited_vector: {
      if (m_id_list.size() > m_max_size) {
        size_type adress_count_need_delete = m_id_list.size() - m_max_size;
        for (size_type id_i = 0; id_i < adress_count_need_delete; id_i++) {
          erase(m_id_list.back());
        }
      } else {
        // �������� �������� �� ���������
      }
    } break;
    case mode_unlimited_vector: {
      // �������� �� ���������
    } break;
    default : {
      IRS_ASSERT_MSG("����������� ��� �������� ������");
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
    size_type remote_host_id = 0;
    mesh_point_list.insert(remote_host_addr, &remote_host_id, &insert_success);
    IRS_LIB_ASSERT(insert_success);
  } else {
    // ������ �� ����� ��� ����� �����������
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
    // ����� �� ������
  }
}

void irs::hardflow::udp_flow_t::start()
{
  if (!m_state_info.lib_socket_loaded) {
    #if defined(IRS_WIN32)
    // ��������� ���������� ������� ������ 2.2
    WORD version_requested = 0;
    IRS_LOBYTE(version_requested) = 2;
    IRS_HIBYTE(version_requested) = 2;
    if (WSAStartup(version_requested, &m_wsd) != 0) {
      // ������ ��� �������� ����������
      SEND_MESSAGE_ERR(m_error_sock.get_last_error());
    } else {
      // ���������� ������ ���������
      m_state_info.lib_socket_loaded = true;
      m_state_info.sock_created = false;
      IRS_LIB_SOCK_DBG_MSG("���������� ������� ������ ���������");
    }
    #elif defined(IRS_LINUX)
    m_state_info.lib_socket_loaded = true;
    #endif // IRS_WINDOWS IRS_LINUX
    m_state_info.sock_created = false;
  } else {
    // ���������� ��� ���������
  }
  if (m_state_info.lib_socket_loaded) {
    if (!m_state_info.sock_created) {
      m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (m_sock != static_cast<socket_type>(m_invalid_socket)) {
        m_state_info.sock_created = true;
        m_state_info.set_io_mode_sock_success = false;
        // ��������� ����������, ����������� ������������ ������ ���������
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
      } else {
        // ����� ������� �� �������
        SEND_MESSAGE_ERR(m_error_sock.get_last_error());
      }
    } else {
      // ����� ��� ������
    }
  } else {
    // ���������� ������� �� ���������
  }
  if (m_state_info.sock_created) {
    if (!m_state_info.set_io_mode_sock_success) {
      // ��� ��������� �������������� ������ ���������� ulblock ������ �����
      // ��������� ��������
      unsigned long ulblock = 1;
      #if defined(IRS_WIN32)
      if (ioctlsocket(m_sock, FIONBIO, &ulblock) == m_socket_error) {
        // ������� ����������� ��������
        SEND_MESSAGE_ERR(m_error_sock.get_last_error());
      } else {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.bind_sock_and_ladr_success = false;
      }
      #elif defined(IRS_LINUX)
      if (fcntl(m_sock, F_SETFL, O_NONBLOCK) == m_socket_error) {
        // ������� ����������� ��������
        SEND_MESSAGE_ERR(m_error_sock.get_last_error());
      } else {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.bind_sock_and_ladr_success = false;
      }
      #endif // IRS_WINDOWS IRS_LINUX             
    } else {
      // ����� ��� ��������� � ������������� �����
    }
  } else {
    // ����� �� ������
  }
  if (m_state_info.set_io_mode_sock_success) {
    if (m_state_info.bind_sock_and_ladr_success) {
      sockaddr_in local_addr;
      bool init_success = true;
      local_addr_init(&local_addr, &init_success);
      if (init_success) {
        m_state_info.bind_sock_and_ladr_success = true;
        if (bind(m_sock, (sockaddr *)&local_addr,
          sizeof(local_addr)) == m_socket_error)
        {
          SEND_MESSAGE_ERR(m_error_sock.get_last_error());
          // ������ ��� ���������� ������ � ��������� �������
        } else {
          m_state_info.bind_sock_and_ladr_success = true;
        }
      } else {
        // ������ �������������
      }                                        
    } else {
      // ����� ��� �������� � ���������� ������
    }
  } else {
    // ������� ������ � ������������� ����� ���������� ��������
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
        // ������ ��������� ������
        *ap_init_success = false;
      } else {
        // ����� ������ ����� � ������
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
      // ������ ��������� ������
      *ap_init_success = false;
    } else {
      // ����� ������ ����� � ������
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

irs::hardflow::udp_flow_t::size_type irs::hardflow::udp_flow_t::read(
  size_type a_channel_ident, irs_u8 *ap_buf, size_type a_size)
{ 
  size_type size_rd = 0;
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
            size_rd = static_cast<size_type>(ret);
            IRS_LIB_SOCK_DBG_MSG("��������� " <<
              static_cast<string_type>(size_rd) << " ����");
          } else {
            // ���������� ���������
            IRS_LIB_SOCK_DBG_MSG("���������� � ������ ������� ���������");
            IRS_LIB_SOCK_DBG_MSG("� ������ " <<
              static_cast<string_type>(mesh_point_list.size()) << "�������");
          }
        } else {
          SEND_MESSAGE_ERR(m_error_sock.get_last_error());
        }
      } else {
        // ��� ������� ��� ������
      }
    } else if (ready_sock_count > 0) {
      SEND_MESSAGE_ERR(m_error_sock.get_last_error());

    } else {
      // ����� �� ����� ��� ������
    }    
  } else {
    // ������ �� ���������
  }
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
    if (mesh_point_list.adress_get(a_channel_ident, &remote_host_adr)) {
      size_type msg_size = min(a_size, m_send_msg_max_size);
      const int ret = sendto(m_sock, reinterpret_cast<const char*>(ap_buf),
        msg_size, 0, reinterpret_cast<sockaddr*>(&remote_host_adr),
          sizeof(remote_host_adr));
      if (ret != m_socket_error) {
        size_wr = static_cast<size_type>(ret);
        IRS_LIB_SOCK_DBG_MSG("�������� " <<
          static_cast<string_type>(size_wr) << " ����");
      } else {
        SEND_MESSAGE_ERR(m_error_sock.get_last_error());
      }
    } else {
      // ������ � ����� ��������������� ���������� � ������
      IRS_LIB_SOCK_DBG_MSG("����������� �������������");
    }
  } else {
    // ������ �� ���������
  }
  return size_wr;
}

irs::hardflow::udp_flow_t::size_type irs::hardflow::udp_flow_t::channel_next()
{
  return invalid_channel;
}

bool irs::hardflow::udp_flow_t::is_channel_exists(size_type a_channel_ident)
{
  return false;
}

void irs::hardflow::udp_flow_t::tick()
{
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    // ������ ������� �������
  } else {
    start();
  }
}

#endif //defined(IRS_WIN32) || defined(IRS_LINUX)

#if defined(IRS_LINUX)

irs::hardflow::tcp_server_t::tcp_server_t(
  irs_u16 local_port
):
  m_addr(zero_struct_t<sockaddr_in>::get()),
  m_serv_select_timeout(zero_struct_t<timeval>::get()),
  m_read_fds(zero_struct_t<fd_set>::get()),
  m_write_fds(zero_struct_t<fd_set>::get()),
  m_is_open(true),
  m_local_port(local_port),
  m_server_sock(0),
  m_map_channel_sock(),
  mp_map_channel_sock_it(m_map_channel_sock.begin()),
  m_channel(invalid_channel)
{
  start_server();
}

irs::hardflow::tcp_server_t::~tcp_server_t()
{
  stop_server();
}

void irs::hardflow::tcp_server_t::start_server()
{
  m_server_sock = socket(PF_INET, SOCK_STREAM, 0);
  m_is_open = true;
  if(m_server_sock >= 0) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(m_local_port);
    m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(m_server_sock, (struct sockaddr *)&m_addr,
      sizeof(m_addr)) >= 0) {
      int queue_lenght = 50; //����� �������
      listen(m_server_sock, queue_lenght);
    } else {
      IRS_TCP_DBG_MSG(perror("bind"););
      m_is_open = false;
    }
  } else {
    IRS_TCP_DBG_MSG(perror("socket"););
    m_is_open = false;
  }
}

irs::hardflow::tcp_server_t::channel_type 
  irs::hardflow::tcp_server_t::channel_next()
{
  channel_type channel = 0;
  if (!m_map_channel_sock.empty()) {
    mp_map_channel_sock_it++;
    if(mp_map_channel_sock_it == m_map_channel_sock.end())
    {
      mp_map_channel_sock_it = m_map_channel_sock.begin();
      IRS_TCP_DBG_MSG(cout << "Reach the end" << endl;);
      IRS_TCP_DBG_MSG(cout << "Go to first socket" << endl;);
    }
    channel = mp_map_channel_sock_it->first;
  } else {
    channel = invalid_channel;
  }
  return channel;
}

bool irs::hardflow::tcp_server_t::is_channel_exists(channel_type a_channel_ident)
{
  return m_map_channel_sock.find(a_channel_ident) != m_map_channel_sock.end();
}

void irs::hardflow::tcp_server_t::new_channel()
{ 
  m_channel++;
  if(m_channel == invalid_channel) {
    m_channel++;
  }
  map<channel_type, int>::iterator it_prev = m_map_channel_sock.find(m_channel);
  map<channel_type, int>::iterator it_cur = it_prev;
  //map<channel_type, int>::iterator it_check = it_prev;
  int defense_cnt = static_cast<int>(m_map_channel_sock.size());
  if(it_cur != m_map_channel_sock.end()) {
    for(;;) {
      it_cur++;
      defense_cnt--;
      if(defense_cnt <= 0) {
        //��� ��������� ���� ��� ����� �����
        IRS_TCP_DBG_MSG_BASE(cout << "No place for add new channel" << endl;);
        m_channel = invalid_channel;
        break;
      }
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
}

irs::hardflow::tcp_server_t::size_type 
  irs::hardflow::tcp_server_t::read(channel_type a_channel_ident,
  irs_u8 *ap_buf, size_type a_size)
{
  size_type rd_data_size = 0;
  if (m_is_open && is_channel_exists(a_channel_ident)) {
    int sock_rd = m_map_channel_sock[a_channel_ident];
    FD_SET(sock_rd, &m_read_fds);
    int sock_ready = select(sock_rd + 1, &m_read_fds,
      NULL, NULL, &m_serv_select_timeout);
    if(sock_ready > 0) {
      if(FD_ISSET(sock_rd, &m_read_fds)) {
        IRS_TCP_DBG_MSG(cout << "recv start" << endl;);
        int server_read = recv(sock_rd, reinterpret_cast<char*>(ap_buf),
          a_size, 0);
        if(server_read > 0){
          rd_data_size = 
            static_cast<irs::hardflow::tcp_client_t::size_type>(server_read);
        }
        else {
          if(server_read < 0) {
            IRS_TCP_DBG_MSG(perror("recv"););
          }
          m_map_channel_sock.erase(a_channel_ident);
          mp_map_channel_sock_it = m_map_channel_sock.begin();
          #ifdef IRS_LIB_SOCK_DEBUG_BASE
          cout << "Delete channel by read command" << endl;
          cout << "-------------------------------" << endl;
          for(map<channel_type, int>::iterator it = m_map_channel_sock.begin();
            it != m_map_channel_sock.end(); it++) {
            cout << "m_channel: " << (int)it->first << 
              " socket: " << it->second << endl;
          }
          cout << "-------------------------------" << endl;
          #endif //IRS_LIB_SOCK_DEBUG

          FD_CLR(sock_rd, &m_read_fds);
          FD_CLR(sock_rd, &m_write_fds);
          IRS_TCP_DBG_MSG_BASE(cout << "FD_CLR" << endl;);
          close(sock_rd);
        }
      }
    } else if(sock_ready < 0) {
      IRS_TCP_DBG_MSG(perror("select");)
      IRS_TCP_DBG_MSG_BASE(cout << "read" << endl;);
      FD_CLR(sock_rd, &m_read_fds);
      FD_CLR(sock_rd, &m_write_fds);
      IRS_TCP_DBG_MSG(cout << "FD_CLR" << endl;);
      close(sock_rd);
    }
    else {
      // ���� select ������� 0, �� ������� �� ���������
    }
    FD_CLR(sock_rd, &m_read_fds);
  }
  return rd_data_size;
}

irs::hardflow::tcp_server_t::size_type 
  irs::hardflow::tcp_server_t::write(channel_type a_channel_ident, 
  const irs_u8 *ap_buf, size_type a_size)  
{
  size_type wr_data_size = 0;
  if (m_is_open && is_channel_exists(a_channel_ident)) {
    int sock_wr = m_map_channel_sock[a_channel_ident]; 
    FD_SET(sock_wr, &m_write_fds); 
    int sock_ready = select(/*m_max_fd + 1*/sock_wr + 1, NULL,
      &m_write_fds, NULL, &m_serv_select_timeout);
    if(sock_ready > 0) {
      if(FD_ISSET(sock_wr, &m_write_fds)) {
        IRS_TCP_DBG_MSG(cout << "send start" << endl;);
        int server_write = send(sock_wr, ap_buf, a_size, 0);
        if(server_write > 0){
          wr_data_size = 
            static_cast<irs::hardflow::tcp_client_t::size_type>(server_write);
        }
        else{
          if(server_write < 0) {
            IRS_TCP_DBG_MSG(perror("send");); 
          }
          m_map_channel_sock.erase(a_channel_ident);
          mp_map_channel_sock_it = m_map_channel_sock.begin();
          #ifdef IRS_LIB_SOCK_DEBUG_BASE
          cout << "Delete channel by write command" << endl;
          cout << "-------------------------------" << endl;
          for(map<channel_type, int>::iterator it = m_map_channel_sock.begin();
            it != m_map_channel_sock.end(); it++) {
            cout << "m_channel: " << (int)it->first << 
              " socket: " << it->second << endl;
          }
          cout << "-------------------------------" << endl;
          #endif //IRS_LIB_SOCK_DEBUG

          IRS_TCP_DBG_MSG_BASE(cout << "FD_CLR" << endl;);
          close(sock_wr);
        }
      }
    } else if(sock_ready < 0) {
      IRS_TCP_DBG_MSG(perror("select"););
      IRS_TCP_DBG_MSG_BASE(cout << "write" << endl;);
      FD_CLR(sock_wr, &m_read_fds);
      FD_CLR(sock_wr, &m_write_fds);
      close(sock_wr);
    } 
    else {
      // ���� select ������� 0, �� ������� �� ���������
    }
    FD_CLR(sock_wr, &m_write_fds);
  }
  return wr_data_size;
}

void irs::hardflow::tcp_server_t::tick()
{
  if(m_is_open) {
    FD_SET(m_server_sock, &m_read_fds);
    //FD_SET(m_server_sock, &m_write_fds);
    /*
    int max_map_elem = 0;
    for(map<size_type, int>::iterator it = m_map_channel_sock.begin();
      it != m_map_channel_sock.end(); it++) {
      max_map_elem = max(max_map_elem, it->second);
    }
    //m_max_fd = max(m_server_sock, max_map_elem);*/
    int se_select = select(m_server_sock + 1, &m_read_fds,
      NULL, NULL, &m_serv_select_timeout);
    if(se_select > 0) {
      if(FD_ISSET(m_server_sock, &m_read_fds)) {
        //sockaddr_in sender_addr;
        //socklen_t sender_addr_len = sizeof(sender_addr);
        int new_sock = accept(m_server_sock, NULL, NULL);
          //(struct sockaddr *)&sender_addr, &sender_addr_len);
        if(new_sock >= 0) {
          /*if((m_channel == invalid_channel) || 
            (m_map_channel_sock.find(m_channel) != m_map_channel_sock.end())) 
          {
            m_channel++;
          }*/
          new_channel();
          if (m_channel != invalid_channel) {
            pair<map<channel_type, int>::iterator, bool> insert_channel =
              m_map_channel_sock.insert(make_pair(m_channel, new_sock));
            #ifdef IRS_LIB_SOCK_DEBUG_BASE
            if(insert_channel.second) {
              cout << "New channel added" << endl;
              cout << "-------------------------------" << endl;
              for(map<channel_type, int>::iterator it = m_map_channel_sock.begin();
                it != m_map_channel_sock.end(); it++) 
              {
                cout << "m_channel: " << (int)it->first << 
                  " socket: " << it->second << endl;
              }
              cout << "-------------------------------" << endl;
            }
            else {
              IRS_TCP_DBG_MSG_BASE(cout << "Channel already exist" << endl;);
            }
            #endif //IRS_LIB_SOCK_DEBUG
          }
          else {
            IRS_TCP_DBG_MSG_BASE(cout << "Invalid channel" << endl;);
            close(new_sock);
          }
        } else {
          IRS_TCP_DBG_MSG(perror("accept"););
        }
      }
    } else if (se_select < 0) {
      IRS_TCP_DBG_MSG(perror("select"););
      IRS_TCP_DBG_MSG(cout << "tick()" << endl;);
    } else {
      // ���� select ������� 0, �� ������� �� ���������
    }
    FD_CLR(m_server_sock, &m_read_fds);
  } else {
    start_server();
  }
}

void irs::hardflow::tcp_server_t::stop_server()
{
  close(m_server_sock);
  tcp_close_t close_op;
  for_each(m_map_channel_sock.begin(), m_map_channel_sock.end(), close_op);
  #ifdef NOP
  for(map<channel_type, int>::iterator it = m_map_channel_sock.begin();
    it != m_map_channel_sock.end(); it++)
  {
    close(it->second);
  }
  #endif //NOP
  m_map_channel_sock.clear();
  m_is_open = false;
}

irs::string irs::hardflow::tcp_server_t::param(const irs::string &a_name)
{
  irs::string param;
  return param;
}
void irs::hardflow::tcp_server_t::set_param(const irs::string &a_name,
  const irs::string &a_value)
{

}

irs::hardflow::tcp_client_t::tcp_client_t(
  mxip_t dest_ip,
  irs_u16 dest_port
):
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
  start_client();
}

irs::hardflow::tcp_client_t::~tcp_client_t()
{
  stop_client();
}

void irs::hardflow::tcp_client_t::start_client()
{ 
  m_client_sock = socket(PF_INET, SOCK_STREAM, 0);
  m_is_open = true;
  if(m_client_sock >= 0) {
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(m_dest_port);
    m_addr.sin_addr.s_addr = reinterpret_cast<unsigned long&>(m_dest_ip);
    if(connect(m_client_sock, (struct sockaddr *)&m_addr, 
      sizeof(m_addr)) == 0) 
    {
      IRS_TCP_DBG_MSG_BASE(cout << "Start client" << endl;);
    }
    else {
      IRS_TCP_DBG_MSG(perror("connect"););
      close(m_client_sock);
      m_is_open = false;
    }
  } else {
    IRS_TCP_DBG_MSG(perror("socket"););
    m_is_open = false;
  }
}

void irs::hardflow::tcp_client_t::stop_client()
{
  close(m_client_sock);
  m_is_open = false;
  FD_ZERO(&m_read_fds);
  FD_ZERO(&m_write_fds);
  IRS_TCP_DBG_MSG_BASE(cout << "Close client" << endl;);
}

irs::hardflow::tcp_client_t::channel_type 
  irs::hardflow::tcp_client_t::channel_next()
{
  channel_type channel = 1;
  return channel;
}

bool irs::hardflow::tcp_client_t::is_channel_exists(channel_type a_channel_ident)
{
  return true;
}


irs::hardflow::tcp_client_t::size_type 
  irs::hardflow::tcp_client_t::read(channel_type a_channel_ident,
  irs_u8 *ap_buf, size_type a_size)
{
  size_type rd_data_size = 0;
  if (m_is_open) {
    a_channel_ident = m_channel;
    FD_SET(m_client_sock, &m_read_fds);
    int sock_ready = select(m_client_sock + 1, &m_read_fds, 
      NULL, NULL, &m_client_select_timeout);
    if(sock_ready > 0) {
      if(FD_ISSET(m_client_sock, &m_read_fds)) {
        int client_read = recv(m_client_sock, reinterpret_cast<char*>(ap_buf),
          a_size, 0);
        if(client_read > 0) {
          rd_data_size = 
            static_cast<size_type>(client_read);
        } else {
          if(client_read < 0) {
            IRS_TCP_DBG_MSG(perror("recv"););
          }
          stop_client();
        }
      }
    } else if(sock_ready < 0) {
      IRS_TCP_DBG_MSG(perror("select"););
      return 0;
    }
    else {
      // ���� select ������� 0, �� ������� �� ���������
    }
    FD_CLR(m_client_sock,&m_read_fds);
  }
  return rd_data_size;
}

irs::hardflow::tcp_client_t::size_type 
  irs::hardflow::tcp_client_t::write(channel_type /*a_channel_ident*/, 
  const irs_u8 *ap_buf, size_type a_size)  
{
  size_type wr_data_size = 0;
  if (m_is_open) {
    int sock_wr = m_client_sock;
    FD_SET(sock_wr, &m_write_fds);
    int sock_ready = select(sock_wr + 1, NULL,
      &m_write_fds, NULL, &m_client_select_timeout);
    if(sock_ready >= 0) {
      if(FD_ISSET(sock_wr, &m_write_fds)) {
        int client_write = send(sock_wr, ap_buf, a_size, 0);
        if(client_write > 0){
          wr_data_size =
            static_cast<size_type>(client_write);
        }
        else{
          IRS_TCP_DBG_MSG(perror("send"););
          stop_client();
        }
      }
    } else if(sock_ready < 0) {
      IRS_TCP_DBG_MSG(perror("select"););
      return 0;
    }
    else {
      // ���� select ������� 0, �� ������� �� ���������
    }
    FD_CLR(sock_wr, &m_write_fds);
  }
  return wr_data_size;
}

irs::string irs::hardflow::tcp_client_t::param(const irs::string &a_name)
{
  irs::string param;
  return param;
}

void irs::hardflow::tcp_client_t::set_param(const irs::string &a_name,
  const irs::string &a_value)
{

}

void irs::hardflow::tcp_client_t::tick()
{
  if(!m_is_open) {
    start_client();
  }
}

irs::hardflow::fixed_flow_t::fixed_flow_t(
  hardflow_t* ap_hardflow
):
  mp_hardflow(ap_hardflow),
  m_read_channel(0),
  mp_read_buf_cur(IRS_NULL),
  m_read_size(0),
  m_size_read(0),
  m_read_status(status_success),
  m_read_size_cur(0),
  m_read_timeout(make_cnt_s(2)),
  m_write_channel(0),
  mp_write_buf_cur(IRS_NULL),
  m_write_size(0),
  m_size_write(0),
  m_write_status(status_success),
  m_write_size_cur(0),
  m_write_timeout(make_cnt_s(2))
{
}

void irs::hardflow::fixed_flow_t::connect(hardflow_t* ap_hardflow) 
{
  mp_hardflow = ap_hardflow;
}

void irs::hardflow::fixed_flow_t::read(channel_type a_channel_ident, 
  irs_u8* ap_buf, size_type a_size)
{
  if (read_status() != status_wait) {
    m_read_status = status_wait;
    m_read_channel = a_channel_ident;
    mp_read_buf_cur = ap_buf;
    mp_read_buf = ap_buf;
    m_read_size = a_size;
    m_read_size_cur = a_size;
  }
}

irs::hardflow::fixed_flow_t::status_t
  irs::hardflow::fixed_flow_t::read_status()
{
  return m_read_status;
}

irs::hardflow::fixed_flow_t::size_type 
  irs::hardflow::fixed_flow_t::read_abort()
{
  size_type m_size_retr = m_read_size - m_read_size_cur;
  m_read_status = status_success;
  mp_read_buf_cur = IRS_NULL;
  m_read_size_cur = 0;
  return m_size_retr;
}

void irs::hardflow::fixed_flow_t::write(channel_type a_channel_ident, 
  const irs_u8 *ap_buf, size_type a_size)
{
  if (write_status() != status_wait) {
    m_write_status = status_wait;
    m_write_channel = a_channel_ident;
    mp_write_buf_cur = ap_buf;
    mp_write_buf = ap_buf;
    m_write_size = a_size;
    m_write_size_cur = a_size;
  }
}

irs::hardflow::fixed_flow_t::status_t 
  irs::hardflow::fixed_flow_t::write_status()
{
  return m_write_status;
}

irs::hardflow::fixed_flow_t::size_type
  irs::hardflow::fixed_flow_t::write_abort()
{
  size_type m_size_rec = m_write_size - m_write_size_cur;
  m_write_status = status_success;
  mp_write_buf_cur = IRS_NULL;
  m_write_size_cur = 0;
  return m_size_rec;
}

void irs::hardflow::fixed_flow_t::tick()
{
  if(read_status() == status_wait) {
    m_size_read = mp_hardflow->read(m_read_channel, mp_read_buf_cur,
      m_read_size_cur);
    if (m_size_read == 0) {
      if (m_read_timeout.stopped()) {
        m_read_timeout.start();
      }
    } else {
      m_read_timeout.stop();
    }
    if (!m_read_timeout.check()) {
      IRS_TCP_DBG_MSG(cout << "m_size_read = " << m_size_read << endl;);
      if(mp_hardflow->is_channel_exists(m_read_channel)) {
        mp_read_buf_cur += m_size_read;
        m_read_size_cur -= m_size_read;
        if(mp_read_buf_cur >= (mp_read_buf + m_read_size)) {
          IRS_TCP_DBG_MSG(cout << "read end" << endl;);
          m_read_status = status_success;
        }
      }
      else {
        m_read_status = status_error;
        IRS_TCP_DBG_MSG(cout << "read_abort" << endl;);
      }
    } else {
      m_read_status = status_error;
      IRS_TCP_DBG_MSG(cout << "read_abort" << endl;);
    }
  }
  if(write_status() == status_wait) {
    m_size_write = mp_hardflow->write(m_write_channel, mp_write_buf_cur,
      m_write_size_cur);
    if(m_size_write == 0) {
      if(m_write_timeout.stopped()) {
        IRS_TCP_DBG_MSG_BASE(cout << "write: stopped()" << endl;);
        m_write_timeout.start();
      }
    } else {
      IRS_TCP_DBG_MSG_BASE(cout << "stop()" << endl;);
      m_write_timeout.stop();
    }
    if(!m_write_timeout.check()) {
      IRS_TCP_DBG_MSG(cout << "m_size_write = " << m_size_write << endl;);
      if(mp_hardflow->is_channel_exists(m_write_channel)) {
        mp_write_buf_cur += m_size_write;
        m_write_size_cur -= m_size_write;
        if(mp_write_buf_cur >= (mp_write_buf + m_write_size) ) {
          IRS_TCP_DBG_MSG(cout << "write end" << endl;);
          m_write_status = status_success;
        }
      }
      else {
        IRS_TCP_DBG_MSG_BASE(cout << "channel not exists" << endl;);
        m_write_status = status_error;
        IRS_TCP_DBG_MSG(cout << "write_abort" << endl;);
      }
    } else {
      IRS_TCP_DBG_MSG_BASE(cout << "write: check()" << endl;);
      m_write_status = status_error;
      IRS_TCP_DBG_MSG(cout << "write_abort" << endl;);
    }
  }
}

#endif // defined(IRS_LINUX)
