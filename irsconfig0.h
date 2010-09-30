// Локальная конфигурация для каждого программиста
// Дата 29.09.2010
// Дата создания 25.09.2009

// Скопируйте этот файл "irsconfig0.h" в файл с именем "irsconfig.h"
// Измените в "irsconfig.h" (но не "irsconfig0.h") опции, если необходимо

#ifndef IRSCONFIGH 
#define IRSCONFIGH

#define IRS_LIB_LAST_VERSION INT_MAX
#define IRS_LIB_VERSION_SUPPORT IRS_LIB_LAST_VERSION

#define IRS_LIB_DEBUG_GLOBAL
//#define IRS_LIB_CHECK

#define IRS_LIB_DEBUG_NONE 0
#define IRS_LIB_DEBUG_BASE 1
#define IRS_LIB_DEBUG_DETAIL 2

//#define IRS_LIB_IRSTCPIP_DEBUG_TYPE IRS_LIB_DEBUG_BASE
//#define IRS_LIB_HARDFLOWG_DEBUG_TYPE IRS_LIB_DEBUG_BASE
//#define IRS_LIB_IRSMBUS_DEBUG_TYPE IRS_LIB_DEBUG_BASE

// Индивидуальные отладочные define-определения
// Крашенинников Максим
#define IRS_LIB_DBG_KRASHENINNIKOV_MAXIM

#ifdef __ICCAVR__
#define IRS_LIB_FLASH_ASSERT
#endif //__ICCAVR__

#ifdef IRS_LIB_DEBUG_GLOBAL 
#if defined(__ICCAVR__) 
//#define IRS_LIB_DEBUG
#else //compilers
#define IRS_LIB_DEBUG 
#endif //compilers 
#endif //IRS_LIB_DEBUG_GLOBAL 
 
#endif //IRSCONFIGH  
