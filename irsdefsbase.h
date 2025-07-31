//! \file
//! \ingroup configuration_group
//! \brief Ãëîáàëüíûå îáúÿâëåíèÿ òèïîâ
//!
//! Äàòà ñîçäàíèÿ: 17.10.2012

#ifndef IRSDEFSBASEH
#define IRSDEFSBASEH

//! \addtogroup configuration_group
//! @{

#define IRS_LIB_VERSION_SUPPORT_LESS(ver) (IRS_LIB_VERSION_SUPPORT < ver)

#define IRS_LIB_VERSION_SUPPORT_GREATER(ver) (IRS_LIB_VERSION_SUPPORT > ver)

#ifdef __ARM_EABI__
#if __ARM_ARCH_PROFILE == 'M'
#define IRS_CORTEX_M
#endif //__ARM_ARCH_PROFILE
#endif //__ARM_EABI__

// Îïðåäåëåíèÿ ïëàòôîðìû
#if (defined(__BCPLUSPLUS__) && defined(__WIN32__)) || defined(__MINGW32__) ||\
  (defined(_MSC_VER) && defined(_WIN32))
#define IRS_WIN32 // Ïëàòôîðìà Win32 API
#elif defined(__GNUC__)
#ifdef IRS_CORTEX_M
#define IRS_GCC_CORTEX_M
#elif defined(__x86_64__)
#define IRS_LINUX // Ïëàòôîðìà Linux
#else
#error "Ïëàòôîðìà íå îïðåäåëåíà"
#endif
#endif // Îïðåäåëåíèÿ ïëàòôîðìû

#if defined(__ICCAVR__) || defined(__ICCARM__) || defined(IRS_GCC_CORTEX_M)
#define IRS_MICROCONTROLLER
#endif //defined(__ICCAVR__) || defined(__ICCARM__)

// Îïðåäåëåíèå ïîääåðæêè C++11
#ifdef __cplusplus
# if (__cplusplus >= 201103L)
#   define IRS_CPP11_SUPPORT 1
# endif
#endif

#ifndef IRS_CPP11_SUPPORT
# define IRS_CPP11_SUPPORT 0
#endif // IRS_CPP11_SUPPORT

// Îïðåäåëåíèå âåðñèè Builder
#define IRS_CPP_BUILDER4            0x0540
#define IRS_CPP_BUILDER6            0x0560
#define IRS_CPP_BUILDER2006         0x0580
#define IRS_CPP_BUILDER2007         0x0590
#define IRS_CPP_BUILDER2010         0x0620
#define IRS_CPP_BUILDERXE           0x0630
#define IRS_CPP_BUILDERXE_UPDATE1   0x0631
#define IRS_CPP_BUILDERXE3          0x0650

// Âêëþ÷åíèå Unicode
#ifdef __BORLANDC__
// _DELPHI_STRING_UNICODE
# if __BORLANDC__ >= IRS_CPP_BUILDER2010
#   define IRS_UNICODE_GLOBAL
    // Äëÿ çàãîëîâî÷íûõ ôàéëîâ Windows
#   define UNICODE
# endif
#elif defined(__MINGW32__) && defined(UNICODE)
// Äëÿ Qt ñ MinGW
#   define IRS_UNICODE_GLOBAL
#elif (defined(__GNUC__) && (__GNUC__ >= 4) && __LINUX__ && defined(UNICODE))
// Äëÿ Qt ñ linux-g++
#   define IRS_UNICODE_GLOBAL
#elif (defined(_MSC_VER) && (_MSC_VER >= 1400))
# define IRS_UNICODE_GLOBAL
#else
  //#define IRS_UNICODE_GLOBAL
#endif //__BORLANDC__

#ifdef IRS_UNICODE_GLOBAL
# if defined(__ICCAVR__)
  //#define IRS_UNICODE
# else //compilers
#   define IRS_UNICODE
# endif //compilers
#endif //IRS_UNICODE_GLOBAL

// Äëÿ èñïðàâëåíèÿ îøèáêè â C++Builder XE Update 1
# if (defined(__BORLANDC__) && (__BORLANDC__ <= IRS_CPP_BUILDERXE_UPDATE1))
#define BOOST_TR1_USE_OLD_TUPLE
#endif // (defined(__BORLANDC__) && (__BORLANDC__ <= IRS_CPP_BUILDERXE_UPDATE1))

#ifdef IRS_CPP11_SUPPORT
// Äëÿ òèïîâ int8_t, int16_t è ò.ä.
# include <stdint.h>
#endif // IRS_CPP11_SUPPORT

#ifdef IRS_WIN32
# include <winsock2.h>
#endif // __WIN32__

#ifdef __BORLANDC__
# include <System.hpp>
#endif //__BORLANDC__

#ifdef __ICCAVR__
# include <inavr.h>
#endif // __ICCAVR__

#include <limits.h>
#include <stddef.h>

#include <irsconfig.h>

#ifndef IRS_STM32H7xx
#include <irsdefsarch.h>
#endif // IRS_STM32H7xx


// Äåðåêòèâà throw
#ifdef __ICCAVR__
#define IRS_THROW(_THROW_LIST_)
#else //__ICCAVR__
#define IRS_THROW(_THROW_LIST_) throw _THROW_LIST_
#endif //__ICCAVR__

// Âîçìîæíîñòè êîìïèëÿòîðîâ
// __ICCAVR__ - Êîìïèëÿòîð IAR äëÿ AVR
// __WATCOMC__ - Êîìïèëÿòîð Open Watcom
// __GNUC__ - Êîìïèëÿòîð GNU
// __BORLANDC__ - Êîìïèëÿòîð Borland
// _MSC_VER - Êîìïèëÿòîð Microsoft Visual C++
// __AVR32__ - Êîìïèëÿòîð GCC äëÿ AVR32

// Ïîääåðæêà ÷àñòè÷íîé ñïåöèàëèçàöèè äëÿ ôóíêöèé
#if !defined(__WATCOMC__)
#define IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED
#endif //IRS_COMPILERS_PARTIAL_SPECIALIZATION_SUPPORTED

// Ïîëíàÿ ïîääåðæêà áèáëèîòåêè C++
#if (defined(__GNUC__) && (__GNUC__ >= 4) && !defined(__CYGWIN__)) || \
  (defined(__BORLANDC__) && (__BORLANDC__>= IRS_CPP_BUILDER4)) || \
  (defined(_MSC_VER) && (_MSC_VER >= 1400))
#define IRS_FULL_STDCPPLIB_SUPPORT
#endif //IRS_FULL_STDCPPLIB_SUPPORT

// Îïöèè äëÿ ôàéëà irscpp.h
#ifdef __embedded_cplusplus
#define NAMESPACE_STD_NOT_SUPPORT
#endif //__embedded_cplusplus

#ifdef NAMESPACE_STD_NOT_SUPPORT
#define IRS_STD
#else // !NAMESPACE_STD_NOT_SUPPORT
#define IRS_STD std::
#endif //!NAMESPACE_STD_NOT_SUPPORT

// Âêëþ÷åíèå 64-áèòíîãî òèïà
#define IRSDEFS_I64

// Âêëþ÷åíèå áîëüøîãî öåëîãî òèïà
#define IRSDEFS_LONG_LONG

// Ñïåöèàëüíûå îïèñàòåëè äëÿ ìàíèïóëÿòîðîâ â ðàçëè÷íûõ êîìïèëÿòîðàõ
#if defined(__WATCOMC__)
#define IRS_STREAMSPECDECL _WPRTLINK
#else //compilers
#define IRS_STREAMSPECDECL
#endif //compilers
// Îïðåäåëåíèå ìîäèôèêàòîðà flash äëÿ êîìïèëÿòîðà IAR AVR
#define IRS_ICCAVR_FLASH __flash
#ifdef __ICCAVR__
#define IRS_CSTR_NONVOLATILE IRS_ICCAVR_FLASH
#else //__ICCAVR__
#define IRS_CSTR_NONVOLATILE
#endif //__ICCAVR__

// GCC âåðñèè < 3.4
#if (defined(__GNUC__) && \
      (\
        ( (__GNUC__ < 3) || (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) )\
      )\
    )
#define IRS_GNUC_VERSION_LESS_3_4
#endif // GCC âåðñèè < 3.4

#ifdef __GNUC__

#define DO_PRAGMA_(x) _Pragma (#x)
#define DO_PRAGMA(x) DO_PRAGMA_(x)

#define IGNORE_DIAGNOSTIC_ENTER(diag) \
  _Pragma("GCC diagnostic push") \
  DO_PRAGMA(GCC diagnostic ignored diag)

#define IGNORE_DIAGNOSTIC_EXIT() _Pragma("GCC diagnostic pop")

#else // __GNUC__

#define IGNORE_DIAGNOSTIC_ENTER(diag)
#define GCC_IGNORE_DIAGNOSTIC_EXIT()

#endif // __GNUC__

#ifndef IRS_USE_SETUPAPI_WIN
# define IRS_USE_SETUPAPI_WIN 0
#endif // IRS_USE_SETUPAPI_WIN

#ifndef IRS_USE_HID_WIN_API
# define IRS_USE_HID_WIN_API 0
#endif // IRS_USE_HID_WIN_API

#ifndef IRS_USE_DEV_EXPRESS
# define IRS_USE_DEV_EXPRESS 0
#endif // IRS_USE_DEV_EXPRESS

#ifndef IRS_USE_FREE_RTOS
# define IRS_USE_FREE_RTOS 0
#endif // IRS_USE_FREE_RTOS

#ifndef IRS_USE_JSON_CPP
# define IRS_USE_JSON_CPP 0
#endif // IRS_USE_JSON_CPP

#ifndef IRS_USE_UTF8_CPP
# define IRS_USE_UTF8_CPP 0
#endif // IRS_USE_UTF8_CPP

#ifndef IRS_USE_BOOST
# define IRS_USE_BOOST 0
#endif // IRS_USE_BOOST

#ifndef IRS_USE_STM32F2_4_USB_HID_HARDFLOW
# define IRS_USE_STM32F2_4_USB_HID_HARDFLOW 0
#endif // IRS_USE_STM32F2_4_USB_HID_HARDFLOW

#ifndef IRS_USE_STM32_USB_HID_HARDFLOW
# define IRS_USE_STM32_USB_HID_HARDFLOW 0
#endif // IRS_USE_STM32_USB_HID_HARDFLOW

// Ñòàòè÷åñêèå óòâåðæäåíèÿ
// Âçÿòî èç STLSoft
// Ïåðåíåñåíî èç irserror.h äëÿ òîãî, ÷òîáû áûëà âîçìîæíîñòü ïðèìåíèòü
// â irsstrdefs.h
#ifdef IRS_GNUC_VERSION_LESS_3_4
    // Ýòà êîíñòàíòà ó ìåíÿ ïîêà îòñóòñòâóåò
    // || defined(STLSOFT_COMPILER_IS_INTEL)
  #define IRS_STATIC_ASSERT(ex)\
    do { typedef int ai[(ex) ? 1 : -1]; } while(0)
#else /* ? compiler */


    #define IRS_STATIC_ASSERT(ex)\
    do { \
        typedef int ai[(ex) ? 1 : 0] __attribute__((unused)); \
    } while(0)

#endif /* compiler */


#if IRS_CPP11_SUPPORT
typedef int8_t              irs_i8;
// 8-áèòîâîå öåëîå áåç çíàêà
typedef uint8_t             irs_u8;
// 16-áèòîâîå öåëîå ñî çíàêîì
typedef int16_t      		 		irs_i16;
// 16-áèòîâîå öåëîå áåç çíàêà
typedef uint16_t            irs_u16;
// 32-áèòîâîå öåëîå ñî çíàêîì
typedef int32_t            	irs_i32;
// 32-áèòîâîå öåëîå áåç çíàêà
typedef uint32_t        		irs_u32;
// 64-áèòîâîå öåëîå ñî çíàêîì
typedef int64_t             irs_i64;
// 64-áèòîâîå öåëîå áåç çíàêà
typedef uint64_t            irs_u64;
#else // !IRS_CPP11_SUPPORT
// Îäèí èç òèïîâ èñïîëüçóåòñÿ â ôóíêöèè detect_cpu_endian, ïîýòîìó ñòîèò âûøå
// äðóãèõ îáúÿâëåíèé òèïîâ
// 8-áèòîâîå öåëîå ñî çíàêîì
typedef signed char 		 		irs_i8;
// 8-áèòîâîå öåëîå áåç çíàêà
typedef unsigned char 	 		irs_u8;
// 16-áèòîâîå öåëîå ñî çíàêîì
typedef signed short 		 		irs_i16;
// 16-áèòîâîå öåëîå áåç çíàêà
typedef unsigned short 	 		irs_u16;
// 32-áèòîâîå öåëîå ñî çíàêîì
typedef signed long 		 		irs_i32;
// 32-áèòîâîå öåëîå áåç çíàêà
typedef unsigned long 	 		irs_u32;

# ifdef __BORLANDC__
// 64-áèòîâîå öåëîå ñî çíàêîì
typedef signed __int64    irs_i64;
// 64-áèòîâîå öåëîå áåç çíàêà
typedef unsigned __int64  irs_u64;
# else //__BORLANDC__
// 64-áèòîâîå öåëîå ñî çíàêîì
typedef signed long long 		irs_i64;
// 64-áèòîâîå öåëîå áåç çíàêà
typedef unsigned long long 	irs_u64;
# endif //__BORLANDC__

#endif // !IRS_CPP11_SUPPORT


#ifdef __BORLANDC__
// Áîëüøîå öåëîå ñî çíàêîì
typedef signed __int64 irs_ilong_long;
// Áîëüøîå öåëîå áåç çíàêà
typedef unsigned __int64 irs_ulong_long;
#else // !__BORLANDC__
// Áîëüøîå öåëîå ñî çíàêîì
typedef signed long long irs_ilong_long;
// Áîëüøîå öåëîå áåç çíàêà
typedef unsigned long long irs_ulong_long;
#endif // !__BORLANDC__
// 32-áèòîâîå ïëàâàþùåå
typedef float irs_float32;
// 64-áèòîâîå ïëàâàþùåå
typedef double irs_float64;

// Èñïîëüçóåòñÿ â ôóíêöèè detect_cpu_endian, ïîýòîìó ñòîèò âûøå
// ìàêðîñîâ IRS_HIBYTE, ...
#define IRS_FIRSTDWORD(_NUM_) (*((reinterpret_cast<irs_u32*>(&(_NUM_)))))
#define IRS_SECONDDWORD(_NUM_) (*((reinterpret_cast<irs_u32*>(&(_NUM_))) + 1))
#define IRS_FIRSTWORD(_NUM_) (*((reinterpret_cast<irs_u16*>(&(_NUM_)))))
#define IRS_SECONDWORD(_NUM_) (*((reinterpret_cast<irs_u16*>(&(_NUM_))) + 1))
#define IRS_FIRSTBYTE(_NUM_) (*((reinterpret_cast<irs_u8*>(&(_NUM_)))))
#define IRS_SECONDBYTE(_NUM_) (*((reinterpret_cast<irs_u8*>(&(_NUM_))) + 1))

//! @}

namespace irs {

//! \addtogroup configuration_group
//! @{

// Îïðåäåëåíèå ïîðÿäêà áàéòîâ âî âðåìÿ âûïîëíåíèÿ
enum endian_t {
  little_endian = 0,
  big_endian = 1
};

inline endian_t detect_cpu_endian()
{
  irs_u16 endian_test_var = static_cast<irs_u16>(1); /* 0x0001 */
  endian_t cpu_endian =
    static_cast<endian_t>(IRS_FIRSTBYTE(endian_test_var) == 0);
  return cpu_endian;
}

//! @}

} // namespace irs

//! \addtogroup configuration_group
//! @{

// Îïðåäåëåíèå ïîðÿäêà áàéòîâ âî âðåìÿ êîìïèëÿöèè
#define IRS_LITTLE_ENDIAN 0
#define IRS_BIG_ENDIAN 1
#ifdef __AVR32__
#define IRS_CPU_ENDIAN IRS_BIG_ENDIAN
#else //__AVR32__
#define IRS_CPU_ENDIAN IRS_LITTLE_ENDIAN
#endif //__AVR32__

// Âû÷èñëåíèå ðàçìåðà ñòàòè÷åñêîãî ìàññèâà
#define IRS_ARRAYSIZE(_ARRAY_) (sizeof(_ARRAY_)/sizeof(*(_ARRAY_)))
#define IRS_ARRAYOFSIZE(_ARRAY_) IRS_ARRAYSIZE(_ARRAY_)
// Îïðåäåëåíèå äëÿ íóëåâîãî óêàçàòåëÿ
#define IRS_NULL 0
// Ìàêðîñû âûäåëåíèÿ èç ïåðåìåííûõ èõ ÷àñòåé
// BYTE - áàéòû, WORD - 2 áàéòà, DWORD - 4 áàéòà
#define IRS_HIDWORD(_NUM_) ((IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)?\
  *((reinterpret_cast<irs_u32*>(&(_NUM_))) + 1):\
  *((reinterpret_cast<irs_u32*>(&(_NUM_)))))
#define IRS_LODWORD(_NUM_) ((IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)?\
  *((reinterpret_cast<irs_u32*>(&(_NUM_)))):\
  *((reinterpret_cast<irs_u32*>(&(_NUM_))) + 1))
#define IRS_HIWORD(_NUM_) ((IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)?\
  *((reinterpret_cast<irs_u16*>(&(_NUM_))) + 1):\
  *((reinterpret_cast<irs_u16*>(&(_NUM_)))))
#define IRS_LOWORD(_NUM_) ((IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)?\
  *((reinterpret_cast<irs_u16*>(&(_NUM_)))):\
  *((reinterpret_cast<irs_u16*>(&(_NUM_))) + 1))
#define IRS_HIBYTE(_NUM_) ((IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)?\
  *((reinterpret_cast<irs_u8*>(&(_NUM_))) + 1):\
  *((reinterpret_cast<irs_u8*>(&(_NUM_)))))
#define IRS_LOBYTE(_NUM_) ((IRS_CPU_ENDIAN==IRS_LITTLE_ENDIAN)?\
  *((reinterpret_cast<irs_u8*>(&(_NUM_)))):\
  *((reinterpret_cast<irs_u8*>(&(_NUM_))) + 1))

// Ìàêðîñû âûäåëåíèÿ èç ïåðåìåííûõ èõ ÷àñòåé äëÿ êîíñòàíò
#define IRS_CONST_HIDWORD(_NUM_)\
  (static_cast<irs_u32>(((_NUM_) >> 32)&0xFFFFFFFF))
#define IRS_CONST_LODWORD(_NUM_)\
  (static_cast<irs_u32>((_NUM_)&0xFFFFFFFF))
#define IRS_CONST_HIWORD(_NUM_)\
  (static_cast<irs_u16>(((_NUM_) >> 16)&0xFFFF))
#define IRS_CONST_LOWORD(_NUM_)\
  (static_cast<irs_u16>((_NUM_)&0xFFFF))
#define IRS_CONST_HIBYTE(_NUM_)\
  (static_cast<irs_u8>(((_NUM_) >> 8)&0xFF))
#define IRS_CONST_LOBYTE(_NUM_)\
  (static_cast<irs_u8>((_NUM_)&0xFF))

// Ìàêðîñû äëÿ íàõîæäåíèÿ ìàêñèìàëüíîãî è ìèíèìàëüíîãî çíà÷åíèÿ
#define irs_max(_A_, _B_) (((_A_) > (_B_))?(_A_):(_B_))
#define irs_min(_A_, _B_) (((_A_) < (_B_))?(_A_):(_B_))
// Ìàêðîñû óäàëåíèÿ
#define IRS_DELETE(_VAR_) { if (_VAR_) delete _VAR_; _VAR_ = IRS_NULL; }
#define IRS_ARDELETE(_VAR_) { if (_VAR_) delete []_VAR_; _VAR_ = IRS_NULL; }
// Ìàêðîñ __FUNC__
#if defined(__GNUC__) || defined(__WATCOMC__) || defined(__ICCAVR__) ||\
  defined(_MSC_VER)
#define __FUNC__ __FUNCTION__
#endif //defined(__GNUC__) || defined(__WATCOMC__) || defined(__ICCAVR__)

#ifdef __ICCAVR__
// Ðàçðåøåíèå ïðåðûâàíèé
#define irs_enable_interrupt() { __enable_interrupt(); }
// Çàïðåùåíèå ïðåðûâàíèé
#define irs_disable_interrupt() { __disable_interrupt(); }
#endif //__ICCAVR__

// Ìàêðîñû äëÿ ðàáîòû c DLL
// Îáúÿâëåíèå âíåøíåé ôóíêöèè
#define IRS_DLL_FUNC_DECL(_RETURN_, _FUNC_NAME_, _ARGS_)\
  extern "C" __declspec(dllexport) _RETURN_ _FUNC_NAME_ _ARGS_;
// Îïðåäåëåíèå òèïà äëÿ ôóíêöèè
#define IRS_DLL_FUNC_TYPE_DEF(_RETURN_, _FUNC_NAME_, _ARGS_)\
  typedef _RETURN_ (*_FUNC_NAME_##_t)_ARGS_;
// Îáúÿâëåíèå óêàçàòåëÿ íà ôóíêöèè
#define IRS_DLL_FUNC_DECL_VAR(_FUNC_)\
  _FUNC_##_t _FUNC_ = IRS_NULL;
// Ïîäêëþ÷åíèå óêçàòåëÿ íà ôóíêöèþ ê ôóíêöèè DLL
#define IRS_DLL_FUNC_ATTACH(_FUNC_)\
{\
  _FUNC_ = (_FUNC_##_t)GetProcAddress(LibHandle, "_" #_FUNC_);\
  if (!_FUNC_) { NumErrFunc++; goto err_func; }\
}

// Ìàòåìàòè÷åñêèå êîíñòàíòû
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

// Ìàêñèìàëüíûå è ìèíèìàëüíûå çíà÷åíèÿ ïî òèïàì
#define IRS_I8_MIN (-128)
#define IRS_I8_MAX 127
#define IRS_U8_MIN 0
#define IRS_U8_MAX 255
#define IRS_I16_MIN (-32767 - 1)
#define IRS_I16_MAX 32767
#define IRS_U16_MIN 0
#define IRS_U16_MAX 65535
#define IRS_I32_MIN (-2147483647L - 1)
#define IRS_I32_MAX 2147483647L
#define IRS_U32_MIN 0UL
#define IRS_U32_MAX 4294967295UL
#define IRS_IARC_MIN INT_MIN
#define IRS_IARC_MAX INT_MAX
#define IRS_UARC_MIN 0U
#define IRS_UARC_MAX UINT_MAX

// Òèï ñ ðàçðÿäíîñòüþ ïðîöåññîðà (ÎÑ) ñî çíàêîì
typedef int           irs_iarc;
// Òèï ñ ðàçðÿäíîñòüþ ïðîöåññîðà (ÎÑ) áåç çíàêà
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
// Ìàêñèìàëüíîå öåëîå ñî çíàêîì
typedef irs_i64 irs_imax;
// Ìàêñèìàëüíîå öåëîå áåç çíàêà
typedef irs_u64 irs_umax;

#else //IRSDEFS_I64

#define IRS_IMAX_MIN IRS_I32_MIN
#define IRS_IMAX_MAX IRS_I32_MAX
#define IRS_UMAX_MIN IRS_U32_MIN
#define IRS_UMAX_MAX IRS_U32_MAX
// Ìàêñèìàëüíîå öåëîå ñî çíàêîì
typedef irs_i32 irs_imax;
// Ìàêñèìàëüíîå öåëîå áåç çíàêà
typedef irs_u32 irs_umax;

#endif //IRSDEFS_I64

// Áóëåâûé òèï
typedef irs_u8   irs_bool;

const irs_bool   irs_true = 1;
const irs_bool   irs_false = 0;

typedef void (*irs_vfunc)();

#if defined(IRS_WIN32)
# define IRS_INVALID_SOCKET INVALID_SOCKET
# define IRS_SOCKET_ERROR SOCKET_ERROR
#elif defined(IRS_LINUX)
# define IRS_INVALID_SOCKET -1
# define IRS_SOCKET_ERROR -1
#endif // IRS_LINUX

// Òèï äëÿ ïèíà
enum irs_pin_t {
  irs_on = 0,
  irs_off = 1
};

// Àáñòðàêòíûé òèï äëÿ ïîðòîâ AVR
enum irs_avr_port_t {
  irs_avr_porta = 0,
  irs_avr_portb = 1,
  irs_avr_portc = 2,
  irs_avr_portd = 3,
  irs_avr_porte = 4,
  irs_avr_portf = 5,
  irs_avr_portg = 6
};

#ifdef __ICCARM__
// Óêàçàòåëü íà ïîðò ARM
typedef volatile irs_u32 arm_port_t;
typedef arm_port_t* p_arm_port_t;
typedef volatile irs_u32 mc_port_t;
typedef arm_port_t* p_mc_port_t;
#define mc arm
#endif // __ICCARM__

#ifdef __ICCAVR__
// Óêàçàòåëü íà ïîðò AVR
typedef irs_u8 volatile __tiny avr_port_t;
typedef avr_port_t* p_avr_port_t;
typedef irs_u8 volatile __tiny mc_port_t;
typedef avr_port_t* p_mc_port_t;
#define mc avr
#endif //__ICCAVR__

// Êëàññ äëÿ ôóíêöèé îáðàáîòêè
class mx_proc_t {
public:
  virtual ~mx_proc_t() {}
  virtual irs_bool tick() = 0;
  virtual void abort() = 0;
};

// Ïåðåîïðåäåëåíèå ñòàíàðòíûõ òèïîâ
typedef size_t irs_size_t;
typedef ptrdiff_t irs_ptrdiff_t;

namespace irs {

const int npos = -1;

enum switch_t {
  sw_off = 0,
  sw_on = 1
};

//! \brief Êîä óñòðîéñòâà
enum device_code_t {
  device_code_unknown = 0,
  device_code_first = 1,
  device_code_itn_2400 = device_code_first,
  device_code_gtch_03m = 14,
  device_code_u5023m = 16,
  device_code_u309m = 17,
  device_code_hrm = 18,
  device_code_gnlf = 19,
  device_code_upms_1v = 20,
  device_code_last = device_code_hrm
};

inline bool is_known_device_code(irs_size_t a_code)
{
  return (a_code == device_code_itn_2400) ||
    (a_code == device_code_gtch_03m) ||
    (a_code == device_code_u5023m) ||
    (a_code == device_code_u309m) ||
    (a_code == device_code_hrm) ||
    (a_code == device_code_gnlf);
}


}//namespace irs

//  Ñòàòóñ âñåãî
enum irs_status_t {
  irs_st_busy,
  irs_st_ready,
  irs_st_error
};

// Ãàñèì íåíóæíûå ïðåäóïðåæäåíèÿ C++ Builder
#ifdef __BCPLUSPLUS__
// Condition is always true OR Condition is always false (Default ON)
#pragma warn -8008
#if __BCPLUSPLUS__ >= IRS_CPP_BUILDER6
// 'type' argument 'specifier' passed to 'function' is a 'iterator category'
// iterator: 'iterator category' iterator required (W8091)
#pragma warn -8091
#endif //__BCPLUSPLUS__ >= IRS_CPP_BUILDER6
#endif //__BCPLUSPLUS__

//! @}

#endif //IRSDEFSH