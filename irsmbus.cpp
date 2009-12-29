// Клиент и сервер modbus
// Дата создания: 16.09.2008
// Дата последнего изменения: 10.12.2009

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
#define ILLEGAL_FUNCTION          1
#define ILLEGAL_DATA_ADDRESS      2
#define ILLEGAL_DATA_VALUE        3
#define SERVER_FAILURE            4
#define ACKNOWLEDGE               5
#define SLAVE_DEVICE_BUSY         6
#define MEMORY_PARITY_ERROR       8
#define GATEWAY_PATH_UNAVALIABLE           0A
#define GATEWAY_TARGET_FAILED_TO_RESPONSE  0B

#define IRS_MBUS_MSG_NONE   0
#define IRS_MBUS_MSG_BASE   1
#define IRS_MBUS_MSG_DETAIL 2

#ifdef IRS_DEBUG
//#define IRS_MBUS_MSG_TYPE IRS_MBUS_MSG_BASE
//#define IRS_MBUS_MSG_TYPE IRS_MBUS_MSG_DETAIL
#endif //IRS_DEBUG
#define IRS_MBUS_MSG_TYPE IRS_MBUS_MSG_NONE

#if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_BASE)
#define IRS_MBUS_DBG_MONITOR(msg) msg
#define IRS_MBUS_DBG_MSG_DETAIL(msg)
#define IRS_MBUS_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG_SRC(msg) 
#elif (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
#define IRS_MBUS_DBG_MONITOR(msg) msg
#define IRS_MBUS_DBG_MSG_DETAIL(msg) IRS_LIB_DBG_MSG_SRC(msg) 
#define IRS_MBUS_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG_SRC(msg) 
#else //IRS_MBUS_MSG_TYPE
#define IRS_MBUS_DBG_MONITOR(msg)
#define IRS_MBUS_DBG_MSG_DETAIL(msg) 
#define IRS_MBUS_DBG_MSG_BASE(msg) 
#endif //IRS_MBUS_MSG_TYPE

ostream &hex_u8(ostream &s)
{
  s.setf(ios::hex, ios::basefield);
  s << setw(2) << setfill('0');
  return s;
}
ostream &hex_u16(ostream &s)
{
  s.setf(ios::showbase);
  s.setf(ios::hex , ios::basefield);
  s << setw(4) << setfill('0');
  return s;
}
ostream &dec_u8(ostream &s)
{
  s.setf(ios::dec, ios::basefield);
  s << setw(3) << setfill('0');
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
  size_t a_index_data_in, size_t a_index_data_out, size_t a_size_data_bit)
{
  int index_in = a_index_data_in%8;
  int index_out = a_index_data_out%8;
  int data_start_in = a_index_data_in/8;
  int data_start_out = a_index_data_out/8;
  int first_part_size = 0;
  int last_part_size = 0;
  if (index_in == index_out)
  {
    if((index_out != 0) && (index_in != 0))
      first_part_size = 8 - index_out;
    last_part_size = (a_size_data_bit - first_part_size)%8;
    int middle_part_size = (a_size_data_bit - 
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
      offset = a_index_data_out%8 - a_index_data_in%8;
      middle_mask = mask_gen(0, 8 - offset);
      index_bit = offset;
      offset_idx = 0;
      if((index_out != 0) && (index_in != 0))
        first_part_size = 8 - index_in;
      last_part_size = (a_size_data_bit + offset - first_part_size)%8;
    } else if((index_in - index_out) > 0) {
      offset = a_index_data_in%8 - a_index_data_out%8;
      middle_mask = mask_gen(0, offset);
      index_bit = 8 - offset;
      offset_idx = 1;
      if((index_out != 0) && (index_in != 0))
        first_part_size = 8 - index_out;
      last_part_size = (a_size_data_bit /*+ offset*/ - first_part_size)%8;
    } 
    int data_cnt = (a_size_data_bit - first_part_size)/8;
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
      int last_external_byte_index = data_cnt + data_start_in + 
        first_part_idx;
      int last_internal_byte_index = data_cnt + data_start_out + 
        first_part_idx; 
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
          if((index_in + a_size_data_bit) < 8) {
            irs_u8 data_in = ap_data_in[last_external_byte_index];
            irs_u8 mask_ext = mask_gen(8 - (a_size_data_bit%8 + index_in),
              a_size_data_bit%8);
            irs_u8 mask_int = mask_gen(0, 8 - a_size_data_bit);
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
      int external_idx = data_idx + data_start_in + 
        first_part_idx*offset_idx;
      int internal_idx = data_idx + data_start_out + 
        first_part_idx*offset_idx;
      second_part(ap_data_out[internal_idx + 1 - offset_idx], 
        ap_data_in[external_idx], index_bit, middle_mask);
    }
  }
}

irs::char_t digit_to_char(int a_digit)
{
  if (a_digit <= 9) {
    return static_cast<irs::char_t>(irst('0') + a_digit);
  } else {
    return static_cast<irs::char_t>(irst('A') + a_digit - 10);
  }
}

irs::string_t int_to_str(int a_number, int a_radix, int a_cnt)
{
  string number_str(a_cnt, irst(' '));
  for(int digit_idx = a_cnt - 1; digit_idx >= 0; digit_idx--)
  {
    int digit = a_number%a_radix;
    //number_str[a_cnt - digit_idx - 1] = digit_to_char(digit);
    number_str[digit_idx] = digit_to_char(digit);
    a_number /= a_radix; 
  }
  return number_str;
}

void irs::test_bit_copy(ostream& a_strm, size_t a_size_data_in,
  size_t a_size_data_out, size_t a_index_data_in, size_t a_index_data_out,
  size_t a_size_data)
{
  irs::raw_data_t<irs_u8> ap_data_in(a_size_data_in);
  irs::raw_data_t<irs_u8> ap_data_out(a_size_data_out);
  rand();
  for(int idx_in = 0; idx_in < static_cast<int>(ap_data_in.size()); idx_in ++)
  {
    int rand_val = rand();
    ap_data_in[idx_in] = 
      static_cast<irs_i64>(rand_val*(IRS_U8_MAX/RAND_MAX));
  }
  for(int idx_out = 0; idx_out < static_cast<int>(ap_data_out.size());
    idx_out ++)
  {
    int rand_val = rand();
    ap_data_out[idx_out] = 
      static_cast<irs_i64>(rand_val*(IRS_U8_MAX/RAND_MAX));
  }
  a_strm << "IN before operation" << endl;
  for(int idx_in = 0; idx_in < static_cast<int>(ap_data_in.size()); idx_in ++)
  {
    a_strm << int_to_str(ap_data_in[idx_in], 2, 8) << " ";
  }
  a_strm << endl;
  a_strm << "OUT before operation" << endl;
  for(int idx_out = 0; idx_out < static_cast<int>(ap_data_out.size());
    idx_out ++)
  {
    a_strm << int_to_str(ap_data_out[idx_out], 2, 8) << " ";
  }
  a_strm << endl;
  bit_copy(ap_data_in.data(), ap_data_out.data(),
    a_index_data_in, a_index_data_out, a_size_data);
  a_strm << "OUT after operation" << endl;
  for(int idx_out = 0; idx_out < static_cast<int>(ap_data_out.size());
    idx_out ++)
  {
    a_strm << int_to_str(ap_data_out[idx_out], 2, 8) << " ";
  }
  a_strm << endl;
}

void convert(irs_u8 *ap_mess, irs_u8 a_start, irs_u8 a_length)
{
  if(irs::cpu_traits_t::endian() == irs::little_endian)
  {
    for(irs_u8 i = a_start; i < (a_length + a_start);i += irs_u8(2))
    {
      irs_u8 var = ap_mess[i + 1];
      ap_mess[i + 1] = ap_mess[i];
      ap_mess[i] = var;
    }
  }
}

void set_bytes(irs::raw_data_t<irs_u16> &data_u16,size_t a_index,
  size_t a_size,const irs_u8 *ap_buf, size_t a_start)
{
  irs_u8* data_u8 = reinterpret_cast<irs_u8*>(data_u16.data());
  irs::memcpyex(data_u8 + a_index, ap_buf + a_start, a_size);
}

void get_bytes(irs::raw_data_t<irs_u16> &data_u16,size_t a_index,
  size_t a_size, irs_u8 *ap_buf, size_t a_start)
{
  IRS_MBUS_DBG_MSG_DETAIL("get_bytes");
  irs_u8* data_u8 = reinterpret_cast<irs_u8*>(data_u16.data());
  irs::memcpyex(ap_buf + a_start, data_u8 + a_index, a_size);
}

void irs::range(size_t a_index, size_t a_size, size_t M1, size_t M2,
size_t *a_num, size_t *a_start)
{
  size_t var = a_index + a_size;
  if((a_index>=M1)&&(a_index<M2)&&(var>M1)&&(var <= M2))
  {
    *a_num = a_size;
    *a_start = (a_index - M1);
  }
  else if((a_index>M1)&&(a_index<M2)&&(var>M2))
  {
    *a_num = (M2 - a_index);
    *a_start = (a_index - M1);
  }
  else if((a_index<M1)&&(var>M1)&&(var < M2))
  {
    *a_num = (var - M1);
    *a_start = 0;
  }
  else if((a_index<=M1)&&(var >= M2))
  {
    *a_num = (M2 - M1);
    *a_start = 0;
  }
}

irs::modbus_server_t::modbus_server_t(
  hardflow_t* ap_hardflow,
  size_t a_discr_inputs_size_byte,
  size_t a_coils_size_byte,
  size_t a_hold_regs_reg,
  size_t a_input_regs_reg
):
  mp_buf(size_of_packet),
  m_size_byte_end(0),
  m_coils_size_bit(size_t(a_coils_size_byte*8)),
  m_di_size_bit(size_t(a_discr_inputs_size_byte*8)),
  m_hr_size_reg(a_hold_regs_reg),
  m_ir_size_reg(a_input_regs_reg),
  m_size_byte(a_discr_inputs_size_byte + a_coils_size_byte + a_hold_regs_reg*2 +
    a_input_regs_reg*2),
  m_di_end_byte(a_discr_inputs_size_byte),
  m_coils_end_byte(a_discr_inputs_size_byte + a_coils_size_byte),
  m_hr_end_byte(a_discr_inputs_size_byte + a_coils_size_byte +
    a_hold_regs_reg*2),
  m_ir_end_byte(a_discr_inputs_size_byte + a_coils_size_byte +
    a_hold_regs_reg*2 + a_input_regs_reg*2),
  m_mode(read_header_mode),
  m_discr_inputs_byte(int(a_discr_inputs_size_byte)),
  m_coils_byte(int(a_coils_size_byte)),
  m_input_regs_reg(int(a_input_regs_reg)),
  m_hold_regs_reg(int(a_hold_regs_reg)),
  m_di_size_byte(0),
  m_di_start_byte(0),
  m_coils_size_byte(0),
  m_coils_start_byte(0),
  m_hr_size_byte(0),
  m_hr_start_byte(0),
  m_ir_size_byte(0),
  m_ir_start_byte(0),
  m_channel(0),
  mp_tcp_server(ap_hardflow),
  m_fixed_flow(mp_tcp_server),
  m_num_of_elem(0)
{
}

irs_uarc irs::modbus_server_t::size()
{
  return m_size_byte;
}

irs_bool irs::modbus_server_t::connected()
{
  return irs_true;
}

#if (IRS_MBUS_MSG_TYPE ==  IRS_MBUS_MSG_DETAIL)
void irs::modbus_server_t::modbus_pack_request_monitor(irs_u8 *ap_buf)
{
  MBAP_header_t &header = 
    reinterpret_cast<MBAP_header_t&>(*ap_buf);
  request_exception_t &req_header = 
    reinterpret_cast<request_exception_t&>(*(ap_buf + size_of_MBAP));
    
  irs::mlog() << "\n ----------- MODBUS's Header----------------\n";
  irs::mlog() << " transaction_id_lo .... " <<
    int(IRS_LOBYTE(header.transaction_id)) << endl;
  irs::mlog() << " transaction_id_hi .... " <<
    int(IRS_HIBYTE(header.transaction_id)) << endl;
  irs::mlog() << " proto_id_lo .......... " <<
    int(IRS_LOBYTE(header.proto_id)) << endl;
  irs::mlog() << " proto_id_hi .......... " <<
    int(IRS_HIBYTE(header.proto_id)) << endl;
  irs::mlog() << " length_lo ............ " <<
    int(IRS_LOBYTE(header.length)) << endl;
  irs::mlog() << " length_hi ............ " <<
    int(IRS_HIBYTE(header.length)) << endl;
  irs::mlog() << " unit_identifier ...... " <<
    int(header.unit_identifier) << endl;
    
  irs::mlog() << " function_code ........ " <<
    int(req_header.function_code);
  switch(req_header.function_code)
  {
    case read_discrete_inputs:
    {
      request_t &req_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read discret inputs" << endl;
      irs::mlog() << " starting_address_lo .. " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi .. " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " quantity_lo .......... " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " quantity_hi .......... " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
    } break;
    case read_coils:
    {
      request_t &req_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read coils" << endl;
      irs::mlog() << " starting_address_lo .. " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi .. " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " quantity_lo .......... " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " quantity_hi .......... " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
    } break;
    case read_hold_registers:
    {
      request_t &req_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read hold registers" << endl;
      irs::mlog() << " starting_address_lo .. " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi .. " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " quantity_lo .......... " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " quantity_hi .......... " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
    } break;
    case read_input_registers:
    {
      request_t &req_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read input registers" << endl;
      irs::mlog() << " starting_address_lo .. " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi .. " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " quantity_lo .......... " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " quantity_hi .......... " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
    } break;
    case write_single_coil:
    {
      request_t &req_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " write single coil" << endl;
      irs::mlog() << " output_address_lo .... " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " output_address_hi .... " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " output_value_lo ...... " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " output_value_hi ...... " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
    } break;
    case write_single_register:
    {
      request_t &req_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " write hold register" << endl;
      irs::mlog() << " register_address_lo .. " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " register_address_hi .. " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " register_value_lo .... " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " register_value_hi .... " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
    } break;
    case read_exception_status:
    {
      irs::mlog() << " read exception status " << endl;
    } break;
    case W_SevHR:
    {
      irs::mlog() << " W_SevHR" << endl;
    } break;
    case write_multiple_coils:
    {
      request_multiple_write_byte_t &req_header = 
        reinterpret_cast<request_multiple_write_byte_t&>(
        *(ap_buf + size_of_MBAP));
      irs::mlog() << " write multiple coils" << endl;
      irs::mlog() << " starting_address_lo ..... " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi ..... " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " quantity_of_outputs_lo .. " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " quantity_of_outputs_hi .. " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
      irs::mlog() << " byte_count .............. " <<
        int(req_header.byte_count) << endl;
      int coils_idx_max = 0;
      if(req_header.quantity%8 == 0)
        coils_idx_max = req_header.quantity/8;
      else
        coils_idx_max = req_header.quantity/8 + 1;
      for(int coils_idx = 0; coils_idx < coils_idx_max; coils_idx++) {
        irs::mlog() << " coils_value ........... " <<
          int(req_header.value[coils_idx]) << endl;
      }
    } break;
    case write_multiple_registers:
    {
      request_multiple_write_regs_t &req_header = 
        reinterpret_cast<request_multiple_write_regs_t&>(
        *(ap_buf + size_of_MBAP));
      irs::mlog() << " write multiple coils" << endl;
      irs::mlog() << " starting_address_lo ....... " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi ....... " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " quantity_of_registers_lo .. " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " quantity_of_registers_hi .. " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
      irs::mlog() << " byte_count ................ " <<
        int(req_header.byte_count) << endl;
      for(int reg_idx = 0; reg_idx < int(req_header.quantity)/8; reg_idx++) {
        irs::mlog() << " register_value_lo ......... " <<
          int(IRS_LOBYTE(req_header.value[reg_idx])) << endl;
        irs::mlog() << " register_value_hi ......... " <<
          int(IRS_HIBYTE(req_header.value[reg_idx])) << endl;
      }
    } break;
    case Rsize:
    {
      irs::mlog() << " Rsize" << endl;
    } break;
    case Rtab:
    {
      irs::mlog() << " Rtab" << endl;
    } break;
    default:
    {
      irs::mlog() << " ILLEGAL_FUNCTION" << endl;
    } break;
  }
}
#endif // IRS_MBUS_MSG_DETAIL

#if (IRS_MBUS_MSG_TYPE ==  IRS_MBUS_MSG_DETAIL)
void irs::modbus_server_t::modbus_pack_response_monitor(irs_u8 *ap_buf)
{
  MBAP_header_t &header = 
    reinterpret_cast<MBAP_header_t&>(*ap_buf);
  request_exception_t &resp_header = 
    reinterpret_cast<request_exception_t&>(*(ap_buf + size_of_MBAP));
    
  irs::mlog() << "\n ----------- MODBUS's Header----------------\n";
  irs::mlog() << " transaction_id_lo .... " <<
    int(IRS_LOBYTE(header.transaction_id)) << endl;
  irs::mlog() << " transaction_id_hi .... " <<
    int(IRS_HIBYTE(header.transaction_id)) << endl;
  irs::mlog() << " proto_id_lo .......... " <<
    int(IRS_LOBYTE(header.proto_id)) << endl;
  irs::mlog() << " proto_id_hi .......... " <<
    int(IRS_HIBYTE(header.proto_id)) << endl;
  irs::mlog() << " length_lo ............ " <<
    int(IRS_LOBYTE(header.length)) << endl;
  irs::mlog() << " length_hi ............ " <<
    int(IRS_HIBYTE(header.length)) << endl;
  irs::mlog() << " unit_identifier ...... " <<
    int(header.unit_identifier) << endl;
    
  irs::mlog() << " function_code ........ " <<
    int(resp_header.function_code);
  switch(resp_header.function_code)
  {
    case read_discrete_inputs:
    {
      response_read_byte_t &resp_header = 
        reinterpret_cast<response_read_byte_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read discret inputs" << endl;
      irs::mlog() << " byte_count ........... " <<
        int(resp_header.byte_count) << endl;
      for(int di_idx = 0; di_idx < resp_header.byte_count; di_idx++) {
        irs::mlog() << " discret input[" << di_idx << "] ..... " <<
          int(resp_header.value[di_idx]) << endl;
      }
    } break;
    case read_coils:
    {
      response_read_byte_t &resp_header = 
        reinterpret_cast<response_read_byte_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read coils" << endl;
      irs::mlog() << " byte_count ........... " <<
        int(resp_header.byte_count) << endl;
      for(int coils_idx = 0; coils_idx < resp_header.byte_count; coils_idx++) {
        irs::mlog() << " coils[" << coils_idx << "] ............. " <<
          int(resp_header.value[coils_idx]) << endl;
      }
    } break;
    case read_hold_registers:
    {
      response_read_reg_t &resp_header = 
        reinterpret_cast<response_read_reg_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read hold registers" << endl;
      irs::mlog() << " byte_count ........... " <<
        int(resp_header.byte_count) << endl;
      for(int h_reg_idx = 0; h_reg_idx < int(resp_header.byte_count)/2;
        h_reg_idx++) 
      {
        irs::mlog() << " hold regs lo[" << h_reg_idx << "] ...... " <<
          int(IRS_LOBYTE(resp_header.value[h_reg_idx])) << endl;
        irs::mlog() << " hold regs hi[" << h_reg_idx << "] ...... " <<
          int(IRS_HIBYTE(resp_header.value[h_reg_idx])) << endl;
      }
    } break;
    case read_input_registers:
    {
      response_read_reg_t &resp_header = 
        reinterpret_cast<response_read_reg_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read input registers" << endl;
      irs::mlog() << " byte_count ........... " <<
        int(resp_header.byte_count) << endl;
      for(int i_reg_idx = 0; i_reg_idx < int(resp_header.byte_count)/2;
        i_reg_idx++) 
      {
        irs::mlog() << " input regs lo[" << i_reg_idx << "] ..... " <<
          int(IRS_LOBYTE(resp_header.value[i_reg_idx])) << endl;
        irs::mlog() << " input regs hi[" << i_reg_idx << "] ..... " <<
          int(IRS_HIBYTE(resp_header.value[i_reg_idx])) << endl;
      }
    } break;
    case write_single_coil:
    {
      response_single_write_t &resp_header = 
        reinterpret_cast<response_single_write_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " write single coil" << endl;
      irs::mlog() << " output_address_lo .... " <<
        int(IRS_LOBYTE(resp_header.address)) << endl;
      irs::mlog() << " output_address_hi .... " <<
        int(IRS_HIBYTE(resp_header.address)) << endl;
      irs::mlog() << " output_value_lo ...... " <<
        int(IRS_LOBYTE(resp_header.value)) << endl;
      irs::mlog() << " output_value_hi ...... " <<
        int(IRS_HIBYTE(resp_header.value)) << endl;
    } break;
    case write_single_register:
    {
      response_single_write_t &resp_header = 
        reinterpret_cast<response_single_write_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " write single coil" << endl;
      irs::mlog() << " output_address_lo .... " <<
        int(IRS_LOBYTE(resp_header.address)) << endl;
      irs::mlog() << " output_address_hi .... " <<
        int(IRS_HIBYTE(resp_header.address)) << endl;
      irs::mlog() << " output_value_lo ...... " <<
        int(IRS_LOBYTE(resp_header.value)) << endl;
      irs::mlog() << " output_value_hi ...... " <<
        int(IRS_HIBYTE(resp_header.value)) << endl;
    } break;
    case read_exception_status:
    {
      irs::mlog() << " read exception status " << endl;
      response_exception_t &resp_header = 
        reinterpret_cast<response_exception_t&>(
        *(ap_buf + size_of_MBAP));
      irs::mlog() << " error_code ........... " <<
        int(resp_header.error_code) << endl;
      irs::mlog() << " exeption_code ........ " <<
        int(resp_header.exeption_code) << endl;
    } break;
    case W_SevHR:
    {
      irs::mlog() << " W_SevHR" << endl;
    } break;
    case write_multiple_coils:
    {
      request_t &resp_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read discret inputs" << endl;
      irs::mlog() << " starting_address_lo .. " <<
        int(IRS_LOBYTE(resp_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi .. " <<
        int(IRS_HIBYTE(resp_header.starting_address)) << endl;
      irs::mlog() << " quantity_lo .......... " <<
        int(IRS_LOBYTE(resp_header.quantity)) << endl;
      irs::mlog() << " quantity_hi .......... " <<
        int(IRS_HIBYTE(resp_header.quantity)) << endl;
    } break;
    case write_multiple_registers:
    {
      request_t &resp_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read discret inputs" << endl;
      irs::mlog() << " starting_address_lo .. " <<
        int(IRS_LOBYTE(resp_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi .. " <<
        int(IRS_HIBYTE(resp_header.starting_address)) << endl;
      irs::mlog() << " quantity_lo .......... " <<
        int(IRS_LOBYTE(resp_header.quantity)) << endl;
      irs::mlog() << " quantity_hi .......... " <<
        int(IRS_HIBYTE(resp_header.quantity)) << endl;
    } break;
    case Rsize:
    {
      irs::mlog() << " Rsize" << endl;
    } break;
    case Rtab:
    {
      irs::mlog() << " Rtab" << endl;
    } break;
    default:
    {
      irs::mlog() << " ILLEGAL_FUNCTION" << endl;
    } break;
  }
}
#endif // IRS_MBUS_MSG_DETAIL

void irs::modbus_server_t::read(irs_u8 *ap_buf, irs_uarc index, irs_uarc a_size)
{
  IRS_MBUS_DBG_MSG_DETAIL("irs::modbus_server_t::read");
  m_di_size_byte = 0, m_di_start_byte = 0;
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  m_hr_size_byte = 0, m_hr_start_byte = 0;
  m_ir_size_byte = 0, m_ir_start_byte = 0;
  
  range(index, a_size, m_di_start_byte, m_di_end_byte, &m_di_size_byte, 
    &m_di_start_byte);
  range(index,a_size,m_di_end_byte,m_coils_end_byte,&m_coils_size_byte,
    &m_coils_start_byte);
  range(index, a_size, m_coils_end_byte, m_hr_end_byte, &m_hr_size_byte,
    &m_hr_start_byte);
  range(index, a_size, m_hr_end_byte, m_ir_end_byte, &m_ir_size_byte,
    &m_ir_start_byte);
    
  irs_u32 i = 0;
  if((m_di_size_byte != 0))
  {
    for(; i < m_di_size_byte; i++) 
      ap_buf[i] = m_discr_inputs_byte[i];
  }
  if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
  {
    for(; i < (m_di_size_byte + m_coils_size_byte); i++) 
      ap_buf[i] = m_coils_byte[i - m_di_size_byte];
  }
  if((m_hr_size_byte != 0)||(m_hr_start_byte != 0))
  {
    get_bytes(m_hold_regs_reg, m_hr_start_byte, m_hr_size_byte, ap_buf, i);
  }
  if((m_ir_size_byte != 0)||(m_ir_start_byte != 0))
  {
    get_bytes(m_input_regs_reg, m_ir_start_byte, m_ir_size_byte, ap_buf,
      i + m_hr_size_byte);
  }
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
  for(int buf_idx = 0; buf_idx < (m_di_size_byte + m_coils_size_byte + 
    m_hr_size_byte + m_ir_size_byte); buf_idx++)
  {
    irs::mlog() << "ap_buf[" << buf_idx << "] = " <<
      (int)ap_buf[buf_idx] << endl;
  }
  #endif //IRS_MBUS_MSG_DETAIL
}

void irs::modbus_server_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
  for(int buf_idx = 0; buf_idx < m_size_byte; buf_idx ++)
  {
    irs::mlog() << "ap_buf[" << buf_idx << "] = " <<
      (int)ap_buf[buf_idx] << endl;
  }
  #endif //IRS_MBUS_MSG_DETAIL

  IRS_MBUS_DBG_MSG_DETAIL("irs::modbus_server_t::write");
  m_di_size_byte = 0, m_di_start_byte = 0;
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  m_hr_size_byte = 0, m_hr_start_byte = 0;
  m_ir_size_byte = 0, m_ir_start_byte = 0;
  
  range(a_index, a_size, m_di_start_byte, m_di_end_byte, &m_di_size_byte,
    &m_di_start_byte);
  range(a_index, a_size, m_di_end_byte, m_coils_end_byte, &m_coils_size_byte,
    &m_coils_start_byte);
  range(a_index, a_size, m_coils_end_byte, m_hr_end_byte, &m_hr_size_byte,
    &m_hr_start_byte);
  range(a_index, a_size, m_hr_end_byte, m_ir_end_byte, &m_ir_size_byte,
    &m_ir_start_byte);

  if((m_di_size_byte != 0))
  {
    bit_copy(ap_buf, m_discr_inputs_byte.data(), 0, m_di_start_byte,
      static_cast<irs_u16>(m_di_size_byte*8));
    #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    for(int buf_idx = 0; buf_idx < m_discr_inputs_byte.size(); buf_idx ++)
    {
      irs::mlog() << "m_discr_inputs_byte[" << buf_idx << "] = " <<
        (int)m_discr_inputs_byte[buf_idx] << endl;
    }
    #endif //IRS_MBUS_MSG_DETAIL
  }
  if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
  {
    bit_copy(ap_buf, m_coils_byte.data(), m_di_size_byte*8, 0,
      m_coils_size_byte*8);
    #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    for(int buf_idx = 0; buf_idx < m_discr_inputs_byte.size(); buf_idx ++)
    {
      irs::mlog() << "m_coils_byte[" << buf_idx << "] = " <<
        (int)m_coils_byte[buf_idx] << endl;
    }
    #endif //IRS_MBUS_MSG_DETAIL
  }
  if((m_hr_size_byte != 0)||(m_hr_start_byte != 0))
  {
    set_bytes(m_hold_regs_reg, 0, m_hr_size_byte, ap_buf, 
      m_di_size_byte + m_coils_size_byte);
    #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    for(int buf_idx = 0; buf_idx < m_hold_regs_reg.size(); buf_idx ++)
    {
      irs::mlog() << "m_hold_regs_reg_lo[" << buf_idx << "] = " <<
        int(IRS_LOBYTE(m_hold_regs_reg[buf_idx])) << endl;
      irs::mlog() << "m_hold_regs_reg_hi[" << buf_idx << "] = " <<
        int(IRS_HIBYTE(m_hold_regs_reg[buf_idx])) << endl;
    }
    #endif //IRS_MBUS_MSG_DETAIL
  }
  if((m_ir_size_byte != 0)||(m_ir_start_byte != 0))
  {
    set_bytes(m_input_regs_reg, 0, m_ir_size_byte, ap_buf,
      m_di_size_byte + m_coils_size_byte + m_hr_size_byte);
    #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    for(int buf_idx = 0; buf_idx < m_input_regs_reg.size(); buf_idx ++)
    {
      irs::mlog() << "m_input_regs_reg_lo[" << buf_idx << "] = " <<
        int(IRS_LOBYTE(m_input_regs_reg[buf_idx])) << endl;
      irs::mlog() << "m_input_regs_reg_hi[" << buf_idx << "] = " <<
        int(IRS_HIBYTE(m_input_regs_reg[buf_idx])) << endl;
    }
    #endif //IRS_MBUS_MSG_DETAIL
  }
}

irs_bool irs::modbus_server_t::bit(irs_uarc byte_index, irs_uarc bit_index)
{
  IRS_MBUS_DBG_MSG_DETAIL("bit..................");
  m_di_size_byte = 0, m_di_start_byte = 0;
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  m_hr_size_byte = 0, m_hr_start_byte = 0;
  m_ir_size_byte = 0, m_ir_start_byte = 0;
  range(byte_index, 1, m_di_start_byte, m_di_end_byte,
    &m_di_size_byte, &m_di_start_byte);
  range(byte_index, 1, m_di_end_byte, m_coils_end_byte, 
    &m_coils_size_byte, &m_coils_start_byte);
  range(byte_index, 1, m_coils_end_byte, m_hr_end_byte,
    &m_hr_size_byte, &m_hr_start_byte);
  range(byte_index, 1, m_hr_end_byte, m_ir_end_byte,
    &m_ir_size_byte, &m_ir_start_byte);
  if (bit_index < 8) {
    if ((m_di_size_byte != 0) || (m_di_start_byte != 0))
    {
      return to_irs_bool(m_discr_inputs_byte[m_di_start_byte + bit_index]);
    }
    if ((m_coils_size_byte != 0) || (m_coils_start_byte != 0))
    {
      return to_irs_bool(m_coils_byte[m_coils_start_byte + bit_index]);
    }
    if ((m_hr_size_byte != 0) || (m_hr_start_byte != 0))
    {
      return to_irs_bool(m_hold_regs_reg[m_ir_start_byte/2]&(1 << bit_index));
    }
    if ((m_ir_size_byte != 0) || (m_ir_start_byte != 0))
    {
      return to_irs_bool(m_input_regs_reg[m_hr_start_byte/2]&(1 << bit_index));
    }
  }
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
  if((m_coils_size_byte == 0)&&(m_di_size_byte == 0)&&
    (m_hr_size_byte == 0)&&(m_ir_size_byte == 0))
  {
    irs::mlog() << "\n***************Illegal index or bufer size"
      "******************\n\n";
  }
  #endif
  return irs_false;
}

void irs::modbus_server_t::set_bit(irs_uarc byte_index, irs_uarc bit_index)
{
  IRS_MBUS_DBG_MSG_DETAIL("set_bit..................");
  IRS_LIB_ASSERT(bit_index < 8);
  IRS_LIB_ASSERT(byte_index <= m_size_byte);
  irs_uarc byte = 0;
  m_di_size_byte = 0, m_di_start_byte = 0;
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  m_hr_size_byte = 0, m_hr_start_byte = 0;
  m_ir_size_byte = 0, m_ir_start_byte = 0;
  range(byte_index, 1, 0, m_di_end_byte, &m_di_size_byte, &m_di_start_byte);
  range(byte_index, 1, m_di_end_byte, m_coils_end_byte,
    &m_coils_size_byte, &m_coils_start_byte);
  range(byte_index, 1, m_coils_end_byte, m_hr_end_byte,
    &m_hr_size_byte, &m_hr_start_byte);
  range(byte_index, 1, m_hr_end_byte, m_ir_end_byte, 
    &m_ir_size_byte, &m_ir_start_byte);
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
  if((m_coils_size_byte == 0)&&(m_di_size_byte == 0)&&
    (m_hr_size_byte == 0)&&(m_ir_size_byte == 0))
  {
    irs::mlog() << "\n************Illegal index or bufer size"
      "***************\n\n";
  }
  #endif
  irs_u8 mask = mask_gen(8 - (bit_index + 1), 1);
  irs::mlog() << " mask = " << (int)mask << endl;
  if (bit_index < 8) {
    if((m_di_size_byte != 0)||(m_di_start_byte != 0))
    {
      irs::mlog() << " set_bit: discret inputs" << endl;
      m_discr_inputs_byte[m_di_start_byte] &= static_cast<irs_u8>(~mask);
      m_discr_inputs_byte[m_di_start_byte] |= static_cast<irs_u8>(mask);
    }
    if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
    {
      irs::mlog() << " set_bit: coils" << endl;
      m_coils_byte[m_coils_start_byte] &= static_cast<irs_u8>(~mask);
      m_coils_byte[m_coils_start_byte] |= static_cast<irs_u8>(mask);
    }
    if((m_hr_size_byte != 0)||(m_hr_start_byte != 0))
    {
      irs::mlog() << " set_bit: hold regs" << endl;
      /*byte = m_hold_regs_reg[m_hr_start_byte/2u];
      byte |= (1<<bit_index);
      m_hold_regs_reg[m_hr_start_byte/2u] = IRS_LOBYTE(byte);*/
      /*if((byte_index - (m_di_size_byte + m_coils_size_byte))%2 == 0) {
        byte = IRS_LOBYTE(m_hold_regs_reg[(byte_index - (m_di_size_byte +
          m_coils_size_byte))/2]);
        irs::mlog() << " m_hr_size_byte = " << (int)m_hr_size_byte << endl;  
        irs::mlog() << " m_hr_start_byte = " << (int)m_hr_start_byte << endl;  
        irs::mlog() << " m_hold_regs_reg[" << (byte_index - (m_di_size_byte +
          m_coils_size_byte))/2 << "] = " << 
          (int)m_hold_regs_reg[(byte_index - (m_di_size_byte +
          m_coils_size_byte))/2] << endl;
        irs::mlog() << " hold regs LOBYTE" << "  byte = " << (int)byte << endl;
      } else {
        byte = IRS_HIBYTE(m_hold_regs_reg[(byte_index - (m_di_size_byte +
          m_coils_size_byte))/2]);
        irs::mlog() << " hold regs HIBYTE" << "  byte = " << (int)byte << endl;
      }
      byte &= static_cast<irs_u8>(~mask);
      byte |= static_cast<irs_u8>(mask);
      if((byte_index - (m_di_size_byte + m_coils_size_byte))%2 == 0) {
        IRS_LOBYTE(m_hold_regs_reg[(byte_index - (m_di_size_byte +
          m_coils_size_byte))/2]) = byte;
      } else {
        IRS_HIBYTE(m_hold_regs_reg[(byte_index - (m_di_size_byte +
          m_coils_size_byte))/2]) = byte;
      }*/
    }
    if((m_ir_size_byte != 0)||(m_ir_start_byte != 0))
    {
      irs::mlog() << " set_bit: input regs" << endl;
      byte = m_input_regs_reg[m_ir_start_byte/2u];
      byte |= (1<<bit_index);
      m_input_regs_reg[m_ir_start_byte/2u] = IRS_LOBYTE(byte);
    }
  }
}

void irs::modbus_server_t::clear_bit(irs_uarc byte_index, irs_uarc bit_index)
{
  IRS_MBUS_DBG_MSG_DETAIL("clear_bit..................");
  irs_u16 byte = 0;
  m_di_size_byte = 0, m_di_start_byte = 0;
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  m_hr_size_byte = 0, m_hr_start_byte = 0;
  m_ir_size_byte = 0, m_ir_start_byte = 0;
  range(byte_index, 1, 0, m_di_end_byte, &m_di_size_byte, &m_di_start_byte);
  range(byte_index, 1, m_di_end_byte, m_coils_end_byte,
    &m_coils_size_byte, &m_coils_start_byte);
  range(byte_index, 1, m_coils_end_byte, m_hr_end_byte, 
    &m_hr_size_byte, &m_hr_start_byte);
  range(byte_index, 1, m_hr_end_byte, m_ir_end_byte, 
    &m_ir_size_byte, &m_ir_start_byte);
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    if((m_coils_size_byte == 0)&&(m_di_size_byte == 0)&&
      (m_hr_size_byte == 0)&&(m_ir_size_byte == 0))
      IRS_MBUS_DBG_MSG_DETAIL("\n****************Illegal index or bufer size"
        "***************\n\n");
  #endif
  if (bit_index < 8) {
    if((m_di_size_byte != 0)||(m_di_start_byte != 0))
    {
      m_discr_inputs_byte[m_di_start_byte + bit_index] = irs_false;
    }
    if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
    {
      m_coils_byte[m_coils_start_byte + bit_index] = irs_false;
    }
    if((m_hr_size_byte != 0)||(m_hr_start_byte != 0))
    {
      byte = m_hold_regs_reg[m_hr_start_byte/2];
      byte&=irs_u16(~(1<<bit_index));
      m_hold_regs_reg[m_hr_start_byte/2] = byte;
    }
    if((m_ir_size_byte != 0)||(m_ir_start_byte != 0))
    {
      byte = m_input_regs_reg[m_ir_start_byte/2];
      byte&=irs_u16(~(1<<bit_index));
      m_input_regs_reg[m_ir_start_byte/2] = byte;
    }
  }
}

void irs::modbus_server_t::Error_response(irs_u8 error_type)
{
  IRS_MBUS_DBG_MSG_DETAIL("Error_response..................");
  request_exception_t &request_exception = 
    reinterpret_cast<request_exception_t&>(*(mp_buf.data() + size_of_MBAP));
  response_exception_t &response_error = 
    reinterpret_cast<response_exception_t&>(*(mp_buf.data() + size_of_MBAP + 
    size_of_resp_header));
  response_error.error_code = request_exception.function_code +
    irs_u8(0x80);
  response_error.exeption_code = error_type;
  bit_copy(&response_error.error_code, mp_buf.data(), 
    response_error.error_code, size_of_MBAP, sizeof(response_error));
}

void irs::modbus_server_t::status()
{
  irs::mlog() << "*******************Discret Inputs"
    "*********************" << endl;
  for(irs_u16 i = 0; i < m_di_size_bit/8; i++)
  {
    irs::mlog() << m_discr_inputs_byte[i] << endl;
  }
  irs::mlog() << "******************COILS"
    "********************************" << endl;
  for(irs_u16 i = 0; i< m_coils_size_bit/8; i++)
  {
    irs::mlog() << m_coils_byte[i] << endl;
  }
  irs::mlog() << "**************Holding registers"
    "***********************" << endl;
  for(irs_u16 i = 0; i< irs_u16(m_hr_size_reg/2);i++)
  {
    irs::mlog() << (int)m_hold_regs_reg[i] << endl;
  }
  irs::mlog() << "**************Input registers"
    "*************************" << endl;
  for(irs_u16 i = 0; i< irs_u16(m_ir_size_reg/2);i++)
  {
    irs::mlog() << (int)m_input_regs_reg[i] << endl;
  }
  irs::mlog() << "**************************"
    "****************************" << endl;     
}

void irs::modbus_server_t::tick()
{
  mp_tcp_server->tick();
  m_fixed_flow.tick();
  if((m_fixed_flow.write_status() == 
    irs::hardflow::fixed_flow_t::status_error) ||
    (m_fixed_flow.read_status() == 
    irs::hardflow::fixed_flow_t::status_error))
  {
    m_channel = mp_tcp_server->channel_next();
    IRS_MBUS_DBG_MSG_BASE(" ********************* abort *********************");
    m_mode = read_header_mode;
  } 
  
  switch(m_mode)
  {
    // 
    case read_header_mode: 
    {
      if((m_fixed_flow.write_status() == 
        irs::hardflow::fixed_flow_t::status_success))
      {
        convert(mp_buf.data(), 0, size_of_MBAP - 1);
        MBAP_header_t& header = 
          reinterpret_cast<MBAP_header_t&>(*mp_buf.data());
        if(header.length == 0) {
          m_fixed_flow.read(m_channel, mp_buf.data(), size_of_MBAP);
          m_mode = read_request_mode;
        } else {
          m_mode = read_request_mode;
        }
      }
    }
    break;
    //    (    )
    case read_request_mode:
    {
      if(m_fixed_flow.read_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      {
        convert(mp_buf.data(), 0, size_of_MBAP - 1);
        MBAP_header_t& header = 
          reinterpret_cast<MBAP_header_t&>(*mp_buf.data());
        if(header.length != 0) {
          m_fixed_flow.read(m_channel, mp_buf.data() + size_of_MBAP,
            header.length);
          m_mode = read_end_mode;
        } else {
          m_mode = read_header_mode;
        }
      }
    }
    break;
    case read_end_mode:
    {
      if(m_fixed_flow.read_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      {
        IRS_MBUS_DBG_MSG_BASE(" read_end_mode")
        request_exception_t &convert_pack_for_read =
          reinterpret_cast<request_exception_t&>(*(mp_buf.data() + 
          size_of_MBAP));
        switch(convert_pack_for_read.function_code)
        {
          case read_discrete_inputs:
          {
            convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case read_coils:
          {
            convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case read_hold_registers:
          {
            convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case read_input_registers:
          {
            convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case write_single_coil:
          {
            convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case write_single_register:
          {
            convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case read_exception_status:
          {
            //  
          } break;
          case W_SevHR:
          {
            
          } break;
          case write_multiple_coils:
          {
            convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case write_multiple_registers:
          {
            convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
            request_multiple_write_regs_t &reg_convert = 
              reinterpret_cast<request_multiple_write_regs_t&>(
              *(mp_buf.data() + size_of_MBAP));
            convert(mp_buf.data(), size_of_MBAP + size_of_req_multi_write,
              reg_convert.byte_count);
          } break;
          case Rsize:
          {
            
          } break;
          case Rtab:
          {
            
          } break;
          default:
          {
            
          } break;
        }
        IRS_MBUS_DBG_MSG_DETAIL("\n recieved packet");
        IRS_MBUS_DBG_MONITOR(modbus_pack_request_monitor(mp_buf.data()););
        MBAP_header_t &header = 
          reinterpret_cast<MBAP_header_t&>(*mp_buf.data());
        request_t &req_header = 
          reinterpret_cast<request_t&>(*(mp_buf.data() + size_of_MBAP));
        m_num_of_elem = req_header.quantity;
        irs_u16 start_addr = req_header.starting_address;
        m_size_byte_end = header.length - 1;
        switch(req_header.function_code)
        {
          case read_discrete_inputs:
          {
            if(m_num_of_elem != 0)
            {
              response_read_byte_t &read_di = 
                reinterpret_cast<response_read_byte_t&>(*(mp_buf.data() +
                size_of_MBAP));
              if(m_num_of_elem%8 == 0) {
                read_di.byte_count = m_num_of_elem/8;
              } else {
                read_di.byte_count = m_num_of_elem/8 + 1;
              }
              header.length = irs_u16(1 + size_of_resp_header + 
                read_di.byte_count);
              m_size_byte_end = header.length - 1;
              bit_copy(m_discr_inputs_byte.data(), read_di.value,
                start_addr, 0, read_di.byte_count*8);
              #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
              for(int di_idx = start_addr; di_idx < read_di.byte_count;
                di_idx++)
              {
                irs::mlog() << " m_discr_inputs_byte[" << di_idx << 
                  "] = " << int(m_discr_inputs_byte[di_idx]) << endl;
              }
              #endif //IRS_MBUS_MSG_DETAIL
            } else {
              Error_response(ILLEGAL_DATA_ADDRESS);
            }
          }
          break;
          case read_coils:
          {
            if(m_num_of_elem != 0) {
              response_read_byte_t &read_coils = 
                reinterpret_cast<response_read_byte_t&>(*(mp_buf.data() + 
                size_of_MBAP));
              if(req_header.quantity%8 == 0) {
                read_coils.byte_count = m_num_of_elem/8;
              } else {
                read_coils.byte_count = m_num_of_elem/8 + 1;
              }
              header.length = irs_u16(1 + size_of_resp_header + 
                read_coils.byte_count);
              bit_copy(m_coils_byte.data(), read_coils.value, start_addr, 0,
                read_coils.byte_count*8);
              #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
              for(int coils_idx = start_addr; coils_idx < read_coils.byte_count;
                coils_idx++)
              {
                irs::mlog() << " read_coils.value[" << coils_idx << 
                  "] = " << int(read_coils.value[coils_idx]) << endl;
              }
              #endif //IRS_MBUS_MSG_DETAIL
              m_size_byte_end = header.length - 1;
            } else {
              Error_response(ILLEGAL_DATA_ADDRESS);
            }
          }   
          break;
          case read_hold_registers:
          {
            if(m_num_of_elem != 0)
            {
              response_read_reg_t &read_hr = 
                reinterpret_cast<response_read_reg_t&>(
                *(mp_buf.data() + size_of_MBAP));
              read_hr.byte_count = m_num_of_elem*2;
              get_bytes(m_hold_regs_reg, start_addr, read_hr.byte_count,
                reinterpret_cast<irs_u8*>(read_hr.value), 0);
              #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
              for(int hr_idx = start_addr; hr_idx < m_num_of_elem;
                hr_idx++)
              {
                irs::mlog() << " hold regs lo[" << hr_idx << 
                  "] = " << int(IRS_LOBYTE(read_hr.value[hr_idx])) << endl;
                irs::mlog() << " hold regs hi[" << hr_idx << 
                  "] = " << int(IRS_HIBYTE(read_hr.value[hr_idx])) << endl;
              }
              #endif //IRS_MBUS_MSG_DETAIL
              header.length = irs_u16(1 + size_of_resp_header + 
                m_num_of_elem*2);
              m_size_byte_end = header.length - 1;
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case read_input_registers:
          {
            if(m_num_of_elem != 0)
            {
              response_read_reg_t &read_ir = 
                reinterpret_cast<response_read_reg_t&>(*(mp_buf.data() + 
                size_of_MBAP));
              read_ir.byte_count = m_num_of_elem*2;
              get_bytes(m_input_regs_reg, start_addr, read_ir.byte_count,
                reinterpret_cast<irs_u8*>(read_ir.value), 0);
              #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
              for(int ir_idx = start_addr; ir_idx < m_num_of_elem;
                ir_idx++)
              {
                irs::mlog() << " input regs lo[" << ir_idx << 
                  "] = " << int(IRS_LOBYTE(read_ir.value[ir_idx])) << endl;
                irs::mlog() << " input regs hi[" << ir_idx << 
                  "] = " << int(IRS_HIBYTE(read_ir.value[ir_idx])) << endl;
              }
              #endif //IRS_MBUS_MSG_DETAIL
              header.length = irs_u16(1 + size_of_resp_header + 
                m_num_of_elem*2);
              m_size_byte_end = header.length - 1;
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case write_single_coil:
          {
            if(m_num_of_elem != 0)
            {
              response_single_write_t &response_write_coil = 
                reinterpret_cast<response_single_write_t&>(*(mp_buf.data() + 
                size_of_MBAP));
              m_coils_byte[IRS_LOBYTE(response_write_coil.address)] =
                IRS_HIBYTE(response_write_coil.value);
            } 
            else 
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case write_multiple_coils:
          {
            if(m_num_of_elem != 0)
            {
              request_multiple_write_byte_t &write_multi_coils =
                reinterpret_cast<request_multiple_write_byte_t&>(
                *(mp_buf.data() + size_of_MBAP));
              bit_copy(reinterpret_cast<irs_u8*>(write_multi_coils.value), 
                m_coils_byte.data(), 0, 
                irs_u8(write_multi_coils.starting_address), 
                irs_u8(write_multi_coils.quantity));
              header.length = irs_u16(1 + 1 + size_of_read_header);
              m_size_byte_end = header.length - 1;
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case write_single_register:
          {
            if(m_num_of_elem != 0)
            {
              response_single_write_t &response_write_reg = 
                reinterpret_cast<response_single_write_t&>(*(mp_buf.data() + 
                size_of_MBAP));
              m_hold_regs_reg[response_write_reg.address] =
                response_write_reg.value;
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case write_multiple_registers:
          {
            if(m_num_of_elem != 0)
            {
              request_multiple_write_regs_t &write_multi_regs =
                reinterpret_cast<request_multiple_write_regs_t&>(
                *(mp_buf.data() + size_of_MBAP));
              set_bytes(m_hold_regs_reg, write_multi_regs.starting_address, 
                write_multi_regs.quantity,
                reinterpret_cast<irs_u8*>(write_multi_regs.value), 0);
              header.length = irs_u16(1 + 1 + size_of_read_header);
              m_size_byte_end = header.length - 1;
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case read_exception_status:
          {
            IRS_MBUS_DBG_MSG_BASE("\n read_exception_status");
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
        IRS_MBUS_DBG_MSG_BASE("send_response_mode");
        m_mode = wait_mode;
      }
    }
    break;
    case send_response_mode:
    {
      if(m_fixed_flow.read_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      {
        IRS_MBUS_DBG_MSG_DETAIL("\n send packet");
        IRS_MBUS_DBG_MONITOR(modbus_pack_response_monitor(mp_buf.data()););
        convert(mp_buf.data(), 0, size_of_MBAP - 1);
        request_exception_t &convert_pack_for_write =
          reinterpret_cast<request_exception_t&>(*(mp_buf.data() + 
          size_of_MBAP));
        switch(convert_pack_for_write.function_code)
        {
          case read_discrete_inputs:
          {
            //  
          } break;
          case read_coils:
          {
            //  
          } break;
          case read_hold_registers:
          {
            response_read_reg_t &read_reg_convert = 
              reinterpret_cast<response_read_reg_t&>(
              *(mp_buf.data() + size_of_MBAP));
            convert(mp_buf.data(), size_of_MBAP + size_of_resp_header,
              sizeof(read_reg_convert) - 2);
          } break;
          case read_input_registers:
          {
            response_read_reg_t &read_reg_convert = 
              reinterpret_cast<response_read_reg_t&>(
              *(mp_buf.data() + size_of_MBAP));
            convert(mp_buf.data(), size_of_MBAP + size_of_resp_header,
              sizeof(read_reg_convert) - 2);
          } break;
          case write_single_coil:
          {
            convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case write_single_register:
          {
            convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case read_exception_status:
          {
            //  
          } break;
          case W_SevHR:
          {
            
          } break;
          case write_multiple_coils:
          {
            convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case write_multiple_registers:
          {
            convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case Rsize:
          {
            
          } break;
          case Rtab:
          {
            
          } break;
          default:
          {
            
          } break;
        }
        m_fixed_flow.write(m_channel, mp_buf.data(), size_of_MBAP + 
          m_size_byte_end);
        m_mode = write_end;
      }
    }
    break;
    case write_end:
    {
      if(m_fixed_flow.write_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      {
        memset(mp_buf.data(), 0, mp_buf.size());
        m_mode = read_header_mode;        
      }
    }
    break;
    case wait_mode:
    {
      if(test_to_cnt(wait))
      { 
        m_mode = send_response_mode;
        m_channel = mp_tcp_server->channel_next();
      }
    }
  }
}

irs::modbus_client_t::modbus_client_t(
  hardflow_t* ap_hardflow,
  size_t a_discr_inputs_size_byte,
  size_t a_coils_size_byte,
  size_t a_hold_regs_reg,
  size_t a_input_regs_reg
):
  m_del_time(TIME_TO_CNT(1, 1)),
  mp_buf(IRS_NULL),
  m_global_index(0),
  m_ibank(0),
  m_coils_size_bit(size_t(a_coils_size_byte*8)),
  m_di_size_bit(size_t(a_discr_inputs_size_byte*8)),
  m_hr_size_reg(a_hold_regs_reg*2),
  m_ir_size_reg(a_input_regs_reg*2),
  m_di_end_byte(a_discr_inputs_size_byte),
  m_coils_end_byte(a_discr_inputs_size_byte + a_coils_size_byte),
  m_hr_end_byte(a_discr_inputs_size_byte + a_coils_size_byte + 
    a_hold_regs_reg*2),
  m_ir_end_byte(a_discr_inputs_size_byte + a_coils_size_byte +
    a_hold_regs_reg*2 + a_input_regs_reg*2),
  m_size_byte_end(0),
  m_nothing(irs_true),
  m_read_table(irs_false),
  m_first_read(irs_true),
  m_part_send(irs_false),
  m_channel(1),
  m_start_block(0),
  m_search_index(0),
  m_bytes(0),
  m_di_size_byte(0),
  m_di_start_byte(0),
  m_coils_size_byte(0),
  m_coils_start_byte(0),
  m_hr_size_byte(0),
  m_hr_start_byte(0),
  m_ir_size_byte(0),
  m_ir_start_byte(0),
  m_rforwr(a_coils_size_byte*8 + a_hold_regs_reg*2),
  m_discr_inputs_byte_r(a_discr_inputs_size_byte),
  m_coils_byte_r(a_coils_size_byte),
  m_coils_byte_w(a_coils_size_byte),
  m_input_regs_reg_r(a_input_regs_reg),
  m_hold_regs_reg_r(a_hold_regs_reg),
  m_hold_regs_reg_w(a_hold_regs_reg),
  m_command(read_discrete_inputs),
  request_type(request_start),
  m_mode(make_request_mode),
  m_loop_timer(make_cnt_ms(200)),
  mp_tcp_client(ap_hardflow),
  m_fixed_flow(mp_tcp_client),
  m_num_of_elem(0),
  m_start_addr(0)
{
  memsetex(m_spacket,IRS_ARRAYSIZE(m_spacket));
  memsetex(m_rpacket,IRS_ARRAYSIZE(m_rpacket));
  init_to_cnt();
}

irs::modbus_client_t::~modbus_client_t()
{
  deinit_to_cnt();
}

irs_uarc irs::modbus_client_t::size()
{
  return m_ir_end_byte;
}

void irs::modbus_client_t::status()
{
}

irs_bool irs::modbus_client_t::connected()
{
  if(m_first_read)
    return irs_false;
  else
    return irs_true;
}

irs_bool irs::modbus_client_t::connected_1()
{
  return irs_true;
}

#if (IRS_MBUS_MSG_TYPE ==  IRS_MBUS_MSG_DETAIL)
void irs::modbus_client_t::modbus_pack_request_monitor(irs_u8 *ap_buf)
{
  MBAP_header_t &header = 
    reinterpret_cast<MBAP_header_t&>(*ap_buf);
  request_exception_t &req_header = 
    reinterpret_cast<request_exception_t&>(*(ap_buf + size_of_MBAP));
    
  irs::mlog() << "\n ----------- MODBUS's Header----------------\n";
  irs::mlog() << " transaction_id_lo .... " <<
    int(IRS_LOBYTE(header.transaction_id)) << endl;
  irs::mlog() << " transaction_id_hi .... " <<
    int(IRS_HIBYTE(header.transaction_id)) << endl;
  irs::mlog() << " proto_id_lo .......... " <<
    int(IRS_LOBYTE(header.proto_id)) << endl;
  irs::mlog() << " proto_id_hi .......... " <<
    int(IRS_HIBYTE(header.proto_id)) << endl;
  irs::mlog() << " length_lo ............ " <<
    int(IRS_LOBYTE(header.length)) << endl;
  irs::mlog() << " length_hi ............ " <<
    int(IRS_HIBYTE(header.length)) << endl;
  irs::mlog() << " unit_identifier ...... " <<
    int(header.unit_identifier) << endl;
    
  irs::mlog() << " function_code ........ " <<
    int(req_header.function_code);
  switch(req_header.function_code)
  {
    case read_discrete_inputs:
    {
      request_t &req_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read discret inputs" << endl;
      irs::mlog() << " starting_address_lo .. " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi .. " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " quantity_lo .......... " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " quantity_hi .......... " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
    } break;
    case read_coils:
    {
      request_t &req_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read coils" << endl;
      irs::mlog() << " starting_address_lo .. " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi .. " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " quantity_lo .......... " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " quantity_hi .......... " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
    } break;
    case read_hold_registers:
    {
      request_t &req_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read hold registers" << endl;
      irs::mlog() << " starting_address_lo .. " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi .. " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " quantity_lo .......... " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " quantity_hi .......... " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
    } break;
    case read_input_registers:
    {
      request_t &req_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read input registers" << endl;
      irs::mlog() << " starting_address_lo .. " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi .. " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " quantity_lo .......... " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " quantity_hi .......... " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
    } break;
    case write_single_coil:
    {
      request_t &req_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " write single coil" << endl;
      irs::mlog() << " output_address_lo .... " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " output_address_hi .... " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " output_value_lo ...... " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " output_value_hi ...... " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
    } break;
    case write_single_register:
    {
      request_t &req_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " write hold register" << endl;
      irs::mlog() << " register_address_lo .. " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " register_address_hi .. " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " register_value_lo .... " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " register_value_hi .... " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
    } break;
    case read_exception_status:
    {
      irs::mlog() << " read exception status " << endl;
    } break;
    case W_SevHR:
    {
      irs::mlog() << " W_SevHR" << endl;
    } break;
    case write_multiple_coils:
    {
      request_multiple_write_byte_t &req_header = 
        reinterpret_cast<request_multiple_write_byte_t&>(
        *(ap_buf + size_of_MBAP));
      irs::mlog() << " write multiple coils" << endl;
      irs::mlog() << " starting_address_lo ..... " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi ..... " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " quantity_of_outputs_lo .. " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " quantity_of_outputs_hi .. " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
      irs::mlog() << " byte_count .............. " <<
        int(req_header.byte_count) << endl;
      int coils_idx_max = 0;
      if(req_header.quantity%8 == 0)
        coils_idx_max = req_header.quantity/8;
      else
        coils_idx_max = req_header.quantity/8 + 1;
      for(int coils_idx = 0; coils_idx < coils_idx_max; coils_idx++) {
        irs::mlog() << " coils_value ........... " <<
          int(req_header.value[coils_idx]) << endl;
      }
    } break;
    case write_multiple_registers:
    {
      request_multiple_write_regs_t &req_header = 
        reinterpret_cast<request_multiple_write_regs_t&>(
        *(ap_buf + size_of_MBAP));
      irs::mlog() << " write multiple coils" << endl;
      irs::mlog() << " starting_address_lo ....... " <<
        int(IRS_LOBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi ....... " <<
        int(IRS_HIBYTE(req_header.starting_address)) << endl;
      irs::mlog() << " quantity_of_registers_lo .. " <<
        int(IRS_LOBYTE(req_header.quantity)) << endl;
      irs::mlog() << " quantity_of_registers_hi .. " <<
        int(IRS_HIBYTE(req_header.quantity)) << endl;
      irs::mlog() << " byte_count ................ " <<
        int(req_header.byte_count) << endl;
      for(int reg_idx = 0; reg_idx < int(req_header.quantity)/8; reg_idx++) {
        irs::mlog() << " register_value_lo ......... " <<
          int(IRS_LOBYTE(req_header.value[reg_idx])) << endl;
        irs::mlog() << " register_value_hi ......... " <<
          int(IRS_HIBYTE(req_header.value[reg_idx])) << endl;
      }
    } break;
    case Rsize:
    {
      irs::mlog() << " Rsize" << endl;
    } break;
    case Rtab:
    {
      irs::mlog() << " Rtab" << endl;
    } break;
    default:
    {
      irs::mlog() << " ILLEGAL_FUNCTION" << endl;
    } break;
  }
}
#endif // IRS_MBUS_MSG_DETAIL

#if (IRS_MBUS_MSG_TYPE ==  IRS_MBUS_MSG_DETAIL)
void irs::modbus_client_t::modbus_pack_response_monitor(irs_u8 *ap_buf)
{
  MBAP_header_t &header = 
    reinterpret_cast<MBAP_header_t&>(*ap_buf);
  request_exception_t &resp_header = 
    reinterpret_cast<request_exception_t&>(*(ap_buf + size_of_MBAP));
    
  irs::mlog() << "\n ----------- MODBUS's Header----------------\n";
  irs::mlog() << " transaction_id_lo .... " <<
    int(IRS_LOBYTE(header.transaction_id)) << endl;
  irs::mlog() << " transaction_id_hi .... " <<
    int(IRS_HIBYTE(header.transaction_id)) << endl;
  irs::mlog() << " proto_id_lo .......... " <<
    int(IRS_LOBYTE(header.proto_id)) << endl;
  irs::mlog() << " proto_id_hi .......... " <<
    int(IRS_HIBYTE(header.proto_id)) << endl;
  irs::mlog() << " length_lo ............ " <<
    int(IRS_LOBYTE(header.length)) << endl;
  irs::mlog() << " length_hi ............ " <<
    int(IRS_HIBYTE(header.length)) << endl;
  irs::mlog() << " unit_identifier ...... " <<
    int(header.unit_identifier) << endl;

  irs::mlog() << " function_code ........ " <<
    int(resp_header.function_code);
  switch(resp_header.function_code)
  {
    case read_discrete_inputs:
    {
      response_read_byte_t &resp_header = 
        reinterpret_cast<response_read_byte_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read discret inputs" << endl;
      irs::mlog() << " byte_count ........... " <<
        int(resp_header.byte_count) << endl;
      for(int di_idx = 0; di_idx < resp_header.byte_count; di_idx++) {
        irs::mlog() << " discret input[" << di_idx << "] ..... " <<
          int(resp_header.value[di_idx]) << endl;
      }
    } break;
    case read_coils:
    {
      response_read_byte_t &resp_header = 
        reinterpret_cast<response_read_byte_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read coils" << endl;
      irs::mlog() << " byte_count ........... " <<
        int(resp_header.byte_count) << endl;
      for(int coils_idx = 0; coils_idx < resp_header.byte_count; coils_idx++) {
        irs::mlog() << " coils[" << coils_idx << "] ............. " <<
          int(resp_header.value[coils_idx]) << endl;
      }
    } break;
    case read_hold_registers:
    {
      response_read_reg_t &resp_header = 
        reinterpret_cast<response_read_reg_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read hold registers" << endl;
      irs::mlog() << " byte_count ........... " <<
        int(resp_header.byte_count) << endl;
      for(int h_reg_idx = 0; h_reg_idx < int(resp_header.byte_count)/2;
        h_reg_idx++) 
      {
        irs::mlog() << " hold regs lo[" << h_reg_idx << "] ...... " <<
          int(IRS_LOBYTE(resp_header.value[h_reg_idx])) << endl;
        irs::mlog() << " hold regs hi[" << h_reg_idx << "] ...... " <<
          int(IRS_HIBYTE(resp_header.value[h_reg_idx])) << endl;
      }
    } break;
    case read_input_registers:
    {
      response_read_reg_t &resp_header = 
        reinterpret_cast<response_read_reg_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read input registers" << endl;
      irs::mlog() << " byte_count ........... " <<
        int(resp_header.byte_count) << endl;
      for(int i_reg_idx = 0; i_reg_idx < int(resp_header.byte_count)/2;
        i_reg_idx++) 
      {
        irs::mlog() << " input regs lo[" << i_reg_idx << "] ..... " <<
          int(IRS_LOBYTE(resp_header.value[i_reg_idx])) << endl;
        irs::mlog() << " input regs hi[" << i_reg_idx << "] ..... " <<
          int(IRS_HIBYTE(resp_header.value[i_reg_idx])) << endl;
      }
    } break;
    case write_single_coil:
    {
      response_single_write_t &resp_header = 
        reinterpret_cast<response_single_write_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " write single coil" << endl;
      irs::mlog() << " output_address_lo .... " <<
        int(IRS_LOBYTE(resp_header.address)) << endl;
      irs::mlog() << " output_address_hi .... " <<
        int(IRS_HIBYTE(resp_header.address)) << endl;
      irs::mlog() << " output_value_lo ...... " <<
        int(IRS_LOBYTE(resp_header.value)) << endl;
      irs::mlog() << " output_value_hi ...... " <<
        int(IRS_HIBYTE(resp_header.value)) << endl;
    } break;
    case write_single_register:
    {
      response_single_write_t &resp_header = 
        reinterpret_cast<response_single_write_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " write single coil" << endl;
      irs::mlog() << " output_address_lo .... " <<
        int(IRS_LOBYTE(resp_header.address)) << endl;
      irs::mlog() << " output_address_hi .... " <<
        int(IRS_HIBYTE(resp_header.address)) << endl;
      irs::mlog() << " output_value_lo ...... " <<
        int(IRS_LOBYTE(resp_header.value)) << endl;
      irs::mlog() << " output_value_hi ...... " <<
        int(IRS_HIBYTE(resp_header.value)) << endl;
    } break;
    case read_exception_status:
    {
      irs::mlog() << " read exception status " << endl;
      response_exception_t &resp_header = 
        reinterpret_cast<response_exception_t&>(
        *(ap_buf + size_of_MBAP));
      irs::mlog() << " error_code ........... " <<
        int(resp_header.error_code) << endl;
      irs::mlog() << " exeption_code ........ " <<
        int(resp_header.exeption_code) << endl;
    } break;
    case W_SevHR:
    {
      irs::mlog() << " W_SevHR" << endl;
    } break;
    case write_multiple_coils:
    {
      request_t &resp_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read discret inputs" << endl;
      irs::mlog() << " starting_address_lo .. " <<
        int(IRS_LOBYTE(resp_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi .. " <<
        int(IRS_HIBYTE(resp_header.starting_address)) << endl;
      irs::mlog() << " quantity_lo .......... " <<
        int(IRS_LOBYTE(resp_header.quantity)) << endl;
      irs::mlog() << " quantity_hi .......... " <<
        int(IRS_HIBYTE(resp_header.quantity)) << endl;
    } break;
    case write_multiple_registers:
    {
      request_t &resp_header = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      irs::mlog() << " read discret inputs" << endl;
      irs::mlog() << " starting_address_lo .. " <<
        int(IRS_LOBYTE(resp_header.starting_address)) << endl;
      irs::mlog() << " starting_address_hi .. " <<
        int(IRS_HIBYTE(resp_header.starting_address)) << endl;
      irs::mlog() << " quantity_lo .......... " <<
        int(IRS_LOBYTE(resp_header.quantity)) << endl;
      irs::mlog() << " quantity_hi .......... " <<
        int(IRS_HIBYTE(resp_header.quantity)) << endl;
    } break;
    case Rsize:
    {
      irs::mlog() << " Rsize" << endl;
    } break;
    case Rtab:
    {
      irs::mlog() << " Rtab" << endl;
    } break;
    default:
    {
      irs::mlog() << " ILLEGAL_FUNCTION" << endl;
    } break;
  }
}
#endif // IRS_MBUS_MSG_DETAIL

void irs::modbus_client_t::read(irs_u8 *ap_buf, irs_uarc index, irs_uarc a_size)
{
  IRS_MBUS_DBG_MSG_DETAIL("read");
  m_di_size_byte = 0; m_di_start_byte = 0;
  m_coils_size_byte = 0; m_coils_start_byte = 0;
  m_hr_size_byte = 0; m_hr_start_byte = 0;
  m_ir_size_byte = 0; m_ir_start_byte = 0;
  
  range(index, a_size, 0, m_di_end_byte, &m_di_size_byte, &m_di_start_byte);
  range(index, a_size, m_di_end_byte, m_coils_end_byte, &m_coils_size_byte,
    &m_coils_start_byte);
  range(index, a_size, m_coils_end_byte, m_hr_end_byte, &m_hr_size_byte,
    &m_hr_start_byte);
  range(index, a_size, m_hr_end_byte, m_ir_end_byte, &m_ir_size_byte,
    &m_ir_start_byte);
  
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
  irs::mlog() << " m_di_size_byte = " << m_di_size_byte << endl;
  irs::mlog() << " m_di_start_byte = " << m_di_start_byte << endl;
  irs::mlog() << " m_coils_size_byte = " << m_coils_size_byte << endl;
  irs::mlog() << " m_coils_start_byte = " << m_coils_start_byte << endl;
  irs::mlog() << " m_hr_size_byte = " << m_hr_size_byte << endl;
  irs::mlog() << " m_hr_start_byte = " << m_hr_start_byte << endl;
  irs::mlog() << " m_ir_size_byte = " << m_ir_size_byte << endl;
  irs::mlog() << " m_ir_start_byte = " << m_ir_start_byte << endl;
  #endif // IRS_MBUS_MSG_DETAIL
  int idx = 0;
  if((m_di_size_byte != 0)||(m_di_start_byte != 0))
  {
    irs::mlog() << "-------------------------------"
      "-------------------------" << endl;
    for(int idx = 0; idx < (int)m_di_size_byte; idx++)
    {
      ap_buf[idx] = m_discr_inputs_byte_r[idx];
      #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
      irs::mlog() << "discret inputs[" << idx << "] = " <<
        (int)m_discr_inputs_byte_r[idx] << endl;
      #endif // IRS_MBUS_MSG_DETAIL
    }
  }
  if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
  {
    for(; idx < int(m_di_size_byte + m_coils_size_byte); idx++)
    {
      ap_buf[idx] = m_coils_byte_r[idx - m_di_size_byte];
      #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
      irs::mlog() << "coils[" << idx << "] = " <<
        (int)m_coils_byte_r[idx] << endl;
      #endif // IRS_MBUS_MSG_DETAIL
    }
  }
  if((m_hr_size_byte != 0)||(m_hr_start_byte != 0))
  {
    get_bytes(m_hold_regs_reg_r, 0, m_hr_size_byte, ap_buf, 
      m_di_size_byte + m_coils_size_byte);
    #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    for(int hr_idx = 0; hr_idx < m_hr_size_byte; hr_idx++)
    {
      irs::mlog() << "hold regs[" << hr_idx << "] = " <<
        (int)m_hold_regs_reg_r[hr_idx] << endl; 
    }
    irs::mlog() << "size of hr = " << m_hold_regs_reg_r.size() << endl;
    #endif // IRS_MBUS_MSG_DETAIL
  }
  if((m_ir_size_byte != 0)||(m_ir_start_byte != 0))
  {
    get_bytes(m_input_regs_reg_r, m_ir_start_byte, m_ir_size_byte, ap_buf,
      m_di_size_byte + m_coils_size_byte + m_hr_size_byte);
    #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    for(int ir_idx = 0; ir_idx < m_ir_size_byte; ir_idx++)
      irs::mlog() << "input regs[" << ir_idx << "] = " <<
        (int)m_input_regs_reg_r[ir_idx] << endl;  
    #endif // IRS_MBUS_MSG_DETAIL
  }
}
void irs::modbus_client_t::write(const irs_u8 *ap_buf, irs_uarc a_index, 
  irs_uarc a_size)
{
  IRS_MBUS_DBG_MSG_DETAIL("write");
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  m_hr_size_byte = 0, m_hr_start_byte = 0;
  
  range(a_index, a_size, m_di_end_byte, m_coils_end_byte, &m_coils_size_byte,
    &m_coils_start_byte);
  range(a_index, a_size, m_coils_end_byte, m_hr_end_byte, &m_hr_size_byte,
    &m_hr_start_byte);
    
  if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
  {
    bit_copy(ap_buf, m_coils_byte_w.data(), m_coils_start_byte*8, 0,
      m_coils_size_byte*8);
    #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    for(int coils_idx = 0; coils_idx < m_coils_size_byte; coils_idx++)
      irs::mlog() << "coils[" << coils_idx << "] = " <<
        (int)m_coils_byte_w[coils_idx] << endl;  
    #endif // IRS_MBUS_MSG_DETAIL
    
    for(irs_u32 i = m_coils_start_byte; i < m_coils_end_byte; i++) {
      m_rforwr[i] = 1;
    }
  }
  if((m_hr_size_byte != 0)||(m_hr_start_byte != 0))
  {
    set_bytes(m_hold_regs_reg_w, 0, m_hr_size_byte, ap_buf,
      m_hr_start_byte);
    IRS_LIB_ASSERT(m_hold_regs_reg_w.size() <= m_hr_size_byte);
    #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    for(int hr_idx = 0; hr_idx < m_hr_size_byte; hr_idx++)
      irs::mlog() << "hold regs[" << hr_idx << "] = " <<
        (int)m_hold_regs_reg_w[hr_idx] << endl;
    #endif // IRS_MBUS_MSG_DETAIL
    for(irs_u32 i = m_hr_start_byte; i < m_hr_end_byte; i++) {
      m_rforwr[i] = 1;
    }
  }
}

irs_bool irs::modbus_client_t::bit(irs_uarc index, irs_uarc a_bit_index)
{
  IRS_MBUS_DBG_MSG_DETAIL("bit");
  m_hr_size_byte = 0; m_hr_start_byte = 0;
  m_coils_size_byte = 0; m_coils_start_byte = 0;
  m_di_start_byte = 0; m_di_size_byte = 0;
  m_ir_start_byte = 0; m_ir_size_byte = 0;
  range(index, 1, 0, m_di_end_byte, &m_di_size_byte, &m_di_start_byte);
  range(index, 1, m_di_end_byte, m_coils_end_byte, 
    &m_coils_size_byte, &m_coils_start_byte);
  range(index, 1, m_coils_end_byte, m_hr_end_byte, 
    &m_hr_size_byte, &m_hr_start_byte);
  range(index, 1, m_hr_end_byte, m_ir_end_byte, 
    &m_ir_size_byte, &m_ir_start_byte);
  irs_uarc byte = 0;
  if((m_di_size_byte != 0)||(m_di_start_byte != 0))
    return m_discr_inputs_byte_r[m_di_start_byte + a_bit_index];
  if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
    return m_coils_byte_r[m_coils_start_byte + a_bit_index];
  if (a_bit_index < 8) {
    if((m_hr_size_byte != 0)||(m_hr_start_byte != 0))
    {
      byte = m_hold_regs_reg_r[m_ir_start_byte/2];
      if(byte &= (1 << a_bit_index))
        return irs_true;
      else
        return irs_false;
    }
    if((m_ir_size_byte != 0)||(m_ir_start_byte != 0))
    {
      byte = m_input_regs_reg_r[m_hr_start_byte/2];
      if(byte &= (1 << a_bit_index))
        return irs_true;
      else
        return irs_false;
    }
  }
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
  if((m_coils_size_byte == 0) && (m_di_size_byte == 0) &&
    (m_hr_size_byte == 0) && (m_ir_size_byte == 0))
  {
    irs::mlog() << "\n***************Illegal index or bufer size"
      "******************\n";
  }
  #endif //IRS_MBUS_MSG_DETAIL
  return irs_false;
}

void irs::modbus_client_t::set_bit(irs_uarc index, irs_uarc a_bit_index)
{
  IRS_MBUS_DBG_MSG_DETAIL("set_bit");
  m_hr_size_byte = 0;
  m_hr_start_byte = 0;
  m_coils_size_byte = 0;
  m_coils_start_byte = 0;
  range(index, 1, m_di_end_byte, m_coils_end_byte,
    &m_coils_size_byte, &m_coils_start_byte);
  range(index, 1, m_hr_end_byte, m_ir_end_byte, 
    &m_hr_size_byte, &m_hr_start_byte);
  size_t byte = 0;
  if (a_bit_index < 8) {
    if((m_coils_size_byte != 0) || (m_coils_start_byte != 0))
    {
      m_coils_byte_w[m_coils_start_byte + a_bit_index] = 1;
      m_rforwr[m_coils_start_byte + a_bit_index] = 1;
    }
    if((m_hr_size_byte != 0) || (m_hr_start_byte != 0))
    {
      byte = m_hold_regs_reg_r[m_hr_start_byte/2];
      byte |= (1 << a_bit_index);
      m_hold_regs_reg_w[m_hr_start_byte/2] = irs_u16(byte);
      m_rforwr[m_hr_start_byte + m_coils_size_bit] = 1;
    }
  }
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    if((m_coils_size_byte == 0)&&(m_hr_size_byte == 0))
      irs::mlog() << "ERROR\n";
  #endif
}

void irs::modbus_client_t::clear_bit(irs_uarc index, irs_uarc a_bit_index)
{
  IRS_MBUS_DBG_MSG_DETAIL("clear_bit....................");
  m_hr_size_byte = 0;
  m_hr_start_byte = 0;
  m_coils_size_byte = 0;
  m_coils_start_byte = 0;
  range(index, 1, m_di_end_byte, m_coils_end_byte, 
    &m_coils_size_byte, &m_coils_start_byte);
  range(index, 1, m_hr_end_byte, m_ir_end_byte, 
    &m_hr_size_byte, &m_hr_start_byte);
  size_t byte = 0;
  if (a_bit_index < 8) {
    if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
    {
      m_coils_byte_w[m_coils_start_byte + a_bit_index] = 0;
      m_rforwr[m_coils_start_byte + a_bit_index] = 1;
    }
    if((m_hr_size_byte != 0)||(m_hr_start_byte != 0))
    {
      byte = m_hold_regs_reg_r[m_hr_start_byte/2];
      byte&=~(1 << a_bit_index);
      m_hold_regs_reg_w[m_hr_start_byte/2] = irs_u16(byte);
      m_rforwr[m_hr_start_byte + m_coils_size_bit] = 1;
    }
  }
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
    if((m_coils_size_byte == 0)&&(m_hr_size_byte == 0))
      irs::mlog() << "ERROR";
  #endif
}

void irs::modbus_client_t::make_packet(size_t a_index, size_t a_size)
{
  IRS_MBUS_DBG_MSG_DETAIL(" make_packet");
  MBAP_header_t &header = reinterpret_cast<MBAP_header_t&>(*m_spacket);
  m_bytes = 0;
  m_size_byte_end = 0;
  switch(m_command)
  {
    case read_discrete_inputs:
    {
      request_t &sec_header = 
        reinterpret_cast<request_t&>(*(m_spacket + size_of_MBAP));
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      header.length  = irs_u16(1 + sizeof(sec_header));
      sec_header.function_code = m_command;
    }
    break;
    case read_coils:
    {
      request_t &sec_header = 
        reinterpret_cast<request_t&>(*(m_spacket + size_of_MBAP));
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      header.length  = irs_u16(1 + sizeof(sec_header));
      sec_header.function_code = m_command;
    }
    break;
    case read_hold_registers:
    {
      request_t &sec_header = 
        reinterpret_cast<request_t&>(*(m_spacket + size_of_MBAP));
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      header.length  = irs_u16(1 + sizeof(sec_header));
      sec_header.function_code = m_command;
    }
    break;
    case read_input_registers:
    {
      request_t &sec_header = 
        reinterpret_cast<request_t&>(*(m_spacket + size_of_MBAP));
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      header.length  = irs_u16(1 + sizeof(sec_header));
      sec_header.function_code = m_command;
    }
    break;
    case read_exception_status:
    {
      request_exception_t &sec_header = 
        reinterpret_cast<request_exception_t&>(*(m_spacket + size_of_MBAP));
      header.length  = irs_u16(1 + sizeof(sec_header));
      sec_header.function_code = m_command;
    }
    break;
    case write_multiple_coils:
    {
      request_multiple_write_byte_t &sec_header = 
        reinterpret_cast<request_multiple_write_byte_t&>(*(m_spacket + 
        size_of_MBAP));
      if(a_size%8 == 0)
        m_bytes = irs_u16(a_size/8 + 1);
      else
        m_bytes = irs_u16(a_size/8 + 2);
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      sec_header.byte_count = m_bytes;
      header.length  = irs_u16(size_of_resp_multi_write +
        sec_header.byte_count);
      sec_header.function_code = m_command;
    }
    break;
    case write_multiple_registers:
    {
      request_multiple_write_regs_t &sec_header = 
        reinterpret_cast<request_multiple_write_regs_t&>(*(m_spacket +
        size_of_MBAP));
      m_bytes = irs_u16(a_size*2 + 1);
      convert(m_spacket, size_of_MBAP + size_of_read_header + 1,
        (irs_u8)m_bytes);
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      sec_header.byte_count = m_bytes;
      header.length  = irs_u16(size_of_resp_multi_write + 
        sec_header.byte_count);
      sec_header.function_code = m_command;
    }
    break;
    default : {
    }
  }
  header.transaction_id = IRS_NULL;
  header.proto_id = IRS_NULL;
  header.unit_identifier  = 1;
  m_size_byte_end = header.length;
  #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
  for(irs_u16 t = 0; t < (header.length + size_of_MBAP); t++)
    irs::mlog() << "m_spacket[" << t << "] = " << (int)(m_spacket[t]) << endl;
  #endif // IRS_MBUS_MSG_DETAIL
  IRS_MBUS_DBG_MSG_DETAIL("\nPacket ready");
}

void irs::modbus_client_t::set_delay_time(double time)
{
  m_loop_timer.set(make_cnt_s(time));
}

void irs::modbus_client_t::tick()
{
  mp_tcp_client->tick();
  m_fixed_flow.tick();
  if(connected_1()) 
  { 
    if((m_fixed_flow.write_status() == 
      irs::hardflow::fixed_flow_t::status_error) ||
      (m_fixed_flow.read_status() == 
      irs::hardflow::fixed_flow_t::status_error))
    {
      m_mode = make_request_mode;
      request_type = request_start;
      m_fixed_flow.read_abort();
      m_fixed_flow.write_abort();
    }
    
    switch(m_mode)
    {
      case make_request_mode:
      {
        switch(request_type)
        {
          case request_start:
          {
            if(m_loop_timer.check())
            {
              m_mode = make_request_mode;
              request_type = read_discrete_inputs;
            }
          }
          break;
          case read_discrete_inputs:
          {
            m_command = request_type;
            if((m_global_index + size_of_data_byte) < m_di_size_byte)
            {
              make_packet(m_global_index, size_of_data_byte);
              m_global_index += irs_u16(size_of_data_byte);
              m_ibank += size_of_data_byte;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
            else
            {
              make_packet(m_global_index,
                irs_u16(m_di_size_bit - m_global_index));
              m_ibank += (m_di_size_bit - m_global_index);
              m_global_index = 0;
              request_type = read_coils;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
          }
          break;
          case read_coils:
          {
            m_command = request_type;
            if((m_global_index + size_of_data_byte) < m_coils_size_byte)
            {
              make_packet(m_global_index, size_of_data_byte);
              m_global_index += irs_u16(size_of_data_byte);
              m_ibank += size_of_data_byte;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
            else
            {
              make_packet(m_global_index,
                irs_u16(m_coils_size_bit - m_global_index));
              m_ibank += (m_coils_size_bit - m_global_index);
              m_global_index = 0;
              request_type = read_hold_registers;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
          }
          break;
          case read_hold_registers:
          {
            m_command = request_type;
            if(irs_u32(m_global_index + size_of_data_reg*2) < 
              irs_u32(m_hr_size_reg))
            {
              make_packet(m_global_index, size_of_data_reg*2);
              m_global_index += size_of_data_reg*2;
              m_ibank += size_of_data_reg*2;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
            else
            {
              make_packet(m_global_index,
                irs_u16(m_hr_size_reg/2 - (irs_u32)m_global_index));
              m_ibank += (m_hr_size_reg/2 - m_global_index);
              m_global_index = 0;
              request_type = read_input_registers;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
          }
          break;
          case read_input_registers:
          {
            m_command = request_type;
            if(irs_u32(m_global_index + size_of_data_reg*2) <
              irs_u32(m_ir_size_reg))
            {
              make_packet(m_global_index, size_of_data_reg*2);
              m_global_index += size_of_data_reg*2;
              m_ibank += size_of_data_reg*2;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
            else
            {
              make_packet(m_global_index,
                irs_u16((m_ir_size_reg)/2 - m_global_index));
              m_ibank += (m_ir_size_reg/2 - m_global_index);
              m_global_index = 0;
              request_type = ERT;
              IRS_MBUS_DBG_MSG_BASE("wait_mode");
              m_mode = wait_mode;
            }
          }
          break;
          case ERT:
          {
            m_ibank = 0;
            m_first_read = irs_false;
            m_global_index = 0;
            m_command = read_exception_status;
            #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
            irs::mlog() << "**********************Discret Inputs"
              "*************" << endl;
            for(irs_u16 i = 0; i<  m_di_size_bit/8;i++)
            {
              irs::mlog() << m_discr_inputs_byte_r[i] << endl;
            }
            irs::mlog() << "\n*************************************"
              "**********" << endl;
            irs::mlog() << "**************COILS********************"
              "**********" << endl;
            for(irs_u16 i = 0; i < m_coils_size_bit/8; i++)
            {
              irs::mlog() << m_coils_byte_r[i] << endl;
            }
            irs::mlog() << "\n*************************************"
              "**********" << endl;
            irs::mlog() << "************Holding registers**********"
              "**********" << endl;
            for(irs_u16 i = 0; i < irs_u16(m_hr_size_reg/2); i++)
            {
              irs::mlog() << hex_u16 << (int)m_hold_regs_reg_r[i] << endl;
            }
            irs::mlog() << "\n*************************************"
              "**********" << endl;
            irs::mlog() << "**************Input registers**********"
              "**********" << endl;
            for(irs_u16 i = 0; i < irs_u16(m_ir_size_reg/2); i++)
            {
              irs::mlog() << hex_u16 << (int)m_input_regs_reg_r[i] << endl;;
            }
            irs::mlog() << "\n*************************************"
              "**********" << endl;
            #endif //IRS_MBUS_MSG_DETAIL
            make_packet(m_global_index, 0);
            request_type = request_start;
            IRS_MBUS_DBG_MSG_BASE("wait_mode");
            m_mode = wait_mode;
          }
          break;
          default : {
          }
        }
        m_read_table = irs_false;
      }
      break;
      case send_request_mode:
      {
        if(m_fixed_flow.write_status() == 
          irs::hardflow::fixed_flow_t::status_success)
        {
          IRS_MBUS_DBG_MSG_DETAIL("\n send packet");
          IRS_MBUS_DBG_MONITOR(modbus_pack_request_monitor(m_spacket););
          convert(m_spacket, 0, size_of_MBAP - 1);
          request_exception_t &convert_pack_for_write =
            reinterpret_cast<request_exception_t&>(*(m_spacket + 
            size_of_MBAP));
          switch(convert_pack_for_write.function_code)
          {
            case read_discrete_inputs:
            {
              convert(m_spacket, size_of_MBAP + 1, size_of_read_header);
            } break;
            case read_coils:
            {
              convert(m_spacket, size_of_MBAP + 1, size_of_read_header);
            } break;
            case read_hold_registers:
            {
              convert(m_spacket, size_of_MBAP + 1, size_of_read_header);
            } break;
            case read_input_registers:
            {
              convert(m_spacket, size_of_MBAP + 1, size_of_read_header);
            } break;
            case write_single_coil:
            {
              convert(m_spacket, size_of_MBAP + 1, size_of_read_header);
            } break;
            case write_single_register:
            {
              convert(m_spacket, size_of_MBAP + 1, size_of_read_header);
            } break;
            case read_exception_status:
            {
              //  
            } break;
            case W_SevHR:
            {
              
            } break;
            case write_multiple_coils:
            {
              convert(m_spacket, size_of_MBAP + 1, size_of_read_header);
            } break;
            case write_multiple_registers:
            {
              convert(m_spacket, size_of_MBAP + 1, size_of_read_header);
              request_multiple_write_regs_t &reg_convert = 
                reinterpret_cast<request_multiple_write_regs_t&>(
                *(m_spacket + size_of_MBAP));
              convert(m_spacket, size_of_MBAP + size_of_req_multi_write,
                reg_convert.byte_count);
            } break;
            case Rsize:
            {
              
            } break;
            case Rtab:
            {
              
            } break;
            default:
            {
              
            } break;
          }
          m_fixed_flow.write(m_channel, m_spacket, size_of_MBAP +
            m_size_byte_end);
          IRS_MBUS_DBG_MSG_BASE("write_end_mode");
          m_mode = write_end_mode;
        }
      }
      break;
      case write_end_mode:
      {
        if(m_fixed_flow.write_status() == 
          irs::hardflow::fixed_flow_t::status_success)
        {
          IRS_MBUS_DBG_MSG_BASE("read_header_mode");
          m_mode = read_header_mode;
        }
      }
      break;
      case read_header_mode://read header
      {
        if(m_fixed_flow.read_status() == 
          irs::hardflow::fixed_flow_t::status_success)
        {
          m_fixed_flow.read(m_channel, m_rpacket, size_of_MBAP);
          IRS_MBUS_DBG_MSG_BASE("read_response_mode");
          m_mode = read_response_mode;
        }
      }
      break;
      case read_response_mode: 
      { 
        if(m_fixed_flow.read_status() == 
          irs::hardflow::fixed_flow_t::status_success)
        {
          convert(m_rpacket, 0, size_of_MBAP);
          MBAP_header_t &header = 
            reinterpret_cast<MBAP_header_t&>(*m_rpacket);
          m_fixed_flow.read(m_channel, m_rpacket + size_of_MBAP,
            header.length - 1);
          set_to_cnt(rtimeout,cl_rtime);
          IRS_MBUS_DBG_MSG_BASE("read_end_mode");
          m_mode = read_end_mode;
        }
      }
      break;
      case read_end_mode:
      {
        if(m_fixed_flow.read_status() == 
          irs::hardflow::fixed_flow_t::status_success)
        {
          IRS_MBUS_DBG_MSG_BASE("treatment_response_mode");
          m_mode = treatment_response_mode;
          if(test_to_cnt(rtimeout))
          {
            if(m_read_table)
            {
              IRS_MBUS_DBG_MSG_BASE("make_request_mode");
              m_mode = make_request_mode;
            }
            else
            {
              IRS_MBUS_DBG_MSG_BASE("search_block_mode");
              m_mode = search_block_mode;
            }
            if(m_part_send)
            {
              IRS_MBUS_DBG_MSG_BASE("send_data_mode");
              m_mode = send_data_mode;
            }
            if(m_first_read)
            {
              IRS_MBUS_DBG_MSG_BASE("make_request_mode");
              m_mode = make_request_mode;
            }
          }
        }
      }
      break;
      case treatment_response_mode:
      {
        IRS_MBUS_DBG_MSG_BASE(" treatment_response_mode");
        request_exception_t &convert_pack_for_read =
          reinterpret_cast<request_exception_t&>(*(m_rpacket + 
          size_of_MBAP));
        switch(convert_pack_for_read.function_code)
        {
          case read_discrete_inputs:
          {
            //  
          } break;
          case read_coils:
          {
            //  
          } break;
          case read_hold_registers:
          {
            convert(m_rpacket, size_of_MBAP + size_of_resp_header,
              sizeof(response_read_reg_t) - 2);
          } break;
          case read_input_registers:
          {
            convert(m_rpacket, size_of_MBAP + size_of_resp_header,
              sizeof(response_read_reg_t) - 2);
          } break;
          case write_single_coil:
          {
            convert(m_rpacket, size_of_MBAP + 1, size_of_read_header);
          } break;
          case write_single_register:
          {
            convert(m_rpacket, size_of_MBAP + 1, size_of_read_header);
          } break;
          case read_exception_status:
          {
            //  
          } break;
          case W_SevHR:
          {
            
          } break;
          case write_multiple_coils:
          {
            convert(m_rpacket, size_of_MBAP + 1, size_of_read_header);
          } break;
          case write_multiple_registers:
          {
            convert(m_rpacket, size_of_MBAP + 1, size_of_read_header);
          } break;
          case Rsize:
          {
            
          } break;
          case Rtab:
          {
            
          } break;
          default:
          {
            
          } break;
        }
        convert(m_spacket, size_of_MBAP + 1, size_of_read_header);
        request_t &req_header = 
          reinterpret_cast<request_t&>(*(m_spacket + size_of_MBAP));
        m_num_of_elem = req_header.quantity;
        IRS_MBUS_DBG_MSG_DETAIL("\n recieve packet");
        IRS_MBUS_DBG_MONITOR(modbus_pack_response_monitor(m_rpacket););
        irs_u16 start_addr = req_header.starting_address;
        switch(req_header.function_code)
        {
          case read_discrete_inputs:
          { 
            response_read_byte_t &read_di = 
              reinterpret_cast<response_read_byte_t&>(*(m_rpacket +
              size_of_MBAP));
            IRS_MBUS_DBG_MSG_DETAIL("read discret inputs");
            bit_copy(read_di.value, m_discr_inputs_byte_r.data(), 0,
              start_addr, read_di.byte_count*8);
            #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
            for(int di_idx = start_addr; di_idx < read_di.byte_count;
              di_idx++)
            {
              irs::mlog() << " m_discr_inputs_byte_r[" << di_idx << 
                "] = " << int(m_discr_inputs_byte_r[di_idx]) << endl;
            }
            #endif //IRS_MBUS_MSG_DETAIL
          }
          break;
          case read_coils:
          {
            response_read_byte_t &read_coils = 
              reinterpret_cast<response_read_byte_t&>(*(m_rpacket +
              size_of_MBAP));
            IRS_MBUS_DBG_MSG_DETAIL("read coils");
            bit_copy(read_coils.value, m_coils_byte_r.data(), 0, 
              start_addr, read_coils.byte_count*8);
            #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
            for(int coils_idx = start_addr; coils_idx < read_coils.byte_count;
              coils_idx++)
            {
              irs::mlog() << " m_coils_byte_r[" << coils_idx << 
                "] = " << int(m_coils_byte_r[coils_idx]) << endl;
            }
            #endif //IRS_MBUS_MSG_DETAIL
          }
          break;
          case read_hold_registers:
          {
            IRS_MBUS_DBG_MSG_DETAIL("read hold regs");
            response_read_reg_t &hr_packet = 
              reinterpret_cast<response_read_reg_t&>(*(m_rpacket + 
              size_of_MBAP));
            puts(" set_bytes");
            set_bytes(m_hold_regs_reg_r, start_addr, hr_packet.byte_count, 
              reinterpret_cast<irs_u8*>(hr_packet.value), 0);
            #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
            for(int hr_idx = start_addr; hr_idx < m_num_of_elem;
              hr_idx++)
            {
              irs::mlog() << " m_hold_regs_reg_lo[" << hr_idx << 
                "] = " << int(IRS_LOBYTE(m_hold_regs_reg_r[hr_idx])) << endl;
              irs::mlog() << " m_hold_regs_reg_hi[" << hr_idx << 
                "] = " << int(IRS_HIBYTE(m_hold_regs_reg_r[hr_idx])) << endl;
            }
            #endif //IRS_MBUS_MSG_DETAIL
          }
          break;
          case read_input_registers:
          {
            IRS_MBUS_DBG_MSG_DETAIL("read input regs");
            response_read_reg_t &ir_packet =
              reinterpret_cast<response_read_reg_t&>(*(m_rpacket + 
              size_of_MBAP));
            set_bytes(m_input_regs_reg_r, start_addr, ir_packet.byte_count,
              reinterpret_cast<irs_u8*>(ir_packet.value), 0);
            #if (IRS_MBUS_MSG_TYPE == IRS_MBUS_MSG_DETAIL)
            for(int ir_idx = start_addr; ir_idx < m_num_of_elem;
              ir_idx++)
            {
              irs::mlog() << " m_input_regs_reg_lo[" << ir_idx << 
                "] = " << int(IRS_LOBYTE(m_input_regs_reg_r[ir_idx])) << endl;
              irs::mlog() << " m_input_regs_reg_hi[" << ir_idx << 
                "] = " << int(IRS_HIBYTE(m_input_regs_reg_r[ir_idx])) << endl;
            }
            #endif //IRS_MBUS_MSG_DETAIL
          }
          break;
          case write_multiple_coils:
          {
            request_multiple_write_byte_t &sec_head =
              reinterpret_cast<request_multiple_write_byte_t&>(*(m_spacket + 
              size_of_MBAP));
            for(irs_u16 reg_index = 0; reg_index < sec_head.quantity;
              reg_index++)
            {
              m_rforwr[start_addr + reg_index] = 0;
              m_coils_byte_w[start_addr + reg_index] = 0;
            }
          }
          break;
          case write_multiple_registers:
          {
            request_multiple_write_regs_t &sec_head =
              reinterpret_cast<request_multiple_write_regs_t&>(*(m_spacket + 
              size_of_MBAP));
            for(irs_u16 reg_index = 0; reg_index < sec_head.quantity;
              reg_index++) //*2
            {
              m_rforwr[m_coils_size_bit + start_addr + reg_index] = 0;
              m_hold_regs_reg_w[start_addr + reg_index] = 0;
            }
          }
          break;
          case read_exception_status:
          {
          }
          break;
        }
        for(irs_u16 t = 0; t < size_of_packet; t++)
          m_spacket[t] = 0;
        if(m_read_table)
        {
          IRS_MBUS_DBG_MSG_BASE("make_request_mode");
          m_mode = make_request_mode;
        }
        else
        {
          IRS_MBUS_DBG_MSG_BASE("search_block_mode");
          m_mode = search_block_mode;
        }
        if(m_part_send)
        {
          IRS_MBUS_DBG_MSG_BASE("send_data_mode");
          m_mode = send_data_mode;
        }
        if(m_first_read)
        {
          IRS_MBUS_DBG_MSG_BASE("make_request_mode");
          m_mode = make_request_mode;
        }
      }
      break;
      case search_block_mode:
      {
        IRS_MBUS_DBG_MSG_BASE("Search Block");
        irs_bool catch_block = irs_false;
        for(; m_search_index < (m_coils_size_bit + m_hr_size_reg); )
        {
          if((m_rforwr[m_search_index] == irs_true) &&
            (catch_block == irs_false))
          {
            m_start_block = m_search_index;
            catch_block = irs_true;
            m_nothing = irs_false;
          }
          if((catch_block == irs_true) &&
            (m_rforwr[m_search_index] == irs_false))
          {
            m_num_of_elem = m_search_index - m_start_block;
            IRS_MBUS_DBG_MSG_BASE("send_data_mode");
            m_mode = send_data_mode;
            if((irs_u32(m_num_of_elem + m_start_block) >= 
              irs_u32(m_coils_size_bit))&&
              (m_start_block < (irs_u32)m_coils_size_bit))
            {
              m_search_index -= m_num_of_elem;
              m_num_of_elem = m_coils_size_bit - m_start_block;
              m_search_index += m_num_of_elem;
            }
            break;
          }
          m_search_index++;
          if((catch_block == irs_true) &&
            (m_search_index == (m_coils_size_bit + m_hr_size_reg)))
          { 
            m_num_of_elem = m_search_index - m_start_block;
            IRS_MBUS_DBG_MSG_BASE("send_data_mode");
            m_mode = send_data_mode;
            m_search_index = 0;
            break;
          }
          if(m_search_index >= (m_coils_size_bit + m_hr_size_reg))
          {
            m_search_index = 0;
            m_nothing = irs_true;
            IRS_MBUS_DBG_MSG_BASE("make_request_mode");
            m_mode = make_request_mode;
            break;
          }
        }
      }
      break;
      case send_data_mode:
      {
        m_read_table = irs_true;
        if((m_start_block >= (irs_u32)m_coils_size_bit) &&
          (m_start_block<(irs_u32)(m_coils_size_bit + m_hr_size_reg)))
        {
          m_command = write_multiple_registers;
          m_hr_start_byte = m_start_block - m_coils_size_bit;
          m_hr_size_byte = m_num_of_elem;
          auto_arr<irs_u16> mass(new irs_u16[m_hr_size_reg]);
          if(m_hr_size_byte > ((size_of_data_reg - 1)*2))
          {
            request_multiple_write_regs_t &hr_packet =
              reinterpret_cast<request_multiple_write_regs_t&>(*(m_spacket +
              size_of_MBAP));
            m_start_addr = irs_u16(m_hr_start_byte);
            hr_packet.byte_count = (size_of_data_reg - 1)*2;
            irs_u8 rmass[size_of_data_byte];
            get_bytes(m_hold_regs_reg_w, m_hr_start_byte,
              (size_of_data_reg - 1)*2, rmass, 0);
            IRS_LIB_ASSERT(IRS_ARRAYSIZE(rmass) >= (size_of_data_reg - 1)*2);
            for(irs_u8 i = 0; i< (size_of_data_reg - 1); i++)
            {
              hr_packet.value[i] = rmass[i];
            }
            make_packet(m_hr_start_byte, (size_of_data_reg - 1)*2);
            for(irs_u32 i = m_start_block;
              i < (m_start_block + (size_of_data_reg - 1)*2); i++)
              m_rforwr[i] = 0;
            m_start_block += ((size_of_data_reg - 1)*2);
            m_num_of_elem -= ((size_of_data_reg - 1)*2);
            m_part_send = irs_true;
          }
          else
          {
            for(irs_u16 t = 0; t < size_of_packet; t++)
              m_spacket[t] = 0;
            if(m_hr_size_byte%2 == 0)
            {
              request_multiple_write_regs_t &hr_packet =
                reinterpret_cast<request_multiple_write_regs_t&>(*(m_spacket + 
                size_of_MBAP));
              if((m_hr_start_byte%2) !=0)
              {
                get_bytes(m_hold_regs_reg_r, m_hr_start_byte - 1, 1,
                  reinterpret_cast<irs_u8*>(hr_packet.value), 0);
                IRS_LIB_ASSERT(IRS_ARRAYSIZE(hr_packet.value) >= 1);
                m_start_addr = irs_u16(m_hr_start_byte);
                hr_packet.byte_count = (irs_u8)m_hr_size_byte;
                get_bytes(m_hold_regs_reg_w, m_hr_start_byte,
                  m_hr_size_byte,
                  reinterpret_cast<irs_u8*>(hr_packet.value), 1);
                IRS_LIB_ASSERT((IRS_ARRAYSIZE(hr_packet.value) - 1) >=
                  m_hr_size_byte);
                make_packet(m_hr_start_byte,
                  irs_u16((m_hr_size_byte) + 1));
              }
              else
              {
                m_start_addr = irs_u16(m_hr_start_byte);
                hr_packet.byte_count = irs_u8(m_hr_size_byte);
                get_bytes(m_hold_regs_reg_w, m_hr_start_byte,
                  m_hr_size_byte, reinterpret_cast<irs_u8*>(hr_packet.value),
                  0);
                make_packet(m_hr_start_byte, irs_u16(m_hr_size_byte));
              }
            }
            else
            {
              request_multiple_write_regs_t &hr_packet =
                reinterpret_cast<request_multiple_write_regs_t&>(*(m_spacket + 
                size_of_MBAP));
              if((m_hr_start_byte%2) !=0)
              {
                get_bytes(m_hold_regs_reg_r, m_hr_start_byte - 1, 1,
                  reinterpret_cast<irs_u8*>(hr_packet.value), 0);
                IRS_LIB_ASSERT(IRS_ARRAYSIZE(hr_packet.value) >= 1);
                m_start_addr = irs_u16(m_hr_start_byte);
                hr_packet.byte_count = irs_u8(m_hr_size_byte);
                get_bytes(m_hold_regs_reg_w, m_hr_start_byte,
                  m_hr_size_byte,
                  reinterpret_cast<irs_u8*>(hr_packet.value), 1);
                IRS_LIB_ASSERT((IRS_ARRAYSIZE(hr_packet.value) - 1) >=
                  m_hr_size_byte);
                make_packet(m_hr_start_byte, irs_u16(m_hr_size_byte + 1));
              }
              else
              {
                m_start_addr = irs_u16(m_hr_start_byte);
                hr_packet.byte_count = irs_u8(m_hr_size_byte);
                get_bytes(m_hold_regs_reg_w, m_hr_start_byte,
                  m_hr_size_byte, reinterpret_cast<irs_u8*>(hr_packet.value),
                  0);
                IRS_LIB_ASSERT(IRS_ARRAYSIZE(hr_packet.value) >= 
                  m_hr_size_byte);
                make_packet(m_hr_start_byte, irs_u16(m_hr_size_byte + 1));
              }
            }
          }
        }
        else if (m_start_block < (irs_u32)m_coils_size_bit)
        {
          range(m_start_block, m_num_of_elem, 0, m_coils_size_bit,
            &m_coils_size_byte, &m_coils_start_byte);
          m_command = write_multiple_coils;
          request_multiple_write_byte_t &coils_packet =
            reinterpret_cast<request_multiple_write_byte_t&>(*(m_spacket + 
            size_of_MBAP));
          if(m_coils_size_byte > size_of_data_byte)
          {
            make_packet(m_coils_start_byte, size_of_data_byte);
            coils_packet.byte_count = size_of_data_byte;
            bit_copy(m_coils_byte_w.data(), 
              reinterpret_cast<irs_u8*>(coils_packet.value), 
              irs_u16(m_coils_start_byte*8), 0, m_coils_size_byte*8);
            for(irs_u32 i = m_start_block; 
              i < (m_start_block + size_of_data_byte); i++)
              m_rforwr[i] = 0;
          }
          else
          {
            make_packet(m_coils_start_byte, irs_u16(m_coils_size_byte));
            if(m_coils_size_byte%8 == 0)
              coils_packet.byte_count = irs_u8(m_coils_size_byte);
            else
              coils_packet.byte_count = irs_u8(m_coils_size_byte + 1);
            bit_copy(m_coils_byte_w.data(),
              reinterpret_cast<irs_u8*>(coils_packet.value), 
                irs_u16(m_coils_start_byte*8), 0, m_coils_size_byte*8);
            for(irs_u32 i = m_start_block;
              i < (m_start_block + m_coils_size_byte); i++)
              m_rforwr[i] = 0;
          }
        }
        IRS_MBUS_DBG_MSG_BASE("wait_mode");
        m_mode = wait_mode;
      }
      break;
      case wait_mode:
      {
        IRS_MBUS_DBG_MSG_BASE("send_request_mode");
        m_mode = send_request_mode;
      }
      break;
    }
  }
}
