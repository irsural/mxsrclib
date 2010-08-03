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
  virtual void show() = 0;
  virtual void hide() = 0;
  virtual void add(const String& a_param_name, const String& a_param_value) = 0;
  virtual bool get_param(const string_type& a_param_name,
    String* ap_param_value) = 0;
  virtual bool set_param(const string_type& a_param_name,
    const string_type& a_param_value) = 0;
  virtual void set_ini_name(const string_type& a_ini_name) = 0;
  virtual string_type ini_name() = 0;
  virtual void save() = 0;
  virtual void load() = 0;
  
}; // param_box_base_t 

} // namespace irs

#endif // irsparamabsh