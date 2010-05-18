// Стандартаная библиотека ИРС
// Дата: 18.05.2010
// Ранняя дата: 25.11.2007

#include <irsdefs.h>

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <irserror.h>

#include <irsstd.h>
//#include <irscpp.h>

#include <irsfinal.h>

// Конструктор по умолчанию
mxconsole_t::mxconsole_t()
{
}
// Конструктор
mxconsole_t::mxconsole_t(mxcondrv_t *a_condrv):
  f_condrv(a_condrv)
{
}
// Деструктор
mxconsole_t::~mxconsole_t()
{
}
// Вывод отладочной информации по типу puts
void mxconsole_t::puts(const char *a_msg)
{
  const irs_u8 *msg = (const irs_u8 *)a_msg;
  //const char *dbg_msg = dbg_msg_;
  if (!f_condrv) return;
  if (!msg) return;
  irs_i32 len = strlen((char *)msg);
  //irs_u8 *msg_crlf = (dbg_char *)malloc(len + 2);
  irs_u8 *msg_crlf = new irs_u8[len + 2];
  if (!msg_crlf) return;
  ::strcpy((char *)msg_crlf, (char *)msg);
  msg_crlf[len] = '\n';
  msg_crlf[len + 1] = 0;
  f_condrv->out((char *)msg_crlf);
  delete []msg_crlf;
}
// Вывод отладочной информации по типу printf
void mxconsole_t::printf(const char *a_format, ...)
{
  const irs_u8 *format = (const irs_u8 *)a_format;
  //const char *format = format_;
  if (!f_condrv) return;
  if (!format) return;
  irs_u8 *msgbuf;
  irs_i32 len;
  va_list arglist;
  va_start(arglist, a_format);
  len = vsnprintf(NULL, 0, (char *)format, arglist);
  len++;
  va_end(arglist);
  //msgbuf = (irs_u8 *)malloc(len);
  msgbuf = new irs_u8[len];
  if (!msgbuf) return;
  va_start(arglist, a_format);
  vsnprintf((char *)msgbuf, len, (char *)format, arglist);
  msgbuf[len - 1] = 0;
  va_end(arglist);
  f_condrv->out((char *)msgbuf);
  //free(msgbuf);
  delete []msgbuf;
}

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
