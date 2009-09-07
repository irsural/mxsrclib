#include "config.h"
#include <irsdefs.h>

TStrings *BuilderConsole = IRS_NULL;
mxconsole_t *console = IRS_NULL;
mxcondrv_t *condrv = IRS_NULL;

// ������������� ������������
void cfg_init()
{
  condrv = new mxbuilder_condrv_t(BuilderConsole);
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

