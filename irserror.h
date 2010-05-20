// ��������� ������
// ����: 20.05.2010
// ������ ����: 16.09.2009

#ifndef IRSERRORH
#define IRSERRORH

// ����� �����
#define IRSERRORH_IDX 6

#define IRS_ERROR_OUT_STATIC_EVENT

#include <irsdefs.h>

#ifdef __ICCAVR__
#include <pgmspace.h>
#endif //__ICCAVR__
#ifdef __BORLANDC__
#include <sysutils.hpp>
#endif //__BORLANDC__

#ifdef __ICCAVR__
#include <irsavrutil.h>
#endif //__ICCAVR__
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

// ��������� ������������ ������ irsm � irsu
#ifdef __ICCAVR__
#define irsm(cstr)\
  ""; { static char const IRS_ICCAVR_FLASH dbg_msg_cstring[] = cstr;\
  irs::mlog() << dbg_msg_cstring; } irs::mlog() << ""
#else //__ICCAVR__
#define irsm(cstr) cstr
#endif //__ICCAVR__
#ifdef __ICCAVR__
#define IRS_STRM_ICCAVR_FLASH_OUT(out_stream, flash_string)\
  {\
    static char const IRS_ICCAVR_FLASH irs_stream_flash_string[] =\
      flash_string;\
    out_stream << irs_stream_flash_string;\
  }
#define irsu(out_stream, spec_string)\
  ""; IRS_STRM_ICCAVR_FLASH_OUT(out_stream, spec_string); out_stream << ""
#else //__ICCAVR__
#define irsu(out_stream, spec_string) spec_string
#endif //__ICCAVR__


#ifdef __ICCAVR__

// �������� ������ ��� ������ ����� ����������� �� Flash � �����
// ��� ������������ ���� irs ��������� ���������
inline ostream& operator<<(ostream& a_strm,
  char const IRS_ICCAVR_FLASH* ap_strg)
{
  for (;*ap_strg != 0; ap_strg++) {
    char out_char = *ap_strg;
    a_strm << out_char;
  }
  return a_strm;
}

#ifdef NOP
// ���������� �������� � ������� ����� �����������
// !!! ������� �� �������
inline ostream& operator<<(ostream& a_strm, 
  char const IRS_ICCAVR_FLASH* ap_strg)
{
  enum {
    buf_size = 100
  };
  static char buf[buf_size + 1] = { 0 };
    
  #ifdef IRS_LIB_DEBUG
  if (buf[buf_size] != 0) {
    static char const IRS_ICCAVR_FLASH p_msg_no_zero_terminator[] =
      "� ��������� ������ ��� ����� �� Flash � ������ ����������� "
      "����������� ����";
    a_strm << endl << p_msg_no_zero_terminator << endl;
    for (;;);
  }
  #endif //IRS_LIB_DEBUG
  
  const size_t strg_size = strlen_G(ap_strg);
  const size_t part_cnt = strg_size/buf_size;
  char const IRS_ICCAVR_FLASH* strg_end = ap_strg + strg_size;
  for (size_t part_idx = 0; part_idx < part_cnt; ++part_idx) {
    char const IRS_ICCAVR_FLASH* strg_part_end = ap_strg + buf_size;
    copy(ap_strg, strg_part_end, buf);
    a_strm << buf;
    ap_strg = strg_part_end;
  }
  if (ap_strg < strg_end) {
    copy(ap_strg, strg_end, buf);
    buf[strg_end - ap_strg] = 0;
    a_strm << buf;
  }
  
  return a_strm;
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

// ���� ��� ����� �����������. �� �������!
#ifdef NOP
#define IRS_DELETE_ASSERT(delete_var)\
  {\
    operator delete(delete_var, nothrow);\
    delete_var = IRS_NULL;\
  }
#define IRS_ARRAY_DELETE_ASSERT(delete_var)\
  {\
    operator delete[](delete_var, nothrow);\
    delete_var = IRS_NULL;\
  }
#endif //NOP

#ifdef IRS_LIB_FLASH_ASSERT
#define IRS_NEW_ASSERT(new_expr, file_idx)\
  (irs::new_assert(new (nothrow) new_expr, file_idx, __LINE__))
#else //IRS_LIB_FLASH_ASSERT
#define IRS_NEW_ASSERT(new_expr, file_idx)\
  (new new_expr)
#endif //IRS_LIB_FLASH_ASSERT

#define IRS_LIB_DELETE_ASSERT(delete_var)\
  {\
    delete (delete_var);\
    (delete_var) = IRS_NULL;\
  }
#define IRS_LIB_ARRAY_DELETE_ASSERT(delete_var)\
  {\
    delete[] (delete_var);\
    (delete_var) = IRS_NULL;\
  }

#ifdef IRS_LIB_FLASH_ASSERT
#define IRS_ERROR_HELPER(error_code, spec_data)\
  {\
    const void* message_spec = IRS_NULL;\
    if (sizeof(char const IRS_ICCAVR_FLASH*) == sizeof(void*)) {\
      IRS_SPEC_CSTR_DECLARE(message_fstr, spec_data);\
      message_spec = reinterpret_cast<const void*>(message_fstr);\
    } else {\
      irs::mlog() << irsm("�������������� �������� ��������� ������������ ");\
      irs::mlog() << irsm("���� C-������ (char const IRS_ICCAVR_FLASH*) �");\
      irs::mlog() << irsm("������������ (const char*) � ������� ");\
      irs::mlog() << irsm("IRS_ERROR_HELPER\n");\
      irs::mlog() << irsm("����� ���������: ") << irsm(spec_data);\
      irs::mlog() << irsm("\n����� ������� ����� ��������� �� ������ ");\
      irs::mlog() << irsm("��� ����� ���������...\n") << endl;\
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
    irs::mlog() << irs::stime << "����: " << __FILE__;\
    irs::mlog() << "; ������: " << __LINE__;\
    irs::mlog() << "; �������: " << __FUNC__ << "; ";\
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
#define IRS_LIB_NEW_ASSERT(new_expr, file_idx) (new new_expr)

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

// ��� ��� ����� ������
typedef irs_u16 error_code_t;

// ����������� ��� ������ (� ap_spec_data ���������� ��������� � ���� char*)
const error_code_t ec_standard = 0;
// ��� ������ ��� ����������� (ap_spec_data ���� et_spec_assert_t*)
const error_code_t ec_assert = 1;
// ��� ��������� ������ (ap_spec_data ���� char*)
const error_code_t ec_fatal_error = 2;
// ��� ������ ��� ����������� ��������� new (ap_spec_data ���� int*)
const error_code_t ec_new_assert = 3;
// ���������������� ���� ������
const error_code_t ec_user = 4;

// ������-���������� ������ (���������)
class error_trans_base_t
{
public:
  #ifdef IRS_LIB_FLASH_ASSERT
  typedef char const IRS_ICCAVR_FLASH* cstr_type;
  #else //IRS_LIB_FLASH_ASSERT
  typedef const char* cstr_type;
  #endif //IRS_LIB_FLASH_ASSERT
  
  virtual ~error_trans_base_t() {}
  // �������� ������ (��� ap_spec_data ������������ a_error_code)
  virtual void throw_error(
    error_code_t a_error_code = ec_standard,
    cstr_type ap_file_name = 0,
    int a_line_number = 0,
    const void *ap_spec_data = 0
  ) = 0;
  // ������� ��������� ��� ������
  virtual error_code_t error_code() = 0;
  // ������� ��� ����� � ������� ��������� ��������� ������
  virtual cstr_type file_name() = 0;
  // ������� ����� ������ � ����� � ������� ��������� ������
  virtual int line_number() = 0;
  // ������� ������������� ������ ��������������� ���� ������
  virtual const void *spec_data() = 0;
  // �������� ���������� ������
  virtual void add_handler(mxfact_event_t *ap_handler) = 0;
};

// ������-���������� ������ (���������� ��������� �� ���������)
error_trans_base_t *error_trans();

// ������������� ������ ��� ���������� � error_trans_base_t
struct et_spec_assert_t
{
  // �����������
  error_trans_base_t::cstr_type assert_str;
  // ���������
  error_trans_base_t::cstr_type message;
};

// ������������� ������ ��� ���������� � error_trans_base_t
//  (���������� ��������� �� ���������)
const void *spec_assert(error_trans_base_t::cstr_type assert_str,
  error_trans_base_t::cstr_type message);

// ����� ���������� �� ������ ��� ������������ ������
class error_out_t
{
public:
  typedef error_out_t this_type;
  typedef event_function_t<this_type> out_func_obj_type;
  
  enum {
    m_out_func_list_def_size = ec_user
  };
  
  error_out_t(error_trans_base_t* ap_error_trans,
    irs_size_t a_out_func_list_size = m_out_func_list_def_size);
  void out_general_info(ostream &a_out, 
    error_trans_base_t::cstr_type ap_error_type);
  void out_info(ostream &a_out);
  void insert_out_func(error_code_t a_error_code,
    event_t* ap_out_func);
  void erase_out_func(error_code_t a_error_code);
private:
  error_trans_base_t* mp_error_trans;
  vector<event_t*> m_out_func_ptr_list;
  out_func_obj_type m_out_unknown_error_obj;
  #ifdef IRS_ERROR_OUT_STATIC_EVENT
  out_func_obj_type m_out_standart_error_obj;
  out_func_obj_type m_out_assert_error_obj;
  out_func_obj_type m_out_fatal_error_obj;
  out_func_obj_type m_out_new_assert_error_obj;
  #else //IRS_ERROR_OUT_STATIC_EVENT
  vector<out_func_obj_type> m_out_func_obj_list;
  #endif //IRS_ERROR_OUT_STATIC_EVENT
  ostream* mp_out;

  #ifndef IRS_ERROR_OUT_STATIC_EVENT
  static event_t* out_func_obj_extract_ptr(
    out_func_obj_type& a_out_func_obj);
  #endif //IRS_ERROR_OUT_STATIC_EVENT
  
  void out_standart_error();
  void out_assert_error();
  void out_fatal_error();
  void out_new_assert_error();
  void out_unknown_error();
  bool is_valid_error_code(error_code_t a_error_code);
  error_trans_base_t* error_trans();
};

// ���������� ������ ��� ������ � ostream
class ostream_error_handler_t: public mxfact_event_t
{
private:
  ostream* mp_out;
  error_out_t m_error_out;
  
  void exec()
  {
    mxfact_event_t::exec();
    
    if (mp_out) {
      m_error_out.out_info(*mp_out);
    }
  }
public:
  ostream_error_handler_t(
    ostream* ap_out = IRS_NULL,
    error_trans_base_t* ap_error_trans = error_trans()
  ):
    mp_out(ap_out),
    m_error_out(ap_error_trans)
  {
    ap_error_trans->add_handler(this);
  }
  void out(ostream* ap_out)
  {
    mp_out = ap_out;
  }
};

// ���������� ������ ��� ������ � ostream ���������� ��� AVR
// c ���������� �� ������ � �������� ����������
#ifdef __ICCAVR__
class avr_error_handler_t: public mxfact_event_t
{
private:
  ostream* mp_out;
  error_out_t m_error_out;
  blink_t m_error_blink;
  
  void exec()
  {
    mxfact_event_t::exec();
    
    if (mp_out) {
      m_error_out.out_info(*mp_out);
    }
    for (;;) m_error_blink();
  }
public:
  avr_error_handler_t(
    irs_avr_port_t a_error_blink_port,
    irs_u8 a_error_blink_bit,
    ostream* ap_out = IRS_NULL,
    error_trans_base_t* ap_error_trans = error_trans()
  ):
    mp_out(ap_out),
    m_error_out(ap_error_trans),
    m_error_blink(a_error_blink_port, a_error_blink_bit, make_cnt_ms(25))
  {
    ap_error_trans->add_handler(this);
  }
  void out(ostream* ap_out)
  {
    mp_out = ap_out;
  }
};
#endif //__ICCAVR__

// ���������� ������ ��� ������ ������ � ���� ����������
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
class exception_error_handler_t: public mxfact_event_t
{
private:
  error_out_t m_error_out;
  bool m_is_first_throw;
  
  void exec()
  {
    mxfact_event_t::exec();
    
    if (m_is_first_throw) {
      m_is_first_throw = false;
      ostringstream out_stream;
      m_error_out.out_info(out_stream);
      out_stream << '\0';
      #ifdef __BORLANDC__
      throw Exception(out_stream.str().c_str());
      #else //__BORLANDC__
      throw runtime_error(out_stream.str().c_str());
      #endif //__BORLANDC__
    }
    m_is_first_throw = true;
  }
public:
  exception_error_handler_t(
    error_trans_base_t* ap_error_trans = error_trans()
  ):
    m_error_out(ap_error_trans),
    m_is_first_throw(true)
  {
    ap_error_trans->add_handler(this);
  }
};
#endif //IRS_FULL_STDCPPLIB_SUPPORT

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
// ���������� Watcom C++ �� ������������ ��������� ������ ������ ����� rdbuf
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
    //static char const IRS_ICCAVR_FLASH empty_fstr[] = "";
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

