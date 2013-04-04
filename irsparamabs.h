//! \file
//! \ingroup graphical_user_interface_group
//! Дата: 21.05.2011\n
//! Дата создания: 29.07.2010

#ifndef irsparamabsh
#define irsparamabsh

#include <irsdefs.h>

#include <irsstrdefs.h>
#include <mxdata.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup graphical_user_interface_group
//! @{

class param_box_base_t
{
public:
  typedef string_t string_type;
  
  virtual ~param_box_base_t() {}
  virtual bool show() = 0;
  virtual void hide() = 0;
  virtual void add_edit(const string_type& a_param_name,
    const string_type& a_param_value) = 0;
  //! \brief Добавление выпадающего списка к параметру, созданному add_edit,
  //! поэлементно
  virtual bool add_combo_by_item(const string_type& a_param_name,
    const string_type& a_param_value) = 0;
  //! \brief Добавление выпадающего списка к параметру, созданному add_edit
  virtual void add_combo(const string_type& a_param_name,
    vector<string_type>* ap_param_values_list) = 0;
  virtual void add_bool(const string_type& a_param_name,
    bool a_param_value) = 0;
  virtual bool get_param(const string_type& a_param_name,
    string_type* ap_param_value) const = 0;
  virtual string_type get_param(const string_type& a_param_name) const = 0;
  virtual string_type get_param_def(const string_type& a_param_name) const = 0;
  virtual bool set_param(const string_type& a_param_name,
    const string_type& a_param_value) = 0;
  virtual void set_ini_name(const string_type& a_ini_name) = 0;
  virtual string_type ini_name() const = 0;
  virtual void set_section(const string_type& a_section) = 0;
  virtual string_type section() const = 0;
  virtual void save() const = 0;
  virtual void load() = 0;
  virtual void delete_edit(const string_type& a_param_name) = 0;
  template <class T>
  T read_number(const char_t* a_param_name);
}; // param_box_base_t

//! \brief Чтение числа из param_box_base_t
//!
//! Возвращает параметр, если он может быть преобразован в число
//! Иначе возвращает параметр по умолчанию, если он может быть
//!   преобразован в число
//! Иначе возвращает 0 (то есть T() - что по сути 0)
template <class T>
T param_box_read_number(const param_box_base_t& a_param_box,
  const char_t* a_param_name)
{
  string_t param_name = a_param_name;
  string_t number_str = a_param_box.get_param(param_name);
  T number = T();
  if (!number_str.to_number(number)) {
    number_str = a_param_box.get_param_def(param_name);
    number_str.to_number(number);
  }
  return number;
}
template <class T>
inline T param_box_read_number(const param_box_base_t* ap_param_box,
  const char_t* a_param_name)
{
  param_box_read_number<T>(*ap_param_box, a_param_name);
}
template <class T>
inline T param_box_read_number(handle_t<const param_box_base_t> ap_param_box,
  const char_t* a_param_name)
{
  param_box_read_number<T>(*ap_param_box.get(), a_param_name);
}
template <class T>
T param_box_base_t::read_number(const char_t* a_param_name)
{
  return param_box_read_number<T>(*this, a_param_name);
}


//! @}

} // namespace irs

#endif // irsparamabsh