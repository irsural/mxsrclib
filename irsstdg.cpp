// Стандартаная библиотека ИРС общая часть
// Дата: 28.04.2010
// Ранняя дата: 9.09.2009

// Номер файла
#define IRSSTDGCPP_IDX 15

#include <irsdefs.h>

#include <ctype.h>
#include <irserror.h>

#include <irsstdg.h>

#include <irsfinal.h>

// Класс для приложения
mxapplication_t::mxapplication_t():
  first_exec(irs_true),
  stop_exec(irs_false),
  f_init(IRS_NULL),
  f_deinit(IRS_NULL),
  f_tick(IRS_NULL),
  f_stop(IRS_NULL),
  f_stopped(IRS_NULL)
{
}
mxapplication_t::~mxapplication_t()
{
}
irs_bool mxapplication_t::run(irs_bool exec)
{
  if (exec)
  if (first_exec) {
    first_exec = irs_false;
    init();
  }
  if (!first_exec) {
    if (!exec && !stop_exec) {
      stop_exec = irs_true;
      stop();
    }
    tick();
  }
  if (stopped() && !first_exec) {
    deinit();
    first_exec = irs_true;
    stop_exec = irs_false;
    return irs_false;
  } else {
    return irs_true;
  }
}
void mxapplication_t::set_init_event(void (*a_init)())
{
  f_init = a_init;
}
void mxapplication_t::set_denit_event(void (*a_deinit)())
{
  f_deinit = a_deinit;
}
void mxapplication_t::set_tick_event(void (*a_tick)())
{
  f_tick = a_tick;
}
void mxapplication_t::set_stop_event(void (*a_stop)())
{
  f_stop = a_stop;
}
void mxapplication_t::set_stopped_event(irs_bool (*a_stopped)())
{
  f_stopped = a_stopped;
}
void mxapplication_t::init()
{
  if (f_init) f_init();
}
void mxapplication_t::deinit()
{
  if (f_deinit) f_deinit();
}
void mxapplication_t::tick()
{
  if (f_tick) f_tick();
}
void mxapplication_t::stop()
{
  if (f_stop) f_stop();
}
irs_bool mxapplication_t::stopped()
{
  if (f_stopped) return f_stopped();
  else return irs_true;
}

// Перераспределение памяти с сохранением данных
void *irs_renew(void *pointer, size_t old_size, size_t new_size)
{
  if (old_size == new_size) return pointer;

  void *new_pointer = IRS_NULL;
  if (new_size) {
    new_pointer =
      (void*)IRS_LIB_NEW_ASSERT(irs_u8 *[new_size], IRSSTDGCPP_IDX);
    if (!new_pointer) return new_pointer;
  }
  if (pointer) {
    if (old_size && new_size) {
      memcpy(new_pointer, pointer, irs_min(old_size, new_size));
    }
    IRS_LIB_ARRAY_DELETE_ASSERT((irs_u8*&)pointer);
  }
  return new_pointer;
}
mxapplication_t *mxapplication = IRS_NULL;

void afloat_to_str(char *str, double N, size_t len, size_t accur)
{
  ostrstream strm(str, len + 1);
  strm << setiosflags(ios::fixed) << setw(len) << setprecision(accur) << N;
  strm << '\0';
  str[len] = '\0';
  //str[strm.pcount()] = 0;
}
