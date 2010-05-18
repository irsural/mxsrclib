// Абстакция интерфейса для каналов обмена (интерфейсов)
// Max Interface Abstraction
// Дата: 31.01.2008

#ifndef MXIFALH
#define MXIFALH

// Версия (<старший байт>.<младший байт>)
//#define MXIFA_VERSION 0x0003
#define MXIFA_VERSION 0x0004

// Тип номера канала
typedef irs_u8 mxifa_ch_t;
// Максимальное значение для типа mxifa_sz_t
#define MXIFA_SZ_T_MAX IRS_U16_MAX
// Тип размера буфера
typedef irs_u16 mxifa_sz_t;

#endif //MXIFALH
