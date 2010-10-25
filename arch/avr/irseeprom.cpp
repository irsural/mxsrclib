//! \file
//! \ingroup drivers_group
//! \brief ƒоступ к EEPROM AVR
//!
//! ƒата: 25.10.2010
//! –анн€€ ƒата: 19.08.2009

// Ќомер файла
#define IRSEEPROMCPP_IDX 19

#include <irspch.h>

#include <irseeprom.h>
#include <irserror.h>
#include <mxdata.h>

#include <irsfinal.h>

__no_init __eeprom irs_u8
  irs::avr::eeprom_t::mp_ee_data[irs::avr::eeprom_t::m_data_size] @
  irs::avr::eeprom_t::m_begin_address;

irs::avr::eeprom_t::eeprom_t(irs_uarc a_start_index, irs_uarc a_size):
  m_start_index(a_start_index),
  m_size(a_size),
  m_crc_error(false)
{
  irs_u32 crc_new = calc_new_crc();
  if (crc_new == crc_eeprom()) {
    m_crc_error = false;
  } else {
    irs_u32 crc_old = calc_old_crc();
    if (crc_old == crc_eeprom()) {
      m_crc_error = false;
    } else {
      m_crc_error = true;
    }
  }
}

irs::avr::eeprom_t::~eeprom_t()
{
  //
}

irs_uarc irs::avr::eeprom_t::size()
{
  return m_size;
}
irs_bool irs::avr::eeprom_t::connected()
{
  return irs_true;
}

void irs::avr::eeprom_t::read(irs_u8 *buf, irs_uarc index, irs_uarc size)
{
  if (index >= m_size) return;
  irs_uarc user_size = size;
  if (user_size > m_size - index) user_size = m_size - index;
  copy(mp_ee_data + m_crc_size + m_start_index + index,
    mp_ee_data + m_crc_size + m_start_index + index + user_size, buf);
}

void irs::avr::eeprom_t::write(const irs_u8 *buf, irs_uarc index,
  irs_uarc size)
{
  if (index >= m_size) return;
  irs_uarc user_size = size;
  if (size > m_size - index) user_size = m_size - index;
  copy(buf, buf + user_size, mp_ee_data + m_crc_size + m_start_index + index);
  crc_eeprom() = calc_new_crc();
}

irs_bool irs::avr::eeprom_t::bit(irs_uarc index, irs_uarc bit_index)
{
  if (index >= m_size) return irs_false;
  if (bit_index >= 8) return irs_false;
  irs_u8 value = mp_ee_data[m_start_index + index];
  /*< 7 6 5 4 3 2 1 0 > bit_index */
  value = (value >> bit_index) & 1;
  return value;
}

void irs::avr::eeprom_t::set_bit(irs_uarc index, irs_uarc bit_index)
{
  if (index >= m_size) return;
  if (bit_index >= 8) return;
  mp_ee_data[m_start_index + m_crc_size + index] |= (1 << bit_index);
  crc_eeprom() = calc_new_crc();
}

void irs::avr::eeprom_t::clear_bit(irs_uarc index, irs_uarc bit_index)
{
  if (index >= m_size) return;
  if (bit_index >= 8) return;
  mp_ee_data[m_start_index + m_crc_size + index] |= (1 << bit_index);
  crc_eeprom() = calc_new_crc();
}

void irs::avr::eeprom_t::tick()
{
  return;
}

bool irs::avr::eeprom_t::error()
{
  return m_crc_error;
}

irs_u32 irs::avr::eeprom_t::calc_old_crc() const
{
  return crc32(reinterpret_cast<irs_u32 __eeprom*>(
    &mp_ee_data[m_start_index + m_crc_size]), 0, m_size/4);
}
irs_u32 irs::avr::eeprom_t::calc_new_crc() const
{
  return crc32_table(&mp_ee_data[m_start_index +
    m_crc_size], m_size);
}
irs_u32 __eeprom& irs::avr::eeprom_t::crc_eeprom() const
{
  return reinterpret_cast<irs_u32 __eeprom&>(mp_ee_data[m_start_index]);
}


irs::avr::eeprom_reserve_t::eeprom_reserve_t(eeprom_t *ap_eeprom1,
  eeprom_t *ap_eeprom2, irs_uarc a_size):
  mp_eeprom1(ap_eeprom1),
  mp_eeprom2(ap_eeprom2),
  m_size(a_size),
  m_error(false)
{
  if (mp_eeprom1->error() && !mp_eeprom2->error())
  {
    irs_u8 *temp = new irs_u8[m_size];
    mp_eeprom2->read(temp, 0, m_size);
    mp_eeprom1->write(temp, 0, m_size);
    delete []temp;
  }
  else if (!mp_eeprom1->error() && mp_eeprom2->error())
  {
    irs_u8 *temp = new irs_u8[m_size];
    mp_eeprom1->read(temp, 0, m_size);
    mp_eeprom2->write(temp, 0, m_size);
    delete []temp;
  }
  else if (mp_eeprom1->error() && mp_eeprom2->error()) m_error = true;
}

irs::avr::eeprom_reserve_t::~eeprom_reserve_t()
{
}

irs_uarc irs::avr::eeprom_reserve_t::size()
{
  return m_size;
}

irs_bool irs::avr::eeprom_reserve_t::connected()
{
  return (mp_eeprom1 && mp_eeprom2 && m_size);
}

void irs::avr::eeprom_reserve_t::read(irs_u8 *buf, irs_uarc index,
  irs_uarc size)
{
  mp_eeprom1->read(buf, index, size);
}

void irs::avr::eeprom_reserve_t::write(const irs_u8 *buf, irs_uarc index,
  irs_uarc size)
{
  mp_eeprom1->write(buf, index, size);
  mp_eeprom2->write(buf, index, size);
}

irs_bool irs::avr::eeprom_reserve_t::bit(irs_uarc index, irs_uarc bit_index)
{
  return mp_eeprom1->bit(index, bit_index);
}

void irs::avr::eeprom_reserve_t::set_bit(irs_uarc index, irs_uarc bit_index)
{
  mp_eeprom1->set_bit(index, bit_index);
  mp_eeprom2->set_bit(index, bit_index);
}

void irs::avr::eeprom_reserve_t::clear_bit(irs_uarc index, irs_uarc bit_index)
{
  mp_eeprom1->clear_bit(index, bit_index);
  mp_eeprom2->clear_bit(index, bit_index);
}

void irs::avr::eeprom_reserve_t::tick()
{
}

bool irs::avr::eeprom_reserve_t::double_error()
{
  return m_error;
}

//----------------------------- EEPROM PROTECTED -------------------------------
namespace irs {

namespace avr {

class raw_eeprom_t
{
public:
  typedef irs_u8 value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef size_t size_type;

  static void write(const_pointer ap_buf, size_type a_buf_size,
    size_type a_eeprom_idx);
  static void read(pointer ap_buf, size_type a_buf_size,
    size_type a_eeprom_idx);
  static size_type size();
  static void copy(size_type a_src_idx, size_type a_dest_idx,
    size_type a_size);
  template <class T>
  static bool is_equal(size_type a_eeprom_idx, const T& a_value);
  static irs_u32 calc_crc32(size_type a_eeprom_idx, size_type a_size);
private:
  enum write_read_t { wr_write, wr_read };

  typedef value_type __eeprom* eeprom_pointer;

  enum {
    m_size = eeprom_t::m_end_address - eeprom_t::m_begin_address
  };

  static void write_read(write_read_t a_write_read,
    const_pointer ap_write_buf, pointer ap_read_buf,
    size_type a_buf_size, size_type a_eeprom_idx);
  static eeprom_pointer data();
};
raw_eeprom_t::eeprom_pointer raw_eeprom_t::data()
{
  return reinterpret_cast<eeprom_pointer>(eeprom_t::m_begin_address);
}
void raw_eeprom_t::write_read(write_read_t a_write_read,
  const_pointer ap_write_buf, pointer ap_read_buf, size_type a_buf_size,
  size_type a_eeprom_idx)
{
  IRS_LIB_ERROR_IF(a_eeprom_idx + a_buf_size > m_size, ec_standard, "");
  if (a_eeprom_idx > m_size) return;
  size_type eeprom_size = ::min(a_buf_size, m_size - a_eeprom_idx);
  switch (a_write_read) {
    case wr_write: {
      ::copy(ap_write_buf, ap_write_buf + eeprom_size,
        data() + a_eeprom_idx);
    } break;
    case wr_read: {
      ::copy(data() + a_eeprom_idx, data() + a_eeprom_idx + eeprom_size,
        ap_read_buf);
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("switch default error");
    } break;
  }
}
void raw_eeprom_t::write(const_pointer ap_buf, size_type a_buf_size,
  size_type a_eeprom_idx)
{
  write_read(wr_write, ap_buf, IRS_NULL, a_buf_size, a_eeprom_idx);
}
void raw_eeprom_t::read(pointer ap_buf, size_type a_buf_size,
  size_type a_eeprom_idx)
{
  write_read(wr_read, IRS_NULL, ap_buf, a_buf_size, a_eeprom_idx);
}
raw_eeprom_t::size_type raw_eeprom_t::size()
{
  return m_size;
}
void raw_eeprom_t::copy(size_type a_src_idx, size_type a_dest_idx,
  size_type a_size)
{
  IRS_LIB_ERROR_IF((a_src_idx + a_size > m_size) ||
    (a_dest_idx + a_size > m_size), ec_standard, "");
  if ((a_src_idx + a_size > m_size) || (a_dest_idx + a_size > m_size)) {
    return;
  }
  if (a_src_idx != a_dest_idx) {
    eeprom_pointer p_src = data() + a_src_idx;
    eeprom_pointer p_dest = data() + a_dest_idx;
    if (a_dest_idx < a_src_idx) {
      ::copy(p_src, p_src + a_size, p_dest);
    } else if (a_dest_idx > a_src_idx) {
      ::copy_backward(p_src, p_src + a_size, p_dest + a_size);
    }
  } else {
    // Ќичего не делаем если индексы совпадают - бесполезное зан€тие :)
  }
}
template <class T>
bool raw_eeprom_t::is_equal(size_type a_eeprom_idx, const T& a_value)
{
  IRS_LIB_ERROR_IF(a_eeprom_idx + sizeof(a_value) > m_size, ec_standard, "");
  if (a_eeprom_idx + sizeof(a_value) > m_size) {
    return false;
  }
  return reinterpret_cast<T __eeprom&>(data()[a_eeprom_idx]) == a_value;
}
irs_u32 raw_eeprom_t::calc_crc32(size_type a_eeprom_idx, size_type a_size)
{
  IRS_LIB_ERROR_IF(a_eeprom_idx + a_size > m_size, ec_standard, "");
  if (a_eeprom_idx + a_size > m_size) {
    return 0;
  }
  return crc32_table(data() + a_eeprom_idx, a_size);
}

} //namespace avr

} //namespace irs

// ≈сли a_eeprom_id == m_old_eeprom_id, то работаем со старым EEPROM
// ѕри этом никаких конвертаций из нового обратно в старый не предусмотрено
// ≈сли a_eeprom_id != m_old_eeprom_id, то работаем c новым EEPROM
// ѕри этом старый EEPROM конверитруетс€ в новый, если он находилс€
// Ќа той же позиции, что указанно в первых двух параметрах конструктора
// Ћибо если прилагает€ структура ap_old_eeprom_data с положением старого
// EEPROM
irs::avr::eeprom_protected_t::eeprom_protected_t(
  size_type a_start_index,
  size_type a_size,
  id_type a_eeprom_id,
  old_eeprom_data_t* ap_old_eeprom_data
):
  mp_eeprom_header(),
  mp_eeprom1(),
  mp_eeprom2(),
  m_error(false),
  m_partial_error(false),
  mp_header_conn()
{
  eeprom_protected_init_t eeprom_init =
    zero_struct_t<eeprom_protected_init_t>::get();
  
  if (a_eeprom_id != m_old_eeprom_id) {
    // работаем с новым EEPROM
    new_eeprom_data_init(a_start_index, a_size, a_eeprom_id, &eeprom_init);
    
    bool is_pos_changed = false;
    size_type finded_idx = 0;
    if (find_eeprom(a_eeprom_id, a_start_index, &finded_idx,
      &is_pos_changed))
    {
      // новый найден
      eeprom_t prev_header_eeprom(finded_idx, m_eeprom_header_size);
      header_conn_t prev_header_conn(&prev_header_eeprom);
      size_type prev_eeprom_size = prev_header_conn.eeprom_size;
      bool is_size_changed = (prev_eeprom_size != eeprom_init.size);
      if (is_pos_changed || is_size_changed) {
        // положение или размер изменились
        // копируем пердыдущий EEPROM в новый
        size_type prev_eeprom_first_idx =
          finded_idx + m_eeprom_header_full_size;
        copy_prev_to_new(&eeprom_init, prev_eeprom_first_idx,
          prev_eeprom_size);
        m_partial_error = true;
      } else {
        // положение и размер не изменились
        // ничего не копируетс€
      }
    } else {
      // новый не найден
      size_type prev_eeprom_size = 0;
      size_type prev_eeprom_first_idx = 0;
      if (ap_old_eeprom_data != IRS_NULL) {
        // есть ap_old_eeprom_data
        // копируем старый EEPROM в новый или создаем новый
        prev_eeprom_size = ap_old_eeprom_data->size;
        prev_eeprom_first_idx = ap_old_eeprom_data->index;
      } else {
        // нет ap_old_eeprom_data
        // копируем старый EEPROM в новый или создаем новый
        prev_eeprom_size = eeprom_init.size;
        prev_eeprom_first_idx = eeprom_init.start_index;
      }
      copy_prev_to_new(&eeprom_init, prev_eeprom_first_idx,
        prev_eeprom_size);
    }
    
    new_eeprom_init(&eeprom_init);
    restore_proc(&eeprom_init);
  } else {
    // работаем со старым EEPROM
    // стандартна€ обработка старого EEPROM
    old_eeprom_data_init(a_start_index, a_size, &eeprom_init);
    old_eeprom_init(&eeprom_init);
    restore_proc(&eeprom_init);
  }
}

irs::avr::eeprom_protected_t::~eeprom_protected_t()
{
}

bool irs::avr::eeprom_protected_t::find_eeprom(id_type a_finding_id,
  size_t a_expected_id_idx, size_t* a_finded_idx,
  bool* a_is_pos_changed)
{
  bool is_finded = false;
  size_t eeprom_size = raw_eeprom_t::size() - sizeof(id_type);
  if (!raw_eeprom_t::is_equal(a_expected_id_idx + m_crc_size, a_finding_id)) {
    *a_is_pos_changed = true;
    for (size_t eeprom_idx = m_crc_size; eeprom_idx < eeprom_size;
      eeprom_idx++)
    {
      if (raw_eeprom_t::is_equal(eeprom_idx, a_finding_id)) {
        *a_finded_idx = eeprom_idx - m_crc_size;
        is_finded = true;
        break;
      }
    }
  } else {
    is_finded = true;
    *a_is_pos_changed = false;
    *a_finded_idx = a_expected_id_idx;
  }
  return is_finded;
}

void irs::avr::eeprom_protected_t::restore_proc(
  const eeprom_protected_init_t* ap_eeprom_init)
{
  if (mp_eeprom1->error() && !mp_eeprom2->error()) {
    raw_eeprom_t::copy(ap_eeprom_init->second_array_idx,
      ap_eeprom_init->first_array_idx, ap_eeprom_init->size);
  } else if (!mp_eeprom1->error() && mp_eeprom2->error()) {
    raw_eeprom_t::copy(ap_eeprom_init->first_array_idx,
      ap_eeprom_init->second_array_idx, ap_eeprom_init->size);
  } else if (mp_eeprom1->error() && mp_eeprom2->error()) {
    m_error = true;
  }
}

void irs::avr::eeprom_protected_t::eeprom_data_init(size_type a_shift,
  size_type a_start_index, size_type a_size, id_type a_id,
  eeprom_protected_init_t* ap_eeprom_init) const
{
  ap_eeprom_init->start_index = a_start_index;
  ap_eeprom_init->size = a_size;
  ap_eeprom_init->id = a_id;
  ap_eeprom_init->first_array_idx = ap_eeprom_init->start_index + a_shift;
  //m_crc_size + m_eeprom_header_size;
  ap_eeprom_init->second_array_idx = ap_eeprom_init->first_array_idx +
    m_crc_size + a_size;
}
void irs::avr::eeprom_protected_t::new_eeprom_data_init(
  size_type a_start_index, size_type a_size, id_type a_id,
  eeprom_protected_init_t* ap_eeprom_init) const
{
  eeprom_data_init(m_crc_size + m_eeprom_header_size, a_start_index, a_size,
    a_id, ap_eeprom_init);
}
void irs::avr::eeprom_protected_t::old_eeprom_data_init(
  size_type a_start_index, size_type a_size,
  eeprom_protected_init_t* ap_eeprom_init) const
{
  eeprom_data_init(0, a_start_index, a_size, m_old_eeprom_id, ap_eeprom_init);
}

void irs::avr::eeprom_protected_t::new_eeprom_init(
  const eeprom_protected_init_t* ap_eeprom_init)
{
  mp_eeprom_header.reset(new eeprom_t(ap_eeprom_init->start_index,
    m_eeprom_header_size));
  mp_header_conn.reset(
    IRS_LIB_NEW_ASSERT(header_conn_t(mp_eeprom_header.get(), 0),
    IRSEEPROMCPP_IDX));
  mp_header_conn->eeprom_id = ap_eeprom_init->id;
  mp_header_conn->eeprom_size = ap_eeprom_init->size;
  array_handle_init(ap_eeprom_init);
}
void irs::avr::eeprom_protected_t::old_eeprom_init(
  const eeprom_protected_init_t* ap_eeprom_init)
{
  array_handle_init(ap_eeprom_init);
}
void irs::avr::eeprom_protected_t::array_handle_init(
  const eeprom_protected_init_t* ap_eeprom_init)
{
  mp_eeprom1.reset(IRS_LIB_NEW_ASSERT(
    eeprom_t(ap_eeprom_init->first_array_idx, ap_eeprom_init->size),
    IRSEEPROMCPP_IDX));
  mp_eeprom2.reset(IRS_LIB_NEW_ASSERT(
    eeprom_t(ap_eeprom_init->second_array_idx, ap_eeprom_init->size),
    IRSEEPROMCPP_IDX));
}

void irs::avr::eeprom_protected_t::copy_prev_to_new(
  const eeprom_protected_init_t* ap_eeprom_init,
  size_type a_prev_eeprom_first_idx, size_type a_prev_eeprom_size)
{
  size_type prev_eeprom_full_size = m_crc_size + a_prev_eeprom_size;
  eeprom_t prev_eeprom_first(a_prev_eeprom_first_idx, a_prev_eeprom_size);
  size_type prev_eeprom_second_idx =
    a_prev_eeprom_first_idx + prev_eeprom_full_size;
  eeprom_t prev_eeprom_second(prev_eeprom_second_idx, a_prev_eeprom_size);
  
  if (!prev_eeprom_first.error()) {
    raw_eeprom_t::copy(a_prev_eeprom_first_idx,
      ap_eeprom_init->first_array_idx,
      min(a_prev_eeprom_size, ap_eeprom_init->size));
  } else if (!prev_eeprom_second.error()) {
    raw_eeprom_t::copy(prev_eeprom_second_idx,
      ap_eeprom_init->first_array_idx,
      min(a_prev_eeprom_size, ap_eeprom_init->size));
  } else {
    m_error = true;
  }
  
  irs_u32 check_sum = raw_eeprom_t::calc_crc32(
    ap_eeprom_init->first_array_idx + m_crc_size,
    ap_eeprom_init->size);
  raw_eeprom_t::write(
    reinterpret_cast<raw_eeprom_t::pointer>(&check_sum),
    sizeof(check_sum), ap_eeprom_init->first_array_idx + m_crc_shift);
}

irs::avr::eeprom_protected_t::size_type irs::avr::eeprom_protected_t::size()
{
  return mp_eeprom1->size();
}

irs_bool irs::avr::eeprom_protected_t::connected()
{
  return irs_true;
}

void irs::avr::eeprom_protected_t::read(irs_u8 *buf, irs_uarc index,
  irs_uarc size)
{
  mp_eeprom1->read(buf, index, size);
}

void irs::avr::eeprom_protected_t::write(const irs_u8 *buf, irs_uarc index,
  irs_uarc size)
{
  mp_eeprom1->write(buf, index, size);
  mp_eeprom2->write(buf, index, size);
}

irs_bool irs::avr::eeprom_protected_t::bit(irs_uarc index, irs_uarc bit_index)
{
  return mp_eeprom1->bit(index, bit_index);
}

void irs::avr::eeprom_protected_t::set_bit(irs_uarc index, irs_uarc bit_index)
{
  mp_eeprom1->set_bit(index, bit_index);
  mp_eeprom2->set_bit(index, bit_index);
}

void irs::avr::eeprom_protected_t::clear_bit(irs_uarc index,
  irs_uarc bit_index)
{
  mp_eeprom1->clear_bit(index, bit_index);
  mp_eeprom2->clear_bit(index, bit_index);
}

void irs::avr::eeprom_protected_t::tick()
{
}

bool irs::avr::eeprom_protected_t::error()
{
  return m_error;
}

bool irs::avr::eeprom_protected_t::partial_error()
{
  return m_partial_error;
}
