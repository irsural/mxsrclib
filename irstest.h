// Набор тестов
// Дата: 28.04.2010
// Дата создания: 20.09.2009

#ifndef IRSTESTH
#define IRSTESTH

#include <irsdefs.h>

#include <irscpp.h>
#include <mxdata.h>
#include <irserror.h>

#include <irsfinal.h>

namespace irs {

namespace compiler_test {

namespace template_template_inheritance {

// Наследование шаблонов от шаблонов
template <class T>
struct base_t
{
  typedef size_t size_type;

  enum {
    bits_on_byte = 8
  };

  T f(T x)
  {
    return x*x;
  }
};
template <class T, class B = base_t<T> >
struct derived_t: B
{
};
inline void exec(ostream& a_strm)
{
  derived_t<double> derived;
  a_strm << "base_t::f call through derived_t::f: " << derived.f(1.1) << '\n';
  a_strm << "base_t::bits_on_byte call through derived_t::bits_on_byte: ";
  a_strm << derived_t<double>::bits_on_byte << '\n';
  a_strm << "base_t::size_type call through derived_t::size_type: ";
  derived_t<double>::size_type size = 7;
  a_strm << size << '\n';
  a_strm << endl;
}

} //namespace template_template_inheritance

} //namespace compiler_test


namespace cstr_convert {

template <class T>
int to_int(T a_char)
{
  return static_cast<int>(a_char);
}

inline const char* cstr_from_codecvt_result_helper(
  codecvt_base::result a_result,
  char
)
{
  switch (a_result) {
    case codecvt_base::ok: {
      return "ok";
    } break;
    case codecvt_base::partial: {
      return "partial";
    } break;
    case codecvt_base::error: {
      return "error";
    } break;
    case codecvt_base::noconv: {
      return "noconv";
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("");
    } break;
  }
  return "unknown";
}
inline const wchar_t* cstr_from_codecvt_result_helper(
  codecvt_base::result a_result,
  wchar_t
)
{
  switch (a_result) {
    case codecvt_base::ok: {
      return L"ok";
    } break;
    case codecvt_base::partial: {
      return L"partial";
    } break;
    case codecvt_base::error: {
      return L"error";
    } break;
    case codecvt_base::noconv: {
      return L"noconv";
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("");
    } break;
  }
  return L"unknown";
}
template <class T>
const char* cstr_from_codecvt_result(codecvt_base::result a_result)
{
  return cstr_from_codecvt_result_helper(a_result, T());
}

inline codecvt_base::result codecvt_helper(mbstate_t state,
  const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
  char* to, char* to_end, char*& to_next)
{
  typedef codecvt<wchar_t, char, mbstate_t> convert_t;
  const convert_t& cdcvt = use_facet<convert_t>(locale());
  return cdcvt.out(state, from, from_end, from_next,
    to, to_end, to_next);
}
inline codecvt_base::result codecvt_helper(mbstate_t state,
  const char* from, const char* from_end, const char*& from_next,
  wchar_t* to, wchar_t* to_end, wchar_t*& to_next)
{
  typedef codecvt<wchar_t, char, mbstate_t> convert_t;
  const convert_t& cdcvt = use_facet<convert_t>(locale());
  return cdcvt.in(state, from, from_end, from_next,
    to, to_end, to_next);
}

// Преобразование из строки in_char_type* в строку out_char_type*
// Доступны только варианты:
// I = wchar_t, O = char
// I = char, O = wchar_t
template <class I, class O>
class convert_str_t
{
public:
  typedef I in_char_type;
  typedef O out_char_type;
  typedef irs::raw_data_t<out_char_type> data_type;
private:
  enum {
    m_size_default = static_cast<size_t>(-1)
  };
  data_type m_outstr_data;
public:
  convert_str_t(const in_char_type* ap_instr,
    size_t a_instr_size = m_size_default
  ):
    m_outstr_data()
  {
    IRS_STATIC_ASSERT(
      ((irs::type_detect_t<I>::index == irs::char_idx) &&
       (irs::type_detect_t<O>::index == irs::wchar_idx)) ||
      ((irs::type_detect_t<I>::index == irs::wchar_idx) &&
       (irs::type_detect_t<O>::index == irs::char_idx))
    );

    if (a_instr_size == m_size_default) {
      a_instr_size = wcslen(ap_instr) + 1;
    }
    m_outstr_data.resize(a_instr_size);

    const in_char_type* p_instr_next = ap_instr;
    data_type::pointer p_outstr_next = m_outstr_data.data();

    mbstate_t state;
    codecvt_base::result convert_result =
      codecvt_helper(
        state,

        ap_instr,
        ap_instr + a_instr_size,
        p_instr_next,

        m_outstr_data.data(),
        m_outstr_data.data() + m_outstr_data.size(),
        p_outstr_next
      );
    if (convert_result != codecvt_base::ok)
    {
      IRS_LIB_DBG_MSG(irsm("convert_str_t codecvt result: ") <<
        cstr_from_codecvt_result<char>(convert_result));
    }
  }
  const out_char_type* get() const
  {
    return m_outstr_data.data();
  }
  out_char_type* get()
  {
    return m_outstr_data.data();
  }
};

#ifdef IRS_UNICODE
#define IRS_WIDE_CHAR_FROM_TCHAR_STR(str) (str)
#define IRS_SIMPLE_CHAR_FROM_TCHAR_STR(str)\
  (irs::convert_str_t<wchar_t, char>(str).get())
#else //IRS_UNICODE
#define IRS_WIDE_CHAR_FROM_TCHAR_STR(str)\
  (irs::convert_str_t<char, wchar_t>(str).get())
#define IRS_SIMPLE_CHAR_FROM_TCHAR_STR(str) (str)
#endif //IRS_UNICODE

inline irs::raw_data_t<char> char_from_wchar_str_dbg(
  const irs::raw_data_t<wchar_t>& ins)
{
  irs::raw_data_t<wchar_t>::const_pointer in_it = ins.data();

  irs::raw_data_t<char> outs(ins.size());
  irs::raw_data_t<char>::pointer out_it = outs.data();

  typedef codecvt<wchar_t, char, mbstate_t> convert_t;
  const convert_t& cdcvt = use_facet<convert_t>(locale());

  mbstate_t state;
  codecvt_base::result convert_result = cdcvt.out(state, ins.data(),
    ins.data() + ins.size(), in_it, outs.data(), outs.data() + outs.size(),
    out_it);
  if (convert_result != codecvt_base::ok)
  {
    IRS_LIB_DBG_MSG(irsm("char_from_wchar_str_dbg codecvt error: ") <<
      cstr_from_codecvt_result<char>(convert_result));
  }

  return outs;
}

inline irs::raw_data_t<wchar_t> wchar_from_char_str_dbg(
  const irs::raw_data_t<char>& ins)
{
  irs::raw_data_t<char>::const_pointer in_it = ins.data();

  irs::raw_data_t<wchar_t> outs(ins.size());
  irs::raw_data_t<wchar_t>::pointer out_it = outs.data();

  typedef codecvt<wchar_t, char, mbstate_t> convert_t;
  const convert_t& cdcvt = use_facet<convert_t>(locale());
  mbstate_t state;
  codecvt_base::result convert_result = cdcvt.in(state, ins.data(),
    ins.data() + ins.size(), in_it, outs.data(), outs.data() + outs.size(),
    out_it);
  if (convert_result != codecvt_base::ok)
  {
    IRS_LIB_DBG_MSG(irsm("wchar_from_char_str_dbg codecvt error: ") <<
      cstr_from_codecvt_result<char>(convert_result));
  }

  return outs;
}

template <class T>
inline void raw_data_out(ostream& a_strm, const raw_data_t<T> a_data)
{
  ios::fmtflags strm_flags_save = a_strm.flags();
  a_strm << hex;
  if (a_data.size()) {
    transform(a_data.data(), a_data.data() + a_data.size() - 1,
      ostream_iterator<int>(a_strm, ", "), to_int<T>);
  }
  a_strm << to_int<T>(a_data.data()[a_data.size() - 1]);
  a_strm.flags(strm_flags_save);
}

inline void exec(ostream& a_strm)
{
  {
    wchar_t wstr[] = L"hello!?& Й";
    irs::raw_data_t<wchar_t> wstr_data(IRS_ARRAYSIZE(wstr));
    irs::memcpyex(wstr_data.data(), wstr, IRS_ARRAYSIZE(wstr));

    a_strm << "wchar_t array: ";
    raw_data_out(a_strm, wstr_data);
    a_strm << endl;

    irs::raw_data_t<char> str_data(IRS_ARRAYSIZE(wstr));
    str_data = char_from_wchar_str_dbg(wstr_data);

    a_strm << "char array: ";
    raw_data_out(a_strm, str_data);
    a_strm << endl;

    a_strm << "*" << str_data.data() << "*" << endl;
  }

  {
    char str[] = "Привет yes!?&";
    irs::raw_data_t<char> str_data(IRS_ARRAYSIZE(str));
    irs::memcpyex(str_data.data(), str, IRS_ARRAYSIZE(str));

    irs::raw_data_t<wchar_t> wstr_data(IRS_ARRAYSIZE(str));
    wstr_data = wchar_from_char_str_dbg(str_data);

    a_strm << "wchar_t array: ";
    raw_data_out(a_strm, wstr_data);
    a_strm << endl;

    irs::raw_data_t<char> str_back_data(IRS_ARRAYSIZE(str));
    str_back_data = char_from_wchar_str_dbg(wstr_data);
    a_strm << "*" << str_back_data.data() << "*" << endl;
  }

  {
    a_strm << IRS_SIMPLE_CHAR_FROM_TCHAR_STR(irst("hello!?& Й")) << endl;
  }

  a_strm << endl;
}

} //namespace cstr_convert

#ifdef NOP
// Простой динамический буфер
struct dyn_data_t
{
  irs_u8* data;
  irs_size_t size;

  dyn_data_t(irs_size_t a_size = 0):
    data(IRS_NULL),
    size(a_size)
  {
    resize(size);
  }
  ~dyn_data_t()
  {
    if (size) {
      delete data;
    }
  }
  void resize(irs_size_t a_size)
  {
    if (size) {
      delete data;
    }
    if (a_size) {
      data = new irs_u8[a_size];
      irs::memsetex(data, a_size);
      IRS_LIB_ASSERT(data != IRS_NULL);
      if (data) {
        size = a_size;
      } else {
        size = 0;
      }
    } else {
      data = IRS_NULL;
      size = 0;
    }
  }
};
#endif //NOP

} //namespace irs

#endif //IRSTESTH
