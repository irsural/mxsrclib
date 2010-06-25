// Утилиты ИРС
// Дата: 19.05.2010
// Ранняя дата: 17.09.2009

#ifndef irssysutilsH
#define irssysutilsH

#include <irsdefs.h>

#ifdef IRS_WIN32
#include <Rpc.h>
#else   // Прочие платформы
#include <stdlib.h>
#endif  // Прочие платформы

#include <irsstd.h>
#include <irsstrdefs.h>
#include <irserror.h>
#include <irscpp.h>
#include <irsstrconv.h>

#include <irsfinal.h>

namespace irs {

typedef irs_size_t sizens_t;

// Вид числа
enum {
  num_precision_default = -1
};

enum num_mode_t {
  num_mode_invalid,
  num_mode_general,
  num_mode_fixed,
  num_mode_scientific,
  num_mode_default = num_mode_general
};

enum num_base_t {
  num_base_invalid,
  num_base_dec,
  num_base_hex,
  num_base_oct,
  num_base_default = num_base_dec
};

#if IRS_LIB_VERSION_SUPPORT_LESS(390)
// Универсальная функция перевода чисел в текст
template<class T>
string number_to_str(
  const T a_num,
  const int a_precision = -1,
  const num_mode_t a_num_mode = num_mode_general)
{
  string str_value;
  ostrstream ostr;

  ostr << dec;

  switch (a_num_mode) {
    case num_mode_general: {
    } break;
    case num_mode_fixed: {
      ostr << fixed;
    } break;
    case num_mode_scientific: {
      ostr << scientific;
    } break;
  }

  const bool precision_bad_value = (a_precision <= 0);
  if (precision_bad_value) {
    ostr << setprecision(get_num_precision_def(a_num));
  } else {
    ostr << setprecision(a_precision);
  }

  ostr << a_num << ends;
  str_value = ostr.str();
  // Для совместимости с различными компиляторами
  ostr.rdbuf()->freeze(false);
  return str_value;
}
#endif // #if IRS_LIB_VERSION_SUPPORT_LESS(390)

#ifdef IRS_FULL_STDCPPLIB_SUPPORT

template<class T, class C>
void number_to_string(const T& a_num, basic_string<C>* ap_str,
  const int a_precision,
  const num_mode_t a_num_mode = num_mode_default,
  const locale& a_loc = irs::loc().get())
{
  basic_ostringstream<C> ostr;
  ostr.imbue(a_loc);
  ostr << dec;
  switch (a_num_mode) {
    case num_mode_general: {
    } break;
    case num_mode_fixed: {
      ostr << fixed;
    } break;
    case num_mode_scientific: {
      ostr << scientific;
    } break;
    default : {
      IRS_LIB_ASSERT("Недопустимое значение типа представления числа");
    }
  }

  const bool precision_bad_value = (a_precision <= 0);
  if (precision_bad_value) {
    ostr << setprecision(get_num_precision_def(a_num));
  } else {
    ostr << setprecision(a_precision);
  }

  if ((type_to_index<T>() == char_idx) ||
    (type_to_index<T>() == signed_char_idx) ||
    (type_to_index<T>() == unsigned_char_idx))
  {
    int val_int = static_cast<int>(a_num);
    ostr << val_int;
  } else {
    ostr << a_num;
  }
  *ap_str = ostr.str();
}

template<class T, class C>
void number_to_string(const T& a_num, basic_string<C>* ap_str,
  const locale& a_loc = irs::loc().get())
{
  number_to_string<T, C>(
    a_num,
    ap_str,
    num_precision_default,
    num_mode_general, a_loc
  );
}

template<class N, class S>
void num_to_str(const N& a_num, S* ap_str,
  const locale& a_loc = irs::loc().get())
{
  typename base_str_type<S>::type base_str;
  number_to_string(a_num, &base_str, a_loc);
  *ap_str = str_conv<S>(base_str);
}

template<class T, class C>
void number_to_string_classic(const T& a_num, basic_string<C>* ap_str)
{
  number_to_string(a_num, ap_str, locale::classic());
}

template<class N, class S>
void num_to_str_classic(const N& a_num, S* ap_str)
{
  typename base_str_type<S>::type base_str;
  number_to_string_classic(a_num, &base_str);
  *ap_str = str_conv<S>(base_str);
}

inline bool string_to_number_is_range_valid(int a_num)
{
  int signed_char_min = ::numeric_limits<signed char>::min();
  int unsigned_char_max = ::numeric_limits<unsigned char>::max();
  return (a_num >= signed_char_min) && (a_num <= unsigned_char_max );
}

template<class C, class T>
bool string_to_number(const basic_string<C>& a_str, T* ap_num,
  const locale& a_loc = irs::loc().get())
{
  bool convert_success = true;
  basic_istringstream<C> istr(a_str);
  istr.imbue(a_loc);
  if ((type_to_index<T>() == char_idx) ||
    (type_to_index<T>() == signed_char_idx) ||
    (type_to_index<T>() == unsigned_char_idx))
  {
    int val_int = 0;
    istr >> val_int;
    if (string_to_number_is_range_valid(val_int)) {
      *ap_num = static_cast<C>(val_int);
    } else {
      convert_success = false;
    } 
  } else {
    istr >> *ap_num;
  }
  if (!istr) {
    convert_success = false;
  } else {
    // Поток в нормальном состоянии
  }
  return convert_success;
}

template<class S, class N>
bool str_to_num(const S& a_str, N* ap_num,
  const locale& a_loc = irs::loc().get())
{
  typedef typename base_str_type<S>::type str_type;
  return string_to_number(str_conv<str_type>(a_str),
    ap_num, a_loc);
}

template<class C, class T>
bool string_to_number_classic(const basic_string<C>& a_str, T* ap_num)
{
  return string_to_number(a_str, ap_num, locale::classic());
}

template<class S, class N>
bool str_to_num_classic(const S& a_str, N* ap_num)
{
  typedef typename base_str_type<S>::type str_type;
  return string_to_number_classic(str_conv<str_type>(a_str),
    ap_num);
}

#else  // !IRS_FULL_STDCPPLIB_SUPPORT

template<class T>
void number_to_string(const T& a_num, string* ap_str)
{
  ostrstream ostr;
  ostr << setprecision(get_num_precision_def(a_num));
  if ((type_to_index<T>() == char_idx) ||
    (type_to_index<T>() == signed_char_idx) ||
    (type_to_index<T>() == unsigned_char_idx))
  {
    int val_int = a_num;
    ostr << val_int << irst('\0');
  } else {
    ostr << a_num << irst('\0');
  }
  *ap_str = ostr.str();
  ostr.rdbuf()->freeze(false);
}

template<class T>
void num_to_str(const T& a_num, string* ap_str)
{
  number_to_string(a_num, ap_str);
}

template<class T>
void number_to_string_classic(const T& a_num, string* ap_str)
{
  number_to_string(a_num, ap_str);
}

template<class T>
void num_to_str_classic(const T& a_num, string* ap_str)
{
  number_to_string_classic(a_num, ap_str);
}

template<class T>
bool string_to_number(const string& a_str, T* ap_num)
{
  bool convert_success = true;
  istrstream istr(a_str.c_str());
  if ((type_to_index<T>() == char_idx) ||
    (type_to_index<T>() == signed_char_idx) ||
    (type_to_index<T>() == unsigned_char_idx))
  {
    int val_int = 0;
    istr >> val_int;
    if ((val_int >= SCHAR_MIN) && (val_int <= UCHAR_MAX)) {
      *ap_num = static_cast<T>(val_int);
    } else {
      convert_success = false;
    }
  } else {
    istr >> *ap_num;
  }
  if (!istr) {
    convert_success = false;
  } else {
    // Поток в нормальном состоянии
  }
  return convert_success;
}

template<class T>
bool str_to_num(const string& a_str, T* ap_num)
{
  return string_to_number(a_str, ap_num);
}

template<class T>
bool string_to_number_classic(const string& a_str, T* ap_num)
{
  return string_to_number(a_str, ap_num);
}

template<class T>
bool str_to_num_classic(const string& a_str, T* ap_num)
{
  return string_to_number_classic(a_str, ap_num);
}

#endif // !IRS_FULL_STDCPPLIB_SUPPORT

inline bool get_closed_file_size(const irs::string& a_file_name, int& a_size)
{
  bool fsuccess = true;
  ifstream infile(a_file_name.c_str());
  if (!infile){
    fsuccess = false;
  } else {
    infile.seekg(0, ios::end);
    a_size = infile.tellg();
  }
  infile.close();
  return fsuccess;
}

// Класс для генерации, хранения и преобразования уникальных идентификаторов
class id_t
{
public:
  typedef vector<irs_u8> identifier_type;
  typedef size_t size_type;
  typedef irs::string string_type;
  id_t(): m_identifier()
  { }

  // В конструкторе задается размер идентификатора и сразу генерируется значение
  id_t(const size_type a_byte_count):
    m_identifier(new_id(a_byte_count))
  {
  }
private:
  void generate(
    const size_type a_byte_count,
    identifier_type* ap_identifier) const
  {
    ap_identifier->clear();
    ap_identifier->reserve(a_byte_count);
    #ifdef IRS_WIN32
    UUID id;
    const size_type id_size = sizeof(id);
    const size_type id_count = (a_byte_count / id_size) + 1;
    for (size_type id_i = 0; id_i < id_count; id_i++) {
      UuidCreate(&id);
      for (size_type byte_i = 0; byte_i < id_size; byte_i++) {
        if (ap_identifier->size() >= a_byte_count) {
          break;
        } else {
          irs_u8 id_element = *(reinterpret_cast<irs_u8*>(&id) + byte_i);
          ap_identifier->push_back(id_element);
        }
      }
    }
    #else
    //randomize();
    for (size_type byte_i = 0; byte_i < a_byte_count; byte_i++) {
      int rand_val = rand();
      ap_identifier->push_back(IRS_LOBYTE(rand_val));
    }
    #endif
  }

  identifier_type new_id(
    const size_type a_byte_count = id_byte_count_def) const
  {
    identifier_type id;
    generate(a_byte_count, &id);
    return id;
  }
  
public:
  // Функция генерации нового уникального значения идентификатора с
  // указанием размера
  void generate(const size_type a_byte_count = id_byte_count_def)
  {
    generate(a_byte_count, &m_identifier);
  }

  // Функция преобразует значение ключа в строковый тип
  string_type str() const
  {
    string_type id_str;
    size_type id_size = m_identifier.size();
    id_str.reserve(id_size);
    for (size_type id_elem_i = 0; id_elem_i < id_size; id_elem_i++) {
      ostrstream ostr;
      size_type num = m_identifier[id_elem_i];
      const size_type width = 2;
      char symbol_fill = '0';
      ostr << right << setw(width) << setfill(symbol_fill) <<
        setbase(num_base) << num << ends;
      id_str += ostr.str();
      // Для совместимости с различными компиляторами
      ostr.rdbuf()->freeze(false);
    }
    return id_str;
  }

  // Функция получает строку, преобразует ее в ключ. Возвращает истину, если
  // удалось сконвертировать строку в ключ заданного размера. В случае неудачи
  // значение идентификатора остается прежним
  bool assign_str_no_resize(const string_type& a_id_str)
  {
    bool assign_success = true;
    const size_type id_str_size = a_id_str.size();
    const size_type need_elem_str_size = 2;
    if ((id_str_size / need_elem_str_size) == m_identifier.size()) {
      assign_success = assign_str(a_id_str);
    } else {
      assign_success = false;
    }
    return assign_success;
  }

  // Функция получает строку, преобразует ее в ключ. Возвращает истину, если
  // строку удалось сконвертировать в ключ. В случае неудачи значение
  // идентификатора остается прежним
  bool assign_str(const string_type& a_id_str)
  {
    bool assign_success = true;
    const size_type id_str_size = a_id_str.size();
    const size_type need_elem_str_size = 2;
    string_type elem_str;
    identifier_type id;
    id.reserve(id_str_size / need_elem_str_size);
    for (size_type id_str_index = 0; id_str_index < id_str_size;
      id_str_index++)
    {
      elem_str += a_id_str[id_str_index];
      if (elem_str.size() == need_elem_str_size) {
        int elem_num = 0;
        istrstream istr(const_cast<char*>(elem_str.c_str()));
        istr >> setbase(num_base) >> elem_num;
        if (istr) {
          id.push_back(static_cast<irs_u8>(elem_num));
        } else {
          assign_success = false;
          break;
        }
        elem_str.clear();
      } else {
        // Необходимый размер не достигнут
      }
    }
    if (assign_success) {
      m_identifier = id;
    } else {
      // Преобразовать строку в идентификатор не удалось
    }
    return assign_success;
  }

  /*const string_type& operator= (const string_type& a_id_str)
  {
    m_identifier.clear();
    size_type id_str_size = a_id_str.size();
    const size_type need_elem_str_size = 2;
    string_type elem_str;
    for (size_type id_str_index = 0; id_str_index < id_str_size;
      id_str_index++)
    {
      elem_str += a_id_str[id_str_index];
      if (elem_str.size() == need_elem_str_size) {
        int elem_num = 0;
        istrstream istr(const_cast<char*>(elem_str.c_str()));
        istr >> setbase(num_base) >> elem_num;
        m_identifier.push_back(static_cast<irs_u8>(elem_num));
        elem_str.clear();
      } else {
        // Необходимый размер не достигнут
      }
    }
    return a_id_str;
  }*/

  bool operator== (const id_t& a_id) const
  {
    return (m_identifier == a_id.m_identifier);
  }

  bool operator!= (const id_t& a_id) const
  {
    return (m_identifier != a_id.m_identifier);
  }

  bool operator< (const id_t& a_id) const
  {
    bool less_than_value = true;
    if (m_identifier.size() != a_id.m_identifier.size()) {
      less_than_value = (m_identifier.size() < a_id.m_identifier.size());
    } else {
      less_than_value = false;
      for (size_type elem_i = 0; elem_i < m_identifier.size(); elem_i++) {
        if (m_identifier[elem_i] != a_id.m_identifier[elem_i]) {
          less_than_value = (m_identifier[elem_i] < a_id.m_identifier[elem_i]);
          break;
        } else {
          // Продолжаем сравнение
        }
      }
    }
    return less_than_value;
  }

private:
  identifier_type m_identifier;
  enum { id_byte_count_def = 8 };
  enum { num_base = 16 };
};

// В переданном имени файла изменяется расширение

#ifndef __embedded_cplusplus
template<class T>
void change_file_ext(
  const basic_string<T>& a_ext,
  basic_string<T>* ap_file_name)
{
  typedef irs_size_t size_type;
  size_type pos = ap_file_name->find_last_of(irst("."));
  if (pos != basic_string<T>::npos) {
    ap_file_name->replace(pos + 1, basic_string<T>::npos, a_ext);
  } else {
    ap_file_name->append(a_ext);
  }    
}

template<class T>
basic_string<T> get_file_ext(const basic_string<T>& a_file_name)
{
  typedef irs_size_t size_type;
  basic_string<T> file_ext;
  size_type pos = a_file_name.find_last_of(irst("."));
  if (pos != basic_string<T>::npos) {
    file_ext = a_file_name.substr(pos + 1);
  } else {
    // Возращаем пустую строку
  }
  return file_ext;
}

template<class T>
basic_string<T> get_file_name(const basic_string<T>& a_file_name)
{
  typedef irs_size_t size_type;
  basic_string<T> file_name;
  size_type pos = a_file_name.find_last_of(irst("\\/"));
  if (pos != basic_string<T>::npos) {
    file_name = a_file_name.substr(pos + 1);
  } else {
    // Возращаем пустую строку
  }
  return file_name;
}

template<class T>
basic_string<T> get_file_dir(const basic_string<T>& a_file_name)
{
  typedef irs_size_t size_type;
  basic_string<T> file_dir;
  size_type pos = a_file_name.find_last_of(irst("\\/"));
  if (pos != basic_string<T>::npos) {
    file_dir = a_file_name.substr(0, pos);
  } else {
    // Возращаем пустую строку
  }
  return file_dir;
}
#endif // !__embedded_cplusplus

} //namespace irs

#endif //irssysutilsH
