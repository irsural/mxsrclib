//! \file
//! \ingroup graphical_user_interface_group
//! \brief ������� ����������
//!
//! ����: 21.05.2011\n
//! ���� ��������: 29.07.2010

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irsparam.h>
#include <irssysutils.h>

#include <irsfinal.h>

irs::param_box_t::param_box_t(
  const string_type& a_param_box_name,
  const string_type& a_ini_section,
  const string_type& a_prefix_name,
  const string_type& a_ini_name
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  ,
  TEncoding* ap_encoding
  #endif // (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
):
  m_ini_section(a_ini_section),
  m_prefix_name(a_prefix_name),
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  mp_encoding(ap_encoding),
  #endif // (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  m_ini_file(),
  mp_form(new TForm(IRS_NULL, 1)),
  mp_panel(new TPanel(mp_form.get())),
  mp_ok_btn(new TButton(mp_form.get())),
  mp_cansel_btn(new TButton(mp_form.get())),
  mp_value_list_editor(new TValueListEditor(mp_form.get())),
  m_cur_param_row(0),
  m_param_def_list()
{
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  m_ini_file.set_encoding(mp_encoding);
  #endif // (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))

  mp_form->Width = 280;
  mp_form->Height = 400;
  mp_form->Constraints->MinWidth = 280;
  mp_form->Constraints->MinHeight = 400;
  mp_form->BorderStyle = bsSizeable;
  mp_form->Position = poOwnerFormCenter;
  mp_form->Caption = a_param_box_name.c_str();
  mp_form->OnClose = on_close_event;

  mp_panel->Align = alBottom;
  mp_panel->Parent = mp_form.get();
  mp_panel->Top = 0;

  mp_ok_btn->Parent = mp_panel;
  mp_ok_btn->Anchors >> akLeft;
  mp_ok_btn->Anchors << akRight;
  mp_ok_btn->Left = 0;
  mp_ok_btn->Top = mp_panel->Height/2 - mp_ok_btn->Height/2;
  mp_ok_btn->Caption = irst("OK");
  mp_ok_btn->Default = true;
  mp_ok_btn->ModalResult = mrOk;
  mp_ok_btn->OnClick = ok_btn_click;

  mp_cansel_btn->Parent = mp_panel;
  mp_cansel_btn->Anchors >> akLeft;
  mp_cansel_btn->Anchors << akRight;
  mp_cansel_btn->Left = mp_ok_btn->Left + mp_ok_btn->Width + btn_gap;
  mp_cansel_btn->Top = mp_panel->Height/2 - mp_cansel_btn->Height/2;
  mp_cansel_btn->Caption = irst("������");
  mp_cansel_btn->Cancel = true;
  mp_cansel_btn->ModalResult = mrCancel;
  mp_cansel_btn->OnClick = cancel_btn_click;
      
  mp_value_list_editor->Parent = mp_form.get();
  mp_value_list_editor->Align = alClient;
  mp_value_list_editor->DefaultRowHeight = 17;
  mp_value_list_editor->TitleCaptions->Strings[header_col] = irst("��������");
  mp_value_list_editor->TitleCaptions->Strings[option_col] = irst("��������");
  mp_value_list_editor->OnEditButtonClick = on_edit_btn_click;

  if (a_ini_name == irst("")) {
    m_ini_file.set_ini_name(m_ini_file.ini_name().c_str());
  } else {
    m_ini_file.set_ini_name(a_ini_name.c_str());
  }
  if (m_ini_section == irst("")) {
    m_ini_file.set_section(def_ini_section());
  } else {
    m_ini_file.set_section(m_ini_section.c_str());
  }
  m_ini_file.add(m_prefix_name.c_str(), mp_value_list_editor);
  m_ini_file.load();
  load_form_params();
}

void irs::param_box_t::save_form_params()
{
  TCustomIniFile* IniFile = create_ini_file(
    irs::str_conv<string_type>(m_ini_file.ini_name()));
  int key_col_width = mp_value_list_editor->ColWidths[0];
  IniFile->WriteInteger(
    m_ini_section.c_str(),
    (m_prefix_name + irst("key_col_width")).c_str(),
    key_col_width
  );
  int form_width = mp_form->Width;
  IniFile->WriteInteger(
    m_ini_section.c_str(),
    (m_prefix_name + irst("form_width")).c_str(),
    form_width
  );
  int form_height = mp_form->Height;
  IniFile->WriteInteger(
    m_ini_section.c_str(),
    (m_prefix_name + irst("form_height")).c_str(),
    form_height
  );
  IniFile->UpdateFile();
}

void irs::param_box_t::load_form_params()
{
  TCustomIniFile* IniFile = create_ini_file(
    irs::str_conv<string_type>(m_ini_file.ini_name()));
  mp_value_list_editor->ColWidths[0] = IniFile->ReadInteger(
    m_ini_section.c_str(),
    (m_prefix_name + irst("key_col_width")).c_str(),
    mp_value_list_editor->ColWidths[0]
  );
  mp_form->Width = IniFile->ReadInteger(
    m_ini_section.c_str(),
    (m_prefix_name + irst("form_width")).c_str(),
    mp_form->Width
  );
  mp_form->Height = IniFile->ReadInteger(
    m_ini_section.c_str(),
    (m_prefix_name + irst("form_height")).c_str(),
    mp_form->Height
  );
}

TCustomIniFile* irs::param_box_t::create_ini_file(
  const string_type& a_file_name) const
{
  #if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  return new TMemIniFile(a_file_name.c_str(), mp_encoding);
  #else  // !(defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
  return new TMemIniFile(a_file_name.c_str());
  #endif // !(defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER2010))
}

void __fastcall irs::param_box_t::ok_btn_click(TObject *Sender)
{
  save_form_params();
  m_ini_file.save();
}

void __fastcall irs::param_box_t::cancel_btn_click(TObject *Sender)
{
  save_form_params();
  m_ini_file.load();
}

void __fastcall irs::param_box_t::on_close_event(TObject *Sender,
  TCloseAction &Action)
{
  save_form_params();
  m_ini_file.load();
  Action = caHide;
}

void __fastcall irs::param_box_t::on_edit_btn_click(TObject *Sender)
{
  String key = mp_value_list_editor->Keys[mp_value_list_editor->Row];
  map<string_type, bool>::iterator it =
    m_button_list.find(str_conv<string_type>(key));
  if (it != m_button_list.end()) {
    m_button_list[str_conv<string_type>(key)] = true;
  } else {
    std::map<string_type, param_box_base_t*>::iterator it =
      m_param_box_base_map.find(str_conv<string_type>(key));
    IRS_LIB_ASSERT(it != m_param_box_base_map.end());
    it->second->show();
  }
}

irs::param_box_t::~param_box_t()
{
}

const irs::param_box_t::char_type* irs::param_box_t::def_param_box_name()
{
  return irst("���������");
}

const irs::param_box_t::char_type* irs::param_box_t::def_ini_section()
{
  return irst("Options");
}

const irs::param_box_t::char_type* irs::param_box_t::empty_string()
{
  return irst("");
}

void irs::param_box_t::set_ini_name(const string_type& a_ini_name)
{
  m_ini_file.set_ini_name(a_ini_name.c_str());
}

irs::param_box_t::string_type irs::param_box_t::ini_name() const
{
  return m_ini_file.ini_name().c_str();
}

void irs::param_box_t::set_section(const string_type& a_section)
{
  m_ini_section = a_section;
  m_ini_file.clear_control();
  m_ini_file.set_section(m_ini_section.c_str());
  m_ini_file.add(m_prefix_name.c_str(), mp_value_list_editor);
}

irs::param_box_t::string_type irs::param_box_t::section() const
{
  return m_ini_section;
}

bool irs::param_box_t::show()
{
  return mp_form->ShowModal() == mrOk;
}

void irs::param_box_t::hide()
{
  mp_form->Hide();
}

void irs::param_box_t::add_edit(const string_type& a_param_name,
  const string_type& a_param_value)
{
  String Key = a_param_name.c_str();
  String Value = a_param_value.c_str();
  int row_index = 0;
  if (!mp_value_list_editor->FindRow(Key, row_index)) {
    mp_value_list_editor->InsertRow(Key, Value, true);
    m_param_def_list[a_param_name] = a_param_value;
  }
}

bool irs::param_box_t::add_combo_by_item(
  const string_type& a_param_name,
  const string_type& a_param_value)
{
  String Key = a_param_name.c_str();
  String Value = a_param_value.c_str();
  int row_index = 0;
  if (mp_value_list_editor->FindRow(Key, row_index)) {
    mp_value_list_editor->ItemProps[Key]->EditStyle = esPickList;
    if (mp_value_list_editor->ItemProps[Key]->
      PickList->IndexOf(Value) == -1)
    {
      mp_value_list_editor->ItemProps[Key]->PickList->Add(Value);
      return true;
    }
  }
  return false;
}

void irs::param_box_t::add_combo(const string_type& a_param_name,
  const vector<string_type>* ap_param_values_list)
{
  String Key = a_param_name.c_str();
  int row_index = 0;
  if (mp_value_list_editor->FindRow(Key, row_index)) {
    mp_value_list_editor->ItemProps[Key]->
      EditStyle = esPickList;
    for(size_t list_index = 0; list_index < ap_param_values_list->size();
      list_index++)
    {
      if (mp_value_list_editor->ItemProps[Key]->PickList->
        IndexOf(((*ap_param_values_list)[list_index]).c_str()) == -1)
      {
        mp_value_list_editor->ItemProps[Key]->
          PickList->Add(((*ap_param_values_list)[list_index]).c_str());
      }
    }
  }
}

void irs::param_box_t::clear_combo(const string_type& a_param_name)
{
  String Key = a_param_name.c_str();
  int row_index = 0;
  if (mp_value_list_editor->FindRow(Key, row_index)) {
    if (mp_value_list_editor->ItemProps[Key]->EditStyle == esPickList) {
      mp_value_list_editor->ItemProps[Key]->PickList->Clear();
    }
  }
}

void irs::param_box_t::add_bool(const string_type& a_param_name,
  bool a_param_value)
{
  String Key = a_param_name.c_str();
  int row_index = 0;
  if (!mp_value_list_editor->FindRow(Key, row_index)) {
    mp_value_list_editor->InsertRow(Key, irst(""), true);
    mp_value_list_editor->ItemProps[Key]->
      EditStyle = esPickList;
    mp_value_list_editor->ItemProps[Key]->
        PickList->Add(irst("false"));
    mp_value_list_editor->ItemProps[Key]->
        PickList->Add(irst("true"));
    if (a_param_value) {
      mp_value_list_editor->Values[Key] = irst("true");
    } else {
      mp_value_list_editor->Values[Key] = irst("false");
    }
    m_param_def_list[a_param_name] = a_param_value;
  }
}

void irs::param_box_t::add_param_box(const string_type& a_param_name,
  param_box_base_t* ap_param_box_base)
{
  String Key = str_conv<String>(a_param_name);
  int row_index = 0;
  if (!mp_value_list_editor->FindRow(Key, row_index)) {
    m_param_box_base_map.insert(std::make_pair(a_param_name,
      ap_param_box_base));
    mp_value_list_editor->InsertRow(Key, irst(""), true);
    mp_value_list_editor->ItemProps[Key]->EditStyle = esEllipsis;
    mp_value_list_editor->ItemProps[Key]->ReadOnly = true;
  }
}

void irs::param_box_t::add_button(const string_type& a_param_name)
{
  //m_button_list
  String Key = str_conv<String>(a_param_name);
  int row_index = 0;
  if (!mp_value_list_editor->FindRow(Key, row_index)) {
    m_button_list.insert(std::make_pair(a_param_name, false));
    mp_value_list_editor->InsertRow(Key, irst(""), true);
    mp_value_list_editor->ItemProps[Key]->EditStyle = esEllipsis;
    mp_value_list_editor->ItemProps[Key]->ReadOnly = true;
  }
}

bool irs::param_box_t::get_param(const string_type& a_param_name,
  string_type* ap_param_value) const
{
  String Key = a_param_name.c_str();
  int row_index = 0;
  if (mp_value_list_editor->FindRow(Key, row_index)) {
    String Value = mp_value_list_editor->Values[Key];
    *ap_param_value = Value.c_str();
    return true;
  } else {
    return false;
  }
}

irs::param_box_t::string_type irs::param_box_t::get_param(
  const string_type& a_param_name) const
{
  string_type param_value = irst("");
  get_param(a_param_name, &param_value);
  return param_value;
}

irs::param_box_t::string_type irs::param_box_t::get_param_def(
  const string_type& a_param_name) const
{
  string_type param_value_def = irst("");
  map<string_type, string_type>::const_iterator param_def_it =
    m_param_def_list.find(a_param_name);
  if (param_def_it != m_param_def_list.end()) {
    param_value_def = param_def_it->second;
  }
  return param_value_def;
}

bool irs::param_box_t::is_button_pressed(const string_type& a_param_name)
{
  bool result = m_button_list[a_param_name];
  m_button_list[a_param_name] = false;
  return result;
}

bool irs::param_box_t::set_param(const string_type& a_param_name,
  const string_type& a_param_value)
{
  String Key = a_param_name.c_str();
  String Value = a_param_value.c_str();
  int row_index = 0;
  if (mp_value_list_editor->FindRow(Key, row_index)) {
    mp_value_list_editor->Values[Key] = Value;
    return true;
  } else {
    return false;
  }
}

void irs::param_box_t::save() const
{
  m_ini_file.save();
}

void irs::param_box_t::load()
{
  m_ini_file.load();
}

void irs::param_box_t::delete_edit(const string_type& a_param_name)
{
  String Key = a_param_name.c_str();
  int row_index = 0;
  if (mp_value_list_editor->FindRow(Key, row_index)) {
    mp_value_list_editor->DeleteRow(row_index);
    m_param_box_base_map.erase(a_param_name);
    m_button_list.erase(a_param_name);
  }
}

