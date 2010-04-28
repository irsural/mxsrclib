// Алгоритмы
// Дата: 24.04.2010
// Ранняя дата: 2.09.2009

#ifndef IRSALGH
#define IRSALGH

// Номер файла
#define IRSALGH_IDX 10

#include <irsdefs.h>

#include <irserror.h>
#include <mxdata.h>

#include <irsfinal.h>

// Реализация алгоритма кольцевого буфера
class alg_ring {
  // Текущее начало буфера
  irs_u32 f_begin;
  // Текущая длина буфера
  irs_u32 f_size;
  // Максимальная длина буфера
  irs_u32 f_size_max;
  // Кольцевой буфер
  void **f_buf_ring;
  // Ошибка в конструкторе
  irs_bool f_create_error;
  // Запрет конструктора по умолчанию
  alg_ring();
  // Увеличение начала буфера на 1 с учетом границы
  void begin_inc();
  // Вычисление конца буфера
  irs_u32 get_end();
public:
  // Конструктор
  alg_ring(int size_max);
  //Деструктор
  ~alg_ring();
  // Чтение и удаление первого элемента буфера
  void *read_and_remove_first();
  // Добавление нового элемента в конец буфера
  void *add_last_and_get_removed(void *buf_elem);
  // Ошибка в конструкторе
  irs_bool create_error();
  // Чтение длины буфера
  irs_u32 get_size() { return f_size; }
  // Чтение элемента буфера
  void *operator[](irs_u32 index);
};

namespace irs {

// Расчет СКО
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

// Версия sko_calc_t для sko_calc_t<double, double>
// Т. к. C++ Builder 4 криво работает с параметрами шаблона по умолчанию
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
// Зеркальный табличный метод расчета crc32
// Заркальный метод является наиболее распространенным
// Необходимый объем оперативной памяти не менее 1024 байт
// Значительный прирост в скорости вычисления по сравнению с нетабличным
// методом
irs_u32 crc32_table(const irs_u8* ap_buf, const size_t a_size);

// T должно быть 32 битным
// Алгоритм является зеркальным, но не совпадает с распространенными
// алгоритмами. В распространенных алгоритмах инвертируется выходное значение
// Функция используется для внутренних потребностей и переделыватся не будет.
template <class T>
irs_u32 crc32(T *a_buf, irs_uarc a_start, irs_uarc a_cnt)
{ //  Взято с википедии
  IRS_STATIC_ASSERT(sizeof(T) == 4);
  irs_uarc top = a_start + a_cnt;
  irs_u32 crc = 0xFFFFFFFF; /* Начальное состояние сдвигового регистра */
  irs_u8 flag = 0;
  for (irs_uarc i = a_start; i < top; i++)
  {
    crc = crc^a_buf[i];
    for (irs_uarc j = 1; j <= 32; j++)
    {
      flag = IRS_LOBYTE(crc) & irs_u8(1);
      crc >>= 1; /* Сдвиг регистра на 1 позицию */
      if (flag) crc ^= 0xEDB88320;    //  0xEDB88320 - примитивный полином
    }
  }
  return(crc); /*Здесь должно быть return (~crc)*/
}

template <class T>
inline irs_u32 crc32(T *a_buf, size_t a_size)
{
  return crc32(a_buf, 0, a_size);
}

irs_u8 crc8(irs_u8 *a_buf, irs_u8 a_start, irs_u8 a_cnt);

// Приведение фазы к диапазону a_phase_begin - a_phase_end
double phase_normalize(double a_phase_in, double a_phase_begin = 0.,
  double a_phase_end = 360.);
// Приведение фазы к диапазону -180 - +180
double phase_normalize_180(double a_phase_in);

// функция интерполяции массива точек прямой методом МНК.
// Записывает в a_korf_k и a_koef_b коэффициенты прямой y=a_korf_k*x+a_koef_b,
// апроксимирующей набор точек, представленный массивами a_array_x и a_array_y,
// размера a_size
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
// Вычисляет коэффициенты k и b прямой y = k*x+b
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

// Функция возвращает среднее арифметическое значение элементов массива
// Аналог функции Mean из библиотеки Math.hpp C++ Builder6
double mean(const double* ap_data, const int a_data_size);

// Функция возвращает дисперсию элементов массива
// Аналог функции Variance из библиотеки Math.hpp C++ Builder6
double variance(const double* ap_data, const int a_data_size);

// Функция возвращает среднее квадратическое отклонение элементов массива
// Аналог функции StdDev из библиотеки Math.hpp C++ Builder6
double std_dev(const double* ap_data, const int a_data_size);

} //namespace irs

#endif //IRSALGH

