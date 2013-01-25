//! \file
//! \ingroup drivers_group
//! \brief Работа с National Instruments USB-GPIB

//! Дата: 06.08.2010\n
//! Дата создания: 06.08.2010

#ifndef niusbgpibH
#define niusbgpibH

#include <irsdefs.h>

#if defined(IRS_WIN32)
#include <winsock2.h>
#endif // IRS_WIN32

#include <irsstrdefs.h>
#include <irserror.h>
#include <irsstrconv.h>

#include <irsfinal.h>

//! \addtogroup drivers_group
//! @{

#define TRY_LOAD_FIRST_GPIB_32_DLL

#if defined(IRS_WIN32)

enum ni_usb_gpib_dll_file_name_t {
  ni_fn_first,
  ni_fn_gpib_32 = ni_fn_first,
  ni_fn_agtgpib32,
  ni_fn_last = ni_fn_agtgpib32
};

irs::string_t get_ni_usb_gpib_dll_file_name(
  ni_usb_gpib_dll_file_name_t a_ni_usb_gpib_dll_file_name);

// Класс для работы с National Instruments USB-GPIB
class ni_usb_gpib_t
{
private:
  // Макросы для подключения функций и переменных gpib-32.dll
  #define DEF_DLL_PROC(_LIB_, _PROC_)\
  {\
    f_##_PROC_ = reinterpret_cast<_PROC_##_t>(\
    GetProcAddress(_LIB_, #_PROC_));\
    if (!f_##_PROC_) goto def_dll_err;\
  }
  #define DEF_DLL_VAR(_LIB_, _TYPE_, _VAR_)\
  {\
    f_p##_VAR_ = reinterpret_cast<_TYPE_*>(\
    GetProcAddress(_LIB_, "user_" #_VAR_));\
    if (!f_p##_VAR_) goto def_dll_err;\
  }

  // Определения типов для функций gpib-32.dll
  typedef int (__stdcall *ibdev_t)  (int boardID, int pad, int sad, int tmo,
                                     int eot, int eos);
  typedef int (__stdcall *ibdma_t)  (int ud, int v);
  typedef int (__stdcall *ibfindA_t)  (char* udname);
  typedef int (__stdcall *ibclr_t)  (int ud);
  typedef int (__stdcall *ibcmd_t)  (int ud, PVOID buf, long cnt);
  typedef int (__stdcall *ibwrt_t)  (int ud, PVOID buf, long cnt);
  typedef int (__stdcall *ibrd_t)   (int ud, PVOID buf, long cnt);
  typedef int (__stdcall *ibrda_t)  (int ud, PVOID buf, long cnt);
  typedef int (__stdcall *ibwrta_t) (int ud, PVOID buf, long cnt);
  typedef int (__stdcall *ibrsc_t)  (int ud, int v);
  typedef int (__stdcall *ibsic_t) (int ud);
  typedef int (__stdcall *ibstop_t) (int ud);
  typedef int (__stdcall *ibonl_t)  (int ud, int v);
  typedef int (__stdcall *ibtmo_t)  (int ud, int v);
  typedef int (__stdcall *ibwait_t) (int ud, int mask);
  typedef int (__stdcall *ibconfig_t)(int ud, int option, int v);

  // Указатели на функций и переменные gpib-32.dll
  int *f_pibsta;
  int *f_piberr;
  long *f_pibcntl;
  ibdev_t f_ibdev;
  ibdma_t f_ibdma;
  ibfindA_t f_ibfindA;
  ibclr_t f_ibclr;
  ibcmd_t f_ibcmd;
  ibwrt_t f_ibwrt;
  ibrd_t f_ibrd;
  ibrda_t f_ibrda;
  ibwrta_t f_ibwrta;
  ibrsc_t f_ibrsc;
  ibsic_t f_ibsic;
  ibstop_t f_ibstop;
  ibonl_t f_ibonl;
  ibtmo_t f_ibtmo;
  ibwait_t f_ibwait;
  ibconfig_t f_ibconfig;

  // Ошибка инициализации
  irs_bool f_init_fail;
  // Дескриптор dll
  HINSTANCE f_Gpib32Lib;
public:
  typedef irs::string_t string_type;

  // Конструктор
  ni_usb_gpib_t(const string_type& a_dll_file_name = string_type()):
    f_pibsta(IRS_NULL),
    f_piberr(IRS_NULL),
    f_pibcntl(IRS_NULL),
    f_ibdev(IRS_NULL),
    f_ibdma(IRS_NULL),
    f_ibfindA(IRS_NULL),
    f_ibclr(IRS_NULL),
    f_ibcmd(IRS_NULL),
    f_ibwrt(IRS_NULL),
    f_ibrd(IRS_NULL),
    f_ibrda(IRS_NULL),
    f_ibwrta(IRS_NULL),
    f_ibrsc(IRS_NULL),
    f_ibsic(IRS_NULL),
    f_ibstop(IRS_NULL),
    f_ibonl(IRS_NULL),
    f_ibtmo(IRS_NULL),
    f_ibwait(IRS_NULL),
    f_ibconfig(IRS_NULL),

    f_init_fail(irs_true),
    //f_count_init(0),
    f_Gpib32Lib(IRS_NULL)
  {
    if (a_dll_file_name.empty()) {
      #ifdef TRY_LOAD_FIRST_GPIB_32_DLL
      f_Gpib32Lib = LoadLibrary(irst("GPIB-32.DLL"));
      if (!f_Gpib32Lib) {
        f_Gpib32Lib = LoadLibrary(irst("agtgpib32.dll"));
        if (f_Gpib32Lib) {
          IRS_LIB_DBG_MSG("Загружена agtgpib32.dll");
        }
      } else {
        IRS_LIB_DBG_MSG("Загружена gpib-32.dll");
      }
      #else // !TRY_LOAD_FIRST_GPIB_32_DLL
      f_Gpib32Lib = LoadLibrary(irst("agtgpib32.dll"));
      if (!f_Gpib32Lib) {
        f_Gpib32Lib = LoadLibrary(irst("GPIB-32.DLL"));
        if (f_Gpib32Lib) {
          IRS_LIB_DBG_MSG("Загружена gpib-32.dll");
        }
      } else {
        IRS_LIB_DBG_MSG("Загружена agtgpib32.dll");
      }
      #endif // !TRY_LOAD_FIRST_GPIB_32_DLL
    } else {
      f_Gpib32Lib = LoadLibrary(a_dll_file_name.c_str());
      if (f_Gpib32Lib) {
        IRS_LIB_DBG_MSG(irs::str_conv<irs::irs_string_t>(irst("Загружена ") +
          a_dll_file_name).c_str());
      }
    }
    if (f_Gpib32Lib) {
      f_init_fail = irs_false;
      DEF_DLL_VAR(f_Gpib32Lib, int, ibsta);
      DEF_DLL_VAR(f_Gpib32Lib, int, iberr);
      DEF_DLL_VAR(f_Gpib32Lib, long, ibcntl);
      DEF_DLL_PROC(f_Gpib32Lib, ibdev);
      DEF_DLL_PROC(f_Gpib32Lib, ibdma);
      DEF_DLL_PROC(f_Gpib32Lib, ibfindA);
      DEF_DLL_PROC(f_Gpib32Lib, ibclr);
      DEF_DLL_PROC(f_Gpib32Lib, ibcmd);
      DEF_DLL_PROC(f_Gpib32Lib, ibwrt);
      DEF_DLL_PROC(f_Gpib32Lib, ibrd);
      DEF_DLL_PROC(f_Gpib32Lib, ibrda);
      DEF_DLL_PROC(f_Gpib32Lib, ibwrta);
      DEF_DLL_PROC(f_Gpib32Lib, ibrsc);
      DEF_DLL_PROC(f_Gpib32Lib, ibsic);
      DEF_DLL_PROC(f_Gpib32Lib, ibstop);
      DEF_DLL_PROC(f_Gpib32Lib, ibonl);
      DEF_DLL_PROC(f_Gpib32Lib, ibtmo);
      DEF_DLL_PROC(f_Gpib32Lib, ibwait);
      DEF_DLL_PROC(f_Gpib32Lib, ibconfig);
    } else {
      f_init_fail = irs_true;
      #ifdef __BORLANDC__
      ShowMessage("Библиотеки GPIB-32.DLL и agtgpib32.dll не найдена");
      #else
      IRS_LIB_ERROR(irs::ec_standard, "Библиотека GPIB-32.DLL не найдена");
      #endif //__BORLANDC__
      return;
    }
    return;

    def_dll_err:
    IRS_LIB_ERROR(irs::ec_standard,
      "Ошибка при подключении функций GPIB-32.DLL");
    //ShowMessage("Ошибка при подключении функций GPIB-32.DLL");
    return;
  }
  // Деструктор
  ~ni_usb_gpib_t()
  {
    FreeLibrary(f_Gpib32Lib);
  }
  static ni_usb_gpib_t *get_instance();
  // Инициализация
  void init()
  {
    // Для совместимости со старым кодом
  }
  // Деинициализация
  void deinit()
  {
    // Для совместимости со старым кодом
  }
  irs_i32 get_ibsta()
  {
    if (f_init_fail) return 0;
    return *f_pibsta;
  }
  irs_i32 get_iberr()
  {
    if (f_init_fail) return 0;
    return *f_piberr;
  }
  irs_i32 get_ibcntl()
  {
    if (f_init_fail) return 0;
    return *f_pibcntl;
  }
  irs_i32 ibdev(irs_i32 boardID, irs_i32 pad, irs_i32 sad, irs_i32 tmo,
    irs_i32 eot, irs_i32 eos)
  {
    if (f_init_fail) return 0;
    return f_ibdev(boardID, pad, sad, tmo, eot, eos);
  }
  irs_i32 ibdma(irs_i32 ud, irs_i32 v)
  {
    if (f_init_fail) return 0;
    return f_ibdma(ud, v);
  }
  irs_i32 ibfindA(char* udname)
  {
    if (f_init_fail) return 0;
    return f_ibfindA(udname);
  }
  irs_i32 ibclr(irs_i32 ud)
  {
    if (f_init_fail) return 0;
    return f_ibclr(ud);
  }
  irs_i32 ibcmd(irs_i32 ud, void *buf, irs_i32 cnt)
  {
    if (f_init_fail) return 0;
    return f_ibcmd(ud, buf, cnt);
  }
  irs_i32 ibwrt(irs_i32 ud, void *buf, irs_i32 cnt)
  {
    if (f_init_fail) return 0;
    return f_ibwrt(ud, buf, cnt);
  }
  irs_i32 ibrd(irs_i32 ud, void *buf, irs_i32 cnt)
  {
    if (f_init_fail) return 0;
    return f_ibrd(ud, buf, cnt);
  }
  irs_i32 ibrda(irs_i32 ud, void *buf, irs_i32 cnt)
  {
    if (f_init_fail) return 0;
    return f_ibrda(ud, buf, cnt);
  }
  irs_i32 ibwrta(irs_i32 ud, void *buf, irs_i32 cnt)
  {
    if (f_init_fail) return 0;
    return f_ibwrta(ud, buf, cnt);
  }
  irs_i32 ibrsc(irs_i32 ud, irs_i32 v)
  {
    if (f_init_fail) return 0;
    return f_ibrsc(ud, v);
  }
  irs_i32 ibsic(irs_i32 ud)
  {
    if (f_init_fail) return 0;
    return f_ibsic(ud);
  }
  irs_i32 ibstop(irs_i32 ud)
  {
    if (f_init_fail) return 0;
    return f_ibstop(ud);
  }
  irs_i32 ibonl(irs_i32 ud, irs_i32 v)
  {
    if (f_init_fail) return 0;
    return f_ibonl(ud, v);
  }
  irs_i32 ibtmo(irs_i32 ud, irs_i32 v)
  {
    if (f_init_fail) return 0;
    return f_ibtmo(ud, v);
  }
  irs_i32 ibwait(irs_i32 ud, irs_i32 mask)
  {
    if (f_init_fail) return 0;
    return f_ibwait(ud, mask);
  }
  irs_i32 ibconfig(irs_i32 ud, irs_i32 option, irs_i32 v)
  {
    if (f_init_fail) return 0;
    return f_ibconfig(ud, option, v);
  }
};

// Экземпляр класса ni_usb_gpib_t

#endif // defined(IRS_WIN32)

//! @}

#endif // niusbgpibH
