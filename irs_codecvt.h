//! \file
//! \ingroup string_processing_group
//! \brief Реализации фасетов
//!
//! Дата создания: 10.07.2014

#ifndef IRS_CODECVT_H
#define IRS_CODECVT_H

#include <irsdefs.h>

#include <locale>
#include <map>

#include <irsfinal.h>

namespace irs {

template <class internT, class externT, class stateT>
class codecvt_cp1251_t: public std::codecvt<internT, externT, stateT>
{
public:
  typedef std::codecvt_base::result result;
  const result ok;
  const result partial;
  const result error;
  const result noconv;
explicit codecvt_cp1251_t(size_t r = 0):
  std::codecvt<wchar_t, char, mbstate_t>(r),
  ok(std::codecvt_base::ok),
  partial(std::codecvt_base::partial),
  error(std::codecvt_base::error),
  noconv(std::codecvt_base::noconv)
  {
    // Эта реализация только для случая,
    //  когда внешний и внутренний типы одинаковы
    IRS_STATIC_ASSERT(
      (is_equal_types<intern_type, extern_type>::type == yes_type));
  }
  virtual result do_in(state_type& a_state,
    const extern_type* ap_from,
    const extern_type* ap_from_end,
    const extern_type*& ap_from_next,
    intern_type* ap_to,
    intern_type* ap_to_end,
    intern_type*& ap_to_next) const
  {
    return noconv;
  }

  virtual result do_out(state_type& a_state,
   const intern_type* ap_from,
   const intern_type* ap_from_end,
   const intern_type*& ap_from_next,
   extern_type* ap_to,
   extern_type* ap_to_end,
   extern_type*& ap_to_next) const
  {
    return noconv;
  }

  virtual int do_encoding() const throw()
  {
    return 1;
  }

  virtual bool do_always_noconv() const throw()
  {
    return false;
  }

  /*virtual int do_length(State& s, const E* from, const E* from_end, size_t max) const
  {
    return codecvt<I, E, State>::do_length(s, from, from_end, max);
  }

  virtual int do_max_length() const throw()
  {
    return codecvt<I, E, State>::do_max_length();
  }*/
};

/*//! \brief Фасет для преобразования строк, содержащих символы
//!   в кодировке cp1251, в UNICODE
template <>
class conv_cp1251_to_wchar_t<wchar_t, char>
{
public:
  typedef std::codecvt_base::result result;
  typedef wchar_t intern_type;
  typedef char extern_type;

  const result ok;
  const result partial;
  const result error;
  const result noconv;

  explicit codecvt_cp1251_t(size_t r=0):
    std::codecvt<wchar_t, char, mbstate_t>(r),
    ok(std::codecvt_base::ok),
    partial(std::codecvt_base::partial),
    error(std::codecvt_base::error),
    noconv(std::codecvt_base::noconv),
    m_in_map(),
    m_out_map()
  {
    m_in_map[0x80] = 0x0402;
    m_in_map[0x81] = 0x0403;
    m_in_map[0x82] = 0x201A;
    m_in_map[0x83] = 0x0453;
    m_in_map[0x84] = 0x201E;
    m_in_map[0x85] = 0x2026;
    m_in_map[0x86] = 0x2020;
    m_in_map[0x87] = 0x2021;
    m_in_map[0x88] = 0x20AC;
    m_in_map[0x89] = 0x2030;
    m_in_map[0x8A] = 0x0409;
    m_in_map[0x8B] = 0x2039;
    m_in_map[0x8C] = 0x040A;
    m_in_map[0x8D] = 0x040C;
    m_in_map[0x8E] = 0x040B;
    m_in_map[0x8F] = 0x040F;
    m_in_map[0x90] = 0x0452;
    m_in_map[0x91] = 0x2018;
    m_in_map[0x92] = 0x2019;
    m_in_map[0x93] = 0x201C;
    m_in_map[0x94] = 0x201D;
    m_in_map[0x95] = 0x2022;
    m_in_map[0x96] = 0x2013;
    m_in_map[0x97] = 0x2014;
    //m_in_map[0x98] =     ;
    m_in_map[0x99] = 0x2122;
    m_in_map[0x9A] = 0x0459;
    m_in_map[0x9B] = 0x203A;
    m_in_map[0x9C] = 0x045A;
    m_in_map[0x9D] = 0x045C;
    m_in_map[0x9E] = 0x045B;
    m_in_map[0x9F] = 0x045F;
    m_in_map[0xA0] = 0x00A0;
    m_in_map[0xA1] = 0x040E;
    m_in_map[0xA2] = 0x045E;
    m_in_map[0xA3] = 0x0408;
    m_in_map[0xA4] = 0x00A4;
    m_in_map[0xA5] = 0x0490;
    m_in_map[0xA6] = 0x00A6;
    m_in_map[0xA7] = 0x00A7;
    m_in_map[0xA8] = 0x0401;
    m_in_map[0xA9] = 0x00A9;
    m_in_map[0xAA] = 0x0404;
    m_in_map[0xAB] = 0x00AB;
    m_in_map[0xAC] = 0x00AC;
    m_in_map[0xAD] = 0x00AD;
    m_in_map[0xAE] = 0x00AE;
    m_in_map[0xAF] = 0x0407;
    m_in_map[0xB0] = 0x00B0;
    m_in_map[0xB1] = 0x00B1;
    m_in_map[0xB2] = 0x0406;
    m_in_map[0xB3] = 0x0456;
    m_in_map[0xB4] = 0x0491;
    m_in_map[0xB5] = 0x00B5;
    m_in_map[0xB6] = 0x00B6;
    m_in_map[0xB7] = 0x00B7;
    m_in_map[0xB8] = 0x0451;
    m_in_map[0xB9] = 0x2116;
    m_in_map[0xBA] = 0x0454;
    m_in_map[0xBB] = 0x00BB;
    m_in_map[0xBC] = 0x0458;
    m_in_map[0xBD] = 0x0405;
    m_in_map[0xBE] = 0x0455;
    m_in_map[0xBF] = 0x0457;

    std::map<unsigned char, wchar_t>::const_iterator it = m_in_map.begin();
    while (it != m_in_map.end()) {
      m_out_map.insert(make_pair(it->second, it->first));
      ++it;
    }
  }
  virtual result do_in(const extern_type* ap_from,
    const extern_type* ap_from_end,
    const extern_type*& ap_from_next,
    intern_type* ap_to,
    intern_type* ap_to_end,
    intern_type*& ap_to_next) const
  {
    while(ap_from != ap_from_end) {
      if(ap_to == ap_to_end) {
        ap_from_next = ++ap_from;
        ap_to_next = ap_to;
        return partial;
      }
      const unsigned char from = static_cast<unsigned char>(*ap_from);
      // ASCII
      if (from <= 0x7F) {
        *ap_to = static_cast<wchar_t>(*ap_from);
      } else if (from >= 0xC0) {
        *ap_to = static_cast<wchar_t>(from + 0x350);
      } else {
        std::map<unsigned char, wchar_t>::const_iterator it =
          m_in_map.lower_bound(from);
        if ((it->first != from) || (it == m_in_map.end())) {
          ap_from_next = ++ap_from;
          ap_to_next = ++ap_to;
          return error;
        }
        *ap_to = it->second;
      }
      ++ap_to;
      ++ap_from;
    }
    ap_from_next = ap_from_end;
    ap_to_next = ap_to;
    return ok;
  }


  virtual result do_out(const intern_type* ap_from,
   const intern_type* ap_from_end,
   const intern_type*& ap_from_next,
   extern_type* ap_to,
   extern_type* ap_to_end,
   extern_type*& ap_to_next) const
  {
    while(ap_from != ap_from_end) {
      if(ap_to == ap_to_end) {
        ap_from_next = ++ap_from;
        ap_to_next = ap_to;
        return partial;
      }
      if ((*ap_from >= 0) && (*ap_from <= 0x7F)) {
        *ap_to = static_cast<unsigned char>(*ap_from);
      } else if ((*ap_from >= 0x410) && (*ap_from <=0x44F)) {
        *ap_to = static_cast<unsigned char>(*ap_from - 0x350);
      } else {
        std::map<wchar_t, unsigned char>::const_iterator it =
          m_out_map.lower_bound(*ap_from);
        if ((it->first != *ap_from) || (it == m_out_map.end())) {
          ap_from_next = ++ap_from;
          ap_to_next = ++ap_to;
          return error;
        }
        *ap_to = it->second;
      }
      ++ap_to;
      ++ap_from;
    }
    ap_from_next = ap_from_end;
    ap_to_next = ap_to;
    return ok;
  }

  virtual int do_encoding() const throw()
  {
    return 1;
  }

  virtual bool do_always_noconv() const throw()
  {
    return false;
  }
private:
  std::map<unsigned char, wchar_t> m_in_map;
  std::map<wchar_t, unsigned char> m_out_map;
};*/

//! \brief Фасет для преобразования строк, содержащих символы
//!   в кодировке cp1251, в UNICODE
template <>
class codecvt_cp1251_t<wchar_t, char, mbstate_t>: public
  std::codecvt<wchar_t, char, mbstate_t>
{
public:
  typedef std::codecvt_base::result result;
  const result ok;
  const result partial;
  const result error;
  const result noconv;

  explicit codecvt_cp1251_t(size_t r=0):
    std::codecvt<wchar_t, char, mbstate_t>(r),
    ok(std::codecvt_base::ok),
    partial(std::codecvt_base::partial),
    error(std::codecvt_base::error),
    noconv(std::codecvt_base::noconv),
    m_in_map(),
    m_out_map()
  {
    m_in_map[0x80] = 0x0402;
    m_in_map[0x81] = 0x0403;
    m_in_map[0x82] = 0x201A;
    m_in_map[0x83] = 0x0453;
    m_in_map[0x84] = 0x201E;
    m_in_map[0x85] = 0x2026;
    m_in_map[0x86] = 0x2020;
    m_in_map[0x87] = 0x2021;
    m_in_map[0x88] = 0x20AC;
    m_in_map[0x89] = 0x2030;
    m_in_map[0x8A] = 0x0409;
    m_in_map[0x8B] = 0x2039;
    m_in_map[0x8C] = 0x040A;
    m_in_map[0x8D] = 0x040C;
    m_in_map[0x8E] = 0x040B;
    m_in_map[0x8F] = 0x040F;
    m_in_map[0x90] = 0x0452;
    m_in_map[0x91] = 0x2018;
    m_in_map[0x92] = 0x2019;
    m_in_map[0x93] = 0x201C;
    m_in_map[0x94] = 0x201D;
    m_in_map[0x95] = 0x2022;
    m_in_map[0x96] = 0x2013;
    m_in_map[0x97] = 0x2014;
    //m_in_map[0x98] =     ;
    m_in_map[0x99] = 0x2122;
    m_in_map[0x9A] = 0x0459;
    m_in_map[0x9B] = 0x203A;
    m_in_map[0x9C] = 0x045A;
    m_in_map[0x9D] = 0x045C;
    m_in_map[0x9E] = 0x045B;
    m_in_map[0x9F] = 0x045F;
    m_in_map[0xA0] = 0x00A0;
    m_in_map[0xA1] = 0x040E;
    m_in_map[0xA2] = 0x045E;
    m_in_map[0xA3] = 0x0408;
    m_in_map[0xA4] = 0x00A4;
    m_in_map[0xA5] = 0x0490;
    m_in_map[0xA6] = 0x00A6;
    m_in_map[0xA7] = 0x00A7;
    m_in_map[0xA8] = 0x0401;
    m_in_map[0xA9] = 0x00A9;
    m_in_map[0xAA] = 0x0404;
    m_in_map[0xAB] = 0x00AB;
    m_in_map[0xAC] = 0x00AC;
    m_in_map[0xAD] = 0x00AD;
    m_in_map[0xAE] = 0x00AE;
    m_in_map[0xAF] = 0x0407;
    m_in_map[0xB0] = 0x00B0;
    m_in_map[0xB1] = 0x00B1;
    m_in_map[0xB2] = 0x0406;
    m_in_map[0xB3] = 0x0456;
    m_in_map[0xB4] = 0x0491;
    m_in_map[0xB5] = 0x00B5;
    m_in_map[0xB6] = 0x00B6;
    m_in_map[0xB7] = 0x00B7;
    m_in_map[0xB8] = 0x0451;
    m_in_map[0xB9] = 0x2116;
    m_in_map[0xBA] = 0x0454;
    m_in_map[0xBB] = 0x00BB;
    m_in_map[0xBC] = 0x0458;
    m_in_map[0xBD] = 0x0405;
    m_in_map[0xBE] = 0x0455;
    m_in_map[0xBF] = 0x0457;

    std::map<unsigned char, wchar_t>::const_iterator it = m_in_map.begin();
    while (it != m_in_map.end()) {
      m_out_map.insert(std::make_pair(it->second, it->first));
      ++it;
    }
  }
  virtual result do_in(mbstate_t&,
    const extern_type* ap_from,
    const extern_type* ap_from_end,
    const extern_type*& ap_from_next,
    intern_type* ap_to,
    intern_type* ap_to_end,
    intern_type*& ap_to_next) const
  {
    while(ap_from != ap_from_end) {
      if(ap_to == ap_to_end) {
        ap_from_next = ++ap_from;
        ap_to_next = ap_to;
        return partial;
      }
      const unsigned char from = static_cast<unsigned char>(*ap_from);
      // ASCII
      if (from <= 0x7F) {
        *ap_to = static_cast<wchar_t>(*ap_from);
      } else if (from >= 0xC0) {
        *ap_to = static_cast<wchar_t>(from + 0x350);
      } else {
        std::map<unsigned char, wchar_t>::const_iterator it =
          m_in_map.lower_bound(from);
        if ((it->first != from) || (it == m_in_map.end())) {
          ap_from_next = ++ap_from;
          ap_to_next = ++ap_to;
          return error;
        }
        *ap_to = it->second;
      }
      ++ap_to;
      ++ap_from;
    }
    ap_from_next = ap_from_end;
    ap_to_next = ap_to;
    return ok;
  }


  virtual result do_out(mbstate_t&,
   const intern_type* ap_from,
   const intern_type* ap_from_end,
   const intern_type*& ap_from_next,
   extern_type* ap_to,
   extern_type* ap_to_end,
   extern_type*& ap_to_next) const
  {
    while(ap_from != ap_from_end) {
      if(ap_to == ap_to_end) {
        ap_from_next = ++ap_from;
        ap_to_next = ap_to;
        return partial;
      }
      if ((*ap_from >= 0) && (*ap_from <= 0x7F)) {
        *ap_to = static_cast<unsigned char>(*ap_from);
      } else if ((*ap_from >= 0x410) && (*ap_from <=0x44F)) {
        *ap_to = static_cast<unsigned char>(*ap_from - 0x350);
      } else {
        std::map<wchar_t, unsigned char>::const_iterator it =
          m_out_map.lower_bound(*ap_from);
        if ((it->first != *ap_from) || (it == m_out_map.end())) {
          ap_from_next = ++ap_from;
          ap_to_next = ++ap_to;
          return error;
        }
        *ap_to = it->second;
      }
      ++ap_to;
      ++ap_from;
    }
    ap_from_next = ap_from_end;
    ap_to_next = ap_to;
    return ok;
  }

  virtual int do_encoding() const throw()
  {
    return 1;
  }

  virtual bool do_always_noconv() const throw()
  {
    return false;
  }
private:
  std::map<unsigned char, wchar_t> m_in_map;
  std::map<wchar_t, unsigned char> m_out_map;
};

//! \brief Фасет для преобразования строк, содержащих символы
//!   в кодировке UNICODE, в cp1251
template <>
class codecvt_cp1251_t<char, wchar_t, mbstate_t>: public
  std::codecvt<char, wchar_t, mbstate_t>
{
public:
  typedef std::codecvt_base::result result;

  explicit codecvt_cp1251_t(size_t r=0):
    std::codecvt<char, wchar_t, mbstate_t>(r),
    m_codecvt_wchar_char(r)
  {
  }
  virtual result do_in(state_type& a_state,
    const extern_type* ap_from,
    const extern_type* ap_from_end,
    const extern_type*& ap_from_next,
    intern_type* ap_to,
    intern_type* ap_to_end,
    intern_type*& ap_to_next) const
  {
    return m_codecvt_wchar_char.do_out(a_state,
      ap_from, ap_from_end, ap_from_next, ap_to, ap_to_end, ap_to_next);
  }

  virtual result do_out(state_type& a_state,
   const intern_type* ap_from,
   const intern_type* ap_from_end,
   const intern_type*& ap_from_next,
   extern_type* ap_to,
   extern_type* ap_to_end,
   extern_type*& ap_to_next) const
  {
    return m_codecvt_wchar_char.do_in(a_state,
      ap_from, ap_from_end, ap_from_next, ap_to, ap_to_end, ap_to_next);
  }

  virtual int do_encoding() const throw()
  {
    return 1;
  }

  virtual bool do_always_noconv() const throw()
  {
    return false;
  }

private:
  codecvt_cp1251_t<wchar_t, char, mbstate_t> m_codecvt_wchar_char;
};

} // namespace irs

#endif // IRS_CODECVT_H
