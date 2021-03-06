//! \file
//! \ingroup file_in_out_group
//! \brief ������ � ini-�������
//!
//! ����: 17.04.2011\n
//! ������ ����: 27.08.2009

#ifndef mxiniH
#define mxiniH

#include <irsdefs.h>

#include <inifiles.hpp>
#include <stdctrls.hpp>
#include <comctrls.hpp>
#include <cspin.h>
#include <Vcl.Samples.Spin.hpp>
#include <grids.hpp>
#include <ValEdit.hpp>
# if (defined(__BORLANDC__) && (__BORLANDC__ < IRS_CPP_BUILDERXE))
#include <vcl/dstring.h>
#endif // (defined(__BORLANDC__) && (__BORLANDC__ < IRS_CPP_BUILDERXE))
#include <SysUtils.hpp>

#if IRS_USE_DEV_EXPRESS
# include <cxGrid.hpp>
# include <cxGridDBTableView.hpp>
#endif // IRS_USE_DEV_EXPRESS


#include <irscpp.h>
#include <irsstd.h>
#include <irsstrdefs.h>

#include <irsfinal.h>        

namespace irs {

//! \addtogroup file_in_out_group
//! @{

//! \brief ������ � ini-�������
class ini_file_t
{
public:
  typedef TMemIniFile ini_file_type;
  enum vle_load_t { vle_load_value, vle_load_full };

  ini_file_t();
  ~ini_file_t();
  void set_ini_name(const String& a_ini_name);
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  void set_encoding(TEncoding* ap_encoding);
  #endif // (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  String ini_name() const;
  void set_section(const String& a_name);
  void add(const String& a_name, bool* a_control);
  void add(const String& a_name, int* a_control);
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
  void add(const String& a_name, string_t* a_control);
  void add(const String& a_name, AnsiString* a_control);
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  void add(const String& a_name, String* a_control);
  #endif // (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  void add(const String& a_name, TEdit *a_control);
  void add(const String& a_name, TSpinEdit *a_control);
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
  #if IRS_USE_DEV_EXPRESS
  void add(const String& a_name, TcxGridTableView *a_control,
    const String& a_column_name, TcxGridColumn* ap_column, int a_column_index);
  #endif // IRS_USE_DEV_EXPRESS
  void add(const String& a_name, TValueListEditor* a_control,
    vle_load_t a_vle_load = vle_load_value);
  void add(const String& a_name, TForm* a_control);
  void load();
  void save() const;
  void save_grid_row(TStringGrid *a_control, int a_row_index) const;
  void save_grid_size(TStringGrid *a_control) const;
  #if IRS_USE_DEV_EXPRESS
  void save_cx_grid_table_view_row(TcxGridTableView *ap_control,
    int a_row_index) const;
  void save_cx_grid_table_view_row_count(TcxGridTableView* ap_control) const;
  #endif // IRS_USE_DEV_EXPRESS
  void clear_control();
private:
  TCustomIniFile* create_ini_file(const string_t& a_file_name) const;
  struct control_t {
    string_t section;
    string_t name;

    bool operator==(const control_t &a_control) const
    {
      return (section == a_control.section) && (name == a_control.name);
    }
    control_t(const string_t& a_section, const string_t& a_name):
      section(a_section),
      name(a_name)
    {
    }
  };
  template <class T>
  struct generalized_control_t: control_t {
    T* control;

    bool operator==(const generalized_control_t& a_edit) const
    {
      const control_t& this_base = static_cast<const control_t&>(*this);
      const control_t& edit_base = static_cast<const control_t&>(a_edit);
      return (control == a_edit.control) && (this_base == edit_base);
    }
    bool operator!=(const generalized_control_t& a_edit) const
    {
      return !(*this == a_edit);
    }
    generalized_control_t(string_t& a_section, string_t& a_name,
      T* a_control):
      control_t(a_section, a_name),
      control(a_control)
    {
    }
  };

  typedef generalized_control_t<TEdit> edit_t;
  typedef generalized_control_t<TSpinEdit> spin_edit_t;
  typedef generalized_control_t<TCSpinEdit> cspin_edit_t;
  typedef generalized_control_t<TCheckBox> check_box_t;
  typedef generalized_control_t<TRadioButton> radio_button_t;
  typedef generalized_control_t<TPageControl> page_control_t;
  typedef generalized_control_t<TGroupBox> group_box_t;
  typedef generalized_control_t<TComboBox> combo_box_t;
  typedef generalized_control_t<TRadioGroup> radio_group_t;
  //typedef generalized_control_t<TValueListEditor> value_list_editor_t;
  typedef generalized_control_t<TForm> form_t;

  typedef generalized_control_t<AnsiString> ansi_string_t;
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  typedef generalized_control_t<String> cbstring_t;
  #endif // (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  typedef generalized_control_t<bool> bool_t;
  typedef generalized_control_t<int> int_t;
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
  typedef generalized_control_t<string_t> irs_string_t;

  struct string_grid_t: control_t {
    struct column_t {
      int index;
      string_t name;

      column_t(int a_index, string_t a_name):
        index(a_index),
        name(a_name)
      {
      }
    };

    bool all_columns;
    vector<column_t> columns;

    string_grid_t(const string_t& a_section = irst(""),
      const string_t& a_name = irst("")):
      control_t(a_section, a_name),
      all_columns(true),
      columns()
    {
    }
    void add(int a_index, const string_t& a_name)
    {
      all_columns = false;
      columns.push_back(column_t(a_index, a_name));
    }
  };
  #if IRS_USE_DEV_EXPRESS
  struct cx_grid_table_view_t: control_t {
    struct column_t {
      TcxGridColumn* column;
      int index;
      string_t name;
      column_t(TcxGridColumn* ap_column, int a_index, string_t a_name):
        column(ap_column),
        index(a_index),
        name(a_name)
      {
      }
    };

    vector<column_t> columns;

    cx_grid_table_view_t(const string_t& a_section = irst(""),
      const string_t& a_name = irst("")):
      control_t(a_section, a_name),
      columns()
    {
    }
    void add(TcxGridColumn* ap_column, int a_index, const string_t& a_name)
    {
      columns.push_back(column_t(ap_column, a_index, a_name));
    }
  };
  #endif // IRS_USE_DEV_EXPRESS
  typedef generalized_control_t<TValueListEditor> vle_general_type;
  struct value_list_editor_t {

    vle_load_t vle_load;
    vle_general_type general;

    value_list_editor_t(vle_load_t a_vle_load,
      const vle_general_type& a_general
    ):
      vle_load(a_vle_load),
      general(a_general)
    {
    }
    bool operator==(const value_list_editor_t& a_edit) const
    {
      return (vle_load == a_edit.vle_load) && (general == a_edit.general);
    }
    bool operator!=(const value_list_editor_t& a_edit) const
    {
      return !(*this == a_edit);
    }
  };
  enum load_save_t { ls_load, ls_save };

  vector<bool_t> mv_bools;
  vector<int_t> mv_ints;
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
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  vector<cbstring_t> mv_cbstrings;
  #endif // (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  vector<edit_t> m_edits;
  vector<spin_edit_t> m_spin_edits;
  vector<cspin_edit_t> m_cspin_edits;
  vector<check_box_t> m_check_boxs;
  vector<radio_button_t> m_radio_buttons;
  vector<page_control_t> m_page_controls;
  vector<group_box_t> m_group_boxs;
  vector<combo_box_t> m_combo_boxs;
  vector<radio_group_t> m_radio_groups;
  map<TStringGrid*, string_grid_t> m_string_grids;
  #if IRS_USE_DEV_EXPRESS
  map<TcxGridTableView*, cx_grid_table_view_t> m_cx_grid_table_views;
  #endif // IRS_USE_DEV_EXPRESS
  vector<value_list_editor_t> m_value_list_editors;
  vector<form_t> m_forms;

  string_t m_ini_name;
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  TEncoding* mp_encoding;
  #endif // (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  string_t m_section;

  TTabSheet *FindTabSheet(TPageControl *a_control, const String &a_name);
  TRadioButton *FindRadioButton(TGroupBox *a_control, const String &a_name);
  TRadioButton *ActiveRadioButton(TGroupBox *a_control) const;
  int FindComboBoxItem(TComboBox *a_control, const String &a_name);
  int FindRadioGroupIndex(TRadioGroup *a_control, const String &a_name);
  void load_save_grid_row(TCustomIniFile *ap_ini_file, load_save_t a_load_save,
    TStringGrid *a_control, int a_row_index) const;
  void load_save_grid_cell(TCustomIniFile *ap_ini_file, load_save_t a_load_save,
    TStringGrid *a_control, int a_row_index, int a_col_index,
    const string_t& a_name) const;
  void load_save_grid_size(TCustomIniFile *ap_ini_file, load_save_t a_load_save,
    TStringGrid *a_control) const;
  #if IRS_USE_DEV_EXPRESS
  void load_save_cx_grid_table_view_row(TCustomIniFile *ap_ini_file,
    load_save_t a_load_save,
    TcxGridTableView *ap_control, int a_row_index) const;
  void load_save_cx_grid_table_view_cell(TCustomIniFile *ap_ini_file,
    load_save_t a_load_save,
    TcxGridTableView *ap_control, int a_row_index, TcxGridColumn* ap_column,
    const string_t& a_name) const;
  void load_save_cx_grid_table_view_row_count(TCustomIniFile *ap_ini_file,
    load_save_t a_load_save, TcxGridTableView* ap_control) const;
  #endif // IRS_USE_DEV_EXPRESS
}; //class ini_file_t

//! @}

} //namespace irs

#endif //mxiniH
