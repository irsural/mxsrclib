// ��������� ������������ ��� ������� ������������
// ���� 25.09.2009
// ���� �������� 25.09.2009

// ���������� ���� ���� "irsconfig0.h" � ���� � ������ "irsconfig.h"
// �������� � "irsconfig.h" (�� �� "irsconfig0.h") �����, ���� ����������

#ifndef IRSCONFIGH 
#define IRSCONFIGH 
 
#define IRS_LIB_DEBUG_GLOBAL 
//#define IRS_LIB_CHECK 
 
#ifdef IRS_LIB_DEBUG_GLOBAL 
#if defined(__ICCAVR__) 
//#define IRS_LIB_DEBUG 
#else //compilers 
#define IRS_LIB_DEBUG 
#endif //compilers 
#endif //IRS_LIB_DEBUG_GLOBAL 
 
#endif //IRSCONFIGH  
