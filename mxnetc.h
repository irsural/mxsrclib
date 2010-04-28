// Протокол MxNetC (Max Network Client)
// Дата: 13.04.2010

#ifndef mxnetcH
#define mxnetcH

#include <irsdefs.h>

#include <mxnetd.h>
#include <mxnetda.h>
#include <mxifa.h>
#include <timer.h>
#include <irsstd.h>
#include <mxnetr.h>

#include <irsfinal.h>


// Версия (<старший байт>.<младший байт>)
#define MXNC_VERSION 0x0001


typedef enum _mxnc_status_t {
  mxnc_status_success = 0,
  mxnc_status_busy = 1,
  mxnc_status_error = 2
} mxnc_status_t;
typedef enum _mxnc_mode_t {
  mxnc_mode_free,
  mxnc_mode_packet,
  mxnc_mode_write,
  mxnc_mode_write_wait,
  mxnc_mode_read,
  //mxnc_mode_read_wait,
  mxnc_mode_begin_packet,
  mxnc_mode_bpack_abort,
  mxnc_mode_bcast_in_com,
  mxnc_mode_bcast_in_com_abort,
  mxnc_mode_chunk_read_wait,
  mxnc_mode_checksum,
  mxnc_mode_proc,
  mxnc_mode_reset,

  mxnc_mode_broadcast,
  mxnc_mode_broadcast_wait,
  mxnc_mode_command_abort_wait,
  mxnc_mode_timeout_abort_wait,
  mxnc_mode_broadcast_proc_wait,
  mxnc_mode_bcast_abort_wait
} mxnc_mode_t;
typedef enum _mxnc_error_t {
  mxnc_err_none = 0,
  mxnc_err_invalid_param = 1,
  mxnc_err_nomem = 2,
  mxnc_err_create = 3,
  mxnc_err_abort = 4,
  mxnc_err_busy = 5,
  mxnc_err_bad_server = 6,
  mxnc_err_remote_invalid_param = 7,
  mxnc_err_timeout = 8
} mxnc_error_t;

class mx_broadcast_proc_t;

// Класс для
class mxnetc
{
  // Буфер для приема/отправки пакета
  mxn_packet_t *f_packet;
  // Размер буфера, байт
  mxn_sz_t f_packet_size;
  // Размер принимаемого пакета, байт
  mxn_sz_t f_receive_size;
  // Размер отправляемого пакета, байт
  mxn_sz_t f_send_size;
  // Текущий режим
  mxnc_mode_t f_mode;
  // Режим после возврата из подрежима
  mxnc_mode_t f_mode_return;
  // Текущая команда
  mxn_cnt_t f_command;
  // Текущий статус операции
  mxnc_status_t f_status;
  // Статус операции передаваемый в mxnc_mode_reset
  mxnc_status_t f_local_status;
  // Указатель на переменную для версии
  irs_u16 *f_version;
  // Дескриптор канала
  void *f_handle_channel;
  // Ошибка создания класса
  irs_bool f_create_error;
  // Запрос прерывания операции
  irs_bool f_abort_request;
  // Позиция в пакете для поиска начала пакета
  //mxn_sz_t f_packet_pos;
  // Ожидание чтения для поиска начала пакета
  //irs_bool f_fbpc_wait_read;//!!!!!!!!
  // Признак выхода из fbpc по таймауту
  //irs_bool f_fbpc_end_to;//!!!!!!!!
  // Последняя ошибка
  mxnc_error_t f_last_error;
  // Указатель на ползователскую переменную количества переменных
  mxn_cnt_t *f_user_size;
  // Первая переменная для чтения/записи
  mxn_cnt_t f_user_index;
  // Количество переменных для чтения/записи
  mxn_cnt_t f_user_count;
  // Широковещательный режим заданный пользователем
  irs_bool f_user_broadcast;
  // Указатель на пользовательские переменные
  irs_i32 *f_user_vars;
  // Переменная таймаута сетевых операций
  counter_t f_net_to;
  // Переменная таймаута широковещательного режима
  counter_t f_broadcast_to;
  // Широковещательный таймаут запущен
  //irs_bool f_broadcast_to_en;
  // Буфер для приема широковещательного пакета
  //mxn_packet_t *f_broadcast_packet;
  // Размер буфера для приема широковещательного пакета
  //mxn_sz_t f_broadcast_packet_size;
  // Число переменных полученных при широковещательной рассылке
  //mxn_cnt_t f_broadcast_count;
  // Переменные полученые при широковещательной рассылке
  //irs_i32 *f_broadcast_vars;
  // Широковещательный режим модуля
  irs_bool f_broadcast_mode;
  // Переменная таймаута сетевых операций
  counter_t f_oper_to;
  // Переменная времени сетевых операций
  counter_t f_oper_t;
  // Буфер заголовка
  mxn_packet_t f_header;
  // Процедура поиска начала пакета
  irs::mx_beg_pack_proc_t *f_beg_pack_proc;
  // Обработка широковещательного пакета
  mx_broadcast_proc_t *f_broadcast_proc;
  // Текущий тип контрольной суммы
  irs::mxn_checksum_t f_checksum_type;

  // Запрещаем конструктор по умолчанию
  mxnetc();
  // Запрещение обработки
  irs_bool deny_proc();
  // Заполнение пакета
  irs_bool packet_fill(mxn_cnt_t code_comm, mxn_cnt_t packet_var_first,
    mxn_cnt_t packet_var_count, irs_i32 *vars, mxn_cnt_t var_count);
public:
  // Конструктор
  mxnetc(mxifa_ch_t channel);
  // Деструктор
  ~mxnetc();
  // Чтение версии протокола mxnet
  void get_version(irs_u16 *version);
  // Чтение размера массива (количество переменных)
  void get_size(mxn_cnt_t *size);
  // Чтение переменных
  void read(mxn_cnt_t index, mxn_cnt_t count, irs_i32 *vars);
  // Запись переменных
  void write(mxn_cnt_t index, mxn_cnt_t count, irs_i32 *vars);
  // Включение/выключение широковещательного режима
  void set_broadcast(irs_bool broadcast_mode);
  // Статус текущей операции
  mxnc_status_t status();
  // Элементарное действие
  void tick();
  // Прерывание операции
  void abort();
  // Чтение последней ошибки
  mxnc_error_t get_last_error();
  // Установка таймаута операций, с
  void set_timeout(calccnt_t t_num, calccnt_t t_denom);
  // Установка ip-адреса удаленного устройства
  void set_dest_ip(mxip_t ip);
  // Установка порта удаленного устройства
  void set_dest_port(irs_u16 port);
  // Установка локального порта
  void set_local_port(irs_u16 port);
  // Задание типа контрольной суммы
  void set_checksum_type(irs::mxn_checksum_t a_checksum_type);

  //MXNC_VERSION >= 0x0001
  // Чтение текущего режима
  irs_bool get_broadcast_mode();
};

class mx_broadcast_proc_t: public mx_proc_t
{
  typedef enum _mode_t {
    mode_start,
    mode_vars_wait,
    mode_stop
  } mode_t;

  mode_t f_mode;
  void *f_handle_channel;
  irs_u8 *f_buf;
  irs_bool f_abort_request;
  irs_bool f_proc;
  // Буфер для приема широковещательного пакета
  mxn_packet_t *f_broadcast_packet;
  // Размер буфера для приема широковещательного пакета
  mxn_sz_t f_broadcast_packet_size;
  // Переменные полученые при широковещательной рассылке
  irs_i32 *f_broadcast_vars;
  // Число переменных полученных при широковещательной рассылке
  mxn_cnt_t f_broadcast_count;
  irs_bool f_success;
  // Текущий тип контрольной суммы
  irs::mxn_checksum_t f_checksum_type;
public:
  mx_broadcast_proc_t(void *a_handle_channel,
    irs::mxn_checksum_t a_checksum_type);
  void operator()(irs_u8 *a_buf, irs::mxn_checksum_t a_checksum_type);
  virtual ~mx_broadcast_proc_t();
  virtual irs_bool tick();
  virtual void abort();
  irs_bool success();
  void get_vars(irs_i32 *user_var, mxn_cnt_t index, mxn_cnt_t count);
  mxn_cnt_t get_count();
};

namespace irs {

#define irs_mxdata_to_mxnet_def_interval ((counter_t)-1)

// Преобразование mxnetc в mxdata_t
class mxdata_to_mxnet_t : public mxdata_t
{
  static const mxn_sz_t m_size_var_byte = sizeof(irs_i32);
  static const irs_u8 m_high_bit = 7;

  enum m_status_t
  {
    GET_SIZE,
    WRITE,
    READ,
    FREE
  };
  m_status_t m_status;
  mxnetc *mp_mxnet;
  irs_bool m_is_connected;
  irs_u8 *mp_buf;
  irs_u8 *mp_read_buf;
  mxn_cnt_t m_mxnet_size;    //  в четырехбайтных переменных
  mxn_sz_t m_mxnet_size_byte;
  vector<bool> m_write_vector;
  irs_uarc m_current_index;   //  в байтах
  counter_t m_update_interval_to;
  counter_t m_update_interval_time;
  counter_t m_connect_counter;
  counter_t m_connect_interval;
  auto_ptr<mx_time_int_t> mp_measured_interval_alg;
  bool m_measured_interval_bad_alloc;
  bool m_first_connect;
public:
  mxdata_to_mxnet_t(mxnetc *ap_mxnet,
    const counter_t &a_connect_interval = irs_mxdata_to_mxnet_def_interval,
    const counter_t &a_update_interval = irs_mxdata_to_mxnet_def_interval);
  ~mxdata_to_mxnet_t();
  irs_uarc size();
  irs_bool connected();
  void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  double measured_interval();
  void tick();
};

} //namespace irs

#endif //mxnetcH

