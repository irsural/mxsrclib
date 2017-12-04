#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irsstring.h>

#include <irsfinal.h>

irs::irsstr_locale_style_def_t& irs::irsstr_locale_style_def()
{
  static irsstr_locale_style_def_t locale_style;
  return locale_style;
}
