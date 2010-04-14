// ������������ ���������� ���
// ����: 14.04.2010
// ������ ����: 25.11.2007

#ifndef irsstdH
#define irsstdH

#include <irsdefs.h>

#include <irsstdl.h>
#include <irscpp.h>

#include <irsfinal.h>

// ����� ��� ������ ���������� ����������
class mxconsole_t
{
private:
  mxcondrv_t *f_condrv;

  // ����������� �� ��������� ��������
  mxconsole_t();
public:
  // �����������
  mxconsole_t(mxcondrv_t *a_condrv);
  // ����������
  ~mxconsole_t();

  // ����� �� ������� �� ���� puts
  void puts(const char *a_msg);
  // ����� �� ������� �� ���� printf
  void printf(const char *a_format, ...);
};

#endif //irsstdH
