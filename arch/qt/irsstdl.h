// ������������ ���������� ��� ��������� �����
// ��� linux
// ������ 0.2

#ifndef irsstdlH
#define irsstdlH

#include <irsdefs.h>
#include <irsstdg.h>
// �������������� ����� double � ������ � %g �������
//irs_u8 *irs_gcvt(double value, int ndigits, irs_u8 *buffer);

// ������� ������� ��� linux
class mxlinux_condrv_t: public mxcondrv_t
{
public:
  virtual void out(const char *str);
};
#endif
