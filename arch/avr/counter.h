// ������ ��������
// ���� 29.01.2008

#ifndef COUNTERH
#define COUNTERH

#include <irsdefs.h>

// ��� ��������
typedef irs_i32 counter_t;
// ��� � ������� �������������� ������������� �������
typedef irs_i64 calccnt_t;

// ������������ ����� ������� ����� ��������
#define COUNTER_MAX IRS_I32_MAX
// ����� ������ � ���������
//extern inttime_t SECONDS_PER_INTERVAL;
#define SECONDS_PER_INTERVAL 4 //=62500/(Fclk/1024) � ��������
// ���������� �������� � ���������
//extern counter_t COUNTER_PER_INTERVAL;
#define COUNTER_PER_INTERVAL 62500

// ������������� ��������
void counter_init();
// ������ ��������
counter_t counter_get();
// ��������������� ��������
void counter_deinit();

#endif //COUNTERH
