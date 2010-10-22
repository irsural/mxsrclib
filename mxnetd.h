//! \file
//! \ingroup network_in_out_group
//! \brief �������� MxNet defines
//!
//! ����: 05.04.2010\n
//! ������ ����: 18.06.2008

#ifndef MXNETDH
#define MXNETDH

//! \addtogroup network_in_out_group
//! @{

// ������ ���������, 4 ����� (32-������ �����)
#define MXN_SIZE_OF_HEADER 5
// ������ �������������� ������ ������, ����
#define MXN_SIZE_OF_BEG_PACK 8
// ������ ���������, ����
#define mxn_header_size (MXN_SIZE_OF_HEADER*sizeof(irs_i32))
// ������ ��������� �� ������� ��������������, ����
#define mxn_end_size (mxn_header_size - MXN_SIZE_OF_BEG_PACK)

// ���� ������� ������
#define MXN_READ_COUNT        0
#define MXN_READ              1
#define MXN_WRITE             2
//#define MXN_WRITE_ACK         3
#define MXN_GET_VERSION       4
#define MXN_SET_BROADCAST     5
#define MXN_WRITE_BROADCAST   6

// �������� �������� ����������������� �������
#define MXN_BROADCAST_INTERVAL (TIME_TO_CNT(1, 1))

// ��� ������ ������ - ������ �����
#define MXN_CONST_IDENT_BEG_PACK_FIRST  ((irs_i32)0xFF43439A)
// ��� ������ ������ - ����� �����
#define MXN_CONST_IDENT_BEG_PACK_SECOND ((irs_i32)0xE81F3AAD)

#define MXN_UDP_IP_ETHERNET_HEADER_SIZE 70

//! @}

#endif //MXNETDH
