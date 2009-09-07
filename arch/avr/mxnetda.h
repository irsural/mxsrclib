// �������� MxNet - ���������� ��������� �� �����������
// (MxNet defines architecture depended)
// ������ 0.2
#ifndef MXNETDAH
#define MXNETDAH

// ��� ��� �������� ���������� (������ ������ ���� unsigned)
typedef irs_u8 mxn_cnt_t;
// ��� ��� ���������� ������ � ������
typedef irs_u8 mxn_sz_t;

// ������ ������ ����� ���������� � �������
#define MXN_DUMMY_SIZE (sizeof(irs_i32) - sizeof(mxn_cnt_t))
// �������� ���������� ���� mxn_cnt_t
#define MXN_CNT_MAX IRS_U8_MAX

// ��������� ������ mxnet
typedef struct _mxn_packet_t {
  irs_i32 ident_beg_pack_first;
  irs_i32 ident_beg_pack_second;
  mxn_cnt_t code_comm;
  irs_u8 code_comm_dummy[MXN_DUMMY_SIZE];
  mxn_cnt_t var_ind_first;
  irs_u8 var_ind_first_dummy[MXN_DUMMY_SIZE];
  mxn_cnt_t var_count;
  irs_u8 var_count_dummy[MXN_DUMMY_SIZE];
  irs_i32 var[1];
} mxn_packet_t;

#endif //MXNETDAH
