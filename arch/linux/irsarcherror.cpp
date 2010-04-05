// Обработка ошибок
// Linux
// Дата: 31.03.2010 
// Дата создания: 24.09.2009

//При компиляции с помощью g++ (GCC) 3.4.4 runtime_error не выводит сообщение,
//но выполнение программы завершает.
//При компиляции с помощью g++ (GCC) 4.3.2 20080827 (alpha-testing)
//runtime_error выводит сообщение и прерывает выполнение программы.

#include <irsdefs.h>

#include <irsarcherror.h>
#include <irserror.h>
#include <irsstd.h>
#include <irscpp.h>
#include <exception>
#include <stdexcept>

#include <irsfinal.h>

namespace irs {

namespace gcc_linux {

struct config_t
{
  handler_type_t standard;
  handler_type_t assert;
  handler_type_t fatal_error;
};

// Обработчик ошибок для Linux
class error_handler_t: public mxfact_event_t
{
public:
  error_handler_t(
  irs::error_trans_base_t *ap_error_trans = 0,
  log_msg_t* ap_log_msg = 0);
  error_handler_t(
    handler_type_t a_handler_type,
    irs::error_trans_base_t *ap_error_trans = 0,
    log_msg_t* ap_log_msg = 0);
  virtual void exec();
  void set_error_trans(irs::error_trans_base_t *ap_error_trans);
  void set_config(const config_t& a_config);
private:
  irs::error_trans_base_t *mp_error_trans;
  handler_type_t m_handler_type;
  handler_format_t m_handler_format;
  irs::string m_msg;
  bool m_msg_first;
  config_t m_config;
  log_msg_t* mp_log_msg;
  void error_out(ostrstream &msg_strm, irs::error_code_t error_code,
    const char *error_type) const;
  void show_msg(irs::string &a_msg, handler_type_t a_handler_type);
};

} //namespace gcc_linux

} //namespace irs

// Обработчик ошибок для Linux
irs::gcc_linux::error_handler_t::error_handler_t(
  irs::error_trans_base_t *ap_error_trans,
  log_msg_t* ap_log_msg
):
  mp_error_trans(ap_error_trans),
  m_handler_type(ht_exception),
  m_handler_format(hf_variable),
  m_msg(),
  m_msg_first(true),
  mp_log_msg(ap_log_msg)
  //irs::error_trans()
{
  m_config.standard = ht_log;
  m_config.assert = ht_exception;
  m_config.fatal_error = ht_exception;
  set_error_trans(mp_error_trans);
}
irs::gcc_linux::error_handler_t::error_handler_t(
  handler_type_t a_handler_type,
  irs::error_trans_base_t *ap_error_trans,
  log_msg_t* ap_log_msg
):
  mp_error_trans(ap_error_trans),
  m_handler_type(a_handler_type),
  m_handler_format(hf_fixed),
  m_msg(),
  m_msg_first(true),
  mp_log_msg(ap_log_msg)
  //irs::error_trans()
{
  m_config.standard =  ht_log;
  m_config.assert = ht_exception;
  m_config.fatal_error = ht_exception;
  set_error_trans(mp_error_trans);
}
void irs::gcc_linux::error_handler_t::error_out(ostrstream &msg_strm,
  irs::error_code_t error_code, const char *error_type) const
{
  msg_strm << endl << "IRS ERROR" << endl;
  msg_strm << "Error Code: " << (int)error_code << endl;
  msg_strm << "Error Type: " << error_type << endl;
  const char *file_name = mp_error_trans->file_name();
  if (file_name)
    msg_strm << "File: " << mp_error_trans->file_name() << endl;
  msg_strm << "Line number: " << mp_error_trans->line_number() << endl;
}
void irs::gcc_linux::error_handler_t::show_msg(irs::string &a_msg,
  handler_type_t a_handler_type)
{
  if (m_msg_first) {
    m_msg_first = false;
    throw runtime_error(a_msg.c_str());
  }
  m_msg_first = true;
}
void irs::gcc_linux::error_handler_t::exec()
{
  mxfact_event_t::exec();
  if (mp_error_trans) {
    irs::error_code_t error_code = mp_error_trans->error_code();
    switch (error_code) {
      case irs::ec_standard: {
        ostrstream msg_strm;
        error_out(msg_strm, error_code, "Standart Error");
        const char* msg =
          static_cast<const char*>(mp_error_trans->spec_data());
        msg_strm << "Message: " << msg;
        msg_strm << '\0';
        m_msg = msg_strm.str();
        msg_strm.freeze(false);
        if(m_handler_format == hf_variable){
          m_handler_type = m_config.standard;
        }
        show_msg(m_msg, m_handler_type);
      } break;
      case irs::ec_assert: {
        ostrstream msg_strm;
        error_out(msg_strm, error_code, "Assert");
        const irs::et_spec_assert_t *spec_data =
          static_cast<const irs::et_spec_assert_t *>(
            mp_error_trans->spec_data()
          );
        if (spec_data) {
          if (spec_data->assert_str) {
            msg_strm << "Assert: " << spec_data->assert_str;
            msg_strm << endl;
          }
          if (spec_data->message) {
            msg_strm << "Message: " << spec_data->message;
            msg_strm << endl;
          }
        }
        msg_strm << '\0';
        m_msg = msg_strm.str();
        msg_strm.freeze(false);
        if(m_handler_format == hf_variable){
          m_handler_type = m_config.assert;
        }
        show_msg(m_msg, m_handler_type);
      } break;
      case irs::ec_fatal_error: {
        ostrstream msg_strm;
        error_out(msg_strm, error_code, "Fatal Error");
        const char *spec_data = static_cast<const char *>(
          mp_error_trans->spec_data());
        if (spec_data) {
          msg_strm << "Message: " << spec_data;
          msg_strm << endl;
        }
        msg_strm << '\0';
        m_msg = msg_strm.str();
        msg_strm.freeze(false);
        if(m_handler_format == hf_variable){
          m_handler_type = m_config.fatal_error;
        }
        show_msg(m_msg, m_handler_type);
      } break;
      default: {
        ostrstream msg_strm;
        error_out(msg_strm, error_code, "Unknown");
        msg_strm << '\0';
        m_msg = msg_strm.str();
        msg_strm.freeze(false);
        show_msg(m_msg, m_handler_type);
      } break;
    }
  }
}

void irs::gcc_linux::error_handler_t::set_config(const config_t& a_config)
{
  m_config = a_config;
}

void irs::gcc_linux::error_handler_t::
set_error_trans(irs::error_trans_base_t *ap_error_trans)
{
  mp_error_trans = ap_error_trans;
  if (mp_error_trans) mp_error_trans->add_handler(this);
}

// Установка обработчика ошибок Linux
void irs::gcc_linux::set_error_handler(
  handler_type_t handler_type, log_msg_t* ap_log_msg)
{
  static error_handler_t error_handler_i(
    handler_type, irs::error_trans(), ap_log_msg);
}
