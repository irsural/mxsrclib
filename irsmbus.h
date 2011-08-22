//! \file
//! \ingroup network_in_out_group
//! \brief ������ � ������ modbus
//!
//! ����: 18.08.2010\n
//! ������ ����: 16.09.2008

#ifndef irsmbusH
#define irsmbusH

#include <irsdefs.h>

#include <mxifa.h>
#include <irsstd.h>
#include <irscpp.h>
#include <mxdata.h>

#include <irsfinal.h>

#ifdef IRS_LIB_IRSMBUS_DEBUG_TYPE
# if (IRS_LIB_IRSMBUS_DEBUG_TYPE == IRS_LIB_DEBUG_BASE)
#   define IRS_LIB_IRSMBUS_DBG_OPERATION_TIME(msg) msg
#   define IRS_LIB_IRSMBUS_DBG_MONITOR(msg)
#   define IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(msg)
#   define IRS_LIB_IRSMBUS_DBG_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg << endl)
#   define IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(msg)
# elif (IRS_LIB_IRSMBUS_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
#   define IRS_LIB_IRSMBUS_DBG_OPERATION_TIME(msg) msg
#   define IRS_LIB_IRSMBUS_DBG_MONITOR(msg) msg
#   define IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(msg)\
      IRS_LIB_DBG_RAW_MSG(msg << endl) 
#   define IRS_LIB_IRSMBUS_DBG_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg << endl) 
#   define IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(msg) msg
# endif
#else // IRS_LIB_IRSMBUS_DEBUG_TYPE
# define IRS_LIB_IRSMBUS_DEBUG_TYPE IRS_LIB_DEBUG_NONE
# define IRS_LIB_IRSMBUS_DBG_OPERATION_TIME(msg)
# define IRS_LIB_IRSMBUS_DBG_MONITOR(msg)
# define IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(msg) 
# define IRS_LIB_IRSMBUS_DBG_MSG_BASE(msg) 
# define IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(msg)
#endif // IRS_LIB_IRSMBUS_DEBUG_TYPE

namespace irs {

//! \addtogroup network_in_out_group
//! @{

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

//! \brief ����������� ������ ������� ����������� ���� �������� � �������
//!   ������ �������.
//!
//! \param[in] a_index � ������ ������ ����������� �������;
//! \param[in] a_size � ������ ����������� �������;
//! \param[in] a_start_range � ������ �� ������ ����������� �������;
//! \param[in] a_end_range � ������ �� ����� ����������� �������;
//! \param[out] a_num � ������ ����������� �������;
//! \param[out] a_start � ������ �� ������ ����������� �������.
void range(size_t a_index, size_t a_size, size_t a_start_range, 
  size_t a_end_range, size_t *a_num, size_t *a_start);

//! \brief ������ ��������� Modbus
//! \author Sergeev Sergey
class modbus_server_t : public mxdata_ext_t
{
public:
  typedef hardflow_t::size_type channel_type;
  //! \brief �����������
  //!
  //! \param[in] ap_hardflow � ��������� �� ������������ ����������������
  //!   ��������;
  //! \param[in] a_discr_inputs_size � ������ discret inputs � ������ 0 .. 8192;
  //! \param[in] a_coils_size � ������ coils � ������ 0 .. 8192;
  //! \param[in] a_hold_regs_reg � ������ holding registers � ���������
  //!   0 .. 65536;
  //! \param[in] a_input_regs_reg � ������ input registers � ���������
  //!   0 ..65536;
  //! \param[in] double a_disconnect_time_sec � �����, ����� ������� ������
  //!   ��������� �� ����� ����� � ������ ����� ��� �������� ��������� �����
  //!   �������.
  modbus_server_t(
    hardflow_t* ap_hardflow,
    size_t a_discr_inputs_size_byte = 8,
    size_t a_coils_size_byte = 8,
    size_t a_hold_regs_reg = 7,
    size_t a_input_regs_reg = 7,
    counter_t a_disconnect_time = make_cnt_s(2)
  );
  virtual ~modbus_server_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void mark_to_send(irs_uarc a_index, irs_uarc a_size);
  virtual void mark_to_recieve(irs_uarc a_index, irs_uarc a_size);
  virtual void mark_to_send_bit(irs_uarc a_byte_index, irs_uarc a_bit_index);
  virtual void mark_to_recieve_bit(irs_uarc a_byte_index, irs_uarc a_bit_index);
  virtual void update();
  virtual status_t status() const;
  virtual void set_refresh_mode(mode_refresh_t a_refresh_mode);
  virtual void tick();
  virtual void abort();
  
private:
  enum {
    size_of_MBAP = 7,
    size_of_read_header = 4,
    size_of_resp_header = 2,
    size_of_packet = 260,
    size_of_req_excep = 3,
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
    write_multiple_registers = 16
  };
  //! \brief ����� ������ �������.
  enum condition_t {
    read_header_mode,   //!< \brief ����� ������ ��������� ������.
    read_request_mode,  //!< \brief ����� ������ ������� �� �������.
    send_response_mode, //!< \brief ����� �������� ������ �������.
    read_end_mode,      //!< \brief ����� ���������� ������ ������� �� �������.
    write_end           //!< \brief ����� ���������� �������� ������ �������.
  };
  //! \brief ����� ��� ������ � �������� ������ � ������� �����������������
  //!   ������ fixed_flow.
  raw_data_t<irs_u8>                    mp_buf;
  //! \brief ����� ������ ��� ���������.
  size_t                                m_size_byte_end;
  //! \brief ������ discret_inputs � �����.
  size_t                                m_discret_inputs_size_bit;
  //! \brief ������ discret_inputs � �����.
  size_t                                m_coils_size_bit;
  //! \brief ������ holding_registers � ���������.
  size_t                                m_hold_registers_size_reg;
  //! \brief ������ input_registers � ���������.
  size_t                                m_input_registers_size_reg;
  //! \brief ��������� ������ 4 ��������: discret_inputs, coils,
  //!   holding_registers, input_registers.
  size_t                                m_size_byte;
  //! \brief ����� ������� discret_inputs, ��������� � ������������� ��������
  //!   ��� ���������� ��������.
  size_t                                m_discret_inputs_end_byte;
  //! \brief ����� ������� coils, ��������� � ������������� �������� ���
  //!   ���������� ��������.
  size_t                                m_coils_end_byte;
  //! \brief ����� ������� holding_registers, ��������� � �������������
  //!   �������� ��� ���������� ��������.
  size_t                                m_hold_registers_end_byte;
  //! \brief ����� ������� input_register, ��������� � ������������� ��������
  //!   ��� ���������� ��������.
  size_t                                m_input_registers_end_byte;
  //! \brief ����� ������ �������
  condition_t                           m_mode;
  //! \brief �������� ������ discret_inputs, ������������ � ��������� ������ �
  //!   ������.
  raw_data_t<irs_u8>                    m_discr_inputs_byte;
  //! \brief �������� ������ coils, ������������ � ��������� ������ � ������.
  raw_data_t<irs_u8>                    m_coils_byte;
  //! \brief ����������� ������ input_registers, ������������ �
  //!   ��������� ������ � ������.
  raw_data_t<irs_u16>                   m_input_regs_reg;
  //! \brief ����������� ������ holding_registers, ������������ � ���������
  //!   ������ � ������.
  raw_data_t<irs_u16>                   m_hold_regs_reg;
  //! \brief ������ � ������ discret_inputs, �������� � ������������� ��������
  //!   ��� ���������� ��������.
  size_t                                m_discret_inputs_size_byte;
  //! \brief ������ ������� ������� discret_inputs, �������� � �������������
  //!   ��������.
  size_t                                m_discret_inputs_start_byte;
  //! \brief ������ � ������ coils, �������� � ������������� �������� ���
  //!   ���������� ��������.
  size_t                                m_coils_size_byte;
  //! \brief ������ ������� ������� coils, �������� � ������������� ��������.
  size_t                                m_coils_start_byte;
  //! \brief ������ � ������ holding_registers, �������� � �������������
  //! �������� ��� ���������� ��������.
  size_t                                m_hold_registers_size_byte;
  //! \brief ������ ������� ������� holding_registers, �������� � �������������
  //!   ��������.
  size_t                                m_hold_registers_start_byte;
  //! \brief ������ � ������ input_registers, �������� � ������������� ��������
  //!   ��� ���������� ��������.
  size_t                                m_input_registers_size_byte;
  //! \brief ������ ������� ������� input_registers, �������� � �������������
  //!   ��������.
  size_t                                m_input_registers_start_byte;
  //! \brief ����� ������, ����������� ��� ������� � ��������.
  channel_type                          m_channel;
  //! \brief ���������������� ��������, �� ���� �������� �������� ������.
  hardflow_t*                           mp_hardflow_server;
  //! \brief ��������������� �����, ��������������� ��� ������/������
  //!   ����������(��������������) ������ ������.
  hardflow::fixed_flow_t                m_fixed_flow;
  //! \brief ������ �������� update().
  status_t                              m_operation_status;
  static char const IRS_CSTR_NONVOLATILE       m_read_header_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_read_request_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_send_response_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_read_end_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_write_end[];
  static IRS_CSTR_NONVOLATILE char const IRS_CSTR_NONVOLATILE* const
    m_ident_name_list[];
  
  void error_response(irs_u8 error_code);
  void modbus_pack_request_monitor(irs_u8 *ap_buf);
  void modbus_pack_response_monitor(irs_u8 *ap_buf);
  void view_mode();
};


//! \brief ������ ��������� Modbus
//! \author Sergeev Sergey
class modbus_client_t : public mxdata_ext_t
{
public:
  typedef hardflow_t::size_type channel_type;
  //! \brief �����������
  //!
  //! \param[in] ap_hardflow � ��������� �� ������������ ����������������
  //!   ��������
  //! \param[in] a_refresh_mode � ����� ������ �������: mode_refresh_auto ���
  //!   mode_refresh_manual
  //! \param[in] a_discr_inputs_size_byte � ���������� discret inputs �
  //!   ������ 0 .. 8192
  //! \param[in] a_coils_size_byte � ������ coils � ������ 0 .. 8192
  //! \param[in] a_hold_regs_reg � ������ holding registers �
  //!   ��������� 0 .. 65536
  //! \param[in] a_input_regs_reg � ������ input registers �
  //!   ��������� 0 ..65536
  //! \param[in] a_update_time � ������ ������ ������� ������� ������
  //!   (��� ��������������� ������)
  //! \param[in] a_error_count_max � ������������ ���������� ������� ��������
  //!   ������� �� ����������� status_error. ������� ��������� ���������,
  //!   ���� �������� ������ �� ������ ��� �� ������� ����� �� ������
  //! \param[in] a_disconnect_time � �����, ����� ������� ������
  //!   ��������� �� ����� ����� � ������ ����� ��� �������� ���������
  //!   ����� �������
  modbus_client_t(
    hardflow_t* ap_hardflow,
    mode_refresh_t a_refresh_mode = mode_refresh_auto,
    size_t a_discr_inputs_size_byte = 8,
    size_t a_coils_size_byte = 8,
    size_t a_hold_regs_reg = 7,
    size_t a_input_regs_reg = 7,
    counter_t a_update_time = make_cnt_ms(200),
    irs_u8 a_error_count_max = 3,
    counter_t a_disconnect_time = make_cnt_s(2),
    irs_u16 a_size_of_packet = 260,
    irs_u8 a_unit_id = 0
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
  virtual void mark_to_send(irs_uarc a_index, irs_uarc a_size);
  virtual void mark_to_recieve(irs_uarc a_index, irs_uarc a_size);
  virtual void mark_to_send_bit(irs_uarc a_byte_index, irs_uarc a_bit_index);
  virtual void mark_to_recieve_bit(irs_uarc a_byte_index, irs_uarc a_bit_index);
  virtual void update();
  virtual status_t status() const;
  virtual void set_refresh_mode(mode_refresh_t a_refresh_mode);
  virtual void abort();
  virtual void tick();
  
private:
  enum {
    size_of_MBAP = 7,
    size_of_read_header = 4,
    size_of_resp_header = 2,
    size_of_req_excep = 1,
    size_of_req_multi_write = 6,
    size_of_resp_multi_write = 6,
    packet_size_max = 260,
    packet_size_min = 16
  };
  enum condition_t {
    //! \brief ����� ������ ������� ������� ������ ��� ��������������� ������.
    request_start = 0,
    //! \brief ������ coils.
    read_coils = 1,
    //! \brief ������ discret_inputs.
    read_discrete_inputs = 2,
    //! \brief ������ holding_registers.
    read_hold_registers = 3,
    //! \brief ������ input_registers.
    read_input_registers = 4,
    //! \brief ������ ���������� coils.
    write_single_coil = 5,
    //! \brief ������ ���������� ��������.
    write_single_register = 6,
    //! \brief ������ ���������� �������.
    read_exception_status = 7,
    //! \brief ������ ������������� coils.
    write_multiple_coils = 15,
    //! \brief ������ ������������� ���������.
    write_multiple_registers = 16
  };
  enum mode_t {
    //! \brief �������� ������� update().
    wait_command_mode,
    //! \brief ����� ������ �� ������.
    search_write_data_mode,
    //! \brief ������������ �������� �� ������.
    request_write_data_mode,
    convert_request_mode,
    //! \brief �������� �������.
    send_request_mode,
    //! \brief ������ ��������� ������ �� �������.
    read_header_mode,
    //! \brief ������ ���� ������ �� �������.
    read_response_mode,
    //! \brief ��������� ������.
    treatment_response_mode,
    //! \brief ����� ������ ��� ������.
    search_read_data_mode,
    //! \brief ������������ �������� �� ������.
    request_read_data_mode,
    //! \brief ������������ �������� �� ������ ������� �������
    //!   (��� ��������������� ������).
    make_request_mode
  };
  
  irs_u16                               m_size_of_packet;
  irs_u16                               m_size_of_data_write_byte;
  irs_u16                               m_size_of_data_read_byte;
  irs_u16                               m_size_of_data_write_reg;
  irs_u16                               m_size_of_data_read_reg;
  //! \brief ������, �������������� ��� ������������ �������� �� ������.
  size_t                                m_global_read_index;
  //! \brief ������ discret_inputs � �����.
  size_t                                m_discret_inputs_size_bit;
  //! \brief ������ coils � �����.
  size_t                                m_coils_size_bit;
  //! \brief ������ holding_registers � ���������.
  size_t                                m_hold_registers_size_reg;
  //! \brief ������ input_registers � ���������.
  size_t                                m_input_registers_size_reg;
  //! \brief ����� ������� discret_inputs, ��������� � ������������� ��������
  //!   ��� ���������� ��������.
  size_t                                m_discret_inputs_end_byte;
  //! \brief ����� ������� coils, ��������� � ������������� �������� ���
  //!   ���������� ��������.
  size_t                                m_coils_end_byte;
  //! \brief � ����� ������� holding_registers, ��������� � �������������
  //!   �������� ��� ���������� ��������.
  size_t                                m_hold_registers_end_byte;
  //! \brief ����� ������ input_registers, ��������� � ������������� ��������
  //!   ��� ������� ��������.
  size_t                                m_input_registers_end_byte;
  //! \brief ������ ������, ������������ ��� �������� ������� �������.
  raw_data_t<irs_u8>                    m_spacket;
  //! \brief ������ ������, ������������ ��� ������ ������ �� �������.
  raw_data_t<irs_u8>                    m_rpacket;
  //! \brief ����� ������ ��� ���������.
  size_t                                m_size_byte_end;
  //! \brief ����, ������������ ��������� �������� ������.
  bool                                  m_read_table;
  //! \brief ����, ������������ ��������� �������� ������.
  bool                                  m_write_table;
  bool                                  m_write_complete;
  //! \brief ����� ������ �������.
  channel_type                          m_channel;
  //! \brief ��������� ������ ��������� � ������� ������ ��
  //!   ������/������ ������.
  size_t                                m_start_block;
  //! \brief ������ ����� � ������� ������ �� ������/������.
  size_t                                m_search_index;
  //! \brief ������ � ������ discret_inputs, �������� � ������������� ��������
  //!   ��� ���������� ��������.
  size_t                                m_discret_inputs_size_byte;
  //! \brief ������ ������� ������� discret_inputs, �������� � �������������
  //!   ��������.
  size_t                                m_discret_inputs_start_byte;
  //! \brief ������ � ������ coils, �������� � ������������� �������� ���
  //!   ���������� ��������.
  size_t                                m_coils_size_byte;
  //! \brief ������ ������� ������� coils, �������� � ������������� ��������.
  size_t                                m_coils_start_byte;
  //! \brief ������ � ������ holding_registers, �������� � �������������
  //!   �������� ��� ���������� ��������.
  size_t                                m_hold_registers_size_byte;
  //! \brief ������ ������� ������� holding_registers, �������� �
  //!   ������������� ��������.
  size_t                                m_hold_registers_start_byte;
  //! \brief ������ � ������ input_register, �������� � ������������� ��������
  //!   ��� ���������� ��������.
  size_t                                m_input_registers_size_byte;
  //! \brief ������ ������� ������� inputs_registers, �������� �
  //!   ������������� ��������.
  size_t                                m_input_registers_start_byte;
  //! \brief ������ ��� �������� ������, ���������� ������ ��� ������.
  vector<bool>                          m_need_read;
  //! \brief ������ ��� �������� ������, ���������� ������ ��� ������.
  vector<bool>                          m_need_writes;
  vector<bool>                          m_need_writes_reserve;
  //! \brief �������� ������ discret_inputs, ������������ � ��������� ������.
  raw_data_t<irs_u8>                    m_discr_inputs_byte_read;
  //! \brief �������� ������ coils, ������������ � ��������� ������.
  raw_data_t<irs_u8>                    m_coils_byte_read;
  //! \brief �������� ������ coils, ������������ � ��������� ������.
  raw_data_t<irs_u8>                    m_coils_byte_write;
  //! \brief ����������� ������ input_registers, ������������ �
  //!   ��������� ������.
  raw_data_t<irs_u16>                   m_input_regs_reg_read;
  //! \brief ����������� ������ holding_registers, ������������ �
  //!   ��������� ������.
  raw_data_t<irs_u16>                   m_hold_regs_reg_read;
  //! \brief ����������� ������ holding_registers, ������������ � ���������
  //!   ������.
  raw_data_t<irs_u16>                   m_hold_regs_reg_write;
  //! \brief ��� �������.
  condition_t                           m_command;
  //! \brief ����� ������ ������� ��� ������������ �������� �� ������.
  condition_t                           m_request_type;
  //! \brief ����� ������ ������� �� �������� �����.
  mode_t                                m_mode;
  //! \brief ����������� ������, ������������ �������� ���������� ���������
  //!   ������� ������� ������.
  loop_timer_t                          m_loop_timer;
  //! \brief ���������������� ��������, �� ���� �������� �������� ������.
  hardflow_t*                           mp_hardflow_client;
  //! \brief ��������������� �����, ��������������� ��� ������/������
  //!   ����������(��������������) ������ ������.
  hardflow::fixed_flow_t                m_fixed_flow;
  //! \brief ��������� ����� ��� ��������/������������ ������� ������.
  size_t                                m_start_addr;
  //! \brief �������� ������������� ���� ��� coils.
  irs_u16                               m_coil_write_bit;
  //! \brief ������ ������������� ���� ��� coils.
  size_t                                m_coil_bit_index;
  //! \brief ������������� ������.
  mx_time_int_t                         m_send_measure_time;
  //! \brief ����, ������������ ��� ������ ������ ��������� ������� �����������,
  //!   ������������ ��� ������ ������ ��������� ������� �������.
  bool                                  m_first_read;
  //! \brief ������������� ������.
  measure_time_t                        m_measure_time;
  //! \brief �����, ������������� �� �������� ������/������.
  double                                m_measure_int_time;
  //! \brief ��������� �������� update().
  status_t                              m_operation_status;
  //! \brief ����� ������ ������� (������ ��� ��������������).
  mode_refresh_t                        m_refresh_mode;
  //! \brief ����, ����������� ����� ������ �������  � ������ ������.
  bool                              m_start;
  //! \brief ���������� ������ ��� ������, �����������  � �����
  //!   ����������� �����.
  irs_u16                               m_write_quantity;
  //! \brief ���������� ������ ��� ������, ����������� � �����
  //!   ����������� �����.
  irs_u16                               m_read_quantity;
  //! \brief ������������ ���������� ������� �������� ������� �� �����������
  //!   status_error.
  //!
  //! ������� ��������� ���������, ���� �������� ������ �� ������ ��� ��
  //!   ������� ����� �� ������.
  irs_u8                                m_error_count_max;
  irs_u16                               m_transaction_id;
  irs_u16                               m_request_quantity_discr_inputs_bit;
  irs_u16                               m_request_quantity_coils_bit;
  //! \brief ������� ���������� ��������� ������� �������� �������
  irs_u8                                m_error_count;
  irs_u8                                m_unit_id;
  timer_t                               m_send_request_timer;
  static char const IRS_CSTR_NONVOLATILE       m_wait_command_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_search_write_data_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_request_write_data_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_convert_request_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_send_request_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_read_header_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_read_response_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_treatment_response_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_search_read_data_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_request_read_data_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_make_request_mode[];
  static IRS_CSTR_NONVOLATILE char const IRS_CSTR_NONVOLATILE* const
    m_ident_name_list[];
  //! \brief ������������ ������� �� ������� �������.
  //!
  //! \param[in] a_index � ����� ������ ������������� �������;
  //! \param[in] a_size � ������ ������ ��� ������ ��� ������.
  void make_packet(size_t a_index, irs_u16 a_size);
  void modbus_pack_request_monitor(irs_u8 *ap_buf);
  void modbus_pack_response_monitor(irs_u8 *ap_buf);
  void view_mode();
  size_t get_packet_number();
  void reconnect();
};

//! @}

} //namespace irs

#endif //irsmbusH
