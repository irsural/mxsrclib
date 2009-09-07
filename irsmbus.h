// Клиент и сервер modbus написаный Олегом Коноваловым
// Дата: 7.02.2008

#ifndef irsmbusH
#define irsmbusH

#include <mxifa.h>
#include <irsstd.h>
#include <irscpp.h>

namespace irs {

struct complex_modbus_header_t {
  irs_u16       transaction_id;
  irs_u16       proto_id;
  irs_u16       length;
  irs_u8        unit_identifier;
  irs_u8        function_code;
};
struct request_read_t {
  irs_u16       starting_address;
  irs_u16       num_of_regs;
};
struct request_wr_coils_t
{
  irs_u8        byte_count;
  irs_u8        value[1];
};
struct request_wr_regs_t
{
  irs_u8        byte_count;
  irs_u16       value[1];
};
struct wr_sreg_t
{
  irs_u16       num_of_regs;
  irs_u8       value[1];
};

struct packet_data_8_t
{
  irs_u8        byte_count;
  irs_u8        value[1];
};
struct packet_data_16_t
{
  irs_u8        byte_count;
  irs_u16       value[1];
};
struct pack_tb_t
{
  irs_u16       value[1];
};
struct pack_d8_t
{
  irs_u8       value[1];
};


class irsmb_se : public mxdata_t
{
  private:
    static const irs_u8                   size_of_data                    = 248;
    static const irs_u8                   size_of_MBAP                    = 6;
    static const irs_u8                   size_of_res                     = 3;
    static const irs_u16                  size_of_packet                  = 260;
    static const irs_u16                  s_regbuf                        = 124;
    static const irs_u8                   size_of_header                  = 8;
    static const irs_u8                   size_of_read_header             = 4;
    static const irs_u8                   Rcoil = 1,R_DI = 2,R_HR = 3,R_IR= 4,
                                          Wcoil = 5,W_HR = 6,Status = 7;
    static const irs_u8                   W_SevHR = 10,WMcoils = 15,
                                          WM_regs = 16,Rsize = 70,Rtab = 30;
    irs_u8                                *mp_buf;
    irs_u8                                mess[size_of_packet];
    irs_uarc                              m_size_end;
    irs_uarc                              m_size;
    irs_u16                               m_staddr;
    irs_u16                               m_nregs;
    irs_bool                              m_rcoil;
    irs_u16                               coils_inbits;
    irs_u16                               di_inbits;
    irs_u32                               hr_inbytes;
    irs_u32                               ir_inbytes;
    irs_u32                               di_end;
    irs_u32                               coils_end;
    irs_u32                               hr_end;
    irs_u32                               ir_end;
    mxifa_dest_t                          m_mbchdata;
    void                                  *mp_handle;
    enum                                  m_condition {read_mode,read_pack,
      write_mode,
                                          wait_mode,read_end,write_end};
    m_condition                           mode;
    vector<bool>                          m_discr_inputs;
    vector<bool>                          m_coils;
    vector<irs_u16>                       m_input_regs;
    vector<irs_u16>                       m_hold_regs;
    irs_u8                                in_str[size_of_packet];
    irs_u32                               di_size, di_start;
    irs_u32                               coils_size, coils_start;
    irs_u32                               hr_size, hr_start;
    irs_u32                               ir_size, ir_start;
    void Error_response(irs_u8 error_code);
    void BintoDec(vector<bool> &bits,irs_u32 index,packet_data_8_t &data);
    void DectoBin(vector<bool> &bits,irs_u32 index,
          const packet_data_8_t &data);
    void DectoBin(vector<bool> &bits,irs_u32 index,const pack_d8_t &data,
          irs_u16 bcount);
    void get_bytes(vector<irs_u16> &regs,irs_u32 index,irs_u32 blenth,
          irs_u8 *buf,irs_u32 start);
    void write_bytes(vector<irs_u16> &regs,irs_u32 index,irs_u32 blenth,
          const irs_u8 *buf,irs_u32 start);
    void range(irs_u32 index, irs_u32 size, irs_u32 M1, irs_u32 M2,
          irs_u32 *num,irs_u32 *start);
    void convert(irs_u8 *ap_mess,irs_u8 a_start,irs_u8 a_length);

  public:
    irsmb_se(
      mxifa_ch_t channel,
      irs_u16 local_port,
      irs_u16 a_discr_inputs_size = 8191,
      irs_u16 a_coils_size = 8191,
      irs_u16 a_hold_regs = 2500,
      irs_u16 a_input_regs = 2500
    );
    virtual ~irsmb_se() {}
    virtual irs_uarc size();
    virtual irs_bool connected();
    virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
    virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
    virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
    virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
    virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
    virtual void tick();
    virtual void status();
};

class irsmb_cl : public mxdata_t
{
  private:
    static const irs_u8                   size_of_MBAP        = 6;
    static const irs_u8                   size_of_data        = 248;
    static const irs_u16                  size_of_packet      = 260;
    static const irs_u16                  s_regbuf            = 124;
    static const irs_u8                   size_of_header      = 8;
    static const irs_u8                   size_of_read_header = 4;
    static const irs_u8                   buf_size            = 124;
    static const irs_u8                   buf_bit_size        = 248;
    static const irs_u8                   size_of_res         = 6;
    counter_t                             del_time;
    irs_u8                                *mp_buf;
    irs_u16                               m_global_index;
    irs_u32                               m_ibank;
    irs_u16                               coils_inbits;
    irs_u16                               di_inbits;
    irs_u32                               hr_inbytes;
    irs_u32                               ir_inbytes;
    irs_u32                               di_end;
    irs_u32                               coils_end;
    irs_u32                               hr_end;
    irs_u32                               ir_end;
    irs_u8                                m_spacket[size_of_packet];
    irs_u8                                m_rpacket[size_of_packet];
    irs_uarc                              m_size_end;
    irs_bool                              m_nothing;
    irs_bool                              m_read_table;
    irs_bool                              m_first_read;
    irs_bool                              m_part_send;
    mxifa_ch_t                            m_channel;
    mxip_t                                m_dip;
    irs_u32                               m_nregs;
    irs_u32                               m_start_block;
    irs_u16                               m_dport;
    irs_u32                               search_index;
    irs_u16                               m_stpos;
    irs_u16                               m_bytes;
    irs_u32                               di_size, di_start;
    irs_u32                               coils_size, coils_start;
    irs_u32                               hr_size, hr_start;
    irs_u32                               ir_size, ir_start;
    void                                  *mp_handle;
    mxifa_dest_t                          m_mbchdata;
    vector<bool>                          m_rforwr;
    vector<bool>                          m_rforwr_wait;
    vector<bool>                          m_discr_inputs_r;
    vector<bool>                          m_coils_r;
    vector<bool>                          m_coils_w;
    vector<irs_u16>                       m_input_regs_r;
    vector<irs_u16>                       m_hold_regs_r;
    vector<irs_u16>                       m_hold_regs_w;
    vector<irs_u8>                        m_data_8;
    vector<irs_u16>                       m_data_16;
    vector<irs_u8>                        m_packet;
    enum                                  m_condition {Rcoil = 1,R_DI,R_HR,
                                            R_IR,Wcoil,W_HR,Status,
                                            W_SevHR = 10,WMcoils = 15,
                                            WM_regs = 16,Rsize = 70,
                                            Rtab = 30,ERT =31};
    m_condition                           m_command, RT;
    enum                                  cl_cond {w_mode,wend_w,r_head_mode,
      r_pack_mode,
                                            wend_r,wait_mode,
                                            pr_mode,get_table,send_data,
                                            search_block};
    cl_cond                               m_mode;

    irs_u8                                in_str[size_of_packet];
    void make_packet(irs_uarc a_index, irs_u16 a_size);
    void BintoDec(vector<bool> &bits,irs_u32 index,packet_data_8_t &data);
    void DectoBin(vector<bool> &bits,irs_u32 index,const packet_data_8_t &data);
    void get_bytes(vector<irs_u16> &regs,irs_u32 index,irs_u32 blenth,
          irs_u8 *buf,irs_u32 start);
    void write_bytes(vector<irs_u16> &regs,irs_u32 index,irs_u32 blenth,
          const irs_u8 *buf,irs_u32 start);
    void range(irs_u32 index, irs_u32 size, irs_u32 M1, irs_u32 M2,
           irs_u32 *num,irs_u32 *start);
    void convert(irs_u8 *ap_mess,irs_u8 a_start,irs_u8 a_length);
  public:
    irsmb_cl(
      mxifa_ch_t channel,
      mxip_t dest_ip,
      irs_u16 dest_port,
      irs_u16 a_discr_inputs_size = 8191,
      irs_u16 a_coils_size = 8191,
      irs_u16 a_hold_regs = 2500,
      irs_u16 a_input_regs = 2500
    );
    void set_delay_time(double time);
    virtual ~irsmb_cl();
    virtual irs_uarc size();
    virtual irs_bool connected();
    virtual irs_bool connected_1();
    virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
    virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
    virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
    virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
    virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
    virtual void tick();
    virtual void status();
    //void write_regs();
};

} //namespace irs

#endif //irsmbusH
