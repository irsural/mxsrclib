//! \file
//! \ingroup error_processing_group
//! \brief ���������� ���������� ���
//!
//! ����: 04.10.2009\n
//! ������ ����: 17.11.2008

#ifndef irsexceptH
#define irsexceptH

#include <irsdefs.h>
#include <irscpp.h>
#include <exception>

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

namespace irs {

//! \addtogroup error_processing_group
//! @{

// ������� ����� ��� ���� ���������� �������� ���
class mxexception: public exception
{
public:
  virtual ~mxexception() throw()
  {
  }
  virtual const char *what() const throw()
  {
    return "mxexception";
  }
};
// ������� ����� ��� ����������-�����������
class assert_e: public mxexception
{
public:
  virtual ~assert_e() throw()
  {
  }
  assert_e(const char *ap_assert_str = 0, const char *ap_file_name = 0,
    int a_line_number = 0, const char *ap_message = 0,
    const char* ap_function = 0);
  virtual const char *what() const throw();
private:
  bool m_user_message;
  string m_full_message;
};

//! @}

} //namespace irs

#endif //IRS_FULL_STDCPPLIB_SUPPORT

#endif //irsexceptH
