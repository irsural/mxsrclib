#ifndef configH
#define configH

#include <irsstd.h>

extern mxconsole_t *console;

// ������������� ������������
void cfg_init();
// �������������� ������������
void cfg_deinit();
// ������������ �������� ������������
void cfg_tick();

#endif //configH
