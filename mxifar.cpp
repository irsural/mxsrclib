// Функции для mxifa (mxifa routines)
// Дата: 5.05.2008

//---------------------------------------------------------------------------
//#include <vcl.h>
//#pragma hdrstop

#include <mxifar.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//---------------------------------------------------------------------------
// Нулевой IP
const mxip_t zero_ip = {{0, 0, 0, 0}};
//---------------------------------------------------------------------------
// Оператор == для mxip_t
irs_bool operator ==(mxip_t ip1, mxip_t ip2)
{
  return (ip1.val[0] == ip2.val[0])&&(ip1.val[1] == ip2.val[1])&&
    (ip1.val[2] == ip2.val[2])&&(ip1.val[3] == ip2.val[3]);
}
//---------------------------------------------------------------------------
// Оператор != для mxip_t
irs_bool operator !=(mxip_t ip1, mxip_t ip2)
{
  return (ip1.val[0] != ip2.val[0])||(ip1.val[1] != ip2.val[1])||
    (ip1.val[2] != ip2.val[2])||(ip1.val[3] != ip2.val[3]);
}
//---------------------------------------------------------------------------
// Преобразование IP в строку
void mxip_to_cstr(char *ip_in_str, mxip_t ip) {
  sprintf(ip_in_str, "%hd.%hd.%hd.%hd", irs_u16(ip.val[0]),
    irs_u16(ip.val[1]), irs_u16(ip.val[2]), irs_u16(ip.val[3]));
}
// Определение является ли символ цифрой
irs_bool is_digit(irs_u8 ch)
{
  return (ch >= '0') && (ch <= '9');
}
// Преобразование, если возможно строки в IP
irs_bool cstr_to_mxip(mxip_t &ip, char *a_str_ip)
{
  irs_u8 *str_ip = (irs_u8 *)a_str_ip;
  irs_i32 len = strlen((char *)str_ip);
  const mxip_t zero_ip = {{0, 0, 0, 0}};
  const irs_u8 find_begin_num = 0;
  const irs_u8 find_point = 1;
  const irs_u8 find_end_num = 2;
  const irs_u8 cnt_num_max = 4;
  irs_u8 mode = find_begin_num;
  irs_i32 begin_num = 0;
  irs_i32 end_num = 0;
  irs_u8 cnt_num = 0;
  for (irs_i32 str_ind = 0; str_ind < len; str_ind++) {
    irs_bool exit_for = irs_false;
    irs_u8 char_cur = str_ip[str_ind];
    switch (mode) {
      case find_begin_num: {
        if (char_cur != ' ') {
          if (is_digit(char_cur)) {
            begin_num = str_ind;
            mode = find_end_num;
          } else {
            ip = zero_ip;
            return irs_false;
          }
        }
      } break;
      case find_end_num: {
        if (!is_digit(char_cur)) {
          if (cnt_num >= cnt_num_max) {
          exit_for = irs_true;
            break;
          }
          end_num = str_ind;
          irs_i32 size_num = end_num - begin_num;
          if (size_num > 3) return irs_false;
          irs_u8 num_in_str[4];
          memcpy(num_in_str, str_ip + begin_num, size_num);
          num_in_str[size_num] = 0;
          int num_int = atoi((char *)num_in_str);
          irs_i16 num = IRS_LOWORD(num_int);
          if (num > IPNUM_MAX) return irs_false;
          ip.val[cnt_num] = IRS_LOBYTE(num);
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
            ip = zero_ip;
          } return irs_false;
        }
      } break;
    }
    if (exit_for) break;
  }
  if (mode == find_end_num) {
    irs_i32 size_num = len - begin_num;
    if (size_num > 3) return irs_false;
    irs_u8 num_in_str[4];
    memcpy(num_in_str, str_ip + begin_num, size_num);
    num_in_str[size_num] = 0;
    int num_int = atoi((char *)num_in_str);
    irs_i16 num = IRS_LOWORD(num_int);
    if (num > IPNUM_MAX) return irs_false;
    ip.val[cnt_num] = IRS_LOBYTE(num);

    return irs_true;
  } else {
    if (cnt_num >= cnt_num_max) {
      return irs_true;
    } else {
      return irs_false;
    }
  }
}
//#pragma package(smart_init)
