#include "config.h"
#include <irsdefs.h>

mxconsole_t *console = IRS_NULL;
mxcondrv_t *condrv = IRS_NULL;

// Инициализация конфигурации
void cfg_init()
{
  condrv = new mxlinux_condrv_t();
  console = new mxconsole_t(condrv);
}
// Денициализация конфигурации
void cfg_deinit()
{
  delete console;
  delete condrv;
}
// Элементарное действие конфигурации
void cfg_tick()
{
}

