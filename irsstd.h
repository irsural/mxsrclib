// Стандартаная библиотека ИРС
// Дата 25.11.2007

#ifndef irsstdH
#define irsstdH

#include <irsstdl.h>
#include <irsdefs.h>
#include <irscpp.h>

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

#endif //irsstdH
