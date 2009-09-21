// Набор тестов
// Дата: 20.09.2009
// Дата создания: 20.09.2009

#ifndef IRSTESTH
#define IRSTESTH

#include <irsdefs.h>
#include <irscpp.h>

namespace irs {

namespace compiler_test {

namespace template_template_inheritance {

// Наследование шаблонов от шаблонов
template <class T>
struct base_t
{
  typedef size_t size_type;

  enum {
    bits_on_byte = 8
  };

  T f(T x)
  {
    return x*x;
  }
};
template <class T, class B = base_t<T> >
struct derived_t: B
{
};
inline void exec(ostream& a_strm)
{
  derived_t<double> derived;
  a_strm << "base_t::f call through derived_t::f: " << derived.f(1.1) << '\n';
  a_strm << "base_t::bits_on_byte call through derived_t::bits_on_byte: ";
  a_strm << derived_t<double>::bits_on_byte << '\n';
  a_strm << "base_t::size_type call through derived_t::size_type: ";
  derived_t<double>::size_type size = 7;
  a_strm << size << '\n';
  a_strm << endl;
}

} //namespace template_template_inheritance

} //namespace compiler_test

} //namespace irs

#endif //IRSTESTH
