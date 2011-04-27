//! \file
//! \ingroup network_in_out_group
//! \brief Клиент и сервер modbus
//!
//! Дата: 25.10.2010\n
//! Ранняя дата: 16.09.2008

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

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

#include <irsfinal.h>

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

template <class T>
irs_bool to_irs_bool(T a_value)
{
  if (a_value) {
    return true;
  } else {
    return false;
  }
}

irs::irs_string_t int_to_str(int a_number, int a_radix, int a_cnt);

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

//! \brief Копирует данные в битах
//!
//! \param[in] ap_data_in – указатель на начало массива, источника данных;
//! \param[out] ap_data_out – указатель на начало массива, приемника данных;
//! \param[in] a_index_data_in - индекс в битах, по которому располагаются
//!   данные в массиве – источнике данных;
//! \param[out] a_index_data_out – индекс в битах, по которому нужно переслать
//!   данные в массив – приемник данных;
//! \param[in] a_size_data_bit – размер копируемых данных в битах.\n
//! Передаваемые данные бьются на 3 части:
//!   - middle_part участок, занимающий число бит кратное 8; \n
//!   - first_part участок, стоящий перед middle_part;\n
//!   - last_part участок, стоящий после middle_part.\n
//! Если a_index_data_in и a_index_data_out равны 0, то для копирования участка
//!   middle_part используется функция memcpyex(). В остальных случаях
//!   используются маски для копирования последовательностей битов, смещенных
//!   относительно начала байта.

void bit_copy(const irs_u8 *ap_data_in, irs_u8 *ap_data_out,
  size_t a_index_data_in, size_t a_index_data_out, size_t a_size_data_bit)
{
  size_t index_in = a_index_data_in%8;
  size_t index_out = a_index_data_out%8;
  size_t data_start_in = a_index_data_in/8;
  size_t data_start_out = a_index_data_out/8;
  size_t first_part_size = 0;
  size_t last_part_size = 0;
  if (index_in == index_out) {
    if (index_out && index_in) {
      if ((a_size_data_bit + index_out) >= 8) {
        first_part_size = 8 - index_out;
      } else {
        first_part_size = a_size_data_bit;
      }
    }
    last_part_size = (a_size_data_bit - first_part_size)%8;
    size_t middle_part_size_byte = (a_size_data_bit - 
      (first_part_size + last_part_size))/8;
    int part_idx = 0;
    if (first_part_size) {
      irs_u8 first_mask = 0;
      if ((a_size_data_bit + index_out) >= 8) {
        first_mask = mask_gen(0, first_part_size);
      } else {
        first_mask = mask_gen(8 - (a_size_data_bit + index_out),
          first_part_size);
      }
      irs_u8 first_data_in = ap_data_in[data_start_in];
      first_data_in &= first_mask;
      ap_data_out[data_start_out] &= static_cast<irs_u8>(~first_mask);
      ap_data_out[data_start_out] |= first_data_in;
      part_idx = 1;
    }
    if (last_part_size) {
      irs_u8 last_mask = mask_gen(8 - last_part_size, last_part_size);
      irs_u8 last_data_in = ap_data_in[data_start_in + middle_part_size_byte + 
        first_part_size/8];
      last_data_in &= last_mask;
      ap_data_out[data_start_out + middle_part_size_byte + part_idx] &= 
        static_cast<irs_u8>(~last_mask);
      ap_data_out[data_start_out + middle_part_size_byte + part_idx] |=
        last_data_in;
    }
    irs::memcpyex(ap_data_out + data_start_out + part_idx, ap_data_in +
      data_start_in + part_idx, middle_part_size_byte);
  } else {
    int offset_idx = 0;
    irs_u32 index_bit = 0;
    irs_u8 middle_mask = 0;
    irs_u32 offset = 0;
    if (index_out > index_in) {
      offset = index_out - index_in;
      middle_mask = mask_gen(0, 8 - offset);
      index_bit = offset;
      offset_idx = 0;
      if (index_out && index_in) {
        if ((index_in + a_size_data_bit) > 8) {
          first_part_size = 8 - index_in;
        } else {
          first_part_size = a_size_data_bit;
        }
      }
      size_t rest_size = a_size_data_bit + offset - first_part_size;
      if (!rest_size) {
        last_part_size = rest_size;
      } else if (!(rest_size%8)) {
        last_part_size = 8;
      } else {
        last_part_size = rest_size%8;
      }
    } else if (index_in > index_out) {
      offset = index_in - index_out;
      middle_mask = mask_gen(0, offset);
      index_bit = 8 - offset;
      offset_idx = 1;
      if (index_out && index_in) {
        first_part_size = 8 - index_out;
      }
      last_part_size = (a_size_data_bit /*+ offset*/ - first_part_size)%8;
    }
    size_t data_cnt = 0;
    if (a_size_data_bit >= 8) {
      data_cnt = (a_size_data_bit - first_part_size)/8;
    }
    int first_part_idx = 0;
    if (first_part_size) {
      first_part_idx = 1;
      if (index_out < index_in) {
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
      } else if (index_out > index_in) {
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
    if (last_part_size) {
      size_t last_external_byte_index = data_cnt + data_start_in + 
        first_part_idx;
      size_t last_internal_byte_index = data_cnt + data_start_out +
        first_part_idx; 
      irs_u8 last_mask = 0;
      if (static_cast<irs_u32>(last_part_size) <
        ((8 - 2*offset)*offset_idx + offset))
      {
        if (index_out > index_in) {
          last_mask = mask_gen(8 - last_part_size, last_part_size);
          irs_u8 mask_ext = mask_gen(1, last_part_size);
          irs_u8 data_ext = ap_data_in[last_external_byte_index];
          data_ext &= mask_ext;
          data_ext >>= 8 - (last_part_size + 1);
          ap_data_out[last_internal_byte_index + 1] &=
            static_cast<irs_u8>(~last_mask);
          ap_data_out[last_internal_byte_index + 1] |= data_ext;
          offset_index = 1;
        } else if (index_in > index_out) {
          if ((index_in + a_size_data_bit) < 8) {
            irs_u8 data_in = ap_data_in[last_external_byte_index];
            irs_u8 mask_ext = mask_gen(8 - (a_size_data_bit%8 + index_in),
              a_size_data_bit%8);
            irs_u8 mask_int = mask_gen(0, 8 - a_size_data_bit);
            data_in &= mask_ext;
            data_in >>= offset;
            ap_data_out[last_internal_byte_index] &= mask_int;
            ap_data_out[last_internal_byte_index] |= data_in;
          } else {
            last_mask = mask_gen(8 - offset, offset);
            first_part(ap_data_out[last_internal_byte_index], 
              ap_data_in[last_external_byte_index], index_bit, last_mask);
          }
        }
      } else {
        if (index_in > index_out) {
          second_part(ap_data_out[last_internal_byte_index], 
            ap_data_in[last_external_byte_index - 1 + offset_idx],
            index_bit, middle_mask);
        }
        if (static_cast<irs_u32>(last_part_size) >
          ((8 - 2*offset)*offset_idx + offset))
        {
          if (index_out > index_in) {
            last_mask = mask_gen(8 - last_part_size,
              last_part_size - offset);
            first_part(ap_data_out[last_internal_byte_index],
              ap_data_in[last_external_byte_index], index_bit, last_mask);
          }
          if (index_in > index_out) {
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
    // Middle Part
    for(size_t data_idx = 0; data_idx < data_cnt + offset_index;
      data_idx++)
    {
      size_t external_idx = data_idx + data_start_in + first_part_idx;
      size_t internal_idx = data_idx + data_start_out + first_part_idx;
      first_part(ap_data_out[internal_idx], 
        ap_data_in[external_idx + offset_idx], index_bit, middle_mask);
    }
    for(size_t data_idx = 0; data_idx < data_cnt; data_idx++) {
      size_t external_idx = data_idx + data_start_in + 
        first_part_idx*offset_idx;
      size_t internal_idx = data_idx + data_start_out + 
        first_part_idx*offset_idx;
      second_part(ap_data_out[internal_idx + 1 - offset_idx],
        ap_data_in[external_idx], index_bit, middle_mask);
    }
  }
}

char digit_to_char(int a_digit)
{
  if (a_digit <= 9) {
    return static_cast<char>('0' + a_digit);
  } else {
    return static_cast<char>('A' + a_digit - 10);
  }
}

irs::irs_string_t int_to_str(int a_number, int a_radix, int a_cnt)
{
  irs::irs_string_t number_str(a_cnt, irst(' '));
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
    //irs_i64 rand_val = rand();
    ap_data_in[idx_in] = 0;//static_cast<irs_u8>(rand_val*IRS_U8_MAX/RAND_MAX);
  }
  for(int idx_out = 0; idx_out < static_cast<int>(ap_data_out.size());
    idx_out ++)
  {
    //int rand_val = rand();
    ap_data_out[idx_out] = IRS_U8_MAX;//static_cast<irs_u8>(rand_val*IRS_U8_MAX/RAND_MAX);
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

//! \brief Преобразование порядка байтов в big_endian (от старшего
//!   к младшему), если такая операция требуется для данной архитектуры.
//!
//! \param[in, out] ap_mess – указатель на начало массива, в котором следует
//!   провести такое преобразование;
//! \param[in] a_start – начало области преобразования;
//! \param[in] a_length – длина участка преобразования.
void convert(irs_u8 *ap_mess, irs_u8 a_start, irs_u8 a_length)
{
  if (irs::cpu_traits_t::endian() == irs::little_endian) {
    for(size_t i = a_start; i < static_cast<size_t>(a_start + a_length);
      i += size_t(2))
    {
      irs_u8 var = ap_mess[i + 1];
      ap_mess[i + 1] = ap_mess[i];
      ap_mess[i] = var;
    }
  }
}

void irs::range(size_t a_index, size_t a_size, size_t a_start_range, 
  size_t a_end_range, size_t *a_num, size_t *a_start)
{
  size_t var = a_index + a_size;
  if ((a_index >= a_start_range) && (a_index < a_end_range) && 
    (var > a_start_range) && (var <= a_end_range))
  {
    *a_num = a_size;
    *a_start = (a_index - a_start_range);
  } else if((a_index > a_start_range) && (a_index < a_end_range) && 
    (var > a_end_range))
  {
    *a_num = (a_end_range - a_index);
    *a_start = (a_index - a_start_range);
  } else if((a_index < a_start_range) && (var > a_start_range) && 
    (var < a_end_range))
  {
    *a_num = (var - a_start_range);
    *a_start = 0;
  } else if((a_index <= a_start_range) && (var >= a_end_range)) {
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
  counter_t a_disconnect_time
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
  m_channel(hardflow_t::invalid_channel),
  mp_hardflow_server(ap_hardflow),
  m_fixed_flow(mp_hardflow_server),
  m_operation_status(status_completed)
{
  m_fixed_flow.read_timeout(a_disconnect_time);
  m_fixed_flow.write_timeout(a_disconnect_time);
  mlog() << irsm("SERVER START!") << endl;
}

irs::modbus_server_t::~modbus_server_t()
{
}

char const IRS_CSTR_NONVOLATILE irs::modbus_server_t::m_read_header_mode[] =
  "read_header_mode";
char const IRS_CSTR_NONVOLATILE irs::modbus_server_t::m_read_request_mode[] =
  "read_request_mode";
char const IRS_CSTR_NONVOLATILE irs::modbus_server_t::m_send_response_mode[] =
  "send_response_mode";
char const IRS_CSTR_NONVOLATILE irs::modbus_server_t::m_read_end_mode[] =
  "read_end_mode";
char const IRS_CSTR_NONVOLATILE irs::modbus_server_t::m_write_end[] =
  "write_end";

IRS_CSTR_NONVOLATILE char const IRS_CSTR_NONVOLATILE* const
  irs::modbus_server_t::m_ident_name_list[] =
{
  m_read_header_mode,
  m_read_request_mode,
  m_send_response_mode,
  m_read_end_mode,
  m_write_end
};  
  
void irs::modbus_server_t::view_mode()
{
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(m_ident_name_list[m_mode]);
}

irs_uarc irs::modbus_server_t::size()
{
  return m_size_byte;
}

irs_bool irs::modbus_server_t::connected()
{
  return true;
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
  IRS_LIB_ASSERT((a_index + a_size) <= m_input_registers_end_byte);
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("read"));
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
    
  if ((m_discret_inputs_size_byte != 0)) {
    IRS_LIB_ASSERT((m_discret_inputs_start_byte + m_discret_inputs_size_byte) <=
      m_discr_inputs_byte.size());
    memcpyex(ap_buf, m_discr_inputs_byte.data() + m_discret_inputs_start_byte,
      m_discret_inputs_size_byte);
  }
  if ((m_coils_size_byte != 0)||(m_coils_start_byte != 0)) {
    IRS_LIB_ASSERT((m_coils_start_byte + m_coils_size_byte) <=
      m_coils_byte.size());
    memcpyex(ap_buf + m_discret_inputs_size_byte, 
      m_coils_byte.data() + m_coils_start_byte, m_coils_size_byte);
  }
  if ((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0)) {
    size_t ap_buf_start = m_discret_inputs_size_byte + m_coils_size_byte;
    irs_u8* hold_regs_data = 
      reinterpret_cast<irs_u8*>(m_hold_regs_reg.data());
    IRS_LIB_ASSERT((m_hold_registers_start_byte + 
      m_hold_registers_size_byte)/2 <= m_hold_regs_reg.size());
    memcpyex(ap_buf + ap_buf_start, hold_regs_data + 
      m_hold_registers_start_byte, m_hold_registers_size_byte);
  }
  if ((m_input_registers_size_byte != 0)||(m_input_registers_start_byte != 0)) {
    size_t ap_buf_start = m_discret_inputs_size_byte + m_coils_size_byte + 
      m_hold_registers_size_byte;
    irs_u8* input_regs_data = 
      reinterpret_cast<irs_u8*>(m_input_regs_reg.data());
    IRS_LIB_ASSERT((m_input_registers_start_byte +
      m_input_registers_size_byte)/2 <= m_input_regs_reg.size());
    memcpyex(ap_buf + ap_buf_start, input_regs_data + 
      m_input_registers_start_byte, m_input_registers_size_byte);
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
  IRS_LIB_ASSERT((a_index + a_size) <= m_input_registers_end_byte);
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("write"));
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

  if ((m_discret_inputs_size_byte != 0)) {
    IRS_LIB_ASSERT((m_discret_inputs_start_byte + m_discret_inputs_size_byte) <=
      m_discr_inputs_byte.size());
    memcpyex(m_discr_inputs_byte.data() + m_discret_inputs_start_byte,
      ap_buf, m_discret_inputs_size_byte);
  }
  if ((m_coils_size_byte != 0)||(m_coils_start_byte != 0)) {
    IRS_LIB_ASSERT((m_coils_start_byte + m_coils_size_byte) <=
      m_coils_byte.size());
    memcpyex(m_coils_byte.data() + m_coils_start_byte,
      ap_buf + m_discret_inputs_size_byte, m_coils_size_byte);
  }
  if ((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0)) {
    size_t ap_buf_start = m_discret_inputs_size_byte + m_coils_size_byte;
    irs_u8* hold_regs_data =
      reinterpret_cast<irs_u8*>(m_hold_regs_reg.data());
    IRS_LIB_ASSERT((m_hold_registers_start_byte +
      m_hold_registers_size_byte)/2 <= m_hold_regs_reg.size());
    memcpyex(hold_regs_data + m_hold_registers_start_byte, 
      ap_buf + ap_buf_start, m_hold_registers_size_byte);
  }
  if ((m_input_registers_size_byte != 0)||(m_input_registers_start_byte != 0)) {
    size_t ap_buf_start = m_discret_inputs_size_byte + m_coils_size_byte +
      m_hold_registers_size_byte;
    irs_u8* input_regs_data =
      reinterpret_cast<irs_u8*>(m_input_regs_reg.data());
    IRS_LIB_ASSERT((m_input_registers_start_byte +
      m_input_registers_size_byte)/2 <= m_input_regs_reg.size());
    memcpyex(input_regs_data + m_input_registers_start_byte,
      ap_buf + ap_buf_start, m_input_registers_size_byte);
  }
}

irs_bool irs::modbus_server_t::bit(irs_uarc a_byte_index, irs_uarc a_bit_index)
{
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("read bit"));
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
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" read bit: discret inputs"));
      irs_u8 di_byte_read_bit = 
        m_discr_inputs_byte[m_discret_inputs_start_byte];
      di_byte_read_bit &= static_cast<irs_u8>(mask);
      return to_irs_bool(di_byte_read_bit);
    }
    if ((m_coils_size_byte != 0) || (m_coils_start_byte != 0)) {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" read bit: coils"));
      irs_u8 coils_byte_read_bit = m_coils_byte[m_coils_start_byte];
      coils_byte_read_bit &= static_cast<irs_u8>(mask);
      return to_irs_bool(coils_byte_read_bit);
    }
    if ((m_hold_registers_size_byte != 0) || (m_hold_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" read bit: hold regs"));
      irs_u8 hr_byte_read_bit = 0;
      if (m_hold_registers_start_byte%2 == 0) {
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
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" read bit: input regs"));
      irs_u8 ir_byte_read_bit = 0;
      if (m_input_registers_start_byte%2 == 0) {
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
    if ((m_coils_size_byte == 0)&&(m_discret_inputs_size_byte == 0)&&
      (m_hold_registers_size_byte == 0)&&(m_input_registers_size_byte == 0))
    {
      mlog() << irsm("\n***************Illegal index or bufer size"
        "******************\n") << endl;
    }
  );
  return false;
}

void irs::modbus_server_t::set_bit(irs_uarc a_byte_index, irs_uarc a_bit_index)
{
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("set bit"));
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
    if ((m_coils_size_byte == 0) && (m_discret_inputs_size_byte == 0) &&
      (m_hold_registers_size_byte == 0) && (m_input_registers_size_byte == 0))
    {
      mlog() << irsm("\n************Illegal index or bufer size"
        "***************\n") << endl;
    }
  );
  
  irs_u8 mask = mask_gen(8 - (a_bit_index + 1), 1);
  if (a_bit_index < 8) {
    if ((m_discret_inputs_size_byte != 0)||(m_discret_inputs_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" set bit: discret inputs"));
      m_discr_inputs_byte[m_discret_inputs_start_byte] &=
        static_cast<irs_u8>(~mask);
      m_discr_inputs_byte[m_discret_inputs_start_byte] |= 
        static_cast<irs_u8>(mask);
    }
    if ((m_coils_size_byte != 0)||(m_coils_start_byte != 0)) {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" set bit: coils"));
      m_coils_byte[m_coils_start_byte] &= static_cast<irs_u8>(~mask);
      m_coils_byte[m_coils_start_byte] |= static_cast<irs_u8>(mask);
    }
    if ((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" set bit: hold regs"));
      irs_u8 hr_byte_set_bit = 0;
      if (m_hold_registers_start_byte%2 == 0) {
        hr_byte_set_bit = 
          IRS_LOBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]);
      } else {
        hr_byte_set_bit = 
          IRS_HIBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]);
      }
      hr_byte_set_bit &= static_cast<irs_u8>(~mask);
      hr_byte_set_bit |= static_cast<irs_u8>(mask);
      if (m_hold_registers_start_byte%2 == 0) {
        IRS_LOBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]) =
          hr_byte_set_bit;
      } else {
        IRS_HIBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]) =
          hr_byte_set_bit;
      }
    }
    if ((m_input_registers_size_byte != 0)||(m_input_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" set bit: input regs"));
      irs_u8 ir_byte_set_bit = 0;
      if (m_input_registers_start_byte%2 == 0) {
        ir_byte_set_bit = 
          IRS_LOBYTE(m_input_regs_reg[m_input_registers_start_byte/2]);
      } else {
        ir_byte_set_bit = 
          IRS_HIBYTE(m_input_regs_reg[m_input_registers_start_byte/2]);
      }
      ir_byte_set_bit &= static_cast<irs_u8>(~mask);
      ir_byte_set_bit |= static_cast<irs_u8>(mask);
      if (m_input_registers_start_byte%2 == 0) {
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
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("clear bit"));
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
    if ((m_coils_size_byte == 0) && (m_discret_inputs_size_byte == 0) &&
      (m_hold_registers_size_byte == 0) && (m_input_registers_size_byte == 0))
    {
      mlog() << irsm("\n****************Illegal index or bufer size"
        "***************\n") << endl;
    }
  );
  
  irs_u8 mask = mask_gen(8 - (a_bit_index + 1), 1);
  if (a_bit_index < 8) {
    if ((m_discret_inputs_size_byte != 0)||(m_discret_inputs_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" clear bit: discret inputs"));
      m_discr_inputs_byte[m_discret_inputs_start_byte] &=
        static_cast<irs_u8>(~mask);
    }
    if ((m_coils_size_byte != 0)||(m_coils_start_byte != 0)) {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" clear bit: coils"));
      m_coils_byte[m_coils_start_byte] &= static_cast<irs_u8>(~mask);
    }
    if ((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" clear bit: hold regs"));
      irs_u8 hr_byte_clear_bit = 0;
      if (m_hold_registers_start_byte%2 == 0) {
        hr_byte_clear_bit = 
          IRS_LOBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]);
      } else {
        hr_byte_clear_bit = 
          IRS_HIBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]);
      }
      hr_byte_clear_bit &= static_cast<irs_u8>(~mask);
      if (m_hold_registers_start_byte%2 == 0) {
        IRS_LOBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]) = 
          hr_byte_clear_bit;
      } else {
        IRS_HIBYTE(m_hold_regs_reg[m_hold_registers_start_byte/2]) = 
          hr_byte_clear_bit;
      }
    }
    if ((m_input_registers_size_byte != 0)||(m_input_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" clear bit: input regs"));
      irs_u8 ir_byte_clear_bit = 0;
      if (m_input_registers_start_byte%2 == 0) {
        ir_byte_clear_bit = 
          IRS_LOBYTE(m_input_regs_reg[m_input_registers_start_byte/2]);
      } else {
        ir_byte_clear_bit = 
          IRS_HIBYTE(m_input_regs_reg[m_input_registers_start_byte/2]);
      }
      ir_byte_clear_bit &= static_cast<irs_u8>(~mask);
      if (m_input_registers_start_byte%2 == 0) {
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
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("error_response.................."));
  MBAP_header_t &header = reinterpret_cast<MBAP_header_t&>(*mp_buf.data());
  header.length = size_of_req_excep;
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
  
  switch(m_mode)
  {
    case read_header_mode:
    {
      m_channel = mp_hardflow_server->channel_next();
      if (m_channel != hardflow_t::invalid_channel) {
        m_fixed_flow.read(m_channel, mp_buf.data(), size_of_MBAP);
        m_mode = read_request_mode;
        view_mode();
      }
    }
    break;
    case read_request_mode:
    {
      if (m_fixed_flow.read_status() == 
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
        view_mode();
      } else if (m_fixed_flow.read_status() == 
        irs::hardflow::fixed_flow_t::status_error)
      {
        m_mode = read_header_mode;
        view_mode();
      }
    }
    break;
    case read_end_mode:
    {
      if (m_fixed_flow.read_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      {
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
            for(size_t idx = 0; idx < 20; idx++) {
              mlog() << "mp_buf[" << idx << "] = " << int(mp_buf[idx]) << endl;
            }
          } break;
        }
        IRS_LIB_IRSMBUS_DBG_MONITOR(
          modbus_pack_request_monitor(mp_buf.data()););
        MBAP_header_t &header = 
          reinterpret_cast<MBAP_header_t&>(*mp_buf.data());
        request_t &req_header = 
          reinterpret_cast<request_t&>(*(mp_buf.data() + size_of_MBAP));
        irs_u16 num_of_elem = req_header.quantity;
        irs_u16 start_addr = req_header.starting_address;
        
        switch(req_header.function_code)
        {
          case read_discrete_inputs:
          {
            if(num_of_elem != 0) {
              response_read_byte_t &read_di = 
                reinterpret_cast<response_read_byte_t&>(*(mp_buf.data() +
                size_of_MBAP));
              if(num_of_elem%8 == 0) {
                read_di.byte_count = static_cast<irs_u8>(num_of_elem/8);
              } else {
                read_di.byte_count = static_cast<irs_u8>(num_of_elem/8 + 1);
              }
              header.length = irs_u16(1 + size_of_resp_header + 
                read_di.byte_count);
              IRS_LIB_ASSERT(static_cast<size_t>(start_addr + num_of_elem) <=
                m_discr_inputs_byte.size()*8);
              bit_copy(m_discr_inputs_byte.data(), read_di.value,
                start_addr, 0, num_of_elem);
              IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
                for(int di_idx = 0; di_idx < read_di.byte_count; di_idx++)
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
            if(num_of_elem != 0) {
              response_read_byte_t &read_coils = 
                reinterpret_cast<response_read_byte_t&>(*(mp_buf.data() + 
                size_of_MBAP));
              if(num_of_elem%8 == 0) {
                read_coils.byte_count = static_cast<irs_u8>(num_of_elem/8);
              } else {
                read_coils.byte_count = 
                  static_cast<irs_u8>(num_of_elem/8 + 1);
              }
              header.length = irs_u16(1 + size_of_resp_header +
                read_coils.byte_count);
              IRS_LIB_ASSERT(static_cast<size_t>(start_addr + num_of_elem) <=
                m_coils_byte.size()*8);
              bit_copy(m_coils_byte.data(), read_coils.value, start_addr, 0,
                num_of_elem);
              IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
                for(int coils_idx = 0; coils_idx < read_coils.byte_count;
                  coils_idx++)
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
            if(num_of_elem != 0) {
              response_read_reg_t &read_hr = 
                reinterpret_cast<response_read_reg_t&>(
                *(mp_buf.data() + size_of_MBAP));
              read_hr.byte_count = static_cast<irs_u8>(num_of_elem*2);
              irs_u8* hold_regs_data = 
                reinterpret_cast<irs_u8*>(m_hold_regs_reg.data());
              irs_u8* hr_value_data = 
                reinterpret_cast<irs_u8*>(read_hr.value);
              IRS_LIB_ASSERT(static_cast<size_t>(start_addr + num_of_elem) <=
                m_hold_regs_reg.size());
              memcpyex(hr_value_data, hold_regs_data + start_addr*2,
                read_hr.byte_count);
              
              IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
                for(size_t hr_idx = start_addr;
                  hr_idx < (start_addr + num_of_elem);
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
                num_of_elem*2);
            } else {
              error_response(ILLEGAL_DATA_ADDRESS);
            }
          }
          break;
          case read_input_registers:
          {
            if (num_of_elem != 0) {
              response_read_reg_t &read_ir = 
                reinterpret_cast<response_read_reg_t&>(*(mp_buf.data() + 
                size_of_MBAP));
              read_ir.byte_count = static_cast<irs_u8>(num_of_elem*2);
              irs_u8* input_regs_data = 
                reinterpret_cast<irs_u8*>(m_input_regs_reg.data());
              irs_u8* ir_value_data =
                reinterpret_cast<irs_u8*>(read_ir.value);
              IRS_LIB_ASSERT(static_cast<size_t>(start_addr + num_of_elem) <=
                m_input_regs_reg.size());
              memcpyex(ir_value_data, input_regs_data + start_addr*2,
                read_ir.byte_count);  
              IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
                for(size_t ir_idx = start_addr;
                  ir_idx < (start_addr + num_of_elem);
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
                num_of_elem*2);
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
            irs_u8 coil_byte = 
              static_cast<irs_u8>(response_write_coil.address/8);
            irs_u8 mask = 
              mask_gen(8 - (response_write_coil.address%8 + 1), 1);
            if (int(response_write_coil.value) == 1) {
              m_coils_byte[coil_byte] &= static_cast<irs_u8>(~mask);
              m_coils_byte[coil_byte] |= static_cast<irs_u8>(mask);
            } else {
              m_coils_byte[coil_byte] &= static_cast<irs_u8>(~mask);
            }
          }
          break;
          case write_multiple_coils:
          { 
            if (num_of_elem != 0) {
              request_multiple_write_byte_t &write_multi_coils =
                reinterpret_cast<request_multiple_write_byte_t&>(
                *(mp_buf.data() + size_of_MBAP));
              IRS_LIB_ASSERT(static_cast<size_t>(
                (write_multi_coils.starting_address +
                write_multi_coils.quantity)) <= m_coils_byte.size()*8);
              bit_copy(write_multi_coils.value, m_coils_byte.data(), 0,
                size_t(write_multi_coils.starting_address), 
                size_t(write_multi_coils.quantity));
              header.length = irs_u16(1 + 1 + size_of_read_header);
            } else {
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
            if (num_of_elem != 0) {
              request_multiple_write_regs_t &write_multi_regs =
                reinterpret_cast<request_multiple_write_regs_t&>(
                *(mp_buf.data() + size_of_MBAP));
              irs_u8* hold_regs_data =
                reinterpret_cast<irs_u8*>(m_hold_regs_reg.data());
              irs_u8* multi_hr_data =
                reinterpret_cast<irs_u8*>(write_multi_regs.value);
              IRS_LIB_ASSERT(static_cast<size_t>(
                (write_multi_regs.starting_address +
                write_multi_regs.byte_count/2)) <= m_hold_regs_reg.size());
              memcpyex(hold_regs_data + write_multi_regs.starting_address*2,
                multi_hr_data, write_multi_regs.byte_count);
              header.length = irs_u16(1 + 1 + size_of_read_header);
            } else {
              error_response(ILLEGAL_DATA_ADDRESS);
            }
          }
          break;
          case read_exception_status:
          {
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("\n read_exception_status"));
            header.length = size_of_req_excep;
          }
          break;
          default:
          {
            error_response(ILLEGAL_FUNCTION);
          }
          break;
        }
        m_mode = send_response_mode;
        view_mode();
        m_size_byte_end = header.length - 1;
      } else if (m_fixed_flow.read_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      {
        m_mode = read_header_mode;
        view_mode();
      }
    }
    break;
    case send_response_mode:
    {
      IRS_LIB_IRSMBUS_DBG_MONITOR(
        modbus_pack_response_monitor(mp_buf.data()););
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
      IRS_LIB_ASSERT((size_of_MBAP + m_size_byte_end) <= mp_buf.size());
      m_fixed_flow.write(m_channel, mp_buf.data(), size_of_MBAP + 
        m_size_byte_end);
      m_mode = write_end;
      view_mode();
    }
    break;
    case write_end:
    {
      if (m_fixed_flow.write_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      { 
        memset(mp_buf.data(), 0, mp_buf.size());
        m_mode = read_header_mode;
        view_mode();
      } else if (m_fixed_flow.write_status() == 
        irs::hardflow::fixed_flow_t::status_error)
      {
        m_mode = read_header_mode;
        view_mode();
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
  counter_t a_disconnect_time,
  irs_u16 a_size_of_packet,
  irs_u8 a_unit_id
):
  m_size_of_packet((a_size_of_packet > packet_size_max)? packet_size_max :
    ((a_size_of_packet < packet_size_min)? packet_size_min : a_size_of_packet)),
  m_size_of_data_write_byte(static_cast<irs_u16>(m_size_of_packet - 14)),
  m_size_of_data_read_byte(static_cast<irs_u16>(m_size_of_packet - 10)),
  m_size_of_data_write_reg(static_cast<irs_u16>(m_size_of_data_write_byte/2)),
  m_size_of_data_read_reg(static_cast<irs_u16>(m_size_of_data_read_byte/2)),
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
  m_spacket(m_size_of_packet),
  m_rpacket(m_size_of_packet),
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
  m_need_writes(m_coils_size_bit + a_hold_regs_reg),
  m_need_writes_reserve(m_coils_size_bit + a_hold_regs_reg),
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
  m_first_read(false),
  m_measure_time(),
  m_measure_int_time(0),
  m_operation_status(status_completed),
  m_refresh_mode(a_refresh_mode),
  m_start(false),
  m_write_quantity(0),
  m_read_quantity(0),
  m_error_count_max(a_error_count_max),
  m_transaction_id(0),
  m_request_quantity_discr_inputs_bit(0),
  m_request_quantity_coils_bit(0),
  m_error_count(0),
  m_unit_id(a_unit_id)
{  
  //m_send_request_timer.set(a_update_time/get_packet_number());
  m_send_request_timer.set(0);
  m_fixed_flow.read_timeout(a_disconnect_time);
  m_fixed_flow.write_timeout(a_disconnect_time);
  init_to_cnt();
  m_measure_time.start();
  mlog() << irsm("CLIENT START!") << endl;
}

irs::modbus_client_t::~modbus_client_t()
{
  deinit_to_cnt();
}

char const IRS_CSTR_NONVOLATILE irs::modbus_client_t::m_wait_command_mode[] =
  "m_mode: wait_command_mode";
char const IRS_CSTR_NONVOLATILE
  irs::modbus_client_t::m_search_write_data_mode[] =
  "m_mode: search_write_data_mode";
char const IRS_CSTR_NONVOLATILE
  irs::modbus_client_t::m_request_write_data_mode[] =
  "m_mode: request_write_data_mode";
char const IRS_CSTR_NONVOLATILE
  irs::modbus_client_t::m_convert_request_mode[] =
  "m_mode: convert_request_mode";
char const IRS_CSTR_NONVOLATILE irs::modbus_client_t::m_send_request_mode[] =
  "m_mode: send_request_mode";
char const IRS_CSTR_NONVOLATILE irs::modbus_client_t::m_read_header_mode[] =
  "m_mode: read_header_mode";
char const IRS_CSTR_NONVOLATILE irs::modbus_client_t::m_read_response_mode[] =
  "m_mode: read_response_mode";
char const IRS_CSTR_NONVOLATILE
  irs::modbus_client_t::m_treatment_response_mode[] =
  "m_mode: treatment_response_mode";
char const IRS_CSTR_NONVOLATILE
  irs::modbus_client_t::m_search_read_data_mode[] =
  "m_mode: search_read_data_mode";
char const IRS_CSTR_NONVOLATILE
  irs::modbus_client_t::m_request_read_data_mode[] =
  "m_mode: request_read_data_mode";
char const IRS_CSTR_NONVOLATILE
  irs::modbus_client_t::m_make_request_mode[] =
  "m_mode: make_request_mode";

IRS_CSTR_NONVOLATILE char const IRS_CSTR_NONVOLATILE* const 
  irs::modbus_client_t::m_ident_name_list[] =
{
  m_wait_command_mode,
  m_search_write_data_mode,
  m_request_write_data_mode,
  m_convert_request_mode,
  m_send_request_mode,
  m_read_header_mode,
  m_read_response_mode,
  m_treatment_response_mode,
  m_search_read_data_mode,
  m_request_read_data_mode,
  m_make_request_mode
};

irs_uarc irs::modbus_client_t::size()
{
  return m_input_registers_end_byte;
}

size_t irs::modbus_client_t::get_packet_number()
{
  size_t di_pack_num = 0;
  if ((m_discr_inputs_byte_read.size() % m_size_of_data_read_byte) == 0) {
    di_pack_num = m_discr_inputs_byte_read.size()/m_size_of_data_read_byte;
  } else {
    di_pack_num =
      m_discr_inputs_byte_read.size()/m_size_of_data_read_byte + 1;
  }
  size_t coils_pack_num = 0;
  if ((m_coils_byte_read.size() % m_size_of_data_read_byte) == 0) {
    coils_pack_num = m_coils_byte_read.size()/m_size_of_data_read_byte;
  } else {
    coils_pack_num = m_coils_byte_read.size()/m_size_of_data_read_byte + 1;
  }
  size_t hold_reg_pack_num = 0;
  if ((m_hold_registers_size_reg % m_size_of_data_read_reg) == 0) {
    hold_reg_pack_num = 
      m_hold_registers_size_reg/m_size_of_data_read_reg;
  } else {
    hold_reg_pack_num =
      m_hold_registers_size_reg/m_size_of_data_read_reg + 1;
  }
  size_t input_reg_pack_num = 0;
  if ((m_input_registers_size_reg % m_size_of_data_read_reg) == 0) {
    input_reg_pack_num =
      m_input_registers_size_reg/m_size_of_data_read_reg;
  } else {
    input_reg_pack_num =
      m_input_registers_size_reg/m_size_of_data_read_reg + 1;
  }
  return (di_pack_num + coils_pack_num +
    hold_reg_pack_num + input_reg_pack_num);
}

irs_bool irs::modbus_client_t::connected()
{
  irs_bool connect = false;
  if (m_refresh_mode == mode_refresh_auto) {
    if (m_first_read) {
      connect = true;
    } else {
      connect = false;
    }
  } else if (m_refresh_mode == mode_refresh_manual) {
    connect = true;
  }
  if ((m_fixed_flow.write_status() == 
    irs::hardflow::fixed_flow_t::status_error) ||
    (m_fixed_flow.read_status() == 
    irs::hardflow::fixed_flow_t::status_error))
  {
    connect = false;
  }
  return connect;
}

void irs::modbus_client_t::reconnect()
{
  m_read_table = false;
  m_write_table = false;
  m_write_complete = true;
  m_start_block = 0;
  m_search_index = 0;
  m_first_read = false;
  m_start = false;
  m_error_count = 0;
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
  // В IAR возникает предупреждение, что ap_buf не используется,
  // если сообщения выключены. Для выключения предупреждения
  // предназначен текст приведенный ниже
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
  IRS_LIB_ASSERT((a_index + a_size) <= m_input_registers_end_byte);
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("read"));
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
  
  if ((m_discret_inputs_size_byte != 0) || (m_discret_inputs_start_byte != 0))
  {
    memcpyex(ap_buf, m_discr_inputs_byte_read.data() + 
      m_discret_inputs_start_byte, m_discret_inputs_size_byte);
  }
  if ((m_coils_size_byte != 0) || (m_coils_start_byte != 0)) {
    memcpyex(ap_buf + m_discret_inputs_size_byte, 
      m_coils_byte_read.data() + m_coils_start_byte, m_coils_size_byte);
  }
  if ((m_hold_registers_size_byte != 0) || (m_hold_registers_start_byte != 0))
  {
    size_t ap_buf_start = m_discret_inputs_size_byte + m_coils_size_byte;
    irs_u8* hold_regs_data =
      reinterpret_cast<irs_u8*>(m_hold_regs_reg_read.data());
    memcpyex(ap_buf + ap_buf_start, hold_regs_data +
      m_hold_registers_start_byte, m_hold_registers_size_byte);
  }
  if ((m_input_registers_size_byte != 0) || (m_input_registers_start_byte != 0))
  {
    size_t ap_buf_start = m_discret_inputs_size_byte + m_coils_size_byte +
      m_hold_registers_size_byte;
    irs_u8* input_regs_data = 
      reinterpret_cast<irs_u8*>(m_input_regs_reg_read.data());
    memcpyex(ap_buf + ap_buf_start, input_regs_data + 
      m_input_registers_start_byte, m_input_registers_size_byte);
  }
}

void irs::modbus_client_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  IRS_LIB_ASSERT((a_index + a_size) <= m_input_registers_end_byte);
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" write"));
  m_coils_size_byte = 0, m_coils_start_byte = 0;
  
  m_hold_registers_size_byte = 0, m_hold_registers_start_byte = 0;
  range(a_index, a_size, 0, m_discret_inputs_end_byte, 
    &m_discret_inputs_size_byte, &m_discret_inputs_start_byte);
  range(a_index, a_size, m_discret_inputs_end_byte, m_coils_end_byte,
    &m_coils_size_byte, &m_coils_start_byte);
  range(a_index, a_size, m_coils_end_byte, m_hold_registers_end_byte, 
    &m_hold_registers_size_byte, &m_hold_registers_start_byte);
  range(a_index, a_size, m_hold_registers_end_byte, m_input_registers_end_byte, 
    &m_input_registers_size_byte, &m_input_registers_start_byte);
    
  if ((m_coils_size_byte != 0)||(m_coils_start_byte != 0)) {
    IRS_LIB_ASSERT((m_coils_start_byte + m_coils_size_byte) <= 
      m_coils_byte_write.size());
    size_t ap_buf_start = m_discret_inputs_size_byte;
    IRS_LIB_ASSERT((m_coils_start_byte + m_coils_size_byte) <= 
      m_coils_byte_write.size());
    memcpyex(m_coils_byte_write.data() + m_coils_start_byte,
      ap_buf + ap_buf_start, m_coils_size_byte);
    if (m_refresh_mode == mode_refresh_auto) {
      for(size_t i = m_coils_start_byte*8;
        i < (m_coils_start_byte*8 + m_coils_size_byte*8); i++)
      {
        m_need_writes[i] = 1;
      }
    }
  }
  if ((m_hold_registers_size_byte != 0) || (m_hold_registers_start_byte != 0))
  {
    size_t ap_buf_start = m_discret_inputs_size_byte + m_coils_size_byte;
    irs_u8* hold_regs_data = 
      reinterpret_cast<irs_u8*>(m_hold_regs_reg_write.data());
    IRS_LIB_ASSERT((m_hold_registers_start_byte +
      m_hold_registers_size_byte) <= m_hold_regs_reg_write.size()*2);
    memcpyex(hold_regs_data + m_hold_registers_start_byte,
      ap_buf + ap_buf_start, m_hold_registers_size_byte);
    if (m_hold_registers_size_byte%2) {
      irs_u8* hold_regs_data_rest =
        reinterpret_cast<irs_u8*>(m_hold_regs_reg_read.data());
      if (m_hold_registers_start_byte%2) {
        memcpyex(hold_regs_data + m_hold_registers_start_byte - 1,
          hold_regs_data_rest + m_hold_registers_start_byte - 1, 1);
      } else {
        memcpyex(hold_regs_data + m_hold_registers_start_byte +
          m_hold_registers_size_byte, hold_regs_data_rest +
          m_hold_registers_start_byte + m_hold_registers_size_byte, 1);
      }
    }
    if (m_refresh_mode == mode_refresh_auto) {
      size_t start_block = 0;
      if (m_hold_registers_start_byte%2) {
        start_block = m_coils_size_bit +
          (m_hold_registers_start_byte - 1)/2;
      } else {
        start_block = m_coils_size_bit + m_hold_registers_start_byte/2;
      }
      size_t end_block = 0;
      if (m_hold_registers_size_byte%2) {
        end_block = start_block + (m_hold_registers_size_byte + 1)/2;
      } else {
        end_block = start_block + m_hold_registers_size_byte/2;
      }
      for(size_t i = start_block; i < end_block; i++) {
        m_need_writes[i] = 1;
      }
    }                    
  }
  if (m_refresh_mode == mode_refresh_auto) {
    m_operation_status = status_wait;
  }
}

irs_bool irs::modbus_client_t::bit(irs_uarc a_byte_index, irs_uarc a_bit_index)
{
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("read bit"));
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
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" read bit: discret inputs"));
      irs_u8 di_byte_read_bit = 
        m_discr_inputs_byte_read[m_discret_inputs_start_byte];
      di_byte_read_bit &= static_cast<irs_u8>(mask);
      return to_irs_bool(di_byte_read_bit);
    }
    if ((m_coils_size_byte != 0) || (m_coils_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" read bit: coils"));
      irs_u8 coils_byte_read_bit = m_coils_byte_read[m_coils_start_byte];
      coils_byte_read_bit &= static_cast<irs_u8>(mask);
      return to_irs_bool(coils_byte_read_bit);
    }
    if ((m_hold_registers_size_byte != 0) || (m_hold_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" read bit: hold regs"));
      irs_u8 hr_byte_read_bit = 0;
      if (m_hold_registers_start_byte%2 == 0) 
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
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" read bit: input regs"));
      irs_u8 ir_byte_read_bit = 0;
      if (m_input_registers_start_byte%2 == 0) 
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
    if ((m_coils_size_byte == 0) && (m_discret_inputs_size_byte == 0) &&
      (m_hold_registers_size_byte == 0) && (m_input_registers_size_byte == 0))
    {
      mlog() << irsm("\n***************Illegal index or bufer size"
        "******************\n");
    }
  );
  return false;
}

void irs::modbus_client_t::set_bit(irs_uarc a_byte_index, irs_uarc a_bit_index)
{
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" set_bit"));
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
    if ((m_coils_size_byte != 0)||(m_coils_start_byte != 0)) {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" set bit: coils"));
      m_coil_write_bit = 1;
      m_coil_bit_index = a_byte_index*8 - m_discret_inputs_size_bit + 
        a_bit_index;
      #ifdef NOP
      m_coils_byte_write[m_coil_bit_index/8] &= static_cast<irs_u8>(~mask);
      m_coils_byte_write[m_coil_bit_index/8] |= static_cast<irs_u8>(mask);
      if (m_refresh_mode == mode_refresh_auto) {
        m_need_writes[m_coil_bit_index] = 1;
      }
      #else // NOP
      m_coils_byte_write[m_coil_bit_index/8] &= static_cast<irs_u8>(~mask);
      m_coils_byte_write[m_coil_bit_index/8] |= static_cast<irs_u8>(mask);
      if (m_refresh_mode == mode_refresh_auto) {
        if (!m_need_writes[m_coil_bit_index]) {
          m_need_writes[m_coil_bit_index] = 1;
        } else {
          m_need_writes_reserve[m_coil_bit_index] = 1;
        }
      }
      #endif // NOP
    }
    if ((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" set bit: hold regs"));
      m_hold_regs_reg_write[0] = 
        m_hold_regs_reg_read[m_hold_registers_start_byte/2];
      irs_u8 hr_byte_set_bit = 0;
      if (m_hold_registers_start_byte%2 == 0) {
        hr_byte_set_bit = 
          IRS_LOBYTE(m_hold_regs_reg_read[m_hold_registers_start_byte/2]);
      } else {
        hr_byte_set_bit = 
          IRS_HIBYTE(m_hold_regs_reg_read[m_hold_registers_start_byte/2]);
      }
      hr_byte_set_bit &= static_cast<irs_u8>(~mask);
      hr_byte_set_bit |= static_cast<irs_u8>(mask);
      if (m_hold_registers_start_byte%2 == 0) {
        IRS_LOBYTE(m_hold_regs_reg_write[0]) = hr_byte_set_bit;
      } else { 
        IRS_HIBYTE(m_hold_regs_reg_write[0]) = hr_byte_set_bit;
      }
      #ifdef NOP
      if (m_refresh_mode == mode_refresh_auto) {
        m_need_writes[m_coils_size_bit + m_hold_registers_start_byte/2] = 1;
      }
      #else // NOP
      if (m_refresh_mode == mode_refresh_auto) {
        if (!m_need_writes[m_coils_size_bit + m_hold_registers_start_byte/2]) {
          m_need_writes[m_coils_size_bit + m_hold_registers_start_byte/2] = 1;
        } else {
          m_need_writes_reserve[m_coils_size_bit +
            m_hold_registers_start_byte/2] = 1;
        }
      }
      #endif // NOP
    }
  }
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    if ((m_coils_size_byte == 0)&&(m_hold_registers_size_byte == 0)) {
      mlog() << irsm("ERROR\n");
    }
  );
  if (m_refresh_mode == mode_refresh_auto) {
    m_operation_status = status_wait;
  }
}

void irs::modbus_client_t::clear_bit(irs_uarc a_byte_index, 
  irs_uarc a_bit_index)
{
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("clear_bit"));
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
    if ((m_coils_size_byte != 0)||(m_coils_start_byte != 0)) {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" clear bit: coils"));
      m_coil_write_bit = 0;
      m_coil_bit_index = a_byte_index*8 - m_discret_inputs_size_bit + 
        a_bit_index;
      //irs_u8 mask = mask_gen(8 - (m_coil_bit_index%8 + 1), 1);
      #ifdef NOP
      m_coils_byte_write[m_coil_bit_index/8] &= static_cast<irs_u8>(~mask);
      if (m_refresh_mode == mode_refresh_auto) {
        m_need_writes[m_coil_bit_index] = true;
      }
      #else // NOP
      m_coils_byte_write[m_coil_bit_index/8] &= static_cast<irs_u8>(~mask);
      if (m_refresh_mode == mode_refresh_auto) {
        if (!m_need_writes[m_coil_bit_index]) {
          m_need_writes[m_coil_bit_index] = 1;
        } else {
          m_need_writes_reserve[m_coil_bit_index] = 1;
        }
      }
      #endif // NOP
    }
    if ((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
    {
      IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" clear bit: hold regs"));
      m_hold_regs_reg_write[0] = 
        m_hold_regs_reg_read[m_hold_registers_start_byte/2];
      irs_u8 hr_byte_clear_bit = 0;
      if (m_hold_registers_start_byte%2 == 0) {
        hr_byte_clear_bit = 
          IRS_LOBYTE(m_hold_regs_reg_read[m_hold_registers_start_byte/2]);
      } else {
        hr_byte_clear_bit = 
          IRS_HIBYTE(m_hold_regs_reg_read[m_hold_registers_start_byte/2]);
      }
      hr_byte_clear_bit &= static_cast<irs_u8>(~mask);
      if (m_hold_registers_start_byte%2 == 0) 
        IRS_LOBYTE(m_hold_regs_reg_write[0]) = hr_byte_clear_bit;
      else 
        IRS_HIBYTE(m_hold_regs_reg_write[0]) = hr_byte_clear_bit;
      #ifdef NOP
      if (m_refresh_mode == mode_refresh_auto) {
        m_need_writes[m_coils_size_bit + m_hold_registers_start_byte/2] = 
          true;
      }
      #else // NOP
      if (m_refresh_mode == mode_refresh_auto) {
        if (!m_need_writes[m_coils_size_bit + m_hold_registers_start_byte/2]) {
          m_need_writes[m_coils_size_bit + m_hold_registers_start_byte/2] = 1;
        } else {
          m_need_writes_reserve[m_coils_size_bit +
            m_hold_registers_start_byte/2] = 1;
        }
      }
      #endif // NOP
    }
  }
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    if ((m_coils_size_byte == 0)&&(m_hold_registers_size_byte == 0)) {
      mlog() << irsm("ERROR");
    }
  );
  if (m_refresh_mode == mode_refresh_auto) {
    m_operation_status = status_wait;
  }
}

void irs::modbus_client_t::mark_to_send(irs_uarc a_index, irs_uarc a_size)
{
  if (((m_refresh_mode == mode_refresh_manual) && 
    (m_operation_status == status_completed)) ||
    (m_refresh_mode == mode_refresh_auto))
  {
    m_coils_size_byte = 0, m_coils_start_byte = 0;
    m_hold_registers_size_byte = 0, m_hold_registers_start_byte = 0;

    range(a_index, a_size, m_discret_inputs_end_byte, m_coils_end_byte, 
      &m_coils_size_byte, &m_coils_start_byte);
    range(a_index, a_size, m_coils_end_byte, m_hold_registers_end_byte, 
      &m_hold_registers_size_byte, &m_hold_registers_start_byte);

    if ((m_coils_size_byte != 0)||(m_coils_start_byte != 0)) {
      for(size_t i = m_coils_start_byte*8; 
        i < (m_coils_start_byte*8 + m_coils_size_byte*8); i++)
      {
        m_need_writes[i] = 1;
      }
    }
    if ((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
    {
      size_t hr_size_byte = 0;
      if ((m_hold_registers_size_byte%2 == 0) &&
        (m_hold_registers_start_byte%2 == 0)) {
        hr_size_byte = m_hold_registers_size_byte;
      } else if ((m_hold_registers_size_byte%2 == 0) && 
        (m_hold_registers_start_byte%2 != 0)) {
        hr_size_byte = m_hold_registers_size_byte + 2;
      } else {
        hr_size_byte = m_hold_registers_size_byte + 1;
      }
      size_t hr_index = 0;
      if (m_hold_registers_start_byte%2 == 0) {
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
    if (m_refresh_mode == mode_refresh_auto) {
      m_operation_status = status_wait;
    }
  }
}

void irs::modbus_client_t::mark_to_recieve(irs_uarc a_index, irs_uarc a_size)
{
  if (((m_refresh_mode == mode_refresh_manual) && 
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
    if (m_discret_inputs_size_byte != 0) {
      for(read_index = m_discret_inputs_start_byte*8;
        read_index < (m_discret_inputs_start_byte*8 + 
        m_discret_inputs_size_byte*8); read_index ++)
      {
        m_need_read[read_index] = 1;
      }
    }
    if (m_coils_size_byte != 0) {
      for(read_index = m_discret_inputs_size_bit + m_coils_start_byte*8;
        read_index < (m_discret_inputs_size_bit + m_coils_start_byte*8 +
        m_coils_size_byte*8); read_index ++)
      {
        m_need_read[read_index] = 1;
      }
    }
    if (m_hold_registers_size_byte != 0) {
      size_t hr_size_byte = 0;
      if ((m_hold_registers_size_byte%2 == 0) &&
        (m_hold_registers_start_byte%2 == 0))
      {
        hr_size_byte = m_hold_registers_size_byte;
      } else if ((m_hold_registers_size_byte%2 == 0) && 
        (m_hold_registers_start_byte%2 != 0)) {
        hr_size_byte = m_hold_registers_size_byte + 2;
      } else {
        hr_size_byte = m_hold_registers_size_byte + 1;
      }
      size_t hr_index = 0;
      if (m_hold_registers_start_byte%2 == 0) {
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
    if (m_input_registers_size_byte != 0) {
      size_t ir_size_byte = 0;
      if ((m_input_registers_size_byte%2 == 0) &&
        (m_input_registers_start_byte%2 == 0)) {
        ir_size_byte = m_input_registers_size_byte;
      } else if ((m_input_registers_size_byte%2 == 0) && 
        (m_input_registers_start_byte%2 != 0)) {
        ir_size_byte = m_input_registers_size_byte + 2;
      } else {
        ir_size_byte = m_input_registers_size_byte + 1;
      }
      size_t ir_index = 0;
      if (m_input_registers_start_byte%2 == 0) {
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
    if (m_refresh_mode == mode_refresh_auto) {
      m_operation_status = status_wait;
    }
  }
}

void irs::modbus_client_t::mark_to_send_bit(irs_uarc a_byte_index,
  irs_uarc a_bit_index)
{
  if (((m_refresh_mode == mode_refresh_manual) && 
    (m_operation_status == status_completed)) ||
    (m_refresh_mode == mode_refresh_auto))
  {
    IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" set_bit"));
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
      if ((m_coils_size_byte != 0)||(m_coils_start_byte != 0)) {
        IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" set bit: coils"));
        m_need_writes[a_byte_index*8 - m_discret_inputs_size_bit +
          a_bit_index] = 1;
      }
      if ((m_hold_registers_size_byte != 0)||(m_hold_registers_start_byte != 0))
      {
        IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" set bit: hold regs"));
        m_need_writes[m_coils_size_bit + m_hold_registers_start_byte/2] = 1;
      }
    }
    if (m_refresh_mode == mode_refresh_auto) {
      m_operation_status = status_wait;
    }
  }
}

void irs::modbus_client_t::mark_to_recieve_bit(irs_uarc a_byte_index,
  irs_uarc a_bit_index)
{
  if (((m_refresh_mode == mode_refresh_manual) && 
    (m_operation_status == status_completed)) || 
    (m_refresh_mode == mode_refresh_auto))
  {
    IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("read bit"));
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
        IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" read bit: discret inputs"));
        m_need_read[a_byte_index*8 + a_bit_index] = 1;
      }
      if ((m_coils_size_byte != 0) || 
        (m_coils_start_byte != 0))
      {
        IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" read bit: coils"));
        m_need_read[a_byte_index*8 - m_discret_inputs_size_bit -
          a_bit_index] = 1;
      }
      if ((m_hold_registers_size_byte != 0) || 
        (m_hold_registers_start_byte != 0))
      {
        IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" read bit: hold regs"));
        m_need_read[a_byte_index*8 - m_discret_inputs_size_bit - 
          m_coils_size_bit + a_bit_index] = 1;
      }
      if ((m_input_registers_size_byte != 0) || 
        (m_input_registers_start_byte != 0))
      {
        IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" read bit: input regs"));
        m_need_read[a_byte_index*8 - m_discret_inputs_size_bit - 
          m_coils_size_bit - m_hold_registers_size_reg + a_bit_index] = 1;
      }
    }
    if (m_refresh_mode == mode_refresh_auto) {
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
  if (m_refresh_mode == mode_refresh_manual) {
    m_operation_status = status_wait;
    m_start = true;
    m_read_table = true;
    m_write_table = true;
    IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" UPDATE"));
  }
}

void irs::modbus_client_t::abort()
{
  m_operation_status = status_completed;
  // Снятие меток чтения:
  size_t read_flags_size = m_discret_inputs_size_bit + m_coils_size_bit + 
    m_hold_registers_size_reg + m_input_registers_size_reg;
  m_need_read.clear();
  m_need_read.resize(read_flags_size);
  
  // Снятие меток записи:
  size_t write_flags_size = m_coils_size_bit + m_hold_registers_size_reg;
  m_need_writes.clear();
  m_need_writes.resize(write_flags_size);
  m_read_table = false;
  m_write_table = false;
  m_mode = wait_command_mode;
  m_start = false;
  m_search_index = 0;
  m_request_type = request_start;
  m_global_read_index = 0;
}

void irs::modbus_client_t::make_packet(size_t a_index, irs_u16 a_size)
{
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" make_packet"));
  MBAP_header_t &header = reinterpret_cast<MBAP_header_t&>(*m_spacket.data());
  m_size_byte_end = 0;
  switch(m_command)
  {
    case read_discrete_inputs:
    {
      request_t &sec_header = 
        reinterpret_cast<request_t&>(*(m_spacket.data() + size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      m_request_quantity_discr_inputs_bit = sec_header.quantity;
      header.length  = irs_u16(1 + sizeof(sec_header));
    }
    break;
    case read_coils:
    {
      request_t &sec_header = 
        reinterpret_cast<request_t&>(*(m_spacket.data() + size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      m_request_quantity_coils_bit = sec_header.quantity;
      header.length  = irs_u16(1 + sizeof(sec_header));
    }
    break;
    case read_hold_registers:
    {
      request_t &sec_header = 
        reinterpret_cast<request_t&>(*(m_spacket.data() + size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      header.length  = irs_u16(1 + sizeof(sec_header));
    }
    break;
    case read_input_registers:
    {
      request_t &sec_header = 
        reinterpret_cast<request_t&>(*(m_spacket.data() + size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = a_size;
      header.length  = irs_u16(1 + sizeof(sec_header));
    }
    break;
    case read_exception_status:
    {
      request_exception_t &sec_header = 
        reinterpret_cast<request_exception_t&>(*(m_spacket.data() +
        size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      header.length  = irs_u16(1 + sizeof(sec_header));
    }
    break;
    case write_single_coil:
    {
      response_single_write_t &sec_header = 
        reinterpret_cast<response_single_write_t&>(*(m_spacket.data() + 
        size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      sec_header.address = (irs_u16)a_index;
      header.length  = irs_u16(size_of_resp_multi_write);
    }
    break; 
    case write_single_register:
    {
      response_single_write_t &sec_header = 
        reinterpret_cast<response_single_write_t&>(*(m_spacket.data() +
        size_of_MBAP));
      sec_header.function_code = static_cast<irs_u8>(m_command);
      sec_header.address = (irs_u16)a_index;
      header.length  = irs_u16(size_of_resp_multi_write);
    }
    break;
    case write_multiple_coils:
    {
      request_multiple_write_byte_t &sec_header = 
        reinterpret_cast<request_multiple_write_byte_t&>(*(m_spacket.data() +
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
        reinterpret_cast<request_multiple_write_regs_t&>(*(m_spacket.data() +
        size_of_MBAP));
      sec_header.starting_address = (irs_u16)a_index;
      sec_header.quantity = static_cast<irs_u16>(a_size/2);
      sec_header.byte_count = static_cast<irs_u8>(a_size);
      header.length  = irs_u16(size_of_resp_multi_write + 
        sec_header.byte_count + 1);
      sec_header.function_code = static_cast<irs_u8>(m_command);
    }
    break;
    default : {
    }
  }
  header.transaction_id = IRS_NULL;
  header.proto_id = IRS_NULL;
  //header.unit_identifier  = 1;
  header.unit_identifier  = m_unit_id;
  m_size_byte_end = header.length - 1;
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
    for(irs_u16 send_pack_index = 0;
      send_pack_index < (header.length + size_of_MBAP); send_pack_index++)
    {
      mlog() << irsm(" m_spacket[") << send_pack_index << irsm("] = ") <<
        (int)(m_spacket[send_pack_index]) << endl;
    }
  );
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("\nPacket ready"));
}

void irs::modbus_client_t::set_delay_time(double a_time)
{
  m_loop_timer.set(make_cnt_s(a_time));
}

void irs::modbus_client_t::view_mode()
{
  IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(m_ident_name_list[m_mode]);
}

void irs::modbus_client_t::tick()
{ 
  mp_hardflow_client->tick();
  m_fixed_flow.tick();
  
  switch(m_mode)
  {
    case wait_command_mode:
    {
      if (m_refresh_mode == mode_refresh_manual) {
        if(m_start) {
          m_start = false;
          m_mode = search_write_data_mode;
        }
      } else if (m_refresh_mode == mode_refresh_auto) {
        m_mode = search_write_data_mode;
      }
      view_mode();
    }
    break;
    case search_write_data_mode:
    {
      if (m_coils_size_bit || m_hold_registers_size_reg) {
        m_mode = request_write_data_mode;
        irs_bool catch_block = false;
        m_write_quantity = 0;
        for(; m_search_index < (m_coils_size_bit + m_hold_registers_size_reg);)
        {
          if ((m_need_writes[m_search_index]) && (!catch_block)) {
            m_start_block = m_search_index;
            catch_block = true;
            m_write_complete = false;
          }
          if ((catch_block) && (!m_need_writes[m_search_index])) {
            m_write_quantity =
              static_cast<irs_u16>(m_search_index - m_start_block);
            if ((irs_u32(m_search_index) > irs_u32(m_coils_size_bit))&&
              (m_start_block < m_coils_size_bit))
            {
              m_write_quantity =
                static_cast<irs_u16>(m_coils_size_bit - m_start_block);
              m_search_index = m_coils_size_bit;
            }
            break;
          }
          m_search_index++;
          if ((catch_block) &&
            (m_search_index == (m_coils_size_bit + m_hold_registers_size_reg)))
          { 
            m_write_quantity =
              static_cast<irs_u16>(m_search_index - m_start_block);
            if (m_start_block < m_coils_size_bit) {
              m_write_quantity =
                static_cast<irs_u16>(m_coils_size_bit - m_start_block);
              m_search_index = m_coils_size_bit;
            } else {
              m_search_index = 0;
            }
            break;
          }
          if ((!catch_block) &&
            (m_search_index >= (m_coils_size_bit + m_hold_registers_size_reg)))
          {
            m_search_index = 0;
            m_write_table = false;
            size_t write_flags_cnt = 0;
            for(size_t write_idx = 0; write_idx < (m_coils_size_bit + 
              m_hold_registers_size_reg); write_idx++)
            {
              if (m_need_writes[write_idx] == 1) {
                write_flags_cnt++;
              }
            }
            if (write_flags_cnt == 0) {
              m_write_complete = true;
            }
            m_mode = search_read_data_mode;
            IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(
              irsm(" массив флагов на запись пуст"));
            break;
          }
        }
      } else {
        m_mode = search_read_data_mode;
      }
      view_mode();
    }
    break;
    case request_write_data_mode:
    {
      if (((m_start_block) < m_coils_size_bit) && (m_coils_size_bit)) {
        m_write_table = true;
        m_command = write_multiple_coils;
        request_multiple_write_byte_t &coils_packet =
          reinterpret_cast<request_multiple_write_byte_t&>(*(m_spacket.data() +
          size_of_MBAP));
        if (m_write_quantity > m_size_of_data_write_byte*8) {
          IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(
            irsm(" запись полного пакета coils"));
          make_packet(m_start_block,
            static_cast<irs_u16>(m_size_of_data_write_byte*8));
          coils_packet.byte_count =
            static_cast<irs_u8>(m_size_of_data_write_byte);
          bit_copy(m_coils_byte_write.data(), 
            reinterpret_cast<irs_u8*>(coils_packet.value), 
            m_start_block, 0, m_size_of_data_write_byte*8);
          m_search_index = m_start_block + m_size_of_data_write_byte*8;
        } else {
          IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(
            irsm(" запись не полного пакета coils"));
          if (m_write_quantity > 1) {
            make_packet(m_start_block, m_write_quantity);
            bit_copy(m_coils_byte_write.data(),
              reinterpret_cast<irs_u8*>(coils_packet.value), 
                m_start_block, 0, m_write_quantity);
          } else {
            m_command = write_single_coil;
            response_single_write_t &sec_head =
              reinterpret_cast<response_single_write_t&>(*(m_spacket.data() + 
              size_of_MBAP));
            //sec_head.value = m_coil_write_bit;
            //make_packet(m_coil_bit_index, 0);
            irs_u8 coil_index = static_cast<irs_u8>(m_start_block%8);
            irs_u8 mask = mask_gen(8 - (coil_index + 1), 1);
            if (m_coils_byte_write[m_start_block/8] & mask) {
              sec_head.value = 1;
            } else {
              sec_head.value = 0;
            }
            make_packet(m_start_block, 0);
          }
          m_search_index = m_start_block + m_write_quantity;
        }
      }
      //запись hold regs
      else if ((m_start_block >= m_coils_size_bit) && 
        (m_start_block <= static_cast<size_t>(m_coils_size_bit + 
        m_hold_registers_size_reg)) && (m_hold_registers_size_reg))
      {
        m_write_table = true;
        m_command = write_multiple_registers;
        size_t hr_start = m_start_block - m_coils_size_bit;
        if (m_write_quantity > m_size_of_data_write_reg) {
          IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(
            irsm(" запись полного пакета hold regs"));
          request_multiple_write_regs_t &hr_packet =
            reinterpret_cast<request_multiple_write_regs_t&>(
            *(m_spacket.data() + size_of_MBAP));
          hr_packet.byte_count =
            static_cast<irs_u8>(m_size_of_data_write_reg*2);
          irs_u8* hold_regs_data =
            reinterpret_cast<irs_u8*>(m_hold_regs_reg_write.data());
          irs_u8* hr_packet_data =
            reinterpret_cast<irs_u8*>(hr_packet.value);
          memcpyex(hr_packet_data, hold_regs_data + hr_start*2,
            m_size_of_data_write_reg*2);
          make_packet(hr_start,
            static_cast<irs_u16>(m_size_of_data_write_reg*2));
          m_search_index = m_coils_size_bit + hr_start + 
            m_size_of_data_write_reg;
        } else {
          IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(
            irsm(" запись не полного пакета hold regs"));
          for(irs_u16 send_pack_index = 0; send_pack_index < m_size_of_packet;
            send_pack_index++)
          {
            m_spacket[send_pack_index] = 0;
          }
          if (m_write_quantity == 1) {
            m_command = write_single_register;
            response_single_write_t &hr_packet =
              reinterpret_cast<response_single_write_t&>(*(m_spacket.data() +
              size_of_MBAP));
            irs_u8* hold_regs_data =
              reinterpret_cast<irs_u8*>(m_hold_regs_reg_write.data());
            irs_u8* hr_packet_data =
              reinterpret_cast<irs_u8*>(&hr_packet.value);
            memcpyex(hr_packet_data, hold_regs_data + hr_start*2,
              m_write_quantity*2);
          } else {
            request_multiple_write_regs_t &hr_packet =
              reinterpret_cast<request_multiple_write_regs_t&>(
              *(m_spacket.data() + size_of_MBAP));
            irs_u8* hold_regs_data =
              reinterpret_cast<irs_u8*>(m_hold_regs_reg_write.data());
            irs_u8* hr_packet_data =
              reinterpret_cast<irs_u8*>(hr_packet.value);
            memcpyex(hr_packet_data, hold_regs_data + hr_start*2,
              m_write_quantity*2);
          }
          make_packet(hr_start, static_cast<irs_u16>(m_write_quantity*2));
          m_search_index = m_coils_size_bit + hr_start + m_write_quantity;
          if (m_search_index == m_coils_size_bit + m_hold_registers_size_reg) {
            m_search_index = 0;
          }
        }
      }
      m_mode = convert_request_mode;
      view_mode();
    }
    break;
    case convert_request_mode:
    {
      MBAP_header_t& header =
        reinterpret_cast<MBAP_header_t&>(*m_spacket.data());
      m_transaction_id++;
      header.transaction_id = m_transaction_id;
      IRS_LIB_IRSMBUS_DBG_MONITOR(
        modbus_pack_request_monitor(m_spacket.data());
      );
      convert(m_spacket.data(), 0, size_of_MBAP - 1);
      request_exception_t &convert_pack_for_write =
        reinterpret_cast<request_exception_t&>(*(m_spacket.data() + 
        size_of_MBAP));
      switch(convert_pack_for_write.function_code)
      {
        case read_discrete_inputs:
        {
          convert(m_spacket.data(), size_of_MBAP + 1, size_of_read_header);
        } break;
        case read_coils:
        {
          convert(m_spacket.data(), size_of_MBAP + 1, size_of_read_header);
        } break;
        case read_hold_registers:
        {
          convert(m_spacket.data(), size_of_MBAP + 1, size_of_read_header);
        } break;
        case read_input_registers:
        {
          convert(m_spacket.data(), size_of_MBAP + 1, size_of_read_header);
        } break;
        case write_single_coil:
        {
          convert(m_spacket.data(), size_of_MBAP + 1, size_of_read_header);
        } break;
        case write_single_register:
        {
          convert(m_spacket.data(), size_of_MBAP + 1, size_of_read_header);
        } break;
        case read_exception_status:
        {
          //
        } break;
        case write_multiple_coils:
        {
          convert(m_spacket.data(), size_of_MBAP + 1, size_of_read_header);
        } break;
        case write_multiple_registers:
        {
          convert(m_spacket.data(), size_of_MBAP + 1, size_of_read_header);
          request_multiple_write_regs_t &reg_convert =
            reinterpret_cast<request_multiple_write_regs_t&>(
            *(m_spacket.data() + size_of_MBAP));
          convert(m_spacket.data(), size_of_MBAP + size_of_req_multi_write,
            reg_convert.byte_count);
        } break;
        default:
        {
          
        } break;
      }
      m_mode = send_request_mode;
      view_mode();
    }
    break;
    case send_request_mode:
    {
      m_channel = mp_hardflow_client->channel_next();
      if (m_send_request_timer.check()) {
        // > Убрать в дебаг
        IRS_LIB_ASSERT(!reinterpret_cast<MBAP_header_t&>(
          *m_spacket.data()).proto_id);        
        IRS_LIB_ASSERT(reinterpret_cast<request_t&>(*(m_spacket.data() + 
          size_of_MBAP)).function_code);
        // Убрать в дебаг <
        m_fixed_flow.write(m_channel, m_spacket.data(), size_of_MBAP +
          m_size_byte_end);
        m_mode = read_header_mode;
        view_mode();
        m_send_request_timer.start();
      }
    } break;
    case read_header_mode:
    {
      if (m_fixed_flow.write_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      {
        m_fixed_flow.read(m_channel, m_rpacket.data(), size_of_MBAP);
        m_mode = read_response_mode;
        view_mode();
      } else if (m_fixed_flow.write_status() == 
        irs::hardflow::fixed_flow_t::status_error)
      {
        m_transaction_id--;
        m_mode = wait_command_mode;
        m_request_type = request_start;
        m_start = true;
        m_fixed_flow.read_abort();
        m_fixed_flow.write_abort();
        IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("\n abort \n"));
        m_error_count++;
        if(m_error_count >= m_error_count_max) {
          IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" status error"));
          m_operation_status = status_error;
          reconnect();
        }
        view_mode();
      }
    }
    break;
    case read_response_mode: 
    { 
      if (m_fixed_flow.read_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      {
        MBAP_header_t &send_pack_header = 
          reinterpret_cast<MBAP_header_t&>(*m_spacket.data());
        MBAP_header_t &recv_pack_header = 
          reinterpret_cast<MBAP_header_t&>(*m_rpacket.data());
        if ((send_pack_header.transaction_id ==
          recv_pack_header.transaction_id))
        {
          convert(m_rpacket.data(), 0, size_of_MBAP - 1);
          m_fixed_flow.read(m_channel, m_rpacket.data() + size_of_MBAP,
            recv_pack_header.length - 1);
          m_mode = treatment_response_mode;
        } else {
          m_mode = read_header_mode;
        }
        view_mode();
      } else if (m_fixed_flow.read_status() == 
        irs::hardflow::fixed_flow_t::status_error)
      {  
        m_mode = wait_command_mode;
        m_request_type = request_start;
        m_start = true;
        m_fixed_flow.read_abort();
        m_fixed_flow.write_abort();
        IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("\n abort \n"));
        m_error_count++;
        if(m_error_count >= m_error_count_max) {
          IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" status error"));
          m_operation_status = status_error;
          reconnect();
        }
        view_mode();
      }
    }
    break;
    case treatment_response_mode:
    {
      if (m_fixed_flow.read_status() == 
        irs::hardflow::fixed_flow_t::status_success)
      {
        IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" treatment_response_mode"));
        request_t &convert_pack_for_read =
          reinterpret_cast<request_t&>(*(m_rpacket.data() + 
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
              reinterpret_cast<response_read_reg_t&>(*(m_rpacket.data() + 
              size_of_MBAP));
            convert(m_rpacket.data(), size_of_MBAP + size_of_resp_header,
              read_hold_reg.byte_count);
          } break;
          case read_input_registers:
          {
            response_read_reg_t &read_input_reg =
              reinterpret_cast<response_read_reg_t&>(*(m_rpacket.data() + 
              size_of_MBAP));
            convert(m_rpacket.data(), size_of_MBAP + size_of_resp_header,
              read_input_reg.byte_count);
          } break;
          case write_single_coil:
          {
            convert(m_rpacket.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case write_single_register:
          {
            convert(m_rpacket.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case read_exception_status:
          {
            //  
          } break;
          case write_multiple_coils:
          {
            convert(m_rpacket.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          case write_multiple_registers:
          {
            convert(m_rpacket.data(), size_of_MBAP + 1, size_of_read_header);
          } break;
          default:
          {
            
          } break;
        }
        request_t &req_header = 
          reinterpret_cast<request_t&>(*(m_spacket.data() + size_of_MBAP));
        //Recieve packet:
        IRS_LIB_IRSMBUS_DBG_MONITOR(
          modbus_pack_response_monitor(m_rpacket.data());
        );
        convert(m_spacket.data(), size_of_MBAP + 1, size_of_read_header);
        irs_u16 start_addr = req_header.starting_address;
        //switch(req_header.function_code)
        switch(convert_pack_for_read.function_code)
        {
          case read_discrete_inputs:
          { 
            response_read_byte_t &read_di = 
              reinterpret_cast<response_read_byte_t&>(*(m_rpacket.data() +
              size_of_MBAP));
            IRS_LIB_ASSERT(static_cast<size_t>(start_addr/8 +
              read_di.byte_count) <= m_discr_inputs_byte_read.size());
            bit_copy(read_di.value, m_discr_inputs_byte_read.data(), 0,
              start_addr, m_request_quantity_discr_inputs_bit);
            #ifdef NOP
            for(irs_u16 di_index = 0;
              di_index < m_request_quantity_discr_inputs_bit; di_index++)
            {
              m_need_read[start_addr + di_index] = 0;
            }
            #endif //NOP
            fill_n(m_need_read.begin() + start_addr,
              m_request_quantity_discr_inputs_bit, 0);
            IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
              for(int di_idx = start_addr; 
                di_idx < start_addr + read_di.byte_count; di_idx++)
              {
                mlog() << irsm(" m_discr_inputs_byte_read[") << di_idx << 
                  irsm("] = ") << int(m_discr_inputs_byte_read[di_idx]) << endl;
              }
            );
            if ((m_discret_inputs_end_byte == size()) &&
              ((start_addr + read_di.byte_count) ==
              static_cast<int>(m_discret_inputs_size_bit/8)))
            {
              m_first_read = true;
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
          case read_coils:
          {
            response_read_byte_t &read_coils = 
              reinterpret_cast<response_read_byte_t&>(*(m_rpacket.data() +
              size_of_MBAP));
            #ifdef IRS_LIB_DEBUG
            irs_u16 request_byte = 0;
            if (!(m_request_quantity_coils_bit%8)) {
              request_byte =
                static_cast<irs_u16>(m_request_quantity_coils_bit/8);
            } else {
              request_byte =
                static_cast<irs_u16>(m_request_quantity_coils_bit/8 + 1); 
            }
            //irs_u16 byte_count = read_coils.byte_count;
            IRS_LIB_ASSERT(request_byte == read_coils.byte_count);
            #endif // IRS_LIB_DEBUG
            IRS_LIB_ASSERT(static_cast<size_t>(start_addr/8 +
              read_coils.byte_count) <= m_coils_byte_read.size());
            bit_copy(read_coils.value, m_coils_byte_read.data(), 0, 
              start_addr, m_request_quantity_coils_bit);
            #ifdef NOP
            for(irs_u16 coils_index = 0;
              coils_index < m_request_quantity_coils_bit; coils_index++)
            {
              m_need_read[m_discret_inputs_size_bit + start_addr +
                coils_index] = 0;
            }
            #endif // NOP
            fill_n(m_need_read.begin() + m_discret_inputs_size_bit +
              start_addr, m_request_quantity_coils_bit, 0);
            IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(
              for(int coils_idx = start_addr; 
                coils_idx < start_addr + read_coils.byte_count; coils_idx++)
              {
                mlog() << irsm(" m_coils_byte_read[") << coils_idx << 
                  irsm("] = ") << int(m_coils_byte_read[coils_idx]) << endl;
              }
            );
            if ((!m_input_registers_size_reg) && (!m_hold_registers_size_reg) &&
              ((start_addr + read_coils.byte_count) ==
              static_cast<int>(m_coils_size_bit/8)))
            {
              m_first_read = true;
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
          case read_hold_registers:
          {
            response_read_reg_t &hr_packet = 
              reinterpret_cast<response_read_reg_t&>(*(m_rpacket.data() + 
              size_of_MBAP));
            IRS_LIB_ASSERT(static_cast<size_t>(start_addr +
              hr_packet.byte_count/2) <= m_hold_regs_reg_read.size());
            irs_u8* hold_regs_data =
              reinterpret_cast<irs_u8*>(m_hold_regs_reg_read.data());
            irs_u8* hr_packet_data =
              reinterpret_cast<irs_u8*>(hr_packet.value);
            memcpyex(hold_regs_data + start_addr*2, hr_packet_data,
              hr_packet.byte_count);
            #ifdef NOP
            for(irs_u16 hr_index = 0; hr_index < hr_packet.byte_count/2;
              hr_index++)
            {
              m_need_read[m_discret_inputs_size_bit + m_coils_size_bit + 
                start_addr + hr_index] = 0;
            }
            #endif // NOP
            fill_n(m_need_read.begin() + m_discret_inputs_size_bit +
              m_coils_size_bit + start_addr, hr_packet.byte_count/2, 0);
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
            if ((!m_input_registers_size_reg) &&
              static_cast<size_t>((start_addr + hr_packet.byte_count/2)) ==
              m_hold_registers_size_reg)
            {  
              m_first_read = true;
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
          case read_input_registers:
          {
            response_read_reg_t &ir_packet =
              reinterpret_cast<response_read_reg_t&>(*(m_rpacket.data() + 
              size_of_MBAP));
            IRS_LIB_ASSERT(static_cast<size_t>(start_addr +
              ir_packet.byte_count/2) <= m_input_regs_reg_read.size());
            irs_u8* input_regs_data =
              reinterpret_cast<irs_u8*>(m_input_regs_reg_read.data());
            irs_u8* ir_packet_data =
              reinterpret_cast<irs_u8*>(ir_packet.value);
            memcpyex(input_regs_data + start_addr*2, ir_packet_data,
              ir_packet.byte_count);
            #ifdef NOP
            for(irs_u16 ir_index = 0; ir_index < ir_packet.byte_count/2;
              ir_index++)
            {
              m_need_read[m_discret_inputs_size_bit + m_coils_size_bit + 
                m_hold_registers_size_reg + start_addr + ir_index] = 0;
            }
            #endif // NOP
            fill_n(m_need_read.begin() + m_discret_inputs_size_bit +
              m_coils_size_bit + m_hold_registers_size_reg + start_addr,
              ir_packet.byte_count/2, 0);
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
            if((start_addr + ir_packet.byte_count/2) == 
              static_cast<int>(m_input_registers_size_reg))
            {
              m_first_read = true;
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
              reinterpret_cast<response_single_write_t&>(*(m_spacket.data() + 
              size_of_MBAP));
            if (m_need_writes_reserve[sec_head.address]) {
              m_need_writes_reserve[sec_head.address] = 0;
            } else {
              m_need_writes[sec_head.address] = 0;
            }
          }
          break;
          case write_single_register:
          {
            if (m_need_writes_reserve[m_coils_size_bit + start_addr]) {
              m_need_writes_reserve[m_coils_size_bit + start_addr] = 0;
            } else {
              m_need_writes[m_coils_size_bit + start_addr] = 0;
              m_hold_regs_reg_write[0] = 0;
            }
          }
          break;
          case write_multiple_coils:
          {
            request_multiple_write_byte_t &sec_head =
              reinterpret_cast<request_multiple_write_byte_t&>(
              *(m_spacket.data() + size_of_MBAP));
            for(irs_u16 coils_index = 0; coils_index < sec_head.quantity;
              coils_index++)
            {
              //irs_u8 mask = mask_gen(8 -
                //((start_addr + coils_index)%8 + 1), 1);
              //m_coils_byte_write[coils_index/8] &=
                //static_cast<irs_u8>(~mask);
              if (m_need_writes_reserve[start_addr + coils_index]) {
                m_need_writes_reserve[start_addr + coils_index] = 0;
                //m_coils_byte_write[coils_index/8] &=
                  //static_cast<irs_u8>(~mask);
              } else {
                m_need_writes[start_addr + coils_index] = 0;
              }
            }
          }
          break;
          case write_multiple_registers:
          {
            request_multiple_write_regs_t &sec_head =
              reinterpret_cast<request_multiple_write_regs_t&>(
              *(m_spacket.data() + size_of_MBAP));
            for(irs_u16 reg_index = 0; reg_index < sec_head.quantity;
              reg_index++)
            {
              if (m_need_writes_reserve[m_coils_size_bit + start_addr +
                reg_index])
              {
                m_need_writes_reserve[m_coils_size_bit + start_addr +
                  reg_index] = 0;
              } else {
                m_need_writes[m_coils_size_bit + start_addr + reg_index] = 0;
                m_hold_regs_reg_write[reg_index] = 0;
              }
            }
          }
          break;
          case read_exception_status:
          {
          }
          break;
        }
        for(irs_u16 send_pack_index = 0; send_pack_index < m_size_of_packet;
          send_pack_index++)
        {
          m_spacket[send_pack_index] = 0;
        }
        if (!m_write_table) {
          m_mode = search_read_data_mode;
          IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("write complete "));
          if (!m_read_table) {
            m_mode = wait_command_mode;
            if (m_write_complete) {
              m_operation_status = status_completed;
            }
            IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm("read complete "));
          } else {
            m_mode = search_read_data_mode;
          }
        } else {
          m_mode = search_write_data_mode;
        }
        view_mode();
      } else if (m_fixed_flow.read_status() == 
        irs::hardflow::fixed_flow_t::status_error)
      {  
        m_mode = wait_command_mode;
        m_request_type = request_start;
        m_start = true;
        m_fixed_flow.read_abort();
        m_fixed_flow.write_abort();
        IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("\n abort \n"));
        m_error_count++;
        if(m_error_count >= m_error_count_max) {
          IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm(" status error"));
          m_operation_status = status_error;
          reconnect();
        }
        view_mode();
      }
    }
    break;
    case search_read_data_mode:
    {
      if (m_refresh_mode == mode_refresh_manual) {
        m_mode = request_read_data_mode;
        irs_bool catch_block = false;
        m_read_quantity = 0;
        for(; m_search_index < (m_discret_inputs_size_bit + m_coils_size_bit + 
          m_hold_registers_size_reg + m_input_registers_size_reg);)
        {
          if ((m_need_read[m_search_index] == true) && 
            (catch_block == false)) 
          {
            IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(irsm(" block catched"));
            m_start_block = m_search_index;
            catch_block = true;
          }
          if ((catch_block == true) && 
            (m_need_read[m_search_index] == false)) 
          {
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_data_mode"));
            m_read_quantity =
              static_cast<irs_u16>(m_search_index - m_start_block);
            if ((m_start_block > m_discret_inputs_size_bit) && 
              (m_start_block < (m_discret_inputs_size_bit + 
              m_coils_size_bit)) && 
              (m_search_index > (m_discret_inputs_size_bit + m_coils_size_bit)))
            {
              m_search_index = m_discret_inputs_size_bit + m_coils_size_bit;
              m_read_quantity =
                static_cast<irs_u16>(m_search_index - m_start_block);
            }
            else if ((m_start_block > (m_discret_inputs_size_bit + 
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
            else if ((m_start_block > (m_discret_inputs_size_bit + 
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
          if ((catch_block == true) &&
            (m_search_index == (m_discret_inputs_size_bit + m_coils_size_bit + 
            m_hold_registers_size_reg + m_input_registers_size_reg)))
          { 
            IRS_LIB_IRSMBUS_DBG_MSG_BASE(irsm("send_data_mode"));
            m_write_quantity =
              static_cast<irs_u16>(m_search_index - m_start_block);
            if (m_start_block < m_discret_inputs_size_bit)
            {
              m_search_index = m_discret_inputs_size_bit;
              m_read_quantity =
                static_cast<irs_u16>(m_search_index - m_start_block);
            }
            else if ((m_start_block > m_discret_inputs_size_bit) && 
              (m_start_block < (m_discret_inputs_size_bit + 
              m_coils_size_bit)))
            {
              m_search_index = m_discret_inputs_size_bit + m_coils_size_bit;
              m_read_quantity =
                static_cast<irs_u16>(m_search_index - m_start_block);
            }
            else if ((m_start_block > (m_discret_inputs_size_bit + 
              m_coils_size_bit)) && 
              (m_start_block < (m_discret_inputs_size_bit + m_coils_size_bit +
              m_hold_registers_size_reg)))
            {
              m_search_index = m_discret_inputs_size_bit + m_coils_size_bit +
                m_hold_registers_size_reg;
              m_read_quantity =
                static_cast<irs_u16>(m_search_index - m_start_block);
            }
            else if ((m_start_block > (m_discret_inputs_size_bit + 
              m_coils_size_bit + m_hold_registers_size_reg)) && 
              (m_start_block < (m_discret_inputs_size_bit + m_coils_size_bit +
              m_hold_registers_size_reg + m_input_registers_size_reg)))
            {
              m_search_index = 0;
            }
            break;
          }
          if ((catch_block == false) && 
            (m_search_index >= (m_discret_inputs_size_bit + m_coils_size_bit + 
            m_hold_registers_size_reg + m_input_registers_size_reg)))
          {
            m_search_index = 0;
            m_read_table = false;
            m_mode = treatment_response_mode;
            IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(
              irsm(" массив флагов на чтение пуст"));
            break;
          }
        }
      } else if (m_refresh_mode == mode_refresh_auto) {
        m_mode = make_request_mode;
      }
      view_mode();
    }
    break;
    case request_read_data_mode:
    {
      // Чтение discret inputs
      if (m_start_block < m_discret_inputs_size_bit) {
        m_command = read_discrete_inputs;
        if (m_read_quantity/8 > m_size_of_data_read_byte) {
          make_packet(m_start_block,
            static_cast<irs_u16>(m_size_of_data_read_byte*8));
          m_search_index = m_start_block + m_size_of_data_read_byte*8;
        } else {
          make_packet(m_start_block, m_read_quantity);
          m_search_index = m_start_block + m_read_quantity;
        }
      }
      // Чтение coils
      else if ((m_start_block >= m_discret_inputs_size_bit) &&
        (m_start_block < m_discret_inputs_size_bit + m_coils_size_bit))
      {
        m_command = read_coils;
        if (m_read_quantity/8 > m_size_of_data_read_byte) {
          make_packet(m_start_block - m_discret_inputs_size_bit,
            static_cast<irs_u16>(m_size_of_data_read_byte*8));
          m_search_index = m_start_block + m_size_of_data_read_byte*8;
        } else {
          make_packet(m_start_block - m_discret_inputs_size_bit, 
            m_read_quantity);
          m_search_index = m_start_block + m_read_quantity;
        }
      }
      // Чтение holding registers
      else if ((m_start_block >= (m_discret_inputs_size_bit + 
        m_coils_size_bit)) && (m_start_block < (m_discret_inputs_size_bit + 
        m_coils_size_bit + m_hold_registers_size_reg)))
      {
        m_command = read_hold_registers;
        if (m_read_quantity*2 > m_size_of_data_read_byte) {
          make_packet(m_start_block - m_discret_inputs_size_bit - 
            m_coils_size_bit, m_size_of_data_read_reg);
          m_search_index = m_start_block + m_size_of_data_read_reg;
        } else {
          make_packet(m_start_block - m_discret_inputs_size_bit - 
            m_coils_size_bit, m_read_quantity);
          m_search_index = m_start_block + m_read_quantity;
        }
      }
      // Чтение input registers
      else if ((m_start_block >= (m_discret_inputs_size_bit + 
        m_coils_size_bit + m_hold_registers_size_reg)) && 
        (m_start_block < (m_discret_inputs_size_bit + m_coils_size_bit + 
        m_hold_registers_size_reg + m_input_registers_size_reg))) 
      {
        m_command = read_input_registers;
        if (m_read_quantity*2 > m_size_of_data_read_byte) {
          make_packet(m_start_block - (m_discret_inputs_size_bit +
            m_coils_size_bit + m_hold_registers_size_reg),
            m_size_of_data_read_reg);
          m_search_index = m_start_block + m_size_of_data_read_reg;
        } else {
          make_packet(m_start_block - (m_discret_inputs_size_bit +
            m_coils_size_bit + m_hold_registers_size_reg), m_read_quantity);
          m_search_index = m_start_block + m_read_quantity;
          if (m_search_index == (m_input_registers_size_reg + 
            m_hold_registers_size_reg + m_discret_inputs_size_bit + 
            m_coils_size_bit))
          {
            m_search_index = 0;
          }
        }
      }
      m_mode = convert_request_mode;
      view_mode();
    }
    break;
    case make_request_mode: //чтение полного массива (режим mode_refresh_auto)
    {
      switch(m_request_type)
      {
        case request_start:
        {
          if ((m_loop_timer.check()) && (m_refresh_mode == mode_refresh_auto))
          {
            IRS_LIB_IRSMBUS_DBG_OPERATION_TIME(
              double time_delta_write = m_measure_time.get() - 
                m_measure_int_time;
              m_measure_int_time = m_measure_time.get();
              mlog() << stime << irsm(" ") <<
                time_delta_write << irsm(" write end") << endl;
            );
            m_mode = make_request_mode;
            m_request_type = read_discrete_inputs;
            m_global_read_index = 0;
            view_mode();
          }
        }
        break;
        case read_discrete_inputs:
        {
          if (m_discret_inputs_size_bit) {
            m_command = m_request_type;
            if ((m_global_read_index + m_size_of_data_read_byte) <
              m_discret_inputs_size_bit/8)
            {
              make_packet(m_global_read_index,
                static_cast<irs_u16>(m_size_of_data_read_byte*8));
              m_global_read_index += m_size_of_data_read_byte;
              m_mode = convert_request_mode;
            } else {
              make_packet(m_global_read_index,
                irs_u16(m_discret_inputs_size_bit - m_global_read_index*8));
              m_global_read_index = 0;
              m_request_type = read_coils;
              m_mode = convert_request_mode;
            }
            view_mode();
          } else {
            m_request_type = read_coils;
          }
        }
        break;
        case read_coils:
        {
          if (m_coils_size_bit) {
            m_command = m_request_type;
            if ((m_global_read_index + m_size_of_data_read_byte) <
              m_coils_size_bit/8)
            {
              make_packet(m_global_read_index,
                static_cast<irs_u16>(m_size_of_data_read_byte*8));
              m_global_read_index += m_size_of_data_read_byte;
              m_mode = convert_request_mode;
            } else {
              make_packet(m_global_read_index,
                irs_u16(m_coils_size_bit - m_global_read_index*8));
              m_global_read_index = 0;
              m_request_type = read_hold_registers;
              m_mode = convert_request_mode;
            }
            view_mode();
          } else {
            m_request_type = read_hold_registers;
          }
        }
        break;
        case read_hold_registers:
        {
          if (m_hold_registers_size_reg) {
            m_command = m_request_type;
            if (irs_u32(m_global_read_index + m_size_of_data_read_reg) < 
              irs_u32(m_hold_registers_size_reg))
            {
              make_packet(m_global_read_index, m_size_of_data_read_reg);
              m_global_read_index += m_size_of_data_read_reg;
              m_mode = convert_request_mode;
            } else {
              make_packet(m_global_read_index,
                irs_u16(m_hold_registers_size_reg - m_global_read_index));
              m_global_read_index = 0;
              m_request_type = read_input_registers;
              m_mode = convert_request_mode;
            }
            view_mode();
          } else {
            m_request_type = read_input_registers;
          }
        }
        break;
        case read_input_registers:
        {
          if (m_input_registers_size_reg) {
            m_command = m_request_type;
            if (irs_u32(m_global_read_index + m_size_of_data_read_reg) <
              irs_u32(m_input_registers_size_reg))
            {
              make_packet(m_global_read_index, m_size_of_data_read_reg);
              m_global_read_index += m_size_of_data_read_reg;
              m_mode = convert_request_mode;
            } else {
              make_packet(m_global_read_index,
                irs_u16(m_input_registers_size_reg - m_global_read_index));
              m_global_read_index = 0;
              m_request_type = request_start;
              m_mode = convert_request_mode;
            }
            view_mode();
          } else {
            m_read_table = false;
            m_request_type = request_start;
          }
          size_t read_flags_cnt = 0;
          for(size_t read_idx = 0; read_idx < (m_discret_inputs_size_bit +
            m_coils_size_bit + m_hold_registers_size_reg + 
            m_input_registers_size_reg); read_idx++)
          {
            if (m_need_read[read_idx] == 1) {
              read_flags_cnt++;
            }
          }
          if (read_flags_cnt == 0) {
            m_read_table = false;
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

