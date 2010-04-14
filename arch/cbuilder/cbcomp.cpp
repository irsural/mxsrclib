// Компоненты
// С++ Builder
// Дата: 14.04.2010
// Дата создания: 14.11.2008

#include <irsdefs.h>

#include <vcl.h>
#pragma hdrstop

#include <cbcomp.h>

#include <irsfinal.h>

int irs::cbuilder::combo_box_find_item(
  TComboBox *a_control, const String &a_name)
{
  const int ComboBoxCount = a_control->Items->Count;
  for (int ComboBoxIndex = 0; ComboBoxIndex < ComboBoxCount; ComboBoxIndex++) {
    if (a_control->Items->Strings[ComboBoxIndex] == a_name) {
      return ComboBoxIndex;
    }
  }
  return irs::cbuilder::npos;
}

// Конвертирование событий C++ Builder в события ИРС
irs::cbuilder::notify_event_cnv_t::
  notify_event_cnv_t():
  mp_obj_cont(new obj_cont_t())
{
}
irs::cbuilder::notify_event_cnv_t::
  notify_event_cnv_t(const notify_event_cnv_t& a_notify_event_cnv):
  mp_obj_cont(new obj_cont_t(*a_notify_event_cnv.mp_obj_cont))
{
}
TNotifyEvent irs::cbuilder::notify_event_cnv_t::
  cb_event()
{
  return &mp_obj_cont->cb_event;
}
bool irs::cbuilder::notify_event_cnv_t::
  check()
{
  return reset_val(&mp_obj_cont->m_event_signaled);
}
void irs::cbuilder::notify_event_cnv_t::
  add_event(mxfact_event_t *ap_event)
{
  ap_event->connect(mp_obj_cont->mp_event);
}
irs::cbuilder::notify_event_cnv_t::
  obj_cont_t::obj_cont_t():
  mp_event(IRS_NULL),
  m_event_signaled(false)
{
}
irs::cbuilder::notify_event_cnv_t::
  obj_cont_t::obj_cont_t(const obj_cont_t& a_obj_cont):
  mp_event(a_obj_cont.mp_event),
  m_event_signaled(a_obj_cont.m_event_signaled)
{
}
void __fastcall irs::cbuilder::notify_event_cnv_t::
  obj_cont_t::cb_event(TObject *Sender)
{
  if (mp_event) mp_event->exec();
  m_event_signaled = true;
}

