// Стандартаная библиотека ИРС
// Дата: 14.04.2010
// Ранняя дата: 25.11.2007

#ifndef irsstdH
#define irsstdH

#include <irsdefs.h>

#include <irsstdl.h>
#include <irscpp.h>

#include <irsfinal.h>

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
