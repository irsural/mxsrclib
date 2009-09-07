// ������� ��� mxifa (mxifa routines)
// ����: 2.04.2008

//---------------------------------------------------------------------------
#ifndef mxifarH
#define mxifarH

#include <irsdefs.h>

// ����� ������ ��� ip-������
#define IPSTR_LEN 16
// ������������ ����� �������� ip-������
#define IPNUM_MAX 255
//---------------------------------------------------------------------------
// ��� IP-������
typedef struct _mxip_t {
  irs_u8 val[4];
} mxip_t;
//---------------------------------------------------------------------------
// ������� IP
extern const mxip_t zero_ip;
//---------------------------------------------------------------------------
// �������� == ��� mxip_t
irs_bool operator ==(mxip_t ip1, mxip_t ip2);
// �������� != ��� mxip_t
irs_bool operator !=(mxip_t ip1, mxip_t ip2);
// �������������� IP � ������
void mxip_to_cstr(char *ip_in_str, mxip_t ip);
// ��������������, ���� �������� ������ � IP
irs_bool cstr_to_mxip(mxip_t &ip, char *str_ip);
//---------------------------------------------------------------------------
#endif //mxifarH
