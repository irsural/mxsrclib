//! \file
//! \ingroup system_utils_group
//! \brief ������� ��� ������ � ������������ C++Builder
//!
//! ����: 15.09.2010\n
//! ���� ��������: 9.09.2009

#ifndef cbsysutilsH
#define cbsysutilsH

#include <irsdefs.h>

// ***************************************************************************
// ����������� ������ ����� sysvary.h � C++ Builder 4, ������� ���������
// � include-������ ����� OleCtnrs.hpp. � include-������ sysvary.h �����������
// ����������� ���� utilcls.h
#if __BORLANDC__ < IRS_CPP_BUILDER6
#include <utilcls.h>
#endif //__BORLANDC__ < IRS_CPP_BUILDER6
// ***************************************************************************
#include <OleCtnrs.hpp>
#include <SysUtils.hpp>
#include <Grids.hpp>
#include <wstring.h>

#include <irssysutils.h>
#include <irstable.h>
#include <irserror.h>
#include <irsdefs.h>
#include <irscpp.h>

#include <irsfinal.h>


namespace irs {

namespace cbuilder {

typedef irs::string stringns_t;
typedef size_t sizens_t;

//! \addtogroup system_utils_group
//! @{

#if IRS_LIB_VERSION_SUPPORT_LESS(390)
//! \brief ������������� ������� �������� ����� � �����.
template<class T>
inline AnsiString number_to_str(
  const T a_num,
  const int a_precision = -1,
  const irs::num_mode_t a_num_mode = irs::num_mode_general)
{
  //irs::string str_value = a_num;
  //AnsiString astring_value = str_value.c_str();
  irs::string str_value = irs::number_to_str(a_num, a_precision, a_num_mode);
  AnsiString astring_value = str_value.c_str();
  return astring_value;
};
//! \brief ������������� ������� �������� ������ � �����.
template<class T>
inline bool str_to_number(const AnsiString& a_str, T& a_number)
{
  irs::string str = a_str.c_str();
  return str.to_number(a_number);
};
#endif // #if IRS_LIB_VERSION_SUPPORT_LESS(390)

//! \brief ��������� ������ �����
//! \author Lyashchov Maxim
struct file_version_t
{
  typedef unsigned short size_type;
  size_type major;
  size_type minor;
  size_type release;
  size_type build;
  file_version_t();
  file_version_t(
    size_type a_major,
    size_type a_minor,
    size_type a_release,
    size_type a_build
  );
  bool operator<(const file_version_t& a_file_version) const;
  bool operator==(const file_version_t& a_file_version) const;
  bool operator!=(const file_version_t& a_file_version) const;
};

//! \brief ������ ������ �����
//! \author Lyashchov Maxim
bool get_file_version(const irs::string_t& a_file_name,
  file_version_t& a_version);

//! \brief ��������� ��������� � ������ ����� � ������
//! \author Lyashchov Maxim
irs::string_t file_version_to_str(const file_version_t& a_file_version);

//! \brief ��������� ������ � ��� file_version_t
//! \author Lyashchov Maxim
bool str_to_file_version(const irs::string_t& a_str,
  file_version_t* ap_file_version);

//! \brief ������ ������ �� ����� MSExel(xls) � �������
//! \author Lyashchov Maxim
void file_xls_table_read(const string_t& a_book_name,
  const string_t& a_sheet,
  const string_t& a_cell_first,
  const string_t& a_cell_last,
  table_string_t* ap_table_string,
  bool* ap_read_success);

//! \brief ���������� ������ �� ������� � ���� MSExel(xls).
//! \author Lyashchov Maxim
void file_xls_table_write(const string_t& a_book_name,
  const string_t& a_sheet,
  const string_t& a_cell_first,
  const table_string_t& a_table_string,
  bool* ap_read_success);

//! \brief �������� ������ �� TStringGrid � table_string_t.
//! \author Lyashchov Maxim
//!
//! ���� ������ ������� table_string_t ������������ ��� ������ ������, ��
//!   ������� ����� ������������� ���������.
void string_grid_to_table_string(
  const TStringGrid& a_string_grid,
  irs::table_string_t* ap_table_string
);

//! \brief �������� �������� ����� �� TStringGrid � table_string_t, � ���������
//!   ��������� �������.
//! \author Lyashchov Maxim
//!
//! ���� ������ ������� table_string_t ������������ ��� ������ ������, ��
//!   ������� ����� ������������� ���������.
void string_grid_to_table_string(
  const rect_t& a_string_grid_rect,
  const TStringGrid& a_string_grid,
  const point_t& a_table_string_point,
  irs::table_string_t* ap_table_string
);

//! \brief �������� ������ �� table_string_t � TStringGrig.
//! \author Lyashchov Maxim
//!
//! ���� ������ TStringGrig ������������ ��� ������ ������, ������ �����
//!   ������������� ��������.
void table_string_to_string_grid(
  const table_string_t& a_table_string,
  TStringGrid* ap_string_grid
);

//! \brief �������� �������� ����� �� table_string_t � TStringGrig, � ���������
//!   ��������� �������.
//! \author Lyashchov Maxim
//!
//! ���� ������ TStringGrig ������������ ��� ������ ������, ������ �����
//!   ������������� ��������.
void table_string_to_string_grid(
  const rect_t& a_table_string_rect,
  const table_string_t& a_table_string,
  const point_t& a_string_grid_point,
  TStringGrid* ap_string_grid
);

//������� ��������� ������ ����� � ��� �����, ���� ��� ���
//� ����������� ��������� ����������(������� � ������ .���), ���� ��� ���
string_t file_path(string_t a_file_name, string_t a_extension = string_t());
// ����� � ���������
//void out_to_tree_view();

//! @}

}; //namespace cbuilder

}; //namespace irs

#endif //cbsysutilsH

