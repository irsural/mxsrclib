//! \file
//! \ingroup configuration_group
//! \brief ��������� ������������ ��� ������� ������������
//!
//! ���� 25.11.2010\n
//! ���� �������� 25.09.2009\n
//!
//! ���������� ���� ���� "irsconfig0.h" � ���� � ������ "irsconfig.h".
//! �������� � "irsconfig.h" (�� �� "irsconfig0.h") �����, ���� ����������

#ifndef IRSCONFIGH 
#define IRSCONFIGH

//! \addtogroup configuration_group
//! @{

#define IRS_LIB_LAST_VERSION INT_MAX
#define IRS_LIB_VERSION_SUPPORT IRS_LIB_LAST_VERSION

#define IRS_LIB_DEBUG_GLOBAL
#define IRS_LIB_CHECK

#define IRS_LIB_DEBUG_NONE 0
#define IRS_LIB_DEBUG_BASE 1
#define IRS_LIB_DEBUG_DETAIL 2

//#define IRS_LIB_IRSTCPIP_DEBUG_TYPE IRS_LIB_DEBUG_BASE
//#define IRS_LIB_HARDFLOWG_DEBUG_TYPE IRS_LIB_DEBUG_BASE
//#define IRS_LIB_IRSMBUS_DEBUG_TYPE IRS_LIB_DEBUG_BASE

//! \brief ������� ���������� ���������� ��� �������
//!   irs::hardflow::ni_usb_gpib_flow_t � irs::hardflow::ni_usb_gpib_flow_syn_t
#define IRS_LIB_NIUSBGPIBHF_DEBUG_TYPE IRS_LIB_DEBUG_NONE

// �������������� ���������� define-�����������
// ������������� ������
//#define IRS_LIB_DBG_KRASHENINNIKOV_MAXIM

#ifdef __ICCAVR__
#define IRS_LIB_FLASH_ASSERT
#endif //__ICCAVR__

#ifdef IRS_LIB_DEBUG_GLOBAL 
#if defined(__ICCAVR__) 
//#define IRS_LIB_DEBUG
#else //compilers
#define IRS_LIB_DEBUG 
#endif //compilers 
#endif //IRS_LIB_DEBUG_GLOBAL 

//! @}

#endif //IRSCONFIGH  
