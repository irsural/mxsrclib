// Обработка ошибок
// Дата: 4.08.2009

#ifndef IRSERRORH
#define IRSERRORH

#include <irsdefs.h>
#include <irsstd.h>
#include <irstime.h>
#include <irscpp.h>

#define IRS_LIB_DEBUG
//#define IRS_LIB_CHECK

#define IRS_ERROR_HELPER(error_code, spec_data)\
  {\
    irs::error_trans()->throw_error(error_code, __FILE__, __LINE__,\
      static_cast<const void*>(spec_data));\
  }

#define IRS_ASSERT(assert_expr)\
  {\
    if (!(assert_expr)) {\
      IRS_ERROR_HELPER(irs::ec_assert, irs::spec_assert(#assert_expr, 0))\
    }\
  }
#define IRS_ASSERT_EX(assert_expr, msg)\
  {\
    if (!(assert_expr)) {\
      IRS_ERROR_HELPER(irs::ec_assert, irs::spec_assert(#assert_expr, msg))\
    }\
  }
#define IRS_ERROR(error_code, msg)\
  {\
    IRS_ERROR_HELPER(error_code, msg)\
  }
#define IRS_FATAL_ERROR(msg) IRS_ERROR(irs::ec_fatal_error, msg)
#define IRS_ASSERT_MSG(msg)\
  {\
    IRS_ERROR_HELPER(irs::ec_assert, irs::spec_assert("Assert", msg))\
  }

#define IRS_DBG_RAW_MSG(msg)\
  {\
    irs::mlog() << msg;\
  }
#define IRS_DBG_MSG(msg)\
  {\
    IRS_DBG_RAW_MSG(irs::stime << msg << endl)\
  }
#define IRS_DBG_MSG_SRC(msg)\
  {\
    irs::mlog() << irs::stime << "Файл: " << __FILE__;\
    irs::mlog() << "; Строка: " << __LINE__;\
    irs::mlog() << "; Функция: " << __FUNC__ << "; " << msg << endl;\
  }

#ifdef IRS_LIB_DEBUG
#define IRS_LIB_ASSERT(assert_expr) IRS_ASSERT(assert_expr)
#define IRS_LIB_ASSERT_EX(assert_expr, msg) IRS_ASSERT_EX(assert_expr, msg)
#define IRS_LIB_ERROR(error_code, msg) IRS_ERROR(error_code, msg)
#define IRS_LIB_FATAL_ERROR(msg) IRS_FATAL_ERROR(msg)
#define IRS_LIB_ASSERT_MSG(msg) IRS_ASSERT_MSG(msg)
#define IRS_LIB_DBG_RAW_MSG(msg) IRS_DBG_RAW_MSG(msg)
#define IRS_LIB_DBG_MSG(msg) IRS_DBG_MSG(msg)
#define IRS_LIB_DBG_MSG_SRC(msg) IRS_DBG_MSG_SRC(msg)
#else // IRS_LIB_DEBUG
#define IRS_LIB_ASSERT(assert_expr)
#define IRS_LIB_ASSERT_EX(assert_expr, msg)
#define IRS_LIB_ERROR(error_code, msg)
#define IRS_LIB_FATAL_ERROR(msg))
#define IRS_LIB_ASSERT_MSG(msg)
#define IRS_LIB_DBG_RAW_MSG(msg)
#define IRS_LIB_DBG_MSG(msg)
#define IRS_LIB_DBG_MSG_SRC(msg)
#endif // IRS_LIB_DEBUG

// Статические утверждения
// Взято из STLSoft
#if\
    (\
      defined(__GNUC__) && \
      (\
        (__GNUC__ < 3) ||\
        ( (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) )\
      )\
    )\
    // Эта константа у меня пока отсутствует
    // || defined(STLSOFT_COMPILER_IS_INTEL)
  #define IRS_STATIC_ASSERT(ex)\
    do { typedef int ai[(ex) ? 1 : -1]; } while(0)
#else /* ? compiler */
  #define IRS_STATIC_ASSERT(ex)\
    do { typedef int ai[(ex) ? 1 : 0]; } while(0)
#endif /* compiler */

namespace irs {

// Тип для кодов ошибок
typedef irs_u16 error_code_t;

// Специфические данные для утвердений в error_trans_base_t
struct et_spec_assert_t
{
  // Утверждение
  const char *assert_str;
  // Сообщение
  const char *message;
};

// Стандартный код ошибки (в ap_spec_data содержится сообщение в виде char*)
const error_code_t ec_standard = 0;
// Код ошибки для утверждений (ap_spec_data типа et_spec_assert_t*)
const error_code_t ec_assert = 1;
// Код фатальной ошибки (ap_spec_data типа char*)
const error_code_t ec_fatal_error = 2;
// Пользовательские коды ошибок
const error_code_t ec_user = 3;

// Приемо-передатчик ошибок (Интерфейс)
class error_trans_base_t
{
public:
  virtual ~error_trans_base_t() {}
  // Передать ошибку (тип ap_spec_data определяется a_error_code)
  virtual void throw_error(
    error_code_t a_error_code = ec_standard,
    const char *ap_file_name = 0,
    int a_line_number = 0,
    const void *ap_spec_data = 0
  ) = 0;
  // Считать последний код ошибки
  virtual error_code_t error_code() = 0;
  // Считать имя файла в котором произошла последняя ошибка
  virtual const char *file_name() = 0;
  // Считать номер строки в файле в которой произошла ошибка
  virtual int line_number() = 0;
  // Считать специфические данные соответствующие коду ошибки
  virtual const void *spec_data() = 0;
  // Добавить обработчик ошибки
  virtual void add_handler(mxfact_event_t *ap_handler) = 0;
};

// Приемо-передатчик ошибок (Считывание указателя на экземпляр)
error_trans_base_t *error_trans();

// Специфические данные для утвердений в error_trans_base_t
//  (Считывание указателя на экземпляр)
const void *spec_assert(const char *assert_str,
  const char *message);

ostream& mlog();

} //namespace irs

#endif //IRSERRORH

