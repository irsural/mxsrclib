// ������ ��������
// ����: 1.02.2008

#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>
#include <time.h>

// ��� ��������
typedef clock_t counter_t;
// ��� � ������� �������������� ������������� �������
typedef irs_i32 calccnt_t;

// ������������ ����� ������� ����� ��������
//#define COUNTER_MAX IRS_I32_MAX
extern counter_t COUNTER_MAX;
// ����� ������ � ���������
//extern inttime_t SECONDS_PER_INTERVAL;
//#define SECONDS_PER_INTERVAL 1
// ���������� �������� � ���������
//extern counter_t COUNTER_PER_INTERVAL;
//#define COUNTER_PER_INTERVAL CLOCKS_PER_SEC

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
