// ���������
// ������ ��������
// Watcom DOS
// ���������� ���������� ������� ����������
// ������ 0.3
#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>

// ��� ��������
//typedef irs_i32 counter_t;
typedef irs_i64 counter_t;
// ��� � ������� �������������� ������������� �������
typedef irs_i64 calccnt_t;

// ������������ ����� ������� ����� ��������
#define COUNTER_MAX IRS_I64_MAX/2
// ����� ������ � ���������
extern counter_t SECONDS_PER_INTERVAL;
//#define SECONDS_PER_INTERVAL 1
// ���������� �������� � ���������
extern counter_t COUNTER_PER_INTERVAL;
//#define COUNTER_PER_INTERVAL 3000000000

// ������������� ��������
void counter_init();
// ������ ��������
counter_t counter_get();
// ��������������� ��������
void counter_deinit();

#endif //COUNTERH
