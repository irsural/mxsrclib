// Дата: 03.08.2010
// Дата создания: 29.07.2010

#ifndef irsparamh
#define irsparamh

#include <irsdefs.h>

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
  static const char_type* def_ini_name();
public:
  param_box_t(
    const string_type& a_param_box_name = def_param_box_name(),
    const string_type& a_ini_section_prefix = def_ini_section(),
    const string_type& a_ini_name = def_ini_name()
  );
  virtual ~param_box_t();
  virtual void show();
  virtual void hide();
  virtual void add(const builder_string_type& a_param_name,
    const builder_string_type& a_param_value);
  #ifdef NOP
  builder_string_type get_param_as_string(const string_type& a_param_name);
  #endif // NOP
  virtual bool get_param(const string_type& a_param_name,
    builder_string_type* ap_param_value);
  virtual bool set_param(const string_type& a_param_name,
    const string_type& a_param_value);
  virtual void set_ini_name(const string_type& a_ini_name);
  virtual string_type ini_name();
  #ifdef NOP
  bool get_param_as_u8(const string_type& a_param_name,
    irs_u8* ap_param_value);
  bool get_param_as_i8(const string_type& a_param_name,
    irs_i8* ap_param_value);
  bool get_param_as_u16(const string_type& a_param_name,
    irs_u16* ap_param_value);
  bool get_param_as_i16(const string_type& a_param_name,
    irs_i16* ap_param_value);
  bool get_param_as_u32(const string_type& a_param_name,
    irs_u32* ap_param_value);
  bool get_param_as_i32(const string_type& a_param_name,
    irs_i32* ap_param_value);
  bool get_param_as_u64(const string_type& a_param_name,
    irs_u64* ap_param_value);
  bool get_param_as_i64(const string_type& a_param_name,
    irs_i64* ap_param_value);
  bool get_param_as_float(const string_type& a_param_name,
    float* ap_param_value);
  bool get_param_as_double(const string_type& a_param_name,
    double* ap_param_value);
  bool get_param_as_long_double(const string_type& a_param_name,
    long double* ap_param_value);
  #endif // NOP
  virtual void save();
  virtual void load();
private:
  const string_type m_ini_section;
  irs::ini_file_t m_ini_file;
  handle_t<TForm> mp_form;
  TPanel* mp_panel;
  TButton* mp_ok_btn;
  TButton* mp_cansel_btn;
  TStringGrid* mp_param_grid;
  irs_u8 m_cur_param_row;
  
  void __fastcall OK_BtnClick(TObject *Sender);
  void __fastcall Cancel_BtnClick(TObject *Sender);
  bool get_row_by_param_name(builder_string_type& a_param_name,
    size_t* ap_row_index);
  #ifdef NOP
  template<class T>
  bool get_param(const string_type& a_param_name, T* ap_param_value)
  {
    bool result = false;
    builder_string_type param_name = a_param_name.c_str();
    size_t grid_index = 0;
    if (get_row_by_param_name(param_name, &grid_index)) {
      if (str_to_num(mp_param_grid->Cells[option_col][grid_index],
        ap_param_value))
      {
        result = true;
      }
    }
    return result;
  }
  #endif // NOP
}; // param_box_t

} // namespace irs

#endif // irsparamh