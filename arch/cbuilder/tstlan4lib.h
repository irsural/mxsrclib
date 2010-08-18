// Тест сети 4 - библиотека
// Дата: 06.08.2010
// Дата создания: 17.09.2009

#ifndef tstlan4libH
#define tstlan4libH

#include <irsdefs.h>

//#include <Forms.hpp>
#include <tstlan4abs.h>
#include <irsstd.h>
#include <timer.h>
#include <mxini.h>
#include <MxChart.h>
#include <csvwork.h>
#include <cbsysutils.h>
#include <irstable.h>

#include <irsfinal.h>

namespace irs {

class tstlan4_t: public tstlan4_base_t
{
public:
  typedef string_t string_type;
  typedef char_t char_type;
private:
  static const char_type* def_ini_name();
  static const char_type* def_ini_section_prefix();
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
    const string_type& a_ini_section_prefix = def_ini_section_prefix(),
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
  virtual void save_conf();
private:
  class controls_t: public TObject
  {
  public:
    //controls_t(const controls_t& a_controls, TForm *ap_form);
    controls_t(
      TForm *ap_form,
      const string_type& a_ini_name,
      const string_type& a_ini_section_prefix,
      irs::chart::builder_chart_window_t::stay_on_top_t a_stay_on_top,
      counter_t a_update_time_cnt
    );
    virtual __fastcall ~controls_t();
    void connect(mxdata_t *ap_data);
    void save_conf();
    // Консоль внутри tstlan4
    TMemo* log();
    void tick();
  private:
    struct netconn_t {
      struct item_t {
        enum type_t { type_unknown, type_bit, type_bool ,type_u8, type_i16,
          type_u16, type_u32, type_i32, type_u64, type_float, type_double };

        type_t type;
        int index;

        item_t():
          type(type_unknown),
          index(0)
        {
        }
      }; //item_t

      static const int bit_in_byte = 8;

      const char *name_bit;
      const char *name_bool;
      const char *name_u8;
      const char *name_i16;
      const char *name_u16;
      const char *name_i32;
      const char *name_float;
      const char *name_double;

      vector<bit_data_t> bit_vec;
      vector<conn_data_t<bool> > bool_vec;
      vector<conn_data_t<irs_u8> > u8_vec;
      vector<conn_data_t<irs_i16> > i16_vec;
      vector<conn_data_t<irs_u16> > u16_vec;
      vector<conn_data_t<irs_i32> > i32_vec;
      vector<conn_data_t<float> > float_vec;
      vector<conn_data_t<double> > double_vec;

      vector<item_t> items;
      //vector<type_t, size_t> m_type_sizes;
      mxdata_t* mp_data;

      netconn_t():
        name_bit("bit"),
        name_bool("bool"),
        name_u8("u8"),
        name_i16("i16"),
        name_u16("u16"),
        name_i32("long"),
        name_float("float"),
        name_double("double"),

        bit_vec(),
        bool_vec(),
        u8_vec(),
        i16_vec(),
        u16_vec(),
        i32_vec(),
        float_vec(),
        double_vec(),

        items(),
        mp_data(IRS_NULL)
      {
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
        } else {
          items[a_var_index].type = item_t::type_unknown;
          items[a_var_index].index = 0;
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
        } else {
          items[a_var_index].type = item_t::type_unknown;
          items[a_var_index].index = 0;
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
          String type_str = types->Strings[row];
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
          } else if (type_str == name_u8) {
            add_conn(u8_vec, var_index, conn_index, bit_index,
              item_t::type_u8);
          } else if (type_str == name_i16) {
            add_conn(i16_vec, var_index, conn_index, bit_index,
              item_t::type_i16);
          } else if (type_str == name_u16) {
            add_conn(u16_vec, var_index, conn_index, bit_index,
              item_t::type_u16);
          } else if (type_str == name_i32) {
            add_conn(i32_vec, var_index, conn_index, bit_index,
              item_t::type_i32);
          } else if (type_str == name_double) {
            add_conn(double_vec, var_index, conn_index, bit_index,
              item_t::type_double);
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

    TForm* mp_form;
    TPanel* mp_top_panel;
    TOpenDialog* mp_open_dialog;
    TButton* mp_load_btn;
    TButton* mp_start_btn;
    TButton* mp_chart_btn;
    TMemo* mp_log_memo;
    TSplitter* mp_splitter;
    TMenuItem* mp_grid_popup_insert_item;
    TMenuItem* mp_grid_popup_delete_item;
    TPopupMenu* mp_grid_popup;
    TStringGrid* mp_vars_grid;

    const char *mp_read_on_text;
    const char *mp_read_off_text;

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
    const string_type m_ini_section_prefix;
    bool m_start;
    bool m_first;
    bool m_is_close_csv;

    template <class T>
    void integer_to_string(const T& a_value, string_type* ap_string);
    String var_to_bstr(int a_var_index);
    void bstr_to_var(int a_var_index, const String& a_bstr_val);
    double var_to_double(int a_var_index);
    void fill_grid_index_col();
    bool is_saveable_col(int a_col);
    void save_grid_row(int a_row);
    void creation_csv();
    void __fastcall VarsGridGetEditText(
      TObject *Sender, int ACol, int ARow, String &Value);
    void __fastcall VarsGridKeyDown(
      TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall ChartBtnClick(TObject *Sender);
    void __fastcall CsvSaveBtnClick(TObject *Sender);
    void __fastcall CsvLoadBtnClick(TObject *Sender);
    void __fastcall GridInsertClick(TObject *Sender);
    void __fastcall GridDeleteClick(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
  }; //controls_t

  form_type_t m_form_type;
  auto_ptr<TForm> mp_form_auto;
  TForm *mp_form;
  auto_ptr<controls_t> mp_controls;
  const string_type m_ini_name;
  const string_type m_ini_section_prefix;
  irs::memobuf m_log_buf;
  counter_t m_update_time_cnt;
  global_log_connect_t m_global_log_connect;
}; //tstlan4_t

} //namespace irs

#endif //tstlan4libH
