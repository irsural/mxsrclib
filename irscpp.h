// Заголовки стандартной библиотеки С++
// Используется для переносимости на разные компиляторы
// Дата: 12.07.2010
// Ранняя дата: 26.09.2007

#ifndef IRSCPPH
#define IRSCPPH

#include <irsdefs.h>

#include <complex>
#include <iomanip>
//#include <ios>
//#include <iosfwd>
#include <iostream>
//#include <istream>
#include <new>
//#include <ostream>
#include <sstream>
//#include <streambuf>
#include <string>
#include <strstream>
// ---
// Требуется для файла algorithm в Watcom несомтря на irsfinal
#undef max
#undef min
#include <algorithm>
// ---
#include <deque>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <utility>
#include <vector>
#include <fstream>
#ifndef __embedded_cplusplus
#include <locale>
#include <limits>
#endif //__embedded_cplusplus
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
#include <stdexcept>
#endif //IRS_FULL_STDCPPLIB_SUPPORT

#include <irsfinal.h>

#ifndef NAMESPACE_STD_NOT_SUPPORT
namespace std {
#endif //NAMESPACE_STD_NOT_SUPPORT

// Переносимые манипуляторы
IRS_STREAMSPECDECL inline ios &general(ios &strm)
{
  strm.setf(static_cast<ios::fmtflags>(0), ios::floatfield);
  return strm;
}
#ifdef __WATCOMC__
IRS_STREAMSPECDECL inline ios &fixed(ios &strm, int)
{
  strm.setf(ios::fixed, ios::floatfield);
  return strm;
}
IRS_STREAMSPECDECL inline ios &scientific(ios &strm, int)
{
  strm.setf(ios::scientific, ios::floatfield);
  return strm;
}
IRS_STREAMSPECDECL inline ios &left(ios &strm)
{
  strm.setf(ios::left, ios::adjustfield);
  return strm;
}
IRS_STREAMSPECDECL inline ios &right(ios &strm)
{
  strm.setf(ios::right, ios::adjustfield);
  return strm;
}
IRS_STREAMSPECDECL inline ios &internal(ios &strm)
{
  strm.setf(ios::internal, ios::adjustfield);
  return strm;
}
IRS_STREAMSPECDECL inline ios &showbase(ios &strm, int)
{
  strm.setf(ios::showbase);
  return strm;
}
IRS_STREAMSPECDECL inline ios &noshowbase(ios &strm, int)
{
  strm.unsetf(ios::showbase);
  return strm;
}
IRS_STREAMSPECDECL inline ios &showpos(ios &strm, int)
{
  strm.setf(ios::showpos);
  return strm;
}
IRS_STREAMSPECDECL inline ios &noshowpos(ios &strm, int)
{
  strm.unsetf(ios::showpos);
  return strm;
}
IRS_STREAMSPECDECL inline ios &uppercase(ios &strm, int)
{
  strm.setf(ios::uppercase);
  return strm;
}
IRS_STREAMSPECDECL inline ios &nouppercase(ios &strm, int)
{
  strm.unsetf(ios::uppercase);
  return strm;
}
#endif //__WATCOMC__

#ifdef NOP
inline ostream& operator<<(ostream& strm, ios& (*f)(ios&, int))
{
  f(strm, 1);
  return strm;
}
inline ios &oct(ios &strm, int)
{
  strm.setf(ios::oct, ios::basefield);
  return strm;
}
inline ios &dec(ios &strm, int)
{
  strm.setf(ios::dec, ios::basefield);
  return strm;
}
inline ios &hex(ios &strm, int)
{
  strm.setf(ios::hex, ios::basefield);
  return strm;
}
#ifdef NOP
inline ios &boolalpha(ios &strm, int)
{
  strm.setf(ios::boolalpha);
  return strm;
}
inline ios &noboolalpha(ios &strm, int)
{
  strm.unsetf(ios::boolalpha);
  return strm;
}
inline ios &showpoint(ios &strm, int)
{
  strm.setf(ios::showpoint);
  return strm;
}
inline ios &noshowpoint(ios &strm, int)
{
  strm.unsetf(ios::showpoint);
  return strm;
}
#endif //NOP
inline ios &skipws(ios &strm, int)
{
  strm.setf(ios::skipws);
  return strm;
}
inline ios &noskipws(ios &strm, int)
{
  strm.unsetf(ios::skipws);
  return strm;
}
#endif //NOP

// Cygwin не полностью поддерживает wchar_t и wstring
#ifdef __CYGWIN__
//template<> struct char_traits<wchar_t>;
typedef basic_string<wchar_t> wstring;
#endif //__CYGWIN__

#ifdef __WATCOMC__
typedef int irs_streamsize_t;

// В deque Watcom нет операции минус на итераторах
template <class input_type> inline typename iterator_traits<input_type>::
  difference_type irs_deque_distance(input_type a_first, input_type a_last)
{
  return ::distance(a_first, a_last, input_iterator_tag());
}
template <class container_type>
inline void irs_container_resize(
  container_type* ap_container,
  typename container_type::size_type a_size
)
{
  typedef typename container_type::size_type cont_size_t;
  typedef typename container_type::value_type cont_value_t;
  cont_size_t old_size = ap_container->size();
  cont_size_t new_size = a_size;
  if (new_size > old_size) {
    cont_size_t diff_size = new_size - old_size;
    for (cont_size_t diff_idx = 0; diff_idx < diff_size; diff_idx++) {
      ap_container->push_back(cont_value_t());
    }
  } else {
    cont_size_t diff_size = old_size - new_size;
    for (cont_size_t diff_idx = 0; diff_idx < diff_size; diff_idx++) {
      ap_container->pop_back();
    }
  }
}
#else //__WATCOMC__
typedef streamsize irs_streamsize_t;

template <class input_type> inline typename iterator_traits<input_type>::
  difference_type irs_deque_distance(input_type a_first, input_type a_last)
{
  return distance(a_first, a_last);
}
template <class container_type>
inline void irs_container_resize(
  container_type* ap_container,
  typename container_type::size_type a_size
)
{
  ap_container->resize(a_size);
}
#endif //__WATCOMC__

#ifndef NAMESPACE_STD_NOT_SUPPORT
} //namespace std
#endif //NAMESPACE_STD_NOT_SUPPORT

#ifndef NAMESPACE_STD_NOT_SUPPORT
using namespace std;
#endif //NAMESPACE_STD_NOT_SUPPORT

#endif //IRSCPPH
