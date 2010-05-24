// Дискретные ввод/вывод для ARM Linux Олега Коновалова
// Дата: 24.05.2010

#ifndef armioH
#define armioH

#include <irsdefs.h>
#include <armmap.h>
#include <irsstd.h>
#include <irscpp.h>
//#include <irsio.h>

/*************************/
namespace irs
{
  namespace arm
  {
    class io_t: public mxdata_t
    {
        int             m_fdio, idio;
        void            *io_map_base;
        unsigned int    wreval;
      public:
      //  enum status_t   {HI,LOW};
        enum            io_bank {IOA_ID = 2,IOB_ID = 3,IOC_ID = 4,IOD_ID = 5};
        io_bank         arm_bank;
        io_t(irs_bool bank_A,irs_bool bank_B,irs_bool bank_C,irs_bool bank_D);
        ~io_t() {}
        irs_uarc io_tuning(io_bank a_bank,irs_u8 line,irs_bool PER,
          irs_bool OER,irs_bool IFER, irs_bool SODR,irs_bool IER,irs_bool MDER,
          irs_bool PUER,irs_bool ASR);
/*        virtual irs_uarc set_io(io_bank a_bank,irs_u8 line);
        virtual irs_uarc clear_io(io_bank a_bank,irs_u8 line);
        virtual void get_status(io_bank a_bank,irs_u8 line);
        virtual irs_bool get_on_off_status(io_bank a_bank,irs_u8 line);
*/
        virtual irs_uarc size();
        virtual irs_bool connected();
        virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
        virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
        virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
        virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
        virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
        virtual void tick();
        //virtual void status();

    };
  }
}

#endif
