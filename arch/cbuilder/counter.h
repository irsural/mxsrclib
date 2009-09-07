// ������ ��������
// ���������� ���������� ������� ����������
// ���� 24.10.2008

#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>

// ��� ��������
typedef irs_i64 counter_t;
// ��� � ������� �������������� ������������� �������
typedef irs_i64 calccnt_t;

// ������������ ����� ������� ����� ��������
#define COUNTER_MAX IRS_I64_MAX/2
// ����� ������ � ���������
extern counter_t SECONDS_PER_INTERVAL;
// ���������� �������� � ���������
extern counter_t COUNTER_PER_INTERVAL;

// ������������� ��������
void counter_init();
// ���������� ���������� ������������� ��������
void counter_refresh();
// ������ ��������
counter_t counter_get();
// ��������������� ��������
void counter_deinit();

#endif //COUNTERH

