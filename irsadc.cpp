//! \file
//! \ingroup drivers_group
//! \brief Аналоговый ввод-вывод
//!
//! Дата: 22.04.2011
//! Дата создания: 11.09.2009

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irsadc.h>
#include <irsstrm.h>
#include <timer.h>
#include <irsdsp.h>
#include <irserror.h>

#include <irsfinal.h>

//--------------------------  LM95071 ------------------------------------------

irs::th_lm95071_t::th_lm95071_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  counter_t a_read_delay):
  m_status(TH_FREE),
  mp_spi(ap_spi),
  m_conv_koef(0.0078125f), //0.031025f //0.031075f
  m_read_counter(0),
  m_read_delay(a_read_delay),
  m_connect(false),
  m_wait_timer(irs::make_cnt_us(5)),
  mp_cs_pin(ap_cs_pin)
{
  if (mp_spi && mp_cs_pin) {
    memset(reinterpret_cast<void*>(mp_buf), 0, m_size);
    memset(reinterpret_cast<void*>(mp_spi_buf), 0, m_spi_size);
    mp_cs_pin->clear();
    while(mp_spi->get_lock());
    mp_spi->lock();
    configure_spi();
    mp_spi->write(mp_spi_buf, m_spi_size);
    for (; mp_spi->get_status() != irs::spi_t::FREE; )
      mp_spi->tick();
    mp_spi->write(mp_spi_buf, m_spi_size);
    for (; mp_spi->get_status() != irs::spi_t::FREE; )
      mp_spi->tick();
    mp_cs_pin->set();
    mp_spi->reset_configuration();
    mp_spi->unlock();
    set_to_cnt(m_read_counter, m_read_delay);
  }
}

void irs::th_lm95071_t::configure_spi()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::NEGATIVE_POLARITY);
  mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
}

irs::th_lm95071_t::~th_lm95071_t()
{
}

irs_uarc irs::th_lm95071_t::size()
{
  return m_size;
}

irs_bool irs::th_lm95071_t::connected()
{
  return (mp_spi && mp_cs_pin && m_connect);
}

void irs::th_lm95071_t::read(irs_u8* ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (!a_index && (a_size == m_size)) {
    memcpy(reinterpret_cast<void*>(ap_buf),
      reinterpret_cast<void*>(mp_buf), m_size);
    mp_buf[m_control_byte] &= ~(1 << m_new_data_bit);
  } else {
    if (a_index >= m_size) return;
    irs_u8 size = static_cast<irs_u8>(a_size);
    if (size + a_index > m_size) {
      size = static_cast<irs_u8>(m_size - a_index);
    }
    memcpy(reinterpret_cast<void*>(ap_buf),
      reinterpret_cast<void*>(mp_buf + a_index), size);
    mp_buf[m_control_byte] &= ~(1 << m_new_data_bit);
  }
}

void irs::th_lm95071_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) {
    size = static_cast<irs_u8>(m_size - a_index);
  }
  memcpy(reinterpret_cast<void*>(mp_buf + a_index),
    reinterpret_cast<const void*>(ap_buf), size);
}

irs_bool irs::th_lm95071_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  bool bit = (mp_buf[a_index] >> a_bit_index) & static_cast<irs_u8>(1);
  if (a_index != m_control_byte) {
    mp_buf[m_control_byte] &= ~(1 << m_new_data_bit);
  }
  return bit;
}
void irs::th_lm95071_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
}

void irs::th_lm95071_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
}

void irs::th_lm95071_t::tick()
{
  mp_spi->tick();
  switch (m_status)
  {
    case TH_FREE:
    {
      if (test_to_cnt(m_read_counter) &&
        !(mp_buf[m_control_byte] & (1 << m_stop_bit))) {
        if (!mp_spi->get_lock() && (mp_spi->get_status() == irs::spi_t::FREE))
        {
          set_to_cnt(m_read_counter, m_read_delay);
          mp_spi->lock();
          configure_spi();
          mp_cs_pin->clear();
          mp_spi->read(mp_spi_buf, m_spi_size);
          m_status = TH_READ;
        }
      } break;
    }
    case TH_READ:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        mp_buf[0] = 1;
        mp_buf[1] = mp_spi_buf[1];
        mp_buf[2] = mp_spi_buf[0];
        irs_u16& th_value = reinterpret_cast<irs_u16&>(mp_buf[1]);
        if (th_value != 0x800F) {
          m_connect = true;
          mp_cs_pin->set();
          mp_spi->reset_configuration();
          mp_spi->unlock();
          m_status = TH_FREE;
        } else { // Shutdown mode
          mp_spi_buf[0] = 0;
          mp_spi_buf[1] = 0;
          m_status = TH_RESET;
          mp_spi->write(mp_spi_buf, m_spi_size);
          mp_cs_pin->clear();
        }
      } break;
    }
    case TH_RESET:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_status = TH_FREE;
      } break;
    }
  }
}

float irs::th_lm95071_t::get_conv_koef()
{
  return m_conv_koef;
}

//--------------------------  AD7791  ------------------------------------------

irs::adc_ad7791_t::adc_ad7791_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  counter_t a_read_delay):
  m_status(ADC_FREE),
  mp_spi(ap_spi),
  m_read_counter(0),
  m_read_delay(a_read_delay),
  m_connect(false),
  mp_cs_pin(ap_cs_pin)
{
  memset(static_cast<void*>(mp_buf), 0, m_size);
  memset(static_cast<void*>(mp_spi_buf), 0, m_spi_size);
  for (; (mp_spi->get_status() != irs::spi_t::FREE) && (mp_spi->get_lock()); )
    mp_spi->tick();
  irs_u8 mp_init_buffer[m_init_sequence_size];
  memset(static_cast<void*>(mp_init_buffer), 0xFF, m_reset_sequence_size);
  mp_init_buffer[4] = (1 << RS0)|(0 << CH0)|(0 << CH1);
  mp_init_buffer[5] = (0 << MD1)|(0 << MD0)|(0 << BO)|(1 << UB)|(1 << BUF);
  mp_init_buffer[6] = (1 << RS1);
  mp_init_buffer[7] = (1 << FS2)|(0 << FS1)|(1 << FS0);
  mp_spi->lock();
  configure_spi();
  mp_cs_pin->clear();
  mp_spi->write(mp_init_buffer, m_init_sequence_size);
  for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());
  mp_cs_pin->set();
  mp_spi->reset_configuration();
  mp_spi->unlock();
}

void irs::adc_ad7791_t::configure_spi()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::POSITIVE_POLARITY);
  mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
}

irs::adc_ad7791_t::~adc_ad7791_t()
{
  return;
}

irs_uarc irs::adc_ad7791_t::size()
{
  return m_size;
}

irs_bool irs::adc_ad7791_t::connected()
{
  if (mp_spi && mp_cs_pin && m_connect) return true;
  else return false;
}

void irs::adc_ad7791_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(static_cast<void*>(ap_buf), static_cast<void*>(mp_buf + a_index),
    size);
  mp_buf[0] = 0;
  return;
}

void irs::adc_ad7791_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(static_cast<void*>(mp_buf + a_index),
    static_cast<const void*>(ap_buf), size);
}

irs_bool irs::adc_ad7791_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  bool bit = (mp_buf[a_index] >> a_bit_index) & static_cast<irs_u8>(1);
  mp_buf[0] = 0;
  return bit;
}
void irs::adc_ad7791_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
}

void irs::adc_ad7791_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
}

void irs::adc_ad7791_t::tick()
{
  mp_spi->tick();
  switch (m_status)
  {
  case ADC_FREE:
    {
      if (test_to_cnt(m_read_counter))
      {
        if (!mp_spi->get_lock() && (mp_spi->get_status() == irs::spi_t::FREE))
        {
          set_to_cnt(m_read_counter, m_read_delay);
          configure_spi();
          mp_spi->lock();
          mp_cs_pin->clear();
          memset(static_cast<void*>(mp_spi_buf), 0, m_spi_size);
          mp_spi_buf[0] = (1 << RW)|(1 << RS0)|(1 << RS1)|(0 << CH0)|(0 << CH1);
          mp_spi->write(mp_spi_buf, sizeof(mp_spi_buf[0]));
          m_status = ADC_REQUEST;
        }
      }
      break;
    }
  case ADC_REQUEST:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        mp_spi->read(mp_spi_buf, m_spi_size);
        m_status = ADC_READ;
      }
      break;
    }
  case ADC_READ:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        m_connect = true;
        memset(static_cast<void*>(mp_buf), 0, m_size);
        mp_buf[0] = 1;
        mp_buf[1] = mp_spi_buf[2];
        mp_buf[2] = mp_spi_buf[1];
        mp_buf[3] = mp_spi_buf[0];
        mp_buf[4] = 0;
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_status = ADC_FREE;
      }
      break;
    }
  }
}

// class cyclic_adc_ad7791_t
irs::cyclic_adc_ad7791_t::cyclic_adc_ad7791_t(
  spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
  counter_t a_read_delay
):
  m_adc_ad7791(ap_spi, ap_cs_pin, a_read_delay),
  m_adc_ad7791_data(&m_adc_ad7791)
{
}

irs::cyclic_adc_ad7791_t::size_type
irs::cyclic_adc_ad7791_t::get_resulution() const
{
  return adc_resolution;
}

bool irs::cyclic_adc_ad7791_t::new_value_exists(irs_u8 a_channel) const
{
  if (a_channel > 0) {
    IRS_LIB_ERROR(ec_standard, "Нет канала с таким номером");
  }
  return m_adc_ad7791_data.new_data_bit;
}

irs_u32 irs::cyclic_adc_ad7791_t::get_u32_data(irs_u8 a_channel)
{
  if (a_channel > 0) {
    IRS_LIB_ERROR(ec_standard, "Нет канала с таким номером");
  }
  return m_adc_ad7791_data.voltage_code << (32 - adc_resolution);
}

void irs::cyclic_adc_ad7791_t::tick()
{
  m_adc_ad7791.tick();
}

//--------------------------  ADS8344 ------------------------------------------
irs::cyclic_adc_ads8344_t::cyclic_adc_ads8344_t(spi_t* ap_spi,
  gpio_pin_t* ap_cs_pin, select_channel_type a_selected_channels,
  counter_t a_read_delay
):
  mp_spi(ap_spi),
  mp_cs_pin(ap_cs_pin),
  m_process(process_init),
  m_new_value_exists(false),
  m_delay_timer(a_read_delay),
  m_channels(),
  m_current_channel(0),
  m_selected_other_channels(false),
  m_names()
{
  m_names.push_back(ch_0);
  m_names.push_back(ch_1);
  m_names.push_back(ch_2);
  m_names.push_back(ch_3);
  m_names.push_back(ch_4);
  m_names.push_back(ch_5);
  m_names.push_back(ch_6);
  m_names.push_back(ch_7);

  select_channels(a_selected_channels);
}

void irs::cyclic_adc_ads8344_t::configure_spi()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::NEGATIVE_POLARITY);
  mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
}

irs::cyclic_adc_ads8344_t::size_type
irs::cyclic_adc_ads8344_t::get_resulution() const
{
  return adc_resolution;
}

void irs::cyclic_adc_ads8344_t::select_channels(irs_u32 a_selected_channels)
{
  m_channels.clear();
  for (size_type i = 0; i < m_names.size(); i++) {
    if (a_selected_channels & m_names[i]) {
      channel_t channel;
      channel.index = i;
      m_channels.push_back(channel);
    }
  }
  m_current_channel = 0;
  m_selected_other_channels = true;
}

bool irs::cyclic_adc_ads8344_t::new_value_exists(irs_u8 a_channel) const
{
  if (static_cast<size_type>(a_channel) >= m_channels.size()) {
    IRS_LIB_ERROR(ec_standard, "Нет канала с таким номером");
  }
  return m_channels[a_channel].new_value_exists;
}

irs_u32 irs::cyclic_adc_ads8344_t::get_u32_data(irs_u8 a_channel)
{
  if (static_cast<size_type>(a_channel) >= m_channels.size()) {
    IRS_LIB_ERROR(ec_standard, "Нет канала с таким номером");
  }
  m_channels[a_channel].new_value_exists = false;
  return m_channels[a_channel].value << (32 - adc_resolution);
}

void irs::cyclic_adc_ads8344_t::tick()
{
  mp_spi->tick();
  switch (m_process) {
    case process_init: {
      if (!mp_spi->get_lock() && (mp_spi->get_status() == irs::spi_t::FREE)) {
        mp_spi->lock();
        configure_spi();
        mp_cs_pin->clear();
        memset(static_cast<void*>(mp_spi_read_buf), 0, m_spi_buf_size);
        memset(static_cast<void*>(mp_spi_write_buf), 0, m_spi_buf_size);
        mp_spi_write_buf[0] = make_control_byte(ch_0);
        mp_spi->read_write(mp_spi_read_buf, mp_spi_write_buf, m_spi_buf_size);
        m_process = process_wait_init;;
      }
    } break;
    case process_wait_init: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_process = process_read_write;
      }
    } break;
    case process_read_write: {
      m_delay_timer.check();
      if (m_delay_timer.stopped() && !mp_spi->get_lock() &&
          (mp_spi->get_status() == irs::spi_t::FREE)) {
        mp_spi->lock();
        configure_spi();
        mp_cs_pin->clear();
        memset(static_cast<void*>(mp_spi_read_buf), 0, m_spi_buf_size);
        memset(static_cast<void*>(mp_spi_write_buf), 0, m_spi_buf_size);
        mp_spi_write_buf[0] = make_control_byte(
          static_cast<irs_u8>(m_channels[m_current_channel].index));
        mp_spi->read_write(mp_spi_read_buf, mp_spi_write_buf, m_spi_buf_size);
        m_selected_other_channels = false;
        m_delay_timer.start();
        m_process = process_wait;
      }
    } break;
    case process_wait: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        mp_spi->reset_configuration();
        mp_cs_pin->set();
        mp_spi->unlock();
        if (!m_selected_other_channels) {
          int raw = (mp_spi_read_buf[1] << 9) |
            (mp_spi_read_buf[2] << 1) | ((mp_spi_read_buf[3] & 0x80) >> 7);
          const sample_type sample = static_cast<sample_type>(raw);
          m_channels[m_current_channel].value = sample;
          m_channels[m_current_channel].new_value_exists = true;
          if (m_channels.size() > 1) {
            m_current_channel++;
            if (m_current_channel >= m_channels.size()) {
              m_current_channel = 0;
            }
          }
        }
        m_process = process_read_write;
      }
    } break;
  }
}

irs_u8 irs::cyclic_adc_ads8344_t::make_control_byte(irs_u8 a_channel)
{
  const irs_u8 channel = static_cast<irs_u8>(irs::bound<int>(a_channel, 0, 7));
  irs_u8 channel_code = 0;
  switch (channel) {
    case 0: {
      channel_code = 0x0;
    } break;
    case 1: {
      channel_code = 0x4;
    } break;
    case 2: {
      channel_code = 0x1;
    } break;
    case 3: {
      channel_code = 0x5;
    } break;
    case 4: {
      channel_code = 0x2;
    } break;
    case 5: {
      channel_code = 0x6;
    } break;
    case 6: {
      channel_code = 0x3;
    } break;
    case 7: {
      channel_code = 0x7;
    } break;
  }
  irs_u8 control_byte = static_cast<irs_u8>(
    (1 << s_bit)|(channel_code << a0_bit)|
    (1 << sgl_dif_bit)|(1 << pd1_bit)|(1 << pd0_bit));
  return control_byte;
}

//--------------------------  AD7683  ------------------------------------------

irs::adc_ad7683_t::adc_ad7683_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  counter_t a_read_delay):
  m_status(ADC_FREE),
  mp_spi(ap_spi),
  m_conv_delay(irs::make_cnt_mcs(2)),
  m_read_counter(0),
  m_read_delay(a_read_delay),
  m_connect(false),
  mp_cs_pin(ap_cs_pin)
{
  memset(reinterpret_cast<void*>(mp_buf), 0, m_size);
  memset(reinterpret_cast<void*>(mp_spi_buf), 0, m_spi_size);
  mp_cs_pin->set();
}

irs::adc_ad7683_t::~adc_ad7683_t()
{
  return;
}

irs_uarc irs::adc_ad7683_t::size()
{
  return m_size;
}

irs_bool irs::adc_ad7683_t::connected()
{
  if (mp_spi && mp_cs_pin && m_connect) return true;
  else return false;
}

void irs::adc_ad7683_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(ap_buf),
    reinterpret_cast<void*>(mp_buf + a_index), size);
  mp_buf[0] = 0;
  return;
}

void irs::adc_ad7683_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(mp_buf + a_index),
    reinterpret_cast<const void*>(ap_buf), size);
}

irs_bool irs::adc_ad7683_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  bool bit = (mp_buf[a_index] >> a_bit_index) & static_cast<irs_u8>(1);
  mp_buf[0] = 0;
  return bit;
}
void irs::adc_ad7683_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
}

void irs::adc_ad7683_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
}

void irs::adc_ad7683_t::tick()
{
  mp_spi->tick();

  /*static double max_time = 0;
  double time2 = 0;
  static irs::loop_timer_t timer2(irs::make_cnt_s(5));
  static irs::measure_time_t time;
  */
  switch (m_status)
  {
  case ADC_FREE:
    {
      if (test_to_cnt(m_read_counter) && !mp_spi->get_lock()
          && (mp_spi->get_status() == irs::spi_t::FREE))
      {
        set_to_cnt(m_read_counter, m_read_delay);
        mp_spi->set_order(irs::spi_t::MSB);
        mp_spi->set_polarity(irs::spi_t::NEGATIVE_POLARITY);
        mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
        mp_cs_pin->clear();
        mp_spi->lock();
        mp_spi->read(mp_spi_buf, m_spi_size);
        m_status = ADC_READ;
        //time.start();
      }
      break;
    }
  case ADC_READ:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        m_connect = true;

        irs_u32 value = 0;
        reinterpret_cast<irs_u8*>(&value)[0] = mp_spi_buf[2];
        reinterpret_cast<irs_u8*>(&value)[1] = mp_spi_buf[1];
        reinterpret_cast<irs_u8*>(&value)[2] = mp_spi_buf[0];
        value >>= 2;
        irs_u16 value_u16 = static_cast<irs_u16>(value);
        mp_buf[0] = 1;
        reinterpret_cast<irs_u16&>(mp_buf[1]) = value_u16;
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_status = ADC_FREE;

        /*time2 = time.get();
        max_time = max(max_time, time2);
        if (timer2.check()) {
          IRS_LIB_DBG_MSG("adc tick max_time " << max_time);
          IRS_LIB_DBG_MSG("adc tick time " << time2);
        }
        */
      }
      break;
    }
  }
}

// class cyclic_adc_ad7683_t
irs::cyclic_adc_ad7683_t::cyclic_adc_ad7683_t(
  spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
  counter_t a_read_delay
):
  m_adc_ad7683(ap_spi, ap_cs_pin, a_read_delay),
  m_adc_ad7683_data(&m_adc_ad7683)
{
}

irs::cyclic_adc_ad7683_t::size_type
irs::cyclic_adc_ad7683_t::get_resulution() const
{
  return adc_resolution;
}

bool irs::cyclic_adc_ad7683_t::new_value_exists(irs_u8 a_channel) const
{
  if (a_channel > 0) {
    IRS_LIB_ERROR(ec_standard, "Нет канала с таким номером");
  }
  return m_adc_ad7683_data.new_data_bit;
}

irs_u32 irs::cyclic_adc_ad7683_t::get_u32_data(irs_u8 a_channel)
{
  if (a_channel > 0) {
    IRS_LIB_ERROR(ec_standard, "Нет канала с таким номером");
  }
  return m_adc_ad7683_data.voltage_code << (32 - adc_resolution);
}

void irs::cyclic_adc_ad7683_t::tick()
{
  m_adc_ad7683.tick();
}

//--------------------------  AD7686  ------------------------------------------

irs::adc_ad7686_t::adc_ad7686_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  counter_t a_read_delay):
  m_status(ADC_FREE),
  mp_spi(ap_spi),
  m_conv_delay(irs::make_cnt_mcs(2)),
  m_read_counter(0),
  m_read_delay(a_read_delay),
  m_connect(false),
  mp_cs_pin(ap_cs_pin)
{
  memset(reinterpret_cast<void*>(mp_buf), 0, m_size);
  memset(reinterpret_cast<void*>(mp_spi_buf), 0, m_spi_size);
  mp_cs_pin->set();
}

irs::adc_ad7686_t::~adc_ad7686_t()
{
  return;
}

irs_uarc irs::adc_ad7686_t::size()
{
  return m_size;
}

irs_bool irs::adc_ad7686_t::connected()
{
  if (mp_spi && mp_cs_pin && m_connect) return true;
  else return false;
}

void irs::adc_ad7686_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(ap_buf),
    reinterpret_cast<void*>(mp_buf + a_index), size);
  mp_buf[0] = 0;
  return;
}

void irs::adc_ad7686_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(mp_buf + a_index),
    reinterpret_cast<const void*>(ap_buf), size);
}

irs_bool irs::adc_ad7686_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  bool bit = (mp_buf[a_index] >> a_bit_index) & static_cast<irs_u8>(1);
  mp_buf[0] = 0;
  return bit;
}
void irs::adc_ad7686_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
}

void irs::adc_ad7686_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
}

void irs::adc_ad7686_t::tick()
{
  mp_spi->tick();
  switch (m_status)
  {
  case ADC_FREE:
    {
      if (test_to_cnt(m_read_counter) && !mp_spi->get_lock()
          && (mp_spi->get_status() == irs::spi_t::FREE))
      {
        set_to_cnt(m_read_counter, m_read_delay);
        mp_spi->set_order(irs::spi_t::MSB);
        mp_spi->set_polarity(irs::spi_t::POSITIVE_POLARITY);//FALLING_EDGE);
        mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
        mp_cs_pin->clear();
        mp_spi->lock();
        counter_t delay_cnt = 0;
        for (set_to_cnt(delay_cnt, m_conv_delay); !test_to_cnt(delay_cnt););
        mp_cs_pin->set();
        for (set_to_cnt(delay_cnt, m_conv_delay); !test_to_cnt(delay_cnt););
        mp_cs_pin->clear();
        mp_spi->read(mp_spi_buf, m_spi_size);
        m_status = ADC_READ;
      }
      break;
    }
  case ADC_READ:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        m_connect = true;
        mp_buf[0] = 1;
        mp_buf[1] = mp_spi_buf[1];
        mp_buf[2] = mp_spi_buf[0];
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_status = ADC_FREE;
      }
      break;
    }
  }
}

// class cyclic_adc_ad7686_t
irs::cyclic_adc_ad7686_t::cyclic_adc_ad7686_t(
  spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
  counter_t a_read_delay
):
  m_adc_ad7686(ap_spi, ap_cs_pin, a_read_delay),
  m_adc_ad7686_data(&m_adc_ad7686)
{
}

irs::cyclic_adc_ad7686_t::size_type
irs::cyclic_adc_ad7686_t::get_resulution() const
{
  return adc_resolution;
}

bool irs::cyclic_adc_ad7686_t::new_value_exists(irs_u8 a_channel) const
{
  if (a_channel > 0) {
    IRS_LIB_ERROR(ec_standard, "Нет канала с таким номером");
  }
  return m_adc_ad7686_data.new_data_bit;
}

irs_u32 irs::cyclic_adc_ad7686_t::get_u32_data(irs_u8 a_channel)
{
  if (a_channel > 0) {
    IRS_LIB_ERROR(ec_standard, "Нет канала с таким номером");
  }
  return m_adc_ad7686_data.voltage_code << (32 - adc_resolution);
}

void irs::cyclic_adc_ad7686_t::tick()
{
  m_adc_ad7686.tick();
}

//--------------------------  AD8400  ------------------------------------------

irs::dac_ad8400_t::dac_ad8400_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  irs_u8 a_init_value):
  m_status(DAC_FREE),
  mp_spi(ap_spi),
  m_init_value(a_init_value),
  m_need_write(false),
  mp_cs_pin(ap_cs_pin)
{
  memset(mp_write_buffer, 0, sizeof(mp_write_buffer));
  if (mp_spi && mp_cs_pin)
  {
    mp_buf[0] = 0;
    mp_buf[1] = m_init_value;
    for (; (mp_spi->get_status() != irs::spi_t::FREE) && (mp_spi->get_lock()); )
      mp_spi->tick();
    configure_spi();
    mp_cs_pin->clear();
    irs_u8 mp_write_buffer[m_packet_size];
    mp_write_buffer[0] = 0;
    mp_write_buffer[1] = m_init_value;
    mp_spi->lock();
    mp_spi->write(mp_write_buffer, m_packet_size);
    for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());
    mp_spi->reset_configuration();
    mp_spi->unlock();
    mp_cs_pin->set();
  }
}

irs::dac_ad8400_t::~dac_ad8400_t()
{
  return;
}

void irs::dac_ad8400_t::configure_spi()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::NEGATIVE_POLARITY);//RISING_EDGE);
  mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
}

irs_uarc irs::dac_ad8400_t::size()
{
  return m_size;
}

irs_bool irs::dac_ad8400_t::connected()
{
  if (mp_spi && mp_cs_pin) return true;
  else return false;
}

void irs::dac_ad8400_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(ap_buf),
    reinterpret_cast<void*>(mp_buf + a_index), size);
  return;
}

void irs::dac_ad8400_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(mp_buf + a_index),
    reinterpret_cast<const void*>(ap_buf), size);
  mp_buf[0] = 0;
  m_need_write = true;
}

irs_bool irs::dac_ad8400_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_buf[a_index] & static_cast<irs_u8>(1 << a_bit_index));
}

void irs::dac_ad8400_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
  mp_buf[0] = 0;
  m_need_write = true;
}

void irs::dac_ad8400_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
  mp_buf[0] = 0;
  m_need_write = true;
}

void irs::dac_ad8400_t::tick()
{
  mp_spi->tick();
  switch (m_status)
  {
  case DAC_FREE:
    {
      if (m_need_write && (mp_spi->get_status() == irs::spi_t::FREE))
      {
        if (!mp_spi->get_lock())
        {
          mp_spi->lock();
          configure_spi();
          mp_write_buffer[0] = 0;
          mp_write_buffer[1] = mp_buf[1];
          mp_cs_pin->clear();
          mp_spi->write(mp_write_buffer, m_packet_size);
          m_status = DAC_WRITE;
          m_need_write = false;
        }
      }
      break;
    }
  case DAC_WRITE:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        mp_buf[0] = 1;
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_status = DAC_FREE;
      }
      break;
    }
  }
}

// class simple_dac_ad8400_t
irs::simple_dac_ad8400_t::simple_dac_ad8400_t(spi_t* ap_spi,
  gpio_pin_t* ap_cs_pin, irs_u8 a_init_value):
  m_dac_ad8400(ap_spi, ap_cs_pin, a_init_value),
  m_dac_ad8400_data(&m_dac_ad8400)
{
}

irs_status_t irs::simple_dac_ad8400_t::get_status() const
{
  if (m_dac_ad8400_data.ready_bit) {
    return irs_st_ready;
  }
  return irs_st_busy;
}

size_t irs::simple_dac_ad8400_t::get_resolution() const
{
  return dac_resulution;
}

void irs::simple_dac_ad8400_t::set_u32_data(
  size_t a_channel, const irs_u32 a_data)
{
  if (a_channel > 0) {
    IRS_LIB_ERROR(ec_standard, "Нет канала с таким номером");
  }
  m_dac_ad8400_data.resistance_code =
    static_cast<irs_u8>(a_data >> (32 - dac_resulution));
}

void irs::simple_dac_ad8400_t::tick()
{
  m_dac_ad8400.tick();
}


//--------------------------  AD7376  ------------------------------------------
// Цифровой потенциометр 8 бит

irs::dac_ad7376_t::dac_ad7376_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
    gpio_pin_t *ap_rs_pin, gpio_pin_t *ap_shdn_pin, irs_u8 a_init_value):
  m_status(DAC_FREE),
  mp_spi(ap_spi),
  m_init_value(a_init_value),
  m_need_write(false),
  m_need_reset(false),
  mp_cs_pin(ap_cs_pin),
  mp_rs_pin(ap_rs_pin),
  mp_shdn_pin(ap_shdn_pin),
  timerdelay(irs::make_cnt_s(0.000001))
{
  memset(mp_write_buffer, 0, sizeof(mp_write_buffer));
  if (mp_spi && mp_cs_pin && mp_rs_pin && mp_shdn_pin)
  {
    init_to_cnt();
    mp_buf[0] = 0;
    mp_buf[1] = m_init_value;
    mp_rs_pin->set();
    mp_shdn_pin->set();
    for (; (mp_spi->get_status() != irs::spi_t::FREE) && (mp_spi->get_lock()); )
      mp_spi->tick();
    mp_cs_pin->clear();
    irs_u8 mp_write_buffer[m_packet_size];
    mp_write_buffer[0] = m_init_value;
    mp_spi->write(mp_write_buffer, m_packet_size);
    for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());
    mp_cs_pin->set();
  }
}

irs::dac_ad7376_t::~dac_ad7376_t()
{
  deinit_to_cnt();
  return;
}

irs_uarc irs::dac_ad7376_t::size()
{
  return m_size;
}

irs_bool irs::dac_ad7376_t::connected()
{
  if (mp_spi && mp_cs_pin) return true;
  else return false;
}

void irs::dac_ad7376_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
    if (a_index >= m_size) return;
    irs_u8 size = static_cast<irs_u8>(a_size);
    if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
    memcpy(reinterpret_cast<void*>(ap_buf), reinterpret_cast<void*>(mp_buf + a_index), size);
    return;
}

void irs::dac_ad7376_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  if (((mp_buf[0]>>m_rs_bit_position)&1) == 0) m_need_reset = true;
  if (((mp_buf[0]>>m_shdn_bit_position)&1) == 0) mp_shdn_pin->clear();
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(mp_buf + a_index),
    reinterpret_cast<const void*>(ap_buf), size);
  mp_buf[0] &= (1 << m_ready_bit_position)^0xFF;
  m_need_write = true;
}

irs_bool irs::dac_ad7376_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_buf[a_index] & static_cast<irs_u8>(1 << a_bit_index));
}

void irs::dac_ad7376_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  if (a_index == 0){
    if (a_bit_index == m_rs_bit_position){
      m_need_reset = true;
    }
    if (a_bit_index == m_shdn_bit_position){
      mp_shdn_pin->clear();
    }
  }
  mp_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
  mp_buf[0] &= (1 << m_ready_bit_position)^0xFF;
  m_need_write = true;
}

void irs::dac_ad7376_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
  mp_buf[0] &= (1 << m_ready_bit_position)^0xFF;
  m_need_write = true;
}

void irs::dac_ad7376_t::tick()
{
  mp_spi->tick();
  switch (m_status)
  {
  case DAC_FREE:
    {
      if (m_need_write && (mp_spi->get_status() == irs::spi_t::FREE))
      {
        if (!mp_spi->get_lock())
        {
          mp_write_buffer[0] = mp_buf[1];
          mp_cs_pin->clear();
          mp_spi->lock();
          mp_spi->write(mp_write_buffer, m_packet_size);
          m_status = DAC_WRITE;
          m_need_write = false;
          if(m_need_reset){
            mp_rs_pin->clear();
            timerdelay.check();
          }
          m_need_reset = false;
        }
      }
      break;
    }
  case DAC_WRITE:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        mp_buf[0] &= (0 << m_ready_bit_position)^0xFF;
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_status = DAC_FREE;
      }
      break;
    }
  }
}

//--------------------------  MAX551  ------------------------------------------

irs::dac_max551_t::dac_max551_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  irs_u16 a_init_value):
  m_status(DAC_FREE),
  mp_spi(ap_spi),
  m_init_value(a_init_value),
  m_need_write(false),
  mp_cs_pin(ap_cs_pin)
{
  memset(mp_write_buffer, 0, sizeof(mp_write_buffer));
  if (mp_cs_pin)
  {
    mp_cs_pin->set();
    reinterpret_cast<irs_u16&>(mp_buf[1]) = m_init_value;
    for (; (mp_spi->get_status() != irs::spi_t::FREE) && (mp_spi->get_lock()); )
      mp_spi->tick();
    mp_spi->write(reinterpret_cast<irs_u8*>(&m_init_value), m_packet_size);
    for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());
    mp_cs_pin->clear();
    mp_cs_pin->set();
  }
}

void irs::dac_max551_t::configure_spi()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::NEGATIVE_POLARITY);//RISING_EDGE);
  mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
}

irs::dac_max551_t::~dac_max551_t()
{
  return;
}

irs_uarc irs::dac_max551_t::size()
{
  return m_size;
}

irs_bool irs::dac_max551_t::connected()
{
  if (mp_spi && mp_cs_pin) return true;
  else return false;
}

void irs::dac_max551_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (!a_index && (a_size == m_size))
  {
    memcpy(reinterpret_cast<void*>(ap_buf),
      reinterpret_cast<void*>(mp_buf), m_size);
    return;
  }
  else
  {
    if (a_index >= m_size) return;
    irs_u8 size = static_cast<irs_u8>(a_size);
    if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
    memcpy(reinterpret_cast<void*>(ap_buf),
      reinterpret_cast<void*>(mp_buf + a_index), size);
    return;
  }
}

void irs::dac_max551_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (!a_index && (a_size == m_size))
  {
    memcpy(reinterpret_cast<void*>(mp_buf),
      reinterpret_cast<const void*>(ap_buf), m_size);
    mp_buf[0] = 0;
    m_need_write = true;
    return;
  }
  else
  {
    if (a_index >= m_size) return;
    irs_u8 size = static_cast<irs_u8>(a_size);
    if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
    memcpy(reinterpret_cast<void*>(mp_buf + a_index),
      reinterpret_cast<const void*>(ap_buf), size);
    mp_buf[0] = 0;
    m_need_write = true;
  }
}

irs_bool irs::dac_max551_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_buf[a_index] & static_cast<irs_u8>(1 << a_bit_index));
}

void irs::dac_max551_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
  mp_buf[0] = 0;
  m_need_write = true;
}

void irs::dac_max551_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
  mp_buf[0] = 0;
  m_need_write = true;
}

void irs::dac_max551_t::tick()
{
  mp_spi->tick();
  switch (m_status)
  {
  case DAC_FREE:
    {
      if (m_need_write && (mp_spi->get_status() == irs::spi_t::FREE))
      {
        if (!mp_spi->get_lock())
        {
          mp_spi->lock();
          configure_spi();
          mp_write_buffer[0] = mp_buf[2];
          mp_write_buffer[1] = mp_buf[1];
          mp_spi->write(mp_write_buffer, m_packet_size);
          m_status = DAC_WRITE;
          m_need_write = false;
        }
      }
      break;
    }
  case DAC_WRITE:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        mp_cs_pin->clear();
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        mp_buf[0] = 1;
        m_status = DAC_FREE;
      }
      break;
    }
  }
}

//--------------------------  AD9854  ------------------------------------------

irs::dds_ad9854_t::dds_ad9854_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  gpio_pin_t *ap_reset_pin, gpio_pin_t *ap_update_pin):
  m_status(DDS_FREE),
  mp_spi(ap_spi),
  m_write_vector(),
  m_current_write_index(0),
  m_all_write(false),
  m_need_write_cr(false),
  //  CS
  mp_cs_pin(ap_cs_pin),
  mp_reset_pin(ap_reset_pin),
  mp_update_pin(ap_update_pin),
  m_first_byte(1),
  m_update_time()
{
  mp_cs_pin->set();
  //
  init_to_cnt();
  m_refresh_time = MS_TO_CNT(20);
  memset(reinterpret_cast<void*>(mp_buf), 0, m_size);
  //  внутренние размеры регистров DDS
  mp_reg_size[ADDR_PH1] = SZ_DDS_PH1;
  mp_reg_size[ADDR_PH2] = SZ_DDS_PH2;
  mp_reg_size[ADDR_FREQ1] = SZ_DDS_FREQ1;
  mp_reg_size[ADDR_FREQ2] = SZ_DDS_FREQ2;
  mp_reg_size[ADDR_DFREQ] = SZ_DDS_DFREQ;
  mp_reg_size[ADDR_INTUD] = SZ_DDS_INTUD;
  mp_reg_size[ADDR_RRCR] = SZ_DDS_RRCR;
  mp_reg_size[ADDR_CR] = SZ_DDS_CR;
  mp_reg_size[ADDR_IPATH] = SZ_DDS_IPATH;
  mp_reg_size[ADDR_QPATH] = SZ_DDS_QPATH;
  mp_reg_size[ADDR_SKRR] = SZ_DDS_SKRR;
  mp_reg_size[ADDR_QDAC] = SZ_DDS_QDAC;
  //  размеры регистров в mxdate
  mp_mxdata_reg_size[ADDR_PH1] = SZ_PH1;
  mp_mxdata_reg_size[ADDR_PH2] = SZ_PH2;
  mp_mxdata_reg_size[ADDR_FREQ1] = SZ_FREQ1;
  mp_mxdata_reg_size[ADDR_FREQ2] = SZ_FREQ2;
  mp_mxdata_reg_size[ADDR_DFREQ] = SZ_DFREQ;
  mp_mxdata_reg_size[ADDR_INTUD] = SZ_INTUD;
  mp_mxdata_reg_size[ADDR_RRCR] = SZ_RRCR;
  mp_mxdata_reg_size[ADDR_CR] = SZ_CR;
  mp_mxdata_reg_size[ADDR_IPATH] = SZ_IPATH;
  mp_mxdata_reg_size[ADDR_QPATH] = SZ_QPATH;
  mp_mxdata_reg_size[ADDR_SKRR] = SZ_SKRR;
  mp_mxdata_reg_size[ADDR_QDAC] = SZ_QDAC;
  //  смещения регистров в mxdata
  mp_reg_position[ADDR_PH1] = POS_PH1;
  mp_reg_position[ADDR_PH2] = POS_PH2;
  mp_reg_position[ADDR_FREQ1] = POS_FREQ1;
  mp_reg_position[ADDR_FREQ2] = POS_FREQ2;
  mp_reg_position[ADDR_DFREQ] = POS_DFREQ;
  mp_reg_position[ADDR_INTUD] = POS_INTUD;
  mp_reg_position[ADDR_RRCR] = POS_RRCR;
  mp_reg_position[ADDR_CR] = POS_CR;
  mp_reg_position[ADDR_IPATH] = POS_IPATH;
  mp_reg_position[ADDR_QPATH] = POS_QPATH;
  mp_reg_position[ADDR_SKRR] = POS_SKRR;
  mp_reg_position[ADDR_QDAC] = POS_QDAC;
  //
  m_write_vector.clear();
  m_write_vector.resize(m_size, false);
  mp_update_pin->set_dir(gpio_pin_t::dir_in);
  if (mp_spi && mp_cs_pin)
  {
    irs_u32 DDS_CR = (static_cast<irs_u32>(0) << SKE)|
                     (static_cast<irs_u32>(0) << QPD)|
                     (static_cast<irs_u32>(1) << PLLBYPS)|
                     (static_cast<irs_u32>(0) << INTUD)|
                     (static_cast<irs_u32>(0) << CPD)|
                     (static_cast<irs_u32>(0) << PLLMUL0)|
                     (static_cast<irs_u32>(0) << PLLMUL2);
    irs_u32 *mp_buf_cr = reinterpret_cast<irs_u32*>(&mp_buf[POS_CR]);
    *mp_buf_cr = DDS_CR;
    //  Формирование пакета, записываемого в дедеес
    //  Control register
    irs_u8 spi2_buffer[5];
    spi2_buffer[0] = ADDR_CR;
    spi2_buffer[1] = IRS_HIBYTE(IRS_HIWORD(DDS_CR));
    spi2_buffer[2] = IRS_LOBYTE(IRS_HIWORD(DDS_CR));
    spi2_buffer[3] = IRS_HIBYTE(IRS_LOWORD(DDS_CR));
    spi2_buffer[4] = IRS_LOBYTE(IRS_LOWORD(DDS_CR));
    mp_reset_pin->set();
    counter_t cnt = 0;
    set_to_cnt(cnt, MS_TO_CNT(1));
    while (!test_to_cnt(cnt));
    mp_reset_pin->clear();
    mp_cs_pin->set();
    set_to_cnt(cnt, MS_TO_CNT(1));
    while (!test_to_cnt(cnt));
    for (; (mp_spi->get_status() != irs::spi_t::FREE) && (mp_spi->get_lock()); )
      mp_spi->tick();
    mp_cs_pin->clear();
    mp_spi->lock();
    configure_spi();
    mp_spi->write(spi2_buffer, 5);
    for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());
    mp_spi->reset_configuration();
    mp_spi->unlock();
    mp_cs_pin->set();
    set_to_cnt(cnt, MS_TO_CNT(1));
    while (!test_to_cnt(cnt));
    mp_update_pin->set_dir(gpio_pin_t::dir_out);
    mp_update_pin->set();
    set_to_cnt(cnt, MS_TO_CNT(1));
    while (!test_to_cnt(cnt));
    mp_update_pin->clear();
    set_to_cnt(m_refresh_counter, m_refresh_time);
  }
}

void irs::dds_ad9854_t::configure_spi()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::POSITIVE_POLARITY);//FALLING_EDGE);
  mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
}

irs::dds_ad9854_t::~dds_ad9854_t()
{
  return;
}

irs_uarc irs::dds_ad9854_t::size()
{
  return m_size;
}

irs_bool irs::dds_ad9854_t::connected()
{
  if (mp_spi && mp_cs_pin) return true;
  else return false;
}

void irs::dds_ad9854_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(ap_buf),
    reinterpret_cast<void*>(mp_buf + a_index), size);
  return;
}

void irs::dds_ad9854_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  if ((a_index == 0) && (ap_buf[0] & (1 << RESET_BIT)))
  {
    m_all_write = false;
    reset();
    return;
  }
  //----  Ахтунг!!!  -------  Непроверенный код  -------  Ахтунг!!!  -----------
  if ((a_index == POS_PLL_MUL) && (a_size == 1))
  {
    enum { PLL_MUL_MASK = 0xE0 };
    irs_u8 data_byte = ap_buf[0];
    if ((data_byte & PLL_MUL_MASK) == 0)
    {
      mp_buf[a_index] &= PLL_MUL_MASK;
      mp_buf[a_index] |= data_byte;
      m_write_vector[a_index] = true;
      m_all_write = false;
      mp_buf[POS_STATUS] = 0;
      return;
    }
  }
  //----  Ахтунг!!!  -------  Непроверенный код  -------  Ахтунг!!!  -----------
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(mp_buf + a_index),
    reinterpret_cast<const void*>(ap_buf), size);
  fill_n(m_write_vector.begin() + a_index, size, true);
  m_all_write = false;
  mp_buf[POS_STATUS] = 0;
}

irs_bool irs::dds_ad9854_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_buf[a_index] & static_cast<irs_u8>(1 << a_bit_index));
}

void irs::dds_ad9854_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if ((a_index == 0) && (a_bit_index == RESET_BIT))
  {
    m_all_write = false;
    reset();
    return;
  }
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
  m_write_vector[a_index] = true;
  m_all_write = false;
  mp_buf[POS_STATUS] = 0;
}

void irs::dds_ad9854_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
  m_write_vector[a_index] = true;
  m_all_write = false;
  mp_buf[POS_STATUS] = 0;
}

void irs::dds_ad9854_t::tick()
{
  mp_spi->tick();
  switch (m_status)
  {
  case DDS_FREE:
    {
      if (test_to_cnt(m_refresh_counter))
      {
        set_to_cnt(m_refresh_counter, m_refresh_time);
        if (!mp_spi->get_lock() && (mp_spi->get_status() == irs::spi_t::FREE))
        {
          if (m_need_write_cr)
          {
            m_write_buffer[0] = ADDR_CR;
            m_write_buffer[1] = mp_buf[POS_CR + 3];
            m_write_buffer[2] = mp_buf[POS_CR + 2];
            m_write_buffer[3] = mp_buf[POS_CR + 1];
            m_write_buffer[4] = mp_buf[POS_CR];
            fill_n(m_write_vector.begin() + POS_CR, SZ_CR, false);
            mp_spi->lock();
            configure_spi();
            mp_cs_pin->clear();
            mp_spi->write(m_write_buffer, SZ_CR + 1);
            m_status = DDS_WRITE;
            m_need_write_cr = false;
          }
          else
          {
            for (; m_first_byte < m_size; m_first_byte++)
              if (m_write_vector[m_first_byte] == true) break;
            if (m_first_byte < m_size)
            {
              m_all_write = false;
              irs_u8 register_index = 0;
              for (; register_index < m_num_of_registers; register_index++)
                if (m_first_byte < mp_reg_position[register_index]) break;

              if (register_index) register_index--;
              irs_u8 size =  mp_reg_size[register_index];// + 1;
              irs_u8 start_byte = mp_reg_position[register_index];
              m_write_buffer[0] = register_index;
              switch (size)
              {
                case 1:
                {
                  m_write_buffer[1] = mp_buf[start_byte];
                  break;
                }
                case 2:
                {
                  m_write_buffer[1] = mp_buf[start_byte + 1];
                  m_write_buffer[2] = mp_buf[start_byte];
                  break;
                }
                case 3:
                {
                  m_write_buffer[1] = mp_buf[start_byte + 2];
                  m_write_buffer[2] = mp_buf[start_byte + 1];
                  m_write_buffer[3] = mp_buf[start_byte];
                  break;
                }
                case 4:
                {
                  m_write_buffer[1] = mp_buf[start_byte + 3];
                  m_write_buffer[2] = mp_buf[start_byte + 2];
                  m_write_buffer[3] = mp_buf[start_byte + 1];
                  m_write_buffer[4] = mp_buf[start_byte];
                  break;
                }
                case 6:
                {
                  m_write_buffer[1] = mp_buf[start_byte + 5];
                  m_write_buffer[2] = mp_buf[start_byte + 4];
                  m_write_buffer[3] = mp_buf[start_byte + 3];
                  m_write_buffer[4] = mp_buf[start_byte + 2];
                  m_write_buffer[5] = mp_buf[start_byte + 1];
                  m_write_buffer[6] = mp_buf[start_byte];
                  break;
                }
              }

              fill_n(m_write_vector.begin() + start_byte,
                mp_mxdata_reg_size[register_index], false);
              m_first_byte += mp_mxdata_reg_size[register_index];
              mp_spi->reset_configuration();
              mp_spi->lock();
              mp_cs_pin->clear();
              mp_spi->write(m_write_buffer, size+1);
              m_status = DDS_WRITE;
            }
            //  в 0 - статус
            if (m_first_byte >= m_size)
            {
              m_first_byte = 1;
              m_all_write = true;
            }
          }
        }
      }
    } break;
  case DDS_WRITE:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        mp_cs_pin->set();
        set_to_cnt(m_update_time, MS_TO_CNT(50));
        m_status = DDS_UPDATE;
        /*counter_t cnt;
        set_to_cnt(cnt, MS_TO_CNT(50));
        while (!test_to_cnt(cnt));
        mp_update_pin->set();

        set_to_cnt(cnt, MS_TO_CNT(1));
        while (!test_to_cnt(cnt));
        mp_update_pin->clear();

        mp_update_pin->clear();
        mp_spi->unlock();
        m_status = DDS_FREE;
        if (find(m_write_vector.begin(), m_write_vector.end(), true)
            == m_write_vector.end()) mp_buf[POS_STATUS] |= 0x01;*/
      }
    } break;
    case DDS_UPDATE:
    {
      if (test_to_cnt(m_update_time))
      {
        mp_update_pin->set();
        set_to_cnt(m_update_time, MS_TO_CNT(1));
        m_status = DDS_UPDATE_WAIT;
      }
    } break;
    case DDS_UPDATE_WAIT:
    {
      if (test_to_cnt(m_update_time))
      {
        mp_update_pin->clear();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_status = DDS_FREE;
        if (find(m_write_vector.begin(), m_write_vector.end(), true)
          == m_write_vector.end()) mp_buf[POS_STATUS] |= 0x01;
      }
    } break;
  }
}

void irs::dds_ad9854_t::reset()
{
  m_write_vector.clear();
  m_write_vector.resize(m_size, true);
  m_write_vector[0] = false;
  mp_cs_pin->set();
  mp_reset_pin->set();
  counter_t cnt;
  set_to_cnt(cnt, MS_TO_CNT(1));
  while (!test_to_cnt(cnt));
  mp_reset_pin->clear();
  set_to_cnt(cnt, MS_TO_CNT(1));
  while (!test_to_cnt(cnt));
  mp_buf[POS_STATUS] = 0;
  m_need_write_cr = true;
}

//-------------------------- LTC2622 -------------------------------------------

irs::dac_ltc2622_t::dac_ltc2622_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  irs_u16 a_init_regA, irs_u16 a_init_regB
):
  m_status(DAC_FREE),
  mp_spi(ap_spi),
  m_command (reinterpret_cast<irs_u8&>(mp_write_buf[0])),
  m_regA(reinterpret_cast<irs_u16&>(mp_buf[1])),
  m_regB(reinterpret_cast<irs_u16&>(mp_buf[3])),
  m_write_reg(reinterpret_cast<irs_u16&>(mp_write_buf[1])),
  m_wait_timer(irs::make_cnt_us(1)),
  m_need_write(false),
  mp_cs_pin(ap_cs_pin)
{
  memset(mp_buf, 0, sizeof(mp_buf));
  m_regA = a_init_regA;
  m_regB = a_init_regB;
  for (; (mp_spi->get_status() != irs::spi_t::FREE) && (mp_spi->get_lock()); )
    mp_spi->tick();
  mp_cs_pin->clear();
  mp_spi->lock();

  configure_spi();

  m_command = m_com_write_to_input_register_and_update|m_addr_DACA;

  irs_u8 conv_buf[2];
  memset(conv_buf, 0, 2);

  m_write_reg = m_regA;

  conv_buf[0] = mp_write_buf[1];
  conv_buf[1] = mp_write_buf[2];
  mp_write_buf[1] = conv_buf[1];
  mp_write_buf[2] = conv_buf[0];

  mp_spi->write(mp_write_buf, m_write_buf_size);
  for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());

  mp_cs_pin->set();

  m_wait_timer.start();
  while(!m_wait_timer.check());

  mp_cs_pin->clear();
  m_command = m_com_write_to_input_register_and_update|m_addr_DACB;

  m_write_reg = m_regB;

  conv_buf[0] = mp_write_buf[1];
  conv_buf[1] = mp_write_buf[2];
  mp_write_buf[1] = conv_buf[1];
  mp_write_buf[2] = conv_buf[0];

  mp_spi->write(mp_write_buf, m_write_buf_size);
  for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());
  mp_cs_pin->set();
  mp_spi->reset_configuration();
  mp_spi->unlock();
  //mlog() << "LTC2622 по адресу 0x" << this << " инициализирован" << endl;
}

void irs::dac_ltc2622_t::configure_spi()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::POSITIVE_POLARITY);//FALLING_EDGE);
  mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
}

void irs::dac_ltc2622_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  //if (a_index == 0) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(mp_buf + a_index),
    reinterpret_cast<const void*>(ap_buf), size);
  mp_buf[0] &= (1 << m_log_enable_pos);
  m_need_write = true;
  mp_buf[0] &= ~(1 << m_ready_bit_regA);
  mp_buf[0] &= ~(1 << m_ready_bit_regB);
  #ifdef NOP
  if (mp_buf[0] & (1 << m_log_enable_pos))
  {
    irs_u16 dac_value_A = *(irs_u16*)(&mp_buf[m_data_regA_position]);
    irs_u16 dac_value_B = *(irs_u16*)(&mp_buf[m_data_regB_position]);
    mlog() << "LTC2622 0x" << this << " index = " << a_index << ", size = " <<
      a_size << " write A = " << dac_value_A << ", B = " << dac_value_B << endl;
  }
  #endif //NOP
}



irs_uarc irs::dac_ltc2622_t::size()
{
  return m_size;
}

irs_bool irs::dac_ltc2622_t::connected()
{
  return true;
}

void irs::dac_ltc2622_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
    if (a_index >= m_size) return;
    irs_u8 size = static_cast<irs_u8>(a_size);
    if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
    memcpy(reinterpret_cast<void*>(ap_buf),
      reinterpret_cast<void*>(mp_buf + a_index), size);
    return;
}

irs_bool irs::dac_ltc2622_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_buf[a_index] & static_cast<irs_u8>(1 << a_bit_index));
}

void irs::dac_ltc2622_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  //if (a_index == 0) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
  mp_buf[0] &= (1 << m_log_enable_pos);
  m_need_write = true;
  #ifdef NOP
  if (mp_buf[0] & (1 << m_log_enable_pos))
  {
    //irs_u16 dac_value_A = *(irs_u16*)(&mp_buf[m_data_regA_position]);
    //irs_u16 dac_value_B = *(irs_u16*)(&mp_buf[m_data_regB_position]);
    mlog() << "LTC2622 0x" << this <<
      " set bit index = " << a_index << ", bit = " << a_bit_index <<
      ", A = " << dac_value_A << ", B = " << dac_value_B << endl;
  }
  #endif //NOP
}

void irs::dac_ltc2622_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  //if (a_index == 0) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
  mp_buf[0] &= (1 << m_log_enable_pos);
  m_need_write = true;

  #ifdef NOP
  if (mp_buf[0] & (1 << m_log_enable_pos))
  {
    irs_u16 dac_value_A =
      reinterpret_cast<irs_u16&>(mp_buf[m_data_regA_position]);
    irs_u16 dac_value_B =
      reinterpret_cast<irs_u16&>(mp_buf[m_data_regB_position]);
    mlog() << "LTC2622 0x" << this <<
      " clear bit index = " << a_index << ", bit = " << a_bit_index <<
      ", A = " << dac_value_A << ", B = " << dac_value_B << endl;
  }
  #endif //NOP
}



void irs::dac_ltc2622_t::tick()
{
  mp_spi->tick();
  switch (m_status)
  {
  case DAC_FREE:
    {
      if (m_need_write && (mp_spi->get_status() == irs::spi_t::FREE)){
        if (!mp_spi->get_lock()){
          mp_cs_pin->clear();
          mp_spi->lock();
          configure_spi();
          m_command = m_com_write_to_input_register_and_update|m_addr_DACA;
          m_write_reg = m_regA;
          irs_u8 conv_buf[2];
          conv_buf[0] = mp_write_buf[1];
          conv_buf[1] = mp_write_buf[2];
          mp_write_buf[1] = conv_buf[1];
          mp_write_buf[2] = conv_buf[0];
          mp_spi->write(mp_write_buf, m_write_buf_size);
          m_status = DAC_WRITE_REGA;
        }
      }
      break;
    }
  case DAC_WRITE_REGA:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE){
        mp_buf[0] |= (1 << m_ready_bit_regA);
        mp_cs_pin->set();
        m_status = DAC_DALAY;
        m_wait_timer.start();
      }
      break;
    }
  case DAC_DALAY:
    {
     if(m_wait_timer.check()){
        mp_cs_pin->clear();
        m_command = m_com_write_to_input_register_and_update|m_addr_DACB;
        m_write_reg = m_regB;
        irs_u8 conv_buf[2];
        conv_buf[0] = mp_write_buf[1];
        conv_buf[1] = mp_write_buf[2];
        mp_write_buf[1] = conv_buf[1];
        mp_write_buf[2] = conv_buf[0];
        mp_spi->write(mp_write_buf, m_write_buf_size);
        m_status = DAC_WRITE_REGB;
      }

    }
  case DAC_WRITE_REGB:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE){
        mp_buf[0] |= (1 << m_ready_bit_regB);
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_need_write = false;
        m_status = DAC_FREE;
      }
      break;
    }
  }
}

//--------------------------  AD102S021 ----------------------------------------

irs::adc_adc102s021_t::adc_adc102s021_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  counter_t a_read_delay):
  m_status(ADC_FREE),
  mp_spi(ap_spi),
  m_timer(a_read_delay),
  m_connect(false),
  m_in_first_enable(true),
  m_round(false),
  mp_cs_pin(ap_cs_pin)
{
  memset(reinterpret_cast<void*>(mp_target_buf), 0, m_target_size);
  memset(reinterpret_cast<void*>(mp_read_buf), 0, m_size);
  memset(reinterpret_cast<void*>(mp_write_buf), 0, m_size);
  mp_write_buf[0] = (0<<ADD1)|(1<<ADD0);
  mp_cs_pin->set();
  m_timer.start();
}

irs::adc_adc102s021_t::~adc_adc102s021_t()
{
  return;
}

irs_uarc irs::adc_adc102s021_t::size()
{
  return m_target_size;
}

irs_bool irs::adc_adc102s021_t::connected()
{
  if (mp_spi && mp_cs_pin && m_connect) return true;
  else return false;
}

void irs::adc_adc102s021_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_target_size) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_target_size) size =
    static_cast<irs_u8>(m_target_size - a_index);
  memcpy(reinterpret_cast<void*>(ap_buf),
    reinterpret_cast<void*>(mp_target_buf + a_index), size);
}

void irs::adc_adc102s021_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_target_size) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_target_size) size =
    static_cast<irs_u8>(m_target_size - a_index);
  memcpy(reinterpret_cast<void*>(mp_write_buf + a_index),
    reinterpret_cast<const void*>(ap_buf), size);
}

irs_bool irs::adc_adc102s021_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_target_size) return false;
  if (a_bit_index > 7) return false;
  bool bit = (mp_write_buf[a_index] >> a_bit_index) & static_cast<irs_u8>(1);
  return bit;
}
void irs::adc_adc102s021_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_target_size) return;
  if (a_bit_index > 7) return;
  mp_write_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
}

void irs::adc_adc102s021_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_target_size) return;
  if (a_bit_index > 7) return;
  mp_write_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
}

void irs::adc_adc102s021_t::tick()
{
  mp_spi->tick();
  switch (m_status)
  {
    case ADC_FREE:
    {
      if (!mp_spi->get_lock() && (mp_spi->get_status() == irs::spi_t::FREE))
      {
        if (m_timer.check()) {
          m_timer.stop();
          mp_spi->set_order(irs::spi_t::MSB);
          mp_spi->set_polarity(irs::spi_t::POSITIVE_POLARITY);//FALLING_EDGE);
          mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
          mp_spi->lock();
          mp_cs_pin->clear();
          memset(reinterpret_cast<void*>(mp_read_buf), 0, m_size);
          m_status = ADC_READ;
        }
      }
    } break;
    case ADC_READ:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        mp_spi->read_write(mp_read_buf, mp_write_buf, m_size);
        m_status = ADC_READ_WAIT;
      }
    } break;
    case ADC_READ_WAIT:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        m_connect = true;
        if (m_in_first_enable) {
          mp_target_buf[0] = mp_read_buf[1];
          mp_target_buf[1] = mp_read_buf[0];
          IRS_FIRSTWORD(mp_target_buf[0]) >>= 2;
          mp_target_buf[1] &= static_cast<irs_u8>(((1<<Z5)|(1<<Z4)|
            (1<<Z3)|(1<<Z2)|(1<<Z1)|(1<<Z0))^0xFF);
          mp_write_buf[0] = (0<<ADD1)|(0<<ADD0);
          m_in_first_enable = false;
        } else {
          mp_target_buf[2] = mp_read_buf[1];
          mp_target_buf[3] = mp_read_buf[0];
          IRS_FIRSTWORD(mp_target_buf[2]) >>= 2;
          mp_target_buf[3] &= static_cast<irs_u8>(((1<<Z5)|(1<<Z4)|
            (1<<Z3)|(1<<Z2)|(1<<Z1)|(1<<Z0))^0xFF);
          mp_write_buf[0] = (0<<ADD1)|(1<<ADD0);
          m_in_first_enable = true;
        }
        m_timer.start();
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_status = ADC_FREE;
      }
    } break;
  }
}

//--------------------------  AD5293  ------------------------------------------

irs::dac_ad5293_t::dac_ad5293_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin):
  m_status(FREE),
  mp_spi(ap_spi),
  m_wait_timer(irs::make_cnt_us(2)),
  m_need_write(false),
  mp_cs_pin(ap_cs_pin),
  m_write_status(WS_READY)
{
  if (mp_spi && mp_cs_pin)
  {
    mp_cs_pin->set();

    memset(reinterpret_cast<void*>(mp_buf), 0, m_size);
    memset(reinterpret_cast<void*>(mp_write_buf), 0, m_size);

    while (!spi_ready()) mp_spi->tick();
    mp_spi->lock();
    configure_spi();
    while (!write_ready()) write_tick();
    write_to_dac(RESET);
    while (!write_ready()) write_tick();
    irs::pause(irs::make_cnt_ms(2));
    write_to_dac(HIZ);
    while (!write_ready()) write_tick();
    write_to_dac(ZERO);
    while (!write_ready()) write_tick();
    mp_spi->reset_configuration();
    mp_spi->unlock();
  }
}

irs::dac_ad5293_t::~dac_ad5293_t()
{
  return;
}

irs_uarc irs::dac_ad5293_t::size()
{
  return m_size;
}

irs_bool irs::dac_ad5293_t::connected()
{
  if (mp_spi && mp_cs_pin) return true;
  else return false;
}

void irs::dac_ad5293_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
    if (a_index >= m_size) return;
    irs_u8 size = static_cast<irs_u8>(a_size);
    if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
    memcpy(reinterpret_cast<void*>(ap_buf),
      reinterpret_cast<void*>(mp_buf + a_index), size);
    return;
}

void irs::dac_ad5293_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = static_cast<irs_u8>(a_size);
  if (size + a_index > m_size) size = static_cast<irs_u8>(m_size - a_index);
  memcpy(reinterpret_cast<void*>(mp_buf + a_index),
    reinterpret_cast<const void*>(ap_buf), size);
  m_need_write = true;
}

irs_bool irs::dac_ad5293_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_buf[a_index] & static_cast<irs_u8>(1 << a_bit_index));
}

void irs::dac_ad5293_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
  m_need_write = true;
}

void irs::dac_ad5293_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= static_cast<irs_u8>((1 << a_bit_index)^0xFF);
  m_need_write = true;
}

void irs::dac_ad5293_t::tick()
{
  write_tick();

  switch (m_status)
  {
    case FREE:
    {
      if (m_need_write && write_ready() && spi_ready())
      {
        mp_spi->lock();
        configure_spi();
        m_need_write = false;
        m_status = UNLOCK;
        write_to_dac(UNLOCK);
      }
      break;
    }
    case UNLOCK:
    {
      if (write_ready())
      {
        m_status = RESISTANCE;
        write_to_dac(RESISTANCE);
      }
      break;
    }
    case RESISTANCE:
    {
      if (write_ready())
      {
        m_status = HIZ;
        write_to_dac(HIZ);
      }
      break;
    }
    case HIZ:
    {
      if (write_ready())
      {
        m_status = ZERO;
        write_to_dac(ZERO);
      }
      break;
    }
    case ZERO:
    {
      if (write_ready())
      {
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_status = FREE;
      }
      break;
    }
    case RESET:
    {
      IRS_LIB_ERROR(ec_standard, "m_status == RESET недопустимо в "
        "irs::dac_ad5293_t::tick");
    }
  }
}

bool irs::dac_ad5293_t::write_to_dac(status_t a_command)
{
  if (write_ready())
  {
    switch (a_command)
    {
      case RESET:
      {
        mp_write_buf[0] = (0<<C3)|(1<<C2)|(0<<C1)|(0<<C0);
        mp_write_buf[1] = 0;
        break;
      }
      case UNLOCK:
      {
        mp_write_buf[0] = (0<<C3)|(1<<C2)|(1<<C1)|(0<<C0);
        mp_write_buf[1] = (1<<Control2)|(1<<Control1);
        break;
      }
      case RESISTANCE:
      {
        mp_write_buf[0] = mp_buf[1];
        mp_write_buf[1] = mp_buf[0];
        mp_write_buf[0] &=
          static_cast<irs_u8>(((1<<Z5)|(1<<Z4)|(1<<C3)|(1<<C2)|(1<<C1))^0xFF);
        mp_write_buf[0] |= static_cast<irs_u8>(1<<C0);
        break;
      }
      case HIZ:
      {
        mp_write_buf[0] = (1<<Z5);
        mp_write_buf[1] = (1<<HI);
        break;
      }
      case ZERO:
      {
        mp_write_buf[0] = 0;
        mp_write_buf[1] = 0;
        break;
      }
      case FREE:
      {
        IRS_LIB_ERROR(ec_standard, "a_command == FREE недопустимо в "
          "irs::dac_ad5293_t::write_to_dac");
      }
    }
    mp_cs_pin->clear();
    m_wait_timer.start();
    m_write_status = WS_DOWN_CS;
    return true;
  }
  return false;
}

void irs::dac_ad5293_t::write_tick()
{
  mp_spi->tick();
  switch (m_write_status)
  {
    case WS_READY:
    {
      break;
    }
    case WS_DOWN_CS:
    {
      if (m_wait_timer.check())
      {
        mp_spi->write(mp_write_buf, m_size);
        m_write_status = WS_WRITE;
      }
      break;
    }
    case WS_WRITE:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        m_wait_timer.start();
        m_write_status = WS_UP_CS;
      }
      break;
    }
    case WS_UP_CS:
    {
      if (m_wait_timer.check())
      {
        mp_cs_pin->set();
        m_wait_timer.start();
        m_write_status = WS_PAUSE;
      }
      break;
    }
    case WS_PAUSE:
    {
      if (m_wait_timer.check())
      {
        m_write_status = WS_READY;
      }
      break;
    }
  }
}

void irs::dac_ad5293_t::configure_spi()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::NEGATIVE_POLARITY);//RISING_EDGE);
  mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
}

// class simple_dac_ad5293_t
irs::simple_dac_ad5293_t::simple_dac_ad5293_t(spi_t* ap_spi,
  gpio_pin_t* ap_cs_pin):
  m_dac_ad5293(ap_spi, ap_cs_pin),
  m_dac_ad5293_data(&m_dac_ad5293)
{
}

size_t irs::simple_dac_ad5293_t::get_resolution() const
{
  return dac_resulution;
}

void irs::simple_dac_ad5293_t::set_u32_data(
  size_t a_channel, const irs_u32 a_data)
{
  if (a_channel > 0) {
    IRS_LIB_ERROR(ec_standard, "Нет канала с таким номером");
  }
  m_dac_ad5293_data.resistance_code =
    static_cast<irs_u16>(a_data >> (32 - dac_resulution));
}

void irs::simple_dac_ad5293_t::tick()
{
  m_dac_ad5293.tick();
}

//--------------------------- AD5791 -------------------------------------------
irs::dac_ad5791_t::dac_ad5791_t(
  spi_t *ap_spi,
  gpio_pin_t *ap_cs_pin,
  gpio_pin_t *ap_ldac_pin,
  gpio_pin_t *ap_clr_pin,
  gpio_pin_t *ap_reset_pin,
  irs_u32 a_default_value):
  m_status(st_reset),
  m_target_status(st_free),
  mp_spi(ap_spi),
  m_timer(irs::make_cnt_ms(m_reset_interval)),
  m_need_write_options(true),
  m_need_write_voltage_code(true),
  mp_cs_pin(ap_cs_pin),
  mp_ldac_pin(ap_ldac_pin),
  mp_clr_pin(ap_clr_pin),
  mp_reset_pin(ap_reset_pin)
{
  memset(mp_buf, 0, m_size);
  memset(mp_write_buf, 0, m_write_buf_size);

  mp_cs_pin->set();
  mp_ldac_pin->clear();
  mp_clr_pin->clear();
  mp_reset_pin->set();

  m_timer.start();

  mp_buf[m_options_pos] =
    (0 << m_rbuf_bit_pos) |//1
    (1 << m_opgnd_bit_pos) |
    (1 << m_dactri_bit_pos) |
    (0 << m_bin2sc_bit_pos) |
    (1 << m_sdodis_bit_pos);
  mp_buf[m_lin_comp_pos] = 0;
  irs_u32* voltage_code =
    reinterpret_cast<irs_u32*>(&mp_buf[m_voltage_code_pos]);
  *voltage_code = a_default_value;
}

irs_uarc irs::dac_ad5791_t::size()
{
  return m_size;
}

irs_bool irs::dac_ad5791_t::connected()
{
  return true;
}

void irs::dac_ad5791_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  bool valid_data = (a_index < m_size) && (a_index != m_status_pos) && ap_buf;
  if (valid_data) {
    irs_u8 size = static_cast<irs_u8>(a_size);
    irs_u8 index = static_cast<irs_u8>(a_index);
    if (size + index > m_size) size = static_cast<irs_u8>(m_size - index);
    memcpy(
      reinterpret_cast<void*>(mp_buf + index),
      reinterpret_cast<const void*>(ap_buf),
      size);
    if (index == m_options_pos || index == m_lin_comp_pos) {
      m_need_write_options = true;
    }
    if (index + size >= m_voltage_code_pos) {
      m_need_write_voltage_code = true;
    }
    mp_buf[m_status_pos] &= ~(1 << m_ready_bit_pos);
  }
  return;
}

void irs::dac_ad5791_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  bool valid_data = (a_index < m_size) && ap_buf;
  if (valid_data) {
    irs_u8 size = static_cast<irs_u8>(a_size);
    irs_u8 index = static_cast<irs_u8>(a_index);
    if (size + index > m_size) size = static_cast<irs_u8>(m_size - index);
    memcpy(
      reinterpret_cast<void*>(ap_buf),
      reinterpret_cast<void*>(mp_buf + a_index),
      size);
  }
  return;
}

irs_bool irs::dac_ad5791_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  bool valid_data = (a_index < m_size) & (a_bit_index <= 7);
  irs_bool return_value = false;
  if (valid_data) {
    return_value =  (mp_buf[a_index] & static_cast<irs_u8>(1 << a_bit_index));
  }
  return return_value;
}

void irs::dac_ad5791_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  bool valid_data =
    (a_index < m_size) & (a_index != m_status_pos) & (a_bit_index <= 7);
  if (valid_data) {
    irs_u8 index = static_cast<irs_u8>(a_index);
    mp_buf[a_index] |= static_cast<irs_u8>(1 << a_bit_index);
    if (index == m_options_pos || index == m_lin_comp_pos) {
      m_need_write_options = true;
    }
    if (index >= m_voltage_code_pos) {
      m_need_write_voltage_code = true;
    }
    mp_buf[m_status_pos] &= ~(1 << m_ready_bit_pos);
  }
  return;
}

void irs::dac_ad5791_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  bool valid_data =
    (a_index < m_size) & (a_index != m_status_pos) & (a_bit_index <= 7);
  if (valid_data) {
    irs_u8 index = static_cast<irs_u8>(a_index);
    mp_buf[a_index] = static_cast<irs_u8>
      (mp_buf[a_index] & ~static_cast<irs_u8>(1 << a_bit_index));
    if (index == m_options_pos || index == m_lin_comp_pos) {
      m_need_write_options = true;
    }
    if (index >= m_voltage_code_pos) {
      m_need_write_voltage_code = true;
    }
    mp_buf[m_status_pos] &= ~(1 << m_ready_bit_pos);
  }
  return;
}

void irs::dac_ad5791_t::tick()
{
  mp_spi->tick();

  switch (m_status) {
    case st_reset: {
      if (m_timer.check()) {
        mp_reset_pin->clear();
        m_status = st_prepare_options;
      }
      break;
    }
    case st_prepare_options: {
      mp_write_buf[0] = addr_control;
      mp_write_buf[1] =
        static_cast<irs_u8>((mp_buf[m_lin_comp_pos] >> 2) & 0x03);
      mp_write_buf[2] =
        static_cast<irs_u8>(((mp_buf[m_lin_comp_pos] & 0x03) << 6)
        | (mp_buf[m_options_pos] & 0x3E));
      m_need_write_options = false;
      m_status = st_spi_prepare;
      m_target_status = st_free;
      break;
    }
    case st_prepare_voltage_code: {
      irs_u32 voltage_code =
        *reinterpret_cast<irs_u32*>(&mp_buf[m_voltage_code_pos]);
      irs_u8 master_byte =
        static_cast<irs_u8>(voltage_code >> m_master_byte_shift);
      irs_u8 mid_byte    =
        static_cast<irs_u8>(voltage_code >> m_mid_byte_shift);
      irs_u8 least_byte  =
        static_cast<irs_u8>(voltage_code >> m_least_byte_shift);
      mp_write_buf[0] = static_cast<irs_u8>(addr_code | master_byte);
      mp_write_buf[1] = mid_byte;
      mp_write_buf[2] = least_byte;
      m_need_write_voltage_code = false;
      m_status = st_spi_prepare;
      m_target_status = st_ldac_clear;
      break;
    }
    case st_spi_prepare: {
      if ((mp_spi->get_status() == irs::spi_t::FREE) && !mp_spi->get_lock()) {
        mp_spi->set_order(irs::spi_t::MSB);
        mp_spi->set_polarity(irs::spi_t::NEGATIVE_POLARITY);
        mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
        mp_spi->lock();
        mp_cs_pin->clear();
        mp_spi->write(mp_write_buf, m_write_buf_size);
        m_status = st_spi_wait;
      }
      break;
    }
    case st_spi_wait: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_status = m_target_status;
      }
      break;
    }
    case st_ldac_clear: {
      mp_ldac_pin->set();
      m_timer.set(irs::make_cnt_mcs(10));
      m_timer.start();
      m_status = st_ldac_set;
      break;
    }
    case st_ldac_set: {
      if (m_timer.check()) {
        mp_ldac_pin->clear();
        m_status = st_free;
      }
      break;
    }
    case st_free: {
      if (m_need_write_options == true) {
        m_status = st_prepare_options;
      } else if (m_need_write_voltage_code == true) {
        m_status = st_prepare_voltage_code;
      } else {
        mp_buf[m_status_pos] |= (1 << m_ready_bit_pos);
      }
      break;
    }
  }
}

//-------------------------- AD5686 -------------------------------------------

// class dac_ad5686_t
irs::dac_ad5686_t::dac_ad5686_t(spi_t* ap_spi, gpio_pin_t* ap_cs_pin):
  mp_spi(ap_spi),
  mp_cs_pin(ap_cs_pin),
  m_channels(4),
  m_current_channel(0),
  m_process(process_reset)
{
  memset(reinterpret_cast<void*>(mp_buf), 0, buf_size);
  mp_cs_pin->set();


  /*mp_buf[0] = (command_software_reset << 4) | 0;
  while (mp_spi->get_lock() || (mp_spi->get_status() != irs::spi_t::FREE)) {
    mp_spi->tick();
  }
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::NEGATIVE_POLARITY);
  mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
  mp_cs_pin->clear();
  mp_spi->lock();
  mp_spi->write(mp_buf, buf_size);

  while (mp_spi->get_status() != irs::spi_t::FREE) {
    mp_spi->tick();
  }
  mp_cs_pin->set();
  mp_spi->unlock();          */
}

irs::dac_ad5686_t::~dac_ad5686_t()
{
}

irs_status_t irs::dac_ad5686_t::get_status() const
{
  if (m_process == process_request) {
    for (size_type i = 0; i < m_channels.size(); i++) {
      if (m_channels[i].new_value_exists) {
        return irs_st_busy;
      }
    }
    return irs_st_ready;
  }
  return irs_st_busy;
}

size_t irs::dac_ad5686_t::get_resolution() const
{
  return resolution;
}

void irs::dac_ad5686_t::set_u32_data(size_t a_channel, const irs_u32 a_data)
{
  m_channels[a_channel].value =
    static_cast<sample_type>(a_data >> (32 - resolution));
  m_channels[a_channel].new_value_exists = true;
}

void irs::dac_ad5686_t::tick()
{
  mp_spi->tick();
  switch (m_process) {
    case process_reset: {
      if (!mp_spi->get_lock() && (mp_spi->get_status() == irs::spi_t::FREE)) {
        mp_buf[0] = (command_software_reset << command_shift) | 0;
        configure_spi();
        mp_cs_pin->clear();
        mp_spi->lock();
        mp_spi->write(mp_buf, buf_size);
        m_process = process_write;
      }
    } break;
    case process_request: {
      if (m_channels[m_current_channel].new_value_exists) {
        m_channels[m_current_channel].new_value_exists = false;
        irs_u8 write_to_and_update_channel = 3;
        irs_u8 address = static_cast<irs_u8>(1 << m_current_channel);
        irs_u8 header = static_cast<irs_u8>(
          (write_to_and_update_channel << command_shift) | address);
        mp_buf[0] = header;
        irs_u16 value = m_channels[m_current_channel].value;
        irs_u16 dac_value = 0;
        IRS_HIBYTE(dac_value) = IRS_FIRSTBYTE(value);
        IRS_LOBYTE(dac_value) = IRS_SECONDBYTE(value);
        reinterpret_cast<irs_u16&>(mp_buf[1]) = dac_value;
        m_process = process_free;
      }
      m_current_channel++;
      if (m_current_channel >= m_channels.size()) {
        m_current_channel = 0;
      }
    } break;
    case process_free: {
      if (!mp_spi->get_lock() && (mp_spi->get_status() == irs::spi_t::FREE)) {
        mp_cs_pin->clear();
        mp_spi->lock();
        configure_spi();
        mp_spi->write(mp_buf, buf_size);
        m_process = process_write;
      }
    } break;
    case process_write: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        m_process = process_request;
      }
    } break;
  }
}

void irs::dac_ad5686_t::configure_spi()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::NEGATIVE_POLARITY);
  mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
}

//-------------------------- AD7794 -------------------------------------------
irs::adc_ad7794_t::adc_ad7794_t(spi_t *ap_spi,
  gpio_pin_t *ap_cs_pin):
  mp_spi(ap_spi),
  mp_cs_pin(ap_cs_pin),
  m_status(meas_status_busy),
  m_mode(mode_free),
  m_spi_transaction_size(0),
  m_reg(),
  m_read_all_reg(false),
  m_count_init(0),
  m_shift(0),
  m_read_data(false),
  m_conv_time_vector(),
  m_freq(0),
  m_reserved_interval(1),
  m_timer(make_cnt_ms(m_reserved_interval)),
  m_ready_wait(false),
  m_get_data(false),
  m_value(0),
  m_read_mode(om_continuous),
  m_cur_read_mode(om_continuous),
  mp_value_param(IRS_NULL),
  m_read_calibration_coeff(false)
{
  memset(mp_buf, 0, m_size_buf);
  memset(mp_spi_buf, 0xFF, m_write_buf_size);

  m_conv_time_vector.push_back(m_reserved_interval);    //  0:  reserve
  m_conv_time_vector.push_back(to_int(1000/470.0 + 1)); //  1:  470 Hz
  m_conv_time_vector.push_back(to_int(1000/242.0 + 1)); //  2:  242 Hz
  m_conv_time_vector.push_back(to_int(1000/123.0 + 1)); //  3:  123 Hz
  m_conv_time_vector.push_back(to_int(1000/62.0 + 1));  //  4:  62 Hz
  m_conv_time_vector.push_back(to_int(1000/50.0 + 1));  //  5:  50 Hz
  m_conv_time_vector.push_back(to_int(1000/39.0 + 1));  //  6:  39 Hz
  m_conv_time_vector.push_back(to_int(1000/33.2 + 1));  //  7:  33.2 Hz
  m_conv_time_vector.push_back(to_int(1000/19.6 + 1));  //  8:  19.6 Hz
  m_conv_time_vector.push_back(to_int(1000/16.7 + 1));  //  9:  16.7 Hz
  m_conv_time_vector.push_back(to_int(1000/16.7 + 1));  // 10:  16.7 Hz
  m_conv_time_vector.push_back(to_int(1000/12.5 + 1));  // 11:  12.5 Hz
  m_conv_time_vector.push_back(to_int(1000/10.0 + 1));  // 12:  10 Hz
  m_conv_time_vector.push_back(to_int(1000/8.33 + 1));  // 13:  8.33 Hz
  m_conv_time_vector.push_back(to_int(1000/6.25 + 1));  // 14:  6.25 Hz
  m_conv_time_vector.push_back(to_int(1000/4.17 + 1));  // 15:  4.17 Hz

  m_reg.push_back(reg_t(m_reg_status_index, m_reg_status_size));
  m_reg.push_back(reg_t(m_reg_mode_index, m_reg_mode_size));
  m_reg.push_back(reg_t(m_reg_conf_index, m_reg_conf_size));
  m_reg.push_back(reg_t(m_reg_data_index, m_reg_data_size));
  m_reg.push_back(reg_t(m_reg_id_index, m_reg_id_size));
  m_reg.push_back(reg_t(m_reg_io_index, m_reg_io_size));
  m_reg.push_back(reg_t(m_reg_offs_index, m_reg_offs_size));
  m_reg.push_back(reg_t(m_reg_fs_index, m_reg_fs_size));

  mp_cs_pin->set();

  m_mode = mode_spi_rw;
  m_read_all_reg = true;
  m_spi_transaction_size = m_write_buf_size;
}
irs::adc_ad7794_t::~adc_ad7794_t()
{
}
void irs::adc_ad7794_t::start()
{
  m_cur_read_mode = m_read_mode;
  creation_reg(m_reg[m_reg_mode_index], m_cur_read_mode,
    m_mode_byte_pos, m_mode_pos, m_mode_size);
  switch(m_cur_read_mode) {
    case om_idle:
    case om_power_down: {
      m_read_data = false;
    } break;
    default: {
      m_read_data = true;
    } break;
  }
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7794_t::stop()
{
  set_mode(adc_mode_power_down);
  start();
}
meas_status_t irs::adc_ad7794_t::status() const
{
  return m_status;
}
void irs::adc_ad7794_t::set_channel(int a_channel)
{
  creation_reg(m_reg[m_reg_conf_index], a_channel,
    m_ch_byte_pos, m_ch_pos, m_ch_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7794_t::set_mode(int a_mode)
{
  m_read_mode = static_cast<operating_modes_t>(a_mode);
}
void irs::adc_ad7794_t::set_freq(int a_freq)
{
  m_freq = static_cast<irs_u8>(a_freq);
  creation_reg(m_reg[m_reg_mode_index], m_freq,
    m_freq_byte_pos, m_freq_pos, m_freq_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7794_t::set_gain(int a_gain)
{
  creation_reg(m_reg[m_reg_conf_index], a_gain,
    m_gain_byte_pos, m_gain_pos, m_gain_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7794_t::set_buf(int a_buf)
{
  creation_reg(m_reg[m_reg_conf_index], a_buf,
    m_buf_byte_pos, m_buf_pos, m_buf_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7794_t::set_ref_det(int a_ref_det)
{
  creation_reg(m_reg[m_reg_conf_index], a_ref_det,
    m_ref_det_byte_pos, m_ref_det_pos, m_ref_det_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7794_t::set_ub(int a_ub)
{
  creation_reg(m_reg[m_reg_conf_index], a_ub,
    m_ub_byte_pos, m_ub_pos, m_ub_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7794_t::set_bo(int a_bo)
{
  creation_reg(m_reg[m_reg_conf_index], a_bo,
    m_bo_byte_pos, m_bo_pos, m_bo_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
irs_i32 irs::adc_ad7794_t::get_value()
{
  if ((m_cur_read_mode == om_continuous) && m_read_data) {
    m_status = meas_status_busy;
  }
  return m_value;
}

void irs::adc_ad7794_t::set_param(adc_param_t a_param, const int a_value)
{
  switch(a_param) {
    case adc_mode: {
      set_mode(a_value);
    } break;
    case adc_gain: {
      set_gain(a_value);
    } break;
    case adc_channel: {
      set_channel(a_value);
    } break;
    case adc_freq: {
      set_freq(a_value);
    } break;
    case adc_buf: {
      set_buf(a_value);
    } break;
    case adc_ref_det: {
      set_ref_det(a_value);
    } break;
    case adc_unipolar: {
      set_ub(a_value);
    } break;
    case adc_burnout: {
      set_bo(a_value);
    } break;
    default: {
      IRS_ASSERT_MSG("Попытка установить несуществующий параметр");
    } break;
  }
}

void irs::adc_ad7794_t::get_param(adc_param_t a_param, int* ap_value)
{
  mp_value_param = ap_value;
  switch(a_param) {
    case adc_mode: {
      *mp_value_param = m_read_mode;
    } break;
    case adc_gain: {
      *mp_value_param = get(m_reg[m_reg_conf_index],
        m_gain_byte_pos, m_gain_pos, m_gain_size);
    } break;
    case adc_channel: {
      *mp_value_param = get(m_reg[m_reg_conf_index],
        m_ch_byte_pos, m_ch_pos, m_ch_size);
    } break;
    case adc_freq: {
      *mp_value_param = get(m_reg[m_reg_mode_index],
        m_freq_byte_pos, m_freq_pos, m_freq_size);
    } break;
    case adc_buf: {
      *mp_value_param = get(m_reg[m_reg_conf_index],
        m_buf_byte_pos, m_buf_pos, m_buf_size);
    } break;
    case adc_ref_det: {
      *mp_value_param = get(m_reg[m_reg_conf_index],
        m_ref_det_byte_pos, m_ref_det_pos, m_ref_det_size);
    } break;
    case adc_unipolar: {
      *mp_value_param = get(m_reg[m_reg_conf_index],
        m_ub_byte_pos, m_ub_pos, m_ub_size);
    } break;
    case adc_burnout: {
      *mp_value_param = get(m_reg[m_reg_conf_index],
        m_bo_byte_pos, m_bo_pos, m_bo_size);
    } break;
    case adc_offset: {
      m_read_data = false;
      m_read_calibration_coeff = true;
      creation_reg_comm(m_reg[m_reg_offs_index], tt_read);
      m_mode = mode_spi_rw;
      m_status = meas_status_busy;
    } break;
    case adc_full_scale: {
      m_read_data = false;
      m_read_calibration_coeff = true;
      creation_reg_comm(m_reg[m_reg_fs_index], tt_read);
      m_mode = mode_spi_rw;
      m_status = meas_status_busy;
    } break;
    default: {
      IRS_ASSERT_MSG("Попытка считать несуществующий параметр");
    } break;
  }
}
void irs::adc_ad7794_t::tick()
{
  mp_spi->tick();
  switch(m_mode) {
    case mode_free: {
    } break;
    case mode_spi_rw: {
      if ((mp_spi->get_status() == irs::spi_t::FREE) && !mp_spi->get_lock()) {
        spi_prepare();
        mp_spi->read_write(mp_spi_buf, mp_spi_buf, m_spi_transaction_size);
        if (m_read_data) {
          m_mode = mode_read_wait;
        } else {
          m_mode = mode_spi_rw_wait;
        }
      }
    } break;
    case mode_read_wait: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        spi_release();
        if (m_ready_wait) {
          m_mode = mode_ready_wait;
        } else if (m_get_data) {
          m_mode = mode_get_data;
        } else {
          m_mode = mode_read_data;
          m_timer.set(make_cnt_ms
            (2*m_conv_time_vector[m_freq] + m_reserved_interval));
        }
      }
    } break;
    case mode_ready_wait: {
      bool is_unready =
        (mp_spi_buf[m_reg_comm_size] &
        static_cast<irs_u8>(1 << m_ready_pos));
      if (!is_unready) {
        m_ready_wait = false;
        switch(m_cur_read_mode) {
          case om_internal_zero_scale:
          case om_internal_full_scale:
          case om_system_zero_scale:
          case om_system_full_scale: {
            m_mode = mode_free;
            m_status = meas_status_success;
          } break;
          case om_continuous:
          case om_single: {
            creation_reg_comm(m_reg[m_reg_data_index], tt_read);
            m_mode = mode_spi_rw;
            m_get_data = true;
          } break;
          default: {
            IRS_ASSERT_MSG("Проверка статуса в "
              "недопустимом для этого режиме ацп");
          } break;
        }
      } else {
        m_mode = mode_read_data;
        m_timer.set(make_cnt_ms(2*m_conv_time_vector[m_freq]));
      }
    } break;
    case mode_get_data: {
      m_get_data = false;
      if (m_cur_read_mode == om_single) {
        m_mode = mode_free;
        //m_read_data = false;
      } else if (m_cur_read_mode == om_continuous) {
        m_mode = mode_read_data;
        m_timer.set(make_cnt_ms(m_conv_time_vector[m_freq]));
      } else {
        IRS_ASSERT_MSG("Обработка данных в недопустимом для этого режиме ацп");
      }
      m_value = conversion_spi_value();
      m_status = meas_status_success;
    } break;
    case mode_read_data: {
      if (m_timer.check()) {
        m_ready_wait = true;
        creation_reg_comm(m_reg[m_reg_status_index], tt_read);
        m_mode = mode_spi_rw;
      }
    } break;
    case mode_spi_rw_wait: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        spi_release();
        if (m_read_all_reg) {
          m_mode = mode_read_all_reg;
        } else {
          if (m_read_calibration_coeff) {
            m_read_calibration_coeff = false;
            *mp_value_param = conversion_spi_value();
          }
          m_mode = mode_free;
          m_status = meas_status_success;
        }
      }
    } break;
    case mode_read_all_reg: {
      if (m_count_init != 0) {
        memcpy(mp_buf + m_shift,
          mp_spi_buf + m_reg_comm_size, m_reg[m_count_init-1].size);
        m_shift += m_reg[m_count_init-1].size;
      }
      if (m_count_init < m_reg.size()) {
        if (m_reg[m_count_init].index != m_reg_data_index) {
          creation_reg_comm(m_reg[m_count_init], tt_read);
          m_mode = mode_spi_rw;
        }
        m_count_init++;
      } else {
        m_read_all_reg = false;
        set_gain(0);
      }
    } break;
  }
  if (m_spi_transaction_size > m_write_buf_size) {
    IRS_ASSERT_MSG("Размер передаваемых данных в spi превысил размер буфера");
  }
}
void irs::adc_ad7794_t::spi_prepare()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::POSITIVE_POLARITY);
  mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
  mp_spi->lock();
  mp_cs_pin->clear();
}
void irs::adc_ad7794_t::spi_release()
{
  mp_cs_pin->set();
  mp_spi->reset_configuration();
  mp_spi->unlock();
}

void irs::adc_ad7794_t::creation_reg_comm(reg_t a_reg,
  transaction_type_t a_tt)
{
  memset(mp_spi_buf, 0, m_write_buf_size);
  mp_spi_buf[m_reg_comm_index] |=
    static_cast<irs_u8>(a_reg.index << m_rs_pos);
  switch(a_tt) {
    case tt_read: {
      mp_spi_buf[m_reg_comm_index] |= static_cast<irs_u8>(1 << m_rw_pos);
    } break;
    case tt_write: {
      mp_spi_buf[m_reg_comm_index] &= ~static_cast<irs_u8>(1 << m_rw_pos);
    } break;
    default: {
      IRS_ASSERT_MSG("Недопустимый transaction_type");
    } break;
  }
  m_spi_transaction_size = a_reg.size + m_reg_comm_size;
}

int irs::adc_ad7794_t::calculation_shift(reg_t a_reg)
{
  int shift = 0;
  for(int i = 0; i < a_reg.index; i++) {
    shift += m_reg[i].size;
  }
  return shift;
}
int irs::adc_ad7794_t::calculation_number_byte(reg_t a_reg,
  param_byte_pos_t byte_pos)
{
  //из за того что обратный порядок байт
  int number_byte = (a_reg.size*8 - 1 - byte_pos*8)/8;
  return number_byte;
}

int irs::adc_ad7794_t::to_int(double a_number)
{
  return static_cast<int>(ceil(a_number));
}

int irs::adc_ad7794_t::filling_units(param_size_t a_size)
{
  return static_cast<int>(pow(2.0, a_size) - 1);
}

void irs::adc_ad7794_t::creation_reg(reg_t a_reg, int a_value,
  param_byte_pos_t a_byte_pos, param_pos_t a_pos,
  param_size_t a_size)
{
  m_read_data = false;
  creation_reg_comm(a_reg, tt_write);
  int shift = calculation_shift(a_reg);
  int number_byte = calculation_number_byte(a_reg,
    a_byte_pos);
  mp_buf[shift + number_byte] = static_cast<irs_u8>(
    mp_buf[shift + number_byte] &
    ~static_cast<irs_u8>(filling_units(a_size) << a_pos));
  mp_buf[shift + number_byte]  |= static_cast<irs_u8>(a_value << a_pos);
  for (int i = 0; i < a_reg.size; i++) {
    mp_spi_buf[i + m_reg[m_reg_comm_index].size] =
      mp_buf[shift + i];
  }
}

int irs::adc_ad7794_t::get(reg_t a_reg, param_byte_pos_t a_byte_pos,
  param_pos_t a_pos, param_size_t a_size)
{
  int shift = calculation_shift(a_reg);
  int number_byte = calculation_number_byte(a_reg,
    a_byte_pos);
  irs_u8 value_shift =
    static_cast<irs_u8>(mp_buf[shift + number_byte] >> a_pos);
  irs_u8 mask = static_cast<irs_u8>(filling_units(a_size));
  int value = value_shift & mask;
  return value;
}

irs_i32 irs::adc_ad7794_t::conversion_spi_value()
{
  mp_get_buff[2] = mp_spi_buf[1];
  mp_get_buff[1] = mp_spi_buf[2];
  mp_get_buff[0] = mp_spi_buf[3];
  irs_i32 value = *reinterpret_cast<irs_i32*>(mp_get_buff);
  int unipolar =
    get(m_reg[m_reg_conf_index], m_ub_byte_pos, m_ub_pos, m_ub_size);
  if (unipolar == 1) {
    value = (value << 7);
  } else {
    value = (value << 8);
    value = value - 0x80000000;
  }
  return value;
}

// class cyclic_adc_ad7794_t
irs::cyclic_adc_ad7794_t::cyclic_adc_ad7794_t(
  spi_t* ap_spi, gpio_pin_t* ap_cs_pin,
  select_channel_type a_selected_channels,
  counter_t a_read_delay,
  bool a_unipolar,
  freq_t a_freq
):
  m_adc(ap_spi, ap_cs_pin),
  m_params(),
  m_current_param(0),
  m_channels(),
  m_current_channel(0),
  m_process(process_set_param),
  m_timer(a_read_delay),
  m_unipolar(a_unipolar)
{
  //select_channel_type channel_name = ch_first;

  vector<channel_name_t> names;
  names.push_back(ch_1);
  names.push_back(ch_2);
  names.push_back(ch_3);
  names.push_back(ch_4);
  names.push_back(ch_5);
  names.push_back(ch_6);
  names.push_back(ch_temp_sensor);
  names.push_back(ch_avdd_monitor);
  names.push_back(ch_0);

  for (size_type i = 0; i < names.size(); i++) {
    if (a_selected_channels & names[i]) {
      channel_t channel;
      channel.index = i;
      m_channels.push_back(channel);
    }
  }

  const int unipolar_code = a_unipolar ? 1 : 0;
  m_params.push_back(parameter_t(irs::adc_unipolar, unipolar_code));
  m_params.push_back(parameter_t(irs::adc_gain, 0));
  m_params.push_back(parameter_t(irs::adc_mode,
    irs::adc_mode_single_conversion));
  m_params.push_back(parameter_t(irs::adc_freq, a_freq));
}

irs::cyclic_adc_ad7794_t::size_type
irs::cyclic_adc_ad7794_t::get_resulution() const
{
  return adc_resolution;
}

bool irs::cyclic_adc_ad7794_t::new_value_exists(irs_u8 a_channel) const
{
  if (static_cast<size_type>(a_channel) >= m_channels.size()) {
    IRS_LIB_ERROR(ec_standard, "Нет канала с таким номером");
  }
  return m_channels[a_channel].new_value_exists;
}

irs_u32 irs::cyclic_adc_ad7794_t::get_u32_data(irs_u8 a_channel)
{
  if (static_cast<size_type>(a_channel) >= m_channels.size()) {
    IRS_LIB_ERROR(ec_standard, "Нет канала с таким номером");
  }
  m_channels[a_channel].new_value_exists = false;
  return m_channels[a_channel].value;
}

void irs::cyclic_adc_ad7794_t::tick()
{
  m_adc.tick();
  switch (m_process) {
    case process_set_param: {
      if (m_adc.status() == meas_status_success) {
        if (m_current_param < m_params.size()) {
          const parameter_t param = m_params[m_current_param];
          m_adc.set_param(param.name, param.value);
          m_current_param++;
        } else {
          m_process = process_select_channel;
        }
      }
    } break;
    case process_select_channel: {
      if (m_timer.check()) {
        if (!m_channels.empty()) {
          const channel_t& channel = m_channels[m_current_channel];
          m_adc.set_param(adc_channel, channel.index);
          m_process = process_read_value;
        }
      }
    } break;
    case process_read_value: {
      if (m_adc.status() == meas_status_success) {
        m_adc.start();
        m_process = process_wait_read_value;
      }
    } break;
    case process_wait_read_value: {
      if (m_adc.status() == meas_status_success) {
        if (m_unipolar == 1) {
          m_channels[m_current_channel].value = m_adc.get_value() << 1;
        } else {
          m_channels[m_current_channel].value = m_adc.get_value() + 0x80000000u;
        }
        m_channels[m_current_channel].new_value_exists = true;
        if (m_channels.size() > 1) {
          m_current_channel++;
          if (m_current_channel >= m_channels.size()) {
            m_current_channel = 0;
          }
          m_process = process_select_channel;
        } else {
          m_process = process_read_value;
        }
      }
    } break;
  }
}

//--------------------------- AD7799 -------------------------------------------
irs::adc_ad7799_t::adc_ad7799_t(spi_t *ap_spi,
  gpio_pin_t *ap_cs_pin):
  mp_spi(ap_spi),
  mp_cs_pin(ap_cs_pin),
  m_status(meas_status_busy),
  m_mode(mode_free),
  m_spi_transaction_size(0),
  m_reg(),
  m_read_all_reg(false),
  m_count_init(0),
  m_shift(0),
  m_read_data(false),
  m_conv_time_vector(),
  m_freq(0),
  m_reserved_interval(1),
  m_timer(make_cnt_ms(m_reserved_interval)),
  m_ready_wait(false),
  m_get_data(false),
  m_value(0),
  m_read_mode(om_continuous),
  m_cur_read_mode(om_continuous),
  mp_value_param(IRS_NULL),
  m_read_calibration_coeff(false)
{
  memset(mp_buf, 0, m_size_buf);
  memset(mp_spi_buf, 0xFF, m_write_buf_size);

  m_conv_time_vector.push_back(m_reserved_interval);    //  0:  reserve
  m_conv_time_vector.push_back(to_int(1000/500. + 1));  //  1:  500 Hz
  m_conv_time_vector.push_back(to_int(1000/250. + 1));  //  2:  250 Hz
  m_conv_time_vector.push_back(to_int(1000/125. + 1));  //  3:  125 Hz
  m_conv_time_vector.push_back(to_int(1000/62.5 + 1));  //  4:  62.5 Hz
  m_conv_time_vector.push_back(to_int(1000/50.0 + 1));  //  5:  50 Hz
  m_conv_time_vector.push_back(to_int(1000/39.2 + 1));  //  6:  39.2 Hz
  m_conv_time_vector.push_back(to_int(1000/33.3 + 1));  //  7:  33.3 Hz
  m_conv_time_vector.push_back(to_int(1000/19.6 + 1));  //  8:  19.6 Hz
  m_conv_time_vector.push_back(to_int(1000/19.6 + 1));  //  9:  16.7 Hz
  m_conv_time_vector.push_back(to_int(1000/19.6 + 1));  // 10:  19.6 Hz
  m_conv_time_vector.push_back(to_int(1000/12.5 + 1));  // 11:  12.5 Hz
  m_conv_time_vector.push_back(to_int(1000/10.0 + 1));  // 12:  10 Hz
  m_conv_time_vector.push_back(to_int(1000/8.33 + 1));  // 13:  8.33 Hz
  m_conv_time_vector.push_back(to_int(1000/6.25 + 1));  // 14:  6.25 Hz
  m_conv_time_vector.push_back(to_int(1000/4.17 + 1));  // 15:  4.17 Hz

  m_reg.push_back(reg_t(m_reg_status_index, m_reg_status_size));
  m_reg.push_back(reg_t(m_reg_mode_index, m_reg_mode_size));
  m_reg.push_back(reg_t(m_reg_conf_index, m_reg_conf_size));
  m_reg.push_back(reg_t(m_reg_data_index, m_reg_data_size));
  m_reg.push_back(reg_t(m_reg_id_index, m_reg_id_size));
  m_reg.push_back(reg_t(m_reg_io_index, m_reg_io_size));
  m_reg.push_back(reg_t(m_reg_offs_index, m_reg_offs_size));
  m_reg.push_back(reg_t(m_reg_fs_index, m_reg_fs_size));

  mp_cs_pin->set();

  m_mode = mode_spi_rw;
  m_read_all_reg = true;
  m_spi_transaction_size = m_write_buf_size;
}
irs::adc_ad7799_t::~adc_ad7799_t()
{
}
void irs::adc_ad7799_t::start()
{
  m_cur_read_mode = m_read_mode;
  creation_reg(m_reg[m_reg_mode_index], m_cur_read_mode,
    m_mode_byte_pos, m_mode_pos, m_mode_size);
  switch(m_cur_read_mode) {
    case om_idle:
    case om_power_down: {
      m_read_data = false;
    } break;
    default: {
      m_read_data = true;
    } break;
  }
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7799_t::stop()
{
  set_mode(adc_mode_power_down);
  start();
}
meas_status_t irs::adc_ad7799_t::status() const
{
  return m_status;
}
void irs::adc_ad7799_t::set_channel(int a_channel)
{
  creation_reg(m_reg[m_reg_conf_index], a_channel,
    m_ch_byte_pos, m_ch_pos, m_ch_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7799_t::set_mode(int a_mode)
{
  m_read_mode = static_cast<operating_modes_t>(a_mode);
}
void irs::adc_ad7799_t::set_freq(int a_freq)
{
  m_freq = static_cast<irs_u8>(a_freq);
  creation_reg(m_reg[m_reg_mode_index], m_freq,
    m_freq_byte_pos, m_freq_pos, m_freq_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7799_t::set_gain(int a_gain)
{
  creation_reg(m_reg[m_reg_conf_index], a_gain,
    m_gain_byte_pos, m_gain_pos, m_gain_size);
  /*m_read_data = false;
  creation_reg_comm(m_reg[m_reg_conf_index], tt_write);
  int shift = calculation_shift(m_reg[m_reg_conf_index]);
  int number_byte = calculation_number_byte(m_reg[m_reg_conf_index],
    m_gain_byte_pos);
  mp_buf[shift + number_byte]  &= ~static_cast<irs_u8>((0x7) << m_gain_pos);
  mp_buf[shift + number_byte]  |= static_cast<irs_u8>(a_gain << m_gain_pos);
  for (int i = 0; i < m_reg[m_reg_conf_index].size; i++) {
    mp_spi_buf[m_reg_comm_index + i + m_reg[m_reg_comm_index].size] =
      mp_buf[shift + i];
  }*/
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7799_t::set_buf(int a_buf)
{
  creation_reg(m_reg[m_reg_conf_index], a_buf,
    m_buf_byte_pos, m_buf_pos, m_buf_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7799_t::set_ref_det(int a_ref_det)
{
  creation_reg(m_reg[m_reg_conf_index], a_ref_det,
    m_ref_det_byte_pos, m_ref_det_pos, m_ref_det_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7799_t::set_ub(int a_ub)
{
  creation_reg(m_reg[m_reg_conf_index], a_ub,
    m_ub_byte_pos, m_ub_pos, m_ub_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
void irs::adc_ad7799_t::set_bo(int a_bo)
{
  creation_reg(m_reg[m_reg_conf_index], a_bo,
    m_bo_byte_pos, m_bo_pos, m_bo_size);
  m_mode = mode_spi_rw;
  m_status = meas_status_busy;
}
irs_i32 irs::adc_ad7799_t::get_value()
{
  if ((m_cur_read_mode == om_continuous) && m_read_data) {
    m_status = meas_status_busy;
  }
  return m_value;
}

void irs::adc_ad7799_t::set_param(adc_param_t a_param, const int a_value)
{
  switch(a_param) {
    case adc_mode: {
      set_mode(a_value);
    } break;
    case adc_gain: {
      set_gain(a_value);
    } break;
    case adc_channel: {
      set_channel(a_value);
    } break;
    case adc_freq: {
      set_freq(a_value);
    } break;
    case adc_buf: {
      set_buf(a_value);
    } break;
    case adc_ref_det: {
      set_ref_det(a_value);
    } break;
    case adc_unipolar: {
      set_ub(a_value);
    } break;
    case adc_burnout: {
      set_bo(a_value);
    } break;
    default: {
      IRS_ASSERT_MSG("Попытка установить несуществующий параметр");
    } break;
  }
}

void irs::adc_ad7799_t::get_param(adc_param_t a_param, int* ap_value)
{
  mp_value_param = ap_value;
  switch(a_param) {
    case adc_mode: {
      /**mp_value_param = get(m_reg[m_reg_mode_index],
        m_mode_byte_pos, m_mode_pos, m_mode_size);*/
      *mp_value_param = m_read_mode;
    } break;
    case adc_gain: {
      *mp_value_param = get(m_reg[m_reg_conf_index],
        m_gain_byte_pos, m_gain_pos, m_gain_size);
    } break;
    case adc_channel: {
      *mp_value_param = get(m_reg[m_reg_conf_index],
        m_ch_byte_pos, m_ch_pos, m_ch_size);
    } break;
    case adc_freq: {
      *mp_value_param = get(m_reg[m_reg_mode_index],
        m_freq_byte_pos, m_freq_pos, m_freq_size);
    } break;
    case adc_buf: {
      *mp_value_param = get(m_reg[m_reg_conf_index],
        m_buf_byte_pos, m_buf_pos, m_buf_size);
    } break;
    case adc_ref_det: {
      *mp_value_param = get(m_reg[m_reg_conf_index],
        m_ref_det_byte_pos, m_ref_det_pos, m_ref_det_size);
    } break;
    case adc_unipolar: {
      *mp_value_param = get(m_reg[m_reg_conf_index],
        m_ub_byte_pos, m_ub_pos, m_ub_size);
    } break;
    case adc_burnout: {
      *mp_value_param = get(m_reg[m_reg_conf_index],
        m_bo_byte_pos, m_bo_pos, m_bo_size);
    } break;
    case adc_offset: {
      m_read_data = false;
      m_read_calibration_coeff = true;
      creation_reg_comm(m_reg[m_reg_offs_index], tt_read);
      m_mode = mode_spi_rw;
      m_status = meas_status_busy;
    } break;
    case adc_full_scale: {
      m_read_data = false;
      m_read_calibration_coeff = true;
      creation_reg_comm(m_reg[m_reg_fs_index], tt_read);
      m_mode = mode_spi_rw;
      m_status = meas_status_busy;
    } break;
    default: {
      IRS_ASSERT_MSG("Попытка считать несуществующий параметр");
    } break;
  }
}
void irs::adc_ad7799_t::tick()
{
  mp_spi->tick();
  switch(m_mode) {
    case mode_free: {
    } break;
    case mode_spi_rw: {
      if ((mp_spi->get_status() == irs::spi_t::FREE) && !mp_spi->get_lock()) {
        spi_prepare();
        mp_spi->read_write(mp_spi_buf, mp_spi_buf, m_spi_transaction_size);
        if (m_read_data) {
          m_mode = mode_read_wait;
        } else {
          m_mode = mode_spi_rw_wait;
        }
      }
    } break;
    case mode_read_wait: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        spi_release();
        if (m_ready_wait) {
          m_mode = mode_ready_wait;
        } else if (m_get_data) {
          m_mode = mode_get_data;
        } else {
          m_mode = mode_read_data;
          m_timer.set(make_cnt_ms
            (2*m_conv_time_vector[m_freq] + m_reserved_interval));
        }
      }
    } break;
    case mode_ready_wait: {
      bool is_unready =
        (mp_spi_buf[m_reg_comm_size] &
        static_cast<irs_u8>(1 << m_ready_pos));
      if (!is_unready) {
        m_ready_wait = false;
        switch(m_cur_read_mode) {
          case om_internal_zero_scale:
          case om_internal_full_scale:
          case om_system_zero_scale:
          case om_system_full_scale: {
            m_mode = mode_free;
            m_status = meas_status_success;
          } break;
          case om_continuous:
          case om_single: {
            creation_reg_comm(m_reg[m_reg_data_index], tt_read);
            m_mode = mode_spi_rw;
            m_get_data = true;
          } break;
          default: {
            IRS_ASSERT_MSG("Проверка статуса в "
              "недопустимом для этого режиме ацп");
          } break;
        }
      } else {
        m_mode = mode_read_data;
        m_timer.set(make_cnt_ms(2*m_conv_time_vector[m_freq]));
      }
    } break;
    case mode_get_data: {
      m_get_data = false;
      if (m_cur_read_mode == om_single) {
        m_mode = mode_free;
        //m_read_data = false;
      } else if (m_cur_read_mode == om_continuous) {
        m_mode = mode_read_data;
        m_timer.set(make_cnt_ms(m_conv_time_vector[m_freq]));
      } else {
        IRS_ASSERT_MSG("Обработка данных в недопустимом для этого режиме ацп");
      }
      m_value = conversion_spi_value();
      m_status = meas_status_success;
    } break;
    case mode_read_data: {
      if (m_timer.check()) {
        m_ready_wait = true;
        creation_reg_comm(m_reg[m_reg_status_index], tt_read);
        m_mode = mode_spi_rw;
      }
    } break;
    case mode_spi_rw_wait: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        spi_release();
        if (m_read_all_reg) {
          m_mode = mode_read_all_reg;
        } else {
          if (m_read_calibration_coeff) {
            m_read_calibration_coeff = false;
            *mp_value_param = conversion_spi_value();
          }
          m_mode = mode_free;
          m_status = meas_status_success;
        }
      }
    } break;
    case mode_read_all_reg: {
      if (m_count_init != 0) {
        memcpy(mp_buf + m_shift,
          mp_spi_buf + m_reg_comm_size, m_reg[m_count_init-1].size);
        m_shift += m_reg[m_count_init-1].size;
      }
      if (m_count_init < m_reg.size()) {
        if (m_reg[m_count_init].index != m_reg_data_index) {
          creation_reg_comm(m_reg[m_count_init], tt_read);
          m_mode = mode_spi_rw;
        }
        m_count_init++;
      } else {
        m_read_all_reg = false;
        set_gain(0);
      }
    } break;
  }
  if (m_spi_transaction_size > m_write_buf_size) {
    IRS_ASSERT_MSG("Размер передаваемых данных в spi превысил размер буфера");
  }
}
void irs::adc_ad7799_t::spi_prepare()
{
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::POSITIVE_POLARITY);
  mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
  mp_spi->lock();
  mp_cs_pin->clear();
}
void irs::adc_ad7799_t::spi_release()
{
  mp_cs_pin->set();
  mp_spi->reset_configuration();
  mp_spi->unlock();
}

void irs::adc_ad7799_t::creation_reg_comm(reg_t a_reg,
  transaction_type_t a_tt)
{
  memset(mp_spi_buf, 0, m_write_buf_size);
  mp_spi_buf[m_reg_comm_index] |=
    static_cast<irs_u8>(a_reg.index << m_rs_pos);
  switch(a_tt) {
    case tt_read: {
      mp_spi_buf[m_reg_comm_index] |= static_cast<irs_u8>(1 << m_rw_pos);
    } break;
    case tt_write: {
      mp_spi_buf[m_reg_comm_index] &= ~static_cast<irs_u8>(1 << m_rw_pos);
    } break;
    default: {
      IRS_ASSERT_MSG("Недопустимый transaction_type");
    } break;
  }
  m_spi_transaction_size = a_reg.size + m_reg_comm_size;
}

int irs::adc_ad7799_t::calculation_shift(reg_t a_reg)
{
  int shift = 0;
  for(int i = 0; i < a_reg.index; i++) {
    shift += m_reg[i].size;
  }
  return shift;
}
int irs::adc_ad7799_t::calculation_number_byte(reg_t a_reg,
  param_byte_pos_t byte_pos)
{
  //из за того что обратный порядок байт
  int number_byte = (a_reg.size*8 - 1 - byte_pos*8)/8;
  return number_byte;
}

int irs::adc_ad7799_t::to_int(double a_number)
{
  return static_cast<int>(ceil(a_number));
}

int irs::adc_ad7799_t::filling_units(param_size_t a_size)
{
  return static_cast<int>(pow(2.0, a_size) - 1);
}

void irs::adc_ad7799_t::creation_reg(reg_t a_reg, int a_value,
  param_byte_pos_t a_byte_pos, param_pos_t a_pos,
  param_size_t a_size)
{
  m_read_data = false;
  creation_reg_comm(a_reg, tt_write);
  int shift = calculation_shift(a_reg);
  int number_byte = calculation_number_byte(a_reg,
    a_byte_pos);
  mp_buf[shift + number_byte] = static_cast<irs_u8>(
    mp_buf[shift + number_byte] &
    ~static_cast<irs_u8>(filling_units(a_size) << a_pos));
  mp_buf[shift + number_byte]  |= static_cast<irs_u8>(a_value << a_pos);
  for (int i = 0; i < a_reg.size; i++) {
    mp_spi_buf[i + m_reg[m_reg_comm_index].size] =
      mp_buf[shift + i];
  }
}

int irs::adc_ad7799_t::get(reg_t a_reg, param_byte_pos_t a_byte_pos,
  param_pos_t a_pos, param_size_t a_size)
{
  int shift = calculation_shift(a_reg);
  int number_byte = calculation_number_byte(a_reg,
    a_byte_pos);
  irs_u8 value_shift =
    static_cast<irs_u8>(mp_buf[shift + number_byte] >> a_pos);
  irs_u8 mask = static_cast<irs_u8>(filling_units(a_size));
  int value = value_shift & mask;
  return value;
}

irs_i32 irs::adc_ad7799_t::conversion_spi_value()
{
  mp_get_buff[2] = mp_spi_buf[1];
  mp_get_buff[1] = mp_spi_buf[2];
  mp_get_buff[0] = mp_spi_buf[3];
  irs_i32 value = *reinterpret_cast<irs_i32*>(mp_get_buff);
  int unipolar =
    get(m_reg[m_reg_conf_index], m_ub_byte_pos, m_ub_pos, m_ub_size);
  if (unipolar == 1) {
    value = (value << 7);
  } else if (unipolar == 0) {
    value = (value << 8);
    value = value - 0x80000000;
  }
  return value;
}


irs::dac_8531_t::dac_8531_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  float a_init_data):
  mp_spi(ap_spi),
  mp_cs_pin(ap_cs_pin),
  m_data(0),
  m_new_data(0),
  m_mode(mode_free)
{
  memset(mp_spi_buf, 0, write_buf_size);
  mp_cs_pin->set();
  set_float_data(0, a_init_data);
}

size_t irs::dac_8531_t::get_resolution() const
{
  return dac_resolution;
}

irs_u32 irs::dac_8531_t::get_u32_maximum() const
{
  return static_cast<irs_u32>(dac_max_value) << (32 - dac_resolution);
}

void irs::dac_8531_t::set_u32_data(size_t a_channel, const irs_u32 a_data)
{
  set_u16_normalized_data(a_channel,
    static_cast<irs_u16>(a_data >> (32 - dac_resolution)));
}

float irs::dac_8531_t::get_float_maximum() const
{
  return 1.f;
}

void irs::dac_8531_t::set_float_data(size_t a_channel, const float a_data)
{
  if (a_data > 1.f) {
    IRS_LIB_ERROR(ec_standard, "Значение должно быть от 0 до 1");
  }
  set_u16_normalized_data(a_channel,
    static_cast<irs_u16>(a_data*dac_max_value));
}

void irs::dac_8531_t::set_u16_normalized_data(size_t a_channel,
  const irs_u16 a_data)
{
  if (a_channel != 0) {
    IRS_LIB_ERROR(ec_standard, "Недопустимый канал");
  }
  m_new_data = a_data;
}

void irs::dac_8531_t::tick()
{
  switch (m_mode) {
    case mode_free: {
      if (m_new_data != m_data) {
        m_data = m_new_data;
        mp_spi_buf[0] = 0;
        mp_spi_buf[1] = IRS_HIBYTE(m_data);
        mp_spi_buf[2] = IRS_LOBYTE(m_data);
        m_mode = mode_write;
      }
    } break;
    case mode_write: {
      if ((mp_spi->get_status() == irs::spi_t::FREE) && !mp_spi->get_lock()) {
        mp_spi->lock();
        mp_spi->set_order(irs::spi_t::MSB);
        mp_spi->set_polarity(irs::spi_t::POSITIVE_POLARITY);
        mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
        mp_cs_pin->clear();
        mp_spi->write(mp_spi_buf, write_buf_size);
        m_mode = mode_write_wait;
      }
    } break;
    case mode_write_wait: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        memset(mp_spi_buf, 0, write_buf_size);
        m_mode = mode_free;
      }
    } break;
  }
}

//------------------------------------------------------------------------------

irs::dac_1220_t::dac_1220_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  float a_init_data, counter_t a_startup_pause
):
  mp_spi(ap_spi),
  mp_cs_pin(ap_cs_pin),
  m_data(static_cast<irs_u32>(static_cast<float>(m_dac_max_value)*a_init_data)),
  m_status(st_startup),
  m_return_status(irs_st_busy),
  m_need_write(true),
  m_timer(a_startup_pause)
{
  mp_cs_pin->set();
  m_timer.start();
}

irs_status_t irs::dac_1220_t::get_status() const
{
  return m_return_status;
}

size_t irs::dac_1220_t::get_resolution() const
{
  return m_dac_resolution;
}

irs_u32 irs::dac_1220_t::get_u32_maximum() const
{
  return 0xFFFFF000;
}

void irs::dac_1220_t::set_u32_data(size_t, const irs_u32 a_data)
{
  m_data = a_data >> (32 - m_dac_resolution);
  m_need_write = true;
  m_return_status = irs_st_busy;
}

float irs::dac_1220_t::get_float_maximum() const
{
  return 1.0f;
}

void irs::dac_1220_t::set_float_data(size_t, const float a_data)
{
  m_data = static_cast<irs_u32>(static_cast<float>(m_dac_max_value) * a_data);
  m_need_write = true;
  m_return_status = irs_st_busy;
}

void irs::dac_1220_t::tick()
{
  mp_spi->tick();
  switch (m_status) {
  case st_startup:
    if (m_timer.check()) {
      m_status = st_write_command;
    }
    break;
  case st_write_command:
    mp_spi_buf[m_cmd_pos] = m_command_reg_write_command;
    mp_spi_buf[m_data_pos + 0] =
      (0 << cmr_adpt) | (0 << cmr_calpin) | (1 << cmr_write1) | (0 << cmr_crst);
    mp_spi_buf[m_data_pos + 1] =
      (1 << cmr_res) | (0 << cmr_clr) | (1 << cmr_df) | (1 << cmr_disf) |
      (0 << cmr_bd) | (0 << cmr_msb) | (0 << cmr_md1) | (0 << cmr_md0);
    m_write_buf_size = m_command_reg_size;
    m_status = st_prepare_spi;
    break;
  case st_write_data:
    m_need_write = false;
    mp_spi_buf[m_cmd_pos] = m_data_reg_write_command;
    mp_spi_buf[m_data_pos + 0] = static_cast<irs_u8>(m_data >> 12);
    mp_spi_buf[m_data_pos + 1] = static_cast<irs_u8>(m_data >> 4);
    mp_spi_buf[m_data_pos + 2] = static_cast<irs_u8>(m_data << 4);
    m_write_buf_size = m_data_reg_size;
    m_status = st_prepare_spi;
    break;
  case st_prepare_spi:
    if (!mp_spi->get_lock() && mp_spi->get_status() == irs::spi_t::FREE) {
      mp_spi->lock();
      mp_spi->set_order(irs::spi_t::MSB);
      mp_spi->set_polarity(irs::spi_t::NEGATIVE_POLARITY);
      mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
      mp_cs_pin->clear();
      mp_spi->write(mp_spi_buf, m_write_buf_size);
      m_status = st_wait_spi;
    }
    break;
  case st_wait_spi:
    if (mp_spi->get_status() == irs::spi_t::FREE) {
      mp_cs_pin->set();
      mp_spi->reset_configuration();
      mp_spi->unlock();
      m_status = st_free;
    }
    break;
  case st_free:
    if (!m_need_write) {
      if (m_return_status != irs_st_ready) {
        m_return_status = irs_st_ready;
      }
    } else {
      m_status = st_write_data;
    }
    break;
  }
}

//------------------------------------------------------------------------------

irs::dac_ltc8043_t::dac_ltc8043_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  float a_init_data):
  mp_spi(ap_spi),
  mp_cs_pin(ap_cs_pin),
  m_data(0),
  m_new_data(0),
  m_mode(mode_free),
  m_timer(irs::make_cnt_mcs(cs_interval))
{
  memset(mp_spi_buf, 0, write_buf_size);
  mp_cs_pin->set();
  set_float_data(0, a_init_data);
}

size_t irs::dac_ltc8043_t::get_resolution() const
{
  return dac_resolution;
}

irs_u32 irs::dac_ltc8043_t::get_u32_maximum() const
{
  return static_cast<irs_u32>(dac_max_value) << (32 - dac_resolution);
}

void irs::dac_ltc8043_t::set_u32_data(size_t, const irs_u32 a_data)
{
  m_new_data = static_cast<irs_u16>(a_data >> (32 - dac_resolution));
}

float irs::dac_ltc8043_t::get_float_maximum() const
{
  return 1.f;
}

void irs::dac_ltc8043_t::set_float_data(size_t, const float a_data)
{
  if (a_data > 1.f) {
    IRS_LIB_ERROR(ec_standard, "Значение должно быть от 0 до 1");
  }
  m_new_data = static_cast<irs_u16>(a_data*dac_max_value);
}

void irs::dac_ltc8043_t::tick()
{
  mp_spi->tick();
  switch (m_mode) {
    case mode_free: {
      if (m_new_data != m_data) {
        m_data = m_new_data;
        mp_spi_buf[0] = IRS_HIBYTE(m_data);
        mp_spi_buf[1] = IRS_LOBYTE(m_data);
        m_mode = mode_write;
      }
    } break;
    case mode_write: {
      if ((mp_spi->get_status() == irs::spi_t::FREE) && !mp_spi->get_lock()) {
        mp_spi->lock();
        mp_spi->set_order(irs::spi_t::MSB);
        mp_spi->set_polarity(irs::spi_t::NEGATIVE_POLARITY);
        mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
        mp_spi->write(mp_spi_buf, write_buf_size);
        m_mode = mode_write_wait;
      }
    } break;
    case mode_write_wait: {
      if (mp_spi->get_status() == irs::spi_t::FREE) {
        mp_cs_pin->clear();
        m_timer.start();
        m_mode = mode_wait_cs;
      }
    } break;
    case mode_wait_cs: {
      if (m_timer.check()) {
        mp_cs_pin->set();
        mp_spi->reset_configuration();
        mp_spi->unlock();
        memset(mp_spi_buf, 0, write_buf_size);
        m_mode = mode_free;
      }
    } break;
  }
}
