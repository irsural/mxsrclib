//! \file
//! \ingroup graphical_user_interface_group
//! \brief Тест сети 5 - библиотека
//!
//! Дата создания: 21.06.2013

#ifndef tstlan5libH
#define tstlan5libH

#if IRS_USE_DEV_EXPRESS

#include <irsdefs.h>

#include <tstlan4lib.h>
#include <tstlan4abs.h>
#include <timer.h>
#include <mxini.h>

#include <MxChart.h>
#include <csvwork.h>
#include <cbsysutils.h>
#include <irstable.h>
#include <mxdata.h>
#include <irsparamabs.h>
#include <irschartwin.h>

#include <cxGrid.hpp>
#include <cxGridDBTableView.hpp>

#include <irsfinal.h>



namespace irs {

//! \addtogroup graphical_user_interface_group
//! @{

//String extract_file_ultra_short_name(const String& Name);

template <class T>
T extract_file_ultra_short_name(const T& a_file_name)
{
  String Name = irs::str_conv<String>(a_file_name);
  String ShortFileName = ExtractFileName(Name);
  String ExtFileName = ExtractFileExt(ShortFileName);
  int LengthFileName = ShortFileName.Length();
  int LengthExt = ExtFileName.Length();
  if (LengthFileName > LengthExt) {
    ShortFileName = ShortFileName.SubString(1, LengthFileName - LengthExt);
  }
  return irs::str_conv<T>(ShortFileName);
}

namespace tstlan {

string_t get_grid_options_file_ext();

enum type_t {
  type_first = 0,
  type_unknown = type_first,
  type_bit,
  type_bool,
  type_i8,
  type_u8,
  type_i16,
  type_u16,
  type_i32,
  type_u32,
  type_i64,
  type_u64,
  type_float,
  type_double,
  type_long_double,
  type_count
};

string_t type_to_str(type_t a_type);
bool str_to_type(const string_t& a_str, type_t* ap_type);

class vars_ini_file_t
{
public:
  typedef string_t string_type;
  enum vle_load_t { vle_load_value, vle_load_full };
  vars_ini_file_t(
    TcxGridTableView* ap_cx_grid_table_view,
    TcxGridColumn* ap_category_column,
    TcxGridColumn* ap_name_column,
    TcxGridColumn* ap_type_column,
    TcxGridColumn* ap_chart_column,
    const string_type& a_section_prefix);
  void set_ini_name(const String& a_ini_name);
  void set_encoding(TEncoding* ap_encoding);
  void set_section(const string_type& a_section);
  void load();
  void save() const;
  void save_cx_grid_table_view_row(int a_row_index) const;
  void save_cx_grid_table_view_row_count() const;
private:
  void load_cx_grid_table_view_row_count(TCustomIniFile *ap_ini_file);
  void save_cx_grid_table_view_row_count(TCustomIniFile *ap_ini_file) const;
  void load_cx_grid_table_view_row(TCustomIniFile *ap_ini_file,
    int a_row_index);
  void save_cx_grid_table_view_row(TCustomIniFile *ap_ini_file,
    int a_row_index) const;
  void load_category(TCustomIniFile *ap_ini_file, int a_row_index);
  void save_category(TCustomIniFile *ap_ini_file, int a_row_index) const;
  void load_name(TCustomIniFile *ap_ini_file, int a_row_index);
  void save_name(TCustomIniFile *ap_ini_file, int a_row_index) const;
  void load_type(TCustomIniFile *ap_ini_file, int a_row_index);
  void save_type(TCustomIniFile *ap_ini_file, int a_row_index) const;
  void load_chart(TCustomIniFile *ap_ini_file, int a_row_index);
  void save_chart(TCustomIniFile *ap_ini_file, int a_row_index) const;
  TEncoding* mp_encoding;
  string_type m_ini_name;
  TcxGridTableView* mp_cx_grid_table_view;
  TcxGridColumn* mp_category_column;
  TcxGridColumn* mp_name_column;
  TcxGridColumn* mp_type_column;
  TcxGridColumn* mp_chart_column;
  string_type m_section_prefix;
  const string_type m_section_name;
  string_type m_section_full_name;
  const string_type m_name_row_count;
  const string_type m_category_column_prefix;
  const string_type m_name_column_prefix;
  const string_type m_type_column_prefix;
  const string_type m_chart_column_prefix;
};

class view_t: public tstlan4_base_t
{
public:
  typedef tstlan4_base_t::size_type size_type;
  typedef tstlan4_base_t::string_type string_type;
  typedef tstlan4_base_t::char_type char_type;
private:
  static const char_type* def_ini_name();
  static const char_type* def_ini_section();
public:
  enum form_type_t { ft_internal, ft_external };
  enum global_log_connect_t {
    global_log_connect,
    global_log_unchange
  };

  view_t(const view_t& a_view);
  view_t(
    form_type_t a_form_type = ft_internal,
    chart_window_t* ap_extern_chart = IRS_NULL,
    const string_type& a_ini_name = def_ini_name(),
    const string_type& a_ini_section_prefix = string_type(),
    counter_t a_update_time_cnt = irs::make_cnt_ms(200),
    global_log_connect_t a_global_log_connect = global_log_unchange
  );
  // Подключение к форме и создание элементов управления
  // Можно переподключать к другой форме без вызова deinit
  // На предыдущей форме все компоненты уничтожаются
  void init(TForm *ap_form);
  // Уничтожение всех компонентов формы
  void deinit();

  virtual void tick();
  virtual void show();
  virtual void hide();
  virtual rect_t position() const;
  virtual void set_position(const rect_t &a_position);
  virtual void connect(mxdata_t *ap_data);
  virtual void update_time(const irs_i32 a_update_time);
  virtual void resize_chart(const irs_u32 a_size);
  virtual void reset_chart();
  virtual void options_event_connect(event_t* ap_event);
  virtual void options_event_clear();
  virtual event_t* inner_options_event();
  virtual void save_conf();
  virtual void load_conf();
  virtual void clear_conf();
  virtual string_type conf_section();
  virtual void conf_section(const string_type& a_name);
  virtual string_type ini_name();
  virtual void ini_name(const string_type& a_ini_name);
private:
  class controls_t: public TObject
  {
  public:
    typedef view_t::size_type size_type;
    typedef view_t::string_type string_type;
    typedef view_t::char_type char_type;
    controls_t(
      form_type_t a_form_type,
      TForm *ap_form,
      chart_window_t* ap_extern_chart,
      const string_type& a_ini_name,
      const string_type& a_ini_section,
      irs::chart::builder_chart_window_t::stay_on_top_t a_stay_on_top,
      counter_t a_update_time_cnt
    );
    void create_grid();
    virtual __fastcall ~controls_t();
    void connect(mxdata_t *ap_data);
    void update_time(const irs_i32 a_update_time);
    void resize_chart(const irs_u32 a_size);
    void reset_chart();
    // Консоль внутри tstlan5
    TMemo* log();
    void options_event_connect(event_t* ap_event);
    void options_event_clear();
    virtual event_t* inner_options_event();
    void save_conf();
    void load_conf();
    void clear_conf();
    string_type conf_section();
    void conf_section(const string_type& a_name);
    string_type ini_name();
    void ini_name(const string_type& a_ini_name);
    void save_table_values(const string_type& a_file_name) const;
    void load_table_values(const string_type& a_file_name);
    void tick();
  private:
    struct netconn_t {
      struct item_t {
        type_t type;
        int index;
        int conn_index;
        int bit_index;
        item_t():
          type(type_unknown),
          index(0),
          conn_index(0),
          bit_index(0)
        {
        }
      }; //item_t

      static const int bit_in_byte = 8;

      vector<bit_data_t> bit_vec;
      vector<conn_data_t<bool> > bool_vec;
      vector<conn_data_t<irs_i8> > i8_vec;
      vector<conn_data_t<irs_u8> > u8_vec;
      vector<conn_data_t<irs_i16> > i16_vec;
      vector<conn_data_t<irs_u16> > u16_vec;
      vector<conn_data_t<irs_i32> > i32_vec;
      vector<conn_data_t<irs_u32> > u32_vec;
      vector<conn_data_t<irs_i64> > i64_vec;
      vector<conn_data_t<irs_u64> > u64_vec;
      vector<conn_data_t<float> > float_vec;
      vector<conn_data_t<double> > double_vec;
      vector<conn_data_t<long double> > long_double_vec;

      vector<item_t> items;
      mxdata_t* mp_data;

      netconn_t():
        items(),
        mp_data(IRS_NULL)
      {
      }
      void add_conn(vector<bit_data_t>& a_vec, int a_var_index,
        int& a_conn_index, int& a_bit_index)
      {
        if ((size_t)a_conn_index < mp_data->size()) {
          items[a_var_index].type = type_bit;
          items[a_var_index].index = a_vec.size();
          items[a_var_index].conn_index = a_conn_index;
          items[a_var_index].bit_index = a_bit_index;
          a_vec.push_back(bit_data_t());
          a_conn_index = a_vec.back().connect(mp_data, a_conn_index,
            a_bit_index);
          a_bit_index++;
          if (a_bit_index >= bit_in_byte) {
            a_bit_index = 0;
            a_conn_index++;
          }

        } else {
          items[a_var_index].type = type_unknown;
          items[a_var_index].index = 0;
          items[a_var_index].conn_index = 0;
          items[a_var_index].bit_index = 0;
        }
      }
      template <class T>
      void add_conn(vector<conn_data_t<T> >& a_vec, int a_var_index,
        int& a_conn_index, int& a_bit_index, type_t a_type)
      {
        if (a_bit_index != 0) {
          a_bit_index = 0;
          a_conn_index++;
        }
        if (a_conn_index + sizeof(T) <= mp_data->size()) {
          items[a_var_index].type = a_type;
          items[a_var_index].index = a_vec.size();
          items[a_var_index].conn_index = a_conn_index;
          items[a_var_index].bit_index = 0;
          a_vec.push_back(conn_data_t<T>());
          a_conn_index = a_vec.back().connect(mp_data, a_conn_index);

        } else {
          items[a_var_index].type = type_unknown;
          items[a_var_index].index = 0;
          items[a_var_index].conn_index = 0;
          items[a_var_index].bit_index = 0;
        }
      }
      int connect(mxdata_t* ap_data, TcxGrid* ap_grid, int a_type_col,
        int a_index_col)
      {
        mp_data = ap_data;

        bit_vec.clear();
        bool_vec.clear();
        i8_vec.clear();
        u8_vec.clear();
        i16_vec.clear();
        u16_vec.clear();
        i32_vec.clear();
        u32_vec.clear();
        i64_vec.clear();
        u64_vec.clear();
        float_vec.clear();
        double_vec.clear();
        long_double_vec.clear();

        items.clear();
        const int row_count = ap_grid->ActiveView->DataController->RecordCount;
        ap_grid->ActiveView->BeginUpdate();
        items.resize(row_count);
        int conn_index = 0;
        int bit_index = 0;
        for (int row = 0; row < row_count; row++) {
          Variant type_variant =
            ap_grid->ActiveView->DataController->Values[row][a_type_col];
          String type_bstr;
          if (!type_variant.IsNull()) {
            type_bstr = type_variant;
          }

          string_type type_str = irs::str_conv<string_type>(type_bstr);
          type_t type = type_float;
          str_to_type(type_str, &type);

          const int var_index = row;
          int conn_index_grid = conn_index;
          int bit_index_grid = bit_index;
          bool is_cur_item_bit = false;

          switch (type) {
            case type_bit: {
              add_conn(bit_vec, var_index, conn_index, bit_index);
              is_cur_item_bit = true;
            } break;
            case type_bool: {
              add_conn(bool_vec, var_index, conn_index, bit_index,
                type_bool);
            } break;
            case type_i8: {
              add_conn(i8_vec, var_index, conn_index, bit_index,
                type_i8);
            } break;
            case type_u8: {
              add_conn(u8_vec, var_index, conn_index, bit_index,
                type_u8);
            } break;
            case type_i16: {
              add_conn(i16_vec, var_index, conn_index, bit_index,
                type_i16);
            } break;
            case type_u16: {
              add_conn(u16_vec, var_index, conn_index, bit_index,
                type_u16);
            } break;
            case type_i32: {
              add_conn(i32_vec, var_index, conn_index, bit_index,
                type_i32);
            } break;
            case type_u32: {
              add_conn(u32_vec, var_index, conn_index, bit_index,
                type_u32);
            } break;
            case type_i64: {
              add_conn(i64_vec, var_index, conn_index, bit_index,
                type_i64);
            } break;
            case type_u64: {
              add_conn(u64_vec, var_index, conn_index, bit_index,
                type_u64);
            } break;
            case type_float: {
              add_conn(float_vec, var_index, conn_index, bit_index,
                type_float);
            } break;
            case type_double: {
              add_conn(double_vec, var_index, conn_index, bit_index,
                type_double);
            } break;
            case type_long_double: {
              add_conn(long_double_vec, var_index, conn_index, bit_index,
                type_long_double);
            } break;
          }

          if (!is_cur_item_bit) {
            if (bit_index_grid != 0) {
              bit_index_grid = 0;
              conn_index_grid++;
            }
          }
          String index_bstr =
            /*String(var_index) + "/" + */String(conn_index_grid);
          if (is_cur_item_bit) {
            index_bstr += "-" + String(bit_index_grid);
          }
          ap_grid->ActiveView->DataController->Values[row][a_index_col] =
            index_bstr;
        }
        ap_grid->ActiveView->EndUpdate();
        return conn_index;
      }
    }; //netconn_t

    struct param_box_tune_t {
      param_box_base_t* mp_param_box;

      param_box_tune_t(param_box_base_t* ap_param_box);
    };

    static const int m_grid_size = 1;
    static const int m_header_row = 0;

    enum {
      m_index_col = 0,
      m_network_pos_col,
      m_name_col,
      m_type_col,
      m_value_col,
      m_chart_col,
      m_category_col,
      m_col_count
    };

    static const int m_index_col_width = 45;
    static const int m_network_pos_col_width = 45;
    static const int m_name_col_width = 150;
    static const int m_type_col_width = 80;
    static const int m_value_col_width = 150;
    static const int m_chart_col_width = 40;

    enum {
      m_csv_table_conn_index_col_index = 0,
      m_csv_table_bit_index_col_index = 1,
      m_csv_table_name_col_index = 2,
      m_csv_table_type_col_index = 3,
      m_csv_table_value_index_col_index = 4,
      m_csv_table_col_count = m_csv_table_value_index_col_index + 1
    };

    TForm* mp_form;
    TPanel* mp_top_panel;
    TOpenDialog* mp_open_dialog;
    const String m_csv_files_filter;
    TButton* mp_load_btn;
    TButton* mp_start_btn;
    TButton* mp_chart_btn;
    TButton* mp_options_btn;
    TButton* mp_load_table_values_btn;
    TButton* mp_save_table_values_btn;
    TMemo* mp_log_memo;
    TSplitter* mp_splitter;
    TMenuItem* mp_grid_popup_insert_item;
    TMenuItem* mp_grid_popup_delete_item;
    TMenuItem* mp_grid_popup_set_category_item;
    TPopupMenu* mp_grid_popup;
    TcxGrid* mp_vars_grid;
    TcxGridTableView* mp_view;
    TcxCustomDataController* mp_controller;
    TcxCustomGridTableController* mp_table_controller;
    TcxComboBoxProperties* mp_cx_combo_box_properties;
    TcxGridColumn* mp_index_column;
    TcxGridColumn* mp_network_pos_column;
    TcxGridColumn* mp_category_column;
    TcxGridColumn* mp_name_column;
    TcxGridColumn* mp_type_column;
    TcxGridColumn* mp_value_column;
    TcxGridColumn* mp_chart_column;

    const char_type* mp_read_on_text;
    const char_type* mp_read_off_text;

    form_type_t m_form_type;
    loop_timer_t m_read_loop_timer;
    irs::memobuf m_buf;
    ostream m_out;
    TEncoding* mp_encoding;
    irs::ini_file_t m_ini_file;
    irs::handle_t<vars_ini_file_t> mp_vars_ini_file;
    string_type m_device_name;
    const string_type m_grid_options_file_ext;
    string_type m_grid_options_file_name;
    irs::mxdata_t *mp_data;
    bool m_first_connect;
    bool m_refresh_grid;
    netconn_t m_netconn;
    bool is_edit_mode;
    bool is_write_var;
    irs::handle_t<chart_window_t> mp_local_chart;
    chart_window_t* mp_chart;
    csv_file m_csv_file;
    csvwork::csv_file_synchro_t m_csv_open_file;
    table_string_t m_table;
    bool m_is_edit_chart_items;
    bool m_refresh_chart_items;
    measure_time_t m_time;
    double m_shift_time;
    double m_chart_time;
    double m_minus_shift_time;
    bool m_refresh_table;
    bool m_is_lock;
    int m_chart_row;

    map<string_type, bool> m_chart_names;
    map<string_type, bool> m_csv_names;
    measure_time_t m_timer;
    string_type m_ini_section_prefix;
    const string_type m_ini_options_section;
    bool m_start;
    event_t m_refresh_csv_state_event;
    bool m_is_csv_opened;
    event_t* mp_event;
    event_t m_inner_options_event;
    handle_t<param_box_base_t> mp_param_box;
    param_box_tune_t m_param_box_tune;
    bool m_is_csv_on;

    template <class T>
    void out_number(ostream_t& a_stream, const T& a_value);
    String var_to_bstr(int a_var_index);
    void bstr_to_var(int a_var_index, const String& a_bstr_val);
    long double var_to_long_double(int a_var_index);
    void load_grid_options();
    void save_grid_options();
    void fill_grid_index_col();
    bool is_saveable_col(int a_col);
    void save_grid_row(int a_row);
    void open_csv();
    void close_csv();

    void __fastcall cxGridTableViewColumnNamePropertiesValidate(TObject *Sender,
      Variant &DisplayValue, TCaption &ErrorText, bool &Error);
    void __fastcall cxGridTableView1ColumnTypePropertiesChange(TObject *Sender);
    void __fastcall cxGridTableViewColumnValuePropertiesChange(TObject *Sender);
    void __fastcall cxGridTableViewColumnValuePropertiesValidate(TObject *Sender,
      Variant &DisplayValue, TCaption &ErrorText, bool &Error);
    void __fastcall cxGridTableViewColumnChartPropertiesChange(TObject *Sender);

    void __fastcall VarsGridKeyDown(
      TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall ChartBtnClick(TObject *Sender);
    void __fastcall CsvSaveBtnClick(TObject *Sender);
    void __fastcall CsvLoadBtnClick(TObject *Sender);
    void __fastcall OptionsBtnClick(TObject *Sender);
    void __fastcall LoadTableValuesBtnClick(TObject *Sender);
    void __fastcall SaveTableValuesBtnClick(TObject *Sender);
    void __fastcall GridInsertClick(TObject *Sender);
    void __fastcall GridDeleteClick(TObject *Sender);
    void __fastcall GridSetCategoryClick(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void inner_options_apply();
    void refresh_chart_items();
    bool find_netconn_item(int a_conn_index, int a_bit_index,
      int* ap_index);
    string_type generate_unique_variable_name();
    bool is_variable_exists(const string_type& a_name);
    string_type get_variable_name(size_type a_row);
    string_type get_chart_name(size_type a_row);
    string_type make_chart_name(const string_type& a_variable_name);
    bool get_chart_enabled_status(size_type a_row) const;
  }; //controls_t

  form_type_t m_form_type;
  chart_window_t* mp_extern_chart;
  auto_ptr<TForm> mp_form_auto;

  destructor_test_t m_destructor_test;

  //***
  TForm *mp_form;
  auto_ptr<controls_t> mp_controls;
  const string_type m_ini_name;
  string_type m_ini_section_prefix;
  const string_type m_ini_options_section;
  irs::memobuf m_log_buf;
  counter_t m_update_time_cnt;
  global_log_connect_t m_global_log_connect;
}; //view_t

} // namespace tstlan

//! @}

} // namespace irs

#endif // IRS_USE_DEV_EXPRESS

#endif //tstlan5libH
