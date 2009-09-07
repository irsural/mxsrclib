// �������� MxNet (Max Network)
// ���� 16.04.2008

#ifndef MXNETH
#define MXNETH

#include <irsdefs.h>
#include <mxifa.h>
#include <mxnetda.h>
#include <mxnetr.h>
#include <timer.h>

// ������ (<������� ����>.<������� ����>)
#define MXN_VERSION 0x0003

// ��������������� ���������� ������ ��� ��������� mxnet
struct _mxn_data_t;
typedef _mxn_data_t mxn_data_t;
// ��� ��� ������������ �������
typedef void (*mxn_tick_f)(mxn_data_t &data);
// ������ ��� ��������� mxnet
struct _mxn_data_t {
  // ���������� ����������
  mxn_cnt_t count;
  // ������� ������ ����������
  irs_i32 *vars_ext;
  // ���������� ������ ����������
  mxn_packet_t *packet;
  // ������ ���������� ���������� ������ ��� ������
  irs_bool *read_only;
  // ������ � ������ �������������
  irs_bool is_broadcast;
  // ���������� �������� ����������������� ��������
  counter_t to_broadcast;
  // ��������� ������ ������������� �� �����
  irs_bool is_broadcast_new;
  // ����� �� �������� �������� ��������
  mxifa_ch_t channel;
  // ������ ������
  void *data_ch;
  // ������ �� ��������� �������
  mxifa_dest_t dest_info;
  // ���������� ���������� � �������� ������
  mxn_cnt_t var_cnt_packet;
  // ��������� ������������ �������
  mxn_tick_f mxn_tick_next;
  // ������������ �������
  mxn_tick_f mxn_tick;
  // ���������� ���������� ����
  mxn_sz_t count_send;
  // ����������������� ��������
  irs_bool broadcast_send;
  // ������ � ���������� ������
  irs_bool write_error;
  // ����� ������ ������
  irs::mx_beg_pack_proc_t *beg_pack_proc;
  // ������� ��� ����������� �����
  irs::mxn_checksum_t checksum_type;
};
struct mxn_ext_param_t
{
  // ��������� ��� ��������� ������������ avr128_ether �
  // ������� mxifa_set_config
  mxifa_avr128_cfg *avr128_cfg;
};

// ������������� ���������
void mxn_init(mxn_data_t &data, mxifa_ch_t channel, irs_i32 *vars,
  mxn_cnt_t count, mxn_ext_param_t *ext_param = 0);
// ��������������� ���������
void mxn_deinit(mxn_data_t &data);
// ��������� ������ ��� ������ ���������� ��� ���
void mxn_set_read_only(mxn_data_t &data, mxn_cnt_t ind_var, irs_bool read_only);
// ��������� ������ ������ �������������
void mxn_set_broadcast(mxn_data_t &data, irs_bool is_broadcast);
// ������������ �������
void mxn_tick(mxn_data_t &data);
// ������ ��������� �� ���������� ������
//void *mxn_get_data_channel(mxn_data_t &data);
// ��������� ���������� IP
void mxn_set_ip(mxn_data_t &data, mxip_t ip);
// ������ ���������� IP
mxip_t mxn_get_ip(mxn_data_t &data);
// ��������� ��������� �����
void mxn_set_mask(mxn_data_t &data, mxip_t mask);
// ������ ��������� �����
mxip_t mxn_get_mask(mxn_data_t &data);
// ��������� MAC(Ethernet)-������
void mxn_set_mac(mxn_data_t &data, irs_u8 *mac);
// ������ MAC(Ethernet)-������
void mxn_get_mac(mxn_data_t &data, irs_u8 *mac);
// ��������� ������ AVR � ������� ���������� RTL
void mxn_set_avrport(mxn_data_t &data, irs_avr_port_t data_port, 
  irs_avr_port_t address_port);
// ������ ������ AVR � ������� ���������� RTL
void mxn_get_avrport(mxn_data_t &data, irs_avr_port_t &data_port, 
  irs_avr_port_t &address_port);
// ��������� ���� ����������� �����
void mxn_set_checksum_type(mxn_data_t &data, irs::mxn_checksum_t checksum_type); 
// ������ ���� ����������� �����
irs::mxn_checksum_t mxn_get_checksum_type(mxn_data_t &data); 

namespace irs {

// �������������� �� �������� ������� irs_u8 � mxdata_t
class mxdata_to_u8_t : public mxdata_t
{
  irs_bool m_is_connected;
  irs_u8 *mp_u8_data;
  irs_uarc m_u8_size;
public:
  mxdata_to_u8_t(irs_u8 *ap_u8_data, irs_uarc a_u8_size);
  ~mxdata_to_u8_t();
  irs_uarc size();
  irs_bool connected();
  void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  void tick();
};

} //namespace irs

#endif //MXNETH
