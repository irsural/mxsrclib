// Дата: 11.05.2011
// Дата создания: 6.05.2011

#ifndef armflashh
#define armflashh

#include <irsdefs.h>

#include <armioregs.h>
#include <mxdata.h>

#include <irsfinal.h>

#define FLASH_END_ADDR 0x40000

namespace irs {

namespace arm {

//! \addtogroup drivers_group
//! @{

class flash_t: public mxdata_t
{
public:
  typedef size_t size_type;

  flash_t(size_t a_size, size_t a_flash_end_addr = 0x40000);
  virtual ~flash_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
  bool error();
private:
  enum {
    m_flash_page_size = 0x400, // 1 kB
    m_irs_u32_size = sizeof(irs_u32),
    m_buf_size = m_flash_page_size,
    m_crc_size = sizeof(irs_u32)
  };
  enum {
    m_WRITE_KEY = 0xA4420000,
    m_WRITE = 0x1,
    m_ERASE = 0x2,
    m_MERASE = 0x4
  };
  size_type m_size;
  size_type m_flash_end_addr;
  size_type m_flash_begin_addr;
  bool m_crc_error;

  void flash_erase(irs_u32 a_index);
  void flash_write_block(irs_u8* ap_buf, irs_u32 a_index);
  void flash_read_block(irs_u8* ap_buf, irs_u32 a_index);
  irs_u32 crc_flash();
  irs_u32 calc_old_crc() const;
  irs_u32 calc_new_crc() const;
}; // flash_t

//! @}

class flash_protected_t: public mxdata_t
{
public:
  typedef flash_t::size_type size_type;

  flash_protected_t(size_t a_size, size_t a_flash_end_addr = 0x40000);
  virtual ~flash_protected_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
  bool double_error();
private:
  enum {
    m_crc_size = sizeof(irs_u32),
    m_flash_page_size = 0x400 // 1 kB
  };
  size_type m_size;
  flash_t m_flash1;
  flash_t m_flash2;
  bool m_crc_error;
}; // flash_protected_t

#ifdef IRS_STM32F_2_AND_4

//! \addtogroup drivers_group
//! @{

//! \brief Драйвер главной области флеш-памяти для контроллеров
//!   семейств STM32F2xx и STM32F4xx
//! \author Lyashchov Maxim
class st_flash_t: public various_page_mem_t
{
public:
  typedef std::size_t size_type;
  st_flash_t();
  virtual ~st_flash_t();
  virtual void read(irs_u8* ap_pos, irs_u8 *ap_buf, size_type a_buf_size);
  virtual void erase_page(size_type a_page_index);
  virtual void write(irs_u8* ap_pos, const irs_u8 *ap_buf,
    size_type a_buf_size);
  virtual irs_u8* page_begin(size_type a_page_index);
  virtual size_type page_size(size_type a_page_index) const;
  virtual size_type page_count() const;
  virtual irs_status_t status() const;
  virtual void tick();
private:
  void process_end(FLASH_Status a_flash_status);
  void write_tick(irs_u8** ap_pos, const irs_u8** ap_begin,
    const irs_u8* ap_end);
  static irs_status_t convert_status(FLASH_Status a_flash_status);
  static irs_u32 psize();
  static size_type psize_to_byte_count(irs_u32 a_psize);
  static irs_u32 byte_count_to_psize(size_type a_bype_count);
  static irs_u8 voltage_range();
  enum {
    sector_count = 12
  };
  enum process_t {
    process_erase,
    process_wait_for_erase_operation,
    process_write,
    process_wait_for_write_operation,
    process_wait_command
  };

  process_t m_process;
  irs_status_t m_status;
  irs_u8* mp_pos;
  const irs_u8* mp_buf;
  const irs_u8* mp_end;
  size_type m_page_index;
  size_type m_max_psize;
  size_type m_max_byte_count;
};

//! @}

#endif  //  IRS_STM32F_2_AND_4

} // namespace arm

} // namespace irs

#endif // armflashh
