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
  I2C_FSFreqConfig(100000, RCU_GetSysClkFreq());
  I2C_Cmd(ENABLE);
}

irs::arm::arm_i2c_t::~arm_i2c_t()
{
  RCU_APBRstCmd(RCU_APBRst_I2C, DISABLE);
  RCU_APBClkCmd(RCU_APBClk_I2C, DISABLE);
//  GPIOA->ALTFUNCSET |= GPIO_ALTFUNCSET_PIN0_Msk | GPIO_ALTFUNCSET_PIN1_Msk;
//  GPIOA->PULLMODE |= GPIO_PULLMODE_PIN0_PU << GPIO_PULLMODE_PIN0_Pos;
//  GPIOA->PULLMODE |= GPIO_PULLMODE_PIN1_PU << GPIO_PULLMODE_PIN1_Pos;
//  GPIOA->OUTMODE |= GPIO_OUTMODE_PIN0_OD << GPIO_OUTMODE_PIN0_Pos;
//  GPIOA->OUTMODE |= GPIO_OUTMODE_PIN1_OD << GPIO_OUTMODE_PIN1_Pos;
//  GPIOA->DENSET |= GPIO_DENSET_PIN0_Msk | GPIO_DENSET_PIN1_Msk;
  NVIC_DisableIRQ(I2C_IRQn);
}

void irs::arm::arm_i2c_t::i2c_init()
{

//  while (I2C_BusBusyStatus()) asm ("nop");
//
//  if(I2C->CST_bit.TSDA)
//    I2C->CST |= I2C_CST_TGSCL_Msk;



//  for (int i=0; i<100000;i++) asm ("nop");

//  do {
//  RCU_APBRstCmd(RCU_APBRst_I2C, DISABLE);
//  RCU_APBClkCmd(RCU_APBClk_I2C, DISABLE);
//  I2C_Cmd(DISABLE);
//
//  RCU_APBRstCmd(RCU_APBRst_I2C, ENABLE);
//  RCU_APBClkCmd(RCU_APBClk_I2C, ENABLE);
//  I2C_FSFreqConfig(50000, RCU_GetSysClkFreq());
//  I2C_Cmd(ENABLE);
//
//  while (I2C_BusBusyStatus()) asm ("nop");
//
//  if(I2C->CST_bit.TSDA)
//    I2C->CST |= I2C_CST_TGSCL_Msk;
//
//  I2C_StartCmd();
//
//  for (int i=0; i<100000;i++) {asm ("nop");}
//  } while (I2C_GetState() != I2C_ST_MODE_STDONE);
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

void irs::arm::arm_i2c_t::read(irs_u16 a_mem_addr, uint8_t *ap_buf, irs_u16 a_size)
{
  int mode = 1;
  int msg = 0;


  this->lock();
  m_status = BUSY_READ;
  this->i2c_init();
  //start counter of bus error generation
   I2C_StartCmd();
  while (a_size)
  {
    while (!(I2C->ST_bit.INT)) { asm ("nop"); }

    uint32_t i2c_state = I2C->ST & I2C_ST_MODE_Msk;
    switch (i2c_state) {
      case I2C_ST_MODE_STDONE: { // start condition generated
         switch (this->get_status()){
          case BUSY_READ: {
          I2C_SetData (m_device_address);
          I2C_ITStatusClear();
          } break;
          case BUSY_WRITE: {
          I2C_SetData (m_device_address);
          I2C_ITStatusClear();
          } break;
        }
      }
      break;






    case I2C_ST_MODE_MTADNA: // //mt slave ad sent NACK
       I2C_StopCmd();
       I2C_SetData (m_device_address);
       I2C_StartCmd();
       I2C_ITStatusClear();
       m_device_address+=2;
      break;

      case 14 : break;// byte sent
    default:

      msg++;
    ((msg == 1) && (mode == 1)) ? (I2C_SetData (0x06),1) : 1;
    ((msg == 2) && (mode == 1)) ? (I2C_SetData (0x00),1) : 1;
    ((msg == 3) && (mode == 1)) ? (I2C_SetData (0x00), I2C_StartCmd(),
                                   I2C_SetData (m_device_address), msg = 0, mode++) : 1;
    ((msg == 1) && (mode == 2)) ? (I2C_SetData (0x02),1) : 1;
    ((msg == 2) && (mode == 2)) ? (I2C_SetData (0xFF),1) : 1;
    ((msg == 3) && (mode == 2)) ? (I2C_SetData (0x03),1) : 1;

    I2C_ITStatusClear();










      I2C_SetData (m_device_address);
      I2C_ITStatusClear();
      break;  //error

    }
  }
  this->unlock();
  m_status = READY;
}

void irs::arm::arm_i2c_t::write(irs_u16 a_mem_addr, uint8_t *ap_buf,
  irs_u16 a_size)
{
  this->lock();
  m_status = BUSY_WRITE;

  do {
    this->i2c_init();
       // rename to init of bus condition, not all interface
  } while (I2C->ST_bit.MODE != I2C_ST_MODE_STDONE) ;
  //start counter of bus error generation
while (a_size)
  {
    while (!I2C->ST_bit.INT) { asm ("nop"); }

    uint32_t i2c_state = I2C->ST & I2C_ST_MODE_Msk;
    switch (i2c_state) {
      case I2C_ST_MODE_STDONE: { // start condition generated
         switch (this->get_status()){
          case BUSY_READ: {
          I2C_SetData (m_device_address | 0x01);
          I2C_ITStatusClear();
          } break;
          case BUSY_WRITE: {
          I2C_SetData (m_device_address);
          I2C_ITStatusClear();
          } break;
        }
      }
      break;
    case I2C_ST_MODE_MTADPA: //master transmit Slave address sent, positive ACK

      break; //address sent

      case 14 : break;// byte sent
    default: break;  //error
    //  I2C_ST_MODE_MTADNA                         = 0x5UL,                /*!< FS master transmit - Slave address sent, negative ACK */

    }
  }




  //  w=0
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