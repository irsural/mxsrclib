// Тест сети 4 - библиотека
// Дата: 15.09.2010
// Дата создания: 17.09.2009

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <tstlan4lib.h>
#include <irstime.h>
#include <MxBase.h>
#include <irserror.h>
#include <irslimits.h>

#include <irsfinal.h>

irs::tstlan4_t::tstlan4_t(const tstlan4_t& a_tstlan4):
  m_form_type(a_tstlan4.m_form_type),
  mp_form_auto(IRS_NULL),
  mp_form(IRS_NULL),
  mp_controls(IRS_NULL),
  m_ini_name(a_tstlan4.m_ini_name),
  m_ini_section_prefix(a_tstlan4.m_ini_section_prefix),
  m_log_buf(a_tstlan4.m_log_buf),
  m_update_time_cnt(a_tstlan4.m_update_time_cnt),
  m_global_log_connect(a_tstlan4.m_global_log_connect)
{
  if (m_form_type == ft_internal) {
    mp_form_auto.reset(new TForm(IRS_NULL, 1));
    mp_form = mp_form_auto.get();
    TForm *src_form = a_tstlan4.mp_form_auto.get();
    irs::mxcopy(mp_form, src_form);
  }
  init(mp_form_auto.get());
}
irs::tstlan4_t::tstlan4_t(
  form_type_t a_form_type,
  const string_type& a_ini_name,
  const string_type& a_ini_section_prefix,
  counter_t a_update_time_cnt,
  global_log_connect_t a_global_log_connect
):
  m_form_type(a_form_type),
  mp_form_auto(IRS_NULL),
  mp_form(IRS_NULL),
  mp_controls(IRS_NULL),
  m_ini_name(a_ini_name),
  m_ini_section_prefix(a_ini_section_prefix),
  m_log_buf(IRS_NULL, 100),
  m_update_time_cnt(a_update_time_cnt),
  m_global_log_connect(a_global_log_connect)
{
  if (m_form_type == ft_internal) {
    mp_form_auto.reset(new TForm(IRS_NULL, 1));
    mp_form = mp_form_auto.get();
    mp_form->Width = 600;
    mp_form->Height = 800;
    mp_form->Position = poDesktopCenter;
    mp_form->Caption = "Тест сети 4";
    init(mp_form_auto.get());
  }
}
// Подключение к форме и создание элементов управления
// Можно переподключать к другой форме без вызова deinit
// На предыдущей форме все компоненты уничтожаются
void irs::tstlan4_t::init(TForm *ap_form)
{
  irs::chart::builder_chart_window_t::stay_on_top_t stay_on_top =
    irs::chart::builder_chart_window_t::stay_on_top_on;
  if (m_form_type == ft_internal) {
    stay_on_top = irs::chart::builder_chart_window_t::stay_on_top_off;
  }
  mp_form = ap_form;
  mp_controls.reset(new controls_t(mp_form, m_ini_name, m_ini_section_prefix,
    stay_on_top, m_update_time_cnt));
  if (m_global_log_connect == global_log_connect) {
    m_log_buf.connect(mp_controls->log());
    irs::mlog().rdbuf(&m_log_buf);
  }
}
// Уничтожение всех компонентов формы
void irs::tstlan4_t::deinit()
{
  mp_controls.reset();
}
void irs::tstlan4_t::tick()
{
  mp_controls->tick();
}
const irs::tstlan4_t::char_type* irs::tstlan4_t::def_ini_name()
{
  return irst("tstlan3.ini");
}
const irs::tstlan4_t::char_type* irs::tstlan4_t::def_ini_section_prefix()
{
  return irst("");
}

void irs::tstlan4_t::show()
{
  mp_form->ShowModal();
}
void irs::tstlan4_t::hide()
{
  mp_form->Hide();
}
irs::rect_t irs::tstlan4_t::position() const
{
  return rect_t();
}
void irs::tstlan4_t::set_position(const rect_t &a_position)
{
}
void irs::tstlan4_t::connect(mxdata_t *ap_data)
{
  mp_controls->connect(ap_data);
}
void irs::tstlan4_t::save_conf()
{
  mp_controls->save_conf();
}
void irs::tstlan4_t::update_time(const counter_t a_update_time)
{
  mp_controls->update_time(a_update_time);
}
void irs::tstlan4_t::set_refresh_time_chart(const irs_i32 a_refresh_time)
{
  mp_controls->set_refresh_time_chart(a_refresh_time);
}
void irs::tstlan4_t::resize_chart(const irs_u32 a_size)
{
  mp_controls->resize_chart(a_size);
}

//TComponent* const zero_comp = IRS_NULL;
// Компонентов формы
irs::tstlan4_t::controls_t::controls_t(
  TForm *ap_form,
  const string_type& a_ini_name,
  const string_type& a_ini_section_prefix,
  irs::chart::builder_chart_window_t::stay_on_top_t a_stay_on_top,
  counter_t a_update_time_cnt
):
  mp_form(ap_form),
  mp_top_panel(new TPanel(mp_form)),
  mp_open_dialog(new TOpenDialog(mp_form)),
  mp_load_btn(new TButton(mp_form)),
  mp_start_btn(new TButton(mp_form)),
  mp_chart_btn(new TButton(mp_form)),
  mp_log_memo(new TMemo(mp_form)),
  mp_splitter(new TSplitter(mp_form)),
  mp_grid_popup_insert_item(new TMenuItem(mp_form)),
  mp_grid_popup_delete_item(new TMenuItem(mp_form)),
  mp_grid_popup(new TPopupMenu(mp_form)),
  mp_vars_grid(new TStringGrid(mp_form)),

  mp_read_on_text("Пуск"),
  mp_read_off_text("Стоп"),

  m_read_loop_timer(a_update_time_cnt),
  m_buf(),
  m_out(&m_buf),
  m_ini_file(),
  m_first_connect(true),
  m_refresh_grid(true),
  m_netconn(),
  is_edit_mode(false),
  is_write_var(false),
  write_var_index(0),
  m_builder_chart(10000, 1000, a_stay_on_top),
  mp_chart(&m_builder_chart),
  m_csv_file(),
  m_csv_open_file(),
  m_table(),
  m_is_edit_chart_items(true),
  m_refresh_chart_items(true),
  m_time(),
  m_shift_time(0),
  m_refresh_table(true),
  m_saveable_is_edit(false),
  m_saveable_col(0),
  m_saveable_row(0),
  m_is_lock(true),
  m_chart_row(0),
  m_is_grid_edit(false),
  m_grid_edit_col(0),
  m_grid_edit_row(0),
  m_chart_names(),
  m_csv_names(),
  m_timer(),
  m_ini_section_prefix(a_ini_section_prefix),
  m_start(false),
  m_first(true),
  m_is_created_csv(false)
{
  m_buf.connect(mp_log_memo);
  m_ini_file.set_ini_name(a_ini_name.c_str());
  m_ini_file.set_section(String(m_ini_section_prefix.c_str()) + "Vars");
  m_ini_file.add("", mp_vars_grid, "Name_", m_name_col);
  m_ini_file.add("", mp_vars_grid, "Type_", m_type_col);
  m_ini_file.add("", mp_vars_grid, "Graph_", m_chart_col);

  const int btn_gap = 10;

  mp_form->OnShow = FormShow;
  mp_form->OnHide = FormHide;

  mp_top_panel->Align = alTop;
  mp_top_panel->Parent = mp_form;
  mp_top_panel->Top = 0;

  mp_start_btn->Left = btn_gap;
  mp_start_btn->Top = mp_top_panel->Height/2 - mp_start_btn->Height/2;
  mp_start_btn->Caption = mp_read_off_text;
  mp_start_btn->Parent = mp_top_panel;
  mp_start_btn->OnClick = CsvSaveBtnClick;

  mp_chart_btn->Left = mp_start_btn->Left + mp_start_btn->Width + btn_gap;
  mp_chart_btn->Top = mp_top_panel->Height/2 - mp_chart_btn->Height/2;
  mp_chart_btn->Caption = "График";
  mp_chart_btn->Parent = mp_top_panel;
  mp_chart_btn->OnClick = ChartBtnClick;

  mp_load_btn->Left = mp_chart_btn->Left + mp_chart_btn->Width + btn_gap;
  mp_load_btn->Top = mp_top_panel->Height/2 - mp_load_btn->Height/2;
  mp_load_btn->Caption = "Загрузить";
  mp_load_btn->Parent = mp_top_panel;
  mp_load_btn->OnClick = CsvLoadBtnClick;

  mp_log_memo->Align = alTop;
  mp_log_memo->Height = 100;
  mp_log_memo->Parent = mp_form;
  mp_log_memo->Top = mp_top_panel->Top + mp_log_memo->Height;
  mp_log_memo->ScrollBars = ssVertical;

  mp_splitter->Align = alTop;
  mp_splitter->Top = mp_log_memo->Top + mp_log_memo->Height;
  //mp_splitter->Height = 10;
  mp_splitter->Parent = mp_form;

  mp_grid_popup_insert_item->Caption = "Добавить строку";
  mp_grid_popup_insert_item->OnClick = GridInsertClick;
  mp_grid_popup_delete_item->Caption = "Удалить строку";
  mp_grid_popup_delete_item->OnClick = GridDeleteClick;
  mp_grid_popup->Items->Add(mp_grid_popup_insert_item);
  mp_grid_popup->Items->Add(mp_grid_popup_delete_item);

  mp_vars_grid->Align = alClient;
  mp_vars_grid->DefaultRowHeight = 17;
  mp_vars_grid->RowCount = m_grid_size;
  mp_vars_grid->ColCount = m_col_count;
  mp_vars_grid->Options = mp_vars_grid->Options << goEditing;
  mp_vars_grid->OnGetEditText = VarsGridGetEditText;
  mp_vars_grid->OnKeyDown = VarsGridKeyDown;
  mp_vars_grid->Rows[m_header_row]->Strings[m_index_col] = "№";
  mp_vars_grid->ColWidths[m_index_col] = m_index_col_width;
  mp_vars_grid->Rows[m_header_row]->Strings[m_name_col] = "Имя";
  mp_vars_grid->ColWidths[m_name_col] = m_name_col_width;
  mp_vars_grid->Rows[m_header_row]->Strings[m_type_col] = "Тип";
  mp_vars_grid->ColWidths[m_type_col] = m_type_col_width;
  mp_vars_grid->Rows[m_header_row]->Strings[m_value_col] = "Значение";
  mp_vars_grid->ColWidths[m_value_col] = m_value_col_width;
  mp_vars_grid->Rows[m_header_row]->Strings[m_chart_col] = "Граф.";
  mp_vars_grid->ColWidths[m_chart_col] = m_chart_col_width;
  mp_vars_grid->PopupMenu = mp_grid_popup;
  mp_vars_grid->Parent = mp_form;

  m_ini_file.load();
  fill_grid_index_col();

  //m_out << irs::stime << "start\n";
}
__fastcall irs::tstlan4_t::controls_t::~controls_t()
{
  m_csv_file.close();
  //m_ini_file.save();
}
void irs::tstlan4_t::controls_t::connect(mxdata_t *ap_data)
{
  mp_data = ap_data;
  m_refresh_grid = true;
}
void irs::tstlan4_t::controls_t::save_conf()
{
  //m_ini_file.save();
}
void irs::tstlan4_t::controls_t::set_refresh_time_chart
  (const irs_i32 a_refresh_time)
{
  m_builder_chart.set_refresh_time(a_refresh_time);
}
void irs::tstlan4_t::controls_t::update_time(const counter_t a_update_time)
{
  m_read_loop_timer = a_update_time;
}
void irs::tstlan4_t::controls_t::resize_chart(const irs_u32 a_size)
{
  m_builder_chart.resize(a_size);
}
void irs::tstlan4_t::controls_t::tick()
{
  if (mp_data)
  if (mp_data->connected()) {
    if (m_refresh_grid) {
      m_refresh_grid = false;
      int conn_size = m_netconn.connect(mp_data, mp_vars_grid, m_type_col,
        m_index_col);
      int data_size = (int)mp_data->size();
      if (conn_size < data_size) {
        mp_vars_grid->RowCount = mp_vars_grid->RowCount +
          (data_size - conn_size)/sizeof(irs_i32);
        m_netconn.connect(mp_data, mp_vars_grid, m_type_col, m_index_col);
      }
      m_ini_file.save_grid_size(mp_vars_grid);
      //fill_grid_index_col();
    }
    if (m_first_connect) {
      m_first_connect = false;
      m_ini_file.load();
    }
    if (m_read_loop_timer.check() && m_refresh_table) {
      // Обработка столбца "Значение" при чтении
      int row_count = mp_vars_grid->RowCount;
      TStrings* value_list = mp_vars_grid->Cols[m_value_col];
      if (!m_is_lock) {
        for (int row = m_header_size; row < row_count; row++) {
          int var_index = row - m_header_size;
          if (!is_edit_mode || write_var_index != var_index) {
            value_list->Strings[row] = var_to_bstr(var_index);
          }
        }
      }

      // Обработка столбца "Значение" при записи
      if (is_edit_mode) {
        int write_row = write_var_index + m_header_size;
        bool row_changed = (mp_vars_grid->Row != write_row);
        bool col_changed = (mp_vars_grid->Col != m_value_col);
        if (row_changed || col_changed) {
          is_edit_mode = false;
        }
        if (is_write_var) {
          is_write_var = false;
          is_edit_mode = false;
          bstr_to_var(write_var_index, value_list->Strings[write_row]);
        }
      }

      // Обработка столбца "Граф."
      TStrings* name_list = mp_vars_grid->Cols[m_name_col];
      TStrings* chart_list = mp_vars_grid->Cols[m_chart_col];
      if (m_refresh_chart_items) {
        m_refresh_chart_items = false;
        //mp_chart->clear_param();
        for (int row = m_header_size; row < row_count; row++) {
          string_type chart_name = name_list->Strings[row].c_str();
          if (chart_list->Strings[row] == "1") {
            if (!m_chart_names[chart_name]) {
              m_chart_names[chart_name] = true;
              mp_chart->add_param(chart_name);
            }
          } else {
            #ifdef NOP
            if (m_chart_names[chart_name]) {
              m_chart_names[chart_name] = false;
              mp_chart->delete_param(chart_name);
            }
            #endif //NOP
          }
        }
      }
      double chart_time = m_time.get() + m_shift_time;
      for (int row = m_header_size; row < row_count; row++) {
        if (chart_list->Strings[row] == "1") {
          int var_index = row - m_header_size;
          string_type chart_name = name_list->Strings[row].c_str();
          mp_chart->add(chart_name, chart_time, var_to_double(var_index));
          if (m_first) {
            creation_csv();
            m_first = false;
          }
          string_type csv_names = name_list->Strings[row].c_str();
          if (m_csv_names[csv_names] && (m_is_created_csv)) {
            m_csv_file.set_var(irst("Time"),
              irsstr_from_number_russian(char_t(), m_timer.get()));
            double value = var_to_double(var_index);
            string_type value_str = FloatToStr(value).c_str();
            m_csv_file.set_var(csv_names, value_str);
          }
        }
      }
      if(m_is_created_csv) {
        m_csv_file.write_line();
      }
    }
  }

  if (m_is_grid_edit) {
    bool row_changed = (mp_vars_grid->Row != m_grid_edit_row);
    bool col_changed = (mp_vars_grid->Col != m_grid_edit_col);
    if (row_changed || col_changed) {
      m_is_grid_edit = false;
    }
  }
  if (m_saveable_is_edit) {
    bool row_changed = (mp_vars_grid->Row != m_saveable_row);
    bool col_changed = (mp_vars_grid->Col != m_saveable_col);
    if (row_changed || col_changed) {
      m_saveable_is_edit = false;
      save_grid_row(m_saveable_row);
    }
  }
  if (m_is_edit_chart_items) {
    bool row_changed = (mp_vars_grid->Row != m_chart_row);
    bool col_changed = (mp_vars_grid->Col != m_chart_col);
    if (row_changed || col_changed) {
      m_is_edit_chart_items = false;
      m_refresh_chart_items = true;
    }
  }
}
template <class T>
void irs::tstlan4_t::controls_t::out_number(ostream_t& a_stream,
  const T& a_value)
{
  switch (type_to_index<T>())
  {
    case bool_idx:
    case signed_char_idx:
    case unsigned_char_idx: {
      a_stream << static_cast<int>(a_value);
    } break;
    default: {
      a_stream << a_value;
    } break;
  }
}
template <class T>
void irs::tstlan4_t::controls_t::integer_to_string_helper(const T& a_value,
  string_type* ap_string)
{
  stringstream_t strm;
  strm.imbue(locale::classic());
  out_number(strm, a_value);
  strm << internal << setfill(irst('0')) << hex << uppercase;
  const int type_hex_width = 2*sizeof(T);
  strm << irst(" (0x") << setw(type_hex_width);
  out_number(strm, a_value);
  strm << irst(")\0");
  *ap_string = strm.str();
}
void irs::tstlan4_t::controls_t::integer_to_string(bool a_value,
  string_type* ap_string)
{
  integer_to_string_helper(a_value, ap_string);
}
void irs::tstlan4_t::controls_t::integer_to_string(unsigned char a_value,
  string_type* ap_string)
{
  integer_to_string_helper(a_value, ap_string);
}
void irs::tstlan4_t::controls_t::integer_to_string(signed char a_value,
  string_type* ap_string)
{
  integer_to_string_helper(a_value, ap_string);
}
void irs::tstlan4_t::controls_t::integer_to_string(unsigned short a_value,
  string_type* ap_string)
{
  integer_to_string_helper(a_value, ap_string);
}
void irs::tstlan4_t::controls_t::integer_to_string(signed short a_value,
  string_type* ap_string)
{
  integer_to_string_helper(a_value, ap_string);
}
void irs::tstlan4_t::controls_t::integer_to_string(unsigned int a_value,
  string_type* ap_string)
{
  integer_to_string_helper(a_value, ap_string);
}
void irs::tstlan4_t::controls_t::integer_to_string(signed int a_value,
  string_type* ap_string)
{
  integer_to_string_helper(a_value, ap_string);
}
void irs::tstlan4_t::controls_t::integer_to_string(unsigned long a_value,
  string_type* ap_string)
{
  integer_to_string_helper(a_value, ap_string);
}
void irs::tstlan4_t::controls_t::integer_to_string(signed long a_value,
  string_type* ap_string)
{
  integer_to_string_helper(a_value, ap_string);
}
String irs::tstlan4_t::controls_t::var_to_bstr(int a_var_index)
{
  if ((a_var_index == 19) || (a_var_index == 30)) {
    int i = 0;
    i++;
  }

  string_type val = 0;
  val.locale_style(irsstrloc_current);
  netconn_t::item_t item = m_netconn.items[a_var_index];
  switch (item.type) {
    case netconn_t::item_t::type_bit: {
      val = m_netconn.bit_vec[item.index];
    } break;
    case netconn_t::item_t::type_bool: {
      integer_to_string(m_netconn.bool_vec[item.index], &val);
    } break;
    case netconn_t::item_t::type_u8: {
      integer_to_string(m_netconn.u8_vec[item.index], &val);
    } break;
    case netconn_t::item_t::type_i16: {
      integer_to_string(m_netconn.i16_vec[item.index], &val);
    } break;
    case netconn_t::item_t::type_u16: {
      integer_to_string(m_netconn.u16_vec[item.index], &val);
    } break;
    case netconn_t::item_t::type_i32: {
      integer_to_string(m_netconn.i32_vec[item.index], &val);
    } break;
    case netconn_t::item_t::type_float: {
      val = m_netconn.float_vec[item.index];
    } break;
    case netconn_t::item_t::type_double: {
      val = m_netconn.double_vec[item.index];
    } break;
    default: {
      val = irst("bad");
    } break;
  }
  return val.c_str();
}
void irs::tstlan4_t::controls_t::bstr_to_var(int a_var_index,
  const String& a_bstr_val)
{
  string_type val = a_bstr_val.c_str();
  netconn_t::item_t item = m_netconn.items[a_var_index];
  switch (item.type) {
    case netconn_t::item_t::type_bit: {
      if (a_bstr_val == "1") {
        m_netconn.bit_vec[item.index] = 1;
      } else {
        m_netconn.bit_vec[item.index] = 0;
      }
    } break;
    case netconn_t::item_t::type_bool: {
      bool val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.bool_vec[item.index] = val_conn;
      }
    } break;
    case netconn_t::item_t::type_u8: {
      irs_u8 val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.u8_vec[item.index] = val_conn;
      }
    } break;
    case netconn_t::item_t::type_i16: {
      irs_i16 val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.i16_vec[item.index] = val_conn;
      }
    } break;
    case netconn_t::item_t::type_u16: {
      irs_u16 val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.u16_vec[item.index] = val_conn;
      }
    } break;
    case netconn_t::item_t::type_i32: {
      irs_i32 val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.i32_vec[item.index] = val_conn;
      }
    } break;
    case netconn_t::item_t::type_float: {
      float val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.float_vec[item.index] = val_conn;
      }
    } break;
    case netconn_t::item_t::type_double: {
      double val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.double_vec[item.index] = val_conn;
      }
    } break;
  }
}
double irs::tstlan4_t::controls_t::var_to_double(int a_var_index)
{
  double val = 0;
  netconn_t::item_t item = m_netconn.items[a_var_index];
  switch (item.type) {
    case netconn_t::item_t::type_bit: {
      val = m_netconn.bit_vec[item.index];
    } break;
    case netconn_t::item_t::type_bool: {
      val = m_netconn.bool_vec[item.index];
    } break;
    case netconn_t::item_t::type_u8: {
      val = m_netconn.u8_vec[item.index];
    } break;
    case netconn_t::item_t::type_i16: {
      val = m_netconn.i16_vec[item.index];
    } break;
    case netconn_t::item_t::type_u16: {
      val = m_netconn.u16_vec[item.index];
    } break;
    case netconn_t::item_t::type_i32: {
      val = m_netconn.i32_vec[item.index];
    } break;
    case netconn_t::item_t::type_float: {
      val = m_netconn.float_vec[item.index];
    } break;
    case netconn_t::item_t::type_double: {
      val = m_netconn.double_vec[item.index];
    } break;
  }
  return val;
}
void irs::tstlan4_t::controls_t::fill_grid_index_col()
{
  for (int col = m_header_size; col < mp_vars_grid->RowCount; col++) {
    int var_index = col - m_header_size;
    mp_vars_grid->Cols[m_index_col]->Strings[col] = var_index;
  }
}
bool irs::tstlan4_t::controls_t::is_saveable_col(int a_col)
{
  return ((m_name_col == a_col) || (m_type_col == a_col) ||
      (m_chart_col == a_col));
}
void irs::tstlan4_t::controls_t::save_grid_row(int a_row)
{
  m_ini_file.save_grid_row(mp_vars_grid, a_row);
  m_netconn.connect(mp_data, mp_vars_grid, m_type_col, m_index_col);
}
void irs::tstlan4_t::controls_t::creation_csv()
{
  m_timer.start();
  string_type file_name = file_name_time(irst(".csv"));
  string_type ExePath = ExtractFilePath(Application->ExeName).c_str();
  string_type path =  ExePath + irst("Out\\");
  MkDir(path.c_str());
  string_type path_file = cbuilder::file_path(path + file_name, irst(".csv"));
  if (m_csv_file.open(path_file)) {
    m_is_created_csv = true;
    TStrings* name_list = mp_vars_grid->Cols[m_name_col];
    TStrings* chart_list = mp_vars_grid->Cols[m_chart_col];
    int row_count = mp_vars_grid->RowCount;
    m_csv_file.add_col(irst("Time"), ec_str_type);
    for (int row = m_header_size; row < row_count; row++) {
      string_type csv_names = name_list->Strings[row].c_str();
      m_csv_names[csv_names] = false;
      if (chart_list->Strings[row] == irst("1")) {
        if (!m_csv_names[csv_names]) {
          m_csv_names[csv_names] = true;
          m_csv_file.add_col(csv_names, ec_str_type);
        }
      }
    }
  } else {
    MessageBox(NULL, irst("CSV Файл не создан!!"), irst("Error"), MB_OK);
  }
}
void __fastcall irs::tstlan4_t::controls_t::VarsGridGetEditText(
  TObject *Sender, int ACol, int ARow, String &Value)
{
  m_is_grid_edit = true;
  m_grid_edit_col = ACol;
  m_grid_edit_row = ARow;

  if (ACol == m_value_col) {
    is_edit_mode = true;
    write_var_index = ARow - m_header_size;
  }
  if (ACol == m_chart_col) {
    m_is_edit_chart_items = true;
    m_chart_row = ARow;
  }
  if (is_saveable_col(ACol)) {
    m_saveable_is_edit = true;
    m_saveable_col = ACol;
    m_saveable_row = ARow;
  }
}
void __fastcall irs::tstlan4_t::controls_t::VarsGridKeyDown(
  TObject *Sender, WORD &Key, TShiftState Shift)
{
  if (m_is_grid_edit) {
    if (Key == VK_RETURN) {
      m_is_grid_edit = false;
    }
  }

  if (is_edit_mode) {
    if (Key == VK_RETURN) {
      is_write_var = true;
    }
  }
  if (m_is_edit_chart_items) {
    if (Key == VK_RETURN) {
      m_is_edit_chart_items = false;
      m_refresh_chart_items = true;
    }
  }
  if (m_saveable_is_edit) {
    if (Key == VK_RETURN) {
      m_saveable_is_edit = false;
      save_grid_row(m_saveable_row);
    }
  }

  // Вставака/удаление строк
  if (!m_is_grid_edit) {
    switch (Key) {
      case VK_INSERT: {
        GridInsertClick(this);
      } break;
      case VK_DELETE: {
        GridDeleteClick(this);
      } break;
    }
  }
}
void __fastcall irs::tstlan4_t::controls_t::ChartBtnClick(TObject *Sender)
{
  mp_chart->show();
}
void __fastcall irs::tstlan4_t::controls_t::CsvSaveBtnClick(TObject *Sender)
{
  if (!m_start) {
    m_start = true;
    m_refresh_table = false;
    if (m_csv_file.close()) {
      m_is_created_csv = false;
    }
    mp_start_btn->Caption = mp_read_on_text;
  } else {
    m_start = false;
    m_refresh_table = true;
    creation_csv();
    mp_start_btn->Caption = mp_read_off_text;
  }
}
void __fastcall irs::tstlan4_t::controls_t::CsvLoadBtnClick(TObject *Sender)
{
  if (mp_open_dialog->Execute()) {
    string_type csv_file_name = mp_open_dialog->FileName.c_str();
    m_csv_open_file.open(csv_file_name);
    m_csv_open_file.load(m_table);

    size_t col_size = m_table.get_col_count();
    size_t row_size = m_table.get_row_count();
    if (col_size > 0) {
      col_size = col_size-1;
    }
    bool is_null = false;
    double time_chart = m_time.get() + m_shift_time;
    string_type time_zero_str =  m_table.read_cell(0, 1);
    double time_zero = static_cast<double>(StrToFloat(time_zero_str.c_str()));
    for (size_t col_index = 1; col_index < col_size; col_index++) {
      string_type chart_name = m_table.read_cell(col_index, 0);
      if (!m_chart_names[chart_name]) {
        mp_chart->add_param(chart_name);
      }
      for (size_t row_index = 1; row_index < row_size; row_index++) {
        string_type value = m_table.read_cell(col_index, row_index);
        if (value.empty()) {
          is_null = true;
        }
        if (!is_null) {
          string_type time =  m_table.read_cell(0, row_index);
          double delta_time =  time_chart +
            static_cast<double>(StrToFloat(time.c_str())) - time_zero;
          mp_chart->add(chart_name, delta_time,
            static_cast<double>(StrToFloat(value.c_str())));
        }
        is_null = false;
      }
    }
    string_type time_end =  m_table.read_cell(0, row_size - 1);
    m_shift_time = m_shift_time +
      static_cast<double>(StrToFloat(time_end.c_str())) - time_zero;
    m_csv_open_file.close();
  }
}
void __fastcall irs::tstlan4_t::controls_t::GridInsertClick(TObject *Sender)
{
  m_refresh_grid = true;
  const int added_count = 1;
  mp_vars_grid->RowCount = mp_vars_grid->RowCount + added_count;
  int row_count = mp_vars_grid->RowCount;
  int col_count = mp_vars_grid->ColCount;
  int row_cur = mp_vars_grid->Row;
  for (int row = row_count - added_count; row > row_cur; row--) {
    for (int col = 0; col < col_count ; col++) {
      mp_vars_grid->Rows[row]->Strings[col] =
        mp_vars_grid->Rows[row - added_count]->Strings[col];
    }
  }
  for (int col = 0; col < col_count ; col++) {
    mp_vars_grid->Rows[row_cur]->Strings[col] = "";
  }
  m_ini_file.save();
}
void __fastcall irs::tstlan4_t::controls_t::GridDeleteClick(TObject *Sender)
{
  m_refresh_grid = true;
  const int deleted_count = 1;
  int row_count = mp_vars_grid->RowCount;
  int col_count = mp_vars_grid->ColCount;
  int row_cur = mp_vars_grid->Row;
  for (int row = row_cur; row < row_count - deleted_count; row++) {
    for (int col = 0; col < col_count ; col++) {
      mp_vars_grid->Rows[row]->Strings[col] =
        mp_vars_grid->Rows[row + deleted_count]->Strings[col];
    }
  }
  mp_vars_grid->RowCount = mp_vars_grid->RowCount - deleted_count;
  m_ini_file.save();
}
void __fastcall irs::tstlan4_t::controls_t::FormHide(TObject *Sender)
{
  m_is_lock = true;
  mp_chart->hide();
}
void __fastcall irs::tstlan4_t::controls_t::FormShow(TObject *Sender)
{
  m_is_lock = false;
  m_refresh_grid = true;
  m_ini_file.load();
}
// Консоль внутри tstlan4
TMemo* irs::tstlan4_t::controls_t::log()
{
  return mp_log_memo;
}

