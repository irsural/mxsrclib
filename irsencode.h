// @brief функции перевода строк в различнык кодировки
//
// Дата: 27.08.2021
// Дата создания: 12.04.2021

#ifndef ENCODE_H
#define ENCODE_H

#include <irsdefs.h>
#include <irscpp.h>
#include <irsnetdefs.h>
#include <irserror.h>

#if IRS_USE_UTF8_CPP
#include <utf8.h>
#endif // IRS_USE_UTF8_CPP

namespace irs {

/**
 * @brief функция перевода строки из кодировки cp1251 в utf8.
 *
 * @param a_in: ссылка на вектор символов в кодировке cp1251.
 * @param a_size_in: количество считываемых символов из входного вектора.
 * @param a_out: ссылка на вектор для перекодированных символов в utf8.
 *
 * @return size_t: количество вставленных символов в a_out
 */
inline size_t cp1251_to_utf8(const vector<char>& a_in, const size_t a_size_in,
  vector<char>& a_out
)
{
  if (a_size_in > a_in.size()) {
#ifndef IRS_NOEXCEPTION
    throw runtime_error("a_size_in cannot be bigger than a_in.size()");
#endif // IRS_NOEXCEPTION
  }

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

  size_t j = 0;

  for (size_t i = 0; i < a_size_in && i < a_in.size(); i++) {
    if (a_in.at(i) & 0x80) {
      const char* p = &table[3 * (0x7f & a_in.at(i))];

      if (*p == ' ') { continue; }

      a_out.at(j++) = *p++;
      a_out.at(j++) = *p++;

      if (*p == ' ') { continue; }

      a_out.at(j++) = *p++;
    } else {
      a_out.at(j++) = a_in.at(i);
    }
  }

  return j;
}

#if IRS_USE_UTF8_CPP
/**
 * @brief функция перевода строки из кодировки UTF-16 (UTF-32) в UTF-8. Функция
 * опеределят размерность wchat_t и в зависимости от этого определяет начальную
 * кодировку. В случае, если размерность не равна ни 2, ни 4 - выдает ошибку.
 *
 * @param a_str - ссылка на строку в кодировке UTF-16 (UTF-32).
 * @param a_size_in - количество кодируемых символов.
 * @param a_out - ссылка на выходной вектор.
 */
// const wstring& a_str
inline void wstring_to_utf8(const vector<wchar_t>& a_in, const size_t a_size_in,
  vector<char>& a_out)
{
   if (sizeof(wchar_t) == 2) {
    utf8::unchecked::utf16to8(a_in.begin(), a_in.begin() + a_size_in,
      back_inserter(a_out));
  } else if (sizeof(wchar_t) == 4) {
    utf8::unchecked::utf32to8(a_in.begin(), a_in.begin() + a_size_in,
      back_inserter(a_out));
  } else {
    IRS_STATIC_ASSERT((sizeof(wchar_t) == 2) || (sizeof(wchar_t) == 4));
  }
}
#endif // IRS_USE_UTF8_CPP

} // namespace irs

#endif // ENCODE_H
