// Глобальные объявления типов
// Дата: 4.08.2009

#ifndef IRSDEFSH
#define IRSDEFSH

#include <limits.h>

// Деректива throw
#define IRS_THROW(_THROW_LIST_) throw _THROW_LIST_

#include <lirsdefs.h>

// Определение версии Builder
#define IRS_CPP_BUILDER4    0x0540
#define IRS_CPP_BUILDER6    0x0560
#define IRS_CPP_BUILDER2006 0x0580

// Определения платформы
#if (defined(__BCPLUSPLUS__) && defined(__WIN32__)) || defined(__MINGW32__)
#define IRS_WIN32 // Платформа Win32 API
#elif defined(__GNUC__)
#define IRS_LINUX // Платформа Linux
#endif // Определения платформы

// Вычисление размера статического массива
#define IRS_ARRAYOFSIZE(_ARRAY_) (sizeof(_ARRAY_)/sizeof(*(_ARRAY_)))
// Определение для нулевого указателя
#define IRS_NULL 0
// Макросы выделения из переменных их частей
// BYTE - байты, WORD - 2 байта, DWORD - 4 байта
#define IRS_HIDWORD(_NUM_) (*(((irs_u32 *)(&(_NUM_))) + 1))
#define IRS_LODWORD(_NUM_) (*(((irs_u32 *)(&(_NUM_)))))
#define IRS_HIWORD(_NUM_) (*(((irs_u16 *)(&(_NUM_))) + 1))
#define IRS_LOWORD(_NUM_) (*(((irs_u16 *)(&(_NUM_)))))
#define IRS_HIBYTE(_NUM_) (*(((irs_u8 *)(&(_NUM_))) + 1))
#define IRS_LOBYTE(_NUM_) (*(((irs_u8 *)(&(_NUM_)))))
// Макросы для нахождения максимального и минимального значения
#define irs_max(_A_, _B_) (((_A_) > (_B_))?(_A_):(_B_))
#define irs_min(_A_, _B_) (((_A_) < (_B_))?(_A_):(_B_))
// Макросы удаления
#define IRS_DELETE(_VAR_) { if (_VAR_) delete _VAR_; _VAR_ = IRS_NULL; }
#define IRS_ARDELETE(_VAR_) { if (_VAR_) delete []_VAR_; _VAR_ = IRS_NULL; }

// Макросы для работы c DLL
// Объявление внешней функции
#define IRS_DLL_FUNC_DECL(_RETURN_, _FUNC_NAME_, _ARGS_)\
  extern "C" __declspec(dllexport) _RETURN_ _FUNC_NAME_ _ARGS_;
// Определение типа для функции
#define IRS_DLL_FUNC_TYPE_DEF(_RETURN_, _FUNC_NAME_, _ARGS_)\
  typedef _RETURN_ (*_FUNC_NAME_##_t)_ARGS_;
// Объявление указателя на функции
#define IRS_DLL_FUNC_DECL_VAR(_FUNC_)\
  _FUNC_##_t _FUNC_ = IRS_NULL;
// Подключение укзателя на функцию к функции DLL
#define IRS_DLL_FUNC_ATTACH(_FUNC_)\
{\
  _FUNC_ = (_FUNC_##_t)GetProcAddress(LibHandle, "_" #_FUNC_);\
  if (!_FUNC_) { NumErrFunc++; goto err_func; }\
}

// Математические константы
#define IRS_E         2.71828182845904523536
#define IRS_LOG2E     1.44269504088896340736
#define IRS_LOG10E    0.434294481903251827651
#define IRS_LN2       0.693147180559945309417
#define IRS_LN10      2.30258509299404568402
#define IRS_PI        3.14159265358979323846
#define IRS_PI_2      1.57079632679489661923
#define IRS_PI_4      0.785398163397448309616
#define IRS_1_PI      0.318309886183790671538
#define IRS_2_PI      0.636619772367581343076
#define IRS_1_SQRTPI  0.564189583547756286948
#define IRS_2_SQRTPI  1.12837916709551257390
#define IRS_SQRT2     1.41421356237309504880
#define IRS_SQRT_2    0.707106781186547524401

// Максимальные и минимальные значения по типам
#define IRS_I8_MIN -128
#define IRS_I8_MAX 127
#define IRS_U8_MIN 0
#define IRS_U8_MAX 255
#define IRS_I16_MIN -32768
#define IRS_I16_MAX 32767
#define IRS_U16_MIN 0
#define IRS_U16_MAX 65535
#define IRS_I32_MIN -2147483648L
#define IRS_I32_MAX 2147483647L
#define IRS_U32_MIN 0UL
#define IRS_U32_MAX 4294967295UL
#define IRS_IARC_MIN INT_MIN
#define IRS_IARC_MAX INT_MAX
#define IRS_UARC_MIN 0U
#define IRS_UARC_MAX UINT_MAX

// Тип с разрядностью процессора (ОС) со знаком
typedef int           irs_iarc;
// Тип с разрядностью процессора (ОС) без знака
typedef unsigned int  irs_uarc;

#ifdef IRSDEFS_I64

#define IRS_I64_MIN -9223372036854775808
#define IRS_I64_MAX 9223372036854775807
#define IRS_U64_MIN 0
#define IRS_U64_MAX 18446744073709551615

#define IRS_IMAX_MIN IRS_I64_MIN
#define IRS_IMAX_MAX IRS_I64_MAX
#define IRS_UMAX_MIN IRS_U64_MIN
#define IRS_UMAX_MAX IRS_U64_MAX
// Максимальное целое со знаком
typedef irs_i64 irs_imax;
// Максимальное целое без знака
typedef irs_u64 irs_umax;

#else //IRSDEFS_I64

#define IRS_IMAX_MIN IRS_I32_MIN
#define IRS_IMAX_MAX IRS_I32_MAX
#define IRS_UMAX_MIN IRS_U32_MIN
#define IRS_UMAX_MAX IRS_U32_MAX
// Максимальное целое со знаком
typedef irs_i32 irs_imax;
// Максимальное целое без знака
typedef irs_u32 irs_umax;

#endif //IRSDEFS_I64

// Булевый тип
typedef irs_u8   irs_bool;

const irs_bool   irs_true = 1;
const irs_bool   irs_false = 0;

typedef void (*irs_vfunc)();

// Гасим ненужные предупреждения C++ Builder
#ifdef __BCPLUSPLUS__
// Condition is always true OR Condition is always false (Default ON)
#pragma warn -8008
#if __BCPLUSPLUS__ >= IRS_CPP_BUILDER6
// 'type' argument 'specifier' passed to 'function' is a 'iterator category'
// iterator: 'iterator category' iterator required (W8091)
#pragma warn -8091
#endif //__BCPLUSPLUS__ >= IRS_CPP_BUILDER6
#endif //__BCPLUSPLUS__

#endif //IRSDEFSH
