// Дата: 03.08.2010
// Дата создания: 29.07.2010

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

#include <irsfinal.h>

namespace irs {

class param_box_t: public param_box_base_t
{
public:
  typedef param_box_base_t::string_type string_type;
  typedef String builder_string_type;
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
    const string_type& a_ini_section_prefix = def_ini_section(),
    const string_type& a_prefix_name = empty_string(),
    const string_type& a_ini_name = empty_string()
  );
  virtual ~param_box_t();
  virtual bool show();
  virtual void hide();
  virtual void add_edit(const builder_string_type& a_param_name,
    const builder_string_type& a_param_value);
  virtual bool add_combo_by_item(const builder_string_type& a_param_name,
    const builder_string_type& a_param_value);
  virtual void add_combo(const builder_string_type& a_param_name,
    vector<string_type>* ap_param_values_list);
  virtual void add_bool(const builder_string_type& a_param_name,
    bool a_param_value);
  virtual bool get_param(const string_type& a_param_name,
    builder_string_type* ap_param_value);
  virtual bool set_param(const string_type& a_param_name,
    const string_type& a_param_value);
  virtual void set_ini_name(const string_type& a_ini_name);
  virtual string_type ini_name();
  virtual void save();
  virtual void load();
private:
  const string_type m_ini_section;
  irs::ini_file_t m_ini_file;
  handle_t<TForm> mp_form;
  TPanel* mp_panel;
  TButton* mp_ok_btn;
  TButton* mp_cansel_btn;
  TValueListEditor* mp_value_list_editor;
  irs_u8 m_cur_param_row;
  
  void __fastcall ok_btn_click(TObject *Sender);
  void __fastcall cancel_btn_click(TObject *Sender);
  void __fastcall on_close_event(TObject *Sender, TCloseAction &Action);
}; // param_box_t

} // namespace irs

#endif // irsparamh