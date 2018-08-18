//! \file
//! \ingroup network_in_out_group
//! \brief Определение типов mxip_t, mxmac_t и сопутствующих им функций
//!
//! Дата: 13.07.2010\n
//! Дата создания: 01.04.2010

#ifndef IRSNETDEFSH
#define IRSNETDEFSH

#include <irsdefs.h>

#include <irsstd.h>
#include <irsstrdefs.h>
#include <irsstrm.h>
#include <irscpp.h>
#include <ctype.h>

#include <irsfinal.h>

//! \addtogroup network_in_out_group
//! @{

//! \author Sergeev Sergey
//! \brief Определение типов mxip_t, mxmac_t и сопутствующих им функций

// Длина строки для ip-адреса
#define IP_STR_LEN 16
// Длина строки для mac-адреса
#define MAC_STR_LEN 18
// Максимальное число сегмента ip-адреса
#define IP_NUM_MAX 255
// Максимальное число сегмента mac-адреса
#define MAC_NUM_MAX 255

enum {
  ip_length = 4,
  mac_length = 6
};

#pragma pack(push, 1)
struct mxip_t
{
  union {
    irs_u8 val[ip_length];
    irs_u32 addr;
  };
  static mxip_t zero_ip()
  {
    mxip_t ip = {{{0, 0, 0, 0}}};
    return ip;
  }
  static mxip_t any_ip()
  {
    return zero_ip();
  }
  static mxip_t loopback_ip()
  {
    mxip_t ip = {{{127, 0, 0, 1}}};
    return ip;
  }
  static mxip_t broadcast_ip()
  {
    mxip_t ip = {{{0xFF, 0xFF, 0xFF, 0xFF}}};
    return ip;
  }
};
#pragma pack(pop)

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
  strm << static_cast<int>(a_ip.val[0]) << '.';
  strm << static_cast<int>(a_ip.val[1]) << '.';
  strm << static_cast<int>(a_ip.val[2]) << '.';
  strm << static_cast<int>(a_ip.val[3]) << '\0';
  a_ip_in_str[IP_STR_LEN - 1] = 0;
}

#ifndef __ICCAVR__
inline irs::string_t mxip_to_str(const mxip_t& a_ip)
{
  char cstr[IP_STR_LEN];
  mxip_to_cstr(cstr, a_ip);
  string str(cstr, strlen(cstr));
  return irs::str_conv<irs::string_t>(str);
}
#endif //__ICCAVR__

inline bool cstr_to_mxip(mxip_t& a_ip, const char* a_str_ip)
{
  const irs_u8* str_ip = reinterpret_cast<const irs_u8*>(a_str_ip);
  irs_i32 len = strlen(reinterpret_cast<const char*>(str_ip));
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
          int num_int = atoi(reinterpret_cast<char*>(num_in_str));
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
    int num_int = atoi(reinterpret_cast<char*>(num_in_str));
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

#if defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)
template<class S>
bool str_to_mxip(const S& a_str, mxip_t* a_ip)
{
  const std::string s = irs::str_conv<std::string>(a_str);
  return cstr_to_mxip(*a_ip, s.c_str());
}
#endif // defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)

#pragma pack(push, 1)
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
  bool operator<(const mxmac_t& a_mac) const
  {
    for (irs_size_t i = 0; i < mac_length; i++) {
      if (val[i] < a_mac.val[i]) {
        return true;
      } else if (val[i] > a_mac.val[i]) {
        return false;
      }
    }
    return false;
  }
};
#pragma pack(pop)

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
  strm << hex << setfill('0') <<
    setw(2) << static_cast<int>(a_mac.val[0]) << ':' <<
    setw(2) << static_cast<int>(a_mac.val[1]) << ':' <<
    setw(2) << static_cast<int>(a_mac.val[2]) << ':' <<
    setw(2) << static_cast<int>(a_mac.val[3]) << ':' <<
    setw(2) << static_cast<int>(a_mac.val[4]) << ':' <<
    setw(2) << static_cast<int>(a_mac.val[5]) << '\0';
  a_mac_in_str[MAC_STR_LEN - 1] = 0;
}

#ifndef __ICCAVR__
inline irs::string_t mxmac_to_str(const mxmac_t& a_mac)
{
  char cstr[MAC_STR_LEN];
  mxmac_to_cstr(cstr, a_mac);
  std::string str(cstr, MAC_STR_LEN - 1);
  return irs::str_conv<irs::string_t>(str);
}
#endif //__ICCAVR__

inline bool cstr_to_hex_num_helper(char* a_str, int* ap_num)
{
  istrstream istr(a_str);
  istr >> hex >> *ap_num;
  if (!istr) {
    return false;
  }
  return true;
}

inline bool cstr_to_mxmac(mxmac_t& a_mac, const char* a_str_mac)
{
  const irs_u8* str_mac = reinterpret_cast<const irs_u8*>(a_str_mac);
  irs_i32 len = strlen(reinterpret_cast<const char*>(str_mac));
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
          if (isxdigit(char_cur)) {
            begin_num = str_ind;
            mode = find_end_num;
          } else {
            a_mac = mxmac_t::zero_mac();
            return false;
          }
        }
      } break;
      case find_end_num: {
        if (!isxdigit(char_cur)) {
          if (cnt_num >= cnt_num_max) {
            exit_for = true;
            break;
          }
          end_num = str_ind;
          irs_i32 size_num = end_num - begin_num;
          if (size_num > 2) return false;
          irs_u8 num_in_str[3];
          memcpy(num_in_str, str_mac + begin_num, size_num);
          num_in_str[size_num] = 0;
          int num_int = 0;
          if (!cstr_to_hex_num_helper(
            reinterpret_cast<char*>(num_in_str), &num_int)) {
            exit_for = true;
            break;
          }
          /*istrstream istr(reinterpret_cast<char*>(num_in_str));
          int num_int = 0;
          istr >> hex >> num_int;
          if (!istr) {
            exit_for = true;
            break;
          }*/
          irs_i16 num = IRS_LOWORD(num_int);
          if (num > MAC_NUM_MAX) return false;
          a_mac.val[cnt_num] = IRS_LOBYTE(num);
          cnt_num++;
          if (char_cur == ':') {
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
          case ':': {
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
    if (size_num > 2) return false;
    irs_u8 num_in_str[3];
    memcpy(num_in_str, str_mac + begin_num, size_num);
    num_in_str[size_num] = 0;
    int num_int = 0;
    if (!cstr_to_hex_num_helper(reinterpret_cast<char*>(num_in_str), &num_int)) {
      return false;
    }
    /*istrstream istr(reinterpret_cast<char*>(num_in_str));
    int num_int = 0;
    istr >> hex >> num_int;
    if (!istr) {
      return false;
    }*/
    irs_i16 num = IRS_LOWORD(num_int);
    if (num > MAC_NUM_MAX) return false;
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

#if defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)
template<class S>
bool str_to_mxmac(const S& a_str, mxmac_t* a_mac)
{
  const std::string s = irs::str_conv<std::string>(a_str);
  return cstr_to_mxmac(*a_mac, s.c_str());
}
#endif // defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)

//namespace irs_shift_operator_ip_mac {

inline ostream& operator<<(ostream& a_stream, const mxip_t& a_ip)
{
  a_stream << int(a_ip.val[0]) << "." << int(a_ip.val[1]) << ".";
  a_stream << int(a_ip.val[2]) << "." << int(a_ip.val[3]);
  return a_stream;
}

inline ostream& operator<<(ostream& a_stream, const mxmac_t& a_mac)
{
  irs::ostream_format_save_t format_save(&a_stream);
  a_stream << hex << setfill('0');
  for (int mac_index = 0; mac_index < (mac_length - 1); mac_index++) {
    a_stream << setw(2) << int(a_mac.val[mac_index]) << "-";
  }
  a_stream << setw(2) << int(a_mac.val[5]);
  return a_stream;
}

//} //namespace irs_shift_operator_ip_mac

//using namespace irs_shift_operator_ip_mac;

//! @}

namespace irs {

//! \addtogroup network_in_out_group
//! @{

inline mxip_t make_mxip(int a_first_octet, int a_second_octet,
  int a_third_octet, int a_fourth_octet)
{
  mxip_t ip = {{{
    static_cast<irs_u8>(a_first_octet),
    static_cast<irs_u8>(a_second_octet),
    static_cast<irs_u8>(a_third_octet),
    static_cast<irs_u8>(a_fourth_octet)
  }}};
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

inline mxmac_t
generate_mac(device_code_t a_device_code, irs_u16 a_device_id)
{
  IRS_STATIC_ASSERT(device_code_last <= 0xFF);
  mxmac_t mac = mxmac_t::broadcast_mac();
  mac.val[0] = 0x2;
  mac.val[1] = 0x80;
  mac.val[2] = 0xE1;
  mac.val[3] = static_cast<irs_u8>(a_device_code);
  mac.val[4] = IRS_HIBYTE(a_device_id);
  mac.val[5] = IRS_LOBYTE(a_device_id);
  return mac;
}

//! @}

} //namespace irs

#endif //IRSNETDEFSH
