//#include <irsstd.h>
#include <irsdefs.h>
#include <timer.h>

#include "mainprg.h"
#include "config.h"
#include "hard.h"

counter_t action_to = 0;
#define action_time TIME_TO_CNT(2, 1)

// Инициализация
void init()
{
  cfg_init();
  init_to_cnt();
  set_to_cnt(action_to, action_time);
}
// Деинициализация
void deinit()
{
  deinit_to_cnt();
  cfg_deinit();
}
// Элементарное действие
void tick()
{
  cfg_tick();
  if (test_to_cnt(action_to)) {
    set_to_cnt(action_to, action_time);
    test_action();
  }
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

