//! \file
//! \ingroup configuration_group
//! \brief Начальная инициализация всех типов платформ (ОС + компилятор +
//!   библиотека)
//!
//! Дата: 13.08.2012\n
//! Дата создания: 01.06.2012

#include <irspch.h>

#ifdef QT_CORE_LIB
#include <QTextCodec>
#endif //QT_CORE_LIB

#include <irsinit.h>
#include <irsstring.h>
#include <irslocale.h>

#ifdef __ICCARM__
#include <armcfg.h>
#endif //__ICCARM__

#include <irsfinal.h>

namespace irs {

struct first_init_t
{
  first_init_t()
  {
  }
};

class init_implement_t: public init_t
{
public:
  init_implement_t();
  virtual void dummy();
private:
  first_init_t m_first_init;
  handle_t<mxfact_event_t> mp_error_handler;
};

} //namespace irs

irs::init_implement_t::init_implement_t():
  m_first_init()
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  , mp_error_handler(new irs::exception_error_handler_t)
  #else //IRS_FULL_STDCPPLIB_SUPPORT
  #endif //IRS_FULL_STDCPPLIB_SUPPORT
{
  #ifdef QT_CORE_LIB
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  #endif //QT_CORE_LIB
  
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  irs::loc();
  #endif //IRS_FULL_STDCPPLIB_SUPPORT
  
  irs::irsstr_locale_style_def().set(irs::irsstrloc_current);
}
void irs::init_implement_t::dummy()
{
  // Функция нужна, чтобы можно было использовать абстактный интерфейс
}

irs::init_t& irs::init()
{
  static init_implement_t init_obj;
  return init_obj;
}
