// Аналоговый ввод-вывод
// Дата: 11.09.2009

#include <irsadc.h>

//--------------------------  LM95071 ------------------------------------------

irs::th_lm95071_t::th_lm95071_t(spi_t *ap_spi, gpio_pin_t *ap_cs_pin,
  counter_t a_read_delay):
  m_status(TH_FREE),
  mp_spi(ap_spi),
  m_conv_koef(0.031025f), //0.031075f
  m_read_counter(0),
  m_read_delay(a_read_delay),
  m_connect(false),
  mp_cs_pin(ap_cs_pin)
{
  memset((void*)mp_buf, 0, m_size);
  memset((void*)mp_spi_buf, 0, m_spi_size);
  mp_cs_pin->clear();
  mp_spi->write(mp_spi_buf, m_spi_size);
  for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());
  mp_spi->write(mp_spi_buf, m_spi_size);
  for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());
  mp_cs_pin->set();
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
  if (mp_spi && mp_cs_pin && m_connect) return true;
  else return false;
}

void irs::th_lm95071_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (!a_index && (a_size == m_size))
  {
    memcpy((void*)ap_buf, (void*)mp_buf, m_size);
    mp_buf[0] = 0;
    return;
  }
  else
  {
    if (a_index >= m_size) return;
    irs_u8 size = (irs_u8)a_size;
    if (size + a_index > m_size) size = irs_u8(m_size - a_index);
    memcpy((void*)ap_buf, (void*)(mp_buf + a_index), size);
    mp_buf[0] = 0;
    return;
  }
}

void irs::th_lm95071_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_size) size = irs_u8(m_size - a_index);
  memcpy((void*)(mp_buf + a_index), (void*)ap_buf, size);
  mp_buf[0] = 0;
}

irs_bool irs::th_lm95071_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  bool bit = (mp_buf[a_index] >> a_bit_index) & irs_u8(1);
  mp_buf[0] = 0;
  return bit;
}
void irs::th_lm95071_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= irs_u8(1 << a_bit_index);
  mp_buf[0] = 0;
}

void irs::th_lm95071_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= irs_u8((1 << a_bit_index)^0xFF);
  mp_buf[0] = 0;
}

void irs::th_lm95071_t::tick()
{
  mp_spi->tick();
  switch (m_status)
  {
  case TH_FREE:
    {
      if (test_to_cnt(m_read_counter))
      {
        if (!mp_spi->get_lock() && (mp_spi->get_status() == irs::spi_t::FREE))
        {
          set_to_cnt(m_read_counter, m_read_delay);
          mp_spi->set_order(irs::spi_t::MSB);
          mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
          mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
          mp_spi->lock();
          mp_cs_pin->clear();
          mp_spi->read(mp_spi_buf, m_spi_size);
          m_status = TH_READ;
        }
      }
      break;
    }
  case TH_READ:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        irs_u16 th_value = 0;
        IRS_LOBYTE(th_value) = mp_spi_buf[1];
        IRS_HIBYTE(th_value) = mp_spi_buf[0];
        mp_buf[0] = 1;
        mp_buf[1] = mp_spi_buf[1];
        mp_buf[2] = mp_spi_buf[0];
        *((irs_u16*)&mp_buf[1]) >>= 2;
        if (th_value != 0x800F)
        {
          m_connect = true;
          mp_cs_pin->set();
          mp_spi->unlock();
          m_status = TH_FREE;
        }
        else
        {
          mp_spi_buf[0] = 0;
          mp_spi_buf[1] = 0;
          m_status = TH_RESET;
          mp_spi->write(mp_spi_buf, m_spi_size);
          mp_cs_pin->clear();
        }
      }
      break;
    }
  case TH_RESET:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        mp_cs_pin->set();
        mp_spi->unlock();
        m_status = TH_FREE;
      }
      break;
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
  memset((void*)mp_buf, 0, m_size);
  memset((void*)mp_spi_buf, 0, m_spi_size);
  for (; (mp_spi->get_status() != irs::spi_t::FREE) && (mp_spi->get_lock()); )
    mp_spi->tick();
  irs_u8 mp_init_buffer[m_init_sequence_size];
  memset((void*)mp_init_buffer, 0xFF, m_reset_sequence_size);
  mp_init_buffer[4] = (1 << RS0)|(0 << CH0)|(0 << CH1);
  mp_init_buffer[5] = (0 << MD1)|(0 << MD0)|(1 << BO)|(1 << UB)|(1 << BUF);
  mp_init_buffer[6] = (1 << RS1);
  mp_init_buffer[7] = (1 << FS2)|(0 << FS1)|(1 << FS0);
  mp_cs_pin->clear();
  mp_spi->write(mp_init_buffer, m_init_sequence_size);
  for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());
  mp_cs_pin->set();
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
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_size) size = irs_u8(m_size - a_index);
  memcpy((void*)ap_buf, (void*)(mp_buf + a_index), size);
  mp_buf[0] = 0;
  return;
}

void irs::adc_ad7791_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_size) size = irs_u8(m_size - a_index);
  memcpy((void*)(mp_buf + a_index), (void*)ap_buf, size);
}

irs_bool irs::adc_ad7791_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  bool bit = (mp_buf[a_index] >> a_bit_index) & irs_u8(1);
  mp_buf[0] = 0;
  return bit;
}
void irs::adc_ad7791_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= irs_u8(1 << a_bit_index);
}

void irs::adc_ad7791_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= irs_u8((1 << a_bit_index)^0xFF);
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
          mp_spi->set_order(irs::spi_t::MSB);
          mp_spi->set_polarity(irs::spi_t::FALLING_EDGE);
          mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
          mp_spi->lock();
          mp_cs_pin->clear();
          irs_u8 request = (1 << RW)|(1 << RS0)|(1 << RS1)|(0 << CH0)|(0 << CH1);
          mp_spi->write(&request, sizeof(request));
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
        memset((void*)mp_buf, 0, m_size);
        mp_buf[0] = 1;
        mp_buf[1] = mp_spi_buf[2];
        mp_buf[2] = mp_spi_buf[1];
        mp_buf[3] = mp_spi_buf[0];
        mp_buf[4] = 0;
        mp_cs_pin->set();
        mp_spi->unlock();
        m_status = ADC_FREE;
      }
      break;
    }
  }
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
  memset((void*)mp_buf, 0, m_size);
  memset((void*)mp_spi_buf, 0, m_spi_size);
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
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_size) size = irs_u8(m_size - a_index);
  memcpy((void*)ap_buf, (void*)(mp_buf + a_index), size);
  mp_buf[0] = 0;
  return;
}

void irs::adc_ad7686_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_size) size = irs_u8(m_size - a_index);
  memcpy((void*)(mp_buf + a_index), (void*)ap_buf, size);
}

irs_bool irs::adc_ad7686_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  bool bit = (mp_buf[a_index] >> a_bit_index) & irs_u8(1);
  mp_buf[0] = 0;
  return bit;
}
void irs::adc_ad7686_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= irs_u8(1 << a_bit_index);
}

void irs::adc_ad7686_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= irs_u8((1 << a_bit_index)^0xFF);
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
        mp_spi->set_polarity(irs::spi_t::FALLING_EDGE);
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
        mp_spi->unlock();
        m_status = ADC_FREE;
      }
      break;
    }
  }
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
  if (mp_spi && mp_cs_pin)
  {
    mp_buf[0] = 0;
    mp_buf[1] = m_init_value;
    for (; (mp_spi->get_status() != irs::spi_t::FREE) && (mp_spi->get_lock()); )
      mp_spi->tick();
    mp_cs_pin->clear();
    irs_u8 mp_write_buffer[m_packet_size];
    mp_write_buffer[0] = 0;
    mp_write_buffer[1] = m_init_value;
    mp_spi->write(mp_write_buffer, m_packet_size);
    for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());
    mp_cs_pin->set();
  }
}

irs::dac_ad8400_t::~dac_ad8400_t()
{
  return;
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
    irs_u8 size = (irs_u8)a_size;
    if (size + a_index > m_size) size = irs_u8(m_size - a_index);
    memcpy((void*)ap_buf, (void*)(mp_buf + a_index), size);
    return;
}

void irs::dac_ad8400_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_size) size = irs_u8(m_size - a_index);
  memcpy((void*)(mp_buf + a_index), (void*)ap_buf, size);
  mp_buf[0] = 0;
  m_need_write = true;
}

irs_bool irs::dac_ad8400_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_buf[a_index] & irs_u8(1 << a_bit_index));
}

void irs::dac_ad8400_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= irs_u8(1 << a_bit_index);
  mp_buf[0] = 0;
  m_need_write = true;
}

void irs::dac_ad8400_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= irs_u8((1 << a_bit_index)^0xFF);
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
          irs_u8 mp_write_buffer[m_packet_size];
          mp_write_buffer[0] = 0;
          mp_write_buffer[1] = mp_buf[1];
          mp_cs_pin->clear();
          mp_spi->lock();
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
        mp_spi->unlock();
        m_status = DAC_FREE;
      }
      break;
    }
  }
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
    irs_u8 size = (irs_u8)a_size;
    if (size + a_index > m_size) size = irs_u8(m_size - a_index);
    memcpy((void*)ap_buf, (void*)(mp_buf + a_index), size);
    return;
}

void irs::dac_ad7376_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  if (((mp_buf[0]>>m_rs_bit_position)&1) == 0) m_need_reset = true;
  if (((mp_buf[0]>>m_shdn_bit_position)&1) == 0) mp_shdn_pin->clear();     
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_size) size = irs_u8(m_size - a_index);
  memcpy((void*)(mp_buf + a_index), (void*)ap_buf, size);
  mp_buf[0] &= (1 << m_ready_bit_position)^0xFF;
  m_need_write = true;
}

irs_bool irs::dac_ad7376_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_buf[a_index] & irs_u8(1 << a_bit_index));
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
  mp_buf[a_index] |= irs_u8(1 << a_bit_index);
  mp_buf[0] &= (1 << m_ready_bit_position)^0xFF;
  m_need_write = true;
}

void irs::dac_ad7376_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= irs_u8((1 << a_bit_index)^0xFF);
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
          irs_u8 mp_write_buffer[m_packet_size];
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
  if (mp_cs_pin)
  {
    mp_cs_pin->set();
    *(irs_u16*)&mp_buf[1] = m_init_value;
    for (; (mp_spi->get_status() != irs::spi_t::FREE) && (mp_spi->get_lock()); )
      mp_spi->tick();
    mp_spi->write((irs_u8*)&m_init_value, m_packet_size);
    for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());
    mp_cs_pin->clear();
    mp_cs_pin->set();
  }
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
    memcpy((void*)ap_buf, (void*)mp_buf, m_size);
    return;
  }
  else
  {
    if (a_index >= m_size) return;
    irs_u8 size = (irs_u8)a_size;
    if (size + a_index > m_size) size = irs_u8(m_size - a_index);
    memcpy((void*)ap_buf, (void*)(mp_buf + a_index), size);
    return;
  }
}

void irs::dac_max551_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (!a_index && (a_size == m_size))
  {
    memcpy((void*)mp_buf, (void*)ap_buf, m_size);
    mp_buf[0] = 0;
    m_need_write = true;
    return;
  }
  else
  {
    if (a_index >= m_size) return;
    irs_u8 size = (irs_u8)a_size;
    if (size + a_index > m_size) size = irs_u8(m_size - a_index);
    memcpy((void*)(mp_buf + a_index), (void*)ap_buf, size);
    mp_buf[0] = 0;
    m_need_write = true;
  }
}

irs_bool irs::dac_max551_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_buf[a_index] & irs_u8(1 << a_bit_index));
}

void irs::dac_max551_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= irs_u8(1 << a_bit_index);
  mp_buf[0] = 0;
  m_need_write = true;
}

void irs::dac_max551_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= irs_u8((1 << a_bit_index)^0xFF);
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
          mp_spi->set_order(irs::spi_t::MSB);
          mp_spi->set_polarity(irs::spi_t::RISING_EDGE);
          mp_spi->set_phase(irs::spi_t::LEAD_EDGE);
          irs_u8 write_buffer[2];
          write_buffer[0] = mp_buf[2];
          write_buffer[1] = mp_buf[1];
          mp_spi->write(write_buffer, m_packet_size);
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
        mp_spi->set_polarity(irs::spi_t::FALLING_EDGE);
        mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
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
  m_first_byte(1)
{
  mp_cs_pin->set();
  //
  init_to_cnt();
  m_refresh_time = MS_TO_CNT(20);
  memset((void*)mp_buf, 0, m_size);
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
    irs_u32 DDS_CR = ((irs_u32)0<<SKE)|
                     ((irs_u32)0<<QPD)|
                     ((irs_u32)1<<PLLBYPS)|
                     ((irs_u32)0 << INTUD)|
                     ((irs_u32)0 << CPD)|
                     ((irs_u32)0 << PLLMUL0)|
                     ((irs_u32)0 << PLLMUL2);
    irs_u32 *mp_buf_cr = (irs_u32*)&mp_buf[POS_CR];
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
    mp_spi->write(spi2_buffer, 5);
    for (; mp_spi->get_status() != irs::spi_t::FREE; mp_spi->tick());
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
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_size) size = irs_u8(m_size - a_index);
  memcpy((void*)ap_buf, (void*)(mp_buf + a_index), size);
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
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_size) size = irs_u8(m_size - a_index);
  memcpy((void*)(mp_buf + a_index), (void*)ap_buf, size);
  fill_n(m_write_vector.begin() + a_index, size, true);
  m_all_write = false;
  mp_buf[POS_STATUS] = 0;
}

irs_bool irs::dds_ad9854_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_buf[a_index] & irs_u8(1 << a_bit_index));
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
  mp_buf[a_index] |= irs_u8(1 << a_bit_index);
  m_write_vector[a_index] = true;
  m_all_write = false;
  mp_buf[POS_STATUS] = 0;
}

void irs::dds_ad9854_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= irs_u8((1 << a_bit_index)^0xFF);
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
            irs_u8 write_buffer[m_max_write_bytes];
            write_buffer[0] = ADDR_CR;
            write_buffer[1] = mp_buf[POS_CR + 3];
            write_buffer[2] = mp_buf[POS_CR + 2];
            write_buffer[3] = mp_buf[POS_CR + 1];
            write_buffer[4] = mp_buf[POS_CR];
            fill_n(m_write_vector.begin() + POS_CR, SZ_CR, false);
            mp_spi->set_order(irs::spi_t::MSB);
            mp_spi->set_polarity(irs::spi_t::FALLING_EDGE);
            mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
            mp_spi->lock();
            mp_cs_pin->clear();
            mp_spi->write(write_buffer, SZ_CR + 1);
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
              irs_u8 write_buffer[m_max_write_bytes];
              irs_u8 size =  mp_reg_size[register_index];// + 1;
              irs_u8 start_byte = mp_reg_position[register_index];
              write_buffer[0] = register_index;
              switch (size)
              {
                case 1:
                {
                  write_buffer[1] = mp_buf[start_byte];
                  break;
                }
                case 2:
                {
                  write_buffer[1] = mp_buf[start_byte + 1];
                  write_buffer[2] = mp_buf[start_byte];
                  break;
                }
                case 3:
                {
                  write_buffer[1] = mp_buf[start_byte + 2];
                  write_buffer[2] = mp_buf[start_byte + 1];
                  write_buffer[3] = mp_buf[start_byte];
                  break;
                }
                case 4:
                {
                  write_buffer[1] = mp_buf[start_byte + 3];
                  write_buffer[2] = mp_buf[start_byte + 2];
                  write_buffer[3] = mp_buf[start_byte + 1];
                  write_buffer[4] = mp_buf[start_byte];
                  break;
                }
                case 6:
                {
                  write_buffer[1] = mp_buf[start_byte + 5];
                  write_buffer[2] = mp_buf[start_byte + 4];
                  write_buffer[3] = mp_buf[start_byte + 3];
                  write_buffer[4] = mp_buf[start_byte + 2];
                  write_buffer[5] = mp_buf[start_byte + 1];
                  write_buffer[6] = mp_buf[start_byte];
                  break;
                }
              }

              fill_n(m_write_vector.begin() + start_byte,
                mp_mxdata_reg_size[register_index], false);
              m_first_byte += mp_mxdata_reg_size[register_index];
              mp_spi->set_order(irs::spi_t::MSB);
              mp_spi->set_polarity(irs::spi_t::FALLING_EDGE);
              mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
              mp_spi->lock();
              mp_cs_pin->clear();
              mp_spi->write(write_buffer, size+1);
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
      break;
    }
  case DDS_WRITE:
    {
      if (mp_spi->get_status() == irs::spi_t::FREE)
      {
        mp_cs_pin->set();
        counter_t cnt;
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
            == m_write_vector.end()) mp_buf[POS_STATUS] |= 0x01;
      }
      break;
    }
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
  
  mp_spi->set_order(irs::spi_t::MSB);
  mp_spi->set_polarity(irs::spi_t::FALLING_EDGE);
  mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
  
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
}
 
void irs::dac_ltc2622_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  irs_u8 size = (irs_u8)a_size;
  if (size + a_index > m_size) size = irs_u8(m_size - a_index);
  memcpy((void*)(mp_buf + a_index), (void*)ap_buf, size);
  mp_buf[0] = 0;
  m_need_write = true;
  mp_buf[0] &= ~(1 << m_ready_bit_regA);
  mp_buf[0] &= ~(1 << m_ready_bit_regB);
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
    irs_u8 size = (irs_u8)a_size;
    if (size + a_index > m_size) size = irs_u8(m_size - a_index);
    memcpy((void*)ap_buf, (void*)(mp_buf + a_index), size);
    return;
}

irs_bool irs::dac_ltc2622_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return false;
  if (a_bit_index > 7) return false;
  return (mp_buf[a_index] & irs_u8(1 << a_bit_index));
}

void irs::dac_ltc2622_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] |= irs_u8(1 << a_bit_index);
  mp_buf[0] = 0;
  m_need_write = true;
}

void irs::dac_ltc2622_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (a_index >= m_size) return;
  if (a_index == 0) return;
  if (a_bit_index > 7) return;
  mp_buf[a_index] &= irs_u8((1 << a_bit_index)^0xFF);
  mp_buf[0] = 0;
  m_need_write = true;
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
          mp_spi->set_order(irs::spi_t::MSB);
          mp_spi->set_polarity(irs::spi_t::FALLING_EDGE);
          mp_spi->set_phase(irs::spi_t::TRAIL_EDGE);
          mp_spi->lock();
          
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
        mp_spi->unlock();
        m_need_write = false;
        m_status = DAC_FREE; 
      }
      break;
    }
  }
}
