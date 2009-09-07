// SPI для ARM Linux Олега Коновалова
// Дата: 12.02.2009

#ifndef armspiH
#define armspiH

#include <irsdefs.h>
#include <irscpp.h>
#include <irsspi.h>
#include <at91_spimode.h>

namespace irs
{

struct spi_param_t
{
  irs_u8 cpol;
  irs_u8 ncpha;
  irs_u8 bpt;
  irs_u8 brate;
  irs_u8 dbspck;
  irs_u8 dbtrans;

};

class armspi_t : public spi_t
{
  static const irs_uarc   SIZE_OF_BUFFER = 500;
  enum                    m_mode_t {spi_read,spi_write,spi_wait,spi_cs};
  m_mode_t                m_spimode;
  irs_bool                m_is_read;
  irs_bool                m_is_write;
  irs_bool                m_msbset;
  irs_u8                  m_rbuf[SIZE_OF_BUFFER];
  irs_u8                  m_device;
  irs_u8                  m_line;
  irs_u8                  m_rbufsize;
  irs_u8                  m_wbuf[SIZE_OF_BUFFER];
  irs_u8                  m_wbufsize;
  irs_u8                  m_cpol;
  irs_u8                  m_ncpha;
  irs_u8                  m_bpt;
  irs_u8                  m_brate;
  irs_u8                  m_dbspck;
  irs_u8                  m_dbtrans;
  int                     m_spi_fd;
  struct spimode_t        inspi,*spiPtr;
  irs_bool                select_of_cs(irs_uarc cs);
  void                    reverse(void);
public:
  struct spi_param_t spipar;
//  enum status_t   {FREE,IDLE};
//  enum polarity_t {POSITIVE_EDGE, ZERO_EDGE};
//  enum phase_t    {LEAD_EDGE, TRAIL_EDGE};
//  enum order_t    {LSB,MSB};
  status_t        spi_status;
  polarity_t      spi_pol;
  phase_t         spi_phase;
  armspi_t(irs_uarc MCK_div,irs_u8 SPI_mode,irs_uarc width);
  ~armspi_t();
  virtual void abort();
  virtual void read(irs_u8 *ap_buf,irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf,irs_uarc a_size);
  virtual void set_cs(irs_uarc line_num);
  virtual void reset_cs();
  virtual spi_t::status_t get_status();
  irs_bool set_bpt(irs_u8 a_bpt);
  virtual bool set_bitrate(irs_u32 a_bitrate);
  virtual bool set_polarity(spi_t::polarity_t a_polarity);
  virtual bool set_phase(spi_t::phase_t a_phase);
  virtual bool set_order(spi_t::order_t a_order);
  virtual void tick();
  void status();
  void set_cpuclock(char var);
  irs_uarc fast_read(irs_u8 *ap_buf,irs_uarc a_size);
  irs_uarc fast_write(irs_u8 *ap_buf,irs_uarc a_size);
};

}

#endif //armspiH
