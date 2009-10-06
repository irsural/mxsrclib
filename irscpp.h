// Заголовки стандартной библиотеки С++
// Используется для переносимости на разные компиляторы
// Дата: 06.10.2009
// Ранняя дата: 26.09.2007

#ifndef IRSCPPH
#define IRSCPPH

#include <irsdefs.h>

#undef max
#undef min

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
#include <algorithm>
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
#include <locale>

#ifndef NAMESPACE_STD_NOT_SUPPORT
namespace std {
#endif //NAMESPACE_STD_NOT_SUPPORT

// Переносимые манипуляторы
inline ios &general(ios &strm)
{
  strm.setf(static_cast<ios::fmtflags>(0), ios::floatfield);
  return strm;
}
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
inline ios &fixed(ios &strm, int)
{
  strm.setf(ios::fixed, ios::floatfield);
  return strm;
}
inline ios &scientific(ios &strm, int)
{
  strm.setf(ios::scientific, ios::floatfield);
  return strm;
}
inline ios &left(ios &strm, int)
{
  strm.setf(ios::left, ios::adjustfield);
  return strm;
}
inline ios &right(ios &strm, int)
{
  strm.setf(ios::right, ios::adjustfield);
  return strm;
}
inline ios &internal(ios &strm, int)
{
  strm.setf(ios::internal, ios::adjustfield);
  return strm;
}
inline ios &showbase(ios &strm, int)
{
  strm.setf(ios::showbase);
  return strm;
}
inline ios &noshowbase(ios &strm, int)
{
  strm.unsetf(ios::showbase);
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
inline ios &showpos(ios &strm, int)
{
  strm.setf(ios::showpos);
  return strm;
}
inline ios &noshowpos(ios &strm, int)
{
  strm.unsetf(ios::showpos);
  return strm;
}
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
inline ios &uppercase(ios &strm, int)
{
  strm.setf(ios::uppercase);
  return strm;
}
inline ios &nouppercase(ios &strm, int)
{
  strm.unsetf(ios::uppercase);
  return strm;
}
#endif //NOP

#ifndef NAMESPACE_STD_NOT_SUPPORT
} //namespace std
#endif //NAMESPACE_STD_NOT_SUPPORT

#ifndef NAMESPACE_STD_NOT_SUPPORT
using namespace std;
#endif //NAMESPACE_STD_NOT_SUPPORT

#endif //IRSCPPH
