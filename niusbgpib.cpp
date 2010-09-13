// Работа с National Instruments USB-GPIB
// Дата: 06.08.2010
// Дата создания: 06.08.2010

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <niusbgpib.h>

#include <irsfinal.h>

#if defined(IRS_WIN32)

ni_usb_gpib_t ni_usb_gpib_t::f_instance;

#endif // defined(IRS_WIN32)
