// Обработка ошибок
// Дата: 20.09.2009
// Ранняя дата: 4.08.2009

#include <irserror.h>

namespace irs {

// Приемо-передатчик ошибок (Реализация)
class error_trans_t: public error_trans_base_t
{
public:
  error_trans_t();
  virtual ~error_trans_t();
  // Передать ошибку (тип ap_spec_data определяется a_error_code)
  virtual void throw_error(
    error_code_t a_error_code = ec_standard,
    const char *ap_file_name = 0,
    int a_line_number = 0,
    const void *ap_spec_data = 0
  );
  // Считать последний код ошибки
  virtual error_code_t error_code();
  // Считать имя файла в котором произошла последняя ошибка
  virtual const char *file_name();
  // Считать номер строки в файле в которой произошла ошибка
  virtual int line_number();
  // Считать специфические данные соответствующие коду ошибки
  virtual const void *spec_data();
  // Добавить обработчик ошибки
  virtual void add_handler(mxfact_event_t *ap_handler);
private:
  error_code_t m_error_code;
  const char *mp_file_name;
  int m_line_number;
  const void *mp_spec_data;
  mxfact_event_t *mp_handler;
};

} //namespace irs

// Приемо-передатчик ошибок (Реализация)
irs::error_trans_t::error_trans_t():
  m_error_code(ec_standard),
  mp_file_name(0),
  m_line_number(0),
  mp_spec_data(0),
  mp_handler(0)
{
}
irs::error_trans_t::~error_trans_t()
{
}
// Передать ошибку (тип ap_spec_data определяется a_error_code)
void irs::error_trans_t::throw_error(
  error_code_t a_error_code,
  const char *ap_file_name,
  int a_line_number,
  const void *ap_spec_data
)
{
  m_error_code = a_error_code;
  mp_file_name = ap_file_name;
  m_line_number = a_line_number;
  mp_spec_data = ap_spec_data;
  if (mp_handler) mp_handler->exec();
}
// Считать последний код ошибки
irs::error_code_t irs::error_trans_t::error_code()
{
  return m_error_code;
}
// Считать имя файла в котором произошла последняя ошибка
const char *irs::error_trans_t::file_name()
{
  return mp_file_name;
}
// Считать номер строки в файле в которой произошла ошибка
int irs::error_trans_t::line_number()
{
  return m_line_number;
}
// Считать специфические данные соответствующие коду ошибки
const void *irs::error_trans_t::spec_data()
{
  return mp_spec_data;
}
// Добавить обработчик ошибки
void irs::error_trans_t::add_handler(mxfact_event_t *ap_handler)
{
  ap_handler->connect(mp_handler);
}

// Приемо-передатчик ошибок (Считывание указателя на экземпляр)
irs::error_trans_base_t *irs::error_trans()
{
  static error_trans_t error_trans_i;
  return &error_trans_i; 
}

// Специфические данные для утвердений в error_trans_base_t
//  (Считывание указателя на экземпляр)
const  void* irs::spec_assert(const char *assert_str,
  const char *message)
{
  static et_spec_assert_t spec_assert_i;
  spec_assert_i.assert_str = assert_str;
  spec_assert_i.message = message;
  return static_cast<const  void*>(&spec_assert_i);
}

ostream& irs::mlog()
{
  static irs::zerobuf buf;
  static ostream mlog_obj(&buf);
  return mlog_obj;
}
