//---------------------------------------------------------------------------
// ������� ��� ������ � ������������ C++Builder
// ����: 2.09.2009

#ifndef cbsysutilsH
#define cbsysutilsH

// ***************************************************************************
// ����������� ������ ����� sysvary.h � C++ Builder 4, ������� ���������
// � include-������ ����� OleCtnrs.hpp. � include-������ sysvary.h �����������
// ����������� ���� utilcls.h
#include <irsdefs.h>
#if __BORLANDC__ < IRS_CPP_BUILDER6
#include <utilcls.h>
#endif //__BORLANDC__ < IRS_CPP_BUILDER6
// ***************************************************************************
#include <OleCtnrs.hpp>
#include <SysUtils.hpp>

#include <irssysutils.h>
#include <irstable.h>
#include <irserror.h>

namespace irs {
namespace cbuilder {
typedef irs::string string_t;
typedef size_t sizens_t;

// ������������� ������� �������� ����� � �����
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
// ������������� ������� �������� ������ � �����
template<class T>
inline bool str_to_number(const AnsiString& a_str, T& a_number)
{
  irs::string str = a_str.c_str();
  return str.to_number(a_number);
};

// ��������� ������ �����
struct file_version_t
{
  unsigned short major;
  unsigned short minor;
  unsigned short release;
  unsigned short build;
};

// ������ ������ �����
bool get_file_version(
  const irs::string& a_file_name, file_version_t& a_version);
// ������� ��������� � ������ ����� � ������   
irs::string file_version_to_str(const file_version_t& a_file_version);

void file_xls_table_read(const string_t& a_book_name,
  const string_t& a_sheet,
  const string_t& a_cell_first,
  const string_t& a_cell_last,
  table_string_t* ap_table_string,
  bool* ap_read_success);

void file_xls_table_write(const string_t& a_book_name,
  const string_t& a_sheet,
  const string_t& a_cell_first,    
  const table_string_t& a_table_string,
  bool* ap_read_success);


// ����� � ���������
//void out_to_tree_view();
}; //namespace cbuilder
}; //namespace irs

//---------------------------------------------------------------------------
#endif //cbsysutilsH
