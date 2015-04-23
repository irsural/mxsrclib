//! \file
//! \ingroup drivers_group
//! \brief Классы для работы с приборами
//!
//! Дата создания: 05.07.2013

#ifndef irsdevicesH
#define irsdevicesH

#include <irsdefs.h>

#include <tstlan4abs.h>
#include <hardflowg.h>
#include <mxdata.h>
#include <measmul.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup network_in_out_group
//! @{

class mxdata_assembly_t
{
public:
  typedef string_t string_type;
  typedef deque<string_type> error_string_list_type;
  enum status_t {
    //! \brief Статус не поддерживается
    //! \details Этот статус выдается, если работа со статусами не
    //!   поддерживается. Он выдается по умолчанию
    status_not_supported,
    //! \brief Соединение выключено
    status_disabled,
    //! \brief Соединение установлено
    status_connected,
    //! \brief Ожидание соединения
    //! \details Этот статус следует выдавать даже тогда, когда происходит
    //!   ошибка соединения, но это не мешает циклически пытаться установить
    //!   соединение
    status_busy,
    //! \brief Произошла ошибка и попыток соединения больше не происходит
    status_error
  };
  virtual ~mxdata_assembly_t() {}
  virtual bool enabled() const = 0;
  virtual void enabled(bool a_enabled) = 0;
  virtual irs::mxdata_t* mxdata() = 0;
  virtual void tick() = 0;
  virtual void show_options() = 0;
  virtual void tstlan4(tstlan4_base_t* ap_tstlan4) = 0;
  virtual status_t get_status();
  virtual error_string_list_type get_last_error_string_list();
};

class mxdata_assembly_params_t
{
public:
  typedef mxdata_assembly_t::string_type string_type;

  mxdata_assembly_params_t(string_type a_ini_name);
};

class mxdata_assembly_creator_t
{
public:
  typedef mxdata_assembly_t::string_type string_type;

  virtual ~mxdata_assembly_creator_t() {}
  virtual handle_t<mxdata_assembly_t> make(tstlan4_base_t* ap_tstlan4,
    const string_type& a_name) = 0;
private:
};

class mxdata_assembly_types_t
{
public:
  typedef mxdata_assembly_t::string_type string_type;

  virtual ~mxdata_assembly_types_t() {}
  virtual void enum_types(vector<string_type>* ap_types) const = 0;
  virtual handle_t<mxdata_assembly_t> make_assembly(
    const string_type& a_assembly_type, tstlan4_base_t* ap_tstlan4,
    const string_type& a_name) = 0;
};

//mxdata_assembly_names_base_t* mxdata_assembly_names();

mxdata_assembly_types_t* mxdata_assembly_types();

class mxmultimeter_assembly_t
{
public:
  typedef string_t string_type;
  virtual ~mxmultimeter_assembly_t() {}
  virtual bool enabled() const = 0;
  virtual void enabled(bool a_enabled) = 0;
  virtual mxmultimeter_t* mxmultimeter() = 0;
  virtual void tick() = 0;
  virtual void show_options() = 0;
};

class mxmultimeter_assembly_creator_t
{
public:
  typedef mxmultimeter_assembly_t::string_type string_type;

  virtual ~mxmultimeter_assembly_creator_t() {}
  virtual handle_t<mxmultimeter_assembly_t> make(const string_type& a_name) = 0;
private:
};

class mxmultimeter_assembly_types_t
{
public:
  typedef mxmultimeter_assembly_t::string_type string_type;

  virtual ~mxmultimeter_assembly_types_t() {}
  virtual void enum_types(vector<string_type>* ap_types) const = 0;
  virtual handle_t<mxmultimeter_assembly_t> make_assembly(
    const string_type& a_assembly_type, const string_type& a_name) = 0;
};

//mxdata_assembly_names_base_t* mxdata_assembly_names();

mxmultimeter_assembly_types_t* mxmultimeter_assembly_types();

//! @}

} // namespace irs

#include <irsdefs.h>

#endif // irsdevicesH
