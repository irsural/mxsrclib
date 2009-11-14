// Коммуникационные потоки
// Дата: 27.08.2009

#ifndef hardflowgH
#define hardflowgH

#include <irsdefs.h>

// Standart C++ headers
#include <vector>
#include <string.h>

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

// mxsrclib headers
#include <irsstd.h>
#include <timer.h>
#include <irserror.h>

// Для вывода отладочных сообщений
#define IRS_LIB_SOCK_DEBUG

#ifdef IRS_LIB_SOCK_DEBUG
#define IRS_LIB_SOCK_DBG_MSG(msg) IRS_LIB_DBG_MSG(msg)
#else
#define IRS_LIB_SOCK_DBG_MSG(msg)
#endif

/*#ifdef IRS_LIB_DEBUG
#define SEND_MESSAGE(msg) IRS_LIB_ASSERT_EX(false, msg)
#else // IRS_LIB_DEBUG
#define SEND_MESSAGE(msg)
#endif // IRS_LIB_DEBUG
*/
namespace irs {

class hardflow_t {
public:
  virtual ~hardflow_t() {}
  virtual irs::string param(const irs::string &a_name) = 0;
  virtual void set_param(const irs::string &a_name,
    const irs::string &a_value) = 0;
  virtual irs_uarc read(irs_uarc &a_channel_ident, irs_u8 *ap_buf,
    irs_uarc a_size) = 0;
  virtual irs_uarc write(irs_uarc a_channel_ident, const irs_u8 *ap_buf,
    irs_uarc a_size) = 0;
  virtual void tick() = 0;
};

namespace hardflow {

typedef size_t sizens_t;
typedef irs::string string_t;
#if defined(IRS_WIN32) || defined(IRS_LINUX)
#if defined(IRS_WIN32)
inline void send_format_msg(
   int a_error_code,
   const char* ap_file,
   int a_line)
{
  LPVOID lpMsgBuf;
  //LPVOID lpDisplayBuf;
  FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      a_error_code,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,
      0, NULL);
  irs::string message = static_cast<irs::string>(a_error_code)+
    ": "+static_cast<char*>(lpMsgBuf);
  irs::error_trans()->throw_error(
    ec_standard, ap_file, a_line, (void*)(message.c_str()));
  LocalFree(lpMsgBuf);
}
#elif defined(IRS_LINUX)
inline void send_format_msg(
   int a_error_code,
   const char* ap_file,
   int a_line)
{
  char* errmsg = strerror(a_error_code);
  irs::string message = static_cast<irs::string>(a_error_code)+
    ": "+static_cast<char*>(errmsg);
   irs::error_trans()->throw_error(
    ec_standard, ap_file, a_line, (void*)(message.c_str()));
}
#endif //__GNUC__

#ifdef IRS_LIB_DEBUG
#define SEND_MESSAGE_ERR(error_code)\
  send_format_msg(error_code,__FILE__,__LINE__)
#else // IRS_LIB_DEBUG
#define SEND_MESSAGE_ERR(error_code)
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

class host_list_t
{
public:
  class less_t;
  typedef sizens_t size_type;
  typedef string_t string_type;
  typedef sockaddr_in adress_type;
  typedef size_type id_type;
  typedef map<id_type, adress_type> map_id_adress_type;
  typedef map<adress_type, id_type, less_t> map_adress_id_type;
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
      a_first_adr.sin_addr;
      bool first_less_second = false;
      //if (a_first_adr.sin_addr.S_un.S_addr <
        //a_second_adr.sin_addr.S_un.S_addr)
      if (extract_ip(a_first_adr) < extract_ip(a_second_adr))
      {
        first_less_second = true;
      //} else if (a_first_adr.sin_addr.S_un.S_addr ==
        //a_second_adr.sin_addr.S_un.S_addr)
      } else if (extract_ip(a_first_adr) == extract_ip(a_second_adr)) {
        first_less_second = (a_first_adr.sin_port < a_second_adr.sin_port);
      } else {
        // Первый больше второго
      }
      return first_less_second;
    };
  };
  enum mode_t {
    invalid_mode,             // Недопустимый режим
    mode_queue,               // Учитывается переменная m_max_size
    mode_limited_vector,      // Учитывается переменная m_max_size
    mode_unlimited_vector};   // Переменная m_max_size не учитывается  
  host_list_t(mode_t a_mode = mode_queue, size_type a_max_size = 1000);
  bool id_get(adress_type a_adress, id_type* ap_id);
  bool adress_get(id_type a_id, adress_type* ap_adress);
  // Возвращает false, если достигнут лимит, и true, если адрес успешно
  // добавлен в список или такой адрес уже есть
  void insert(adress_type a_adress, id_type* ap_id, bool* ap_insert_success);
  void erase(const id_type a_id);
  void clear();
  size_type size();
  void mode_set(const mode_t a_mode);
  size_type max_size_get();
  void max_size_set(size_type a_max_size);
private:
  mode_t m_mode;
  size_type m_max_size;
  size_type m_counter;
  map_id_adress_type m_map_id_adress;
  map_adress_id_type m_map_adress_id;
  queue_id_type m_id_list;
};

class udp_flow_t : public hardflow_t
{
public:
  typedef size_t size_type;
  typedef irs::string string_type;
  typedef int errcode_type;
  #if defined(IRS_WIN32)
  typedef SOCKET socket_type;
  typedef int socklen_type;
  #elif defined(IRS_LINUX)
  typedef int socket_type;
  typedef socklen_t socklen_type;
  #endif // IRS_WINDOWS IRS_LINUX
private:
  #if defined(IRS_WIN32)
  enum { m_socket_error = SOCKET_ERROR };
  enum { m_invalid_socket = INVALID_SOCKET };
  #elif defined(IRS_LINUX)
  enum { m_socket_error = -1 };
  enum { m_invalid_socket = -1 };
  #endif // IRS_WINDOWS IRS_LINUX
  /*#ifdef __BORLANDC__
  error_trans_base_t *mp_error_trans;
  #endif // __BORLANDC__*/
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
  host_list_t mesh_point_list;
  //size_type m_recv_msg_max_size;     
public:
  udp_flow_t(
    const string_type& a_local_host_name = "",
    const string_type& a_local_host_port = "",
    const string_type& a_remote_host_name = "",
    const string_type& a_remote_host_port = "");
  virtual ~udp_flow_t();
private:
  void start();
  void sock_close();
  void local_addr_init(sockaddr_in* ap_sockaddr, bool* ap_init_success);
  void addr_init(const string_type& a_remote_host_name,
    const string_type& a_remote_host_port,
    sockaddr_in* ap_sockaddr,
    bool* ap_init_success);
public:
  virtual irs::string param(const irs::string &a_name);
  virtual void set_param(const irs::string &a_name,
    const irs::string &a_value);
  // В случае если функция read возвращает irs_uarc == 0, буфер ap_buf может
  // быть испорчен
  virtual irs_uarc read(irs_uarc &a_channel_ident, irs_u8 *ap_buf,
    irs_uarc a_size);
  virtual irs_uarc write(irs_uarc a_channel_ident, const irs_u8 *ap_buf,
    irs_uarc a_size);
  virtual void tick();
};

#endif //defined(IRS_WIN32) || defined(IRS_LINUX)

} // namespace hardflow

} // namespace irs

#endif //hardflowgH
