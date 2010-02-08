// Математические функции
// Дата: 16.09.2009

#ifndef irsmathH
#define irsmathH

#include <irsstdg.h>
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

// Обратный гиперболический синус
inline double asinh(const double a_x);
// Обратный гиперболический косинус, a_x >= 1
inline double acosh(const double a_x);
// Обратный гиперболический тангенс
inline double atanh(const double a_x);

// Градусы в радианы
inline double deg_to_rad(const double a_degree);
// Грады в радианы
inline double gra_to_rad(const double a_grad);
// Радианы в градусы
inline double rad_to_deg(const double a_radian);
// Радианы в грады
inline double rad_to_gra(const double a_radian);

inline double sin_deg(const double a_degree);
inline double sin_rad(const double a_radian);
inline double sin_gra(const double a_grad);

inline double cos_deg(const double a_degree);
inline double cos_rad(const double a_radian);
inline double cos_gra(const double a_grad);

inline double tan_deg(const double a_degree);
inline double tan_rad(const double a_radian);
inline double tan_gra(const double a_grad);

inline double asin_deg(const double a_value);
inline double asin_rad(const double a_value);
inline double asin_gra(const double a_value);

inline double acos_deg(const double a_value);
inline double acos_rad(const double a_value);
inline double acos_gra(const double a_value);

inline double atan_deg(const double a_value);
inline double atan_rad(const double a_value);
inline double atan_gra(const double a_value);

// Функция реализует алгоритм Евклида -
// алгоритм для нахождения наибольшего делителя двух
// целых чисел или наибольшей общей меры двух однородных величин.
inline int gcd(int a, int b);

// Нахождения наименьшего общего кратного
inline int lcm(int a, int b);

inline double asinh(const double a_x)
{
  return log(a_x + sqrt(pow(a_x, 2) + 1));
}

inline double acosh(const double a_x)
{
  return log(a_x + sqrt(pow(a_x, 2) - 1));
}

inline double atanh(const double a_x)
{
  return 0.5 * log((1 + a_x) / (1 - a_x));
}

inline double deg_to_rad(const double a_degree)
{
  const double const_pi_180 = IRS_PI / 180;
  return a_degree * const_pi_180;
}

inline double gra_to_rad(const double a_grad)
{
  const double const_pi_200 = IRS_PI / 200;
  return a_grad * const_pi_200;
}

inline double rad_to_deg(const double a_radian)
{
  const double const_180_pi = 180 / IRS_PI;
  return a_radian * const_180_pi;
}

inline double rad_to_gra(const double a_radian)
{
  const double const_200_pi = 200 / IRS_PI;
  return a_radian * const_200_pi;
}         

inline double sin_deg(const double a_degree)
{
  double rad = deg_to_rad(a_degree);
  return sin(rad);
}

inline double sin_rad(const double a_radian)
{
  return sin(a_radian);
}

inline double sin_gra(const double a_grad)
{
  double rad = gra_to_rad(a_grad);
  return sin(rad);
}

inline double cos_deg(const double a_degree)
{
  double rad = deg_to_rad(a_degree);
  return cos(rad);
}

inline double cos_rad(const double a_radian)
{
  return cos(a_radian);
}

inline double cos_gra(const double a_grad)
{
  double rad = gra_to_rad(a_grad);
  return cos(rad);
}

inline double tan_deg(const double a_degree)
{
  double rad = deg_to_rad(a_degree);
  return tan(rad);
}

inline double tan_rad(const double a_radian)
{
  return tan(a_radian);
}

inline double tan_gra(const double a_grad)
{
  double rad = gra_to_rad(a_grad);
  return tan(rad);
}

inline double asin_deg(const double a_value)
{
  double radian = asin(a_value);
  return rad_to_deg(radian);
}

inline double asin_rad(const double a_value)
{
  return asin(a_value);
}

inline double asin_gra(const double a_value)
{
  double radian = asin(a_value);
  return rad_to_gra(radian);
}

inline double acos_deg(const double a_value)
{
  double radian = acos(a_value);
  return rad_to_deg(radian);
}

inline double acos_rad(const double a_value)
{
  return acos(a_value);
}

inline double acos_gra(const double a_value)
{
  double radian = acos(a_value);
  return rad_to_gra(radian);
}

inline double atan_deg(const double a_value)
{
  double radian = atan(a_value);
  return rad_to_deg(radian);
}

inline double atan_rad(const double a_value)
{
  return atan(a_value);
}

inline double atan_gra(const double a_value)
{
  double radian = atan(a_value);
  return rad_to_gra(radian);
}

inline int gcd(int a, int b)
{
  int c;
  while (b) {
    c = a % b;
    a = b;
    b = c;
  }
  return abs(a);
}

inline int lcm(int a, int b)
{
  return (a * b) / gcd(a, b);
}

}; // namespace irs
//---------------------------------------------------------------------------
#endif
