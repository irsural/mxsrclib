// Работа с ini-файлами
// Дата: 29.02.2007

#include <vcl.h>
#pragma hdrstop

#include "mxini.h"
//---------------------------------------------------------------------------
// Работа с ini-файлами

irs::ini_file_t::ini_file_t():
  m_edits(),
  m_check_boxs(),
  m_page_controls(),
  m_group_boxs(),
  m_combo_boxs(),
  m_ini_name(),
  m_section("Options")
{
  String ExeName = Application->ExeName;
  int Length = ExeName.Length();
  if (Length < 3) throw bad_exe_path();
  m_ini_name = ExeName.SubString(1, Length - 3);
  m_ini_name += "ini";
}
irs::ini_file_t::~ini_file_t()
{
  //save();
}
void irs::ini_file_t::set_section(String a_name)
{
  m_section = a_name;
}
void irs::ini_file_t::add(String a_name, TEdit *a_control)
{
  m_edits.push_back(edit_t(m_section, a_name, a_control));
}
void irs::ini_file_t::add(String a_name, TCheckBox *a_control)
{
  m_check_boxs.push_back(check_box_t(m_section, a_name, a_control));
}
void irs::ini_file_t::add(String a_name, TPageControl *a_control)
{
  m_page_controls.push_back(page_control_t(m_section, a_name, a_control));
}
void irs::ini_file_t::add(String a_name, TGroupBox *a_control)
{
  m_group_boxs.push_back(group_box_t(m_section, a_name, a_control));
}
void irs::ini_file_t::add(String a_name, TComboBox *a_control)
{
  m_combo_boxs.push_back(combo_box_t(m_section, a_name, a_control));
}
void irs::ini_file_t::load()
{
  if (FileExists(m_ini_name)) {
    auto_ptr<TIniFile> IniFile(new TIniFile(m_ini_name));
    for (vector<edit_t>::iterator edit_it = m_edits.begin();
      edit_it != m_edits.end(); edit_it++) {
      (*edit_it).control->Text = IniFile->ReadString((*edit_it).section,
        (*edit_it).name, (*edit_it).control->Text);
    }
    for (vector<check_box_t>::iterator cb_it = m_check_boxs.begin();
      cb_it != m_check_boxs.end(); cb_it++) {
      (*cb_it).control->Checked = IniFile->ReadBool((*cb_it).section,
        (*cb_it).name, (*cb_it).control->Checked);
    }
    for (vector<page_control_t>::iterator pc_it = m_page_controls.begin();
      pc_it != m_page_controls.end(); pc_it++) {
      TTabSheet *ts =
        FindTabSheet(
          (*pc_it).control,
          IniFile->ReadString(
            (*pc_it).section,
            (*pc_it).name,
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
            (*gb_it).section,
            (*gb_it).name,
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
            (*combo_box_it).section,
            (*combo_box_it).name,
            (*combo_box_it).control->Text
          )
        );
      if (ComboIndex != -1) (*combo_box_it).control->ItemIndex = ComboIndex;
    }
  }
}
void irs::ini_file_t::save()
{
  auto_ptr<TIniFile> IniFile(new TIniFile(m_ini_name));
  for (vector<edit_t>::iterator edit_it = m_edits.begin();
    edit_it != m_edits.end(); edit_it++) {
    IniFile->WriteString((*edit_it).section,
      (*edit_it).name, (*edit_it).control->Text);
  }
  for (vector<check_box_t>::iterator cb_it = m_check_boxs.begin();
    cb_it != m_check_boxs.end(); cb_it++) {
    IniFile->WriteBool((*cb_it).section,
      (*cb_it).name, (*cb_it).control->Checked);
  }
  for (vector<page_control_t>::iterator pc_it = m_page_controls.begin();
    pc_it != m_page_controls.end(); pc_it++) {
    IniFile->WriteString((*pc_it).section,
      (*pc_it).name, (*pc_it).control->ActivePage->Name);
  }
  for (vector<group_box_t>::iterator gb_it = m_group_boxs.begin();
    gb_it != m_group_boxs.end(); gb_it++) {
    IniFile->WriteString(
      (*gb_it).section,
      (*gb_it).name,
      ActiveRadioButton((*gb_it).control)->Name
    );
  }
  for (vector<combo_box_t>::iterator combo_box_it = m_combo_boxs.begin();
    combo_box_it != m_combo_boxs.end(); combo_box_it++) {
    IniFile->WriteString((*combo_box_it).section,
      (*combo_box_it).name, (*combo_box_it).control->Text);
  }
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
TRadioButton *irs::ini_file_t::ActiveRadioButton(TGroupBox *a_control)
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
int irs::ini_file_t::FindComboBoxItem(TComboBox *a_control, const String &a_name)
{
  const int ComboBoxCount = a_control->Items->Count;
  for (int ComboBoxIndex = 0; ComboBoxIndex < ComboBoxCount; ComboBoxIndex++) {
    if (a_control->Items->Strings[ComboBoxIndex] == a_name) {
      return ComboBoxIndex;
    }
  }
  return -1;
}

#pragma package(smart_init)
