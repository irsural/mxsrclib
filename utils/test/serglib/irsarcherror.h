// Обработчик ошибок
// Дата: 25.09.2009
// дата создания: 24.09.2009

#ifndef IRSARCHERRORH
#define IRSARCHERRORH

#include <irserror.h>

namespace irs {

namespace gcc_linux {

enum handler_type_t       { ht_exception, ht_message, ht_log, ht_none };
enum handler_format_t     {hf_fixed, hf_variable};

class log_msg_t
{
public:
  virtual void send_msg(const irs::string& a_msg_str) = 0;
};

// Установка стандартного обработчика ошибок
void set_error_handler(
  handler_type_t handler_type = ht_exception,
  log_msg_t* ap_log_msg = 0);

} //namespace gcc_linux

} //namespace irs

#endif //IRSARCHERRORH
