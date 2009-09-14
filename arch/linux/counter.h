// ������ ��������
// Linux
// ���� 13.09.2009

#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>
#include <time.h>
#include <irslimits.h>

// ��� ��������� ��������
typedef clock_t basic_counter_t;
// ��� ��������
//typedef irs_i32 counter_t;
typedef irs::type_relative_t<basic_counter_t>::signed_type counter_t;
// ��� � ������� �������������� ������������� �������
//typedef irs_i64 calccnt_t;
typedef irs::type_relative_t<basic_counter_t>::larger_type calccnt_t;

// ������������ ����� ������� ����� ��������
extern counter_t COUNTER_MAX;
// ����� ������ � ���������
extern counter_t SECONDS_PER_INTERVAL;
// ���������� �������� � �������
extern counter_t COUNTER_PER_INTERVAL;

// ������������� ��������
void counter_init();
// ������ ��������
counter_t counter_get();
// ��������������� ��������
void counter_deinit();

#endif //COUNTERH
