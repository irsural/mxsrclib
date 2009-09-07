//#include <irsstd.h>
#include "mainprg.h"
#include "config.h"

// Инициализация
void init()
{
  cfg_init();
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
}
// Останов программы извне
void stop()
{
}
// Ожидание останова
irs_bool stopped()
{
  return irs_false;
}

