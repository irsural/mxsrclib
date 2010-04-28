// Стандартаная библиотека ИРС общая часть
// Дата: 28.04.2010
// Ранняя дата: 9.09.2009

#ifndef irsstdgH
#define irsstdgH

// Номер файла
#define IRSSTDGH_IDX 16

#include <irsdefs.h>

#include <string.h>

#include <irscpp.h>
#include <timer.h>
#include <irsint.h>
#include <irsstring.h>
#include <mxdatastd.h>
#include <irsconsolestd.h>
#include <irsstrmstd.h>
#include <irschartwin.h>

#include <irsfinal.h>

//typedef string std_string;

// Устарел
// Абстактный базовый класс драйвера консоли
class mxcondrv_t
{
public:
  virtual ~mxcondrv_t() {}
  virtual void out(const char *str) = 0;
};

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

#endif //irsstdgH

