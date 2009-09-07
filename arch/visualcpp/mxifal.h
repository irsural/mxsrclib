// Абстакция интерфейса для каналов обмена (интерфейсов)
// Max Interface Abstraction
// Версия 0.5

#ifndef MXIFALH
#define MXIFALH

// Версия (<старший байт>.<младший байт>)
#define MXIFA_VERSION 0x0004

// Тип номера канала
typedef irs_u32 mxifa_ch_t;
// Максимальное значение для типа mxifa_sz_t
#define MXIFA_SZ_T_MAX IRS_U32_MAX
// Тип размера буфера
typedef irs_u32 mxifa_sz_t;

#endif //MXIFALH

