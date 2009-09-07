#ifndef configH
#define configH

#include <vcl.h>
#include <irsstd.h>

extern TStrings *BuilderConsole;
extern mxconsole_t *console;

// Инициализация конфигурации
void cfg_init();
// Денициализация конфигурации
void cfg_deinit();
// Элементарное действие конфигурации
void cfg_tick();

#endif //configH
