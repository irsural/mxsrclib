// @brief функции перевода строк в различнык кодировки
//
// Дата: 31.08.2021
// Дата создания: 12.04.2021

#ifndef ENCODE_H
#define ENCODE_H

#include <irsdefs.h>
#include <irscpp.h>
#include <irsnetdefs.h>
#include <irserror.h>

#define IRS_LEAD_SURROGATE_MIN 0xd800u
#define IRS_LEAD_SURROGATE_MAX 0xdbffu
#define IRS_TRAIL_SURROGATE_MIN 0xdc00u
#define TRAIL_SURROGATE_MAX 0xdfffu
#define IRS_LEAD_OFFSET (IRS_LEAD_SURROGATE_MIN - (0x10000 >> 10))
#define IRS_SURROGATE_OFFSET 0x10000u - (IRS_LEAD_SURROGATE_MIN << 10) - \
        IRS_TRAIL_SURROGATE_MIN

namespace irs
{

/**
 * @brief функция перевода строки из кодировки cp1251 в utf8.
 *
 * @param a_buffer: ссылка на буфер.
 * @param a_msg_start_index: индекс, с которого начинается сообщение. Считывание
 * осуществляется до конца буфера (to a_buffer.size()).
 *
 * @return size_t: размерность перекодированных данных.
 */
inline size_t cp1251_to_utf8(vector<char> &a_buffer,
  const size_t a_msg_start_index)
{
  static const char table[128 * 3 + 1] = {
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
      "\321\210 \321\211 \321\212 \321\213 \321\214 \321\215 \321\216 \321\217 "};

  size_t j = 0;

  for (size_t i = a_msg_start_index; i < a_buffer.size(); i++) {
    if (a_buffer.at(i) & 0x80) {
      const char *p = &table[3 * (0x7f & a_buffer[i])];

      if (*p == ' ') { continue; }

      a_buffer.at(j++) = *p++;
      a_buffer.at(j++) = *p++;

      if (*p == ' ') { continue; }

      a_buffer.at(j++) = *p++;
    } else {
      a_buffer.at(j++) = a_buffer[i];
    }
  }

  return j;
}

template<typename octet_iterator>
inline octet_iterator wchar_to_utf8(irs_u32 cp, octet_iterator& iter)
{
  if (cp < 0x80) { // one octet
    *(iter++) = static_cast<irs_u8>(cp);
  } else if (cp < 0x800) { // two octets
    *(iter++) = static_cast<irs_u8>((cp >> 6) | 0xc0);
    *(iter++) = static_cast<irs_u8>((cp & 0x3f) | 0x80);
  } else if (cp < 0x10000) { // three octets
    *(iter++) = static_cast<irs_u8>((cp >> 12) | 0xe0);
    *(iter++) = static_cast<irs_u8>(((cp >> 6) & 0x3f) | 0x80);
    *(iter++) = static_cast<irs_u8>((cp & 0x3f) | 0x80);
  } else { // four octets
    *(iter++) = static_cast<irs_u8>((cp >> 18) | 0xf0);
    *(iter++) = static_cast<irs_u8>(((cp >> 12) & 0x3f) | 0x80);
    *(iter++) = static_cast<irs_u8>(((cp >> 6) & 0x3f) | 0x80);
    *(iter++) = static_cast<irs_u8>((cp & 0x3f) | 0x80);
  }

  return iter;
}

template<typename u16_type>
inline irs_u16 mask16(u16_type oc)
{ return static_cast<irs_u16>(0xffff & oc); }

template<typename u16_type>
inline bool is_lead_surrogate(u16_type cp)
{ return (cp >= IRS_LEAD_SURROGATE_MIN && cp <= IRS_LEAD_SURROGATE_MAX); }

template<typename u16bit_iterator, typename octet_iterator>
inline octet_iterator utf16to8(u16bit_iterator start, u16bit_iterator end,
  octet_iterator& result
)
{
  while (start != end) {
    uint32_t cp = mask16(*start++);
    // Take care of surrogate pairs first
    if (is_lead_surrogate(cp)) {
      uint32_t trail_surrogate = mask16(*start++);
      cp = (cp << 10) + trail_surrogate + IRS_SURROGATE_OFFSET;
    }
    result = wchar_to_utf8(cp, result);
  }
  return result;
}

template<typename octet_iterator, typename u32bit_iterator>
inline octet_iterator utf32to8(u32bit_iterator start, u32bit_iterator end,
  octet_iterator& result
)
{
  while (start != end) {
    result = wchar_to_utf8(*(start++), result);
  }

  return result;
}

/**
 * @brief функция перевода строки из кодировки UTF-16 (UTF-32) в UTF-8. Функция
 * опеределят размерность wchat_t и в зависимости от этого определяет начальную
 * кодировку. В случае, если размерность не равна ни 2, ни 4 - выдает ошибку.
 * 
 * @param a_buffer: буффер в кодироке UTF-16(32).
 * @param a_msg_start_index: индекс начала сообщения в буфере.
 * 
 * @return size_t: размерность перекодированного сообщения.
 */
inline size_t wsymbols_to_utf8(vector<char>& a_buffer,
  const size_t a_msg_start_index
)
{
  vector<char>::iterator iter;

  if (sizeof(wchar_t) == 2) { // utf16
    // utf16
    iter = a_buffer.begin();

    for (size_t i = a_msg_start_index; i < a_buffer.size(); i++) {
      irs_u16 character = 0;

      if (a_buffer.at(i) != '\r') {
        irs_u8 octet_1 = static_cast<irs_u8>(a_buffer.at(i++));
        irs_u8 octet_2 = static_cast<irs_u8>(a_buffer.at(i));

        character = (character | octet_2) << 8;
        character |= octet_1;
      } else {
        *(iter++) = static_cast<irs_u8>(a_buffer.at(i));
        continue;
      }

      irs_u32 u32_character = mask16(character);
      if (is_lead_surrogate(u32_character)) {
        irs_u16 surrogate = 0;

        i++;
        irs_u8 octet_1 = static_cast<irs_u8>(a_buffer.at(i++));
        irs_u8 octet_2 = static_cast<irs_u8>(a_buffer.at(i));

        surrogate = (surrogate | octet_2) << 8;
        surrogate |= octet_1;

        irs_u32 trail_surrogate = mask16(surrogate);
        u32_character = (u32_character << 10) + trail_surrogate + 
          IRS_SURROGATE_OFFSET;
      }

      iter = wchar_to_utf8(u32_character, iter);
    }
  } else if (sizeof(wchar_t) == 4) { // utf32
    iter = a_buffer.begin();

    for (size_t i = a_msg_start_index; i < a_buffer.size(); i++) {
      irs_u32 character = 0;

      if (a_buffer.at(i) != '\r') {
        irs_u8 octet_1 = static_cast<irs_u8>(a_buffer.at(i++));
        irs_u8 octet_2 = static_cast<irs_u8>(a_buffer.at(i++));
        irs_u8 octet_3 = static_cast<irs_u8>(a_buffer.at(i++));
        irs_u8 octet_4 = static_cast<irs_u8>(a_buffer.at(i));

        character = (character | octet_4) << 8;
        character = (character | octet_3) << 8;
        character = (character | octet_2) << 8;
        character |= octet_1;
        
        iter = wchar_to_utf8(character, iter);
      } else {
        *(iter++) = static_cast<irs_u8>(a_buffer.at(i));
      }
    }
  } else {
    IRS_STATIC_ASSERT((sizeof(wchar_t) == 2) || (sizeof(wchar_t) == 4));
  }

  return distance(a_buffer.begin(), iter);
}

} // namespace irs

#endif // ENCODE_H
