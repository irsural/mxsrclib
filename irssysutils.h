//! \file
//! \ingroup system_utils_group
//! \brief Утилиты ИРС
//!
//! Дата: 07.07.2010\n
//! Ранняя дата: 17.09.2009

#ifndef irssysutilsH
#define irssysutilsH

#include <irsdefs.h>

#ifdef IRS_WIN32
#include <Rpc.h>
#if IRS_USE_HID_WIN_API
# include <Hidsdi++.h>
# include <Setupapi.h>
#endif // IRS_USE_HID_WIN_API
#else   // Прочие платформы
#include <stdlib.h>
#endif  // Прочие платформы

#if IRS_USE_BOOST
//#define BOOST_SYSTEM_NO_DEPRECATED
# include <boost/version.hpp>
#endif // IRS_USE_BOOST

#include <irsstd.h>
#include <irsstrdefs.h>
#include <irserror.h>
#include <irscpp.h>
#include <irsstrconv.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup system_utils_group
//! @{

typedef irs_size_t sizens_t;

// Вид числа
enum {
  num_precision_default = -1
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

template<class N>
irs::string_t num_to_str(const N& a_num, const locale& a_loc = irs::loc().get())
{
  irs::string_t base_str;
  number_to_string(a_num, &base_str, a_loc);
  return base_str;
}

template<class N>
irs::string_t num_to_str_classic(const N& a_num)
{
  irs::string_t base_str;
  num_to_str_classic(a_num, &base_str);
  return base_str;
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
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  S str = str_conv<S>(base_str);
  const S::value_type dot_ch = '.';
  const S dot_str = dot_ch;
  const S::value_type comma_ch = ',';
  S::size_type pos = str.find(comma_ch);
  while (pos != S::npos) {
    str.replace(pos, 1, dot_str);
    pos = str.find(comma_ch, pos + 1);
  }
  //str.find
  *ap_str = str;
  #else // !(defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  *ap_str = str_conv<S>(base_str);
  #endif
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
    int val_int = static_cast<int>(a_num);
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
  istrstream istr(const_cast<char *>(a_str.c_str()));
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

#if defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)

// Класс для генерации, хранения и преобразования уникальных идентификаторов
class id_t
{
public:
  typedef vector<irs_u8> identifier_type;
  typedef size_t size_type;
  typedef irs::char_t char_type;
  typedef irs::string_t string_type;
  typedef irs::ostringstream_t ostringstream_type;
  typedef irs::istringstream_t istringstream_type;
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
      ostringstream_type ostr;
      size_type num = m_identifier[id_elem_i];
      const size_type width = 2;
      char_type symbol_fill = irst('0');
      ostr << right << setw(width) << setfill(symbol_fill) <<
        setbase(num_base) << num;
      id_str += ostr.str();
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
        istringstream_type istr(elem_str.c_str());
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
    bool less_than_value_result = true;
    if (m_identifier.size() != a_id.m_identifier.size()) {
      less_than_value_result = (m_identifier.size() < a_id.m_identifier.size());
    } else {
      less_than_value_result = false;
      for (size_type elem_i = 0; elem_i < m_identifier.size(); elem_i++) {
        if (m_identifier[elem_i] != a_id.m_identifier[elem_i]) {
          less_than_value_result = 
            (m_identifier[elem_i] < a_id.m_identifier[elem_i]);
          break;
        } else {
          // Продолжаем сравнение
        }
      }
    }
    return less_than_value_result;
  }

private:
  identifier_type m_identifier;
  enum { id_byte_count_def = 8 };
  enum { num_base = 16 };
};

// В переданном имени файла изменяется расширение
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
    file_name = a_file_name;
  }
  return file_name;
}

template<class T>
basic_string<T> get_ultra_short_file_name(const basic_string<T>& a_file_name)
{
  typedef irs_size_t size_type;
  basic_string<T> file_name = irs::get_file_name(a_file_name);
  size_type pos = file_name.find_last_of(irst("."));
  if (pos != basic_string<T>::npos) {
    file_name = file_name.substr(0, pos);
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
#endif // defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)

// Округление до ближайшего целого
template <class I, class O>
void round(const I& a_input, O* ap_output)
{
  *ap_output = static_cast<O>(floor(a_input + 0.5));
}
template <class I, class O>
O round(const I& a_input, O)
{
  O output = O();
  round(a_input, &output);
  return output;
}
template <class I, class O>
O round(const I& a_input)
{
  O output = O();
  round(a_input, &output);
  return output;
}
//stringns_t to_lower(const stringns_t& a_str,
  //const locale& a_loc = irs::loc().get());

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
//! \brief Преобразует символы строки в верхний регистр
template <class T>
inline T to_upper(const T& a_str, const locale& a_loc = loc().get())
{
  T str = a_str;
  for (sizens_t i = 0; i < str.size(); i++) {
    str[i] = toupper(str[i], a_loc);
  }
  return str;
}

//! \brief Преобразует символы строки в нижний регистр
template <class T>
inline T to_lower(const T& a_str, const locale& a_loc = loc().get())
{
  T str = a_str;
  for (sizens_t i = 0; i < str.size(); i++) {
    str[i] = tolower(str[i], a_loc);
  }
  return str;
}
#else // !IRS_FULL_STDCPPLIB_SUPPORT
//! \brief Преобразует символы строки в верхний регистр
inline irs_string_t to_upper(const irs_string_t& a_str)
{
  irs_string_t str = a_str;
  for (sizens_t i = 0; i < str.size(); i++) {
    str[i] = toupper(str[i]);
  }
  return str;
}

//! \brief Преобразует символы строки в нижний регистр
inline irs_string_t to_lower(const irs_string_t& a_str)
{
  irs_string_t str = a_str;
  for (sizens_t i = 0; i < str.size(); i++) {
    str[i] = tolower(str[i]);
  }
  return str;
}
#endif // !IRS_FULL_STDCPPLIB_SUPPORT

IRS_STRING_TEMPLATE
inline bool cstr_to_bool(
  const IRS_STRING_CHAR_TYPE* a_string,
  bool& a_value)
{
  bool result = false;
  IRS_STRING_TYPE_SPEC string_value = a_string;
  string_value = to_lower(string_value);
  if ((string_value == "true") || (string_value == "истина")) {
    a_value = true;
    result = true;
  } else if ((string_value == "false") || (string_value == "ложь")) {
    a_value = false;
    result = true;
  }
  return result;
}

#if defined(IRS_FULL_STDCPPLIB_SUPPORT) && defined(IRS_WIN32)
struct wave_format_t
{
  int format_tag;           // Метка фомата
  int channels;             // Число каналов
  int bits;                 // Разрядность канала в битах
  int sample_size;          // Размер отсчета в байтах (для ИКМ /PCM/)
  std::size_t frecuency;    // Частота дискретизации в Гц (отсчет/с)
  std::size_t speed;        // Скорость данных в байт/с
  std::size_t size;         // Размер данных в байтах
  std::size_t pos;          // Начало данных
};

bool get_wave_format(irs::string_t a_file_name, wave_format_t* ap_wave_format);

bool get_pcm_wave_data(irs::string_t a_file_name,
  const wave_format_t& a_wave_format, std::size_t a_channel,
  std::vector<irs_i16>* ap_samples);

enum wave_pcm_sampling_rate_t {
  wave_pcm_sr_8_khz = 8000,
  wave_pcm_sr_11_025_khz = 11025,
  wave_pcm_sr_22_05_khz = 22050,
  wave_pcm_sr_44_1_khz = 44100
};

void create_wave_pcm_16_mono_file(irs::string_t a_file_name,
  const std::size_t a_sampling_rate,
  const std::vector<irs_i16>& a_samples);

#if IRS_USE_HID_WIN_API
struct usb_hid_device_attributes_t
{
  typedef irs::string_t string_type;
  irs_u16 vendor_id;
  irs_u16 product_id;
  irs_u16 version_number;
  string_type manufacturer;
  string_type product;
  string_type serial_number;
  usb_hid_device_attributes_t():
    vendor_id(0),
    product_id(0),
    version_number(0),
    manufacturer(),
    product(),
    serial_number()
  {
  }
};

struct usb_hid_device_info_t
{
  typedef irs::string_t string_type;
  string_type path;
  usb_hid_device_attributes_t attributes;
  usb_hid_device_info_t():
    path(),
    attributes()
  {
  }
};

class usb_hid_info_t
{
public:
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;
  ~usb_hid_info_t();
  static usb_hid_info_t *get_instance();
  std::vector<string_type> get_devices() const;
  usb_hid_device_attributes_t get_attributes(const string_type& a_path) const;
  std::vector<usb_hid_device_info_t> get_devices_info() const;
private:
  usb_hid_info_t();
  typedef BOOLEAN (__stdcall *PHidD_GetProductString)(HANDLE, PVOID, ULONG);
  typedef VOID (__stdcall *PHidD_GetHidGuid)(LPGUID);
  typedef BOOLEAN (__stdcall *PHidD_GetAttributes)(HANDLE, PHIDD_ATTRIBUTES);
  typedef BOOLEAN (__stdcall *PHidD_GetNumInputBuffers)(HANDLE, PULONG);
  typedef BOOLEAN (__stdcall *PHidD_SetFeature)(HANDLE, PVOID, ULONG);
  typedef BOOLEAN (__stdcall *PHidD_GetFeature)(HANDLE, PVOID, ULONG);
  typedef BOOLEAN (__stdcall *PHidD_GetInputReport)(HANDLE, PVOID, ULONG);
  typedef BOOLEAN (__stdcall *PHidD_SetOutputReport)(HANDLE, PVOID, ULONG);

  typedef BOOLEAN (__stdcall *PHidD_GetManufacturerString)
    (HANDLE, PVOID, ULONG);
  typedef BOOLEAN (__stdcall *PHidD_GetSerialNumberString)
    (HANDLE, PVOID, ULONG);
  typedef BOOLEAN (__stdcall *PHidD_GetPreparsedData)(HANDLE,
    PHIDP_PREPARSED_DATA*);
  typedef BOOLEAN (__stdcall *PHidD_FreePreparsedData)(PHIDP_PREPARSED_DATA);
  typedef NTSTATUS (__stdcall *PHidP_GetCaps)(PHIDP_PREPARSED_DATA,
    PHIDP_CAPS Capabilities);

  HINSTANCE m_hid_dll;
  PHidD_GetManufacturerString HidD_GetManufacturerString;
  PHidD_GetProductString HidD_GetProductString;
  PHidD_GetSerialNumberString HidD_GetSerialNumberString;
  PHidD_GetPreparsedData HidD_GetPreparsedData;
  PHidD_FreePreparsedData HidD_FreePreparsedData;
  PHidD_GetHidGuid HidD_GetHidGuid;
  PHidD_GetAttributes HidD_GetAttributes;
  PHidD_GetNumInputBuffers HidD_GetNumInputBuffers;
  PHidD_SetFeature HidD_SetFeature;
  PHidD_GetFeature HidD_GetFeature;
  PHidD_GetInputReport HidD_GetInputReport;
  PHidD_SetOutputReport HidD_SetOutputReport;
  PHidP_GetCaps HidP_GetCaps;
};
#endif // IRS_USE_HID_WIN_API

#endif // defined(IRS_FULL_STDCPPLIB_SUPPORT) && defined(IRS_WIN32)

#if defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)

//! \brief Конвертирует бинарные данные в строку шестнадцатиричных символов
//! \param[in] ap_buf - массив данных
//! \param[in] a_buf_size - размер массива данных
//! \param[out] ap_str - строка любого типа, для которой определена функция
//!   irs::str_conv
//! \see hex_str_to_binary_data
template<class T>
void binary_data_to_hex_str(const irs_u8* ap_buf, std::size_t a_buf_size,
  T* ap_str)
{
  irs::string_t s;
  binary_data_to_hex_str(ap_buf, a_buf_size, &s);
  *ap_str = irs::str_conv<T>(s);
}

//! \brief Конвертирует бинарные данные в строку шестнадцатиричных символов
//! \param[in] ap_buf - массив данных
//! \param[in] a_buf_size - размер массива данных
//! \param[out] ap_str - строка
//! \see hex_str_to_binary_data
void binary_data_to_hex_str(const irs_u8* ap_buf, std::size_t a_buf_size,
  irs::string_t* ap_str);

//! \brief Конвертирует строку шестнадцатиричных символов в бинарные данные
//! \param[in] a_str - строка любого типа, для которой определена функция
//!   irs::str_conv
//! \param[in] ap_vector - объект, в который будут скопированы бинарные данные.
//!   Объект может быть любого типа, у которого реализованы функции size и
//!   operator[], например std::vector или irs::raw_data_t
//! \see binary_data_to_hex_str
template<class T, class VECTOR>
bool hex_str_to_binary_data(const T& a_str,
  VECTOR* ap_vector)
{
  const std::string s = irs::str_conv<T>(a_str);
  return hex_str_to_binary_data(s, ap_vector);
}

//! \brief Конвертирует строку шестнадцатиричных символов в бинарные данные
//! \param[in] a_str - строка символов
//! \param[in] ap_vector - объект, в который будут скопированы бинарные данные.
//!   Объект может быть любого типа, у которого реализованы функции size и
//!   operator[], например std::vector или irs::raw_data_t
//! \see binary_data_to_hex_str
template<class ARRAY>
bool hex_str_to_binary_data(const irs::string_t& a_str,
  ARRAY* ap_vector)
{
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;
  const int hex_data_map[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15
  };

  const size_type str_elem_size = 2;
  bool convert_success = true;
  if (a_str.size() % str_elem_size == 0) {
    ap_vector->resize(a_str.size() / str_elem_size);
    const size_type str_size = a_str.size();

    for (size_type elem_i = 0, data_i = 0; elem_i < str_size;
      elem_i += 2, data_i++) {
      if ((a_str[elem_i] > irst('f')) || (a_str[elem_i + 1] > irst('f'))) {
        convert_success = false;
        break;
      }
      const int first_part = hex_data_map[static_cast<unsigned char>(a_str[elem_i])];
      const int second_part = hex_data_map[static_cast<unsigned char>(a_str[elem_i + 1])];
      if ((first_part == -1) || (second_part == -1)) {
        convert_success = false;
        break;
      }
      (*ap_vector)[data_i] = static_cast<irs_u8>(
        (first_part << 4) + second_part);
    }
  } else {
    convert_success = false;
  }
  return convert_success;
}

template<class T>
void print_binary_data_to_str(
  const irs_u8* ap_buf, std::size_t a_buf_size, T* ap_str) {
  irs::string_t msg;
  irs::binary_data_to_hex_str(ap_buf, a_buf_size, &msg);
  irs::ostringstream_t ostr;
  std::size_t width = std::min(static_cast<std::size_t>(16), msg.size()/2);
  ostr << setw(5) << irst(" ");
  for (std::size_t i = 0; i < width; i++) {
    ostr << std::hex << setw(2) << i;
    if (i < (width - 1)) {
      ostr << irst(" ");
    }
  }
  ostr << std::dec << std::endl;
  std::size_t elem = 0;
  std::size_t col = 0;
  std::size_t row = 0;
  irs::ostringstream_t text;
  for (std::size_t i = 0; i < msg.size(); i++) {
    if ((elem == 0) && (col == 0)) {
      ostr << setw(4) << std::hex << row*0x10 << std::dec << irst(" ");
    }
    ostr << msg[i];
    if ((elem >= 1) && (col < width)) {
      const irs::char_t ch = ap_buf[col];
      if (irs::isprintt(ch)) {
        text << ap_buf[i/2];
      } else {
        text << irst(".");
      }
    }
    if ((elem >= 1) && (col < (width - 1))) {
      ostr << irst(" ");
    }
    if ((elem >= 1) && (i < (msg.size() - 1)) && (col == (width - 1))) {
      row++;
      col = 0;
      elem = 0;
      ostr << irst("|") << text.str() << std::endl;
      text.str(irst(""));
      text.clear();
    } else if (elem >= 1) {
      elem = 0;
      col++;
    } else {
      elem++;
    }
  }
  *ap_str = irs::str_conv<T>(ostr.str());
}

template <class T>
irs::string_t value_to_bin_str(const T& a_value)
{
  irs::string_t str;
  const size_t bit_count = 8*sizeof(a_value);

  size_t bit_pos = bit_count;
  do {
    bit_pos--;
    const bool bit = a_value & (1 << bit_pos);
    str += bit ? irst("1") : irst("0");

    if ((bit_pos != 0) && (bit_pos%4 == 0)) {
      str += irst(" ");
    }
  } while (bit_pos > 0);
  return str;
}

//! \brief Ищет пару в словаре, содержащую указанное значение, и возвращает
//!   итератор для этой пары
//! \param[in] a_begin - начальная позиция
//! \param[in] a_end - конечная позиция, не входит в диапазон поиска
//! \param[in] a_value - значение, которое должна содержать па
//! \return итератор найденной пары
template <class map_iterator_t, class value_t>
map_iterator_t find_value(map_iterator_t a_begin, map_iterator_t a_end,
  const value_t& a_value)
{
  while (a_begin != a_end) {
    if (a_begin->second == a_value) {
      break;
    }
    ++a_begin;
  }
  return a_begin;
}

#endif // defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)

#if IRS_USE_BOOST
#if ((BOOST_VERSION < 104600) || (BOOST_VERSION >= 106000))

string_t absolute_path_to_relative(const irs::string_t& a_path,
  const irs::string_t& a_base);

string_t relative_path_to_absolute(const irs::string_t& a_path,
  const irs::string_t& a_base);

#endif // (BOOST_VERSION < 104600) || (BOOST_VERSION >= 106600)
#endif // IRS_USE_BOOST

string_t default_ini_name();

//! @}

} //namespace irs

#endif //irssysutilsH
