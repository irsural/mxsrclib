//! \file
//! \ingroup text_user_interface_group
//! \brief Текстовое меню
//!
//! Дата: 04.04.2011n
//! Ранняя дата: 15.09.2009

#ifndef IRSMENUH
#define IRSMENUH

#include <irsdefs.h>

#include <timer.h>
//#include <irslcd.h>
#include <irsstd.h>
#include <irserror.h>
#include <irsdbgutil.h>

#include <irsfinal.h>

//! \addtogroup text_user_interface_group
//! @{

#define CUR_BLINK 500//ms
#define EDIT_LINE 2

// -------------------- КЛАСС БЕГУЩЩЕЙ СТРОКИ ДЛЯ МЕНЮ ------------------------

class irs_menu_creep_t
{
  typedef size_t size_type;
  char *f_buffer;
  //char f_line[LINE_LEN+1];
  char *f_line;
  size_type f_line_len;
  size_type f_buffer_len;
  size_type f_static_len;
  size_type f_message_len;
  size_type f_dynamic_len;
  counter_t f_creeptimer;
  counter_t f_creeptime;
  irs_bool end_of_line;
  size_type f_position;
public:
  irs_menu_creep_t(char *a_buffer, size_type a_buffer_len, size_type a_line_len,
    size_type a_static_len, size_type a_message_len, irs_u8 time_num,
    irs_u8 time_denom);
  ~irs_menu_creep_t();
  void reset();
  bool shift();
  void clear_dynamic();
  void soft_clear_dynamic();
  void change_static(char *a_static);
  void change_message(char *a_message);
  char *get_creep_buffer();
  char *get_line();
  irs_bool eol();
};

//------------------------------------------------------------------------------

typedef enum _irs_menu_state_t{
  ims_hide,
  ims_edit,
  ims_show
} irs_menu_state_t;

enum irs_menu_param_show_mode_t
{
  IMM_FULL,
  IMM_WITHOUT_PREFIX,
  IMM_WITHOUT_SUFFIX
};

enum irs_menu_param_update_t
{
  IMU_UPDATE,
  IMU_NO_UPDATE
};

//! \brief тип
enum irs_menu_key_type_t
{
  IMK_DIGITS,
  IMK_ARROWS
};

typedef void (*double_trans_t)(double *a_parametr);
typedef void (*ip_trans_t)(irs_u8 *a_parametr);
typedef void (*bool_trans_t)(irs_bool *a_parametr);

class irs_menu_creep_t;

class irs_menu_base_t
{
public:
  typedef size_t size_type;
protected:
  static const int empty_str_size = 1;
  static char empty_str[empty_str_size];
  static const int zero_str_size = 2;
  static char zero_str[zero_str_size];

  char *f_header;
  char *f_message;
  irs_menu_creep_t *f_creep;
  char *f_creep_buffer;
  char f_cursor[2];
  char f_cursor_symbol;
  counter_t f_blink_counter;
  size_type f_cur_symbol;
  irs_bool f_can_edit;
  irs_menu_state_t f_state;
  irs_menu_base_t *f_master_menu;
  irs_bool f_want_redraw;
  size_type f_message_count;
  //char* f_message_array[MAX_MESSAGE_COUNT];
  //irs_u8 f_message_len_array[MAX_MESSAGE_COUNT];
  mxkey_event_t *f_key_event;
  irs_bool f_show_needed;
  mxdisplay_drv_service_t *mp_disp_drv;
  mxfact_event_t *mp_event;
  bool m_updated;
  void* mp_data_attached;
public:
  irs_menu_base_t();
  ~irs_menu_base_t();
  void set_key_event(mxkey_event_t *key_event);
  void add_change_event(mxfact_event_t *ap_event);
  virtual void set_disp_drv(mxdisplay_drv_service_t *ap_disp_drv);
  void set_cursor_symbol(char a_cursor_symbol);
  //! \brief Задает родительское меню, переход к которому осуществляется по
  //!   нажатию клавиши irskey_escape
  void set_master_menu(irs_menu_base_t *a_master_menu);
  void set_header(char *a_header);
  char *get_header();
  //! \brief Часть бегущей строки
  void set_message(char *a_message);
  char *get_message();
  void set_creep(irs_menu_creep_t *a_creep);
  irs_menu_state_t get_state();
  void show();
  void hide();
  void edit();
  irs_bool can_edit();
  void can_edit(irs_bool a_can_edit);
  void* get_data_attached();
  void set_data_attached(void* ap_data);

  virtual size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE) = 0;
  virtual size_type get_dynamic_string(char *a_buffer,
    size_type a_length = 0) = 0;
  virtual bool is_updated();
  //! \brief Функция для обработки событий в режиме опроса. Аналог метода
  //!   tick, имеющегося во многих классах библиотеки mxsrclib
  virtual void draw(irs_menu_base_t **a_cur_menu) = 0;
};

class irs_menu_t: public irs_menu_base_t
{
  //irs_menu_base_t *f_menu_array[MAX_MENU_ITEMS];
  irs_menu_base_t **f_menu_array;
  size_type f_arrow_position;
  size_type f_items_count;
  size_type f_current_item;
public:
  irs_menu_t();
  ~irs_menu_t();
  size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE);
  void draw(irs_menu_base_t **a_cur_menu);
  size_type get_current_item();
  size_type get_items_count();
  irs_bool add(irs_menu_base_t *item);
  size_type get_dynamic_string(char *a_buffer, size_type a_length = 0);
};

class irs_advanced_menu_t: public irs_menu_base_t
{
  struct menu_item_t
  {
    irs_menu_base_t* p_item;
    bool show;
  };

  vector<menu_item_t> m_menu_vector;
  size_type m_arrow_position;
  size_type m_current_item;
  bool find_prev_shown_item(size_type &a_item);
  bool find_next_shown_item(size_type &a_item);
public:
  irs_advanced_menu_t();
  ~irs_advanced_menu_t();
  size_type get_parametr_string(
    char *ap_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE);
  void draw(irs_menu_base_t **a_cur_menu);
  size_type get_current_item();
  size_type get_items_count();
  size_type add(irs_menu_base_t *item, bool a_show = true);
  size_type get_dynamic_string(char *ap_buffer, size_type a_length = 0);
  size_type get_last_item_number();
  void hide_item(size_type a_item_number);
  void show_item(size_type a_item_number);
  bool item_is_hidden(size_type a_item_number);
  bool find_item(irs_menu_base_t *ap_item, size_t* ap_finded_item_idx);
  void hide_item(irs_menu_base_t *ap_item);
  void show_item(irs_menu_base_t *ap_item);
  bool item_is_hidden(irs_menu_base_t *ap_item);
};

class irs_tablo_t: public irs_menu_base_t
{
  size_type f_parametr_count;
  //char mp_lcd_string[LINE_LEN+1];
  char *mp_lcd_string;
  //irs_menu_base_t *f_parametr_array[MAX_PARAMETR_COUNT];
  irs_menu_base_t **f_parametr_array;
  irs_menu_base_t *f_slave_menu;
public:
  irs_tablo_t();
  ~irs_tablo_t();
  virtual void set_disp_drv(mxdisplay_drv_service_t *ap_disp_drv);
  void draw(irs_menu_base_t **a_cur_menu);
  size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE);
  void set_slave_menu(irs_menu_base_t *a_slave_menu);
  irs_bool add(irs_menu_base_t *a_parametr);
  size_type get_dynamic_string(char *a_buffer, size_type a_length = 0);
};

class irs_advanced_tablo_t: public irs_menu_base_t
{
  struct tablo_item_t
  {
    irs_menu_base_t *p_parametr;
    size_type x;
    size_type y;
    irs_menu_param_show_mode_t show_mode;
    bool is_hidden;
  };
  vector<tablo_item_t> m_parametr_vector;
  char *mp_lcd_string;
  irs_menu_base_t *mp_slave_menu;
  bool m_creep_stopped;
  size_type m_updated_item;
public:
  irs_advanced_tablo_t();
  ~irs_advanced_tablo_t();
  virtual void set_disp_drv(mxdisplay_drv_service_t *ap_disp_drv);
  void draw(irs_menu_base_t **a_cur_menu);
  size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE);
  //! \brief Задает дочернее меню, переход к которому осуществляется по нажатию
  //!   клавиши irskey_enter, а возврат по нажатию клавиши irskey_escape
  void set_slave_menu(irs_menu_base_t *ap_slave_menu);
  bool add(irs_menu_base_t* ap_parametr, size_type a_x, size_type a_y,
    irs_menu_param_show_mode_t a_show_mode);
  size_type get_dynamic_string(char *a_buffer, size_type a_length = 0);
  void creep_start();
  void creep_stop();
  bool creep_stopped();
  size_type get_last_item_number();
  void hide_item(size_type a_item_number);
  void show_item(size_type a_item_number);
  bool item_is_hidden(size_type a_item_number);
  bool find_item(irs_menu_base_t *ap_parametr,
    size_t* ap_finded_item_idx = IRS_NULL);
  void hide_item(irs_menu_base_t *ap_parametr);
  void show_item(irs_menu_base_t *ap_parametr);
  bool item_is_hidden(irs_menu_base_t *ap_parametr);
};

class irs_menu_double_item_t: public irs_menu_base_t
{
  char *f_prefix;
  char *f_suffix;
  char *f_value_string;
  irs::num_mode_t f_num_mode;
  size_type f_len;
  size_type f_accur;
  float f_max;
  float f_min;
  double *f_parametr;
  irs_bool f_point_flag;
  double_trans_t f_double_trans;
  irs_bool f_blink_accept;
  counter_t f_blink_accept_to;
  double f_copy_parametr;
  char *f_copy_parametr_string;
  irs_menu_key_type_t f_key_type;
  double f_step;
  bool f_apply_immediately;
public:
  irs_menu_double_item_t(double *a_parametr, irs_bool a_can_edit);
  ~irs_menu_double_item_t();
  //! \brief Задает буфер для вывода числа, префикс, суффикс, длину числа,
  //!   точность отображения числа
  //! \param[in] a_value_string буфер для вывода числа, включая префикс и суффикс
  //! \param[in] a_prefix префикс
  //! \param[in] a_suffix суффикс
  //! \param[in] a_len длина выводимого числа, без суффикса и префикса
  //! \param[in] a_accur точность вывода числа
  //! \param[in] a_num_mode режим вывода числа, фиксированный по умолчанию
  void set_str(char *a_value_string, char *a_prefix, char *a_suffix,
    size_type a_len, size_type a_accur,
    irs::num_mode_t a_num_mode = irs::num_mode_fixed);
  void reset_str();
  void set_min_value(float a_min_value);
  float get_min_value();
  void set_max_value(float a_max_value);
  float get_max_value();
  virtual void draw(irs_menu_base_t **a_cur_menu);
  double *get_parametr();
  virtual void set_trans_function(double_trans_t a_double_trans);
  size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE);
  size_type get_dynamic_string(char *a_buffer, size_type a_length = 0);
  void set_can_edit(irs_bool a_can_edit);
  void set_key_type(irs_menu_key_type_t a_key_type);
  void set_change_step(double a_step);
  void set_apply_immediately(bool a_apply_immediately);
  virtual bool is_updated();
};

//------------------------- SIMPLY ITEM ---------------------------------------

template <class T>
class irs_menu_simply_item_t: public irs_menu_base_t
{
  char *mp_prefix;
  char *mp_suffix;
  char *mp_value_string;
  char *mp_copy_parametr_string;
  size_type m_len;
  size_type m_accur;
  T *mp_parametr;
  T m_copy_parametr;
public:
  irs_menu_simply_item_t(T *ap_parametr);
  ~irs_menu_simply_item_t();
  void set_str(char *ap_value_string, char *ap_prefix, char *ap_suffix,
    size_type a_len, size_type a_accur);
  void set_accuracy(size_type a_accuracy);
  virtual void draw(irs_menu_base_t **a_cur_menu);
  T *get_parametr();
  virtual size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE);
  virtual size_type get_dynamic_string(char *a_buffer, size_type a_length = 0);
  virtual bool is_updated();
};

template <class T>
irs_menu_simply_item_t<T>::irs_menu_simply_item_t(T *ap_parametr):
  mp_prefix(IRS_NULL),
  mp_suffix(IRS_NULL),
  mp_value_string(IRS_NULL),
  mp_copy_parametr_string(IRS_NULL),
  m_len(0),
  m_accur(0),
  mp_parametr(ap_parametr),
  m_copy_parametr(*ap_parametr)
{
}

template <class T>
irs_menu_simply_item_t<T>::~irs_menu_simply_item_t()
{
  delete []mp_copy_parametr_string;
}

template <class T>
void irs_menu_simply_item_t<T>::set_str(char *ap_value_string, char *ap_prefix,
  char *ap_suffix, size_type a_len, size_type a_accur)
{
  mp_value_string = ap_value_string;
  m_len = a_len;
  m_accur = a_accur;
  mp_prefix = ap_prefix;
  mp_suffix = ap_suffix;
  size_type space = 1;
  size_type full_len
    = m_len + space + strlen(mp_prefix) + space + strlen(mp_suffix);
  delete []mp_copy_parametr_string;
  mp_copy_parametr_string = new char [full_len + 1];
  m_copy_parametr = *mp_parametr;

  ostrstream strm(mp_value_string, m_len + 1);
  strm << fixed << setw(m_len) << setprecision(m_accur);
  strm << m_copy_parametr << '\0';
  mp_value_string[m_len] = '\0';

  strcpy(mp_copy_parametr_string, mp_value_string);
  m_updated = true;
}

template <class T>
void irs_menu_simply_item_t<T>::set_accuracy(size_type a_accuracy)
{
  m_accur = a_accuracy;
  if (mp_value_string && mp_prefix && mp_suffix) {
    set_str(mp_value_string, mp_prefix, mp_suffix, m_len, m_accur);
  }
}

template <class T>
void irs_menu_simply_item_t<T>::draw(irs_menu_base_t **a_cur_menu)
{
  irskey_t a_key = irskey_none;
  if (f_key_event) a_key = f_key_event->check();

  if (a_key == irskey_escape || a_key == irskey_enter)
  {
    if (f_master_menu != IRS_NULL)
    {
      *a_cur_menu = f_master_menu;
    }
    else
    {
      mp_disp_drv->clear();
      mp_disp_drv->outtextpos(0, 0, f_header);

      ostrstream strm(mp_value_string, m_len + 1);
      strm << fixed << setw(m_len) << setprecision(m_accur);
      strm << m_copy_parametr << '\0';
      mp_value_string[m_len] = '\0';

      mxdisp_pos_t space = 1;
      mxdisp_pos_t prf_x_pos = 0;
      mxdisp_pos_t val_x_pos = strlen(mp_prefix) + space;
      mxdisp_pos_t suf_x_pos = val_x_pos + strlen(mp_value_string) + space;
      mxdisp_pos_t y_pos = 1;

      mp_disp_drv->outtextpos(prf_x_pos, y_pos, mp_prefix);
      mp_disp_drv->outtextpos(val_x_pos, y_pos, mp_value_string);
      mp_disp_drv->outtextpos(suf_x_pos, y_pos, mp_suffix);

      if (f_creep != IRS_NULL)
      {
        f_creep->shift();
        mxdisp_pos_t creep_x_pos = 0;
        mxdisp_pos_t creep_y_pos = mp_disp_drv->get_height() - 1;
        mp_disp_drv->outtextpos(creep_x_pos, creep_y_pos, f_creep->get_line());
      }
    }
  }
}

template <class T>
T* irs_menu_simply_item_t<T>::get_parametr()
{
  return mp_parametr;
}

template <class T>
irs_menu_base_t::size_type irs_menu_simply_item_t<T>::get_parametr_string(
  char *a_parametr_string,
  size_type a_length,
  irs_menu_param_show_mode_t a_show_mode,
  irs_menu_param_update_t a_update)
{
  if (!a_parametr_string) return 0;

  if (m_copy_parametr != *mp_parametr)
  {
    m_updated = true;

    if (a_update == IMU_UPDATE)
    {
      m_copy_parametr = *mp_parametr;
      ostrstream strm(mp_value_string, m_len + 1);
      strm << fixed << setw(m_len) << setprecision(m_accur);
      strm << m_copy_parametr << '\0';
      mp_value_string[m_len] = '\0';
      strcpy(mp_copy_parametr_string, mp_value_string);
    }
    else
    {
      strcpy(mp_value_string, mp_copy_parametr_string);
    }
  }
  else
  {
    strcpy(mp_value_string, mp_copy_parametr_string);
  }

  const char *space_str = " ";

  switch (a_show_mode)
  {
    case IMM_WITHOUT_PREFIX:
    {
      strcat(mp_value_string, space_str);
      strcat(mp_value_string, mp_suffix);
      break;
    }
    case IMM_WITHOUT_SUFFIX:
    {
      strcpy(mp_value_string, mp_prefix);
      strcat(mp_value_string, space_str);
      strcat(mp_value_string, mp_copy_parametr_string);
      break;
    }
    case IMM_FULL:
    {
      strcpy(mp_value_string, mp_prefix);
      strcat(mp_value_string, space_str);
      strcat(mp_value_string, mp_copy_parametr_string);
      strcat(mp_value_string, space_str);
      strcat(mp_value_string, mp_suffix);
      break;
    }
  }

  size_type param_str_len = strlen(mp_value_string);

  if (a_length < param_str_len)
  {
    memcpy(a_parametr_string, mp_value_string, a_length);
    return a_length;
  }
  strcpy(a_parametr_string, mp_value_string);
  return param_str_len;
}

template <class T>
irs_menu_base_t::size_type irs_menu_simply_item_t<T>::get_dynamic_string(
  char *a_buffer, size_type a_length)
{
  return get_parametr_string(a_buffer, a_length);
}

template <class T>
bool irs_menu_simply_item_t<T>::is_updated()
{
  if (m_copy_parametr != *mp_parametr)
  {
    m_updated = false;
    return true;
  }
  bool updated = m_updated;
  m_updated = false;
  return updated;
}

//------------------------------------------------------------------------------

class irs_menu_bool_item_t: public irs_menu_base_t
{
  char *f_true_string;
  char *f_false_string;
  irs_bool *f_parametr;
  irs_bool f_temp_parametr;
  bool_trans_t f_bool_trans;
public:
  irs_menu_bool_item_t(irs_bool *a_parametr, irs_bool a_can_edit);
  void set_str(char *a_true_string, char *a_false_string);
  virtual void draw(irs_menu_base_t **a_cur_menu);
  irs_bool *get_parametr();
  virtual void set_trans_function(bool_trans_t a_bool_trans);
  size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE);
  size_type get_dynamic_string(char *a_buffer, size_type a_length = 0);
};

class irs_menu_ip_item_t: public irs_menu_base_t
{
  char *f_value_string;
  irs_menu_base_t *f_extra_parametr;
  irs_u8 *f_parametr;
  ip_trans_t f_ip_trans;
public:
  irs_menu_ip_item_t(irs_u8 *a_parametr, char *a_value_string,
    irs_bool a_can_edit);
  void reset_str();
  virtual void draw(irs_menu_base_t **a_cur_menu);
  irs_u8 *get_parametr();
  virtual void set_trans_function(ip_trans_t a_ip_trans);
  void set_extra_parametr(irs_menu_base_t *a_extra_parametr);
  size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE);
  size_type get_dynamic_string(char *a_buffer, size_type a_length = 0);
  const char *get_messge();
};

class irs_menu_trimmer_item_t: public irs_menu_base_t
{
  char *f_prefix;
  char *f_suffix;
  char *f_start_prefix;
  char *f_trim_prefix;
  char *f_step_prefix;
  char *f_value_string;
  size_type f_len;
  size_type f_trim_len;
  size_type f_step_len;
  size_type f_trim_accur;
  size_type f_step_accur;
  size_type f_accur;
  float f_max;
  float f_min;
  double f_start_value;
  double *f_parametr;
  double f_trim_value;
  double_trans_t f_double_trans;
  size_type f_step_count;
  double f_begin_step;
  double f_step;
  irs_bool f_save_after_exit;
  irs_bool f_first_raz;
  irs_menu_base_t *f_extra_parametr;

  size_type get_step_string(char *a_string, size_type a_length);
  void put_step_string();
public:
  irs_menu_trimmer_item_t(double *a_parametr, irs_bool a_save_after_exit);
  void set_str(char *a_value_string, char *a_prefix, char *a_start_prefix,
    char *a_trim_prefix, char *a_step_prefix, char *a_suffix, size_type a_len,
    size_type a_accur, size_type a_trim_len, size_type a_trim_accur,
    size_type a_step_len, size_type a_step_accur);
  void reset_str();
  void set_min_value(float a_min_value);
  float get_min_value();
  void set_max_value(float a_max_value);
  float get_max_value();
  void set_extra_parametr(irs_menu_base_t *a_extra_parametr);
  void set_step(double a_begin_step, size_type a_step_count);
  virtual void draw(irs_menu_base_t **a_cur_menu);
  virtual void set_trans_function(double_trans_t a_double_trans);
  virtual size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE);
  size_type get_dynamic_string(char *a_buffer, size_type a_length = 0);
};

//---------------------  2 PARAMETER MASTER  -----------------------------------

class irs_menu_2param_master_item_t: public irs_menu_base_t
{
  typedef size_type point_cnt_t;
  typedef double param_t;
  struct point_data_t
  {
    param_t value_1;
    param_t value_2;
    param_t out_value;
  };
  enum edit_status_t
  {
    ON,
    OFF
  };
  enum
  {
    m_max_point_count = 100,
    m_max_point_str_len = 3
  };
  char *mp_value_string;
  size_type m_len;
  size_type m_accur;
  char *mp_prefix_1;
  char *mp_suffix_1;
  irs_menu_param_show_mode_t m_show_mode_1;
  char *mp_prefix_2;
  char *mp_suffix_2;
  irs_menu_param_show_mode_t m_show_mode_2;
  char *mp_prefix_out;
  char *mp_suffix_out;
  irs_menu_param_show_mode_t m_show_mode_out;
  edit_status_t m_edit_status;
  point_cnt_t m_current_point;
  vector<point_data_t> m_point_vector;
  param_t m_step;
  param_t &m_param_1;
  param_t &m_param_2;
  param_t &m_out_param;
  param_t m_user_value_1;
  param_t m_user_value_2;
  param_t m_min_out_value;
  param_t m_max_out_value;
  param_t m_old_value;
  bool m_was_apply_point;
  point_cnt_t m_last_point;
  bool m_need_process_change;
  bool m_need_process_start;
  bool m_need_process_stop;
  bool m_need_out_param_change;
  bool m_started;
  bool m_prev_started;
  bool m_operating_duty;
  bool m_prev_operating_duty;
  bool m_entered;

  irskey_t m_start_key;
  irskey_t m_stop_key;

  void make_str(char *ap_prefix, char *ap_suffix,
    irs_menu_param_show_mode_t a_show_mode, param_t &a_param);
  void show_point_info();
  void show_process_status();
public:
  enum value_number_t {
    value_first,
    value_second
  };

  irs_menu_2param_master_item_t(
    param_t &a_param_1, param_t &a_param_2, param_t &a_out_param);
  ~irs_menu_2param_master_item_t();
  void set_str(char *ap_value_string, size_type a_len, size_type a_accur);
  void set_param_1_str(char *ap_prefix, char *ap_suffix,
    irs_menu_param_show_mode_t a_show_mode);
  void set_param_2_str(char *ap_prefix, char *ap_suffix,
    irs_menu_param_show_mode_t a_show_mode);
  void set_param_out_str(char *ap_prefix, char *ap_suffix,
    irs_menu_param_show_mode_t a_show_mode);
  void set_step(param_t a_step);
  void set_limit_values(param_t a_min_out_value, param_t a_max_out_value);
  void add_point(param_t a_value_1, param_t a_value_2, param_t a_out_value);
  bool get_last_point(point_cnt_t &a_last_point, param_t &a_point_value);
  void set_start_key(irskey_t a_start_key);
  void set_stop_key(irskey_t a_stop_key);
  void set_operating_duty(bool a_operating_duty);
  void external_process_stop();
  virtual size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE);
  virtual size_type get_dynamic_string(char *ap_buffer, size_type a_length = 0);
  virtual void draw(irs_menu_base_t **ap_cur_menu);
  //
  bool check_process_change_param();
  bool check_process_start();
  bool check_process_stop();
  bool check_out_param_change();
  void reserve(size_t a_size);
  void reset_cur_param_to(value_number_t a_value_number);
};

//------------------------------------------------------------------------------

// Класс строкового пункта меню предназначеный только для irs_menu_tablo_t
class irs_menu_string_item_t: public irs_menu_base_t
{
  char *mp_string;
public:
  irs_menu_string_item_t();
  ~irs_menu_string_item_t();
  void set_parametr_string(char *ap_parametr_string);
  virtual size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE);
  virtual size_type get_dynamic_string(char *ap_buffer, size_type a_length = 0);
  virtual void draw(irs_menu_base_t **ap_cur_menu);
};

//-----------------------------  PROGRESS BAR  ---------------------------------

class irs_menu_progress_bar_t: public irs_menu_base_t
{
  char *mp_bar;
  size_type m_symbol;
  size_type m_length;
  size_type m_progress;
  float m_max_value;
  float m_value;
  void fill_bar();
public:
  irs_menu_progress_bar_t(size_type a_symbol, size_type a_length,
    float a_max_value);
  ~irs_menu_progress_bar_t();
  void set_length(size_type a_length);
  void set_value(float a_value);
  void set_max_value(float a_max_value);
  virtual size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE);
  virtual size_type get_dynamic_string(char *ap_buffer, size_type a_length = 0);
  virtual void draw(irs_menu_base_t **ap_cur_menu);
};

//! @}

#endif //IRSMENUH
