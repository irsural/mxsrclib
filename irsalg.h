//! \file
//! \ingroup signal_processing_group
//! \brief Алгоритмы
//!
//! Дата: 09.09.2011\n
//! Ранняя дата: 2.09.2009

#ifndef IRSALGH
#define IRSALGH

// Номер файла
#define IRSALGH_IDX 10

#include <irsdefs.h>

#include <irserror.h>
#include <mxdata.h>
#include <irslimits.h>

#include <irsfinal.h>

//! \addtogroup signal_processing_group
//! @{

// Реализация алгоритма кольцевого буфера
class alg_ring {
  //! \brief Текущее начало буфера
  irs_u32 f_begin;
  //! \brief Текущая длина буфера
  irs_u32 f_size;
  //! \brief Максимальная длина буфера
  irs_u32 f_size_max;
  //! \brief Кольцевой буфер
  void **f_buf_ring;
  //! \brief Ошибка в конструкторе
  irs_bool f_create_error;

  //! \brief Запрет конструктора по умолчанию
  alg_ring();
  //! \brief Увеличение начала буфера на 1 с учетом границы
  void begin_inc();
  //! \brief Вычисление конца буфера
  irs_u32 get_end();

public:
  //! \brief Конструктор
  alg_ring(int size_max);
  //! brief Деструктор
  ~alg_ring();
  //! brief Чтение и удаление первого элемента буфера
  void *read_and_remove_first();
  //! \brief Добавление нового элемента в конец буфера
  void *add_last_and_get_removed(void *buf_elem);
  //! \brief Ошибка в конструкторе
  irs_bool create_error();

  //! \brief Чтение длины буфера
  irs_u32 get_size()const {
    return f_size;
  }
  //! \brief Чтение элемента буфера
  void *operator[](irs_u32 index)const;
};

//! @}
//------------------------------------------------------------------------------
namespace irs {

//! \addtogroup signal_processing_group
//! @{

//! \brief Расчитывает СКО и среднее значение входной последовательности. СКО
//!   рассчитывается по формуле
//!   \f$\sqrt{\frac{1}{N}\sum_{i=1}^{N}(x_i-\bar{x})^2}\f$
//! \param[in] ap_first - итератор на первую позицию последовательности
//! \param[in] ap_last - итератор на последнюю позицию последовательности,
//!   последний элемент не входит в диапазон обработки
//! \param[out] ap_result - переменная, в которую будет помещено значение СКО
//! \param[out] ap_average - переменная, в которую будет помещено среднее
//!   значение. Можно не передавать указатель, если среднее значение не
//!   требуется
//! \see sample_standard_deviation
//! \see sko_calc_t
//! \see fast_sko_t
//! \see fast_multi_sko_with_single_average_t
//! \see fast_multi_sko_t
template <class forward_iterator, class T>
void standard_deviation(forward_iterator ap_first, forward_iterator ap_last,
  T* ap_result, T* ap_average = NULL,
  bool a_use_sample_standard_deviation = false)
{
  const size_t size = distance(ap_first, ap_last);
  if (size == 0) {
    *ap_result = 0;
    if (ap_average) {
      *ap_average = 0;
    }
    return;
  }

  const T sum = accumulate(ap_first, ap_last,  0.0);

  const double average = sum/size;
  if (ap_average) {
    *ap_average = average;
  }

  T square_sum = 0;
  while (ap_first != ap_last) {
    const T value = *ap_first - average;
    square_sum += value*value;
    ++ap_first;
  }
  if (a_use_sample_standard_deviation && (size > 1)) {
    *ap_result = sqrt(square_sum/(size - 1));
  } else {
    *ap_result = sqrt(square_sum/size);
  }
}

//! \brief Расчитывает СКО и среднее значение входной последовательности.
//!   СКО рассчитывается по формуле
//!   \f$\sqrt{\frac{1}{N-1}\sum_{i=1}^{N}(x_i-\bar{x})^2}\f$
//! \param[in] ap_first - итератор на первую позицию последовательности
//! \param[in] ap_last - итератор на последнюю позицию последовательности,
//!   последний элемент не входит в диапазон обработки
//! \param[out] ap_result - переменная, в которую будет помещено значение СКО
//! \param[out] ap_average - переменная, в которую будет помещено среднее
//!   значение. Можно не передавать указатель, если среднее значение не
//!   требуется
//! \see standard_deviation
//! \see sko_calc_t
//! \see fast_sko_t
//! \see fast_multi_sko_with_single_average_t
//! \see fast_multi_sko_t
template <class forward_iterator, class T>
void sample_standard_deviation(forward_iterator ap_first,
  forward_iterator ap_last,
  T* ap_result, T* ap_average = NULL)
{
  const bool use_sample_standatd_deviation = true;
  return standard_deviation<forward_iterator, T>(
    ap_first, ap_last, ap_result, ap_average, use_sample_standatd_deviation);
}

//! \brief Доверительная вероятность
enum confidence_level_t {
  confidence_level_0_95,
  confidence_level_0_99,
  confidence_level_0_999
};

//! \brief Уровень значимости
enum level_of_significance_t {
  level_of_significance_0_01,
  level_of_significance_0_05,
  level_of_significance_0_1
};

//! \brief Возвращает двустороннее обратное t-распределение Стьюдента
//! \param[in] a_probability доверительная вероятность.
//!   Реализовано только для значений 0.95, 0.99, 0.999
//! \param[in] a_degrees_of_freedom количество степеней свободы
//! \return Значение распределения
double student_t_inverse_distribution_2x(
  confidence_level_t a_confidence_level,
  size_t a_degrees_of_freedom);

//! \brief Возвращает критическое значение критерия ta
//! \details
//! \param[in] a_level_of_significance уровень значимости.
//!   Реализовано только для значений 0.1, 0.05, 0.01
//! \param[in] a_sample_size размер последовательности. Для значений
//!   меньше 3, возвращается значение для 3. Для значений больше 25,
//!   возвращается приближенное значение
//! \return Критическое значение
double critical_values_for_t_a_criterion(
  level_of_significance_t a_level_of_significance,
  size_t a_sample_size);

//! \brief Возвращает критическое значение критерия Смирнова
//! \param[in] a_level_of_significance уровень значимости.
//!   Реализовано только для значений 0.1, 0.05, 0.01
//! \param[in] a_sample_size размер последовательности.
//!   Реализовано только для значений от 3 до 25. Если значение выходит
//!   за диапазон, то выбирается ближайшее граничное значение
//! \return Критическое значение
double critical_values_for_smirnov_criterion(
  level_of_significance_t a_level_of_significance,
  size_t a_sample_size);

template <class T>
class pred_eliminating_outliers_t
{
public:
  pred_eliminating_outliers_t(T a_sko, T a_average,
    T a_critical_value
  ):
    m_sko(a_sko),
    m_average(a_average),
    m_critical_value(a_critical_value)
  {
  }
  bool operator() (T a_value) const
  {
    return fabs(a_value - m_average)/m_sko > m_critical_value;
  }
private:
  pred_eliminating_outliers_t();
  const T m_sko;
  const T m_average;
  const T m_critical_value;
};

//! \brief Обнаруживает выбросы по критерию t_a, переносит их в
//!   конец контейнера и возвращает новую границу диапазона. Однопроходный
//!   вариант алгоритма
//! \details Данный способ удаления выбросов применяется для случаев, когда
//!   среднеквадратическое отклонение заранее известно или оно рассчитано
//!   по отсчетам без выбросов. Если же оно заранее не известно, то следует
//!   использовать функцию eliminating_outliers_smirnov_criterion.
//!   Подробнее об исключении выбросов можно почитать
//!   [сдесь] (http://termist.com/laborat/stat/otsew/stepnow_01.htm)
//! \par Алгоритм
//!   По переданным параметрам проверяется каждый элемент последовательности
//!   и если он является выбросом, то переносится в конец последовательности.
//!   Функция возвращает итератор на элемент, следующий за последним элементом,
//!   не являющимся выбросом.
//! \param[in] ap_first итератор на первую позицию последовательности
//! \param[in] ap_last итератор на последнюю позицию последовательности,
//!   последний элемент не входит в диапазон обработки
//! \param[in] a_level_of_significance уровень значимости ошибки
//! \param[in] a_standard_deviation среднеквадратическое отклонение, известное
//!   заранее или рассчитанное по отсчетам, не содержащим выбросов
//! \return итератор на элемент, следующий за последним неперенесенным элементом
//! \see irs::eliminating_outliers_t_a_criterion(
//!   forward_iterator, forward_iterator, level_of_significance_t, T, T)
template <class forward_iterator, class T>
forward_iterator eliminating_outliers_t_a_criterion(
  forward_iterator ap_first,
  forward_iterator ap_last,
  level_of_significance_t a_level_of_significance,
  T a_standard_deviation)
{
  const level_of_significance_t level = a_level_of_significance;
  const size_t size = distance(ap_first, ap_last);
  double critical_value = 0;
  if (size < 3) {
    return ap_last;
  } else {
    critical_value = critical_values_for_t_a_criterion(level, size);
  }
  const T average = accumulate(ap_first, ap_last, T(0))/size;
  pred_eliminating_outliers_t<T> pred(a_standard_deviation, average,
    critical_value);
  return remove_if(ap_first, ap_last, pred);
}

//! \brief Обнаруживает выбросы по критерию t_a, переносит их в
//!   конец контейнера и возвращает новую границу диапазона. Однопроходный
//!   вариант алгоритма. Это перегруженный вариант функции
//!   irs::eliminating_outliers_t_a_criterion(
//!   forward_iterator, forward_iterator, level_of_significance_t, T),
//!   позволяющий передать среднее значение, если оно уже рассчитано
//! \param[in] ap_first итератор на первую позицию последовательности
//! \param[in] ap_last итератор на последнюю позицию последовательности,
//!   последний элемент не входит в диапазон обработки
//! \param[in] a_level_of_significance уровень значимости ошибки
//! \param[in] a_standard_deviation среднеквадратическое отклонение, известное
//!   заранее или рассчитанное по отсчетам, не содержащим выбросов
//! \param[in] a_average среднее значение, рассчитанное по последовательности,
//!   которая может содержать выбросы
//! \return итератор на элемент, следующий за последним неперенесенным элементом
//! \see eliminating_outliers_t_a_criterion(
//!   forward_iterator, forward_iterator, level_of_significance_t, T)
template <class forward_iterator, class T>
forward_iterator eliminating_outliers_t_a_criterion(
  forward_iterator ap_first,
  forward_iterator ap_last,
  level_of_significance_t a_level_of_significance,
  T a_standard_deviation,
  T a_average)
{
  const level_of_significance_t level = a_level_of_significance;
  const size_t size = distance(ap_first, ap_last);
  double critical_value = 0;
  if (size < 3) {
    return ap_last;
  } else {
    critical_value = critical_values_for_t_a_criterion(level, size);
  }
  pred_eliminating_outliers_t<T> pred(a_standard_deviation, a_average,
    critical_value);
  return remove_if(ap_first, ap_last, pred);
}

//! \brief Возвращает \c true, если a_value является выбросом по
//!   критерию 3 сигрма
//! \details Эта функция используется, если СКО и среднее значение
//!   заранее известны
//! \par Пример использования
//! \code{.cpp}
//!   // СКО и среднее значение известны и вычислены без учета выбросов
//!   const double standard_deviation = 10;
//!   const double average = 1;
//!   std::vector<double> samples;
//!   // Код получения отсчетов
//!   // ...
//!   // Набор отсчетов без выбросов
//!   std::vector<double> correct_samples;
//!   for (std::size_t i = 0; i < samples.size(); i++) {
//!     if (!is_outlier_3_sigma_criterion(samples[i],
//!         standard_deviation, average)) {
//!       correct_samples.push_back(sample);
//!     }
//!   }
//! \endcode
//! \param[in] a_value тестируемое значение
//! \param[in] a_standard_diviation стандартное отклонение, рассчитанное по
//!   формуле \f$\sqrt{\frac{1}{N-1}\sum_{i=1}^{N}(x_i-\bar{x})^2}\f$.
//!   Последовательность, по которой рассчитывается стандартное
//!   отклонение, не должна включать выбросов
//! \param[in] a_average среднее значение, рассчитанное по
//!   формуле \f$\frac{1}{N}\sum_{i=1}^{N}(x_i)\f$.
//!   Последовательность, по которой рассчитывается среднее
//!   значение, не должна включать выбросов
//! \return \c true, если тестируемое значение является выбросом,
//!   и \c false в противном случае
template <class forward_iterator, class T>
bool is_outlier_3_sigma_criterion(T a_value,
  T a_standard_deviation, T a_average)
{
  return is_outlier_max_diviation_criterion(a_value, a_average,
    3*a_standard_deviation);
}

//! \brief Эта функция перемещает выбросы в конец последовательности.
//!   Каждое значение проверяется с помощью функции
//!   is_outlier_max_diviation_criterion
//! \details СКО и среднее значение должны быть рассчитаны без учета выбросов
//! \param[in] ap_first итератор на первую позицию последовательности
//! \param[in] ap_last итератор на последнюю позицию последовательности,
//!   последний элемент не входит в диапазон обработки
//! \param[in] a_standard_diviation стандартное отклонение, рассчитанное
//!   без учета выбросов
//! \param[in] a_average среднее значение, рассчитанное без учета выбросов
//!   Последовательность, по которой рассчитывается среднее
//!   значение, не должна включать выбросов
template <class forward_iterator, class T>
forward_iterator eliminating_outliers_3_sigma_criterion(
  forward_iterator ap_first,
  forward_iterator ap_last, T a_standard_deviation, T a_average)
{
  return eliminating_outliers_max_diviation_criterion(
    ap_first, ap_last, a_average, 3*a_standard_deviation);
}

//! \brief Эта функция работает аналогично функции is_outlier_3_sigma_criterion,
//!   но вместо СКО принимает максимальное отклонение
template <class forward_iterator, class T>
bool is_outlier_max_diviation_criterion(T a_value,
  T a_max_diviation, T a_average)
{
  return (fabs(a_value - a_average) > a_max_diviation);
}

//! \brief Эта функция работает аналогично функции
//!   irs::eliminating_outliers_3_sigma_criterion, но вместо СКО принимает
//!   максимальное отклонение
template <class forward_iterator, class T>
forward_iterator eliminating_outliers_max_diviation_criterion(
  forward_iterator ap_first,
  forward_iterator ap_last, T a_max_deviation, T a_average)
{
  forward_iterator it = ap_first;
  forward_iterator result = ap_first;
  while (it != ap_last) {
    const T value = *it;
    if (is_outlier_max_diviation_criterion(value, a_average, a_max_deviation)) {
      *result = *it;
      ++result;
    }
    ++it;
  }
  return result;
}

//! \brief Обнаруживает выбросы по критерию Смирнова, переносит их в
//!   конец контейнера и возвращает новую границу диапазона. Однопроходный
//!   вариант алгоритма
//! \details Данный способ удаления выбросов применяется для случаев, когда
//!   среднеквадратическое отклонение заранее не известно.
//! \par Алгоритм
//!   Выполняется расчет стандартного отклонения и среднего значения. Далее
//!   по рассчитанным параметрам проверяется каждое значение последовательности
//!   и если оно является выбросом, то переносится в конец последовательности.
//!   Функция возвращает итератор на элемент, следующий за последним элементом,
//!   не являющимся выбросом.
//!   Эта функция требует явного указания параметров шаблона, один из которых
//!   является типом вычислений (T). Этот вариант функции следует использовать,
//!   если тип расчета \c double не подходит. Если же элементы имеют тип
//!   \c double или тип меньшей точности, то можно использовать перегруженный
//!   вариант этой функции с одним параметром шаблона,
//!   который не требуется указывать явно
//! \par Пример использования
//! \code{.cpp}
//!   std::vector<double> samples;
//!   // Здесь код заполнения samples
//!   // ...
//!   // Переносим выбросы в конец
//!   std::vector<double>::iterator new_end =
//!     eliminating_outliers_smirnov_criterion<std::vector<double>::iterator,
//!     long double>(samples.begin(), samples.end(),
//!     level_of_significance_0_01);
//!   // Удаляем выбросы
//!   samples.erase(new_end, samples.end());
//! \endcode
//! \param[in] ap_first итератор на первую позицию последовательности
//! \param[in] ap_last итератор на последнюю позицию последовательности,
//!   последний элемент не входит в диапазон обработки
//! \param[in] a_level_of_significance уровень значимости ошибки
//! \return итератор на элемент, следующий за последним неперенесенным элементом
//! \see template <class forward_iterator>
//!   forward_iterator eliminating_outliers_smirnov_criterion(
//!   forward_iterator, forward_iterator, level_of_significance_t)
template <class forward_iterator, class T>
forward_iterator eliminating_outliers_smirnov_criterion(
  forward_iterator ap_first,
  forward_iterator ap_last,
  level_of_significance_t a_level_of_significance)
{
  const level_of_significance_t level = a_level_of_significance;
  const size_t size = distance(ap_first, ap_last);
  double critical_value = 0;
  if (size < 3) {
    return ap_last;
  } else if (size <= 25) {
    critical_value = critical_values_for_smirnov_criterion(level, size);
  } else {
    critical_value = critical_values_for_t_a_criterion(level, size);
  }

  T sko = 0;
  T average = 0;
  sample_standard_deviation(ap_first, ap_last, &sko, &average);

  pred_eliminating_outliers_t<T> pred(sko, average, critical_value);
  return remove_if(ap_first, ap_last, pred);
}

//! \brief Это перегруженный вариант функции
//!   template <class forward_iterator, class T>
//!   forward_iterator eliminating_outliers_smirnov_criterion(
//!     forward_iterator, forward_iterator,level_of_significance_t).
//!   Позволяет не указывать явно тип, который будет использован для расчетов.
//!   Для расчетов используется тип \c double
//! \details
//! \par Пример использования
//! \code{.cpp}
//!   std::vector<double> samples;
//!   // Здесь код заполнения samples
//!   // ...
//!   // Переносим выбросы в конец
//!   std::vector<double>::iterator new_end =
//!     eliminating_outliers_smirnov_criterion(samples.begin(), samples.end(),
//!     level_of_significance_0_01);
//!   // Удаляем выбросы
//!   samples.erase(new_end, samples.end());
//! \endcode
template <class forward_iterator>
forward_iterator eliminating_outliers_smirnov_criterion(
  forward_iterator ap_first,
  forward_iterator ap_last,
  level_of_significance_t a_level_of_significance)
{
  return eliminating_outliers_smirnov_criterion<forward_iterator, double>(
    ap_first, ap_last, a_level_of_significance);
}

//! \brief Возвращает \c true, если a_value является выбросом по
//!   критерию Смирнова
//! \details Эту функцию следует использовать, если не известно заранее
//!   значение среднеквадратического отклонения и среднего значения. Функции
//!   необходимо передать рассчитанные значения стандартного
//!   отклонения и среднего значения последовательности. Их можно рассчитать
//!   с использованием функции sample_standard_deviation или классов:
//!   sko_calc_t, fast_multi_sko_with_single_average_t, fast_multi_sko_t.
//!   Функция начинает детектировать выбросы для последовательности из трех и
//!   более отсчетов. Для последовательности размером до 25 отсчетов
//!   испльзуется критерий Смирнова, а для размера более 25 используется t_a
//!   критерий.
//! \par Пример использования
//! \code{.cpp}
//!   irs::sko_calc_t sko_calc(100);
//!   std::vector<double> correct_samples;
//!   for (int i = 0; i < 1000; i++) {
//!     // Здесь код ожидания нового отсчета
//!     // ...
//!     // Получение нового отсчета, например с АЦП
//!     const double sample = get_sample_from_adc();
//!     sko_calc.add(sample);
//!     const double sko = sko_calc;
//!     const average = sko_calc.average();
//!     if (!is_outlier_smirnov_criterion(sample, 0.01, sko_calc.size(),
//!         sko, average)) {
//!       // Это значение не является выбросом
//!       correct_samples.push_back(sample);
//!     }
//!   }
//! \endcode
//! \param[in] a_value тестируемое значение
//! \param[in] a_level_of_significance уровень значимости ошибки
//! \param[in] a_size размер выборки
//! \param[in] a_standard_diviation стандартное отклонение, рассчитанное по
//!   формуле \f$\sqrt{\frac{1}{N-1}\sum_{i=1}^{N}(x_i-\bar{x})^2}\f$.
//!   Последовательность, по которой рассчитывается стандартное
//!   отклонение, должна включать и тестируемое значение
//! \param[in] a_average среднее значение, рассчитанное по
//!   формуле \f$\frac{1}{N}\sum_{i=1}^{N}(x_i)\f$.
//!   Последовательность, по которой рассчитывается среднее
//!   значение, должна включать и тестируемое значение
//! \return \c true, если тестируемое значение является выбросом,
//!   и \c false в противном случае
template <class forward_iterator, class T>
bool is_outlier_smirnov_criterion(T a_value,
  level_of_significance_t a_level_of_significance,
  size_t a_size, T a_standard_diviation,
  T a_average)
{
  const level_of_significance_t level = a_level_of_significance;
  double critical_value = 0;
  if (a_size < 3) {
    return false;
  } else if (a_size <= 25) {
    critical_value = critical_values_for_smirnov_criterion(level, a_size);
  } else {
    critical_value = critical_values_for_t_a_criterion(level, a_size);
  }
  return (abs(a_value - a_average)/a_standard_diviation > critical_value);
}

//! \brief Обнаруживает выбросы по критерию Смирнова, переносит их в
//!   конец контейнера и возвращает новую границу диапазона. Многопроходный
//!   вариант алгоритма
//! \details Данный способ удаления выбросов применяется для случаев, когда
//!   среднеквадратическое отклонение заранее не известно.
//!   Алгоритм.
//!   Выполняется поиск наибольшего выброса по критерию Н.В. Смирнова. Если
//!   выброс обнаружен, он переносится в конец последовательности. После этого
//!   происходит пересчет параметров и процедура поиска и удаления наибольшего
//!   выброса повторяется. Этот процесс повторяется до тех пор, пока все
//!   выбросы не будут перенесены в конец. Функция возвращает итератор на
//!   элемент, следующий за последним элементом, не являющимся выбросом
//! \param[in] ap_first итератор на первую позицию последовательности
//! \param[in] ap_last итератор на последнюю позицию последовательности,
//!   последний элемент не входит в диапазон обработки
//! \param[in] a_level_of_significance уровень значимости ошибки.
//!   Реализовано только для значений 0.1, 0.05, 0.01
//! \return итератор на элемент, следующий за последним неперенесенным элементом
template <class forward_iterator, class T>
forward_iterator eliminating_outliers_smirnov_criterion_multiple_pass(
  forward_iterator ap_first,
  forward_iterator ap_last,
  level_of_significance_t a_level_of_significance)
{
  typedef iterator_traits<forward_iterator> traits;
  typedef typename traits::value_type value_type;

  const level_of_significance_t level = a_level_of_significance;
  multimap<value_type, forward_iterator> values_map;

  forward_iterator current = ap_first;
  while (current != ap_last) {
    values_map.insert(make_pair(fabs(*current), current));
    ++current;
  }

  if (values_map.size() < 3) {
    return ap_last;
  }

  typename multimap<value_type, forward_iterator>::const_iterator end_it_map =
    values_map.end();
  set<forward_iterator> bad_elements;
  while (end_it_map != values_map.begin()) {
    T sko = 0;
    T average = 0;
    vector<value_type> values;
    values.reserve(values_map.size());
    typename multimap<T, forward_iterator>::const_iterator it =
      values_map.begin();
    while (it != end_it_map) {
      values.push_back(*it->second);
      ++it;
    }
    const size_t size = values.size();
    sample_standard_deviation<forward_iterator, T>(
      values.begin(), values.end(), &sko, &average);
    --end_it_map;
    const value_type value = *end_it_map->second;
    double critical_value = 0;
    if (size < 3) {
      break;
    } else if (size <= 25) {
      critical_value = critical_values_for_smirnov_criterion(level, size);
    } else {
      critical_value = critical_values_for_t_a_criterion(level, size);
    }
    if (sko == 0) {
      break;
    }
    if (fabs(value - average)/sko > critical_value) {
      bad_elements.insert(end_it_map->second);
    } else {
      break;
    }
  }

  forward_iterator it = ap_first;
  forward_iterator result = ap_first;
  while (it != ap_last) {
    if (bad_elements.find(it) == bad_elements.end()) {
      *result = *it;
      ++result;
    }
    ++it;
  }
  return result;
}

template <class forward_iterator>
forward_iterator eliminating_outliers_smirnov_criterion_multiple_pass(
  forward_iterator ap_first,
  forward_iterator ap_last,
  level_of_significance_t a_level_of_significance)
{
  return eliminating_outliers_smirnov_criterion_multiple_pass
    <forward_iterator, double>(ap_first, ap_last, a_level_of_significance);
}

//! \brief Расчет СКО
// template <class data_t = double, class calc_t = double>
template<class data_t, class calc_t>
class sko_calc_t {
  irs_u32 m_count;
  deque<data_t> m_val_array;
  calc_t m_average;
  bool m_accepted_average;
  calc_t sum()const;
  sko_calc_t();
public:
  sko_calc_t(irs_u32 a_count);
  ~sko_calc_t();
  void clear();
  void add(data_t a_val);
  operator data_t()const;
  data_t relative()const;
  data_t average()const;
  void resize(irs_u32 a_count);
  void set_average(data_t a_average);
  void clear_average();
  irs_u32 size();
};

// Версия sko_calc_t для sko_calc_t<double, double>
// Т. к. C++ Builder 4 криво работает с параметрами шаблона по умолчанию
class sko_calc_dbl_t : public sko_calc_t<double, double> {
public:
  sko_calc_dbl_t(irs_u32 a_count) : sko_calc_t<double, double>(a_count) {
  }
private:
  sko_calc_dbl_t();
};

//! @}

} // namespace irs

template<class data_t, class calc_t>
irs::sko_calc_t<data_t, calc_t>::sko_calc_t(irs_u32 a_count):
  m_count((a_count >= 1)?a_count:1),
  m_val_array(),
  m_average(0),
  m_accepted_average(false)
{
}

template<class data_t, class calc_t>
irs::sko_calc_t<data_t, calc_t>::~sko_calc_t() {
  clear();
}

template<class data_t, class calc_t>
calc_t irs::sko_calc_t<data_t, calc_t>::sum()const {
  const size_t size = m_val_array.size();
  calc_t sum = 0;
  for (size_t i = 0; i < size; i++) {
    sum += m_val_array[i];
  }
  return sum;
}

template<class data_t, class calc_t>
void irs::sko_calc_t<data_t, calc_t>::resize(irs_u32 a_count) {
  m_count = ((a_count >= 1)?a_count:1);
  if (m_val_array.size() > m_count) {
    m_val_array.resize(m_count);
  }
}

template<class data_t, class calc_t>
irs_u32 irs::sko_calc_t<data_t, calc_t>::size() {
  irs_u32 size = static_cast<irs_u32>(m_val_array.size());
  return size;
}

template<class data_t, class calc_t>
void irs::sko_calc_t<data_t, calc_t>::set_average(data_t a_average)
{
  m_average = static_cast<calc_t>(a_average);
  m_accepted_average = true;
}

template<class data_t, class calc_t>
void irs::sko_calc_t<data_t, calc_t>::clear_average()
{
  m_accepted_average = false;
}

template<class data_t, class calc_t>
void irs::sko_calc_t<data_t, calc_t>::clear() {
  m_val_array.resize(0);
}

template<class data_t, class calc_t>
void irs::sko_calc_t<data_t, calc_t>::add(data_t a_val) {
  const size_t size = m_val_array.size();
  if (size < m_count) {
    m_val_array.push_back(a_val);
  }
  else if (size == m_count) {
    m_val_array.pop_front();
    m_val_array.push_back(a_val);
  }
  else if (size > m_count) {
    size_t margin = size - m_count;
    for (size_t i = 0; i < margin; i++) {
      m_val_array.pop_front();
    }
  }
}

template<class data_t, class calc_t>
irs::sko_calc_t<data_t, calc_t>:: operator data_t()const {
  const size_t size = m_val_array.size();
  if (size) {
    calc_t square_sum = 0.;
    calc_t average = 0;
    if (!m_accepted_average) {
      average = sum() / size;
    } else {
      average = m_average;
    }
    for (size_t i = 0; i < size; i++) {
      calc_t val = static_cast<calc_t> (m_val_array[i]);
      val -= average;
      square_sum += val * val;
    }
    calc_t calced_sko = sqrt(square_sum / size);
    return static_cast<data_t>(calced_sko);
  }
  return 0;
}

template<class data_t, class calc_t>
data_t irs::sko_calc_t<data_t, calc_t>::relative() const {
  const size_t size = m_val_array.size();
  if (size) {
    calc_t average = sum() / size;
    calc_t sko = *this;
    if (average != 0) {
      calc_t sko_reltive = sko / average;
      return sko_reltive;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

template<class data_t, class calc_t>
data_t irs::sko_calc_t<data_t, calc_t>::average()const {
  size_t size = m_val_array.size();
  if (size == 0) {
    size = 1;
  }
  return sum() / size;
}

namespace irs {
  template<class data_t, class calc_t>
  calc_t sko_calc(data_t* ap_buff, size_t a_size) {
    calc_t result = 0;
    if ((ap_buff != IRS_NULL) && (a_size != 0)) {
      irs::c_array_view_t<data_t> buff =
        irs::c_array_view_t<data_t>(
        reinterpret_cast<data_t*>(ap_buff), a_size);
      calc_t sum = 0;
      for (size_t i = 0; i < a_size; i++) {
        sum += static_cast<calc_t>(buff[i]);
      }
      calc_t average = sum / a_size;
      calc_t val = 0;
      calc_t square_sum = 0;
      for (size_t i = 0; i < a_size; i++) {
        val = static_cast<calc_t>(buff[i]);
        val -= average;
        square_sum += val * val;
      }
      result = sqrt(square_sum / a_size);
    }
    return result;
  }
} //namespace irs


namespace irs {

  //! \ingroup signal_processing_group
  //! \brief Расчет Дельта
  template<class T>
  class delta_calc_t {
  public:
    delta_calc_t(size_t a_count = 100);
    ~delta_calc_t();
    void add(T a_val);
    T max()const;
    T min()const;
    T absolute()const;
    void clear();
    void resize(size_t a_count);
    T average()const;
    T relative()const;
    size_t size();

  private:
    deque<T>m_array;
    size_t m_count;
  };

} // namespace irs

template<class T>
irs::delta_calc_t<T>::delta_calc_t(size_t a_count) : m_array(),
m_count(a_count) {
}

template<class T>
irs::delta_calc_t<T>::~delta_calc_t() {
}

template<class T>
void irs::delta_calc_t<T>::resize(size_t a_count) {
  m_count = a_count;
  if (m_array.size() > m_count) {
    m_array.resize(m_count);
  }
}

template<class T>
void irs::delta_calc_t<T>::add(T a_val) {
  size_t size = m_array.size();
  if (size < m_count) {
    m_array.push_back(a_val);
  }
  else if (size == m_count) {
    m_array.pop_front();
    m_array.push_back(a_val);
  }
  else if (size > m_count) {
    size_t margin = size - m_count;
    for (size_t i = 0; i < margin; i++) {
      m_array.pop_front();
    }
  }
}

template<class T>
T irs::delta_calc_t<T>::max()const {
  size_t size = m_array.size();
  T max = 0;
  if (size != 0) {
    max = *max_element(m_array.begin(), m_array.end());
  }
  return max;
}

template<class T>
T irs::delta_calc_t<T>::min()const {
  size_t size = m_array.size();
  T min = 0;
  if (size != 0) {
    min = *min_element(m_array.begin(), m_array.end());
  }
  return min;
}

template<class T>
T irs::delta_calc_t<T>::absolute()const {
  T delta = max() - min();
  return delta;
}

template<class T>
void irs::delta_calc_t<T>::clear() {
  m_array.resize(0);
}

template<class T>
T irs::delta_calc_t<T>::average()const {
  size_t size = m_array.size();
  T sum = 0;
  for (size_t i = 0; i < size; i++) {
    sum = sum + m_array[i];
  }
  if (size == 0) {
    size = 1;
  }
  return sum / size;
}

template<class T>
T irs::delta_calc_t<T>::relative()const {
  T average_value = average();
  T relative_value = 0;
  if (average_value != 0) {
    relative_value = absolute() / average_value;
  } else {
    relative_value = 0;
  }
  return relative_value;
}

template<class T>
size_t irs::delta_calc_t<T>::size() {
  size_t size = m_array.size();
  return size;
}

namespace irs {

#define IRS_USE_DEQUE_DATA 1

template <class data_t, class calc_t>
class fast_average_t
{
public:
  typedef size_t size_type;
  fast_average_t(size_type a_count);
  void add(data_t a_val);
  calc_t get() const;
  void resize(size_type a_count);
  size_type size() const;
  size_type max_size() const;
  bool is_full() const;
  void clear();
  void preset(size_type a_start_pos, size_type a_count);
private:
  fast_average_t();
  size_type m_max_count;
  #if IRS_USE_DEQUE_DATA
  irs::deque_data_t<data_t> m_samples;
  #else
  std::deque<data_t> m_samples;
  #endif // IRS_USE_DEQUE_DATA
  calc_t m_sum;
};

template <class data_t, class calc_t>
fast_average_t<data_t, calc_t>::fast_average_t(size_type a_count):
  m_max_count(a_count),
  m_samples(),
  m_sum(0)
{
  #if IRS_USE_DEQUE_DATA
  m_samples.reserve(a_count);
  #endif // IRS_USE_DEQUE_DATA
}

template <class data_t, class calc_t>
void fast_average_t<data_t, calc_t>::add(data_t a_val)
{
  if (m_max_count > 0) {
    IRS_LIB_ASSERT(m_samples.size() <= m_max_count);
    if (m_samples.size() == m_max_count) {
      m_sum -= m_samples.front();
      m_samples.pop_front();
    }
    m_samples.push_back(a_val);
    m_sum += a_val;
  }
}

template <class data_t, class calc_t>
calc_t fast_average_t<data_t, calc_t>::get() const
{
  if (m_samples.empty()) {
    return 0;
  }
  return m_sum/m_samples.size();
}

template <class data_t, class calc_t>
void fast_average_t<data_t, calc_t>::resize(size_type a_count)
{
  #if IRS_USE_DEQUE_DATA
  m_samples.reserve(a_count);
  #endif // IRS_USE_DEQUE_DATA
  m_max_count = a_count;
  while (m_samples.size() > m_max_count) {
    m_sum -= m_samples.front();
    m_samples.pop_front();
  }
}

template <class data_t, class calc_t>
typename fast_average_t<data_t, calc_t>::size_type
fast_average_t<data_t, calc_t>::size() const
{
  return m_samples.size();
}

template <class data_t, class calc_t>
typename fast_average_t<data_t, calc_t>::size_type
fast_average_t<data_t, calc_t>::max_size() const
{
  return m_max_count;
}

template <class data_t, class calc_t>
bool fast_average_t<data_t, calc_t>::is_full() const
{
  return m_samples.size() == m_max_count;
}

template <class data_t, class calc_t>
void fast_average_t<data_t, calc_t>::clear()
{
  m_samples.clear();
  m_sum = 0;
}

template <class data_t, class calc_t>
void fast_average_t<data_t, calc_t>::preset(
  size_type a_start_pos, size_type a_count)
{
  const size_type erase_front_count = min(m_samples.size(), a_start_pos);
  #if IRS_USE_DEQUE_DATA
  m_samples.pop_front(erase_front_count);
  #else // !IRS_USE_DEQUE_DATA
  m_samples.erase(m_samples.begin(), m_samples.begin() + erase_front_count);
  #endif // !IRS_USE_DEQUE_DATA
  const size_type erase_back_count = min(m_samples.size(),
    m_samples.size() - a_count);
  #if IRS_USE_DEQUE_DATA
  m_samples.pop_back(erase_back_count);
  #else // !IRS_USE_DEQUE_DATA
  m_samples.erase(m_samples.end() - erase_back_count, m_samples.end());
  #endif // !IRS_USE_DEQUE_DATA
  const size_type size = min(m_samples.size(), a_count);
  if (size > 0) {
    while (m_samples.size() < m_max_count) {
      for (size_type i = 0; i < size; i++) {
        m_samples.push_back(m_samples[i]);
        if (m_samples.size() == m_max_count) {
          break;
        }
      }
    }
  }
  m_sum = 0;
  for (size_type i = 0; i < m_samples.size(); i++) {
    m_sum += m_samples[i];
  }
}

template <class data_t, class calc_t>
class fast_multi_average_t
{
public:
  typedef size_t size_type;
  //! \brief Конструктор
  //! \param[in] a_sizes - массив размеров окон
  fast_multi_average_t(const vector<size_type>& a_sizes);
  //! \brief Добавляет отсчет
  //! \param[in] a_val - значение отсчета
  void add(data_t a_val);
  //! \brief Возвращает значение окна с индексом a_index
  //! \param[in] a_index - индекс окна
  calc_t get(size_type a_index) const;
  //! \brief Изменение размера окна
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна
  void resize(size_type a_index, size_type a_new_size);
  //! \brief Возвращает текущий размер окна
  //! \param[in] a_index - индекс окна
  size_type size(size_type a_index) const;
  //! \brief Возвращает размер окона
  //! \param[in] a_index - индекс окна
  size_type max_size(size_type a_index) const;
  //! \brief Возвращает статус заполения окна
  //! \param[in] a_index - индекс окна
  bool is_full(size_type a_index) const;
  //! \brief Очищает окно
  //! \param[in] a_index - индекс окна
  void clear(size_type a_index);
  //! \brief Очищает все окна
  void clear();
private:
  fast_multi_average_t();
  size_type m_count;
  #if IRS_USE_DEQUE_DATA
  irs::deque_data_t<data_t> m_samples;
  #else
  std::deque<data_t> m_samples;
  #endif // IRS_USE_DEQUE_DATA
  struct window_t
  {
    size_type max_size;
    size_type size;
    calc_t sum;
    window_t():
      max_size(0),
      size(0),
      sum(0)
    {
    }
  };

  vector<window_t> m_windows;
};

template <class data_t, class calc_t>
fast_multi_average_t<data_t, calc_t>::fast_multi_average_t(const vector<size_type>& a_sizes):
  m_count(0),
  m_samples(),
  m_windows(0)
{
  for (size_type i = 0; i < a_sizes.size(); i++) {
    m_count = max(m_count, a_sizes[i]);
    window_t window;
    window.max_size = a_sizes[i];
    m_windows.push_back(window);
  }
  #if IRS_USE_DEQUE_DATA
  m_samples.reserve(m_count);
  #endif // IRS_USE_DEQUE_DATA
}

template <class data_t, class calc_t>
void fast_multi_average_t<data_t, calc_t>::add(data_t a_val)
{
  if (m_count > 0) {
    IRS_LIB_ASSERT(m_samples.size() <= m_count);
    for (size_type i = 0; i < m_windows.size(); i++) {
      window_t& window = m_windows[i];
      if ((window.size > 0) && (window.size == window.max_size)) {
        window.sum -= m_samples[m_samples.size() - window.max_size];
        window.size--;
      }
    }
    if (m_samples.size() == m_count) {
      m_samples.pop_front();
    }
    m_samples.push_back(a_val);

    for (size_type i = 0; i < m_windows.size(); i++) {
      window_t& window = m_windows[i];
      window.sum += a_val;
      if (window.size < window.max_size) {
        window.size++;
      }
    }
  }
}

template <class data_t, class calc_t>
calc_t fast_multi_average_t<data_t, calc_t>::get(size_type a_index) const
{
  if (m_samples.empty()) {
    return 0;
  }
  return m_windows[a_index].sum/m_windows[a_index].size;
}

template <class data_t, class calc_t>
void fast_multi_average_t<data_t, calc_t>::resize(size_type a_index,
  size_type a_new_size)
{
  window_t& window = m_windows[a_index];
  
  window.max_size = a_new_size;
  if (window.size > window.max_size) {
    const size_type count = window.size - window.max_size;
	const size_type start_index = m_samples.size() - window.size;
    for (size_type i = 0; i < count; i++) {      
      window.sum -= m_samples[start_index + i];
    }
    window.size = window.max_size;
  } else {
    const size_type count = min(window.max_size - window.size, m_samples.size() - window.size);
    const size_type start_index = m_samples.size() - window.size - count;
    for (size_type i = 0; i < count; i++) {
      window.sum += m_samples[start_index + i];
    }
    window.size += count;
  }

  m_count = 0;
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_count = max(m_count, m_windows[i].max_size);
  }

  #if IRS_USE_DEQUE_DATA
  m_samples.reserve(m_count);
  #endif // IRS_USE_DEQUE_DATA

  while (m_samples.size() > m_count) {
    m_samples.pop_front();
  }
}

template <class data_t, class calc_t>
typename fast_multi_average_t<data_t, calc_t>::size_type
fast_multi_average_t<data_t, calc_t>::size(size_type a_index) const
{
  return m_windows[a_index].size;
}

template <class data_t, class calc_t>
typename fast_multi_average_t<data_t, calc_t>::size_type
fast_multi_average_t<data_t, calc_t>::max_size(size_type a_index) const
{
  return m_windows[a_index].max_size;
}

template <class data_t, class calc_t>
bool fast_multi_average_t<data_t, calc_t>::is_full(size_type a_index) const
{
  return m_windows[a_index].size == m_windows[a_index].max_size;
}

template <class data_t, class calc_t>
void fast_multi_average_t<data_t, calc_t>::clear(size_type a_index)
{
  m_windows[a_index].size = 0;
  m_windows[a_index].sum = 0;
}

template <class data_t, class calc_t>
void fast_multi_average_t<data_t, calc_t>::clear()
{
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_windows[i].size = 0;
    m_windows[i].sum = 0;
  }
}

//! \brief Расчет среднего значения сигнала, полученного
//!   асинхронной дискретизацией
//! \details Суффикс "as" означает "asynchronous sampling"
template <class data_t, class calc_t>
class fast_average_as_t
{
public:
  typedef size_t size_type;
  fast_average_as_t(calc_t a_count, calc_t a_default = 0);
  void add(data_t a_val);
  calc_t get() const;
  void resize(calc_t a_count);
  //! \brief Изменяет размер окна для периодического сигнала
  //!   c предустановкой из первоначального окна.
  //!   Если окно не заполнено или уменьшается, то эта функция вызывает resize.
  //! \param[in] a_count - новый размер окна в отсчетах
  //! \param[in] a_period - период сигнала в отсчетах
  void resize_preset(calc_t a_count, calc_t a_period);
  size_type size() const;
  size_type max_size() const;
  bool is_full() const;
  void clear();
  void preset(size_type a_start_pos, size_type a_count);
private:
  fast_average_as_t();
  //! \brief Полный дробный размер окна
  calc_t m_count;
  //! \brief Целая часть размера окна + 1
  size_type m_max_count;
  //! \brief Дробная часть размера окна
  calc_t m_count_fractional;
  irs::deque_data_t<data_t> m_samples;
  calc_t m_sum;
  calc_t m_default;
  
  // Переменные для предустановки resize_preset
  //! \brief Текущий целый размер окна
  size_type m_size;
  //! \brief Текущий предустановленный остаток суммы
  calc_t m_sum_preset_part;
  //! \brief Полный дробный размер окна ближайшего расширения
  calc_t m_count_expand;
  //! \brief Целая часть размера окна + 1 ближайшего расширения
  size_type m_max_count_expand;
  //! \brief Сумма постоянно расширяемая
  calc_t m_sum_expand;
  //! \brief Полный дробный размер окна после предустановки
  calc_t m_count_preset;
  //! \brief Размер окна после предустановки
  //!   Он также используется всегда как размер окна для отсчетов.
  size_type m_max_count_preset;
  //! \brief Период
  calc_t m_period;
  //! \brief Сумма на периоде
  calc_t m_sum_period;
  //! \brief Режим предустановки
  bool m_is_preset_mode;
};

template <class data_t, class calc_t>
fast_average_as_t<data_t, calc_t>::fast_average_as_t(calc_t a_count,
  calc_t a_default
):
  m_count(a_count),
  m_max_count(0),
  m_count_fractional(),
  m_samples(),
  m_sum(),
  m_default(a_default),
  
  m_size(0),
  m_sum_preset_part(),
  m_count_expand(),
  m_max_count_expand(0),
  m_sum_expand(),
  m_count_preset(),
  m_max_count_preset(0),
  m_period(),
  m_sum_period(),
  m_is_preset_mode(false)
{
  m_max_count = static_cast<size_type>(a_count);
  m_count_fractional = a_count - m_max_count;
  m_max_count++;
  m_max_count_preset = m_max_count;
  m_samples.reserve(m_max_count_preset);
}

template <class data_t, class calc_t>
void fast_average_as_t<data_t, calc_t>::add(data_t a_val)
{
  if (m_max_count > 0) {
    IRS_LIB_ASSERT(m_samples.size() <= m_max_count_preset);
    if (!m_samples.empty()) {
      if ((m_size > 0) && (m_size == m_max_count - 1)) {
        m_sum -= m_samples[m_samples.size() - m_max_count];
        m_size--;
      }
      m_sum += m_samples.back();
      m_size++;
      
      if (m_is_preset_mode) {
        // Накапливаем данные для будущего расширения окна
        m_sum_expand += m_samples.back();
        
        // Если окно предустановки полностью заполнено
        if (m_samples.size() == m_max_count_preset - 1) {
          // Увеличиваем исходное окно до окна предустановки
          m_count = m_count_preset;
          m_size = m_samples.size();
          m_count_fractional = m_count - m_size;
          m_max_count = m_max_count_preset;
          m_sum = m_sum_expand;
          m_sum_preset_part = calc_t();
          
          // Выходим из режима предустановки
          m_is_preset_mode = false;
          
        // Иначе если окно для расширения полностью заполнено
        } else if (m_samples.size() == m_max_count_expand - 1) {
          // Увеличиваем исходное окно на 1 период сигнала
          m_count = m_count_expand;
          m_size = m_samples.size();
          m_count_fractional = m_count - m_size;
          m_max_count = m_max_count_expand;
          m_sum = m_sum_expand;
          m_sum_preset_part -= m_sum_period;
          
          // Подготавливаем следующее расширение окна
          m_count_expand += m_period;
          m_max_count_expand =
            static_cast<size_type>(m_count_expand) + 1;
        }
      }
    }
  }
  
  if (m_max_count_preset > 0) {
    if (!m_samples.empty()) {
      if (m_max_count_preset == m_samples.size()) {
        m_samples.pop_front();
      }
    }
    
    m_samples.push_back(a_val);
  }
}

template <class data_t, class calc_t>
calc_t fast_average_as_t<data_t, calc_t>::get() const
{
  if ((m_size + 1 < m_max_count) || m_samples.empty()) {
    return m_default;
  }
  
  calc_t sum = m_sum + m_samples.back()*m_count_fractional;
  calc_t n = m_count;
  
  if (m_is_preset_mode) {
    sum += m_sum_preset_part;
    n = m_count_preset;
  }
  
  return sum/n;
}

template <class data_t, class calc_t>
void fast_average_as_t<data_t, calc_t>::resize(calc_t a_count)
{
  m_count = a_count;
  m_max_count = static_cast<size_type>(a_count);
  m_count_fractional = a_count - m_max_count;
  m_max_count++;
  m_max_count_preset = m_max_count;
  m_samples.reserve(m_max_count_preset);
  
  m_is_preset_mode = false;

  if (m_max_count == 0) {
    clear();
  } else {
    size_t i = m_samples.size() - m_size - 1;
    while (m_size >= m_max_count) {
      m_sum -= m_samples[i];
      m_size--;
      i++;
    }
    if (m_samples.size() > m_max_count) {
      m_samples.pop_front(m_samples.size() - m_max_count);
    }
  }
}

template <class data_t, class calc_t>
void fast_average_as_t<data_t, calc_t>::resize_preset(calc_t a_count,
  calc_t a_period)
{
  m_count_preset = a_count;
  m_max_count_preset = static_cast<size_type>(a_count) + 1;

  // Окно уменьшается
  bool is_window_reduce = (m_max_count_preset <= m_max_count);
  // Окно заполнено
  bool is_window_full = (m_size >= m_max_count - 1);
  
  if (is_window_reduce || !is_window_full) {
    resize(a_count);
  } else {
    m_is_preset_mode = true;
    m_period = a_period;
    
    calc_t periods_in_window = m_count/m_period;
    calc_t sum = m_sum + m_samples.back()*m_count_fractional;
    m_sum_period = sum/periods_in_window;
    
    double periods_in_preset_part =
      (m_count_preset - m_count)/m_period;
    m_sum_preset_part =
      periods_in_preset_part*m_sum_period;
    
    m_count_expand = m_count + m_period;
    m_max_count_expand = static_cast<size_type>(m_count_expand) + 1;
    m_sum_expand = m_sum;

    m_samples.reserve(m_max_count_preset);
  }
}

template <class data_t, class calc_t>
typename fast_average_as_t<data_t, calc_t>::size_type
fast_average_as_t<data_t, calc_t>::size() const
{
  return m_size + 1;
}

template <class data_t, class calc_t>
typename fast_average_as_t<data_t, calc_t>::size_type
fast_average_as_t<data_t, calc_t>::max_size() const
{
  return m_max_count_preset;
}

template <class data_t, class calc_t>
bool fast_average_as_t<data_t, calc_t>::is_full() const
{
  // Во время предустановки это услувие всегда будет возвращать true.
  // И это правильно. Поэтому здесь m_max_count, а не m_max_count_preset.
  return m_size + 1 == m_max_count;
}

template <class data_t, class calc_t>
void fast_average_as_t<data_t, calc_t>::clear()
{
  m_samples.clear();
  m_sum = 0;
  m_is_preset_mode = false;
  m_size = 0;
}

template <class data_t, class calc_t>
void fast_average_as_t<data_t, calc_t>::preset(
  size_type a_start_pos, size_type a_count)
{
  const size_type erase_front_count = min(m_size + 1, a_start_pos);
  m_samples.pop_front(erase_front_count);
  m_size -= erase_front_count;
  const size_type erase_back_count = min(m_size + 1, m_size + 1 - a_count);
  m_samples.pop_back(erase_back_count);
  m_size -= erase_back_count;
  m_sum = 0;
  const size_type size = min(m_size + 1, a_count);
  if (size > 0) {
    while (m_size + 1 < m_max_count) {
      for (size_type i = 0; i < size; i++) {
        m_samples.push_back(m_samples[i]);
        m_size++;
        if (m_size + 1 == m_max_count) {
          break;
        }
      }
    }
    for (size_type i = 0; i < m_size + 1; i++) {
      m_sum += m_samples[i];
    }
  }
}

//! \brief Расчет среднего значения сигнала, полученного
//!   асинхронной дискретизацией
//! \details Суффикс "as" означает "asynchronous sampling"
template <class data_t, class calc_t>
class fast_multi_average_as_t
{
public:
  typedef size_t size_type;
  //! \brief Конструктор
  //! \param[in] a_sizes - массив размеров окон
  //! \param[in] a_default - значение, возвращаемое, когда среднее значение еще не рассчитано
  fast_multi_average_as_t(const vector<calc_t>& a_sizes, calc_t a_default = 0);
  //! \brief Добавляет отсчет
  //! \param[in] a_val - значение отсчета
  void add(data_t a_val);
  //! \brief Возвращает значение окна с индексом a_index
  //! \param[in] a_index - индекс окна
  calc_t get(size_type a_index) const;
  //! \brief Изменение размера окна
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна
  void resize(size_type a_index, calc_t a_new_size);
  //! \brief Возвращает текущий размер окна
  //! \param[in] a_index - индекс окна
  size_type size(size_type a_index) const;
  //! \brief Возвращает размер окона
  //! \param[in] a_index - индекс окна
  size_type max_size(size_type a_index) const;
  //! \brief Возвращает статус заполения окна
  //! \param[in] a_index - индекс окна
  bool is_full(size_type a_index) const;
  //! \brief Очищает окно
  //! \param[in] a_index - индекс окна
  void clear(size_type a_index);
  //! \brief Очищает все окна
  void clear();  
private:
  fast_multi_average_as_t();
  calc_t m_default;
  size_type m_max_count;
  irs::deque_data_t<data_t> m_samples;
  struct window_t
  {
    calc_t count;
    size_type size;
    size_type max_size;
    calc_t count_fractional;
    calc_t sum;
    window_t():
      count(0),
      size(0),
      max_size(0),
      count_fractional(0),
      sum(0)
    {
    }
  };
  vector<window_t> m_windows;
};

template <class data_t, class calc_t>
fast_multi_average_as_t<data_t, calc_t>::fast_multi_average_as_t(
  const vector<calc_t>& a_sizes, calc_t a_default
):
  m_default(a_default),
  m_max_count(0),
  m_samples(),
  m_windows()
{
  for (size_type i = 0; i < a_sizes.size(); i++) {
    window_t window;
    window.count = a_sizes[i];
    calc_t max_size = 0;
    window.count_fractional = modf(a_sizes[i], &max_size);
    window.max_size = static_cast<size_type>(max_size);
    m_max_count = max(m_max_count, window.max_size + 1);
    m_windows.push_back(window);
  }
  m_samples.reserve(m_max_count);
}

template <class data_t, class calc_t>
void fast_multi_average_as_t<data_t, calc_t>::add(data_t a_val)
{
  if (m_max_count > 0) {
    IRS_LIB_ASSERT(m_samples.size() <= m_max_count);
    for (size_type i = 0; i < m_windows.size(); i++) {
      window_t& window = m_windows[i];
      if ((window.size > 0) && (window.size == window.max_size)) {
        window.sum -= m_samples[m_samples.size() - (window.max_size + 1)];
        window.size--;
      }
    }
    if (!m_samples.empty()) {
      for (size_type i = 0; i < m_windows.size(); i++) {
        window_t& window = m_windows[i];
        if (window.size < window.max_size) {
          window.sum += m_samples.back();
          window.size++;
        }
      }
      if (m_samples.size() == m_max_count) {
        m_samples.pop_front();
      }
    }
    m_samples.push_back(a_val);
  }
}

template <class data_t, class calc_t>
calc_t fast_multi_average_as_t<data_t, calc_t>::get(size_type a_index) const
{  
  const window_t& window = m_windows[a_index];
  if (window.size < window.max_size) {
    return m_default;
  }
  calc_t sum = window.sum + m_samples.back()*window.count_fractional;
  return sum/window.count;
}

template <class data_t, class calc_t>
void fast_multi_average_as_t<data_t, calc_t>::resize(size_type a_index, calc_t a_new_size)
{
  window_t& window = m_windows[a_index];
  window.count = a_new_size;
  calc_t max_size = 0;
  window.count_fractional = modf(a_new_size, &max_size);
  window.max_size = static_cast<size_type>(max_size);
  if (window.max_size == 0) {
    window.size = 0;
    window.sum = 0;
  } else if (window.max_size == 1) {
    window.size = 0;
    window.sum = 0;
    if (m_samples.size() >= 2) {
      // Берем предпоследний элемент
      window.sum = m_samples[m_samples.size() - 2];
    }
  } else if (window.size > window.max_size) {
    if (window.size/window.max_size >= 2) {
      window.sum = 0;
      size_type start_index = 0;
      if (m_samples.size() > (window.max_size + 1)) {
        start_index = m_samples.size() - (window.max_size + 1);
      }
      for (size_type i = 0; i < window.max_size; i++) {
        window.sum += m_samples[start_index + i];
      }
      window.size = window.max_size;
    } else {
      size_type start_index = m_samples.size() - (window.size + 1);
      const size_type count = window.size - window.max_size;
      for (size_type i = 0; i < count; i++) {
        window.sum -= m_samples[start_index + i];
      }
      window.size = window.max_size;
    }
  } else if ((window.size + 1) < m_samples.size()) {
    const size_type count = min(window.max_size - window.size, m_samples.size() - (window.size + 1));
    size_type start_index = m_samples.size() - (window.size + 1) - count;
    for (size_type i = 0; i < count; i++) {
      window.sum += m_samples[start_index + i];
    }
    window.size += count;
  }

  m_max_count = 0;
  for (size_type i = 0; i < m_windows.size(); i++) {
    window_t& w = m_windows[i];
    m_max_count = max(m_max_count, w.max_size + 1);
  }

  m_samples.reserve(m_max_count);

  if (m_max_count == 0) {
    clear();
  } else if (m_max_count == 1) {    
    if (!m_samples.empty()) {
      m_samples.pop_front(m_samples.size() - 1);
    }
  } else if (m_samples.size() > m_max_count) {
    m_samples.pop_front(m_samples.size() - m_max_count);
  }
}

template <class data_t, class calc_t>
typename fast_multi_average_as_t<data_t, calc_t>::size_type
fast_multi_average_as_t<data_t, calc_t>::size(size_type a_index) const
{
  return m_windows[a_index].size;
}

template <class data_t, class calc_t>
typename fast_multi_average_as_t<data_t, calc_t>::size_type
fast_multi_average_as_t<data_t, calc_t>::max_size(size_type a_index) const
{
  return m_windows[a_index].max_size;
}

template <class data_t, class calc_t>
bool fast_multi_average_as_t<data_t, calc_t>::is_full(size_type a_index) const
{
  return m_windows[a_index].size == m_windows[a_index].max_size;
}

template <class data_t, class calc_t>
void fast_multi_average_as_t<data_t, calc_t>::clear(size_type a_index)
{
  m_windows[a_index].size = 0;
  m_windows[a_index].sum = 0;
}

template <class data_t, class calc_t>
void fast_multi_average_as_t<data_t, calc_t>::clear()
{
  m_samples.clear();
  for (size_type i = 0; i < m_windows.size(); i++) {
    clear(i);
  }
}

template<class data_t, class calc_t>
class fast_sko_t
{
public:
  typedef size_t size_type;
  fast_sko_t(size_type a_count, size_type a_average_sample_count);
  ~fast_sko_t();
  void clear();
  void add(data_t a_val);
  operator calc_t()const;
  calc_t relative()const;
  calc_t average()const;
  void resize(size_type a_size);
  void resize_average(size_type a_size);
  void clear_average();
  size_type size() const;
  size_type average_size() const;
  size_type max_size() const;
  size_type average_max_size() const;
  bool is_full();
private:
  fast_sko_t();
  size_type m_count;
  fast_average_t<data_t, calc_t> m_average;
  #if IRS_USE_DEQUE_DATA
  irs::deque_data_t<data_t> m_square_elems;
  #else
  std::deque<data_t> m_square_elems;
  #endif // IRS_USE_DEQUE_DATA
  calc_t m_square_sum;
};

template<class data_t, class calc_t>
fast_sko_t<data_t, calc_t>::fast_sko_t(
  size_type a_count, size_type a_average_sample_count
):
  m_count(a_count),
  m_average(a_average_sample_count),
  m_square_elems(),
  m_square_sum(0)
{
  #if IRS_USE_DEQUE_DATA
  m_square_elems.reserve(a_count);
  #endif // IRS_USE_DEQUE_DATA
}

template<class data_t, class calc_t>
fast_sko_t<data_t, calc_t>::~fast_sko_t()
{
}

template<class data_t, class calc_t>
void fast_sko_t<data_t, calc_t>::resize(size_type a_size)
{
  #if IRS_USE_DEQUE_DATA
  m_square_elems.reserve(a_size);
  #endif // IRS_USE_DEQUE_DATA
  m_count = a_size;
  while (m_square_elems.size() > m_count) {
    m_square_sum -= m_square_elems.front();
    m_square_elems.pop_front();
  }
}

template<class data_t, class calc_t>
void fast_sko_t<data_t, calc_t>::resize_average(size_type a_size)
{
  m_average.resize(a_size);
}

template<class data_t, class calc_t>
typename fast_sko_t<data_t, calc_t>::size_type
fast_sko_t<data_t, calc_t>::size() const
{
  return m_square_elems.size();
}

template<class data_t, class calc_t>
typename fast_sko_t<data_t, calc_t>::size_type
fast_sko_t<data_t, calc_t>::average_size() const
{
  return m_average.size();
}

template<class data_t, class calc_t>
typename fast_sko_t<data_t, calc_t>::size_type
fast_sko_t<data_t, calc_t>::max_size() const
{
  return m_count;
}

template<class data_t, class calc_t>
typename fast_sko_t<data_t, calc_t>::size_type
fast_sko_t<data_t, calc_t>::average_max_size() const
{
  return m_average.max_size();
}

template<class data_t, class calc_t>
bool fast_sko_t<data_t, calc_t>::is_full()
{
  return (m_square_elems.size() == m_count) && m_average.is_full();
}

template<class data_t, class calc_t>
void fast_sko_t<data_t, calc_t>::clear()
{
  m_square_elems.clear();
  m_average.clear();
  m_square_sum = 0;
}

template<class data_t, class calc_t>
void fast_sko_t<data_t, calc_t>::add(data_t a_val)
{
  m_average.add(a_val);
  if (m_count > 0) {
    IRS_LIB_ASSERT(m_square_elems.size() <= m_count);
    if (m_count == m_square_elems.size()) {
      m_square_sum -= m_square_elems.front();
      m_square_elems.pop_front();
    }
    calc_t elem = a_val - m_average.get();
    elem *= elem;
    m_square_elems.push_back(elem);
    m_square_sum += elem;
  }
}

template<class data_t, class calc_t>
fast_sko_t<data_t, calc_t>::operator calc_t()const
{
  if (m_square_elems.empty()) {
    return 0;
  }
  if (m_square_sum < 0) {
    return 0;
  }
  return sqrt(m_square_sum/m_square_elems.size());
}

template<class data_t, class calc_t>
calc_t fast_sko_t<data_t, calc_t>::relative() const
{
  if (m_square_elems.empty()) {
    return 0;
  }
  const calc_t average = m_average.get();
  if (average == 0) {
    return 0;
  }
  const calc_t sko = *this;
  calc_t sko_reltive = sko/average;
  return sko_reltive;
}

template<class data_t, class calc_t>
calc_t fast_sko_t<data_t, calc_t>::average()const
{
  return m_average.get();
}

template<class data_t, class calc_t>
class fast_multi_sko_with_single_average_t
{
public:
  typedef size_t size_type;
  //! \brief Конструктор
  //! \param[in] a_sizes - массив размеров окон
  //! \param[in] a_average_sizes - массив размеров окон для среднего значения
  fast_multi_sko_with_single_average_t(
    const vector<size_type>& a_sizes, size_type a_average_size);
  ~fast_multi_sko_with_single_average_t();
  //! \brief Очищает все окна
  void clear();
  //! \brief Добавляет отсчет
  void add(data_t a_val);
  //! \brief Добавляет отсчет в среднее значение
  void add_to_average(data_t a_val);
  //! \brief Возвращает значение sko
  //! \param[in] a_index - индекс окна
  calc_t get(size_type a_index) const;
  //! \brief Возвращает среднее значение
  calc_t average() const;
  //! \brief Изменяет размер окна
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна
  void resize(size_type a_index, size_type a_new_size);
  //! \brief Изменяет размер окна среднего значения
  //! \param[in] a_new_size - новый размер окна
  void resize_average(size_type a_new_size);
  //! \brief Очищает среднее значение
  void clear_average();
  //! \brief Возвращает размер окна sko
  //! \param[in] a_index - индекс окна
  size_type size(size_type a_index) const;
  //! \brief Возвращает размер окна sko с наибольшим количеством элементов
  size_type size() const;
  //! \brief Возвращает размер окна среднего значения
  size_type average_size() const;
  //! \brief Возвращает максимальный размер окна sko
  //! \param[in] a_index - индекс окна
  size_type max_size(size_type a_index) const;
  //! \brief Возвращает максимальный размер самого большого окна sko
  size_type max_size() const;
  //! \brief Возвращает максимальный размер окна среднего значения
  size_type average_max_size() const;
  //! \brief Возвращает статус заполнения окна
  //! \param[in] a_index - индекс окна
  bool is_full(size_type a_index);
  void preset(size_type a_start_pos, size_type a_size);
  void preset_average(size_type a_start_pos, size_type a_size);
private:
  fast_multi_sko_with_single_average_t();
  size_type m_max_count;
  fast_average_t<data_t, calc_t> m_average;
  #if IRS_USE_DEQUE_DATA
  irs::deque_data_t<data_t> m_square_elems;
  #else
  std::deque<data_t> m_square_elems;
  #endif // IRS_USE_DEQUE_DATA
  struct window_t
  {
    size_type max_size;
    size_type size;
    calc_t square_sum;
    window_t():
      max_size(0),
      size(0),
      square_sum()
    {
    }
  };
  vector<window_t> m_windows;
};

template<class data_t, class calc_t>
fast_multi_sko_with_single_average_t<data_t, calc_t>::
fast_multi_sko_with_single_average_t(
  const vector<size_type>& a_sizes,
  size_type a_average_size
):
  m_max_count(0),
  m_average(a_average_size),
  m_square_elems(),
  m_windows()
{
  for (size_type i = 0; i < a_sizes.size(); i++) {
    m_max_count = max(m_max_count, a_sizes[i]);
    window_t window;
    window.max_size = a_sizes[i];
    m_windows.push_back(window);
  }
  #if IRS_USE_DEQUE_DATA
  m_square_elems.reserve(m_max_count);
  #endif // IRS_USE_DEQUE_DATA
}

template<class data_t, class calc_t>
fast_multi_sko_with_single_average_t<data_t, calc_t>::
~fast_multi_sko_with_single_average_t()
{
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::resize(
  size_type a_index,
  size_type a_new_size)
{
  window_t& window = m_windows[a_index];
  //const size_type start_index = m_square_elems.size() - window.size;
  window.max_size = a_new_size;
  if (window.size > window.max_size) {
    const size_type count = window.size - window.max_size;
    for (size_type i = 0; i < count; i++) {
      window.square_sum -= m_square_elems[i];
    }
    window.size = window.max_size;
  }

  m_max_count = 0;
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_max_count = max(m_max_count, m_windows[i].max_size);
  }

  #if IRS_USE_DEQUE_DATA
  m_square_elems.reserve(m_max_count);
  #endif // IRS_USE_DEQUE_DATA

  while (m_square_elems.size() > m_max_count) {
    m_square_elems.pop_front();
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::
resize_average(size_type a_new_size)
{
  m_average.resize(a_new_size);
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_t<data_t, calc_t>::size(
  size_type a_index) const
{
  return m_windows[a_index].size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_t<data_t, calc_t>::size() const
{
  return m_square_elems.size();
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_t<data_t, calc_t>::average_size() const
{
  return m_average.size();
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_t<data_t, calc_t>::
max_size(size_type a_index) const
{
  return m_windows[a_index].max_size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_t<data_t, calc_t>::max_size() const
{
  return m_max_count;
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_t<data_t, calc_t>::average_max_size() const
{
  return m_average.max_size();
}

template<class data_t, class calc_t>
bool fast_multi_sko_with_single_average_t<data_t, calc_t>::
is_full(size_type a_index)
{
  return (m_windows[a_index].size == m_windows[a_index].max_size) &&
    m_average.is_full();
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::preset(
  size_type a_start_pos, size_type a_size)
{
  preset_average(a_start_pos, a_size);

  const size_type erase_front_count = min(m_square_elems.size(), a_start_pos);
  #if IRS_USE_DEQUE_DATA
  m_square_elems.pop_front(erase_front_count);
  #else // !IRS_USE_DEQUE_DATA
  m_square_elems.erase(m_square_elems.begin(), m_square_elems.begin() +
    erase_front_count);
  #endif // !IRS_USE_DEQUE_DATA
  const size_type erase_back_count = min(m_square_elems.size(),
    m_square_elems.size() - a_size);
  #if IRS_USE_DEQUE_DATA
  m_square_elems.pop_back(erase_back_count);
  #else // !IRS_USE_DEQUE_DATA
  m_square_elems.erase(m_square_elems.end() - erase_back_count,
    m_square_elems.end());
  #endif // !IRS_USE_DEQUE_DATA
  const size_type size = min(m_square_elems.size(), a_size);
  if (size > 0) {
    while (m_square_elems.size() < m_max_count) {
      for (size_type i = 0; i < size; i++) {
        m_square_elems.push_back(m_square_elems[i]);
      }
    }
  }
  for (size_type i = 0; i < m_windows.size(); i++) {
    window_t& window = m_windows[i];
    window.square_sum = 0;
    const size_type min_size = min(m_square_elems.size(), window.max_size);
    const size_type start_pos = m_square_elems.size() - min_size;
    for (size_type j = start_pos; j < m_square_elems.size(); j++) {
      window.square_sum += m_square_elems[j];
    }
    window.size = min_size;
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::preset_average(
  size_type a_start_pos, size_type a_size)
{
  const size_type average_start_pos =
    m_square_elems.size() - m_average.size();
  if ((a_start_pos + a_size) > average_start_pos) {
    size_type average_pos = 0;
    if (a_start_pos < average_start_pos) {
      average_pos = 0;
    } else {
      average_pos = a_start_pos - average_start_pos;
    }
    size_type size = 0;
    if ((a_start_pos + a_size) > (average_start_pos + m_average.size())) {
      size = m_average.size();
    } else {
      size = (average_start_pos + m_average.size()) - (a_start_pos + a_size);
    }
    m_average.preset(average_pos, size);
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::clear()
{
  m_square_elems.clear();
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_windows[i].size = 0;
    m_windows[i].square_sum = 0;
  }
  m_average.clear();
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::add(data_t a_val)
{
  m_average.add(a_val);

  if (m_max_count == 0) {
    return;
  }

  IRS_LIB_ASSERT(m_square_elems.size() <= m_max_count);
  for (size_type i = 0; i < m_windows.size(); i++) {
    window_t& window = m_windows[i];
    if ((window.size > 0) && (window.size == window.max_size)) {
      window.square_sum -=
        m_square_elems[m_square_elems.size() - window.max_size];
      window.size--;
    }
  }
  calc_t elem = a_val - m_average.get();
  elem *= elem;

  if (m_max_count == m_square_elems.size()) {
    m_square_elems.pop_front();
  }

  m_square_elems.push_back(elem);
  for (size_type i = 0; i < m_windows.size(); i++) {
    window_t& window = m_windows[i];
    if (window.size < window.max_size) {
      window.square_sum += elem;
      window.size++;
    }
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_t<data_t, calc_t>::add_to_average(
  data_t a_val)
{
  m_average.add(a_val);
}

template<class data_t, class calc_t>
calc_t fast_multi_sko_with_single_average_t<data_t, calc_t>::get(
  size_type a_index) const
{
  const window_t& window = m_windows[a_index];
  if (window.size == 0) {
    return 0;
  }
  if (window.square_sum < 0) {
    return 0;
  }
  return sqrt(window.square_sum/window.size);
}

template<class data_t, class calc_t>
calc_t fast_multi_sko_with_single_average_t<data_t, calc_t>::average() const
{
  return m_average.get();
}

template<class data_t, class calc_t>
class fast_multi_sko_t
{
public:
  typedef size_t size_type;
  //! \brief Конструктор
  //! \param[in] a_sizes - массив размеров окон
  //! \param[in] a_average_sizes - массив размеров окон для среднего значения
  fast_multi_sko_t(const vector<size_type>& a_sizes,
    const vector<size_type>& a_average_sizes);
  ~fast_multi_sko_t();
  //! \brief Очищает все окна
  void clear();
  //! \brief Добавляет отсчет
  void add(data_t a_val);
  //! \brief Возвращает значение sko
  //! \param[in] a_index - индекс окна
  calc_t get(size_type a_index) const;
  //! \brief Возвращает среднее значение
  //! \param[in] a_index - индекс окна
  calc_t average(size_type a_index) const;
  //! \brief Изменяет размер окна
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна
  void resize(size_type a_index, size_type a_new_size);
  //! \brief Изменяет размер окна среднего значения
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна
  void resize_average(size_type a_index, size_type a_new_size);
  //! \brief Очищает среднее значение для всех окон
  void clear_average();
  //! \brief Возвращает размер окна sko
  //! \param[in] a_index - индекс окна
  size_type size(size_type a_index) const;
  //! \brief Возвращает размер окна среднего значения
  //! \param[in] a_index - индекс окна
  size_type average_size(size_type a_index) const;
  //! \brief Возвращает максимальный размер окна sko
  //! \param[in] a_index - индекс окна
  size_type max_size(size_type a_index) const;
  //! \brief Возвращает максимальный размер окна среднего значения
  //! \param[in] a_index - индекс окна
  size_type average_max_size(size_type a_index) const;
  //! \brief Возвращает статус заполнения окна
  //! \param[in] a_index - индекс окна
  bool is_full(size_type a_index);
private:
  fast_multi_sko_t();
  size_type m_count;
  fast_multi_average_t<data_t, calc_t> m_average;
  #if IRS_USE_DEQUE_DATA
  irs::deque_data_t<data_t> m_square_elems;
  #else
  std::deque<data_t> m_square_elems;
  #endif // IRS_USE_DEQUE_DATA
  struct window_t
  {
    size_type max_size;
    size_type size;
    calc_t square_sum;
    #if IRS_USE_DEQUE_DATA
    irs::deque_data_t<data_t> m_square_elems;
    #else
    std::deque<data_t> m_square_elems;
    #endif // IRS_USE_DEQUE_DATA
    window_t():
      max_size(0),
      square_sum(),
      m_square_elems()
    {
    }
  };
  vector<window_t> m_windows;
};

template<class data_t, class calc_t>
fast_multi_sko_t<data_t, calc_t>::fast_multi_sko_t(
  const vector<size_type>& a_sizes,
  const vector<size_type>& a_average_sizes
):
  m_count(0),
  m_average(a_average_sizes),
  m_square_elems(),
  m_windows()
  //m_square_sum(0)
{
  for (size_type i = 0; i < a_sizes.size(); i++) {
    m_count = max(m_count, a_sizes[i]);
    window_t window;
    window.max_size = a_sizes[i];
    #if IRS_USE_DEQUE_DATA
    m_square_elems.reserve(window.max_size);
    #endif // IRS_USE_DEQUE_DATA
    m_windows.push_back(window);
  }
}

template<class data_t, class calc_t>
fast_multi_sko_t<data_t, calc_t>::~fast_multi_sko_t()
{
}

template<class data_t, class calc_t>
void fast_multi_sko_t<data_t, calc_t>::resize(size_type a_index,
  size_type a_new_size)
{
  window_t& window = m_windows[a_index];
  window.max_size = a_new_size;
  while (window.square_elems.size() > window.max_size) {
    window.square_sum -= window.square_elems.front();
    window.square_elems.pop_front();
  }
  #if IRS_USE_DEQUE_DATA
  window.square_elems.reserve(m_count);
  #endif // IRS_USE_DEQUE_DATA
}

template<class data_t, class calc_t>
void fast_multi_sko_t<data_t, calc_t>::resize_average(size_type a_index,
  size_type a_new_size)
{
  m_average.resize(a_index, a_new_size);
}

template<class data_t, class calc_t>
typename fast_multi_sko_t<data_t, calc_t>::size_type
fast_multi_sko_t<data_t, calc_t>::size(size_type a_index) const
{
  return m_windows[a_index].size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_t<data_t, calc_t>::size_type
fast_multi_sko_t<data_t, calc_t>::average_size(size_type a_index) const
{
  return m_average.size(a_index);
}

template<class data_t, class calc_t>
typename fast_multi_sko_t<data_t, calc_t>::size_type
fast_multi_sko_t<data_t, calc_t>::max_size(size_type a_index) const
{
  return m_windows[a_index].max_size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_t<data_t, calc_t>::size_type
fast_multi_sko_t<data_t, calc_t>::average_max_size(size_type a_index) const
{
  return m_average.max_size(a_index);
}

template<class data_t, class calc_t>
bool fast_multi_sko_t<data_t, calc_t>::is_full(size_type a_index)
{
  return (
    m_windows[a_index].square_elems.size() == m_windows[a_index].max_size) &&
    m_average.is_full(a_index);
}

template<class data_t, class calc_t>
void fast_multi_sko_t<data_t, calc_t>::clear()
{
  m_square_elems.clear();
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_windows[i].square_elems.clear();
    m_windows[i].square_sum = 0;
  }
  m_average.clear();
}

template<class data_t, class calc_t>
void fast_multi_sko_t<data_t, calc_t>::add(data_t a_val)
{
  m_average.add(a_val);

  if (m_count == 0) {
    return;
  }

  IRS_LIB_ASSERT(m_square_elems.size() <= m_count);
  for (size_type i = 0; i < m_windows.size(); i++) {
    window_t& window = m_windows[i];
    if (!window.square_elems.empty() &&
        (window.square_elems.size() == window.max_size)) {
      window.square_sum -= window.square_elems.front();
      window.square_elems.pop_front();
    }
    if (window.square_elems.size() < window.max_size) {
      calc_t elem = a_val - m_average.get(i);
      elem *= elem;
      window.square_sum += elem;
      window.square_elems.push_back(elem);
    }
  }
}

template<class data_t, class calc_t>
calc_t fast_multi_sko_t<data_t, calc_t>::get(size_type a_index) const
{
  window_t& window = m_windows[a_index];
  if (window.square_elems.empty()) {
    return 0;
  }
  if (window.square_sum < 0) {
    return 0;
  }
  return sqrt(window.square_sum/window.square_elems.size());
}

template<class data_t, class calc_t>
calc_t fast_multi_sko_t<data_t, calc_t>::average(size_type a_index) const
{
  return m_average.get(a_index);
}


//! \brief Расчет СКО сигнала, полученного
//!   асинхронной дискретизацией
//! \details Суффикс "as" означает "asynchronous sampling"
template<class data_t, class calc_t>
class fast_multi_sko_with_single_average_as_t
{
public:
  typedef size_t size_type;
  //! \brief Конструктор
  //! \param[in] a_sizes - массив размеров окон
  //! \param[in] a_average_sizes - размер окна для среднего значения
  fast_multi_sko_with_single_average_as_t(
    const vector<calc_t>& a_sizes, calc_t a_average_size,
    calc_t a_average_default = 0);
  ~fast_multi_sko_with_single_average_as_t();
  //! \brief Очищает все окна
  void clear();
  //! \brief Добавляет отсчет
  void add(data_t a_val);
  //! \brief Добавляет отсчет в среднее значение
  void add_to_average(data_t a_val);
  //! \brief Возвращает значение sko
  //! \param[in] a_index - индекс окна
  calc_t get(size_type a_index) const;
  //! \brief Возвращает среднее значение
  calc_t average() const;
  //! \brief Изменяет размер окна
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна
  void resize(size_type a_index, calc_t a_new_size);
  //! \brief Изменяет размер окна для периодического сигнала
  //!   c предустановкой из первоначального окна.
  //!   Если окно не заполнено или уменьшается, то эта функция вызывает resize.
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна в отсчетах
  //! \param[in] a_period - период сигнала в отсчетах
  void resize_preset(size_type a_index, calc_t a_new_size, double a_period);
  //! \brief Изменяет размер окна среднего значения
  //! \param[in] a_new_size - новый размер окна
  void resize_average(calc_t a_new_size);
  //! \brief Изменяет размер окна среднего значения
  //!   c предустановкой из первоначального окна
  //! \param[in] a_new_size - новый размер окна
  void resize_preset_average(calc_t a_new_size, double a_period);
  //! \brief Очищает среднее значение
  void clear_average();
  //! \brief Возвращает размер окна sko
  //! \param[in] a_index - индекс окна
  size_type size(size_type a_index) const;
  //! \brief Возвращает размер окна sko с наибольшим количеством элементов
  size_type size() const;
  //! \brief Возвращает размер окна среднего значения
  size_type average_size() const;
  //! \brief Возвращает максимальный размер окна sko
  //! \param[in] a_index - индекс окна
  size_type max_size(size_type a_index) const;
  //! \brief Возвращает максимальный размер самого большого окна sko
  size_type max_size() const;
  //! \brief Возвращает максимальный размер окна среднего значения
  size_type average_max_size() const;
  //! \brief Возвращает статус заполнения окна
  //! \details Для полного заполнения требуется количество отсчетов, равное
  //!   сумме размеров, заданных через функции resize и resize_average. Вначале
  //!   отсчеты используются только для вычисления с среднего значения
  //! \param[in] a_index - индекс окна
  bool is_full(size_type a_index) const;
  void preset(size_type a_start_pos, size_type a_size);
  void preset_average(size_type a_start_pos, size_type a_size);
private:
  fast_multi_sko_with_single_average_as_t();
  
  size_type m_max_count;
  fast_average_as_t<data_t, calc_t> m_average;
  irs::deque_data_t<data_t> m_square_elems;
  struct window_t
  {
    //! \brief Полный дробный размер окна
    double count;
    //! \brief Дробная часть размера окна
    double count_fractional;
    //! \brief Целая часть размера окна + 1
    size_type max_size;
    //! \brief Текущий размер окна
    size_type size;
    //! \brief Сумма квадратов
    calc_t square_sum;
    
    // Переменные для предустановки
    //! \brief Текущий предустановленный остаток суммы
    calc_t square_sum_preset_part;
    //! \brief Полный дробный размер окна ближайшего расширения
    double count_expand;
    //! \brief Целая часть размера окна + 1 ближайшего расширения
    size_type max_size_expand;
    //! \brief Текущий размер окна ближайшего расширения
    size_type size_expand;
    //! \brief Сумма квадратов постоянно расширяемая
    calc_t square_sum_expand;
    //! \brief Полный дробный размер окна после предустановки
    double count_preset;
    //! \brief Размер окна после предустановки
    size_type max_size_preset;
    //! \brief Период
    double period;
    //! \brief Сумма квадратов на периоде
    calc_t square_sum_period;
    //! \brief Режим предустановки
    bool is_preset_mode;
    
    window_t():
      count(0.),
      count_fractional(0.),
      max_size(0),
      size(0),
      square_sum(),
      
      // Переменные для предустановки
      square_sum_preset_part(),
      count_expand(0.),
      max_size_expand(0),
      size_expand(0),
      square_sum_expand(),
      count_preset(0.),
      max_size_preset(0),
      period(0.),
      square_sum_period(),
      is_preset_mode(false)
    {
    }
  };
  vector<window_t> m_windows;
};

template<class data_t, class calc_t>
fast_multi_sko_with_single_average_as_t<data_t, calc_t>::
fast_multi_sko_with_single_average_as_t(const vector<calc_t> &a_sizes,
  calc_t a_average_size, calc_t a_average_default
):
  m_max_count(0),
  m_average(a_average_size, a_average_default),
  m_square_elems(),
  m_windows()
{
  m_windows.reserve(a_sizes.size());
  for (size_type i = 0; i < a_sizes.size(); i++) {
    window_t window;
    window.count = a_sizes[i];
    window.max_size = static_cast<size_type>(a_sizes[i]);
    window.count_fractional = a_sizes[i] - window.max_size;
    window.max_size++;
    m_max_count = max(m_max_count, window.max_size);
    m_windows.push_back(window);
  }
  #if IRS_USE_DEQUE_DATA
  m_square_elems.reserve(m_max_count);
  #endif // IRS_USE_DEQUE_DATA
}

template<class data_t, class calc_t>
fast_multi_sko_with_single_average_as_t<data_t, calc_t>::
~fast_multi_sko_with_single_average_as_t()
{
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_as_t<data_t, calc_t>::resize(
  size_type a_index,
  calc_t a_new_size)
{
  window_t& window = m_windows[a_index];

  window.count = a_new_size;
  window.max_size = static_cast<size_type>(a_new_size);
  window.count_fractional = a_new_size - window.max_size;
  
  // Нужен дополнительный отсчет для работы с дробной частью
  window.max_size++;
  
  window.is_preset_mode = false;
  
  if (window.size >= window.max_size) {
    const size_type shift = window.size + 1;
    const size_type count = shift - window.max_size;
    
    // Если окно больше количества удаляемых элементов,
    // то удаляем элементы из суммы
    if (window.size > count) {
      const size_type start_index = m_square_elems.size() - shift;
      const size_type stop_index = start_index + count;
      for (size_type i = start_index; i < stop_index; i++) {
        window.square_sum -= m_square_elems[i];
      }
      
    // Если окно меньше или равно количеству удаляемых элементов,
    // то рассчитываем сумму заново
    } else {
      const size_type start_index = m_square_elems.size() - window.max_size;
      const size_type stop_index = m_square_elems.size() - 1;
      window.square_sum = 0;
      for (size_type i = start_index; i < stop_index; i++) {
        window.square_sum += m_square_elems[i];
      }
    }
    
    window.size -= count;
  }

  m_max_count = 0;
  for (size_type i = 0; i < m_windows.size(); i++) {
    if (m_windows[i].is_preset_mode) {
      m_max_count = max(m_max_count, m_windows[i].max_size_preset);
    } else {
      m_max_count = max(m_max_count, m_windows[i].max_size);
    }
  }

  m_square_elems.reserve(m_max_count);

  if (m_square_elems.size() > m_max_count) {
    m_square_elems.pop_front(m_square_elems.size() - m_max_count);
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_as_t<data_t, calc_t>::resize_preset(
  size_type a_index, calc_t a_new_size, double a_period)
{
  window_t& window = m_windows[a_index];
  
  window.count_preset = a_new_size;
  window.max_size_preset = static_cast<size_type>(a_new_size) + 1;
  
  // Окно уменьшается
  bool is_window_reduce = (window.max_size_preset <= window.max_size);
  // Окно заполнено
  bool is_window_full = (window.size >= window.max_size - 1);
  
  if (is_window_reduce || !is_window_full) {
    resize(a_index, a_new_size);
  } else {
    window.is_preset_mode = true;
    window.period = a_period;
    
    double periods_in_window = window.count/window.period;
    calc_t square_sum = window.square_sum +
      m_square_elems.back()*window.count_fractional;
    window.square_sum_period = square_sum/periods_in_window;
    
    double periods_in_preset_part =
      (window.count_preset - window.count)/window.period;
    window.square_sum_preset_part =
      periods_in_preset_part*window.square_sum_period;
    
    window.count_expand = window.count + window.period;
    window.max_size_expand = static_cast<size_type>(window.count_expand) + 1;
    window.size_expand = window.size;
    window.square_sum_expand = window.square_sum;

    m_max_count = 0;
    for (size_type i = 0; i < m_windows.size(); i++) {
      if (m_windows[i].is_preset_mode) {
        m_max_count = max(m_max_count, m_windows[i].max_size_preset);
      } else {
        m_max_count = max(m_max_count, m_windows[i].max_size);
      }
    }

    m_square_elems.reserve(m_max_count);
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_as_t<data_t, calc_t>::
resize_average(calc_t a_new_size)
{
  m_average.resize(a_new_size);
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_as_t<data_t, calc_t>::
resize_preset_average(calc_t a_new_size, double a_period)
{
  m_average.resize_preset(a_new_size, a_period);
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_as_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_as_t<data_t, calc_t>::size(
  size_type a_index) const
{
  return m_windows[a_index].size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_as_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_as_t<data_t, calc_t>::size() const
{
  return m_square_elems.size();
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_as_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_as_t<data_t, calc_t>::average_size() const
{
  return m_average.size();
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_as_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_as_t<data_t, calc_t>::
max_size(size_type a_index) const
{
  return m_windows[a_index].max_size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_as_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_as_t<data_t, calc_t>::max_size() const
{
  return m_max_count;
}

template<class data_t, class calc_t>
typename fast_multi_sko_with_single_average_as_t<data_t, calc_t>::size_type
fast_multi_sko_with_single_average_as_t<data_t, calc_t>::average_max_size() const
{
  return m_average.max_size();
}

template<class data_t, class calc_t>
bool fast_multi_sko_with_single_average_as_t<data_t, calc_t>::
is_full(size_type a_index) const
{
  return (m_windows[a_index].size == (m_windows[a_index].max_size - 1)) &&
    m_average.is_full();
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_as_t<data_t, calc_t>::preset(
  size_type a_start_pos, size_type a_size)
{
  preset_average(a_start_pos, a_size);

  const size_type erase_front_count = min(m_square_elems.size(), a_start_pos);
  m_square_elems.pop_front(erase_front_count);
  const size_type erase_back_count = min(m_square_elems.size(),
    m_square_elems.size() - a_size);
  m_square_elems.pop_back(erase_back_count);
  const size_type size = min(m_square_elems.size(), a_size);
  if (size > 0) {
    while (m_square_elems.size() < m_max_count) {
      for (size_type i = 0; i < size; i++) {
        m_square_elems.push_back(m_square_elems[i]);
        if (m_square_elems.size() == m_max_count) {
          break;
        }
      }
    }
  }
  for (size_type i = 0; i < m_windows.size(); i++) {
    window_t& window = m_windows[i];
    window.square_sum = 0;
    const size_type min_size = min(m_square_elems.size(), window.max_size);
    const size_type start_pos = m_square_elems.size() - min_size;
    for (size_type j = start_pos; j < m_square_elems.size(); j++) {
      window.square_sum += m_square_elems[j];
    }
    window.size = min_size;
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_as_t<data_t, calc_t>::preset_average(
  size_type a_start_pos, size_type a_size)
{
  const size_type average_start_pos =
    m_square_elems.size() - m_average.size();
  if ((a_start_pos + a_size) > average_start_pos) {
    size_type average_pos = 0;
    if (a_start_pos < average_start_pos) {
      average_pos = 0;
    } else {
      average_pos = a_start_pos - average_start_pos;
    }
    size_type size = 0;
    if ((a_start_pos + a_size) > (average_start_pos + m_average.size())) {
      size = m_average.size();
    } else {
      size = (average_start_pos + m_average.size()) - (a_start_pos + a_size);
    }
    m_average.preset(average_pos, size);
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_as_t<data_t, calc_t>::clear()
{
  m_square_elems.clear();
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_windows[i].size = 0;
    m_windows[i].square_sum = 0;
    m_windows[i].is_preset_mode = false;
  }
  m_average.clear();
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_as_t<data_t, calc_t>::add(data_t a_val)
{
  m_average.add(a_val);
  
  if (m_max_count == 0) {
    return;
  }

  if (!m_average.is_full()) {
    return;
  }

  IRS_LIB_ASSERT(m_square_elems.size() <= m_max_count);
  for (size_type i = 0; i < m_windows.size(); i++) {
    window_t& window = m_windows[i];
    if (window.max_size > 0) {
      if ((window.size > 0) && (window.size == (window.max_size - 1))) {
        window.square_sum -=
          m_square_elems[m_square_elems.size() - window.max_size];
        window.size--;
      }
    }
  }

  if (!m_square_elems.empty()) {
    for (size_type i = 0; i < m_windows.size(); i++) {
      window_t& window = m_windows[i];
      if (window.size < window.max_size) {
        window.square_sum += m_square_elems.back();
        window.size++;
      }
      if (window.is_preset_mode) {
        if (window.size_expand < window.max_size_expand - 1) {
          // Накапливаем данные для будущего расширения окна
          window.square_sum_expand += m_square_elems.back();
          window.size_expand++;
          
          // Если окно предустановки полностью заполнено
          if (window.size_expand == window.max_size_preset - 1) {
            // Увеличиваем исходное окно до окна предустановки
            window.count = window.count_preset;
            window.size = window.size_expand;
            window.count_fractional = window.count - window.size;
            window.max_size = window.max_size_preset;
            window.square_sum = window.square_sum_expand;
            window.square_sum_preset_part = calc_t();
            
            // Выходим из режима предустановки
            window.is_preset_mode = false;
            
          // Иначе если окно для расширения полностью заполнено
          } else if (window.size_expand == window.max_size_expand - 1) {
            // Увеличиваем исходное окно на 1 период сигнала
            window.count = window.count_expand;
            window.size = window.size_expand;
            window.count_fractional = window.count - window.size;
            window.max_size = window.max_size_expand;
            window.square_sum = window.square_sum_expand;
            window.square_sum_preset_part -= window.square_sum_period;
            
            // Подготавливаем следующее расширение окна
            window.count_expand += window.period;
            window.max_size_expand =
              static_cast<size_type>(window.count_expand) + 1;
          }
        }
      }
    }
  }

  if (m_max_count == m_square_elems.size()) {
    m_square_elems.pop_front();
  }

  calc_t elem = a_val - m_average.get();
  elem *= elem;
  m_square_elems.push_back(elem);
}

template<class data_t, class calc_t>
void fast_multi_sko_with_single_average_as_t<data_t, calc_t>::add_to_average(
  data_t a_val)
{
  m_average.add(a_val);
}

template<class data_t, class calc_t>
calc_t fast_multi_sko_with_single_average_as_t<data_t, calc_t>::get(
  size_type a_index) const
{
  const window_t& window = m_windows[a_index];
  if (window.size == 0) {
    return 0;
  }
  if (window.square_sum < 0) {
    return 0;
  }
  
  calc_t square_sum = window.square_sum +
    m_square_elems.back()*window.count_fractional;
  calc_t n = window.size + window.count_fractional;
  if (window.is_preset_mode) {
    square_sum += window.square_sum_preset_part;
    n = window.count_preset;
  }
  
  return sqrt(square_sum/n);
}

template<class data_t, class calc_t>
calc_t fast_multi_sko_with_single_average_as_t<data_t, calc_t>::average() const
{
  return m_average.get();
}

//! \brief Расчет СКО сигнала, полученного
//!   асинхронной дискретизацией
//! \details Суффикс "as" означает "asynchronous sampling"
template<class data_t, class calc_t>
class fast_multi_sko_as_t
{
public:
  typedef size_t size_type;
  //! \brief Конструктор
  //! \param[in] a_sizes - массив размеров окон
  //! \param[in] a_average_sizes - размер окна для среднего значения
  fast_multi_sko_as_t(const vector<calc_t>& a_sizes,
    const vector<calc_t>& a_average_sizes,
    calc_t a_average_default = 0);
  ~fast_multi_sko_as_t();
  //! \brief Очищает все окна
  void clear();
  //! \brief Добавляет отсчет
  void add(data_t a_val);
  //! \brief Добавляет отсчет в среднее значение
  void add_to_average(data_t a_val);
  //! \brief Возвращает значение sko
  //! \param[in] a_index - индекс окна
  calc_t get(size_type a_index) const;
  //! \brief Возвращает среднее значение
  //! \param[in] a_index - индекс окна
  calc_t average(size_type a_index) const;
  //! \brief Изменяет размер окна
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна
  void resize(size_type a_index, calc_t a_new_size);
  //! \brief Изменяет размер окна среднего значения
  //! \param[in] a_index - индекс окна
  //! \param[in] a_new_size - новый размер окна
  void resize_average(size_type a_index, calc_t a_new_size);
  //! \brief Очищает среднее значение
  void clear_average();
  //! \brief Возвращает размер окна sko
  //! \param[in] a_index - индекс окна
  size_type size(size_type a_index) const;
  //! \brief Возвращает размер окна sko с наибольшим количеством элементов
  size_type size() const;
  //! \brief Возвращает размер окна среднего значения
  //! \param[in] a_index - индекс окна
  size_type average_size(size_type a_index) const;
  //! \brief Возвращает максимальный размер окна sko
  //! \param[in] a_index - индекс окна
  size_type max_size(size_type a_index) const;
  //! \brief Возвращает максимальный размер самого большого окна sko
  size_type max_size() const;
  //! \brief Возвращает максимальный размер окна среднего значения
  //! \param[in] a_index - индекс окна
  size_type average_max_size(size_type a_index) const;
  //! \brief Возвращает статус заполнения окна
  //! \details Для полного заполнения требуется количество отсчетов, равное
  //!   сумме размеров, заданных через функции resize и resize_average. Вначале
  //!   отсчеты используются только для вычисления с среднего значения
  //! \param[in] a_index - индекс окна
  bool is_full(size_type a_index) const;
private:
  fast_multi_sko_as_t();
  size_type m_max_count;
  fast_multi_average_as_t<data_t, calc_t> m_average;
  vector<irs::deque_data_t<data_t> > m_square_elems;
  struct window_t
  {
    calc_t count;
    calc_t count_fractional;
    size_type max_size;
    size_type size;
    calc_t square_sum;
    window_t():
      max_size(0),
      size(0),
      square_sum()
    {
    }
  };
  vector<window_t> m_windows;
};

template<class data_t, class calc_t>
fast_multi_sko_as_t<data_t, calc_t>::
fast_multi_sko_as_t(const vector<calc_t> &a_sizes,
  const vector<calc_t> &a_average_sizes, calc_t a_average_default
):
  m_max_count(0),
  m_average(a_average_sizes, a_average_default),
  m_square_elems(),
  m_windows()
{
  m_square_elems.resize(a_sizes.size());
  for (size_type i = 0; i < a_sizes.size(); i++) {
    window_t window;
    calc_t max_count = 0;
    window.count = a_sizes[i];
    window.count_fractional = modf(a_sizes[i], &max_count);
    window.max_size = static_cast<size_type>(max_count) + 1;
    m_max_count = max(m_max_count, window.max_size);

    #if IRS_USE_DEQUE_DATA
    m_square_elems[i].reserve(window.max_size);
    #endif // IRS_USE_DEQUE_DATA

    m_windows.push_back(window);
  }
}

template<class data_t, class calc_t>
fast_multi_sko_as_t<data_t, calc_t>::
~fast_multi_sko_as_t()
{
}

template<class data_t, class calc_t>
void fast_multi_sko_as_t<data_t, calc_t>::resize(
  size_type a_index,
  calc_t a_new_size)
{
  window_t& window = m_windows[a_index];

  calc_t max_count = 0;
  window.count = a_new_size;
  window.count_fractional = modf(a_new_size, &max_count);
  window.max_size = static_cast<size_type>(max_count) + 1;

  irs::deque_data_t<data_t>& square_elems = m_square_elems[a_index];

  if ((window.size > 0) && (window.size >= window.max_size)) {
    //const size_type start_index = square_elems.size() - window.max_size;
    const size_type count = (window.size - window.max_size) + 1;
    for (size_type i = 0; i < count; i++) {
      window.square_sum -= square_elems[i]; //square_elems[start_index + i];
    }
    window.size -= count;
  }

  m_max_count = 0;
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_max_count = max(m_max_count, m_windows[i].max_size);
  }

  square_elems.reserve(window.max_size);

  if (square_elems.size() > window.max_size) {
    square_elems.pop_front(square_elems.size() - window.max_size);
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_as_t<data_t, calc_t>::
resize_average(size_type a_index, calc_t a_new_size)
{
  m_average.resize(a_index, a_new_size);
}

template<class data_t, class calc_t>
typename fast_multi_sko_as_t<data_t, calc_t>::size_type
fast_multi_sko_as_t<data_t, calc_t>::size(
  size_type a_index) const
{
  return m_windows[a_index].size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_as_t<data_t, calc_t>::size_type
fast_multi_sko_as_t<data_t, calc_t>::size() const
{
  size_type size = 0;
  for (size_type i = 0; i < m_square_elems.size(); i++) {
    size = max(size, m_square_elems[i].size());
  }
  return size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_as_t<data_t, calc_t>::size_type
fast_multi_sko_as_t<data_t, calc_t>::average_size(size_type a_index) const
{
  return m_average.size(a_index);
}

template<class data_t, class calc_t>
typename fast_multi_sko_as_t<data_t, calc_t>::size_type
fast_multi_sko_as_t<data_t, calc_t>::
max_size(size_type a_index) const
{
  return m_windows[a_index].max_size;
}

template<class data_t, class calc_t>
typename fast_multi_sko_as_t<data_t, calc_t>::size_type
fast_multi_sko_as_t<data_t, calc_t>::max_size() const
{
  return m_max_count;
}

template<class data_t, class calc_t>
typename fast_multi_sko_as_t<data_t, calc_t>::size_type
fast_multi_sko_as_t<data_t, calc_t>::average_max_size(size_type a_index) const
{
  return m_average.max_size(a_index);
}

template<class data_t, class calc_t>
bool fast_multi_sko_as_t<data_t, calc_t>::
is_full(size_type a_index) const
{
  return (m_windows[a_index].size == (m_windows[a_index].max_size - 1)) &&
    m_average.is_full(a_index);
}

template<class data_t, class calc_t>
void fast_multi_sko_as_t<data_t, calc_t>::clear()
{
  for (size_type i = 0; i < m_windows.size(); i++) {
    m_windows[i].size = 0;
    m_windows[i].square_sum = 0;
    m_square_elems[i].clear();
  }
  m_average.clear();
}

template<class data_t, class calc_t>
void fast_multi_sko_as_t<data_t, calc_t>::add(data_t a_val)
{
  m_average.add(a_val);

  if (m_max_count == 0) {
    return;
  }

  for (size_type i = 0; i < m_windows.size(); i++) {
    if (!m_average.is_full(i)) {
      continue;
    }

    window_t& window = m_windows[i];
    irs::deque_data_t<data_t>& square_elems = m_square_elems[i];
    IRS_LIB_ASSERT(m_square_elems.size() <= window.max_size);

    if (window.max_size > 0) {
      if ((window.size > 0) && (window.size == (window.max_size - 1))) {
        window.square_sum -=
          square_elems[square_elems.size() - window.max_size];
        window.size--;
      }
    }

    if (!square_elems.empty()) {
      if (window.size < window.max_size) {
        window.square_sum += square_elems.back();
        window.size++;
      }
    }

    if (window.max_size == square_elems.size()) {
      square_elems.pop_front();
    }

    calc_t elem = a_val - m_average.get(i);
    elem *= elem;
    square_elems.push_back(elem);
  }
}

template<class data_t, class calc_t>
void fast_multi_sko_as_t<data_t, calc_t>::add_to_average(
  data_t a_val)
{
  m_average.add(a_val);
}

template<class data_t, class calc_t>
calc_t fast_multi_sko_as_t<data_t, calc_t>::get(
  size_type a_index) const
{
  const window_t& window = m_windows[a_index];
  if (window.size == 0) {
    return 0;
  }
  if (window.square_sum < 0) {
    return 0;
  }
  calc_t square_sum = window.square_sum +
    m_square_elems[a_index].back()*window.count_fractional;
  const calc_t n = window.size + window.count_fractional;
  return sqrt(square_sum/n);
}

template<class data_t, class calc_t>
calc_t fast_multi_sko_as_t<data_t, calc_t>::average(size_type a_index) const
{
  return m_average.get(a_index);
}

//! \brief Реализация медианного фильтра
template<class data_t, class calc_t>
class median_filter_t
{
public:
  typedef size_t size_type;
  median_filter_t(size_type a_max_count);
  ~median_filter_t();
  void clear();
  void add(data_t a_val);
  calc_t get() const;
  void resize(size_type a_size);
  size_type size() const;
  size_type max_size() const;
  bool is_full();
private:
  typedef typename multiset<data_t>::iterator multiset_iterator_type;
  median_filter_t();
  void update_result();
  size_type difference_size_first_second_half() const;
  size_type m_max_count;
  enum half_t {
    first_half,
    second_half
  };
  deque_data_t<data_t> m_deque;
  multiset<data_t> m_first_half;
  multiset<data_t> m_second_half;
  calc_t m_result;
};

template<class data_t, class calc_t>
median_filter_t<data_t, calc_t>::
median_filter_t(size_type a_max_count
):
  m_max_count(a_max_count),
  m_deque(),
  m_first_half(),
  m_second_half(),
  m_result(0)
{
  m_deque.reserve(m_max_count);
}

template<class data_t, class calc_t>
median_filter_t<data_t, calc_t>::~median_filter_t()
{
}

template<class data_t, class calc_t>
void median_filter_t<data_t, calc_t>::clear()
{
  m_deque.clear();
  m_first_half.clear();
  m_second_half.clear();
  m_result = 0;
}

template<class data_t, class calc_t>
void median_filter_t<data_t, calc_t>::add(data_t a_val)
{
  if (m_max_count == 0) {
    return;
  } else if (m_max_count == 1) {
    m_first_half.clear();
    m_second_half.clear();
    m_first_half.insert(a_val);
    m_deque.push_back(a_val);
    m_result = a_val;
    return;
  }

  if (m_deque.size() >= m_max_count) {
    const data_t erase_value = m_deque.front();
    multiset_iterator_type it = m_first_half.find(erase_value);
    if (it != m_first_half.end()) {
      m_first_half.erase(it);
    } else {
      multiset_iterator_type erase_it = m_second_half.find(erase_value);
      IRS_LIB_ASSERT(erase_it != m_second_half.end());
      m_second_half.erase(erase_it);
    }
    m_deque.pop_front();
  }
  if (m_first_half.empty() && m_second_half.empty()) {
    m_first_half.insert(a_val);
  } else if (!m_first_half.empty() && m_second_half.empty()) {
    const data_t first_half_back = *m_first_half.rbegin();
    if (first_half_back < a_val) {
      m_second_half.insert(a_val);
    } else {
      m_second_half.insert(first_half_back);
      multiset_iterator_type back_it = --m_first_half.end();
      m_first_half.erase(back_it);
      m_first_half.insert(a_val);
    }
  } else if (m_first_half.empty() && !m_second_half.empty()) {
    const data_t second_half_front = *m_second_half.begin();
    if (second_half_front > a_val) {
      m_first_half.insert(a_val);
    } else {
      m_first_half.insert(second_half_front);
      m_second_half.erase(m_second_half.begin());
      m_second_half.insert(a_val);
    }
  } else {
    const data_t first_half_back = *m_first_half.rbegin();
    if (first_half_back < a_val) {
      if (m_first_half.size() < m_second_half.size()) {
        multiset_iterator_type back_it = --m_first_half.end();
        m_first_half.insert(back_it, *m_second_half.begin());
        m_second_half.erase(m_second_half.begin());
      }
      m_second_half.insert(a_val);
    } else {
      if (m_first_half.size() > m_second_half.size()) {
        m_second_half.insert(m_second_half.begin(), *m_first_half.rbegin());
        multiset_iterator_type back_it = --m_first_half.end();
        m_first_half.erase(back_it);
      }
      m_first_half.insert(a_val);
    }
  }
  m_deque.push_back(a_val);
  update_result();
}

template<class data_t, class calc_t>
void median_filter_t<data_t, calc_t>::update_result()
{
  if (m_first_half.empty() && m_second_half.empty()) {
    m_result = 0;
    return;
  }
  if (m_first_half.size() > m_second_half.size()) {
    m_result = *m_first_half.rbegin();
  } else if (m_first_half.size() < m_second_half.size()) {
    m_result = *m_second_half.begin();
  } else {
    m_result = (static_cast<calc_t>(*m_first_half.rbegin()) +
      static_cast<calc_t>(*m_second_half.begin()))/2;
  }
}

template<class data_t, class calc_t>
calc_t median_filter_t<data_t, calc_t>::get() const
{
  return m_result;
}

template<class data_t, class calc_t>
void median_filter_t<data_t, calc_t>::resize(size_type a_size)
{
  m_max_count = a_size;
  while (m_deque.size() > m_max_count) {
    const data_t erase_value = m_deque.front();
    multiset_iterator_type it = m_first_half.find(erase_value);
    if (it != m_first_half.end()) {
      m_first_half.erase(it);
    } else {
      multiset_iterator_type erase_it = m_second_half.find(erase_value);
      IRS_LIB_ASSERT(erase_it != m_second_half.end());
      m_second_half.erase(erase_it);
    }
    m_deque.pop_front();
  }
  while (difference_size_first_second_half() > 1) {
    if (m_first_half.size() > m_second_half.size()) {
      m_second_half.insert(m_second_half.begin(), *m_first_half.rbegin());
      multiset_iterator_type back_it = --m_first_half.end();
      m_first_half.erase(back_it);
    } else if (m_first_half.size() < m_second_half.size()) {
      m_first_half.insert(m_first_half.end(), *m_second_half.begin());
      m_second_half.erase(m_second_half.begin());
    }
  }
  m_deque.reserve(a_size);
  update_result();
}

template<class data_t, class calc_t>
typename median_filter_t<data_t, calc_t>::size_type
median_filter_t<data_t, calc_t>::difference_size_first_second_half() const
{
  const size_type size_1 = m_first_half.size();
  const size_type size_2 = m_second_half.size();

  return size_1 > size_2 ? (size_1 - size_2) : (size_2 - size_1);
}

template<class data_t, class calc_t>
typename median_filter_t<data_t, calc_t>::size_type
median_filter_t<data_t, calc_t>::size() const
{
  return m_deque.size();
}

template<class data_t, class calc_t>
typename median_filter_t<data_t, calc_t>::size_type
median_filter_t<data_t, calc_t>::max_size() const
{
  return m_max_count;
}

template<class data_t, class calc_t>
bool median_filter_t<data_t, calc_t>::is_full()
{
  return (m_deque.size() == m_max_count);
}

/*
template<class data_t, class calc_t>
class eliminating_outliers_smirnov_criterion_t
{
public:
  typedef size_t size_type;
  eliminating_outliers_smirnov_criterion_t(size_type a_count,
    size_type a_average_sample_count,
    const double a_level_of_significance);
  void clear();
  bool add(data_t a_value);
  calc_t sko() const;
  calc_t relative() const;
  calc_t average() const;
  void resize(size_type a_size);
  void resize_average(size_type a_size);
  void clear_average();
  size_type size() const;
  size_type average_size() const;
  size_type max_size() const;
  size_type average_max_size();
  bool is_full();
private:
  eliminating_outliers_smirnov_criterion_t();
  const double m_level_of_significance;
  fast_sko_t<data_t, calc_t> m_sko;
};

template<class data_t, class calc_t>
eliminating_outliers_smirnov_criterion_t::
eliminating_outliers_smirnov_criterion_t(size_type a_count,
  size_type a_average_sample_count,
  const double a_level_of_significance
):
  m_sko(a_count, a_average_sample_count),
  m_level_of_significance(a_level_of_significance)
{
}

template<class data_t, class calc_t>
bool eliminating_outliers_smirnov_criterion_t::add(data_t a_value)
{
  m_sko.add(a_value);
  const calc_t sko = m_sko;
  const calc_t mean = m_sko.average();
  const calc_t level = m_level_of_significance;
  const size_type n = m_sko.size();
  double critical_value = 0;
  if (size < 3) {
    return ap_last;
  } else if (size <= 25) {
    critical_value = critical_values_for_smirnov_criterion(level, size);
  } else {
    critical_value = critical_values_for_t_a_criterion(alevel, size);
  }
  if (abs(a_value - mean)/sko <= critical_value) {
    return true;
  }
}

template<class data_t, class calc_t>
calc_t eliminating_outliers_smirnov_criterion_t::sko() const
{
  return m_sko;
}

template<class data_t, class calc_t>
calc_t eliminating_outliers_smirnov_criterion_t::relative() const
{
  return m_sko.relative();
}

template<class data_t, class calc_t>
calc_t eliminating_outliers_smirnov_criterion_t::average() const
{
  return m_sko.average();
}

template<class data_t, class calc_t>
void eliminating_outliers_smirnov_criterion_t::resize(size_type a_size)
{
  return m_sko.resize(a_size);
}

template<class data_t, class calc_t>
void eliminating_outliers_smirnov_criterion_t::resize_average(size_type a_size)
{
  m_sko.resize_average(a_size);
}

template<class data_t, class calc_t>
void eliminating_outliers_smirnov_criterion_t::clear_average()
{
  m_sko.clear_average();
}

template<class data_t, class calc_t>
eliminating_outliers_smirnov_criterion_t::size_type
eliminating_outliers_smirnov_criterion_t::size() const
{
  return m_sko.size();
}

template<class data_t, class calc_t>
eliminating_outliers_smirnov_criterion_t::size_type
eliminating_outliers_smirnov_criterion_t::average_size() const
{
  m_sko.average_size();
}

template<class data_t, class calc_t>
eliminating_outliers_smirnov_criterion_t::size_type
eliminating_outliers_smirnov_criterion_t::max_size() const
{
  m_sko.max_size();
}

template<class data_t, class calc_t>
eliminating_outliers_smirnov_criterion_t::size_type
eliminating_outliers_smirnov_criterion_t::average_max_size()
{
  m_sko.average_max_size();
}

template <class T>
bool eliminating_outliers_smirnov_criterion_t::is_full()
{
  m_sko.is_full();
}
*/

//! \addtogroup signal_processing_group
//! @{

//! \brief CRC-16/IBM
//! \details
//!   Width: 16
//!   Poly: 0x8005 x^16 + x^15 + x^2 + 1
//!   Init: 0xFFFF
//!   RefIn: true
//!   RefOut: true
//!   XorOut: 0x0000
//!   Check : 0x4B37 ("123456789")
//!   Максимальная длина: 4095 байт (32767 бит) - обнаружение
//!     одинарных, двойных, тройных и всех нечетных ошибок
inline irs_u16 crc16(const irs_u8* ap_buf, size_t a_size)
{
  irs_u16 crc = 0xffff;
  for (size_t i = 0; i < a_size; i++) {
    crc = static_cast<irs_u16>(crc ^ ap_buf[i]);
    for (size_t j = 0;j < 8; j++) {
      crc = static_cast<irs_u16>(
        (crc & 0x0001) ? ((crc >> 1) ^ 0xA001) : (crc >> 1));
    }
  }
  return (crc);
}

struct crc16_data_t {
  enum {
    size = 256
  };
#ifdef __ICCAVR__
#define STATIC_MEMORY_MODIFIER static IRS_ICCAVR_FLASH
#define MEMORY_MODIFIER IRS_ICCAVR_FLASH
#else //__ICCAVR__
#define STATIC_MEMORY_MODIFIER
#define MEMORY_MODIFIER
#endif //__ICCAVR__

  STATIC_MEMORY_MODIFIER irs_u16 table[size];
#ifndef __ICCAVR__

  crc16_data_t();
#endif //__ICCAVR__
};

//! \brief CRC-16/IBM
//! \details
//!   Width: 16
//!   Poly: 0x8005 x^16 + x^15 + x^2 + 1
//!   Init: 0xFFFF
//!   RefIn: true
//!   RefOut: true
//!   XorOut: 0x0000
//!   Check : 0x4B37 ("123456789")
//!   Максимальная длина: 4095 байт (32767 бит) - обнаружение
//!     одинарных, двойных, тройных и всех нечетных ошибок
//!   Потребление памяти: не менее 512 байт
inline irs_u16 crc16_table(const irs_u8* ap_buf, size_t a_size)
{
  static handle_t<crc16_data_t> crc16_data = new crc16_data_t();
  irs_u16 crc = 0xFFFF;
  for (size_t i = 0; i < a_size; i++) {
    crc = static_cast<irs_u16>(
      (crc >> 8) ^ crc16_data->table[(crc & 0xFF) ^ *(ap_buf++)]);
  }
  return crc;
}


struct crc32_data_t {
  enum {
    size = 256
  };
#ifdef __ICCAVR__
#define STATIC_MEMORY_MODIFIER static IRS_ICCAVR_FLASH
#define MEMORY_MODIFIER IRS_ICCAVR_FLASH
#else //__ICCAVR__
#define STATIC_MEMORY_MODIFIER
#define MEMORY_MODIFIER
#endif //__ICCAVR__

  STATIC_MEMORY_MODIFIER irs_u32 table[size];
#ifndef __ICCAVR__

  crc32_data_t();
#endif //__ICCAVR__
};

//! \brief Зеркальный табличный метод расчета crc32
//! \details Зеркальный метод является наиболее распространенным
//!   Необходимый объем оперативной памяти не менее 1024 байт
//!   Значительный прирост в скорости вычисления по сравнению с нетабличным
//!   методом.
//!   Тесты производительности (массив на 1000 байт):
//!     - STM32f217ZGT (120 МГц, IAR 6.3 без оптимизации) - 421 мкс.
// Перенесен из cpp- в h-файл и сделан шаблонным, чтобы можно было вычислять
//  для eeprom-указателей в AVR
template<class T>
irs_u32 crc32_table(const T* ap_buf, const size_t a_size)
{
  IRS_STATIC_ASSERT(sizeof(T) == 1);
  static handle_t<crc32_data_t> crc32_data = new crc32_data_t();
  irs_u32 crc = 0xFFFFFFFF;
  for (size_t i = 0; i < a_size; i++) {
    crc = ((crc >> 8) & 0x00FFFFFF) ^
      crc32_data->table[(crc ^ *(ap_buf++)) & 0xFF];
  }
  return ~crc;
}

class crc32_table_stream_t
{
public:
  typedef size_t size_type;
  crc32_table_stream_t();
  template<class T>
  inline void put(const T* ap_buf, size_type a_size = 1);
  irs_u32 get_crc();
  void reset();
private:
  static handle_t<crc32_data_t> mp_crc32_data;
  irs_u32 m_crc;
};

template<class T>
inline void crc32_table_stream_t::put(const T* ap_buf, size_type a_size)
{
  IRS_STATIC_ASSERT(sizeof(T) == 1);
  for (size_t i = 0; i < a_size; i++) {
    m_crc = ((m_crc >> 8) & 0x00FFFFFF) ^
      mp_crc32_data->table[(m_crc ^ *(ap_buf++)) & 0xFF];
  }
}

//! \brief Расчет СКО. Тип T должен быть 32-х битным
//! \details Алгоритм является зеркальным, но не совпадает с распространенными
//!   алгоритмами. В распространенных алгоритмах инвертируется выходное
//!   значение. Функция используется для внутренних потребностей и
//!   переделыватся не будет.
template<class T>
irs_u32 crc32(T * a_buf, irs_uarc a_start, irs_uarc a_cnt) {
  // Взято с википедии
  IRS_STATIC_ASSERT(sizeof(T) == 4);
  irs_uarc top = a_start + a_cnt;
  irs_u32 crc = 0xFFFFFFFF; /* Начальное состояние сдвигового регистра */
  irs_u8 flag = 0;
  for (irs_uarc i = a_start; i < top; i++) {
    crc = crc ^ a_buf[i];
    for (irs_uarc j = 1; j <= 32; j++) {
      flag = IRS_LOBYTE(crc) & irs_u8(1);
      crc >>= 1; /* Сдвиг регистра на 1 позицию */
      if (flag)
        crc ^= 0xEDB88320; // 0xEDB88320 - примитивный полином
    }
  }
  return(crc); /* Здесь должно быть return (~crc) */
}

template<class T>
inline irs_u32 crc32(T * a_buf, size_t a_size) {
  return crc32(a_buf, 0, a_size);
}

irs_u8 crc8(irs_u8 * a_buf, irs_u8 a_start, irs_u8 a_cnt);

//! \brief Приведение фазы к диапазону a_phase_begin - a_phase_end
double phase_normalize(double a_phase_in, double a_phase_begin = 0.,
  double a_phase_end = 360.);

//! \brief Приведение фазы к диапазону -180 - +180
double phase_normalize_180(double a_phase_in);

//! \brief Приведение фазы к диапазону a_phase_begin - a_phase_end
template<class T>
T phase_norm(T a_phase_in, T a_phase_begin = 0., T a_phase_end = 360.) {
  const T phase_period = a_phase_end - a_phase_begin;
  T div_int = floor((a_phase_in - a_phase_begin) / phase_period);
  T phase_out = a_phase_in - div_int * phase_period;

  return phase_out;
}

//! \brief Приведение фазы к диапазону -180 - +180
template<class T>
T phase_norm_180(T a_phase_in) {
  return phase_norm(a_phase_in, T(-180.), T(+180.));
}

//! \brief функция интерполяции массива точек прямой методом МНК.
//! \details Записывает в a_korf_k и a_koef_b коэффициенты
//!   прямой y=a_korf_k*x+a_koef_b, апроксимирующей набор точек, представленный
//!   массивами a_array_x и a_array_y, размера a_size
template<class TYPE>
void interp_line_mnk(TYPE* a_array_x, TYPE* a_array_y,
  const unsigned int& a_size, long double& a_koef_k, long double& a_koef_b) {
  long double S_xy = 0.0, S_x2 = 0.0, S_x = 0.0, S_y = 0.0;
  for (unsigned int i = 0; i < a_size; i++) {
    S_xy = S_xy + a_array_x[i] * a_array_y[i];
    S_x2 = S_x2 + a_array_x[i] * a_array_x[i];
    S_x = S_x + a_array_x[i];
    S_y = S_y + a_array_y[i];
  }
  a_koef_k = (S_xy - S_x * S_y / a_size) / (S_x2 - S_x * S_x / a_size);
  a_koef_b = (S_y - a_koef_k * S_x) / a_size;
}

//! \brief Вычисляет коэффициенты k и b прямой y = k*x+b
template<class TYPE>
int koef_line(const TYPE a_y1, const TYPE a_x1, const TYPE a_y2,
  const TYPE a_x2, TYPE& a_k, TYPE& a_b) {
  if (a_x1 == a_x2)
    return 1;
  TYPE difference = a_x1 - a_x2;
  a_k = (a_y1 - a_y2) / difference;
  a_b = (a_y2 * a_x1 - a_x2 * a_y1) / difference;
  return 0;
}

//! \brief Функция возвращает среднее арифметическое значение элементов массива
//! \details Аналог функции Mean из библиотеки Math.hpp C++ Builder6
double mean(const double* ap_data, const int a_data_size);

//! \brief Функция возвращает дисперсию элементов массива. Рассчитывается по
//!   формуле \f$\frac{1}{N-1}\sum_{i=1}^{N}(x_i-\bar{x})^2\f$
//! \details Аналог функции Variance из библиотеки Math.hpp C++ Builder6
double variance(const double* ap_data, const int a_data_size);

//! \brief Функция возвращает среднее квадратическое отклонение элементов
//!   массива. Рассчитывается по формуле
//!   \f$\sqrt{\frac{1}{N-1}\sum_{i=1}^{N}(x_i-\bar{x})^2}\f$
//! \details Аналог функции StdDev из библиотеки Math.hpp C++ Builder6
//! \see sample_standard_deviation
//! \see sko_calc_t
//! \see fast_sko_t
//! \see fast_multi_sko_with_single_average_t
//! \see fast_multi_sko_t
double std_dev(const double* ap_data, const int a_data_size);

template<class In, class Out, class Pred>
Out copy_if(In a_first, In a_last, Out a_res, Pred a_pred) {
  while (a_first != a_last) {
    if (a_pred(*a_first)) {
      *a_res++ = *a_first;
    }
    ++a_first;
  }
  return a_res;
}

//! @}

} // namespace irs

#endif //IRSALGH
