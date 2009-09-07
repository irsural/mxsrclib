//#include <irsstd.h>
#include "mainprg.h"
#include "config.h"

irs_bool stopping = irs_false;
irs_bool first = irs_true;

// Инициализация
void init()
{
  cfg_init();
  stopping = irs_false;
}
// Деинициализация
void deinit()
{
  cfg_deinit();
}
// Элементарное действие
void tick()
{
  cfg_tick();
  if (first) {
    first = irs_false;
    console->puts("Привет!");
    int x = 77;
    console->printf("x = %d\n", x);
  }
}
// Останов программы извне
void stop()
{
  stopping = irs_true;
}
// Ожидание останова
irs_bool stopped()
{
  return stopping;
}

