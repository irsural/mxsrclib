// Глобальные объявления типов
// Дата: 26.04.2010
// Ранняя дата: 16.09.2009

#ifndef IRSDEFSH
#define IRSDEFSH

// Определения платформы
#if (defined(__BCPLUSPLUS__) && defined(__WIN32__)) || defined(__MINGW32__) ||\
  (defined(_MSC_VER) && defined(_WIN32))
#define IRS_WIN32 // Платформа Win32 API
#elif defined(__GNUC__)
#define IRS_LINUX // Платформа Linux
#endif // Определения платформы


#ifdef IRS_WIN32
#include <winsock2.h>
#endif // __WIN32__

#include <limits.h>
#include <stddef.h>

//#include <lirsdefs.h>


// Деректива throw
#ifdef __ICCAVR__
#define IRS_THROW(_THROW_LIST_)
#else //__ICCAVR__
#define IRS_THROW(_THROW_LIST_) throw _THROW_LIST_
#endif //__ICCAVR__

// Определение версии Builder
#define IRS_CPP_BUILDER4    0x0540
#define IRS_CPP_BUILDER6    0x0560
#define IRS_CPP_BUILDER2006 0x0580
#define IRS_CPP_BUILDER2010 0x0620

// Включение Unicode
//#define IRS_UNICODE_GLOBAL
#ifdef IRS_UNICODE_GLOBAL
#if defined(__ICCAVR__)
//#define IRS_UNICODE
#else //compilers
#define IRS_UNICODE
#endif //compilers
#endif //IRS_UNICODE_GLOBAL

// Возможности компиляторов
// __ICCAVR__ - Компилятор IAR для AVR
// __WATCOMC__ - Компилятор Open Watcom
// __GNUC__ - Компилятор GNU
// __BORLANDC__ - Компилятор Borland
// __AVR32__ - Компилятор GCC для AVR32

// Поддержка частичной специализации для функций
#if !defined(__WATCOMC__)
#define IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED
#endif //IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED

// Полная поддержка библиотеки C++
#if (defined(__BORLANDC__) && (__BORLANDC__>= IRS_CPP_BUILDER4)) ||\
  (defined(__GNUC__) && (__GNUC__ >= 4) && !defined(__CYGWIN__)) ||\
  (defined(_MSC_VER) && (_MSC_VER >= 1400))
#define IRS_FULL_STDCPPLIB_SUPPORT
#endif //IRS_FULL_STDCPPLIB_SUPPORT

// Опции для файла irscpp.h
#ifdef __ICCAVR__
#define NAMESPACE_STD_NOT_SUPPORT
#endif //__ICCAVR__

// Включение 64-битного типа
#define IRSDEFS_I64

// Включение большого целого типа
#define IRSDEFS_LONG_LONG

// Специальные описатели для манипуляторов в различных компиляторах
#if defined(__WATCOMC__)
#define IRS_STREAMSPECDECL _WPRTLINK
#else //compilers
#define IRS_STREAMSPECDECL
#endif //compilers
// Определение модификатора flash для компилятора IAR AVR
#define IRS_ICCAVR_FLASH __flash

// GCC версии < 3.4
#if (defined(__GNUC__) && \
      (\
        ( (__GNUC__ < 3) || (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) )\
      )\
    )
#define IRS_GNUC_VERSION_LESS_3_4
#endif // GCC версии < 3.4

// Статические утверждения
// Взято из STLSoft
// Перенесено из irserror.h для того, чтобы была возможность применить
// в irsstrdefs.h
#ifdef IRS_GNUC_VERSION_LESS_3_4
    // Эта константа у меня пока отсутствует
    // || defined(STLSOFT_COMPILER_IS_INTEL)
  #define IRS_STATIC_ASSERT(ex)\
    do { typedef int ai[(ex) ? 1 : -1]; } while(0)
#else /* ? compiler */
  #define IRS_STATIC_ASSERT(ex)\
    do { typedef int ai[(ex) ? 1 : 0]; } while(0)
#endif /* compiler */

// Один из типов используется в функции detect_cpu_endian, поэтому стоит выше
// других объявлений типов
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
#ifdef __BORLANDC__
// 64-битовое целое со знаком
typedef signed __int64    irs_i64;
// 64-битовое целое без знака
typedef unsigned __int64  irs_u64;
#else //__BORLANDC__
// 64-битовое целое со знаком
typedef signed long long 		irs_i64;
// 64-битовое целое без знака
typedef unsigned long long 	irs_u64;
#endif //__BORLANDC__

#ifdef __BORLANDC__
// Большое целое со знаком
typedef signed __int64 irs_ilong_long;
// Большое целое без знака
typedef unsigned __int64 irs_ulong_long;
#else // !__BORLANDC__
// Большое целое со знаком
typedef signed long long irs_ilong_long;
// Большое целое без знака
typedef unsigned long long irs_ulong_long;
#endif // !__BORLANDC__
// 32-битовое плавающее
typedef float irs_float32;
// 64-битовое плавающее
typedef double irs_float64;

// Используется в функции detect_cpu_endian, поэтому стоит выше
// макросов IRS_HIBYTE, ...
#define IRS_FIRSTDWORD(_NUM_) (*(((irs_u32 *)(&(_NUM_)))))
#define IRS_SECONDDWORD(_NUM_) (*(((irs_u32 *)(&(_NUM_))) + 1))
#define IRS_FIRSTWORD(_NUM_) (*(((irs_u16 *)(&(_NUM_)))))
#define IRS_SECONDWORD(_NUM_) (*(((irs_u16 *)(&(_NUM_))) + 1))
#define IRS_FIRSTBYTE(_NUM_) (*(((irs_u8 *)(&(_NUM_)))))
#define IRS_SECONDBYTE(_NUM_) (*(((irs_u8 *)(&(_NUM_))) + 1))

namespace irs {

// Определение порядка байтов во время выполнения
enum endian_t {
  little_endian = 0,
  big_endian = 1
};

inline endian_t detect_cpu_endian()
{
  const irs_u16 endian_test_var = static_cast<irs_u16>(1); /* 0x0001 */
  endian_t cpu_endian = 
    static_cast<endian_t>(IRS_FIRSTBYTE(endian_test_var) == 0);
  return cpu_endian;
}

}//namespace irs

// Определение порядка байтов во время компиляции
#define IRS_LITTLE_ENDIAN 0
#define IRS_BIG_ENDIAN 1
#ifdef __AVR32__
#define IRS_CPU_ENDIAN IRS_BIG_ENDIAN
#else //__AVR32__
#define IRS_CPU_ENDIAN IRS_LITTLE_ENDIAN
#endif //__AVR32__

// Вычисление размера статического массива
#define IRS_ARRAYSIZE(_ARRAY_) (sizeof(_ARRAY_)/sizeof(*(_ARRAY_)))
#define IRS_ARRAYOFSIZE(_ARRAY_) IRS_ARRAYSIZE(_ARRAY_)
// Определение для нулевого указателя
#define IRS_NULL 0
// Макросы выделения из переменных их частей
// BYTE - байты, WORD - 2 байта, DWORD - 4 байта
#define IRS_HIDWORD(_NUM_) ((IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)?\
  *(((irs_u32 *)(&(_NUM_))) + 1):*(((irs_u32 *)(&(_NUM_)))))
#define IRS_LODWORD(_NUM_) ((IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)?\
  *(((irs_u32 *)(&(_NUM_)))):*(((irs_u32 *)(&(_NUM_))) + 1))
#define IRS_HIWORD(_NUM_) ((IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)?\
  *(((irs_u16 *)(&(_NUM_))) + 1):*(((irs_u16 *)(&(_NUM_)))))
#define IRS_LOWORD(_NUM_) ((IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)?\
  *(((irs_u16 *)(&(_NUM_)))):*(((irs_u16 *)(&(_NUM_))) + 1))
#define IRS_HIBYTE(_NUM_) ((IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)?\
  *(((irs_u8 *)(&(_NUM_))) + 1):*(((irs_u8 *)(&(_NUM_)))))
#define IRS_LOBYTE(_NUM_) ((IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)?\
  *(((irs_u8 *)(&(_NUM_)))):*(((irs_u8 *)(&(_NUM_))) + 1))

// Макросы выделения из переменных их частей для констант
#define IRS_CONST_HIDWORD(_NUM_)\
  (static_cast<irs_u32>((_NUM_ >> 32)&0xFFFFFFFF))
#define IRS_CONST_LODWORD(_NUM_)\
  (static_cast<irs_u32>(_NUM_&0xFFFFFFFF))
#define IRS_CONST_HIWORD(_NUM_)\
  (static_cast<irs_u16>((_NUM_ >> 16)&0xFFFF))
#define IRS_CONST_LOWORD(_NUM_)\
  (static_cast<irs_u16>(_NUM_&0xFFFF))
#define IRS_CONST_HIBYTE(_NUM_)\
  (static_cast<irs_u8>((_NUM_ >> 8)&0xFF))
#define IRS_CONST_LOBYTE(_NUM_)\
  (static_cast<irs_u8>(_NUM_&0xFF))

// Макросы для нахождения максимального и минимального значения
#define irs_max(_A_, _B_) (((_A_) > (_B_))?(_A_):(_B_))
#define irs_min(_A_, _B_) (((_A_) < (_B_))?(_A_):(_B_))
// Макросы удаления
#define IRS_DELETE(_VAR_) { if (_VAR_) delete _VAR_; _VAR_ = IRS_NULL; }
#define IRS_ARDELETE(_VAR_) { if (_VAR_) delete []_VAR_; _VAR_ = IRS_NULL; }
// Макрос __FUNC__
#if defined(__GNUC__) || defined(__WATCOMC__) || defined(__ICCAVR__) ||\
  defined(_MSC_VER)
#define __FUNC__ __FUNCTION__
#endif //defined(__GNUC__) || defined(__WATCOMC__) || defined(__ICCAVR__)

#ifdef __ICCAVR__
// Разрешение прерываний
#define irs_enable_interrupt() { __enable_interrupt(); }
// Запрещение прерываний
#define irs_disable_interrupt() { __disable_interrupt(); }
#endif //__ICCAVR__

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

// Тип для пина
enum irs_pin_t {
  irs_on = 0,
  irs_off = 1
};

// Абстрактный тип для портов AVR
enum irs_avr_port_t {
  irs_avr_porta = 0,
  irs_avr_portb = 1,
  irs_avr_portc = 2,
  irs_avr_portd = 3,
  irs_avr_porte = 4,
  irs_avr_portf = 5,
  irs_avr_portg = 6
};

// Класс для функций обработки
class mx_proc_t {
public:
  virtual ~mx_proc_t() {}
  virtual irs_bool tick() = 0;
  virtual void abort() = 0;
};

// Переопределение станартных типов
typedef size_t irs_size_t;
typedef ptrdiff_t irs_ptrdiff_t;

namespace irs {

const int npos = -1;

}

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

