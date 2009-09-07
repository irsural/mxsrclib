#ifndef mainprgH
#define mainprgH

#include <irsdefs.h>

// Инициализация
void init();
// Деинициализация
void deinit();
// Элементарное действие
void tick();
// Останов программы извне
void stop();
// Ожидание останова
irs_bool stopped();

#endif //mainprgH
