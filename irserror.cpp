//! \file
//! \ingroup error_processing_group
//! \brief Обработка ошибок
//!
//! Дата: 20.05.2010\n
//! Ранняя дата: 4.08.2009

// Номер файла
#define IRSERRORCPP_IDX 5

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irserror.h>

#include <irsfinal.h>

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

// Вывод информации об ошибке для обработчиков ошибок
irs::error_out_t::error_out_t(
  error_trans_base_t* ap_error_trans,
  irs_size_t a_out_func_list_size
):
  mp_error_trans(ap_error_trans),
  m_out_func_ptr_list(a_out_func_list_size),
  m_out_unknown_error_obj(this, &irs::error_out_t::out_unknown_error),
  #ifdef IRS_ERROR_OUT_STATIC_EVENT
  m_out_standart_error_obj(this, &irs::error_out_t::out_standart_error),
  m_out_assert_error_obj(this, &irs::error_out_t::out_assert_error),
  m_out_fatal_error_obj(this, &irs::error_out_t::out_fatal_error),
  m_out_new_assert_error_obj(this, &irs::error_out_t::out_new_assert_error),
  #else //IRS_ERROR_OUT_STATIC_EVENT
  m_out_func_obj_list(a_out_func_list_size, m_out_unknown_error_obj),
  #endif //IRS_ERROR_OUT_STATIC_EVENT
  mp_out(IRS_NULL)
{
  #ifdef IRS_ERROR_OUT_STATIC_EVENT
  m_out_func_ptr_list[ec_standard] = &m_out_standart_error_obj;
  m_out_func_ptr_list[ec_assert] = &m_out_assert_error_obj;
  m_out_func_ptr_list[ec_fatal_error] = &m_out_fatal_error_obj;
  m_out_func_ptr_list[ec_new_assert] = &m_out_new_assert_error_obj;
  #else //IRS_ERROR_OUT_STATIC_EVENT
  m_out_func_obj_list[ec_standard] =
    out_func_obj_type(this, &irs::error_out_t::out_standart_error);
  m_out_func_obj_list[ec_assert] =
    out_func_obj_type(this, &irs::error_out_t::out_assert_error);
  m_out_func_obj_list[ec_fatal_error] =
    out_func_obj_type(this, &irs::error_out_t::out_fatal_error);
  m_out_func_obj_list[ec_new_assert] =
    out_func_obj_type(this, &irs::error_out_t::out_new_assert_error);

  ::transform(m_out_func_obj_list.begin(), m_out_func_obj_list.end(),
    m_out_func_ptr_list.begin(), out_func_obj_extract_ptr);
  #endif //IRS_ERROR_OUT_STATIC_EVENT
}
void irs::error_out_t::out_general_info(ostream &a_out,
  error_trans_base_t::cstr_type ap_error_type)
{
  a_out << endl << irsu(a_out, "IRS ERROR") << endl;
  a_out << irsu(a_out, "Error Code: ");
  a_out << static_cast<int>(mp_error_trans->error_code()) << endl;
  a_out << irsu(a_out, "Error Type: ") << ap_error_type << endl;
  irs::error_trans_base_t::cstr_type file_name = mp_error_trans->file_name();
  if (file_name) {
    a_out << irsu(a_out, "File: ");
    a_out << mp_error_trans->file_name() << endl;
  }
  a_out << irsu(a_out, "Line number: ");
  a_out << mp_error_trans->line_number() << endl;
}
void irs::error_out_t::out_info(ostream &a_out)
{
  if (mp_error_trans) {
    irs::error_code_t error_code = mp_error_trans->error_code();
    if (is_valid_error_code(error_code)) {
      mp_out = &a_out;
      if (mp_out) {
        m_out_func_ptr_list[error_code]->exec();
      } else {
        IRS_LIB_DBG_RAW_MSG(irsm("\nОшибка в irs::error_out_t::out_info!\n"));
        IRS_LIB_DBG_RAW_MSG(irsm("mp_out не должен быть "));
        IRS_LIB_DBG_RAW_MSG(irsm("равен нулю\n") << endl);
      }
    }
  }
}
void irs::error_out_t::insert_out_func(error_code_t a_error_code,
  event_t* ap_out_func)
{
  if (!is_valid_error_code(a_error_code)) {
    m_out_func_ptr_list.resize(a_error_code + 1);
  } else {
    // Если код ошибки правильный (т. е. память под него выделена),
    // то выделение памяти не требуется
  }
  m_out_func_ptr_list[a_error_code] = ap_out_func;
}
void irs::error_out_t::erase_out_func(error_code_t a_error_code)
{
  if (is_valid_error_code(a_error_code)) {
    m_out_func_ptr_list[a_error_code] = &m_out_unknown_error_obj;
  }
}
#ifndef IRS_ERROR_OUT_STATIC_EVENT
irs::event_t* irs::error_out_t::out_func_obj_extract_ptr(
  out_func_obj_type& a_out_func_obj)
{
  return &a_out_func_obj;
}
#endif //IRS_ERROR_OUT_STATIC_EVENT
void irs::error_out_t::out_standart_error()
{
  IRS_SPEC_CSTR_DECLARE(error_name, "Standart Error");
  out_general_info(*mp_out, error_name);
  irs::error_trans_base_t::cstr_type msg =
    reinterpret_cast<irs::error_trans_base_t::cstr_type>(
    mp_error_trans->spec_data());
  *mp_out << irsu(*mp_out, "Message: ") << msg << endl;
}
void irs::error_out_t::out_assert_error()
{
  IRS_SPEC_CSTR_DECLARE(error_name, "Assert");
  out_general_info(*mp_out, error_name);
  const irs::et_spec_assert_t *spec_data =
    static_cast<const irs::et_spec_assert_t *>(
      mp_error_trans->spec_data()
    );
  if (spec_data) {
    if (spec_data->assert_str) {
      *mp_out << irsu(*mp_out, "Assert: ") << spec_data->assert_str;
      *mp_out << endl;
    }
    if (spec_data->message) {
      *mp_out << irsu(*mp_out, "Message: ") << spec_data->message;
      *mp_out << endl;
    }
  }
}
void irs::error_out_t::out_fatal_error()
{
  IRS_SPEC_CSTR_DECLARE(error_name, "Fatal Error");
  out_general_info(*mp_out, error_name);
  irs::error_trans_base_t::cstr_type spec_data =
    reinterpret_cast<irs::error_trans_base_t::cstr_type>(
    mp_error_trans->spec_data());
  if (spec_data) {
    *mp_out << irsu(*mp_out, "Message: ") << spec_data;
    *mp_out << endl;
  }
}
void irs::error_out_t::out_new_assert_error()
{
  IRS_SPEC_CSTR_DECLARE(error_name, "Assert for new");
  out_general_info(*mp_out, error_name);
  const int& file_idx = *static_cast<const int*>(
    mp_error_trans->spec_data());
  *mp_out << irsu(*mp_out, "File Index: ") << file_idx << endl;
  *mp_out << irsu(*mp_out, "Allocation Error (operator new)") << endl;
}
void irs::error_out_t::out_unknown_error()
{
  IRS_SPEC_CSTR_DECLARE(error_name, "Unknown");
  out_general_info(*mp_out, error_name);
}
bool irs::error_out_t::is_valid_error_code(error_code_t a_error_code)
{
  return static_cast<size_t>(a_error_code) < m_out_func_ptr_list.size();
}
irs::error_trans_base_t* irs::error_out_t::error_trans()
{
  return mp_error_trans;
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
  FormatMessageA(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    a_error_code,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    reinterpret_cast<LPSTR>(&lpMsgBuf),
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
   const char* ap_file,
   int a_line)
{
  #if defined (IRS_WIN32) || defined (IRS_LINUX)
  const irs::string message = static_cast<irs::string>(a_error_code)+
    ": " + error_str(a_error_code);
  irs::error_trans()->throw_error(ec_standard, ap_file, a_line,
    reinterpret_cast<const void*>(message.c_str()));
  #endif // defined (IRS_WIN32) || defined (IRS_LINUX)
}

void irs::send_last_message_err(const char* ap_file, int a_line)
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
void irs::send_wsa_last_message_err(const char* ap_file, int a_line)
{
  int error_code = WSAGetLastError();
  send_format_msg(error_code, ap_file, a_line);
}
#endif // IRS_WIN32

void irs::send_message_err(int a_error_code, const char* ap_file,
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

namespace irs {

class debug_param_implementation_t: public debug_param_t
{
public:
  typedef map<string_type, size_t> param_list_type;
  //typedef param_list_type::iterator pl_iterator;
  typedef param_list_type::const_iterator pl_const_iterator;

  debug_param_implementation_t():
    m_param_list()
  {
  }
  virtual void set(const string_type& a_name, size_t a_value_num)
  {
    m_param_list[a_name] = a_value_num;
  }
  virtual size_t get(const string_type& a_name) const
  {
    size_t param = 0;
    pl_const_iterator it = m_param_list.find(a_name);
    if (it != m_param_list.end()) {
      param = it->second;
    }
    return param;
  }
  virtual bool is_exist(const string_type& a_name) const
  {
    return m_param_list.find(a_name) != m_param_list.end();
  }
private:
  param_list_type m_param_list;
};

} //namespace irs

irs::debug_param_t& irs::debug_param()
{
  static debug_param_implementation_t debug_param_implementation;
  return debug_param_implementation;
}

irs::irs_string_t irs::conv_notprintable_to_hex(const irs_string_t& a_str)
{
  irs_string_t result_str = "";
  for (size_t i = 0; i < a_str.size(); i++) {
    const char space_char = 32;
    if (a_str[i] >= space_char) {
      result_str += a_str[i];
    } else {
      ostrstream stream;
      stream << "\\x";
      stream << hex << setw(2) << setfill('0') << (int)a_str[i];
      result_str += stream.str();
      stream.rdbuf()->freeze(false);
    }
  }
  return result_str;
}

