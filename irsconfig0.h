// ��������� ������������ ��� ������� ������������
// ���� 24.03.2010
// ���� �������� 25.09.2009

// ���������� ���� ���� "irsconfig0.h" � ���� � ������ "irsconfig.h"
// �������� � "irsconfig.h" (�� �� "irsconfig0.h") �����, ���� ����������

#ifndef IRSCONFIGH 
#define IRSCONFIGH 

#define IRS_LIB_DEBUG_GLOBAL
//#define IRS_LIB_CHECK

#define IRS_LIB_DEBUG_BASE 1
#define IRS_LIB_DEBUG_DETAIL 2

#define IRS_LIB_IRSTCPIP_DEBUG_TYPE IRS_LIB_DEBUG_BASE
//#define IRS_LIB_HARDFLOWG_DEBUG_TYPE IRS_LIB_DEBUG_BASE
//#define IRS_LIB_IRSMBUS_DEBUG_TYPE IRS_LIB_DEBUG_BASE

//#define IRS_LIB_RTL_OLD_INTERRUPT
//#define IRS_LIB_UDP_RTL_STATIC_BUFS
 
#ifdef IRS_LIB_DEBUG_GLOBAL 
#if defined(__ICCAVR__) 
//#define IRS_LIB_DEBUG
#else //compilers
#define IRS_LIB_DEBUG 
#endif //compilers 
#endif //IRS_LIB_DEBUG_GLOBAL 
 
#endif //IRSCONFIGH  
