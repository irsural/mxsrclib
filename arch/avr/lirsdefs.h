// Локальные объявления типов
// Дата: 23.04.2010
// Ранняя дата: 26.02.2009
// AVR

#ifndef LIRSDEFSH
#define LIRSDEFSH

#include <inavr.h>

// Включение 64-битного типа
#define IRSDEFS_I64

// Опции для файла irscpp.h
#define NAMESPACE_STD_NOT_SUPPORT

// Разрешение прерываний
#define irs_enable_interrupt() { __enable_interrupt(); }
// Запрещение прерываний
#define irs_disable_interrupt() { __disable_interrupt(); }

// Деректива throw
#undef IRS_THROW
#define IRS_THROW(_THROW_LIST_)

// 8-битовое целое со знаком
typedef signed char 		 		irs_i8;
// 8-битовое целое без знака
typedef unsigned char 	 		irs_u8;
// 16-битовое целое со знаком
typedef signed short 		 		irs_i16;
// 16-битовое целое без знака
typedef unsigned short 	 		irs_u16;
// 32-битовое целое со знаком
typedef signed long 		 		irs_i32;
// 32-битовое целое без знака
typedef unsigned long 	 		irs_u32;
// 64-битовое целое со знаком
typedef signed long long 		irs_i64;
// 64-битовое целое без знака
typedef unsigned long long 	irs_u64;

#endif //LIRSDEFSH
