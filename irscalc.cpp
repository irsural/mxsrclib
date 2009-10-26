// Калькулятор
// Дата: 01.10.2008
// Ранняя дата: 24.09.2009

// В Watcom C++ есть только не-const версия функции map::find
#ifndef __WATCOMC__

// В GNU C++ версии >= 3.4 строки char* имеют кодировку UTF-8
// Русские символы в этой кодировке 'Я' имеют размер 2 байта, что недопустимо
// по стандарту C++
//#if defined(IRS_UNICODE) || defined(IRS_GNUC_VERSION_LESS_3_4)

#include <irscalc.h>

irs::string irs::preprocessing_str(const irs::string& a_str)
{
  irs::string str = a_str;
  //Удаляем пробелы
  /*int pos_ch_space = str.find(' ');
  while (pos_ch_space != irs::string::npos) {
    str.erase(pos_ch_space, 1);
    pos_ch_space = str.find(' ');
  }*/
  // Удаляем переход на новую строку
  int pos_ch_return = str.find("\r");
  while (pos_ch_return != irs::string::npos) {
    str.erase(pos_ch_return, 2);
    pos_ch_return = str.find("\r");
  }
  // Удаляем табуляции
  int pos_ch_tab = str.find("\t");
  while (pos_ch_tab != irs::string::npos) {
    str.erase(pos_ch_tab, 2);
    pos_ch_tab = str.find("\t");
  }
  // Заменяем запятые на точки
  /*int pos_ch_decimal_point = str.find(",");
  while (pos_ch_decimal_point != irs::string::npos) {
    str.replace(pos_ch_decimal_point, 1, ".");
    pos_ch_decimal_point = str.find(",");
  }*/
  return str;
}

void irs::calc::create_keyword_map(keyword_map_type* ap_keyword_map)
{
  ap_keyword_map->clear();
  (*ap_keyword_map)[irst("and")] = keyword_and;
  (*ap_keyword_map)[irst("and_eq")] = keyword_and_eq;
  (*ap_keyword_map)[irst("asm")] = keyword_asm;
  (*ap_keyword_map)[irst("auto")] = keyword_auto;
  (*ap_keyword_map)[irst("bitand")] = keyword_bitand;
  (*ap_keyword_map)[irst("bitor")] = keyword_bitor;
  (*ap_keyword_map)[irst("bool")] = keyword_bool;
  (*ap_keyword_map)[irst("break")] = keyword_break;
  (*ap_keyword_map)[irst("case")] = keyword_case;
  (*ap_keyword_map)[irst("catch")] = keyword_catch;
  (*ap_keyword_map)[irst("char")] = keyword_char;
  (*ap_keyword_map)[irst("class")] = keyword_class;
  (*ap_keyword_map)[irst("compl")] = keyword_compl;
  (*ap_keyword_map)[irst("const")] = keyword_const;
  (*ap_keyword_map)[irst("const_cast")] = keyword_const_cast;
  (*ap_keyword_map)[irst("continue")] = keyword_continue;
  (*ap_keyword_map)[irst("default")] = keyword_default;
  (*ap_keyword_map)[irst("delete")] = keyword_delete;
  (*ap_keyword_map)[irst("do")] = keyword_do;
  (*ap_keyword_map)[irst("double")] = keyword_double;
  (*ap_keyword_map)[irst("dynamic_cast")] = keyword_dynamic_cast;
  (*ap_keyword_map)[irst("else")] = keyword_else;
  (*ap_keyword_map)[irst("enum")] = keyword_enum;
  (*ap_keyword_map)[irst("explicit")] = keyword_explicit;
  (*ap_keyword_map)[irst("export")] = keyword_export;
  (*ap_keyword_map)[irst("extern")] = keyword_extern;
  (*ap_keyword_map)[irst("false")] = keyword_false;
  (*ap_keyword_map)[irst("float")] = keyword_float;
  (*ap_keyword_map)[irst("for")] = keyword_for;
  (*ap_keyword_map)[irst("friend")] = keyword_friend;
  (*ap_keyword_map)[irst("goto")] = keyword_goto;
  (*ap_keyword_map)[irst("if")] = keyword_if;
  (*ap_keyword_map)[irst("inline")] = keyword_inline;
  (*ap_keyword_map)[irst("int")] = keyword_int;
  (*ap_keyword_map)[irst("long")] = keyword_long;
  (*ap_keyword_map)[irst("mutable")] = keyword_mutable;
  (*ap_keyword_map)[irst("namespace")] = keyword_namespace;
  (*ap_keyword_map)[irst("new")] = keyword_new;
  (*ap_keyword_map)[irst("not")] = keyword_not;
  (*ap_keyword_map)[irst("not_eq")] = keyword_not_eq;
  (*ap_keyword_map)[irst("operator")] = keyword_operator;
  (*ap_keyword_map)[irst("or")] = keyword_or;
  (*ap_keyword_map)[irst("or_eq")] = keyword_or_eq;
  (*ap_keyword_map)[irst("private")] = keyword_private;
  (*ap_keyword_map)[irst("protected")] = keyword_protected;
  (*ap_keyword_map)[irst("public")] = keyword_public;
  (*ap_keyword_map)[irst("register")] = keyword_register;
  (*ap_keyword_map)[irst("reinterpret_cast")] = keyword_reinterpret_cast;
  (*ap_keyword_map)[irst("return")] = keyword_return;
  (*ap_keyword_map)[irst("short")] = keyword_short;
  (*ap_keyword_map)[irst("signed")] = keyword_signed;
  (*ap_keyword_map)[irst("sizeof")] = keyword_sizeof;
  (*ap_keyword_map)[irst("static")] = keyword_static;
  (*ap_keyword_map)[irst("static_cast")] = keyword_static_cast;
  (*ap_keyword_map)[irst("struct")] = keyword_struct;
  (*ap_keyword_map)[irst("switch")] = keyword_switch;
  (*ap_keyword_map)[irst("template")] = keyword_template;
  (*ap_keyword_map)[irst("this")] = keyword_this;
  (*ap_keyword_map)[irst("throw")] = keyword_throw;
  (*ap_keyword_map)[irst("true")] = keyword_true;
  (*ap_keyword_map)[irst("try")] = keyword_try;
  (*ap_keyword_map)[irst("typedef")] = keyword_typedef;
  (*ap_keyword_map)[irst("typeid")] = keyword_typeid;
  (*ap_keyword_map)[irst("typename")] = keyword_typename;
  (*ap_keyword_map)[irst("union")] = keyword_union;
  (*ap_keyword_map)[irst("unsigned")] = keyword_unsigned;
  (*ap_keyword_map)[irst("using")] = keyword_using;
  (*ap_keyword_map)[irst("virtual")] = keyword_virtual;
  (*ap_keyword_map)[irst("void")] = keyword_void;
  (*ap_keyword_map)[irst("volatile")] = keyword_volatile;
  (*ap_keyword_map)[irst("wchar_t")] = keyword_wchar_t;
  (*ap_keyword_map)[irst("while")] = keyword_while;
  (*ap_keyword_map)[irst("xor")] = keyword_xor;
  (*ap_keyword_map)[irst("xor_eq")] = keyword_xor_eq;
}

bool irs::calc::part_id_name_get(
  const part_id_variable_const_iterator a_begin_part_id_it,
  const part_id_variable_const_iterator a_end_part_id_it,
  stringns_t* ap_name)
{
  IRS_LIB_ASSERT(ap_name != IRS_NULL);
  bool fsuccess = false;
  const int single_part_id = 1;
  if (distance(a_begin_part_id_it, a_end_part_id_it) >= single_part_id) {
    if (a_begin_part_id_it->type == part_id_type_name) {
      if (a_begin_part_id_it->part_id.type() == irs::variant::var_type_string) {
        *ap_name = a_begin_part_id_it->part_id.as_string();
      } else {
        IRS_LIB_ASSERT_MSG("Имя имеет неправильный тип");
      }
    } else {
      // Текущая часть идентификатора не является именем
    }
  } else {
    // Нет частей идентификатора для обработки
  }
  return fsuccess;
}

bool irs::calc::part_id_index_get(
  const part_id_variable_const_iterator a_begin_part_id_it,
  const part_id_variable_const_iterator a_end_part_id_it,
  irs::variant::variant_t* ap_index)
{
  IRS_LIB_ASSERT(ap_index != IRS_NULL);
  bool fsuccess = false;
  const int single_part_id = 1;
  if (distance(a_begin_part_id_it, a_end_part_id_it) >= single_part_id) {
    if (a_begin_part_id_it->type == part_id_type_index) {
      if (a_begin_part_id_it->part_id.type() == ap_index->type()) {
        ap_index->assign_no_cast(a_begin_part_id_it->part_id);
      } else {
        IRS_LIB_ASSERT_MSG("Имя имеет неправильный тип");
      }
    } else {
      // Текущая часть идентификатора не является именем
    }
  } else {
    // Нет частей идентификатора для обработки
  }
  return fsuccess;
}

#ifdef NOP
// class mutable_ref_t
irs::calc::mutable_ref_t::mutable_ref_t():
  m_type(type_unknown)
{
  m_variable.p_value = IRS_NULL;
}

irs::calc::mutable_ref_t::mutable_ref_t(type_t a_type):
  m_type(type_unknown)
{
  change_type(a_type);
}

irs::calc::mutable_ref_t::mutable_ref_t(const variant_t& a_value, value_tag)
{
  value(a_value);
}

irs::calc::mutable_ref_t::mutable_ref_t(
  const id_variable_type& a_id_variable, id_variable_tag)
{
  id(a_id_variable);
}

irs::calc::mutable_ref_t::mutable_ref_t(const mutable_ref_t& a_mutable_ref):
  m_type(type_unknown)
{
  change_type(a_mutable_ref.m_type);
  switch (a_mutable_ref.m_type) {
    case type_value: {
      *m_variable.p_value = *a_mutable_ref.m_variable.p_value;
    } break;
    case type_id: {
      *m_variable.p_id = *a_mutable_ref.m_variable.p_id;
    } break;
    case type_unknown: {
      // Нет данных для копирования
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("Неучтенный тип");
    }
  }
}

irs::calc::mutable_ref_t::variant_t& irs::calc::mutable_ref_t::value()
{
  return *m_variable.p_value;
}

const irs::calc::mutable_ref_t::variant_t&
irs::calc::mutable_ref_t::value() const
{
  return *m_variable.p_value;
}

void irs::calc::mutable_ref_t::value(const variant_t& a_value)
{
  change_type(type_value);
  *m_variable.p_value = a_value;
}

irs::calc::id_variable_type& irs::calc::mutable_ref_t::id()
{
  return *m_variable.p_id;
}

const irs::calc::id_variable_type& irs::calc::mutable_ref_t::id() const
{
  return *m_variable.p_id;
}

void irs::calc::mutable_ref_t::id(const id_variable_type& a_id)
{
  change_type(type_id);
  *m_variable.p_id = a_id;
}       

irs::calc::mutable_ref_t::type_t irs::calc::mutable_ref_t::type(type_t) const
{
  return m_type;
}

void irs::calc::mutable_ref_t::change_type(const type_t a_type)
{
  if (a_type != m_type) {
    switch (m_type) {
      case type_value: {
        delete m_variable.p_value;
      } break;
      case type_id: {
        delete m_variable.p_id;
      } break;
      case type_unknown: {
        // Память не выделена
      } break;
      default : {
        IRS_LIB_ASSERT_MSG("Неучтенный тип");
      }
    }
    m_type = a_type;
    switch (a_type) {
      case type_value: {
        m_variable.p_value = new variant_t();
      } break;
      case type_id: {
        m_variable.p_id = new id_variable_type();
      } break;
      case type_unknown: {
        // Выделять память не требуется
      } break;
      default : {
        IRS_LIB_ASSERT_MSG("Неучтенный тип");
      }
    }
  } else {
    // Объект уже имеет требуемый тип
  }
}

void irs::calc::mutable_ref_t::swap(mutable_ref_t& a_mutable_ref)
{                                              
  ::swap(m_type, a_mutable_ref.m_type);
  ::swap(m_variable, a_mutable_ref.m_variable);
}

irs::calc::mutable_ref_t&
irs::calc::mutable_ref_t::operator=(const mutable_ref_t& a_mutable_ref)
{
  mutable_ref_t mutable_ref(a_mutable_ref);
  ::swap(mutable_ref, *this);
  return *this;
}

#endif // NOP

//#endif // GNU C++ specific

#endif //__WATCOMC__
