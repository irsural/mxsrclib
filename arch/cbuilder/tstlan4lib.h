//! \file
//! \ingroup graphical_user_interface_group
//! \brief ���� ���� 4 - ����������
//!
//! ����: 22.05.2011\n
//! ���� ��������: 17.09.2009

#ifndef tstlan4libH
#define tstlan4libH

#include <irsdefs.h>

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

#include <irsfinal.h>

namespace irs {

//! \addtogroup graphical_user_interface_group
//! @{

struct destructor_test_t
{
  destructor_test_t()
  {
    irs::mlog() << "constructor" << endl;
  }
  ~destructor_test_t()
  {
    irs::mlog() << "destructor" << endl;
  }
};

class tstlan4_t: public tstlan4_base_t
{
public:
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

  tstlan4_t(const tstlan4_t& a_tstlan4);
  tstlan4_t(
    form_type_t a_form_type = ft_internal,
    const string_type& a_ini_name = def_ini_name(),
    const string_type& a_ini_section = def_ini_section(),
    counter_t a_update_time_cnt = irs::make_cnt_ms(200),
    global_log_connect_t a_global_log_connect = global_log_unchange
  );
  // ����������� � ����� � �������� ��������� ����������
  // ����� �������������� � ������ ����� ��� ������ deinit
  // �� ���������� ����� ��� ���������� ������������
  void init(TForm *ap_form);
  // ����������� ���� ����������� �����
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
    //controls_t(const controls_t& a_controls, TForm *ap_form);
    controls_t(
      form_type_t a_form_type,
      TForm *ap_form,
      const string_type& a_ini_name,
      const string_type& a_ini_section,
      irs::chart::builder_chart_window_t::stay_on_top_t a_stay_on_top,
      counter_t a_update_time_cnt
    );
    virtual __fastcall ~controls_t();
    void connect(mxdata_t *ap_data);
    void update_time(const irs_i32 a_update_time);
    void resize_chart(const irs_u32 a_size);
    // ������� ������ tstlan4
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
        enum type_t { type_unknown, type_bit, type_bool, type_i8,
          type_u8, type_i16, type_u16, type_i32, type_u32, type_i64, type_u64,
          type_float,
          type_double,
          type_long_double };

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

      const string_type name_bit;
      const string_type name_bool;
      const string_type name_i8;
      const string_type name_u8;
      const string_type name_i16;
      const string_type name_u16;
      const string_type name_long;
      const string_type name_i32;
      const string_type name_u32;
      const string_type name_i64;
      const string_type name_u64;
      const string_type name_float;
      const string_type name_double;
      const string_type name_long_double;

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
      //vector<type_t, size_t> m_type_sizes;
      mxdata_t* mp_data;

      netconn_t():
        name_bit(irst("bit")),
        name_bool(irst("bool")),
        name_i8(irst("i8")),
        name_u8(irst("u8")),
        name_i16(irst("i16")),
        name_u16(irst("u16")),
        name_long(irst("long")),
        name_i32(irst("i32")),
        name_u32(irst("u32")),
        name_i64(irst("i64")),
        name_u64(irst("u64")),
        name_float(irst("float")),
        name_double(irst("double")),
        name_long_double(irst("long double")),

        bit_vec(),
        bool_vec(),
        i8_vec(),
        u8_vec(),
        i16_vec(),
        u16_vec(),
        i32_vec(),
        u32_vec(),
        i64_vec(),
        u64_vec(),
        float_vec(),
        double_vec(),
        long_double_vec(),
        items(),
        mp_data(IRS_NULL)
      {
      }
      string_type type_to_str(item_t::type_t a_type) const
      {
        switch (a_type) {
          case item_t::type_bit: {
            return name_bit;
          } break;
          case item_t::type_bool: {
            return name_bool;
          } break;
          case item_t::type_i8: {
            return name_i8;
          } break;
          case item_t::type_u8: {
            return name_u8;
          } break;
          case item_t::type_i16: {
            return name_i16;
          } break;
          case item_t::type_u16: {
            return name_u16;
          } break;
          case item_t::type_i32: {
            return name_i32;
          } break;
          case item_t::type_u32: {
            return name_u32;
          } break;
          case item_t::type_i64: {
            return name_i64;
          } break;
          case item_t::type_u64: {
            return name_u64;
          } break;
          case item_t::type_float: {
            return name_float;
          } break;
          case item_t::type_double: {
            return name_double;
          } break;
          case item_t::type_long_double: {
            return name_long_double;
          } break;
        }
        return string_type();
      }
      bool str_to_type(const string_type& a_str, item_t::type_t* ap_type)
      {
        bool status = true;
        if (a_str == name_bit) {
          *ap_type = item_t::type_bit;
        } else if (a_str == name_bool) {
          *ap_type = item_t::type_bool;
        } else if (a_str == name_i8) {
          *ap_type = item_t::type_i8;
        } else if (a_str == name_u8) {
          *ap_type = item_t::type_u8;
        } else if (a_str == name_i16) {
          *ap_type = item_t::type_i16;
        } else if (a_str == name_u16) {
          *ap_type = item_t::type_u16;
        } else if ((a_str == name_long) || (a_str == name_i32)) {
          *ap_type = item_t::type_i32;
        } else if (a_str == name_u32) {
          *ap_type = item_t::type_u32;
        } else if (a_str == name_i64) {
          *ap_type = item_t::type_i64;
        } else if (a_str == name_u64) {
          *ap_type = item_t::type_u64;
        } else if (a_str == name_float) {
          *ap_type = item_t::type_float;
        } else if (a_str == name_double) {
          *ap_type = item_t::type_double;
        } else if (a_str == name_long_double) {
          *ap_type = item_t::type_long_double;
        } else {
          status = false;
        }
        return status;
      }
      void add_conn(vector<bit_data_t>& a_vec, int a_var_index,
        int& a_conn_index, int& a_bit_index)
      {
        if ((size_t)a_conn_index < mp_data->size()) {
          items[a_var_index].type = item_t::type_bit;
          items[a_var_index].index = a_vec.size();
          a_vec.push_back(bit_data_t());
          a_conn_index = a_vec.back().connect(mp_data, a_conn_index,
            a_bit_index);
          a_bit_index++;
          if (a_bit_index >= bit_in_byte) {
            a_bit_index = 0;
            a_conn_index++;
          }
          items[a_var_index].conn_index = a_conn_index;
          items[a_var_index].bit_index = a_bit_index;
        } else {
          items[a_var_index].type = item_t::type_unknown;
          items[a_var_index].index = 0;
          items[a_var_index].conn_index = 0;
          items[a_var_index].bit_index = 0;
        }
      }
      template <class T>
      void add_conn(vector<conn_data_t<T> >& a_vec, int a_var_index,
        int& a_conn_index, int& a_bit_index, item_t::type_t a_type)
      {
        if (a_bit_index != 0) {
          a_bit_index = 0;
          a_conn_index++;
        }
        if (a_conn_index + sizeof(T) <= mp_data->size()) {
          items[a_var_index].type = a_type;
          items[a_var_index].index = a_vec.size();
          a_vec.push_back(conn_data_t<T>());
          a_conn_index = a_vec.back().connect(mp_data, a_conn_index);
          items[a_var_index].conn_index = a_conn_index;
          items[a_var_index].bit_index = 0;
        } else {
          items[a_var_index].type = item_t::type_unknown;
          items[a_var_index].index = 0;
          items[a_var_index].conn_index = 0;
          items[a_var_index].bit_index = 0;
        }
      }
      int connect(mxdata_t* ap_data, TStringGrid* ap_grid, int a_type_col,
        int a_index_col)
      {
        mp_data = ap_data;

        bit_vec.clear();
        bool_vec.clear();
        u8_vec.clear();
        i32_vec.clear();
        float_vec.clear();
        double_vec.clear();

        items.clear();

        TStrings *types = ap_grid->Cols[a_type_col];
        TStrings *indexes = ap_grid->Cols[a_index_col];
        int row_count = ap_grid->RowCount;

        items.resize(row_count);
        int conn_index = 0;
        int bit_index = 0;
        for (int row = 1; row < row_count; row++) {
          string_type type_str =
            irs::str_conv<string_type>(types->Strings[row]);
          const int var_index = row - 1;
          int conn_index_grid = conn_index;
          int bit_index_grid = bit_index;
          bool is_cur_item_bit = false;

          if (type_str == name_bit) {
            add_conn(bit_vec, var_index, conn_index, bit_index);
            is_cur_item_bit = true;
          } else if (type_str == name_bool) {
            add_conn(bool_vec, var_index, conn_index, bit_index,
              item_t::type_bool);
          } else if (type_str == name_i8) {
            add_conn(i8_vec, var_index, conn_index, bit_index,
              item_t::type_i8);
          } else if (type_str == name_u8) {
            add_conn(u8_vec, var_index, conn_index, bit_index,
              item_t::type_u8);
          } else if (type_str == name_i16) {
            add_conn(i16_vec, var_index, conn_index, bit_index,
              item_t::type_i16);
          } else if (type_str == name_u16) {
            add_conn(u16_vec, var_index, conn_index, bit_index,
              item_t::type_u16);
          } else if ((type_str == name_long) || (type_str == name_i32)) {
            add_conn(i32_vec, var_index, conn_index, bit_index,
              item_t::type_i32);
          } else if (type_str == name_u32) {
            add_conn(u32_vec, var_index, conn_index, bit_index,
              item_t::type_u32);
          } else if (type_str == name_i64) {
            add_conn(i64_vec, var_index, conn_index, bit_index,
              item_t::type_i64);
          } else if (type_str == name_u64) {
            add_conn(u64_vec, var_index, conn_index, bit_index,
              item_t::type_u64);
          } else if (type_str == name_double) {
            add_conn(double_vec, var_index, conn_index, bit_index,
              item_t::type_double);
          } else if (type_str == name_long_double) {
            add_conn(long_double_vec, var_index, conn_index, bit_index,
              item_t::type_long_double);
          } else { //type_float
            add_conn(float_vec, var_index, conn_index, bit_index,
              item_t::type_float);
          }

          if (!is_cur_item_bit) {
            if (bit_index_grid != 0) {
              bit_index_grid = 0;
              conn_index_grid++;
            }
          }
          String index_bstr =
            String(var_index) + "/" + String(conn_index_grid);
          if (is_cur_item_bit) {
            index_bstr += "-" + String(bit_index_grid);
          }
          indexes->Strings[row] = index_bstr;
        }
        return conn_index;
      }
    }; //netconn_t

    struct param_box_tune_t {
      param_box_base_t* mp_param_box;

      param_box_tune_t(param_box_base_t* ap_param_box);
    };


    static const int m_vars_count_start = 1;
    static const int m_header_size = 1;
    static const int m_grid_size = m_vars_count_start + m_header_size;
    static const int m_header_row = 0;
    static const int m_index_col = 0;
    static const int m_name_col = m_index_col + 1;
    static const int m_type_col = m_name_col + 1;
    static const int m_value_col = m_type_col + 1;
    static const int m_chart_col = m_value_col + 1;
    static const int m_col_count = m_chart_col + 1;

    static const int m_index_col_width = 60;
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
    TPopupMenu* mp_grid_popup;
    TStringGrid* mp_vars_grid;

    const char_type* mp_read_on_text;
    const char_type* mp_read_off_text;

    form_type_t m_form_type;
    loop_timer_t m_read_loop_timer;
    irs::memobuf m_buf;
    ostream m_out;
    irs::ini_file_t m_ini_file;
    irs::mxdata_t *mp_data;
    bool m_first_connect;
    bool m_refresh_grid;
    netconn_t m_netconn;
    bool is_edit_mode;
    bool is_write_var;
    int write_var_index;
    irs::chart::builder_chart_window_t m_builder_chart;
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
    bool m_saveable_is_edit;
    int m_saveable_col;
    int m_saveable_row;
    bool m_is_lock;
    int m_chart_row;
    bool m_is_grid_edit;
    int m_grid_edit_col;
    int m_grid_edit_row;
    map<string_type, bool> m_chart_names;
    map<string_type, bool> m_csv_names;
    measure_time_t m_timer;
    string_type m_ini_section;
    bool m_start;
    //bool m_first;
    event_t m_refresh_csv_state_event;
    bool m_is_csv_opened;
    event_t* mp_event;
    event_t m_inner_options_event;
    handle_t<param_box_base_t> mp_param_box;
    param_box_tune_t m_param_box_tune;
    bool m_is_csv_on;

    template <class T>
    void out_number(ostream_t& a_stream, const T& a_value);
    template <class T>
    void integer_to_string_helper(const T& a_value, string_type* ap_string);
    void irs::tstlan4_t::controls_t::integer_to_string(bool a_value,
      string_type* ap_string);
    void irs::tstlan4_t::controls_t::integer_to_string(unsigned char a_value,
      string_type* ap_string);
    void irs::tstlan4_t::controls_t::integer_to_string(signed char a_value,
      string_type* ap_string);
    void irs::tstlan4_t::controls_t::integer_to_string(unsigned short a_value,
      string_type* ap_string);
    void irs::tstlan4_t::controls_t::integer_to_string(signed short a_value,
      string_type* ap_string);
    void irs::tstlan4_t::controls_t::integer_to_string(unsigned int a_value,
      string_type* ap_string);
    void irs::tstlan4_t::controls_t::integer_to_string(signed int a_value,
      string_type* ap_string);
    void irs::tstlan4_t::controls_t::integer_to_string(unsigned long a_value,
      string_type* ap_string);
    void irs::tstlan4_t::controls_t::integer_to_string(signed long a_value,
      string_type* ap_string);
    void irs::tstlan4_t::controls_t::integer_to_string(long long a_value,
      string_type* ap_string);
    void irs::tstlan4_t::controls_t::integer_to_string(
      unsigned long long a_value, string_type* ap_string);
    String var_to_bstr(int a_var_index);
    void bstr_to_var(int a_var_index, const String& a_bstr_val);
    long double var_to_long_double(int a_var_index);
    void fill_grid_index_col();
    bool is_saveable_col(int a_col);
    void save_grid_row(int a_row);
    void open_csv();
    void close_csv();
    void __fastcall VarsGridGetEditText(
      TObject *Sender, int ACol, int ARow, String &Value);
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
    void __fastcall FormHide(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void inner_options_apply();
  }; //controls_t

  form_type_t m_form_type;
  auto_ptr<TForm> mp_form_auto;

  destructor_test_t m_destructor_test;

  //***
  TForm *mp_form;
  auto_ptr<controls_t> mp_controls;
  const string_type m_ini_name;
  const string_type m_ini_section;
  irs::memobuf m_log_buf;
  counter_t m_update_time_cnt;
  global_log_connect_t m_global_log_connect;
}; //tstlan4_t

//! @}

} //namespace irs

#endif //tstlan4libH
