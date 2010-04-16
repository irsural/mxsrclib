// ������ ��������
// ����: 14.04.2010
// ������ ����: 18.08.2009

#include <irsdefs.h>

#include <ioavr.h>
#include <inavr.h>

#include <irsconfig.h>
#include <counter.h>
#include <irsarchint.h>
#include <irscpu.h>
#include <irserror.h>

#include <irsfinal.h>

irs_i16 HW_TCNT = 0;
irs_u8 init_cnt = 0;

// ���������� �������� � ���������
extern counter_t COUNTER_PER_INTERVAL = 0x10000;
// ����� ������ � ���������
extern counter_t SECONDS_PER_INTERVAL =
  COUNTER_PER_INTERVAL/(irs::cpu_traits_t::frequency()/1024);

class timer_overflow_event_t: public mxfact_event_t
{
public:
  virtual void exec()
  {
    mxfact_event_t::exec();
    irs_u8 sreg = SREG;
    HW_TCNT++;
    SREG = sreg;
  }
};

// ������������� ��������
void counter_init()
{
  init_cnt++;
  if (init_cnt == 1) {

    static timer_overflow_event_t timer_overflow_event;

    #ifdef __ATmega128__
    //������������� Timer3
    TCCR3A = 0x00;        //  ���������� ����� ����� �� 0 �� 0xFF
    TCCR3B = 0x05;        //  ��������� �� 1024
    TCCR3C = 0x00;        //  Force Output Compare - �����
    TCNT3 = 0x0000;       //  ����� �������� �� 0
    OCR3A = 0xFFFF;       //  �������� ���������
    OCR3B = 0xFFFF;
    OCR3C = 0xFFFF;
    ETIMSK_TOIE3 = 1;
    irs_avr_timer3_ovf_int.add(&timer_overflow_event);
    #else //__ATmega128__
    //������������� Timer5
    TCCR5A = 0x00;        //  ���������� ����� ����� �� 0 �� 0xFF
    TCCR5B = 0x05;        //  ��������� �� 1024
    TCCR5C = 0x00;        //  Force Output Compare - �����
    TCNT5 = 0x0000;       //  ����� �������� �� 0
    OCR5A = 0xFFFF;       //  �������� ���������
    OCR5B = 0xFFFF;
    OCR5C = 0xFFFF;
    TIMSK5 = 1<<TOIE5;
    irs_avr_timer5_ovf_int.add(&timer_overflow_event);
    #endif //__ATmega128__
  }
}

// ������ ��������
counter_t counter_get()
{
  irs_u8 sreg=SREG;
  irs_i16 Temp_TCNT;
  irs_i32 Out;
  __disable_interrupt();
  #ifdef __ATmega128__
  Temp_TCNT=TCNT3;
  if(ETIFR&(1<<TOV3))
  {
    Temp_TCNT=TCNT3;
    HW_TCNT++;
    ETIFR&=(1<<TOV3);
  }
  IRS_LOWORD(Out)=Temp_TCNT;
  IRS_HIWORD(Out)=HW_TCNT;
  #else //__ATmega128__
  Temp_TCNT=TCNT5;
  if(TIFR5&(1<<TOV5))
  {
    Temp_TCNT=TCNT5;
    HW_TCNT++;
    TIFR5&=(1<<TOV5);
  }
  #endif //__ATmega128__
  IRS_LOWORD(Out)=Temp_TCNT;
  IRS_HIWORD(Out)=HW_TCNT;
  SREG=sreg;
  return Out;    //  ����������� ���������� ������
}

// ��������������� ��������
void counter_deinit()
{
  init_cnt--;
  if (init_cnt == 0) {
    #ifdef __ATmega128__
    //��������������� Timer3
    ETIMSK_TOIE3 = 0;
    TCCR3B = 0x00;        //  ��������� ��������
    #else //__ATmega128__
    //��������������� Timer5
    TIMSK5 = 0;
    TCCR5B = 0x00;        //  ��������� ��������
    #endif //__ATmega128__
  }
}
