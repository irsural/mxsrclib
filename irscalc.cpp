// �����������
// ����: 19.09.2009
// ������ ����: 01.10.2008

// � Watcom C++ ���� ������ ��-const ������ ������� map::find
#ifndef __WATCOMC__

#include <irscalc.h>

irs::string irs::preprocessing_str(const irs::string& a_str)
{
  irs::string str = a_str;
  //������� �������
  /*int pos_ch_space = str.find(' ');
  while (pos_ch_space != irs::string::npos) {
    str.erase(pos_ch_space, 1);
    pos_ch_space = str.find(' ');
  }*/
  // ������� ������� �� ����� ������
  int pos_ch_return = str.find("\r");
  while (pos_ch_return != irs::string::npos) {
    str.erase(pos_ch_return, 2);
    pos_ch_return = str.find("\r");
  }
  // ������� ���������
  int pos_ch_tab = str.find("\t");
  while (pos_ch_tab != irs::string::npos) {
    str.erase(pos_ch_tab, 2);
    pos_ch_tab = str.find("\t");
  }
  // �������� ������� �� �����
  int pos_ch_decimal_point = str.find(",");
  while (pos_ch_decimal_point != irs::string::npos) {
    str.replace(pos_ch_decimal_point, 1, ".");
    pos_ch_decimal_point = str.find(",");
  }   
  return str;
}

#endif //__WATCOMC__
