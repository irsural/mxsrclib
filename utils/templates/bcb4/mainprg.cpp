#include "mainprg.h"
#include "config.h"

irs_bool stopping = irs_false;
irs_bool first = irs_true;

// �������������
void init()
{
  cfg_init();
  stopping = irs_false;
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
  if (first) {
    first = irs_false;
    console->puts((irs_u8 *)"������!");
    int x = 77;
    console->printf((irs_u8 *)"x = %d\n", x);
  }
}
// ������� ��������� �����
void stop()
{
  stopping = irs_true;
}
// �������� ��������
irs_bool stopped()
{
  return stopping;
}

