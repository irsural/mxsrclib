//! \file
//! \ingroup user_interface_group
//! \brief Тест сети 4 - библиотека
//!
//! Дата: 22.05.2011\n
//! Дата создания: 17.09.2009

#if IRS_USE_DEV_EXPRESS

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <tstlan5lib.h>
#include <irstime.h>
#include <MxBase.h>
#include <irserror.h>
#include <irslimits.h>
#include <irsparam.h>

#include <irsfinal.h>

vector<irs::string_t> make_type_str_map()
{
  vector<irs::string_t> type_str_map(irs::tstlan::type_count);
  type_str_map[irs::tstlan::type_bit] = irst("bit");
  type_str_map[irs::tstlan::type_bool] = irst("bool");
  type_str_map[irs::tstlan::type_i8] = irst("i8");
  type_str_map[irs::tstlan::type_u8] = irst("u8");
  type_str_map[irs::tstlan::type_i16] = irst("i16");
  type_str_map[irs::tstlan::type_u16] = irst("u16");
  type_str_map[irs::tstlan::type_i32] = irst("i32");
  type_str_map[irs::tstlan::type_u32] = irst("u32");
  type_str_map[irs::tstlan::type_i64] = irst("i64");
  type_str_map[irs::tstlan::type_u64] = irst("u64");
  type_str_map[irs::tstlan::type_float] = irst("float");
  type_str_map[irs::tstlan::type_double] = irst("double");
  type_str_map[irs::tstlan::type_long_double] = irst("long double");
  return type_str_map;
}

const vector<irs::string_t>& get_type_str_map()
{
  static const vector<irs::string_t> type_str_map = make_type_str_map();
  return type_str_map;
}

irs::string_t irs::tstlan::type_to_str(irs::tstlan::type_t a_type)
{
  static const vector<irs::string_t>& type_str_map = get_type_str_map();
  return type_str_map[a_type];
}

bool irs::tstlan::str_to_type(const irs::string_t& a_str,
  irs::tstlan::type_t* ap_type)
{
  static const vector<irs::string_t>& type_str_map = get_type_str_map();
  irs::string_t type_str = a_str;
  if (type_str == irst("long")) {
    *ap_type = type_i32;
    return true;
  }
  for (std::size_t i = 0; i < type_count; i++) {
    if (type_str_map[i] == type_str) {
      *ap_type = i;
      return true;
    }
  }
  return false;
}

irs::tstlan::vars_ini_file_t::vars_ini_file_t(
  TcxGridTableView* ap_cx_grid_table_view,
  TcxGridColumn* ap_name_column,
  TcxGridColumn* ap_type_column,
  TcxGridColumn* ap_chart_column,
  const string_type& a_section_prefix
):
  m_ini_name(),
  mp_cx_grid_table_view(ap_cx_grid_table_view),
  mp_name_column(ap_name_column),
  mp_type_column(ap_type_column),
  mp_chart_column(ap_chart_column),
  m_section_prefix(a_section_prefix),
  m_section_name(irst("vars")),
  m_section_full_name(m_section_prefix + m_section_name),
  m_name_row_count(irst("_row_count")),
  m_name_column_prefix(irst("Name_")),
  m_type_column_prefix(irst("Type_")),
  m_chart_column_prefix(irst("Graph_"))
{
}

void irs::tstlan::vars_ini_file_t::set_ini_name(const String& a_ini_name)
{
  const char_t *str_begin = a_ini_name.c_str();
  const char_t *str_end = str_begin + a_ini_name.Length();
  if (find(str_begin, str_end, irst('\\')) != str_end) {
    m_ini_name = a_ini_name.c_str();
  } else {
    String ExePath = ExtractFilePath(Application->ExeName);
    string_t exe_path = ExePath.c_str();
    m_ini_name = exe_path + string_t(a_ini_name.c_str());
  }
}

void irs::tstlan::vars_ini_file_t::set_section(const string_type& a_section)
{
  m_section_prefix = a_section;
  m_section_full_name = m_section_prefix + m_section_name;
}

void irs::tstlan::vars_ini_file_t::load()
{
  auto_ptr<TIniFile> IniFile(new TIniFile(m_ini_name.c_str()));
  mp_cx_grid_table_view->BeginUpdate();
  load_cx_grid_table_view_row_count(IniFile.get());
  const int row_count = mp_cx_grid_table_view->DataController->RecordCount;
  for (int row = 0; row < row_count; row++) {
    load_cx_grid_table_view_row(IniFile.get(), row);
  }
  mp_cx_grid_table_view->EndUpdate();
}

void irs::tstlan::vars_ini_file_t::save() const
{
  auto_ptr<TIniFile> IniFile(new TIniFile(m_ini_name.c_str()));
  const int row_count =
    mp_cx_grid_table_view->DataController->RecordCount;
  for (int row = 0; row < row_count; row++) {
    save_cx_grid_table_view_row(IniFile.get(), row);
  }
  save_cx_grid_table_view_row_count(IniFile.get());
}

/*void irs::vars_ini_file_t::load_cx_grid_table_view_row(int a_row_index)
{
  auto_ptr<TIniFile> IniFile(new TIniFile(m_ini_name.c_str()));
  load_cx_grid_table_view_row(IniFile.get(), a_row_index);
}*/

void irs::tstlan::vars_ini_file_t::save_cx_grid_table_view_row(
  int a_row_index) const
{
  auto_ptr<TIniFile> IniFile(new TIniFile(m_ini_name.c_str()));
  save_cx_grid_table_view_row(IniFile.get(), a_row_index);
}

void irs::tstlan::vars_ini_file_t::save_cx_grid_table_view_row_count() const
{
  auto_ptr<TIniFile> IniFile(new TIniFile(m_ini_name.c_str()));
  save_cx_grid_table_view_row_count(IniFile.get());
}

void irs::tstlan::vars_ini_file_t::load_cx_grid_table_view_row_count(
  TIniFile *ap_ini_file)
{
  //mp_cx_grid_table_view->DataController->RecordCount = 0;
  mp_cx_grid_table_view->DataController->RecordCount =
    ap_ini_file->ReadInteger(m_section_full_name.c_str(),
    m_name_row_count.c_str(),
    mp_cx_grid_table_view->DataController->RecordCount);
}

void irs::tstlan::vars_ini_file_t::save_cx_grid_table_view_row_count(
  TIniFile *ap_ini_file) const
{
  ap_ini_file->WriteInteger(m_section_full_name.c_str(),
    m_name_row_count.c_str(),
    mp_cx_grid_table_view->DataController->RecordCount);
}

void irs::tstlan::vars_ini_file_t::load_cx_grid_table_view_row(
  TIniFile *ap_ini_file, int a_row_index)
{
  load_name(ap_ini_file, a_row_index);
  load_type(ap_ini_file, a_row_index);
  load_chart(ap_ini_file, a_row_index);
}

void irs::tstlan::vars_ini_file_t::save_cx_grid_table_view_row(
  TIniFile *ap_ini_file, int a_row_index) const
{
  save_name(ap_ini_file, a_row_index);
  save_type(ap_ini_file, a_row_index);
  save_chart(ap_ini_file, a_row_index);
}

void irs::tstlan::vars_ini_file_t::load_name(TIniFile *ap_ini_file,
  int a_row_index)
{
  string_t name = m_name_column_prefix + string_t(a_row_index);
  mp_cx_grid_table_view->DataController->
    Values[a_row_index][mp_name_column->Index] =
    ap_ini_file->ReadString(m_section_full_name.c_str(),
    name.c_str(), irst(""));
}

void irs::tstlan::vars_ini_file_t::save_name(
  TIniFile *ap_ini_file, int a_row_index) const
{
  Variant value_variant = mp_cx_grid_table_view->DataController->Values
    [a_row_index][mp_name_column->Index];
  String value_bstr;
  if (!value_variant.IsNull()) {
    value_bstr = value_variant;
  }
  string_t name = m_name_column_prefix + string_t(a_row_index);
  ap_ini_file->WriteString(
    m_section_full_name.c_str(), name.c_str(), value_bstr);
}

namespace {

bool is_type_valid(const irs::string_t& a_type_str)
{
  irs::tstlan::type_t type;
  return str_to_type(a_type_str, &type);
}

} // unnamed namespace

void irs::tstlan::vars_ini_file_t::load_type(TIniFile *ap_ini_file,
  int a_row_index)
{
  string_t name = m_type_column_prefix + string_t(a_row_index);
  String type_bstr =
    ap_ini_file->ReadString(
    m_section_full_name.c_str(), name.c_str(), irst(""));
  irs::tstlan::type_t type;
  if (!str_to_type(irs::str_conv<string_type>(type_bstr), &type)) {
    type = type_i32;
  }
  mp_cx_grid_table_view->DataController->
    Values[a_row_index][mp_type_column->Index] =
    irs::str_conv<String>(type_to_str(type));
}

void irs::tstlan::vars_ini_file_t::save_type(
  TIniFile *ap_ini_file, int a_row_index) const
{
  Variant value_variant = mp_cx_grid_table_view->DataController->Values
    [a_row_index][mp_type_column->Index];
  String value_bstr;
  if (!value_variant.IsNull()) {
    value_bstr = value_variant;
  }
  string_t name = m_type_column_prefix + string_t(a_row_index);
  ap_ini_file->WriteString(
    m_section_full_name.c_str(), name.c_str(), value_bstr);
}

void irs::tstlan::vars_ini_file_t::load_chart(
  TIniFile *ap_ini_file, int a_row_index)
{
  string_t name = m_chart_column_prefix + string_t(a_row_index);
  String graph_enabled_bstr =
    ap_ini_file->ReadString(m_section_full_name.c_str(),
    name.c_str(), irst(""));
  mp_cx_grid_table_view->DataController->
    Values[a_row_index][mp_chart_column->Index] =
    graph_enabled_bstr == irst("1") ? true : false;
}

void irs::tstlan::vars_ini_file_t::save_chart(
  TIniFile *ap_ini_file, int a_row_index) const
{
  Variant value_variant = mp_cx_grid_table_view->DataController->Values
    [a_row_index][mp_chart_column->Index];
  bool chart_enabled = (value_variant.Type() == varBoolean) ?
    value_variant : false;
  String value_bstr;
  if (chart_enabled) {
    value_bstr = irst("1");
  } else {
    value_bstr = irst("0");
  }
  string_t name = m_chart_column_prefix + string_t(a_row_index);
  ap_ini_file->WriteString(m_section_full_name.c_str(),
    name.c_str(), value_bstr);
}

irs::tstlan::view_t::view_t(const view_t& a_view):
  m_form_type(a_view.m_form_type),
  mp_extern_chart(a_view.mp_extern_chart),
  mp_form_auto(IRS_NULL),
  mp_form(IRS_NULL),
  mp_controls(IRS_NULL),
  m_ini_name(a_view.m_ini_name),
  m_ini_section_prefix(a_view.m_ini_section_prefix),
  m_ini_options_section(a_view.m_ini_options_section),
  //m_ini_section(a_view.m_ini_section),
  m_log_buf(a_view.m_log_buf),
  m_update_time_cnt(a_view.m_update_time_cnt),
  m_global_log_connect(a_view.m_global_log_connect)
{
  if (m_form_type == ft_internal) {
    mp_form_auto.reset(new TForm(IRS_NULL, 1));
    mp_form = mp_form_auto.get();
    TForm *src_form = a_view.mp_form_auto.get();
    irs::mxcopy(mp_form, src_form);
  }
  init(mp_form_auto.get());
}
irs::tstlan::view_t::view_t(
  form_type_t a_form_type,
  chart_window_t* ap_extern_chart,
  const string_type& a_ini_name,
  const string_type& a_ini_section_prefix,
  counter_t a_update_time_cnt,
  global_log_connect_t a_global_log_connect
):
  m_form_type(a_form_type),
  mp_extern_chart(ap_extern_chart),
  mp_form_auto(IRS_NULL),
  mp_form(IRS_NULL),
  mp_controls(IRS_NULL),
  m_ini_name(a_ini_name),
  m_ini_section_prefix(a_ini_section_prefix),
  m_ini_options_section(irst("options")),
  //m_ini_section(irst("vars")),
  m_log_buf(IRS_NULL, 100),
  m_update_time_cnt(a_update_time_cnt),
  m_global_log_connect(a_global_log_connect)
{
  if (m_form_type == ft_internal) {
    mp_form_auto.reset(new TForm(IRS_NULL, 1));
    mp_form = mp_form_auto.get();
    mp_form->Left = 10;
    mp_form->Top = 50;
    mp_form->Width = 500;
    mp_form->Height = 800;
    mp_form->Position = poDesigned;
    mp_form->Caption = irst("Тест сети 4");
    init(mp_form_auto.get());
  }
}
// Подключение к форме и создание элементов управления
// Можно переподключать к другой форме без вызова deinit
// На предыдущей форме все компоненты уничтожаются
void irs::tstlan::view_t::init(TForm *ap_form)
{
  irs::chart::builder_chart_window_t::stay_on_top_t stay_on_top =
    irs::chart::builder_chart_window_t::stay_on_top_on;
  if (m_form_type == ft_internal) {
    stay_on_top = irs::chart::builder_chart_window_t::stay_on_top_off;
  }
  mp_form = ap_form;
  mp_controls.reset(new controls_t(m_form_type, mp_form, mp_extern_chart,
    m_ini_name,
    m_ini_section_prefix, stay_on_top, m_update_time_cnt));
  if (m_global_log_connect == global_log_connect) {
    m_log_buf.connect(mp_controls->log());
    irs::mlog().rdbuf(&m_log_buf);
  }
}
// Уничтожение всех компонентов формы
void irs::tstlan::view_t::deinit()
{
  mp_controls.reset();
}
void irs::tstlan::view_t::tick()
{
  mp_controls->tick();
}
const irs::tstlan::view_t::char_type* irs::tstlan::view_t::def_ini_name()
{
  return irst("tstlan3.ini");
}
const irs::tstlan::view_t::char_type* irs::tstlan::view_t::def_ini_section()
{
  return irst("");
}

void irs::tstlan::view_t::show()
{
  mp_form->Show();
}
void irs::tstlan::view_t::hide()
{
  mp_form->Hide();
}
irs::rect_t irs::tstlan::view_t::position() const
{
  return rect_t();
}
void irs::tstlan::view_t::set_position(const rect_t &a_position)
{
}
void irs::tstlan::view_t::connect(mxdata_t *ap_data)
{
  mp_controls->connect(ap_data);
}
void irs::tstlan::view_t::update_time(const irs_i32 a_update_time)
{
  mp_controls->update_time(a_update_time);

}
void irs::tstlan::view_t::resize_chart(const irs_u32 a_size)
{
  mp_controls->resize_chart(a_size);
}
void irs::tstlan::view_t::reset_chart()
{
  mp_controls->reset_chart();
}
void irs::tstlan::view_t::options_event_connect(event_t* ap_event)
{
  mp_controls->options_event_connect(ap_event);
}
void irs::tstlan::view_t::options_event_clear()
{
  mp_controls->options_event_clear();
}
irs::event_t* irs::tstlan::view_t::inner_options_event()
{
  return mp_controls->inner_options_event();
}
void irs::tstlan::view_t::save_conf()
{
  mp_controls->save_conf();
}
void irs::tstlan::view_t::load_conf()
{
  mp_controls->load_conf();
}
void irs::tstlan::view_t::clear_conf()
{
  mp_controls->clear_conf();
}
irs::tstlan::view_t::string_type irs::tstlan::view_t::conf_section()
{
  return mp_controls->conf_section();
}
void irs::tstlan::view_t::conf_section(const string_type& a_name)
{
  mp_controls->conf_section(a_name);
}
irs::tstlan::view_t::string_type irs::tstlan::view_t::ini_name()
{
  return mp_controls->ini_name();
}
void irs::tstlan::view_t::ini_name(const string_type& a_ini_name)
{
  mp_controls->ini_name(a_ini_name);
}

//TComponent* const zero_comp = IRS_NULL;
// Компонентов формы
irs::tstlan::view_t::controls_t::param_box_tune_t::param_box_tune_t(
  param_box_base_t* ap_param_box
):
  mp_param_box(ap_param_box)
{
  mp_param_box->add_bool(irst("Отображать лог"), false);
  mp_param_box->add_edit(irst("Время обновления, мс"), irst("200"));
  mp_param_box->add_edit(irst("Количество точек в графике"), irst("1000"));
  mp_param_box->add_bool(irst("Сбросить время"), false);
  mp_param_box->add_bool(irst("Запись CSV включена"), false);
}
void irs::tstlan::view_t::controls_t::inner_options_apply()
{
  if (mp_param_box->get_param(irst("Отображать лог")) == irst("true")) {
    // Отобразить лог
    mp_splitter->Show();
    mp_log_memo->Show();
    mp_splitter->Top = mp_log_memo->Top + mp_log_memo->Height;
  } else {
    // Скрыть лог
    mp_log_memo->Hide();
    mp_splitter->Hide();
  }
  update_time(param_box_read_number<irs_i32>(*mp_param_box,
    irst("Время обновления, мс")));
  resize_chart(param_box_read_number<irs_u32>(*mp_param_box,
    irst("Количество точек в графике")));
  m_refresh_csv_state_event.exec();
  if (mp_param_box->get_param(irst("Запись CSV включена")) == irst("true")) {
    m_is_csv_on = true;
  } else {
    m_is_csv_on = false;
  }
}
irs::tstlan::view_t::controls_t::controls_t(
  form_type_t a_form_type,
  TForm *ap_form,
  chart_window_t* ap_extern_chart,
  const string_type& a_ini_name,
  const string_type& a_ini_section,
  irs::chart::builder_chart_window_t::stay_on_top_t a_stay_on_top,
  counter_t a_update_time_cnt
):
  m_form_type(a_form_type),
  mp_form(ap_form),
  mp_top_panel(new TPanel(mp_form)),
  mp_open_dialog(new TOpenDialog(mp_form)),
  mp_load_btn(new TButton(mp_form)),
  mp_start_btn(new TButton(mp_form)),
  mp_chart_btn(new TButton(mp_form)),
  mp_options_btn(new TButton(mp_form)),
  mp_load_table_values_btn(new TButton(mp_form)),
  mp_save_table_values_btn(new TButton(mp_form)),
  mp_log_memo(new TMemo(mp_form)),
  mp_splitter(new TSplitter(mp_form)),
  mp_grid_popup_insert_item(new TMenuItem(mp_form)),
  mp_grid_popup_delete_item(new TMenuItem(mp_form)),
  mp_grid_popup(new TPopupMenu(mp_form)),
  mp_vars_grid(new TcxGrid(mp_form)),
  mp_view(NULL),
  mp_controller(NULL),
  mp_table_controller(NULL),
  mp_cx_combo_box_properties(new TcxComboBoxProperties(mp_vars_grid)),
  mp_index_column(NULL),
  mp_name_column(NULL),
  mp_type_column(NULL),
  mp_value_column(NULL),
  mp_chart_column(NULL),
  mp_read_on_text(irst("Пуск")),
  mp_read_off_text(irst("Стоп")),
  m_read_loop_timer(a_update_time_cnt),
  m_buf(),
  m_out(&m_buf),
  m_ini_file(),
  mp_vars_ini_file(NULL),
  m_device_name(),
  m_first_connect(true),
  m_refresh_grid(true),
  m_netconn(),
  is_edit_mode(false),
  is_write_var(false),
  mp_local_chart(NULL),
  mp_chart(ap_extern_chart),
  m_csv_file(),
  m_csv_open_file(),
  m_table(),
  m_is_edit_chart_items(true),
  m_refresh_chart_items(true),
  m_time(),
  m_shift_time(0),
  m_chart_time(0),
  m_minus_shift_time(0),
  m_refresh_table(true),
  m_is_lock(true),
  m_chart_row(0),
  m_chart_names(),
  m_csv_names(),
  m_timer(),
  m_ini_section_prefix(a_ini_section),
  m_ini_options_section(irst("options")),
  //m_ini_vars_section(irst("vars")),
  m_start(false),
  //m_first(true),
  m_refresh_csv_state_event(),
  m_is_csv_opened(false),
  mp_event(IRS_NULL),
  m_inner_options_event(),
  mp_param_box(new param_box_t(irst("Внутренние настройки tstlan5"),
    irst("inner_options"))),
  m_param_box_tune(mp_param_box.get()),
  m_is_csv_on(false)
{
  m_buf.connect(mp_log_memo);

  create_grid();

  if (!mp_chart) {
    mp_local_chart.reset(
      new chart::builder_chart_window_t(10000, 1000, a_stay_on_top));
    mp_chart = mp_local_chart.get();
  }

  mp_vars_ini_file.reset(new vars_ini_file_t(mp_view, mp_name_column,
    mp_type_column, mp_chart_column, m_ini_section_prefix));

  if (m_form_type == ft_internal) {
    ini_name(a_ini_name);
    conf_section(m_ini_section_prefix);
  }

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
  mp_chart_btn->Caption = irst("График");
  mp_chart_btn->Parent = mp_top_panel;
  mp_chart_btn->OnClick = ChartBtnClick;

  mp_load_btn->Left = mp_chart_btn->Left + mp_chart_btn->Width + btn_gap;
  mp_load_btn->Top = mp_top_panel->Height/2 - mp_load_btn->Height/2;
  mp_load_btn->Caption = irst("Загрузить");
  mp_load_btn->Parent = mp_top_panel;
  mp_load_btn->OnClick = CsvLoadBtnClick;

  mp_options_btn->Left = mp_load_btn->Left + mp_load_btn->Width + btn_gap;
  mp_options_btn->Top = mp_top_panel->Height/2 - mp_options_btn->Height/2;
  mp_options_btn->Caption = irst("Настройки");
  mp_options_btn->Parent = mp_top_panel;
  mp_options_btn->OnClick = OptionsBtnClick;

  mp_load_table_values_btn->Left = mp_options_btn->Left +
    mp_options_btn->Width + btn_gap;
  mp_load_table_values_btn->Top = mp_top_panel->Height/2 -
    mp_options_btn->Height/2;
  mp_load_table_values_btn->Caption = irst("Загр. знач.");
  mp_load_table_values_btn->Parent = mp_top_panel;
  mp_load_table_values_btn->OnClick = LoadTableValuesBtnClick;

  mp_save_table_values_btn->Left = mp_load_table_values_btn->Left +
    mp_load_table_values_btn->Width + btn_gap;
  mp_save_table_values_btn->Top = mp_top_panel->Height/2 -
    mp_load_table_values_btn->Height/2;
  mp_save_table_values_btn->Caption = irst("Сохр. знач.");
  mp_save_table_values_btn->Parent = mp_top_panel;
  mp_save_table_values_btn->OnClick = SaveTableValuesBtnClick;

  mp_log_memo->Align = alTop;
  mp_log_memo->Height = 100;
  mp_log_memo->Parent = mp_form;
  mp_log_memo->Top = mp_top_panel->Top + mp_log_memo->Height;
  mp_log_memo->ScrollBars = ssVertical;

  mp_splitter->Align = alTop;
  mp_splitter->Top = mp_log_memo->Top + mp_log_memo->Height;
  //mp_splitter->Height = 10;
  mp_splitter->Parent = mp_form;

  mp_grid_popup_insert_item->Caption = irst("Добавить строку");
  mp_grid_popup_insert_item->OnClick = GridInsertClick;
  //mp_grid_popup_insert_item->ShortCut =
    //ShortCut(VK_INSERT, TShiftState() << ssCtrl);
  mp_grid_popup_delete_item->Caption = irst("Удалить строку");
  mp_grid_popup_delete_item->OnClick = GridDeleteClick;
  mp_grid_popup->Items->Add(mp_grid_popup_insert_item);
  mp_grid_popup->Items->Add(mp_grid_popup_delete_item);


  mp_vars_grid->PopupMenu = mp_grid_popup;
  mp_vars_grid->Parent = mp_form;

  if (m_form_type == ft_internal) {
    load_conf();
  }

  //m_out << irs::stime << "start\n";
  string_type ExePath = ExtractFilePath(Application->ExeName).c_str();
  string_type path =  ExePath + irst("Out\\");
  MkDir(path.c_str());

}

void irs::tstlan::view_t::controls_t::create_grid()
{
  TcxGridLevel* Level = mp_vars_grid->Levels->Add();
  Level->Name = irst("SomeLevelName");
  mp_view = (dynamic_cast<TcxGridTableView*>(
    mp_vars_grid->CreateView(__classid(TcxGridTableView))));
  mp_view->Name = irst("SomeViewName");
  Level->GridView = mp_view;
  mp_controller = mp_vars_grid->ActiveView->DataController;
  mp_table_controller = dynamic_cast<TcxCustomGridTableView*>(
    mp_vars_grid->FocusedView)->Controller;

  mp_view->OptionsData->Appending = false;
  mp_view->OptionsData->Inserting = false;
  mp_view->OptionsData->Deleting = false;
  mp_view->OptionsData->DeletingConfirmation = false;
  mp_view->OptionsSelection->MultiSelect = true;
  //mp_view->OptionsView->ShowEditButtons = gsebAlways;
  mp_view->OnKeyDown = VarsGridKeyDown;

  mp_vars_grid->Align = alClient;


  //mp_cx_combo_box_properties->Items->Add(
    //irs::str_conv<String>(m_netconn.type_to_str(netconn_t::item_t::type_bit)));



  //mp_vars_grid->DefaultRowHeight = 17;
  mp_index_column = mp_view->CreateColumn();
  mp_index_column->Caption = irst("№");
  mp_index_column->DataBinding->ValueTypeClass = __classid(TcxStringValueType);
  mp_index_column->Width = m_index_col_width;
  mp_index_column->Options->Editing = false;

  mp_name_column = mp_view->CreateColumn();
  mp_name_column->Caption = irst("Имя");
  mp_name_column->DataBinding->ValueTypeClass = __classid(TcxStringValueType);
  mp_name_column->Width = m_name_col_width;
  mp_name_column->PropertiesClass = __classid(TcxTextEditProperties);
  dynamic_cast<TcxTextEditProperties*>(mp_name_column->Properties)->
    OnValidate = cxGridTableViewColumnNamePropertiesValidate;

  mp_type_column = mp_view->CreateColumn();
  mp_type_column->Caption = irst("Тип");
  mp_type_column->DataBinding->ValueTypeClass = __classid(TcxStringValueType);
  mp_type_column->Width = m_type_col_width;
  mp_type_column->PropertiesClass = __classid(TcxComboBoxProperties); // = mp_cx_combo_box_properties;
  dynamic_cast<TcxComboBoxProperties*>(mp_type_column->Properties)->
    OnChange = cxGridTableView1ColumnTypePropertiesChange;


  for (int i = type_bit; i < type_count; i++) {
    type_t type = static_cast<type_t>(i);
    static_cast<TcxComboBoxProperties*>(mp_type_column->Properties)->
      Items->Add(irs::str_conv<String>(type_to_str(type)));
  }
  /*static_cast<TcxComboBoxProperties*>(mp_type_column->Properties)->
    Items->Add(irst("long"));*/
  dynamic_cast<TcxComboBoxProperties*>(mp_type_column->Properties)->
    DropDownRows = type_count - type_bit;
  dynamic_cast<TcxComboBoxProperties*>(mp_type_column->Properties)->
    DropDownListStyle = lsFixedList;


  mp_value_column = mp_view->CreateColumn();
  mp_value_column->Caption = irst("Значение");
  mp_value_column->DataBinding->ValueTypeClass = __classid(TcxStringValueType);
  mp_value_column->Width = m_value_col_width;
  mp_value_column->PropertiesClass = __classid(TcxTextEditProperties);
  //dynamic_cast<TcxTextEditProperties*>(mp_value_column->Properties)->
    //OnChange = cxGridTableViewColumnValuePropertiesChange;
  dynamic_cast<TcxTextEditProperties*>(mp_value_column->Properties)->
    OnValidate = cxGridTableViewColumnValuePropertiesValidate;

  mp_chart_column = mp_view->CreateColumn();
  mp_chart_column->Caption = irst("Граф.");
  mp_chart_column->DataBinding->ValueTypeClass = __classid(TcxBooleanValueType);
  mp_chart_column->Width = m_chart_col_width;
  mp_chart_column->PropertiesClass = __classid(TcxCheckBoxProperties);
  dynamic_cast<TcxCheckBoxProperties*>(mp_chart_column->Properties)->
    OnChange = cxGridTableViewColumnChartPropertiesChange;

  mp_vars_grid->ActiveView->DataController->RecordCount = m_grid_size;

  //refresh_chart_items();
}

__fastcall irs::tstlan::view_t::controls_t::~controls_t()
{
  if (m_form_type == ft_internal) {
    save_conf();
  }

  mp_form->OnShow = IRS_NULL;
  mp_form->OnHide = IRS_NULL;
}
void irs::tstlan::view_t::controls_t::connect(mxdata_t *ap_data)
{
  mp_data = ap_data;
  m_refresh_grid = true;
}
void irs::tstlan::view_t::controls_t::update_time(const irs_i32 a_update_time)
{
  m_read_loop_timer.set(irs::make_cnt_ms(a_update_time));
  m_read_loop_timer.start();
  if (!mp_local_chart.is_empty()) {
    int def_time = 1000;
    if (a_update_time > def_time) {
      mp_chart->set_refresh_time(a_update_time);
    } else {
      mp_chart->set_refresh_time(def_time);
    }
  }
}
void irs::tstlan::view_t::controls_t::resize_chart(const irs_u32 a_size)
{
  if (!mp_local_chart.is_empty()) {
    mp_chart->resize(a_size);
  }
}

void irs::tstlan::view_t::controls_t::reset_chart()
{
  m_time.start();
  m_shift_time = 0;
  m_minus_shift_time = 0;
  if (!mp_local_chart.is_empty()) {
    mp_chart->clear();
  }
}

void irs::tstlan::view_t::controls_t::refresh_chart_items()
{

  const int row_count = mp_controller->RecordCount;
  for (int row = 0; row < row_count; row++) {
    const string_type chart_name = get_chart_name(row);
    bool chart_enabled = get_chart_enabled_status(row);
    if (chart_enabled && !chart_name.empty()) {
      if (!m_chart_names[chart_name]) {
        m_chart_names[chart_name] = true;
        mp_chart->add_param(chart_name);
      }
    } else {
      if (m_chart_names[chart_name]) {
        m_chart_names[chart_name] = false;
        mp_chart->delete_param(chart_name);
      }
    }
  }
}

irs::tstlan::view_t::controls_t::string_type
irs::tstlan::view_t::controls_t::get_variable_name(size_type a_row)
{
  Variant name_variant =
    mp_controller->Values[a_row][mp_name_column->Index];
  String chart_name_bstr;
  if (!name_variant.IsNull()) {
    chart_name_bstr = name_variant;
  }
  return irs::str_conv<string_type>(chart_name_bstr);
}

irs::tstlan::view_t::controls_t::string_type
irs::tstlan::view_t::controls_t::get_chart_name(size_type a_row)
{
  return make_chart_name(get_variable_name(a_row));
}

irs::tstlan::view_t::controls_t::string_type
irs::tstlan::view_t::controls_t::make_chart_name(
  const string_type& a_variable_name)
{
  string_type chart_name = a_variable_name;
  if (!chart_name.empty() && mp_local_chart.is_empty()) {
    chart_name = m_device_name + irst(":") + chart_name;
  }
  return chart_name;
}

bool irs::tstlan::view_t::controls_t::get_chart_enabled_status(
  size_type a_row) const
{
  Variant enabled_variant =
     mp_controller->Values[a_row][mp_chart_column->Index];
  bool enabled = false;
  if (!enabled_variant.IsNull()) {
    enabled = enabled_variant;
  }
  return enabled;
}

void irs::tstlan::view_t::controls_t::tick()
{
  if (m_inner_options_event.check()) {
    if (mp_param_box->show()) {
      inner_options_apply();
      if (!mp_local_chart.is_empty()) {
        if (mp_param_box->get_param(irst("Сбросить время")) == irst("true")) {
          mp_param_box->set_param(irst("Сбросить время"), irst("false"));
          reset_chart();
        }
      }
    }
    mp_param_box->save();
  }

  if (mp_data)
  if (mp_data->connected()) {
    if (m_refresh_grid) {
      m_refresh_grid = false;
      int conn_size = m_netconn.connect(mp_data, mp_vars_grid, m_type_col,
        m_index_col);
      int data_size = (int)mp_data->size();
      if (conn_size < data_size) {
        mp_controller->RecordCount =
          mp_controller->RecordCount +
          (data_size - conn_size)/sizeof(irs_i32);
        m_netconn.connect(mp_data, mp_vars_grid, m_type_col, m_index_col);
      }
      mp_vars_ini_file->save_cx_grid_table_view_row_count();
    }
    if (m_first_connect) {
      m_first_connect = false;
      mp_vars_ini_file->load();
    }
    if (m_refresh_csv_state_event.check()) {
      bool csv_start = false;
      if (m_is_csv_on) {
        if (m_refresh_table) {
          csv_start = true;
        } else {
          csv_start = false;
        }
      } else {
        csv_start = false;
      }
      if (csv_start != m_is_csv_opened) {
        if (m_is_csv_opened) {
          close_csv();
        } else {
          open_csv();
        }
      }
    }
    if (m_read_loop_timer.check() && m_refresh_table) {
      // Обработка столбца "Значение" при чтении
      const int row_count = mp_controller->RecordCount;
      if (!m_is_lock) {
        mp_view->BeginUpdate();
        for (int row = 0; row < row_count; row++) {
          mp_controller->Values[row][mp_value_column->Index] =
            var_to_bstr(row);
        }
        mp_view->EndUpdate();
      }
      // Обработка столбца "Значение" при записи
      /*if (is_edit_mode) {
        int write_row = write_var_index;// + m_header_size;
        const int row = mp_table_controller->FocusedRecordIndex;
        const int col = mp_table_controller->FocusedItemIndex;
        bool row_changed = (row != write_row);
        bool col_changed = (col != m_value_col);
        if (row_changed || col_changed) {
          is_edit_mode = false;
        }
        if (is_write_var) {
          is_write_var = false;
          is_edit_mode = false;
          bstr_to_var(write_var_index,
            mp_controller->Values[write_row][mp_value_column->Index]);
        }
      }*/

      // Обработка столбца "Граф."
      if (m_refresh_chart_items) {
        m_refresh_chart_items = false;
        refresh_chart_items();
      }
      m_chart_time = m_time.get() + m_shift_time - m_minus_shift_time;
      for (int row = 0; row < row_count; row++) {
        //continue;
        Variant variant =
          mp_controller->Values[row][mp_chart_column->Index];
        bool chart_enabled = (variant.Type() == varBoolean) ? variant : false;
        if (chart_enabled) {
          int var_index = row;
          const string_type chart_name = get_chart_name(row);
          if (!chart_name.empty()) {
            mp_chart->add(chart_name, m_chart_time,
              static_cast<double>(var_to_long_double(var_index)));
            if (m_is_csv_opened) {
              const string_type csv_name = get_variable_name(row);
              if (m_csv_names[csv_name] && (m_is_csv_opened)) {
                m_csv_file.set_var(irst("Time"),
                  irsstr_from_number_russian(char_t(), m_timer.get()));

                m_csv_file.set_var(csv_name,
                  irsstr_from_number_russian(char_t(),
                  var_to_long_double(var_index)));
              }
            }
          }
        }
      }
      if (m_is_csv_opened) {
        m_csv_file.write_line();
      }
    }
  }
}
template <class T>
void out_number(irs::ostream_t& a_stream, const T& a_value)
{
  switch (irs::type_to_index<T>())
  {
    case irs::bool_idx:
    case irs::signed_char_idx:
    case irs::unsigned_char_idx: {
      a_stream << static_cast<int>(a_value);
    } break;
    default: {
      a_stream << a_value;
    } break;
  }
}

template <class T>
void integer_to_string(
  irs::conn_data_t<T>& a_value, irs::string_t* ap_string)
{
  irs::stringstream_t strm;
  strm.imbue(locale::classic());
  out_number(strm, static_cast<T>(a_value));
  strm << internal << setfill(irst('0')) << hex << uppercase;
  const int type_hex_width = 2*sizeof(T);
  strm << irst(" (0x") << setw(type_hex_width);
  out_number(strm, static_cast<T>(a_value));
  strm << irst(")\0");
  *ap_string = strm.str();
}

String irs::tstlan::view_t::controls_t::var_to_bstr(int a_var_index)
{
  string_type val = 0;
  val.locale_style(irsstrloc_russian);
  netconn_t::item_t item = m_netconn.items[a_var_index];
  switch (item.type) {
    case type_bit: {
      val = m_netconn.bit_vec[item.index];
    } break;
    case type_bool: {
      integer_to_string(m_netconn.bool_vec[item.index], &val);
    } break;
    case type_i8: {
      integer_to_string(m_netconn.i8_vec[item.index], &val);
    } break;
    case type_u8: {
      integer_to_string(m_netconn.u8_vec[item.index], &val);
    } break;
    case type_i16: {
      integer_to_string(m_netconn.i16_vec[item.index], &val);
    } break;
    case type_u16: {
      integer_to_string(m_netconn.u16_vec[item.index], &val);
    } break;
    case type_i32: {
      integer_to_string(m_netconn.i32_vec[item.index], &val);
    } break;
    case type_u32: {
      integer_to_string(m_netconn.u32_vec[item.index], &val);
    } break;
    case type_i64: {
      integer_to_string(m_netconn.i64_vec[item.index], &val);
    } break;
    case type_u64: {
      integer_to_string(m_netconn.u64_vec[item.index], &val);
    } break;
    case type_float: {
      val = m_netconn.float_vec[item.index];
    } break;
    case type_double: {
      val = m_netconn.double_vec[item.index];
    } break;
    case type_long_double: {
      val = m_netconn.long_double_vec[item.index];
    } break;
    default: {
      val = irst("bad");
    } break;
  }
  return val.c_str();
}
void irs::tstlan::view_t::controls_t::bstr_to_var(int a_var_index,
  const String& a_bstr_val)
{
  string_type val = a_bstr_val.c_str();
  val.locale_style(irsstrloc_russian);
  netconn_t::item_t item = m_netconn.items[a_var_index];
  switch (item.type) {
    case type_bit: {
      if (a_bstr_val == irst("1")) {
        m_netconn.bit_vec[item.index] = 1;
      } else {
        m_netconn.bit_vec[item.index] = 0;
      }
    } break;
    case type_bool: {
      bool val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.bool_vec[item.index] = val_conn;
      }
    } break;
    case type_i8: {
      irs_i8 val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.i8_vec[item.index] = val_conn;
      }
    } break;
    case type_u8: {
      irs_u8 val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.u8_vec[item.index] = val_conn;
      }
    } break;
    case type_i16: {
      irs_i16 val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.i16_vec[item.index] = val_conn;
      }
    } break;
    case type_u16: {
      irs_u16 val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.u16_vec[item.index] = val_conn;
      }
    } break;
    case type_i32: {
      irs_i32 val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.i32_vec[item.index] = val_conn;
      }
    } break;
    case type_u32: {
      irs_u32 val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.u32_vec[item.index] = val_conn;
      }
    } break;
    case type_i64: {
      irs_i64 val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.i64_vec[item.index] = val_conn;
      }
    } break;
    case type_u64: {
      irs_u64 val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.u64_vec[item.index] = val_conn;
      }
    } break;
    case type_float: {
      float val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.float_vec[item.index] = val_conn;
      }
    } break;
    case type_double: {
      double val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.double_vec[item.index] = val_conn;
      }
    } break;
    case type_long_double: {
      long double val_conn = 0;
      if (val.to_number(val_conn)) {
        m_netconn.long_double_vec[item.index] = val_conn;
      }
    } break;
  }
}
long double irs::tstlan::view_t::controls_t::var_to_long_double(int a_var_index)
{
  double val = 0;
  netconn_t::item_t item = m_netconn.items[a_var_index];
  switch (item.type) {
    case type_bit: {
      val = m_netconn.bit_vec[item.index];
    } break;
    case type_bool: {
      val = m_netconn.bool_vec[item.index];
    } break;
    case type_i8: {
      val = m_netconn.i8_vec[item.index];
    } break;
    case type_u8: {
      val = m_netconn.u8_vec[item.index];
    } break;
    case type_i16: {
      val = m_netconn.i16_vec[item.index];
    } break;
    case type_u16: {
      val = m_netconn.u16_vec[item.index];
    } break;
    case type_i32: {
      val = m_netconn.i32_vec[item.index];
    } break;
    case type_u32: {
      val = m_netconn.u32_vec[item.index];
    } break;
    case type_i64: {
      val = m_netconn.i64_vec[item.index];
    } break;
    case type_u64: {
      val = m_netconn.u64_vec[item.index];
    } break;
    case type_float: {
      val = m_netconn.float_vec[item.index];
    } break;
    case type_double: {
      val = m_netconn.double_vec[item.index];
    } break;
    case type_long_double: {
      val = m_netconn.long_double_vec[item.index];
    } break;
  }
  return val;
}
void irs::tstlan::view_t::controls_t::fill_grid_index_col()
{
  for (int row = 0; row < mp_controller->RecordCount; row++) {
    mp_controller->Values[row][mp_index_column->Index] = String(row);
  }
}
bool irs::tstlan::view_t::controls_t::is_saveable_col(int a_col)
{
  return ((m_name_col == a_col) || (m_type_col == a_col) ||
    (m_chart_col == a_col));
}
void irs::tstlan::view_t::controls_t::save_grid_row(int a_row)
{
  mp_vars_ini_file->save_cx_grid_table_view_row(a_row);
  m_netconn.connect(mp_data, mp_vars_grid, m_type_col, m_index_col);
}
void irs::tstlan::view_t::controls_t::open_csv()
{
  m_timer.start();
  string_type file_name = file_name_time(irst(".csv"));
  string_type ExePath = ExtractFilePath(Application->ExeName).c_str();
  string_type path = ExePath + irst("Out\\");

  if (mp_local_chart.is_empty()) {
    file_name = m_device_name + irst(" ") + file_name;
  }
  string_type path_file = cbuilder::file_path(path + file_name, irst(".csv"));
  if (m_csv_file.open(path_file)) {
    m_is_csv_opened = true;
    const int row_count = mp_controller->RecordCount;
    m_csv_file.clear_pars();
    m_csv_file.add_col(irst("Time"), ec_str_type);
    for (int row = 0; row < row_count; row++) {
      const string_type csv_name = get_variable_name(row);
      m_csv_names[csv_name] = false;
      bool chart_enabled =
        mp_controller->Values[row][mp_chart_column->Index];
      if (chart_enabled) {
        if (!m_csv_names[csv_name]) {
          m_csv_names[csv_name] = true;
          m_csv_file.add_col(csv_name, ec_str_type);
        }
      }
    }
  } else {
    MessageBox(NULL, irst("CSV Файл не создан!!"), irst("Error"), MB_OK);
  }
}
void irs::tstlan::view_t::controls_t::close_csv()
{
  if (m_csv_file.close()) {
    m_is_csv_opened = false;
  }
}

void __fastcall irs::tstlan::view_t::controls_t::
cxGridTableViewColumnNamePropertiesValidate(TObject *Sender,
  Variant &DisplayValue, TCaption &ErrorText, bool &Error)
{
  mp_view->DataController->PostEditingData();
  const int row = mp_view->Controller->FocusedRecordIndex;
  save_grid_row(row);
}

void __fastcall irs::tstlan::view_t::controls_t::
  cxGridTableView1ColumnTypePropertiesChange(TObject *Sender)
{
  mp_view->DataController->PostEditingData();
  const int row = mp_view->Controller->FocusedRecordIndex;
  save_grid_row(row);
}

void __fastcall irs::tstlan::view_t::controls_t::
cxGridTableViewColumnValuePropertiesChange(TObject *Sender)
{

}

void __fastcall irs::tstlan::view_t::controls_t::
cxGridTableViewColumnValuePropertiesValidate(TObject *Sender,
  Variant &DisplayValue, TCaption &ErrorText, bool &Error)
{
  mp_view->DataController->PostEditingData();
  const int row = mp_view->Controller->FocusedRecordIndex;
  bstr_to_var(row, mp_controller->Values[row][mp_value_column->Index]);
  save_grid_row(row);
}

void __fastcall irs::tstlan::view_t::controls_t::
cxGridTableViewColumnChartPropertiesChange(TObject *Sender)
{
  mp_view->DataController->PostEditingData();
  const int row = mp_view->Controller->FocusedRecordIndex;
  refresh_chart_items();
  save_grid_row(row);
}

void __fastcall irs::tstlan::view_t::controls_t::VarsGridKeyDown(
  TObject *Sender, WORD &Key, TShiftState Shift)
{
  // Вставака/удаление строк
  if (Shift.Contains(ssCtrl)) {
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
void __fastcall irs::tstlan::view_t::controls_t::ChartBtnClick(
  TObject *Sender)
{
  mp_chart->show();
}
void __fastcall irs::tstlan::view_t::controls_t::CsvSaveBtnClick(
  TObject *Sender)
{
  m_refresh_csv_state_event.exec();
  if (!m_start) {
    m_start = true;
    m_refresh_table = false;
    mp_start_btn->Caption = mp_read_on_text;
    m_minus_shift_time = m_time.get();
  } else {
    m_start = false;
    m_refresh_table = true;
    mp_start_btn->Caption = mp_read_off_text;
    m_minus_shift_time = m_time.get() - m_minus_shift_time;
  }
}
void __fastcall irs::tstlan::view_t::controls_t::CsvLoadBtnClick(
  TObject *Sender)
{
  if (mp_open_dialog->Execute()) {
    string_type csv_file_name = mp_open_dialog->FileName.c_str();
    m_csv_open_file.set_file_name(csv_file_name);
    m_csv_open_file.load(&m_table);

    size_t col_size = m_table.get_col_count();
    size_t row_size = m_table.get_row_count();
    if (col_size > 0) {
      col_size = col_size-1;
    }
    bool is_null = false;
    double time_chart = m_chart_time;
    string_type time_zero_str =  m_table.read_cell(0, 1);
    double time_zero = static_cast<double>(StrToFloat(time_zero_str.c_str()));
    for (size_t col_index = 1; col_index < col_size; col_index++) {
      const string_type variable_name = m_table.read_cell(col_index, 0);
      const string_type chart_name = make_chart_name(variable_name);
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
    m_chart_time = m_chart_time +
      static_cast<double>(StrToFloat(time_end.c_str())) - time_zero; 
  }
}
void __fastcall irs::tstlan::view_t::controls_t::OptionsBtnClick(
  TObject *Sender)
{
  if (mp_event) {
    mp_event->exec();
  } else {
    m_inner_options_event.exec();
  }
}
void __fastcall irs::tstlan::view_t::controls_t::LoadTableValuesBtnClick(
  TObject *Sender)
{
  handle_t<TOpenDialog> open_dialog(new TOpenDialog(NULL));
  if (open_dialog->Execute()) {
    string_type file_name = str_conv<string_type>(open_dialog->FileName);
    load_table_values(file_name);
  }
}
void __fastcall irs::tstlan::view_t::controls_t::SaveTableValuesBtnClick(
  TObject *Sender)
{
  handle_t<TOpenDialog> open_dialog(new TOpenDialog(NULL));
  if (open_dialog->Execute()) {
    string_type file_name = str_conv<string_type>(open_dialog->FileName);
    save_table_values(file_name);
  }
}
void __fastcall irs::tstlan::view_t::controls_t::GridInsertClick(
  TObject *Sender)
{
  m_refresh_grid = true;
  mp_controller->InsertRecord(mp_table_controller->FocusedRecordIndex);
  mp_vars_ini_file->save();
}
void __fastcall irs::tstlan::view_t::controls_t::GridDeleteClick(
  TObject *Sender)
{
  m_refresh_grid = true;
  mp_controller->DeleteFocused();
  mp_vars_ini_file->save();
}
void __fastcall irs::tstlan::view_t::controls_t::FormHide(TObject *Sender)
{
  m_is_lock = true;
  if (!mp_local_chart.is_empty()) {
    mp_chart->hide();
  }
}
void __fastcall irs::tstlan::view_t::controls_t::FormShow(TObject *Sender)
{
  m_is_lock = false;
  m_refresh_grid = true;
  measure_time_t t;
  t.start();
  m_ini_file.load();
  double time = t.get();
  int i = 0;
}
// Консоль внутри tstlan5
TMemo* irs::tstlan::view_t::controls_t::log()
{
  return mp_log_memo;
}
void irs::tstlan::view_t::controls_t::options_event_connect(event_t* ap_event)
{
  mp_event = ap_event;
}
void irs::tstlan::view_t::controls_t::options_event_clear()
{
  mp_event = IRS_NULL;
}
irs::event_t* irs::tstlan::view_t::controls_t::inner_options_event()
{
  return &m_inner_options_event;
}
void irs::tstlan::view_t::controls_t::save_conf()
{
  m_ini_file.save();
  mp_vars_ini_file->save();
  mp_param_box->save();
}
void irs::tstlan::view_t::controls_t::load_conf()
{
  m_ini_file.load();
  mp_vars_ini_file->load();
  fill_grid_index_col();
  mp_param_box->load();
  inner_options_apply();
}
void irs::tstlan::view_t::controls_t::clear_conf()
{
  mp_controller->RecordCount = m_grid_size;
  mp_vars_ini_file->save();
}
irs::tstlan::view_t::string_type
irs::tstlan::view_t::controls_t::conf_section()
{
  return m_ini_section_prefix;
}
void irs::tstlan::view_t::controls_t::conf_section(
  const string_type& a_ini_section_prefix)
{
  m_ini_section_prefix = a_ini_section_prefix;
  m_ini_file.clear_control();
  m_ini_file.set_section(
    irs::str_conv<String>(m_ini_section_prefix + m_ini_options_section));
  m_ini_file.add("tstlan4lib_form_", mp_form);
  mp_vars_ini_file->set_section(m_ini_section_prefix);
}
irs::tstlan::view_t::string_type irs::tstlan::view_t::controls_t::ini_name()
{
  return m_ini_file.ini_name().c_str();
}
void irs::tstlan::view_t::controls_t::ini_name(const string_type& a_ini_name)
{
  m_ini_file.set_ini_name(a_ini_name.c_str());
  mp_vars_ini_file->set_ini_name(a_ini_name.c_str());
  mp_param_box->set_ini_name(a_ini_name);
  m_device_name = extract_file_ultra_short_name(a_ini_name);
}

void irs::tstlan::view_t::controls_t::save_table_values(
  const string_type& a_file_name) const
{
  if (static_cast<std::size_t>(mp_controller->RecordCount) >
    m_netconn.items.size()) {
    Application->MessageBox(
      irst("Для сохранения значений необходимо, чтобы произошло подключение"),
      irst("Ошибка"),
      MB_OK + MB_ICONERROR);
    return;
  }

  table_string_t table;

  const int row_count = mp_controller->RecordCount;
  table.set_col_count(m_csv_table_col_count);
  table.set_row_count(mp_controller->RecordCount );

  for (int row = 0; row < row_count; row++) {
    int var_index = row;// - m_header_size;
    netconn_t::item_t item = m_netconn.items[var_index];
    string_type type_str = type_to_str(item.type);
    table.write_cell(m_csv_table_conn_index_col_index, var_index,
      num_to_str(item.conn_index));
    table.write_cell(m_csv_table_bit_index_col_index, var_index,
      num_to_str(item.bit_index));

    String name_bstr = mp_controller->Values[row][mp_name_column->Index];
    table.write_cell(m_csv_table_name_col_index, var_index,
      irs::str_conv<string_type>(name_bstr));
    table.write_cell(m_csv_table_type_col_index, var_index, type_str);
    String value_bstr = mp_controller->Values[row][mp_value_column->Index];
    table.write_cell(m_csv_table_value_index_col_index, var_index,
      irs::str_conv<string_type>(value_bstr));
  }
  csvwork::csv_file_synchro_t csv_file(a_file_name);
  csv_file.save(table);
}

void irs::tstlan::view_t::controls_t::load_table_values(
  const string_type& a_file_name)
{
  if (!mp_data->connected()) {
    Application->MessageBox(
      irst("Для загрузки значений необходимо подключение"),
      irst("Ошибка"),
      MB_OK + MB_ICONERROR);
    return;
  }
  table_string_t table;
  csvwork::csv_file_synchro_t csv_file(a_file_name);
  csv_file.load(&table);

  if (table.get_col_count() < m_csv_table_col_count) {
    Application->MessageBox(
      irst("В загруженной таблице мало столбцов"),
      irst("Ошибка"),
      MB_OK + MB_ICONERROR);
    return;
  }

  int row_count = std::min(static_cast<std::size_t>(mp_controller->RecordCount),
    m_netconn.items.size());
  std::vector<int> rows_with_errors;
  for (int row = 0; row < row_count; row++) {
    int var_index = row;// - m_header_size;
    netconn_t::item_t item = m_netconn.items[var_index];
    string_type conn_index_str = table.read_cell(
      m_csv_table_conn_index_col_index, var_index);
    int conn_index = 0;
    if (!str_to_num(conn_index_str, &conn_index)) {
      rows_with_errors.push_back(var_index);
      continue;
    }
    if (conn_index != item.conn_index) {
      rows_with_errors.push_back(var_index);
      continue;
    }
    string_type bit_index_str = table.read_cell(
      m_csv_table_bit_index_col_index, var_index);
    int bit_index = 0;
    if (!str_to_num(bit_index_str, &bit_index)) {
      rows_with_errors.push_back(var_index);
      continue;
    }
    if (bit_index != item.bit_index) {
      rows_with_errors.push_back(var_index);
      continue;
    }

    string_type type_str = table.read_cell(m_csv_table_type_col_index,
      var_index);
    type_t type = type_unknown;
    if (!str_to_type(type_str, &type)) {
      rows_with_errors.push_back(var_index);
      continue;
    }
    if (type != item.type) {
      rows_with_errors.push_back(var_index);
      continue;
    }
    string_type value_str =
      table.read_cell(m_csv_table_value_index_col_index, var_index);
    bstr_to_var(var_index, irs::str_conv<String>(value_str));
  }
  if (!rows_with_errors.empty()) {
    irs::ostringstream_t error_msg_ostr;
    error_msg_ostr <<
      irst("При загрузке возникли ошибки в следующих строках: ");
    for (std::size_t i = 0; i < rows_with_errors.size(); i++) {
      if (i > 0) {
        error_msg_ostr << irst(", ");
      }
      error_msg_ostr << rows_with_errors[i];
    }
    error_msg_ostr << irst(".");
    Application->MessageBox(
      error_msg_ostr.str().c_str(),
      irst("Ошибка"),
      MB_OK + MB_ICONWARNING);
  }
}

#endif // IRS_USE_DEV_EXPRESS

