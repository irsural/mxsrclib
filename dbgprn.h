// Отладочный вывод
// 18.06.2008

//---------------------------------------------------------------------------
#ifndef dbgprnH
#define dbgprnH

#include <irsdefs.h>
//---------------------------------------------------------------------------
// typedef-определения
typedef irs_u8 dbg_char;
typedef irs_u32 dbg_int;
typedef void (*out_str_fn)(const dbg_char *s);
#ifdef NOP
//---------------------------------------------------------------------------
// Вывод отладочной информации по типу puts
void dputs(out_str_fn out_str, const dbg_char *dbg_msg);
// Вывод отладочной информации по типу printf
void dprintf(out_str_fn out_str, const dbg_char *format, ...);
#endif
//---------------------------------------------------------------------------
// Класс для вывода отладочной информации
class dbg_out
{
private:
  // Поля
  out_str_fn f_out_str;

public:
  // Конструкторы и деструкторы
  dbg_out();
  ~dbg_out();
  dbg_out(out_str_fn a_out_str);

  // Методы
  // Вывод отладочной информации по типу puts
  void puts(const char *dbg_msg);
  // Вывод отладочной информации по типу printf
  void printf(const char *format, ...);
}; //dbg_out
//---------------------------------------------------------------------------
#endif
