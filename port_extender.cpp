#include <port_extender.h>
#include <irserror.h>
#include <plib035.h>
#include <timer.h>

port_extender_pca9539_t::port_extender_pca9539_t(i2c_t* ap_i2c, irs_u8 a_i2c_addr, irs_u8 a_default_port_0, irs_u8 a_default_port_1) :
  m_status(in_initialize),
  mp_i2c(ap_i2c),
  m_i2c_addr(a_i2c_addr),
  m_buffer(),
  m_default_port_0(a_default_port_0),
  m_default_port_1(a_default_port_1),
  m_pin(0),
  m_cmd(cmd_none),
  m_port_0(0),
  m_port_1(0),
  mp_port(nullptr),
  mp_user_pin(nullptr)
{
  irs::loop_timer_t wait_reset(irs::make_cnt_us(5));

  RCU->HCLKCFG_bit.GPIOAEN = 1;
  RCU->HRSTCFG_bit.GPIOAEN = 1;

  RCU->HCLKCFG_bit.GPIOBEN = 1;
  RCU->HRSTCFG_bit.GPIOBEN = 1;

  SET_BIT(GPIOB->DENSET, GPIO_Pin_15);
  SET_BIT(GPIOB->OUTENSET, GPIO_Pin_15);

  SET_BIT(GPIOB->DATAOUTCLR, GPIO_Pin_15);
  wait_reset.start();
  while(!wait_reset.check());
  SET_BIT(GPIOB->DATAOUTSET, GPIO_Pin_15);
}

void port_extender_pca9539_t::write_pin(irs_u8 a_pin)
{
  initialize_io_operation(a_pin, in_write_pin, cmd_output_port_0);
}

void  port_extender_pca9539_t::read_pin(irs_u8 a_pin, irs_u8* a_user_pin)
{
  mp_user_pin = a_user_pin;
  initialize_io_operation(a_pin, in_read_pin, cmd_input_port_0);
}

void  port_extender_pca9539_t::clear_pin(irs_u8 a_pin)
{
  initialize_io_operation(a_pin, in_clear_pin, cmd_output_port_0);
}

void port_extender_pca9539_t::tick()
{
    mp_i2c->tick();

    if (mp_i2c->get_status() != irs_st_ready)
        return;

    switch(m_status) {
        case in_free: {
        } break;

        case in_initialize: {
          if(mp_i2c->get_lock())
            break;

          initialize_port_extender();

          m_status = in_wait;
        } break;

        case in_write_pin: {
          *mp_port |= (1 << m_pin);
          send_i2c();

          m_status = in_wait;
        } break;

        case in_read_pin: {
          if(*mp_port & (1 << m_pin))
          {
            *mp_user_pin = 1;
          }
          else
          {
            *mp_user_pin = 0;
          }

          m_status = in_wait;
        } break;

        case in_clear_pin: {
          *mp_port &= ~(1 << m_pin);
          send_i2c();

          m_status = in_wait;
        } break;

        case in_wait: {
          abort();
          m_status = in_free;
        }

        case in_wait_reset: {

          m_status = in_free;
        }
    }
}

void port_extender_pca9539_t::abort()
{
  m_status = in_free;
  for(int i = 0; i < buffer_size; i++)
    m_buffer[i] = 0;
  m_pin = 0;
  m_cmd = cmd_none;
  mp_port = nullptr;
  mp_user_pin = nullptr;
  mp_i2c->unlock();
}

void port_extender_pca9539_t::initialize_port_extender()
{
  mp_i2c->lock();
  mp_i2c->set_device_address(m_i2c_addr);
  m_buffer[0] = cmd_config_port_0;
  m_buffer[1] = m_default_port_0;
  m_buffer[2] = m_default_port_1;

  mp_i2c->write(m_buffer, 3);
}

void port_extender_pca9539_t::initialize_io_operation(irs_u8 a_pin, status_t a_status, command_t a_cmd)
{
  IRS_ASSERT(!mp_i2c->get_lock());
  IRS_ASSERT(m_status == in_free);

  mp_i2c->lock();
  mp_i2c->set_device_address(m_i2c_addr);
  m_pin = a_pin;
  m_status = a_status;

  if(m_pin >= port_size)
  {
    m_cmd = static_cast<command_t>(a_cmd + 1);
    mp_port = &m_port_1;
    m_pin -= port_size;
  }
  else
  {
    m_cmd = a_cmd;
    mp_port = &m_port_0;
  }
}

void port_extender_pca9539_t::send_i2c()
{
  mp_i2c->lock();

  m_buffer[0] = m_cmd;
  m_buffer[1] = *mp_port;

  mp_i2c->write(m_buffer, 2);
}

irs_status_t port_extender_pca9539_t::get_status()
{
  return (m_status == in_free) ? irs_st_ready : irs_st_busy;
}
