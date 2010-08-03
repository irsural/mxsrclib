// Дата: 03.08.2010
// Дата создания: 29.07.2010

#include <irsdefs.h>

#include <irsparam.h>
#include <irssysutils.h>

#include <irsfinal.h>

irs::param_box_t::param_box_t(
  const string_type& a_param_box_name,
  const string_type& a_ini_section,
  const string_type& a_ini_name
):
  m_ini_section(a_ini_section),
  m_ini_file(),
  mp_form(new TForm(IRS_NULL, 1)),
  mp_panel(new TPanel(mp_form.get())),
  mp_ok_btn(new TButton(mp_form.get())),
  mp_cansel_btn(new TButton(mp_form.get())),
  mp_param_grid(new TStringGrid(mp_form.get())),
  m_cur_param_row(0)
{
  mp_form->Width = 280;
  mp_form->Height = 400;
  mp_form->Constraints->MinWidth = 280;
  mp_form->Constraints->MinHeight = 400;
  mp_form->BorderStyle = bsSizeable;
  mp_form->Position = poDesktopCenter;
  mp_form->Caption = a_param_box_name.c_str();

  mp_panel->Align = alBottom;
  mp_panel->Parent = mp_form.get();
  mp_panel->Top = 0;

  mp_ok_btn->Parent = mp_panel;
  mp_ok_btn->Anchors >> akLeft;
  mp_ok_btn->Anchors << akRight;
  mp_ok_btn->Left = 0;
  mp_ok_btn->Top = mp_panel->Height/2 - mp_ok_btn->Height/2;
  mp_ok_btn->Caption = "OK";
  mp_ok_btn->Default = true;
  mp_ok_btn->ModalResult = mrOk;
  mp_ok_btn->OnClick = OK_BtnClick;

  mp_cansel_btn->Parent = mp_panel;
  mp_cansel_btn->Anchors >> akLeft;
  mp_cansel_btn->Anchors << akRight;
  mp_cansel_btn->Left = mp_ok_btn->Left + mp_ok_btn->Width + btn_gap;
  mp_cansel_btn->Top = mp_panel->Height/2 - mp_cansel_btn->Height/2;
  mp_cansel_btn->Caption = "Отмена";
  mp_cansel_btn->Cancel = true;
  mp_cansel_btn->ModalResult = mrCancel;
  mp_cansel_btn->OnClick = Cancel_BtnClick;
      
  mp_param_grid->Parent = mp_form.get();
  mp_param_grid->Align = alClient;
  mp_param_grid->DefaultRowHeight = 17;
  mp_param_grid->RowCount = vars_count_start + header_size;
  mp_param_grid->ColCount = col_count;
  mp_param_grid->Height = mp_form->Height - mp_panel->Height;
  mp_param_grid->Options = mp_param_grid->Options << goEditing <<
    goColSizing;
  mp_param_grid->Cells[header_col][m_cur_param_row] = "Параметр";
  mp_param_grid->Cells[options_col][m_cur_param_row] = "Значение";
  mp_param_grid->ColWidths[header_col] = header_col_width;
  mp_param_grid->ColWidths[options_col] = options_col_width;
  mp_param_grid->Width = grid_width;
  mp_param_grid->FixedRows = 1;

  if (a_ini_name == irst("")) {
    m_ini_file.set_ini_name(m_ini_file.ini_name().c_str());
  } else {
    m_ini_file.set_ini_name(a_ini_name.c_str());
  }
  m_ini_file.set_section(m_ini_section.c_str());
  m_ini_file.add("", mp_param_grid, "Name", header_col);
  m_ini_file.add("", mp_param_grid, "Value", options_col);
  m_ini_file.load();
}

void __fastcall irs::param_box_t::OK_BtnClick(TObject *Sender)
{
  m_ini_file.save();
}

void __fastcall irs::param_box_t::Cancel_BtnClick(TObject *Sender)
{
  m_ini_file.load();
}

irs::param_box_t::~param_box_t()
{

}

const irs::param_box_t::char_type* irs::param_box_t::def_param_box_name()
{
  return irst("Настройки");
}

const irs::param_box_t::char_type* irs::param_box_t::def_ini_section()
{
  return irst("Options");
}

const irs::param_box_t::char_type* irs::param_box_t::def_ini_name()
{
  return irst("");
}

void irs::param_box_t::set_ini_name(const string_type& a_ini_name)
{
  m_ini_file.set_ini_name(a_ini_name.c_str());
}

irs::param_box_t::string_type irs::param_box_t::ini_name()
{
  return m_ini_file.ini_name().c_str();
}

void irs::param_box_t::show()
{
  mp_form->ShowModal();
}

void irs::param_box_t::hide()
{
  mp_form->Hide();
}

void irs::param_box_t::add(const builder_string_type& a_param_name,
  const builder_string_type& a_param_value)
{
  m_cur_param_row++;
  mp_param_grid->RowCount = mp_param_grid->RowCount + 1;
  mp_param_grid->Cells[header_col][m_cur_param_row] = a_param_name.c_str();
  mp_param_grid->Cells[option_col][m_cur_param_row] = a_param_value.c_str();
}

#ifdef NOP
irs::param_box_t::builder_string_type irs::param_box_t::get_param_as_string(
  const string_type& a_param_name)
{
  builder_string_type param_name = a_param_name.c_str();
  builder_string_type param_value;
  for (size_t grid_index = 0;
    grid_index < static_cast<size_t>(mp_param_grid->RowCount); grid_index++)
  {
    if (mp_param_grid->Cells[header_col][grid_index] == param_name) {
      param_value = mp_param_grid->Cells[option_col][grid_index];
    }
  }
  return param_value;
}
#endif // NOP

bool irs::param_box_t::get_param(const string_type& a_param_name,
  builder_string_type* ap_param_value)
{
  builder_string_type param_name = a_param_name.c_str();
  size_t grid_index = 0;
  if (get_row_by_param_name(param_name, &grid_index)) {
    *ap_param_value = mp_param_grid->Cells[option_col][grid_index];
    return true;
  }
  return false;
}

bool irs::param_box_t::set_param(const string_type& a_param_name,
  const string_type& a_param_value)
{
  builder_string_type param_name = a_param_name.c_str();
  size_t grid_index = 0;
  if (get_row_by_param_name(param_name, &grid_index)) {
    mp_param_grid->Cells[option_col][grid_index] = a_param_value.c_str();
    return true;
  }
  return false;
}

bool irs::param_box_t::get_row_by_param_name(builder_string_type& a_param_name,
  size_t* ap_row_index)
{
  for (size_t grid_index = 0;
    grid_index < static_cast<size_t>(mp_param_grid->RowCount); grid_index++)
  {
    if (mp_param_grid->Cells[header_col][grid_index] == a_param_name) {
      *ap_row_index = grid_index;
      return true;
    }
  }
  return false;
}

#ifdef NOP
bool irs::param_box_t::get_param_as_u8(const string_type& a_param_name,
  irs_u8* ap_param_value)
{
  return get_param(a_param_name, ap_param_value);
}

bool irs::param_box_t::get_param_as_i8(const string_type& a_param_name,
  irs_i8* ap_param_value)
{
  return get_param(a_param_name, ap_param_value);
}

bool irs::param_box_t::get_param_as_u16(const string_type& a_param_name,
  irs_u16* ap_param_value)
{
  return get_param(a_param_name, ap_param_value);
}

bool irs::param_box_t::get_param_as_i16(const string_type& a_param_name,
  irs_i16* ap_param_value)
{
  return get_param(a_param_name, ap_param_value);
}

bool irs::param_box_t::get_param_as_u32(const string_type& a_param_name,
  irs_u32* ap_param_value)
{
  return get_param(a_param_name, ap_param_value);
}

bool irs::param_box_t::get_param_as_i32(const string_type& a_param_name,
  irs_i32* ap_param_value)
{
  return get_param(a_param_name, ap_param_value);
}

bool irs::param_box_t::get_param_as_u64(const string_type& a_param_name,
  irs_u64* ap_param_value)
{
  return get_param(a_param_name, ap_param_value);
}

bool irs::param_box_t::get_param_as_i64(const string_type& a_param_name,
  irs_i64* ap_param_value)
{
  return get_param(a_param_name, ap_param_value);
}

bool irs::param_box_t::get_param_as_float(const string_type& a_param_name,
  float* ap_param_value)
{
  return get_param(a_param_name, ap_param_value);
}

bool irs::param_box_t::get_param_as_double(const string_type& a_param_name,
  double* ap_param_value)
{
  return get_param(a_param_name, ap_param_value);
}

bool irs::param_box_t::get_param_as_long_double(const string_type& a_param_name,
  long double* ap_param_value)
{
  return get_param(a_param_name, ap_param_value);
}
#endif // NOP

void irs::param_box_t::save()
{
  m_ini_file.save();
}

void irs::param_box_t::load()
{
  m_ini_file.load();
}
