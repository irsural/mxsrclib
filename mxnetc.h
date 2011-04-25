//! \file
//! \ingroup network_in_out_group
//! \brief �������� MxNetC (Max Network Client)
//!
//! ����: 21.04.2011\n
//! ������ ����: 12.02.2009

#ifndef mxnetcH
#define mxnetcH

// ����� �����
#define MXNETCH_IDX 18

#include <irsdefs.h>

#include <mxnetd.h>
#include <mxnetda.h>
#include <timer.h>
#include <irsstd.h>
#include <mxnetr.h>
#include <mxifa.h>

#include <irsfinal.h>

//! \addtogroup network_in_out_group
//! @{

// ������ (<������� ����>.<������� ����>)
#define MXNC_VERSION 0x0001


typedef enum _mxnc_status_t {
  mxnc_status_success = 0,
  mxnc_status_busy = 1,
  mxnc_status_error = 2
} mxnc_status_t;
typedef enum _mxnc_mode_t {
  mxnc_mode_free,
  mxnc_mode_packet,
  mxnc_mode_write,
  mxnc_mode_write_wait,
  mxnc_mode_read,
  //mxnc_mode_read_wait,
  mxnc_mode_begin_packet,
  mxnc_mode_bpack_abort,
  mxnc_mode_bcast_in_com,
  mxnc_mode_bcast_in_com_abort,
  mxnc_mode_chunk_read_wait,
  mxnc_mode_checksum,
  mxnc_mode_proc,
  mxnc_mode_reset,

  mxnc_mode_broadcast,
  mxnc_mode_broadcast_wait,
  mxnc_mode_command_abort_wait,
  mxnc_mode_timeout_abort_wait,
  mxnc_mode_broadcast_proc_wait,
  mxnc_mode_bcast_abort_wait
} mxnc_mode_t;
typedef enum _mxnc_error_t {
  mxnc_err_none = 0,
  mxnc_err_invalid_param = 1,
  mxnc_err_nomem = 2,
  mxnc_err_create = 3,
  mxnc_err_abort = 4,
  mxnc_err_busy = 5,
  mxnc_err_bad_server = 6,
  mxnc_err_remote_invalid_param = 7,
  mxnc_err_timeout = 8
} mxnc_error_t;

class mx_broadcast_proc_t;

// ������ ��������� mxnet
class mxnetc
{
  // ����� ��� ������/�������� ������
  mxn_packet_t *f_packet;
  // ������ ������, ����
  mxn_sz_t f_packet_size;
  // ������ ������������ ������, ����
  mxn_sz_t f_receive_size;
  // ������ ������������� ������, ����
  mxn_sz_t f_send_size;
  // ������� �����
  mxnc_mode_t f_mode;
  // ����� ����� �������� �� ���������
  mxnc_mode_t f_mode_return;
  // ������� �������
  mxn_cnt_t f_command;
  // ������� ������ ��������
  mxnc_status_t f_status;
  // ������ �������� ������������ � mxnc_mode_reset
  mxnc_status_t f_local_status;
  // ��������� �� ���������� ��� ������
  irs_u16 *f_version;
  // ���������� ������
  void *f_handle_channel;
  // ������ �������� ������
  irs_bool f_create_error;
  // ������ ���������� ��������
  irs_bool f_abort_request;
  // ������� � ������ ��� ������ ������ ������
  //mxn_sz_t f_packet_pos;
  // �������� ������ ��� ������ ������ ������
  //irs_bool f_fbpc_wait_read;//!!!!!!!!
  // ������� ������ �� fbpc �� ��������
  //irs_bool f_fbpc_end_to;//!!!!!!!!
  // ��������� ������
  mxnc_error_t f_last_error;
  // ��������� �� �������������� ���������� ���������� ����������
  mxn_cnt_t *f_user_size;
  // ������ ���������� ��� ������/������
  mxn_cnt_t f_user_index;
  // ���������� ���������� ��� ������/������
  mxn_cnt_t f_user_count;
  // ����������������� ����� �������� �������������
  irs_bool f_user_broadcast;
  // ��������� �� ���������������� ����������
  irs_i32 *f_user_vars;
  // ���������� �������� ������� ��������
  counter_t f_net_to;
  // ���������� �������� ������������������ ������
  counter_t f_broadcast_to;
  // ����������������� ������� �������
  //irs_bool f_broadcast_to_en;
  // ����� ��� ������ ������������������ ������
  //mxn_packet_t *f_broadcast_packet;
  // ������ ������ ��� ������ ������������������ ������
  //mxn_sz_t f_broadcast_packet_size;
  // ����� ���������� ���������� ��� ����������������� ��������
  //mxn_cnt_t f_broadcast_count;
  // ���������� ��������� ��� ����������������� ��������
  //irs_i32 *f_broadcast_vars;
  // ����������������� ����� ������
  irs_bool f_broadcast_mode;
  // ���������� �������� ������� ��������
  counter_t f_oper_to;
  // ���������� ������� ������� ��������
  counter_t f_oper_t;
  // ����� ���������
  mxn_packet_t f_header;
  // ��������� ������ ������ ������
  irs::mx_beg_pack_proc_t *f_beg_pack_proc;
  // ��������� ������������������ ������
  mx_broadcast_proc_t *f_broadcast_proc;
  // ������� ��� ����������� �����
  irs::mxn_checksum_t f_checksum_type;

  // ��������� ����������� �� ���������
  mxnetc();
  // ���������� ���������
  irs_bool deny_proc();
  // ���������� ������
  irs_bool packet_fill(mxn_cnt_t code_comm, mxn_cnt_t packet_var_first,
    mxn_cnt_t packet_var_count, irs_i32 *vars, mxn_cnt_t var_count);
public:
  // �����������
  mxnetc(mxifa_ch_t channel);
  // ����������
  ~mxnetc();
  // ������ ������ ��������� mxnet
  void get_version(irs_u16 *version);
  // ������ ������� ������� (���������� ����������)
  void get_size(mxn_cnt_t *size);
  // ������ ����������
  void read(mxn_cnt_t index, mxn_cnt_t count, irs_i32 *vars);
  // ������ ����������
  void write(mxn_cnt_t index, mxn_cnt_t count, irs_i32 *vars);
  // ���������/���������� ������������������ ������
  void set_broadcast(irs_bool broadcast_mode);
  // ������ ������� ��������
  mxnc_status_t status();
  // ������������ ��������
  void tick();
  // ���������� ��������
  void abort();
  // ������ ��������� ������
  mxnc_error_t get_last_error();
  // ��������� �������� ��������, �
  void set_timeout(calccnt_t t_num, calccnt_t t_denom);
  // ��������� ip-������ ���������� ����������
  void set_dest_ip(mxip_t ip);
  // ��������� ����� ���������� ����������
  void set_dest_port(irs_u16 port);
  // ��������� ���������� �����
  void set_local_port(irs_u16 port);
  // ������� ���� ����������� �����
  void set_checksum_type(irs::mxn_checksum_t a_checksum_type);

  //MXNC_VERSION >= 0x0001
  // ������ �������� ������
  irs_bool get_broadcast_mode();
};

class mx_broadcast_proc_t: public mx_proc_t
{
  typedef enum _mode_t {
    mode_start,
    mode_vars_wait,
    mode_stop
  } mode_t;

  mode_t f_mode;
  void *f_handle_channel;
  irs_u8 *f_buf;
  irs_bool f_abort_request;
  irs_bool f_proc;
  // ����� ��� ������ ������������������ ������
  mxn_packet_t *f_broadcast_packet;
  // ������ ������ ��� ������ ������������������ ������
  mxn_sz_t f_broadcast_packet_size;
  // ���������� ��������� ��� ����������������� ��������
  irs_i32 *f_broadcast_vars;
  // ����� ���������� ���������� ��� ����������������� ��������
  mxn_cnt_t f_broadcast_count;
  irs_bool f_success;
  // ������� ��� ����������� �����
  irs::mxn_checksum_t f_checksum_type;
public:
  mx_broadcast_proc_t(void *a_handle_channel,
    irs::mxn_checksum_t a_checksum_type);
  void operator()(irs_u8 *a_buf, irs::mxn_checksum_t a_checksum_type);
  virtual ~mx_broadcast_proc_t();
  virtual irs_bool tick();
  virtual void abort();
  irs_bool success();
  void get_vars(irs_i32 *user_var, mxn_cnt_t index, mxn_cnt_t count);
  mxn_cnt_t get_count();
};

//! @}

namespace irs {

//! \addtogroup network_in_out_group
//! @{

#define irs_mxdata_to_mxnet_def_interval ((counter_t)-1)

//! \brief �������������� mxnetc � mxdata_t
class mxdata_to_mxnet_t : public mxdata_t
{
  static const mxn_sz_t m_size_var_byte = sizeof(irs_i32);
  static const irs_u8 m_high_bit = 7;

  enum m_status_t
  {
    GET_SIZE,
    WRITE,
    READ,
    FREE
  };
  m_status_t m_status;
  mxnetc *mp_mxnet;
  irs_bool m_is_connected;
  irs_u8 *mp_buf;
  irs_u8 *mp_read_buf;
  mxn_cnt_t m_mxnet_size;    //  � �������������� ����������
  mxn_sz_t m_mxnet_size_byte;
  vector<bool> m_write_vector;
  irs_uarc m_current_index;   //  � ������
  counter_t m_update_interval_to;
  counter_t m_update_interval_time;
  counter_t m_connect_counter;
  counter_t m_connect_interval;
  auto_ptr<mx_time_int_t> mp_measured_interval_alg;
  bool m_measured_interval_bad_alloc;
  bool m_first_connect;
protected:
  void connect(mxnetc *ap_mxnet);
public:
  mxdata_to_mxnet_t(mxnetc *ap_mxnet,
    const counter_t &a_connect_interval = irs_mxdata_to_mxnet_def_interval,
    const counter_t &a_update_interval = irs_mxdata_to_mxnet_def_interval);
  ~mxdata_to_mxnet_t();
  irs_uarc size();
  irs_bool connected();
  void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  double measured_interval();
  void tick();
};

//! \brief ��������������� mxnetc � mxdata_t �� ���������� mxnetc
class mxnetc_data_t: public mxdata_to_mxnet_t
{
public:
  mxnetc_data_t(
    mxifa_ch_t a_channel,
    const counter_t &a_connect_interval = irs_mxdata_to_mxnet_def_interval,
    const counter_t &a_update_interval = irs_mxdata_to_mxnet_def_interval
  );
  void ip(mxip_t a_ip);
  void port(irs_u16 a_port);
private:
  mxnetc m_mxnetc;
};

//! \brief ������ ��������� mxnet, ���������� ����� hardflow_t
class mxnet_client_command_t
{
public:
  enum status_t {
    mxnc_status_success = 0,
    mxnc_status_busy = 1,
    mxnc_status_error = 2
  };
  enum error_t {
    mxnc_err_none = 0,
    mxnc_err_invalid_param = 1,
    mxnc_err_nomem = 2,
    mxnc_err_create = 3,
    mxnc_err_abort = 4,
    mxnc_err_busy = 5,
    mxnc_err_bad_server = 6,
    mxnc_err_remote_invalid_param = 7,
    mxnc_err_timeout = 8
  };

  //! \brief �����������
  mxnet_client_command_t(hardflow_t& a_hardflow);
  //! \brief ����������
  ~mxnet_client_command_t();
  //! \brief ������ ������ ��������� mxnet
  void get_version(irs_u16 *version);
  //! \brief ������ ������� ������� (���������� ����������)
  void get_size(mxn_cnt_t *size);
  //! \brief ������ ����������
  void read(mxn_cnt_t index, mxn_cnt_t count, irs_i32 *vars);
  //! \brief ������ ����������
  void write(mxn_cnt_t index, mxn_cnt_t count, irs_i32 *vars);
  //! \brief ������ ������� ��������
  status_t status();
  //! \brief ������������ ��������
  void tick();
  //! \brief ���������� ��������
  void abort();
  //! \brief ������ ��������� ������
  error_t get_last_error();
  //! \brief ��������� �������� ��������, �
  void disconnect_time(counter_t a_time);
  //! \brief ��������� ip-������ ���������� ����������
  void ip(mxip_t a_ip);
  //! \brief ��������� ����� ���������� ����������
  void port(irs_u16 port);
  //! \brief ������� ���� ����������� �����
  void checksum_type(irs::mxn_checksum_t a_checksum_type);

private:
  typedef mx_beg_pack_proc_fix_flow_t beg_pack_type;
  typedef hardflow::fixed_flow_t::status_t ff_status_type;

  enum mode_t {
    mxnc_mode_free,
    mxnc_mode_packet,
    mxnc_mode_write,
    mxnc_mode_write_wait,
    mxnc_mode_read,
    mxnc_mode_begin_packet,
    mxnc_mode_chunk_read_wait,
    mxnc_mode_checksum,
    mxnc_mode_proc,
    mxnc_mode_reset,
  };

  //! \brief ���������������� �����
  hardflow_t& m_hardflow;
  //! \brief ������� ����������������� ������
  hardflow::fixed_flow_t m_fixed_flow;
  //! \brief ����� ������ ������
  beg_pack_type m_beg_pack_proc;
  //! \brief ������� ������������� ������
  hardflow_t::size_type m_channel_ident;
  //! \brief ����� ��� ������/�������� ������
  irs::raw_data_t<irs_u8> m_packet_data;
  //! \brief ��������� ������
  mxn_packet_t* mp_packet;
  //! \brief ������ ������������ ������, ����
  mxn_sz_t m_receive_size;
  //! \brief ������ ������������� ������, ����
  mxn_sz_t m_send_size;
  //! \brief ������� �����
  mode_t m_mode;
  //! \brief ����� ����� �������� �� ���������
  mode_t m_mode_return;
  //! \brief ������� �������
  mxn_cnt_t m_command;
  //! \brief ������� ������ ��������
  status_t m_status;
  //! \brief ��������� �� ���������� ��� ������
  irs_u16* mp_version;
  //! \brief ������ �������� ������
  bool m_create_error;
  //! \brief ��������� ������
  error_t m_last_error;
  //! \brief ��������� �� �������������� ���������� ���������� ����������
  mxn_cnt_t* mp_user_size;
  //! \brief ������ ���������� ��� ������/������
  mxn_cnt_t m_user_index;
  //! \brief ���������� ���������� ��� ������/������
  mxn_cnt_t m_user_count;
  //! \brief ��������� �� ���������������� ����������
  irs_i32* mp_user_vars;
  //! \brief ������� ��� ����������� �����
  irs::mxn_checksum_t m_checksum_type;

  //! \brief ��������� ����������� �� ���������
  mxnet_client_command_t();
  //! \brief ���������� ���������
  bool deny_proc();
  //! \brief ���������� ������
  void packet_fill(mxn_cnt_t a_code_comm, mxn_cnt_t a_packet_var_first,
    mxn_cnt_t a_packet_var_count, irs_i32 *ap_vars, mxn_cnt_t a_var_count);
  //! \brief ����� �������� ���������� ��������� � �������� ���������
  void reset_parametrs();
};


//! \brief ��������� ��� ������� �������� �������� mxnet_client_t
struct mxnet_client_queue_item_t
{
  irs_uarc index;
  raw_data_t<irs_u8> data;

  mxnet_client_queue_item_t():
    index(0),
    data()
  {
  }
};

//! \brief ������ ��������� mxnet, ����������� ��������� mxdata_t,
//! ���������� ����� hardflow_t
class mxnet_client_t: public mxdata_t
{
public:
  mxnet_client_t(hardflow_t& a_hardflow);
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index,
    irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
private:
  typedef mxnet_client_queue_item_t queue_item_type;

  enum mode_t {
    mode_free
  };

  enum {
    m_size_var_byte = sizeof(irs_i32),
  };

  mode_t m_mode;
  irs_uarc m_size;
  bool m_is_connected;
  irs::raw_data_t<irs_u32> m_data_vars;
  raw_data_view_t<irs_u8, irs_u32> m_data_bytes;
  deque<queue_item_type> m_write_queue;
  mxnet_client_command_t m_mxnet_client_command;

  void fill_write_flags(irs_uarc a_index, irs_uarc a_size, bool a_value);
};

//! @}

} //namespace irs

#endif //mxnetcH

