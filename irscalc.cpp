// Калькулятор
// Дата: 19.09.2009
// Ранняя дата: 01.10.2008

// В Watcom C++ есть только не-const версия функции map::find
#ifndef __WATCOMC__

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
  int pos_ch_decimal_point = str.find(",");
  while (pos_ch_decimal_point != irs::string::npos) {
    str.replace(pos_ch_decimal_point, 1, ".");
    pos_ch_decimal_point = str.find(",");
  }   
  return str;
}

void irs::calc::create_keyword_map(keyword_map_type* ap_keyword_map)
{
  ap_keyword_map->clear();
  (*ap_keyword_map)["and"] = keyword_and;
  (*ap_keyword_map)["and_eq"] = keyword_and_eq;
  (*ap_keyword_map)["asm"] = keyword_asm;
  (*ap_keyword_map)["auto"] = keyword_auto;
  (*ap_keyword_map)["bitand"] = keyword_bitand;
  (*ap_keyword_map)["bitor"] = keyword_bitor;
  (*ap_keyword_map)["bool"] = keyword_bool;             
  (*ap_keyword_map)["break"] = keyword_break;
  (*ap_keyword_map)["case"] = keyword_case;
  (*ap_keyword_map)["catch"] = keyword_catch;
  (*ap_keyword_map)["char"] = keyword_char;
  (*ap_keyword_map)["class"] = keyword_class;
  (*ap_keyword_map)["compl"] = keyword_compl;
  (*ap_keyword_map)["const"] = keyword_const;
  (*ap_keyword_map)["const_cast"] = keyword_const_cast;
  (*ap_keyword_map)["continue"] = keyword_continue;
  (*ap_keyword_map)["default"] = keyword_default;
  (*ap_keyword_map)["delete"] = keyword_delete;
  (*ap_keyword_map)["do"] = keyword_do;
  (*ap_keyword_map)["double"] = keyword_double;
  (*ap_keyword_map)["dynamic_cast"] = keyword_dynamic_cast;
  (*ap_keyword_map)["else"] = keyword_else;
  (*ap_keyword_map)["enum"] = keyword_enum;
  (*ap_keyword_map)["explicit"] = keyword_explicit;
  (*ap_keyword_map)["export"] = keyword_export;
  (*ap_keyword_map)["extern"] = keyword_extern;
  (*ap_keyword_map)["false"] = keyword_false;
  (*ap_keyword_map)["float"] = keyword_float;
  (*ap_keyword_map)["for"] = keyword_for;
  (*ap_keyword_map)["friend"] = keyword_friend;
  (*ap_keyword_map)["goto"] = keyword_goto;
  (*ap_keyword_map)["if"] = keyword_if;
  (*ap_keyword_map)["inline"] = keyword_inline;
  (*ap_keyword_map)["int"] = keyword_int;
  (*ap_keyword_map)["long"] = keyword_long;
  (*ap_keyword_map)["mutable"] = keyword_mutable;
  (*ap_keyword_map)["namespace"] = keyword_namespace;
  (*ap_keyword_map)["new"] = keyword_new;
  (*ap_keyword_map)["not"] = keyword_not;
  (*ap_keyword_map)["not_eq"] = keyword_not_eq;
  (*ap_keyword_map)["operator"] = keyword_operator;
  (*ap_keyword_map)["or"] = keyword_or;
  (*ap_keyword_map)["or_eq"] = keyword_or_eq;
  (*ap_keyword_map)["private"] = keyword_private;
  (*ap_keyword_map)["protected"] = keyword_protected;
  (*ap_keyword_map)["public"] = keyword_public;
  (*ap_keyword_map)["register"] = keyword_register;
  (*ap_keyword_map)["reinterpret_cast"] = keyword_reinterpret_cast;
  (*ap_keyword_map)["return"] = keyword_return;
  (*ap_keyword_map)["short"] = keyword_short;
  (*ap_keyword_map)["signed"] = keyword_signed;
  (*ap_keyword_map)["sizeof"] = keyword_sizeof;
  (*ap_keyword_map)["static"] = keyword_static;
  (*ap_keyword_map)["static_cast"] = keyword_static_cast;
  (*ap_keyword_map)["struct"] = keyword_struct;
  (*ap_keyword_map)["switch"] = keyword_switch;
  (*ap_keyword_map)["template"] = keyword_template;
  (*ap_keyword_map)["this"] = keyword_this;
  (*ap_keyword_map)["throw"] = keyword_throw;
  (*ap_keyword_map)["true"] = keyword_true;
  (*ap_keyword_map)["try"] = keyword_try;
  (*ap_keyword_map)["typedef"] = keyword_typedef;
  (*ap_keyword_map)["typeid"] = keyword_typeid;
  (*ap_keyword_map)["typename"] = keyword_typename;
  (*ap_keyword_map)["union"] = keyword_union;
  (*ap_keyword_map)["unsigned"] = keyword_unsigned;
  (*ap_keyword_map)["using"] = keyword_using;
  (*ap_keyword_map)["virtual"] = keyword_virtual;
  (*ap_keyword_map)["void"] = keyword_void;
  (*ap_keyword_map)["volatile"] = keyword_volatile;
  (*ap_keyword_map)["wchar_t"] = keyword_wchar_t;
  (*ap_keyword_map)["while"] = keyword_while;
  (*ap_keyword_map)["xor"] = keyword_xor;
  (*ap_keyword_map)["xor_eq"] = keyword_xor_eq;
}

#endif //__WATCOMC__
