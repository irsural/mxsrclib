// Библиотека исключений ИРС
// Дата: 9.06.2008

#include "irsexcept.h"

// Базовый класс для исключений-утверждений
irs::assert_e::assert_e(const char *ap_assert_str, const char *ap_file_name,
  int a_line_number, const char *ap_message):
  m_user_message(ap_assert_str != 0),
  m_full_message()
{
  if (m_user_message) {
    ostrstream m_full_message_stream;
    m_full_message_stream << "Утверждение не верно: " << ap_assert_str
      << endl;
    m_full_message_stream << "Файл: " << ap_file_name << endl;
    m_full_message_stream << "Строка: " << a_line_number << endl;
    m_full_message_stream << "Сообщение: " << ap_message << endl;
    m_full_message.
      assign(m_full_message_stream.str(), m_full_message_stream.pcount());
    m_full_message_stream.rdbuf()->freeze(false);
  }
}
const char *irs::assert_e::what() const IRS_THROW(())
{
  if (m_user_message) {
    return m_full_message.c_str();
  } else {
    return "assert_e";
  }
}

