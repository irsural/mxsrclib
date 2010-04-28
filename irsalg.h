// ���������
// ����: 24.04.2010
// ������ ����: 2.09.2009

#ifndef IRSALGH
#define IRSALGH

// ����� �����
#define IRSALGH_IDX 10

#include <irsdefs.h>

#include <irserror.h>
#include <mxdata.h>

#include <irsfinal.h>

// ���������� ��������� ���������� ������
class alg_ring {
  // ������� ������ ������
  irs_u32 f_begin;
  // ������� ����� ������
  irs_u32 f_size;
  // ������������ ����� ������
  irs_u32 f_size_max;
  // ��������� �����
  void **f_buf_ring;
  // ������ � ������������
  irs_bool f_create_error;
  // ������ ������������ �� ���������
  alg_ring();
  // ���������� ������ ������ �� 1 � ������ �������
  void begin_inc();
  // ���������� ����� ������
  irs_u32 get_end();
public:
  // �����������
  alg_ring(int size_max);
  //����������
  ~alg_ring();
  // ������ � �������� ������� �������� ������
  void *read_and_remove_first();
  // ���������� ������ �������� � ����� ������
  void *add_last_and_get_removed(void *buf_elem);
  // ������ � ������������
  irs_bool create_error();
  // ������ ����� ������
  irs_u32 get_size() { return f_size; }
  // ������ �������� ������
  void *operator[](irs_u32 index);
};

namespace irs {

// ������ ���
//template <class data_t = double, class calc_t = double>
template <class data_t, class calc_t>
class sko_calc_t
{
  alg_ring m_val_ring;
  calc_t m_sum;

  sko_calc_t();
public:
  sko_calc_t(irs_u32 a_count);
  ~sko_calc_t();
  void clear();
  void add(data_t a_val);
  operator data_t();
  data_t relative();
  data_t average();
};

// ������ sko_calc_t ��� sko_calc_t<double, double>
// �. �. C++ Builder 4 ����� �������� � ����������� ������� �� ���������
class sko_calc_dbl_t: public sko_calc_t<double, double>
{
public:
  sko_calc_dbl_t(irs_u32 a_count):
    sko_calc_t<double, double>(a_count)
  {
  }
private:
  sko_calc_dbl_t();
};

} // namespace irs

template <class data_t, class calc_t>
irs::sko_calc_t<data_t, calc_t>::sko_calc_t(irs_u32 a_count):
  m_val_ring(a_count),
  m_sum(0.)
{}
template <class data_t, class calc_t>
irs::sko_calc_t<data_t, calc_t>::~sko_calc_t()
{
  clear();
}
template <class data_t, class calc_t>
void irs::sko_calc_t<data_t, calc_t>::clear()
{
  void *p_rem_val = m_val_ring.read_and_remove_first();
  while (p_rem_val) {
    data_t *dp_rem_val = (data_t *)p_rem_val;
    IRS_LIB_DELETE_ASSERT(dp_rem_val);
    p_rem_val = m_val_ring.read_and_remove_first();
  }
  m_sum = 0.;
}
template <class data_t, class calc_t>
void irs::sko_calc_t<data_t, calc_t>::add(data_t a_val)
{
  m_sum += a_val;

  void *p_val = (void *)IRS_LIB_NEW_ASSERT(data_t, IRSALGCPP_IDX);
  *(data_t *)p_val = a_val;
  void *p_rem_val = m_val_ring.add_last_and_get_removed(p_val);

  if (p_rem_val) {
    data_t *dp_rem_val = (data_t *)p_rem_val;
    data_t rem_val = *dp_rem_val;
    IRS_LIB_DELETE_ASSERT(dp_rem_val);

    m_sum -= rem_val;
  }
}
template <class data_t, class calc_t>
irs::sko_calc_t<data_t, calc_t>::operator data_t()
{
  irs_u32 size = m_val_ring.get_size();
  if (size) {
    calc_t square_sum = 0.;
    calc_t average = m_sum/size;
    for (irs_u32 i = 0; i < size; i++) {
      void *p_val = m_val_ring[i];
      if (p_val) {
        calc_t val = *(calc_t *)p_val;
        val -= average;
        square_sum += val*val;
      }
    }
    return sqrt(square_sum/size);
  }
  return 0.;
}
template <class data_t, class calc_t>
data_t irs::sko_calc_t<data_t, calc_t>::relative()
{
  irs_u32 size = m_val_ring.get_size();
  if (size) {
    calc_t average = m_sum/size;
    return *this/average;
  }
  return 0.;
}
template <class data_t, class calc_t>
data_t irs::sko_calc_t<data_t, calc_t>::average()
{
  return m_sum / m_val_ring.get_size();
}

namespace irs {

struct crc32_data_t
{
  enum {
    size = 256
  };
  irs_u32 table[size];
  crc32_data_t();
};
// ���������� ��������� ����� ������� crc32
// ���������� ����� �������� �������� ����������������
// ����������� ����� ����������� ������ �� ����� 1024 ����
// ������������ ������� � �������� ���������� �� ��������� � �����������
// �������
irs_u32 crc32_table(const irs_u8* ap_buf, const size_t a_size);

// T ������ ���� 32 ������
// �������� �������� ����������, �� �� ��������� � �����������������
// �����������. � ���������������� ���������� ������������� �������� ��������
// ������� ������������ ��� ���������� ������������ � ������������� �� �����.
template <class T>
irs_u32 crc32(T *a_buf, irs_uarc a_start, irs_uarc a_cnt)
{ //  ����� � ���������
  IRS_STATIC_ASSERT(sizeof(T) == 4);
  irs_uarc top = a_start + a_cnt;
  irs_u32 crc = 0xFFFFFFFF; /* ��������� ��������� ���������� �������� */
  irs_u8 flag = 0;
  for (irs_uarc i = a_start; i < top; i++)
  {
    crc = crc^a_buf[i];
    for (irs_uarc j = 1; j <= 32; j++)
    {
      flag = IRS_LOBYTE(crc) & irs_u8(1);
      crc >>= 1; /* ����� �������� �� 1 ������� */
      if (flag) crc ^= 0xEDB88320;    //  0xEDB88320 - ����������� �������
    }
  }
  return(crc); /*����� ������ ���� return (~crc)*/
}

template <class T>
inline irs_u32 crc32(T *a_buf, size_t a_size)
{
  return crc32(a_buf, 0, a_size);
}

irs_u8 crc8(irs_u8 *a_buf, irs_u8 a_start, irs_u8 a_cnt);

// ���������� ���� � ��������� a_phase_begin - a_phase_end
double phase_normalize(double a_phase_in, double a_phase_begin = 0.,
  double a_phase_end = 360.);
// ���������� ���� � ��������� -180 - +180
double phase_normalize_180(double a_phase_in);

// ������� ������������ ������� ����� ������ ������� ���.
// ���������� � a_korf_k � a_koef_b ������������ ������ y=a_korf_k*x+a_koef_b,
// ��������������� ����� �����, �������������� ��������� a_array_x � a_array_y,
// ������� a_size
template<class TYPE>
void interp_line_mnk(
  TYPE* a_array_x,
  TYPE* a_array_y,
  const unsigned int& a_size,
  long double& a_koef_k,
  long double& a_koef_b)
{
  long double S_xy = 0.0, S_x2 = 0.0, S_x = 0.0, S_y = 0.0;
  for(unsigned int i = 0; i < a_size; i++)
  {
    S_xy = S_xy+a_array_x[i]*a_array_y[i];
    S_x2 = S_x2+a_array_x[i]*a_array_x[i];
    S_x = S_x+a_array_x[i];
    S_y = S_y+a_array_y[i];
  }
  a_koef_k = (S_xy-S_x*S_y/a_size)/(S_x2-S_x*S_x/a_size);
  a_koef_b = (S_y-a_koef_k*S_x)/a_size;
}
// ��������� ������������ k � b ������ y = k*x+b
template<class TYPE>
int koef_line(
  const TYPE a_y1,
  const TYPE a_x1,
  const TYPE a_y2,
  const TYPE a_x2,
  TYPE& a_k,
  TYPE& a_b)
{
  if(a_x1 == a_x2)
    return 1;
  TYPE difference = a_x1 - a_x2;
  a_k = (a_y1 - a_y2)/difference;
  a_b = (a_y2*a_x1 - a_x2*a_y1)/difference;
  return 0;
}

// ������� ���������� ������� �������������� �������� ��������� �������
// ������ ������� Mean �� ���������� Math.hpp C++ Builder6
double mean(const double* ap_data, const int a_data_size);

// ������� ���������� ��������� ��������� �������
// ������ ������� Variance �� ���������� Math.hpp C++ Builder6
double variance(const double* ap_data, const int a_data_size);

// ������� ���������� ������� �������������� ���������� ��������� �������
// ������ ������� StdDev �� ���������� Math.hpp C++ Builder6
double std_dev(const double* ap_data, const int a_data_size);

} //namespace irs

#endif //IRSALGH

