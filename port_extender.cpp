#include <port_extender.h>
#include <irserror.h>
#include <plib035.h>
#include <timer.h>

using namespace irs;

port_extender_pca9539_t::port_extender_pca9539_t(irs_u8 a_i2c_addr, i2c_t* ap_i2c, irs_u8 a_default_port_0, irs_u8 a_default_port_1, bool do_reset) :
  m_status(in_initialize),
  mp_i2c(ap_i2c),
  m_i2c_addr(a_i2c_addr),
  m_buffer(),
  m_default_port_0(a_default_port_0),
  m_default_port_1(a_default_port_1),
  m_pin(pin_none),
  m_cmd(cmd_none),
  m_inner_port_0(0),
  m_inner_port_1(0),
  mp_inner_port(nullptr),
  mp_user_pin(nullptr),
  m_error(err_none),
  m_port(0)
{
  if(do_reset)
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
}

void port_extender_pca9539_t::write_pin(irs_u8 a_port, irs_u8 a_pin)
{
  check_arg(a_port, a_pin);
  if(m_status == in_error)
    return;

  initialize_io_operation(a_port, a_pin, in_write_pin, cmd_output_port_0);
}

void port_extender_pca9539_t::read_pin(irs_u8 a_port, irs_u8 a_pin, irs_u8* a_user_pin)
{
  check_arg(a_port, a_pin);
  if(m_status == in_error)
    return;

  mp_user_pin = a_user_pin;
  initialize_io_operation(a_port, a_pin, in_read_pin, cmd_input_port_0);
}

void port_extender_pca9539_t::clear_pin(irs_u8 a_port, irs_u8 a_pin)
{
  check_arg(a_port, a_pin);
  if(m_status == in_error)
    return;

  initialize_io_operation(a_port, a_pin, in_clear_pin, cmd_output_port_0);
}

void port_extender_pca9539_t::toggle_pin(irs_u8 a_port, irs_u8 a_pin)
{
  check_arg(a_port, a_pin);
  if(m_status == in_error)
    return;

  initialize_io_operation(a_port, a_pin, in_toggle_pin, cmd_output_port_0);
}

void port_extender_pca9539_t::tick()
{
    mp_i2c->tick();

    switch(m_status) {
        case in_free: {
        } break;

        case in_error: {
        } break;

        case in_initialize: {
          if(mp_i2c->get_lock())
            break;

          initialize_port_extender();

          m_status = in_wait;
        } break;

        case in_write_pin: {
          *mp_inner_port |= (1 << m_pin);
          send_i2c();

          m_status = in_wait;
        } break;

        case in_read_pin: {
          if(*mp_inner_port & (1 << m_pin))
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
          *mp_inner_port &= ~(1 << m_pin);
          send_i2c();

          m_status = in_wait;
        } break;

        case in_toggle_pin: {
          if(*mp_inner_port & (1 << m_pin))
          {
            clear_pin(m_port, m_pin);
          }
          else
          {
            write_pin(m_port, m_pin);
          }
        } break;

        case in_wait: {
          if (!is_ready())
            return;
            
          abort();
          m_status = in_free;
        }
    }
}

bool port_extender_pca9539_t::is_ready()
{
  return (mp_i2c->get_status() == irs_st_ready);
}

port_extender_pca9539_t::error_t port_extender_pca9539_t::get_error()
{
  return m_error;
}

void port_extender_pca9539_t::abort()
{
  m_status = in_free;
  for(int i = 0; i < buffer_size; i++)
    m_buffer[i] = 0;
  m_pin = pin_none;
  m_cmd = cmd_none;
  mp_inner_port = nullptr;
  mp_user_pin = nullptr;
  mp_i2c->unlock();
  m_error = err_none;
}

irs_status_t port_extender_pca9539_t::get_status()
{
  return (m_status == in_free) ? irs_st_ready : irs_st_busy;
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

void port_extender_pca9539_t::initialize_io_operation(irs_u8 a_port, irs_u8 a_pin, status_t a_status, command_t a_cmd)
{
  IRS_ASSERT(!mp_i2c->get_lock());
  IRS_ASSERT(m_status == in_free);

  mp_i2c->lock();
  mp_i2c->set_device_address(m_i2c_addr);
  m_pin = a_pin;
  m_status = a_status;
  m_port = a_port;

  switch(a_port) {
    case port_0: {
      m_cmd = a_cmd;
      mp_inner_port = &m_inner_port_0;
    } break;

    case port_1: {
      m_cmd = static_cast<command_t>(a_cmd + 1);
      mp_inner_port = &m_inner_port_1;
    } break;
  }
}

void port_extender_pca9539_t::send_i2c()
{
  mp_i2c->lock();

  m_buffer[0] = m_cmd;
  m_buffer[1] = *mp_inner_port;

  mp_i2c->write(m_buffer, 2);
}

void port_extender_pca9539_t::check_arg(irs_u8 a_port, irs_u8 a_pin)
{
  if(a_port > port_count)
  {
    m_error =  err_invalid_arg;
    m_status = in_error;
    return;
  }
  if(a_pin > port_size)
  {
    m_error =  err_invalid_arg;
    m_status = in_error;
    return;
  }
}
