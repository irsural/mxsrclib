// ��������� ���������� �����
// ����: 23.04.2010
// ������ ����: 26.02.2009
// AVR

#ifndef LIRSDEFSH
#define LIRSDEFSH

#include <inavr.h>

// ��������� 64-������� ����
#define IRSDEFS_I64

// ����� ��� ����� irscpp.h
#define NAMESPACE_STD_NOT_SUPPORT

// ���������� ����������
#define irs_enable_interrupt() { __enable_interrupt(); }
// ���������� ����������
#define irs_disable_interrupt() { __disable_interrupt(); }

// ��������� throw
#undef IRS_THROW
#define IRS_THROW(_THROW_LIST_)

// 8-������� ����� �� ������
typedef signed char 		 		irs_i8;
// 8-������� ����� ��� �����
typedef unsigned char 	 		irs_u8;
// 16-������� ����� �� ������
typedef signed short 		 		irs_i16;
// 16-������� ����� ��� �����
typedef unsigned short 	 		irs_u16;
// 32-������� ����� �� ������
typedef signed long 		 		irs_i32;
// 32-������� ����� ��� �����
typedef unsigned long 	 		irs_u32;
// 64-������� ����� �� ������
typedef signed long long 		irs_i64;
// 64-������� ����� ��� �����
typedef unsigned long long 	irs_u64;

#endif //LIRSDEFSH
