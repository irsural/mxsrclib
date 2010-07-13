// Калибровка приборов
// Дата: 06.07.2010
// Ранняя дата: 27.03.2009

#include <irsdefs.h>

#include <correct_alg.h>
#include <irsalg.h>

#include <irsfinal.h>

irs::avr::step_correct_t::step_correct_t(correct_map_t *ap_map):
  m_current_error(error_null),
  mp_map(ap_map)
{
  if (ap_map->m_map_id == 0)
  {
    m_current_error = error_default_map;
    //  запполнить коэффициенты единичками и нулями, если еепром недоступен
    mp_map->m_col_count = 0;
    mp_map->m_row_count = 0;
  }
}

irs::avr::step_correct_t::~step_correct_t()
{
  //
}

irs::avr::step_correct_t::correct_error_t irs::avr::step_correct_t::get_error()
{
  return m_current_error;
}

irs::avr::step_correct_t::ver_t
  irs::avr::step_correct_t::apply(hor_t a_hor_value, ver_t a_ver_value)
{
  ver_t value = a_ver_value;
  ver_t user_value = a_ver_value;
  irs_uarc col_offset = 0;

  if ((mp_map->m_col_count > 0) && (mp_map->m_row_count > 0))
  {
    if (a_ver_value > mp_map->mp_ver_points[mp_map->m_row_count-1])
    {
      col_offset = (mp_map->m_row_count-1)*mp_map->m_col_count;
    }
    else
    {
      for (irs_u8 row_index = 0; row_index < mp_map->m_row_count; row_index++)
        if (a_ver_value <= mp_map->mp_ver_points[row_index])
        {
          col_offset = row_index*mp_map->m_col_count;
          break;
        }
    }
    if (a_hor_value > mp_map->mp_hor_points[mp_map->m_col_count-1])
    {
      user_value = ver_t(float(value)
        * mp_map->mp_k_array[mp_map->m_col_count-1 + col_offset]
        + mp_map->mp_b_array[mp_map->m_col_count-1 + col_offset]);
    }
    else
    {
      for (irs_u8 col_index = 0; col_index < mp_map->m_col_count; col_index++)
        if (a_hor_value <= mp_map->mp_hor_points[col_index])
        {
          user_value = ver_t(float(value)
            * mp_map->mp_k_array[col_index + col_offset]
            + mp_map->mp_b_array[col_index + col_offset]);
          break;
        }
    }
  }
  return user_value;
}

irs_bool irs::avr::step_correct_t::apply_map(correct_map_t *ap_map)
{
  mp_map = ap_map;
  if (ap_map->m_map_id == 0)
  {
    m_current_error = error_default_map;
    //  запполнить коэффициенты единичками и нулями, если еепром недоступен
    mp_map->m_col_count = 0;
    mp_map->m_row_count = 0;
    mp_map->mp_hor_points[0] = 0;
    mp_map->mp_ver_points[0] = 0;
    mp_map->mp_k_array[0] = 0.0f;
    mp_map->mp_b_array[0] = 0.0f;
    return irs_false;
  }
  else return irs_true;
}

//------------------------------------------------------------------------------

irs::quasi_eeprom_t::quasi_eeprom_t(irs_u8 *ap_buf, irs_uarc a_size):
  mp_buf(ap_buf),
  m_size(a_size)
{
}

irs::quasi_eeprom_t::~quasi_eeprom_t()
{
  //
}

irs_uarc irs::quasi_eeprom_t::size()
{
  return m_size;
}

irs_bool irs::quasi_eeprom_t::connected()
{
  if (mp_buf && m_size) return irs_true;
  else return irs_false;
}

void irs::quasi_eeprom_t::read(irs_u8 *ap_buf, irs_uarc a_index,irs_uarc a_size)
{
  if (mp_buf && m_size)
  {
    irs_uarc real_index = a_index;
    irs_uarc real_size = a_size;
    if (real_index > m_size - 1) return;
    memset((void*)ap_buf, 0, real_size);
    if (real_size > m_size - real_index) real_size = m_size - real_index;
    memcpy((void*)ap_buf, (void*)(mp_buf + real_index), real_size);
  }
}

void irs::quasi_eeprom_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  if (mp_buf && m_size)
  {
    irs_uarc real_index = a_index;
    irs_uarc real_size = a_size;
    if (real_index > m_size - 1) real_index = m_size - 1;
    if (real_size > m_size - real_index) real_size = m_size - real_index;
    memcpy((void*)(mp_buf + real_index), (void*)ap_buf, real_size);
  }
}

irs_bool irs::quasi_eeprom_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (mp_buf && m_size)
  {
    irs_uarc index = a_index;
    irs_uarc bit_index = a_bit_index;
    if (index >= m_size) index = m_size - 1;
    if (bit_index > 7) bit_index = 7;
    return (mp_buf[index] & irs_u8(1 << bit_index));
  }
  else return irs_false;
}

void irs::quasi_eeprom_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  if (mp_buf && m_size)
  {
    irs_uarc index = a_index;
    irs_uarc bit_index = a_bit_index;
    if (index >= m_size) index = m_size - 1;
    if (bit_index > 7) bit_index = 7;
    mp_buf[index] |= irs_u8(1 << bit_index);
  }
}

void irs::quasi_eeprom_t::clear_bit(irs_uarc a_index,irs_uarc a_bit_index)
{
  if (mp_buf && m_size)
  {
    irs_uarc index = a_index;
    irs_uarc bit_index = a_bit_index;
    if (index >= m_size) index = m_size - 1;
    if (bit_index > 7) bit_index = 7;
    mp_buf[index] &= irs_u8((1 << bit_index)^0xFF);
  }
}

void irs::quasi_eeprom_t::tick()
{
  //
}
