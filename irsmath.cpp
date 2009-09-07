//---------------------------------------------------------------------------
// Математические функции
// Дата: 2.02.2009

#include <irsmath.h>

int irs::compare_value(
  const double& a_a, const double& a_b, const double& a_epsilon)
{
  if (!((a_a == 0.) && (a_b == 0.))) {
    int result_compare = equals_value;
    double difference = (a_a-a_b)/max(fabs(a_a), fabs(a_b));
    if ((fabs(difference) < a_epsilon)){
      result_compare = irs::equals_value;
    } else {
      result_compare = (a_a > a_b) ? greater_than_value : less_than_value;
    }
    return result_compare;
  } else {
    return irs::equals_value;
  }
}
//---------------------------------------------------------------------------


