// Обработка ошибок
// С++ Builder
// Дата: 10.04.2009

#ifndef irscberrorH
#define irscberrorH

#include <winsock2.h>

#include <irserror.h>

#define SEND_WIN_LAST_MESSAGE_ERR(error_trans)\
  irs::cbuilder::send_last_message_err(error_trans,__FILE__,__LINE__)

#define SEND_WIN_WSA_LAST_MESSAGE_ERR(error_trans)\
  irs::cbuilder::send_wsa_last_message_err(error_trans,__FILE__,__LINE__)

#define SEND_WIN_MESSAGE_ERR(error_code)\
  irs::cbuilder::send_message_err(error_code,__FILE__,__LINE__)

namespace irs {

namespace cbuilder {

enum handler_type_t { ht_exception, ht_message, ht_log, ht_none };
enum handler_format_t{hf_fixed, hf_variable};

class log_msg_t
{
public:
  virtual void send_msg(const irs::string& a_msg_str) = 0;
};

// Установка стандартного обработчика ошибок
void set_error_handler(
  handler_type_t handler_type = ht_exception,
  log_msg_t* ap_log_msg = 0);

void send_format_msg(
  int a_error_code,
  error_trans_base_t* ap_error_trans,
  char* ap_file,
  int a_line);

void send_last_message_err(
  error_trans_base_t* ap_error_trans, char* ap_file, int a_line);

void send_wsa_last_message_err(
  error_trans_base_t* ap_error_trans, char* ap_file, int a_line);

void send_message_err(DWORD a_error_code, char* ap_file, int a_line);  

} //namespace cbuilder

} //namespace irs

#endif //irscberrorH
