// Клиент и сервер modbus написаный Олегом Коноваловым
// Дата: 13.10.2009
// Ранняя дата: 16.09.2009

#include <irsmbus.h>
#include <string.h>
#include <timer.h>
#include <irserror.h>
#include <irscpu.h>
#include <stdlib.h>
#include <irsstrdefs.h>

counter_t rtimeout;
counter_t wait;
#define cl_rtime TIME_TO_CNT(2,1)
//#define WAIT_LIENT_TIME 0.2
//#define bit_msize   65535
//#define reg_msize   65535
//#define word_msize  131070
//#define byte_msize  8191
//Error cods
#define ILLEGAL_FUNCTION        1
#define ILLEGAL_DATA_ADDRESS    2
#define ILLEGAL_DATA_VALUE      3
#define SLAVE_DEVICE_FAILURE    4
//enum commands {Rcoil = 1,R_DI,R_HR,R_IR,Wcoil,W_HR,Status,W_SevHR = 10};

//#define OUTDBG(out) out
//#define OUTDBG(out)
#define IRS_MBUS_MSG_NONE   0
#define IRS_MBUS_MSG_BASE   1
#define IRS_MBUS_MSG_DETAIL 2

#define IRS_MBUS_MSG_TYPE IRS_MBUS_MSG_NONE
//#define IRS_MBUS_MSG_TYPE IRS_MBUS_MSG_BASE
//#define IRS_MBUS_MSG_TYPE IRS_MBUS_MSG_DETAIL

#if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_BASE)
#define IRS_MBUS_DBG_MSG_DETAIL(msg)
#define IRS_MBUS_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG_SRC_ENG(msg) 
#elif (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
#define IRS_MBUS_DBG_MSG_DETAIL(msg) IRS_LIB_DBG_MSG_SRC_ENG(msg) 
#define IRS_MBUS_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG_SRC_ENG(msg) 
#else //IRS_MBUS_MSG_TYPE
#define IRS_MBUS_DBG_MSG_DETAIL(msg) 
#define IRS_MBUS_DBG_MSG_BASE(msg) 
#endif //IRS_MBUS_MSG_TYPE

ostream &hex_u8(ostream &s)
{
  s.setf(ios::hex, ios::basefield);
  s<<setw(2)<<setfill('0');
  return s;
}
ostream &hex_u16(ostream &s)
{
  s.setf(ios::showbase);
  s.setf(ios::hex , ios::basefield);
  s<<setw(4)<<setfill('0');
  return s;
}
ostream &dec_u8(ostream &s)
{
  s.setf(ios::dec, ios::basefield);
  s<<setw(3)<<setfill('0');
  return s;
}
template <class T>
irs_bool to_irs_bool(T a_value)
{
  if (a_value) {
    return irs_true;
  } else {
    return irs_false;
  }
}

inline irs_u8 mask_gen(int a_start, int a_size)
{
  int left_shift = 8 - a_size;
  int right_shift = left_shift - a_start;
  irs_u8 mask = IRS_U8_MAX;
  mask >>= left_shift;
  mask <<= right_shift;
  return mask;
}

void first_part(irs_u8 &internal_byte, irs_u8 external_byte,
  int index_bit, irs_u8 mask)
{
  external_byte <<= index_bit;
  internal_byte &= static_cast<irs_u8>(~mask);
  internal_byte |= external_byte;
}
void second_part(irs_u8 &internal_byte, irs_u8 external_byte,
  int index_bit, irs_u8 mask)
{
  external_byte >>= (8 - index_bit);
  internal_byte &= mask;
  internal_byte |= external_byte;
}

void bit_copy(const irs_u8 *ap_data_in, irs_u8 *ap_data_out,
  irs_u32 index_data_in, irs_u32 index_data_out, irs_u16 size_data_bit)
{
  int index_in = index_data_in%8;
  int index_out = index_data_out%8;
  int data_start_in = index_data_in/8;
  int data_start_out = index_data_out/8;
  int first_part_size = 0;
  int last_part_size = 0;
  if (index_in == index_out)
  {
    if((index_out != 0) && (index_in != 0))
      first_part_size = 8 - index_out;
    last_part_size = (size_data_bit - first_part_size)%8;
    int size_data_byte = size_data_bit/8;
    int middle_part_size = (size_data_bit - 
      (first_part_size + last_part_size))/8;
    int part_idx = 0;
    if(first_part_size != 0) {
      irs_u8 first_mask = mask_gen(0, first_part_size);
      irs_u8 first_data_in = ap_data_in[data_start_in];
      first_data_in &= first_mask;
      ap_data_out[data_start_out] &= static_cast<irs_u8>(~first_mask);
      ap_data_out[data_start_out] |= first_data_in;
      part_idx = 1;
    }
    if(last_part_size != 0) {
      irs_u8 last_mask = mask_gen(8 - last_part_size, last_part_size);
      irs_u8 last_data_in = ap_data_in[data_start_in + middle_part_size + 
        first_part_size];
      last_data_in &= last_mask;
      ap_data_out[data_start_out + middle_part_size + part_idx] &= 
        static_cast<irs_u8>(~last_mask);
      ap_data_out[data_start_out + middle_part_size + part_idx] |=
        last_data_in;
    }
    irs::memcpyex(ap_data_out + data_start_out + part_idx, ap_data_in + 
      data_start_in + part_idx, middle_part_size);
  }
  else
  {
    int offset_idx = 0;
    irs_u32 index_bit = 0;
    irs_u8 middle_mask = 0;
    irs_u32 offset = 0;
    IRS_LIB_ASSERT(index_in != index_out);
    if((index_out - index_in) > 0) {
      offset = index_data_out%8 - index_data_in%8;
      middle_mask = mask_gen(0, 8 - offset);
      index_bit = offset;
      offset_idx = 0;
      if((index_out != 0) && (index_in != 0))
        first_part_size = 8 - index_in;
      last_part_size = (size_data_bit + offset - first_part_size)%8;
    } else if((index_in - index_out) > 0) {
      offset = index_data_in%8 - index_data_out%8;
      middle_mask = mask_gen(0, offset);
      index_bit = 8 - offset;
      offset_idx = 1;
      if((index_out != 0) && (index_in != 0))
        first_part_size = 8 - index_out;
      last_part_size = (size_data_bit /*+ offset*/ - first_part_size)%8;
    } 
    int data_cnt = (size_data_bit - first_part_size)/8;
    int first_part_idx = 0;
    if(first_part_size > 0) {
      first_part_idx = 1;
      if(index_out < index_in) {
        irs_u8 out_mask = mask_gen(8 - offset, offset);
        irs_u8 first_in_mask = mask_gen(0, 8 - index_in);
        irs_u8 data_first_in = ap_data_in[data_start_in];
        irs_u8 data_out = 0;
        data_first_in &= first_in_mask;
        data_first_in >>= offset;
        irs_u8 data_second_in = ap_data_in[data_start_in + 1];
        irs_u8 second_in_mask = mask_gen(8 - offset, offset);
        data_second_in &= second_in_mask;
        data_second_in <<= (8 - offset);
        data_out |= data_first_in;
        data_out |= data_second_in;
        ap_data_out[data_start_out] &= out_mask;
        ap_data_out[data_start_out] |= data_out;
      }
      if(index_out > index_in) {
        irs_u8 data_first_in = ap_data_in[data_start_in];
        irs_u8 data_second_in = ap_data_in[data_start_in];
        irs_u8 first_in_mask = mask_gen(offset, 8 - index_out);
        data_first_in &= first_in_mask;
        data_first_in <<= offset;
        irs_u8 second_in_mask = mask_gen(0, offset);
        data_second_in &= second_in_mask;
        data_second_in >>= (8 - offset);
        irs_u8 first_out_mask = mask_gen(0, 8 - index_out);
        ap_data_out[data_start_out] &= static_cast<irs_u8>(~first_out_mask);
        ap_data_out[data_start_out] |= data_first_in;
        irs_u8 second_out_mask = mask_gen(8 - offset, offset);
        ap_data_out[data_start_out + 1] &=
          static_cast<irs_u8>(~second_out_mask);
        ap_data_out[data_start_out + 1] |= data_second_in;
      }
    }
    int offset_index = 0;
    if(last_part_size > 0) {
      int last_external_byte_index = data_cnt + data_start_in + first_part_idx;
      int last_internal_byte_index = data_cnt + data_start_out + first_part_idx; 
      irs_u8 last_mask = 0;
      if(static_cast<irs_u32>(last_part_size) <
        ((8 - 2*offset)*offset_idx + offset))
      {
        if((index_out - index_in) > 0)
        {
          last_mask = mask_gen(8 - last_part_size, last_part_size);
          irs_u8 mask_ext = mask_gen(1, last_part_size);
          irs_u8 data_ext = ap_data_in[last_external_byte_index];
          data_ext &= mask_ext;
          data_ext >>= 8 - (last_part_size + 1);
          ap_data_out[last_internal_byte_index + 1] |= data_ext;
          offset_index = 1;
        }
        if((index_in - index_out) > 0)
        {
          if((index_in + size_data_bit) < 8) {
            irs_u8 data_in = ap_data_in[last_external_byte_index];
            irs_u8 mask_ext = mask_gen(8 - (size_data_bit%8 + index_in),
              size_data_bit%8);
            irs_u8 mask_int = mask_gen(0, 8 - size_data_bit);
            data_in &= mask_ext;
            data_in >>= offset;
            ap_data_out[last_internal_byte_index] &= mask_int;
            ap_data_out[last_internal_byte_index] |= data_in;
          }
          else {
            last_mask = mask_gen(8 - offset, offset);
            first_part(ap_data_out[last_internal_byte_index], 
              ap_data_in[last_external_byte_index], index_bit, last_mask);
          }
        }
      }
      else
      {     
        second_part(ap_data_out[last_internal_byte_index], 
          ap_data_in[last_external_byte_index - 1 + offset_idx],
          index_bit, middle_mask);
        if(static_cast<irs_u32>(last_part_size) >
          ((8 - 2*offset)*offset_idx + offset))
        {
          if((index_out - index_in) > 0)
          {
            last_mask = mask_gen(8 - last_part_size, 
              last_part_size - offset);  
            first_part(ap_data_out[last_internal_byte_index], 
              ap_data_in[last_external_byte_index], index_bit, last_mask);
          }
          if((index_in - index_out) > 0)
          {
            irs_u8 mask_ext = mask_gen(8 - (last_part_size - (8 - offset)),
              last_part_size - (8 - offset));
            irs_u8 mask_int = mask_gen(8 - last_part_size,
              last_part_size - (8 - offset));
            irs_u8 data_ext = ap_data_in[last_external_byte_index+1];
            data_ext &= mask_ext;
            data_ext <<= 8 - offset;
            ap_data_out[last_internal_byte_index] &=
              static_cast<irs_u8>(~mask_int);
            ap_data_out[last_internal_byte_index] |= data_ext;
          }
        }  
      }
    }
    for(int data_idx = 0; data_idx < data_cnt + offset_index;
      data_idx++)
    {
      int external_idx = data_idx + data_start_in + first_part_idx;
      int internal_idx = data_idx + data_start_out + first_part_idx;
      first_part(ap_data_out[internal_idx], 
        ap_data_in[external_idx + offset_idx], index_bit, middle_mask);
    }
    for(int data_idx = 0; data_idx < data_cnt; data_idx++)
    {
      int external_idx = data_idx + data_start_in + first_part_idx*offset_idx;
      int internal_idx = data_idx + data_start_out + first_part_idx*offset_idx;
      second_part(ap_data_out[internal_idx + 1 - offset_idx], 
        ap_data_in[external_idx], index_bit, middle_mask);
    }
  }
}

irs::char_t digit_to_char(int digit)
{
  if (digit <= 9) {
    return static_cast<irs::char_t>(irst('0') + digit);
  } else {
    return static_cast<irs::char_t>(irst('A') + digit - 10);
  }
}

irs::string_t int_to_str(int number, int radix, int cnt)
{
  string number_str(cnt, irst(' '));
  for(int digit_idx = cnt - 1; digit_idx >= 0; digit_idx--)
  {
    int digit = number%radix;
    //number_str[cnt - digit_idx - 1] = digit_to_char(digit);
    number_str[digit_idx] = digit_to_char(digit);
    number /= radix; 
  }
  return number_str;
}

void irs::test_bit_copy(ostream& strm, irs_u16 size_data_in,
  irs_u16 size_data_out, irs_u32 index_data_in, irs_u32 index_data_out,
  irs_u16 size_data)
{
  irs::raw_data_t<irs_u8> ap_data_in(size_data_in);
  irs::raw_data_t<irs_u8> ap_data_out(size_data_out);
  rand();
  for(int idx_in = 0; idx_in < static_cast<int>(ap_data_in.size()); idx_in ++)
  {
    int rand_val = rand();
    ap_data_in[idx_in] = static_cast<irs_i64>(rand_val*(IRS_U8_MAX/RAND_MAX));
  }
  for(int idx_out = 0; idx_out < static_cast<int>(ap_data_out.size());
    idx_out ++)
  {
    int rand_val = rand();
    ap_data_out[idx_out] = static_cast
      <irs_i64>(rand_val*(IRS_U8_MAX/RAND_MAX));
  }
  strm << "IN before operation" << endl;
  for(int idx_in = 0; idx_in < static_cast<int>(ap_data_in.size()); idx_in ++)
  {
    strm << int_to_str(ap_data_in[idx_in], 2, 8) << " ";
  }
  strm << endl;
  strm << "OUT before operation" << endl;
  for(int idx_out = 0; idx_out < static_cast<int>(ap_data_out.size());
    idx_out ++)
  {
    strm << int_to_str(ap_data_out[idx_out], 2, 8) << " ";
  }
  strm << endl;
  bit_copy(ap_data_in.data(), ap_data_out.data(),
    index_data_in, index_data_out, size_data);
  strm << "OUT after operation" << endl;
  for(int idx_out = 0; idx_out < static_cast<int>(ap_data_out.size());
    idx_out ++)
  {
    strm << int_to_str(ap_data_out[idx_out], 2, 8) << " ";
  }
  strm << endl;
}

void convert(irs_u8 *ap_mess,irs_u8 a_start,irs_u8 a_length)
{
  if(irs::cpu_traits_t::endian() == irs::little_endian)
  {
    for(irs_u8 i = a_start; i < (a_length+a_start);i+=irs_u8(2))
    {
      irs_u8 var = ap_mess[i+1];
      ap_mess[i+1] = ap_mess[i];
      ap_mess[i] = var;
    }
  }
}

irs::irsmb_se::irsmb_se(
  channel_type channel,
  irs_u16 local_port,
  irs_u16 a_discr_inputs_size_byte,
  irs_u16 a_coils_size_byte,
  irs_u16 a_hold_regs_reg,
  irs_u16 a_input_regs_reg
):
  m_discr_inputs_bit(int(a_discr_inputs_size_byte)),
  m_coils_bit(int(a_coils_size_byte)),
  m_input_regs_reg(int(a_input_regs_reg)),
  m_hold_regs_reg(int(a_hold_regs_reg)),
  coils_bit(irs_u16(a_coils_size_byte*8)),
  di_bit(irs_u16(a_discr_inputs_size_byte*8)),
  hr_reg(a_hold_regs_reg),
  ir_reg(a_input_regs_reg),
  di_end_byte(a_discr_inputs_size_byte),
  coils_end_byte(a_discr_inputs_size_byte + a_coils_size_byte),
  hr_end_byte(a_discr_inputs_size_byte + a_coils_size_byte 
    + a_hold_regs_reg*2),
  ir_end_byte(a_discr_inputs_size_byte + a_coils_size_byte
    + a_hold_regs_reg*2 + a_input_regs_reg*2),
  m_size_byte(a_coils_size_byte + a_discr_inputs_size_byte + hr_reg + ir_reg),
  mp_buf(IRS_NULL),
  m_staddr(0),
  m_nregs(0),
  m_rcoil(irs_false),
  mode(read_mode),
  //mp_handle(IRS_NULL),
  di_size_byte(0),
  di_start_byte(0),
  coils_size_byte(0),
  coils_start_byte(0),
  hr_size_byte(0),
  hr_start_byte(0),
  ir_size_byte(0),
  ir_start_byte(0),
  m_size_byte_end(0)
{
  memsetex(mess, IRS_ARRAYSIZE(mess));
  memsetex(in_str, IRS_ARRAYSIZE(in_str));
  /*const mxip_t zero_ip = {{0,0,0,0}};
  m_mbchdata.mxifa_linux_dest.ip      = zero_ip;
  m_mbchdata.mxifa_linux_dest.port    = 0;
  mxifa_init();
  #ifdef NOP
  mp_handle = mxifa_open(channel,irs_false);
  mxifa_linux_tcp_ip_cl_cfg cfg;
  mxifa_get_config(mp_handle, (void *)&cfg);
  cfg.local_port = local_port;
  mxifa_set_config(mp_handle, (void *)&cfg);
  #endif //NOP
  mxifa_linux_tcp_ip_cfg cfg;
  cfg.local_port = local_port;
  mp_handle = mxifa_open_ex(channel,(void *)&cfg, irs_false);*/
}

irs_uarc irs::irsmb_se::size()
{
  return m_size_byte;
}

irs_bool irs::irsmb_se::connected()
{
  return irs_true;
}

void irs::irsmb_se::write_bytes(raw_data_t<irs_u16> &data_u16,irs_u32 index,
  irs_u32 size,const irs_u8 *buf,irs_u32 start)
{
  IRS_MBUS_DBG_MSG_DETAIL("write_bytes ..................");
  irs_u8* data_u8 = reinterpret_cast<irs_u8*>(data_u16.data());
  irs::memcpyex(data_u8 + index, buf + start, size);
  #ifdef NOP//IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL
  for(irs_u16 i = (index/2); i<(size/2+index/2);i++)
  {
    IRS_MBUS_DBG_MSG_DETAIL(dec<<"Vector["<<(int)i<<"] = "<< hex<<showbase<<
      (int)vec[i]);
  }
  IRS_MBUS_DBG_MSG_DETAIL("");
  for(irs_u16 i = (start); i<(size+start);i++)
  {
    IRS_MBUS_DBG_MSG_DETAIL(dec<<"Buf["<<(int)i<<"] = "<<hex<<showbase <<
      (int)buf[i]);
  }
  IRS_MBUS_DBG_MSG_DETAIL("");
  #endif
}

void irs::irsmb_se::get_bytes(raw_data_t<irs_u16> &data_u16,irs_u32 index,
  irs_u32 size,irs_u8 *buf,irs_u32 start)
{
  IRS_MBUS_DBG_MSG_DETAIL("get_bytes.............................");
  irs_u8* data_u8 = reinterpret_cast<irs_u8*>(data_u16.data());
  irs::memcpyex(buf + start, data_u8 + index, size);
}

void irs::irsmb_se::range(irs_u32 index, irs_u32 size, irs_u32 M1, irs_u32 M2,
irs_u32 *num,irs_u32 *start)
{
  irs_u32 var = index + size;
  if((index>=M1)&&(index<M2)&&(var>M1)&&(var <= M2))
  {
    *num = size;
    *start = index - M1;
  }
  else if((index>M1)&&(index<M2)&&(var>M2))
  {
    *num = M2 - index;
    *start = index - M1;
  }
  else if((index<M1)&&(var>M1)&&(var < M2))
  {
    *num = var - M1;
    *start = 0;
  }
  else if((index<=M1)&&(var >= M2))
  {
    *num = M2 - M1;
    *start = 0;
  }
}
//Чтение из массива данных в буфер размером size с позиции index
void irs::irsmb_se::read(irs_u8 *ap_buf, irs_uarc index, irs_uarc a_size)
{
  IRS_MBUS_DBG_MSG_DETAIL("read..................");
  irs_u32 di_size_byte = 0, di_start_byte = 0;
  irs_u32 coils_size_byte = 0, coils_start_byte = 0;
  irs_u32 hr_size_byte = 0, hr_start_byte = 0;
  irs_u32 ir_size_byte = 0, ir_start_byte = 0;
  range(index,a_size,di_start_byte,di_end_byte,&di_size_byte,&di_start_byte);
  range(index,a_size,di_end_byte,coils_end_byte,&coils_size_byte,
    &coils_start_byte);
  range(index,a_size,coils_end_byte,hr_end_byte,&hr_size_byte,&hr_start_byte);
  range(index,a_size,hr_end_byte,ir_end_byte,&ir_size_byte,&ir_start_byte);
  irs_u32 i = 0;
  if((di_size_byte != 0))
  {
    IRS_MBUS_DBG_MSG_DETAIL("DI size: " << di_size_byte <<
      " di_start_byte: " << di_start_byte);
    packet_data_8_t &m_packet_di = *(packet_data_8_t *)in_str;
    m_packet_di.byte_count = (irs_u8)di_size_byte;
    bit_copy(in_str, m_discr_inputs_bit.data(),di_start_byte*8,
      0, sizeof(di_size_byte));
    for(;i<=di_size_byte;i++)
      ap_buf[i] = m_packet_di.value[i];
  }
  if((coils_size_byte != 0)||(coils_start_byte != 0))
  {
    IRS_MBUS_DBG_MSG_DETAIL("coils_size_byte: " << coils_size_byte <<
      " coils_start_byte: " << coils_start_byte);
    packet_data_8_t &m_packet_coils = *(packet_data_8_t *)in_str;
    m_packet_coils.byte_count = (irs_u8)coils_size_byte;
    bit_copy(in_str, m_coils_bit.data(),coils_start_byte*8,
      0, sizeof(coils_size_byte));
    for(i=(di_size_byte + 1);i<=(di_size_byte + coils_size_byte);i++)
      ap_buf[i] = m_packet_coils.value[i-di_size_byte];
  }
  if((hr_size_byte != 0)||(hr_start_byte != 0))
  {
    IRS_MBUS_DBG_MSG_DETAIL("HR size: "<<hr_size_byte<<" hr_start_byte: "<<
      hr_start_byte);
    get_bytes(m_hold_regs_reg,hr_start_byte,hr_size_byte,ap_buf,i);
    IRS_LIB_ASSERT((IRS_ARRAYSIZE(ap_buf)-i) >= hr_size_byte);
  }
  if((ir_size_byte != 0)||(ir_start_byte != 0))
  {
    IRS_MBUS_DBG_MSG_DETAIL("ir_size_byte: " << ir_size_byte <<
      " ir_start_byte: " << ir_start_byte);
    get_bytes(m_input_regs_reg, ir_start_byte, ir_size_byte, ap_buf,
      i + hr_size_byte);
    IRS_LIB_ASSERT((IRS_ARRAYSIZE(ap_buf)-(i + hr_size_byte)) >= ir_size_byte);
    IRS_MBUS_DBG_MSG_DETAIL("Regs");
  }
}

void irs::irsmb_se::write(const irs_u8 *buf, irs_uarc index, irs_uarc size)
{
  IRS_MBUS_DBG_MSG_DETAIL("write..................");
  irs_u32 di_size_byte = 0, di_start_byte = 0;
  irs_u32 coils_size_byte = 0, coils_start_byte = 0;
  irs_u32 hr_size_byte = 0, hr_start_byte = 0;
  irs_u32 ir_size_byte = 0, ir_start_byte = 0;
  range(index,size,di_start_byte,di_end_byte,&di_size_byte,&di_start_byte);
  range(index,size,di_end_byte,coils_end_byte,&coils_size_byte,
    &coils_start_byte);
  range(index,size,coils_end_byte,hr_end_byte,&hr_size_byte,&hr_start_byte);
  range(index,size,hr_end_byte,ir_end_byte,&ir_size_byte,&ir_start_byte);
  if((di_size_byte != 0))
  {
    IRS_MBUS_DBG_MSG_DETAIL("di_size_byte: " << di_size_byte <<
      " di_start_byte: " << di_start_byte);
    bit_copy(buf,m_discr_inputs_bit.data(),0,di_start_byte*8,
      static_cast<irs_u16>((di_size_byte)*8));
  }
  if((coils_size_byte != 0)||(coils_start_byte != 0))
  {
    IRS_MBUS_DBG_MSG_DETAIL("coils_size_byte: " << coils_size_byte << 
      " coils_start_byte: " << coils_start_byte);
    bit_copy(buf+di_size_byte,m_coils_bit.data(), 0, coils_start_byte*8,
      (irs_u16)(coils_size_byte));
  }
  if((hr_size_byte != 0)||(hr_start_byte != 0))
  {
    IRS_MBUS_DBG_MSG_DETAIL("HR size: " << hr_size_byte <<
      " hr_start_byte: " << hr_start_byte);
    write_bytes(m_hold_regs_reg,hr_start_byte,hr_size_byte,buf,
      di_size_byte + coils_size_byte);
    IRS_LIB_ASSERT(m_hold_regs_reg.size() >= hr_size_byte);
 }
  if((ir_size_byte != 0)||(ir_start_byte != 0))
  {
    IRS_MBUS_DBG_MSG_DETAIL("IR size: " << ir_size_byte <<
      " ir_start_byte: " << ir_start_byte);
    write_bytes(m_input_regs_reg,ir_start_byte,ir_size_byte,buf,
      di_size_byte + coils_size_byte + hr_size_byte);
    IRS_LIB_ASSERT(m_input_regs_reg.size() >= ir_size_byte);
  }
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    if((coils_size_byte == 0)&&(di_size_byte == 0)&&
      (hr_size_byte == 0)&&(ir_size_byte == 0))
      cout << "\n***************Illegal index or bufer size"
        "******************\n\n";
  #endif

}

irs_bool irs::irsmb_se::bit(irs_uarc index, irs_uarc bit_index)
{
  IRS_MBUS_DBG_MSG_DETAIL("bit..................");
  irs_u32 di_size_byte = 0, di_start_byte = 0;
  irs_u32 coils_size_byte = 0, coils_start_byte = 0;
  irs_u32 hr_size_byte = 0, hr_start_byte = 0;
  irs_u32 ir_size_byte = 0, ir_start_byte = 0;
  range(index,1,di_start_byte,di_end_byte,&di_size_byte,&di_start_byte);
  range(index,1,di_end_byte,coils_end_byte,&coils_size_byte,&coils_start_byte);
  range(index,1,coils_end_byte,hr_end_byte,&hr_size_byte,&hr_start_byte);
  range(index,1,hr_end_byte,ir_end_byte,&ir_size_byte,&ir_start_byte);
  if (bit_index < 8) {
    if ((di_size_byte != 0) || (di_start_byte != 0))
    {
      return to_irs_bool(m_discr_inputs_bit[di_start_byte*8 + bit_index]);
    }
    if ((coils_size_byte != 0) || (coils_start_byte != 0))
    {
      return to_irs_bool(m_coils_bit[coils_start_byte*8 + bit_index]);
    }
    if ((hr_size_byte != 0) || (hr_start_byte != 0))
    {
      return to_irs_bool(m_hold_regs_reg[ir_start_byte/2]&(1 << bit_index));
    }
    if ((ir_size_byte != 0) || (ir_start_byte != 0))
    {
      return to_irs_bool(m_input_regs_reg[hr_start_byte/2]&(1 << bit_index));
    }
  }
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    if((coils_size_byte == 0)&&(di_size_byte == 0)&&
      (hr_size_byte == 0)&&(ir_size_byte == 0))
      cout << "\n***************Illegal index or bufer size******************"
        "\n\n";
  #endif
  return irs_false;
}

void irs::irsmb_se::set_bit(irs_uarc index, irs_uarc bit_index)
{
  IRS_MBUS_DBG_MSG_DETAIL("set_bit..................");
  irs_uarc byte = 0;
  irs_u32 di_size_byte = 0, di_start_byte = 0;
  irs_u32 coils_size_byte = 0, coils_start_byte = 0;
  irs_u32 hr_size_byte = 0, hr_start_byte = 0;
  irs_u32 ir_size_byte = 0, ir_start_byte = 0;
  range(index,1,0,di_end_byte,&di_size_byte,&di_start_byte);
  range(index,1,di_end_byte,coils_end_byte,&coils_size_byte,&coils_start_byte);
  range(index,1,coils_end_byte,hr_end_byte,&hr_size_byte,&hr_start_byte);
  range(index,1,hr_end_byte,ir_end_byte,&ir_size_byte,&ir_start_byte);
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    if((coils_size_byte == 0)&&(di_size_byte == 0)&&
      (hr_size_byte == 0)&&(ir_size_byte == 0))
      cout << "\n************Illegal index or bufer size***************\n\n";
  #endif
  if (bit_index < 8) {
    if((di_size_byte != 0)||(di_start_byte != 0))
    {
      m_discr_inputs_bit[di_start_byte*8+bit_index] = irs_true;
    }
    if((coils_size_byte != 0)||(coils_start_byte != 0))
    {
      m_coils_bit[coils_start_byte*8+bit_index] = irs_true;
    }
    if((hr_size_byte != 0)||(hr_start_byte != 0))
    {
      byte = m_hold_regs_reg[hr_start_byte/2u];
      byte |= (1<<bit_index);
      m_hold_regs_reg[hr_start_byte/2u] = IRS_LOBYTE(byte);
    }
    if((ir_size_byte != 0)||(ir_start_byte != 0))
    {
      byte = m_input_regs_reg[ir_start_byte/2u];
      byte |= (1<<bit_index);
      m_input_regs_reg[ir_start_byte/2u] = IRS_LOBYTE(byte);
    }
  }
}

void irs::irsmb_se::clear_bit(irs_uarc index, irs_uarc bit_index)
{
  IRS_MBUS_DBG_MSG_DETAIL("clear_bit..................");
  irs_u16 byte = 0;
  irs_u32 di_size_byte = 0, di_start_byte = 0;
  irs_u32 coils_size_byte = 0, coils_start_byte = 0;
  irs_u32 hr_size_byte = 0, hr_start_byte = 0;
  irs_u32 ir_size_byte = 0, ir_start_byte = 0;
  range(index,1,0,di_end_byte,&di_size_byte,&di_start_byte);
  range(index,1,di_end_byte,coils_end_byte,&coils_size_byte,&coils_start_byte);
  range(index,1,coils_end_byte,hr_end_byte,&hr_size_byte,&hr_start_byte);
  range(index,1,hr_end_byte,ir_end_byte,&ir_size_byte,&ir_start_byte);
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    if((coils_size_byte == 0)&&(di_size_byte == 0)&&
      (hr_size_byte == 0)&&(ir_size_byte == 0))
      IRS_MBUS_DBG_MSG_DETAIL("\n****************Illegal index or bufer size"
        "***************\n\n");
  #endif
  if (bit_index < 8) {
    if((di_size_byte != 0)||(di_start_byte != 0))
    {
      m_discr_inputs_bit[di_start_byte*8+bit_index] = irs_false;
    }
    if((coils_size_byte != 0)||(coils_start_byte != 0))
    {
      m_coils_bit[coils_start_byte*8+bit_index] = irs_false;
    }
    if((hr_size_byte != 0)||(hr_start_byte != 0))
    {
      byte = m_hold_regs_reg[hr_start_byte/2];
      byte&=irs_u16(~(1<<bit_index));
      m_hold_regs_reg[hr_start_byte/2] = byte;
    }
    if((ir_size_byte != 0)||(ir_start_byte != 0))
    {
      byte = m_input_regs_reg[ir_start_byte/2];
      byte&=irs_u16(~(1<<bit_index));
      m_input_regs_reg[ir_start_byte/2] = byte;
    }
  }
}

void irs::irsmb_se::Error_response(irs_u8 error_code)
{
  IRS_MBUS_DBG_MSG_DETAIL("Error_response..................");
  complex_modbus_header_t &cmb_head = *(complex_modbus_header_t *)mess;
  pack_d8_t &m_ercode = *(pack_d8_t *)(mess+size_of_header);
  cmb_head.function_code += irs_u8(0x80);
  m_ercode.value[0] = error_code;
}

void irs::irsmb_se::status()
{
  IRS_MBUS_DBG_MSG_DETAIL("status()..................");
  IRS_MBUS_DBG_MSG_DETAIL("*******************Discret Inputs"
    "*********************");
  for(irs_u16 i = 0; i< di_bit;i++)
  {
      IRS_MBUS_DBG_MSG_DETAIL(m_discr_inputs_bit[i]);
  }
  IRS_MBUS_DBG_MSG_DETAIL("*************************"
    "****************************");
  IRS_MBUS_DBG_MSG_DETAIL("******************COILS"
    "********************************");
  for(irs_u16 i = 0; i< coils_bit;i++)
  {
      IRS_MBUS_DBG_MSG_DETAIL(m_coils_bit[i]);
  }
  IRS_MBUS_DBG_MSG_DETAIL("**************************"
    "***************************");
  IRS_MBUS_DBG_MSG_DETAIL("**************Holding registers"
    "***********************");
  for(irs_u16 i = 0; i< irs_u16(hr_reg/2);i++)
  {
      IRS_MBUS_DBG_MSG_DETAIL((int)m_hold_regs_reg[i]<<' ');
  }
  IRS_MBUS_DBG_MSG_DETAIL("****************************"
    "*************************");
  IRS_MBUS_DBG_MSG_DETAIL("**************Input registers"
    "*************************");
  for(irs_u16 i = 0; i< irs_u16(ir_reg/2);i++)
  {
      IRS_MBUS_DBG_MSG_DETAIL((int)m_input_regs_reg[i]<<' ');
  }
  IRS_MBUS_DBG_MSG_DETAIL("*************************************");
}

void irs::irsmb_se::tick()
{
  switch(mode)
  {
    case read_mode:
    {
      if(mxifa_read_end(mp_handle,irs_false))
      {
        complex_modbus_header_t& header = 
          reinterpret_cast<complex_modbus_header_t&>(*mess);
        if(header.length != 0)
        {  
          mode = read_pack;
          IRS_MBUS_DBG_MSG_BASE("read_pack");
        }  
        else
        {
          mxifa_read_begin(mp_handle,&m_mbchdata,mess,size_of_MBAP);         
          IRS_MBUS_DBG_MSG_BASE("read_pack");
          mode = read_pack;
        }
      }
    }
    break;
    case read_pack:
    {
      if(mxifa_read_end(mp_handle,irs_false))
      {
        convert(mess,0,size_of_MBAP);
        complex_modbus_header_t& cmb_head = 
          reinterpret_cast<complex_modbus_header_t&>(*mess);
        #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
        for(irs_u16 t=0; t<sizeof(mess);t++)
          cout << "mess:" << (int)mess[t] << ' ';
        cout << "\n********************************\n";
        cout << "\n**************************** cmb_head.length= " <<
          cmb_head.length << endl;
        cout << "**************************** size_of_MBAP= " <<
          size_of_MBAP << endl;
        cout << " transaction_id=  " <<cmb_head.transaction_id << endl;
        cout << " proto_id=        " <<cmb_head.proto_id << endl;
        cout << " length_lo=       " <<int(IRS_LOBYTE(cmb_head.length)) << endl;
        cout << " length_hi=       " <<int(IRS_HIBYTE(cmb_head.length)) << endl;
        cout << " unit_identifier= " <<cmb_head.unit_identifier << endl;
        cout << " function_code=   " <<cmb_head.function_code << endl;
        #endif
        IRS_LIB_ASSERT(cmb_head.length < 255);
        if(cmb_head.length == 0)
        {
          IRS_MBUS_DBG_MSG_BASE("read_mode");
          mode = read_mode;
        }
        else
        {
          IRS_MBUS_DBG_MSG_BASE("MBAP header");
          for(irs_u16 t = 0; t<size_of_MBAP;t++)
            IRS_MBUS_DBG_MSG_DETAIL(hex_u8<< ((int)(mess[t])) << ' ');
          IRS_MBUS_DBG_MSG_DETAIL("");
          pack_d8_t &raw_bytes = *(pack_d8_t *)(mess+size_of_MBAP);
          mxifa_read_begin(mp_handle,&m_mbchdata,raw_bytes.value,
            cmb_head.length);
          IRS_MBUS_DBG_MSG_BASE("read_end");
          mode = read_end;
        }
      }
    }
    break;
    case read_end:
    {
      if(mxifa_read_end(mp_handle,irs_false))
      {
       for(irs_u16 t = 0; t<12;t++)
          IRS_MBUS_DBG_MSG_DETAIL(hex_u8<< ((int)(mess[t])) << ' ');
        IRS_MBUS_DBG_MSG_DETAIL("");
        convert(mess,8,size_of_read_header);
        IRS_MBUS_DBG_MSG_DETAIL("\nRecieved Message after convert: ");
        for(irs_u16 t = 0; t<12;t++)
          IRS_MBUS_DBG_MSG_DETAIL(hex_u8<< ((int)(mess[t])) << ' ');
        IRS_MBUS_DBG_MSG_DETAIL("");
        complex_modbus_header_t &cmb_head = *(complex_modbus_header_t *)mess;
        request_read_t &sec_head = *(request_read_t *)(mess+size_of_header);        
        IRS_LIB_ASSERT((cmb_head.length + size_of_MBAP) < 260);
        for(irs_u16 t=0; t<(cmb_head.length+size_of_MBAP);t++)
          IRS_MBUS_DBG_MSG_DETAIL(hex_u8<<(int)mess[t]<< ' ');
        IRS_MBUS_DBG_MSG_DETAIL("\n ----------- MODBUS's Header"
          " -----------");
        IRS_MBUS_DBG_MSG_DETAIL("Recieved header - transaction_id:    " <<
          hex_u16<<cmb_head.transaction_id);
        IRS_MBUS_DBG_MSG_DETAIL("Recieved header - proto_id:          " <<
          hex_u16<<cmb_head.proto_id);
        IRS_MBUS_DBG_MSG_DETAIL("Recieved header - length:            " <<
          hex_u16<<cmb_head.length);
        IRS_MBUS_DBG_MSG_DETAIL("Recieved header - unit_identifier:   " <<
          hex_u8<<(int)cmb_head.unit_identifier);
        IRS_MBUS_DBG_MSG_DETAIL("Recieved header - function_code:     " <<
          hex_u8<<(int)cmb_head.function_code);
        IRS_MBUS_DBG_MSG_DETAIL("Recieved header - starting_address:  " <<
          sec_head.starting_address);
        IRS_MBUS_DBG_MSG_DETAIL("Recieved header - num_of_regs:       " <<
          sec_head.num_of_regs);
        IRS_MBUS_DBG_MSG_DETAIL("M_hold regs "<< (int)m_hold_regs_reg[0]);
        m_nregs = sec_head.num_of_regs;
        m_staddr = sec_head.starting_address;
        IRS_MBUS_DBG_MSG_DETAIL("Recieved header - starting_address:  " 
          << hex_u16<<m_staddr);
        IRS_MBUS_DBG_MSG_DETAIL("Recieved header - num_of_regs:       " 
          << hex_u16<<m_nregs);
        switch(cmb_head.function_code)
        {
          case Rcoil:
          {
            IRS_MBUS_DBG_MSG_BASE("\n Rcoil");
            range(m_staddr,m_nregs,0,coils_bit,&coils_size_byte,
              &coils_start_byte);
            IRS_MBUS_DBG_MSG_DETAIL("coils_start_byte " << coils_start_byte);
            IRS_MBUS_DBG_MSG_DETAIL("coils_size_byte " << coils_size_byte);
            if((coils_size_byte != 0)||(coils_start_byte != 0))
            {
              //m_nregs = sec_head.num_of_regs;
              m_rcoil = irs_true;
              packet_data_8_t &m_packet_coils =
                *(packet_data_8_t*)(mess+size_of_header);
              if((sec_head.num_of_regs%8)==0)
                m_packet_coils.byte_count = irs_u8(sec_head.num_of_regs/8);
              else
                m_packet_coils.byte_count =
                  irs_u8(sec_head.num_of_regs/8 + 1);
              IRS_MBUS_DBG_MSG_DETAIL("Recieved header - starting_address:  " <<
                m_staddr);
              bit_copy(mess+size_of_header, m_coils_bit.data(),m_staddr, 0,
                sizeof(coils_size_byte));
              cmb_head.length =
                irs_u16(size_of_res + m_packet_coils.byte_count);
              m_size_byte_end = cmb_head.length;
              convert(mess,0,size_of_MBAP);
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case R_DI:
          {
            IRS_MBUS_DBG_MSG_BASE("\n R_DI");
            range(m_staddr,m_nregs,0,di_bit,&di_size_byte,&di_start_byte);
            IRS_MBUS_DBG_MSG_DETAIL("\ndi_start_byte " << di_start_byte);
            IRS_MBUS_DBG_MSG_DETAIL("\ndi_size_byte " << di_size_byte);
            if((di_size_byte != 0)||(di_start_byte != 0))
            {
              m_rcoil = irs_false;
              packet_data_8_t &m_packet_di =
                *(packet_data_8_t*)(mess+size_of_header);
              if((sec_head.num_of_regs%8)==0)
                m_packet_di.byte_count = irs_u8(sec_head.num_of_regs/8);
              else
                m_packet_di.byte_count = irs_u8(sec_head.num_of_regs/8 + 1);
              bit_copy(mess+size_of_header, m_discr_inputs_bit.data(),
                m_staddr, 0, sizeof(di_size_byte));
              cmb_head.length = irs_u16(size_of_res + m_packet_di.byte_count);
              m_size_byte_end = cmb_head.length;
              convert(mess,0,size_of_MBAP);
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case R_HR:
          {
            IRS_MBUS_DBG_MSG_BASE("\n R_HR");
            range(m_staddr,m_nregs,0,hr_reg/2,&hr_size_byte,&hr_start_byte);
            IRS_MBUS_DBG_MSG_DETAIL("\nhr_start_byte " << hr_start_byte);
            IRS_MBUS_DBG_MSG_DETAIL("\nhr_size_byte " << hr_size_byte);
            if((hr_size_byte != 0)||(hr_start_byte != 0))
            {
              request_wr_coils_t &dop_head =
                *(request_wr_coils_t*)(mess+size_of_header);
              dop_head.byte_count = irs_u8(hr_size_byte*2);
              get_bytes(m_hold_regs_reg,hr_start_byte,hr_size_byte*2,
                dop_head.value,0);
              IRS_LIB_ASSERT(static_cast<irs_u32>(dop_head.byte_count) >=
                hr_size_byte);
              IRS_MBUS_DBG_MSG_DETAIL("\nMemory contein: ");
              for(irs_u8 i = 0; i< irs_u8(hr_size_byte*2);i++)
              {
                IRS_MBUS_DBG_MSG_DETAIL(hex_u8<< (int)dop_head.value[i]<<'|');
              }
              IRS_MBUS_DBG_MSG_DETAIL("");
              //OUTDBG(cout <<dec<<noshowbase<< "\n");
              IRS_MBUS_DBG_MSG_DETAIL("\nByte count: "<< hex_u8 <<
                (int)dop_head.byte_count);
              cmb_head.length = irs_u16(size_of_res + dop_head.byte_count);
              m_size_byte_end = cmb_head.length;
              convert(mess,size_of_header+1,dop_head.byte_count);
              convert(mess,0,size_of_MBAP);
              //convert(mess,8,size_of_read_header);
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case R_IR:
          {
            IRS_MBUS_DBG_MSG_BASE("\n R_IR");
            range(m_staddr,m_nregs,0,ir_reg/2,&ir_size_byte,&ir_start_byte);
            IRS_MBUS_DBG_MSG_DETAIL("\nir_start_byte " << ir_start_byte);
            IRS_MBUS_DBG_MSG_DETAIL("\nir_size_byte " << ir_size_byte);
            if((ir_size_byte != 0)||(ir_start_byte != 0))
            {
              request_wr_coils_t &dop_head =
                *(request_wr_coils_t*)(mess+size_of_header);
              dop_head.byte_count = irs_u8(ir_size_byte*2);
              IRS_MBUS_DBG_MSG_DETAIL("INPUT REGS" );
              get_bytes(m_input_regs_reg,ir_start_byte,ir_size_byte*2,
                dop_head.value,0);
              IRS_LIB_ASSERT(static_cast<irs_u32>(dop_head.byte_count) >=
                ir_size_byte);
              for(irs_u16 i = 0; i< irs_u16(ir_reg/2);i++)
              {
                IRS_MBUS_DBG_MSG_DETAIL(hex_u16<<(int)dop_head.value[i]<<' ');
              }
              cmb_head.length = irs_u16(size_of_res + dop_head.byte_count);
              m_size_byte_end = cmb_head.length;
              IRS_MBUS_DBG_MSG_DETAIL("before convert" );
              for(irs_u16 t=0; t<(size_of_header+1+dop_head.byte_count);t++)
              IRS_MBUS_DBG_MSG_DETAIL(hex_u8<<(int)mess[t]<< ' ');
              IRS_MBUS_DBG_MSG_DETAIL("");
              convert(mess,size_of_header+1,dop_head.byte_count);
              IRS_MBUS_DBG_MSG_DETAIL("after convert" );
              for(irs_u16 t=0; t<(size_of_header+1+dop_head.byte_count);t++)
              IRS_MBUS_DBG_MSG_DETAIL(hex_u8<<(int)mess[t]<< ' ');
              IRS_MBUS_DBG_MSG_DETAIL("");
              convert(mess,0,size_of_MBAP);
              //convert(mess,8,size_of_read_header);
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case Wcoil:
          {
            IRS_MBUS_DBG_MSG_BASE("\n Wcoil");
            if(IRS_HIBYTE(sec_head.num_of_regs) == 0xFF)
              m_coils_bit[sec_head.starting_address] = irs_true;
            else
              m_coils_bit[sec_head.starting_address] = irs_false;
            m_size_byte_end = size_of_MBAP;
            convert(mess,0,size_of_MBAP);
            convert(mess,8,size_of_read_header);
          }
          break;
          case WMcoils:
          {
            IRS_MBUS_DBG_MSG_BASE("\n WMcoils");
            packet_data_8_t &dop_head =
              *(packet_data_8_t*)(mess+size_of_header+size_of_read_header);
            for(irs_u16 t=0; t<(size_of_MBAP + cmb_head.length);t++)
              IRS_MBUS_DBG_MSG_DETAIL((int)mess[t]<< ' ');
            IRS_MBUS_DBG_MSG_DETAIL("");
            for(irs_u8 i = 0;i<dop_head.byte_count;i++)
              IRS_MBUS_DBG_MSG_DETAIL((int) dop_head.value[i]<< ' ');
            IRS_MBUS_DBG_MSG_DETAIL("");
            IRS_MBUS_DBG_MSG_DETAIL("Byte count: "<<(int)dop_head.byte_count);
            IRS_MBUS_DBG_MSG_DETAIL("Starting address: "
              << sec_head.starting_address);
            //irs_u8& dop_head_value = *(irs_u8*)dop_head.value; 
            irs_u16 dop_head_size = static_cast<irs_u16>(dop_head.byte_count*8); 
            bit_copy(dop_head.value,m_coils_bit.data(), 0, 
              sec_head.starting_address, dop_head_size);
            for(irs_u16 i = sec_head.starting_address;
              i<(sec_head.num_of_regs+sec_head.starting_address);i++)
              IRS_MBUS_DBG_MSG_DETAIL(m_coils_bit[i]);
            IRS_MBUS_DBG_MSG_DETAIL("END");
            cmb_head.length = size_of_res*2;
            m_size_byte_end = cmb_head.length;
            convert(mess,0,size_of_MBAP);
            convert(mess,8,size_of_read_header);
          }
          break;
          case WM_regs:
          {
            IRS_MBUS_DBG_MSG_BASE("\n WM_regs");
            for(irs_u16 t=0; t<size_of_packet;t++)
              IRS_MBUS_DBG_MSG_DETAIL((int)mess[t]<< ' ');
            request_wr_coils_t &dop_head =
              *(request_wr_coils_t*)(mess+size_of_header+size_of_read_header);
            convert(
              mess,
              irs_u8(size_of_header+size_of_read_header+1),
              irs_u8(sec_head.num_of_regs*2)
            );
            for(irs_u8 i = 0;i<sec_head.num_of_regs*2;i++)
              IRS_MBUS_DBG_MSG_DETAIL((int)dop_head.value[i]<< ' ');
            range(m_staddr*2, sec_head.num_of_regs*2, 0, hr_reg,
              &hr_size_byte, &hr_start_byte);
            write_bytes(m_hold_regs_reg,hr_start_byte,hr_size_byte,
              dop_head.value,0);
            IRS_LIB_ASSERT(m_hold_regs_reg.size() >= hr_size_byte);
            IRS_MBUS_DBG_MSG_DETAIL("---Starting address: "
              << sec_head.starting_address);
            for(irs_u16 i = sec_head.starting_address;
              i<(sec_head.num_of_regs+sec_head.starting_address);i++)
              IRS_MBUS_DBG_MSG_DETAIL(m_hold_regs_reg[i]<< ' ');
            cmb_head.length = size_of_res*2;
            m_size_byte_end = cmb_head.length;
            convert(mess,0,size_of_MBAP);
            convert(mess,8,size_of_read_header);
          }
          break;
          case W_HR:
          {
            IRS_MBUS_DBG_MSG_BASE("\n W_HR");
            wr_sreg_t &dop_head =
              *(wr_sreg_t*)(mess+size_of_header);
            hr_size_byte = 2;
            range(m_staddr*2,2,0,hr_reg,&hr_size_byte,&hr_start_byte);
            write_bytes(m_hold_regs_reg,hr_start_byte,hr_size_byte,
              dop_head.value,0);
            IRS_LIB_ASSERT(m_hold_regs_reg.size() >= hr_size_byte);
            m_size_byte_end = 6;
            convert(mess,0,size_of_MBAP);
            convert(mess,8,size_of_read_header);
          }
          break;
          case Status:
          {
            IRS_MBUS_DBG_MSG_BASE("\n Status");
            IRS_MBUS_DBG_MSG_DETAIL("\n*******************Discret Inputs*******"
              << di_bit<<"**************");
            for(irs_u16 i = 0; i< di_bit;i++)
            {
              IRS_MBUS_DBG_MSG_DETAIL(m_discr_inputs_bit[i]);
            }
            IRS_MBUS_DBG_MSG_DETAIL("\n*********************************"
              "********************");
            IRS_MBUS_DBG_MSG_DETAIL("\n******************COILS**************"<<
              coils_bit<<"******************");
            for(irs_u16 i = 0; i< coils_bit;i++)
            {
              IRS_MBUS_DBG_MSG_DETAIL(m_coils_bit[i]);
            }
            IRS_MBUS_DBG_MSG_DETAIL("\n****************************************"
              "*************");
            IRS_MBUS_DBG_MSG_DETAIL("\n**************Holding registers*********"
              "**************");
            for(irs_u16 i = 0; i< irs_u16(hr_reg/2);i++)
            {
              IRS_MBUS_DBG_MSG_DETAIL(hex_u16<<(int)m_hold_regs_reg[i]<<' ');
            }
            IRS_MBUS_DBG_MSG_DETAIL("\n**************************************"
              "***************\n");
            IRS_MBUS_DBG_MSG_DETAIL("\n**************Input registers***********"
              "**************");
            for(irs_u16 i = 0; i< irs_u16(ir_reg/2);i++)
            {
              IRS_MBUS_DBG_MSG_DETAIL(hex_u16<<(int)m_input_regs_reg[i]<<' ');
            }
            IRS_MBUS_DBG_MSG_DETAIL("\n*************************************");
            convert(mess,0,size_of_MBAP);
            convert(mess,8,size_of_read_header);
            m_size_byte_end = size_of_MBAP;
            for(irs_u16 t = 0; t<12;t++)
              IRS_MBUS_DBG_MSG_DETAIL(hex_u8<< ((int)(mess[t])) << ' ');
            //cin.get();
          }
          break;
          case Rsize:
          {
            IRS_MBUS_DBG_MSG_BASE("\n Rsize");
          }
          break;
          case Rtab:
          {
            IRS_MBUS_DBG_MSG_BASE("\n Rtab");
          }
          break;
          default:
          {
            Error_response(ILLEGAL_FUNCTION);
          }
          break;
        }
        IRS_MBUS_DBG_MSG_BASE("write_mode");
        mode = wait_mode;
      }
    }
    break;
    case write_mode:
    {
      if(mxifa_write_end(mp_handle,irs_false))
      {
        #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
        puts("Header of message: ");
        for(irs_u16 t = 0; t<(size_of_MBAP + size_of_res);t++)
          cout << hex_u8 << ((int)(mess[t])) << ' ';
        puts("\nTransmit message: ");
        for(irs_u16 t=0; t<irs_u16(size_of_MBAP + m_size_byte_end);t++)
          cout << hex_u8 << (int)mess[t] << ' ';
        cout << "\nSize of transmit message "<<
          (size_of_MBAP + m_size_byte_end) << "\n" << endl ;
        #endif  
        mxifa_write_begin(mp_handle,&m_mbchdata,mess,size_of_MBAP + 
          m_size_byte_end);
        IRS_MBUS_DBG_MSG_BASE("write_end");
        mode = write_end;
      }
    }
    break;
    case write_end:
    {
      if(mxifa_write_end(mp_handle,irs_false))
      {
        IRS_MBUS_DBG_MSG_BASE("read_mode");
        memset(mess,0,sizeof(mess));
        m_mbchdata.mxifa_linux_dest.ip      = zero_ip;
        m_mbchdata.mxifa_linux_dest.port    = 0;
        mode = read_mode;        
      }
    }
    break;
    case wait_mode:
    {
      if(test_to_cnt(wait))
        { 
          IRS_MBUS_DBG_MSG_BASE("write_mode");
          mode = write_mode;
        }
    }
  }
  mxifa_tick();
}

irs::irsmb_cl::irsmb_cl(
  channel_type channel,
  mxip_t dest_ip,
  irs_u16 dest_port,
  irs_u16 a_discr_inputs_size_byte,
  irs_u16 a_coils_size_byte,
  irs_u16 a_hold_regs_reg,
  irs_u16 a_input_regs_reg
):
  m_rforwr(a_coils_size_byte*8 + a_hold_regs_reg*2),
  m_discr_inputs_bit_r(a_discr_inputs_size_byte),
  m_coils_bit_r(a_coils_size_byte),
  m_coils_bit_w(a_coils_size_byte),
  m_input_regs_reg_r(a_input_regs_reg),
  m_hold_regs_reg_r(a_hold_regs_reg),
  m_hold_regs_reg_w(a_hold_regs_reg),
  m_loop_timer(make_cnt_ms(200)),
  di_size_byte(0),
  di_start_byte(0),
  coils_size_byte(0),
  coils_start_byte(0),
  hr_size_byte(0),
  hr_start_byte(0),
  ir_size_byte(0),
  ir_start_byte(0),
  m_ibank(0),
  m_global_index(0),
  m_size_byte_end(0),
  m_read_table(0),
  m_nregs(0),
  m_start_block(0),
  search_index(0),
  m_bytes(0),
  m_stpos(0),
  del_time(TIME_TO_CNT(1, 1)),
  m_command(R_DI),
  coils_bit(irs_u16(a_coils_size_byte*8)),
  di_bit(irs_u16(a_discr_inputs_size_byte*8)),
  hr_reg(a_hold_regs_reg*2),
  ir_reg(a_input_regs_reg*2),
  di_end_byte(a_discr_inputs_size_byte),
  coils_end_byte(a_discr_inputs_size_byte + a_coils_size_byte),
  hr_end_byte(a_discr_inputs_size_byte + a_coils_size_byte + 
    a_hold_regs_reg*2),
  ir_end_byte(a_discr_inputs_size_byte + a_coils_size_byte +
    a_hold_regs_reg*2 + a_input_regs_reg*2),
  m_channel(channel),
  m_dport(dest_port),
  m_dip(dest_ip),
  mp_buf(IRS_NULL),
  m_part_send(irs_false),
  m_mode(get_table),
  m_nothing(irs_true),
  RT(gt_start),
  m_first_read(irs_true)
{
  memsetex(m_spacket,IRS_ARRAYSIZE(m_spacket));
  memsetex(m_rpacket,IRS_ARRAYSIZE(m_rpacket));
  memsetex(in_str,IRS_ARRAYSIZE(in_str));
  init_to_cnt();
  mxifa_init();
  m_mbchdata.mxifa_linux_dest.ip = dest_ip;
  m_mbchdata.mxifa_linux_dest.port = dest_port;
  mxifa_linux_tcp_ip_cl_cfg cfg;
  cfg.dest_port = m_dport;
  cfg.dest_ip = m_dip;
  cfg.local_port = 0;
  mp_handle = mxifa_open_ex(channel,(void *)&cfg, irs_false);
  #ifdef NOP
  mp_handle = mxifa_open(channel,irs_false);
  mxifa_linux_tcp_ip_cl_cfg cfg;
  mxifa_get_config(mp_handle, (void *)&cfg);
  cfg.dest_port = m_dport;
  cfg.dest_ip = m_dip;
  mxifa_set_config(mp_handle, (void *)&cfg);
  #endif //NOP
}

irs::irsmb_cl::~irsmb_cl()
{
  mxifa_close(mp_handle);
  mxifa_deinit();
  deinit_to_cnt();
}

irs_uarc irs::irsmb_cl::size()
{
  return ir_end_byte;
}

void irs::irsmb_cl::status()
{
}

irs_bool irs::irsmb_cl::connected()
{
  if(m_first_read)
    return irs_false;
  else
    return irs_true;
}

irs_bool irs::irsmb_cl::connected_1()
{
  return irs_true;
}

void irs::irsmb_cl::write_bytes(raw_data_t<irs_u16>& data_u16,irs_u32 index,
  irs_u32 size,const irs_u8* buf,irs_u32 start)
{ 
  IRS_MBUS_DBG_MSG_DETAIL("Start " << start);
  IRS_MBUS_DBG_MSG_DETAIL("\n**************************Write bytes"
    "************************");
  irs_u8* data_u8 = reinterpret_cast<irs_u8*>(data_u16.data());
  irs::memcpyex(data_u8 + index, buf + start, size);
  #ifdef NOP//(IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
  for(irs_u16 i = (index/2); i<(size/2+index/2);i++)
  {
    IRS_MBUS_DBG_MSG_DETAIL(dec<<"Vector["<<(int)i<<"] = "<< hex<<showbase 
      <<(int)vec[i]);
  }
  for(irs_u16 i = (start); i<(size+start);i++)
  {
    IRS_MBUS_DBG_MSG_DETAIL(dec<<"Buf["<<(int)i<<"] = "<<hex<<showbase
      <<(int)buf[i]);
  }
  #endif
}

void irs::irsmb_cl::get_bytes(raw_data_t<irs_u16> &data_u16,irs_u32 index,
  irs_u32 size,irs_u8 *buf,irs_u32 start)
{
  IRS_MBUS_DBG_MSG_DETAIL("get_bytes....................");
  irs_u8* data_u8 = reinterpret_cast<irs_u8*>(data_u16.data());
  irs::memcpyex(buf + start, data_u8 + index, size);
  #ifdef NOP//(IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
  IRS_MBUS_DBG_MSG_DETAIL("\n**************************Get bytes"
    "************************");
  for(irs_u16 i = (index/2); i<(size/2+index/2);i++)
  {
    IRS_MBUS_DBG_MSG_DETAIL(dec<<"Vector["<<(int)i<<"] = "<< hex<<showbase 
      <<(int)vec[i]);
  }
  for(irs_u16 i = (start); i<(size+start);i++)
  {
    IRS_MBUS_DBG_MSG_DETAIL(dec<<"Buf["<<(int)i<<"] = "<<hex<<showbase
      <<(int)buf[i]);
  }
  IRS_MBUS_DBG_MSG_DETAIL(" OK");
  if(buf[0] == 0)
  {
    IRS_MBUS_DBG_MSG_DETAIL("Index: "<<dec<<(int)index);
    IRS_MBUS_DBG_MSG_DETAIL("Size: "<<dec <<(int)size);
    IRS_MBUS_DBG_MSG_DETAIL("Vec[" <<dec<<(int)(index/2)<<"] = "
      << vec[index/2]);
  }
  #endif
}

void irs::irsmb_cl::range(irs_u32 index, irs_u32 size, irs_u32 M1, irs_u32 M2,
  irs_u32 *num,irs_u32 *start)
{
  IRS_MBUS_DBG_MSG_DETAIL("range....................");
  irs_u32 var = index + size;
  if((index>=M1)&&(index<M2)&&(var>M1)&&(var <= M2))
  {
    *num = size;
    *start = index - M1;
  }
  else if((index>M1)&&(index<M2)&&(var>M2))
  {
    *num = M2 - index;
    *start = index - M1;
  }
  else if((index<M1)&&(var>M1)&&(var < M2))
  {
    *num = var - M1;
    *start = 0;
  }
  else if((index<=M1)&&(var >= M2))
  {
    *num = M2 - M1;
    *start = 0;
  }
}

void irs::irsmb_cl::read(irs_u8 *ap_buf, irs_uarc index, irs_uarc a_size)
{
  IRS_MBUS_DBG_MSG_DETAIL("read....................");
  di_size_byte = 0; di_start_byte = 0;
  coils_size_byte = 0; coils_start_byte = 0;
  hr_size_byte = 0; hr_start_byte = 0;
  ir_size_byte = 0; ir_start_byte = 0;
  range(index,a_size,0,di_end_byte,&di_size_byte,&di_start_byte);
  range(index,a_size,di_end_byte,coils_end_byte,&coils_size_byte,
    &coils_start_byte);
  range(index,a_size,coils_end_byte,hr_end_byte,&hr_size_byte,&hr_start_byte);
  range(index,a_size,hr_end_byte,ir_end_byte,&ir_size_byte,&ir_start_byte);
  irs_u16 i = 0;
  if((di_size_byte != 0)||(di_start_byte != 0))
  {
    packet_data_8_t &m_packet_di = *(packet_data_8_t *)in_str;
    m_packet_di.byte_count = irs_u8(di_size_byte);
    bit_copy(in_str, m_discr_inputs_bit_r.data(), 
      di_start_byte*8, 0, sizeof(di_size_byte));
    for(;i<(irs_u16)di_size_byte;i++)
      ap_buf[i] = m_packet_di.value[i];
  }
  if((coils_size_byte != 0)||(coils_start_byte != 0))
  {
    packet_data_8_t &m_packet_coils = *(packet_data_8_t *)in_str;
    m_packet_coils.byte_count = irs_u8(coils_size_byte);
    bit_copy(in_str, m_coils_bit_r.data(), coils_start_byte*8, 0,
      sizeof(coils_size_byte));
    for(;i<irs_u16(coils_size_byte + di_size_byte);i++)
    {
      ap_buf[i] = m_packet_coils.value[i-di_size_byte];
    }
  }
  if((hr_size_byte != 0)||(hr_start_byte != 0))
  {
    get_bytes(m_hold_regs_reg_r,hr_start_byte,hr_size_byte,ap_buf,i);\
    IRS_LIB_ASSERT((IRS_ARRAYSIZE(ap_buf)-i) >= hr_size_byte);
  }
  if((ir_size_byte != 0)||(ir_start_byte != 0))
  {
    get_bytes(m_input_regs_reg_r,ir_start_byte,ir_size_byte,ap_buf,
      i+hr_size_byte);
    IRS_LIB_ASSERT((IRS_ARRAYSIZE(ap_buf)-(i+hr_size_byte)) >= ir_size_byte);
  }
}
void irs::irsmb_cl::write(const irs_u8 *buf, irs_uarc index, irs_uarc size)
{
  IRS_MBUS_DBG_MSG_DETAIL("write....................");
  irs_u32 coils_size_byte = 0, coils_start_byte = 0;
  irs_u32 hr_size_byte = 0, hr_start_byte = 0;
  range(index,size,di_end_byte,coils_end_byte,&coils_size_byte,
    &coils_start_byte);
  range(index,size,coils_end_byte,hr_end_byte,&hr_size_byte,&hr_start_byte);
  if((coils_size_byte != 0)||(coils_start_byte != 0))
  {
    packet_data_8_t &m_pack_coils = *(packet_data_8_t *)(buf-1);
    m_pack_coils.byte_count = irs_u8(coils_size_byte);
    bit_copy(buf-1,m_coils_bit_w.data(),0,coils_start_byte*8,
      (irs_u16)size);
    for(irs_u16 i = irs_u16(coils_start_byte*8);
      i < irs_u16(coils_start_byte*8+coils_size_byte*8);i++)
    {
      m_rforwr[i] = 1;
    }
  }
  if((hr_size_byte != 0)||(hr_start_byte != 0))
  {
    write_bytes(m_hold_regs_reg_w,hr_start_byte,hr_size_byte,buf,
      coils_size_byte);
    IRS_LIB_ASSERT(m_hold_regs_reg_w.size() >= hr_size_byte);
    for(irs_u32 i = (hr_start_byte+coils_bit); i < (hr_size_byte + coils_bit 
      + hr_start_byte); i++)
    {
      m_rforwr[i] = 1;
    }
  }
}

irs_bool irs::irsmb_cl::bit(irs_uarc index, irs_uarc a_bit_index)
{
  IRS_MBUS_DBG_MSG_DETAIL("bit....................");
  hr_size_byte = 0;hr_start_byte = 0;
  coils_size_byte = 0;coils_start_byte = 0;
  di_start_byte = 0; di_size_byte = 0;
  ir_start_byte = 0; ir_size_byte = 0;
  range(index,1,0,di_end_byte,&di_size_byte,&di_start_byte);
  range(index,1,di_end_byte,coils_end_byte,&coils_size_byte,&coils_start_byte);
  range(index,1,coils_end_byte,hr_end_byte,&hr_size_byte,&hr_start_byte);
  range(index,1,hr_end_byte,ir_end_byte,&ir_size_byte,&ir_start_byte);
  irs_uarc byte = 0;
  if((di_size_byte != 0)||(di_start_byte != 0))
    return m_discr_inputs_bit_r[di_start_byte*8+a_bit_index];
  if((coils_size_byte != 0)||(coils_start_byte != 0))
    return m_coils_bit_r[coils_start_byte*8+a_bit_index];
  if (a_bit_index < 8) {
    if((hr_size_byte != 0)||(hr_start_byte != 0))
    {
      byte = m_hold_regs_reg_r[ir_start_byte/2];
      if(byte&=(1<<a_bit_index))
        return irs_true;
      else
        return irs_false;
    }
    if((ir_size_byte != 0)||(ir_start_byte != 0))
    {
      byte = m_input_regs_reg_r[hr_start_byte/2];
      if(byte &= (1<<a_bit_index))
        return irs_true;
      else
        return irs_false;
    }
  }
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    if((coils_size_byte == 0)&&(di_size_byte == 0)&&
      (hr_size_byte == 0)&&(ir_size_byte == 0))
      cout << "\n***************Illegal index or bufer size******************"
        "\n";
  #endif
  return irs_false;
}

void irs::irsmb_cl::set_bit(irs_uarc index, irs_uarc a_bit_index)
{
  IRS_MBUS_DBG_MSG_DETAIL("set_bit....................");
  hr_size_byte = 0;
  hr_start_byte = 0;
  coils_size_byte = 0;
  coils_start_byte = 0;
  range(index,1,di_end_byte,coils_end_byte,&coils_size_byte,&coils_start_byte);
  range(index,1,hr_end_byte,ir_end_byte,&hr_size_byte,&hr_start_byte);
  irs_uarc byte = 0;
  if (a_bit_index < 8) {
    if((coils_size_byte != 0) || (coils_start_byte != 0))
    {
      m_coils_bit_w[coils_start_byte*8+a_bit_index] = 1;
      m_rforwr[coils_start_byte*8+a_bit_index] = 1;
    }
    if((hr_size_byte != 0) || (hr_start_byte != 0))
    {
      byte = m_hold_regs_reg_r[hr_start_byte/2];
      byte |= (1<<a_bit_index);
      m_hold_regs_reg_w[hr_start_byte/2] = irs_u16(byte);
      m_rforwr[hr_start_byte + coils_bit] = 1;
    }
  }
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    if((coils_size_byte == 0)&&(hr_size_byte == 0))
      cout<<"ERROR\n";
  #endif
}

void irs::irsmb_cl::clear_bit(irs_uarc index, irs_uarc a_bit_index)
{
  IRS_MBUS_DBG_MSG_DETAIL("clear_bit....................");
  hr_size_byte = 0;
  hr_start_byte = 0;
  coils_size_byte = 0;
  coils_start_byte = 0;
  range(index,1,di_end_byte,coils_end_byte,&coils_size_byte,&coils_start_byte);
  range(index,1,hr_end_byte,ir_end_byte,&hr_size_byte,&hr_start_byte);
  irs_uarc byte = 0;
  if (a_bit_index < 8) {
    if((coils_size_byte != 0)||(coils_start_byte != 0))
    {
      m_coils_bit_w[coils_start_byte*8+a_bit_index] = 0;
      m_rforwr[coils_start_byte*8+a_bit_index] = 1;
    }
    if((hr_size_byte != 0)||(hr_start_byte != 0))
    {
      byte = m_hold_regs_reg_r[hr_start_byte/2];
      byte&=~(1<<a_bit_index);
      m_hold_regs_reg_w[hr_start_byte/2] = irs_u16(byte);
      m_rforwr[hr_start_byte + coils_bit] = 1;
    }
  }
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    if((coils_size_byte == 0)&&(hr_size_byte == 0))
      cout<<"ERROR";
  #endif
}

void irs::irsmb_cl::make_packet(irs_uarc a_index, irs_u16 a_size)
{
  IRS_MBUS_DBG_MSG_DETAIL("make_packet....................");
  complex_modbus_header_t &cmb_head = *(complex_modbus_header_t *)m_spacket;
  request_read_t &sec_head = *(request_read_t *)(m_spacket+size_of_header);
  m_bytes = 0;
  m_size_byte_end = 0;
  switch(m_command)
  {
    case WMcoils:
    {
      if(a_size%8 == 0)
         m_bytes = irs_u16(a_size/8 + 1);
      else
         m_bytes = irs_u16(a_size/8 + 2);
    }
    break;
    case WM_regs:
    {
      m_bytes = irs_u16(a_size*2 + 1);
      convert(m_spacket,size_of_header + size_of_read_header+1,(irs_u8)m_bytes);
    }
    break;
    default : {
    }
  }
  cmb_head.transaction_id   = IRS_NULL;
  cmb_head.proto_id         = IRS_NULL;
  cmb_head.length           =
    irs_u16(size_of_header+size_of_read_header+m_bytes-size_of_MBAP);
  cmb_head.unit_identifier  = 1;
  cmb_head.function_code    = m_command;
  sec_head.starting_address = (irs_u16)a_index;
  sec_head.num_of_regs      = a_size;
  m_stpos = sec_head.starting_address;
  m_size_byte_end = cmb_head.length;
  for(irs_u16 t = 0; t<(cmb_head.length+size_of_MBAP);t++)
    IRS_MBUS_DBG_MSG_DETAIL(hex_u8<< ((int)(m_spacket[t])) << ' ');
  convert(m_spacket,0,size_of_MBAP);
  convert(m_spacket,size_of_header,size_of_read_header);
  IRS_MBUS_DBG_MSG_DETAIL("\nPacket ready");
}

void irs::irsmb_cl::set_delay_time(double time)
{
  m_loop_timer.set(make_cnt_s(time));
}

void irs::irsmb_cl::tick()
{
  if(connected_1()) 
  { 
    switch(m_mode)
    {
      case get_table:
      {
        switch(RT)
        {
          case gt_start:
          {
            if(m_loop_timer.check())
            {
              m_mode = get_table;
              RT = R_DI;
            }
          } break;
          case R_DI:
          {
            m_command = RT;
            if((m_global_index + size_of_data*8) < di_bit)
            {
              make_packet(m_global_index,size_of_data*8);
              m_global_index += irs_u16(size_of_data*8);
              m_ibank += size_of_data;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
            else
            {
              make_packet(m_global_index,irs_u16(di_bit-m_global_index));
              m_ibank += (di_bit-m_global_index)/8;
              m_global_index = 0;
              RT = Rcoil;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
          }
          break;
          case Rcoil:
          {
            m_command = RT;
            if((m_global_index + size_of_data*8) < coils_bit)
            {
              make_packet(m_global_index,(size_of_data)*8);
              m_global_index += irs_u16(size_of_data*8);
              m_ibank += size_of_data;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
            else
            {
              make_packet(m_global_index,irs_u16(coils_bit-m_global_index));
              m_ibank += (coils_bit-m_global_index)/8;
              m_global_index = 0;
              RT = R_HR;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
          }
          break;
          case R_HR:
          {
            m_command = RT;
            if(irs_u32(m_global_index + s_regbuf) < irs_u32(hr_reg/2))
            {
              make_packet(m_global_index,s_regbuf);
              m_global_index += s_regbuf;
              m_ibank += s_regbuf;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
            else
            {
              make_packet(m_global_index,
                irs_u16(hr_reg/2u-(irs_u32)m_global_index));
              m_ibank += ((hr_reg/2u)-m_global_index);
              m_global_index = 0;
              RT = R_IR;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
          }
          break;
          case R_IR:
          {
            m_command = RT;
            if(irs_u32(m_global_index + s_regbuf) < irs_u32(ir_reg/2))
            {
              make_packet(m_global_index,s_regbuf);
              m_global_index += s_regbuf;
              m_ibank += s_regbuf;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
            else
            {
              make_packet(m_global_index,
                irs_u16((ir_reg/2) - m_global_index));
              m_ibank += ((ir_reg/2) - m_global_index);
              m_global_index = 0;
              RT = ERT;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
          }
          break;
          case ERT:
          {
            m_ibank = 0;
            m_first_read = irs_false;
            //OUTDBG(cout << "We read entire table\n");
            m_global_index = 0;
            m_command = Status;
            //#ifdef NOP
            IRS_MBUS_DBG_MSG_DETAIL("**********************Discret Inputs"
              "*******************");
            for(irs_u16 i = 0; i<  di_bit;i++)
            {
              IRS_MBUS_DBG_MSG_DETAIL(m_discr_inputs_bit_r[i]);
            }
            IRS_MBUS_DBG_MSG_DETAIL("\n*******************************"
              "******************");
            IRS_MBUS_DBG_MSG_DETAIL("**************COILS**************"
              "******************");
            for(irs_u16 i = 0; i< coils_bit;i++)
            {
              IRS_MBUS_DBG_MSG_DETAIL(m_coils_bit_r[i]);
            }
            IRS_MBUS_DBG_MSG_DETAIL("\n*******************************"
              "*******************");
            IRS_MBUS_DBG_MSG_DETAIL("************Holding registers****"
              "*******************");
            for(irs_u16 i = 0; i< irs_u16(hr_reg/2);i++)
            {
              IRS_MBUS_DBG_MSG_DETAIL(hex_u16<<
                (int)m_hold_regs_reg_r[i]<<' ');
            }
            IRS_MBUS_DBG_MSG_DETAIL("\n*******************************"
              "*********************");
            IRS_MBUS_DBG_MSG_DETAIL("**************Input registers****"
            "*********************");
            for(irs_u16 i = 0; i<irs_u16(ir_reg/2) ;i++)
            {
              IRS_MBUS_DBG_MSG_DETAIL(hex_u16<<
                (int)m_input_regs_reg_r[i]<<' ');
            }
            IRS_MBUS_DBG_MSG_DETAIL(hex_u16<< "\n*********************"
              "**************");
            //#endif
            //cin.get();
            make_packet(m_global_index,0);
            RT = gt_start;
            IRS_MBUS_DBG_MSG_BASE("wait_mode");
            m_mode = wait_mode;
          }
          break;
          default : {
          }
        }
        m_read_table = irs_false;
        //OUTDBG(cout << "get_table \n");
      }
      break;
      case w_mode:
      {
        //OUTDBG(cout << "Write mode\n");
        for(irs_u16 t = 0; t<irs_u16(size_of_MBAP + m_size_byte_end);t++)
          IRS_MBUS_DBG_MSG_DETAIL("(" << ((int)(m_spacket[t])) << ") ");
        IRS_MBUS_DBG_MSG_DETAIL("\nPacket: "<< ((int)m_spacket));
        mxifa_write_begin(mp_handle, &m_mbchdata, m_spacket, size_of_MBAP
          + m_size_byte_end);
        IRS_MBUS_DBG_MSG_BASE("wend_w");
        m_mode = wend_w;
        //set_delay_time(WAIT_LIENT_TIME);
        ///set_to_cnt(wait, del_time);
      }
      break;
      case wend_w:
      {
        //OUTDBG(cout << "Write end 1 .. \n");
        if(mxifa_write_end(mp_handle,irs_false)) 
        {
          //OUTDBG(cout << "Write end\n");
          IRS_MBUS_DBG_MSG_BASE("r_head_mode");
          m_mode = r_head_mode;
          //OUTDBG(cout << "Write end 2 .. \n");
        }
      }
      break;
      case r_head_mode:
      {
        //OUTDBG(cout << "r_head_mode \n");
        mxifa_read_begin(mp_handle,&m_mbchdata,m_rpacket,size_of_MBAP);
        IRS_MBUS_DBG_MSG_BASE("r_pack_mode");
        m_mode = r_pack_mode;
      }
      break;
      case r_pack_mode:
      { 
        //IRS_MBUS_DBG_MSG_BASE("wend_r .........");
        if(mxifa_read_end(mp_handle,irs_false))
        {
          convert(m_rpacket,0,size_of_MBAP);
          for(irs_u16 t = 0; t<size_of_MBAP;t++)
            IRS_MBUS_DBG_MSG_DETAIL(hex_u8<< ((int)(m_rpacket[t])) << ' ');
          complex_modbus_header_t &cmb_head =
            *(complex_modbus_header_t *)m_rpacket;
          pack_d8_t &raw_bytes = *(pack_d8_t *)(m_rpacket+size_of_MBAP);
          mxifa_read_begin(mp_handle, &m_mbchdata, raw_bytes.value, 
            cmb_head.length);
          set_to_cnt(rtimeout,cl_rtime);
          IRS_MBUS_DBG_MSG_BASE("wend_r");
          m_mode = wend_r;
          //OUTDBG(cout << "r_pack_mode \n");
        }
      }
      break;
      case wend_r:
      {
        if(mxifa_read_end(mp_handle,irs_false))
        {
          convert(m_spacket,8,size_of_read_header);
          for(irs_u16 t = 0; t<(size_of_MBAP*2);t++)
            IRS_MBUS_DBG_MSG_DETAIL(hex_u8<< ((int)(m_rpacket[t])) << ' ');
          IRS_MBUS_DBG_MSG_BASE("pr_mode");
          m_mode = pr_mode;
        }
        if(test_to_cnt(rtimeout))
        {
          if(m_read_table)
          {
            IRS_MBUS_DBG_MSG_BASE("get_table");
            m_mode = get_table;
          }
          else
          {
            IRS_MBUS_DBG_MSG_BASE("search_block");
            m_mode = search_block;
          }
          if(m_part_send)
          {
            IRS_MBUS_DBG_MSG_BASE("send_data");
            m_mode = send_data;
          }
          if(m_first_read)
          {
            IRS_MBUS_DBG_MSG_BASE("get_table");
            m_mode = get_table;
          }
        }
      }
      break;
      case pr_mode:
      {
        complex_modbus_header_t &cmb_head =
          *(complex_modbus_header_t *)m_rpacket;
        IRS_MBUS_DBG_MSG_DETAIL("Send packet - length:            " <<
          hex_u16 << cmb_head.length);
        IRS_MBUS_DBG_MSG_DETAIL("Send packet - unit_identifier:   " <<
          hex_u16 << (int)cmb_head.unit_identifier);
        IRS_MBUS_DBG_MSG_DETAIL("Send packet - function_code:     " <<
          hex_u16 << (int)cmb_head.function_code);

        for(irs_u16 i = 0;i<(cmb_head.length + size_of_MBAP);i++)
          IRS_MBUS_DBG_MSG_DETAIL(hex_u8<<(int)m_rpacket[i]<<' ');
        IRS_MBUS_DBG_MSG_DETAIL("Length of whole send packet: " <<
          (cmb_head.length + size_of_MBAP));
        switch(cmb_head.function_code)
        {
          case Rcoil:
          {
            packet_data_8_t &m_pack_coils =
              *(packet_data_8_t *)(m_rpacket + size_of_header);
            bit_copy(m_rpacket+size_of_header,m_coils_bit_r.data(),
              0, m_stpos,(irs_u16)coils_size_byte);
          }
          break;
          case R_DI:
          {
            packet_data_8_t &m_pack_di =
              *(packet_data_8_t *)(m_rpacket + size_of_header);
            bit_copy(m_rpacket + size_of_header, 
              m_discr_inputs_bit_r.data(), 0, m_stpos,(irs_u16)di_size_byte);
          }
          break;
          case R_HR:
          {
            request_wr_coils_t &m_packet_hr =
              *(request_wr_coils_t*)(m_rpacket+size_of_header);
            convert(m_rpacket,size_of_header + 1,m_packet_hr.byte_count);
            range(m_stpos*2, m_packet_hr.byte_count, 0, hr_reg,
              &hr_size_byte, &hr_start_byte);
            write_bytes(m_hold_regs_reg_r,hr_start_byte,
              hr_size_byte,m_packet_hr.value,0);
            IRS_LIB_ASSERT(m_hold_regs_reg_r.size() >= hr_size_byte);
            IRS_MBUS_DBG_MSG_DETAIL("");
            IRS_MBUS_DBG_MSG_DETAIL("Start position: "<<m_stpos);
            IRS_MBUS_DBG_MSG_DETAIL("Count of regs: "
              <<(int)m_packet_hr.byte_count);
            for(irs_u16 i = m_stpos;i<(m_stpos + m_packet_hr.byte_count/2);i++)
              IRS_MBUS_DBG_MSG_DETAIL(hex_u16<<(int)m_hold_regs_reg_r[i]<<' ');
          }
          break;
          case R_IR:
          {
            request_wr_coils_t &m_packet_ir =
              *(request_wr_coils_t*)(m_rpacket+size_of_header);
            convert(m_rpacket,size_of_header + 1,m_packet_ir.byte_count);
            range(m_stpos*2, m_packet_ir.byte_count, 0, ir_reg,
              &ir_size_byte, &ir_start_byte);
            write_bytes(m_input_regs_reg_r,ir_start_byte,
              ir_size_byte,m_packet_ir.value,0);
            IRS_LIB_ASSERT(m_input_regs_reg_r.size() >= ir_size_byte);
          }
          break;
          case WMcoils:
          {
            request_read_t &sec_head =
              *(request_read_t*)(m_spacket+size_of_header);
            for(irs_u16 reg_index = 0; reg_index < sec_head.num_of_regs;
              reg_index++)
            {
              m_rforwr[m_stpos + reg_index] = 0;
              m_coils_bit_w[m_stpos + reg_index] = 0;
            }
          }
          break;
          case WM_regs:
          {
            request_read_t &sec_head = 
              *(request_read_t*)(m_spacket+size_of_header);
            for(irs_u16 reg_index = 0; reg_index < sec_head.num_of_regs;
              reg_index++) //*2
            {
              m_rforwr[coils_bit + m_stpos + reg_index] = 0;
              m_hold_regs_reg_w[m_stpos + reg_index] = 0;
            }
          }
          break;
          case Status:
          {
          }
          break;
        }
          for(irs_u16 t = 0; t<size_of_packet;t++)
            m_spacket[t] = 0;
        if(m_read_table)
        {
          IRS_MBUS_DBG_MSG_BASE("get_table");
          m_mode = get_table;
        }
        else
        {
          IRS_MBUS_DBG_MSG_BASE("search_block");
          m_mode = search_block;
        }
        if(m_part_send)
        {
          IRS_MBUS_DBG_MSG_BASE("send_data");
          m_mode = send_data;
        }
        if(m_first_read)
        {
          IRS_MBUS_DBG_MSG_BASE("get_table");
          m_mode = get_table;
        }
      }
      break;
      case search_block:
      {
        IRS_MBUS_DBG_MSG_DETAIL("!!!!!!!!!!!!!!!!Search Block"
          "!!!!!!!!!!!!!!!!!!");
        for(irs_u8 i = 0; i < (irs_u8)m_rforwr.size();i++)
          IRS_MBUS_DBG_MSG_DETAIL(m_rforwr[i]<<' ');
        irs_bool catch_block = irs_false;
        //OUTDBG(cout << "ok";
        for(;search_index <(coils_bit+hr_reg);)
        {
          if((m_rforwr[search_index] == irs_true)&&(catch_block == irs_false))
          {
            m_start_block = search_index;
            catch_block = irs_true;
            m_nothing = irs_false;
          }
          if((catch_block == irs_true)&&(m_rforwr[search_index] == irs_false))
          {
            m_nregs = search_index - m_start_block;
            IRS_MBUS_DBG_MSG_BASE("send_data");
            m_mode = send_data;
            if((irs_u32(m_nregs + m_start_block) >= irs_u32(coils_bit))&&
              (m_start_block < (irs_u32)coils_bit))
            {
              search_index -= m_nregs;
              m_nregs = coils_bit - m_start_block;
              search_index += m_nregs;
            }
            break;
          }
          search_index++;
          if((catch_block == irs_true)&&
            (search_index == (coils_bit+hr_reg)))
          { 
            m_nregs = search_index - m_start_block;
            IRS_MBUS_DBG_MSG_BASE("send_data");
            m_mode = send_data;
            search_index = 0;
            break;
          }
          if(search_index >= (coils_bit+hr_reg))
          {
            search_index = 0;
            m_nothing = irs_true;
            IRS_MBUS_DBG_MSG_BASE("get_table");
            m_mode = get_table;
            break;
          }
        }
      }
      break;
      case send_data:
      {
        m_read_table = irs_true;
        if((m_start_block>=(irs_u32)coils_bit) &&
          (m_start_block<(irs_u32)(coils_bit + hr_reg)))
        {
          m_command = WM_regs;//function_code = m_command
          hr_start_byte = m_start_block - coils_bit;
          hr_size_byte = m_nregs;
          auto_arr<irs_u16> mass(new irs_u16[hr_reg/2]);
          if(hr_size_byte > ((s_regbuf-1)*2))
          {
            request_wr_regs_t &m_packet_hr =
              *(request_wr_regs_t*)(m_spacket + size_of_header
                + size_of_read_header);
            m_stpos = irs_u16(hr_start_byte/2);
            m_packet_hr.byte_count = (s_regbuf-1)*2;
            irs_u8 rmass[size_of_data];
            irs_u16 nul = 0;
            get_bytes(m_hold_regs_reg_w,hr_start_byte,(s_regbuf-1)*2,rmass,nul);
            IRS_LIB_ASSERT(IRS_ARRAYSIZE(rmass) >= (s_regbuf-1)*2);
            for(irs_u8 i = 0;i<(s_regbuf-1);i++)
            {
              m_packet_hr.value[i] = rmass[i];
            }
            make_packet(hr_start_byte/2,(s_regbuf-1));
            for(irs_u32 i = m_start_block;
              i < (m_start_block +(s_regbuf-1)*2);i++)
              m_rforwr[i] = 0;
            m_start_block += ((s_regbuf-1)*2);
            m_nregs -= ((s_regbuf-1)*2);
            m_part_send = irs_true;
          }
          else
          {
            for(irs_u16 t = 0; t<size_of_packet;t++)
              m_spacket[t] = 0;
            if(hr_size_byte%2 == 0)
            {
              packet_data_8_t &m_packet_hr =
                *(packet_data_8_t * )(m_spacket + size_of_header 
                  + size_of_read_header);
              if((hr_start_byte%2) !=0)
              {
                irs_u16 nul = 0;
                get_bytes(m_hold_regs_reg_r,hr_start_byte-1,1,
                  m_packet_hr.value,nul);
                IRS_LIB_ASSERT(IRS_ARRAYSIZE(m_packet_hr.value) >= 1);
                m_stpos = irs_u16(hr_start_byte/2);
                m_packet_hr.byte_count = (irs_u8)hr_size_byte;
                get_bytes(m_hold_regs_reg_w,hr_start_byte,
                  hr_size_byte,m_packet_hr.value,1);
                IRS_LIB_ASSERT((IRS_ARRAYSIZE(m_packet_hr.value)-1) >=
                  hr_size_byte);
                make_packet(hr_start_byte/2,irs_u16((hr_size_byte/2)+1));
              }
              else
              {
                m_stpos = irs_u16(hr_start_byte/2);
                irs_u16 nul = 0;
                m_packet_hr.byte_count = irs_u8(hr_size_byte);
                get_bytes(m_hold_regs_reg_w,hr_start_byte,
                  hr_size_byte,m_packet_hr.value,nul);
                IRS_LIB_ASSERT(IRS_ARRAYSIZE(m_packet_hr.value) >=hr_size_byte);
                make_packet(hr_start_byte/2,irs_u16(hr_size_byte/2));
              }
            }
            else
            {
              packet_data_8_t &m_packet_hr =
                *(packet_data_8_t * )(m_spacket + size_of_header 
                  + size_of_read_header);
              if((hr_start_byte%2) !=0)
              {
                irs_u16 nul = 0;
                get_bytes(m_hold_regs_reg_r,hr_start_byte-1,1,
                  m_packet_hr.value,nul);
                IRS_LIB_ASSERT(IRS_ARRAYSIZE(m_packet_hr.value) >= 1);
                m_stpos = irs_u16(hr_start_byte/2);
                m_packet_hr.byte_count = irs_u8(hr_size_byte);
                get_bytes(m_hold_regs_reg_w,hr_start_byte,
                  hr_size_byte,m_packet_hr.value,1);
                IRS_LIB_ASSERT((IRS_ARRAYSIZE(m_packet_hr.value)-1) >=
                  hr_size_byte);
                make_packet(hr_start_byte/2,irs_u16(hr_size_byte/2+1));
              }
              else
              {
                m_stpos = irs_u16(hr_start_byte/2);
                m_packet_hr.byte_count = irs_u8(hr_size_byte);
                irs_u16 nul = 0;
                get_bytes(m_hold_regs_reg_w,hr_start_byte,
                  hr_size_byte,m_packet_hr.value,nul);
                IRS_LIB_ASSERT(IRS_ARRAYSIZE(m_packet_hr.value) >=hr_size_byte);
                make_packet(hr_start_byte/2,irs_u16(hr_size_byte/2+1));
              }
            }
          }
        }
        else if (m_start_block<(irs_u32)coils_bit)
        {
          irs_u32 nul = 0;
          range(m_start_block,m_nregs,nul,coils_bit,
            &coils_size_byte,&coils_start_byte);
          m_command = WMcoils;
          packet_data_8_t &m_packet_coils =
            *(packet_data_8_t*)(m_spacket+size_of_header+size_of_read_header);
          if(coils_size_byte > size_of_data*8)
          {
            make_packet(coils_start_byte,size_of_data*8);
            m_stpos = irs_u16(coils_start_byte);
            m_packet_coils.byte_count   = size_of_data;
            bit_copy(m_spacket+size_of_header+size_of_read_header,
              m_coils_bit_w.data(), coils_start_byte, 0, 
                sizeof(coils_size_byte));
            for(irs_u32 i = m_start_block;
              i < (m_start_block + size_of_data*8);i++)
              m_rforwr[i] = 0;
          }
          else
          {
            make_packet(coils_start_byte, irs_u16(coils_size_byte));
            m_stpos = irs_u16(coils_start_byte);
            if(coils_size_byte%8 == 0)
              m_packet_coils.byte_count = irs_u8(coils_size_byte/8);
            else
              m_packet_coils.byte_count = irs_u8(coils_size_byte/8+1);
            bit_copy(m_spacket+size_of_header+size_of_read_header,
              m_coils_bit_w.data(),coils_start_byte,
                0, sizeof(coils_size_byte));
            for(irs_u32 i = m_start_block;
              i < (m_start_block + coils_size_byte);i++)
              m_rforwr[i] = 0;
          }
        }
        IRS_MBUS_DBG_MSG_BASE("wait_mode");
        m_mode = wait_mode;
      }
      break;
      case wait_mode:
      {
        IRS_MBUS_DBG_MSG_BASE("w_mode");
        m_mode = w_mode;
      }
      break;
    }
  }
  else
  {
    mp_handle = mxifa_open(m_channel,irs_false);
    IRS_MBUS_DBG_MSG_BASE("Channel open");
    mxifa_linux_tcp_ip_cl_cfg cfg;
    mxifa_get_config(mp_handle, (void *)&cfg);
    cfg.dest_port = m_dport;
    cfg.dest_ip = m_dip;
    mxifa_set_config(mp_handle, (void *)&cfg);
  }
  mxifa_tick();
}
