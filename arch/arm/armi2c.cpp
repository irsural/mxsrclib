#include "armi2c.h"

#include <armioregs.h>
#include <irsfinal.h>

#if defined (IRS_NIIET_1921)
#include "plib035_gpio.h"
#include "plib035_rcu.h"
#endif

//class arm_i2c_t
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

irs_u16 irs::arm::arm_i2c_t::get_device_addr()
{
  return m_device_address;
}

void irs::arm::arm_i2c_t::set_status(status_t a_status)
{
  m_status = a_status;
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
      asm ("nop");
      break;
    }
  }
  for (int i=0; i<500; i++) {asm ("nop");} // bus free time parameter. i<407 is min for 400kHz
                                    // for more information read datasheet of i2c slave
  I2C_StopCmd();
  I2C_ITStatusClear();

  for (int i=0; i<50; i++) {asm ("nop");}

  I2C_Cmd(DISABLE);
  this->unlock();
  m_status = READY;
}

void irs::arm::arm_i2c_t::write(irs_u8 *ap_buf, irs_u16 a_size)
{
  this->lock();
  m_status = BUSY_WRITE;
  I2C_Cmd(ENABLE);
  I2C_StartCmd();

  irs_u16 temp_variable = 0;
  while (a_size)
  {
    switch (I2C->ST & I2C_ST_MODE_Msk) {
      case I2C_ST_MODE_STDONE: {
      I2C_SetData (m_device_address);
      I2C_ITStatusClear();
      }  break;

      case I2C_ST_MODE_MTADNA: {
      I2C_StopCmd();
      I2C_SetData (m_device_address);
      I2C_StartCmd();
      I2C_ITStatusClear();
      }  break;

      case I2C_ST_MODE_MTADPA:
      case I2C_ST_MODE_MTDAPA: {
      I2C_SetData (reinterpret_cast <irs_u32&>(*(ap_buf+temp_variable)));
      I2C_ITStatusClear();
      a_size--;
      temp_variable++;
      }  break;
      default: {
      asm ("nop");
      }  break;
    }
  }
  for (int i=0; i<500; i++) { asm ("nop"); } // bus free time parameter. i<407 is min for 400kHz
                                    // for more information read datasheet of i2c slave
  I2C_StopCmd();
  I2C_ITStatusClear();

  for (int i=0; i<50; i++) { asm ("nop"); }

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
// end of class arm_i2c_t


//class port_extender_i2c_t
irs::arm::port_extender_i2c_t::port_extender_i2c_t(arm_i2c_t* ap_arm_i2c_t)
{
  mp_arm_i2c_t = ap_arm_i2c_t;
//  mp_arm_i2c_t->set_device_addr(0xE8);
}

irs::arm::port_extender_i2c_t::~port_extender_i2c_t()
{

}

irs_u8 irs::arm::port_extender_i2c_t::read_register(irs_u8 a_reg)
{
  irs_u8 a_size = 1;
  mp_arm_i2c_t->lock();
  mp_arm_i2c_t->set_device_addr(0xE8);
  mp_arm_i2c_t->set_status(mp_arm_i2c_t->BUSY_READ);
  I2C_Cmd(ENABLE);
  I2C_StartCmd();
  irs_u16 temp_variable = 0;

  irs_u8* read_buf = new irs_u8;

  while (a_size)
  {
    switch (I2C->ST & I2C_ST_MODE_Msk) {
      case I2C_ST_MODE_STDONE: { // start condition generated
      I2C_SetData (mp_arm_i2c_t->get_device_addr());
      I2C_ITStatusClear();
      } break;

      case I2C_ST_MODE_MTADPA: {
      I2C_SetData (a_reg); //a_reg
      I2C_ITStatusClear();
      I2C_StartCmd();
      I2C_ITStatusClear();
      break;
      }

      case I2C_ST_MODE_RSDONE: {
      I2C_SetData (mp_arm_i2c_t->get_device_addr() | 0x01);
      I2C_ITStatusClear();
      } break;

      case I2C_ST_MODE_MTDAPA: {
      I2C_ITStatusClear();
      } break;

      case I2C_ST_MODE_MRADPA:
      a_size == 1 ? (I2C_NACKCmd(),1) : 1;
      I2C_ITStatusClear();
      break;

      case I2C_ST_MODE_MRDAPA:  {
      *(read_buf+temp_variable) = I2C_GetData();
      temp_variable++;
      a_size--;
      a_size == 1 ? (I2C_NACKCmd(),1) : 1;
      I2C_ITStatusClear();
      }  break;

      case I2C_ST_MODE_MRDANA: {
      *(read_buf+temp_variable) = I2C_GetData();
      a_size--;
      I2C_ITStatusClear();
      } break;
      default:
      asm("nop");
      break;
    }
  }
  for (int i=0; i<500; i++) { asm ("nop"); } // bus free time. 407 is min for 400kHz
                            // for more information read datasheet of i2c slave
  I2C_StopCmd();
  I2C_ITStatusClear();
  for (int i=0; i<50; i++) { asm ("nop"); } // bus free time. 407 is min for 400kHz
                            // for more information read datasheet of i2c slave
  I2C_Cmd(DISABLE);

  mp_arm_i2c_t->unlock();
  mp_arm_i2c_t->set_status(mp_arm_i2c_t->READY);

  irs_u8 return_value = *read_buf;
  delete read_buf;

  return return_value;
}

irs_u16 irs::arm::port_extender_i2c_t::read_registers_group(irs_u8 a_first_reg)
{
  irs_u8 a_size = 2;
  mp_arm_i2c_t->lock();
  mp_arm_i2c_t->set_device_addr(0xE8);
  mp_arm_i2c_t->set_status(mp_arm_i2c_t->BUSY_READ);
  I2C_Cmd(ENABLE);
  I2C_StartCmd();

  irs_u8* read_buf = new irs_u8 [2];

  while (a_size)
  {
    switch (I2C->ST & I2C_ST_MODE_Msk) {
      case I2C_ST_MODE_STDONE: { // start condition generated
      I2C_SetData (mp_arm_i2c_t->get_device_addr());
      I2C_ITStatusClear();
      } break;

      case I2C_ST_MODE_MTADPA: {
      I2C_SetData (a_first_reg); //a_reg
      I2C_ITStatusClear();
      I2C_StartCmd();
      I2C_ITStatusClear();
      } break;


      case I2C_ST_MODE_RSDONE: {
      I2C_SetData (mp_arm_i2c_t->get_device_addr() | 0x01);
      I2C_ITStatusClear();
      } break;

      case I2C_ST_MODE_MTDAPA: {
      I2C_ITStatusClear();
      } break;

      case I2C_ST_MODE_MRADPA: {
      a_size == 1 ? (I2C_NACKCmd(),1) : 1;
      I2C_ITStatusClear();
      } break;

      case I2C_ST_MODE_MRDAPA: {
      *(read_buf+a_size-1) = I2C_GetData();
      a_size--;
      a_size == 1 ? (I2C_NACKCmd(),1) : 1;
      I2C_ITStatusClear();
      }  break;

      case I2C_ST_MODE_MRDANA: {
      *(read_buf+a_size-1) = I2C_GetData();
      a_size--;
      } break;
      default:
      asm ("nop");
      break;
    }
  }
  for (int i=0; i<500; i++) { asm ("nop"); } // bus free time. 407 is min for 400kHz
                            // for more information read datasheet of i2c slave
  I2C_StopCmd();
  I2C_ITStatusClear();
  for (int i=0; i<50; i++) { asm ("nop"); } // bus free time. 407 is min for 400kHz
                            // for more information read datasheet of i2c slave
  I2C_Cmd(DISABLE);

  mp_arm_i2c_t->unlock();
  mp_arm_i2c_t->set_status(mp_arm_i2c_t->READY);

  irs_u16 return_value = read_buf[0] << 8;
  return_value |= read_buf[1];
  delete [] read_buf;
  return return_value;
}

void irs::arm::port_extender_i2c_t::write_to_register(irs_u8 a_reg, irs_u8 a_data)
{
  irs_u8* write_buf = new irs_u8[2];
  write_buf[0] = a_reg;
  write_buf[1] = a_data;
  if(!(mp_arm_i2c_t->get_lock())) {
    //mp_arm_i2c_t->lock();
    //mp_arm_i2c_t->set_device_addr(0xE8);
  }
  mp_arm_i2c_t->write(write_buf,2);
    //mp_arm_i2c_t->unlock();
  delete[] write_buf;
}

void irs::arm::port_extender_i2c_t::write_to_registers_group(irs_u8 a_reg, irs_u16 a_data)
{
  irs_u8* write_buf = new irs_u8[3];
  write_buf[0] = a_reg;
  write_buf[1] = (a_data & 0x00FF);
  write_buf[2] = (a_data & 0xFF00) >> 8;
  if(!(mp_arm_i2c_t->get_lock())) {
    //mp_arm_i2c_t->lock();
    //mp_arm_i2c_t->set_device_addr(0xE8);
  }
  mp_arm_i2c_t->write(write_buf,3);
  delete[] write_buf;
}

void irs::arm::port_extender_i2c_t::set_logic_level_data
  (irs_u16 a_logic_level_data)
{
  m_logic_level_data = a_logic_level_data;
}

irs_u16 irs::arm::port_extender_i2c_t::get_logic_level_data()
{
  return m_logic_level_data;
}

void irs::arm::port_extender_i2c_t::set_input_output_direction
  (irs_u16 a_input_output_direction)
{
  m_input_output_direction = a_input_output_direction;
}
irs_u16 irs::arm::port_extender_i2c_t::get_input_output_direction()
{
  return m_input_output_direction;
}
//end of class port_extender_i2c_t


//class eeprom_i2c_t
irs::arm::eeprom_i2c_t::eeprom_i2c_t(arm_i2c_t* ap_arm_i2c_t)
{
  mp_arm_i2c_t = ap_arm_i2c_t;
}

irs::arm::eeprom_i2c_t::~eeprom_i2c_t()
{

}

void irs::arm::eeprom_i2c_t::read_byte(irs_u16 a_address, irs_u8* ap_buf)
{
  if(!(mp_arm_i2c_t->get_lock())) {
    mp_arm_i2c_t->lock();
    mp_arm_i2c_t->set_device_addr(0xA0);
    mp_arm_i2c_t->set_status(BUSY_READ);

    I2C_Cmd(ENABLE);
    I2C_StartCmd();
    irs_u8 temp_variable = 1; //num of bytes to read

    while (temp_variable) {
      switch (I2C->ST & I2C_ST_MODE_Msk) {

        case I2C_ST_MODE_STDONE: {
        I2C_SetData (mp_arm_i2c_t->get_device_addr() | 0x00);
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_MTADPA: {
        I2C_SetData ((a_address & 0xFF00) >> 8);
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_MTDAPA: {
        I2C_SetData (a_address & 0xFF);
        I2C_ITStatusClear();
        I2C_StartCmd();
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_RSDONE: {
        I2C_SetData (mp_arm_i2c_t->get_device_addr() | 0x01);
        I2C_ITStatusClear();
        } break;

      case I2C_ST_MODE_MRADPA: {
          if (temp_variable == 1) { I2C_NACKCmd();}
          I2C_ITStatusClear();
      }
        break;
        case I2C_ST_MODE_MRDANA: {
        temp_variable--;
        *(ap_buf) = I2C_GetData();
        I2C_ITStatusClear();
        } break;

        default:
        asm ("nop");
        break;
      }
    }
    I2C_StopCmd();
    I2C_ITStatusClear();
    for (int i=0; i<50; i++) { asm ("nop"); } // bus free time. 407 is min for 400kHz
                            // for more information read datasheet of i2c slave
    I2C_Cmd(DISABLE);

    mp_arm_i2c_t->set_status(mp_arm_i2c_t ->READY);
    mp_arm_i2c_t->unlock();
  }
}

void irs::arm::eeprom_i2c_t::read_page(irs_u16 a_address, irs_u8* ap_buf, irs_u16 a_size)
{
  if(!(mp_arm_i2c_t->get_lock())) {
    mp_arm_i2c_t->lock();
    mp_arm_i2c_t->set_device_addr(0xA0);
    mp_arm_i2c_t->set_status(BUSY_READ);

    I2C_Cmd(ENABLE);
    I2C_StartCmd();
    irs_u8 bytes_to_read = a_size; //num of bytes to read
    while (bytes_to_read) {
      switch (I2C->ST & I2C_ST_MODE_Msk) {

        case I2C_ST_MODE_STDONE: {
        I2C_SetData (mp_arm_i2c_t->get_device_addr() | 0x00);
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_MTADPA: {
        I2C_SetData ((a_address & 0xFF00) >> 8);
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_MTDAPA: {
        I2C_SetData (a_address & 0xFF);
        I2C_ITStatusClear();
        I2C_StartCmd();
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_RSDONE: {
        I2C_SetData (mp_arm_i2c_t->get_device_addr() | 0x01);
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_MRADPA: {
        if (bytes_to_read == 1) { I2C_NACKCmd();}
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_MRDAPA: {
        *(ap_buf+(a_size - bytes_to_read)) = I2C_GetData();
        bytes_to_read--;
        if (bytes_to_read == 1) { I2C_NACKCmd();}
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_MRDANA: {
        *(ap_buf+(a_size - bytes_to_read)) = I2C_GetData();
        bytes_to_read--;
        } break;

        default:
        asm ("nop");
        break;
      }
    }
//     while (!(I2C->ST_bit.INT)) {asm ("nop");} // dont delete, need for transmit last byte to eeprom
    I2C_StopCmd();
    I2C_ITStatusClear();
    for (int i=0; i<50; i++) { asm ("nop"); } // bus free time. 407 is min for 400kHz
                            // for more information read datasheet of i2c slave
    I2C_Cmd(DISABLE);

    mp_arm_i2c_t->set_status(mp_arm_i2c_t ->READY);
    mp_arm_i2c_t->unlock();
  }
}

void irs::arm::eeprom_i2c_t::write_byte(irs_u16 a_address, irs_u8* ap_buf)
{
  if(!(mp_arm_i2c_t->get_lock())) {
    mp_arm_i2c_t->lock();
    mp_arm_i2c_t->set_device_addr(0xA0);
    mp_arm_i2c_t->set_status(BUSY_WRITE);

    I2C_Cmd(ENABLE);
    I2C_StartCmd();
    irs_u8 bytes_to_write = 1;
    bytes_to_write++;
    while (bytes_to_write) {
      switch (I2C->ST & I2C_ST_MODE_Msk) {

        case I2C_ST_MODE_STDONE: {
        I2C_SetData (mp_arm_i2c_t->get_device_addr());
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_MTADPA: {
        I2C_SetData ((a_address & 0xFF00) >> 8);
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_MTDAPA: {
        if (bytes_to_write == 2) {
          I2C_SetData (a_address & 0xFF);
          I2C_ITStatusClear();
        } else {
          I2C_SetData((uint32_t)*ap_buf);
          I2C_ITStatusClear();
        }
        bytes_to_write--;
        } break;
        default:
        asm ("nop");
        break;
      }
    }
    while (!(I2C->ST_bit.INT)) {asm ("nop");} // dont delete, need for transmit last byte to eeprom
    I2C_StopCmd();
    I2C_ITStatusClear();
    for (int i=0; i<50; i++) {asm ("nop");}

    I2C_Cmd(DISABLE);

    mp_arm_i2c_t->set_status(mp_arm_i2c_t ->READY);
    mp_arm_i2c_t->unlock();
  }

}

void irs::arm::eeprom_i2c_t::write_page(irs_u16 a_address, irs_u8* ap_buf, irs_u16 a_size)
{
  if(!(mp_arm_i2c_t->get_lock())) {
    mp_arm_i2c_t->lock();
    mp_arm_i2c_t->set_device_addr(0xA0);
    mp_arm_i2c_t->set_status(BUSY_WRITE);

    I2C_Cmd(ENABLE);
    I2C_StartCmd();
    irs_u8 bytes_to_write = a_size;
    bytes_to_write++;
    while (bytes_to_write) {
      switch (I2C->ST & I2C_ST_MODE_Msk) {

        case I2C_ST_MODE_STDONE: {
        I2C_SetData (mp_arm_i2c_t->get_device_addr());
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_MTADPA: {
        I2C_SetData ((a_address & 0xFF00) >> 8);
        I2C_ITStatusClear();
        } break;

        case I2C_ST_MODE_MTDAPA: {
        if (bytes_to_write == a_size + 1) {
          I2C_SetData (a_address & 0xFF);
          I2C_ITStatusClear();
        } else {
          I2C_SetData((uint32_t)*(ap_buf+a_size-bytes_to_write));
          I2C_ITStatusClear();
        }
        bytes_to_write--;
        } break;
        default:
        asm ("nop");
        break;
      }
    }
    while (!(I2C->ST_bit.INT)) {asm ("nop");} // dont delete, need for transmit last byte to eeprom
    I2C_StopCmd();
    I2C_ITStatusClear();
    for (int i=0; i<50; i++) {asm ("nop");}
    I2C_Cmd(DISABLE);

    mp_arm_i2c_t->set_status(mp_arm_i2c_t ->READY);
    mp_arm_i2c_t->unlock();
  }
}

void irs::arm::eeprom_i2c_t::set_page_size(irs_u8 a_page_size)
{
  m_page_size = a_page_size;
}

irs_u8 irs::arm::eeprom_i2c_t::get_page_size(void)
{
  return this->m_page_size;
}
//end of class eeprom_i2c_t