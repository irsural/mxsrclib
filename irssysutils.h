// ������� ���
// ����: 12.03.2010
// ������ ����: 17.09.2009

#ifndef irssysutilsH
#define irssysutilsH

#ifdef IRS_WIN32
#include <Rpc.h>
#else   // ������ ���������
#include <stdlib.h>
#endif  // ������ ���������

#include <irsstd.h>
#include <irsstrdefs.h>
#include <irserror.h>
#include <irscpp.h>

#undef min
#undef max

namespace irs {

// ��� �����
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

// ������������� ������� �������� ����� � �����
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
    // ��� ������������� � ���������� �������������
    ostr.rdbuf()->freeze(false);
  }
  return str_value;
}

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
      IRS_LIB_ASSERT("������������ �������� ���� ������������� �����");
    }
  }
  if (a_precision != num_precision_default) {
    ostr << setprecision(a_precision);
  } else {
    // ��������� �������� �� ���������  
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
      ::numeric_limits<typename type_relative_t<C>::signed_type>::min();
    int unsigned_char_max =
      ::numeric_limits<typename type_relative_t<C>::unsigned_type>::max();
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
    // ����� � ���������� ���������
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
    // ����� � ���������� ���������
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

// ����� ��� ���������, �������� � �������������� ���������� ���������������
class id_t
{
public:
  typedef vector<irs_u8> identificator;
  typedef size_t size_type;
  typedef irs::string string_type;
  id_t(): m_id()
  { }

  // � ������������ �������� ������ �������������� � ����� ������������ ��������
  id_t(const size_type a_byte_count): m_id(a_byte_count, 0)
  {
    generate(a_byte_count);
  }

  // ������� ��������� ������ ����������� �������� �������������� �
  // ��������� �������
  void generate(const size_type a_byte_count = id_byte_count_def)
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

  // ������� ����������� �������� ����� � ��������� ���
  string_type str() const
  {
    string_type id_str;
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
      // ��� ������������� � ���������� �������������
      ostr.rdbuf()->freeze(false);
    }
    return id_str;
  }

  // ������� �������� ������, ����������� �� � ����. ���������� ������, ����
  // ������� ��������������� ������ � ���� ��������� �������. � ������ �������
  // �������� �������������� �������� �������
  bool assign_str_no_resize(const string_type& a_id_str)
  {
    bool assign_success = true;
    const size_type id_str_size = a_id_str.size();
    const size_type need_elem_str_size = 2;
    if ((id_str_size / need_elem_str_size) == m_id.size()) {
      assign_success = assign_str(a_id_str);
    } else {
      assign_success = false;
    }
    return assign_success;
  }

  // ������� �������� ������, ����������� �� � ����. ���������� ������, ����
  // ������ ������� ��������������� � ����. � ������ ������� ��������
  // �������������� �������� �������
  bool assign_str(const string_type& a_id_str)
  {
    bool assign_success = true;
    const size_type id_str_size = a_id_str.size();
    const size_type need_elem_str_size = 2;
    string_type elem_str;
    identificator id(m_id.size(), 0);
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
        // ����������� ������ �� ���������
      }
    }
    if (assign_success) {
      m_id = id;
    } else {
      // ������������� ������ � ������������� �� �������
    }
    return assign_success;
  }

  /*const string_type& operator= (const string_type& a_id_str)
  {
    m_id.clear();
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
        m_id.push_back(static_cast<irs_u8>(elem_num));
        elem_str.clear();
      } else {
        // ����������� ������ �� ���������
      }
    }
    return a_id_str;
  }*/

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
  enum { id_byte_count_def = 8 };
  enum { num_base = 16 };
};

}; //namespace irs

#endif //irssysutilsH
