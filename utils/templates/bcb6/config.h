#ifndef configH
#define configH

#include <vcl.h>
#include <irsstd.h>

extern TStrings *BuilderConsole;
extern mxconsole_t *console;

// ������������� ������������
void cfg_init();
// �������������� ������������
void cfg_deinit();
// ������������ �������� ������������
void cfg_tick();

#endif //configH
