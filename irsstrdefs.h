// ����������� ��� ��������������� ������������ ����� ����� char � wchar_t
// ����: 17.09.2009
// ���� ��������: 17.09.2009

#ifndef IRSSTRDEFSH
#define IRSSTRDEFSH

#include <irsdefs.h>
#include <irsstring.h>
#include <mxdata.h>

namespace irs {

#ifdef IRS_UNICODE

#define irst(cstr) L##cstr

typedef wint_t char_int_t;
typedef wchar_t char_t;
typedef wstring string_t;
typedef wostream ostream_t;
typedef wistream istream_t;

#else //IRS_UNICODE

#define irst(cstr) cstr

typedef int char_int_t;
typedef char char_t;
typedef irs::string string_t;
typedef ostream ostream_t;
typedef istream istream_t;

#endif //IRS_UNICODE

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

inline bool isalnumt(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::alnum,
    static_cast<char_t>(a_ch));
}
                                              
inline bool isalphat(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::alpha,
    static_cast<char_t>(a_ch));
}

inline bool iscntrlt(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::cntrl,
    static_cast<char_t>(a_ch));
}

inline bool isdigitt(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::digit,
    static_cast<char_t>(a_ch));
}

inline bool isgrapht(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::graph,
    static_cast<char_t>(a_ch));
}

inline bool islowert(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::lower,
    static_cast<char_t>(a_ch));
}

inline bool isprintt(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::print,
    static_cast<char_t>(a_ch));
}

inline bool ispunctt(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::punct,
    static_cast<char_t>(a_ch));
}

inline bool isspacet(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::space,
    static_cast<char_t>(a_ch));
}

inline bool isuppert(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::upper,
    static_cast<char_t>(a_ch));
}

inline bool isxdigitt(char_int_t a_ch, const locale& a_loc = locale())
{
  return use_facet<ctype<char_t> >(a_loc).is(ctype<char_t>::xdigit,
    static_cast<char_t>(a_ch));
}

inline const char_t* strchrt(const char_t* a_str, char_int_t a_ch)
{
  #ifdef IRS_UNICODE
  return wcschr(a_str, a_ch);
  #else //IRS_UNICODE
  return strchr(a_str, a_ch);
  #endif //IRS_UNICODE
}

#else //IRS_FULL_STDCPPLIB_SUPPORT
inline bool isalnumt(char_int_t a_ch)
{
  return ::isalnum(a_ch);
}

inline bool isalphat(char_int_t a_ch)
{
  return ::isalpha(a_ch);
}

inline bool iscntrlt(char_int_t a_ch)
{
  return ::iscntrl(a_ch);
}

inline bool isdigitt(char_int_t a_ch)
{
  return ::isdigit(a_ch);
}

inline bool isgrapht(char_int_t a_ch)
{
  return ::isgraph(a_ch);
}

inline bool islowert(char_int_t a_ch)
{
  return ::islower(a_ch);
}

inline bool isprintt(char_int_t a_ch)
{
  return ::isprint(a_ch);
}

inline bool ispunctt(char_int_t a_ch)
{
  return ::ispunct(a_ch);
}

inline bool isspacet(char_int_t a_ch)
{
  return ::isspace(a_ch);
}

inline bool isuppert(char_int_t a_ch)
{
  return ::isupper(a_ch);
}

inline bool isxdigitt(char_int_t a_ch)
{
  return ::isxdigit(a_ch);
}

inline const char_t* strchrt(const char_t* a_str, int a_ch)
{
  return strchr(a_str, a_ch);
}

#endif //IRS_FULL_STDCPPLIB_SUPPORT

} //namespace irs

#if !defined(__WATCOMC__) && !defined(__ICCAVR__)
#ifndef NOP
inline irs::raw_data_t<char> char_from_wchar_str(
  const irs::raw_data_t<wchar_t>& ins)
{
  irs::raw_data_t<char> outs(ins.size());
  irs::raw_data_t<wchar_t>::const_pointer in_it = ins.data();
  irs::raw_data_t<char>::pointer out_it = outs.data();
  const codecvt<wchar_t, char, mbstate_t>& cdcvt =
    use_facet<codecvt<wchar_t, char, mbstate_t> >(locale());

  mbstate_t state;
  cdcvt.out(state, ins.data(), ins.data() + ins.size(), in_it,
    outs.data(), outs.data() + outs.size(), out_it);

  return outs;

  #ifdef NOP
  using namespace std;
  mbstate_t state;
  string ins("\xfc \xcc \xcd \x61 \xe1 \xd9 \xc6 \xf5");
  string ins2(ins.size(),'.');
  string outs(ins.size(),'.');
  // Print initial contents of buffers
  cout << "Before:\n" << ins << endl;
  cout << ins2 << endl;
  cout << outs << endl << endl;
  // Initialize buffers
  string::iterator in_it = ins.begin();
  string::iterator out_it = outs.begin();
  locale loc(locale(),new ex_codecvt);
  // Now get the facet from the locale 
  const codecvt<char,char,mbstate_t>& cdcvt = 
    use_facet<codecvt<char,char,mbstate_t> >(loc);

  // convert the buffer
  cdcvt.in(state,ins.begin(),ins.end(),in_it,
    outs.begin(),outs.end(),out_it);
  cout << "After in:\n" << ins << endl;
  cout << ins2 << endl;
  cout << outs << endl << endl;
  
  // Lastly, convert back to the original codeset
  in_it = ins.begin();
  out_it = outs.begin();
  cdcvt.out(state, outs.begin(),outs.end(),out_it,
    ins2.begin(),ins2.end(),in_it);
  cout << "After out:\n" << ins << endl;
  cout << ins2 << endl;
  cout << outs << endl;
  
  return 0;
  #endif //NOP
}
#endif //NOP
#endif //__WATCOMC__

#endif //IRSSTRDEFSH
