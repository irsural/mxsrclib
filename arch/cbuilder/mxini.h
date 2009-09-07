// Работа с ini-файлами
// Дата: 27.08.2009

#ifndef mxiniH
#define mxiniH

#include <inifiles.hpp>
#include <stdctrls.hpp>
#include <comctrls.hpp>
#include <cspin.h>
#include <grids.hpp>
#include <vcl/dstring.h>
#include <SysUtils.hpp>

#include <irscpp.h>
#include <irsstd.h>        

namespace irs {

// Работа с ini-файлами
class ini_file_t
{
public:
  ini_file_t();
  ~ini_file_t();
  void set_ini_name(const String& a_ini_name);
  String ini_name();
  void set_section(const String& a_name);
  void add(const String& a_name, bool* a_control);
  void add(const String& a_name, irs_u8* a_control);
  void add(const String& a_name, irs_i8* a_control);
  void add(const String& a_name, irs_u16* a_control);
  void add(const String& a_name, irs_i16* a_control);
  void add(const String& a_name, irs_u32* a_control);
  void add(const String& a_name, irs_i32* a_control);
  void add(const String& a_name, irs_u64* a_control);
  void add(const String& a_name, irs_i64* a_control);
  void add(const String& a_name, float* a_control);
  void add(const String& a_name, double* a_control);
  void add(const String& a_name, long double* a_control);
  void add(const String& a_name, irs::string* a_control);
  void add(const String& a_name, AnsiString* a_control);
  void add(const String& a_name, TEdit *a_control);
  void add(const String& a_name, TCSpinEdit *a_control);
  void add(const String& a_name, TCheckBox *a_control);
  void add(const String& a_name, TRadioButton *a_control);
  void add(const String& a_name, TPageControl *a_control);
  void add(const String& a_name, TGroupBox *a_control);
  void add(const String& a_name, TComboBox *a_control);
  void add(const String& a_name, TRadioGroup *a_control);
  void add(const String& a_name, TStringGrid *a_control);
  void add(const String& a_name, TStringGrid *a_control,
    const String& a_column_name, int a_column_index);
  void load();
  void save() const;
  void save_grid_row(TStringGrid *a_control, int a_row_index) const;
  void save_grid_size(TStringGrid *a_control) const;
  void clear_control();

  struct bad_exe_path {};
private:
  struct control_t {
    irs::string section;
    irs::string name;

    bool operator==(const control_t &a_control)
    {
      return (section == a_control.section) && (name == a_control.name);
    }
    control_t(const irs::string& a_section, const irs::string& a_name):
      section(a_section),
      name(a_name)
    {
    }
  };
  template <class T>
  struct generalized_control_t: control_t {
    T* control;

    bool operator==(const generalized_control_t& a_edit)
    {
      control_t& this_base = static_cast<control_t&>(*this);
      const control_t& edit_base = static_cast<const control_t&>(a_edit);
      return (control == a_edit.control) && (this_base == edit_base);
    }
    bool operator!=(const generalized_control_t& a_edit)
    {
      return !(*this == a_edit);
    }
    generalized_control_t(irs::string& a_section, irs::string& a_name,
      T* a_control):
      control_t(a_section, a_name),
      control(a_control)
    {
    }
  };

  typedef generalized_control_t<TEdit> edit_t;
  typedef generalized_control_t<TCSpinEdit> cspin_edit_t;
  typedef generalized_control_t<TCheckBox> check_box_t;
  typedef generalized_control_t<TRadioButton> radio_button_t;
  typedef generalized_control_t<TPageControl> page_control_t;
  typedef generalized_control_t<TGroupBox> group_box_t;
  typedef generalized_control_t<TComboBox> combo_box_t;
  typedef generalized_control_t<TRadioGroup> radio_group_t;

  typedef generalized_control_t<AnsiString> ansi_string_t;
  typedef generalized_control_t<bool> bool_t;
  typedef generalized_control_t<irs_u8> irs_u8_t;
  typedef generalized_control_t<irs_i8> irs_i8_t;
  typedef generalized_control_t<irs_u16> irs_u16_t;
  typedef generalized_control_t<irs_i16> irs_i16_t;
  typedef generalized_control_t<irs_u32> irs_u32_t;
  typedef generalized_control_t<irs_i32> irs_i32_t;
  typedef generalized_control_t<irs_u64> irs_u64_t;
  typedef generalized_control_t<irs_i64> irs_i64_t;
  typedef generalized_control_t<float> float_t;
  typedef generalized_control_t<double> double_t;
  typedef generalized_control_t<long double> long_double_t;
  typedef generalized_control_t<irs::string> irs_string_t;

  struct string_grid_t: control_t {
    struct column_t {
      int index;
      irs::string name;

      column_t(int a_index, irs::string a_name):
        index(a_index),
        name(a_name)
      {
      }
    };

    bool all_columns;
    vector<column_t> columns;

    string_grid_t(const irs::string& a_section = "",
      const irs::string& a_name = ""):
      control_t(a_section, a_name),
      all_columns(true),
      columns()
    {
    }
    void add(int a_index, const irs::string& a_name)
    {
      all_columns = false;
      columns.push_back(column_t(a_index, a_name));
    }
  };
  enum load_save_t { ls_load, ls_save };

  vector<bool_t> mv_bools;
  vector<irs_u8_t> mv_irs_u8s;
  vector<irs_i8_t> mv_irs_i8s;
  vector<irs_u16_t> mv_irs_u16s;
  vector<irs_i16_t> mv_irs_i16s;
  vector<irs_u32_t> mv_irs_u32s;
  vector<irs_i32_t> mv_irs_i32s;
  vector<irs_u64_t> mv_irs_u64s;
  vector<irs_i64_t> mv_irs_i64s;
  vector<float_t> mv_floats;
  vector<double_t> mv_doubles;
  vector<long_double_t> mv_long_doubles;
  vector<irs_string_t> mv_irs_strings;
  vector<ansi_string_t> mv_ansi_strings;
  vector<edit_t> m_edits;
  vector<cspin_edit_t> m_cspin_edits;
  vector<check_box_t> m_check_boxs;
  vector<radio_button_t> m_radio_buttons;
  vector<page_control_t> m_page_controls;
  vector<group_box_t> m_group_boxs;
  vector<combo_box_t> m_combo_boxs;
  vector<radio_group_t> m_radio_groups;
  map<TStringGrid*, string_grid_t> m_string_grids;
  irs::string m_ini_name;
  irs::string m_section;

  TTabSheet *FindTabSheet(TPageControl *a_control, const String &a_name);
  TRadioButton *FindRadioButton(TGroupBox *a_control, const String &a_name);
  TRadioButton *ActiveRadioButton(TGroupBox *a_control) const;
  int FindComboBoxItem(TComboBox *a_control, const String &a_name);
  int FindRadioGroupIndex(TRadioGroup *a_control, const String &a_name);
  void load_save_grid_row(TIniFile *ap_ini_file, load_save_t a_load_save,
    TStringGrid *a_control, int a_row_index) const;
  void load_save_grid_cell(TIniFile *ap_ini_file, load_save_t a_load_save,
    TStringGrid *a_control, int a_row_index, int a_col_index,
    const irs::string& a_name) const;
  void load_save_grid_size(TIniFile *ap_ini_file, load_save_t a_load_save,
    TStringGrid *a_control) const;

}; //class ini_file_t    

} //namespace irs

#endif //mxiniH
