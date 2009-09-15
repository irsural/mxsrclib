// Текстовое меню
// Дата: 8.09.2009

#include <irsmenu.h>
#include <irsdefs.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <mxifa.h>

#define blink_accept_time TIME_TO_CNT(1, 2)

double my_str_to_double(irs_u8 *str);

typedef double (* str_to_double_t)(irs_u8 *);

char irs_menu_base_t::empty_str[] = "";
char irs_menu_base_t::zero_str[] = "0";
irs_menu_base_t::irs_menu_base_t():
  f_master_menu(IRS_NULL),
  f_header(empty_str),
  f_message(empty_str),
  f_creep(IRS_NULL),
  f_creep_buffer(empty_str),
  f_blink_counter(0),
  f_cur_symbol(0),
  f_can_edit(irs_false),
  f_state(ims_show),
  f_want_redraw(irs_true),
  f_message_count(0),
  f_key_event(IRS_NULL),
  f_show_needed(irs_true),
  f_cursor_symbol('_'),
  mp_disp_drv(IRS_NULL),
  mp_event(IRS_NULL),
  m_updated(true)
{
  f_cursor[0] = f_cursor_symbol;
  f_cursor[1] = 0;
  //memset((void *)f_message_array, 0, sizeof(f_message_array));
  //memset((void *)f_message_len_array, 0, sizeof(f_message_len_array));

  init_to_cnt();
  //show();
}

irs_menu_base_t::~irs_menu_base_t()
{
  deinit_to_cnt();
}
void irs_menu_base_t::set_key_event(mxkey_event_t *key_event)
{
  f_key_event = key_event;
}
void irs_menu_base_t::add_change_event(mxfact_event_t *ap_event)
{
  ap_event->connect(mp_event);
}
void irs_menu_base_t::set_disp_drv(mxdisplay_drv_service_t *ap_disp_drv)
{
  mp_disp_drv = ap_disp_drv;
}
void irs_menu_base_t::set_cursor_symbol(char a_cursor_symbol)
{
  f_cursor_symbol = a_cursor_symbol;
}
void irs_menu_base_t::set_master_menu(irs_menu_base_t *a_master_menu)
{
  f_master_menu = a_master_menu;
}

void irs_menu_base_t::set_header(char *a_header)
{
  f_header = a_header;
}
char *irs_menu_base_t::get_header()
{
  return f_header;
}

irs_bool irs_menu_base_t::can_edit()
{
  return f_can_edit;
}

void irs_menu_base_t::set_message(char *a_message)
{
  f_message = a_message;
}
char *irs_menu_base_t::get_message()
{
  return f_message;
}

void irs_menu_base_t::set_creep(irs_menu_creep_t *a_creep)
{
  f_creep = a_creep;
  f_creep_buffer = f_creep->get_creep_buffer();
}

/*void irs_menu_base_t::set_master_menu(irs_menu_base_t *a_master_menu)
{
  f_master_menu = a_master_menu;
}*/

irs_menu_state_t irs_menu_base_t::get_state()
{
  return f_state;
}

void irs_menu_base_t::show()
{
  f_state = ims_show;
  f_want_redraw = irs_true;
  if (f_creep != IRS_NULL)
  {
    f_creep->change_message(f_message);
    f_creep->clear_dynamic();
    get_dynamic_string(f_creep_buffer);
  }
}
void irs_menu_base_t::hide()
{
  f_state = ims_hide;
}
void irs_menu_base_t::edit()
{
  f_state = ims_edit;
}

bool irs_menu_base_t::is_updated()
{
  bool updated = m_updated;
  m_updated = false;
  return updated;
}

irs_menu_t::irs_menu_t():
  f_menu_array(IRS_NULL),
  f_items_count(0),
  f_current_item(0),
  f_arrow_position(0)
{
  init_to_cnt();
  f_want_redraw = irs_false;
  f_creep = IRS_NULL;
  //show();
}

irs_menu_t::~irs_menu_t()
{
  IRS_DELETE(f_menu_array);
}

irs_bool irs_menu_t::add(irs_menu_base_t *item)
{
  if (f_key_event) item->set_key_event(f_key_event);
  if (f_creep) item->set_creep(f_creep);
  if (mp_disp_drv) item->set_disp_drv(mp_disp_drv);
  item->set_cursor_symbol(f_cursor_symbol);
  f_menu_array = (irs_menu_base_t **)irs_renew(
    (void *)f_menu_array,
    f_items_count*sizeof(irs_menu_base_t*),
    (f_items_count + 1)*sizeof(irs_menu_base_t*)
  );
  f_menu_array[f_items_count] = item;
  item->set_master_menu(this);
  f_items_count++;
  return irs_true;
}

irs_menu_base_t::size_type irs_menu_t::get_current_item()
{
  return f_current_item;
}
irs_menu_base_t::size_type irs_menu_t::get_items_count()
{
  return f_items_count;
}

irs_menu_base_t::size_type irs_menu_t::get_parametr_string(
  char *a_parametr_string,
  size_type a_length,
  irs_menu_param_show_mode_t /*a_show_mode*/,
  irs_menu_param_update_t /*a_update*/)
{
  if (a_parametr_string)
  {
    size_type copy_length = strlen(f_header);
    size_type max_length = mp_disp_drv->get_width();
    if (a_length == 0)
    {
      if (copy_length > max_length)
      {
        copy_length = mp_disp_drv->get_width();
      }
    }
    else
    {
      if (copy_length > a_length)
      {
        copy_length = a_length;
      }
    }
    memcpy(a_parametr_string, f_header, copy_length + 1);
    return copy_length;
  }
  return 0;
}

irs_menu_base_t::size_type irs_menu_t::get_dynamic_string(char *a_buffer,
  irs_menu_base_t::size_type /*a_length*/)
{
  a_buffer = a_buffer;
  return 0;
}

void irs_menu_t::draw(irs_menu_base_t **a_cur_menu)
{
  //lcd_tick();
  if (f_show_needed) {
    f_show_needed = irs_false;
    show();
  }
  irskey_t a_key = irskey_none;
  if (f_key_event) a_key = f_key_event->check();
  switch (f_state)
  {
  case ims_hide:
    {
      break;
    }
  case ims_edit:
    {
      *a_cur_menu = f_menu_array[f_current_item];
      f_show_needed = irs_true;
      //f_menu_array[f_current_item]->show();
      //f_menu_array[f_current_item]->draw(a_key);
      //if (f_menu_array[f_current_item]->get_state() == ims_hide) show();
      break;
    }
  case ims_show:
    {
      if (f_want_redraw)
      {
        mp_disp_drv->clear();
        mp_disp_drv->outtextpos(0, 0, "^~");
        mp_disp_drv->outtextpos(3, 0, f_header);
        const mxdisp_pos_t delta = f_current_item - f_arrow_position;
        const mxdisp_pos_t bottom_line = mp_disp_drv->get_height() - 2;
        const mxdisp_pos_t max_index = f_items_count - delta;
        for (mxdisp_pos_t i = 0; i <= bottom_line && i < max_index; i++)
        {
          mxdisp_pos_t x_pos = 1;
          mxdisp_pos_t y_pos = i + 1;
          size_type cur_item = i + delta;
          mp_disp_drv->outtextpos(x_pos, y_pos,
           f_menu_array[cur_item]->get_header());
        }
        size_type cursor_x_pos = 0;
        size_type cursor_y_pos = f_arrow_position + 1;
        mp_disp_drv->outtextpos(cursor_x_pos, cursor_y_pos, ">");
        f_want_redraw = irs_false;
      }
      //f_want_redraw = irs_true;
      switch (a_key)
      {
      case irskey_up:
        {
          if (f_arrow_position == 0)
          {
            if (f_current_item > 0)
            {
              f_current_item--;
              f_want_redraw = irs_true;
            }
          }
          else
          {
            f_arrow_position--;
            if (f_current_item > 0)
            {
              f_current_item--;
            }
            f_want_redraw = irs_true;
          }
          break;
        }
      case irskey_down:
        {
          if (f_arrow_position >= (mp_disp_drv->get_height() - 2))
          {
            if (f_current_item < (f_items_count-1))
            {
              f_current_item++;
              f_want_redraw = irs_true;
            }
          }
          else
          {
            if (f_current_item < (f_items_count-1))
            {
              f_arrow_position++;
              f_current_item++;
              f_want_redraw = irs_true;
            }
          }
          break;
        }
      case irskey_enter:
        {
          if (f_items_count > 0) {
            //f_menu_array[f_current_item]->show();
            f_menu_array[f_current_item]->set_master_menu(this);
            edit();
            f_want_redraw = irs_true;
          }
          break;
        }
      case irskey_escape:
        {
          if (f_master_menu != IRS_NULL)
          {
            //hide();
            f_want_redraw = irs_true;
            //f_master_menu->show();
            *a_cur_menu = f_master_menu;
            f_show_needed = irs_true;
            //show();
          }
          break;
        }
      default:
      {

        break;
      }
      }
      if (f_want_redraw)
      {
        mp_disp_drv->clear();
        mp_disp_drv->outtextpos(0,0,"^~");
        mp_disp_drv->outtextpos(3,0, f_header);
        size_type delta = f_current_item - f_arrow_position;
        for (size_t i = 0; (i <= (mp_disp_drv->get_height() - 2)) &&
          ((i + delta) < f_items_count); i++)
        {
          size_type x_pos = 1;
          size_type y_pos = i + 1;
          size_type cur_item = i + delta;
          mp_disp_drv->outtextpos(x_pos, y_pos,
            f_menu_array[cur_item]->get_header());
        }
        size_type cursor_x_pos = 0;
        size_type cursor_y_pos = f_arrow_position + 1;
        mp_disp_drv->outtextpos(cursor_x_pos, cursor_y_pos, ">");
        f_want_redraw = irs_false;
      }
    }
  }
}

//--------------------------  MENU_DOUBLE_ITEM  --------------------------------

irs_menu_double_item_t::irs_menu_double_item_t(double *a_parametr,
  irs_bool a_can_edit):
  f_suffix(empty_str),
  f_prefix(empty_str),
  f_value_string(empty_str),
  f_len(0),
  f_accur(0),
  f_min(0.0f),
  f_max(0.0f),
  f_parametr(a_parametr),
  f_point_flag(irs_false),
  f_double_trans(IRS_NULL),
  f_blink_accept(irs_true),
  f_blink_accept_to(0),
  f_copy_parametr(*f_parametr),
  f_copy_parametr_string(0),
  f_key_type(IMK_DIGITS),
  f_step(0.),
  f_apply_immediately(false)
{
  f_master_menu = IRS_NULL;
  f_cur_symbol = 0;
  f_cursor[0] = f_cursor_symbol;
  f_cursor[1] = '\0';
  f_can_edit = a_can_edit;
  f_creep = IRS_NULL;
  init_to_cnt();
  set_to_cnt(f_blink_counter, MS_TO_CNT(CUR_BLINK));
}

irs_menu_double_item_t::~irs_menu_double_item_t()
{
  deinit_to_cnt();
}

void irs_menu_double_item_t::set_trans_function(double_trans_t a_double_trans)
{
  f_double_trans = a_double_trans;
  delete []f_copy_parametr_string;
}

void irs_menu_double_item_t::set_max_value(float a_max_value)
{
  f_max = a_max_value;
}

void irs_menu_double_item_t::set_min_value(float a_min_value)
{
  f_min = a_min_value;
}

void irs_menu_double_item_t::set_str(char *a_value_string, char *a_prefix,
  char *a_suffix, size_type a_len, size_type a_accur)
{
  f_value_string = a_value_string;
  f_len = a_len;
  f_accur = a_accur;
  f_prefix = a_prefix;
  f_suffix = a_suffix;
  
  size_type space = 1;
  size_type full_len 
    = f_len + space + strlen(f_prefix) + space + strlen(f_suffix);
  
  delete []f_copy_parametr_string;
  f_copy_parametr_string = new char [full_len + 1];
  f_copy_parametr = *f_parametr;
  afloat_to_str(f_value_string, f_copy_parametr, f_len, f_accur);
  strcpy(f_copy_parametr_string, f_value_string);
  m_updated = true;
}

void irs_menu_double_item_t::reset_str()
{
  //for (irs_u8 i = 0; i < f_len; f_value_string[i++] = ' ');
  memset(f_value_string, ' ', f_len);
  f_value_string[f_len] = '\0';
}

double *irs_menu_double_item_t::get_parametr()
{
  return f_parametr;
}

irs_menu_base_t::size_type irs_menu_double_item_t::get_parametr_string(
  char *a_parametr_string,
  irs_menu_base_t::size_type a_length,
  irs_menu_param_show_mode_t a_show_mode,
  irs_menu_param_update_t a_update)
{
  if (a_parametr_string)
  {
    size_type len = 0;
    if (a_show_mode != IMM_WITHOUT_PREFIX)
    {
      len = strlen(f_prefix);
      if ((a_length > 0) && (len > a_length))
      {
        len = a_length;
        memcpy(a_parametr_string, f_prefix, len);
        a_parametr_string[len] = '\0';
        return len;
      }
      for (size_type i = 0; i < len; i++)
      {
        a_parametr_string[i] = f_prefix[i];
      }
      a_parametr_string[len] = ' ';
      len++;
    }
    if (f_copy_parametr != *f_parametr)
    {
      m_updated = true;

      if (a_update == IMU_UPDATE)
      {
        f_copy_parametr = *f_parametr;
        afloat_to_str(f_value_string, f_copy_parametr, f_len, f_accur);
        strcpy(f_copy_parametr_string, f_value_string);
      }
      else
      {
        strcpy(f_value_string, f_copy_parametr_string);
      }
    }
    else
    {
      strcpy(f_value_string, f_copy_parametr_string);
    }
    size_type len2 = strlen(f_value_string);

    if ((a_length > 0) && ((len + len2) > a_length))
    {
      len2 = a_length - len;
      memcpy((void*)&a_parametr_string[len], f_value_string, len2);
      size_type real_len = len + len2;
      a_parametr_string[real_len] = '\0';
      return real_len;
    }

    for (size_type i = 0; i < len2; i++)
    {
      a_parametr_string[i+len] = f_value_string[i];
    }

    if (a_show_mode != IMM_WITHOUT_SUFFIX)
    {
      a_parametr_string[len+len2] = ' ';
      len2++;
      size_type len3 = strlen(f_suffix);

      if ((a_length > 0) && ((len + len2 + len3) > a_length))
      {
        len3 = a_length - len - len2;
        memcpy((void*)&a_parametr_string[len+len2], f_suffix, len3);
        size_type real_len = len + len2 + len3;
        a_parametr_string[real_len] = '\0';
        return real_len;
      }

      for (size_type i = 0; i < len3; i++)
      {
        a_parametr_string[i+len+len2] = f_suffix[i];
      }
      size_type real_len = len + len2 + len3;
      a_parametr_string[real_len] = '\0';
      return real_len;
    }
    else
    {
      size_type real_len = len + len2;
      a_parametr_string[real_len] = '\0';
      return real_len;
    }
  }
  return 0;
}

irs_menu_base_t::size_type irs_menu_double_item_t::get_dynamic_string(char *a_buffer,
  irs_menu_base_t::size_type /*a_length*/)
{
  a_buffer = a_buffer;
  return 0;
}

void irs_menu_double_item_t::set_can_edit(irs_bool a_can_edit)
{
  f_can_edit = a_can_edit;
}

void irs_menu_double_item_t::set_key_type(irs_menu_key_type_t a_key_type)
{
  f_key_type = a_key_type;
  if (f_key_type == IMK_DIGITS) f_apply_immediately = false;
}

void irs_menu_double_item_t::set_change_step(double a_step)
{
  f_step = a_step;
}

void irs_menu_double_item_t::set_apply_immediately(bool a_apply_immediately)
{
  if (f_key_type == IMK_ARROWS) f_apply_immediately = a_apply_immediately;
}

bool irs_menu_double_item_t::is_updated()
{
  if (f_copy_parametr != *f_parametr)
  {
    // Sobaka рылась here
    //f_copy_parametr = *f_parametr;
    m_updated = false;
    return true;
  }
  bool updated = m_updated;
  m_updated = false;
  return updated;
}

void irs_menu_double_item_t::draw(irs_menu_base_t **a_cur_menu)
{
  if (f_show_needed) {
    f_show_needed = irs_false;
    show();
  }
  irskey_t a_key = irskey_none;
  if (f_key_event) a_key = f_key_event->check();
  switch (f_state)
  {
  case ims_show:
    {
      if (f_can_edit)
      {
        reset_str();
        mp_disp_drv->clear();
        f_want_redraw = irs_true;
        set_to_cnt(f_blink_counter,MS_TO_CNT(CUR_BLINK));
        f_point_flag = irs_false;
        f_cur_symbol = 0;
        //mp_disp_drv->clear_line(EDIT_LINE);
        mp_disp_drv->outtextpos(0, 0, f_header);
        afloat_to_str(f_value_string, *f_parametr, f_len, f_accur);

        const size_type space = 1;
        size_type prf_x_pos = 0;
        size_type val_x_pos = strlen(f_prefix) + space;
        size_type suf_x_pos = val_x_pos + space + strlen(f_value_string);
        size_type y_pos = 1;

        mp_disp_drv->outtextpos(prf_x_pos, y_pos, f_prefix);
        mp_disp_drv->outtextpos(val_x_pos, y_pos, f_value_string);
        mp_disp_drv->outtextpos(suf_x_pos, y_pos, f_suffix);
        f_cursor[0] = ' ';
        f_copy_parametr = *f_parametr;
        edit();
      }
      else
      {
        switch(a_key)
        {
        case irskey_enter:
          {
            if (f_can_edit)
            {
              //reset_str();
              f_want_redraw = irs_true;
              //set_to_cnt(f_blink_counter,MS_TO_CNT(CUR_BLINK));
              //f_point_flag = irs_false;
              //f_cur_symbol = 0;
              //mp_disp_drv->clear_line(EDIT_LINE);
              //edit();
              *a_cur_menu = f_master_menu;
              f_show_needed = irs_true;
              if (!f_creep)
                mp_disp_drv->clear_line(mp_disp_drv->get_height() - 1);
              //f_master_menu->show();
            }
            break;
          }
        case irskey_escape:
          {
            //hide();
            *a_cur_menu = f_master_menu;
            f_show_needed = irs_true;
            //f_master_menu->show();
            break;
          }
        default:
          {
            if (f_want_redraw || (!f_can_edit))
            {
              mp_disp_drv->clear();
              mp_disp_drv->outtextpos(0, 0, f_header);
              afloat_to_str(f_value_string, *f_parametr, f_len, f_accur);

              const size_type space = 1;
              size_type prf_x_pos = 0;
              size_type val_x_pos = strlen(f_prefix) + space;
              size_type suf_x_pos = val_x_pos + space + strlen(f_value_string);
              size_type y_pos = 1;

              mp_disp_drv->outtextpos(prf_x_pos, y_pos, f_prefix);
              mp_disp_drv->outtextpos(val_x_pos, y_pos, f_value_string);
              mp_disp_drv->outtextpos(suf_x_pos, y_pos, f_suffix);
              f_want_redraw = irs_false;
            }
            if (f_creep != IRS_NULL)
            {
              f_creep->shift();
              mxdisp_pos_t x_pos = 0;
              mxdisp_pos_t y_pos = mp_disp_drv->get_height() - 1;
              mp_disp_drv->outtextpos(x_pos, y_pos, f_creep->get_line());
            }
            else mp_disp_drv->clear_line(mp_disp_drv->get_height() - 1);
            break;
          }
        }
      }
      break;
    }
  case ims_edit:
    {
      switch (f_key_type)
      {
        case IMK_DIGITS:
        {
          if (a_key == irskey_backspace)
          {
            if (f_cur_symbol > 0)
            {
              f_cur_symbol--;
              mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "  ");
              if (f_value_string[f_cur_symbol] == '.') f_point_flag = irs_false;
              f_value_string[f_cur_symbol] = ' ';
            }
            else
            {
              *a_cur_menu = f_master_menu;
              f_show_needed = irs_true;
            }
          }
          if (a_key == irskey_escape)
          {
            *a_cur_menu = f_master_menu;
            f_show_needed = irs_true;
          }
          if (a_key == irskey_enter)
          {
            f_value_string[f_cur_symbol] = '\0';
            if (f_cur_symbol > 0)
            {
              *f_parametr = atof(f_value_string);
              if (*f_parametr > f_max) *f_parametr = f_max;
              if (*f_parametr < f_min) *f_parametr = f_min;
              if (f_double_trans) f_double_trans(f_parametr);
              if (mp_event) mp_event->exec();
              m_updated = true;
              mp_disp_drv->outtextpos(0, 0, f_value_string);
            }
            *a_cur_menu = f_master_menu;
            f_show_needed = irs_true;
          }
          if (f_creep)
          {
            f_creep->shift();
            mxdisp_pos_t x_pos = 0;
            mxdisp_pos_t y_pos = mp_disp_drv->get_height() - 1;
            mp_disp_drv->outtextpos(x_pos, y_pos, f_creep->get_line());
          }
          if (f_cur_symbol < f_len)
          {
            switch(a_key)
            {
            case irskey_1:
              {
                f_value_string[f_cur_symbol] = '1';
                mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "1");
                f_cur_symbol++;
                break;
              }
            case irskey_2:
              {
                f_value_string[f_cur_symbol] = '2';
                mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "2");
                f_cur_symbol++;
                break;
              }
            case irskey_3:
              {
                f_value_string[f_cur_symbol] = '3';
                mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "3");
                f_cur_symbol++;
                break;
              }
            case irskey_4:
              {
                f_value_string[f_cur_symbol] = '4';
                mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "4");
                f_cur_symbol++;
                break;
              }
            case irskey_5:
              {
                f_value_string[f_cur_symbol] = '5';
                mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "5");
                f_cur_symbol++;
                break;
              }
            case irskey_6:
              {
                f_value_string[f_cur_symbol] = '6';
                mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "6");
                f_cur_symbol++;
                break;
              }
            case irskey_7:
              {
                f_value_string[f_cur_symbol] = '7';
                mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "7");
                f_cur_symbol++;
                break;
              }
            case irskey_8:
              {
                f_value_string[f_cur_symbol] = '8';
                mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "8");
                f_cur_symbol++;
                break;
              }
            case irskey_9:
              {
                f_value_string[f_cur_symbol] = '9';
                mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "9");
                f_cur_symbol++;
                break;
              }
            case irskey_0:
              {
                f_value_string[f_cur_symbol] = '0';
                mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "0");
                f_cur_symbol++;
                break;
              }
            case irskey_point:
              {
                if (!f_point_flag)
                {
                  f_value_string[f_cur_symbol] = '.';
                  mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, ".");
                  f_cur_symbol++;
                  f_point_flag = irs_true;
                }
                break;
              }
            }
          }

          if (a_key != irskey_none) {
            set_to_cnt(f_blink_accept_to, blink_accept_time);
            f_blink_accept = irs_false;
            f_cursor[0] = f_cursor_symbol;
            mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, f_cursor);
          }
          if (f_blink_accept) {
            if (test_to_cnt(f_blink_counter)) {
              set_to_cnt(f_blink_counter, MS_TO_CNT(CUR_BLINK));
              //set_to_cnt(f_blink_counter, TIME_TO_CNT(CUR_BLINK, 1000));
              f_cursor[0] =
                (f_cursor[0] == f_cursor_symbol) ? ' ' : f_cursor_symbol;
              mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, f_cursor);
            }
          } else {
            if (test_to_cnt(f_blink_accept_to)) {
              set_to_cnt(f_blink_counter, MS_TO_CNT(CUR_BLINK));
              f_blink_accept = irs_true;
            }
          }
          break;
        }
        case IMK_ARROWS:
        {
          switch (a_key)
          {
            case irskey_escape:
            {
              *a_cur_menu = f_master_menu;
              f_show_needed = irs_true;
              if (f_apply_immediately)
              {
                *f_parametr = f_copy_parametr;
              }
              else
              {
                f_copy_parametr = *f_parametr;
              }
              afloat_to_str(f_value_string, *f_parametr, f_len, f_accur);
              strcpy(f_copy_parametr_string, f_value_string);
              if (f_double_trans) f_double_trans(f_parametr);
              if (mp_event) mp_event->exec();
              m_updated = true;
              break;
            }
            case irskey_enter:
            {
              *a_cur_menu = f_master_menu;
              f_show_needed = irs_true;
              if (f_apply_immediately)
              {
                f_copy_parametr = *f_parametr;
              }
              else
              {
                *f_parametr = f_copy_parametr;
              }
              afloat_to_str(f_value_string, *f_parametr, f_len, f_accur);
              strcpy(f_copy_parametr_string, f_value_string);
              if (f_double_trans) f_double_trans(f_parametr);
              if (mp_event) mp_event->exec();
              m_updated = true;
              break;
            }
            case irskey_up:
            {
              if (f_apply_immediately)
              {
                *f_parametr += f_step;
                if (*f_parametr > f_max) *f_parametr = f_max;
                if (f_double_trans) f_double_trans(f_parametr);
                if (mp_event) mp_event->exec();
                m_updated = true;
              }
              else
              {
                f_copy_parametr += f_step;
                if (f_copy_parametr > f_max) f_copy_parametr = f_max;
              }
              f_want_redraw = true;
              break;
            }
            case irskey_down:
            {
              if (f_apply_immediately)
              {
                *f_parametr -= f_step;
                if (*f_parametr < f_min) *f_parametr = f_min;
                if (f_double_trans) f_double_trans(f_parametr);
                if (mp_event) mp_event->exec();
                m_updated = true;
              }
              else
              {
                f_copy_parametr -= f_step;
                if (f_copy_parametr < f_min) f_copy_parametr = f_min;
              }
              f_want_redraw = true;
              break;
            }
          }
          if (f_want_redraw)
          {
            f_want_redraw = false;

            if (f_apply_immediately)
            {
              afloat_to_str(f_value_string, *f_parametr, f_len, f_accur);
            }
            else
            {
              afloat_to_str(f_value_string, f_copy_parametr, f_len, f_accur);
            }

            mxdisp_pos_t pref_len = mxdisp_pos_t(strlen(f_prefix));
            mxdisp_pos_t val_len = mxdisp_pos_t(strlen(f_value_string));
            mxdisp_pos_t space = 1;
            mxdisp_pos_t val_pos_x =
              static_cast<mxdisp_pos_t>(pref_len + space);
            mxdisp_pos_t suff_pos_x = static_cast<mxdisp_pos_t>(pref_len +
              space + val_len + space);
            mxdisp_pos_t line_num = 1;

            mp_disp_drv->clear_line(1);
            mp_disp_drv->outtextpos(0, 1, f_prefix);
            mp_disp_drv->outtextpos(val_pos_x, line_num, f_value_string);
            mp_disp_drv->outtextpos(suff_pos_x, line_num, f_suffix);
          }
          break;
        }
      }
      break;
    }
  }
}

//------------------------------------------------------------------------------

irs_tablo_t::irs_tablo_t():
  f_slave_menu(IRS_NULL),
  f_parametr_count(0),
  mp_lcd_string(IRS_NULL)
{
  f_state = ims_hide;
  f_creep = IRS_NULL;
}

irs_tablo_t::~irs_tablo_t()
{
  IRS_ARDELETE(f_parametr_array);
  IRS_ARDELETE(mp_lcd_string);
}

void irs_tablo_t::set_disp_drv(mxdisplay_drv_service_t *ap_disp_drv)
{
  irs_menu_base_t::set_disp_drv(ap_disp_drv);
  if (mp_disp_drv) {
    mp_lcd_string = new char[mp_disp_drv->get_width() + 1];
    memset((void *)mp_lcd_string, 0, mp_disp_drv->get_width() + 1);
    f_parametr_array = new irs_menu_base_t*[mp_disp_drv->get_height() - 1];
    memset((void *)f_parametr_array, 0,
      sizeof(irs_menu_base_t*)*irs_u8(mp_disp_drv->get_height() - 1));
  }
}

irs_bool irs_tablo_t::add(irs_menu_base_t *a_parametr)
{
  if (f_parametr_count < (mp_disp_drv->get_height() - 1))
  {
    if (f_key_event) a_parametr->set_key_event(f_key_event);
    if (f_creep) a_parametr->set_creep(f_creep);
    if (mp_disp_drv) a_parametr->set_disp_drv(mp_disp_drv);
    a_parametr->set_cursor_symbol(f_cursor_symbol);
    f_parametr_array[f_parametr_count] = a_parametr;
    f_parametr_count++;
    return irs_true;
  }
  else return irs_false;
}

void irs_tablo_t::set_slave_menu(irs_menu_base_t *a_slave_menu)
{
  if (f_key_event) a_slave_menu->set_key_event(f_key_event);
  if (f_creep) a_slave_menu->set_creep(f_creep);
  if (mp_disp_drv) a_slave_menu->set_disp_drv(mp_disp_drv);
  a_slave_menu->set_cursor_symbol(f_cursor_symbol);
  f_slave_menu = a_slave_menu;
  f_slave_menu->set_master_menu(this);
}

irs_menu_base_t::size_type irs_tablo_t::get_parametr_string(
  char *a_parametr_string,
  irs_menu_base_t::size_type /*a_length*/,
  irs_menu_param_show_mode_t /*a_show_mode*/,
  irs_menu_param_update_t /*a_update*/)
{
  if (a_parametr_string)
  {
    strcpy(a_parametr_string, empty_str);
  }
  return 0;
}

irs_menu_base_t::size_type irs_tablo_t::get_dynamic_string(char *a_buffer,
  irs_menu_base_t::size_type /*a_length*/)
{
  a_buffer = a_buffer;
  return 0;
}

void irs_tablo_t::draw(irs_menu_base_t **a_cur_menu)
{
  if (f_show_needed) {
    f_show_needed = irs_false;
    show();
  }
  irskey_t a_key = irskey_none;
  if (f_key_event) a_key = f_key_event->check();
  switch (f_state)
  {
    case ims_show:
    {
      switch(a_key)
      {
      case irskey_enter:
        {
          if (f_slave_menu != IRS_NULL)
          {
            //edit();
            f_want_redraw = irs_false;
            //f_slave_menu->show();
            *a_cur_menu = f_slave_menu;
            f_show_needed = irs_true;
            //show();
          }
          break;
        }
      case irskey_escape:
        {
          if (f_master_menu != IRS_NULL)
          {
            //hide();
            f_want_redraw = irs_false;
            //f_master_menu->show();
            *a_cur_menu = f_master_menu;
            f_show_needed = irs_true;
            //show();
          }
          break;
        }
      default:
        {
          mp_disp_drv->clear();
          for (size_t i = 0;
            ((i < f_parametr_count) && (i < mp_disp_drv->get_height()));
            i++)
          {
            if ((f_want_redraw) || (!f_parametr_array[i]->can_edit()))
            {
              f_parametr_array[i]->get_parametr_string(mp_lcd_string);
              mp_disp_drv->outtextpos(0,i,mp_lcd_string);
            }
          }
          if (f_creep != IRS_NULL)
          {
            f_creep->shift();
            mxdisp_pos_t x_pos = 0;
            mxdisp_pos_t y_pos = mp_disp_drv->get_height() - 1;
            mp_disp_drv->outtextpos(x_pos, y_pos, f_creep->get_line());
          }
          else mp_disp_drv->clear_line(mp_disp_drv->get_height() - 1);
          break;
        }
      }
      break;
    }
    case ims_edit:
    {
      *a_cur_menu = f_slave_menu;
      f_show_needed = irs_true;
      break;
    }
    case ims_hide:
    {
      break;
    }
  }
}

//-----------------------------  ADVANCED TABLO  -------------------------------

irs_advanced_tablo_t::irs_advanced_tablo_t():
  mp_slave_menu(IRS_NULL),
  mp_lcd_string(IRS_NULL),
  m_parametr_vector(),
  m_creep_stopped(false),
  m_updated_item(0)
{
  f_state = ims_hide;
  f_creep = IRS_NULL;
}

irs_advanced_tablo_t::~irs_advanced_tablo_t()
{
  IRS_ARDELETE(mp_lcd_string);
}

void irs_advanced_tablo_t::set_disp_drv(mxdisplay_drv_service_t *ap_disp_drv)
{
  irs_menu_base_t::set_disp_drv(ap_disp_drv);
  if (mp_disp_drv) {
    mp_lcd_string = new char[mp_disp_drv->get_width() + 1];
    memset((void *)mp_lcd_string, 0, mp_disp_drv->get_width() + 1);
  }
}

bool irs_advanced_tablo_t::add(irs_menu_base_t *ap_parametr,
  size_t a_x, size_t a_y, irs_menu_param_show_mode_t a_show_mode)
{
  if (a_x > mp_disp_drv->get_width() - 1) return false;
  if (a_y > mp_disp_drv->get_height() - 1) return false;

  if (f_key_event) ap_parametr->set_key_event(f_key_event);
  if (f_creep) ap_parametr->set_creep(f_creep);
  if (mp_disp_drv) ap_parametr->set_disp_drv(mp_disp_drv);
  ap_parametr->set_cursor_symbol(f_cursor_symbol);

  tablo_item_t new_item;
  new_item.p_parametr = ap_parametr;
  new_item.x = a_x;
  new_item.y = a_y;
  new_item.show_mode = a_show_mode;
  new_item.is_hidden = false;
  m_parametr_vector.push_back(new_item);
  return true;
}

void irs_advanced_tablo_t::set_slave_menu(irs_menu_base_t *ap_slave_menu)
{
  if (f_key_event) ap_slave_menu->set_key_event(f_key_event);
  if (f_creep) ap_slave_menu->set_creep(f_creep);
  if (mp_disp_drv) ap_slave_menu->set_disp_drv(mp_disp_drv);
  ap_slave_menu->set_cursor_symbol(f_cursor_symbol);
  mp_slave_menu = ap_slave_menu;
  mp_slave_menu->set_master_menu(this);
}

irs_menu_base_t::size_type irs_advanced_tablo_t::get_parametr_string(
  char *a_parametr_string,
  irs_menu_base_t::size_type /*a_length*/,
  irs_menu_param_show_mode_t /*a_show_mode*/,
  irs_menu_param_update_t /*a_update*/)
{
  if (a_parametr_string)
  {
    strcpy(a_parametr_string, empty_str);
  }
  return 0;
}

irs_menu_base_t::size_type irs_advanced_tablo_t::get_dynamic_string(char *a_buffer,
  irs_menu_base_t::size_type /*a_length*/)
{
  a_buffer = a_buffer;
  return 0;
}

void irs_advanced_tablo_t::draw(irs_menu_base_t **a_cur_menu)
{
  if (f_show_needed) {
    f_show_needed = irs_false;
    show();
  }
  irskey_t a_key = irskey_none;
  if (f_key_event) a_key = f_key_event->check();
  switch (f_state)
  {
    case ims_show:
    {
      switch(a_key)
      {
        case irskey_enter:
        {
          if (mp_slave_menu != IRS_NULL)
          {
            f_want_redraw = irs_false;
            *a_cur_menu = mp_slave_menu;
            f_show_needed = irs_true;
          }
          break;
        }
        case irskey_escape:
        {
          if (f_master_menu != IRS_NULL)
          {
            f_want_redraw = irs_false;
            *a_cur_menu = f_master_menu;
            f_show_needed = irs_true;
          }
          break;
        }
        default:
        {
          mp_disp_drv->clear();
          mxdisp_pos_t creep_y_pos = mp_disp_drv->get_height() - 1;
          bool one_updated = false;
          irs_menu_param_update_t update_current_item = IMU_NO_UPDATE;
          for (size_t i = 0; i < m_parametr_vector.size(); i++)
          {
            if (m_parametr_vector[i].is_hidden) continue;
            update_current_item = IMU_NO_UPDATE;
            if ((m_parametr_vector[i].y < creep_y_pos) || m_creep_stopped)
            {
              if (i >= m_updated_item)
              {
                if (!one_updated)
                {
                  if (m_parametr_vector[i].p_parametr->is_updated())
                  {
                    one_updated = true;
                    update_current_item = IMU_UPDATE;
                    m_updated_item = i+1;
                    if (m_updated_item >= m_parametr_vector.size())
                      m_updated_item = 0;
                  }
                }
              }

              m_parametr_vector[i].p_parametr->get_parametr_string(
                mp_lcd_string,
                mp_disp_drv->get_width(),
                m_parametr_vector[i].show_mode,
                update_current_item);
              mp_disp_drv->outtextpos(
                m_parametr_vector[i].x, m_parametr_vector[i].y, mp_lcd_string);
            }
          }
          if (!one_updated) m_updated_item = 0;

          if (!m_creep_stopped)
          {
            if (f_creep != IRS_NULL)
            {
              f_creep->shift();
              mp_disp_drv->outtextpos(0, creep_y_pos, f_creep->get_line());
            }
            else mp_disp_drv->clear_line(creep_y_pos);
          }
          break;
        }
      }
      break;
    }
    case ims_edit:
    {
      *a_cur_menu = mp_slave_menu;
      f_show_needed = irs_true;
      break;
    }
    case ims_hide:
    {
      break;
    }
  }
}

void irs_advanced_tablo_t::creep_start()
{
  m_creep_stopped = false;
}

void irs_advanced_tablo_t::creep_stop()
{
  m_creep_stopped = true;
}

bool irs_advanced_tablo_t::creep_stopped()
{
  return m_creep_stopped;
}

irs_menu_base_t::size_type irs_advanced_tablo_t::get_last_item_number()
{
  if (m_parametr_vector.size() > 0) return m_parametr_vector.size() - 1;
  return 0;
}

void irs_advanced_tablo_t::hide_item(size_type a_item_number)
{
  if (a_item_number < m_parametr_vector.size())
  {
    m_parametr_vector[a_item_number].is_hidden = true;
  }
}

void irs_advanced_tablo_t::show_item(size_type a_item_number)
{
  if (a_item_number < m_parametr_vector.size())
  {
    m_parametr_vector[a_item_number].is_hidden = false;
  }
}

bool irs_advanced_tablo_t::item_is_hidden(size_type a_item_number)
{
  if (a_item_number < m_parametr_vector.size())
  {
    return m_parametr_vector[a_item_number].is_hidden;
  }
  return false;
}

//------------------------------------------------------------------------------


irs_menu_trimmer_item_t::irs_menu_trimmer_item_t(double *a_parametr,
  irs_bool a_save_after_exit):
  f_min(0.0f),
  f_max(0.0f),
  f_parametr(a_parametr),
  f_value_string(empty_str),
  f_suffix(empty_str),
  f_prefix(empty_str),
  f_start_prefix(empty_str),
  f_trim_prefix(empty_str),
  f_len(0),
  f_accur(0),
  f_trim_len(0),
  f_trim_accur(0),
  f_double_trans(IRS_NULL),
  f_step_count(0),
  f_trim_value(0.0f),
  f_start_value(*a_parametr),
  f_save_after_exit(a_save_after_exit),
  f_first_raz(irs_true),
  f_extra_parametr(IRS_NULL),
  f_step_prefix(empty_str),
  f_step_len(0),
  f_step_accur(0),
  f_begin_step(1.f),
  f_step(f_begin_step)
{
  f_master_menu = IRS_NULL;
  f_state = ims_hide;
  f_message_count = 0;
  f_creep = IRS_NULL;
}

irs_menu_base_t::size_type irs_menu_trimmer_item_t::get_dynamic_string(
  char *a_buffer, irs_menu_base_t::size_type a_length)
{
  if (a_buffer)
  {
    size_type len = strlen(f_start_prefix);
    if ((a_length > 0) && (len > a_length))
    {
      len = a_length;
      memcpy(a_buffer, f_prefix, len);
      a_buffer[len] = '\0';
      return len;
    }
    memcpy(a_buffer, f_start_prefix, len);
    a_buffer[len] = ' ';
    size_type len2 = f_len;
    if ((a_length > 0) && (len + 1 + len2 > a_length))
    {
      len2 = a_length - len - 1;
      char *x = new char[len2];
      afloat_to_str(x, f_start_value, len2, f_accur);
      memcpy((void*)&a_buffer[len + 1], x, len2);
      IRS_ARDELETE(x);
      size_type real_len = len + 1 + len2;
      a_buffer[real_len] = '\0';
      return real_len;
    }
    char *x = new char[len2];
    afloat_to_str(x, f_start_value, len2, f_accur);
    memcpy((void*)&a_buffer[len + 1], x, len2);
    IRS_ARDELETE(x);
    a_buffer[len + 1 + len2] = ' ';
    size_type len3 = strlen(f_suffix);
    if ((a_length > 0) && (len + 1 + len2 + 1 + len3 > a_length))
    {
      len3 = a_length - len - 1 - len2 - 1;
      memcpy((void*)&a_buffer[len + 1 + len2 + 1], f_suffix, len3);
      size_type real_len = len + 1 + len2 + 1 + len3;
      a_buffer[real_len] = '\0';
      return real_len;
    }
    memcpy((void*)&a_buffer[len + 1 + len2 + 1], f_suffix, len3);
    if (f_extra_parametr != IRS_NULL)
    {
      size_type len4 = a_length - len - 1 - len2 - 1 - len3;
      char const* zpt = ", ";
      if (len4 > strlen(zpt))
      {
        memcpy((void*)&a_buffer[len + 1 + len2 + 1 + len3], zpt, strlen(zpt));
        len4 -= strlen(zpt);
        len4 = f_extra_parametr->get_parametr_string(
          &a_buffer[len + 1 + len2 + 1 + len3 + strlen(zpt)], len4);
        size_type real_len = len + 1 + len2 + 1 + len3 + strlen(zpt) + len4;
        a_buffer[real_len] = '\0';
        return real_len;
      }
      else
      {
        size_type real_len = len + 1 + len2 + 1 + len3;
        a_buffer[real_len] = '\0';
        return real_len;
      }
    }
    else
    {
      size_type real_len = len + 1 + len2 + 1 + len3;
      a_buffer[real_len] = '\0';
      return real_len;
    }

    /*
    a_buffer[0] = '\0';
    strcat(a_buffer, f_start_prefix);
    strcat(a_buffer, " ");
    irs_u8 x_len = (irs_u8)mp_disp_drv->get_width();
    char *x = new char[x_len + 1];
    afloat_to_str(x, f_start_value, f_len, f_accur);
    strcat(a_buffer, x);
    strcat(a_buffer, " ");
    strcat(a_buffer, f_suffix);
    if (f_extra_parametr != IRS_NULL)
    {
      strcat(a_buffer, ", ");
      f_extra_parametr->get_parametr_string(x);
      strcat(a_buffer, x);
    }
    IRS_ARDELETE(x);
    */
  }
  return 0;
}

void irs_menu_trimmer_item_t::set_extra_parametr(
  irs_menu_base_t *a_extra_parametr)
{
  f_extra_parametr = a_extra_parametr;
}

void irs_menu_trimmer_item_t::set_step(double a_begin_step, size_type a_step_count)
{
  f_begin_step = a_begin_step;
  f_step = f_begin_step;
  if (a_step_count < 10)
  {
    f_step_count = a_step_count;
  }
}

void irs_menu_trimmer_item_t::set_trans_function(double_trans_t a_double_trans)
{
  f_double_trans = a_double_trans;
}

void irs_menu_trimmer_item_t::set_max_value(float a_max_value)
{
  f_max = a_max_value;
}
void irs_menu_trimmer_item_t::set_min_value(float a_min_value)
{
  f_min = a_min_value;
}

float irs_menu_trimmer_item_t::get_min_value()
{
  return f_min;
}

void irs_menu_trimmer_item_t::set_str(char *a_value_string, char *a_prefix,
  char *a_start_prefix, char *a_trim_prefix, char *a_step_prefix,
  char *a_suffix, size_type a_len, size_type a_accur, size_type a_trim_len,
  size_type a_trim_accur, size_type a_step_len, size_type a_step_accur)
{
  f_value_string = a_value_string;
  f_len = a_len;
  f_accur = a_accur;
  f_trim_len = a_trim_len;
  f_trim_accur = a_trim_accur;
  f_step_len = a_step_len;
  f_step_accur = a_step_accur;
  f_prefix = a_prefix;
  f_start_prefix = a_start_prefix;
  f_trim_prefix = a_trim_prefix;
  f_step_prefix = a_step_prefix;
  f_suffix = a_suffix;
  //for (irs_u8 i = 0; i < f_len; f_value_string[i++] = ' ');
  memset(f_value_string, ' ', f_len);
  f_value_string[f_len] = '\0';
}

void irs_menu_trimmer_item_t::reset_str()
{
  //for (irs_u8 i = 0; i < f_len; f_value_string[i++] = ' ');
  memset(f_value_string, ' ', f_len);
  f_value_string[f_len] = '\0';
}

irs_menu_base_t::size_type irs_menu_trimmer_item_t::get_parametr_string(
  char *a_parametr_string,
  irs_menu_base_t::size_type a_length,
  irs_menu_param_show_mode_t a_show_mode,
  irs_menu_param_update_t /*a_update*/)
{
  if (a_parametr_string)
  {
    size_type len = 0;
    if (a_show_mode != IMM_WITHOUT_PREFIX)
    {
      len = strlen(f_trim_prefix);
      if ((a_length > 0) && (a_length < len))
      {
        len = a_length;
        memcpy(a_parametr_string, f_trim_prefix, len);
        a_parametr_string[len] = '\0';
        return len;
      }
      a_parametr_string[len] = ' ';
    }
    afloat_to_str(f_value_string, (((*f_parametr-f_start_value)/f_start_value)*
      100.), f_trim_len, f_trim_accur);
    size_type len2 = strlen(f_value_string);
    size_type start_index = len + 1;
    if ((a_length > 0) && (start_index + len2 > a_length))
    {
      len2 = a_length - start_index;
      memcpy((void*)&a_parametr_string[start_index], f_value_string, len2);
      size_type real_len = start_index + len2;

      a_parametr_string[real_len] = '\0';
      return real_len;
    }
    memcpy((void*)&a_parametr_string[start_index], f_value_string, len2);

    size_type real_len = start_index + len2;

    if (a_show_mode != IMM_WITHOUT_SUFFIX)
    {
      char const* prc = " %";
      if (real_len + strlen(prc) < a_length)
      {
        memcpy((void*)&a_parametr_string[real_len], f_value_string, strlen(prc));
        real_len += strlen(prc);
        a_parametr_string[real_len] = '\0';
        return real_len;
      }
    }
    a_parametr_string[real_len] = '\0';
    return real_len;
  }
  return 0;

  /*
  irs_u8 len = irs_u8(strlen(f_trim_prefix));
  for (irs_u8 i = 0; i < len; i++)
  {
    temp[i] = f_trim_prefix[i];
  }
  temp[len] = ' ';
  afloat_to_str(f_value_string, (((*f_parametr-f_start_value)/f_start_value)*
    100.), f_trim_len, f_trim_accur);
  irs_u8 len2 = irs_u8(strlen(f_value_string));
  for (irs_u8 i = 0; i < len2; i++)
  {
    temp[i+len+1] = f_value_string[i];
  }
  temp[len+len2+1] = ' ';
  temp[len+len2+2] = '%';
  temp[len+len2+3] = '\0';
  */
}

void irs_menu_trimmer_item_t::put_step_string()
{
  const size_type space = 1;
  const size_type prf_x_pos = 0;
  const size_type val_x_pos = prf_x_pos + strlen(f_step_prefix) + space;
  const size_type suf_x_pos = val_x_pos + strlen(f_value_string) + space;
  const size_type y_pos = 2;

  afloat_to_str(f_value_string, f_step, f_step_len, f_step_accur);

  mp_disp_drv->clear_line(y_pos);
  mp_disp_drv->outtextpos(prf_x_pos, y_pos, f_step_prefix);
  mp_disp_drv->outtextpos(val_x_pos, y_pos, f_value_string);
  mp_disp_drv->outtextpos(suf_x_pos, y_pos, f_suffix);
}

void irs_menu_trimmer_item_t::draw(irs_menu_base_t **a_cur_menu)
{
  if (f_show_needed) {
    f_show_needed = irs_false;
    show();
  }
  irskey_t a_key = irskey_none;
  if (f_key_event) a_key = f_key_event->check();
  switch (f_state)
  {
    case ims_hide:
    {
      break;
    }
    case ims_edit:
    {
      break;
    }
    case ims_show:
    {
      if (f_first_raz)
      {
        f_start_value = *f_parametr;
        mp_disp_drv->clear();
        afloat_to_str(f_value_string, f_step, f_step_len, f_step_accur);

        const size_type space = 1;
        const size_type prf_x_pos = 0;
        const size_type val_x_pos = strlen(f_step_prefix) + space;
        const size_type suf_x_pos = val_x_pos + strlen(f_value_string) + space;
        const size_type y_pos = 2;

        mp_disp_drv->outtextpos(prf_x_pos, y_pos, f_step_prefix);
        mp_disp_drv->outtextpos(val_x_pos, y_pos, f_value_string);
        mp_disp_drv->outtextpos(suf_x_pos, y_pos, f_suffix);
        f_first_raz = irs_false;
      }

      const size_type y_pos = 0;
      mp_disp_drv->clear_line(0);
      afloat_to_str(f_value_string, *f_parametr, f_len, f_accur);
      const size_type space = 1;
      const size_type prf_x_pos = 0;
      const size_type val_x_pos = prf_x_pos + strlen(f_prefix) + space;
      const size_type suf_x_pos = val_x_pos + strlen(f_value_string) + space;
      mp_disp_drv->outtextpos(prf_x_pos, y_pos, f_prefix);
      mp_disp_drv->outtextpos(val_x_pos, y_pos, f_value_string);
      mp_disp_drv->outtextpos(suf_x_pos, y_pos, f_suffix);
      mp_disp_drv->clear_line(1);
      if (f_start_value != 0.0)
      {
        afloat_to_str(f_value_string, (((*f_parametr-f_start_value)/
          f_start_value)*100.), f_trim_len, f_trim_accur);
      }
      if ((f_start_value == 0.0) && (*f_parametr != 0.0))
      {
        afloat_to_str(f_value_string, (((*f_parametr-f_start_value)/
          (*f_parametr))*100.), f_trim_len, f_trim_accur);
      }
      if ((f_start_value == 0.0) && (*f_parametr == 0.0))
      {
        //afloat_to_str(f_value_string, (((*f_parametr-f_start_value)/
          // *f_parametr)*100.), f_trim_len, f_trim_accur);
        f_value_string = zero_str;
      }

      const size_type trim_prf_x_pos
        = 0;
      const size_type trim_val_x_pos
        = trim_prf_x_pos + strlen(f_trim_prefix) + space;
      const size_type trim_suf_x_pos
        = trim_val_x_pos + strlen(f_value_string) + space;
      const size_type trim_y_pos = 1;
      mp_disp_drv->outtextpos(trim_prf_x_pos, trim_y_pos, f_trim_prefix);
      mp_disp_drv->outtextpos(trim_val_x_pos, trim_y_pos, f_value_string);
      mp_disp_drv->outtextpos(trim_suf_x_pos, trim_y_pos, "%");
      if (f_want_redraw)
      {
        if (f_double_trans) f_double_trans(f_parametr);
        if (mp_event) mp_event->exec();
        f_want_redraw = irs_false;
      }
      if (f_creep != IRS_NULL)
      {
        if (f_creep->eol())
        {
          f_creep->soft_clear_dynamic();
          get_dynamic_string(f_creep->get_creep_buffer());
        }
        f_creep->shift();
        const size_type creep_x_pos = 0;
        const size_type creep_y_pos = mp_disp_drv->get_height() - 1;
        mp_disp_drv->outtextpos(creep_x_pos, creep_y_pos, f_creep->get_line());
      }
      else mp_disp_drv->clear_line(mp_disp_drv->get_height() - 1);

      switch(a_key)
      {
        case irskey_enter:
        {
          //hide();
          *a_cur_menu = f_master_menu;
          f_show_needed = irs_true;
          //f_master_menu->show();
          if (f_double_trans) f_double_trans(f_parametr);
          if (mp_event) mp_event->exec();
          f_first_raz = irs_true;
          break;
        }
        case irskey_escape:
        {
          //hide();
          *a_cur_menu = f_master_menu;
          f_show_needed = irs_true;
          //f_master_menu->show();
          *f_parametr = f_start_value;
          if (f_double_trans) f_double_trans(&f_start_value);
          if (mp_event) mp_event->exec();
          f_first_raz = irs_true;
          break;
        }
        case irskey_up:
        {
          *f_parametr += f_step;
          if (*f_parametr > f_max)
          {
            *f_parametr = f_max;
          }
          f_want_redraw = irs_true;
          break;
        }
        case irskey_down:
        {
          *f_parametr -= f_step;
          if (*f_parametr < f_min)
          {
            *f_parametr = f_min;
          }
          f_want_redraw = irs_true;
          break;
        }
        case irskey_0:
        {
          f_step = f_begin_step;
          put_step_string();
          f_want_redraw = irs_true;
          break;
        }
        case irskey_1:
        {
          if (f_step_count >= 1)
          {
            f_step = f_begin_step*1e1;
            put_step_string();
            f_want_redraw = irs_true;
          }
          break;
        }
        case irskey_2:
        {
          if (f_step_count >= 2)
          {
            f_step = f_begin_step*1e2;
            put_step_string();
            f_want_redraw = irs_true;
          }
          break;
        }
        case irskey_3:
        {
          if (f_step_count >= 3)
          {
            f_step = f_begin_step*1e3;
            put_step_string();
            f_want_redraw = irs_true;
          }
          break;
        }
        case irskey_4:
        {
          if (f_step_count >= 4)
          {
            f_step = f_begin_step*1e4;
            put_step_string();
            f_want_redraw = irs_true;
          }
          break;
        }
        case irskey_5:
        {
          if (f_step_count >= 5)
          {
            f_step = f_begin_step*1e5;
            put_step_string();
            f_want_redraw = irs_true;
          }
          break;
        }
        case irskey_6:
        {
          if (f_step_count >= 6)
          {
            f_step = f_begin_step*1e6;
            put_step_string();
            f_want_redraw = irs_true;
          }
          break;
        }
        case irskey_7:
        {
          if (f_step_count >= 7)
          {
            f_step = f_begin_step*1e7;
            put_step_string();
            f_want_redraw = irs_true;
          }
          break;
        }
        case irskey_8:
        {
          if (f_step_count >= 8)
          {
            f_step = f_begin_step*1e8;
            put_step_string();
            f_want_redraw = irs_true;
          }
          break;
        }
        case irskey_9:
        {
          if (f_step_count >= 9)
          {
            f_step = f_begin_step*1e9;
            put_step_string();
            f_want_redraw = irs_true;
          }
          break;
        }
      }
      break;
    }
  }
}

irs_menu_base_t::size_type irs_menu_trimmer_item_t::get_step_string(
  char *a_string, irs_menu_base_t::size_type a_length)
{
  if (a_string)
  {
    size_type buffer_len = 1 + strlen(f_start_prefix) + 1;
    buffer_len += (f_len + 1);
    buffer_len += (strlen(f_suffix) + 1 + strlen(f_step_prefix));
    buffer_len += (1 + f_step_len + 1 + strlen(f_suffix));
    char *buffer = new char[buffer_len + 1];
    if (!buffer) return 0;
    buffer[0] = ' ';
    buffer[1] = '\0';
    strcat(buffer, f_start_prefix);
    strcat(buffer, " ");
    char *x = new char[f_len+1];
    afloat_to_str(x, f_start_value, f_len, f_accur);
    strcat(buffer, x);
    strcat(buffer, " ");
    strcat(buffer, f_suffix);
    strcat(buffer, ",");
    strcat(buffer, f_step_prefix);
    strcat(buffer, " ");
    afloat_to_str(x, f_step, f_step_len, f_step_accur);
    strcat(buffer, x);
    strcat(buffer, " ");
    strcat(buffer, f_suffix);
    if (buffer_len > a_length) buffer_len = a_length;
    memcpy(a_string, buffer, buffer_len);
    a_string[buffer_len] = '\0';
    IRS_ARDELETE(x);
    IRS_ARDELETE(buffer);
    return buffer_len;
  }
  return 0;
}

irs_menu_ip_item_t::irs_menu_ip_item_t(irs_u8 *a_parametr, char *a_value_string,
  irs_bool a_can_edit):
  f_value_string(a_value_string),
  f_parametr(a_parametr),
  f_ip_trans(IRS_NULL),
  f_extra_parametr(IRS_NULL)
{
  f_master_menu = IRS_NULL;
  f_cur_symbol = 0;
  f_cursor[0] = ' ';
  f_cursor[1] = '\0';
  f_can_edit = a_can_edit;
  f_creep = IRS_NULL;
  set_to_cnt(f_blink_counter, MS_TO_CNT(CUR_BLINK));
}

const char *irs_menu_ip_item_t::get_messge()
{
  return f_message;
}

void irs_menu_ip_item_t::reset_str()
{
  const size_type end_symbol = 15;
  //for (irs_u8 i = 0; i < 15; f_value_string[i++] = ' ');
  memset(f_value_string, ' ', end_symbol);
  f_value_string[end_symbol] = '\0';
}

void irs_menu_ip_item_t::set_trans_function(ip_trans_t a_ip_trans)
{
  f_ip_trans = a_ip_trans;
}
void irs_menu_ip_item_t::set_extra_parametr(irs_menu_base_t *a_extra_parametr)
{
  f_extra_parametr = a_extra_parametr;
}

irs_menu_base_t::size_type irs_menu_ip_item_t::get_parametr_string(
  char *a_parametr_string,
  irs_menu_base_t::size_type a_length,
  irs_menu_param_show_mode_t /*a_show_mode*/,
  irs_menu_param_update_t /*a_update*/)
{
  if (a_parametr_string)
  {
    char ip_string[16];
    mxip_to_cstr(ip_string, *(mxip_t *)&f_parametr);
    size_type real_len = strlen(ip_string);
    if ((a_length > 0) && (real_len > a_length)) real_len = a_length;
    memcpy(a_parametr_string, ip_string, real_len);
    a_parametr_string[real_len] = '\0';
    return real_len;
  }
  return 0;
}

irs_menu_base_t::size_type irs_menu_ip_item_t::get_dynamic_string(
  char *a_string, irs_menu_base_t::size_type a_length)
{
  if (a_string)
  {
    static const size_type x_len = 32;
    char x[x_len];
    x[0] = '\0';
    if (f_extra_parametr == IRS_NULL)
    {
      strcat(x, get_header());
      strcat(x, " ");
      size_t astr_len = strlen(x);
      mxip_to_cstr(x + astr_len, *(mxip_t *)&f_parametr);
    }
    else
    {
      f_extra_parametr->get_parametr_string(x);
    }
    size_type real_len = strlen(x);
    if (real_len > a_length) real_len = a_length;
    memcpy(a_string, x, real_len);
    a_string[real_len] = '\0';
    return real_len;
  }
  return 0;
}

void irs_menu_ip_item_t::draw(irs_menu_base_t **a_cur_menu)
{
  if (f_show_needed) {
    f_show_needed = irs_false;
    show();
  }
  irskey_t a_key = irskey_none;
  if (f_key_event) a_key = f_key_event->check();
  switch (f_state)
  {
  case ims_show:
    {
      switch(a_key)
      {
      case irskey_enter:
        {
          if (f_can_edit)
          {
            reset_str();
            f_want_redraw = irs_true;
            set_to_cnt(f_blink_counter,MS_TO_CNT(CUR_BLINK));
            f_cur_symbol = 0;
            mp_disp_drv->clear_line(EDIT_LINE);
            edit();
          }
          break;
        }
      case irskey_escape:
        {
          //hide();
          *a_cur_menu = f_master_menu;
          f_show_needed = irs_true;
          break;
        }
      default:
        {
          if (f_want_redraw || (!f_can_edit))
          {
            mp_disp_drv->clear();
            mp_disp_drv->outtextpos(0,0,f_header);
            mxip_to_cstr(f_value_string, *(mxip_t *)&f_parametr);
            mp_disp_drv->outtextpos(0, 1, f_value_string);
            f_want_redraw = irs_false;
          }
          if (f_creep != IRS_NULL)
          {
            if (f_creep->eol())
            {
              f_creep->soft_clear_dynamic();
              get_dynamic_string(f_creep->get_creep_buffer());
            }
            f_creep->shift();
            mp_disp_drv->outtextpos(0, mp_disp_drv->get_height() - 1,
              f_creep->get_line());
          }
          else mp_disp_drv->clear_line(irs_u8(mp_disp_drv->get_height() - 1));
          break;
        }
      }
      break;
    }
  case ims_edit:
    {
      if (test_to_cnt(f_blink_counter))
      {
        f_cursor[0] = (f_cursor[0] == f_cursor_symbol) ? ' ' : f_cursor_symbol;
        set_to_cnt(f_blink_counter, MS_TO_CNT(CUR_BLINK));
        mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, f_cursor);
      }
      if (a_key == irskey_backspace)
      {
        if (f_cur_symbol > 0)
        {
          f_value_string[f_cur_symbol] = ' ';
          mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, " ");
          f_cur_symbol--;
        }
        else show();
      }
      if (a_key == irskey_enter)
      {
        f_value_string[f_cur_symbol] = '\0';
        if (f_cur_symbol > 0)
        {
          if (!cstr_to_mxip(*(mxip_t *)&f_parametr, f_value_string)) {
            *(mxip_t *)&f_parametr = zero_ip;
          }
          if (f_ip_trans) f_ip_trans(f_parametr);
          if (mp_event) mp_event->exec();
        }
        show();
      }
      if (f_creep != IRS_NULL)
      {
        if (f_creep->eol())
        {
          f_creep->soft_clear_dynamic();
          get_dynamic_string(f_creep->get_creep_buffer());
        }
        f_creep->shift();
        mp_disp_drv->outtextpos(0, mp_disp_drv->get_height() - 1,
          f_creep->get_line());
      }
      else mp_disp_drv->clear_line(mp_disp_drv->get_height() - 1);
      if (f_cur_symbol < 15)
      {
        switch(a_key)
        {
        case irskey_1:
          {
            f_value_string[f_cur_symbol] = '1';
            mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "1");
            f_cur_symbol++;
            break;
          }
        case irskey_2:
          {
            f_value_string[f_cur_symbol] = '2';
            mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "2");
            f_cur_symbol++;
            break;
          }
        case irskey_3:
          {
            f_value_string[f_cur_symbol] = '3';
            mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "3");
            f_cur_symbol++;
            break;
          }
        case irskey_4:
          {
            f_value_string[f_cur_symbol] = '4';
            mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "4");
            f_cur_symbol++;
            break;
          }
        case irskey_5:
          {
            f_value_string[f_cur_symbol] = '5';
            mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "5");
            f_cur_symbol++;
            break;
          }
        case irskey_6:
          {
            f_value_string[f_cur_symbol] = '6';
            mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "6");
            f_cur_symbol++;
            break;
          }
        case irskey_7:
          {
            f_value_string[f_cur_symbol] = '7';
            mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "7");
            f_cur_symbol++;
            break;
          }
        case irskey_8:
          {
            f_value_string[f_cur_symbol] = '8';
            mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "8");
            f_cur_symbol++;
            break;
          }
        case irskey_9:
          {
            f_value_string[f_cur_symbol] = '9';
            mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "9");
            f_cur_symbol++;
            break;
          }
        case irskey_0:
          {
            f_value_string[f_cur_symbol] = '0';
            mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, "0");
            f_cur_symbol++;
            break;
          }
        case irskey_point:
          {
            f_value_string[f_cur_symbol] = '.';
            mp_disp_drv->outtextpos(f_cur_symbol, EDIT_LINE, ".");
            f_cur_symbol++;
            break;
          }
        }
      }
      break;
    }
  }
}
//---------------------------------------------------------------------------
irs_menu_bool_item_t::irs_menu_bool_item_t(irs_bool *a_parametr,
  irs_bool a_can_edit):
  f_parametr(a_parametr),
  f_true_string(empty_str),
  f_false_string(empty_str),
  f_temp_parametr(*a_parametr),
  f_bool_trans(IRS_NULL)
{
  f_master_menu = IRS_NULL;
  f_can_edit = a_can_edit;
  f_creep = IRS_NULL;
}

void irs_menu_bool_item_t::set_trans_function(bool_trans_t a_bool_trans)
{
  f_bool_trans = a_bool_trans;
}

void irs_menu_bool_item_t::set_str(char *a_true_string, char *a_false_string)
{
  f_true_string = a_true_string;
  f_false_string = a_false_string;
}

irs_bool *irs_menu_bool_item_t::get_parametr()
{
  return f_parametr;
}

irs_menu_base_t::size_type irs_menu_bool_item_t::get_parametr_string(
  char *a_parametr_string,
  irs_menu_base_t::size_type a_length,
  irs_menu_param_show_mode_t a_show_mode,
  irs_menu_param_update_t /*a_update*/)
{
  if (a_parametr_string)
  {
    char const* dstr = " : ";
    if (a_length > 0)
    {
      size_type buffer_len = 1;
      if (a_show_mode != IMM_WITHOUT_PREFIX)
      {
        buffer_len += strlen(f_header) + strlen(dstr);
      }
      if (*f_parametr == irs_true)
      {
        buffer_len += strlen(f_true_string);
      }
      else buffer_len += strlen(f_false_string);

      char *buffer = new char[buffer_len];
      buffer[0] = '\0';
      if (a_show_mode != IMM_WITHOUT_PREFIX)
      {
        strcat(buffer, f_header);
        strcat(buffer, dstr);
      }
      if (*f_parametr == irs_true)
      {
        strcat(buffer, f_true_string);
      }
      else strcat(buffer, f_false_string);
      if (buffer_len > a_length) buffer_len = a_length;
      memcpy(a_parametr_string, buffer, buffer_len);
      a_parametr_string[buffer_len] = '\0';
      delete []buffer;
      return buffer_len;
    }
    else
    {
      a_parametr_string[0] = '\0';
      if (a_show_mode != IMM_WITHOUT_SUFFIX)
      {
        strcat(a_parametr_string, f_header);
        strcat(a_parametr_string, " : ");
      }
      if (*f_parametr == irs_true)
      {
        strcat(a_parametr_string, f_true_string);
      }
      else strcat(a_parametr_string, f_false_string);
      return strlen(a_parametr_string);
    }
  }
  return 0;
}

irs_menu_base_t::size_type irs_menu_bool_item_t::get_dynamic_string(char *a_buffer,
  irs_menu_base_t::size_type /*a_length*/)
{
  a_buffer = a_buffer;
  return 0;
}

void irs_menu_bool_item_t::draw(irs_menu_base_t **a_cur_menu)
{
  if (f_show_needed) {
    f_show_needed = irs_false;
    show();
  }
  irskey_t a_key = irskey_none;
  if (f_key_event) a_key = f_key_event->check();
  switch (f_state)
  {
  case ims_show:
    {
      if (f_can_edit)
      {
        mp_disp_drv->clear();
        mp_disp_drv->outtextpos(0,0,f_header);
        mp_disp_drv->outtextpos(0, 1, "^~");
        f_temp_parametr = *f_parametr;
        f_want_redraw = irs_false;
        edit();
      }
      else
      {
        switch(a_key)
        {
        case irskey_enter:
          {
            if (f_can_edit)
            {
              f_want_redraw = irs_true;
              mp_disp_drv->clear_line(EDIT_LINE);
              edit();
              f_temp_parametr = *f_parametr;
            }
            break;
          }
        case irskey_escape:
          {
            //hide();
            *a_cur_menu = f_master_menu;
            f_show_needed = irs_true;
            //f_master_menu->show();
            break;
          }
        default:
          {
            if (f_want_redraw || (!f_can_edit))
            {
              mp_disp_drv->clear();
              mp_disp_drv->outtextpos(0,0,f_header);
              if (*f_parametr == irs_true)
              {
                mp_disp_drv->outtextpos(0,1,f_true_string);
              }
              else mp_disp_drv->outtextpos(0,1,f_false_string);
              f_want_redraw = irs_false;
            }
            if (f_creep != IRS_NULL)
            {
              f_creep->shift();
              mp_disp_drv->outtextpos(0, mp_disp_drv->get_height() - 1,
                f_creep->get_line());
            }
            else mp_disp_drv->clear_line(mp_disp_drv->get_height() - 1);
            break;
          }
        }
      }
      break;
    }
  case ims_edit:
    {
      mp_disp_drv->clear_line(1);
      mp_disp_drv->clear_line(EDIT_LINE);
      mp_disp_drv->outtextpos(0, 1, "^~");
      if (*f_parametr == irs_true)
        mp_disp_drv->outtextpos(3,1,f_true_string);
      else
        mp_disp_drv->outtextpos(3,1,f_false_string);
      if (f_temp_parametr == irs_true)
        mp_disp_drv->outtextpos(3,2,f_true_string);
      else
        mp_disp_drv->outtextpos(3,2,f_false_string);
      if (a_key == irskey_escape)
      {
        //show();
        *a_cur_menu = f_master_menu;
        f_show_needed = irs_true;
        //f_master_menu->show();
      }
      if (a_key == irskey_enter)
      {
        *f_parametr = f_temp_parametr;
        if (f_bool_trans) f_bool_trans(f_parametr);
        if (mp_event) mp_event->exec();
        //show();
        *a_cur_menu = f_master_menu;
        f_show_needed = irs_true;
        //f_master_menu->show();
      }
      if (a_key == irskey_up)
      {
        if (f_temp_parametr == irs_true)
        {
          f_temp_parametr = irs_false;
        }
        else f_temp_parametr = irs_true;
      }
      if (a_key == irskey_down)
      {
        if (f_temp_parametr == irs_true)
        {
          f_temp_parametr = irs_false;
        }
        else f_temp_parametr = irs_true;
      }
      if (f_creep != IRS_NULL)
      {
        f_creep->shift();
        mp_disp_drv->outtextpos(0, mp_disp_drv->get_height() - 1,
          f_creep->get_line());
      }
      else mp_disp_drv->clear_line(mp_disp_drv->get_height() - 1);

      break;
    }
  }
}

//-------------------------------------------------------------------------
// Класс строкового пункта меню предназначеный только для irs_menu_tablo_t

irs_menu_string_item_t::irs_menu_string_item_t():
  mp_string(IRS_NULL)
{
}
irs_menu_string_item_t::~irs_menu_string_item_t()
{
}
void irs_menu_string_item_t::set_parametr_string(char *ap_parametr_string)
{
  m_updated = true;
  mp_string = ap_parametr_string;
}
irs_menu_base_t::size_type irs_menu_string_item_t::get_parametr_string(
  char *ap_parametr_string,
  irs_menu_base_t::size_type /*a_length*/,
  irs_menu_param_show_mode_t /*a_show_mode*/,
  irs_menu_param_update_t /*a_update*/)
{
  if (ap_parametr_string)
  if (mp_string) {
    size_t parstr_len = (size_t)mp_disp_drv->get_width();
    memcpy((void *)ap_parametr_string, (void *)mp_string, parstr_len);
    ap_parametr_string[parstr_len] = 0;
    return (parstr_len);
  }
  return 0;
}
irs_menu_base_t::size_type irs_menu_string_item_t::get_dynamic_string(char *ap_buffer,
  irs_menu_base_t::size_type /*a_length*/)
{
  ap_buffer = ap_buffer;
  return 0;
}
void irs_menu_string_item_t::draw(irs_menu_base_t **ap_cur_menu)
{
  *ap_cur_menu = f_master_menu;
}

//-----------------------------  PROGRESS BAR  ---------------------------------

irs_menu_progress_bar_t::irs_menu_progress_bar_t(
  irs_menu_base_t::size_type a_symbol,
  irs_menu_base_t::size_type /*a_length*/,
  float a_max_value):

  mp_bar(0),
  m_symbol(a_symbol),
  m_length(0),
  m_progress(0),
  m_max_value(a_max_value),
  m_value(0.f)
{
  mp_bar = new char[m_length + 1];
  memset(mp_bar, ' ', m_length);
  mp_bar[m_length] = '\0';
}

void irs_menu_progress_bar_t::fill_bar()
{
  if ((m_max_value > 0.f) && (m_length > 0))
  {
    m_progress = static_cast<irs_u8>(m_length * irs_u8(m_value / m_max_value));
    memset(mp_bar, m_symbol, m_progress);
    memset(&mp_bar[m_progress], ' ', m_length - m_progress);
    mp_bar[m_length] = '\0';
  }
}

irs_menu_progress_bar_t::~irs_menu_progress_bar_t()
{
  delete []mp_bar;
}

void irs_menu_progress_bar_t::set_length(irs_menu_base_t::size_type a_length)
{
  if (a_length != m_length)
  {
    m_length = a_length;
    delete []mp_bar;
    mp_bar = new char[m_length + 1];
    fill_bar();
  }
}

void irs_menu_progress_bar_t::set_value(float a_value)
{
  if (a_value < m_max_value)
  {
    m_value = a_value;
    fill_bar();
  }
}

void irs_menu_progress_bar_t::set_max_value(float a_max_value)
{
  if (a_max_value >= 0)
  {
    m_max_value = a_max_value;
    if (m_value > m_max_value) m_value = m_max_value;
    fill_bar();
  }
}

irs_menu_base_t::size_type irs_menu_progress_bar_t::get_parametr_string(
  char *a_parametr_string,
  irs_menu_base_t::size_type a_length,
  irs_menu_param_show_mode_t /*a_show_mode*/,
  irs_menu_param_update_t /*a_update*/)
{
  if ((a_length > 0) && (a_length < m_length))
  {
    memcpy(a_parametr_string, mp_bar, a_length);
    a_parametr_string[a_length] = '\0';
    return a_length;
  }
  else
  {
    strcpy(a_parametr_string, mp_bar);
    return m_length;
  }
}

irs_menu_base_t::size_type irs_menu_progress_bar_t::get_dynamic_string(char *ap_buffer,
  irs_menu_base_t::size_type a_length)
{
  if (a_length > 0) strcpy(ap_buffer, empty_str);
  return 0;
}

void irs_menu_progress_bar_t::draw(irs_menu_base_t **ap_cur_menu)
{
  *ap_cur_menu = f_master_menu;
}

//---------------------  2 PARAMETER MASTER  -----------------------------------

irs_menu_2param_master_item_t::irs_menu_2param_master_item_t(
    param_t &a_param_1, param_t &a_param_2, param_t &a_out_param):
  mp_value_string(IRS_NULL),
  m_len(0),
  m_accur(0),
  mp_prefix_1(IRS_NULL),
  mp_suffix_1(IRS_NULL),
  m_show_mode_1(IMM_FULL),
  m_show_mode_2(IMM_FULL),
  m_show_mode_out(IMM_FULL),
  mp_prefix_2(IRS_NULL),
  mp_suffix_2(IRS_NULL),
  mp_prefix_out(IRS_NULL),
  mp_suffix_out(IRS_NULL),
  m_edit_status(OFF),
  m_current_point(0),
  m_point_vector(),
  m_step(0.),
  m_param_1(a_param_1),
  m_param_2(a_param_2),
  m_out_param(a_out_param),
  m_user_value_1(0.),
  m_user_value_2(0.),
  m_min_out_value(0.),
  m_max_out_value(0.),
  m_old_value(0.),
  m_was_apply_point(false),
  m_last_point(0),
  m_need_process_change(false),
  m_need_process_start(false),
  m_need_process_stop(false),
  m_need_out_param_change(false),
  m_started(false),
  m_prev_started(false),
  m_operating_duty(false),
  m_prev_operating_duty(false),
  m_entered(false),
  m_start_key(irskey_none),
  m_stop_key(irskey_none)
{
}

irs_menu_2param_master_item_t::~irs_menu_2param_master_item_t()
{
}

void irs_menu_2param_master_item_t::make_str(char *ap_prefix, char *ap_suffix,
  irs_menu_param_show_mode_t a_show_mode, param_t &a_param)
{
  size_type value_pos = 0;
  if (a_show_mode != IMM_WITHOUT_PREFIX)
  {
    strcpy(mp_value_string, ap_prefix);
    strcat(mp_value_string, " ");
    value_pos = strlen(ap_prefix) + 1;
  }
  afloat_to_str(&mp_value_string[value_pos], a_param, m_len, m_accur);
  if (a_show_mode != IMM_WITHOUT_SUFFIX)
  {
    strcat(mp_value_string, " ");
    strcat(mp_value_string, ap_suffix);
  }
}

void irs_menu_2param_master_item_t::show_point_info()
{
  const char *point_str = "Точка";
  mp_disp_drv->outtextpos(0, 0, point_str);

  const mxdisp_pos_t pos_point_num = strlen(point_str) + 1;
  afloat_to_str(mp_value_string, m_current_point + 1,
    m_max_point_str_len, 0);
  mp_disp_drv->outtextpos(pos_point_num, 0, mp_value_string);

  const char *from_str = "из";
  const mxdisp_pos_t pos_from_str = pos_point_num + strlen(mp_value_string) + 1;
  mp_disp_drv->outtextpos(pos_from_str, 0, from_str);

  const mxdisp_pos_t pos_point_cnt = pos_from_str + strlen(from_str) + 1;
  afloat_to_str(mp_value_string, m_point_vector.size(),
    m_max_point_str_len, 0);
  mp_disp_drv->outtextpos(pos_point_cnt, 0, mp_value_string);

  make_str(mp_prefix_1, mp_suffix_1,
    m_show_mode_1, m_point_vector[m_current_point].value_1);
  mp_disp_drv->outtextpos(0, 1, mp_value_string);

  const mxdisp_pos_t str_pos_x = mp_disp_drv->get_width() / 2;
  make_str(mp_prefix_2, mp_suffix_2,
    m_show_mode_2, m_point_vector[m_current_point].value_2);
  mp_disp_drv->outtextpos(str_pos_x, 1, mp_value_string);
}

void irs_menu_2param_master_item_t::show_process_status()
{
  const mxdisp_pos_t message_pos_x = mp_disp_drv->get_width() / 2;
  const mxdisp_pos_t message_pos_y = 2;
  if (!m_started)
  {
    mp_disp_drv->outtextpos(message_pos_x, message_pos_y, "Стоп");
  }
  else
  {
    if (m_operating_duty)
    {
      mp_disp_drv->outtextpos(message_pos_x, message_pos_y, "Готов");
    }
    else
    {
      mp_disp_drv->outtextpos(message_pos_x, message_pos_y, "В процессе");
    }
  }
}

void irs_menu_2param_master_item_t::set_str(char *ap_value_string,
  size_type a_len, size_type a_accur)
{
  mp_value_string = ap_value_string;
  m_len = a_len;
  m_accur = a_accur;
  memset(mp_value_string, ' ', m_len);
  mp_value_string[m_len] = '\0';
}

void irs_menu_2param_master_item_t::set_param_1_str(
  char *ap_prefix, char *ap_suffix, irs_menu_param_show_mode_t a_show_mode)
{
  mp_prefix_1 = ap_prefix;
  mp_suffix_1 = ap_suffix;
  m_show_mode_1 = a_show_mode;
}

void irs_menu_2param_master_item_t::set_param_2_str(
  char *ap_prefix, char *ap_suffix, irs_menu_param_show_mode_t a_show_mode)
{
  mp_prefix_2 = ap_prefix;
  mp_suffix_2 = ap_suffix;
  m_show_mode_2 = a_show_mode;
}

void irs_menu_2param_master_item_t::set_param_out_str(
  char *ap_prefix, char *ap_suffix, irs_menu_param_show_mode_t a_show_mode)
{
  mp_prefix_out = ap_prefix;
  mp_suffix_out = ap_suffix;
  m_show_mode_out = a_show_mode;
}

void irs_menu_2param_master_item_t::set_step(param_t a_step)
{
  m_step = a_step;
}

void irs_menu_2param_master_item_t::set_limit_values(param_t a_min_out_value,
  param_t a_max_out_value)
{
  m_min_out_value = a_min_out_value;
  m_max_out_value = a_max_out_value;
}

void irs_menu_2param_master_item_t::add_point(param_t a_value_1,
  param_t a_value_2, param_t a_out_value)
{
  if (m_point_vector.size() < m_max_point_count)
  {
    point_data_t new_data;
    new_data.value_1 = a_value_1;
    new_data.value_2 = a_value_2;
    new_data.out_value = a_out_value;
    m_point_vector.push_back(new_data);
  }
}

bool irs_menu_2param_master_item_t::get_last_point(point_cnt_t &a_last_point,
  param_t &a_point_value)
{
  a_last_point = m_last_point;
  a_point_value = m_point_vector[m_last_point].out_value;
  bool was_apply = m_was_apply_point;
  m_was_apply_point = false;
  return was_apply;
}

void irs_menu_2param_master_item_t::set_operating_duty(bool a_operating_duty)
{
  m_operating_duty = a_operating_duty;
}

void irs_menu_2param_master_item_t::set_start_key(irskey_t a_start_key)
{
  m_start_key = a_start_key;
}

void irs_menu_2param_master_item_t::set_stop_key(irskey_t a_stop_key)
{
  m_stop_key = a_stop_key;
}

irs_menu_base_t::size_type irs_menu_2param_master_item_t::get_parametr_string(
  char */*a_parametr_string*/,
  irs_menu_base_t::size_type /*a_length*/,
  irs_menu_param_show_mode_t /*a_show_mode*/,
  irs_menu_param_update_t /*a_update*/)
{
  return 0;
}

irs_menu_base_t::size_type irs_menu_2param_master_item_t::get_dynamic_string(
  char */*ap_buffer*/,
  irs_menu_base_t::size_type /*a_length*/)
{
  return 0;
}

void irs_menu_2param_master_item_t::draw(irs_menu_base_t **ap_cur_menu)
{
  if (f_show_needed) {
    f_show_needed = irs_false;
    show();
  }

  if (!m_entered)
  {
    m_user_value_1 = m_param_1;
    m_user_value_2 = m_param_2;
  }
  m_entered = true;

  irskey_t a_key = irskey_none;
  if (f_key_event) a_key = f_key_event->check();
  if (a_key != irskey_none)
  {
    if (!m_started && a_key == m_start_key)
    {
      m_need_process_start = true;
      m_need_process_change = true;
      m_started = true;
      m_param_1 = m_point_vector[m_current_point].value_1;
      m_param_2 = m_point_vector[m_current_point].value_2;
      f_want_redraw = true;
    }
    else if (m_started && a_key == m_stop_key)
    {
      m_need_process_stop = true;
      m_started = false;
      m_param_1 = m_user_value_1;
      m_param_2 = m_user_value_2;
      f_want_redraw = true;
      m_operating_duty = false;
      m_prev_operating_duty = false;
    }
  }

  if (m_operating_duty != m_prev_operating_duty)
  {
    m_prev_operating_duty = m_operating_duty;
    f_want_redraw = true;
  }

  switch (f_state)
  {
    case ims_show:
    {
      if (f_want_redraw)
      {
        mp_disp_drv->clear();

        if (m_point_vector.size() > 0)
        {
          show_point_info();
          show_process_status();
          mp_disp_drv->outtextpos(0, 2, "Просмотр");

          make_str(mp_prefix_out, mp_suffix_out,
            m_show_mode_out, m_point_vector[m_current_point].out_value);
          mp_disp_drv->outtextpos(0, 3, mp_value_string);
        }
        else
        {
          mp_disp_drv->outtextpos(0, 0, "Нет точек");
        }

        f_want_redraw = false;
      }
      switch(a_key)
      {
        case irskey_enter:
        {
          f_want_redraw = true;
          if (m_point_vector.size() > 0)
          {
            f_state = ims_edit;
            m_old_value = m_point_vector[m_current_point].out_value;
          }
          else
          {
            *ap_cur_menu = f_master_menu;
            f_show_needed = true;
          }
          break;
        }
        case irskey_escape:
        {
          if (f_master_menu != IRS_NULL)
          {
            f_want_redraw = true;
            *ap_cur_menu = f_master_menu;
            f_show_needed = true;

            m_param_1 = m_user_value_1;
            m_param_2 = m_user_value_2;
            m_need_process_change = true;

            if (m_started) m_need_process_stop = true;
            m_started = false;
            m_operating_duty = false;
            m_prev_operating_duty = false;

            m_entered = false;
          }
          break;
        }
        case irskey_up:
        {
          if (m_point_vector.size() > 1)
          {
            if (m_current_point < m_point_vector.size() - 1)
            {
              m_current_point++;
              m_param_1 = m_point_vector[m_current_point].value_1;
              m_param_2 = m_point_vector[m_current_point].value_2;
              f_want_redraw = true;
              m_need_process_change = true;
            }
          }
          break;
        }
        case irskey_down:
        {
          if (m_point_vector.size() > 1)
          {
            if (m_current_point > 0)
            {
              m_current_point--;
              m_param_1 = m_point_vector[m_current_point].value_1;
              m_param_2 = m_point_vector[m_current_point].value_2;
              f_want_redraw = true;
              m_need_process_change = true;
            }
          }
          break;
        }
      }
      break;
    }
    case ims_edit:
    {
      if (f_want_redraw)
      {
        mp_disp_drv->clear();

        show_point_info();
        show_process_status();
        mp_disp_drv->outtextpos(0, 2, "Правка");

        make_str(mp_prefix_out, mp_suffix_out,
          m_show_mode_out, m_point_vector[m_current_point].out_value);
        mp_disp_drv->outtextpos(0, 3, mp_value_string);
      }
      switch (a_key)
      {
        case irskey_enter:
        {
          f_want_redraw = true;
          f_state = ims_show;
          if (m_point_vector[m_current_point].out_value != m_old_value)
          {
            m_need_out_param_change = true;
            m_last_point = m_current_point;
          }
          break;
        }
        case irskey_escape:
        {
          f_want_redraw = true;
          f_state = ims_show;
          m_point_vector[m_current_point].out_value = m_old_value;
          break;
        }
        case irskey_up:
        {
          f_want_redraw = true;
          if (m_point_vector[m_current_point].out_value + m_step
              < m_max_out_value)
          {
            m_point_vector[m_current_point].out_value += m_step;
          }
          else m_point_vector[m_current_point].out_value = m_max_out_value;
          break;
        }
        case irskey_down:
        {
          f_want_redraw = true;
          if (m_point_vector[m_current_point].out_value - m_step
              > m_min_out_value)
          {
            m_point_vector[m_current_point].out_value -= m_step;
          }
          else m_point_vector[m_current_point].out_value = m_min_out_value;
          break;
        }
      }
      break;
    }
  }
}

bool irs_menu_2param_master_item_t::check_process_change_param()
{
  bool need_process_change = m_need_process_change;
  m_need_process_change = false;
  return need_process_change;
}

bool irs_menu_2param_master_item_t::check_process_start()
{
  bool need_process_start = m_need_process_start;
  m_need_process_start = false;
  return need_process_start;
}

bool irs_menu_2param_master_item_t::check_process_stop()
{
  bool need_process_stop = m_need_process_stop;
  m_need_process_stop = false;
  return need_process_stop;
}

bool irs_menu_2param_master_item_t::check_out_param_change()
{
  bool need_out_param_change = m_need_out_param_change;
  m_need_out_param_change = false;
  return need_out_param_change;
}

void irs_menu_2param_master_item_t::external_process_stop()
{
  if (m_started)
  {
    m_started = false;
    m_param_1 = m_user_value_1;
    m_param_2 = m_user_value_2;
    f_want_redraw = true;
    m_operating_duty = false;
    m_prev_operating_duty = false;
  }
}

//------------------------- БЕГУШШАЯ СТРОКА -----------------------------

irs_menu_creep_t::irs_menu_creep_t(
  char *a_buffer,
  irs_menu_base_t::size_type a_buffer_len,
  irs_menu_base_t::size_type a_line_len,
  irs_menu_base_t::size_type a_static_len,
  irs_menu_base_t::size_type a_message_len,
  irs_u8 time_num,
  irs_u8 time_denom):

  f_buffer(a_buffer),
  f_buffer_len(a_buffer_len),
  f_line_len(a_line_len),
  f_static_len(a_static_len),
  f_message_len(a_message_len),
  f_dynamic_len(irs_u8(f_buffer_len - f_static_len - f_message_len)),
  f_position(0),
  f_creeptime(0)
{
  //for (irs_u8 i = 0; i < f_buffer_len; f_buffer[i++] = ' ');
  memset(f_buffer, ' ', f_buffer_len);
  init_to_cnt();
  f_creeptime = TIME_TO_CNT(time_num, time_denom);
  set_to_cnt(f_creeptimer, f_creeptime);
  f_line = new char[f_line_len + 1];
}

irs_menu_creep_t::~irs_menu_creep_t()
{
  IRS_ARDELETE(f_line);
  deinit_to_cnt();
}

void irs_menu_creep_t::change_static(char *a_static)
{
  if (a_static)
  {
    f_position = 0;
    for (size_type i = 0; i < f_static_len; i++)
    {
      if (i < strlen(a_static))
      {
        f_buffer[i] = a_static[i];
      }
      else f_buffer[i] = ' ';
    }
    if (f_line_len > 0)
    {
      memset(f_line, ' ', f_line_len);
      f_line[f_line_len] = '\0';
    }
  }
}

void irs_menu_creep_t::change_message(char *a_message)
{
  if (a_message != IRS_NULL)
  {
    f_position = 0;
    for (size_type i = 0; i < f_message_len; i++)
    {
      if (i < strlen(a_message))
      {
        f_buffer[i + f_static_len] = a_message[i];
      }
      else f_buffer[i + f_static_len] = ' ';
    }
    if (f_line_len > 0)
    {
      memset(f_line, ' ', f_line_len);
      f_line[f_line_len] = '\0';
    }
  }
}

void irs_menu_creep_t::clear_dynamic()
{
  f_position = 0;
  for (size_type i = 0; i < f_dynamic_len; i++)
  {
    f_buffer[i + f_static_len + f_message_len] = ' ';
  }
  if (f_line_len > 0)
  {
    memset(f_line, ' ', f_line_len);
    f_line[f_line_len] = '\0';
  }
}

void irs_menu_creep_t::soft_clear_dynamic()
{
  for (size_type i = 0; i < f_dynamic_len; i++)
  {
    f_buffer[i + f_static_len + f_message_len] = ' ';
  }
}

char *irs_menu_creep_t::get_line()
{
  return f_line;
}

char *irs_menu_creep_t::get_creep_buffer()
{
  return &f_buffer[f_static_len + f_message_len];
}

void irs_menu_creep_t::shift()
{
  if (test_to_cnt(f_creeptimer))
  {
    if ((f_line_len > 0) && (f_buffer_len > 0))
    {
      set_to_cnt(f_creeptimer, f_creeptime);
      //next_to_cnt(f_creeptimer, f_creeptime);
      if (f_position > f_buffer_len)
      {
        f_position = 0;
        end_of_line = irs_true;
      }
      for (size_type i = 1; i < f_line_len; i++)
      {
        f_line[i-1] = f_line[i];
      }
      if ((f_position < f_buffer_len) && (f_buffer[f_position] != '\0'))
      {
        f_line[f_line_len-1] = f_buffer[f_position];
      }
      else f_line[f_line_len-1] = ' ';
      f_position++;
    }
    else f_line[0] = '\0';
  }
}

irs_bool irs_menu_creep_t::eol()
{
  if (end_of_line)
  {
    end_of_line = irs_false;
    return irs_true;
  }
  else return irs_false;
}

void irs_menu_creep_t::reset()
{
  f_position = 0;
  if (f_line_len > 0)
  {
    memset(f_line, ' ', f_line_len);
    f_line[f_line_len] = '\0';
  }
  for (size_type i = f_static_len; i < f_buffer_len; f_buffer[i++] = ' ');
}


