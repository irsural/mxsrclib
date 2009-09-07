// Текстовое меню
// Дата: 31.08.2009

#ifndef IRSMENUH
#define IRSMENUH

#include <irsdefs.h>
#include <timer.h>
//#include <irslcd.h>
#include <irsstd.h>

#define CUR_BLINK 500//ms
#define EDIT_LINE 2

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
public:
  irs_menu_base_t();
  ~irs_menu_base_t();
  void set_key_event(mxkey_event_t *key_event);
  void add_change_event(mxfact_event_t *ap_event);
  virtual void set_disp_drv(mxdisplay_drv_service_t *ap_disp_drv);
  void set_cursor_symbol(char a_cursor_symbol);
  void set_master_menu(irs_menu_base_t *a_master_menu);
  void set_header(char *a_header);
  char *get_header();
  void set_message(char *a_message);
  char *get_message();
  void set_creep(irs_menu_creep_t *a_creep);
  irs_menu_state_t get_state();
  void show();
  void hide();
  void edit();
  irs_bool can_edit();
  virtual size_type get_parametr_string(
    char *a_parametr_string,
    size_type a_length = 0,
    irs_menu_param_show_mode_t a_show_mode = IMM_FULL,
    irs_menu_param_update_t a_update = IMU_UPDATE) = 0;
  virtual size_type get_dynamic_string(char *a_buffer, size_type a_length = 0) = 0;
  virtual bool is_updated();
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
  void set_slave_menu(irs_menu_base_t *ap_slave_menu);
  bool add(irs_menu_base_t *ap_parametr, size_type a_x, size_type a_y,
    irs_menu_param_show_mode_t a_show_mode);
  size_type get_dynamic_string(char *a_buffer, size_type a_length = 0);
  void creep_start();
  void creep_stop();
  bool creep_stopped();
};

class irs_menu_double_item_t: public irs_menu_base_t
{
  char *f_prefix;
  char *f_suffix;
  char *f_value_string;
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
  void set_str(char *a_value_string, char *a_prefix, char *a_suffix,
    size_type a_len, size_type a_accur);
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
  irs_menu_ip_item_t(irs_u8 *a_parametr, char *a_value_string, irs_bool a_can_edit);
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
    size_type a_accur, size_type a_trim_len, size_type a_trim_accur, size_type a_step_len,
    size_type a_step_accur);
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
  irs_menu_progress_bar_t(size_type a_symbol, size_type a_length, float a_max_value);
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
  void shift();
  void clear_dynamic();
  void soft_clear_dynamic();
  void change_static(char *a_static);
  void change_message(char *a_message);
  char *get_creep_buffer();
  char *get_line();
  irs_bool eol();
};


#endif //IRSMENUH
