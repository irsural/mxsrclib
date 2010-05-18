// Стандартаная библиотека ИРС
// Дата: 18.05.2010
// Ранняя дата: 25.11.2007

#ifndef irsstdH
#define irsstdH

#include <irsdefs.h>

#include <string.h>

//#include <irsstdl.h>
#include <irscpp.h>
#include <timer.h>
#include <irsint.h>
#include <irsstring.h>
#include <mxdatastd.h>
#include <irsconsolestd.h>
#include <irsstrmstd.h>
#include <irschartwin.h>

#include <irsfinal.h>

// Устарел
// Абстактный базовый класс драйвера консоли
class mxcondrv_t
{
public:
  virtual ~mxcondrv_t() {}
  virtual void out(const char *str) = 0;
};

// Класс для вывода отладочной информации
class mxconsole_t
{
private:
  mxcondrv_t *f_condrv;

  // Конструктор по умолчанию запрещен
  mxconsole_t();
public:
  // Конструктор
  mxconsole_t(mxcondrv_t *a_condrv);
  // Деструктор
  ~mxconsole_t();

  // Вывод на консоль по типу puts
  void puts(const char *a_msg);
  // Вывод на консоль по типу printf
  void printf(const char *a_format, ...);
};

#ifdef __embedded_cplusplus
// Приведение IAR-complex в нормальный complex
template <class T>
class complex
{
};
template<> class complex<double>;
template<>
class complex<float>: public float_complex
{
  typedef float T;
  typedef float_complex base;
public:
  complex(const T& a_real = 0, const T& a_imag = 0):
    base(a_real, a_imag)
  {
  }
  complex(const base& a_val):
    base(a_val)
  {
  }
  explicit complex(const complex<double>& a_val):
    base(reinterpret_cast<const double_complex&>(a_val))
  {
  }
  complex<T>& operator=(const T& a_val)
  {
    static_cast<base&>(*this) = a_val;
    return *this;
  }
};
template<>
class complex<double>: public double_complex
{
  typedef double T;
  typedef double_complex base;
public:
  complex(const T& a_real = 0, const T& a_imag = 0):
    base(a_real, a_imag)
  {
  }
  complex(const base& a_val):
    base(a_val)
  {
  }
  complex(const complex<float>& a_val):
    base(static_cast<const float_complex&>(a_val))
  {
  }
  complex<T>& operator=(const T& a_val)
  {
    static_cast<base&>(*this) = a_val;
    return *this;
  }
};
#endif //__embedded_cplusplus

// Устарел
// Класс для приложения
class mxapplication_t
{
  irs_bool first_exec;
  irs_bool stop_exec;
  void (*f_init)();
  void (*f_deinit)();
  void (*f_tick)();
  void (*f_stop)();
  irs_bool (*f_stopped)();

  void init();
  void deinit();
  void tick();
  void stop();
  irs_bool stopped();
public:
  mxapplication_t();
  ~mxapplication_t();
  void set_init_event(void (*a_init)());
  void set_denit_event(void (*a_deinit)());
  void set_tick_event(void (*a_tick)());
  void set_stop_event(void (*a_stop)());
  void set_stopped_event(irs_bool (*a_stopped)());
  irs_bool run(irs_bool exec);
};

// Перераспределение памяти с сохранением данных
void *irs_renew(void *pointer, size_t old_size, size_t new_size);

// Функция Макса Полякова для преобразования double в char*
void afloat_to_str(char *str, double N, size_t len, size_t accur);

extern mxapplication_t *mxapplication;

namespace irs {

// Число на выходе будет в заданных границах
template <class T>
T range(T val, T min_val, T max_val)
{
  return min(max(min_val, val), max_val);
}

} //namespace irs

#endif //irsstdH
