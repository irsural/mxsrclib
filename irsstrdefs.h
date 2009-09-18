// Определения для автоматического переключения строк между char и wchar_t
// Дата: 17.09.2009
// Дата создания: 17.09.2009

#ifndef IRSSTRDEFSH
#define IRSSTRDEFSH

#include <irsdefs.h>
#include <irsstring.h>
#include <mxdata.h>

namespace irs {

#ifdef IRS_UNICODE

#define irst(cstr) L##cstr

typedef wchar_t char_t;
typedef wstring string_t;
typedef wostream ostream_t;
typedef wistream istream_t;

#else //IRS_UNICODE

#define irst(cstr) cstr

typedef char char_t;
typedef irs::string string_t;
typedef ostream ostream_t;
typedef istream istream_t;

#endif //IRS_UNICODE

} //namespace irs

#ifdef NOP
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

#endif //IRSSTRDEFSH
