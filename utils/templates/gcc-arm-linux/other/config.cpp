#include "config.h"
#include <irsdefs.h>

mxconsole_t *console = IRS_NULL;
mxcondrv_t *condrv = IRS_NULL;

// ������������� ������������
void cfg_init()
{
  condrv = new mxlinux_condrv_t();
  console = new mxconsole_t(condrv);
}
// �������������� ������������
void cfg_deinit()
{
  delete console;
  delete condrv;
}
// ������������ �������� ������������
void cfg_tick()
{
}

