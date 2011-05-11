// Дата: 11.05.2011
// Дата создания: 6.05.2011

#ifndef armflashh
#define armflashh

#include <irsdefs.h>

#include <armioregs.h>
#include <mxdata.h>

#include <irsfinal.h>

namespace irs {

namespace arm {

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
private:
  enum {
    m_flash_page_size = 0x400, // 1 kB
    m_irs_u32_size = sizeof(irs_u32),
    m_buf_size = m_flash_page_size
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
  
  void flash_erase(irs_u32 a_index);
  void flash_write_block(irs_u8* ap_buf, irs_u32 a_index);
  void flash_read_block(irs_u8* ap_buf, irs_u32 a_index);
}; // flash_t

} // namespace arm

} // namespace irs

#endif // armflashh
