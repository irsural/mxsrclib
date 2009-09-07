// Математические функции
// Дата: 4.08.2009

#ifndef irsmathH
#define irsmathH

#include <math.h>
#include <irscpp.h>

namespace irs {

// a < b
const int less_than_value = -1;
// a == b
const int equals_value = 0;
// a > b
const int greater_than_value = 1;
// Сравнивает два значения с точностью до a_epsilon
// Аналог функции CompareValue из библиотеки Math.hpp C++ Builder6
int compare_value(
  const double& a_a, const double& a_b, const double& a_epsilon);

}; // namespace irs
//---------------------------------------------------------------------------
#endif //irsmathH
