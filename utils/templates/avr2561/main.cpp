#include <irsdefs.h>
#include "mainprg.h"

int main()
{
  init();
  for(;;) {
    tick();
    if (stopped()) break;
  }
  deinit();
  
  return 0;
}
