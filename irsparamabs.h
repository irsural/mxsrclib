// Дата: 03.08.2010
// Дата создания: 29.07.2010

#ifndef irsparamabsh
#define irsparamabsh

#include <irsdefs.h>

#include <irsstrdefs.h>

#include <irsfinal.h>

namespace irs {

class param_box_base_t
{
public:
  typedef string_t string_type;
  
  virtual ~param_box_base_t() {}
  virtual bool show() = 0;
  virtual void hide() = 0;
  virtual void add_edit(const String& a_param_name,
    const String& a_param_value) = 0;
  virtual bool add_combo_by_item(const String& a_param_name,
    const String& a_param_value) = 0;
  virtual void add_combo(const String& a_param_name,
    vector<string_type>* ap_param_values_list) = 0;
  virtual void add_bool(const String& a_param_name,
    bool a_param_value) = 0;
  virtual bool get_param(const string_type& a_param_name,
    String* ap_param_value) = 0;
  virtual bool set_param(const string_type& a_param_name,
    const string_type& a_param_value) = 0;
  virtual void set_ini_name(const string_type& a_ini_name) = 0;
  virtual string_type ini_name() = 0;
  virtual void save() = 0;
  virtual void load() = 0;
  virtual void delete_edit(const String& a_param_name) = 0;
  
}; // param_box_base_t 

} // namespace irs

#endif // irsparamabsh