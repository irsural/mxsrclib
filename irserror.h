// Обработка ошибок
// Дата: 15.04.2010
// Ранняя дата: 16.09.2009

#ifndef IRSERRORH
#define IRSERRORH

// Номер файла
#define IRSERRORH_IDX 6

#include <irsdefs.h>

#include <irsconfig.h>
#include <irsdefs.h>
#include <irsint.h>
#include <irstime.h>
#include <irscpp.h>
#include <irsstrdefs.h>

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
  ""; { static char IRS_ICCAVR_FLASH dbg_msg_cstring[] = cstr;\
  irs::mlog() << dbg_msg_cstring; } irs::mlog() << ""
#else //__ICCAVR__
#define irsm(cstr) cstr
#endif //__ICCAVR__

#ifdef __ICCAVR__

// Операция сдвига для вывода строк размещенных во Flash в поток
// Вне пространства имен irs размещена намеренно
inline ostream& operator<<(ostream& a_strm, char IRS_ICCAVR_FLASH* ap_strg)
{
  for (;*ap_strg != 0; ap_strg++) {
    char out_char = *ap_strg;
    a_strm << out_char;
  }
  return a_strm;
}

#ifdef NOP
// Реализация котороая в будущем может понадобится
// !!! Просьба не удалять
ostream& operator<<(ostream& a_strm, char IRS_ICCAVR_FLASH* ap_strg)
{
  enum {
    buf_size = 100
  };
  static char buf[buf_size + 1] = { 0 };
    
  #ifdef IRS_LIB_DEBUG
  if (buf[buf_size] != 0) {
    static char IRS_ICCAVR_FLASH p_msg_no_zero_terminator[] =
      "В операторе сдвига для строк во Flash в буфере отсутствует "
      "завершающий нуль";
    a_strm << endl << p_msg_no_zero_terminator << endl;
    for (;;);
  }
  #endif //IRS_LIB_DEBUG
  
  const size_t strg_size = strlen_G(ap_strg);
  const size_t part_cnt = strg_size/buf_size;
  char IRS_ICCAVR_FLASH* strg_end = ap_strg + strg_size;
  for (size_t part_idx = 0; part_idx < part_cnt; ++part_idx) {
    char IRS_ICCAVR_FLASH* strg_part_end = ap_strg + buf_size;
    copy(ap_strg, strg_part_end, buf);
    a_strm << buf;
    ap_strg = strg_part_end;
  }
  if (ap_strg < strg_end) {
    copy(ap_strg, strg_end, buf);
    buf[strg_end - ap_strg] = 0;
    a_strm << buf;
  }
}
#endif //NOP

#endif //__ICCAVR__


#ifdef IRS_LIB_FLASH_ASSERT
#define IRS_ASSERT_HELPER(error_code, assert_expr, message)\
  {\
    IRS_SPEC_CSTR_DECLARE(assert_expr_fstr, #assert_expr);\
    IRS_SPEC_CSTR_DECLARE(file_fstr, __FILE__);\
    IRS_SPEC_CSTR_DECLARE(message_fstr, message);\
    irs::error_trans()->throw_error(error_code, file_fstr, __LINE__,\
      irs::spec_assert(assert_expr_fstr, message_fstr));\
  }
#else //IRS_LIB_FLASH_ASSERT
#define IRS_ASSERT_HELPER(error_code, assert_expr, message)\
  {\
    irs::error_trans()->throw_error(error_code, __FILE__, __LINE__,\
      irs::spec_assert(#assert_expr, message));\
  }
#endif //IRS_LIB_FLASH_ASSERT
#define IRS_ASSERT(assert_expr)\
  {\
    if (!(assert_expr)) {\
      IRS_ASSERT_HELPER(irs::ec_assert, assert_expr, "");\
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
#ifdef IRS_LIB_FLASH_ASSERT
#define IRS_NEW_ASSERT(new_expr, file_idx)\
  (irs::new_assert(new_expr, file_idx, __LINE__))
#else //IRS_LIB_FLASH_ASSERT
#define IRS_NEW_ASSERT(new_expr, file_idx)\
  (irs::new_assert(new_expr, __FILE__, __LINE__))
#endif //IRS_LIB_FLASH_ASSERT

#ifdef IRS_LIB_FLASH_ASSERT
#define IRS_ERROR_HELPER(error_code, spec_data)\
  {\
    const void* message_spec = IRS_NULL;\
    if (sizeof(char IRS_ICCAVR_FLASH*) == sizeof(void*)) {\
      IRS_SPEC_CSTR_DECLARE(message_fstr, spec_data);\
      message_spec = reinterpret_cast<const void*>(message_fstr);\
    } else {\
      irs::mlog() << irsm("Несоответствие размеров указателя специального ");\
      irs::mlog() << irsm("типа C-строки (char IRS_ICCAVR_FLASH*) и");\
      irs::mlog() << irsm("стандартного (const char*) в макросе ");\
      irs::mlog() << irsm("IRS_ERROR_HELPER\n");\
      irs::mlog() << irsm("Текст сообщения: ") << irsm(spec_data);\
      irs::mlog() << irsm("\nДалее следует вывод сообщения об ошибке ");\
      irs::mlog() << irsm("без этого сообщения...\n") << endl;\
      IRS_SPEC_CSTR_DECLARE(empty_fstr, "");\
      message_spec = reinterpret_cast<const void*>(empty_fstr);\
    }\
    IRS_SPEC_CSTR_DECLARE(file_fstr, __FILE__);\
    irs::error_trans()->throw_error(error_code, file_fstr, __LINE__,\
      message_spec);\
  }
#else //IRS_LIB_FLASH_ASSERT
#define IRS_ERROR_HELPER(error_code, spec_data)\
  {\
    irs::error_trans()->throw_error(error_code, __FILE__, __LINE__,\
      static_cast<const void*>(spec_data));\
  }
#endif //IRS_LIB_FLASH_ASSERT
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
#define IRS_LIB_NEW_ASSERT(new_expr, file_idx)\
  IRS_NEW_ASSERT(new_expr, file_idx)

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
#define IRS_LIB_NEW_ASSERT(new_expr, file_idx) new_expr

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

// Стандартный код ошибки (в ap_spec_data содержится сообщение в виде char*)
const error_code_t ec_standard = 0;
// Код ошибки для утверждений (ap_spec_data типа et_spec_assert_t*)
const error_code_t ec_assert = 1;
// Код фатальной ошибки (ap_spec_data типа char*)
const error_code_t ec_fatal_error = 2;
// Код ошибки для утверждений оператора new (ap_spec_data типа int*)
const error_code_t ec_new_assert = 3;
// Пользовательские коды ошибок
const error_code_t ec_user = 4;

// Приемо-передатчик ошибок (Интерфейс)
class error_trans_base_t
{
public:
  #ifdef IRS_LIB_FLASH_ASSERT
  typedef char IRS_ICCAVR_FLASH* cstr_type;
  #else //IRS_LIB_FLASH_ASSERT
  typedef const char* cstr_type;
  #endif //IRS_LIB_FLASH_ASSERT
  
  virtual ~error_trans_base_t() {}
  // Передать ошибку (тип ap_spec_data определяется a_error_code)
  virtual void throw_error(
    error_code_t a_error_code = ec_standard,
    cstr_type ap_file_name = 0,
    int a_line_number = 0,
    const void *ap_spec_data = 0
  ) = 0;
  // Считать последний код ошибки
  virtual error_code_t error_code() = 0;
  // Считать имя файла в котором произошла последняя ошибка
  virtual cstr_type file_name() = 0;
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
struct et_spec_assert_t
{
  // Утверждение
  error_trans_base_t::cstr_type assert_str;
  // Сообщение
  error_trans_base_t::cstr_type message;
};

// Специфические данные для утвердений в error_trans_base_t
//  (Считывание указателя на экземпляр)
const void *spec_assert(error_trans_base_t::cstr_type assert_str,
  error_trans_base_t::cstr_type message);

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

#ifdef IRS_LIB_FLASH_ASSERT
template <class T>
T* new_assert(T* a_new_expr, int a_file_idx, int a_line)
{
  if (a_new_expr == IRS_NULL) {
    static int a_file_idx_hold = a_file_idx;
    //static char IRS_ICCAVR_FLASH empty_fstr[] = "";
    irs::error_trans()->throw_error(irs::ec_new_assert, IRS_NULL, a_line,
      &a_file_idx_hold);
  }
  return a_new_expr;
}
#else //IRS_LIB_FLASH_ASSERT
template <class T>
T* new_assert(T* a_new_expr, error_trans_base_t::cstr_type a_file,
  int a_line)
{
  if (a_new_expr == IRS_NULL) {
    irs::error_trans()->throw_error(irs::ec_assert, a_file, a_line,
      irs::spec_assert("new != NULL", IRS_NULL));
  }
  return a_new_expr;
}
#endif //IRS_LIB_FLASH_ASSERT

} //namespace irs

#endif //IRSERRORH

