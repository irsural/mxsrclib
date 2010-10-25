//! \file
//! \ingroup drivers_group
//! \brief Доступ к EEPROM AVR
//!
//! Дата: 22.10.2010\n
//! Ранняя дата: 19.08.2009


#ifndef irseepromh
#define irseepromh

// Для сохранения данных в eeprom при перепрошивке
// необходимо выставить Fuse:
// Preserve EEPROM memory through Chip Erase cycle

// Номер файла
#define IRSEEPROMH_IDX 20

#include <irsdefs.h>

#include <ioavr.h>

#include <irsstd.h>
#include <irsalg.h>

#include <irsfinal.h>

namespace irs
{
namespace avr
{

//! \addtogroup drivers_group
//! @{

class eeprom_t : public mxdata_t
{
public:
  enum {
    m_begin_address = 0x0002,
    m_end_address = m_begin_address +
      irs_uarc((E2END - m_begin_address)/4) * 4
  };

  eeprom_t(irs_uarc a_start_index, irs_uarc a_size);
  ~eeprom_t();
  irs_uarc size();
  irs_bool connected();
  void read(irs_u8 *buf, irs_uarc index, irs_uarc size);
  void write(const irs_u8 *buf, irs_uarc index, irs_uarc size);
  irs_bool bit(irs_uarc index, irs_uarc bit_index);
  void set_bit(irs_uarc index, irs_uarc bit_index);
  void clear_bit(irs_uarc index, irs_uarc bit_index);
  void tick();
  bool error();
private:
  enum {
    m_crc_size = sizeof(irs_u32),
    m_data_size = m_end_address - m_begin_address
  };

  static __no_init __eeprom irs_u8 mp_ee_data[m_data_size] @ m_begin_address;

  irs_uarc m_start_index;
  irs_uarc m_size;
  bool m_crc_error;
  
  irs_u32 calc_old_crc() const;
  irs_u32 calc_new_crc() const;
  irs_u32 __eeprom& crc_eeprom() const;
};

class eeprom_reserve_t : public mxdata_t
{
public:
  eeprom_reserve_t(eeprom_t *ap_eeprom1, eeprom_t *ap_eeprom2,
    irs_uarc a_size);
  ~eeprom_reserve_t();
  irs_uarc size();
  irs_bool connected();
  void read(irs_u8 *buf, irs_uarc index, irs_uarc size);
  void write(const irs_u8 *buf, irs_uarc index, irs_uarc size);
  irs_bool bit(irs_uarc index, irs_uarc bit_index);
  void set_bit(irs_uarc index, irs_uarc bit_index);
  void clear_bit(irs_uarc index, irs_uarc bit_index);
  void tick();
  bool double_error();
private:
  eeprom_t *mp_eeprom1;
  eeprom_t *mp_eeprom2;
  irs_uarc m_size;
  bool m_error;
};


struct eeprom_protected_traits_t {
  typedef irs_u32 id_type;
  typedef size_t size_type;
};

struct eeprom_protected_init_t {
  typedef eeprom_protected_traits_t::id_type id_type;
  typedef eeprom_protected_traits_t::size_type size_type;
  
  size_type start_index;
  size_type size;
  id_type id;
  size_type first_array_idx;
  size_type second_array_idx;
};

class eeprom_protected_t : public mxdata_t
{
public:
  typedef eeprom_protected_traits_t::id_type id_type;
  typedef eeprom_protected_traits_t::size_type size_type;

  enum {
    m_old_eeprom_id = 0
  };

  struct old_eeprom_data_t {
    size_type index;
    size_type size;

    old_eeprom_data_t(
      size_type a_index,
      size_type a_size
    ):
      index(a_index),
      size(a_size)
    {
    }
  };

  struct header_conn_t {
    conn_data_t<id_type> eeprom_id;
    conn_data_t<size_type> eeprom_size;

    header_conn_t(mxdata_t* ap_data = IRS_NULL, irs_uarc a_index = 0)
    {
      connect(ap_data, a_index);
    }
    size_type connect(mxdata_t* ap_data, irs_uarc a_index = 0)
    {
      a_index = eeprom_id.connect(ap_data, a_index);
      a_index = eeprom_size.connect(ap_data, a_index);
      return a_index;
    }
  };

  eeprom_protected_t(
    size_type a_start_index,
    size_type a_size,
    id_type a_eeprom_id = m_old_eeprom_id,
    old_eeprom_data_t* ap_old_eeprom_data = IRS_NULL);
  ~eeprom_protected_t();
  size_type size();
  irs_bool connected();
  void read(irs_u8 *buf, irs_uarc index, irs_uarc size);
  void write(const irs_u8 *buf, irs_uarc index, irs_uarc size);
  irs_bool bit(irs_uarc index, irs_uarc bit_index);
  void set_bit(irs_uarc index, irs_uarc bit_index);
  void clear_bit(irs_uarc index, irs_uarc bit_index);
  void tick();
  bool error();
  bool partial_error();
private:
  enum {
    m_crc_size = sizeof(irs_u32),
    m_crc_shift = 0,
    m_id_size = sizeof(id_type),
    m_id_shift = m_crc_shift + m_crc_size,
    m_size_size = sizeof(size_type),
    m_size_shift = m_id_shift + m_id_size,
    m_eeprom_header_size = m_size_size + m_id_size,
    m_eeprom_header_full_size = m_crc_size + m_eeprom_header_size
  };

  handle_t<eeprom_t> mp_eeprom_header;
  handle_t<eeprom_t> mp_eeprom1;
  handle_t<eeprom_t> mp_eeprom2;
  bool m_error;
  bool m_partial_error;
  handle_t<header_conn_t> mp_header_conn;

  bool find_eeprom(id_type a_finding_id, size_t a_expected_id_idx,
    size_type* a_finded_idx, bool* a_is_pos_changed);
  void restore_proc(const eeprom_protected_init_t* ap_eeprom_init);
  void eeprom_data_init(size_type a_shift, size_type a_start_index,
    size_type a_size, id_type a_id,
    eeprom_protected_init_t* ap_eeprom_init) const;
  void new_eeprom_data_init(size_type a_start_index,
    size_type a_size, id_type a_id,
    eeprom_protected_init_t* ap_eeprom_init) const;
  void old_eeprom_data_init(size_type a_start_index,
    size_type a_size, eeprom_protected_init_t* ap_eeprom_init) const;
  void eeprom_data_init(size_type a_shift, size_type a_start_index,
    size_type a_size, eeprom_protected_init_t* ap_eeprom_init);
  void new_eeprom_init(const eeprom_protected_init_t* ap_eeprom_init);
  void old_eeprom_init(const eeprom_protected_init_t* ap_eeprom_init);
  void array_handle_init(const eeprom_protected_init_t* ap_eeprom_init);
  void copy_prev_to_new(const eeprom_protected_init_t* ap_eeprom_init,
    size_type a_prev_eeprom_first_idx, size_type a_prev_eeprom_size);
};

//! @}

} // namespace avr

} // namespace irs

#endif  //  irseepromh
