// Работа с ini-файлами
// Дата: 14.04.2010
// Дата создания: 10.09.2009

#include <irsdefs.h>

#include <vcl.h>
#pragma hdrstop
#pragma link "cspin"

#include <cbsysutils.h>

#include <mxini.h>

#include <irsfinal.h>
//---------------------------------------------------------------------------
// Работа с ini-файлами

irs::ini_file_t::ini_file_t():
  m_edits(),
  m_check_boxs(),
  m_page_controls(),
  m_group_boxs(),
  m_combo_boxs(),
  m_radio_groups(),
  m_ini_name(),
  m_section("Options")
{
  String ExeName = Application->ExeName;
  int Length = ExeName.Length();
  if (Length < 3) throw bad_exe_path();
  m_ini_name = ExeName.SubString(1, Length - 3).c_str();
  m_ini_name += "ini";
}
irs::ini_file_t::~ini_file_t()
{
  //save();
}
void irs::ini_file_t::set_ini_name(const String& a_ini_name)
{
  const char *str_begin = static_cast<AnsiString>(a_ini_name).c_str();
  const char *str_end = str_begin + a_ini_name.Length();
  if (find(str_begin, str_end, '\\') != str_end) {
    m_ini_name = a_ini_name.c_str();
  } else {
    String ExePath = ExtractFilePath(Application->ExeName);
    irs::string exe_path = ExePath.c_str();
    m_ini_name = exe_path + irs::string(a_ini_name.c_str());
  }
}
String irs::ini_file_t::ini_name()
{
  return m_ini_name.c_str();
}
void irs::ini_file_t::set_section(const String& a_name)
{
  m_section = a_name.c_str();
}
void irs::ini_file_t::add(const String& a_name, bool* a_control)
{
  irs::string name = a_name.c_str();
  bool_t bool_var(m_section, name, a_control);
  if(find(mv_bools.begin(), mv_bools.end(), bool_var) == mv_bools.end()) {
    mv_bools.push_back(bool_var);
  }
}
void irs::ini_file_t::add(const String& a_name, irs_u8* a_control)
{
  irs::string name = a_name.c_str();
  irs_u8_t irs_u8_var(m_section, name, a_control);
  if(find(mv_irs_u8s.begin(), mv_irs_u8s.end(),
    irs_u8_var) == mv_irs_u8s.end())
  {
    mv_irs_u8s.push_back(irs_u8_var);
  }
}
void irs::ini_file_t::add(const String& a_name, irs_i8* a_control)
{
  irs::string name = a_name.c_str();
  irs_i8_t irs_i8_var(m_section, name, a_control);
  if(find(mv_irs_i8s.begin(), mv_irs_i8s.end(),
    irs_i8_var) == mv_irs_i8s.end())
  {
    mv_irs_i8s.push_back(irs_i8_var);
  }
}
void irs::ini_file_t::add(const String& a_name, irs_u16* a_control)
{
  irs::string name = a_name.c_str();
  irs_u16_t irs_u16_var(m_section, name, a_control);
  if(
    find(
      mv_irs_u16s.begin(),
      mv_irs_u16s.end(),
      irs_u16_var) == mv_irs_u16s.end()){
    mv_irs_u16s.push_back(irs_u16_var);
  }
}
void irs::ini_file_t::add(const String& a_name, irs_i16* a_control)
{
  irs::string name = a_name.c_str();
  irs_i16_t irs_i16_var(m_section, name, a_control);
  if(
    find(
      mv_irs_i16s.begin(),
      mv_irs_i16s.end(),
      irs_i16_var) == mv_irs_i16s.end()){
    mv_irs_i16s.push_back(irs_i16_var);
  }
}
void irs::ini_file_t::add(const String& a_name, irs_u32* a_control)
{
  irs::string name = a_name.c_str();
  irs_u32_t irs_u32_var(m_section, name, a_control);
  if(
    find(
      mv_irs_u32s.begin(),
      mv_irs_u32s.end(),
      irs_u32_var) == mv_irs_u32s.end()){
    mv_irs_u32s.push_back(irs_u32_var);
  }
}
void irs::ini_file_t::add(const String& a_name, irs_i32* a_control)
{
  irs::string name = a_name.c_str();
  irs_i32_t irs_i32_var(m_section, name, a_control);
  if(
    find(
      mv_irs_i32s.begin(),
      mv_irs_i32s.end(),
      irs_i32_var) == mv_irs_i32s.end()){
    mv_irs_i32s.push_back(irs_i32_var);
  }
}
void irs::ini_file_t::add(const String& a_name, irs_u64* a_control)
{
  irs::string name = a_name.c_str();
  irs_u64_t irs_u64_var(m_section, name, a_control);
  if(
    find(
      mv_irs_u64s.begin(),
      mv_irs_u64s.end(),
      irs_u64_var) == mv_irs_u64s.end()){
    mv_irs_u64s.push_back(irs_u64_var);
  }
}
void irs::ini_file_t::add(const String& a_name, irs_i64* a_control)
{
  irs::string name = a_name.c_str();
  irs_i64_t irs_i64_var(m_section, name, a_control);
  if(
    find(
      mv_irs_i64s.begin(),
      mv_irs_i64s.end(),
      irs_i64_var) == mv_irs_i64s.end()){
    mv_irs_i64s.push_back(irs_i64_var);
  }
}
void irs::ini_file_t::add(const String& a_name, float* a_control)
{
  irs::string name = a_name.c_str();
  float_t float_var(m_section, name, a_control);
  if(
    find(
      mv_floats.begin(),
      mv_floats.end(),
      float_var) == mv_floats.end()){
    mv_floats.push_back(float_var);
  }
}
void irs::ini_file_t::add(const String& a_name, double* a_control)
{
  irs::string name = a_name.c_str();
  double_t double_var(m_section, name, a_control);
  if(
    find(
      mv_doubles.begin(),
      mv_doubles.end(),
      double_var) == mv_doubles.end()){
    mv_doubles.push_back(double_var);
  }
}
void irs::ini_file_t::add(const String& a_name, long double* a_control)
{
  irs::string name = a_name.c_str();
  long_double_t long_double_var(m_section, name, a_control);
  if(
    find(
      mv_long_doubles.begin(),
      mv_long_doubles.end(),
      long_double_var) == mv_long_doubles.end()){
    mv_long_doubles.push_back(long_double_var);
  }
}
void irs::ini_file_t::add(const String& a_name, irs::string* a_control)
{
  irs::string name = a_name.c_str();
  irs_string_t irs_string_var(m_section, name, a_control);
  if(
    find(
      mv_irs_strings.begin(),
      mv_irs_strings.end(),
      irs_string_var) == mv_irs_strings.end()){
    mv_irs_strings.push_back(irs_string_var);
  }
}
void irs::ini_file_t::add(const String& a_name, AnsiString* a_control)
{
  irs::string name = a_name.c_str();
  ansi_string_t ansi_string_var(m_section, name, a_control);
  if(
    find(
      mv_ansi_strings.begin(),
      mv_ansi_strings.end(),
      ansi_string_var) == mv_ansi_strings.end()){
    mv_ansi_strings.push_back(ansi_string_var);
  }
}
void irs::ini_file_t::add(const String& a_name, TEdit *a_control)
{
  irs::string name = a_name.c_str();
  edit_t edit(m_section, name, a_control);
  if (find(m_edits.begin(), m_edits.end(), edit) == m_edits.end()) {
    m_edits.push_back(edit);
  }
}
void irs::ini_file_t::add(const String& a_name, TCSpinEdit *a_control)
{
  irs::string name = a_name.c_str();
  cspin_edit_t cspin_edit(m_section, name, a_control);
  if (find(m_cspin_edits.begin(), m_cspin_edits.end(), cspin_edit) ==
    m_cspin_edits.end())
  {
    m_cspin_edits.push_back(cspin_edit);
  }
}
void irs::ini_file_t::add(const String& a_name, TCheckBox *a_control)
{
  irs::string name = a_name.c_str();
  check_box_t check_box(m_section, name, a_control);
  if (find(m_check_boxs.begin(), m_check_boxs.end(), check_box) ==
    m_check_boxs.end()) {
    m_check_boxs.push_back(check_box);
  }
}
void irs::ini_file_t::add(const String& a_name, TRadioButton *a_control)
{
  irs::string name = a_name.c_str();
  radio_button_t radio_button(m_section, name, a_control);
  if (find(m_radio_buttons.begin(), m_radio_buttons.end(), radio_button) ==
    m_radio_buttons.end()) {
    m_radio_buttons.push_back(radio_button);
  }
}
void irs::ini_file_t::add(const String& a_name, TPageControl *a_control)
{
  irs::string name = a_name.c_str();
  page_control_t page_control(m_section, name, a_control);
  if (find(m_page_controls.begin(), m_page_controls.end(), page_control) ==
    m_page_controls.end()) {
    m_page_controls.push_back(page_control);
  }
}
void irs::ini_file_t::add(const String& a_name, TGroupBox *a_control)
{
  irs::string name = a_name.c_str();
  group_box_t group_box(m_section, name, a_control);
  if (find(m_group_boxs.begin(), m_group_boxs.end(), group_box) ==
    m_group_boxs.end()) {
    m_group_boxs.push_back(group_box);
  }
}
void irs::ini_file_t::add(const String& a_name, TComboBox *a_control)
{
  irs::string name = a_name.c_str();
  combo_box_t combo_box(m_section, name, a_control);
  if (find(m_combo_boxs.begin(), m_combo_boxs.end(), combo_box) ==
    m_combo_boxs.end()) {
    m_combo_boxs.push_back(combo_box);
  }
}
void irs::ini_file_t::add(const String& a_name, TRadioGroup *a_control)
{
  irs::string name = a_name.c_str();
  radio_group_t radio_group(m_section, name, a_control);
  if (find(m_radio_groups.begin(), m_radio_groups.end(), radio_group) ==
    m_radio_groups.end()) {
    m_radio_groups.push_back(radio_group);
  }
}
void irs::ini_file_t::add(const String& a_name, TStringGrid *a_control)
{
  irs::string name = a_name.c_str();
  m_string_grids[a_control] = string_grid_t(m_section, name);
}
void irs::ini_file_t::add(const String& a_name, TStringGrid *a_control,
  const String& a_column_name, int a_column_index)
{
  irs::string name = a_name.c_str();
  if (m_string_grids.count(a_control) == 0) {
    m_string_grids[a_control] = string_grid_t(m_section, name);
  }
  irs::string column_name = a_column_name.c_str();
  m_string_grids[a_control].add(a_column_index, column_name);
}
void irs::ini_file_t::load()
{
  if (FileExists(m_ini_name.c_str())) {
    auto_ptr<TIniFile> IniFile(new TIniFile(m_ini_name.c_str()));
    for (vector<bool_t>::iterator bool_it = mv_bools.begin();
      bool_it != mv_bools.end(); bool_it++) {
      *(bool_it->control) = IniFile->ReadBool(
        bool_it->section.c_str(),
        bool_it->name.c_str(),
        *(bool_it->control)
      );
    }
    for (vector<irs_u8_t>::iterator irs_u8_it = mv_irs_u8s.begin();
      irs_u8_it != mv_irs_u8s.end(); irs_u8_it++) {
      *(irs_u8_it->control) = static_cast<irs_u8>(IniFile->ReadInteger(
        irs_u8_it->section.c_str(),
        irs_u8_it->name.c_str(),
        *(irs_u8_it->control)));
    }
    for (vector<irs_i8_t>::iterator irs_i8_it = mv_irs_i8s.begin();
      irs_i8_it != mv_irs_i8s.end(); irs_i8_it++) {
      *(irs_i8_it->control) = static_cast<irs_u8>(IniFile->ReadInteger(
        irs_i8_it->section.c_str(),
        irs_i8_it->name.c_str(),
        *(irs_i8_it->control)));
    }
    for (vector<irs_u16_t>::iterator irs_u16_it = mv_irs_u16s.begin();
      irs_u16_it != mv_irs_u16s.end(); irs_u16_it++) {
      *(irs_u16_it->control) = static_cast<irs_u16>(IniFile->ReadInteger(
        irs_u16_it->section.c_str(),
        irs_u16_it->name.c_str(),
        *(irs_u16_it->control)));
    }
    for (vector<irs_i16_t>::iterator irs_i16_it = mv_irs_i16s.begin();
      irs_i16_it != mv_irs_i16s.end(); irs_i16_it++) {
      *(irs_i16_it->control) = static_cast<irs_i16>(IniFile->ReadInteger(
        irs_i16_it->section.c_str(),
        irs_i16_it->name.c_str(),
        *(irs_i16_it->control)));
    }
    for (vector<irs_u32_t>::iterator irs_u32_it = mv_irs_u32s.begin();
      irs_u32_it != mv_irs_u32s.end(); irs_u32_it++) {
      *(irs_u32_it->control) = static_cast<irs_u32>(IniFile->ReadInteger(
        irs_u32_it->section.c_str(),
        irs_u32_it->name.c_str(),
        *(irs_u32_it->control)));
    }
    for (vector<irs_i32_t>::iterator irs_i32_it = mv_irs_i32s.begin();
      irs_i32_it != mv_irs_i32s.end(); irs_i32_it++) {
      *(irs_i32_it->control) = IniFile->ReadInteger(
        irs_i32_it->section.c_str(),
        irs_i32_it->name.c_str(),
        *(irs_i32_it->control));
    }
    for (vector<irs_u64_t>::iterator irs_u64_it = mv_irs_u64s.begin();
      irs_u64_it != mv_irs_u64s.end(); irs_u64_it++) {
      irs::string var_str = *(irs_u64_it->control);
      var_str = (IniFile->ReadString(
        irs_u64_it->section.c_str(),
        irs_u64_it->name.c_str(),
        var_str.c_str()).c_str());
      var_str.to_number<irs_u64>(*(irs_u64_it->control));
    }
    for (vector<irs_i64_t>::iterator irs_i64_it = mv_irs_i64s.begin();
      irs_i64_it != mv_irs_i64s.end(); irs_i64_it++) {
      irs::string var_str = *(irs_i64_it->control);
      var_str = (IniFile->ReadString(
        irs_i64_it->section.c_str(),
        irs_i64_it->name.c_str(),
        var_str.c_str()).c_str());
      var_str.to_number<irs_i64>(*(irs_i64_it->control));
    }
    for (vector<float_t>::iterator float_it = mv_floats.begin();
      float_it != mv_floats.end(); float_it++) {
      *(float_it->control) =
        static_cast<float>(
          IniFile->ReadFloat(
          float_it->section.c_str(),
          float_it->name.c_str(),
          *(float_it->control))
        );
    }
    for (vector<double_t>::iterator double_it = mv_doubles.begin();
      double_it != mv_doubles.end(); double_it++) {
      *(double_it->control) = IniFile->ReadFloat(
        double_it->section.c_str(),
        double_it->name.c_str(),
        *(double_it->control));
    }
    for (
      vector<long_double_t>::iterator long_double_it = mv_long_doubles.begin();
      long_double_it != mv_long_doubles.end(); long_double_it++) {
      irs::string var_str = *(long_double_it->control);
      var_str = (IniFile->ReadString(
        long_double_it->section.c_str(),
        long_double_it->name.c_str(),
        var_str.c_str()).c_str());
      var_str.to_number<long double>(*(long_double_it->control));
    }
    for (
      vector<irs_string_t>::iterator irs_string_it = mv_irs_strings.begin();
      irs_string_it != mv_irs_strings.end(); irs_string_it++) {
      irs::string var_str = *(irs_string_it->control);
      var_str = IniFile->ReadString(
        irs_string_it->section.c_str(),
        irs_string_it->name.c_str(),
        var_str.c_str()).c_str();
      *(irs_string_it->control) = var_str;
    }
    for (
      vector<ansi_string_t>::iterator ansi_string_it = mv_ansi_strings.begin();
      ansi_string_it != mv_ansi_strings.end(); ansi_string_it++) {
      *(ansi_string_it->control) = IniFile->ReadString(
        ansi_string_it->section.c_str(),
        ansi_string_it->name.c_str(),
        *(ansi_string_it->control));
    }
    for (vector<edit_t>::iterator edit_it = m_edits.begin();
      edit_it != m_edits.end(); edit_it++) {
      (*edit_it).control->Text = IniFile->ReadString(
        (*edit_it).section.c_str(),
        (*edit_it).name.c_str(),
        (*edit_it).control->Text
      );
    }
    for (vector<cspin_edit_t>::iterator cspin_edit_it = m_cspin_edits.begin();
      cspin_edit_it != m_cspin_edits.end(); cspin_edit_it++) {
      (*cspin_edit_it).control->Text = IniFile->ReadString(
        (*cspin_edit_it).section.c_str(),
        (*cspin_edit_it).name.c_str(),
        (*cspin_edit_it).control->Text
      );
    }
    for (vector<check_box_t>::iterator cb_it = m_check_boxs.begin();
      cb_it != m_check_boxs.end(); cb_it++) {
      (*cb_it).control->Checked = IniFile->ReadBool((*cb_it).section.c_str(),
        (*cb_it).name.c_str(), (*cb_it).control->Checked);
    }
    for (vector<radio_button_t>::iterator rb_it = m_radio_buttons.begin();
      rb_it != m_radio_buttons.end(); rb_it++) {
      (*rb_it).control->Checked = IniFile->ReadBool((*rb_it).section.c_str(),
        (*rb_it).name.c_str(), (*rb_it).control->Checked);
    }
    for (vector<page_control_t>::iterator pc_it = m_page_controls.begin();
      pc_it != m_page_controls.end(); pc_it++) {
      TTabSheet *ts =
        FindTabSheet(
          (*pc_it).control,
          IniFile->ReadString(
            (*pc_it).section.c_str(),
            (*pc_it).name.c_str(),
            (*pc_it).control->ActivePage->Name
          )
        );
      if (ts) (*pc_it).control->ActivePage = ts;
    }
    for (vector<group_box_t>::iterator gb_it = m_group_boxs.begin();
      gb_it != m_group_boxs.end(); gb_it++) {
      TRadioButton *gb =
        FindRadioButton(
          (*gb_it).control,
          IniFile->ReadString(
            (*gb_it).section.c_str(),
            (*gb_it).name.c_str(),
            ActiveRadioButton((*gb_it).control)->Name
          )
        );
      if (gb) gb->Checked = true;
    }
    for (vector<combo_box_t>::iterator combo_box_it = m_combo_boxs.begin();
      combo_box_it != m_combo_boxs.end(); combo_box_it++) {
      int ComboIndex =
        FindComboBoxItem(
          (*combo_box_it).control,
          IniFile->ReadString(
            (*combo_box_it).section.c_str(),
            (*combo_box_it).name.c_str(),
            (*combo_box_it).control->Text
          )
        );
      if (ComboIndex != -1) (*combo_box_it).control->ItemIndex = ComboIndex;
    }
    for (vector<radio_group_t>::iterator
      radio_group_it = m_radio_groups.begin();
      radio_group_it != m_radio_groups.end(); radio_group_it++) {
      if (radio_group_it->control->Items->Count != 0) {
        int RadioCurPosition = radio_group_it->control->ItemIndex;
        if (RadioCurPosition == -1) RadioCurPosition = 0;
        int RadioIndex =
          FindRadioGroupIndex(
            radio_group_it->control,
            IniFile->ReadString(
              radio_group_it->section.c_str(),
              radio_group_it->name.c_str(),
              radio_group_it->control->Items->Strings[RadioCurPosition]
            )
          );
        if (RadioIndex != -1) radio_group_it->control->ItemIndex = RadioIndex;
      }
    }
    typedef map<TStringGrid*, string_grid_t>::iterator sg_it_t;
    for (
      sg_it_t sg_it = m_string_grids.begin();
      sg_it != m_string_grids.end();
      sg_it++
    ) {
      load_save_grid_size(IniFile.get(), ls_load, sg_it->first);
      const int row_count = sg_it->first->RowCount;
      if (sg_it->second.all_columns) {
        for (int row = 1; row < row_count; row++) {
          load_save_grid_row(IniFile.get(), ls_load, sg_it->first, row);
        }
      } else {
        for (int row = 1; row < row_count; row++) {
          load_save_grid_row(IniFile.get(), ls_load, sg_it->first, row);
        }
      }
    }
  }                        
}

void irs::ini_file_t::save() const
{
  auto_ptr<TIniFile> IniFile(new TIniFile(m_ini_name.c_str()));

  for (vector<bool_t>::const_iterator bool_it = mv_bools.begin();
    bool_it != mv_bools.end(); bool_it++) {
    IniFile->WriteBool(bool_it->section.c_str(),
      bool_it->name.c_str(), *(bool_it->control));
  }
  for (vector<irs_u8_t>::const_iterator irs_u8_it = mv_irs_u8s.begin();
    irs_u8_it != mv_irs_u8s.end(); irs_u8_it++) {
    IniFile->WriteInteger(irs_u8_it->section.c_str(),
      irs_u8_it->name.c_str(), *(irs_u8_it->control));
  }
  for (vector<irs_i8_t>::const_iterator irs_i8_it = mv_irs_i8s.begin();
    irs_i8_it != mv_irs_i8s.end(); irs_i8_it++) {
    IniFile->WriteString(irs_i8_it->section.c_str(),
      irs_i8_it->name.c_str(), *(irs_i8_it->control));
  }
  for (vector<irs_u16_t>::const_iterator irs_u16_it = mv_irs_u16s.begin();
    irs_u16_it != mv_irs_u16s.end(); irs_u16_it++) {
    IniFile->WriteInteger(irs_u16_it->section.c_str(),
      irs_u16_it->name.c_str(), *(irs_u16_it->control));
  }
  for (vector<irs_i16_t>::const_iterator irs_i16_it = mv_irs_i16s.begin();
    irs_i16_it != mv_irs_i16s.end(); irs_i16_it++) {
    IniFile->WriteInteger(irs_i16_it->section.c_str(),
      irs_i16_it->name.c_str(), *(irs_i16_it->control));
  }
  for (vector<irs_u32_t>::const_iterator irs_u32_it = mv_irs_u32s.begin();
    irs_u32_it != mv_irs_u32s.end(); irs_u32_it++) {
    IniFile->WriteInteger(irs_u32_it->section.c_str(),
      irs_u32_it->name.c_str(), *(irs_u32_it->control));
  }
  for (vector<irs_i32_t>::const_iterator irs_i32_it = mv_irs_i32s.begin();
    irs_i32_it != mv_irs_i32s.end(); irs_i32_it++) {
    IniFile->WriteInteger(irs_i32_it->section.c_str(),
      irs_i32_it->name.c_str(), *(irs_i32_it->control));
  }
  for (vector<irs_u64_t>::const_iterator irs_u64_it = mv_irs_u64s.begin();
    irs_u64_it != mv_irs_u64s.end(); irs_u64_it++) {
    irs::string var_str = *(irs_u64_it->control);
    IniFile->WriteString(irs_u64_it->section.c_str(),
      irs_u64_it->name.c_str(), var_str.c_str());
  }
  for (vector<irs_i64_t>::const_iterator irs_i64_it = mv_irs_i64s.begin();
    irs_i64_it != mv_irs_i64s.end(); irs_i64_it++) {
    irs::string var_str = *(irs_i64_it->control);
    IniFile->WriteString(irs_i64_it->section.c_str(),
      irs_i64_it->name.c_str(), var_str.c_str());
  }

  for (vector<float_t>::const_iterator float_it = mv_floats.begin();
    float_it != mv_floats.end(); float_it++) {
    IniFile->WriteFloat((*float_it).section.c_str(),
      (*float_it).name.c_str(), *((*float_it).control));
  }
  for (vector<double_t>::const_iterator double_it = mv_doubles.begin();
    double_it != mv_doubles.end(); double_it++) {
    IniFile->WriteFloat((*double_it).section.c_str(),
      (*double_it).name.c_str(), *((*double_it).control));
  }
  for (vector<long_double_t>::const_iterator long_double_it =
    mv_long_doubles.begin();
    long_double_it != mv_long_doubles.end(); long_double_it++) {
    irs::string var_str = *((*long_double_it).control);
    IniFile->WriteString((*long_double_it).section.c_str(),
      (*long_double_it).name.c_str(), var_str.c_str());
  }
  for (vector<irs_string_t>::const_iterator irs_string_it =
    mv_irs_strings.begin();
    irs_string_it != mv_irs_strings.end(); irs_string_it++) {
    IniFile->WriteString((*irs_string_it).section.c_str(),
      (*irs_string_it).name.c_str(), (*(irs_string_it->control)).c_str());
  }
  for (vector<ansi_string_t>::const_iterator ansi_string_it =
    mv_ansi_strings.begin();
    ansi_string_it != mv_ansi_strings.end(); ansi_string_it++) {
    IniFile->WriteString((*ansi_string_it).section.c_str(),
      (*ansi_string_it).name.c_str(), *((*ansi_string_it).control));
  }
  for (vector<edit_t>::const_iterator edit_it = m_edits.begin();
    edit_it != m_edits.end(); edit_it++) {
    IniFile->WriteString((*edit_it).section.c_str(),
      (*edit_it).name.c_str(), (*edit_it).control->Text);
  }
  for (vector<cspin_edit_t>::const_iterator cspin_edit_it =
    m_cspin_edits.begin();
    cspin_edit_it != m_cspin_edits.end(); cspin_edit_it++) {
    IniFile->WriteString((*cspin_edit_it).section.c_str(),
      (*cspin_edit_it).name.c_str(), (*cspin_edit_it).control->Text);
  }
  for (vector<check_box_t>::const_iterator cb_it = m_check_boxs.begin();
    cb_it != m_check_boxs.end(); cb_it++) {

    IniFile->WriteBool((*cb_it).section.c_str(),
      (*cb_it).name.c_str(), (*cb_it).control->Checked);
  }
  for (vector<radio_button_t>::const_iterator rb_it = m_radio_buttons.begin();
    rb_it != m_radio_buttons.end(); rb_it++) {

    IniFile->WriteBool((*rb_it).section.c_str(),
      (*rb_it).name.c_str(), (*rb_it).control->Checked);
  }
  for (vector<page_control_t>::const_iterator pc_it = m_page_controls.begin();
    pc_it != m_page_controls.end(); pc_it++) {

    IniFile->WriteString((*pc_it).section.c_str(),
      (*pc_it).name.c_str(), (*pc_it).control->ActivePage->Name);
  }
  for (vector<group_box_t>::const_iterator gb_it = m_group_boxs.begin();
    gb_it != m_group_boxs.end(); gb_it++) {

    IniFile->WriteString(
      (*gb_it).section.c_str(),
      (*gb_it).name.c_str(),
      ActiveRadioButton((*gb_it).control)->Name
    );
  }
  for (vector<combo_box_t>::const_iterator combo_box_it = m_combo_boxs.begin();
    combo_box_it != m_combo_boxs.end(); combo_box_it++) {

    IniFile->WriteString((*combo_box_it).section.c_str(),
      (*combo_box_it).name.c_str(), (*combo_box_it).control->Text);
  }
  for (vector<radio_group_t>::const_iterator
    radio_group_it = m_radio_groups.begin();
    radio_group_it != m_radio_groups.end(); radio_group_it++) {

    String RadioGroupPositionName = "";
    int RadioGroupPositionIndex = radio_group_it->control->ItemIndex;
    if (RadioGroupPositionIndex != -1) {
      RadioGroupPositionName =
        radio_group_it->control->Items->Strings[RadioGroupPositionIndex];
    }
    
    IniFile->WriteString(
      radio_group_it->section.c_str(),
      radio_group_it->name.c_str(),
      RadioGroupPositionName
    );
  }
  typedef map<TStringGrid*, string_grid_t>::const_iterator sg_it_t;
  for (
    sg_it_t sg_it = m_string_grids.begin();
    sg_it != m_string_grids.end();
    sg_it++
  ) {
    const int row_count = sg_it->first->RowCount;
    if (sg_it->second.all_columns) {
      for (int row = 1; row < row_count; row++) {
        load_save_grid_row(IniFile.get(), ls_save, sg_it->first, row);
      }
    } else {
      for (int row = 1; row < row_count; row++) {
        load_save_grid_row(IniFile.get(), ls_save, sg_it->first, row);
      }
    }
    load_save_grid_size(IniFile.get(), ls_save, sg_it->first);
  }
}
void irs::ini_file_t::save_grid_row(TStringGrid *a_control,
  int a_row_index) const
{
  auto_ptr<TIniFile> IniFile(new TIniFile(m_ini_name.c_str()));
  load_save_grid_row(IniFile.get(), ls_save, a_control, a_row_index);
}
void irs::ini_file_t::save_grid_size(TStringGrid *a_control) const
{
  auto_ptr<TIniFile> IniFile(new TIniFile(m_ini_name.c_str()));
  load_save_grid_size(IniFile.get(), ls_save, a_control);
}
void irs::ini_file_t::clear_control()
{
  mv_bools.clear();
  mv_irs_u8s.clear();
  mv_irs_i8s.clear();
  mv_irs_u16s.clear();
  mv_irs_i16s.clear();
  mv_irs_u32s.clear();
  mv_irs_i32s.clear();
  mv_irs_u64s.clear();
  mv_irs_i64s.clear();
  mv_floats.clear();
  mv_doubles.clear();
  mv_long_doubles.clear();
  mv_irs_strings.clear();
  mv_ansi_strings.clear();
  m_edits.clear();
  m_cspin_edits.clear();
  m_check_boxs.clear();
  m_radio_buttons.clear();
  m_page_controls.clear();
  m_group_boxs.clear();
  m_combo_boxs.clear();
  m_radio_groups.clear();
  m_string_grids.clear();
}
TTabSheet *irs::ini_file_t::FindTabSheet(TPageControl *a_control,
  const String &a_name)
{
  int TabSheetCount = a_control->PageCount;
  for (int TabSheetIndex = 0; TabSheetIndex < TabSheetCount; TabSheetIndex++) {
    if (a_control->Pages[TabSheetIndex]->Name == a_name)
      return a_control->Pages[TabSheetIndex];
  }
  return IRS_NULL;
}
TRadioButton *irs::ini_file_t::FindRadioButton(TGroupBox *a_control,
  const String &a_name)
{
  int RadioButtonCount = a_control->ControlCount;
  for (int RadioButtonIndex = 0; RadioButtonIndex < RadioButtonCount;
    RadioButtonIndex++) {
    if (a_control->Controls[RadioButtonIndex]->Name == a_name)
      return (TRadioButton *)a_control->Controls[RadioButtonIndex];
  }
  return IRS_NULL;
}
TRadioButton *irs::ini_file_t::ActiveRadioButton(TGroupBox *a_control) const
{
  const int RadioButtonCount = a_control->ControlCount;
  for (int RadioButtonIndex = 0; RadioButtonIndex < RadioButtonCount;
    RadioButtonIndex++) {
    String ClassName = a_control->Controls[RadioButtonIndex]->ClassName();
    if (ClassName == "TRadioButton") {
      TControl *Control = a_control->Controls[RadioButtonIndex];
      TRadioButton *RadioButton = (TRadioButton *)Control;
      if (RadioButton->Checked) return RadioButton;
    }
  }
  return IRS_NULL;
}
int irs::ini_file_t::FindComboBoxItem(TComboBox *a_control,
  const String &a_name)
{
  const int ComboBoxCount = a_control->Items->Count;
  for (int ComboBoxIndex = 0; ComboBoxIndex < ComboBoxCount; ComboBoxIndex++) {
    if (a_control->Items->Strings[ComboBoxIndex] == a_name) {
      return ComboBoxIndex;
    }
  }
  return -1;
}
int irs::ini_file_t::FindRadioGroupIndex(TRadioGroup *a_control,
  const String &a_name)
{
  int Count = a_control->Items->Count;
  for (int RadioGroupIndex = 0; RadioGroupIndex < Count; RadioGroupIndex++) {
    if (a_control->Items->Strings[RadioGroupIndex] == a_name) {
      return RadioGroupIndex;
    }
  }
  return -1;
}
void irs::ini_file_t::load_save_grid_row(TIniFile *ap_ini_file,
  load_save_t a_load_save, TStringGrid *a_control, int a_row_index) const
{
  map<TStringGrid*, string_grid_t>::const_iterator it_sg =
    m_string_grids.find(a_control);
  const int header_size = 1;
  const int header_col_size = 1;
  if (it_sg->second.all_columns) {
    const int col_count = a_control->ColCount;
    for (int col = header_col_size; col < col_count; col++) {
      irs::string name = it_sg->second.name + "_row" +
        irs::string(a_row_index) + "_col" + irs::string(col);
      load_save_grid_cell(ap_ini_file, a_load_save, a_control, a_row_index,
        col, name);
    }
  } else {
    typedef vector<string_grid_t::column_t>::const_iterator col_it_t;
    for (
      col_it_t col_it = it_sg->second.columns.begin();
      col_it != it_sg->second.columns.end();
      col_it++
    ) {
      int col = col_it->index;
      int var_index = a_row_index - header_size;
      irs::string name = it_sg->second.name + col_it->name +
        irs::string(var_index);
      load_save_grid_cell(ap_ini_file, a_load_save, a_control, a_row_index,
        col, name);
    }
  }
}
void irs::ini_file_t::load_save_grid_cell(TIniFile *ap_ini_file,
  load_save_t a_load_save, TStringGrid *a_control, int a_row_index,
  int a_col_index, const irs::string& a_name) const
{
  map<TStringGrid*, string_grid_t>::const_iterator it_sg =
    m_string_grids.find(a_control);
  String name_bstr = a_name.c_str();
  String section_bstr = it_sg->second.section.c_str();
  String value_bstr = a_control->Cells[a_col_index][a_row_index];
  switch (a_load_save) {
    case ls_load: {
      a_control->Cells[a_col_index][a_row_index] =
        ap_ini_file->ReadString(section_bstr, name_bstr, value_bstr);
    } break;
    case ls_save: {
      ap_ini_file->WriteString(section_bstr, name_bstr, value_bstr);
    }
  }
}
void irs::ini_file_t::load_save_grid_size(TIniFile *ap_ini_file,
  load_save_t a_load_save, TStringGrid *a_control) const
{
  map<TStringGrid*, string_grid_t>::const_iterator it_sg =
    m_string_grids.find(a_control);
  //string_grid_t& sg = m_string_grids[a_control];
  String section_bstr = it_sg->second.section.c_str();
  String name_bstr = it_sg->second.name.c_str();
  String name_row_count = name_bstr + "_row_count";
  String name_col_count = name_bstr + "_col_count";
  switch (a_load_save) {
    case ls_load: {
      a_control->RowCount = ap_ini_file->ReadInteger(section_bstr,
        name_row_count, a_control->RowCount);
      a_control->ColCount = ap_ini_file->ReadInteger(section_bstr,
        name_col_count, a_control->ColCount);
    } break;
    case ls_save: {
      ap_ini_file->WriteInteger(section_bstr, name_row_count,
        a_control->RowCount);
      ap_ini_file->WriteInteger(section_bstr, name_col_count,
        a_control->ColCount);
    }
  }
}

#pragma package(smart_init)



