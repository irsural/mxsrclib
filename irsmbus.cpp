// ������ � ������ modbus
// ����: 12.03.2010
// ���� ��������: 16.09.2008

#include <irsmbus.h>
#include <string.h>
#include <timer.h>
#include <irserror.h>
#include <irscpu.h>
#include <stdlib.h>
#include <irsstrdefs.h>
#ifdef __ICCAVR__
#include <irsdbgutil.h>
#endif // __ICCAVR__

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
  size_t index_in = a_index_data_in%8;
  size_t index_out = a_index_data_out%8;
  size_t data_start_in = a_index_data_in/8;
  size_t data_start_out = a_index_data_out/8;
  size_t first_part_size = 0;
  size_t last_part_size = 0;
  if (index_in == index_out)
  {
    if((index_out != 0) && (index_in != 0))
      first_part_size = 8 - index_out;
    last_part_size = (a_size_data_bit - first_part_size)%8;
    size_t middle_part_size = (a_size_data_bit - 
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
    size_t data_cnt = (a_size_data_bit - first_part_size)/8;
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
      size_t last_external_byte_index = data_cnt + data_start_in + 
        first_part_idx;
      size_t last_internal_byte_index = data_cnt + data_start_out + 
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
    for(size_t data_idx = 0; data_idx < data_cnt + offset_index;
      data_idx++)
    {
      size_t external_idx = data_idx + data_start_in + first_part_idx;
      size_t internal_idx = data_idx + data_start_out + first_part_idx;
      first_part(ap_data_out[internal_idx], 
        ap_data_in[external_idx + offset_idx], index_bit, middle_mask);
    }
    for(size_t data_idx = 0; data_idx < data_cnt; data_idx++)
    {
      size_t external_idx = data_idx + data_start_in + 
        first_part_idx*offset_idx;
      size_t internal_idx = data_idx + data_start_out + 
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
    irs_i64 rand_val = rand();
    ap_data_in[idx_in] = static_cast<irs_u8>(rand_val*IRS_U8_MAX/RAND_MAX);
  }
  for(int idx_out = 0; idx_out < static_cast<int>(ap_data_out.size());
    idx_out ++)
  {
    int rand_val = rand();
    ap_data_out[idx_out] = static_cast<irs_u8>(rand_val*IRS_U8_MAX/RAND_MAX);
  }
  a_strm << irsm("IN before operation") << endl;
  for(int idx_in = 0; idx_in < static_cast<int>(ap_data_in.size()); idx_in ++)
  {
    a_strm << int_to_str(ap_data_in[idx_in], 2, 8) << " ";
  }
  a_strm << endl;
  a_strm << irsm("OUT before operation") << endl;
  for(int idx_out = 0; idx_out < static_cast<int>(ap_data_out.size());
    idx_out ++)
  {
    a_strm << int_to_str(ap_data_out[idx_out], 2, 8) << " ";
  }
  a_strm << endl;
  bit_copy(ap_data_in.data(), ap_data_out.data(),
    a_index_data_in, a_index_data_out, a_size_data);
  a_strm << irsm("OUT after operation") << endl;
  for(int idx_out = 0; idx_out < static_cast<int>(ap_data_out.size());
    idx_out ++)
  {
    a_strm << int_to_str(ap_data_out[idx_out], 2, 8) << irsm(" ");
  }
  a_strm << endl;
}

void convert(irs_u8 *ap_mess, irs_u8 a_start, irs_u8 a_length)
{
  if(irs::cpu_traits_t::endian() == irs::little_endian)
  {
    for(size_t i = a_start; i < static_cast<size_t>(a_start + a_length);
      i += size_t(2))
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

void get_bytes(irs::raw_data_t<irs_u16> &data_u16, size_t a_index,
  size_t a_size, irs_u8 *ap_buf, size_t a_start)
{
  irs_u8* data_u8 = reinterpret_cast<irs_u8*>(data_u16.data());
  irs::memcpyex(ap_buf + a_start, data_u8 + a_index, a_size);
}

void irs::range(size_t a_index, size_t a_size, size_t a_start_range, 
  size_t a_end_range, size_t *a_num, size_t *a_start)
{
  size_t var = a_index + a_size;
  if((a_index >= a_start_range) && (a_index < a_end_range) && 
    (var > a_start_range) && (var <= a_end_range))
  {
    *a_num = a_size;
    *a_start = (a_index - a_start_range);
  }
  else if((a_index > a_start_range) && (a_index < a_end_range) && 
    (var > a_end_range))
  {
    *a_num = (a_end_range - a_index);
    *a_start = (a_index - a_start_range);
  }
  else if((a_index < a_start_range) && (var > a_start_range) && 
    (var < a_end_range))
  {
    *a_num = (var - a_start_range);
    *a_start = 0;
  }
  else if((a_index <= a_start_range) && (var >= a_end_range))
  {
    *a_num = (a_end_range - a_start_range);
    *a_start = 0;
  }
}

irs::modbus_server_t::modbus_server_t(
  hardflow_t* ap_hardflow,
  size_t a_discr_inputs_size_byte,
  size_t a_coils_size_byte,
  size_t a_hold_regs_reg,
  size_t a_input_regs_reg,
  double a_disconnect_time_sec
):
  mp_buf(size_of_packet),
  m_size_byte_end(0),
  m_discret_inputs_size_bit(size_t(a_discr_inputs_size_byte*8)),
  m_coils_size_bit(size_t(a_coils_size_byte*8)),
  m_hold_registers_size_reg(a_hold_regs_reg),
  m_input_registers_size_reg(a_input_regs_reg),
  m_size_byte(a_discr_inputs_size_byte + a_coils_size_byte + 
    a_hold_regs_reg*2 + a_input_regs_reg*2),
  m_discret_inputs_end_byte(a_discr_inputs_size_byte),
  m_coils_end_byte(a_discr_inputs_size_byte + a_coils_size_byte),
  m_hold_registers_end_byte(a_discr_inputs_size_byte + a_coils_size_byte +
    a_hold_regs_reg*2),
  m_input_registers_end_byte(a_discr_inputs_size_byte + a_coils_size_byte +
    a_hold_regs_reg*2 + a_input_regs_reg*2),
  m_mode(read_header_mode),
  m_discr_inputs_byte(int(a_discr_inputs_size_byte)),
  m_coils_byte(int(a_coils_size_byte)),
  m_input_regs_reg(int(a_input_regs_reg)),
  m_hold_regs_reg(int(a_hold_regs_reg)),
  m_discret_inputs_size_byte(0),
  m_discret_inputs_start_byte(0),
  m_coils_size_byte(0),
  m_coils_start_byte(0),
  m_hold_registers_size_byte(0),
  m_hold_registers_start_byte(0),
  m_input_registers_size_byte(0),
  m_input_registers_start_byte(0),
  m_channel(0),
  mp_hardflow_server(ap_hardflow),
  m_fixed_flow(mp_hardflow_server),
  m_num_of_elem(0),
  m_operation_status(status_completed)
{
  m_fixed_flow.read_timeout(a_disconnect_time_sec);
  m_fixed_flow.write_timeout(a_disconnect_time_sec);
  mlog() << irsm("SERVER START!") << endl;
}

irs_uarc irs::modbus_server_t::size()
{
  return m_size_byte;
}

irs_bool irs::modbus_server_t::connected()
{
  return irs_true;
}

void irs::modbus_server_t::modbus_pack_request_monitor(irs_u8 *ap_buf)
{
  mlog() << irsm("\n recieved packet") << endl;
  MBAP_header_t &header = 
    reinterpret_cast<MBAP_header_t&>(*ap_buf);
  request_exception_t &req_header = 
    reinterpret_cast<request_exception_t&>(*(ap_buf + size_of_MBAP));
    
  mlog() << irsm("\n ----------- MODBUS's Header----------------\n");
  mlog() << irsm(" transaction_id_lo .... ") <<
    int(IRS_LOBYTE(header.transaction_id)) << endl;
  mlog() << irsm(" transaction_id_hi .... ") <<
    int(IRS_HIBYTE(header.transaction_id)) << endl;
  mlog() << irsm(" proto_id_lo .......... ") <<
    int(IRS_LOBYTE(header.proto_id)) << endl;
  mlog() << irsm(" proto_id_hi .......... ") <<
    int(IRS_HIBYTE(header.proto_id)) << endl;
  mlog() << irsm(" length_lo ............ ") <<
    int(IRS_LOBYTE(header.length)) << endl;
  mlog() << irsm(" length_hi ............ ") <<
    int(IRS_HIBYTE(header.length)) << endl;
  mlog() << irsm(" unit_identifier ...... ") <<
    int(header.unit_identifier) << endl;
    
  mlog() << irsm(" function_code ........ ") <<
    int(req_header.function_code);
  switch(req_header.function_code)
  {
    case read_discrete_inputs:
    {
      request_t &req_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read discret inputs") << endl;
      mlog() << irsm(" starting_address_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_lo .......... ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_hi .......... ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
    } break;
    case read_coils:
    {
      request_t &req_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read coils") << endl;
      mlog() << irsm(" starting_address_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_lo .......... ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_hi .......... ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
    } break;
    case read_hold_registers:
    {
      request_t &req_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read hold registers") << endl;
      mlog() << irsm(" starting_address_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_lo .......... ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_hi .......... ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
    } break;
    case read_input_registers:
    {
      request_t &req_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read input registers") << endl;
      mlog() << irsm(" starting_address_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_lo .......... ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_hi .......... ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
    } break;
    case write_single_coil:
    {
      response_single_write_t &req_header_inner = 
        reinterpret_cast<response_single_write_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" write single coil") << endl;
      mlog() << irsm(" output_address_lo .... ") <<
        int(IRS_LOBYTE(req_header_inner.address)) << endl;
      mlog() << irsm(" output_address_hi .... ") <<
        int(IRS_HIBYTE(req_header_inner.address)) << endl;
      mlog() << irsm(" output_value_lo ...... ") <<
        int(IRS_LOBYTE(req_header_inner.value)) << endl;
      mlog() << irsm(" output_value_hi ...... ") <<
        int(IRS_HIBYTE(req_header_inner.value)) << endl;
    } break;
    case write_single_register:
    {
      response_single_write_t &req_header_inner = 
        reinterpret_cast<response_single_write_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" write hold register") << endl;
      mlog() << irsm(" register_address_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.address)) << endl;
      mlog() << irsm(" register_address_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.address)) << endl;
      mlog() << irsm(" register_value_lo .... ") <<
        int(IRS_LOBYTE(req_header_inner.value)) << endl;
      mlog() << irsm(" register_value_hi .... ") <<
        int(IRS_HIBYTE(req_header_inner.value)) << endl;
    } break;
    case read_exception_status:
    {
      mlog() << irsm(" read exception status ") << endl;
    } break;
    case write_multiple_coils:
    {
      request_multiple_write_byte_t &req_header_inner = 
        reinterpret_cast<request_multiple_write_byte_t&>(
        *(ap_buf + size_of_MBAP));
      mlog() << irsm(" write multiple coils") << endl;
      mlog() << irsm(" starting_address_lo ..... ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi ..... ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_of_outputs_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_of_outputs_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" byte_count .............. ") <<
        int(req_header_inner.byte_count) << endl;
      int coils_idx_max = 0;
      if(req_header_inner.quantity%8 == 0) {
        coils_idx_max = req_header_inner.quantity/8;
      } else {
        coils_idx_max = req_header_inner.quantity/8 + 1;
      }
      for(int coils_idx = 0; coils_idx < coils_idx_max; coils_idx++) {
        mlog() << irsm(" coils_value ........... ") <<
          int(req_header_inner.value[coils_idx]) << endl;
      }
    } break;
    case write_multiple_registers:
    {
      request_multiple_write_regs_t &req_header_inner = 
        reinterpret_cast<request_multiple_write_regs_t&>(
        *(ap_buf + size_of_MBAP));
      mlog() << irsm(" write multiple register") << endl;
      mlog() << irsm(" starting_address_lo ....... ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi ....... ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_of_registers_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_of_registers_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" byte_count ................ ") <<
        int(req_header_inner.byte_count) << endl;
      for(int reg_idx = 0; reg_idx < int(req_header_inner.quantity); reg_idx++)
      {
        mlog() << irsm(" register_value_lo ......... ") <<
          int(IRS_LOBYTE(req_header_inner.value[reg_idx])) << endl;
        mlog() << irsm(" register_value_hi ......... ") <<
          int(IRS_HIBYTE(req_header_inner.value[reg_idx])) << endl;
      }
    } break;
    default:
    {
      mlog() << irsm(" ILLEGAL_FUNCTION") << endl;
    } break;
  }
}


void irs::modbus_server_t::modbus_pack_response_monitor(irs_u8 *ap_buf)
{
  mlog() << irsm("\n send packet") << endl;
  MBAP_header_t &header = 
    reinterpret_cast<MBAP_header_t&>(*ap_buf);
  request_exception_t &resp_header = 
    reinterpret_cast<request_exception_t&>(*(ap_buf + size_of_MBAP));
    
  mlog() << irsm("\n ----------- MODBUS's Header----------------\n");
  mlog() << irsm(" transaction_id_lo .... ") <<
    int(IRS_LOBYTE(header.transaction_id)) << endl;
  mlog() << irsm(" transaction_id_hi .... ") <<
    int(IRS_HIBYTE(header.transaction_id)) << endl;
  mlog() << irsm(" proto_id_lo .......... ") <<
    int(IRS_LOBYTE(header.proto_id)) << endl;
  mlog() << irsm(" proto_id_hi .......... ") <<
    int(IRS_HIBYTE(header.proto_id)) << endl;
  mlog() << irsm(" length_lo ............ ") <<
    int(IRS_LOBYTE(header.length)) << endl;
  mlog() << irsm(" length_hi ............ ") <<
    int(IRS_HIBYTE(header.length)) << endl;
  mlog() << irsm(" unit_identifier ...... ") <<
    int(header.unit_identifier) << endl;
    
  mlog() << irsm(" function_code ........ ") <<
    int(resp_header.function_code);
  switch(resp_header.function_code)
  {
    case read_discrete_inputs:
    {
      response_read_byte_t &resp_header_inner = 
        reinterpret_cast<response_read_byte_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read discret inputs") << endl;
      mlog() << irsm(" byte_count ........... ") <<
        int(resp_header_inner.byte_count) << endl;
      for(int di_idx = 0; di_idx < resp_header_inner.byte_count; di_idx++) {
        mlog() << irsm(" discret input[") << di_idx << irsm("] ..... ") <<
          int(resp_header_inner.value[di_idx]) << endl;
      }
    } break;
    case read_coils:
    {
      response_read_byte_t &resp_header_inner = 
        reinterpret_cast<response_read_byte_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read coils") << endl;
      mlog() << irsm(" byte_count ........... ") <<
        int(resp_header_inner.byte_count) << endl;
      for(int coils_idx = 0; coils_idx < resp_header_inner.byte_count; 
        coils_idx++) 
      {
        mlog() << irsm(" coils[") << coils_idx <<
          irsm("] ............. ") <<
          int(resp_header_inner.value[coils_idx]) << endl;
      }
    } break;
    case read_hold_registers:
    {
      response_read_reg_t &resp_header_inner = 
        reinterpret_cast<response_read_reg_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read hold registers") << endl;
      mlog() << irsm(" byte_count ........... ") <<
        int(resp_header_inner.byte_count) << endl;
      for(int h_reg_idx = 0; h_reg_idx < int(resp_header_inner.byte_count)/2;
        h_reg_idx++) 
      {
        mlog() << irsm(" hold regs lo[") << h_reg_idx <<
          irsm("] ...... ") <<
          int(IRS_LOBYTE(resp_header_inner.value[h_reg_idx])) << endl;
        mlog() << irsm(" hold regs hi[") << h_reg_idx <<
          irsm("] ...... ") <<
          int(IRS_HIBYTE(resp_header_inner.value[h_reg_idx])) << endl;
      }
    } break;
    case read_input_registers:
    {
      response_read_reg_t &resp_header_inner = 
        reinterpret_cast<response_read_reg_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read input registers") << endl;
      mlog() << irsm(" byte_count ........... ") <<
        int(resp_header_inner.byte_count) << endl;
      for(int i_reg_idx = 0; i_reg_idx < int(resp_header_inner.byte_count)/2;
        i_reg_idx++) 
      {
        mlog() << irsm(" input regs lo[") << i_reg_idx <<
          irsm("] ..... ") <<
          int(IRS_LOBYTE(resp_header_inner.value[i_reg_idx])) << endl;
        mlog() << irsm(" input regs hi[") << i_reg_idx <<
          irsm("] ..... ") <<
          int(IRS_HIBYTE(resp_header_inner.value[i_reg_idx])) << endl;
      }
    } break;
    case write_single_coil:
    {
      response_single_write_t &resp_header_inner = 
        reinterpret_cast<response_single_write_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" write single coil") << endl;
      mlog() << irsm(" output_address_lo .... ") <<
        int(IRS_LOBYTE(resp_header_inner.address)) << endl;
      mlog() << irsm(" output_address_hi .... ") <<
        int(IRS_HIBYTE(resp_header_inner.address)) << endl;
      mlog() << irsm(" output_value_lo ...... ") <<
        int(IRS_LOBYTE(resp_header_inner.value)) << endl;
      mlog() << irsm(" output_value_hi ...... ") <<
        int(IRS_HIBYTE(resp_header_inner.value)) << endl;
    } break;
    case write_single_register:
    {
      response_single_write_t &resp_header_inner = 
        reinterpret_cast<response_single_write_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" write single register") << endl;
      mlog() << irsm(" output_address_lo .... ") <<
        int(IRS_LOBYTE(resp_header_inner.address)) << endl;
      mlog() << irsm(" output_address_hi .... ") <<
        int(IRS_HIBYTE(resp_header_inner.address)) << endl;
      mlog() << irsm(" output_value_lo ...... ") <<
        int(IRS_LOBYTE(resp_header_inner.value)) << endl;
      mlog() << irsm(" output_value_hi ...... ") <<
        int(IRS_HIBYTE(resp_header_inner.value)) << endl;
    } break;
    case read_exception_status:
    {
      response_exception_t &resp_header_inner = 
        reinterpret_cast<response_exception_t&>(
        *(ap_buf + size_of_MBAP));
      mlog() << irsm(" read exception status ") << endl;
      mlog() << irsm(" error_code ........... ") <<
        int(resp_header_inner.error_code) << endl;
      mlog() << irsm(" exeption_code ........ ") <<
        int(resp_header_inner.exeption_code) << endl;
    } break;
    case write_multiple_coils:
    {
      request_t &resp_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" write multiple coils") << endl;
      mlog() << irsm(" starting_address_lo .. ") <<
        int(IRS_LOBYTE(resp_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi .. ") <<
        int(IRS_HIBYTE(resp_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_lo .......... ") <<
        int(IRS_LOBYTE(resp_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_hi .......... ") <<
        int(IRS_HIBYTE(resp_header_inner.quantity)) << endl;
    } break;
    case write_multiple_registers:
    {
      request_t &resp_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" write multiple registers") << endl;
      mlog() << irsm(" starting_address_lo .. ") <<
        int(IRS_LOBYTE(resp_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi .. ") <<
        int(IRS_HIBYTE(resp_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_lo .......... ") <<
        int(IRS_LOBYTE(resp_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_hi .......... ") <<
        int(IRS_HIBYTE(resp_header_inner.quantity)) << endl;
    } break;
    default:
    {
      mlog() << irsm(" ILLEGAL_FUNCTION") << endl;
    } break;
  }
}

void irs::modbus_server_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm("read"));
  m_discret_inputs_size_byte = 0, m_discret_inputs_start_byte = 0;
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  m_hold_registers_size_byte = 0, m_hold_registers_start_byte = 0;
  m_input_registers_size_byte = 0, m_input_registers_start_byte = 0;
  
  range(a_index, a_size, 0, m_discret_inputs_end_byte, 
    &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
  range(a_index, a_size, m_discret_inputs_end_byte, m_coils_end_byte,
    &m_coils_size_byte, &m_coils_start_byte);
  range(a_index, a_size, m_coils_end_byte, m_hold_registers_end_byte,
    &m_hold_registers_size_byte, &m_hold_registers_start_byte);
  range(a_index, a_size, m_hold_registers_end_byte, m_input_registers_end_byte,
    &m_input_registers_size_byte, &m_input_registers_start_byte);
    
  size_t i = 0;
  if((m_discret_inputs_size_byte != 0))
  {
    for(; i < m_discret_inputs_size_byte; i++) 
      ap_buf[i] = m_discr_inputs_byte[i];
  }
  if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
  {
    for(; i < (m_discret_inputs_size_byte + m_coils_size_byte); i++) 
      ap_buf[i] = m_coils_byte[i - m_discret_inputs_size_byte];
  }
  if((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
  {
    get_bytes(m_hold_regs_reg, m_hold_registers_start_byte,
      m_hold_registers_size_byte, ap_buf, i);
  }
  if((m_input_registers_size_byte != 0)||(m_input_registers_start_byte != 0))
  {
    get_bytes(m_input_regs_reg, m_input_registers_start_byte, 
      m_input_registers_size_byte, ap_buf, i + m_hold_registers_size_byte);
  }
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    for(size_t buf_idx = 0;
      buf_idx < (m_discret_inputs_size_byte + 
      m_coils_size_byte + m_hold_registers_size_byte + 
      m_input_registers_size_byte); buf_idx++)
    {
      mlog() << irsm("ap_buf[") << buf_idx << irsm("] = ") <<
        (int)ap_buf[buf_idx] << endl;
    }
  );
}

void irs::modbus_server_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    for(size_t buf_idx = 0; buf_idx < m_size_byte; buf_idx ++)
    {
      mlog() << "ap_buf[" << buf_idx << "] = " <<
        (int)ap_buf[buf_idx] << endl;
    }
  );

  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm("write"));
  m_discret_inputs_size_byte = 0, m_discret_inputs_start_byte = 0;
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  m_hold_registers_size_byte = 0, m_hold_registers_start_byte = 0;
  m_input_registers_size_byte = 0, m_input_registers_start_byte = 0;
  
  range(a_index, a_size, m_discret_inputs_start_byte, m_discret_inputs_end_byte,
    &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
  range(a_index, a_size, m_discret_inputs_end_byte, m_coils_end_byte,
    &m_coils_size_byte, &m_coils_start_byte);
  range(a_index, a_size, m_coils_end_byte, m_hold_registers_end_byte, 
    &m_hold_registers_size_byte, &m_hold_registers_start_byte);
  range(a_index, a_size, m_hold_registers_end_byte, m_input_registers_end_byte,
    &m_input_registers_size_byte, &m_input_registers_start_byte);

  if((m_discret_inputs_size_byte != 0))
  {
    bit_copy(ap_buf, m_discr_inputs_byte.data(), 0, 
      m_discret_inputs_start_byte*8, m_discret_inputs_size_byte*8);
    IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
      for(size_t buf_idx = 0; buf_idx < m_discr_inputs_byte.size(); buf_idx ++)
      {
        mlog() << irsm("m_discr_inputs_byte[") << buf_idx << irsm("] = ") <<
          (int)m_discr_inputs_byte[buf_idx] << endl;
      }
    );
  }
  if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
  {
    bit_copy(ap_buf, m_coils_byte.data(), m_discret_inputs_size_byte*8,
      m_coils_start_byte*8, m_coils_size_byte*8);
    IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
      for(size_t buf_idx = 0; buf_idx < m_discr_inputs_byte.size(); buf_idx ++)
      {
        mlog() << irsm("m_coils_byte[") << buf_idx << irsm("] = ") <<
          (int)m_coils_byte[buf_idx] << endl;
      }
    );
  }
  if((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
  {
    set_bytes(m_hold_regs_reg, 0, m_hold_registers_size_byte, ap_buf, 
      m_discret_inputs_size_byte + m_coils_size_byte);
    IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
      for(size_t buf_idx = 0;
        buf_idx < m_hold_regs_reg.size(); buf_idx ++)
      {
        mlog() << irsm("m_hold_regs_reg_lo[") << buf_idx << irsm("] = ") <<
          int(IRS_LOBYTE(m_hold_regs_reg[buf_idx])) << endl;
        mlog() << irsm("m_hold_regs_reg_hi[") << buf_idx << irsm("] = ") <<
          int(IRS_HIBYTE(m_hold_regs_reg[buf_idx])) << endl;
      }
    );
  }
  if((m_input_registers_size_byte != 0)||(m_input_registers_start_byte != 0))
  {
    set_bytes(m_input_regs_reg, 0, m_input_registers_size_byte, ap_buf,
      m_discret_inputs_size_byte + m_coils_size_byte + 
      m_hold_registers_size_byte);
    IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
      for(size_t buf_idx = 0; buf_idx < m_input_regs_reg.size(); buf_idx ++)
      {
        mlog() << irsm("m_input_regs_reg_lo[") << buf_idx << irsm("] = ") <<
          int(IRS_LOBYTE(m_input_regs_reg[buf_idx])) << endl;
        mlog() << irsm("m_input_regs_reg_hi[") << buf_idx << irsm("] = ") <<
          int(IRS_HIBYTE(m_input_regs_reg[buf_idx])) << endl;
      }
    );
  }
}

irs_bool irs::modbus_server_t::bit(irs_uarc a_byte_index, irs_uarc a_bit_index)
{
  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm("read bit"));
  IRS_LIB_ASSERT(a_bit_index < 8);
  m_discret_inputs_size_byte = 0, m_discret_inputs_start_byte = 0;
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  m_hold_registers_size_byte = 0, m_hold_registers_start_byte = 0;
  m_input_registers_size_byte = 0, m_input_registers_start_byte = 0;
  range(a_byte_index, 1, m_discret_inputs_start_byte, m_discret_inputs_end_byte,
    &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
  range(a_byte_index, 1, m_discret_inputs_end_byte, m_coils_end_byte, 
    &m_coils_size_byte, &m_coils_start_byte);
  range(a_byte_index, 1, m_coils_end_byte, m_hold_registers_end_byte,
    &m_hold_registers_size_byte, &m_hold_registers_start_byte);
  range(a_byte_index, 1, m_hold_registers_end_byte, m_input_registers_end_byte,
    &m_input_registers_size_byte, &m_input_registers_start_byte);
  irs_u8 mask = mask_gen(8 - (a_bit_index + 1), 1);  
  if (a_bit_index < 8) {
    if ((m_discret_inputs_size_byte != 0) || (m_discret_inputs_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" read bit: discret inputs"));
      irs_u8 di_byte_read_bit = 
        m_discr_inputs_byte[m_discret_inputs_start_byte];
      di_byte_read_bit &= static_cast<irs_u8>(mask);
      return to_irs_bool(di_byte_read_bit);
    }
    if ((m_coils_size_byte != 0) || (m_coils_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" read bit: coils"));
      irs_u8 coils_byte_read_bit = m_coils_byte[m_coils_start_byte];
      coils_byte_read_bit &= static_cast<irs_u8>(mask);
      return to_irs_bool(coils_byte_read_bit);
    }
    if ((m_hold_registers_size_byte != 0) || (m_hold_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" read bit: hold regs"));
      irs_u8 hr_byte_read_bit = 0;
      if(m_hold_registers_start_byte%2 == 0) {
        hr_byte_read_bit = 
          IRS_LOBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]);
      } else {
        hr_byte_read_bit = 
          IRS_HIBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]);
      }
      hr_byte_read_bit &= static_cast<irs_u8>(mask);
      return to_irs_bool(hr_byte_read_bit);
    }
    if ((m_input_registers_size_byte != 0) || 
      (m_input_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" read bit: input regs"));
      irs_u8 ir_byte_read_bit = 0;
      if(m_input_registers_start_byte%2 == 0) {
        ir_byte_read_bit = 
          IRS_LOBYTE(m_input_regs_reg[m_input_registers_start_byte/2]);
      } else {
        ir_byte_read_bit = 
          IRS_HIBYTE(m_input_regs_reg[m_input_registers_start_byte/2]);
      }
      ir_byte_read_bit &= static_cast<irs_u8>(mask);
      return to_irs_bool(ir_byte_read_bit);
    }
  }
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    if((m_coils_size_byte == 0)&&(m_discret_inputs_size_byte == 0)&&
      (m_hold_registers_size_byte == 0)&&(m_input_registers_size_byte == 0))
    {
      mlog() << irsm("\n***************Illegal index or bufer size"
        "******************\n") << endl;
    }
  );
  return irs_false;
}

void irs::modbus_server_t::set_bit(irs_uarc a_byte_index, irs_uarc a_bit_index)
{
  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm("set bit"));
  IRS_LIB_ASSERT(a_bit_index < 8);
  IRS_LIB_ASSERT(a_byte_index <= m_size_byte);
  m_discret_inputs_size_byte = 0, m_discret_inputs_start_byte = 0;
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  m_hold_registers_size_byte = 0, m_hold_registers_start_byte = 0;
  m_input_registers_size_byte = 0, m_input_registers_start_byte = 0;
  range(a_byte_index, 1, 0, m_discret_inputs_end_byte, 
    &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
  range(a_byte_index, 1, m_discret_inputs_end_byte, m_coils_end_byte,
    &m_coils_size_byte, &m_coils_start_byte);
  range(a_byte_index, 1, m_coils_end_byte, m_hold_registers_end_byte,
    &m_hold_registers_size_byte, &m_hold_registers_start_byte);
  range(a_byte_index, 1, m_hold_registers_end_byte, m_input_registers_end_byte, 
    &m_input_registers_size_byte, &m_input_registers_start_byte);
  
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    if((m_coils_size_byte == 0) && (m_discret_inputs_size_byte == 0) &&
      (m_hold_registers_size_byte == 0) && (m_input_registers_size_byte == 0))
    {
      mlog() << irsm("\n************Illegal index or bufer size"
        "***************\n") << endl;
    }
  );
  
  irs_u8 mask = mask_gen(8 - (a_bit_index + 1), 1);
  if (a_bit_index < 8) {
    if((m_discret_inputs_size_byte != 0)||(m_discret_inputs_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" set bit: discret inputs"));
      m_discr_inputs_byte[m_discret_inputs_start_byte] &=
        static_cast<irs_u8>(~mask);
      m_discr_inputs_byte[m_discret_inputs_start_byte] |= 
        static_cast<irs_u8>(mask);
    }
    if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" set bit: coils"));
      m_coils_byte[m_coils_start_byte] &= static_cast<irs_u8>(~mask);
      m_coils_byte[m_coils_start_byte] |= static_cast<irs_u8>(mask);
    }
    if((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" set bit: hold regs"));
      irs_u8 hr_byte_set_bit = 0;
      if(m_hold_registers_start_byte%2 == 0) {
        hr_byte_set_bit = 
          IRS_LOBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]);
      } else {
        hr_byte_set_bit = 
          IRS_HIBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]);
      }
      hr_byte_set_bit &= static_cast<irs_u8>(~mask);
      hr_byte_set_bit |= static_cast<irs_u8>(mask);
      if(m_hold_registers_start_byte%2 == 0) {
        IRS_LOBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]) =
          hr_byte_set_bit;
      } else {
        IRS_HIBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]) =
          hr_byte_set_bit;
      }
    }
    if((m_input_registers_size_byte != 0)||(m_input_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" set bit: input regs"));
      irs_u8 ir_byte_set_bit = 0;
      if(m_input_registers_start_byte%2 == 0) {
        ir_byte_set_bit = 
          IRS_LOBYTE(m_input_regs_reg[m_input_registers_start_byte/2]);
      } else {
        ir_byte_set_bit = 
          IRS_HIBYTE(m_input_regs_reg[m_input_registers_start_byte/2]);
      }
      ir_byte_set_bit &= static_cast<irs_u8>(~mask);
      ir_byte_set_bit |= static_cast<irs_u8>(mask);
      if(m_input_registers_start_byte%2 == 0) {
        IRS_LOBYTE(m_input_regs_reg[m_input_registers_start_byte/2]) =
          ir_byte_set_bit;
      } else {
        IRS_HIBYTE(m_input_regs_reg[m_input_registers_start_byte/2]) = 
          ir_byte_set_bit;
      }
    }
  }
}

void irs::modbus_server_t::clear_bit(irs_uarc a_byte_index, 
  irs_uarc a_bit_index)
{
  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm("clear bit"));
  IRS_LIB_ASSERT(a_bit_index < 8);
  m_discret_inputs_size_byte = 0, m_discret_inputs_start_byte = 0;
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  m_hold_registers_size_byte = 0, m_hold_registers_start_byte = 0;
  m_input_registers_size_byte = 0, m_input_registers_start_byte = 0;
  range(a_byte_index, 1, 0, m_discret_inputs_end_byte, 
    &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
  range(a_byte_index, 1, m_discret_inputs_end_byte, m_coils_end_byte,
    &m_coils_size_byte, &m_coils_start_byte);
  range(a_byte_index, 1, m_coils_end_byte, m_hold_registers_end_byte, 
    &m_hold_registers_size_byte, &m_hold_registers_start_byte);
  range(a_byte_index, 1, m_hold_registers_end_byte, m_input_registers_end_byte, 
    &m_input_registers_size_byte, &m_input_registers_start_byte);
  
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    if((m_coils_size_byte == 0)&&(m_discret_inputs_size_byte == 0)&&
      (m_hold_registers_size_byte == 0)&&(m_input_registers_size_byte == 0))
    {
      mlog() << irsm("\n****************Illegal index or bufer size"
        "***************\n") << endl;
    }
  );
  
  irs_u8 mask = mask_gen(8 - (a_bit_index + 1), 1);
  if (a_bit_index < 8) {
    if((m_discret_inputs_size_byte != 0)||(m_discret_inputs_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" clear bit: discret inputs"));
      m_discr_inputs_byte[m_discret_inputs_start_byte] &=
        static_cast<irs_u8>(~mask);
    }
    if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" clear bit: coils"));
      m_coils_byte[m_coils_start_byte] &= static_cast<irs_u8>(~mask);
    }
    if((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" clear bit: hold regs"));
      irs_u8 hr_byte_clear_bit = 0;
      if(m_hold_registers_start_byte%2 == 0) {
        hr_byte_clear_bit = 
          IRS_LOBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]);
      } else {
        hr_byte_clear_bit = 
          IRS_HIBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]);
      }
      hr_byte_clear_bit &= static_cast<irs_u8>(~mask);
      if(m_hold_registers_start_byte%2 == 0) {
        IRS_LOBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]) = 
          hr_byte_clear_bit;
      } else {
        IRS_HIBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]) = 
          hr_byte_clear_bit;
      }
    }
    if((m_input_registers_size_byte != 0)||(m_input_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" clear bit: input regs"));
      irs_u8 ir_byte_clear_bit = 0;
      if(m_input_registers_start_byte%2 == 0) {
        ir_byte_clear_bit = 
          IRS_LOBYTE(m_input_regs_reg[m_input_registers_start_byte/2]);
      } else {
        ir_byte_clear_bit = 
          IRS_HIBYTE(m_input_regs_reg[m_input_registers_start_byte/2]);
      }
      ir_byte_clear_bit &= static_cast<irs_u8>(~mask);
      if(m_input_registers_start_byte%2 == 0) {
        IRS_LOBYTE(m_input_regs_reg[m_input_registers_start_byte/2]) = 
          ir_byte_clear_bit;
      } else {
        IRS_HIBYTE(m_input_regs_reg[m_input_registers_start_byte/2]) = 
          ir_byte_clear_bit;
      }
    }
  }
}

void irs::modbus_server_t::error_response(irs_u8 error_type)
{
  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm("error_response.................."));
  request_exception_t &request_exception = 
    reinterpret_cast<request_exception_t&>(*(mp_buf.data() + size_of_MBAP));
  response_exception_t &response_error = 
    reinterpret_cast<response_exception_t&>(*(mp_buf.data() + size_of_MBAP + 
    size_of_resp_header));
  response_error.error_code =
    static_cast<irs_u8>(request_exception.function_code + irs_u8(0x80));
  response_error.exeption_code = error_type;
  bit_copy(&response_error.error_code, mp_buf.data(), 
    response_error.error_code, size_of_MBAP*8, sizeof(response_error)*8);
}

void irs::modbus_server_t::mark_to_send(irs_uarc /*a_index*/, 
  irs_uarc /*a_size*/)
{
  m_operation_status = status_completed;
}

void irs::modbus_server_t::mark_to_recieve(irs_uarc /*a_index*/, 
  irs_uarc /*a_size*/)
{
  m_operation_status = status_completed;
}

void irs::modbus_server_t::mark_to_send_bit(irs_uarc /*a_byte_index*/,
  irs_uarc /*a_bit_index*/)
{
  m_operation_status = status_completed;
}

void irs::modbus_server_t::mark_to_recieve_bit(irs_uarc /*a_byte_index*/,
  irs_uarc /*a_bit_index*/)
{
  m_operation_status = status_completed;
}

irs::modbus_server_t::status_t irs::modbus_server_t::status() const
{
  return m_operation_status;
}

void irs::modbus_server_t::set_refresh_mode(mode_refresh_t /*a_refresh_mode*/)
{
  
}

void irs::modbus_server_t::update()
{

}

void irs::modbus_server_t::abort()
{

}

void irs::modbus_server_t::tick()
{
  mp_hardflow_server->tick();
  m_fixed_flow.tick();
  if((m_fixed_flow.write_status() == 
    irs::hardflow::fixed_flow_t::status_error) ||
    (m_fixed_flow.read_status() == 
    irs::hardflow::fixed_flow_t::status_error))
  {
    IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" abort"));
    m_channel = mp_hardflow_server->channel_next();
    m_mode = read_header_mode;
    mlog() << irsm("abort") << endl;
  }
  
  switch(m_mode)
  {
    case read_header_mode:
    {
      m_fixed_flow.read(m_channel, mp_buf.data(), size_of_MBAP);
      m_mode = read_request_mode;
    }
    break;
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
            header.length - 1);
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
        IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" read_end_mode"));
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
          default:
          {
            mlog() << irsm("ILLEGAL_PACKET") << endl;
          } break;
        }
        IRS_LIB_IRSMBUS_DBG_MONITOR(
          modbus_pack_request_monitor(mp_buf.data()););
        MBAP_header_t &header = 
          reinterpret_cast<MBAP_header_t&>(*mp_buf.data());
        request_t &req_header = 
          reinterpret_cast<request_t&>(*(mp_buf.data() + size_of_MBAP));
        m_num_of_elem = req_header.quantity;
        irs_u16 start_addr = req_header.starting_address;
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
                read_di.byte_count = static_cast<irs_u8>(m_num_of_elem/8);
              } else {
                read_di.byte_count = static_cast<irs_u8>(m_num_of_elem/8 + 1);
              }
              header.length = irs_u16(1 + size_of_resp_header + 
                read_di.byte_count);
              bit_copy(m_discr_inputs_byte.data(), read_di.value,
                start_addr, 0, read_di.byte_count*8);
              IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
                for(int di_idx = start_addr; 
                  di_idx < start_addr + read_di.byte_count; di_idx++)
                {
                  mlog() << irsm(" m_discr_inputs_byte[") << di_idx << 
                    irsm("] = ") << int(read_di.value[di_idx]) << endl;
                }
              );
            } else {
              error_response(ILLEGAL_DATA_ADDRESS);
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
                read_coils.byte_count = static_cast<irs_u8>(m_num_of_elem/8);
              } else {
                read_coils.byte_count = 
                  static_cast<irs_u8>(m_num_of_elem/8 + 1);
              }
              header.length = irs_u16(1 + size_of_resp_header + 
                read_coils.byte_count);
              bit_copy(m_coils_byte.data(), read_coils.value, start_addr, 0,
                read_coils.byte_count*8);
              IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
                for(int coils_idx = start_addr;
                  coils_idx < start_addr + read_coils.byte_count; coils_idx++)
                {
                  mlog() << irsm(" m_coils_byte[") << coils_idx << 
                    irsm("] = ") << int(read_coils.value[coils_idx]) << endl;
                }
              );
            } else {
              error_response(ILLEGAL_DATA_ADDRESS);
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
              read_hr.byte_count = static_cast<irs_u8>(m_num_of_elem*2);
              get_bytes(m_hold_regs_reg, start_addr*2, read_hr.byte_count,
                reinterpret_cast<irs_u8*>(read_hr.value), 0);
              IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
                for(size_t hr_idx = start_addr;
                  hr_idx < (start_addr + m_num_of_elem);
                  hr_idx++)
                {
                  mlog() << irsm(" hold regs lo[") << hr_idx << 
                    irsm("] = ") <<
                    int(IRS_LOBYTE(read_hr.value[hr_idx])) << endl;
                  mlog() << irsm(" hold regs hi[") << hr_idx << 
                    irsm("] = ") <<
                    int(IRS_HIBYTE(read_hr.value[hr_idx])) << endl;
                }
              );
              header.length = irs_u16(1 + size_of_resp_header + 
                m_num_of_elem*2);
            } else {
              error_response(ILLEGAL_DATA_ADDRESS);
            }
          }
          break;
          case read_input_registers:
          {
            if(m_num_of_elem != 0)
            {
              response_read_reg_t &read_ir = 
                reinterpret_cast<response_read_reg_t&>(*(mp_buf.data() + 
                size_of_MBAP));
              read_ir.byte_count = static_cast<irs_u8>(m_num_of_elem*2);
              get_bytes(m_input_regs_reg, start_addr*2, read_ir.byte_count,
                reinterpret_cast<irs_u8*>(read_ir.value), 0);
              IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
                for(size_t ir_idx = start_addr;
                  ir_idx < (start_addr + m_num_of_elem);
                  ir_idx++)
                {
                  mlog() << irsm(" input regs lo[") << ir_idx << 
                    irsm("] = ") <<
                    int(IRS_LOBYTE(read_ir.value[ir_idx])) << endl;
                  mlog() << irsm(" input regs hi[") << ir_idx << 
                    irsm("] = ") <<
                    int(IRS_HIBYTE(read_ir.value[ir_idx])) << endl;
                }
              );
              header.length = irs_u16(1 + size_of_resp_header + 
                m_num_of_elem*2);
            } else {
              error_response(ILLEGAL_DATA_ADDRESS);
            }
          }
          break;
          case write_single_coil:
          {
            response_single_write_t &response_write_coil = 
              reinterpret_cast<response_single_write_t&>(*(mp_buf.data() + 
              size_of_MBAP));
            irs_u8 coil_byte = 0;
            if(response_write_coil.address%8 == 0) {
              coil_byte =
                static_cast<irs_u8>(response_write_coil.address/8 - 1);
            } else {
              coil_byte =
                static_cast<irs_u8>(response_write_coil.address/8);
            }
            irs_u8 mask = 
              mask_gen(8 - (response_write_coil.address%8 + 1), 1);
            if(int(response_write_coil.value) == 1) {
              m_coils_byte[coil_byte] &= static_cast<irs_u8>(~mask);
              m_coils_byte[coil_byte] |= static_cast<irs_u8>(mask);
            } else {
              m_coils_byte[coil_byte] &= static_cast<irs_u8>(~mask);
            }
          }
          break;
          case write_multiple_coils:
          { 
            if(m_num_of_elem != 0)
            {
              request_multiple_write_byte_t &write_multi_coils =
                reinterpret_cast<request_multiple_write_byte_t&>(
                *(mp_buf.data() + size_of_MBAP));
              bit_copy(write_multi_coils.value, m_coils_byte.data(), 0,
                size_t(write_multi_coils.starting_address), 
                size_t(write_multi_coils.quantity));
              header.length = irs_u16(1 + 1 + size_of_read_header);
            }
            else {
              error_response(ILLEGAL_DATA_ADDRESS);
            }
          }
          break;
          case write_single_register:
          {
            response_single_write_t &response_write_reg = 
              reinterpret_cast<response_single_write_t&>(*(mp_buf.data() + 
              size_of_MBAP));
            m_hold_regs_reg[response_write_reg.address] =
              response_write_reg.value;
          }
          break;
          case write_multiple_registers:
          {
            if(m_num_of_elem != 0)
            {
              request_multiple_write_regs_t &write_multi_regs =
                reinterpret_cast<request_multiple_write_regs_t&>(
                *(mp_buf.data() + size_of_MBAP));
              set_bytes(m_hold_regs_reg, 
                write_multi_regs.starting_address*2,
                write_multi_regs.byte_count,
                reinterpret_cast<irs_u8*>(write_multi_regs.value), 0);
              header.length = irs_u16(1 + 1 + size_of_read_header);
            } else {
              error_response(ILLEGAL_DATA_ADDRESS);
            }
          }
          break;
          case read_exception_status:
          {
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("\n read_exception_status"));
          }
          break;
          default:
          {
            error_response(ILLEGAL_FUNCTION);
          }
          break;
        }
        IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_response_mode"));
        m_mode = send_response_mode;
        m_size_byte_end = header.length - 1;
      }
    }
    break;
    case send_response_mode:
    {
      IRS_LIB_IRSMBUS_DBG_MONITOR(modbus_pack_response_monitor(mp_buf.data()););
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
          response_read_reg_t &read_hold_reg =
            reinterpret_cast<response_read_reg_t&>(*(mp_buf.data() + 
            size_of_MBAP));
          convert(mp_buf.data(), size_of_MBAP + size_of_resp_header,
            read_hold_reg.byte_count);
        } break;
        case read_input_registers:
        {
          response_read_reg_t &read_input_reg =
            reinterpret_cast<response_read_reg_t&>(*(mp_buf.data() + 
            size_of_MBAP));
          convert(mp_buf.data(), size_of_MBAP + size_of_resp_header,
            read_input_reg.byte_count);
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
        case write_multiple_coils:
        {
          convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
        } break;
        case write_multiple_registers:
        {
          convert(mp_buf.data(), size_of_MBAP + 1, size_of_read_header);
        } break;
        default:
        {
          
        } break;
      }
      m_fixed_flow.write(m_channel, mp_buf.data(), size_of_MBAP + 
        m_size_byte_end);
      m_mode = write_end;
    }
    break;
    case write_end:
    {
      if(m_fixed_flow.write_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      { 
        memset(mp_buf.data(), 0, mp_buf.size());
        m_mode = read_header_mode;
        m_channel = mp_hardflow_server->channel_next();
      }
    }
    break;
  }
}

irs::modbus_client_t::modbus_client_t(
  hardflow_t* ap_hardflow,
  mxdata_ext_t::mode_refresh_t a_refresh_mode,
  size_t a_discr_inputs_size_byte,
  size_t a_coils_size_byte,
  size_t a_hold_regs_reg,
  size_t a_input_regs_reg,
  counter_t a_update_time,
  irs_u8 a_error_count_max,
  double a_disconnect_time_sec
):
  m_global_read_index(0),
  m_discret_inputs_size_bit(size_t(a_discr_inputs_size_byte*8)),
  m_coils_size_bit(size_t(a_coils_size_byte*8)),
  m_hold_registers_size_reg(a_hold_regs_reg),
  m_input_registers_size_reg(a_input_regs_reg),
  m_discret_inputs_end_byte(a_discr_inputs_size_byte),
  m_coils_end_byte(a_discr_inputs_size_byte + a_coils_size_byte),
  m_hold_registers_end_byte(a_discr_inputs_size_byte + a_coils_size_byte + 
    a_hold_regs_reg*2),
  m_input_registers_end_byte(a_discr_inputs_size_byte + a_coils_size_byte +
    a_hold_regs_reg*2 + a_input_regs_reg*2),
  m_size_byte_end(0),
  m_read_table(false),
  m_write_table(false),
  m_write_complete(true),
  m_channel(2),
  m_start_block(0),
  m_search_index(0),
  m_discret_inputs_size_byte(0),
  m_discret_inputs_start_byte(0),
  m_coils_size_byte(0),
  m_coils_start_byte(0),
  m_hold_registers_size_byte(0),
  m_hold_registers_start_byte(0),
  m_input_registers_size_byte(0),
  m_input_registers_start_byte(0),
  m_need_read(m_discret_inputs_size_bit + m_coils_size_bit + 
    a_hold_regs_reg + a_input_regs_reg),
  m_need_writes(m_discret_inputs_size_bit + a_hold_regs_reg),
  m_discr_inputs_byte_read(a_discr_inputs_size_byte),
  m_coils_byte_read(a_coils_size_byte),
  m_coils_byte_write(a_coils_size_byte),
  m_input_regs_reg_read(a_input_regs_reg),
  m_hold_regs_reg_read(a_hold_regs_reg),
  m_hold_regs_reg_write(a_hold_regs_reg),
  m_command(read_discrete_inputs),
  m_request_type(request_start),
  m_mode(wait_command_mode),
  m_loop_timer(a_update_time),
  mp_hardflow_client(ap_hardflow),
  m_fixed_flow(mp_hardflow_client),
  m_start_addr(0),
  m_coil_write_bit(0),
  m_coil_bit_index(0),
  m_first_read(irs_false),
  m_measure_time(),
  m_measure_int_time(0),
  m_operation_status(status_completed),
  m_refresh_mode(a_refresh_mode),
  m_start(irs_false),
  m_write_quantity(0),
  m_read_quantity(0),
  m_error_count_max(a_error_count_max)
{
  m_fixed_flow.read_timeout(a_disconnect_time_sec);
  m_fixed_flow.write_timeout(a_disconnect_time_sec);
  memsetex(m_spacket,IRS_ARRAYSIZE(m_spacket));
  memsetex(m_rpacket,IRS_ARRAYSIZE(m_rpacket));
  init_to_cnt();
  m_measure_time.start();
  IRS_LIB_IRSMBUS_DBG_OPERATION_TIME(mlog() <<
    irsm(" IRS_MBUS_DBG_OPERATION_TIME") << endl;);
  mlog() << irsm("CLIENT START!") << endl;
}

irs::modbus_client_t::~modbus_client_t()
{
  deinit_to_cnt();
}

irs_uarc irs::modbus_client_t::size()
{
  return m_input_registers_end_byte;
}

irs_bool irs::modbus_client_t::connected()
{
  irs_bool connect = irs_false;
  if(m_refresh_mode == mode_refresh_auto) {
    if(m_first_read) {
      connect = irs_true;
    } else {
      connect = irs_false;
    }
  } else if(m_refresh_mode == mode_refresh_manual) {
    connect = irs_true;
  }
  if((m_fixed_flow.write_status() == 
    irs::hardflow::fixed_flow_t::status_error) ||
    (m_fixed_flow.read_status() == 
    irs::hardflow::fixed_flow_t::status_error))
  {
    connect = irs_false;
  }
  return connect;
}

void irs::modbus_client_t::modbus_pack_request_monitor(irs_u8 *ap_buf)
{
  mlog() << irsm("\n send packet") << endl;
  MBAP_header_t &header =
    reinterpret_cast<MBAP_header_t&>(*ap_buf);
  request_exception_t &req_header =
    reinterpret_cast<request_exception_t&>(*(ap_buf + size_of_MBAP));
    
  mlog() << irsm("\n ----------- MODBUS's Header----------------\n");
  mlog() << irsm(" transaction_id_lo .... ") <<
    int(IRS_LOBYTE(header.transaction_id)) << endl;
  mlog() << irsm(" transaction_id_hi .... ") <<
    int(IRS_HIBYTE(header.transaction_id)) << endl;
  mlog() << irsm(" proto_id_lo .......... ") <<
    int(IRS_LOBYTE(header.proto_id)) << endl;
  mlog() << irsm(" proto_id_hi .......... ") <<
    int(IRS_HIBYTE(header.proto_id)) << endl;
  mlog() << irsm(" length_lo ............ ") <<
    int(IRS_LOBYTE(header.length)) << endl;
  mlog() << irsm(" length_hi ............ ") <<
    int(IRS_HIBYTE(header.length)) << endl;
  mlog() << irsm(" unit_identifier ...... ") <<
    int(header.unit_identifier) << endl;
    
  mlog() << irsm(" function_code ........ ") <<
    int(req_header.function_code);
  switch(req_header.function_code)
  {
    case read_discrete_inputs:
    {
      request_t &req_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read discret inputs") << endl;
      mlog() << irsm(" starting_address_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_lo .......... ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_hi .......... ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
    } break;
    case read_coils:
    {
      request_t &req_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read coils") << endl;
      mlog() << irsm(" starting_address_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_lo .......... ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_hi .......... ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
    } break;
    case read_hold_registers:
    {
      request_t &req_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read hold registers") << endl;
      mlog() << irsm(" starting_address_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_lo .......... ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_hi .......... ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
    } break;
    case read_input_registers:
    {
      request_t &req_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read input registers") << endl;
      mlog() << irsm(" starting_address_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_lo .......... ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_hi .......... ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
    } break;
    case write_single_coil:
    {
      request_t &req_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" write single coil") << endl;
      mlog() << irsm(" output_address_lo .... ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" output_address_hi .... ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" output_value_lo ...... ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" output_value_hi ...... ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
    } break;
    case write_single_register:
    {
      response_single_write_t &req_header_inner = 
        reinterpret_cast<response_single_write_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" write single register") << endl;
      mlog() << irsm(" register_address_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.address)) << endl;
      mlog() << irsm(" register_address_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.address)) << endl;
      mlog() << irsm(" register_value_lo .... ") <<
        int(IRS_LOBYTE(req_header_inner.value)) << endl;
      mlog() << irsm(" register_value_hi .... ") <<
        int(IRS_HIBYTE(req_header_inner.value)) << endl;
    } break;
    case read_exception_status:
    {
      mlog() << irsm(" read exception status ") << endl;
    } break;
    case write_multiple_coils:
    {
      request_multiple_write_byte_t &req_header_inner = 
        reinterpret_cast<request_multiple_write_byte_t&>(
        *(ap_buf + size_of_MBAP));
      mlog() << irsm(" write multiple coils") << endl;
      mlog() << irsm(" starting_address_lo ..... ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi ..... ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_of_outputs_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_of_outputs_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" byte_count .............. ") <<
        int(req_header_inner.byte_count) << endl;
      int coils_idx_max = 0;
      if(req_header_inner.quantity%8 == 0)
        coils_idx_max = req_header_inner.quantity/8;
      else
        coils_idx_max = req_header_inner.quantity/8 + 1;
      for(int coils_idx = 0; coils_idx < coils_idx_max; coils_idx++) {
        mlog() << irsm(" coils_value ........... ") <<
          int(req_header_inner.value[coils_idx]) << endl;
      }
    } break;
    case write_multiple_registers:
    {
      request_multiple_write_regs_t &req_header_inner = 
        reinterpret_cast<request_multiple_write_regs_t&>(
        *(ap_buf + size_of_MBAP));
      mlog() << irsm(" write multiple registers") << endl;
      mlog() << irsm(" starting_address_lo ....... ") <<
        int(IRS_LOBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi ....... ") <<
        int(IRS_HIBYTE(req_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_of_registers_lo .. ") <<
        int(IRS_LOBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_of_registers_hi .. ") <<
        int(IRS_HIBYTE(req_header_inner.quantity)) << endl;
      mlog() << irsm(" byte_count ................ ") <<
        int(req_header_inner.byte_count) << endl;
      for(int reg_idx = 0; reg_idx < int(req_header_inner.quantity); reg_idx++)
      {
        mlog() << irsm(" register_value_lo ......... ") <<
          int(IRS_LOBYTE(req_header_inner.value[reg_idx])) << endl;
        mlog() << irsm(" register_value_hi ......... ") <<
          int(IRS_HIBYTE(req_header_inner.value[reg_idx])) << endl;
      }
    } break;
    default:
    {
      mlog() << irsm(" ILLEGAL_FUNCTION") << endl;
    } break;
  }
}

void irs::modbus_client_t::modbus_pack_response_monitor(irs_u8 *ap_buf)
{
  // � IAR ��������� ��������������, ��� ap_buf �� ������������,
  // ���� ��������� ���������. ��� ���������� ��������������
  // ������������ ����� ����������� ����
  ap_buf = ap_buf;
  mlog() << irsm("\n recieved packet") << endl;
  MBAP_header_t &header = 
    reinterpret_cast<MBAP_header_t&>(*ap_buf);
  request_exception_t &resp_header = 
    reinterpret_cast<request_exception_t&>(*(ap_buf + size_of_MBAP));
    
  mlog() << irsm("\n ----------- MODBUS's Header----------------\n");
  mlog() << irsm(" transaction_id_lo .... ") <<
    int(IRS_LOBYTE(header.transaction_id)) << endl;
  mlog() << irsm(" transaction_id_hi .... ") <<
    int(IRS_HIBYTE(header.transaction_id)) << endl;
  mlog() << irsm(" proto_id_lo .......... ") <<
    int(IRS_LOBYTE(header.proto_id)) << endl;
  mlog() << irsm(" proto_id_hi .......... ") <<
    int(IRS_HIBYTE(header.proto_id)) << endl;
  mlog() << irsm(" length_lo ............ ") <<
    int(IRS_LOBYTE(header.length)) << endl;
  mlog() << irsm(" length_hi ............ ") <<
    int(IRS_HIBYTE(header.length)) << endl;
  mlog() << irsm(" unit_identifier ...... ") <<
    int(header.unit_identifier) << endl;

  mlog() << irsm(" function_code ........ ") <<
    int(resp_header.function_code);
  switch(resp_header.function_code)
  {
    case read_discrete_inputs:
    {
      response_read_byte_t &resp_header_inner = 
        reinterpret_cast<response_read_byte_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read discret inputs") << endl;
      mlog() << irsm(" byte_count ........... ") <<
        int(resp_header_inner.byte_count) << endl;
      for(int di_idx = 0; di_idx < resp_header_inner.byte_count; di_idx++) {
        mlog() << irsm(" discret input[") << di_idx << irsm("] ..... ") <<
          int(resp_header_inner.value[di_idx]) << endl;
      }
    } break;
    case read_coils:
    {
      response_read_byte_t &resp_header_inner = 
        reinterpret_cast<response_read_byte_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read coils") << endl;
      mlog() << irsm(" byte_count ........... ") <<
        int(resp_header_inner.byte_count) << endl;
      for(int coils_idx = 0; coils_idx < resp_header_inner.byte_count;
        coils_idx++)
      {
        mlog() << irsm(" coils[") << coils_idx << 
          irsm("] ............. ") <<
          int(resp_header_inner.value[coils_idx]) << endl;
      }
    } break;
    case read_hold_registers:
    {
      response_read_reg_t &resp_header_inner = 
        reinterpret_cast<response_read_reg_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read hold registers") << endl;
      mlog() << irsm(" byte_count ........... ") <<
        int(resp_header_inner.byte_count) << endl;
      for(int h_reg_idx = 0; h_reg_idx < int(resp_header_inner.byte_count)/2;
        h_reg_idx++) 
      {
        mlog() << irsm(" hold regs lo[") << h_reg_idx << 
          irsm("] ...... ") << 
          int(IRS_LOBYTE(resp_header_inner.value[h_reg_idx])) << endl;
        mlog() << irsm(" hold regs hi[") << h_reg_idx << 
          irsm("] ...... ") <<
          int(IRS_HIBYTE(resp_header_inner.value[h_reg_idx])) << endl;
      }
    } break;
    case read_input_registers:
    {
      response_read_reg_t &resp_header_inner = 
        reinterpret_cast<response_read_reg_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" read input registers") << endl;
      mlog() << irsm(" byte_count ........... ") <<
        int(resp_header_inner.byte_count) << endl;
      for(int i_reg_idx = 0; i_reg_idx < int(resp_header_inner.byte_count)/2;
        i_reg_idx++) 
      {
        mlog() << irsm(" input regs lo[") << i_reg_idx <<
          irsm("] ..... ") <<
          int(IRS_LOBYTE(resp_header_inner.value[i_reg_idx])) << endl;
        mlog() << irsm(" input regs hi[") << i_reg_idx << 
          irsm("] ..... ") <<
          int(IRS_HIBYTE(resp_header_inner.value[i_reg_idx])) << endl;
      }
    } break;
    case write_single_coil:
    {
      response_single_write_t &resp_header_inner = 
        reinterpret_cast<response_single_write_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" write single coil") << endl;
      mlog() << irsm(" output_address_lo .... ") <<
        int(IRS_LOBYTE(resp_header_inner.address)) << endl;
      mlog() << irsm(" output_address_hi .... ") <<
        int(IRS_HIBYTE(resp_header_inner.address)) << endl;
      mlog() << irsm(" output_value_lo ...... ") <<
        int(IRS_LOBYTE(resp_header_inner.value)) << endl;
      mlog() << irsm(" output_value_hi ...... ") <<
        int(IRS_HIBYTE(resp_header_inner.value)) << endl;
    } break;
    case write_single_register:
    {
      response_single_write_t &resp_header_inner = 
        reinterpret_cast<response_single_write_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" write single register") << endl;
      mlog() << irsm(" output_address_lo .... ") <<
        int(IRS_LOBYTE(resp_header_inner.address)) << endl;
      mlog() << irsm(" output_address_hi .... ") <<
        int(IRS_HIBYTE(resp_header_inner.address)) << endl;
      mlog() << irsm(" output_value_lo ...... ") <<
        int(IRS_LOBYTE(resp_header_inner.value)) << endl;
      mlog() << irsm(" output_value_hi ...... ") <<
        int(IRS_HIBYTE(resp_header_inner.value)) << endl;
    } break;
    case read_exception_status:
    {
      mlog() << irsm(" read exception status ") << endl;
      response_exception_t &resp_header_inner = 
        reinterpret_cast<response_exception_t&>(
        *(ap_buf + size_of_MBAP));
      mlog() << irsm(" error_code ........... ") <<
        int(resp_header_inner.error_code) << endl;
      mlog() << irsm(" exeption_code ........ ") <<
        int(resp_header_inner.exeption_code) << endl;
    } break;
    case write_multiple_coils:
    {
      request_t &resp_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" write multiple coils") << endl;
      mlog() << irsm(" starting_address_lo .. ") <<
        int(IRS_LOBYTE(resp_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi .. ") <<
        int(IRS_HIBYTE(resp_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_lo .......... ") <<
        int(IRS_LOBYTE(resp_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_hi .......... ") <<
        int(IRS_HIBYTE(resp_header_inner.quantity)) << endl;
    } break;
    case write_multiple_registers:
    {
      request_t &resp_header_inner = 
        reinterpret_cast<request_t&>(*(ap_buf + size_of_MBAP));
      mlog() << irsm(" write multiple registers") << endl;
      mlog() << irsm(" starting_address_lo .. ") <<
        int(IRS_LOBYTE(resp_header_inner.starting_address)) << endl;
      mlog() << irsm(" starting_address_hi .. ") <<
        int(IRS_HIBYTE(resp_header_inner.starting_address)) << endl;
      mlog() << irsm(" quantity_lo .......... ") <<
        int(IRS_LOBYTE(resp_header_inner.quantity)) << endl;
      mlog() << irsm(" quantity_hi .......... ") <<
        int(IRS_HIBYTE(resp_header_inner.quantity)) << endl;
    } break;
    default:
    {
      mlog() << irsm(" ILLEGAL_FUNCTION") << endl;
    } break;
  }
}

void irs::modbus_client_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm("read"));
  m_discret_inputs_size_byte = 0; m_discret_inputs_start_byte = 0;
  m_coils_size_byte = 0; m_coils_start_byte = 0;
  m_hold_registers_size_byte = 0; m_hold_registers_start_byte = 0;
  m_input_registers_size_byte = 0; m_input_registers_start_byte = 0;

  range(a_index, a_size, 0, m_discret_inputs_end_byte, 
    &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
  range(a_index, a_size, m_discret_inputs_end_byte, m_coils_end_byte,
    &m_coils_size_byte, &m_coils_start_byte);
  range(a_index, a_size, m_coils_end_byte, m_hold_registers_end_byte, 
    &m_hold_registers_size_byte, &m_hold_registers_start_byte);
  range(a_index, a_size, m_hold_registers_end_byte, m_input_registers_end_byte, 
    &m_input_registers_size_byte, &m_input_registers_start_byte);
  
  size_t idx = 0;
  if((m_discret_inputs_size_byte != 0) || (m_discret_inputs_start_byte != 0))
  {
    mlog() << irsm("read di") << endl;
    for(; idx < m_discret_inputs_size_byte; idx++)
    {
      ap_buf[idx] = m_discr_inputs_byte_read[idx];
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
        mlog() << irsm("discret inputs[") << idx << irsm("] = ") <<
          (int)m_discr_inputs_byte_read[idx] << endl;
      );
    }
  }
  if((m_coils_size_byte != 0) || (m_coils_start_byte != 0))
  {
    mlog() << irsm("read coils") << endl;
    for(; idx < static_cast<size_t>(m_discret_inputs_size_byte + 
      m_coils_size_byte); idx++)
    {
      ap_buf[idx] = m_coils_byte_read[m_coils_start_byte + idx - 
        m_discret_inputs_size_byte];
      mlog() << irsm(" ap_buf[") << idx << irsm("] = ") <<
        int(ap_buf[idx]) << endl;
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
        mlog() << irsm("coils[") << m_coils_start_byte + idx -  
          m_discret_inputs_size_byte << irsm("] = ") <<
          (int)m_coils_byte_read[m_coils_start_byte + idx - 
          m_discret_inputs_size_byte] << endl;
      );
    }
  }
  if((m_hold_registers_size_byte != 0) || (m_hold_registers_start_byte != 0))
  {
    mlog() << irsm("read hr") << endl;
    size_t hr_size_byte = 0;
    if((m_hold_registers_size_byte%2 == 0) &&
      (m_hold_registers_start_byte%2 == 0))
    {
      hr_size_byte = m_hold_registers_size_byte;
    } else if((m_hold_registers_size_byte%2 == 0) && 
      (m_hold_registers_start_byte%2 != 0)) 
    {
      hr_size_byte = m_hold_registers_size_byte + 2;
    } else {
      hr_size_byte = m_hold_registers_size_byte + 1;
    }
    m_hold_registers_size_byte = hr_size_byte;//?
    size_t hr_index = 0;
    if(m_hold_registers_start_byte%2 == 0) {
      hr_index = m_hold_registers_start_byte;
    } else {
      hr_index = m_hold_registers_start_byte - 1;
    }
    IRS_LIB_ASSERT((m_discret_inputs_size_byte + m_coils_size_byte +
      hr_index + hr_size_byte) <= (m_discret_inputs_size_bit/8 + 
      m_coils_size_bit/8 + m_hold_registers_size_reg*2 + 
      m_input_registers_size_reg*2));
    /*get_bytes(m_hold_regs_reg_read, 0, hr_size_byte, ap_buf, 
      m_discret_inputs_size_byte + m_coils_size_byte + hr_index);*/
    get_bytes(m_hold_regs_reg_read, hr_index, m_hold_registers_size_byte, 
      ap_buf, m_discret_inputs_size_byte + m_coils_size_byte);
    IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
      for(size_t hr_idx = 0; hr_idx < hr_size_byte/2; hr_idx++)
      {
        mlog() << irsm("hold regs lo[") << hr_idx << irsm("] = ") <<
          int(IRS_LOBYTE(m_hold_regs_reg_read[hr_idx])) << endl; 
        mlog() << irsm("hold regs hi[") << hr_idx << irsm("] = ") <<
          int(IRS_HIBYTE(m_hold_regs_reg_read[hr_idx])) << endl;  
      }
    );
  }
  if((m_input_registers_size_byte != 0) || (m_input_registers_start_byte != 0))
  {
    mlog() << irsm("read ir") << endl;
    size_t ir_size_byte = 0;
    if((m_input_registers_size_byte%2 == 0) &&
      (m_input_registers_start_byte%2 == 0))
    {
      ir_size_byte = m_input_registers_size_byte;
    } else if((m_input_registers_size_byte%2 == 0) && 
      (m_input_registers_start_byte%2 != 0)) 
    {
      ir_size_byte = m_input_registers_size_byte + 2;
    } else {
      ir_size_byte = m_input_registers_size_byte + 1;
    }
    size_t ir_index = 0;
    if(m_input_registers_start_byte%2 == 0) {
      ir_index = m_input_registers_start_byte;
    } else {
      ir_index = m_input_registers_start_byte - 1;
    }
    IRS_LIB_ASSERT((m_discret_inputs_size_byte + m_coils_size_byte + 
      m_hold_registers_size_reg*2 + ir_index + ir_size_byte) <= 
      (m_discret_inputs_size_bit/8 + m_coils_size_bit/8 + 
      m_hold_registers_size_reg*2 + m_input_registers_size_reg*2));
    get_bytes(m_input_regs_reg_read, ir_index, 
      ir_size_byte, ap_buf, m_discret_inputs_size_byte + 
      m_coils_size_byte + m_hold_registers_size_byte);
    IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
      for(size_t ir_idx = 0; ir_idx < ir_size_byte/2; ir_idx++) {
        mlog() << irsm("input regs lo[") << ir_idx << irsm("] = ") <<
          int(IRS_LOBYTE(m_input_regs_reg_read[ir_idx])) << endl;  
        mlog() << irsm("input regs hi[") << ir_idx << irsm("] = ") <<
          int(IRS_HIBYTE(m_input_regs_reg_read[ir_idx])) << endl;  
      }
    );
  }
}

void irs::modbus_client_t::write(const irs_u8 *ap_buf, irs_uarc a_index, 
irs_uarc a_size)
{
  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" write"));
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  m_hold_registers_size_byte = 0, m_hold_registers_start_byte = 0;
  range(a_index, a_size, m_discret_inputs_end_byte, m_coils_end_byte, 
    &m_coils_size_byte, &m_coils_start_byte);
  range(a_index, a_size, m_coils_end_byte, m_hold_registers_end_byte, 
    &m_hold_registers_size_byte, &m_hold_registers_start_byte);
  if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
  {
    IRS_LIB_ASSERT((m_coils_start_byte + m_coils_size_byte) <= 
      m_coils_byte_write.size());
    bit_copy(ap_buf, m_coils_byte_write.data(), 
      m_discret_inputs_size_bit + m_coils_start_byte*8, m_coils_start_byte*8, 
      m_coils_size_byte*8);
    IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
      for(size_t coils_idx = m_coils_start_byte;
        coils_idx < (m_coils_start_byte + m_coils_size_byte); coils_idx++) 
      {
        mlog() << irsm("ap_buf[") << coils_idx << irsm("] = ") <<
          (int)ap_buf[m_discret_inputs_size_bit/8 + coils_idx] << endl;
        mlog() << irsm("coils[") << coils_idx << irsm("] = ") <<
          (int)m_coils_byte_write[coils_idx] << endl;
      }
    );
    if(m_refresh_mode == mode_refresh_auto) {
      for(size_t i = m_coils_start_byte*8;
        i < (m_coils_start_byte*8 + m_coils_size_byte*8); i++)
      {
        m_need_writes[i] = 1;
      }
    }
  }
  if((m_hold_registers_size_byte != 0) || (m_hold_registers_start_byte != 0))
  {
    size_t hr_size_byte = 0;
    if((m_hold_registers_size_byte%2 == 0) &&
      (m_hold_registers_start_byte%2 == 0)) 
    {
      hr_size_byte = m_hold_registers_size_byte;
    } else if((m_hold_registers_size_byte%2 == 0) && 
      (m_hold_registers_start_byte%2 != 0)) 
    {
      hr_size_byte = m_hold_registers_size_byte + 2;
    } else {
      hr_size_byte = m_hold_registers_size_byte + 1;
    }
    size_t hr_index = 0;
    if(m_hold_registers_start_byte%2 == 0) {
      hr_index = m_hold_registers_start_byte;
    } else {
      hr_index = m_hold_registers_start_byte - 1;
    }
    IRS_LIB_ASSERT(((hr_index + hr_size_byte)/2) <= 
      m_hold_regs_reg_write.size());
    set_bytes(m_hold_regs_reg_write, hr_index, hr_size_byte, ap_buf,
      m_discret_inputs_size_bit/8 + m_coils_size_bit/8 + hr_index);
    IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
      for(size_t hr_idx = hr_index/2; 
        hr_idx < (hr_index/2 + hr_size_byte/2); hr_idx++)
      {
        mlog() << irsm("hold regs lo[") << hr_idx << irsm("] = ") <<
          int(IRS_LOBYTE(m_hold_regs_reg_write[hr_idx])) << endl;
        mlog() << irsm("hold regs hi[") << hr_idx << irsm("] = ") <<
          int(IRS_HIBYTE(m_hold_regs_reg_write[hr_idx])) << endl;
      }
    );
    size_t hr_size = 0;
    if((m_hold_registers_size_byte%2 == 0) && 
      (m_hold_registers_start_byte%2 == 0))
    {
      hr_size = m_hold_registers_size_byte/2;
    } else {
      hr_size = m_hold_registers_size_byte/2 + 1;
    }
    
    if(m_refresh_mode == mode_refresh_auto) {
      for(size_t i = m_coils_size_bit + m_hold_registers_start_byte/2;
        i < m_coils_size_bit + m_hold_registers_start_byte/2 + hr_size; i++) 
      {
        m_need_writes[i] = 1;
      }
    }                    
  }
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    for(size_t write_idx = 0;
      write_idx < (m_coils_size_bit/8 + m_hold_registers_size_reg); write_idx++) 
    {
      mlog() << irsm("m_need_writes[") << write_idx << irsm("] = ") <<
        (int)(m_need_writes[write_idx]) << endl;
    }
  );
  if(m_refresh_mode == mode_refresh_auto) {
    m_operation_status = status_wait;
  }
}

irs_bool irs::modbus_client_t::bit(irs_uarc a_byte_index, irs_uarc a_bit_index)
{
  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm("read bit"));
  IRS_LIB_ASSERT(a_bit_index < 8);
  m_hold_registers_size_byte = 0; m_hold_registers_start_byte = 0;
  m_coils_size_byte = 0; m_coils_start_byte = 0;
  m_discret_inputs_start_byte = 0; m_discret_inputs_size_byte = 0;
  m_input_registers_start_byte = 0; m_input_registers_size_byte = 0;
  
  range(a_byte_index, 1, 0, m_discret_inputs_end_byte, 
    &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
  range(a_byte_index, 1, m_discret_inputs_end_byte, m_coils_end_byte, 
    &m_coils_size_byte, &m_coils_start_byte);
  range(a_byte_index, 1, m_coils_end_byte, m_hold_registers_end_byte, 
    &m_hold_registers_size_byte, &m_hold_registers_start_byte);
  range(a_byte_index, 1, m_hold_registers_end_byte, m_input_registers_end_byte, 
    &m_input_registers_size_byte, &m_input_registers_start_byte);
    
  irs_u8 mask = mask_gen(8 - (a_bit_index + 1), 1);  
  if (a_bit_index < 8) {
    if ((m_discret_inputs_size_byte != 0) || (m_discret_inputs_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" read bit: discret inputs"));
      irs_u8 di_byte_read_bit = 
        m_discr_inputs_byte_read[m_discret_inputs_start_byte];
      di_byte_read_bit &= static_cast<irs_u8>(mask);
      return to_irs_bool(di_byte_read_bit);
    }
    if ((m_coils_size_byte != 0) || (m_coils_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" read bit: coils"));
      irs_u8 coils_byte_read_bit = m_coils_byte_read[m_coils_start_byte];
      coils_byte_read_bit &= static_cast<irs_u8>(mask);
      return to_irs_bool(coils_byte_read_bit);
    }
    if ((m_hold_registers_size_byte != 0) || (m_hold_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" read bit: hold regs"));
      irs_u8 hr_byte_read_bit = 0;
      if(m_hold_registers_start_byte%2 == 0) 
        hr_byte_read_bit =
          IRS_LOBYTE(m_hold_regs_reg_read[m_hold_registers_start_byte/2]);
      else 
        hr_byte_read_bit = 
          IRS_HIBYTE(m_hold_regs_reg_read[m_hold_registers_start_byte/2]);
      hr_byte_read_bit &= static_cast<irs_u8>(mask);
      return to_irs_bool(hr_byte_read_bit);
    }
    if ((m_input_registers_size_byte != 0) || 
      (m_input_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" read bit: input regs"));
      irs_u8 ir_byte_read_bit = 0;
      if(m_input_registers_start_byte%2 == 0) 
        ir_byte_read_bit = 
          IRS_LOBYTE(m_input_regs_reg_read[m_input_registers_start_byte/2]);
      else 
        ir_byte_read_bit = 
          IRS_HIBYTE(m_input_regs_reg_read[m_input_registers_start_byte/2]);
      ir_byte_read_bit &= static_cast<irs_u8>(mask);
      return to_irs_bool(ir_byte_read_bit);
    }
  }
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    if((m_coils_size_byte == 0) && (m_discret_inputs_size_byte == 0) &&
      (m_hold_registers_size_byte == 0) && (m_input_registers_size_byte == 0))
    {
      mlog() << irsm("\n***************Illegal index or bufer size"
        "******************\n");
    }
  );
  return irs_false;
}

void irs::modbus_client_t::set_bit(irs_uarc a_byte_index, irs_uarc a_bit_index)
{
  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" set_bit"));
  IRS_LIB_ASSERT(a_bit_index < 8);
  m_hold_registers_size_byte = 0;
  m_hold_registers_start_byte = 0;
  m_coils_size_byte = 0;
  m_coils_start_byte = 0;
  
  range(a_byte_index, 1, 0, m_discret_inputs_end_byte, 
    &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
  range(a_byte_index, 1, m_discret_inputs_end_byte, m_coils_end_byte, 
    &m_coils_size_byte, &m_coils_start_byte);
  range(a_byte_index, 1, m_coils_end_byte, m_hold_registers_end_byte, 
    &m_hold_registers_size_byte, &m_hold_registers_start_byte);
  range(a_byte_index, 1, m_hold_registers_end_byte, m_input_registers_end_byte, 
    &m_input_registers_size_byte, &m_input_registers_start_byte);
    
  irs_u8 mask = mask_gen(8 - (a_bit_index + 1), 1);
  
  if (a_bit_index < 8) {
    if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" set bit: coils"));
      m_coil_write_bit = 1;
      m_coil_bit_index = a_byte_index*8 - m_discret_inputs_size_bit + 
        a_bit_index;
      if(m_refresh_mode == mode_refresh_auto) {
        m_need_writes[m_coil_bit_index] = 1;
      }
    }
    if((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" set bit: hold regs"));
      m_hold_regs_reg_write[0] = 
        m_hold_regs_reg_read[m_hold_registers_start_byte/2];
      irs_u8 hr_byte_set_bit = 0;
      if(m_hold_registers_start_byte%2 == 0) {
        hr_byte_set_bit = 
          IRS_LOBYTE(m_hold_regs_reg_read[m_hold_registers_start_byte/2]);
      } else {
        hr_byte_set_bit = 
          IRS_HIBYTE(m_hold_regs_reg_read[m_hold_registers_start_byte/2]);
      }
      hr_byte_set_bit &= static_cast<irs_u8>(~mask);
      hr_byte_set_bit |= static_cast<irs_u8>(mask);
      if(m_hold_registers_start_byte%2 == 0) {
        IRS_LOBYTE(m_hold_regs_reg_write[0]) = hr_byte_set_bit;
      } else { 
        IRS_HIBYTE(m_hold_regs_reg_write[0]) = hr_byte_set_bit;
      }
      if(m_refresh_mode == mode_refresh_auto) {
        m_need_writes[m_coils_size_bit + m_hold_registers_start_byte/2] = 1;
      }
    }
  }
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    if((m_coils_size_byte == 0)&&(m_hold_registers_size_byte == 0)) {
      mlog() << irsm("ERROR\n");
    }
  );
}

void irs::modbus_client_t::clear_bit(irs_uarc a_byte_index, 
  irs_uarc a_bit_index)
{
  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm("clear_bit"));
  IRS_LIB_ASSERT(a_bit_index < 8);
  m_hold_registers_size_byte = 0;
  m_hold_registers_start_byte = 0;
  m_coils_size_byte = 0;
  m_coils_start_byte = 0;
  range(a_byte_index, 1, 0, m_discret_inputs_end_byte, 
    &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
  range(a_byte_index, 1, m_discret_inputs_end_byte, m_coils_end_byte, 
    &m_coils_size_byte, &m_coils_start_byte);
  range(a_byte_index, 1, m_coils_end_byte, m_hold_registers_end_byte, 
    &m_hold_registers_size_byte, &m_hold_registers_start_byte);
  range(a_byte_index, 1, m_hold_registers_end_byte, m_input_registers_end_byte, 
    &m_input_registers_size_byte, &m_input_registers_start_byte);
  irs_u8 mask = mask_gen(8 - (a_bit_index + 1), 1);
  if (a_bit_index < 8) {
    if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" clear bit: coils"));
      m_coil_write_bit = 0;
      m_coil_bit_index = a_byte_index*8 - m_discret_inputs_size_bit + 
        a_bit_index;
      if(m_refresh_mode == mode_refresh_auto) {
        m_need_writes[m_coil_bit_index] = irs_true;
      }
    }
    if((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" clear bit: hold regs"));
      m_hold_regs_reg_write[0] = 
        m_hold_regs_reg_read[m_hold_registers_start_byte/2];
      irs_u8 hr_byte_clear_bit = 0;
      if(m_hold_registers_start_byte%2 == 0) {
        hr_byte_clear_bit = 
          IRS_LOBYTE(m_hold_regs_reg_read[m_hold_registers_start_byte/2]);
      }
      else {
        hr_byte_clear_bit = 
          IRS_HIBYTE(m_hold_regs_reg_read[m_hold_registers_start_byte/2]);
      }
      hr_byte_clear_bit &= static_cast<irs_u8>(~mask);
      if(m_hold_registers_start_byte%2 == 0) 
        IRS_LOBYTE(m_hold_regs_reg_write[0]) = hr_byte_clear_bit;
      else 
        IRS_HIBYTE(m_hold_regs_reg_write[0]) = hr_byte_clear_bit;
      if(m_refresh_mode == mode_refresh_auto) {
        m_need_writes[m_coils_size_bit + m_hold_registers_start_byte/2] = 
          irs_true;
      }
    }
  }
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    if((m_coils_size_byte == 0)&&(m_hold_registers_size_byte == 0)) {
      mlog() << irsm("ERROR");
    }
  );
}

void irs::modbus_client_t::mark_to_send(irs_uarc a_index, irs_uarc a_size)
{
  if(((m_refresh_mode == mode_refresh_manual) && 
    (m_operation_status == status_completed)) ||
    (m_refresh_mode == mode_refresh_auto))
  {
    m_coils_size_byte = 0, m_coils_start_byte = 0;
    m_hold_registers_size_byte = 0, m_hold_registers_start_byte = 0;

    range(a_index, a_size, m_discret_inputs_end_byte, m_coils_end_byte, 
      &m_coils_size_byte, &m_coils_start_byte);
    range(a_index, a_size, m_coils_end_byte, m_hold_registers_end_byte, 
      &m_hold_registers_size_byte, &m_hold_registers_start_byte);

    if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
    {
      for(size_t i = m_coils_start_byte*8; 
        i < (m_coils_start_byte*8 + m_coils_size_byte*8); i++)
      {
        m_need_writes[i] = 1;
      }
    }
    if((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
    {
      size_t hr_size_byte = 0;
      if((m_hold_registers_size_byte%2 == 0) &&
        (m_hold_registers_start_byte%2 == 0)) {
        hr_size_byte = m_hold_registers_size_byte;
      } else if((m_hold_registers_size_byte%2 == 0) && 
        (m_hold_registers_start_byte%2 != 0)) {
        hr_size_byte = m_hold_registers_size_byte + 2;
      } else {
        hr_size_byte = m_hold_registers_size_byte + 1;
      }
      size_t hr_index = 0;
      if(m_hold_registers_start_byte%2 == 0) {
        hr_index = m_hold_registers_start_byte;
      } else {
        hr_index = m_hold_registers_start_byte - 1;
      }
      for(size_t i = m_coils_size_bit + hr_index/2;
        i < (m_coils_size_bit + hr_index/2 + hr_size_byte/2); i++) 
      {
        m_need_writes[i] = 1;
      }
    }
    if(m_refresh_mode == mode_refresh_auto) {
      m_operation_status = status_wait;
    }
  }
}

void irs::modbus_client_t::mark_to_recieve(irs_uarc a_index, irs_uarc a_size)
{
  if(((m_refresh_mode == mode_refresh_manual) && 
    (m_operation_status == status_completed)) ||
    (m_refresh_mode == mode_refresh_auto))
  {
    m_discret_inputs_size_byte = 0; m_discret_inputs_start_byte = 0;
    m_coils_size_byte = 0; m_coils_start_byte = 0;
    m_hold_registers_size_byte = 0; m_hold_registers_start_byte = 0;
    m_input_registers_size_byte = 0; m_input_registers_start_byte = 0;

    range(a_index, a_size, 0, m_discret_inputs_end_byte, 
      &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
    range(a_index, a_size, m_discret_inputs_end_byte, m_coils_end_byte,
      &m_coils_size_byte, &m_coils_start_byte);
    range(a_index, a_size, m_coils_end_byte, m_hold_registers_end_byte, 
      &m_hold_registers_size_byte, &m_hold_registers_start_byte);
    range(a_index, a_size, m_hold_registers_end_byte, 
      m_input_registers_end_byte, &m_input_registers_size_byte,
      &m_input_registers_start_byte);
      
    size_t read_index = 0;
    if(m_discret_inputs_size_byte != 0)
    {
      for(read_index = m_discret_inputs_start_byte*8;
        read_index < (m_discret_inputs_start_byte*8 + 
        m_discret_inputs_size_byte*8); read_index ++)
      {
        m_need_read[read_index] = 1;
      }
    }
    if(m_coils_size_byte != 0)
    {
      for(read_index = m_discret_inputs_size_bit + m_coils_start_byte*8;
        read_index < (m_discret_inputs_size_bit + m_coils_start_byte*8 +
        m_coils_size_byte*8); read_index ++)
      {
        m_need_read[read_index] = 1;
      }
    }
    if(m_hold_registers_size_byte != 0)
    {
      size_t hr_size_byte = 0;
      if((m_hold_registers_size_byte%2 == 0) &&
        (m_hold_registers_start_byte%2 == 0)) {
        hr_size_byte = m_hold_registers_size_byte;
      } else if((m_hold_registers_size_byte%2 == 0) && 
        (m_hold_registers_start_byte%2 != 0)) {
        hr_size_byte = m_hold_registers_size_byte + 2;
      } else {
        hr_size_byte = m_hold_registers_size_byte + 1;
      }
      size_t hr_index = 0;
      if(m_hold_registers_start_byte%2 == 0) {
        hr_index = m_hold_registers_start_byte;
      } else {
        hr_index = m_hold_registers_start_byte - 1;
      }
      for(read_index = m_discret_inputs_size_bit + m_coils_size_bit + 
        hr_index/2; read_index < (m_discret_inputs_size_bit + 
        m_coils_size_bit + hr_index/2 + hr_size_byte/2); read_index ++)
      {
        m_need_read[read_index] = 1;
      }
    }
    if(m_input_registers_size_byte != 0)
    {
      size_t ir_size_byte = 0;
      if((m_input_registers_size_byte%2 == 0) &&
        (m_input_registers_start_byte%2 == 0)) {
        ir_size_byte = m_input_registers_size_byte;
      } else if((m_input_registers_size_byte%2 == 0) && 
        (m_input_registers_start_byte%2 != 0)) {
        ir_size_byte = m_input_registers_size_byte + 2;
      } else {
        ir_size_byte = m_input_registers_size_byte + 1;
      }
      size_t ir_index = 0;
      if(m_input_registers_start_byte%2 == 0) {
        ir_index = m_input_registers_start_byte;
      } else {
        ir_index = m_input_registers_start_byte - 1;
      }
      for(read_index = m_discret_inputs_size_bit + m_coils_size_bit +
        m_hold_registers_size_reg + ir_index/2;
        read_index < (m_discret_inputs_size_bit + m_coils_size_bit +
        m_hold_registers_size_reg + ir_index/2 + ir_size_byte/2); read_index ++)
      {
        m_need_read[read_index] = 1;
      }
    }
    if(m_refresh_mode == mode_refresh_auto) {
      m_operation_status = status_wait;
    }
  }
}

void irs::modbus_client_t::mark_to_send_bit(irs_uarc a_byte_index,
  irs_uarc a_bit_index)
{
  if(((m_refresh_mode == mode_refresh_manual) && 
    (m_operation_status == status_completed)) ||
    (m_refresh_mode == mode_refresh_auto))
  {
    IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" set_bit"));
    IRS_LIB_ASSERT(a_bit_index < 8);
    m_hold_registers_size_byte = 0;
    m_hold_registers_start_byte = 0;
    m_coils_size_byte = 0;
    m_coils_start_byte = 0;
    
    range(a_byte_index, 1, 0, m_discret_inputs_end_byte, 
      &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
    range(a_byte_index, 1, m_discret_inputs_end_byte, m_coils_end_byte, 
      &m_coils_size_byte, &m_coils_start_byte);
    range(a_byte_index, 1, m_coils_end_byte, m_hold_registers_end_byte, 
      &m_hold_registers_size_byte, &m_hold_registers_start_byte);
    range(a_byte_index, 1, m_hold_registers_end_byte, 
      m_input_registers_end_byte, &m_input_registers_size_byte, 
      &m_input_registers_start_byte);
      
    if (a_bit_index < 8) {
      if((m_coils_size_byte != 0)||(m_coils_start_byte != 0))
      {
        IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" set bit: coils"));
        m_need_writes[a_byte_index*8 - m_discret_inputs_size_bit +
          a_bit_index] = 1;
      }
      if((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
      {
        IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" set bit: hold regs"));
        m_need_writes[m_coils_size_bit + m_hold_registers_start_byte/2] = 1;
      }
    }
    if(m_refresh_mode == mode_refresh_auto) {
      m_operation_status = status_wait;
    }
  }
}

void irs::modbus_client_t::mark_to_recieve_bit(irs_uarc a_byte_index,
  irs_uarc a_bit_index)
{
  if(((m_refresh_mode == mode_refresh_manual) && 
    (m_operation_status == status_completed)) || 
    (m_refresh_mode == mode_refresh_auto))
  {
    IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm("read bit"));
    IRS_LIB_ASSERT(a_bit_index < 8);
    m_hold_registers_size_byte = 0; m_hold_registers_start_byte = 0;
    m_coils_size_byte = 0; m_coils_start_byte = 0;
    m_discret_inputs_start_byte = 0; m_discret_inputs_size_byte = 0;
    m_input_registers_start_byte = 0; m_input_registers_size_byte = 0;
    
    range(a_byte_index, 1, 0, m_discret_inputs_end_byte, 
      &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
    range(a_byte_index, 1, m_discret_inputs_end_byte, m_coils_end_byte, 
      &m_coils_size_byte, &m_coils_start_byte);
    range(a_byte_index, 1, m_coils_end_byte, m_hold_registers_end_byte, 
      &m_hold_registers_size_byte, &m_hold_registers_start_byte);
    range(a_byte_index, 1, m_hold_registers_end_byte, 
      m_input_registers_end_byte, &m_input_registers_size_byte, 
      &m_input_registers_start_byte);
      
    if (a_bit_index < 8) {
      if ((m_discret_inputs_size_byte != 0) || 
        (m_discret_inputs_start_byte != 0))
      {
        IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" read bit: discret inputs"));
        m_need_read[a_byte_index*8 + a_bit_index] = 1;
      }
      if ((m_coils_size_byte != 0) || 
        (m_coils_start_byte != 0))
      {
        IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" read bit: coils"));
        m_need_read[a_byte_index*8 - m_discret_inputs_size_bit - 
          a_bit_index] = 1;
      }
      if ((m_hold_registers_size_byte != 0) || 
        (m_hold_registers_start_byte != 0))
      {
        IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" read bit: hold regs"));
        m_need_read[a_byte_index*8 - m_discret_inputs_size_bit - 
          m_coils_size_bit + a_bit_index] = 1;
      }
      if ((m_input_registers_size_byte != 0) || 
        (m_input_registers_start_byte != 0))
      {
        IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" read bit: input regs"));
        m_need_read[a_byte_index*8 - m_discret_inputs_size_bit - 
          m_coils_size_bit - m_hold_registers_size_reg + a_bit_index] = 1;
      }
    }
    if(m_refresh_mode == mode_refresh_auto) {
      m_operation_status = status_wait;
    }
  }
}

irs::modbus_client_t::status_t irs::modbus_client_t::status() const
{
  return m_operation_status;
}

void irs::modbus_client_t::set_refresh_mode(mode_refresh_t a_refresh_mode)
{
  m_refresh_mode = a_refresh_mode;
}

void irs::modbus_client_t::update()
{
  if(m_refresh_mode == mode_refresh_manual) {
    m_operation_status = status_wait;
    m_start = irs_true;
    m_read_table = true;
    m_write_table = true;
    IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" UPDATE"));
  }
}

void irs::modbus_client_t::abort()
{
  m_operation_status = status_completed;
  // ������ ����� ������:
  for(size_t clear_read_idx = 0; clear_read_idx < (m_discret_inputs_size_bit +
    m_coils_size_bit + m_hold_registers_size_reg + m_input_registers_size_reg);
    clear_read_idx++)
  {
    m_need_read[clear_read_idx] = 0;
  }
  // ������ ����� ������:
  for(size_t clear_write_idx = 0; clear_write_idx < (m_coils_size_bit + 
    m_hold_registers_size_reg); clear_write_idx++)
  {
    m_need_writes[clear_write_idx] = 0;
  }
  m_read_table = false;
  m_write_table = false;
  m_mode = wait_command_mode;
  m_start = irs_false;
  m_search_index = 0;
  m_request_type = request_start;
  m_global_read_index = 0;
}

void irs::modbus_client_t::make_packet(size_t a_index, irs_u16 a_size)
{
  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" make_packet"));
  MBAP_header_t &header = reinterpret_cast<MBAP_header_t&>(*m_spacket);
  m_size_byte_end = 0;
  switch(m_command)
  {
    case read_discrete_inputs:
    {
      request_t &sec_header = 
        reinterpret_cast<request_t&>(*(m_spacket + size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      header.length  = irs_u16(1 + sizeof(sec_header));
    }
    break;
    case read_coils:
    {
      request_t &sec_header = 
        reinterpret_cast<request_t&>(*(m_spacket + size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      header.length  = irs_u16(1 + sizeof(sec_header));
    }
    break;
    case read_hold_registers:
    {
      request_t &sec_header = 
        reinterpret_cast<request_t&>(*(m_spacket + size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      header.length  = irs_u16(1 + sizeof(sec_header));
    }
    break;
    case read_input_registers:
    {
      request_t &sec_header = 
        reinterpret_cast<request_t&>(*(m_spacket + size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      header.length  = irs_u16(1 + sizeof(sec_header));
    }
    break;
    case read_exception_status:
    {
      request_exception_t &sec_header = 
        reinterpret_cast<request_exception_t&>(*(m_spacket + size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      header.length  = irs_u16(1 + sizeof(sec_header));
    }
    break;
    case write_single_coil:
    {
      response_single_write_t &sec_header = 
        reinterpret_cast<response_single_write_t&>(*(m_spacket + 
        size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      sec_header.address = (irs_u16)a_index;
      header.length  = irs_u16(size_of_resp_multi_write);
    }
    break; 
    case write_single_register:
    {
      response_single_write_t &sec_header = 
        reinterpret_cast<response_single_write_t&>(*(m_spacket +
        size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      sec_header.address = (irs_u16)a_index;
      header.length  = irs_u16(size_of_resp_multi_write);
    }
    break;
    case write_multiple_coils:
    {
      request_multiple_write_byte_t &sec_header = 
        reinterpret_cast<request_multiple_write_byte_t&>(*(m_spacket +
        size_of_MBAP));
      irs_u16 bytes_count = 0;
      if(a_size%8 == 0)
        bytes_count = irs_u16(a_size/8);
      else
        bytes_count = irs_u16(a_size/8 + 1);
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      sec_header.byte_count = static_cast<irs_u8>(bytes_count);
      header.length  = irs_u16(size_of_resp_multi_write +
        sec_header.byte_count + 1);
      sec_header.function_code = static_cast<irs_u8>(m_command);
    }
    break;
    case write_multiple_registers:
    {
      request_multiple_write_regs_t &sec_header = 
        reinterpret_cast<request_multiple_write_regs_t&>(*(m_spacket +
        size_of_MBAP));
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = static_cast<irs_u16>(a_size/2);  
      sec_header.byte_count = static_cast<irs_u8>(a_size);
      header.length  = irs_u16(size_of_resp_multi_write + 
        sec_header.byte_count);
      sec_header.function_code = static_cast<irs_u8>(m_command);
    }
    break;
    default : {
    }
  }
  header.transaction_id = IRS_NULL;
  header.proto_id = IRS_NULL;
  header.unit_identifier  = 1;
  m_size_byte_end = header.length - 1;
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    for(irs_u16 send_pack_index = 0;
      send_pack_index < (header.length + size_of_MBAP); send_pack_index++)
    {
      mlog() << irsm(" m_spacket[") << send_pack_index << irsm("] = ") <<
        (int)(m_spacket[send_pack_index]) << endl;
    }
  );
  IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm("\nPacket ready"));
}

void irs::modbus_client_t::set_delay_time(double a_time)
{
  m_loop_timer.set(make_cnt_s(a_time));
}

void irs::modbus_client_t::tick()
{
  mp_hardflow_client->tick();
  m_fixed_flow.tick();
  if((m_fixed_flow.write_status() == 
    irs::hardflow::fixed_flow_t::status_error) ||
    (m_fixed_flow.read_status() == 
    irs::hardflow::fixed_flow_t::status_error))
  {
    m_mode = wait_command_mode;
    m_request_type = request_start;
    m_start = irs_true;
    m_fixed_flow.read_abort();
    m_fixed_flow.write_abort();
    IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("\n abort \n"));
    mlog() << irsm("\n abort") << endl;
    static irs_u8 error_count = 0;
    error_count++;
    if(error_count >= m_error_count_max) {
      IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" status error"));
      error_count = 0;
      m_operation_status = status_error;
      m_read_table = false;
      m_write_table = false;
      m_start = irs_false;
    }
  }
  switch(m_mode)
  {
    case wait_command_mode:
    {
      if(m_refresh_mode == mode_refresh_manual) {
        if(m_start) {
          IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" wait_command_mode"));
          m_start = irs_false;
          m_mode = search_write_data_mode;
          IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("search_write_data_mode"));
        }
      } else if(m_refresh_mode == mode_refresh_auto) {
        m_mode = search_write_data_mode;
        IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("search_write_data_mode"));
      }
    }
    break;
    case search_write_data_mode:
    {
      IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" search_write_data_mode"));
      m_mode = request_write_data_mode;
      irs_bool catch_block = irs_false;
      m_write_quantity = 0;
      for(; m_search_index < (m_coils_size_bit + m_hold_registers_size_reg);)
      {
        if((m_need_writes[m_search_index] == true) && 
          (catch_block == irs_false)) 
        {
          m_start_block = m_search_index;
          catch_block = irs_true;
          m_write_complete = false;
        }
        if((catch_block == irs_true) && 
          (m_need_writes[m_search_index] == false)) 
        {
          IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" send_data_mode"));
          m_write_quantity =
            static_cast<irs_u16>(m_search_index - m_start_block);
          if((irs_u32(m_search_index) > irs_u32(m_coils_size_bit))&&
            (m_start_block < m_coils_size_bit))
          {
            m_write_quantity =
              static_cast<irs_u16>(m_coils_size_bit - m_start_block);
            m_search_index = m_coils_size_bit;
          }
          break;
        }
        m_search_index++;
        if((catch_block == irs_true) &&
          (m_search_index == (m_coils_size_bit + m_hold_registers_size_reg)))
        { 
          IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" send_data_mode"));
          m_write_quantity =
            static_cast<irs_u16>(m_search_index - m_start_block);
          if(m_start_block < m_coils_size_bit) {
            m_write_quantity =
              static_cast<irs_u16>(m_coils_size_bit - m_start_block);
            m_search_index = m_coils_size_bit;
          } else {
            m_search_index = 0;
          }
          break;
        }
        if((catch_block == irs_false) && 
          (m_search_index >= (m_coils_size_bit + m_hold_registers_size_reg)))
        {
          m_search_index = 0;
          m_write_table = false;
          size_t write_flags_cnt = 0;
          for(size_t write_idx = 0; write_idx < (m_coils_size_bit + 
            m_hold_registers_size_reg); write_idx++)
          {
            if(m_need_writes[write_idx] == 1) {
              write_flags_cnt++;
            }
          }
          if(write_flags_cnt == 0)
            m_write_complete = true;
          IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" search_read_data_mode"));
          m_mode = search_read_data_mode;
          IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" ������ ������ �� ������ ����"));
          break;
        }
      }
    }
    break;
    case request_write_data_mode:
    {
      IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" request_write_data_mode"));
      if ((m_start_block) < m_coils_size_bit)
      {
        m_write_table = true;
        m_command = write_multiple_coils;
        request_multiple_write_byte_t &coils_packet =
          reinterpret_cast<request_multiple_write_byte_t&>(*(m_spacket +
          size_of_MBAP));
        if(m_write_quantity > size_of_data_write_byte*8)
        {
          IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" ������ ������� ������ coils"));
          make_packet(m_start_block, size_of_data_write_byte*8);
          coils_packet.byte_count = size_of_data_write_byte;
          bit_copy(m_coils_byte_write.data(), 
            reinterpret_cast<irs_u8*>(coils_packet.value), 
            m_start_block, 0, size_of_data_write_byte*8);
          m_search_index = m_start_block + size_of_data_write_byte*8;
        }
        else
        {
          IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(
            irsm(" ������ �� ������� ������ coils"));
          if(m_write_quantity > 1) {
            make_packet(m_start_block, m_write_quantity);
            bit_copy(m_coils_byte_write.data(),
              reinterpret_cast<irs_u8*>(coils_packet.value), 
                m_start_block, 0, m_write_quantity);
          } else {
            m_command = write_single_coil;
            response_single_write_t &sec_head =
              reinterpret_cast<response_single_write_t&>(*(m_spacket + 
              size_of_MBAP));
            sec_head.value = m_coil_write_bit;
            make_packet(m_coil_bit_index, 0);
          }
          m_search_index = m_start_block + m_write_quantity;
        }
      }
      //������ hold regs
      else if((m_start_block >=
        m_coils_size_bit) && 
        (m_start_block <= static_cast<size_t>(m_coils_size_bit + 
        m_hold_registers_size_reg)))
      {
        m_write_table = true;
        m_command = write_multiple_registers;
        size_t hr_start = m_start_block - m_coils_size_bit;
        if(m_write_quantity > size_of_data_write_reg)
        {
          IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(
            irsm(" ������ ������� ������ hold regs"));
          request_multiple_write_regs_t &hr_packet =
            reinterpret_cast<request_multiple_write_regs_t&>(*(m_spacket +
            size_of_MBAP));
          hr_packet.byte_count = size_of_data_write_reg*2;
          get_bytes(m_hold_regs_reg_write, hr_start*2,
            size_of_data_write_reg*2,
            reinterpret_cast<irs_u8*>(hr_packet.value), 0);
          make_packet(hr_start, size_of_data_write_reg*2);
          m_search_index = m_coils_size_bit + hr_start + 
            size_of_data_write_reg;
        }
        else
        {
          IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(
            irsm(" ������ �� ������� ������ hold regs"));
          for(irs_u16 send_pack_index = 0; send_pack_index < size_of_packet;
            send_pack_index++)
          {
            m_spacket[send_pack_index] = 0;
          }
          if(m_write_quantity == 1) {
            m_command = write_single_register;
            response_single_write_t &hr_packet =
              reinterpret_cast<response_single_write_t&>(*(m_spacket + 
              size_of_MBAP));
            get_bytes(m_hold_regs_reg_write, hr_start*2, m_write_quantity*2, 
              reinterpret_cast<irs_u8*>(&hr_packet.value), 0);
          } else {
            request_multiple_write_regs_t &hr_packet =
              reinterpret_cast<request_multiple_write_regs_t&>(*(m_spacket + 
              size_of_MBAP));
            get_bytes(m_hold_regs_reg_write, hr_start*2, m_write_quantity*2, 
              reinterpret_cast<irs_u8*>(hr_packet.value), 0);
          }
          make_packet(hr_start, static_cast<irs_u16>(m_write_quantity*2));
          m_search_index = m_coils_size_bit + hr_start + m_write_quantity;
          if(m_search_index == m_coils_size_bit + m_hold_registers_size_reg)
            m_search_index = 0;
        }
      }
      IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_request_mode"));
      m_mode = send_request_mode;
    }
    break;
    case send_request_mode:
    {
      IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" send_request_mode"));
      IRS_LIB_IRSMBUS_DBG_MONITOR(modbus_pack_request_monitor(m_spacket););
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
        default:
        {
          
        } break;
      }
      m_fixed_flow.write(m_channel, m_spacket, size_of_MBAP +
        m_size_byte_end);
      IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("read_header_mode"));
      m_mode = read_header_mode;
    }
    break;
    case read_header_mode:
    {
      if(m_fixed_flow.write_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      {
        m_fixed_flow.read(m_channel, m_rpacket, size_of_MBAP);
        IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("read_response_mode"));
        m_mode = read_response_mode;
      }
    }
    break;
    case read_response_mode: 
    { 
      if(m_fixed_flow.read_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      {
        convert(m_rpacket, 0, size_of_MBAP - 1);
        MBAP_header_t &header = 
          reinterpret_cast<MBAP_header_t&>(*m_rpacket);
        m_fixed_flow.read(m_channel, m_rpacket + size_of_MBAP,
          header.length - 1);
        IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" treatment_response_mode"));
        m_mode = treatment_response_mode;
      }
    }
    break;
    case treatment_response_mode:
    {
      if(m_fixed_flow.read_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      {
        IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" treatment_response_mode"));
        request_t &convert_pack_for_read =
          reinterpret_cast<request_t&>(*(m_rpacket + 
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
            response_read_reg_t &read_hold_reg =
              reinterpret_cast<response_read_reg_t&>(*(m_rpacket + 
              size_of_MBAP));
            convert(m_rpacket, size_of_MBAP + size_of_resp_header,
              read_hold_reg.byte_count);
          } break;
          case read_input_registers:
          {
            response_read_reg_t &read_input_reg =
              reinterpret_cast<response_read_reg_t&>(*(m_rpacket + 
              size_of_MBAP));
            convert(m_rpacket, size_of_MBAP + size_of_resp_header,
              read_input_reg.byte_count);
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
          case write_multiple_coils:
          {
            convert(m_rpacket, size_of_MBAP + 1, size_of_read_header);
          } break;
          case write_multiple_registers:
          {
            convert(m_rpacket, size_of_MBAP + 1, size_of_read_header);
          } break;
          default:
          {
            
          } break;
        }
        request_t &req_header = 
          reinterpret_cast<request_t&>(*(m_spacket + size_of_MBAP));
        //Recieve packet:
        IRS_LIB_IRSMBUS_DBG_MONITOR(modbus_pack_response_monitor(m_rpacket););
        convert(m_spacket, size_of_MBAP + 1, size_of_read_header);
        irs_u16 start_addr = req_header.starting_address;
        switch(req_header.function_code)
        {
          case read_discrete_inputs:
          { 
            response_read_byte_t &read_di = 
              reinterpret_cast<response_read_byte_t&>(*(m_rpacket +
              size_of_MBAP));
            IRS_LIB_ASSERT(static_cast<size_t>(start_addr/8 +
              read_di.byte_count) <= m_discr_inputs_byte_read.size());
            bit_copy(read_di.value, m_discr_inputs_byte_read.data(), 0,
              start_addr, read_di.byte_count*8);
            for(irs_u16 di_index = 0; di_index < read_di.byte_count*8;
              di_index++)
            {
              m_need_read[start_addr + di_index] = 0;
            }
            IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
              for(int di_idx = start_addr; 
                di_idx < start_addr + read_di.byte_count; di_idx++)
              {
                mlog() << irsm(" m_discr_inputs_byte_read[") << di_idx << 
                  irsm("] = ") << int(m_discr_inputs_byte_read[di_idx]) << endl;
              }
            );
          }
          break;
          case read_coils:
          {
            response_read_byte_t &read_coils = 
              reinterpret_cast<response_read_byte_t&>(*(m_rpacket +
              size_of_MBAP));
            IRS_LIB_ASSERT(static_cast<size_t>(start_addr/8 +
              read_coils.byte_count) <= m_coils_byte_read.size());
            bit_copy(read_coils.value, m_coils_byte_read.data(), 0, 
              start_addr, read_coils.byte_count*8);
            for(irs_u16 coils_index = 0; coils_index < read_coils.byte_count*8;
              coils_index++)
            {
              m_need_read[m_discret_inputs_size_bit + start_addr +
                coils_index] = 0;
            }
            IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
              for(int coils_idx = start_addr; 
                coils_idx < start_addr + read_coils.byte_count; coils_idx++)
              {
                mlog() << irsm(" m_coils_byte_read[") << coils_idx << 
                  irsm("] = ") << int(m_coils_byte_read[coils_idx]) << endl;
              }
            );
          }
          break;
          case read_hold_registers:
          {
            response_read_reg_t &hr_packet = 
              reinterpret_cast<response_read_reg_t&>(*(m_rpacket + 
              size_of_MBAP));
            IRS_LIB_ASSERT(static_cast<size_t>(start_addr +
              hr_packet.byte_count/2) <= m_hold_regs_reg_read.size());
            set_bytes(m_hold_regs_reg_read, start_addr*2, hr_packet.byte_count,
              reinterpret_cast<irs_u8*>(hr_packet.value), 0);
            for(irs_u16 hr_index = 0; hr_index < hr_packet.byte_count/2;
              hr_index++)
            {
              m_need_read[m_discret_inputs_size_bit + m_coils_size_bit + 
                start_addr + hr_index] = 0;
            }
            IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
              for(int hr_idx = start_addr; 
                hr_idx < start_addr + hr_packet.byte_count/2; hr_idx++)
              {
                mlog() << irsm(" m_hold_regs_reg_lo[") <<
                  hr_idx << irsm("] = ") <<
                  int(IRS_LOBYTE(m_hold_regs_reg_read[hr_idx])) << endl;
                mlog() << irsm(" m_hold_regs_reg_hi[") <<
                  hr_idx << irsm("] = ") << 
                  int(IRS_HIBYTE(m_hold_regs_reg_read[hr_idx])) << endl;
              }
            );
          }
          break;
          case read_input_registers:
          {
            response_read_reg_t &ir_packet =
              reinterpret_cast<response_read_reg_t&>(*(m_rpacket + 
              size_of_MBAP));
            IRS_LIB_ASSERT(static_cast<size_t>(start_addr +
              ir_packet.byte_count/2) <= m_input_regs_reg_read.size());
            set_bytes(m_input_regs_reg_read, start_addr*2, 
              ir_packet.byte_count,
              reinterpret_cast<irs_u8*>(ir_packet.value), 0);
            for(irs_u16 ir_index = 0; ir_index < ir_packet.byte_count/2;
              ir_index++)
            {
              m_need_read[m_discret_inputs_size_bit + m_coils_size_bit + 
                m_hold_registers_size_reg + start_addr + ir_index] = 0;
            }
            IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
              for(int ir_idx = start_addr; 
                ir_idx < start_addr + ir_packet.byte_count/2; ir_idx++)
              {
                mlog() << irsm(" m_input_regs_reg_lo[") <<
                  ir_idx << irsm("] = ") <<
                  int(IRS_LOBYTE(m_input_regs_reg_read[ir_idx])) << endl;
                mlog() << irsm(" m_input_regs_reg_hi[") <<
                  ir_idx << irsm("] = ") << 
                  int(IRS_HIBYTE(m_input_regs_reg_read[ir_idx])) << endl;
              }
            );
            if((start_addr + ir_packet.byte_count/2) >= 
              static_cast<int>(m_input_registers_size_reg))
            {
              m_first_read = irs_true;
              IRS_LIB_IRSMBUS_DBG_OPERATION_TIME(
                double time_delta_read = m_measure_time.get() - 
                  m_measure_int_time;
                m_measure_int_time = m_measure_time.get();
                mlog() << stime << irsm(" ") <<
                  time_delta_read << irsm(" read end") << endl;
              );
            }
          }
          break;
          case write_single_coil:
          {
            response_single_write_t &sec_head =
              reinterpret_cast<response_single_write_t&>(*(m_spacket + 
              size_of_MBAP));
            m_need_writes[sec_head.address] = 0;
          }
          break;
          case write_single_register:
          {
            m_need_writes[m_coils_size_bit + start_addr] = 0;
            m_hold_regs_reg_write[0] = 0;
          }
          break;
          case write_multiple_coils:
          {
            request_multiple_write_byte_t &sec_head =
              reinterpret_cast<request_multiple_write_byte_t&>(*(m_spacket + 
              size_of_MBAP));
            for(irs_u16 coils_index = 0; coils_index < sec_head.quantity;
              coils_index++)
            {
              m_need_writes[start_addr + coils_index] = 0;
              m_coils_byte_write[coils_index/8] = 0;
            }
          }
          break;
          case write_multiple_registers:
          {
            request_multiple_write_regs_t &sec_head =
              reinterpret_cast<request_multiple_write_regs_t&>(*(m_spacket + 
              size_of_MBAP));
            for(irs_u16 reg_index = 0; reg_index < sec_head.quantity;
              reg_index++)
            {
              m_need_writes[m_coils_size_bit + start_addr + reg_index] = 0;
              m_hold_regs_reg_write[reg_index] = 0;
            }
          }
          break;
          case read_exception_status:
          {
          }
          break;
        }
        for(irs_u16 send_pack_index = 0; send_pack_index < size_of_packet;
          send_pack_index++)
        {
          m_spacket[send_pack_index] = 0;
        }
        if(!m_write_table) {
          m_mode = search_read_data_mode;
          IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" *********** write complete "
            "**********"));
          if(!m_read_table) {
            m_mode = wait_command_mode;
            if(m_write_complete) {
              m_operation_status = status_completed;
            }
            IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" ********* read complete "
              "*********"));
          } else {
            m_mode = search_read_data_mode;
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(
              irsm(" search_read_data_mode continue"));
          }
        } else {
          m_mode = search_write_data_mode;
          IRS_LIB_IRSMBUS_DBG_MSG_BASE(
            irsm(" search_write_data_mode continue"));
        }
      }
    }
    break;
    case search_read_data_mode:
    {
      IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("search_read_data_mode"));
      if(m_refresh_mode == mode_refresh_manual) {
        m_mode = request_read_data_mode;
        irs_bool catch_block = irs_false;
        m_read_quantity = 0;
        for(; m_search_index < (m_discret_inputs_size_bit + m_coils_size_bit + 
          m_hold_registers_size_reg + m_input_registers_size_reg);)
        {
          if((m_need_read[m_search_index] == true) && 
            (catch_block == irs_false)) 
          {
            IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(irsm(" block catched"));
            m_start_block = m_search_index;
            catch_block = irs_true;
          }
          if((catch_block == irs_true) && 
            (m_need_read[m_search_index] == false)) 
          {
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_data_mode"));
            m_read_quantity =
              static_cast<irs_u16>(m_search_index - m_start_block);
            if((m_start_block > m_discret_inputs_size_bit) && 
              (m_start_block < (m_discret_inputs_size_bit + 
              m_coils_size_bit)) && 
              (m_search_index > (m_discret_inputs_size_bit + m_coils_size_bit)))
            {
              m_search_index = m_discret_inputs_size_bit + m_coils_size_bit;
              m_read_quantity =
                static_cast<irs_u16>(m_search_index - m_start_block);
            }
            else if((m_start_block > (m_discret_inputs_size_bit + 
              m_coils_size_bit)) && 
              (m_start_block < (m_discret_inputs_size_bit + m_coils_size_bit +
              m_hold_registers_size_reg)) && 
              (m_search_index > (m_discret_inputs_size_bit + m_coils_size_bit + 
              m_hold_registers_size_reg)))
            {
              m_search_index = m_discret_inputs_size_bit + m_coils_size_bit +
                m_hold_registers_size_reg;
              m_read_quantity =
                static_cast<irs_u16>(m_search_index - m_start_block);
            }
            else if((m_start_block > (m_discret_inputs_size_bit + 
              m_coils_size_bit + m_hold_registers_size_reg)) && 
              (m_start_block < (m_discret_inputs_size_bit + m_coils_size_bit +
              m_hold_registers_size_reg + m_input_registers_size_reg)) && 
              (m_search_index >= (m_discret_inputs_size_bit + m_coils_size_bit + 
              m_hold_registers_size_reg + m_input_registers_size_reg)))
            {
              m_search_index = m_discret_inputs_size_bit + m_coils_size_bit +
                m_hold_registers_size_reg + m_input_registers_size_reg;
              m_read_quantity =
                static_cast<irs_u16>(m_search_index - m_start_block);
            }
            break;
          }
          m_search_index++;
          if((catch_block == irs_true) &&
            (m_search_index == (m_discret_inputs_size_bit + m_coils_size_bit + 
            m_hold_registers_size_reg + m_input_registers_size_reg)))
          { 
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_data_mode"));
            m_write_quantity =
              static_cast<irs_u16>(m_search_index - m_start_block);
            if(m_start_block < m_discret_inputs_size_bit)
            {
              m_search_index = m_discret_inputs_size_bit;
              m_read_quantity =
                static_cast<irs_u16>(m_search_index - m_start_block);
            }
            else if((m_start_block > m_discret_inputs_size_bit) && 
              (m_start_block < (m_discret_inputs_size_bit + 
              m_coils_size_bit)))
            {
              m_search_index = m_discret_inputs_size_bit + m_coils_size_bit;
              m_read_quantity =
                static_cast<irs_u16>(m_search_index - m_start_block);
            }
            else if((m_start_block > (m_discret_inputs_size_bit + 
              m_coils_size_bit)) && 
              (m_start_block < (m_discret_inputs_size_bit + m_coils_size_bit +
              m_hold_registers_size_reg)))
            {
              m_search_index = m_discret_inputs_size_bit + m_coils_size_bit +
                m_hold_registers_size_reg;
              m_read_quantity =
                static_cast<irs_u16>(m_search_index - m_start_block);
            }
            else if((m_start_block > (m_discret_inputs_size_bit + 
              m_coils_size_bit + m_hold_registers_size_reg)) && 
              (m_start_block < (m_discret_inputs_size_bit + m_coils_size_bit +
              m_hold_registers_size_reg + m_input_registers_size_reg)))
            {
              m_search_index = 0;
            }
            break;
          }
          if((catch_block == irs_false) && 
            (m_search_index >= (m_discret_inputs_size_bit + m_coils_size_bit + 
            m_hold_registers_size_reg + m_input_registers_size_reg)))
          {
            m_search_index = 0;
            m_read_table = false;
            m_mode = treatment_response_mode;
            IRS_LIB_IRSMBUS_DBG_MSG_DETAIL(
              irsm(" ������ ������ �� ������ ����"));
            break;
          }
        }
      } else if(m_refresh_mode == mode_refresh_auto) {
        m_mode = make_request_mode;
        IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("make_request_mode"));
      }
    }
    break;
    case request_read_data_mode:
    {
      IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_request_mode"));
      m_mode = send_request_mode;
      // ������ discret inputs
      if(m_start_block < m_discret_inputs_size_bit) {
        m_command = read_discrete_inputs;
        if(m_read_quantity/8 > size_of_data_read_byte)
        {
          make_packet(m_start_block, size_of_data_read_byte*8);
          m_search_index = m_start_block + size_of_data_read_byte*8;
        } else {
          make_packet(m_start_block, m_read_quantity);
          m_search_index = m_start_block + m_read_quantity;
        }
      }
      // ������ coils
      else if((m_start_block >= m_discret_inputs_size_bit) &&
        (m_start_block < m_discret_inputs_size_bit + m_coils_size_bit))
      {
        m_command = read_coils;
        if(m_read_quantity/8 > size_of_data_read_byte)
        {
          make_packet(m_start_block - m_discret_inputs_size_bit,
            size_of_data_read_byte*8);
          m_search_index = m_start_block + size_of_data_read_byte*8;
        } else {
          make_packet(m_start_block - m_discret_inputs_size_bit, 
            m_read_quantity);
          m_search_index = m_start_block + m_read_quantity;
        }
      }
      // ������ holding registers
      else if((m_start_block >= (m_discret_inputs_size_bit + 
        m_coils_size_bit)) && (m_start_block < (m_discret_inputs_size_bit + 
        m_coils_size_bit + m_hold_registers_size_reg)))
      {
        m_command = read_hold_registers;
        if(m_read_quantity*2 > size_of_data_read_byte)
        {
          make_packet(m_start_block - m_discret_inputs_size_bit - 
            m_coils_size_bit, size_of_data_read_reg);
          m_search_index = m_start_block + size_of_data_read_reg;
        } else {
          make_packet(m_start_block - m_discret_inputs_size_bit - 
            m_coils_size_bit, m_read_quantity);
          m_search_index = m_start_block + m_read_quantity;
        }
      }
      // ������ input registers
      else if((m_start_block >= (m_discret_inputs_size_bit + 
        m_coils_size_bit + m_hold_registers_size_reg)) && 
        (m_start_block < (m_discret_inputs_size_bit + m_coils_size_bit + 
        m_hold_registers_size_reg + m_input_registers_size_reg))) 
      {
        m_command = read_input_registers;
        if(m_read_quantity*2 > size_of_data_read_byte)
        {
          make_packet(m_start_block - (m_discret_inputs_size_bit +
            m_coils_size_bit + m_hold_registers_size_reg),
            size_of_data_read_reg);
          m_search_index = m_start_block + size_of_data_read_reg;
        } else {
          make_packet(m_start_block - (m_discret_inputs_size_bit +
            m_coils_size_bit + m_hold_registers_size_reg), m_read_quantity);
          m_search_index = m_start_block + m_read_quantity;
          if(m_search_index == (m_input_registers_size_reg + 
            m_hold_registers_size_reg + m_discret_inputs_size_bit + 
            m_coils_size_bit))
          {
            m_search_index = 0;
          }
        }
      }
      m_mode = send_request_mode;
    }
    break;
    case make_request_mode: //������ ������� ������� (����� mode_refresh_auto)
    {
      switch(m_request_type)
      {
        case request_start:
        {
          if((m_loop_timer.check()) && (m_refresh_mode == mode_refresh_auto)) {
            IRS_LIB_IRSMBUS_DBG_OPERATION_TIME(
              double time_delta_write = m_measure_time.get() - 
                m_measure_int_time;
              m_measure_int_time = m_measure_time.get();
              mlog() << stime << irsm(" ") <<
                time_delta_write << irsm(" write end") << endl;
            );
            m_mode = make_request_mode;
            m_request_type = read_discrete_inputs;
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" make_request_mode"));
          }
        }
        break;
        case read_discrete_inputs:
        {
          m_command = m_request_type;
          if((m_global_read_index + size_of_data_read_byte) <
            m_discret_inputs_size_bit/8)
          {
            make_packet(m_global_read_index, size_of_data_read_byte*8);
            m_global_read_index += size_of_data_read_byte;
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_request_mode"));
            m_mode = send_request_mode;
          } else {
            make_packet(m_global_read_index,
              irs_u16(m_discret_inputs_size_bit - m_global_read_index*8));
            m_global_read_index = 0;
            m_request_type = read_coils;
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_request_mode"));
            m_mode = send_request_mode;
          }
        }
        break;
        case read_coils:
        {
          m_command = m_request_type;
          if((m_global_read_index + size_of_data_read_byte) <
            m_coils_size_bit/8)
          {
            make_packet(m_global_read_index, size_of_data_read_byte*8);
            m_global_read_index += size_of_data_read_byte;
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_request_mode"));
            m_mode = send_request_mode;
          } else {
            make_packet(m_global_read_index,
              irs_u16(m_coils_size_bit - m_global_read_index*8));
            m_global_read_index = 0;
            m_request_type = read_hold_registers;
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_request_mode"));
            m_mode = send_request_mode;
          }
        }
        break;
        case read_hold_registers:
        {
          m_command = m_request_type;
          if(irs_u32(m_global_read_index + size_of_data_read_reg) < 
            irs_u32(m_hold_registers_size_reg))
          {
            make_packet(m_global_read_index, size_of_data_read_reg);
            m_global_read_index += size_of_data_read_reg;
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_request_mode"));
            m_mode = send_request_mode;
          } else {
            make_packet(m_global_read_index,
              irs_u16(m_hold_registers_size_reg - m_global_read_index));
            m_global_read_index = 0;
            m_request_type = read_input_registers;
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_request_mode"));
            m_mode = send_request_mode;
          }
        }
        break;
        case read_input_registers:
        {
          m_command = m_request_type;
          if(irs_u32(m_global_read_index + size_of_data_read_reg) <
            irs_u32(m_input_registers_size_reg))
          {
            make_packet(m_global_read_index, size_of_data_read_reg);
            m_global_read_index += size_of_data_read_reg;
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_request_mode"));
            m_mode = send_request_mode;
          } else {
            make_packet(m_global_read_index,
              irs_u16(m_input_registers_size_reg - m_global_read_index));
            m_global_read_index = 0;
            m_request_type = request_start;
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_request_mode"));
            m_mode = send_request_mode;
            size_t read_flags_cnt = 0;
            for(size_t read_idx = 0; read_idx < (m_discret_inputs_size_bit +
              m_coils_size_bit + m_hold_registers_size_reg + 
              m_input_registers_size_reg); read_idx++)
            {
              if(m_need_read[read_idx] == 1) {
                read_flags_cnt++;
              }
            }
            if(read_flags_cnt == 0) {
              m_read_table = false;
            }
          }
        }
        break;
        default :
        {
        } break;
      }
    }
    break;
  }
}
