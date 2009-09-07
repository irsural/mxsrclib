// Стандартаная библиотека ИРС локальная часть
// Дата 26.09.2008

#include <irsstdl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//---------------------------------------------------------------------------
// Преобразование числа double в строку в %g формате
irs_u8 *irs_gcvt(double value, int ndigits, irs_u8 *buffer)
{
  //return (irs_u8 *)gcvt(value, ndigits, (char *)buffer);
	return buffer;
}
//---------------------------------------------------------------------------
void mxlinux_condrv_t::out(const char *str)
{
  printf(str);
}
//---------------------------------------------------------------------------

