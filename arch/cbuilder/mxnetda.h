// �������� MxNet - ���������� ��������� �� ����������� 
// (MxNet defines architecture depended)
// ������ 0.1
#ifndef MXNETDAH
#define MXNETDAH

// ��� ��� �������� ���������� (������ ������ ���� unsigned)
typedef irs_u32 mxn_cnt_t;
// ��� ��� ���������� ������ � ������
typedef irs_u32 mxn_sz_t;

// ������ ������ ����� ���������� � �������
#define MXN_DUMMY_SIZE (sizeof(irs_i32) - sizeof(mxn_cnt_t))
// �������� ���������� ���� mxn_cnt_t
#define MXN_CNT_MAX IRS_U32_MAX

// ��������� ������ mxnet
#pragma pack ( push, 1 )
typedef struct _mxn_packet_t {
  irs_i32 ident_beg_pack_first;
  irs_i32 ident_beg_pack_second;
  mxn_cnt_t code_comm;
  mxn_cnt_t var_ind_first;
  mxn_cnt_t var_count;
  irs_i32 var[1];
} mxn_packet_t;
#pragma pack ( pop )

#endif //MXNETDAH
