#include "irsi2c.h"
#include "plib035_gpio.h"
#include "plib035_rcu.h"
#include <armioregs.h>

#include <irsfinal.h>



irs::arm::arm_i2c_t::arm_i2c_t()
{
  RCU_AHBClkCmd(RCU_AHBClk_GPIOA, ENABLE);
  RCU_AHBRstCmd(RCU_AHBRst_GPIOA, ENABLE);
  GPIOA->ALTFUNCSET |= GPIO_ALTFUNCSET_PIN0_Msk | GPIO_ALTFUNCSET_PIN1_Msk;
  GPIOA->PULLMODE |= GPIO_PULLMODE_PIN0_PU << GPIO_PULLMODE_PIN0_Pos;
  GPIOA->PULLMODE |= GPIO_PULLMODE_PIN1_PU << GPIO_PULLMODE_PIN1_Pos;
  GPIOA->OUTMODE |= GPIO_OUTMODE_PIN0_OD << GPIO_OUTMODE_PIN0_Pos;
  GPIOA->OUTMODE |= GPIO_OUTMODE_PIN1_OD << GPIO_OUTMODE_PIN1_Pos;
  GPIOA->DENSET |= GPIO_DENSET_PIN0_Msk | GPIO_DENSET_PIN1_Msk;
  RCU_APBRstCmd(RCU_APBRst_I2C, ENABLE);
  RCU_APBClkCmd(RCU_APBClk_I2C, ENABLE);
  I2C_FSFreqConfig(400000, RCU_GetSysClkFreq());
  I2C_Cmd(ENABLE);
}

irs::arm::arm_i2c_t::~arm_i2c_t()
{
  RCU_APBRstCmd(RCU_APBRst_I2C, DISABLE);
  RCU_APBClkCmd(RCU_APBClk_I2C, DISABLE);
  GPIOA->ALTFUNCCLR |= GPIO_ALTFUNCCLR_PIN0_Msk | GPIO_ALTFUNCCLR_PIN1_Msk;
  GPIOA->PULLMODE &= ~GPIO_PULLMODE_PIN0_Msk;
  GPIOA->PULLMODE &= ~GPIO_PULLMODE_PIN1_Msk;
  GPIOA->OUTMODE &= ~GPIO_OUTMODE_PIN0_Msk;
  GPIOA->OUTMODE &= ~GPIO_OUTMODE_PIN1_Msk;
  GPIOA->DENCLR |= GPIO_DENCLR_PIN0_Msk | GPIO_DENCLR_PIN1_Msk;
  I2C_Cmd(DISABLE);
}

void irs::arm::arm_i2c_t::lock()
{
  m_lock = true;
}

void irs::arm::arm_i2c_t::unlock()
{
  m_lock = false;
}

bool irs::arm::arm_i2c_t::get_lock()
{
  return m_lock;
}

void irs::arm::arm_i2c_t::set_device_addr(irs_u16 a_device_addr)
{
  m_device_address = a_device_addr;
}

void irs::arm::arm_i2c_t::read(uint8_t *ap_buf, irs_u16 a_size)
{
  this->lock();
  m_status = BUSY_READ;
  I2C_Cmd(ENABLE);
  I2C_StartCmd();
  irs_u16 temp_variable = 0; //used for ap_buf offset

  while (a_size)
  {
    while (!(I2C->ST_bit.INT)) { asm ("nop"); }

    switch (I2C->ST & I2C_ST_MODE_Msk) {
      case I2C_ST_MODE_STDONE: { // start condition generated
      I2C_SetData (m_device_address | 0x01);
      if (a_size == 1) { I2C_NACKCmd(); }
      I2C_ITStatusClear();
      } break;

      case I2C_ST_MODE_MRADPA:
      case I2C_ST_MODE_MRDAPA: {
      *(ap_buf+temp_variable) = I2C_GetData();
      temp_variable++;
      a_size--;
      if (a_size == 1) { I2C_NACKCmd(); }
      I2C_ITStatusClear();
      } break;

      case I2C_ST_MODE_MRDANA: {
      *(ap_buf+temp_variable) = I2C_GetData();
//      temp_variable++;
      a_size--;
      I2C_ITStatusClear();
      } break;

      default:
      for (;;){ asm ("nop"); }
      break;
    }
  }
  for (int i=0; i<500; i++) { asm ("nop"); } // bus free time. 407 is min for 400kHz
                            // for more information read datasheet of i2c slave
  I2C_StopCmd();
  I2C_Cmd(DISABLE);
  this->unlock();
  m_status = READY;
}

void irs::arm::arm_i2c_t::write(uint8_t *ap_buf, irs_u16 a_size)
{
  this->lock();
  m_status = BUSY_WRITE;
  I2C_Cmd(ENABLE);
  I2C_StartCmd();

  irs_u16 temp_variable = 0;
  while (a_size)
  {
    while (!(I2C->ST_bit.INT)) { asm ("nop"); }

    switch (I2C->ST & I2C_ST_MODE_Msk) {

      case I2C_ST_MODE_STDONE: { // start condition generated
      I2C_SetData (m_device_address);
      I2C_ITStatusClear();
      }  break;

      case I2C_ST_MODE_MTADNA:  { // //mt slave ad sent NACK
      I2C_StopCmd();
      I2C_SetData (m_device_address);
      I2C_StartCmd();
      I2C_ITStatusClear();
      }  break;

      case I2C_ST_MODE_MTADPA:
      case I2C_ST_MODE_MTDAPA:  {
      I2C_SetData (reinterpret_cast <irs_u32&>(*(ap_buf+temp_variable)));
      I2C_ITStatusClear();
      a_size--;
      temp_variable++;
      }  break;
      default: {
      for (;;){ asm ("nop"); }
      }  break;
    }
  }
  for (int i=0; i<500; i++) { asm ("nop"); } // bus free time. 407 is min for 400kHz
                            // for more information read datasheet of i2c slave
  I2C_StopCmd();
  I2C_Cmd(DISABLE);
  this->unlock();
  m_status = READY;
}

irs::arm::arm_i2c_t::status_t irs::arm::arm_i2c_t::get_status()
{
  return m_status;
}

void irs::arm::arm_i2c_t::tick()
{
  //check i2c bus for pending conditions
}