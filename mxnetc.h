//! \file
//! \ingroup network_in_out_group
//! \brief Протокол MxNetC (Max Network Client)
//!
//! Дата: 21.04.2011\n
//! Ранняя дата: 12.02.2009

#ifndef mxnetcH
#define mxnetcH

// Номер файла
#define MXNETCH_IDX 18

#include <irsdefs.h>

#include <mxnetd.h>
#include <mxnetda.h>
#include <timer.h>
#include <irsstd.h>
#include <mxnetr.h>
#include <mxifa.h>

#include <irsfinal.h>

//! \addtogroup network_in_out_group
//! @{

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

// Клиент протокола mxnet
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

//! @}

namespace irs {

//! \addtogroup network_in_out_group
//! @{

#define irs_mxdata_to_mxnet_def_interval ((counter_t)-1)

//! \brief Преобразование mxnetc в mxdata_t
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
protected:
  void connect(mxnetc *ap_mxnet);
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

//! \brief Преобразователь mxnetc в mxdata_t со встроенным mxnetc
class mxnetc_data_t: public mxdata_to_mxnet_t
{
public:
  mxnetc_data_t(
    mxifa_ch_t a_channel,
    const counter_t &a_connect_interval = irs_mxdata_to_mxnet_def_interval,
    const counter_t &a_update_interval = irs_mxdata_to_mxnet_def_interval
  );
  void ip(mxip_t a_ip);
  void port(irs_u16 a_port);
private:
  mxnetc m_mxnetc;
};

//! \brief Клиент протокола mxnet, работающий через hardflow_t
class mxnet_client_command_t
{
public:
  enum status_t {
    mxnc_status_success = 0,
    mxnc_status_busy = 1,
    mxnc_status_error = 2
  };
  enum error_t {
    mxnc_err_none = 0,
    mxnc_err_invalid_param = 1,
    mxnc_err_nomem = 2,
    mxnc_err_create = 3,
    mxnc_err_abort = 4,
    mxnc_err_busy = 5,
    mxnc_err_bad_server = 6,
    mxnc_err_remote_invalid_param = 7,
    mxnc_err_timeout = 8
  };

  //! \brief Конструктор
  mxnet_client_command_t(hardflow_t& a_hardflow);
  //! \brief Деструктор
  ~mxnet_client_command_t();
  //! \brief Чтение версии протокола mxnet
  void get_version(irs_u16 *version);
  //! \brief Чтение размера массива (количество переменных)
  void get_size(mxn_cnt_t *size);
  //! \brief Чтение переменных
  void read(mxn_cnt_t index, mxn_cnt_t count, irs_i32 *vars);
  //! \brief Запись переменных
  void write(mxn_cnt_t index, mxn_cnt_t count, irs_i32 *vars);
  //! \brief Статус текущей операции
  status_t status();
  //! \brief Элементарное действие
  void tick();
  //! \brief Прерывание операции
  void abort();
  //! \brief Чтение последней ошибки
  error_t get_last_error();
  //! \brief Установка таймаута операций, с
  void disconnect_time(counter_t a_time);
  //! \brief Установка ip-адреса удаленного устройства
  void ip(mxip_t a_ip);
  //! \brief Установка порта удаленного устройства
  void port(irs_u16 port);
  //! \brief Задание типа контрольной суммы
  void checksum_type(irs::mxn_checksum_t a_checksum_type);

private:
  typedef mx_beg_pack_proc_fix_flow_t beg_pack_type;
  typedef hardflow::fixed_flow_t::status_t ff_status_type;

  enum mode_t {
    mxnc_mode_free,
    mxnc_mode_packet,
    mxnc_mode_write,
    mxnc_mode_write_wait,
    mxnc_mode_read,
    mxnc_mode_begin_packet,
    mxnc_mode_chunk_read_wait,
    mxnc_mode_checksum,
    mxnc_mode_proc,
    mxnc_mode_reset,
  };

  //! \brief Коммуникационный канал
  hardflow_t& m_hardflow;
  //! \brief Адаптер коммуникационного канала
  hardflow::fixed_flow_t m_fixed_flow;
  //! \brief Поиск начала пакета
  beg_pack_type m_beg_pack_proc;
  //! \brief Текущий идентификатор канала
  hardflow_t::size_type m_channel_ident;
  //! \brief Буфер для приема/отправки пакета
  irs::raw_data_t<irs_u8> m_packet_data;
  //! \brief Структура пакета
  mxn_packet_t* mp_packet;
  //! \brief Размер принимаемого пакета, байт
  mxn_sz_t m_receive_size;
  //! \brief Размер отправляемого пакета, байт
  mxn_sz_t m_send_size;
  //! \brief Текущий режим
  mode_t m_mode;
  //! \brief Режим после возврата из подрежима
  mode_t m_mode_return;
  //! \brief Текущая команда
  mxn_cnt_t m_command;
  //! \brief Текущий статус операции
  status_t m_status;
  //! \brief Указатель на переменную для версии
  irs_u16* mp_version;
  //! \brief Ошибка создания класса
  bool m_create_error;
  //! \brief Последняя ошибка
  error_t m_last_error;
  //! \brief Указатель на ползователскую переменную количества переменных
  mxn_cnt_t* mp_user_size;
  //! \brief Первая переменная для чтения/записи
  mxn_cnt_t m_user_index;
  //! \brief Количество переменных для чтения/записи
  mxn_cnt_t m_user_count;
  //! \brief Указатель на пользовательские переменные
  irs_i32* mp_user_vars;
  //! \brief Текущий тип контрольной суммы
  irs::mxn_checksum_t m_checksum_type;

  //! \brief Запрещаем конструктор по умолчанию
  mxnet_client_command_t();
  //! \brief Запрещение обработки
  bool deny_proc();
  //! \brief Заполнение пакета
  void packet_fill(mxn_cnt_t a_code_comm, mxn_cnt_t a_packet_var_first,
    mxn_cnt_t a_packet_var_count, irs_i32 *ap_vars, mxn_cnt_t a_var_count);
  //! \brief Сброс основных параметров алгоритма в исходное состояние
  void reset_parametrs();
};


//! \brief Структура для очереди байтовых операций mxnet_client_t
struct mxnet_client_queue_item_t
{
  irs_uarc index;
  raw_data_t<irs_u8> data;

  mxnet_client_queue_item_t():
    index(0),
    data()
  {
  }
};

//! \brief Клиент протокола mxnet, реализующий интерфейс mxdata_t,
//! работающий через hardflow_t
class mxnet_client_t: public mxdata_t
{
public:
  mxnet_client_t(hardflow_t& a_hardflow);
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index,
    irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void tick();
private:
  typedef mxnet_client_queue_item_t queue_item_type;

  enum mode_t {
    mode_free
  };

  enum {
    m_size_var_byte = sizeof(irs_i32),
  };

  mode_t m_mode;
  irs_uarc m_size;
  bool m_is_connected;
  irs::raw_data_t<irs_u32> m_data_vars;
  raw_data_view_t<irs_u8, irs_u32> m_data_bytes;
  deque<queue_item_type> m_write_queue;
  mxnet_client_command_t m_mxnet_client_command;

  void fill_write_flags(irs_uarc a_index, irs_uarc a_size, bool a_value);
};

//! @}

} //namespace irs

#endif //mxnetcH

