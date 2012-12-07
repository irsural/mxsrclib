#ifndef irsmessengerH
#define irsmessengerH

#include <irscpp.h>
#include <hardflowg.h>
#include <irsalg.h>

#define MESSENGER_DEBUG

#include <irserror.h>
#include <irsstrm.h>

#ifdef MESSENGER_DEBUG
#define MESSENGER_ASSERT(assert_expr) IRS_ASSERT(assert_expr)
#define MESSENGER_ASSERT_EX(assert_expr, msg) IRS_ASSERT_EX(assert_expr, msg)
#define MESSENGER_ERROR(error_code, msg) IRS_ERROR(error_code, msg)
#define MESSENGER_ERROR_IF_NOT(assert_expr, error_code, msg)\
  IRS_LIB_ERROR_IF_NOT(assert_expr, error_code, msg)
#define MESSENGER_FATAL_ERROR(msg) IRS_FATAL_ERROR(msg)
#define MESSENGER_ASSERT_MSG(msg) IRS_ASSERT_MSG(msg)
#define MESSENGER_DBG_MSG(msg) IRS_DBG_MSG(msg)
#define MESSENGER_DBG_MSG_SRC(msg) IRS_DBG_MSG_SRC(msg)
#else // !MESSENGER_DEBUG
#define MESSENGER_ASSERT(assert_expr)
#define MESSENGER_ASSERT_EX(assert_expr, msg)
#define MESSENGER_ERROR(error_code, msg)
#define MESSENGER_FATAL_ERROR(msg))
#define MESSENGER_ASSERT_MSG(msg)
#define MESSENGER_DBG_MSG(msg)
#endif // !MESSENGER_DEBUG

namespace irs {

enum message_type_t {invalid_message_type, delivery_confirmation_message,
  service_message, user_message};

class sender_t
{
public:
  typedef size_t size_type;
  sender_t(irs::hardflow_t* ap_hardflow, const size_type a_channel_ident,
  const size_type a_msg_max_size = 1400
  ):
    mp_hardflow(ap_hardflow),
    m_channel_ident(a_channel_ident),
    m_msg_max_size(a_msg_max_size)
  {
    IRS_ASSERT(ap_hardflow != IRS_NULL);
  }
  inline size_type write(const irs_u8* ap_buf,
    const size_type a_buf_size)
  {
    size_type write_byte_count = 0;
    if (mp_hardflow != IRS_NULL) {
      const size_type buf_size = min(a_buf_size, m_msg_max_size);
      write_byte_count = mp_hardflow->write(m_channel_ident, ap_buf, buf_size);
    } else {
      MESSENGER_ASSERT_MSG("Интерфейс связи не установлен");
    }
    return write_byte_count;
  }
private:
  irs::hardflow_t* mp_hardflow;
  size_type m_channel_ident;
  size_type m_msg_max_size;
  sender_t();
};

class channel_messages_t
{
public:
  typedef size_t size_type;
  enum mode_message_recv_t {
    mmr_invalid,                  // Недопустимый статус
    mmr_data_recv,                // Получение данных
    mmr_recv_data_processing,     // Обработка полученных данных
    mmr_msg_recv_success};        // Данные успешно получены
  enum mode_message_send_t {
    mms_invalid,                  // Недопустимый статус
    mms_busy_packege_for_send,    // Ожидание данных для отправки
    mms_select_package_for_send,  // Выбор отправляемого сообщения
    mms_send_data,                // Отправка данных
    mms_delivery_confirmation};   // Подтверждение доставки
  enum message_status_t {
    msg_invalid_status,           // Недопустимый статус
    msg_success,                  // Успех
    msg_busy};                    // Ожидание
  channel_messages_t(irs::hardflow_t* ap_hardflow, size_type a_id_channel);
  // Функция копирует сообщение во внутренний буфер для отправки
  // Возвращает true, если в буфере есть место
  bool message_send(const irs::raw_data_t<irs_u8>& a_message,
    const irs_u64 a_data);
  // Функция достает сообщение из внутреннего буфера
  // Возвращает true, если в буфере было сообщение
  bool message_recv(irs::raw_data_t<irs_u8>* ap_message, irs_u64* ap_data);
  // Состояние отправки сообщений
  message_status_t message_send_status() const;
  // Состояние получения сообщений
  message_status_t message_recv_status() const;
  // Отменить передачу данных
  void message_send_abort();
  // Запись данных
  void write(irs::raw_data_t<irs_u8>::const_pointer a_first,
    irs::raw_data_t<irs_u8>::const_pointer a_last);
  // Чтение данных
  void read(sender_t* ap_sender);
  void tick();
private:
  channel_messages_t();
  enum message_type_t {
    type_msg_invalid,
    type_msg_delivery_confirmation,
    type_msg_service,
    type_msg_user,
  };
  enum pack_type_t {
    pack_type_invalid,
    pack_type_user_msg,
    pack_confirm_delivery,
    pack_type_service_msg,
    pack_type_request_send_user_msg,
    pack_type_confirm_delivery_user_msg
  };
  // Структура должна быть без выравнивания
  #pragma pack( push, 1 )
  // Заголовок пользовательского сообщения
  struct user_msg_heading_t
  {
    irs_u8 msg_id;          // Идентификатор сообщения
    irs_u32 msg_size;        // Размер сообщения
    irs_u32 msg_crc32;       // Контрольная сумма сообщения
    irs_u32 heading_crc32;   // Контрольная сумма заголовка сообщения
  };
  // Заголовок пакета пользовательского сообщения
  struct user_msg_pack_heading_t
  {
    irs_u32 heading_id_begin; // Идентификатор начала пакета
    irs_u8 pack_type;         // Тип пакета
    irs_u32 data_pos;         // Позиция части сообщения в исходном сообщении
    irs_u32 data_size;        // Размер части сообщения
    irs_u32 pack_data_crc32;  // Контрольная сумма данных пакета
    irs_u32 heading_crc32;    // Контрольная сумма заголовка пакета
  };
  // Заголовок сервисного пакета
  struct service_pack_heading_t
  {
    irs_u32 heading_id_begin; // Идентификатор начала пакета
    irs_u8 pack_type;         // Тип пакета
    irs_u8 type_service_data; // Тип сервисных данных
    irs_u64 service_data;     // Сервисные данные
    irs_u32 pack_crc32;       // Контрольная сумма пакета
  };
  #pragma pack( pop )
  irs::hardflow_t* mp_hardflow;
  irs::hardflow::fixed_flow_t m_fixed_flow;
  const m_id_channel;
  heading_t m_recv_msg_heading;
  const irs_u32 m_heading_id_begin;
  const irs_u8 m_heading_id_end;
  irs_u64 m_recv_msg_heading_data;
  irs_u8 m_recv_msg_type;
  irs_u32 m_recv_msg_crc32;
  irs_u32 m_recv_msg_heading_crc32;
  irs_u64 m_recv_msg_size;
  const irs_u32 m_heading_size;
  const size_type m_pack_send_max_size;

  size_type m_recv_raw_data_buf_max_size;
  size_type m_send_service_msg_max_count;
  size_type m_send_user_msg_max_count;
  size_type m_send_delivery_confirmation_msg_max_count;
  size_type m_recv_service_max_count;
  size_type m_recv_user_msg_max_count;
  counter_t m_delivery_confirmation_time_out;
  irs::timer_t m_timer_delivery_confirmation;
  message_status_t m_send_msg_status;
  message_status_t m_recv_msg_status;
  mode_message_recv_t m_mode_message_recv;
  mode_message_send_t m_mode_message_send;
  bool recv_msg_heading_success;
  irs::raw_data_t<irs_u8> m_recv_msg_buf;
  irs::raw_data_t<irs_u8> m_pack_buf_send;
  irs::deque_data_t<irs_u8> m_data_recv_buf;
  irs::raw_data_t<irs_u8> m_heading_buf;
  struct message_t
  {
    irs::raw_data_t<irs_u8> raw_message; // Сообщение без заголовка
    message_t(): raw_message() { }
    message_t(const irs::raw_data_t<irs_u8>& a_raw_message
    ): raw_message(a_raw_message)
    { }
  };

  struct send_message_t
  {
    const irs::raw_data_t<irs_u8> message_data;
    const size_type id;
    size_type data_send_size;
    bool fully_divided_into_packets;
    send_message_t(
      const irs::raw_data_t<irs_u8>& a_message_data,
      const size_type a_id,
      const size_type a_data_send_size = 0,
    ):
      message_data(a_message_data),
      id(a_id),
      data_send_size(a_data_send_size),
      fully_divided_into_packets(false)
    { }
  private:
    send_message_t();
  };
  // Буфер сервисных пакетов для отправки
  deque<irs::raw_data_t<irs_u8> > m_service_packages_buf_send;
  const size_type m_service_packages_buf_send_max_size;
  // Буфер пакетов пользовательского сообщения для отправки
  deque<irs::raw_data_t<irs_u8> > m_user_msg_packages_buf_send;
  const size_type m_user_msg_packages_buf_send_max_size;
  // Буфер принятых сервисных пакетов
  deque<irs::raw_data_t<irs_u8> > m_service_packages_buf_recv;
  const size_type m_service_packages_buf_recv_max_size;
  // Буфер принятых пакетов пользовательского сообщения
  deque<irs::raw_data_t<irs_u8> > m_user_msg_packages_buf_recv;
  const size_type m_user_msg_packages_buf_recv_max_size;
  const size_type m_package_max_size;
  irs::timer_t m_user_msg_delivery_confirm_timeout;
  size_type m_user_msg_send_count;
  size_type m_user_msg_send_max_count;
  // Буфер пользовательских сообщений для отправки
  deque<send_message_t> m_user_messages_buf_send;
  // Буфер сообщений подтверждения доставки
  deque<send_message_t> m_delivery_confirmation_messages_buf_send;
  // Буфер полученных сервисных сообщений
  deque<message_t> m_service_messages_buf_recv;
  // Буфер полученных пользовательских сообщений
  deque<message_t> m_user_messages_buf_recv;

  deque<irs::raw_data_t<irs_u8> >* mp_cur_packeges_buf_send;
  deque<send_message_t>::iterator it_cur_send_msg;
  message_type_t m_message_recv_type;
  size_type m_buf_send_cur_pos;
  size_type m_buf_recv_cur_pos;
  // Идентификатор последнего полученного сообщения
  // Сообщения типа "подтверждение доставки" не учитываются
  size_type m_last_recv_msg_id;
  // Идентификатор последнего подтвержденного сообщения
  // Сообщения типа "подтверждение доставки" не учитываются
  typedef list<size_type> delivered_msg_ids_type;
  delivered_msg_ids_type m_delivered_msg_id;
  //size_type m_last_delivery_confirmation_messages_id;
  // Идентификатор текущего сообщения, ожидающего подтверждение
  // Сообщения типа "подтверждение доставки" не учитываются
  size_type m_cur_msg_id_busy_delivery_confirmation;

  messenger_hannel_t();
  bool heading_check(const heading_t& a_heading);
  inline void heading_buf_push_back(const irs_u8 a_byte_data);
  bool detector_heading(heading_t* ap_heading);
  void erase_data(irs::raw_data_t<irs_u8>* ap_raw_data, size_type a_start_pos,
    size_type a_count);
  void heading_create(const irs::raw_data_t<irs_u8>& a_message,
    const irs_u64 a_user_data,
    const message_type_t a_message_type,
    heading_t* ap_heading);
  bool message_check(const irs::raw_data_t<irs_u8>& a_message,
    const heading_t& a_heading);
  void pack_create(const irs_u8* ap_buf, const size_type a_size);
  void create_and_push_back_to_buf_user_msg_package();
  inline size_type make_message(
    const irs::raw_data_t<irs_u8>& a_raw_message,
    const irs_u64 a_data,
    const message_type_t a_message_type,
    irs::raw_data_t<irs_u8>* ap_message);
  inline void delivery_confirmation_send(const size_type a_message_index);
  inline size_type delivered_msg_id_get(
    const irs::raw_data_t<irs_u8>& a_message,
    const heading_t& a_heading);
};

inline void irs::channel_messages_t::heading_buf_push_back(
  const irs_u8 a_byte_data)
{
  MESSENGER_ASSERT(m_heading_buf.size() == m_heading_size);
  //Делаем сдвиг на один байт вперед
  memmove(m_heading_buf.data(), (m_heading_buf.data()+1), m_heading_size-1);
  // В последний элемент массива записываем новый байт инфомации
  m_heading_buf[m_heading_buf.size()-1] = a_byte_data;
}

inline channel_messages_t::size_type
channel_messages_t::make_message(
  const irs::raw_data_t<irs_u8>& a_raw_message,
  const irs_u64 a_data,
  const message_type_t a_message_type,
  irs::raw_data_t<irs_u8>* ap_message)
{
  size_type message_id = 0;
  ap_message->clear();
  heading_t heading;
  message_id = heading.msg_number;
  irs_u8* p_first = reinterpret_cast<irs_u8*>(&heading);
  irs_u8* p_last = p_first + sizeof(heading);
  ap_message->insert(ap_message->data() + ap_message->size(), p_first, p_last);

  const irs_u8* p_raw_msg_first = a_raw_message.data();
  const irs_u8* p_raw_msg_last = p_raw_msg_first + a_raw_message.size();
  ap_message->insert(ap_message->data() + ap_message->size(),
    p_raw_msg_first, p_raw_msg_last);
  return message_id;
}

inline void channel_messages_t::delivery_confirmation_send(
  const size_type a_message_index)
{
  // Пустое сообщение
  irs::raw_data_t<irs_u8> raw_message;
  irs::raw_data_t<irs_u8> message;
  const message_type_t message_type = delivery_confirmation_message;
  size_type message_id = make_message(raw_message, a_message_index,
    message_type, &message);
  send_message_t send_message(message, message_id);
  m_delivery_confirmation_messages_buf_send.push_back(send_message);
}

inline channel_messages_t::size_type
channel_messages_t::delivered_msg_id_get(
  const irs::raw_data_t<irs_u8>& a_message,
  const heading_t& a_heading)
{
  return a_heading.msg_number;
}

class messenger_t
{
public:
  typedef sizens_t size_type;
  typedef string_t string_type;
  typedef sockaddr_in adress_type;
  typedef size_type id_type;
  typedef channel_messages_t channel_t;
  typedef map<id_type, channel_t> id_channel_map_type;
  typedef map<id_type, channel_t>::iterator id_channel_map_iterator;
  enum {
    invalid_channel = irs::hardflow_t::invalid_channel
  };
  messenger_t(irs::hardflow_t* ap_hardflow);
  bool channel_exists(const id_type a_id);
  void channel_destroy(const id_type a_id);
  void channel_all_destroy();
  size_type channel_count() const;
  void channel_max_count(size_type a_max_size);
  bool message_send(const id_type a_id,
    const irs::raw_data_t<irs_u8>& a_message, const irs_u64 a_data = 0);
  bool message_recv(const id_type a_id, irs::raw_data_t<irs_u8>* ap_message,
    irs_u64* ap_data = IRS_NULL);
  channel_messages_t::message_status_t message_send_status(const id_type a_id);
  void message_send_abort(const id_type a_id);
  size_type channel_next();
  void tick();
private:
  irs::hardflow_t* mp_hardflow;
  id_channel_map_type m_id_channel_map;
  id_channel_map_iterator m_it_cur_channel;
  id_channel_map_iterator m_it_cur_channel_user;
  id_channel_map_iterator m_it_cur_channel_for_exists_check;       
  loop_timer_t m_channel_exists_check_loop_timer;
  messenger_t();
};

} // namespace irs

#endif
