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

#include <irslimits.h>

#include <irsfinal.h>

namespace irs {

template <class cpT, class ucT>
class cp1251_unicode_converter_t
{
public:
  typedef std::codecvt_base::result result;
  const result ok;
  const result partial;
  const result error;

  explicit cp1251_unicode_converter_t():
    ok(std::codecvt_base::ok),
    partial(std::codecvt_base::partial),
    error(std::codecvt_base::error),
    m_cp_to_uc_map(),
    m_uc_to_cp_map()
  {
    m_cp_to_uc_map[0x80] = 0x0402;
    m_cp_to_uc_map[0x81] = 0x0403;
    m_cp_to_uc_map[0x82] = 0x201A;
    m_cp_to_uc_map[0x83] = 0x0453;
    m_cp_to_uc_map[0x84] = 0x201E;
    m_cp_to_uc_map[0x85] = 0x2026;
    m_cp_to_uc_map[0x86] = 0x2020;
    m_cp_to_uc_map[0x87] = 0x2021;
    m_cp_to_uc_map[0x88] = 0x20AC;
    m_cp_to_uc_map[0x89] = 0x2030;
    m_cp_to_uc_map[0x8A] = 0x0409;
    m_cp_to_uc_map[0x8B] = 0x2039;
    m_cp_to_uc_map[0x8C] = 0x040A;
    m_cp_to_uc_map[0x8D] = 0x040C;
    m_cp_to_uc_map[0x8E] = 0x040B;
    m_cp_to_uc_map[0x8F] = 0x040F;
    m_cp_to_uc_map[0x90] = 0x0452;
    m_cp_to_uc_map[0x91] = 0x2018;
    m_cp_to_uc_map[0x92] = 0x2019;
    m_cp_to_uc_map[0x93] = 0x201C;
    m_cp_to_uc_map[0x94] = 0x201D;
    m_cp_to_uc_map[0x95] = 0x2022;
    m_cp_to_uc_map[0x96] = 0x2013;
    m_cp_to_uc_map[0x97] = 0x2014;
    //m_cp_to_uc_map[0x98] =     ;
    m_cp_to_uc_map[0x99] = 0x2122;
    m_cp_to_uc_map[0x9A] = 0x0459;
    m_cp_to_uc_map[0x9B] = 0x203A;
    m_cp_to_uc_map[0x9C] = 0x045A;
    m_cp_to_uc_map[0x9D] = 0x045C;
    m_cp_to_uc_map[0x9E] = 0x045B;
    m_cp_to_uc_map[0x9F] = 0x045F;
    m_cp_to_uc_map[0xA0] = 0x00A0;
    m_cp_to_uc_map[0xA1] = 0x040E;
    m_cp_to_uc_map[0xA2] = 0x045E;
    m_cp_to_uc_map[0xA3] = 0x0408;
    m_cp_to_uc_map[0xA4] = 0x00A4;
    m_cp_to_uc_map[0xA5] = 0x0490;
    m_cp_to_uc_map[0xA6] = 0x00A6;
    m_cp_to_uc_map[0xA7] = 0x00A7;
    m_cp_to_uc_map[0xA8] = 0x0401;
    m_cp_to_uc_map[0xA9] = 0x00A9;
    m_cp_to_uc_map[0xAA] = 0x0404;
    m_cp_to_uc_map[0xAB] = 0x00AB;
    m_cp_to_uc_map[0xAC] = 0x00AC;
    m_cp_to_uc_map[0xAD] = 0x00AD;
    m_cp_to_uc_map[0xAE] = 0x00AE;
    m_cp_to_uc_map[0xAF] = 0x0407;
    m_cp_to_uc_map[0xB0] = 0x00B0;
    m_cp_to_uc_map[0xB1] = 0x00B1;
    m_cp_to_uc_map[0xB2] = 0x0406;
    m_cp_to_uc_map[0xB3] = 0x0456;
    m_cp_to_uc_map[0xB4] = 0x0491;
    m_cp_to_uc_map[0xB5] = 0x00B5;
    m_cp_to_uc_map[0xB6] = 0x00B6;
    m_cp_to_uc_map[0xB7] = 0x00B7;
    m_cp_to_uc_map[0xB8] = 0x0451;
    m_cp_to_uc_map[0xB9] = 0x2116;
    m_cp_to_uc_map[0xBA] = 0x0454;
    m_cp_to_uc_map[0xBB] = 0x00BB;
    m_cp_to_uc_map[0xBC] = 0x0458;
    m_cp_to_uc_map[0xBD] = 0x0405;
    m_cp_to_uc_map[0xBE] = 0x0455;
    m_cp_to_uc_map[0xBF] = 0x0457;

    typename std::map<cpT, ucT>::const_iterator it = m_cp_to_uc_map.begin();
    while (it != m_cp_to_uc_map.end()) {
      m_uc_to_cp_map.insert(std::make_pair(it->second, it->first));
      ++it;
    }
  }

  result cp1251_to_unicode(const cpT* ap_from, const cpT* ap_from_end,
    const cpT*& ap_from_next, ucT* ap_to, ucT* ap_to_end,
    ucT*& ap_to_next) const
  {
    while(ap_from != ap_from_end) {
      if(ap_to == ap_to_end) {
        ap_from_next = ++ap_from;
        ap_to_next = ap_to;
        return partial;
      }
      const cpT from = static_cast<cpT>(*ap_from);
      // ASCII
      if (from <= 0x7F) {
        *ap_to = static_cast<ucT>(*ap_from);
      } else if (from >= 0xC0) {
        *ap_to = static_cast<ucT>(from + 0x350);
      } else {
        typename std::map<cpT, ucT>::const_iterator it =
          m_cp_to_uc_map.lower_bound(from);
        if ((it->first != from) || (it == m_cp_to_uc_map.end())) {
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


  result unicode_to_cp1251(const ucT* ap_from, const ucT* ap_from_end,
    const ucT*& ap_from_next, cpT* ap_to, cpT* ap_to_end,
    cpT*& ap_to_next) const
  {
    while(ap_from != ap_from_end) {
      if(ap_to == ap_to_end) {
        ap_from_next = ++ap_from;
        ap_to_next = ap_to;
        return partial;
      }
      #ifdef __ICCARM__
      #pragma diag_suppress=Pe186
      #endif // __ICCARM__
      IRS_STATIC_ASSERT(sizeof(ucT) <= sizeof(unsigned int));
      const unsigned int from = *ap_from;
      if (from <= 0x7F) {
        #ifdef __ICCARM__
        #pragma diag_default=Pe186
        #endif // __ICCARM__
        *ap_to = static_cast<cpT>(*ap_from);
      } else if ((from >= 0x410) && (from <=0x44F)) {
        *ap_to = static_cast<cpT>(*ap_from - 0x350);
      } else {
        typename std::map<ucT, cpT>::const_iterator it =
          m_uc_to_cp_map.lower_bound(*ap_from);
        if ((it->first != *ap_from) || (it == m_uc_to_cp_map.end())) {
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

private:
  std::map<cpT, ucT> m_cp_to_uc_map;
  std::map<ucT, cpT> m_uc_to_cp_map;
};


template <class internT, class externT, class stateT>
class codecvt_cp1251_t: public std::codecvt<internT, externT, stateT>
{
public:
  typedef std::codecvt_base::result result;
  //#ifdef __ICCARM__
  typedef typename std::codecvt<internT, externT, stateT>::intern_type
    intern_type;
  typedef typename std::codecvt<internT, externT, stateT>::extern_type
    extern_type;
  typedef typename std::codecvt<internT, externT, stateT>::state_type
    state_type;
  //#endif // __ICCARM__
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
    #ifdef __ICCARM__
    IRS_ASSERT(false);
    #else // !__ICCARM__
    IRS_STATIC_ASSERT(
      (is_equal_types<intern_type, extern_type>::type == yes_type()));
    #endif // !__ICCARM__
  }
  virtual result do_in(state_type& /*a_state*/,
    const extern_type* /*ap_from*/,
    const extern_type* /*ap_from_end*/,
    const extern_type*& /*ap_from_next*/,
    intern_type* /*ap_to*/,
    intern_type* /*ap_to_end*/,
    intern_type*& /*ap_to_next*/) const
  {
    return noconv;
  }

  virtual result do_out(state_type& /*a_state*/,
   const intern_type* /*ap_from*/,
   const intern_type* /*ap_from_end*/,
   const intern_type*& /*ap_from_next*/,
   extern_type* /*ap_to*/,
   extern_type* /*ap_to_end*/,
   extern_type*& /*ap_to_next*/) const
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
};


template <>
class codecvt_cp1251_t<uint16_t, char, mbstate_t>: public
  std::codecvt<uint16_t, char, mbstate_t>
{
public:
  typedef std::codecvt_base::result result;

  explicit codecvt_cp1251_t(size_t r=0):
    std::codecvt<uint16_t, char, mbstate_t>(r),
    m_converter()
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
    return m_converter.cp1251_to_unicode(
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
    return m_converter.unicode_to_cp1251(
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
  cp1251_unicode_converter_t<char, uint16_t> m_converter;
};


template <>
class codecvt_cp1251_t<char, uint16_t, mbstate_t>: public
  std::codecvt<char, uint16_t, mbstate_t>
{
public:
  typedef std::codecvt_base::result result;

  explicit codecvt_cp1251_t(size_t r=0):
    std::codecvt<char, uint16_t, mbstate_t>(r),
    m_converter()
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
    return m_converter.unicode_to_cp1251(
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
    return m_converter.cp1251_to_unicode(
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
  cp1251_unicode_converter_t<char, uint16_t> m_converter;
};


//! \brief Фасет для преобразования строк, содержащих символы
//!   в кодировке cp1251, в UNICODE
template <>
class codecvt_cp1251_t<wchar_t, char, mbstate_t>: public
  std::codecvt<wchar_t, char, mbstate_t>
{
public:
  typedef std::codecvt_base::result result;

  explicit codecvt_cp1251_t(size_t r=0):
    std::codecvt<wchar_t, char, mbstate_t>(r),
    m_converter()
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
    return m_converter.cp1251_to_unicode(
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
    return m_converter.unicode_to_cp1251(
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
  cp1251_unicode_converter_t<char, wchar_t> m_converter;
};

#if !(defined(__BORLANDC__) && (__BORLANDC__ < IRS_CPP_BUILDER2010))

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
    m_converter()
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
    return m_converter.unicode_to_cp1251(
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
    return m_converter.cp1251_to_unicode(
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
  cp1251_unicode_converter_t<char, wchar_t> m_converter;
};

#endif // !(defined(__BORLANDC__) && (__BORLANDC__>= IRS_CPP_BUILDER4))

} // namespace irs

#endif // IRS_CODECVT_H
