// Ћокальные объ€влени€ типов
// ƒата: 20.11.2007
// C++ Builder

#ifndef LIRSDEFSH
#define LIRSDEFSH

// ¬ключение 64-битного типа
#define IRSDEFS_I64

// ¬ключение большого целого типа
#define IRSDEFS_LONG_LONG

// 8-битовое целое со знаком
typedef signed char       irs_i8;
// 8-битовое целое без знака
typedef unsigned char     irs_u8;
// 16-битовое целое со знаком
typedef signed short      irs_i16;
// 16-битовое целое без знака
typedef unsigned short    irs_u16;
// 32-битовое целое со знаком
typedef signed long       irs_i32;
// 32-битовое целое без знака
typedef unsigned long     irs_u32;
// 64-битовое целое со знаком
typedef signed __int64    irs_i64;
// 64-битовое целое без знака
typedef unsigned __int64  irs_u64;   

#endif //LIRSDEFSH
