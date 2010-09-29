// Доступ к EEPROM AVR
// Дата: 9.08.2010
// Ранняя Дата: 19.08.2009


// Для сохранения данных в eeprom при перепрошивке
// необходимо выставить Fuse:
// Preserve EEPROM memory through Chip Erase cycle

#ifndef irseepromh
#define irseepromh

#include <ioavr.h>

#include <irsdefs.h>
#include <irsstd.h>
#include <irsalg.h>

namespace irs
{
namespace avr
{

class eeprom_t : public mxdata_t
{
public:
  enum {
    m_begin_address = 0x0002
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
    m_end_address = m_begin_address +
      irs_uarc((E2END - m_begin_address)/4) * 4,
    m_data_size = m_end_address - m_begin_address
  };

  static __no_init __eeprom irs_u8 mp_ee_data[m_data_size] @ m_begin_address;

  irs_uarc m_start_index;
  irs_uarc m_size;
  bool m_crc_error;
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


class eeprom_protected_t : public mxdata_t
{
public:
  struct old_eeprom_data_t {
    irs_uarc index;
    irs_uarc size;

    old_eeprom_data_t(
      irs_uarc a_index,
      irs_uarc a_size
    ):
      index(a_index),
      size(a_size)
    {
    }
  };

  eeprom_protected_t(
    irs_uarc a_start_index,
    irs_uarc a_size,
    irs_u32 a_eeprom_id = 0,
    old_eeprom_data_t* ap_old_eeprom_data = IRS_NULL);
  ~eeprom_protected_t();
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
    m_eeprom_id_size = sizeof(irs_u32),
    m_eeprom_size_size = sizeof(irs_uarc),
    m_eeprom_header_size = m_eeprom_size_size + m_eeprom_id_size,
    m_eeprom_begin_address = eeprom_t::m_begin_address,
    m_eeprom_size = irs_uarc((E2END - m_eeprom_begin_address)/4) * 4,
    m_crc_size = sizeof(irs_u32)
  };

  handle_t<eeprom_t> mp_eeprom_header;
  handle_t<eeprom_t> mp_eeprom1;
  handle_t<eeprom_t> mp_eeprom2;
  bool m_error;

  bool old_eeprom_id_index(irs_u32 a_search_id, irs_uarc& a_old_eeprom_id_idx);
};

} // namespace avr
} // namespace irs

#endif  //  irseepromh
