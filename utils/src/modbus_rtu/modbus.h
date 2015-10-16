#ifndef MODBUS_H
#define MODBUS_H

#include <stdlib.h>

class modbus_service_base_t
{
public:
  // �������� �������
  virtual long counter() = 0;
    // ����� ������ ����� ������� ����� �������� ��������� ��� ����� �����
    // � �������� modbus_service_base_t::counter()
  virtual long silence_time() = 0;
  virtual void switch_on_receive() = 0;
  virtual bool receive_ready() = 0;
  virtual bool receive_error() = 0;
  virtual unsigned char receive() = 0;
  virtual void switch_on_transmit() = 0;
  virtual bool transmit_ready() = 0;
  virtual void transmit(unsigned char a_byte) = 0;
};

class modbusc {
public:
	//typedef enum {ON, OFF} sw_t;
	modbusc(
    // ��������� �� ���������� MODBUS
    unsigned short* ap_vars,
    // ���������� ���������� MODBUS
    unsigned short a_vars_count,
    // ����� ��� ��������� � ��������� �������� � �������� ������� �
    // COM-����� (RS485)
    modbus_service_base_t* ap_service
   );
	~modbusc();
  void interrupt_handler();
private:
	unsigned char ic[1];
  unsigned char oc[1];
	unsigned short ir[5];
  unsigned short* out_regs;
  unsigned short m_vars_count;
  modbus_service_base_t* mp_service;
  long m_silence_time;
  bool m_timer_silence_checked;
  long m_silence_conter_target;

  void timer_silence_start();
  bool timer_silence_check();
};

#ifdef NOP
// ������ ���������� modbus_service_t ��� ATmega324P
class modbus_service_t: public modbus_service_base_t
{
public:
  modbus_service_t();
  // �������� �������. ������� ������ ��������������� 100 ���
  virtual long counter();
  virtual long silence_time();
  virtual void switch_on_receive();
  virtual bool receive_ready();
  virtual bool receive_error();
  virtual unsigned char receive();
  virtual void switch_on_transmit();
  virtual bool transmit_ready();
  virtual void transmit(unsigned char a_byte);
private:
  long m_baud_rate;
};
#endif //NOP

#endif  //MODBUS_H
