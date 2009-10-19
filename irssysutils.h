// Утилиты ИРС
// Дата: 17.09.2009

#ifndef irssysutilsH
#define irssysutilsH

#include <irsstd.h>
#include <irsstrdefs.h>
#include <irscpp.h>

#ifdef IRS_WIN32
#include <Rpc.h>
#else   // Прочие платформы
#include <stdlib.h>
#endif  // Прочие платформы

namespace irs {
// Вид числа
enum num_mode_t{num_mode_general, num_mode_fixed, num_mode_scientific};
enum num_base_t {num_base_dec, num_base_hex, num_base_oct};

// Универсальная функция перевода чисел в текст
template<class T>
string number_to_str(
  const T a_num,
  const int a_precision = -1,
  const num_mode_t a_num_mode = num_mode_general)
{
  bool precision_bad_value =
    (a_precision < 0) || ((a_precision == 0) &&
    (a_num_mode == num_mode_general));

  string str_value;

  if (precision_bad_value) {
    str_value = a_num;
  } else {
    ostrstream ostr;
    int precision = a_precision;
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

    ostr << setprecision(precision);
    ostr << a_num << ends;
    str_value = ostr.str();
    // Для совместимости с различными компиляторами
    ostr.rdbuf()->freeze(false);
  }
  return str_value;
}

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
template<class T, class C>
void number_to_string(const T& a_num, basic_string<C>* ap_str,
  const locale& a_loc = irs::loc().get())
{
  basic_ostringstream<C> ostr;
  ostr.imbue(a_loc);
  if ((type_to_index<T>() == char_idx) ||
    (type_to_index<T>() == signed_char_idx) ||
    (type_to_index<T>() == unsigned_char_idx))
  {
    int val_int = static_cast<int>(a_num);
    ostr << val_int << irst('\0');
  } else {
    ostr << a_num << irst('\0');
  }
  *ap_str = ostr.str();
}

template<class T, class C>
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
    int signed_char_min =
      std::numeric_limits<typename type_relative_t<C>::signed_type>::min();
    int unsigned_char_max =
      std::numeric_limits<typename type_relative_t<C>::unsigned_type>::max();
    if ((val_int >= signed_char_min) && (val_int <= unsigned_char_max )) {
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

#else  // IRS_FULL_STDCPPLIB_SUPPORT

template<class T>
void number_to_string(const T& a_num, string* ap_str)
{
  ostrstream ostr;
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

#endif // IRS_FULL_STDCPPLIB_SUPPORT

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
  typedef vector<irs_u8> identificator;
  typedef size_t size_type;
  typedef irs::string str_type;
  id_t(): m_id()
  { }
  id_t(const size_type a_byte_count): m_id()
  {
    generate(a_byte_count);
  }
  void generate(const size_type a_byte_count)
  {
    m_id.clear();
    m_id.reserve(a_byte_count);
    #ifdef IRS_WIN32
    UUID id;
    const size_type id_size = sizeof(id);
    const size_type id_count = (a_byte_count / id_size) + 1;
    for (size_type id_i = 0; id_i < id_count; id_i++) {
      UuidCreate(&id);
      for (size_type byte_i = 0; byte_i < id_size; byte_i++) {
        if (m_id.size() >= a_byte_count) {
          break;
        } else {
          irs_u8 id_element = *(reinterpret_cast<irs_u8*>(&id)+byte_i);
          m_id.push_back(id_element);
        }
      }
    }
    #else
    //randomize();
    for (size_type byte_i = 0; byte_i < a_byte_count; byte_i++) {
      int rand_val = rand();
      m_id.push_back(IRS_LOBYTE(rand_val));
    }
    #endif
  }
  str_type str() const
  {
    str_type id_str;
    size_type id_size = m_id.size();
    id_str.reserve(id_size);
    for (size_type id_elem_i = 0; id_elem_i < id_size; id_elem_i++) {
      ostrstream ostr;
      size_type num = m_id[id_elem_i];
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
  const str_type& operator= (const str_type& a_id_str)
  {
    m_id.clear();
    size_type id_str_size = a_id_str.size();
    const size_type need_elem_str_size = 2;
    str_type elem_str;
    for (size_type id_str_index = 0; id_str_index < id_str_size;
      id_str_index++)
    {
      elem_str += a_id_str[id_str_index];
      if (elem_str.size() == need_elem_str_size) {
        int elem_num = 0;
        istrstream istr(const_cast<char*>(elem_str.c_str()));
        istr >> setbase(num_base) >> elem_num;
        m_id.push_back(static_cast<irs_u8>(elem_num));
        elem_str.clear();
      } else {
        // Необходимый размер не достигнут
      }
    }
    return a_id_str;
  }
  bool operator== (const id_t& a_id) const
  {
    return (m_id == a_id.m_id);
  }
  bool operator!= (const id_t& a_id) const
  {
    return (m_id != a_id.m_id);
  }
private:
  identificator m_id;
  enum {num_base = 16};
};

}; //namespace irs

//---------------------------------------------------------------------------
#endif
