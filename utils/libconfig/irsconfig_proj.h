//! \file
//! \ingroup configuration_group
//! \brief R����������� mxsrclib ��� ������� �������
//!
//! ���� �������� 19.04.2013\n
//!
//! ���� ��������� ������������ ������ ��������� mxsrclib ��� �������,
//!   ���������� ���� ���� "irsconfig_proj.h" � ������� �������. � ����������
//!   ������� ���������� define IRS_USE_MXSRCLIB_PROJECT_CONFIGURATION=1

#ifndef IRSCONFIG_PROJH
#define IRSCONFIG_PROJH

#if IRS_USE_MXSRCLIB_PROJECT_CONFIGURATION

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

#endif // IRS_USE_MXSRCLIB_PROJECT_CONFIGURATION

#endif //IRSCONFIG_PROJH
