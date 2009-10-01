// ������ ��������
// ���� 01.10.2009
// ������ ���� 29.01.2008

#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>
#include <irslimits.h>

// ��� ��������� ��������
typedef irs_i32 basic_counter_t;
// ��� ��������
typedef irs::type_relative_t<basic_counter_t>::signed_type counter_t;
// ��� � ������� �������������� ������������� �������
typedef irs::type_relative_t<basic_counter_t>::larger_type calccnt_t;

// ������������ ����� ������� ����� ��������
#define COUNTER_MAX IRS_I32_MAX
// ����� ������ � ���������
extern counter_t SECONDS_PER_INTERVAL;
//#define SECONDS_PER_INTERVAL 4 //=62500/(Fclk/1024) � ��������
// ���������� �������� � ���������
extern counter_t COUNTER_PER_INTERVAL;
//#define COUNTER_PER_INTERVAL 62500

// ������������� ��������
void counter_init();
// ������ ��������
counter_t counter_get();
// ��������������� ��������
void counter_deinit();

#endif //COUNTERH
