// ������ ��������
// ���� 17.05.2010
// ������ ���� 17.05.2010

#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>
#include <irslimits.h>

// ��� ��������� ��������
typedef irs_i64 basic_counter_t;
// ��� ��������
typedef irs::type_relative_t<basic_counter_t>::signed_type counter_t;
// ��� � ������� �������������� ������������� �������
typedef irs::type_relative_t<basic_counter_t>::larger_type calccnt_t;

// ������������ ����� ������� ����� ��������
#define COUNTER_MAX IRS_I64_MAX
// ����� ������ � ���������
extern counter_t SECONDS_PER_INTERVAL;
// ���������� �������� � ���������
extern counter_t COUNTER_PER_INTERVAL;

// ������������� ��������
void counter_init();
// ������ ��������
counter_t counter_get();
// ��������������� ��������
void counter_deinit();

#endif //COUNTERH
