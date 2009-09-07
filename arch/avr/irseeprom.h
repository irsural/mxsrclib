// Доступ к EEPROM AVR
// Дата: 19.08.2009

#ifndef correctH
#define correctH

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

  static const irs_uarc m_begin_address = 0x0002;
  static const irs_u8 m_crc_size = sizeof(irs_u32);
  static const irs_uarc m_end_address = m_begin_address +
    irs_uarc((E2END - m_begin_address)/4) * 4;
  static const irs_uarc m_data_size = m_end_address - m_begin_address;
  static __no_init __eeprom irs_u8 mp_ee_data[m_data_size] @ m_begin_address;

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
  bool m_crc_error;
  irs_uarc m_start_index;
  irs_uarc m_size;
};

class eeprom_reserve_t : public mxdata_t
{
  eeprom_t *mp_eeprom1;
  eeprom_t *mp_eeprom2;
  irs_uarc m_size;
  bool m_error;
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
};

class eeprom_protected_t : public mxdata_t
{
  const irs_uarc m_index_delta;
  irs_uarc m_size;
  eeprom_t m_eeprom1;
  eeprom_t m_eeprom2;
  bool m_error;
public:
  eeprom_protected_t(irs_uarc a_start_index, irs_uarc a_size);
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
};

} //avr
} //irs

#endif  //  correctH
