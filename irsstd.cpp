// Стандартаная библиотека ИРС
// Дата: 1.12.2008

#include <irsstd.h>
#include <stdio.h>
#include <stdarg.h>
//#include <irscpp.h>

//---------------------------------------------------------------------------
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

