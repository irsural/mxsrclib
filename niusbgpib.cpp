// ������ � National Instruments USB-GPIB
// ����: 06.08.2010
// ���� ��������: 06.08.2010

#include <irsdefs.h>

#include <niusbgpib.h>

#include <irsfinal.h>

#if defined(IRS_WIN32)

ni_usb_gpib_t ni_usb_gpib_t::f_instance;

#endif // defined(IRS_WIN32)
