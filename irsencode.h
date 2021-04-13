#pragma once

#ifndef ENCODE_H
#define ENCODE_H

#include <irsdefs.h>
#include <irscpp.h>
#include <irsnetdefs.h>

#include <stdint.h>

namespace irs
{
  typedef uint8_t utf8_t;
  typedef uint16_t utf16_t;

  // The type of a single Unicode codepoint
  typedef uint32_t codepoint_t; 

  // The last codepoint of the Basic Multilingual Plane, which is the part of Unicode that
  // UTF-16 can encode without surrogates
  #define BMP_END 0xFFFF

  // The highest valid Unicode codepoint
  #define UNICODE_MAX 0x10FFFF

  // The codepoint that is used to replace invalid encodings
  #define INVALID_CODEPOINT 0xFFFD

  // If a character, masked with GENERIC_SURROGATE_MASK, matches this value, it is a surrogate.
  #define GENERIC_SURROGATE_VALUE 0xD800
  // The mask to apply to a character before testing it against GENERIC_SURROGATE_VALUE
  #define GENERIC_SURROGATE_MASK 0xF800

  // If a character, masked with SURROGATE_MASK, matches this value, it is a high surrogate.
  #define HIGH_SURROGATE_VALUE 0xD800
  // If a character, masked with SURROGATE_MASK, matches this value, it is a low surrogate.
  #define LOW_SURROGATE_VALUE 0xDC00
  // The mask to apply to a character before testing it against HIGH_SURROGATE_VALUE or LOW_SURROGATE_VALUE
  #define SURROGATE_MASK 0xFC00

  // The value that is subtracted from a codepoint before encoding it in a surrogate pair
  #define SURROGATE_CODEPOINT_OFFSET 0x10000
  // A mask that can be applied to a surrogate to extract the codepoint value contained in it
  #define SURROGATE_CODEPOINT_MASK 0x03FF
  // The number of bits of SURROGATE_CODEPOINT_MASK
  #define SURROGATE_CODEPOINT_BITS 10


  // The highest codepoint that can be encoded with 1 byte in UTF-8
  #define UTF8_1_MAX 0x7F
  // The highest codepoint that can be encoded with 2 bytes in UTF-8
  #define UTF8_2_MAX 0x7FF
  // The highest codepoint that can be encoded with 3 bytes in UTF-8
  #define UTF8_3_MAX 0xFFFF
  // The highest codepoint that can be encoded with 4 bytes in UTF-8
  #define UTF8_4_MAX 0x10FFFF

  // If a character, masked with UTF8_CONTINUATION_MASK, matches this value, it is a UTF-8 continuation byte
  #define UTF8_CONTINUATION_VALUE 0x80
  // The mask to a apply to a character before testing it against UTF8_CONTINUATION_VALUE
  #define UTF8_CONTINUATION_MASK 0xC0
  // The number of bits of a codepoint that are contained in a UTF-8 continuation byte
  #define UTF8_CONTINUATION_CODEPOINT_BITS 6

  // Represents a UTF-8 bit pattern that can be set or verified
  typedef struct
  {
      // The mask that should be applied to the character before testing it
      utf8_t mask;
      // The value that the character should be tested against after applying the mask
      utf8_t value;
  } utf8_pattern;

  // The patterns for leading bytes of a UTF-8 codepoint encoding
  // Each pattern represents the leading byte for a character encoded with N UTF-8 bytes,
  // where N is the index + 1
  static const utf8_pattern utf8_leading_bytes[] =
  {
      { 0x80, 0x00 }, // 0xxxxxxx
      { 0xE0, 0xC0 }, // 110xxxxx
      { 0xF0, 0xE0 }, // 1110xxxx
      { 0xF8, 0xF0 }  // 11110xxx
  };

  // The number of elements in utf8_leading_bytes
  #define UTF8_LEADING_BYTES_LEN 4

  // Gets a codepoint from a UTF-16 string
  // utf16: The UTF-16 string
  // len: The length of the UTF-16 string, in UTF-16 characters
  // index:
  // A pointer to the current index on the string.
  // When the function returns, this will be left at the index of the last character
  // that composes the returned codepoint.
  // For surrogate pairs, this means the index will be left at the low surrogate.
  static codepoint_t decode_utf16(utf16_t const* utf16, size_t len, 
                                  size_t* index)
  {
      utf16_t high = utf16[*index];

      // BMP character
      if ((high & GENERIC_SURROGATE_MASK) != GENERIC_SURROGATE_VALUE)
          return high; 

      // Unmatched low surrogate, invalid
      if ((high & SURROGATE_MASK) != HIGH_SURROGATE_VALUE)
          return INVALID_CODEPOINT;

      // String ended with an unmatched high surrogate, invalid
      if (*index == len - 1)
          return INVALID_CODEPOINT;
      
      utf16_t low = utf16[*index + 1];

      // Unmatched high surrogate, invalid
      if ((low & SURROGATE_MASK) != LOW_SURROGATE_VALUE)
          return INVALID_CODEPOINT;

      // Two correctly matched surrogates, increase index to indicate we've consumed
      // two characters
      (*index)++;

      // The high bits of the codepoint are the value bits of the high surrogate
      // The low bits of the codepoint are the value bits of the low surrogate
      codepoint_t result = high & SURROGATE_CODEPOINT_MASK;
      result <<= SURROGATE_CODEPOINT_BITS;
      result |= low & SURROGATE_CODEPOINT_MASK;
      result += SURROGATE_CODEPOINT_OFFSET;
      
      // And if all else fails, it's valid
      return result;
  }

  // Calculates the number of UTF-8 characters it would take to encode a codepoint
  // The codepoint won't be checked for validity, that should be done beforehand.
  static int calculate_utf8_len(codepoint_t codepoint)
  {
      // An array with the max values would be more elegant, but a bit too heavy
      // for this common function

      if (codepoint <= UTF8_1_MAX)
          return 1;

      if (codepoint <= UTF8_2_MAX)
          return 2;

      if (codepoint <= UTF8_3_MAX)
          return 3;

      return 4;
  }

  // Encodes a codepoint in a UTF-8 string.
  // The codepoint won't be checked for validity, that should be done beforehand.
  //
  // codepoint: The codepoint to be encoded.
  // utf8: The UTF-8 string
  // len: The length of the UTF-8 string, in UTF-8 characters
  // index: The first empty index on the string.
  //
  // return: The number of characters written to the string.
  static size_t encode_utf8(codepoint_t codepoint, utf8_t* utf8, 
                            size_t len, size_t index)
  {
      int size = calculate_utf8_len(codepoint);

      // Not enough space left on the string
      if (index + size > len)
          return 0;

      // Write the continuation bytes in reverse order first
      for (int cont_index = size - 1; cont_index > 0; cont_index--)
      {
          utf8_t cont = codepoint & ~UTF8_CONTINUATION_MASK;
          cont |= UTF8_CONTINUATION_VALUE;

          utf8[index + cont_index] = cont;
          codepoint >>= UTF8_CONTINUATION_CODEPOINT_BITS;
      }

      // Write the leading byte
      utf8_pattern pattern = utf8_leading_bytes[size - 1];

      utf8_t lead = codepoint & ~(pattern.mask);
      lead |= pattern.value;

      utf8[index] = lead;

      return size;
  }

  size_t utf16_to_utf8(utf16_t const* utf16, size_t utf16_len, 
                       utf8_t* utf8, size_t utf8_len)
  {
      // The next codepoint that will be written in the UTF-8 string
      // or the size of the required buffer if utf8 is NULL
      size_t utf8_index = 0;

      for (size_t utf16_index = 0; utf16_index < utf16_len; utf16_index++)
      {
          codepoint_t codepoint = decode_utf16(utf16, utf16_len, &utf16_index);

          if (utf8 == NULL)
              utf8_index += calculate_utf8_len(codepoint);
          else
              utf8_index += encode_utf8(codepoint, utf8, utf8_len, utf8_index);
      }

      return utf8_index;
  }

  // Gets a codepoint from a UTF-8 string
  // utf8: The UTF-8 string
  // len: The length of the UTF-8 string, in UTF-8 characters
  // index:
  // A pointer to the current index on the string.
  // When the function returns, this will be left at the index of the last character
  // that composes the returned codepoint.
  // For example, for a 3-byte codepoint, the index will be left at the third character.
  static codepoint_t decode_utf8(utf8_t const* utf8, size_t len, size_t* index)
  {
      utf8_t leading = utf8[*index];

      // The number of bytes that are used to encode the codepoint
      int encoding_len = 0;
      // The pattern of the leading byte
      utf8_pattern leading_pattern;
      // If the leading byte matches the current leading pattern
      bool matches = false;
      
      do
      {
          encoding_len++;
          leading_pattern = utf8_leading_bytes[encoding_len - 1];

          matches = (leading & leading_pattern.mask) == leading_pattern.value;

      } while (!matches && encoding_len < UTF8_LEADING_BYTES_LEN);

      // Leading byte doesn't match any known pattern, consider it invalid
      if (!matches)
          return INVALID_CODEPOINT;

      codepoint_t codepoint = leading & ~leading_pattern.mask;

      for (int i = 0; i < encoding_len - 1; i++)
      {
          // String ended before all continuation bytes were found
          // Invalid encoding
          if (*index + 1 >= len)
              return INVALID_CODEPOINT;

          utf8_t continuation = utf8[*index + 1];

          // Number of continuation bytes not the same as advertised on the leading byte
          // Invalid encoding
          if ((continuation & UTF8_CONTINUATION_MASK) != UTF8_CONTINUATION_VALUE)
              return INVALID_CODEPOINT;

          codepoint <<= UTF8_CONTINUATION_CODEPOINT_BITS;
          codepoint |= continuation & ~UTF8_CONTINUATION_MASK;

          (*index)++;
      }

      int proper_len = calculate_utf8_len(codepoint);

      // Overlong encoding: too many bytes were used to encode a short codepoint
      // Invalid encoding
      if (proper_len != encoding_len)
          return INVALID_CODEPOINT;

      // Surrogates are invalid Unicode codepoints, and should only be used in UTF-16
      // Invalid encoding
      if (codepoint < BMP_END && (codepoint & GENERIC_SURROGATE_MASK) == GENERIC_SURROGATE_VALUE)
          return INVALID_CODEPOINT;

      // UTF-8 can encode codepoints larger than the Unicode standard allows
      // Invalid encoding
      if (codepoint > UNICODE_MAX)
          return INVALID_CODEPOINT;

      return codepoint;
  }

  // Calculates the number of UTF-16 characters it would take to encode a codepoint
  // The codepoint won't be checked for validity, that should be done beforehand.
  static int calculate_utf16_len(codepoint_t codepoint)
  {
      if (codepoint <= BMP_END)
          return 1;

      return 2;
  }

  // Encodes a codepoint in a UTF-16 string.
  // The codepoint won't be checked for validity, that should be done beforehand.
  //
  // codepoint: The codepoint to be encoded.
  // utf16: The UTF-16 string
  // len: The length of the UTF-16 string, in UTF-16 characters
  // index: The first empty index on the string.
  //
  // return: The number of characters written to the string.
  static size_t encode_utf16(codepoint_t codepoint, utf16_t* utf16, 
                             size_t len, size_t index)
  {
      // Not enough space on the string
      if (index >= len)
          return 0;

      if (codepoint <= BMP_END)
      {
          utf16[index] = codepoint;
          return 1;
      }

      // Not enough space on the string for two surrogates
      if (index + 1 >= len)
          return 0;

      codepoint -= SURROGATE_CODEPOINT_OFFSET;

      utf16_t low = LOW_SURROGATE_VALUE;
      low |= codepoint & SURROGATE_CODEPOINT_MASK;

      codepoint >>= SURROGATE_CODEPOINT_BITS;

      utf16_t high = HIGH_SURROGATE_VALUE;
      high |= codepoint & SURROGATE_CODEPOINT_MASK;

      utf16[index] = high;
      utf16[index + 1] = low;

      return 2;
  }


  size_t utf8_to_utf16(utf8_t const* utf8, size_t utf8_len, 
                       utf16_t* utf16, size_t utf16_len)
  {
      // The next codepoint that will be written in the UTF-16 string
      // or the size of the required buffer if utf16 is NULL
      size_t utf16_index = 0;

      for (size_t utf8_index = 0; utf8_index < utf8_len; utf8_index++)
      {
          codepoint_t codepoint = decode_utf8(utf8, utf8_len, &utf8_index);

          if (utf16 == NULL)
              utf16_index += calculate_utf16_len(codepoint);
          else
              utf16_index += encode_utf16(codepoint, utf16, utf16_len, utf16_index);
      }

      return utf16_index;
  }

  inline void cp1251_to_utf8(char *out, const char *in)
  {
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

    while (*in) {
        if (*in & 0x80) {
            const char *p = &table[3 * (0x7f & *in++)];
            if (*p == ' ') { continue; }
            *out++ = *p++;
            *out++ = *p++;
            if (*p == ' ') { continue; }
            *out++ = *p++;
        }
        else {
            *out++ = *in++;
        }
    }

    *out = 0;
  }

} // namespace irs

#endif // ENCODE_H