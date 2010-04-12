// Коммуникационные потоки
// Дата: 09.04.2010
// Дата создания: 27.08.2009

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
#include <irssysutils.h>
#include <irscpp.h>
#include <irstcpip.h>
#include <irsnetdefs.h>

#ifdef IRS_LIB_HARDFLOWG_DEBUG_TYPE
# if (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_BASE)
#   define IRS_LIB_HARDFLOW_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
#   define IRS_LIB_HARDFLOW_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG(msg)
#   define IRS_LIB_HARDFLOW_DBG_MSG_SRC_BASE(msg) IRS_LIB_DBG_MSG_SRC(msg)
#   define IRS_LIB_HARDFLOW_DBG_RAW_MSG_DETAIL(msg)
#   define IRS_LIB_HARDFLOW_DBG_MSG_DETAIL(msg)
# elif (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
#   define IRS_LIB_HARDFLOW_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
#   define IRS_LIB_HARDFLOW_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG(msg)
#   define IRS_LIB_HARDFLOW_DBG_MSG_SRC_BASE(msg) IRS_LIB_DBG_MSG_SRC(msg)
#   define IRS_LIB_HARDFLOW_DBG_RAW_MSG_DETAIL(msg) IRS_LIB_DBG_RAW_MSG(msg)
#   define IRS_LIB_HARDFLOW_DBG_MSG_DETAIL(msg) IRS_LIB_DBG_MSG_SRC(msg)
# endif 
#else // IRS_LIB_HARDFLOWG_DEBUG_TYPE
# define IRS_LIB_HARDFLOW_DBG_RAW_MSG_BASE(msg)
# define IRS_LIB_HARDFLOW_DBG_MSG_BASE(msg)
# define IRS_LIB_HARDFLOW_DBG_MSG_SRC_BASE(msg)
# define IRS_LIB_HARDFLOW_DBG_RAW_MSG_DETAIL(msg)
# define IRS_LIB_HARDFLOW_DBG_MSG_DETAIL(msg)
# define IRS_LIB_HARDFLOW_DBG_MSG_SRC_DETAIL(msg)
#endif // IRS_LIB_HARDFLOWG_DEBUG_TYPE

namespace irs {

class hardflow_t {
public:
  typedef size_t size_type;
  enum {
    invalid_channel = 0
  };

  virtual ~hardflow_t() {}
  virtual irs::string param(const irs::string &a_name) = 0;
  virtual void set_param(const irs::string &a_name,
    const irs::string &a_value) = 0;
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size) = 0;
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size) = 0;
  virtual size_type channel_next() = 0;
  virtual bool is_channel_exists(size_type a_channel_ident) = 0;
  virtual void tick() = 0;
};

namespace hardflow {
typedef irs::string_t stringns_t;

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


class error_sock_t
{
public:
  #if defined(IRS_WIN32)
  enum error_t {
    eacess = WSAEACCES,                   // For Windows only
    eaddrenuse = WSAEADDRINUSE,
    eaddrnotavail = WSAEADDRNOTAVAIL,
    eafnosupport = WSAEAFNOSUPPORT,
    econnaborned = WSAECONNABORTED,
    econnrefused = WSAECONNREFUSED,
    econnreset = WSAECONNRESET,
    edestaddrreq = WSAEDESTADDRREQ,
    efault = WSAEFAULT,                   // For Windows only
    ehostdown = WSAEHOSTDOWN,
    ehostunread = WSAEHOSTUNREACH,
    einprogress = WSAEINPROGRESS,         // For Windows only
    eintr = WSAEINTR,                     // For Windows only
    eisconn = WSAEISCONN,
    einval = WSAEINVAL,                   // For Windows only
    emfile = WSAEMFILE,                   // For Windows only
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
    eproclim = WSAEPROCLIM,               // For Windows only
    eprotonosupport = WSAEPROTONOSUPPORT,
    eprototype = WSAEPROTOTYPE,
    esocktnosupport = WSAESOCKTNOSUPPORT,
    eshutdown = WSAESHUTDOWN,             // For Windows only
    etimedout = WSAETIMEDOUT,
    ewouldblock = WSAEWOULDBLOCK,
    notinitialised = WSANOTINITIALISED,   // For Windows only
    sysnotready = WSASYSNOTREADY,         // For Windows only
    vernotsupported = WSAVERNOTSUPPORTED  // For Windows only
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
  error_sock_t();
  errcode_type get_last_error();
};

#if defined(IRS_WIN32)
enum { socket_error = SOCKET_ERROR };
enum { invalid_socket = INVALID_SOCKET };
#elif defined(IRS_LINUX)
enum { socket_error = -1 };
enum { invalid_socket = -1 };
#endif // IRS_WINDOWS IRS_LINUX

#if defined(IRS_WIN32)
typedef SOCKET socketns_t;
#elif defined(IRS_LINUX)
typedef int socketns_t;
#endif // IRS_WINDOWS IRS_LINUX

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

enum udp_limit_connections_mode_t {
  udplc_mode_invalid,      // Недопустимый режим
  udplc_mode_queue,        // Учитывается переменная m_max_size
  udplc_mode_limited,      // Учитывается переменная m_max_size
  udplc_mode_unlimited     // Переменная m_max_size не учитывается*/
};

class udp_channel_list_t
{
public:
  class less_t;
  class udp_flow_t;
  typedef hardflow_t::size_type size_type;
  typedef string_t string_type;
  typedef sockaddr_in adress_type;
  typedef size_type id_type;
  typedef irs::deque_data_t<irs_u8> bufer_type;
  struct channel_t
  {
    adress_type adress;
    bufer_type bufer;
    measure_time_t lifetime;
    measure_time_t downtime;
  };
  typedef map<adress_type, id_type, less_t> map_adress_id_type;
  typedef map<id_type, channel_t> map_id_channel_type;
  typedef map<id_type, channel_t>::iterator map_id_channel_iterator;
  typedef map<id_type, channel_t>::const_iterator map_id_channel_const_iterator;   
  typedef deque<id_type> queue_id_type;
  class less_t
  {
  public:
    unsigned long extract_ip(const adress_type& a_address) const
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
    bool operator()(const adress_type& a_first_adr,
      const adress_type& a_second_adr) const
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
  };
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
  bool id_get(adress_type a_adress, id_type* ap_id);
  bool adress_get(id_type a_id, adress_type* ap_adress);
  // Возвращает false, если достигнут лимит, и true, если адрес успешно
  // добавлен в список или такой адрес уже есть
  void insert(adress_type a_adress, id_type* ap_id, bool* ap_insert_success);
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
  void channel_adress_get(const id_type a_channel_id,
    adress_type* ap_adress) ;
  void channel_adress_set(const id_type a_channel_id,
    const adress_type& a_adress);
  void clear();
  size_type size();
  void mode_set(const udp_limit_connections_mode_t a_mode);
  size_type max_size_get();
  void max_size_set(size_type a_max_size);
  size_type write(const adress_type& a_adress, const irs_u8 *ap_buf,
    size_type a_size);
  size_type read(size_type a_id, irs_u8 *ap_buf,
    size_type a_size);
  size_type channel_next();
  size_type cur_channel() const; 
  void tick();
private:    
  bool lifetime_exceeded(const map_id_channel_iterator a_it_cur_channel);
  bool downtime_exceeded(const map_id_channel_iterator a_it_cur_channel);
  void next_free_channel_id();
  udp_limit_connections_mode_t m_mode;
  size_type m_max_size;
  size_type m_channel_id;
  bool m_channel_id_overflow;
  const size_type m_channel_max_count;
  map_id_channel_type m_map_id_channel;
  map_adress_id_type m_map_adress_id;
  queue_id_type m_id_list;
  size_type m_buf_max_size;
  map_id_channel_iterator m_it_cur_channel;
  map_id_channel_iterator m_it_cur_channel_for_check;
  bool m_max_lifetime_enabled;
  bool m_max_downtime_enabled;
  counter_t m_max_lifetime;
  counter_t m_max_downtime;
};

class udp_flow_t: public hardflow_t
{
public:
  typedef hardflow_t::size_type size_type;
  typedef irs::string string_type;
  typedef udp_channel_list_t::adress_type adress_type;
  enum mode_t {
    invalid_mode,             // Недопустимый режим
    mode_queue,               // Учитывается переменная m_max_size
    mode_limited_vector,      // Учитывается переменная m_max_size
    mode_unlimited_vector};   // Переменная m_max_size не учитывается
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
private:
  #if defined(IRS_WIN32)
  enum { m_socket_error = SOCKET_ERROR };
  enum { m_invalid_socket = INVALID_SOCKET };
  #elif defined(IRS_LINUX)
  enum { m_socket_error = -1 };
  enum { m_invalid_socket = -1 };
  #endif // IRS_WINDOWS IRS_LINUX
  error_sock_t m_error_sock;
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
  } m_state_info;
  #if defined(IRS_WIN32)
  WSADATA m_wsd;
  #endif // IRS_WIN32
  socket_type m_sock;
  string_type m_local_host_name;
  string_type m_local_host_port;
  timeval m_func_select_timeout;
  fd_set m_s_kit;
  size_type m_send_msg_max_size;
  udp_channel_list_t m_channel_list;
  irs::raw_data_t<irs_u8> m_read_buf;
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
public:
  udp_flow_t(
    const string_type& a_local_host_name = "",
    const string_type& a_local_host_port = "",
    const string_type& a_remote_host_name = "",
    const string_type& a_remote_host_port = "",
    const udp_limit_connections_mode_t a_mode = udplc_mode_queue,
    const size_type a_channel_max_count = 1000,
    const size_type a_channel_buf_max_size = 0xFFFF,
    const bool a_limit_lifetime_enabled = false,
    const double a_max_lifetime_sec = 24*60*60,
    const bool a_limit_downtime_enabled = false,
    const double a_max_downtime_sec = 60*60
  );
  virtual ~udp_flow_t();
private:
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
  virtual irs::string param(const irs::string &a_param_name);
  virtual void set_param(const irs::string &a_param_name,
    const irs::string &a_value);
  // В случае если функция read возвращает size_type == 0, буфер ap_buf может
  // быть испорчен
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  virtual void tick();
};

#if defined(IRS_WIN32) || defined(IRS_LINUX)

class tcp_server_t: public hardflow_t
{
public:
  typedef hardflow_t::size_type size_type;
  typedef socketns_t socket_type;
  
  tcp_server_t(irs_u16 local_port);
  virtual ~tcp_server_t();
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual void tick();
  virtual irs::string param(const irs::string &a_name);
  virtual void set_param(const irs::string &a_name,
    const irs::string &a_value);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);

private:
  #if defined(IRS_WIN32)
  enum { m_socket_error = SOCKET_ERROR };
  enum { m_invalid_socket = INVALID_SOCKET };
  #elif defined(IRS_LINUX)
  enum { m_socket_error = -1 };
  enum { m_invalid_socket = -1 };
  #endif // IRS_WINDOWS IRS_LINUX
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

  void start_server();
  void stop_server();
  void new_channel();
};

class tcp_client_t: public hardflow_t
{
public:
  typedef hardflow_t::size_type size_type;
  typedef socketns_t socket_type;
  
  tcp_client_t (mxip_t dest_ip, irs_u16 dest_port);
  virtual ~tcp_client_t();
  virtual size_type read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  virtual size_type write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  virtual void tick();
  virtual irs::string param(const irs::string &a_name);
  virtual void set_param(const irs::string &a_name,
    const irs::string &a_value);
  virtual size_type channel_next();
  virtual bool is_channel_exists(size_type a_channel_ident);
  
private:
  typedef int errcode_type;
  
  #if defined(IRS_WIN32)
  enum { m_socket_error = SOCKET_ERROR };
  enum { m_invalid_socket = INVALID_SOCKET };
  #elif defined(IRS_LINUX)
  enum { m_socket_error = -1 };
  enum { m_invalid_socket = -1 };
  #endif // IRS_WINDOWS IRS_LINUX
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
  
  void start_client();
  void stop_client();
};

#endif //defined(IRS_WIN32) || defined(IRS_LINUX)

#endif //defined(IRS_WIN32) || defined(IRS_LINUX)

// Прием/передача фиксированных объемов данных
class fixed_flow_t
{
public:
  typedef hardflow_t::size_type size_type;

  enum status_t {
    status_wait,
    status_success,
    status_error
  };

  fixed_flow_t(hardflow_t* ap_hardflow = IRS_NULL);
  void read(size_type a_channel_ident, irs_u8 *ap_buf,
    size_type a_size);
  void write(size_type a_channel_ident, const irs_u8 *ap_buf,
    size_type a_size);
  status_t read_status();
  size_type read_abort();
  status_t write_status();
  size_type write_abort();
  void tick();
  void connect(hardflow_t* ap_hardflow);
  size_type read_byte_count() const;
  size_type write_byte_count() const;
  void read_timeout(double a_read_timeout_sec);
  double read_timeout() const;
  void write_timeout(double a_write_timeout_sec);
  double write_timeout() const;
private:
  hardflow_t* mp_hardflow;   
  size_type m_read_channel;
  irs_u8* mp_read_buf_cur;
  irs_u8* mp_read_buf;
  size_type m_read_size_need;
  size_type m_read_size_rest;
  status_t m_read_status;
  timer_t m_read_timeout;

  size_type m_write_channel;
  const irs_u8* mp_write_buf_cur;
  const irs_u8* mp_write_buf;
  size_type m_write_size_need;
  size_type m_write_size_rest;
  status_t  m_write_status;
  timer_t m_write_timeout;
};

class simple_udp_flow_t: public hardflow_t
{
public:
  typedef hardflow_t::size_type size_type;
  
  simple_udp_flow_t(
    simple_tcpip_t* ap_simple_udp,
    mxip_t a_local_ip,
    irs_u16 a_local_port,
    mxip_t a_dest_ip,
    irs_u16 a_dest_port,
    size_type a_channel_max_count = 3
  );
  virtual ~simple_udp_flow_t();
  virtual irs::string param(const irs::string &a_name);
  virtual void set_param(const irs::string &a_name,
    const irs::string &a_value);
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
  irs_u8* mp_send_buf;
  mxip_t m_cur_dest_ip;
  irs_u16 m_cur_dest_port;
  //counter_t m_max_channel_downtime;
  const size_type m_channel_max_count;
  deque<udp_channel_t> m_channel_list;
  deque<udp_channel_t>::iterator m_channel_list_it;
  size_type m_udp_max_data_size;
  
  void new_channel(mxip_t a_ip, irs_u16 a_port);
  void view_channel_list();
};

} // namespace hardflow

} // namespace irs

#endif //hardflowgH
