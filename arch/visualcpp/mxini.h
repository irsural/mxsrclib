// Работа с ini-файлами
// Дата: 29.02.2007

#ifndef mxiniH
#define mxiniH

#include <irscpp.h>
#include <inifiles.hpp>
#include <stdctrls.hpp>
#include <comctrls.hpp>
#include <vcl/dstring.h>

namespace irs {

// Работа с ini-файлами
class ini_file_t
{
public:
  ini_file_t();
  ~ini_file_t();
  void set_section(String a_name);
  void add(String a_name, TEdit *a_control);
  void add(String a_name, TCheckBox *a_control);
  void add(String a_name, TPageControl *a_control);
  void add(String a_name, TGroupBox *a_control);
  void add(String a_name, TComboBox *a_control);
  void load();
  void save();

  struct bad_exe_path {};
private:
  struct control_t {
    String section;
    String name;

    control_t(String a_section, String a_name):
      section(a_section),
      name(a_name)
    {
    }
  };
  struct edit_t: control_t {
    TEdit *control;

    edit_t(String a_section, String a_name, TEdit *a_control):
      control_t(a_section, a_name),
      control(a_control)
    {
    }
  };
  struct check_box_t: control_t {
    TCheckBox *control;

    check_box_t(String a_section, String a_name, TCheckBox *a_control):
      control_t(a_section, a_name),
      control(a_control)
    {
    }
  };
  struct page_control_t: control_t {
    TPageControl *control;

    page_control_t(String a_section, String a_name, TPageControl *a_control):
      control_t(a_section, a_name),
      control(a_control)
    {
    }
  };
  struct group_box_t: control_t {
    TGroupBox *control;

    group_box_t(String a_section, String a_name, TGroupBox *a_control):
      control_t(a_section, a_name),
      control(a_control)
    {
    }
  };
  struct combo_box_t: control_t {
    TComboBox *control;

    combo_box_t(String a_section, String a_name, TComboBox *a_control):
      control_t(a_section, a_name),
      control(a_control)
    {
    }
  };

  vector<edit_t> m_edits;
  vector<check_box_t> m_check_boxs;
  vector<page_control_t> m_page_controls;
  vector<group_box_t> m_group_boxs;
  vector<combo_box_t> m_combo_boxs;
  String m_ini_name;
  String m_section;

  TTabSheet *FindTabSheet(TPageControl *a_control, const String &a_name);
  TRadioButton *FindRadioButton(TGroupBox *a_control, const String &a_name);
  TRadioButton *ActiveRadioButton(TGroupBox *a_control);
  int FindComboBoxItem(TComboBox *a_control, const String &a_name);
}; //class ini_file_t

} //namespace irs

#endif //mxiniH
