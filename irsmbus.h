// Клиент и сервер modbus написаный Олегом Коноваловым
// Дата: 7.02.2008

#ifndef irsmbusH
#define irsmbusH

#include <mxifa.h>
#include <irsstd.h>
#include <irscpp.h>
#include <mxdata.h>
#include <irsdefs.h>

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
/*struct pack_tb_t
{
  irs_u16       value[1];
};*/
struct pack_d8_t
{
  irs_u8       value[1];
  irs_u8       byte_count;
};

void test_bit_copy(ostream& strm, irs_u16 size_data_in, irs_u16 size_data_out, 
  irs_u32 index_data_in, irs_u32 index_data_out, irs_u16 size_data);  

class irsmb_se : public mxdata_t
{
public:
  typedef hardflow_t::size_type channel_type;
  
  irsmb_se(
    channel_type channel,
    irs_u16 local_port,
    irs_u16 a_discr_inputs_size_byte = 8191,
    irs_u16 a_coils_size_byte = 8191,
    irs_u16 a_hold_regs_reg = 2500,
    irs_u16 a_input_regs_reg = 2500
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
  
private:
  enum{
    size_of_data = 248,
    size_of_MBAP = 6,
    size_of_res = 3,
    size_of_packet = 260,
    s_regbuf = 124,
    size_of_header = 8,
    size_of_read_header =4,
    Rcoil = 1,
    R_DI = 2,
    R_HR = 3,
    R_IR= 4,
    Wcoil = 5,
    W_HR = 6,
    Status = 7,
    W_SevHR = 10,
    WMcoils = 15,
    WM_regs = 16,
    Rsize = 70,
    Rtab = 30
  };
  enum m_condition {
    read_mode,
    read_pack,
    write_mode,
    wait_mode,
    read_end,
    write_end
  };
  
  irs_u8                                *mp_buf;
  irs_u8                                mess[size_of_packet];
  irs_uarc                              m_size_byte_end;
  irs_uarc                              m_size_byte;
  irs_u16                               m_staddr;
  irs_u16                               m_nregs;
  irs_bool                              m_rcoil;
  irs_u16                               coils_bit;
  irs_u16                               di_bit;
  irs_u32                               hr_reg;
  irs_u32                               ir_reg;
  irs_u32                               di_end_byte;
  irs_u32                               coils_end_byte;
  irs_u32                               hr_end_byte;
  irs_u32                               ir_end_byte;
  mxifa_dest_t                          m_mbchdata;
  void                                  *mp_handle;
  m_condition                           mode;
  raw_data_t<irs_u8>                    m_discr_inputs_bit;
  raw_data_t<irs_u8>                    m_coils_bit;
  raw_data_t<irs_u16>                   m_input_regs_reg;
  raw_data_t<irs_u16>                   m_hold_regs_reg;
  irs_u8                                in_str[size_of_packet];
  irs_u32                               di_size_byte, di_start_byte;
  irs_u32                               coils_size_byte, coils_start_byte;
  irs_u32                               hr_size_byte, hr_start_byte;
  irs_u32                               ir_size_byte, ir_start_byte;
  void Error_response(irs_u8 error_code);
  void get_bytes(raw_data_t<irs_u16> &regs,irs_u32 index,irs_u32 blenth,
        irs_u8 *buf,irs_u32 start);
  void write_bytes(raw_data_t<irs_u16> &regs,irs_u32 index,irs_u32 blenth,
        const irs_u8 *buf,irs_u32 start);
  void range(irs_u32 index, irs_u32 size, irs_u32 M1, irs_u32 M2,
        irs_u32 *num,irs_u32 *start); 
};

class irsmb_cl : public mxdata_t
{
public:
  typedef hardflow_t::size_type channel_type;
  
  irsmb_cl(
    channel_type channel,
    mxip_t dest_ip,
    irs_u16 dest_port,
    irs_u16 a_discr_inputs_size_byte = 8191,
    irs_u16 a_coils_size_byte = 8191,
    irs_u16 a_hold_regs_reg = 2500,
    irs_u16 a_input_regs_reg = 2500
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
  
private:
  enum {
    size_of_MBAP =6,
    size_of_data = 248,
    size_of_packet = 260,
    s_regbuf = 124,
    size_of_header = 8,
    size_of_read_header = 4,
    buf_size = 124,
    buf_bit_size = 248,
    size_of_res = 6
  };
  enum m_condition {
    gt_start = 0,
    Rcoil = 1,
    R_DI,
    R_HR,
    R_IR,
    Wcoil,
    W_HR,
    Status,
    W_SevHR = 10,
    WMcoils = 15,
    WM_regs = 16,
    Rsize = 70,
    Rtab = 30,
    ERT =31
  };
  enum mode_t {
    w_mode,
    wend_w,
    r_head_mode,
    r_pack_mode,
    wend_r,
    wait_mode,
    pr_mode,
    get_table,
    send_data,
    search_block
  };
  
  counter_t                             del_time;
  irs_u8                                *mp_buf;
  irs_u16                               m_global_index;
  irs_u32                               m_ibank;
  irs_u16                               coils_bit;
  irs_u16                               di_bit;
  irs_u32                               hr_reg;
  irs_u32                               ir_reg;
  irs_u32                               di_end_byte;
  irs_u32                               coils_end_byte;
  irs_u32                               hr_end_byte;
  irs_u32                               ir_end_byte;
  irs_u8                                m_spacket[size_of_packet];
  irs_u8                                m_rpacket[size_of_packet];
  irs_uarc                              m_size_byte_end;
  irs_bool                              m_nothing;
  irs_bool                              m_read_table;
  irs_bool                              m_first_read;
  irs_bool                              m_part_send;
  //mxifa_ch_t                            m_channel;
  channel_type                          m_channel;
  mxip_t                                m_dip;
  irs_u32                               m_nregs;
  irs_u32                               m_start_block;
  irs_u16                               m_dport;
  irs_u32                               search_index;
  irs_u16                               m_stpos;
  irs_u16                               m_bytes;
  irs_u32                               di_size_byte, di_start_byte;
  irs_u32                               coils_size_byte, coils_start_byte;
  irs_u32                               hr_size_byte, hr_start_byte;
  irs_u32                               ir_size_byte, ir_start_byte;
  void                                  *mp_handle;
  mxifa_dest_t                          m_mbchdata;
  vector<bool>                          m_rforwr;
  raw_data_t<irs_u8>                    m_discr_inputs_bit_r;
  raw_data_t<irs_u8>                    m_coils_bit_r;
  raw_data_t<irs_u8>                    m_coils_bit_w;
  raw_data_t<irs_u16>                   m_input_regs_reg_r;
  raw_data_t<irs_u16>                   m_hold_regs_reg_r;
  raw_data_t<irs_u16>                   m_hold_regs_reg_w;
  raw_data_t<irs_u8>                    m_packet;
  m_condition                           m_command, RT;
  mode_t                                m_mode;

  irs_u8                                in_str[size_of_packet];
  loop_timer_t m_loop_timer;

  void make_packet(irs_uarc a_index, irs_u16 a_size);
  void get_bytes(raw_data_t<irs_u16> &regs,irs_u32 index,irs_u32 blenth,
        irs_u8 *buf,irs_u32 start);
  void write_bytes(raw_data_t<irs_u16> &regs,irs_u32 index,irs_u32 blenth,
        const irs_u8 *buf,irs_u32 start);
  void range(irs_u32 index, irs_u32 size, irs_u32 M1, irs_u32 M2,
         irs_u32 *num,irs_u32 *start);

};

} //namespace irs

#endif //irsmbusH
