// Обработка ошибок
// Дата: 14.04.2010
// Ранняя дата: 4.08.2009

// Номер файла
#define IRSERRORCPP_IDX 5

#include <irserror.h>

namespace irs {

// Приемо-передатчик ошибок (Реализация)
class error_trans_t: public error_trans_base_t
{
public:
  error_trans_t();
  virtual ~error_trans_t();
  // Передать ошибку (тип ap_spec_data определяется a_error_code)
  virtual void throw_error(
    error_code_t a_error_code = ec_standard,
    cstr_type ap_file_name = 0,
    int a_line_number = 0,
    const void *ap_spec_data = 0
  );
  // Считать последний код ошибки
  virtual error_code_t error_code();
  // Считать имя файла в котором произошла последняя ошибка
  virtual cstr_type file_name();
  // Считать номер строки в файле в которой произошла ошибка
  virtual int line_number();
  // Считать специфические данные соответствующие коду ошибки
  virtual const void *spec_data();
  // Добавить обработчик ошибки
  virtual void add_handler(mxfact_event_t *ap_handler);
private:
  error_code_t m_error_code;
  cstr_type mp_file_name;
  int m_line_number;
  const void *mp_spec_data;
  mxfact_event_t *mp_handler;
};

} //namespace irs

// Приемо-передатчик ошибок (Реализация)
irs::error_trans_t::error_trans_t():
  m_error_code(ec_standard),
  mp_file_name(0),
  m_line_number(0),
  mp_spec_data(0),
  mp_handler(0)
{
}
irs::error_trans_t::~error_trans_t()
{
}
// Передать ошибку (тип ap_spec_data определяется a_error_code)
void irs::error_trans_t::throw_error(
  error_code_t a_error_code,
  cstr_type ap_file_name,
  int a_line_number,
  const void *ap_spec_data
)
{
  m_error_code = a_error_code;
  mp_file_name = ap_file_name;
  m_line_number = a_line_number;
  mp_spec_data = ap_spec_data;
  if (mp_handler) mp_handler->exec();
}
// Считать последний код ошибки
irs::error_code_t irs::error_trans_t::error_code()
{
  return m_error_code;
}
// Считать имя файла в котором произошла последняя ошибка
irs::error_trans_t::cstr_type irs::error_trans_t::file_name()
{
  return mp_file_name;
}
// Считать номер строки в файле в которой произошла ошибка
int irs::error_trans_t::line_number()
{
  return m_line_number;
}
// Считать специфические данные соответствующие коду ошибки
const void *irs::error_trans_t::spec_data()
{
  return mp_spec_data;
}
// Добавить обработчик ошибки
void irs::error_trans_t::add_handler(mxfact_event_t *ap_handler)
{
  ap_handler->connect(mp_handler);
}

// Приемо-передатчик ошибок (Считывание указателя на экземпляр)
irs::error_trans_base_t *irs::error_trans()
{
  static error_trans_t error_trans_i;
  return &error_trans_i; 
}

// Специфические данные для утвердений в error_trans_base_t
//  (Считывание указателя на экземпляр)
const void* irs::spec_assert(error_trans_base_t::cstr_type assert_str,
  error_trans_base_t::cstr_type message)
{
  static et_spec_assert_t spec_assert_i;
  spec_assert_i.assert_str = assert_str;
  spec_assert_i.message = message;
  return static_cast<const void*>(&spec_assert_i);
}

#if defined(IRS_WIN32) || defined(IRS_LINUX)

int irs::last_error_code()
{
  int error_code = 0;
  #if defined(IRS_WIN32)
  error_code = GetLastError();
  #elif defined(IRS_LINUX)
  error_code = errno;
  #endif // defined(IRS_LINUX)
  return error_code;
}

irs::string irs::last_error_str()
{
  return error_str(last_error_code());
}

irs::string irs::error_str(int a_error_code)
{
  irs::string message;
  #if defined(IRS_WIN32)
  LPVOID lpMsgBuf;
  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    a_error_code,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR) &lpMsgBuf,
    0,
    NULL);
  message = static_cast<char*>(lpMsgBuf);
  LocalFree(lpMsgBuf);
  #elif defined(IRS_LINUX)
  message = strerror(a_error_code);
  #endif // defined(IRS_LINUX)
  return message;
}

void irs::send_format_msg(
   int a_error_code,
   char* ap_file,
   int a_line)
{
  #if defined (IRS_WIN32)
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
  #elif defined(IRS_LINUX)
  char* errmsg = strerror(a_error_code);
  irs::string message = static_cast<irs::string>(a_error_code)+
    ": "+static_cast<char*>(errmsg);
   irs::error_trans()->throw_error(
    ec_standard, ap_file, a_line, (void*)(message.c_str()));
  #endif // defined(IRS_LINUX)
}

void irs::send_last_message_err(char* ap_file, int a_line)
{
  int error_code = 0;
  #if defined(IRS_WIN32)
  error_code = GetLastError();
  #elif defined(IRS_LINUX)
  error_code = errno;
  #endif // defined(IRS_LINUX)
  send_format_msg(error_code, ap_file, a_line);
}

#ifdef IRS_WIN32
void irs::send_wsa_last_message_err(char* ap_file, int a_line)
{
  int error_code = WSAGetLastError();
  send_format_msg(error_code, ap_file, a_line);
}
#endif // IRS_WIN32

void irs::send_message_err(int a_error_code, char* ap_file,
  int a_line)
{
  send_format_msg(a_error_code, ap_file, a_line);
}
#endif // defined(IRS_WIN32) || defined(IRS_LINUX)

ostream& irs::mlog()
{ 
  static irs::zerobuf buf;
  static ostream mlog_obj(&buf);
  return mlog_obj;
}

