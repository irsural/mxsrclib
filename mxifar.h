// ������� ��� mxifa (mxifa routines)
// ����: 17.09.2009

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

namespace irs {

// �������� mxip_t
inline mxip_t make_mxip(int first_octet, int second_octet, int third_octet,
  int fourth_octet)
{
  mxip_t ip = {{
    static_cast<irs_u8>(first_octet),
    static_cast<irs_u8>(second_octet),
    static_cast<irs_u8>(third_octet),
    static_cast<irs_u8>(fourth_octet)
  }};
  return ip;
}

} //namespace irs
//---------------------------------------------------------------------------
#endif //mxifarH
