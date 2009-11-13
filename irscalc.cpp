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
/*
irs::string irs::preprocessing_str(const irs::string& a_str)
{
  irs::string str = a_str;
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
  return str;
}
*/

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

bool irs::calc::calculator_t::local_variable_value(
  const mode_io_value_t a_mode_io_value,
  const id_variable_type& id_variable,
  variant_t* ap_value)
{
  IRS_LIB_ASSERT(ap_value != IRS_NULL);
  bool fsuccess = true;
  const size_type part_count = id_variable.size();
  variant_t* p_value_variant = IRS_NULL;
  list_identifier_t::variable_t* p_variable = IRS_NULL;
  if (part_count > 0) {
    //variant_t value_helper;
    variant_t index_variant;
    IRS_LIB_ASSERT(id_variable.front().type == part_id_type_name);
    if (m_list_identifier.variable_find(id_variable.front().part_id,
      &p_variable))
    {
      p_value_variant = &p_variable->value;
      for (size_type part_i = 1; part_i < part_count; part_i++) {
        if (id_variable[part_i].type == part_id_type_index) {
          if (p_value_variant->type() == irs::variant::var_type_array) {
            index_variant = id_variable[part_i].part_id;
            if (irs::variant::is_number_type(index_variant.type())) {
              p_value_variant =
                &(*p_value_variant)[index_variant.as_type<size_type>()];
              //value = value_helper;
            } else {
              // Недопустимый тип индекса
              fsuccess = false;
              break;
            }
          } else {
            // Недопустимое количество индексов
            fsuccess = false;
            break;
          }
        } else {
          // Часть идентификатора должна быть индексом
          fsuccess = false;
          break;
        }
      }
    } else {
      fsuccess = false;
    }
  } else {
    // Пустой идентификатор
    fsuccess = false;
  }
  if (fsuccess) {
    switch (a_mode_io_value) {
      case mode_io_value_read: {
        *ap_value = *p_value_variant;
      } break;
      case mode_io_value_write: {
        if (p_variable->variability == v_is_variable) {
          *p_value_variant = *ap_value;
        } else {
          // Переменная является константой
          fsuccess = false;
        }
      } break;
      default : {
        IRS_LIB_ASSERT_MSG("Недопустимый режим");
      }
    }
  } else {
    // Получить значение не удалось
  }
  return fsuccess;
}

bool irs::calc::calculator_t::extern_variable_value(
  const mode_io_value_t a_mode_io_value,
  const id_variable_type& id_variable,
  variant_t* ap_value
)
{
  IRS_LIB_ASSERT(ap_value != IRS_NULL);
  bool fsuccess = false;
  if (mp_handle_extern_variable) {
    switch (a_mode_io_value) {
      case mode_io_value_read: {
        fsuccess = mp_handle_extern_variable->get(id_variable, ap_value);
      } break;
      case mode_io_value_write: {
        fsuccess = mp_handle_extern_variable->set(id_variable, *ap_value);
      } break;
      default : {
        IRS_LIB_ASSERT_MSG("Недопустимый режим");
      }
    }
  } else {
    // Список на внешние переменные не установлен
  }
  return fsuccess;
}

bool irs::calc::calculator_t::assign_variable(
  const mutable_ref_t& a_right_value_mref,
  mutable_ref_t* ap_left_value_mref)
{
  bool assign_success = false;
  if (ap_left_value_mref->type() == mutable_ref_t::type_id) {
    assign_success = value_write(a_right_value_mref, ap_left_value_mref);
  } else {
    // Левое значение должно быть ссылкой на переменную
  }
  return assign_success;
}

bool irs::calc::calculator_t::value_read(
  const mutable_ref_t& a_mutable_ref_src,
  variant_t* ap_value_dest)
{
  IRS_LIB_ASSERT(ap_value_dest != IRS_NULL);
  bool value_read_success = false;
  const mutable_ref_t::type_t type = a_mutable_ref_src.type();
  switch (type) {
    case mutable_ref_t::type_value: {
      *ap_value_dest = a_mutable_ref_src.value();
      value_read_success = true;
    } break;
    case mutable_ref_t::type_id: {
      const id_variable_type id_variable = a_mutable_ref_src.id();
      variant_t value_src_variant;
      if (local_variable_value(
        mode_io_value_read, id_variable, ap_value_dest))
      {
        //*ap_value_dest = value_src_variant;
        value_read_success = true;
      } else if (extern_variable_value(mode_io_value_read, id_variable,
        ap_value_dest)) {
        value_read_success = true;
      } else {
        // Переменная не найдена
      }
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("Неизвестный тип");
    }
  }
  return value_read_success;
}

bool irs::calc::calculator_t::value_write(
  const mutable_ref_t& a_mutable_ref_src,
  mutable_ref_t* ap_mutable_ref_dest)
{              
  IRS_LIB_ASSERT(ap_mutable_ref_dest != IRS_NULL);
  bool value_write_success = false;
  const mutable_ref_t::type_t mutable_ref_dest_type =
    ap_mutable_ref_dest->type();
  switch (mutable_ref_dest_type) {
    case mutable_ref_t::type_value: {
      variant_t value_dest;
      if (value_read(a_mutable_ref_src, &value_dest)) {
        ap_mutable_ref_dest->value(value_dest);
        value_write_success = true;
      } else {
        // Прочить значение не удалось
      }
    } break;
    case mutable_ref_t::type_id: {
      const id_variable_type id_variable_dest = ap_mutable_ref_dest->id();
      variant_t value_variant_src;
      if (value_read(a_mutable_ref_src, &value_variant_src)) {
        if (local_variable_value(
          mode_io_value_write, id_variable_dest, &value_variant_src))
        {
          value_write_success = true;
        } else if (extern_variable_value(mode_io_value_write, id_variable_dest,
          &value_variant_src)) {
          value_write_success = true;
        } else {
          // Записать значение не удалось
        }
      } else {
        // Прочить значение не удалось
      }
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("Неизвестный тип");
    }
  }
  return value_write_success;
}


#ifndef NOP
// class mutable_ref_t   
irs::calc::mutable_ref_t::mutable_ref_t(type_t a_type):
  m_type(a_type),
  m_value(),
  m_id()
{
}

irs::calc::mutable_ref_t::mutable_ref_t(const variant_t& a_value, value_tag):
  m_type(type_value),
  m_value(a_value),
  m_id()
{     
}

irs::calc::mutable_ref_t::mutable_ref_t(
  const id_variable_type& a_id_variable, id_variable_tag
):
  m_type(type_id),
  m_value(),
  m_id(a_id_variable)
{           
}

irs::calc::mutable_ref_t::mutable_ref_t(const mutable_ref_t& a_mutable_ref):
  m_type(type_unknown)
{
  m_type = a_mutable_ref.m_type;
  m_value = a_mutable_ref.m_value;
  m_id = a_mutable_ref.m_id;
}

irs::calc::mutable_ref_t::variant_t& irs::calc::mutable_ref_t::value()
{
  return m_value;
}

const irs::calc::mutable_ref_t::variant_t&
irs::calc::mutable_ref_t::value() const
{
  return m_value;
}

void irs::calc::mutable_ref_t::value(const variant_t& a_value)
{
  m_type = type_value;
  m_value = a_value;
  m_id.clear();
}

irs::calc::id_variable_type& irs::calc::mutable_ref_t::id()
{
  return m_id;
}

const irs::calc::id_variable_type& irs::calc::mutable_ref_t::id() const
{
  return m_id;
}

void irs::calc::mutable_ref_t::id(const id_variable_type& a_id)
{
  m_type = type_id;
  m_id = a_id;
}       

irs::calc::mutable_ref_t::type_t irs::calc::mutable_ref_t::type() const
{
  return m_type;
}

void irs::calc::mutable_ref_t::type(const type_t a_type)
{
  change_type(a_type);
}

void irs::calc::mutable_ref_t::change_type(const type_t a_type)
{
  m_type = a_type;
  m_value.type(irs::variant::var_type_unknown);
  m_id.clear();
}

void irs::calc::mutable_ref_t::swap(mutable_ref_t& a_mutable_ref)
{
  ::swap(m_type, a_mutable_ref.m_type);
  ::swap(m_value, a_mutable_ref.m_value);
  ::swap(m_id, a_mutable_ref.m_id);
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
