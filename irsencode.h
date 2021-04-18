// @brief ������� �������� ����� � ��������� ���������.
//
// ����: 18.04.2021
// ���� ��������: 12.04.2021

#pragma once

#ifndef ENCODE_H
#define ENCODE_H

#include <irsdefs.h>
#include <irscpp.h>
#include <irsnetdefs.h>
#include <irserror.h>

#if IRS_USE_UTF8_CPP
#include <utf8.h>
#endif // IRS_USE_UTF8_CPP

namespace irs
{
  /* @brief ������� �������� ������ �� ��������� cp1251 � utf8.
   * 
   * @param ap_out - ��������� �� ������, � ������� ����� ������������
   * ���������������� ������.
   * @param ap_in - ��������� �� ������ � ��������� cp1251.
   */
  inline void cp1251_to_utf8(char* ap_out, const char* ap_in)
  {
    IRS_ASSERT(ap_out == NULL || ap_in == NULL);
    
    static const char table[128*3 + 1] = {
      "\320\202 \320\203 \342\200\232\321\223 \342\200\236\342\200\246\342\200\240\342\200\241"
      "\342\202\254\342\200\260\320\211 \342\200\271\320\212 \320\214 \320\213 \320\217 "
      "\321\222 \342\200\230\342\200\231\342\200\234\342\200\235\342\200\242\342\200\223\342\200\224"
      "   \342\204\242\321\231 \342\200\272\321\232 \321\234 \321\233 \321\237 "
      "\302\240 \320\216 \321\236 \320\210 \302\244 \322\220 \302\246 \302\247 "
      "\320\201 \302\251 \320\204 \302\253 \302\254 \302\255 \302\256 \320\207 "
      "\302\260 \302\261 \320\206 \321\226 \322\221 \302\265 \302\266 \302\267 "
      "\321\221 \342\204\226\321\224 \302\273 \321\230 \320\205 \321\225 \321\227 "
      "\320\220 \320\221 \320\222 \320\223 \320\224 \320\225 \320\226 \320\227 "
      "\320\230 \320\231 \320\232 \320\233 \320\234 \320\235 \320\236 \320\237 "
      "\320\240 \320\241 \320\242 \320\243 \320\244 \320\245 \320\246 \320\247 "
      "\320\250 \320\251 \320\252 \320\253 \320\254 \320\255 \320\256 \320\257 "
      "\320\260 \320\261 \320\262 \320\263 \320\264 \320\265 \320\266 \320\267 "
      "\320\270 \320\271 \320\272 \320\273 \320\274 \320\275 \320\276 \320\277 "
      "\321\200 \321\201 \321\202 \321\203 \321\204 \321\205 \321\206 \321\207 "
      "\321\210 \321\211 \321\212 \321\213 \321\214 \321\215 \321\216 \321\217 "
    };

    while (*ap_in) {
      if (*ap_in & 0x80) {
        const char *p = &table[3 * (0x7f & *ap_in++)];
        if (*p == ' ') { continue; }
        *ap_out++ = *p++;
        *ap_out++ = *p++;
        if (*p == ' ') { continue; }
        *ap_out++ = *p++;
      } else {
        *ap_out++ = *ap_in++;
      }
    }

    *ap_out = 0;
  }

  // TODO: ����������� ������� � ����������� const char*, char*.
  /* @brief ������� �������� ������ �� ��������� UTF-16 (UTF-32) � UTF-8. �������
   * ���������� ����������� wchar_t � ������������� �� ����� ���������� ���������
   * ���������. � ������, ���� ����������� �� ����� 2 ��� 4, ������ ������.
   * 
   * @param a_str - ������ � ��������� UTF-16 (UTF-32).
   *
   * @return string - ���������������� ������ � UTF-8.
   */
  inline std::string wstring_to_utf8(const std::wstring& a_str)
  {
     string str_utf8;
     
     if (sizeof(wchar_t) == 2) {
      utf8::unchecked::utf16to8(a_str.begin(), a_str.end(), back_inserter(str_utf8));
    } else if (sizeof(wchar_t) == 4) {
      utf8::unchecked::utf32to8(a_str.begin(), a_str.end(), back_inserter(str_utf8));
    } else {
      IRS_STATIC_ASSERT((sizeof(wchar_t) == 2) || (sizeof(wchar_t) == 4));
    }
    
    return str_utf8;
  }

} // namespace irs

#endif // ENCODE_H