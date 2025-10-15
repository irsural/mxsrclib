//! \file
//! \ingroup configuration_group
//! \brief Список строковых параметров

#ifndef IRS_PARAM_LISTH
#define IRS_PARAM_LISTH

#include <irsdefs.h>

#include <irscpp.h>
#include <irsfs.h>

#include <irsfinal.h>

namespace irs {

template<class T>
class param_list_base_t
{
public:
  typedef T string_type;

  virtual ~param_list_base_t()
  {
  }

  virtual void child_param_list(param_list_base_t* ap_child_param_list) = 0;
  virtual void add_param(const string_type& a_name, const string_type& a_value) = 0;
  virtual void delete_param(const string_type& a_name) = 0;
  virtual void clear_params() = 0;
  virtual string_type param(const string_type& a_name) = 0;
  virtual void param(const string_type& a_name, const string_type& a_value) const = 0;
  virtual void save_all() const = 0;
  virtual void save(const string_type& a_name) const = 0;
  virtual void load_all() = 0;
  virtual void load(const string_type& a_name) = 0;
  virtual void enum_params_start() = 0;
  virtual bool enum_param_next(string_type* a_name, string_type* a_value) = 0;
};

template<class T>
class param_list_ram_t: public param_list_base_t<T>
{
public:
  typedef T string_type;

  param_list_ram_t();
  virtual void child_param_list(param_list_base_t<T>* ap_child_param_list);
  virtual void add_param(const string_type& a_name, const string_type& a_value);
  virtual void delete_param(const string_type& a_name) = 0;
  virtual void clear_params() = 0;
  virtual string_type param(const string_type& a_name);
  virtual void param(const string_type& a_name, const string_type& a_value) const;
  virtual void save_all() const;
  virtual void save(const string_type& a_name) const = 0;
  virtual void load_all() = 0;
  virtual void load(const string_type& a_name) = 0;
  virtual void enum_params_start() = 0;
  virtual bool enum_param_next(string_type* a_name, string_type* a_value) = 0;

private:
  param_list_ram_t* mp_child_param_list;
  std::map<string_type, string_type> m_param_list;
  typename std::map<string_type, string_type>::const_iterator mp_enum_it;
};

template<class T>
class param_list_fs_t: public param_list_base_t<T>
{
public:
  typedef T string_type;
  typedef std::string (*str_to_utf8_fn_type)(const string_type& a_str);

  param_list_fs_t(
    fs_t* ap_fs, const string_type& a_path, str_to_utf8_fn_type ap_str_to_utf8_fn = NULL
  );
  virtual void child_param_list(param_list_base_t<T>* ap_child_param_list);
  virtual void add_param(const string_type& a_name, const string_type& a_value);
  virtual void delete_param(const string_type& a_name) = 0;
  virtual void clear_params() = 0;
  virtual string_type param(const string_type& a_name);
  virtual void param(const string_type& a_name, const string_type& a_value) const;
  virtual void save_all() const;
  virtual void save(const string_type& a_name) const = 0;
  virtual void load_all() = 0;
  virtual void load(const string_type& a_name) = 0;
  virtual void enum_params_start() = 0;
  virtual bool enum_param_next(string_type* a_name, string_type* a_value) = 0;

private:
  fs_t* mp_fs;
  string_type m_path;
  str_to_utf8_fn_type mp_str_to_utf8_fn;
  std::map<string_type, string_type> m_param_list;
  param_list_base_t<T>* m_child_param_list;
};

} // namespace irs

#endif //IRS_PARAM_LISTH
