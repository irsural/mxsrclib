// Функции для mxifa (mxifa routines)
// Дата: 17.09.2009

#ifndef mxifarH
#define mxifarH

#include <irsdefs.h>
#include <irsstd.h>
#include <irsstrdefs.h>

// Длина строки для ip-адреса
#define IPSTR_LEN 16
// Максимальное число сегмента ip-адреса
#define IPNUM_MAX 255
//---------------------------------------------------------------------------
// Тип IP-адреса
struct mxip_t {
  irs_u8 val[4];

  static mxip_t zero_ip()
  {
    mxip_t ip = {{0, 0, 0, 0}};
    return ip;
  }
};
//---------------------------------------------------------------------------
// Нулевой IP
extern const mxip_t zero_ip;
//---------------------------------------------------------------------------
// Оператор == для mxip_t
irs_bool operator ==(mxip_t ip1, mxip_t ip2);
// Оператор != для mxip_t
irs_bool operator !=(mxip_t ip1, mxip_t ip2);
// Преобразование IP в строку
void mxip_to_cstr(char *ip_in_str, mxip_t ip);
// Преобразование, если возможно строки в IP
irs_bool cstr_to_mxip(mxip_t &ip, const char *str_ip);

namespace irs {

// Создание mxip_t
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
inline mxip_t make_mxip(const char_t* a_ip)
{
  mxip_t ip = mxip_t::zero_ip();
  if (!cstr_to_mxip(ip, IRS_SIMPLE_CHAR_FROM_TCHAR_STR(a_ip))) {
    ip = mxip_t();
  }
  return ip;
}

} //namespace irs
//---------------------------------------------------------------------------
#endif //mxifarH

