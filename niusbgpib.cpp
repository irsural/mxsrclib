//! \file
//! \ingroup drivers_group
//! \brief Работа с National Instruments USB-GPIB

//! Дата: 06.08.2010\n
//! Дата создания: 06.08.2010

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <niusbgpib.h>

#include <irsfinal.h>

#if defined(IRS_WIN32)

irs::string_t get_ni_usb_gpib_dll_file_name(
  ni_usb_gpib_dll_file_name_t a_ni_usb_gpib_dll_file_name)
{
  irs::string_t file_name;
  if (a_ni_usb_gpib_dll_file_name == ni_fn_gpib_32) {
    file_name = irst("GPIB-32.DLL");
  } else if (a_ni_usb_gpib_dll_file_name == ni_fn_agtgpib32) {
    file_name = irst("agtgpib32.dll");
  } else {
    IRS_LIB_ERROR(irs::ec_standard, "Неучтенное значение перечисляемого типа");
  }
  return file_name;
}

//ni_usb_gpib_t ni_usb_gpib_t::f_instance;
ni_usb_gpib_t* ni_usb_gpib_t::get_instance()
{
  static ni_usb_gpib_t f_instance;
  return &f_instance;
}

#endif // defined(IRS_WIN32)
