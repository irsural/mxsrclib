// ����� ���稪�
// Watcom DOS
// �ᯮ��㥬 ���஥��� ���稪 ������
// ����� 0.2
#include <time.h>
#include <counter.h>

// �६� �����஢�� � ⨪��
const clock_t time_int_calibr = 9;
// �६� �����஢��, �. ���⮨� �� �᫨⥫� � �������⥫�.
// ��᫨⥫�
const calccnt_t time_calibr_num = 1;
// �������⥫�
const calccnt_t time_calibr_denom = 10;

// ��᫮ ᥪ㭤 � ���ࢠ��
counter_t SECONDS_PER_INTERVAL = 1;
// ������⢮ ����⮢ � ᥪ㭤�
counter_t COUNTER_PER_INTERVAL = 300000000;

static irs_u32 count_init = 0;

// �⥭�� ���஥����� ���稪� ������
extern counter_t get_rdtsc(void);
#pragma aux get_rdtsc = \
        ".586"          \
        "db 0Fh, 31h"   \
        "cld"           \
        "nop"           \
        "nop"           \
        "nop"           \
        "nop"           \
        modify [eax edx];
// ���樠������ ���稪�
void counter_init()
{
  if (!count_init) {
    clock_t time_int_calibr = time_calibr_num*CLOCKS_PER_SEC/time_calibr_denom;
    clock_t start_clock = clock();
    clock_t cur_clock = clock();
    while (cur_clock != start_clock) {
      cur_clock = clock();
    }
    start_clock = cur_clock;
    counter_t start_counter = counter_get();
    while ((cur_clock - start_clock) < time_int_calibr) {
      cur_clock = clock();
    }
    counter_t stop_counter = counter_get();
    COUNTER_PER_INTERVAL = calccnt_t(SECONDS_PER_INTERVAL)*
      calccnt_t(CLOCKS_PER_SEC)*calccnt_t(stop_counter - start_counter)/
      calccnt_t(time_int_calibr);
  }
  count_init++;
}
// �⥭�� ���稪�
counter_t counter_get()
{
	counter_t cnt = get_rdtsc();
	return cnt;
}
// �����樠������ ���稪�
void counter_deinit()
{
  count_init--;
  if (!count_init) {
  }
}
