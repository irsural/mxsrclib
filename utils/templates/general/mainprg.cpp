//#include <irsstd.h>
#include "mainprg.h"
#include "config.h"

// �������������
void init()
{
  cfg_init();
}
// ���������������
void deinit()
{
  cfg_deinit();
}
// ������������ ��������
void tick()
{
  cfg_tick();
}
// ������� ��������� �����
void stop()
{
}
// �������� ��������
irs_bool stopped()
{
  return irs_false;
}

