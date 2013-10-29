//! \file
//! \ingroup graphical_user_interface_group
//! \brief Таблица параметров
//!
//! Дата: 21.05.2011\n
//! Дата создания: 29.07.2010

#ifndef irsparamh
#define irsparamh

#include <irsdefs.h>

#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>

#include <irsparamabs.h>
#include <mxini.h>
#include <mxdata.h>
#include <irscpp.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup graphical_user_interface_group
//! @{

//! \author Sergeev Sergey
//! \brief Таблица параметров
class param_box_t: public param_box_base_t
{
public:
  typedef param_box_base_t::string_type string_type;
  //typedef String builder_string_type;
  typedef char_t char_type;
  enum {
    form_min_height = 120,
    form_max_height = 600,
    btn_gap = 20,
    header_size = 1,
    vars_count_start = 1,
    col_count = 2,
    option_col = 1,
    header_col = 0,
    grid_width = 270,
    header_col_width = 135,
    options_col = 1,
    options_col_width = grid_width - header_col_width
  };
private:
  static const char_type* def_param_box_name();
  static const char_type* def_ini_section();
  static const char_type* empty_string();
public:
  param_box_t(
    const string_type& a_param_box_name = def_param_box_name(),
    const string_type& a_ini_section = def_ini_section(),
    const string_type& a_prefix_name = empty_string(),
    const string_type& a_ini_name = empty_string()
    #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
    ,
    TEncoding* ap_encoding = TEncoding::Default
    #endif // (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  );

  virtual ~param_box_t();
  virtual bool show();
  virtual void hide();
  virtual void add_edit(const string_type& a_param_name,
    const string_type& a_param_value);
  virtual bool add_combo_by_item(const string_type& a_param_name,
    const string_type& a_param_value);
  virtual void add_combo(const string_type& a_param_name,
    const vector<string_type>* ap_param_values_list);
  virtual void add_bool(const string_type& a_param_name,
    bool a_param_value);
  virtual void add_param_box(const string_type& a_param_name,
    param_box_base_t* ap_param_box_base);
  virtual void add_button(const string_type& a_param_name);
  virtual bool get_param(const string_type& a_param_name,
    string_type* ap_param_value) const;
  virtual string_type get_param(const string_type& a_param_name) const;
  virtual string_type get_param_def(const string_type& a_param_name) const;
  virtual bool is_button_pressed(const string_type& a_param_name);
  virtual bool set_param(const string_type& a_param_name,
    const string_type& a_param_value);
  virtual void set_ini_name(const string_type& a_ini_name);
  virtual string_type ini_name() const;
  virtual void set_section(const string_type& a_section);
  virtual string_type section() const;
  virtual void save() const;
  virtual void load();
  virtual void delete_edit(const string_type& a_param_name);
private:
  TCustomIniFile* create_ini_file(const string_type& a_file_name) const;
  string_type m_ini_section;
  const string_type m_prefix_name;
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  TEncoding* mp_encoding;
  #endif // (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  irs::ini_file_t m_ini_file;
  handle_t<TForm> mp_form;
  TPanel* mp_panel;
  TButton* mp_ok_btn;
  TButton* mp_cansel_btn;
  TValueListEditor* mp_value_list_editor;
  irs_u8 m_cur_param_row;
  map<string_type, string_type> m_param_def_list;
  map<string_type, bool> m_button_list;
  std::map<string_type, param_box_base_t*> m_param_box_base_map;

  void save_form_params();
  void load_form_params();
  void __fastcall ok_btn_click(TObject *Sender);
  void __fastcall cancel_btn_click(TObject *Sender);
  void __fastcall on_close_event(TObject *Sender, TCloseAction &Action);
  void __fastcall on_edit_btn_click(TObject *Sender);
}; // param_box_t

//! @}

} // namespace irs

#endif // irsparamh