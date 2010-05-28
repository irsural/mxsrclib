// Определение типов mxip_t, mxmac_t и сопутствующих им функций
// Дата: 28.05.2010
// Дата создания: 01.04.2010

#ifndef IRSNETDEFSH
#define IRSNETDEFSH

#include <irsdefs.h>

#include <irsstd.h>
#include <irsstrdefs.h>
#include <irsstrm.h>
#include <ctype.h>

#include <irsfinal.h>

// Длина строки для ip-адреса
#define IP_STR_LEN 16
// Длина строки для mac-адреса
#define MAC_STR_LEN 16
// Максимальное число сегмента ip-адреса
#define IP_NUM_MAX 255
// Максимальное число сегмента mac-адреса
#define MAC_NUM_MAX 255

enum {
  ip_length = 4,
  mac_length = 6
};

struct mxip_t
{
  irs_u8 val[ip_length];

  static mxip_t zero_ip()
  {
    mxip_t ip = {{0, 0, 0, 0}};
    return ip;
  }
  static mxip_t broadcast_ip()
  {
    mxip_t ip = {{0xFF, 0xFF, 0xFF, 0xFF}};
    return ip;
  }
};

inline bool operator ==(mxip_t a_ip1, mxip_t a_ip2)
{
  return (a_ip1.val[0] == a_ip2.val[0])&&(a_ip1.val[1] == a_ip2.val[1])&&
    (a_ip1.val[2] == a_ip2.val[2])&&(a_ip1.val[3] == a_ip2.val[3]);
}

inline bool operator !=(mxip_t a_ip1, mxip_t a_ip2)
{
  return (a_ip1.val[0] != a_ip2.val[0])||(a_ip1.val[1] != a_ip2.val[1])||
    (a_ip1.val[2] != a_ip2.val[2])||(a_ip1.val[3] != a_ip2.val[3]);
}

inline void mxip_to_cstr(char* a_ip_in_str, mxip_t a_ip)
{
  ostrstream strm(a_ip_in_str, IP_STR_LEN);
  strm << a_ip.val[0] << "." << a_ip.val[1] << ".";
  strm << a_ip.val[2] << "." << a_ip.val[3] << "\0";
  a_ip_in_str[IP_STR_LEN - 1] = 0;
}

inline bool cstr_to_mxip(mxip_t& a_ip, const char* a_str_ip)
{
  irs_u8* str_ip = (irs_u8*)a_str_ip;
  irs_i32 len = strlen((char*)str_ip);
  const irs_u8 find_begin_num = 0;
  const irs_u8 find_point = 1;
  const irs_u8 find_end_num = 2;
  const irs_u8 cnt_num_max = 4;
  irs_u8 mode = find_begin_num;
  irs_i32 begin_num = 0;
  irs_i32 end_num = 0;
  irs_u8 cnt_num = 0;
  for (irs_i32 str_ind = 0; str_ind < len; str_ind++) {
    bool exit_for = false;
    irs_u8 char_cur = str_ip[str_ind];
    switch (mode) {
      case find_begin_num: {
        if (char_cur != ' ') {
          if (isdigit(char_cur)) {
            begin_num = str_ind;
            mode = find_end_num;
          } else {
            a_ip = mxip_t::zero_ip();
            return false;
          }
        }
      } break;
      case find_end_num: {
        if (!isdigit(char_cur)) {
          if (cnt_num >= cnt_num_max) {
            exit_for = true;
            break;
          }
          end_num = str_ind;
          irs_i32 size_num = end_num - begin_num;
          if (size_num > 3) return false;
          irs_u8 num_in_str[4];
          memcpy(num_in_str, str_ip + begin_num, size_num);
          num_in_str[size_num] = 0;
          int num_int = atoi((char*)num_in_str);
          irs_i16 num = IRS_LOWORD(num_int);
          if (num > IP_NUM_MAX) return false;
          a_ip.val[cnt_num] = IRS_LOBYTE(num);
          cnt_num++;
          if (char_cur == '.') {
            mode = find_begin_num;
          } else {
            mode = find_point;
          }
        }
      } break;
      case find_point: {
        switch (char_cur) {
          case ' ': {
          } break;
          case '.': {
            mode = find_begin_num;
          } break;
          default: {
            a_ip = mxip_t::zero_ip();
          } return false;
        }
      } break;
    }
    if (exit_for) break;
  }
  if (mode == find_end_num) {
    irs_i32 size_num = len - begin_num;
    if (size_num > 3) return false;
    irs_u8 num_in_str[4];
    memcpy(num_in_str, str_ip + begin_num, size_num);
    num_in_str[size_num] = 0;
    int num_int = atoi((char*)num_in_str);
    irs_i16 num = IRS_LOWORD(num_int);
    if (num > IP_NUM_MAX) return false;
    a_ip.val[cnt_num] = IRS_LOBYTE(num);

    return true;
  } else {
    if (cnt_num >= cnt_num_max) {
      return true;
    } else {
      return false;
    }
  }
}

struct mxmac_t
{
  irs_u8 val[mac_length];

  static mxmac_t zero_mac()
  {
    mxmac_t mac = {{0, 0, 0, 0, 0, 0}};
    return mac;
  }
  static mxmac_t broadcast_mac()
  {
    mxmac_t mac = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    return mac;
  }
};

inline bool operator ==(mxmac_t a_mac1, mxmac_t a_mac2)
{
  return (a_mac1.val[0] == a_mac2.val[0]) &&
    (a_mac1.val[1] == a_mac2.val[1]) &&
    (a_mac1.val[2] == a_mac2.val[2]) &&
    (a_mac1.val[3] == a_mac2.val[3]) &&
    (a_mac1.val[4] == a_mac2.val[4]) &&
    (a_mac1.val[5] == a_mac2.val[5]);
}

inline bool operator !=(mxmac_t a_mac1, mxmac_t a_mac2)
{
  return (a_mac1.val[0] != a_mac2.val[0]) ||
    (a_mac1.val[1] != a_mac2.val[1]) ||
    (a_mac1.val[2] != a_mac2.val[2]) ||
    (a_mac1.val[3] != a_mac2.val[3]) ||
    (a_mac1.val[4] != a_mac2.val[4]) ||
    (a_mac1.val[5] != a_mac2.val[5]);
}

inline void mxmac_to_cstr(char* a_mac_in_str, mxmac_t a_mac)
{
  ostrstream strm(a_mac_in_str, MAC_STR_LEN);
  strm << a_mac.val[0] << "." << a_mac.val[1] << ".";
  strm << a_mac.val[2] << "." << a_mac.val[3] << ".";
  strm << a_mac.val[4] << "." << a_mac.val[5] << "\0";
  a_mac_in_str[MAC_STR_LEN - 1] = 0;
}

inline bool cstr_to_mxmac(mxmac_t& a_mac, const char* a_str_mac)
{
  irs_u8* str_mac = (irs_u8*)a_str_mac;
  irs_i32 len = strlen((char*)str_mac);
  const irs_u8 find_begin_num = 0;
  const irs_u8 find_point = 1;
  const irs_u8 find_end_num = 2;
  const irs_u8 cnt_num_max = 6;
  irs_u8 mode = find_begin_num;
  irs_i32 begin_num = 0;
  irs_i32 end_num = 0;
  irs_u8 cnt_num = 0;
  for (irs_i32 str_ind = 0; str_ind < len; str_ind++) {
    bool exit_for = false;
    irs_u8 char_cur = str_mac[str_ind];
    switch (mode) {
      case find_begin_num: {
        if (char_cur != ' ') {
          if (isdigit(char_cur)) {
            begin_num = str_ind;
            mode = find_end_num;
          } else {
            a_mac = mxmac_t::zero_mac();
            return false;
          }
        }
      } break;
      case find_end_num: {
        if (!isdigit(char_cur)) {
          if (cnt_num >= cnt_num_max) {
            exit_for = true;
            break;
          }
          end_num = str_ind;
          irs_i32 size_num = end_num - begin_num;
          if (size_num > 3) return false;
          irs_u8 num_in_str[4];
          memcpy(num_in_str, str_mac + begin_num, size_num);
          num_in_str[size_num] = 0;
          int num_int = atoi((char*)num_in_str);
          irs_i16 num = IRS_LOWORD(num_int);
          if (num > IP_NUM_MAX) return false;
          a_mac.val[cnt_num] = IRS_LOBYTE(num);
          cnt_num++;
          if (char_cur == '.') {
            mode = find_begin_num;
          } else {
            mode = find_point;
          }
        }
      } break;
      case find_point: {
        switch (char_cur) {
          case ' ': {
          } break;
          case '.': {
            mode = find_begin_num;
          } break;
          default: {
            a_mac = mxmac_t::zero_mac();
          } return false;
        }
      } break;
    }
    if (exit_for) break;
  }
  if (mode == find_end_num) {
    irs_i32 size_num = len - begin_num;
    if (size_num > 3) return false;
    irs_u8 num_in_str[4];
    memcpy(num_in_str, str_mac + begin_num, size_num);
    num_in_str[size_num] = 0;
    int num_int = atoi((char*)num_in_str);
    irs_i16 num = IRS_LOWORD(num_int);
    if (num > IP_NUM_MAX) return false;
    a_mac.val[cnt_num] = IRS_LOBYTE(num);

    return true;
  } else {
    if (cnt_num >= cnt_num_max) {
      return true;
    } else {
      return false;
    }
  }
}

#ifndef NAMESPACE_STD_NOT_SUPPORT
namespace std {
#endif //NAMESPACE_STD_NOT_SUPPORT
inline ostream& operator<<(ostream& a_stream, mxip_t a_ip)
{
  a_stream << int(a_ip.val[0]) << "." << int(a_ip.val[1]) << ".";
  a_stream << int(a_ip.val[2]) << "." << int(a_ip.val[3]);
  return a_stream;
}

inline ostream& operator<<(ostream& a_stream, mxmac_t a_mac)
{
  irs::ostream_format_save_t format_save(&a_stream);
  a_stream << hex << setfill('0');
  for (int mac_index = 0; mac_index < (mac_length - 1); mac_index++) {
    a_stream << setw(2) << int(a_mac.val[mac_index]) << "-";
  }
  a_stream << setw(2) << int(a_mac.val[5]);
  return a_stream;
}

#ifndef NAMESPACE_STD_NOT_SUPPORT
} //namespace std
#endif //NAMESPACE_STD_NOT_SUPPORT

namespace irs {

inline mxip_t make_mxip(int a_first_octet, int a_second_octet,
  int a_third_octet, int a_fourth_octet)
{
  mxip_t ip = {{
    static_cast<irs_u8>(a_first_octet),
    static_cast<irs_u8>(a_second_octet),
    static_cast<irs_u8>(a_third_octet),
    static_cast<irs_u8>(a_fourth_octet)
  }};
  return ip;
}

inline mxip_t make_mxip(const irs::char_t* a_ip)
{
  mxip_t ip = mxip_t::zero_ip();
  if (!cstr_to_mxip(ip, IRS_SIMPLE_FROM_TYPE_STR(a_ip))) {
    ip = mxip_t();
  }
  return ip;
}

inline mxmac_t make_mxmac(int a_first_octet, int a_second_octet,
  int a_third_octet, int a_fourth_octet, int a_fifth_octet, int a_sixth_octet)
{
  mxmac_t mac = {{
    static_cast<irs_u8>(a_first_octet),
    static_cast<irs_u8>(a_second_octet),
    static_cast<irs_u8>(a_third_octet),
    static_cast<irs_u8>(a_fourth_octet),
    static_cast<irs_u8>(a_fifth_octet),
    static_cast<irs_u8>(a_sixth_octet)
  }};
  return mac;
}

inline mxmac_t make_mxmac(const irs::char_t* a_mac)
{
  mxmac_t mac = mxmac_t::zero_mac();
  if (!cstr_to_mxmac(mac, IRS_SIMPLE_FROM_TYPE_STR(a_mac))) {
    mac = mxmac_t();
  }
  return mac;
}

} //namespace irs

#endif //IRSNETDEFSH
