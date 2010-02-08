// Клиент и сервер modbus
// Дата создания: 7.02.2008
// Дата последнего изменения: 02.02.2010

#ifndef irsmbusH
#define irsmbusH

#include <mxifa.h>
#include <irsstd.h>
#include <irscpp.h>
#include <mxdata.h>
#include <irsdefs.h>

namespace irs {

#pragma pack(push, 1)

struct MBAP_header_t {
  irs_u16       transaction_id;
  irs_u16       proto_id;
  irs_u16       length;
  irs_u8        unit_identifier;
};
struct request_t {
  irs_u8        function_code;
  irs_u16       starting_address;
  irs_u16       quantity;
};
struct request_multiple_write_byte_t {
  irs_u8        function_code;
  irs_u16       starting_address;
  irs_u16       quantity;
  irs_u8        byte_count;
  irs_u8        value[1];
};
struct request_multiple_write_regs_t {
  irs_u8        function_code;
  irs_u16       starting_address;
  irs_u16       quantity;
  irs_u8        byte_count;
  irs_u16       value[1];
};
struct request_exception_t {
  irs_u8        function_code;
};
struct response_read_byte_t {
  irs_u8        function_code;
  irs_u8        byte_count;
  irs_u8        value[1];
};
struct response_read_reg_t {
  irs_u8        function_code;
  irs_u8        byte_count;
  irs_u16       value[1];
};
struct response_single_write_t {
  irs_u8        function_code;
  irs_u16       address;
  irs_u16       value;
};
struct response_exception_t {
  irs_u8        error_code;
  irs_u8        exeption_code;
};

#pragma pack(pop)

void test_bit_copy(ostream& strm, size_t size_data_in, size_t size_data_out, 
  size_t index_data_in, size_t index_data_out, size_t size_data);  
  
void range(size_t a_index, size_t a_size, size_t a_start_range, 
  size_t a_end_range, size_t *a_num, size_t *a_start);
  
class modbus_server_t : public mxdata_t
{
public:
  typedef hardflow_t::size_type channel_type;
  
  modbus_server_t(
    hardflow_t* ap_hardflow,
    size_t a_discr_inputs_size_byte = 8192,
    size_t a_coils_size_byte = 8192,
    size_t a_hold_regs_reg = 65535,//2500,
    size_t a_input_regs_reg = 65535//2500
  );
  virtual ~modbus_server_t() {}
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
    size_of_MBAP = 7,
    size_of_read_header = 4,
    size_of_resp_header = 2,
    size_of_packet = 260,
    size_of_req_excep = 1,
    size_of_req_multi_write = 6,
    size_of_resp_multi_write = 6,
    read_coils = 1,
    read_discrete_inputs = 2,
    read_hold_registers = 3,
    read_input_registers = 4,
    write_single_coil = 5,
    write_single_register = 6,
    read_exception_status = 7,
    write_multiple_coils = 15,
    write_multiple_registers = 16,
  };
  enum condition_t {
    read_header_mode,
    read_request_mode,
    send_response_mode,
    read_end_mode,
    write_end
  };
  
  raw_data_t<irs_u8>                    mp_buf;
  size_t                                m_size_byte_end;
  size_t                                m_coils_size_bit;
  size_t                                m_discret_inputs_size_bit;
  size_t                                m_hold_registers_size_reg;
  size_t                                m_input_registers_size_reg;
  size_t                                m_size_byte;
  size_t                                m_discret_inputs_end_byte;
  size_t                                m_coils_end_byte;
  size_t                                m_hold_registers_end_byte;
  size_t                                m_input_registers_end_byte;
  condition_t                           m_mode;
  raw_data_t<irs_u8>                    m_discr_inputs_byte;
  raw_data_t<irs_u8>                    m_coils_byte;
  raw_data_t<irs_u16>                   m_input_regs_reg;
  raw_data_t<irs_u16>                   m_hold_regs_reg;
  size_t                                m_discret_inputs_size_byte;
  size_t                                m_discret_inputs_start_byte;
  size_t                                m_coils_size_byte;
  size_t                                m_coils_start_byte;
  size_t                                m_hold_registers_size_byte;
  size_t                                m_hold_registers_start_byte;
  size_t                                m_input_registers_size_byte;
  size_t                                m_input_registers_start_byte;
  channel_type                          m_channel;
  hardflow_t*                           mp_tcp_server;
  hardflow::fixed_flow_t                m_fixed_flow;
  size_t                                m_num_of_elem;
  
  void error_response(irs_u8 error_code);
  void modbus_pack_request_monitor(irs_u8 *ap_buf);
  void modbus_pack_response_monitor(irs_u8 *ap_buf);
};

class modbus_client_t : public mxdata_t
{
public:
  typedef hardflow_t::size_type channel_type;
  
  modbus_client_t(
    hardflow_t* ap_hardflow,
    size_t a_discr_inputs_size_byte = 8192,
    size_t a_coils_size_byte = 8192,
    size_t a_hold_regs_reg = 65535,
    size_t a_input_regs_reg = 65535,
    loop_timer_t a_loop_timer = make_cnt_ms(200)
  );
  void set_delay_time(double time);
  virtual ~modbus_client_t();
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
    size_of_MBAP = 7,
    size_of_read_header = 4,
    size_of_resp_header = 2,
    size_of_packet = 260,
    size_of_req_excep = 1,
    size_of_req_multi_write = 6,
    size_of_resp_multi_write = 6,
    size_of_data_write_byte = 246,
    size_of_data_read_byte = 250,
    size_of_data_write_reg = size_of_data_write_byte/2,
    size_of_data_read_reg = size_of_data_read_byte/2
  };
  enum condition_t {
    request_start = 0,
    read_coils = 1,
    read_discrete_inputs = 2,
    read_hold_registers = 3,
    read_input_registers = 4,
    write_single_coil = 5,
    write_single_register = 6,
    read_exception_status = 7,
    write_multiple_coils = 15,
    write_multiple_registers = 16,
  };
  enum mode_t {
    send_request_mode,
    //write_end_mode,
    read_header_mode,
    read_response_mode,
    //read_end_mode,
    //wait_mode,
    treatment_response_mode,
    make_request_mode,
    send_data_mode,
    search_block_mode
  };
  
  counter_t                             m_del_time;
  irs_u8                                *mp_buf;
  size_t                                m_global_read_index;
  size_t                                m_global_write_index;
  size_t                                m_coils_size_bit;
  size_t                                m_discret_inputs_size_bit;
  size_t                                m_hold_registers_size_reg;
  size_t                                m_input_registers_size_reg;
  size_t                                m_discret_inputs_end_byte;
  size_t                                m_coils_end_byte;
  size_t                                m_hold_registers_end_byte;
  size_t                                m_input_registers_end_byte;
  irs_u8                                m_spacket[size_of_packet];
  irs_u8                                m_rpacket[size_of_packet];
  size_t                                m_size_byte_end;
  bool                                  m_read_table;
  bool                                  m_write_coils;
  bool                                  m_write_hold_registers;
  channel_type                          m_channel;
  size_t                                m_start_block;
  size_t                                m_search_index;
  irs_u16                               m_bytes;
  size_t                                m_discret_inputs_size_byte;
  size_t                                m_discret_inputs_start_byte;
  size_t                                m_coils_size_byte;
  size_t                                m_coils_start_byte;
  size_t                                m_hold_registers_size_byte;
  size_t                                m_hold_registers_start_byte;
  size_t                                m_input_registers_size_byte;
  size_t                                m_input_registers_start_byte;
  vector<bool>                          m_need_writes;
  raw_data_t<irs_u8>                    m_discr_inputs_byte_read;
  raw_data_t<irs_u8>                    m_coils_byte_read;
  raw_data_t<irs_u8>                    m_coils_byte_write;
  raw_data_t<irs_u16>                   m_input_regs_reg_read;
  raw_data_t<irs_u16>                   m_hold_regs_reg_read;
  raw_data_t<irs_u16>                   m_hold_regs_reg_write;
  condition_t                           m_command, m_request_type;
  mode_t                                m_mode;
  loop_timer_t                          m_loop_timer;
  hardflow_t*                           mp_tcp_client;
  hardflow::fixed_flow_t                m_fixed_flow;
  size_t                                m_num_of_elem;
  size_t                                m_start_addr;
  irs_u16                               m_coil_write_bit;
  size_t                                m_coil_bit_index;
  mx_time_int_t                         m_send_measure_time;
  irs_bool                              m_first_read;
  measure_time_t                        m_measure_time;
  double                                m_measure_int_time;

  void make_packet(size_t a_index, irs_u16 a_size);
  void modbus_pack_request_monitor(irs_u8 *ap_buf);
  void modbus_pack_response_monitor(irs_u8 *ap_buf);

};

} //namespace irs

#endif //irsmbusH
