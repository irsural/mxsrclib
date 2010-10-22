//! \file
//! \ingroup error_processing_group
//! \brief Обработка ошибок в С++ Builder
//!
//! Дата: 14.04.2010\n
//! Дата создания: 17.10.2009

#ifndef irscberrorH
#define irscberrorH

#include <irsdefs.h>

#include <irserror.h>

#include <irsfinal.h>

namespace irs {

namespace cbuilder {

//! \addtogroup error_processing_group
//! @{

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

//! @}

} //namespace cbuilder

} //namespace irs

#endif //irscberrorH
