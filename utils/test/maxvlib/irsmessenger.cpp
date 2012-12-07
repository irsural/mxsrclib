#include <irsmessenger.h>
// class message_recv_t
irs::channel_messages_t::channel_messages_t(irs::hardflow_t* ap_hardflow,
  size_type a_id_channel
):
  mp_hardflow(ap_hardflow),
  m_fixed_flow(mp_hardflow),
  m_id_channel(a_id_channel),
  m_heading_id_begin(*reinterpret_cast<int*>("m001")),  // 'm001'
  m_heading_id_end('='),
  m_recv_msg_heading_data(0),
  m_recv_msg_type(user_message),
  m_recv_msg_crc32(0),
  m_recv_msg_heading_crc32(0),
  m_recv_msg_size(0),
  m_heading_size(sizeof(heading_t)),
  m_pack_send_max_size(1024),
  m_recv_raw_data_buf_max_size(1400),
  m_send_service_msg_max_count(2),
  m_send_user_msg_max_count(2),
  m_send_delivery_confirmation_msg_max_count(1),
  m_recv_service_max_count(2),
  m_recv_user_msg_max_count(1),
  m_delivery_confirmation_time_out(irs::make_cnt_ms(2000)),
  m_timer_delivery_confirmation(),
  m_send_msg_status(msg_busy),
  m_recv_msg_status(msg_success),
  m_mode_message_recv(mmr_data_recv),
  m_mode_message_send(mms_busy_packege_for_send),
  recv_msg_heading_success(false),
  m_recv_msg_buf(),
  m_pack_buf_send(),
  m_data_recv_buf(),
  m_heading_buf(m_heading_size),
  m_service_messages_buf_send(),

  m_service_packages_buf_send(),
  m_service_packages_buf_send_max_size(2),
  m_user_msg_packages_buf_send(),
  m_user_msg_packages_buf_send_max_size(2),
  m_service_packages_buf_recv(),
  m_service_packages_buf_recv_max_size(2),
  m_user_msg_packages_buf_recv(),
  m_user_msg_packages_buf_recv_max_size(2),
  m_package_max_size(256),
  m_user_msg_delivery_confirm_timeout(make_cnt_ms(500)),
  m_user_msg_send_count(0),
  m_user_msg_send_max_count(3),
  m_user_messages_buf_send(),
  m_service_messages_buf_recv(),
  m_user_messages_buf_recv(),
  m_buf_send_cur_pos(0),
  m_buf_recv_cur_pos(0),
  m_last_recv_msg_id(0),
  m_delivered_msg_id()
{
  memset(m_heading_buf.data(), 0, m_heading_buf.size());
}

bool irs::channel_messages_t::message_send(
  const irs::raw_data_t<irs_u8>& a_msg,
  const irs_u64 a_data)
{
  bool message_copy_to_local_buf = false;
  if (m_user_messages_buf_send.size() < m_send_user_msg_max_count) {
    irs::raw_data_t<irs_u8> message;
    const message_type_t message_type = user_message;
    size_type message_id = make_message(a_msg, a_data, message_type, &message);
    send_message_t send_message(message, message_id);
    m_user_messages_buf_send.push_back(send_message);
    message_copy_to_local_buf = true;
  } else {
    // Буфер заполнен
  }
  return message_copy_to_local_buf;
}

bool irs::channel_messages_t::message_recv(
  irs::raw_data_t<irs_u8>* ap_msg, irs_u64* ap_data)
{
  bool message_exist = false;
  if (m_user_messages_buf_recv.size() > 0) {
    *ap_msg = m_user_messages_buf_recv.front().raw_message;
    *ap_data = m_user_messages_buf_recv.front().data;
    m_user_messages_buf_recv.pop_front();
    if (m_user_messages_buf_recv.size() == 0) {
      m_recv_msg_status == msg_busy;
    } else {
      // В буфере еще есть сообщения
    }
    message_exist = true;
  } else {
    //DC_ASSERT(m_recv_msg_status == msg_busy);
    // Нет полученных сообщений
  }
  return message_exist;
}

irs::channel_messages_t::message_status_t
irs::channel_messages_t::message_send_status() const
{
  return m_send_msg_status;
}

irs::channel_messages_t::message_status_t
irs::channel_messages_t::message_recv_status() const
{
  return m_recv_msg_status;
}

void irs::channel_messages_t::message_send_abort()
{
  m_pack_buf_send.clear();
  m_send_msg_status = msg_success;
}

void irs::channel_messages_t::write(
  irs::raw_data_t<irs_u8>::const_pointer a_first,
  irs::raw_data_t<irs_u8>::const_pointer a_last)
{
  if (m_mode_message_recv == mmr_data_recv) {
    size_type m_write_byte_count = min ((a_last - a_first),
      (m_recv_raw_data_buf_max_size - m_data_recv_buf.size()));
    irs::raw_data_t<irs_u8>::pointer p_end =
      m_data_recv_buf.data() + m_data_recv_buf.size();
    m_data_recv_buf.insert(p_end, a_first, (a_first + m_write_byte_count));
  } else {
    // Прием данных запрещен
  }
}

void irs::channel_messages_t::read(sender_t* ap_sender)
{
  if (ap_sender != IRS_NULL) {
    if (m_mode_message_send == mms_send_data) {
      if (it_cur_send_msg->data_send_size <
        it_cur_send_msg->message_data.size())
      {
        const irs_u8* first = it_cur_send_msg->message_data.data() +
          it_cur_send_msg->data_send_size;
        const size_type need_send_byte_count =
          (it_cur_send_msg->message_data.size() -
            it_cur_send_msg->data_send_size);

        size_type count = ap_sender->write(first, need_send_byte_count);
        it_cur_send_msg->data_send_size += count;
        MESSENGER_ASSERT(it_cur_send_msg->data_send_size <=
          it_cur_send_msg->message_data.size());
      } else {
        // Все данные из буфера уже отправлены
      }
    } else {
      // Интерфейс передачи не установлен
    }
  } else {
    // Отправка данных запрещена
  }
} 

void irs::channel_messages_t::tick()
{
  IRS_LIB_ASSERT(mp_hardflow);
  // Обработка получения сообщений
  switch(m_mode_message_recv) {
    case mmr_data_recv: {
      // Обработка массива полученных данных
      const size_type byte_count = m_data_recv_buf.size();
      size_type byte_extract_count = 0;
      size_type byte_data_i = 0;
      while (byte_data_i < byte_count) {
        irs_u8 byte_data = m_data_recv_buf[byte_data_i];
        byte_extract_count++;
        heading_buf_push_back(byte_data);
        heading_t heading;
        const bool heading_detect = detector_heading(&heading);
        if (!heading_detect) {
          if (recv_msg_heading_success) {
            // Увеличиваем размер буфера для приема сообщения на единицу
            m_recv_msg_buf.resize(m_recv_msg_buf.size() + 1);
            const size_type byte_count = 1;
            memcpy((m_recv_msg_buf.data() + m_recv_msg_buf.size() - 1),
              &byte_data, byte_count);
            if (m_recv_msg_buf.size() == m_recv_msg_heading.msg_size) {
              m_mode_message_recv = mmr_recv_data_processing;
              break;
            } else {
              // Сообщение еще не сформировано
            }
          } else {
            // Ждем обнаружения заголовка
          }
        } else {
          m_recv_msg_heading = heading;
          m_recv_msg_buf.clear();
          recv_msg_heading_success = true;
        }
        byte_data_i++;
      }
      m_data_recv_buf.erase(m_data_recv_buf.data(),
        m_data_recv_buf.data()+byte_extract_count);
    } break;
    case mmr_recv_data_processing: {
      //DC_DBG_MSG("Получено новое сообщение");
      // Проверяем целостность сообщения
      bool recv_message_valid =
        message_check(m_recv_msg_buf, m_recv_msg_heading);
      if (recv_message_valid) {
        MESSENGER_DBG_MSG("Целостность сообщения подтверждена");
        if (m_recv_msg_heading.msg_type == delivery_confirmation_message) {
          IRS_DBG_MSG("Cообщения относится к типу подтверждающих доставку");
          size_type delivered_msg_id = delivered_msg_id_get(m_recv_msg_status,
            m_recv_msg_heading);
          m_delivered_msg_id.push_back(delivered_msg_id);
        } else {
          m_last_recv_msg_id = m_recv_msg_heading.msg_number;
          // Создаем сообщение подтверждение доставки
          delivery_confirmation_send(m_recv_msg_heading.msg_number);
          if (m_recv_msg_heading.msg_type == type_msg_user) {
            MESSENGER_DBG_MSG("Cообщения относится к типу пользовательских");
            message_t message(m_recv_msg_buf, m_recv_msg_heading.user_data);
            m_user_messages_buf_recv.push_back(message);
            m_recv_msg_status = msg_success;
            m_mode_message_recv = mmr_msg_recv_success;
            break;
          } else if (m_recv_msg_heading.msg_type == service_message) {
            MESSENGER_DBG_MSG("Cообщения относится к типу сервисных");
            message_t message(m_recv_msg_buf, m_recv_msg_heading.user_data);
            m_service_messages_buf_recv.push_back(message);
          } else {
            // Неизвестный тип сообщения
          }
        }
      } else {
        MESSENGER_DBG_MSG("Сообщение повреждено");
        m_mode_message_recv = mmr_data_recv;
      }
      m_recv_msg_buf.clear();
      recv_msg_heading_success = false;
    } break;
    case mmr_msg_recv_success: {
      if (m_recv_msg_status == msg_busy) {
        if (m_service_messages_buf_send.size()  == 0) {
          m_mode_message_recv = mmr_data_recv;
        } else {
          // Ожидаем отправки всех сервисных сообщений
        }
      } else if (m_recv_msg_status == msg_success) {
        // Ожидаем пока пользователь считает полученное сообщение
      } else {
        MESSENGER_DBG_MSG("Неизвестный статус получаемого сообщения");
      }
    } break;
    default : {
      MESSENGER_ASSERT("Неизвестный режим");
    }
  }

  // Обработка отправки сообщений
  switch(m_mode_message_send) {
    /*case mms_busy_packege_for_send: {
      // Режим ожидания данных для отправки
      if ((m_service_packages_buf_send.size() > 0) ||
        (m_user_msg_packages_buf_send.size() > 0))
      {
        MESSENGER_DBG_MSG(
          "Появились пользовательские или сервисные сообщения для отправки");
        m_mode_message_send = mms_select_package_for_send;
      } else {
        // Ожидаем данные для посылки
      }
    } break;
    case mms_create_user_msg_pack_for_send: {
      create_and_push_back_to_buf_user_msg_package();
      m_mode_message_send = mms_select_package_for_send;
    } break;*/
    case mms_select_package_for_send: {
      // Сервисные сообщения имеют приоритет
      if (m_service_packages_buf_send.size() > 0) {
        MESSENGER_DBG_MSG("Отправляем сервисный пакет");
        mp_cur_packeges_buf_send = &m_service_packages_buf_send;
        m_fixed_flow.write(m_id_channel,
          m_service_packages_buf_send.front().data(),
          m_service_packages_buf_send.front().size());
        m_mode_message_send = mms_send_data;
      } else if (m_user_msg_packages_buf_send.size() > 0) {
        MESSENGER_DBG_MSG("Отправляем пакет пользовательского сообщения");
        mp_cur_packeges_buf_send = &m_user_msg_packages_buf_send;
        m_fixed_flow.write(m_id_channel,
          m_user_msg_packages_buf_send.front().data(),
          m_user_msg_packages_buf_send.front().size());
        m_mode_message_send = mms_send_data;
      } else {
        // Ожидаем пакетов для отправки
        //m_mode_message_send = mms_busy_packege_for_send;
      }
    } break;
    case mms_send_data: {
      if (m_fixed_flow.write_status() ==
        irs::hardflow::fixed_flow_t::status_success)
      {
        mp_cur_packeges_buf_send->pop_front();
        if (mp_cur_packeges_buf_send == &m_user_msg_packages_buf_send) {
          deque<irs::raw_data_t>::const_iterator it_user_msg =
            m_user_messages_buf_send.begin();
          if (m_user_msg_packages_buf_send.empty() &&
            (it_user_msg->message_data.fully_divided_into_packets))
          {
            m_user_msg_send_count++;
            // Сообщение отправлено, ожидаем подтверждения
            m_user_msg_delivery_confirm_timeout.start();
          } else {
            // Сообщение еще не отправлено
          }
        } else {
          // Остальные буферы пакетов не требуют подтвеждения отправки
        }
      } else if (m_fixed_flow.write_status() ==
        irs::hardflow::fixed_flow_t::status_error)
      {
        m_mode_message_send = mms_select_package_for_send;
      } else {
        // Ожидаем отправки данных
      }
    } break;
    default: {
      MESSENGER_ASSERT("Неизвестный режим");
    }
  }
  // Создаем пакеты из пользовательского сообщения, если оно есть
  create_and_push_back_to_buf_user_msg_package();
  if (!m_user_msg_delivery_confirm_timeout.stopped()) {
    // Ожидается подтверждение доставки пользовательского сообщения
    if (m_user_msg_delivery_confirm_timeout.check()) {
      if (m_user_msg_send_count < m_user_msg_send_max_count) {
        // Лимит посылок одного сообщения исчерпан, удаляем все сообщения
      } else {
        // Лимит количества посылок одного сообщения не исчерпан
      }
    } else {
      // Время ожидания подтверждения еще не вышло
    }
  } else {
    // Подтверждение доставки не ожидается
  }
  m_fixed_flow.tick();
}

bool irs::channel_messages_t::heading_check(const heading_t& a_heading)
{
  bool heading_valid = true;
  heading_valid = (a_heading.heading_id_begin == m_heading_id_begin);
  heading_valid = heading_valid &&
    (a_heading.heading_id_end == m_heading_id_end);

  if (heading_valid) {
    const size_type size = sizeof(a_heading) -
      (sizeof(a_heading.heading_crc32) + sizeof(a_heading.heading_id_end));
    // Проверяем чек сумму заголовка
    size_type heading_crc32 =
      irs::crc32_table(reinterpret_cast<const irs_u8*>(&a_heading),
        size);
    heading_valid = (heading_crc32 == a_heading.heading_crc32);
  } else {
    // Заголовок содержит недопустимые данные
  }
  return heading_valid;
}

void irs::channel_messages_t::erase_data(
  irs::raw_data_t<irs_u8>* ap_raw_data,
  size_type a_start_pos,
  size_type a_count)
{
  MESSENGER_ASSERT(ap_raw_data->size() >= (a_start_pos + a_count));
  irs_u8* data = ap_raw_data->data();
  irs_u8* dest = data+a_start_pos;
  irs_u8* src = dest + a_count;
  const size_type count = (data + ap_raw_data->size()) - src;
  memmove(dest, src, count);
  size_type new_size = ap_raw_data->size() - a_count;
  ap_raw_data->resize(new_size);
}

bool irs::channel_messages_t::message_check(
  const irs::raw_data_t<irs_u8>& a_message, const heading_t& a_heading)
{
  bool message_valid = false;
  irs_u32 message_crc32 = irs::crc32_table(a_message.data(),
    a_message.size());
  if (message_crc32 == a_heading.msg_crc32) {
    message_valid = true;
  } else {
    // Сообщение повреждено
  }
  return message_valid;
}

void irs::channel_messages_t::pack_create(const irs_u8* ap_buf,
  const size_type a_size)
{
  const size_type pack_size = min(m_pack_send_max_size, a_size);

  size_type message_id = 0;
  ap_message->clear();
  heading_t heading;
  heading_create(ap_buf, a_data, a_message_type,
    &heading);
  message_id = heading.msg_number;
  irs_u8* p_first = reinterpret_cast<irs_u8*>(&heading);
  irs_u8* p_last = p_first + sizeof(heading);
  ap_message->insert(ap_message->data() + ap_message->size(), p_first, p_last);

  const irs_u8* p_raw_msg_first = a_raw_message.data();
  const irs_u8* p_raw_msg_last = p_raw_msg_first + a_raw_message.size();
  ap_message->insert(ap_message->data() + ap_message->size(),
    p_raw_msg_first, p_raw_msg_last);
  return message_id;

  m_pack_buf_send.clear();
  m_pack_buf_send.insert(0, ap_buf, ap_buf + pack_size);
}

void irs::channel_messages_t::create_and_push_back_to_buf_user_msg_package()
{
  if ((m_user_messages_buf_send.size() > 0) &&
    (m_user_msg_packages_buf_send.size() <
    m_user_msg_packages_buf_send_max_size))
  {
    deque<irs::raw_data_t>::iterator it_user_msg =
      m_user_messages_buf_send.begin();
    if (it_user_msg->message_data.fully_divided_into_packets) {
      user_msg_pack_heading_t user_msg_pack_heading;
      user_msg_pack_heading.heading_id_begin = m_heading_id_begin;
      user_msg_pack_heading.pack_type = pack_type_user_msg;
      user_msg_pack_heading.data_pos = it_user_msg->data_send_size;
      user_msg_pack_heading.data_size = min(m_package_max_size,
        it_user_msg->message_data.size() - it_user_msg->data_send_size)
      irs_u8* data_begin = it_user_msg->message_data.data() +
        it_user_msg->data_send_size;
      user_msg_pack_heading.pack_data_crc32 = irs::crc32_table(
        data_begin, data_begin + user_msg_pack_heading.data_size);
      user_msg_pack_heading.heading_crc32 = irs::crc32_table(
        reinterpret_cast<irs_u8*>(user_msg_pack_heading),
        reinterpret_cast<irs_u8*>(user_msg_pack_heading) +
          (sizeof(user_msg_pack_heading) -
            sizeof(user_msg_pack_heading->heading_crc32))
      );
      irs::raw_data_t package;
      package.insert(0,
        reinterpret_cast<irs_u8*>(user_msg_pack_heading),
        reinterpret_cast<irs_u8*>(user_msg_pack_heading) +
        sizeof(user_msg_pack_heading)
      );
      package.insert(package.data() + package.size(),
        it_user_msg->message_data.data(), it_user_msg->message_data.data() +
          user_msg_pack_heading.data_size);
      it_user_msg->data_send_size += user_msg_pack_heading.data_size;
      if (it_user_msg->data_send_size == it_user_msg->message_data.size()) {
        it_user_msg->message_data.fully_divided_into_packets = true;
      } else {
        // Объект еще не полностью разбит на пакеты
      }
    } else {
      // Все данные отправлены
    }
  } else {
    // Нет сообщений или буфер пакетов заполнен
  }
}

bool irs::channel_messages_t::detector_heading(heading_t* ap_heading)
{
  bool heading_valid = true;
  heading_t heading;
  heading = *reinterpret_cast<heading_t*>(m_heading_buf.data());
  heading_valid = (heading.heading_id_begin == m_heading_id_begin);
  heading_valid = heading_valid && (heading.heading_id_end == m_heading_id_end);

  if (heading_valid) {
    const size_type size = sizeof(heading) -
      (sizeof(heading.heading_crc32) + sizeof(heading.heading_id_end));
    // Проверяем чек сумму заголовка
    size_type heading_crc32 =
      irs::crc32_table(reinterpret_cast<const irs_u8*>(&heading),
      size);
    heading_valid = (heading_crc32 == heading.heading_crc32);
  } else {
    // Заголовок содержит недопустимые данные
  }
  if (heading_valid) {
    // Заголовок правильный
    *ap_heading = heading;
  } else {
    // Кусочек данных, Заполняющих буфер заголовка, не является заголовком
  }
  return heading_valid;
}

// class messenger_t
irs::messenger_t::messenger_t(irs::hardflow_t* ap_hardflow):
  mp_hardflow(ap_hardflow),
  m_id_channel_map(),
  m_it_cur_channel(m_id_channel_map.end()),
  m_it_cur_channel_user(m_id_channel_map.end()),
  m_it_cur_channel_for_exists_check(m_id_channel_map.end()),
  m_channel_exists_check_loop_timer(make_cnt_ms(1000))
{ }

bool irs::messenger_t::channel_exists(const id_type a_id)
{ 
  return m_id_channel_map.find(a_id) != m_id_channel_map.end();
}

irs::messenger_t::size_type irs::messenger_t::channel_count() const
{ 
  return m_id_channel_map.size();
}

bool irs::messenger_t::message_send(const id_type a_id,
  const irs::raw_data_t<irs_u8>& a_message, const irs_u64 a_data)
{
  bool message_send_success = false;
  id_channel_map_iterator it_id_channel = m_id_channel_map.find(a_id);
  if (it_id_channel != m_id_channel_map.end()) {
    message_send_success =
      it_id_channel->second.message_send(a_message, a_data);
  } else {
    MESSENGER_DBG_MSG("Канала с таким идентификатором не существует");
  }
  return message_send_success;
}

bool irs::messenger_t::message_recv(const id_type a_id,
  irs::raw_data_t<irs_u8>* ap_message,
  irs_u64* ap_data)
{
  bool message_recv_success = false;
  id_channel_map_iterator it_id_channel = m_id_channel_map.find(a_id);
  if (it_id_channel != m_id_channel_map.end()) {
    message_recv_success =
      it_id_channel->second.message_recv(ap_message, ap_data);
  } else {
    MESSENGER_DBG_MSG("Канала с таким идентификатором не существует");
  }
  return message_recv_success;
}

irs::channel_messages_t::message_status_t
irs::messenger_t::message_send_status(const id_type a_id)
{
  channel_messages_t::message_status_t msg_send_status =
    channel_messages_t::msg_invalid_status;
  id_channel_map_iterator it_id_channel = m_id_channel_map.find(a_id);
  if (it_id_channel != m_id_channel_map.end()) {
    msg_send_status =
      it_id_channel->second.message_send_status();
  } else {
    MESSENGER_DBG_MSG("Канала с таким идентификатором не существует");
  }
  return msg_send_status;
}

void irs::messenger_t::message_send_abort(const id_type a_id)
{  
  id_channel_map_iterator it_id_channel = m_id_channel_map.find(a_id);
  if (it_id_channel != m_id_channel_map.end()) {
    it_id_channel->second.message_send_abort();
  } else {
    MESSENGER_DBG_MSG("Канала с таким идентификатором не существует");
  }
}

irs::messenger_t::size_type irs::messenger_t::channel_next()
{
  size_type cur_channel_user_id = invalid_channel;
  if (m_it_cur_channel_user != m_id_channel_map.end()) {
    ++m_it_cur_channel_user;
  } else {
    m_it_cur_channel_user != m_id_channel_map.begin();
  }
  if (m_it_cur_channel_user != m_id_channel_map.end()) {
    cur_channel_user_id = m_it_cur_channel_user->first;
  } else {
    // Нет каналов
  }
  return cur_channel_user_id;
}

void irs::messenger_t::tick()
{
  if (mp_hardflow) {
    if (m_channel_exists_check_loop_timer.check()) {
      if (m_it_cur_channel_for_exists_check != m_id_channel_map.end()) {
        ++m_it_cur_channel_for_exists_check;
      } else {
        m_it_cur_channel_for_exists_check = m_id_channel_map.begin();
      }
      if (m_it_cur_channel_for_exists_check != m_id_channel_map.end()) {
        if (!mp_hardflow->is_channel_exists(
          m_it_cur_channel_for_exists_check->first))
        {
          id_channel_map_iterator it_erase_channel =
            m_it_cur_channel_for_exists_check;
          ++m_it_cur_channel_for_exists_check;
          if (it_erase_channel == m_it_cur_channel) {
            m_it_cur_channel++;
          } else {
            // Смена текущего канала не требуется
          }
          m_id_channel_map.erase(it_erase_channel);
        } else {
          // Канал существует
        }
        ++m_it_cur_channel_for_exists_check;
      } else {
        // Нет каналов
      }
    } else {
      // Промежуток времени еще не пройден
    }
    const size_type id_channel = mp_hardflow->channel_next();
    if (id_channel != irs::hardflow_t::invalid_channel) {
      if (m_id_channel_map.find(id_channel) == m_id_channel_map.end()) {
        m_id_channel_map.insert(
          make_pair(id_channel, channel_t(mp_hardflow, id_channel)));
      } else {
        // Канал существует
      }
    } else {
      // Неизвестный канал
    }
  } else {
    MESSENGER_DBG_MSG("hardflow не установлен");
  }
  if (m_it_cur_channel != m_id_channel_map.end()) {
    ++m_it_cur_channel;
  } else {
    m_it_cur_channel = m_id_channel_map.begin();
  }
  if (m_it_cur_channel != m_id_channel_map.end()) {
    m_it_cur_channel->second.tick();
  } else {
    // Канал не установлен
  }
}

