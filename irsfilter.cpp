// Реализация БИФ фильтра
// Дата: 09.09.2010
// Дата создания: 01.09.2010

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irsfilter.h>

#include <irsfinal.h>

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

struct p_data_t
{
  enum { size = 11 };
  double data[size];
  p_data_t()
  {
    double p_src[] =
    {
      1.37982864606273237150E-4,
      2.28025724005875567385E-3,
      7.97404013220415179367E-3,
      9.85821379021226008714E-3,
      6.87489687449949877925E-3,
      6.18901033637687613229E-3,
      8.79078273952743772254E-3,
      1.49380448916805252718E-2,
      3.08851465246711995998E-2,
      9.65735902811690126535E-2,
      1.38629436111989062502E0
    };
    irs::memcpyex(data, p_src, IRS_ARRAYSIZE(p_src));
  }
};

double* irs::get_p()
{

  handle_t<p_data_t> p_data(new p_data_t());
  return p_data->data;
}

struct q_data_t
{
  enum { size = 11 };
  double data[size];
  q_data_t()
  {

    double q_src[] =
    {
      2.94078955048598507511E-5,
      9.14184723865917226571E-4,
      5.94058303753167793257E-3,
      1.54850516649762399335E-2,
      2.39089602715924892727E-2,
      3.01204715227604046988E-2,
      3.73774314173823228969E-2,
      4.88280347570998239232E-2,
      7.03124996963957469739E-2,
      1.24999999999870820058E-1,
      4.99999999999999999821E-1
    };
    IRS_LIB_ASSERT(size == IRS_ARRAYSIZE(q_src));
    irs::memcpyex(data, q_src, IRS_ARRAYSIZE(q_src));
  }
};

double* irs::get_q()
{  
  handle_t<q_data_t> q_data(new q_data_t());
  return q_data->data;
}    

double irs::get_c1()
{
  return 1.3862943611198906188E0; /* log(4) */
}

#endif // IRS_FULL_STDCPPLIB_SUPPORT

