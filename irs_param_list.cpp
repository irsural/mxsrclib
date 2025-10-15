//! \file
//! \ingroup configuration_group
//! \brief Список строковых параметров

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irs_param_list.h>

#include <irsfinal.h>

namespace irs {

// class param_list_ram_t --------------------------------------------------------------------------

template<class T>
param_list_ram_t<T>::param_list_ram_t():
  mp_child_param_list(NULL),
  m_param_list(),
  mp_enum_it(m_param_list.end())
{
}

template<class T>
void param_list_ram_t<T>::child_param_list(param_list_base_t<T>* ap_child_param_list)
{
  mp_child_param_list = ap_child_param_list;
}

template<class T>
void param_list_ram_t<T>::add_param(const string_type& a_name, const string_type& a_value)
{
  m_param_list.insert(std::make_pair(a_name, a_value));
}

template<class T>
void param_list_ram_t<T>::delete_param(const string_type& a_name)
{
  m_param_list.erase(a_name);
}

template<class T>
void param_list_ram_t<T>::clear_params()
{
  m_param_list.clear();
}

template<class T>
typename param_list_ram_t<T>::string_type param_list_ram_t<T>::param(const string_type& a_name)
{
  if (mp_child_param_list) {
    return mp_child_param_list->param(a_name);
  }
  typename std::map<string_type, string_type>::iterator it = m_param_list.find(a_name);
  if (it != m_param_list.end())
    return it->second;
  IRS_LIB_ERROR(
    ec_standard,
    "Function param_list_ram_t<T>::param(a_name): Trying to read a missing parameter"
  );
  return string_type();
}

template<class T>
void param_list_ram_t<T>::param(const string_type& a_name, const string_type& a_value) const
{
  if (mp_child_param_list) {
    mp_child_param_list->param(a_name, a_value);
    return;
  }
  typename std::map<string_type, string_type>::iterator it = m_param_list.find(a_name);
  if (it != m_param_list.end())
    it->second = a_value;
  else
    IRS_LIB_ERROR(
      ec_standard,
      "Function param_list_ram_t<T>::param(a_name, a_value): Trying to write a missing parameter"
    );
}

template<class T>
void param_list_ram_t<T>::save_all() const
{
}

template<class T>
void param_list_ram_t<T>::save(const string_type& a_name) const
{
}

template<class T>
void param_list_ram_t<T>::load_all()
{
}

template<class T>
void param_list_ram_t<T>::load(const string_type& a_name)
{
}

template<class T>
void param_list_ram_t<T>::enum_params_start()
{
  mp_enum_it = m_param_list.begin();
}

template<class T>
bool param_list_ram_t<T>::enum_param_next(string_type* a_name, string_type* a_value)
{
  if (mp_enum_it != m_param_list.end()) {
    *a_name = mp_enum_it->first;
    *a_value = mp_enum_it->second;
    ++mp_enum_it;
    return false;
  }
  return true;
}

// class param_list_fs_t --------------------------------------------------------------------------

template<class T>
param_list_fs_t<T>::param_list_fs_t(
  fs_t* ap_fs, const string_type& a_path, str_to_utf8_fn_type ap_str_to_utf8_fn
):
  mp_fs(ap_fs),
  m_path(a_path),
  mp_str_to_utf8_fn(ap_str_to_utf8_fn),
  m_param_list(),
  m_child_param_list(NULL)
{
}

template<class T>
void param_list_fs_t<T>::child_param_list(param_list_base_t<T>* ap_child_param_list)
{
}

template<class T>
void param_list_fs_t<T>::add_param(const string_type& a_name, const string_type& a_value)
{
}

template<class T>
void param_list_fs_t<T>::delete_param(const string_type& a_name)
{
}

template<class T>
void param_list_fs_t<T>::clear_params()
{
}

template<class T>
typename param_list_fs_t<T>::string_type param_list_fs_t<T>::param(const string_type& a_name)
{
  return string_type();
}

template<class T>
void param_list_fs_t<T>::param(const string_type& a_name, const string_type& a_value) const
{
}

template<class T>
void param_list_fs_t<T>::save_all() const
{
}

template<class T>
void param_list_fs_t<T>::save(const string_type& a_name) const
{
}

template<class T>
void param_list_fs_t<T>::load_all()
{
}

template<class T>
void param_list_fs_t<T>::load(const string_type& a_name)
{
}

template<class T>
void param_list_fs_t<T>::enum_params_start()
{
}

template<class T>
bool param_list_fs_t<T>::enum_param_next(string_type* a_name, string_type* a_value)
{
  return false;
}

// functions --------------------------------------------------------------------------

template<class T>
void copy_param_list(const param_list_base_t<T>* ap_from, param_list_base_t<T>* ap_to)
{
  typedef typename param_list_base_t<T>::string_type string_type;

  ap_to->clear_params();
  ap_from->enum_params_start();
  string_type name, value;
  while (ap_from->enum_param_next(&name, &value)) {
    ap_to->add_param(name, value);
  }
}

} // namespace irs
