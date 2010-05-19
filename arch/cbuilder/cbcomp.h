// Компоненты
// С++ Builder
// Дата: 14.04.2010
// Дата создания: 10.12.2008

#ifndef cbcompH
#define cbcompH

//#include <vcl.h>
#include <irsdefs.h>

#include <windows.h>
#include <StdCtrls.hpp>

#include <irsstd.h>
#include <mxdata.h>

#include <irsfinal.h>

namespace irs {

namespace cbuilder {

const int npos = -1;

// Функции для компонента ComboBox
int combo_box_find_item(TComboBox *a_control, const String &a_name);

// Конвертирование событий C++ Builder в события ИРС
class notify_event_cnv_t
{
public:
  notify_event_cnv_t();
  notify_event_cnv_t(const notify_event_cnv_t& a_notify_event_cnv);
  TNotifyEvent cb_event();
  bool check();
  void add_event(mxfact_event_t *ap_event);
private:
  struct obj_cont_t: public TObject
  {
    mxfact_event_t *mp_event;
    bool m_event_signaled;

    obj_cont_t();
    obj_cont_t(const obj_cont_t& a_obj_cont);
    void __fastcall cb_event(TObject *Sender);
  };

  auto_ptr<obj_cont_t> mp_obj_cont;
};

// Предикат Equal для класса change_obj_t
// Сравнивает TEdit*
struct change_obj_edit_pred_t {
  static inline bool equal(TEdit& edit, irs::string& prev)
  {
    return edit.Text == prev.c_str();
  }
  static inline irs::string prev(TEdit& edit)
  {
    return edit.Text.c_str();
  }
};
// Тип для проверки изменения TEdit
typedef irs::change_obj_t<TEdit, irs::string, change_obj_edit_pred_t>
  change_edit_t;

// Предикат Equal для класса change_obj_t
// Сравнивает TComboBox*
struct change_obj_combo_box_pred_t {
  static inline bool equal(TComboBox& edit, irs::string& prev)
  {
    return edit.Text == prev.c_str();
  }
  static inline irs::string prev(TComboBox& edit)
  {
    return edit.Text.c_str();
  }
};
// Тип для проверки изменения TEdit
typedef irs::change_obj_t<TComboBox, irs::string, change_obj_combo_box_pred_t>
  change_combo_box_t;

// Предикат Equal для класса change_obj_t
// Сравнивает TCheckBox*
struct change_obj_check_box_pred_t {
  static inline bool equal(TCheckBox& a_check_box, bool& prev)
  {
    return a_check_box.Checked == prev;
  }
  static inline bool prev(TCheckBox& a_check_box)
  {
    return a_check_box.Checked;
  }
};
// Тип для проверки изменения TCheckBox
typedef irs::change_obj_t<TCheckBox, bool, change_obj_check_box_pred_t>
  change_check_box_t;

}; //namespace cbuilder

}; //namespace irs

#endif //cbcompH

