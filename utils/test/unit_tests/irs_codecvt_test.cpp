#include <irs_codecvt.h>
#include <irsstrdefs.h>
#include <irserror.h>

#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE test_irs_codecvt

BOOST_AUTO_TEST_SUITE(test_codecvt_cp1251_t)

#ifndef __GNUC__

BOOST_AUTO_TEST_CASE(test_case1)
{
  irs::codecvt_cp1251_t<wchar_t, char, std::mbstate_t> codecvt_wchar_cp1251;
  mbstate_t state;

  //char in_str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    //"јЅ¬√ƒ≈®∆«»… ЋћЌќѕ–—“”‘’÷„ЎўЏџ№ЁёяабвгдеЄжзийклмнопрстуфхцчшщъыьэю€";

  // —трока в кодировке cp1251 записана в виде escape-последовательности,
  //  чтобы не зависеть от кодировки исходных файлов
  char in_str[] = "\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e"
    "\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x61\x62\x63\x64\x65\x66"
    "\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78"
    "\x79\x7a\xc0\xc1\xc2\xc3\xc4\xc5\xa8\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce"
    "\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0"
    "\xe1\xe2\xe3\xe4\xe5\xb8\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1"
    "\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

  wchar_t etalon_str_wchar[] =
    L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    L"јЅ¬√ƒ≈®∆«»… ЋћЌќѕ–—“”‘’÷„ЎўЏџ№ЁёяабвгдеЄжзийклмнопрстуфхцчшщъыьэю€";

  IRS_LIB_ASSERT(IRS_ARRAYSIZE(in_str) == IRS_ARRAYSIZE(etalon_str_wchar));

  const char* in_str_end = in_str + IRS_ARRAYSIZE(in_str);
  const char* in_str_next = in_str;

  wchar_t out_str[IRS_ARRAYSIZE(in_str)];
  wchar_t* out_str_end = out_str + IRS_ARRAYSIZE(out_str);
  wchar_t* out_str_next = out_str;

  std::codecvt_base::result convert_result = codecvt_wchar_cp1251.in(
    state, in_str, in_str_end, in_str_next, out_str, out_str_end, out_str_next);

  BOOST_CHECK(convert_result == std::codecvt_base::ok);
  BOOST_CHECK(std::equal(out_str, out_str_end, etalon_str_wchar));
}

BOOST_AUTO_TEST_CASE(test_case2)
{
  irs::codecvt_cp1251_t<char, wchar_t, std::mbstate_t> codecvt_cp1251_wchar;
  mbstate_t state;

  wchar_t in_str[] =
    L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    L"јЅ¬√ƒ≈®∆«»… ЋћЌќѕ–—“”‘’÷„ЎўЏџ№ЁёяабвгдеЄжзийклмнопрстуфхцчшщъыьэю€";

  //char etalon_str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    //"јЅ¬√ƒ≈®∆«»… ЋћЌќѕ–—“”‘’÷„ЎўЏџ№ЁёяабвгдеЄжзийклмнопрстуфхцчшщъыьэю€";
  // —трока в кодировке cp1251 записана в виде escape-последовательности,
  //  чтобы не зависеть от кодировки исходных файлов
  char etalon_str[] = "\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e"
    "\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x61\x62\x63\x64\x65\x66"
    "\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78"
    "\x79\x7a\xc0\xc1\xc2\xc3\xc4\xc5\xa8\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce"
    "\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0"
    "\xe1\xe2\xe3\xe4\xe5\xb8\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1"
    "\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

  IRS_LIB_ASSERT(IRS_ARRAYSIZE(in_str) == IRS_ARRAYSIZE(etalon_str));

  const wchar_t* in_str_end = in_str + IRS_ARRAYSIZE(in_str);
  const wchar_t* in_str_next = in_str;

  char out_str[IRS_ARRAYSIZE(in_str)];
  char* out_str_end = out_str + IRS_ARRAYSIZE(out_str);
  char* out_str_next = out_str;

  std::codecvt_base::result convert_result = codecvt_cp1251_wchar.in(
    state, in_str, in_str_end, in_str_next, out_str, out_str_end, out_str_next);

  BOOST_CHECK(convert_result == std::codecvt_base::ok);
  BOOST_CHECK(std::equal(out_str, out_str_end, etalon_str));
}

#endif // __GNUC__

BOOST_AUTO_TEST_SUITE_END()
