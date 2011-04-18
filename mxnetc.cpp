//! \file
//! \ingroup network_in_out_group
//! \brief Протокол MxNetC (Max Network Client)
//!
//! Дата: 29.04.2010\n
//! Ранняя дата: 12.02.2009

// Номер файла
#define MXNETCCPP_IDX 17

//#define INSERT_LEFT_BYTES // Втавка дополнительных байтов для проверки mxnet
//#define MXDATA_TO_MXNET_CHECKED // mxdata_to_mxnet_t c проверкой диапазона

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <mxnetc.h>
#include <string.h>

#include <irsfinal.h>

// Нет команды
#define MXN_COM_NONE          ((mxn_cnt_t)-1)
// Таймаут сетевых операций
#define net_t TIME_TO_CNT(1, 2)
//#define net_t TIME_TO_CNT(3600, 1)
#define broadcast_t TIME_TO_CNT(2, 1)
//#define broadcast_sw_t TIME_TO_CNT(10000, 1)

// Перераспределение памяти с сохранением данных
void *renew(void *pointer, mxn_sz_t old_size, mxn_sz_t new_size);
// Проверка контрольной суммы
irs_bool checksum_valid(mxn_packet_t *packet, mxn_cnt_t var_count,
  mxn_sz_t packet_size, irs::mxn_checksum_t checksum_type);
// Расчет контрольной суммы
irs_i32 checksum_calc(mxn_packet_t *packet, mxn_cnt_t var_count,
  mxn_sz_t packet_size, irs::mxn_checksum_t checksum_type);

irs_u8 bcst_start = 0;

// Конструктор
mxnetc::mxnetc(mxifa_ch_t channel):
  f_packet(IRS_NULL),
  f_packet_size(0),
  f_receive_size(0),
  f_send_size(0),
  f_mode(mxnc_mode_free),
  f_mode_return(mxnc_mode_free),
  f_command(MXN_COM_NONE),
  f_status(mxnc_status_success),
  f_local_status(mxnc_status_success),
  f_version(IRS_NULL),
  f_handle_channel(IRS_NULL),
  f_create_error(irs_false),
  f_abort_request(irs_false),
  f_last_error(mxnc_err_none),
  f_user_size(IRS_NULL),
  f_user_index(0),
  f_user_count(0),
  f_user_broadcast(irs_false),
  f_user_vars(IRS_NULL),
  f_net_to(0),
  f_broadcast_to(0),
  //f_broadcast_to_en(irs_false),
  //f_broadcast_packet(IRS_NULL),
  //f_broadcast_packet_size(0),
  //f_broadcast_count(0),
  //f_broadcast_vars(IRS_NULL),
  f_broadcast_mode(irs_false),
  f_oper_to(0),
  f_oper_t(0),
  f_beg_pack_proc(IRS_NULL),
  f_broadcast_proc(IRS_NULL),
  f_checksum_type(irs::mxncs_reduced_direct_sum)
{
  init_to_cnt();
  mxifa_init();
  f_handle_channel = mxifa_open(channel, irs_false);
  if (!f_handle_channel) {
    f_create_error = irs_true;
    return;
  }
  memset((void *)&f_header, 0, sizeof(f_header));
  f_beg_pack_proc = IRS_LIB_NEW_ASSERT(
    irs::mx_beg_pack_proc_t(f_handle_channel),
    MXNETCCPP_IDX);
  f_broadcast_proc = IRS_LIB_NEW_ASSERT(
    mx_broadcast_proc_t(f_handle_channel, f_checksum_type),
    MXNETCCPP_IDX);
}

// Деструктор
mxnetc::~mxnetc()
{
  if (f_broadcast_proc) IRS_LIB_DELETE_ASSERT(f_broadcast_proc);
  f_broadcast_proc = IRS_NULL;
  if (f_beg_pack_proc) IRS_LIB_DELETE_ASSERT(f_beg_pack_proc);
  f_beg_pack_proc = IRS_NULL;
  if (f_handle_channel) {
    //mxifa_close(f_handle_channel);
    mxifa_close_begin(f_handle_channel);
    mxifa_close_end(f_handle_channel, irs_true);
  }
  mxifa_deinit();
  deinit_to_cnt();
  if (f_packet) {
    IRS_LIB_ARRAY_DELETE_ASSERT((irs_u8*&)f_packet);
  }
}

// Чтение версии протокола mxnet
void mxnetc::get_version(irs_u16 *version)
{
  if (deny_proc()) return;
  if (!version) {
    f_last_error = mxnc_err_invalid_param;
    f_status = mxnc_status_error;
    return;
  }

  f_version = version;
  f_command = MXN_GET_VERSION;
  f_status = mxnc_status_busy;
}

// Чтение размера массива (количество переменных)
void mxnetc::get_size(mxn_cnt_t *size)
{
  if (deny_proc()) return;
  if (!size) {
    f_last_error = mxnc_err_invalid_param;
    f_status = mxnc_status_error;
    return;
  }

  f_user_size = size;
  f_command = MXN_READ_COUNT;
  f_status = mxnc_status_busy;
}

// Чтение переменных
void mxnetc::read(mxn_cnt_t index, mxn_cnt_t count, irs_i32 *vars)
{
  if (deny_proc()) return;
  irs_bool index_over_max = (count > MXN_CNT_MAX - index);
  irs_bool vars_invalid = !vars;
  irs_bool count_zero = !count;
  if (index_over_max || vars_invalid || count_zero) {
    f_last_error = mxnc_err_invalid_param;
    f_status = mxnc_status_error;
    return;
  }

  f_user_index = index;
  f_user_count = count;
  f_user_vars = vars;
  f_command = MXN_READ;
  f_status = mxnc_status_busy;
}

// Запись переменных
void mxnetc::write(mxn_cnt_t index, mxn_cnt_t count, irs_i32 *vars)
{
  if (deny_proc()) return;
  irs_bool index_over_max = (count > MXN_CNT_MAX - index);
  irs_bool vars_invalid = !vars;
  irs_bool count_zero = !count;
  if (index_over_max || vars_invalid || count_zero) {
    f_last_error = mxnc_err_invalid_param;
    f_status = mxnc_status_error;
    return;
  }

  f_user_index = index;
  f_user_count = count;
  f_user_vars = vars;
  f_command = MXN_WRITE;
  f_status = mxnc_status_busy;
}

// Включение/выключение широковещательного режима
void mxnetc::set_broadcast(irs_bool broadcast_mode)
{
  if (deny_proc()) return;
  if ((broadcast_mode != irs_true) && (broadcast_mode != irs_false)) {
    f_last_error = mxnc_err_invalid_param;
    f_status = mxnc_status_error;
    return;
  }

  f_user_broadcast = broadcast_mode;
  f_command = MXN_SET_BROADCAST;
  f_status = mxnc_status_busy;
}

// Статус текущей операции
mxnc_status_t mxnetc::status()
{
  if (f_create_error) {
    f_last_error = mxnc_err_create;
    return mxnc_status_error;
  } else {
    return f_status;
  }
}

// Элементарное действие
void mxnetc::tick()
{
  mxifa_tick();

  //if (f_mode == mxnc_mode_chunk_read_wait) red_blink();

  if (f_broadcast_mode) {
    switch (f_command) {
      case MXN_READ_COUNT: {
        *f_user_size = f_broadcast_proc->get_count();
        f_status = mxnc_status_success;

        f_user_size = IRS_NULL;
        f_command = MXN_COM_NONE;
      } break;
      case MXN_READ: {
        mxn_cnt_t broadcast_count = f_broadcast_proc->get_count();
        if (f_user_index + f_user_count <= broadcast_count) {
          f_broadcast_proc->get_vars(f_user_vars, f_user_index,
            f_user_count);
          f_status = mxnc_status_success;
        } else {
          f_last_error = mxnc_err_invalid_param;
          f_status = mxnc_status_error;
        }

        f_user_index = 0;
        f_user_count = 0;
        f_user_vars = IRS_NULL;
        f_command = MXN_COM_NONE;
      } break;
    }
  }

  switch (f_mode) {
    case mxnc_mode_free: {
      f_mode = mxnc_mode_broadcast;
    } break;
    case mxnc_mode_packet: {
      mxn_cnt_t index = 0;
      mxn_cnt_t count = 0;
      irs_bool command_performed = irs_false;
      switch (f_command) {
        case MXN_GET_VERSION: {
          index = 0;
          count = 0;
        } break;
        case MXN_READ_COUNT: {
          if (f_broadcast_mode) {
            command_performed = irs_true;
            f_mode = mxnc_mode_free;
          } else {
            index = 0;
            count = 0;
            //dbg_write = irs_true;
          }
        } break;
        case MXN_READ: {
          if (f_broadcast_mode) {
            command_performed = irs_true;
            f_mode = mxnc_mode_free;
          } else {
            index = f_user_index;
            count = f_user_count;
          }
        } break;
        case MXN_WRITE: {
          index = f_user_index;
          count = f_user_count;
        } break;
        case MXN_SET_BROADCAST: {
          index = f_user_broadcast;
          count = 0;
        } break;
        default: {
          command_performed = irs_true;
          f_mode = mxnc_mode_free;
        } break;
      }
      if (!command_performed) {
        irs_bool fill_success = packet_fill(f_command, index, count,
          f_user_vars, count);
        if (fill_success) {
          //mxifa_write_begin(f_handle_channel, IRS_NULL, (irs_u8 *)f_packet,
            //f_send_size);
          f_mode = mxnc_mode_write;
        } else {
          f_mode = mxnc_mode_reset;
          f_last_error = mxnc_err_nomem;
          f_local_status = mxnc_status_error;
        }
      }
    } break;
    case mxnc_mode_write: {
      #ifdef INSERT_LEFT_BYTES
      static irs_u8 *buf = 0;
      IRS_LIB_ARRAY_DELETE_ASSERT(buf);

      const mxn_sz_t added_size = 77;
      mxn_sz_t new_size = f_send_size + added_size;
      buf = IRS_LIB_NEW_ASSERT(irs_u8[new_size], MXNETCCPP_IDX);

      const irs_u8 fill_byte = 0x9A;
      memcpy(buf + added_size, f_packet, f_send_size);
      for (mxn_sz_t i = 0; i < added_size; i++) buf[i] = fill_byte;

      mxifa_write_begin(f_handle_channel, IRS_NULL, (irs_u8 *)buf,
        new_size);
      #else //INSERT_LEFT_BYTE
      mxifa_write_begin(f_handle_channel, IRS_NULL, (irs_u8 *)f_packet,
        f_send_size);
      #endif //INSERT_LEFT_BYTE
      if (f_oper_t) {
        set_to_cnt(f_oper_to, f_oper_t);
      }
      f_mode = mxnc_mode_write_wait;
    } break;
    case mxnc_mode_write_wait: {
      if (mxifa_write_end(f_handle_channel, irs_false)) {
        f_mode = mxnc_mode_read;
      } else if (f_abort_request) {
        mxifa_write_end(f_handle_channel, irs_true);
        f_mode = mxnc_mode_reset;
        f_last_error = mxnc_err_abort;
        f_local_status = mxnc_status_error;
      } else if (f_oper_t) {
        if (test_to_cnt(f_oper_to)) {
          mxifa_write_end(f_handle_channel, irs_true);
          f_mode = mxnc_mode_reset;
          f_last_error = mxnc_err_timeout;
          f_local_status = mxnc_status_error;
        }
      }
    } break;
    case mxnc_mode_read: {
      (*f_beg_pack_proc)((irs_u8 *)f_packet);
      set_to_cnt(f_net_to, net_t);
      if (f_oper_t) {
        set_to_cnt(f_oper_to, f_oper_t);
      }
      f_mode = mxnc_mode_begin_packet;
    } break;
    case mxnc_mode_begin_packet: {
      if (!f_beg_pack_proc->tick()) {
        if (f_packet->code_comm == f_command) {
          irs_u8 *buf = ((irs_u8 *)f_packet) + mxn_header_size;
          mxn_sz_t size = f_receive_size - mxn_header_size;
          mxifa_read_begin(f_handle_channel, IRS_NULL, buf, size);
          if (f_oper_t) {
            set_to_cnt(f_oper_to, f_oper_t);
          }
          f_mode = mxnc_mode_chunk_read_wait;
        } else if (f_packet->code_comm == MXN_WRITE_BROADCAST) {
          (*f_broadcast_proc)((irs_u8 *)f_packet, f_checksum_type);
          f_mode = mxnc_mode_bcast_in_com;
        } else {
          f_mode = mxnc_mode_packet;
        }
      } else if (f_abort_request) {
        f_beg_pack_proc->abort();
        f_last_error = mxnc_err_abort;
        f_local_status = mxnc_status_error;
        f_mode_return = mxnc_mode_reset;
        f_mode = mxnc_mode_bpack_abort;
      } else if (test_to_cnt(f_net_to)) {
        f_beg_pack_proc->abort();
        f_mode_return = mxnc_mode_packet;
        f_mode = mxnc_mode_bpack_abort;
      } else if (f_oper_t) {
        if (test_to_cnt(f_oper_to)) {
          f_beg_pack_proc->abort();
          f_last_error = mxnc_err_timeout;
          f_local_status = mxnc_status_error;
          f_mode_return = mxnc_mode_reset;
          f_mode = mxnc_mode_bpack_abort;
        }
      }
    } break;
    case mxnc_mode_bpack_abort: {
      if (!f_beg_pack_proc->tick()) {
        f_mode = f_mode_return;
      }
    } break;
    case mxnc_mode_bcast_in_com: {
      if (!f_broadcast_proc->tick()) {
        if (f_broadcast_proc->success()) {
          f_broadcast_mode = irs_true;
          set_to_cnt(f_broadcast_to, broadcast_t);
          bcst_start = 1;
        }
        f_mode = mxnc_mode_packet;
      } else if (f_abort_request) {
        f_broadcast_proc->abort();
        f_last_error = mxnc_err_abort;
        f_local_status = mxnc_status_error;
        f_mode_return = mxnc_mode_reset;
        f_mode = mxnc_mode_bcast_in_com_abort;
      } else if (test_to_cnt(f_net_to)) {
        f_broadcast_proc->abort();
        f_mode_return = mxnc_mode_packet;
        f_mode = mxnc_mode_bcast_in_com_abort;
      } else if (f_oper_t) {
        if (test_to_cnt(f_oper_to)) {
          f_broadcast_proc->abort();
          f_last_error = mxnc_err_timeout;
          f_local_status = mxnc_status_error;
          f_mode_return = mxnc_mode_reset;
          f_mode = mxnc_mode_bcast_in_com_abort;
        }
      }
    } break;
    case mxnc_mode_bcast_in_com_abort: {
      if (!f_broadcast_proc->tick()) {
        f_mode = f_mode_return;
      }
    } break;
    case mxnc_mode_chunk_read_wait: {
      if (mxifa_read_end(f_handle_channel, irs_false)) {
        f_mode = mxnc_mode_checksum;
      } else if (f_abort_request) {
        mxifa_read_end(f_handle_channel, irs_true);
        f_mode = mxnc_mode_reset;
        f_last_error = mxnc_err_abort;
        f_local_status = mxnc_status_error;
      } else if (f_oper_t) {
        if (test_to_cnt(f_oper_to)) {
          mxifa_read_end(f_handle_channel, irs_true);
          f_mode = mxnc_mode_reset;
          f_last_error = mxnc_err_timeout;
          f_local_status = mxnc_status_error;
        }
      }
    } break;
    case mxnc_mode_checksum: {
      mxn_cnt_t var_count = 0;
      switch (f_packet->code_comm) {
        case MXN_READ: {
          var_count = f_packet->var_count;
        } break;
        default: {
          var_count = 0;
        } break;
      }
      mxn_sz_t cur_packet_size =
        (MXN_SIZE_OF_HEADER + var_count + 1)*sizeof(irs_i32);
      irs_bool packet_size_ok = (cur_packet_size == f_receive_size);
      irs_bool checksum_ok = checksum_valid(f_packet, var_count,
        f_receive_size, f_checksum_type);
      if (packet_size_ok && checksum_ok) {
        f_mode = mxnc_mode_proc;
      } else {
        f_mode = mxnc_mode_packet;
      }
    } break;
    case mxnc_mode_proc: {
      switch (f_command) {
        case MXN_GET_VERSION: {
          if (f_version) {
            *f_version = IRS_LOWORD(f_packet->var_ind_first);
            f_mode = mxnc_mode_reset;
            f_local_status = mxnc_status_success;
          } else {
            f_mode = mxnc_mode_reset;
            f_last_error = mxnc_err_invalid_param;
            f_local_status = mxnc_status_error;
          }
        } break;
        case MXN_READ_COUNT: {
          if (f_user_size) {
            *f_user_size = f_packet->var_count;
            f_mode = mxnc_mode_reset;
            f_local_status = mxnc_status_success;
          } else {
            f_mode = mxnc_mode_reset;
            f_last_error = mxnc_err_invalid_param;
            f_local_status = mxnc_status_error;
          }
        } break;
        case MXN_READ: {
          irs_bool user_vars_valid = (f_user_vars != IRS_NULL);
          irs_bool index_valid = (f_user_index == f_packet->var_ind_first);
          irs_bool count_valid = (f_user_count == f_packet->var_count);
          if (user_vars_valid && index_valid && count_valid) {
            f_mode = mxnc_mode_reset;
            f_local_status = mxnc_status_success;
            memcpy((void *)f_user_vars, (void *)f_packet->var,
              f_user_count*sizeof(irs_i32));
            #ifdef NOP
            ShowMessage(
              Format(
                "pointer = %8X\n"
                "index = %d\n"
                "count = %d\n"
                "vars[1] = %8X",
                ARRAYOFCONST((
                  (int)f_user_vars,
                  (int)f_user_index,
                  (int)f_user_count,
                  (int)f_user_vars[1]
                ))
              )
            );
            #endif //NOP
          } else if (!user_vars_valid) {
            f_mode = mxnc_mode_reset;
            f_last_error = mxnc_err_invalid_param;
            f_local_status = mxnc_status_error;
          } else {
            f_mode = mxnc_mode_reset;
            f_last_error = mxnc_err_bad_server;
            f_local_status = mxnc_status_error;
          }
        } break;
        case MXN_WRITE: {
          irs_bool index_valid = (0 == f_packet->var_ind_first);
          irs_bool par_invalid = (1 == f_packet->var_ind_first);
          irs_bool count_valid = (0 == f_packet->var_count);
          if (index_valid && count_valid) {
            f_mode = mxnc_mode_reset;
            f_local_status = mxnc_status_success;
          } else if (par_invalid) {
            f_mode = mxnc_mode_reset;
            f_last_error = mxnc_err_remote_invalid_param;
            f_local_status = mxnc_status_error;
          } else {
            f_mode = mxnc_mode_reset;
            f_last_error = mxnc_err_bad_server;
            f_local_status = mxnc_status_error;
          }
        } break;
        case MXN_SET_BROADCAST: {
          irs_bool index_valid =
            (f_user_broadcast == (irs_bool)f_packet->var_ind_first);
          irs_bool count_valid = (0 == f_packet->var_count);
          if (index_valid && count_valid) {
            f_mode = mxnc_mode_reset;
            f_local_status = mxnc_status_success;
          } else {
            f_mode = mxnc_mode_reset;
            f_last_error = mxnc_err_bad_server;
            f_local_status = mxnc_status_error;
          }
        } break;
      }
    } break;
    case mxnc_mode_reset: {
      f_abort_request = irs_false;
      f_version = IRS_NULL;
      f_user_size = IRS_NULL;
      f_user_index = 0;
      f_user_count = 0;
      f_user_vars = IRS_NULL;
      f_user_broadcast = irs_false;
      f_command = MXN_COM_NONE;
      f_mode = mxnc_mode_free;
      f_status = f_local_status;
    } break;







    case mxnc_mode_broadcast: {
      (*f_beg_pack_proc)((irs_u8 *)&f_header);
      //set_to_cnt(f_net_to, net_t);
      f_mode = mxnc_mode_broadcast_wait;
    } break;
    case mxnc_mode_broadcast_wait: {
      if (f_command != MXN_COM_NONE) {
        f_beg_pack_proc->abort();
        f_mode = mxnc_mode_command_abort_wait;
      } else {
        if (!f_beg_pack_proc->tick()) {
          if (f_header.code_comm == MXN_WRITE_BROADCAST) {
            (*f_broadcast_proc)((irs_u8 *)&f_header, f_checksum_type);
            f_mode = mxnc_mode_broadcast_proc_wait;
          } else {
            f_mode = mxnc_mode_broadcast;
          }
        } else if (f_broadcast_mode) {
          if (test_to_cnt(f_broadcast_to)) {
            f_beg_pack_proc->abort();
            f_mode = mxnc_mode_timeout_abort_wait;
          }
        }
      }
    } break;
    case mxnc_mode_command_abort_wait: {
      if (!f_beg_pack_proc->tick()) {
        f_mode = mxnc_mode_packet;
      }
    } break;
    case mxnc_mode_timeout_abort_wait: {
      if (!f_beg_pack_proc->tick()) {
        f_broadcast_mode = irs_false;
        f_mode = mxnc_mode_broadcast;
      }
    } break;
    case mxnc_mode_broadcast_proc_wait: {
      if (!f_broadcast_proc->tick()) {
        if (f_broadcast_proc->success()) {
          f_broadcast_mode = irs_true;
          set_to_cnt(f_broadcast_to, broadcast_t);
          bcst_start = 2;
        }
        f_mode = mxnc_mode_broadcast;
      } else if (f_broadcast_mode) {
        if (test_to_cnt(f_broadcast_to)) {
          f_broadcast_proc->abort();
          f_mode = mxnc_mode_bcast_abort_wait;
        }
      }
    } break;
    case mxnc_mode_bcast_abort_wait: {
      if (!f_broadcast_proc->tick()) {
        f_broadcast_mode = irs_false;
        f_mode = mxnc_mode_broadcast;
      }
    } break;
    #ifdef NOP
    case mxnc_mode_free: {
    } break;
    #endif //NOP
  }
}

// Прерывание операции
void mxnetc::abort()
{
  if (f_command != MXN_COM_NONE) {
    f_abort_request = irs_true;
  }
}

// Проверка контрольной суммы
irs_bool checksum_valid(mxn_packet_t *packet, mxn_cnt_t var_count,
    mxn_sz_t packet_size, irs::mxn_checksum_t checksum_type)
{
  mxn_sz_t request_packet_size =
    (MXN_SIZE_OF_HEADER + var_count + 1)*sizeof(irs_i32);
  if (request_packet_size <= packet_size) {
    return packet->var[var_count] == checksum_calc(packet, var_count,
      packet_size, checksum_type);
  } else {
    return irs_false;
  }
}

// Расчет контрольной суммы
irs_i32 checksum_calc(mxn_packet_t *packet, mxn_cnt_t var_count,
  mxn_sz_t packet_size, irs::mxn_checksum_t checksum_type)
{
  mxn_sz_t request_packet_size =
    (MXN_SIZE_OF_HEADER + var_count + 1)*sizeof(irs_i32);
  if (request_packet_size > packet_size) {
    return 0;
  }

  return irs::checksum_calc(checksum_type, packet, var_count);
}

// Чтение последней ошибки
mxnc_error_t mxnetc::get_last_error()
{
  return f_last_error;
}

// Разрешение обработки
irs_bool mxnetc::deny_proc()
{
  if (f_create_error) {
    f_last_error = mxnc_err_invalid_param;
    f_status = mxnc_status_error;
    return irs_true;
  }
  if (f_command != MXN_COM_NONE) {
    f_last_error = mxnc_err_busy;
    f_status = mxnc_status_error;
    return irs_true;
  }
  return irs_false;
}

// Заполнение пакета
irs_bool mxnetc::packet_fill(mxn_cnt_t code_comm, mxn_cnt_t packet_var_first,
  mxn_cnt_t packet_var_count, irs_i32 *vars, mxn_cnt_t var_count)
{
  f_packet_size = (MXN_SIZE_OF_HEADER + var_count + 1)*sizeof(irs_i32);
  if (code_comm == MXN_WRITE) {
    f_receive_size = (MXN_SIZE_OF_HEADER + 1)*sizeof(irs_i32);
  } else {
    f_receive_size = f_packet_size;
  }
  if (code_comm == MXN_READ) {
    f_send_size = (MXN_SIZE_OF_HEADER + 1)*sizeof(irs_i32);
  } else {
    f_send_size = f_packet_size;
  }
  if (f_packet) {
    IRS_LIB_ARRAY_DELETE_ASSERT((irs_u8*&)f_packet);
  }
  f_packet = (mxn_packet_t *)IRS_LIB_NEW_ASSERT(
    irs_u8[f_packet_size], MXNETCCPP_IDX);
  if (f_packet) {
    f_packet->ident_beg_pack_first = MXN_CONST_IDENT_BEG_PACK_FIRST;
    f_packet->ident_beg_pack_second = MXN_CONST_IDENT_BEG_PACK_SECOND;
    f_packet->code_comm = code_comm;
    f_packet->var_ind_first = packet_var_first;
    f_packet->var_count = packet_var_count;
    if (code_comm == MXN_WRITE) {
      memcpy((void *)f_packet->var, (void *)vars, var_count*sizeof(irs_i32));
      f_packet->var[var_count] = checksum_calc(f_packet, var_count,
        f_send_size, f_checksum_type);
    } else {
      f_packet->var[0] = checksum_calc(f_packet, 0, f_send_size,
        f_checksum_type);
    }
    return irs_true;
  } else {
    return irs_false;
  }
}

// Перераспределение памяти с сохранением данных
void *renew(void *pointer, mxn_sz_t old_size, mxn_sz_t new_size)
{
  if (old_size == new_size) return pointer;

  void *new_pointer = IRS_NULL;
  if (new_size) {
    new_pointer = IRS_LIB_NEW_ASSERT(void *[new_size], MXNETCCPP_IDX);
    if (!new_pointer) return new_pointer;
  }
  if (pointer) {
    if (old_size && new_size) {
      memcpy(new_pointer, pointer, irs_min(old_size, new_size));
    }
    IRS_LIB_ARRAY_DELETE_ASSERT((irs_u8*&)pointer);
  }
  return new_pointer;
}

// Установка таймаута операций, с
void mxnetc::set_timeout(calccnt_t t_num, calccnt_t t_denom)
{
  if (t_denom != 0) {
    f_oper_t = TIME_TO_CNT(t_num, t_denom);
  } else {
    f_oper_t = 0;
  }
}

// Установка ip-адреса удаленного устройства
void mxnetc::set_dest_ip(mxip_t ip)
{
  if (mxifa_get_channel_type(f_handle_channel) == mxifa_ei_win32_tcp_ip) {
    mxifa_win32_tcp_ip_cfg cfg;
    mxifa_get_config(f_handle_channel, (void *)&cfg);
    cfg.dest_ip = ip;
    mxifa_set_config(f_handle_channel, (void *)&cfg);
  }
}

// Установка порта удаленного устройства
void mxnetc::set_dest_port(irs_u16 port)
{
  if (mxifa_get_channel_type(f_handle_channel) == mxifa_ei_win32_tcp_ip) {
    mxifa_win32_tcp_ip_cfg cfg;
    mxifa_get_config(f_handle_channel, (void *)&cfg);
    cfg.dest_port = port;
    mxifa_set_config(f_handle_channel, (void *)&cfg);
  }
}

// Установка локального порта
void mxnetc::set_local_port(irs_u16 port)
{
  if (mxifa_get_channel_type(f_handle_channel) == mxifa_ei_win32_tcp_ip) {
    mxifa_win32_tcp_ip_cfg cfg;
    mxifa_get_config(f_handle_channel, (void *)&cfg);
    cfg.local_port = port;
    mxifa_set_config(f_handle_channel, (void *)&cfg);
  }
}

// Задание типа контрольной суммы
void mxnetc::set_checksum_type(irs::mxn_checksum_t a_checksum_type)
{
  f_checksum_type = a_checksum_type;
}

// Чтение текущего режима
irs_bool mxnetc::get_broadcast_mode()
{
  return f_broadcast_mode;
}


mx_broadcast_proc_t::mx_broadcast_proc_t(void *a_handle_channel,
  irs::mxn_checksum_t a_checksum_type):
  f_mode(mode_start),
  f_handle_channel(a_handle_channel),
  f_buf(IRS_NULL),
  f_abort_request(irs_false),
  f_proc(irs_true),
  f_broadcast_packet(IRS_NULL),
  f_broadcast_packet_size(0),
  f_broadcast_vars(IRS_NULL),
  f_broadcast_count(0),
  f_success(irs_false),
  f_checksum_type(a_checksum_type)
{
}

mx_broadcast_proc_t::~mx_broadcast_proc_t()
{
  if (f_mode == mode_vars_wait) {
    mxifa_read_end(f_handle_channel, irs_true);
  }
  if (f_broadcast_packet) {
    IRS_LIB_ARRAY_DELETE_ASSERT((irs_u8*&)f_broadcast_packet);
  }
  if (f_broadcast_vars) {
    IRS_LIB_ARRAY_DELETE_ASSERT(f_broadcast_vars);
  }
}

void mx_broadcast_proc_t::operator()(irs_u8 *a_buf,
  irs::mxn_checksum_t a_checksum_type)
{
  f_buf = a_buf;
  f_proc = irs_true;
  f_mode = mode_start;
  f_checksum_type = a_checksum_type;
}

irs_bool mx_broadcast_proc_t::tick()
{
  switch (f_mode) {
    case mode_start: {
      mxn_packet_t *header = (mxn_packet_t *)f_buf;
      f_broadcast_count = header->var_count;
      mxn_sz_t old_size = f_broadcast_packet_size;
      f_broadcast_packet_size =
        (MXN_SIZE_OF_HEADER + f_broadcast_count + 1)*sizeof(irs_i32);
      f_broadcast_packet = (mxn_packet_t *)renew (f_broadcast_packet,
        old_size, f_broadcast_packet_size);
      memcpy((void *)f_broadcast_packet, (void *)f_buf, mxn_header_size);

      irs_u8 *buf = ((irs_u8 *)f_broadcast_packet) + mxn_header_size;
      mxifa_sz_t size = f_broadcast_packet_size - mxn_header_size;
      mxifa_read_begin(f_handle_channel, IRS_NULL, buf, size);

      f_mode = mode_vars_wait;
    } break;
    case mode_vars_wait: {
      if (mxifa_read_end(f_handle_channel, irs_false)) {
        if (checksum_valid(f_broadcast_packet, f_broadcast_count,
          f_broadcast_packet_size, f_checksum_type)) {
          if (f_broadcast_vars) {
            IRS_LIB_ARRAY_DELETE_ASSERT(f_broadcast_vars);
            f_broadcast_vars = IRS_NULL;
          }
          f_broadcast_vars = IRS_LIB_NEW_ASSERT(irs_i32[f_broadcast_count],
            MXNETCCPP_IDX);
          if (f_broadcast_vars) {
            memcpy((void *)f_broadcast_vars, (void *)f_broadcast_packet->var,
              f_broadcast_count*sizeof(irs_i32));
            f_success = irs_true;
          } else {
            f_success = irs_false;
          }
        } else {
          f_success = irs_false;
        }
        f_mode = mode_stop;
      } else if (f_abort_request) {
        mxifa_read_end(f_handle_channel, irs_true);
        f_mode = mode_stop;
      }
    } break;
    case mode_stop: {
      f_proc = irs_false;
      f_abort_request = irs_false;
    } break;
  }

  return f_proc;
}

void mx_broadcast_proc_t::abort()
{
  f_abort_request = irs_true;
}

irs_bool mx_broadcast_proc_t::success()
{
  return f_success;
}

void mx_broadcast_proc_t::get_vars(irs_i32 *user_var, mxn_cnt_t index,
  mxn_cnt_t count)
{
  void *dest = (void *)user_var;
  void *src = (void *)(f_broadcast_vars + index);
  size_t size = count*sizeof(irs_i32);
  memcpy(dest, src, size);
}

mxn_cnt_t mx_broadcast_proc_t::get_count()
{
  return f_broadcast_count;
}


// Преобразование mxnetc в mxdata_t
irs::mxdata_to_mxnet_t::mxdata_to_mxnet_t(mxnetc *ap_mxnet,
  const counter_t &a_connect_interval,
  const counter_t &a_update_interval):
  m_status(FREE),
  mp_mxnet(ap_mxnet),
  m_is_connected(irs_false),
  mp_buf(IRS_NULL),
  mp_read_buf(IRS_NULL),
  m_mxnet_size(0),
  m_mxnet_size_byte(m_mxnet_size*sizeof(irs_i32)),
  m_current_index(0),
  m_update_interval_to(0),
  m_update_interval_time(a_update_interval),
  m_connect_counter(0),
  m_connect_interval(a_connect_interval),
  mp_measured_interval_alg(),
  m_measured_interval_bad_alloc(false),
  m_first_connect(false)
{
  init_to_cnt();
  if (a_update_interval == irs_mxdata_to_mxnet_def_interval) {
    m_update_interval_time = TIME_TO_CNT(1, 5);
  }
  if (a_connect_interval == irs_mxdata_to_mxnet_def_interval) {
    m_connect_interval = TIME_TO_CNT(2, 1);
  }
  m_write_vector.clear();
  set_to_cnt(m_connect_counter, m_connect_interval);
  set_to_cnt(m_update_interval_to, m_update_interval_time);
}

irs::mxdata_to_mxnet_t::~mxdata_to_mxnet_t()
{
  IRS_LIB_ARRAY_DELETE_ASSERT(mp_buf);
  IRS_LIB_ARRAY_DELETE_ASSERT(mp_read_buf);
  deinit_to_cnt();
}

void irs::mxdata_to_mxnet_t::connect(mxnetc *ap_mxnet)
{
  mp_mxnet = ap_mxnet;
}

double irs::mxdata_to_mxnet_t::measured_interval()
{
  if (!m_measured_interval_bad_alloc) {
    if (mp_measured_interval_alg.get()) {
      return mp_measured_interval_alg->time();
    } else {
      mp_measured_interval_alg.reset(
        IRS_LIB_NEW_ASSERT(mx_time_int_t(TIME_TO_CNT(1, 1)), MXNETCCPP_IDX));

      if (!mp_measured_interval_alg.get())
        m_measured_interval_bad_alloc = true;
    }
  }
  return 0.;
}

void irs::mxdata_to_mxnet_t::tick()
{
  if (mp_mxnet) mp_mxnet->tick();
  switch (m_status)
  {
    case FREE:
    {
      if (test_to_cnt(m_update_interval_to)) {
        set_to_cnt(m_update_interval_to, m_update_interval_time);

        if (mp_measured_interval_alg.get()) {
          (*mp_measured_interval_alg.get())();
        }

        if (m_is_connected)
        {
          for (; m_current_index < m_mxnet_size_byte; m_current_index++)
          {
            if (m_write_vector[m_current_index] == true)
            {
              m_current_index = m_size_var_byte *
                irs_uarc(m_current_index / m_size_var_byte);
              irs_uarc m_finish_index = m_current_index + m_size_var_byte;
              for (; ; m_finish_index+=m_size_var_byte)
              {
                if ((m_write_vector[m_finish_index] == false) ||
                  (m_finish_index >= m_mxnet_size_byte))
                {
                  mp_mxnet->write(m_current_index/m_size_var_byte,
                    (m_finish_index - m_current_index)/m_size_var_byte,
                    (irs_i32*)&mp_buf[m_current_index]);
                  set_to_cnt(m_connect_counter, m_connect_interval);
                  for (irs_uarc i = m_current_index; i <= m_finish_index; i++)
                    m_write_vector[i] = false;
                  m_current_index = m_finish_index;
                  m_status = WRITE;
                  break;
                }
              }
              break;
            }
          }
          if ((m_current_index >= m_mxnet_size_byte - 1) &&
            (m_status != WRITE))
          {
            m_status = READ;
            mp_mxnet->read(0, m_mxnet_size, (irs_i32*)mp_read_buf);
            set_to_cnt(m_connect_counter, m_connect_interval);
            m_current_index = 0;
          }
        }
        else
        {
          mp_mxnet->get_size(&m_mxnet_size);
          m_status = GET_SIZE;
        }
      }
      break;
    }
    case GET_SIZE:
    {
      if (mp_mxnet->status() == mxnc_status_success)
      {
        if (m_mxnet_size > 0)
        {
          m_mxnet_size_byte = m_mxnet_size*sizeof(irs_i32);

          const mxn_sz_t m_mxnet_old_size_byte = m_write_vector.size();
          if (m_mxnet_size_byte != m_mxnet_old_size_byte) {
            IRS_LIB_ARRAY_DELETE_ASSERT(mp_buf);
            IRS_LIB_ARRAY_DELETE_ASSERT(mp_read_buf);
            mp_buf = IRS_LIB_NEW_ASSERT(irs_u8[m_mxnet_size_byte],
              MXNETCCPP_IDX);
            mp_read_buf = IRS_LIB_NEW_ASSERT(irs_u8[m_mxnet_size_byte],
              MXNETCCPP_IDX);
            if (mp_buf && mp_read_buf)
            {
              m_status = READ;
              mp_mxnet->read(0, m_mxnet_size, (irs_i32*)mp_read_buf);
              set_to_cnt(m_connect_counter, m_connect_interval);
              m_write_vector.resize(m_mxnet_size_byte, false);
              for (irs_uarc i = 0; i < m_mxnet_size_byte; i++)
              {
                m_write_vector[i] = false;
              }
            }
          } else {
            m_status = READ;
            mp_mxnet->read(0, m_mxnet_size, (irs_i32*)mp_read_buf);
            set_to_cnt(m_connect_counter, m_connect_interval);
          }
        }
        else mp_mxnet->get_size(&m_mxnet_size);
      }
      else if (mp_mxnet->status() == mxnc_status_error)
      {
        m_status = FREE;
        m_is_connected = false;
      }
      break;
    }
    case READ:
    {
      if (mp_mxnet->status() == mxnc_status_success)
      {
        irs_uarc m_read_index = 0;
        for (; m_read_index < m_mxnet_size_byte; m_read_index++)
        {
          if (m_write_vector[m_read_index] == false)
            mp_buf[m_read_index] = mp_read_buf[m_read_index];
        }
        m_is_connected = irs_true;
        m_first_connect = true;
        m_status = FREE;
      }
      else if (mp_mxnet->status() == mxnc_status_error)
      {
        m_status = FREE;
        m_is_connected = false;
      }
      if (test_to_cnt(m_connect_counter))
      {
        m_is_connected = irs_false;
        mp_mxnet->abort();
        m_status = FREE;
      }
      break;
    }
    case WRITE:
    {
      if (mp_mxnet->status() == mxnc_status_success)
      {
        m_status = FREE;
      }
      else if (mp_mxnet->status() == mxnc_status_error)
      {
        m_status = FREE;
        m_is_connected = false;
      }
      if (test_to_cnt(m_connect_counter))
      {
        m_is_connected = irs_false;
        mp_mxnet->abort();
        m_status = FREE;
      }
      break;
    }
  }
}

irs_uarc irs::mxdata_to_mxnet_t::size()
{
  //if (m_is_connected)
  return m_mxnet_size_byte;
  //else return 0;
}

irs_bool irs::mxdata_to_mxnet_t::connected()
{
  return m_is_connected;
}

void irs::mxdata_to_mxnet_t::read(irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  //if (m_is_connected)
  if (m_first_connect)
  {
    irs_uarc real_size = a_size;
    irs_uarc real_index = a_index;
    #ifdef MXDATA_TO_MXNET_CHECKED
    memset((void*)ap_buf, 0, real_size);
    if (real_index > m_mxnet_size_byte - 1) return;
    if (real_size > m_mxnet_size_byte - real_index)
      real_size = m_mxnet_size_byte - real_index;
    #endif //MXDATA_TO_MXNET_CHECKED
    memcpy((void*)ap_buf, (void*)(mp_buf + real_index), real_size);
  }
}

void irs::mxdata_to_mxnet_t::write(const irs_u8 *ap_buf, irs_uarc a_index,
  irs_uarc a_size)
{
  //if (m_is_connected)
  if (m_first_connect)
  {
    irs_uarc real_size = a_size;
    irs_uarc real_index = a_index;
    #ifdef MXDATA_TO_MXNET_CHECKED
    if (real_index > m_mxnet_size_byte - 1) return;
    if (real_size > m_mxnet_size_byte - real_index)
      real_size = m_mxnet_size_byte - real_index;
    #endif //MXDATA_TO_MXNET_CHECKED
    memcpy((void*)(mp_buf + real_index), (void*)ap_buf, real_size);
    for (irs_uarc i = real_index; i < real_index + real_size; i++)
      m_write_vector[i] = true;
  }
}

irs_bool irs::mxdata_to_mxnet_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  //if (m_is_connected)
  if (m_first_connect)
  {
    irs_uarc index = a_index;
    irs_uarc bit_index = a_bit_index;
    #ifdef MXDATA_TO_MXNET_CHECKED
    if (index >= m_mxnet_size_byte) index = m_mxnet_size_byte - 1;
    if (bit_index > m_high_bit) bit_index = m_high_bit;
    #endif //MXDATA_TO_MXNET_CHECKED
    return static_cast<irs_bool>((mp_buf[index] >> bit_index) & 1);
  }
  else return irs_false;
}

void irs::mxdata_to_mxnet_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  //if (m_is_connected)
  if (m_first_connect)
  {
    irs_uarc index = a_index;
    irs_uarc bit_index = a_bit_index;
    #ifdef MXDATA_TO_MXNET_CHECKED
    if (index >= m_mxnet_size_byte) index = m_mxnet_size_byte - 1;
    if (bit_index > m_high_bit) bit_index = m_high_bit;
    #endif //MXDATA_TO_MXNET_CHECKED
    mp_buf[index] |= irs_u8(1 << bit_index);
    m_write_vector[index] = true;
  }
}

void irs::mxdata_to_mxnet_t::clear_bit(irs_uarc a_index,irs_uarc a_bit_index)
{
  //if (m_is_connected)
  if (m_first_connect)
  {
    irs_uarc index = a_index;
    irs_uarc bit_index = a_bit_index;
    #ifdef MXDATA_TO_MXNET_CHECKED
    if (index >= m_mxnet_size_byte) index = m_mxnet_size_byte - 1;
    if (bit_index > m_high_bit) bit_index = m_high_bit;
    #endif //MXDATA_TO_MXNET_CHECKED
    mp_buf[index] &= irs_u8((1 << bit_index)^0xFF);
    m_write_vector[index] = true;
  }
}

// Преобразователь mxnetc в mxdata_t со встроенным mxnetc
irs::mxnetc_data_t::mxnetc_data_t(mxifa_ch_t a_channel,
  const counter_t &a_connect_interval, const counter_t &a_update_interval
):
  mxdata_to_mxnet_t(IRS_NULL, a_connect_interval, a_update_interval),
  m_mxnetc(a_channel)
{
  this->connect(&m_mxnetc);
}
void irs::mxnetc_data_t::ip(mxip_t a_ip)
{
  m_mxnetc.set_dest_ip(a_ip);
}
void irs::mxnetc_data_t::port(irs_u16 a_port)
{
  m_mxnetc.set_dest_port(a_port);
}

