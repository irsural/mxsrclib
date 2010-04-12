// Обработка ошибок
// Дата: 29.03.2010
// Ранняя дата: 16.09.2009

#ifndef IRSERRORH
#define IRSERRORH

#include <irsdefs.h>

#include <irsconfig.h>
#include <irsdefs.h>
#include <irsint.h>
#include <irstime.h>
#include <irscpp.h>

#if defined(IRS_WIN32)
// Standart Windows headers
#include <winsock2.h>
#elif defined(IRS_LINUX)
// Standart Linux headers
#include <errno.h>
#endif // defined(IRS_LINUX)

#include <irsfinal.h>

#ifdef __ICCAVR__
#define irsm(cstr)\
  ""; { static char __flash dbg_msg_cstring[] = cstr;\
  irs::mlog() << dbg_msg_cstring; } irs::mlog()
#else //__ICCAVR__
#define irsm(cstr) cstr
#endif //__ICCAVR__

#define IRS_ASSERT_HELPER(error_code, assert_expr, message)\
  {\
    irs::error_trans()->throw_error(error_code, __FILE__, __LINE__,\
      irs::spec_assert(#assert_expr, message));\
  }

#define IRS_ASSERT(assert_expr)\
  {\
    if (!(assert_expr)) {\
      IRS_ASSERT_HELPER(irs::ec_assert, assert_expr, 0);\
    }\
  }
#define IRS_ASSERT_EX(assert_expr, msg)\
  {\
    if (!(assert_expr)) {\
      IRS_ASSERT_HELPER(irs::ec_assert, assert_expr, msg);\
    }\
  }
#define IRS_ASSERT_MSG(msg)\
  {\
    IRS_ASSERT_HELPER(irs::ec_assert, "Assert", msg);\
  }
#define IRS_NEW_ASSERT(new_expr)\
  (irs::new_assert(new_expr, __FILE__, __LINE__))



#define IRS_ERROR_HELPER(error_code, spec_data)\
  {\
    irs::error_trans()->throw_error(error_code, __FILE__, __LINE__,\
      static_cast<const void*>(spec_data));\
  }
#define IRS_ERROR(error_code, msg)\
  {\
    IRS_ERROR_HELPER(error_code, msg);\
  }
#define IRS_ERROR_IF_NOT(assert_expr, error_code, msg)\
  {\
    if (!(assert_expr)) {\
      IRS_ERROR_HELPER(error_code, msg);\
    }\
  }
#define IRS_FATAL_ERROR(msg) IRS_ERROR(irs::ec_fatal_error, msg)
#define IRS_DBG_RAW_MSG(msg)\
  {\
    irs::mlog() << msg;\
  }
#define IRS_DBG_MSG(msg)\
  {\
    irs::mlog() << irs::stime << msg << endl;\
  }
#define IRS_DBG_MSG_SRC(msg)\
  {\
    irs::mlog() << irs::stime << "Файл: " << __FILE__;\
    irs::mlog() << "; Строка: " << __LINE__;\
    irs::mlog() << "; Функция: " << __FUNC__ << "; ";\
    irs::mlog() << msg << endl;\
  }

#define IRS_DBG_MSG_SRC_ENG(msg)\
{\
  irs::mlog() << irs::stime << "File: " << __FILE__;\
  irs::mlog() << "; Line: " << __LINE__;\
  irs::mlog() << "; Function: " << __FUNC__ << "; ";\
  irs::mlog() << msg << endl;\
}

#if defined(IRS_WIN32) || defined(IRS_LINUX)
#define IRS_SEND_LAST_ERROR()\
  irs::send_last_message_err(__FILE__,__LINE__)
  
#define IRS_SEND_ERROR(error_code)\
  irs::send_message_err(error_code,__FILE__,__LINE__)
#endif // defined(IRS_WIN32) || defined(IRS_LINUX)

#if defined(IRS_WIN32)
#define IRS_SEND_WIN_WSA_LAST_ERROR()\
  irs::send_wsa_last_message_err(__FILE__,__LINE__)
#endif // IRS_WIN32

#ifdef IRS_LIB_DEBUG
#define IRS_LIB_ASSERT(assert_expr) IRS_ASSERT(assert_expr)
#define IRS_LIB_ASSERT_EX(assert_expr, msg) IRS_ASSERT_EX(assert_expr, msg)
#define IRS_LIB_ASSERT_MSG(msg) IRS_ASSERT_MSG(msg)
#define IRS_LIB_NEW_ASSERT(new_expr) IRS_NEW_ASSERT(new_expr)
#define IRS_LIB_ERROR(error_code, msg) IRS_ERROR(error_code, msg)
#define IRS_LIB_ERROR_IF_NOT(assert_expr, error_code, msg)\
  IRS_ERROR_IF_NOT(assert_expr, error_code, msg)
#define IRS_LIB_FATAL_ERROR(msg) IRS_FATAL_ERROR(msg)
#define IRS_LIB_DBG_RAW_MSG(msg) IRS_DBG_RAW_MSG(msg)
#define IRS_LIB_DBG_MSG(msg) IRS_DBG_MSG(msg)
#define IRS_LIB_DBG_MSG_SRC(msg) IRS_DBG_MSG_SRC(msg)
#define IRS_LIB_DBG_MSG_SRC_ENG(msg) IRS_DBG_MSG_SRC_ENG(msg)
#define IRS_LIB_SEND_LAST_ERROR() IRS_SEND_LAST_ERROR()
#define IRS_LIB_SEND_ERROR() IRS_SEND_ERROR()
#define IRS_LIB_SEND_WIN_WSA_LAST_ERROR() IRS_SEND_WIN_WSA_LAST_ERROR()
#else // IRS_LIB_DEBUG
#define IRS_LIB_ASSERT(assert_expr)
#define IRS_LIB_ASSERT_EX(assert_expr, msg)
#define IRS_LIB_ASSERT_MSG(msg)
#define IRS_LIB_NEW_ASSERT(new_expr) new_expr
#define IRS_LIB_ERROR(error_code, msg)
#define IRS_LIB_ERROR_IF_NOT(assert_expr, error_code, msg)
#define IRS_LIB_FATAL_ERROR(msg)
#define IRS_LIB_DBG_RAW_MSG(msg)
#define IRS_LIB_DBG_MSG(msg)
#define IRS_LIB_DBG_MSG_SRC(msg)
#define IRS_LIB_DBG_MSG_SRC_ENG(msg)
#define IRS_LIB_SEND_LAST_ERROR()
#define IRS_LIB_SEND_ERROR()
#define IRS_LIB_SEND_WIN_WSA_LAST_ERROR()
#endif // IRS_LIB_DEBUG

namespace irs {

// Тип для кодов ошибок
typedef irs_u16 error_code_t;

// Специфические данные для утвердений в error_trans_base_t
struct et_spec_assert_t
{
  // Утверждение
  const char *assert_str;
  // Сообщение
  const char *message;
};

// Стандартный код ошибки (в ap_spec_data содержится сообщение в виде char*)
const error_code_t ec_standard = 0;
// Код ошибки для утверждений (ap_spec_data типа et_spec_assert_t*)
const error_code_t ec_assert = 1;
// Код фатальной ошибки (ap_spec_data типа char*)
const error_code_t ec_fatal_error = 2;
// Пользовательские коды ошибок
const error_code_t ec_user = 3;

// Приемо-передатчик ошибок (Интерфейс)
class error_trans_base_t
{
public:
  virtual ~error_trans_base_t() {}
  // Передать ошибку (тип ap_spec_data определяется a_error_code)
  virtual void throw_error(
    error_code_t a_error_code = ec_standard,
    const char *ap_file_name = 0,
    int a_line_number = 0,
    const void *ap_spec_data = 0
  ) = 0;
  // Считать последний код ошибки
  virtual error_code_t error_code() = 0;
  // Считать имя файла в котором произошла последняя ошибка
  virtual const char *file_name() = 0;
  // Считать номер строки в файле в которой произошла ошибка
  virtual int line_number() = 0;
  // Считать специфические данные соответствующие коду ошибки
  virtual const void *spec_data() = 0;
  // Добавить обработчик ошибки
  virtual void add_handler(mxfact_event_t *ap_handler) = 0;
};

// Приемо-передатчик ошибок (Считывание указателя на экземпляр)
error_trans_base_t *error_trans();

// Специфические данные для утвердений в error_trans_base_t
//  (Считывание указателя на экземпляр)
const void *spec_assert(const char *assert_str,
  const char *message);

class zerobuf: public streambuf
{
public:
  virtual int overflow(int = EOF)
  {
    return 0;
  }
  virtual int underflow()
  {
    return 0;
  }
  virtual int sync()
  {
    return overflow();
  }
};

#ifndef __WATCOMC__
// Библиотека Watcom C++ не поддерживает установку нового буфера через rdbuf
class ostream_buf_init_t
{
public:
  ostream_buf_init_t(ostream* ap_strm, streambuf *ap_buf)
  {
    ap_strm->rdbuf(ap_buf);
  }
};
#endif //__WATCOMC__

#if defined(IRS_WIN32) || defined(IRS_LINUX)
int last_error_code();

irs::string last_error_str();

irs::string error_str(int a_error_code);

void send_format_msg(int a_error_code, char* ap_file, int a_line);

void send_last_message_err(char* ap_file, int a_line);

void send_wsa_last_message_err(char* ap_file, int a_line);

void send_message_err(int a_error_code, char* ap_file, int a_line);
#endif // defined(IRS_WIN32) || defined(IRS_LINUX)

ostream& mlog();

//new_assert(new_expr, __FILE__, __LINE__)
template <class T>
T* new_assert(T* a_new_expr, const char* a_file, int a_line)
{
  if (a_new_expr == IRS_NULL) {
    irs::error_trans()->throw_error(irs::ec_assert, a_file, a_line,
      irs::spec_assert("new != NULL", IRS_NULL));
  }
  return a_new_expr;
}

} //namespace irs

#endif //IRSERRORH

