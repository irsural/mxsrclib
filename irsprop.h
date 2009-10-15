// Свойства
// Дата: 20.09.2009
// Ранняя дата: 17.08.2009

#ifndef irspropH
#define irspropH

#if !defined(__ICCAVR__) && !defined(__WATCOMC__) && !defined(_MSC_VER)

#include <irscpp.h>

//#ifdef NOP
namespace irs {

// Свойство только для чтерения
template<class owner_t, class value_t>
class property_read_only_t
{
  typedef value_t (owner_t::*get_method_t) (void) const;
  owner_t& m_owner;
  get_method_t mp_get_method;
  property_read_only_t();
public:
  property_read_only_t(owner_t& a_owner, get_method_t ap_get_method) :
    m_owner(a_owner),
    mp_get_method(ap_get_method)
  { }
  operator value_t () const
  {
    return (m_owner.*mp_get_method)();
  }
};

// Свойство только для записи
template<class owner_t, class value_t>
class property_write_only_t
{
  typedef void (owner_t::*set_method_t) (const value_t &);
  owner_t& m_owner;
  set_method_t mp_set_method;
  property_write_only_t();
public:
  property_write_only_t(owner_t& a_owner, set_method_t ap_set_method) :
    m_owner(a_owner),
    mp_set_method(ap_set_method)
  { }
  const value_t& operator = (const value_t& a_value)
  {
    (m_owner.*mp_set_method)(a_value);
    return a_value;
  }
};

// Свойство для записи и чтения
template<class owner_t, class value_t>
class property_read_write_t
{
  typedef value_t (owner_t::*get_method_t) (void) const;
  typedef void (owner_t::*set_method_t) (const value_t &);
  owner_t& m_owner;
  get_method_t mp_get_method;
  set_method_t mp_set_method;
  property_read_write_t();
 public:
  property_read_write_t(
    owner_t& a_owner,
    get_method_t ap_get_method,
    set_method_t ap_set_method
  ) :
    m_owner(a_owner),
    mp_get_method(ap_get_method),
    mp_set_method(ap_set_method)
  { }
  operator value_t () const
  {
    return (m_owner.*mp_get_method)();
  }
  const value_t& operator = (const value_t& a_value)
  {
    (m_owner.*mp_set_method)(a_value);
    return a_value;
  }
};

// Свойство только для чтения с индексом
template<
  class owner_t, class index_t, class value_t>
class property_read_only_index_t
{
  typedef value_t (owner_t::*get_method_t)(const index_t) const;
public:
  class property_read_only_t;
private:
  property_read_only_t m_property_read_only;
  index_t m_index;
  property_read_only_index_t();
public:
  property_read_only_index_t(
    owner_t& a_owner,
    get_method_t a_get_method
  ) :
    m_property_read_only(a_owner, a_get_method),
    m_index()
  { }
  inline property_read_only_t operator[](
    const index_t a_index)
  {
    m_property_read_only.set_index(a_index);
    return m_property_read_only;
  }
};     

template<
  class owner_t, class index_t, class value_t>
class property_read_only_index_t<
  owner_t, index_t, value_t>::
  property_read_only_t
{
  owner_t& m_owner;
  get_method_t mp_get_method;
  index_t m_index;
  property_read_only_t();
public:
  property_read_only_t(
    owner_t& a_owner,
    get_method_t ap_get_method
  ) :
    m_owner(a_owner),
    mp_get_method(ap_get_method),
    m_index()
  { }
  inline operator value_t () const
  {
    return (m_owner.*mp_get_method)(m_index);
  }
  inline void set_index(
    index_t a_index)
  {
    m_index = a_index;
  }
};

// Свойство только для записи с индексом
template<
  class owner_t, class index_t, class value_t>
class property_write_only_index_t
{
  typedef void (owner_t::*set_method_t)(const index_t, const value_t&);
public:
  class property_write_only_t;
private:
  property_write_only_t m_property_write_only;
  index_t m_index;
  property_write_only_index_t();
public:
  property_write_only_index_t(
    owner_t& a_owner,
    set_method_t a_set_method
  ) :
    m_property_write_only(a_owner, a_set_method),
    m_index()
  { }
  inline void set_index(const index_t a_index)
  {
    m_index = a_index;
  }
  inline property_write_only_t operator[](
    const index_t a_index)
  {
    m_property_write_only.set_index(a_index);
    return m_property_write_only;
  }
};

template<
  class owner_t, class index_t, class value_t>
class property_write_only_index_t<
  owner_t, index_t, value_t>::
  property_write_only_t
{
  owner_t& m_owner;
  set_method_t mp_set_method;
  index_t m_index;
  property_write_only_t();
public:
  property_write_only_t(
    owner_t& a_owner,
    set_method_t ap_set_method
  ) :
    m_owner(a_owner),
    mp_set_method(ap_set_method),
    m_index()
  { }
  inline const value_t& operator = (const value_t& a_value)
  {
    (m_owner.*mp_set_method)(m_index, a_value);
    return a_value;
  }
  inline void set_index(const index_t a_index)
  {
    m_index = a_index;
  }
};

// Свойство для чтения и записи с индексом
template<
  class owner_t, class index_t, class value_t>
class property_read_write_index_t
{
  typedef value_t (owner_t::*get_method_t)(const index_t) const;
  typedef void (owner_t::*set_method_t)(const index_t, const value_t&);
public:
  class property_read_write_t;
private:
  property_read_write_t m_property_read_write;
  index_t m_index;
  property_read_write_index_t();
public:
  property_read_write_index_t(
    owner_t& a_owner,
    get_method_t a_get_method,
    set_method_t a_set_method
  ) :
    m_property_read_write(a_owner, a_get_method, a_set_method),
    m_index()
  { }
  inline void set_index(const index_t a_index)
  {
    m_index = a_index;
  }
  inline property_read_write_t operator[](
    const index_t a_index)
  {
    m_property_read_write.set_index(a_index);
    return m_property_read_write;
  }
};     

template<
  class owner_t, class index_t, class value_t>
class property_read_write_index_t<
  owner_t, index_t, value_t>::
  property_read_write_t
{
  owner_t& m_owner;
  get_method_t mp_get_method;
  set_method_t mp_set_method;
  index_t m_index;
  property_read_write_t();
public:
  property_read_write_t(
    owner_t& a_owner,
    get_method_t ap_get_method,
    set_method_t ap_set_method
  ) :
    m_owner(a_owner),
    mp_get_method(ap_get_method),
    mp_set_method(ap_set_method),
    m_index()
  { }
  inline operator value_t () const
  {
    return (m_owner.*mp_get_method)(m_index);
  }
  inline const value_t& operator = (const value_t& a_value)
  {
    (m_owner.*mp_set_method)(m_index, a_value);
    return a_value;
  }
  inline void set_index(
    const index_t a_index)
  {
    m_index = a_index;
  }
};

// Свойство только для чтения с двойным индексом
template<
  class owner_t, class first_index_t, class second_index_t, class value_t>
class property_read_only_dual_index_t
{
  typedef value_t (owner_t::*get_method_t)
    (const first_index_t, const second_index_t) const;
public:
  class property_read_only_second_index_t;
private:
  property_read_only_second_index_t m_property_read_only_second_index;
  property_read_only_dual_index_t();
public:
  property_read_only_dual_index_t(
    owner_t& a_owner,
    get_method_t a_get_method
  ) :
    m_property_read_only_second_index(a_owner, a_get_method)
  { }
  inline property_read_only_second_index_t operator[](
    const first_index_t a_first_index)
  {
    m_property_read_only_second_index.set_index(a_first_index);
    return m_property_read_only_second_index;
  }
};

template<
  class owner_t, class first_index_t, class second_index_t, class value_t>
class property_read_only_dual_index_t<
  owner_t, first_index_t, second_index_t, value_t>::
  property_read_only_second_index_t
{
public:
  class property_read_only_t;
private:
  first_index_t m_first_index;
  property_read_only_t m_property_read_only;
  property_read_only_second_index_t();
public:
  property_read_only_second_index_t(
    owner_t& a_owner,
    get_method_t a_get_method
  ) :
    m_property_read_only(a_owner, a_get_method),
    m_first_index()
  { }
  inline void set_index(const first_index_t a_first_index)
  {
    m_first_index = a_first_index;
  }
  inline property_read_only_t operator[](const second_index_t a_second_index)
  {
    m_property_read_only.set_index(m_first_index, a_second_index);
    return m_property_read_only;
  }
};

template<
  class owner_t, class first_index_t, class second_index_t, class value_t>
class property_read_only_dual_index_t<
  owner_t, first_index_t, second_index_t, value_t>::
  property_read_only_second_index_t::
  property_read_only_t
{
  owner_t& m_owner;
  first_index_t m_first_index;
  second_index_t m_second_index;
  get_method_t mp_get_method;
  property_read_only_t();
public:
  property_read_only_t(
    owner_t& a_owner,
    get_method_t ap_get_method
  ) :
    m_owner(a_owner),
    mp_get_method(ap_get_method),
    m_first_index(IRS_NULL),
    m_second_index(IRS_NULL)
  { }
  inline operator value_t () const
  {
    return (m_owner.*mp_get_method)(m_first_index, m_second_index);
  }
  inline void set_index(
    const first_index_t a_first_index, const first_index_t a_second_index)
  {
    m_first_index = a_first_index;
    m_second_index = a_second_index;
  }
};

// Свойство только для записи с двойным индексом
template<
  class owner_t, class first_index_t, class second_index_t, class value_t>
class property_write_only_dual_index_t
{
  typedef void (owner_t::*set_method_t)
    (const first_index_t, const second_index_t , const value_t&);
public:
  class property_write_only_second_index_t;
private:
  property_write_only_second_index_t m_property_write_only_second_index;
  property_write_only_dual_index_t();
public:
  property_write_only_dual_index_t(
    owner_t& a_owner,
    set_method_t a_set_method
  ) :
    m_property_write_only_second_index(a_owner, a_set_method)
  { }
  inline property_write_only_second_index_t operator[](
    const first_index_t a_first_index)
  {
    m_property_write_only_second_index.set_index(a_first_index);
    return m_property_write_only_second_index;
  }
};

template<
  class owner_t, class first_index_t, class second_index_t, class value_t>
class property_write_only_dual_index_t<
  owner_t, first_index_t, second_index_t, value_t>::
  property_write_only_second_index_t
{
public:
  class property_write_only_t;
private:
  first_index_t m_first_index;
  property_write_only_t m_property_write_only;
  property_write_only_second_index_t();
public:
  property_write_only_second_index_t(
    owner_t& a_owner,
    set_method_t a_set_method
  ) :
    m_property_write_only(a_owner, a_set_method),
    m_first_index()
  { }
  inline void set_index(const first_index_t a_first_index)
  {
    m_first_index = a_first_index;
  }
  inline property_write_only_t operator[](const second_index_t a_second_index)
  {
    m_property_write_only.set_index(m_first_index, a_second_index);
    return m_property_write_only;
  }
};

template<
  class owner_t, class first_index_t, class second_index_t, class value_t>
class property_write_only_dual_index_t<
  owner_t, first_index_t, second_index_t, value_t>::
  property_write_only_second_index_t::
  property_write_only_t
{
  owner_t& m_owner;
  first_index_t m_first_index;
  second_index_t m_second_index;
  set_method_t mp_set_method;
  property_write_only_t();
public:
  property_write_only_t(
    owner_t& a_owner,
    set_method_t ap_set_method
  ) :
    m_owner(a_owner),
    mp_set_method(ap_set_method),
    m_first_index(IRS_NULL),
    m_second_index(IRS_NULL)
  { }
  inline const value_t& operator = (const value_t& a_value)
  {
    (m_owner.*mp_set_method)(m_first_index, m_second_index, a_value);
    return a_value;
  }
  inline void set_index(
    const first_index_t a_first_index, const second_index_t a_second_index)
  {
    m_first_index = a_first_index;
    m_second_index = a_second_index;
  }
};

// Свойство для чтения и записи с двойным индексом
template<
  class owner_t, class first_index_t, class second_index_t, class value_t>
class property_read_write_dual_index_t
{
  typedef value_t (owner_t::*get_method_t)
    (const first_index_t, const second_index_t) const;
  typedef void (owner_t::*set_method_t)
    (const first_index_t, const second_index_t , const value_t&);
public:
  class property_read_write_second_index_t;
private:
  property_read_write_second_index_t m_property_read_write_second_index;
  property_read_write_dual_index_t();
public:
  property_read_write_dual_index_t(
    owner_t& a_owner,
    get_method_t a_get_method,
    set_method_t a_set_method
  ) :
    m_property_read_write_second_index(a_owner, a_get_method, a_set_method)
  { }
  inline property_read_write_second_index_t operator[](
    const first_index_t a_first_index)
  {
    m_property_read_write_second_index.set_index(a_first_index);
    return m_property_read_write_second_index;
  }  
};

template<
  class owner_t, class first_index_t, class second_index_t, class value_t>
class property_read_write_dual_index_t<
  owner_t, first_index_t, second_index_t, value_t>::
  property_read_write_second_index_t
{
public:
  class property_read_write_t;
private:
  first_index_t m_first_index;
  property_read_write_t m_property_read_write;
  property_read_write_second_index_t();
public:
  property_read_write_second_index_t(
    owner_t& a_owner,
    get_method_t a_get_method,
    set_method_t a_set_method
  ) :
    m_property_read_write(a_owner, a_get_method, a_set_method),
    m_first_index()
  { }
  inline void set_index(const first_index_t a_first_index)
  {
    m_first_index = a_first_index;
  }
  inline property_read_write_t operator[](const second_index_t a_second_index)
  {
    m_property_read_write.set_index(m_first_index, a_second_index);
    return m_property_read_write;
  }
};

template<
  class owner_t, class first_index_t, class second_index_t, class value_t>
class property_read_write_dual_index_t<
  owner_t, first_index_t, second_index_t, value_t>::
  property_read_write_second_index_t::
  property_read_write_t
{
  owner_t& m_owner;
  first_index_t m_first_index;
  second_index_t m_second_index;
  get_method_t mp_get_method;
  set_method_t mp_set_method;
  property_read_write_t();
public:
  property_read_write_t(
    owner_t& a_owner,
    get_method_t ap_get_method,
    set_method_t ap_set_method
  ) :
    m_owner(a_owner),
    mp_get_method(ap_get_method),
    mp_set_method(ap_set_method),
    m_first_index(IRS_NULL),
    m_second_index(IRS_NULL)
  { }
  inline operator value_t () const
  {
    return (m_owner.*mp_get_method)(m_first_index, m_second_index);
  }
  inline const value_t& operator = (const value_t& a_value)
  {
    (m_owner.*mp_set_method)(m_first_index, m_second_index, a_value);
    return a_value;
  }
  inline void set_index(
    const first_index_t a_first_index, const first_index_t a_second_index)
  {
    m_first_index = a_first_index;
    m_second_index = a_second_index;
  }
};

//#endif // NOP

}; // namespace irs

#define example_propety
#ifdef example_propety

//inline bool testing_property();

// Пример использования свойства для только для чтения
class ex_prop_r_t
{
private:
  double m_val;
public:
  typedef const double (ex_prop_r_t::*get_method_t) (void) const;
  get_method_t get_method;
  const double& get_value() const
  {
    return m_val;
  }
  irs::property_read_only_t<ex_prop_r_t, const double&> val;
  ex_prop_r_t():
    m_val(0.),
    val(*this, &ex_prop_r_t::get_value)
  { }
  // Функция для отладки свойств
  //friend inline bool testing_property();
};

// Пример использования свойства только для записи
class ex_prop_w_t
{
private:
  double m_val;
public:
  void set_value(const double& a_val)
  {
    m_val = a_val;
  }
  irs::property_write_only_t<ex_prop_w_t, double> val;
  ex_prop_w_t():
    m_val(0.),
    val(*this, &ex_prop_w_t::set_value)
  { }
  // Функция для отладки свойств
  //friend inline bool testing_property();
};

// Пример использования свойства для чтения и записи
class ex_prop_rw_t
{
private:
  double m_val;
public:
  void set_value(const double& a_val)
  {
    m_val = a_val;
  }
  double get_value() const
  {
    return m_val;
  }
  irs::property_read_write_t<ex_prop_rw_t, double> val;
  ex_prop_rw_t():
    m_val(0.),
    val(*this, &ex_prop_rw_t::get_value, &ex_prop_rw_t::set_value)
  { }
  // Функция для отладки свойств
  //friend inline bool testing_property();
};

// Пример использования свойства только для чтения с индексом
class ex_prop_r_i_t
{
private:
  double m_val[3];
public:
  double get_value(int a_index) const
  {
    return m_val[a_index];
  }
  irs::property_read_only_index_t<ex_prop_r_i_t, int, double>
    val;
  ex_prop_r_i_t(
  ):
    val(*this, &ex_prop_r_i_t::get_value)
  { }
  // Функция для отладки свойств
  //friend inline bool testing_property();
};

// Пример использования свойства только для записи с индексом
class ex_prop_w_i_t
{
private:
  double m_val[3];
public:
  void set_value(const int a_index, const double& a_val)
  {
    m_val[a_index] = a_val;
  }
  irs::property_write_only_index_t<ex_prop_w_i_t, int, double>
    val;
  ex_prop_w_i_t(
  ):
    val(*this, &ex_prop_w_i_t::set_value)
  { }
  // Функция для отладки свойств
  //friend inline bool testing_property();
};

// Пример использования свойства для чтения и записи с индексом
class ex_prop_rw_i_t
{
private:
  double m_val[3];
public:
  void set_value(const int a_index, const double& a_val)
  {
    m_val[a_index] = a_val;
  }
  double get_value(const int a_index) const
  {
    return m_val[a_index];
  }
  irs::property_read_write_index_t<ex_prop_rw_i_t, int, double>
    val;
  ex_prop_rw_i_t(
  ):
    val(*this, &ex_prop_rw_i_t::get_value, &ex_prop_rw_i_t::set_value)
  { }
  // Функция для отладки свойств
  //friend inline bool testing_property();
};

// Пример использования свойства только для чтения с двойным индексом
class ex_prop_r_di_t
{
private:
  double m_val[3][3];
public:
  double get_value(const int a_index1, const int a_index2) const
  {
    return m_val[a_index1][a_index2];
  }
  irs::property_read_only_dual_index_t<ex_prop_r_di_t, int, int, double>
    val;
  ex_prop_r_di_t(
  ):
    val(*this, &ex_prop_r_di_t::get_value)
  { }
  // Функция для отладки свойств
  //friend inline bool testing_property();
};

// Пример использования свойства только для записи с двойным индексом
class ex_prop_w_di_t
{
private:
  double m_val[3][3];
public:
  void set_value(const int a_index1, const int a_index2, const double& a_val)
  {
    m_val[a_index1][a_index2] = a_val;
  }
  irs::property_write_only_dual_index_t<ex_prop_w_di_t, int, int, double>
    val;
  ex_prop_w_di_t(
  ):
    val(*this, &ex_prop_w_di_t::set_value)
  { }
  // Функция для отладки свойств
  //friend inline bool testing_property();
};

// Пример использования свойства для чтения и записи с двойным индексом
class ex_prop_rw_di_t
{
private:
  double m_val[3][3];
public:
  void set_value(const int a_index1, const int a_index2, const double& a_val)
  {
    m_val[a_index1][a_index2] = a_val;
  }
  double get_value(const int a_index1, const int a_index2) const
  {
    return m_val[a_index1][a_index2];
  }
  irs::property_read_write_dual_index_t<ex_prop_rw_di_t, int, int, double>
    val;
  ex_prop_rw_di_t(
  ):
    val(*this, &ex_prop_rw_di_t::get_value, &ex_prop_rw_di_t::set_value)
  { }
  inline irs::property_read_write_dual_index_t<ex_prop_rw_di_t, int,
    int, double>::property_read_write_second_index_t
      operator[](int a_index)
  {
    return val[a_index];
  }
  // Функция для отладки свойств
  //friend inline bool testing_property();
};

// Пример использования свойства для чтения и записи с двойным индексом в
// шаблонном классе
template<class T>
class ex_template_prop_rw_di_t
{
private:
  T m_val_2[10][10];
public:
  typedef unsigned int size_type;
  irs::property_read_write_dual_index_t<ex_template_prop_rw_di_t<T>, size_type,
    size_type, T> val;
  T get_value(const size_type a_index1, const size_type a_index2) const
  {
    return m_val_2[a_index1][a_index2];
  }
  void set_value(
    const size_type a_index1, const size_type a_index2, const T& a_val)
  {
    m_val_2[a_index1][a_index2] = a_val;
  }
  ex_template_prop_rw_di_t(
  ) :
    val(*this, &ex_template_prop_rw_di_t::get_value,
      &ex_template_prop_rw_di_t::set_value)
  { }
  // Функция для отладки свойств
  //friend inline bool testing_property();
};

/*class ex_prop_r_t;
class ex_prop_w_t;
class ex_prop_rw_t;
class ex_prop_r_i_t;
class ex_prop_w_i_t;
class ex_prop_rw_i_t;
class ex_prop_r_di_t;
class ex_prop_w_di_t;
class ex_prop_rw_di_t;
class ex_template_prop_rw_di_t;*/
// Функция для отладки свойств
inline bool testing_property()
{
  #ifndef NOP
  double var = 3254.;
  double result = 0;
  ex_template_prop_rw_di_t<double> ex_template_prop_rw_di;
  result = 0;
  result = ex_template_prop_rw_di.val[2][3] = var;
  result = 0;
  result = ex_template_prop_rw_di.val[2][3];
  return true;
  #else //NOP
  bool fsuccess = true;
  double var = 3254, result = 0;
  ex_template_prop_rw_di_t<double> ex_template_prop_rw_di;
  result = 0;
  result = ex_template_prop_rw_di.val[2][3] = var;
  result = 0;
  result = ex_template_prop_rw_di.val[2][3];
  ex_prop_r_t ex_prop_r;
  ex_prop_w_t ex_prop_w;
  ex_prop_rw_t ex_prop_rw;
  ex_prop_r_i_t ex_prop_r_i;
  ex_prop_w_i_t ex_prop_w_i;
  ex_prop_rw_i_t ex_prop_rw_i;
  ex_prop_r_di_t ex_prop_r_di;
  ex_prop_w_di_t ex_prop_w_di;
  ex_prop_rw_di_t ex_prop_rw_di;
  ex_prop_r.m_val = var;
  result = ex_prop_r.val;
  if (result == var) {
    ex_prop_w.val = var;
    result = 0;
    result = ex_prop_r.val;
  } else {
    fsuccess = false;
  }
  if (fsuccess) {
    if ((result == var) && (ex_prop_w.m_val == var)) {
      result = 0;
      result = ex_prop_rw.val = var;
    } else {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if ((result == var) && (ex_prop_rw.m_val == var)) {
      result = 0;
      result = ex_prop_r.val;
    } else {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (result == var) {
      ex_prop_w_i.m_val[1] = var;
      result = 0;
      result = ex_prop_r_i.val[1];
    } else {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (result == var) {
      result = 0;
      result = ex_prop_w_i.val[1] = var;
    } else {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if ((result == var) && (ex_prop_w_i.m_val[1] == var)) {
      result = 0;
      result = ex_prop_rw_i.val[1] = var;
    } else {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if ((result == var) && (ex_prop_rw_i.m_val[1] == var)) {
      result = 0;
      result = ex_prop_rw_i.val[1];
    } else {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (result == var) {
      ex_prop_r_di.m_val[1][2] = var;
      result = 0;
      result = ex_prop_r_di.val[1][2];
    } else {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (result == var) {
      result = 0;
      result = ex_prop_w_di.val[1][2] = var;
    } else {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if ((result == var) && (ex_prop_w_di.m_val[1][2] == var)) {
      result = 0;
      result = ex_prop_rw_di.val[1][2] = var;
    } else {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if ((result == var) && (ex_prop_rw_di.m_val[1][2] == var)) {
      result = 0;
      result = ex_prop_rw_di.val[1][2];
    } else {
      fsuccess = false;
    }
  }
  if (fsuccess) {
    if (result == var) {
      // Результат верный
    } else {
      fsuccess = false;
    }
  }
  return fsuccess;
  #endif //NOP
}
#endif // example_propety

#endif //!defined(__ICCAVR__) && !defined(__WATCOMC__) && !defined(_MSC_VER)

#endif //irspropH
