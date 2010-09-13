// ������ �� ��������
// ����: 13.08.2010
// ������ ����: 10.07.2009

#ifndef irstimeH
#define irstimeH

#include <irsdefs.h>

#include <irsstring.h>
#include <timer.h>

#include <irsfinal.h>

namespace irs {

typedef irs_u32 millisecond_t;

const millisecond_t min_in_hour = 60;
const millisecond_t sec_in_min = 60;
const millisecond_t ms_in_sec = 1000;
const millisecond_t ms_in_min = ms_in_sec*sec_in_min;
const millisecond_t ms_in_hour = ms_in_min*min_in_hour;

// �������������� ����� � �� � ������ � �������: ��:��:��.���
irs::string ms_to_strtime(millisecond_t ms, bool show_ms = false);
// �������� ��� � ������ � �������: ��:��:��.���
irs::string cnt_to_strtime(counter_t cnt, bool show_ms = false);

millisecond_t system_time();
// ������ � ����� �������� �������
IRS_STREAMSPECDECL ostream &stime(ostream &a_strm);
//���������� ������� ���� � ������� ��� ��� � ��������� ��������� ����������
//���������� ������� � ������
irs_string_t file_name_time(irs_string_t a_extension = irs_string_t());

} //namespace irs

#endif //irstimeH
