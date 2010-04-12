// Коммуникационные потоки
// Дата: 09.04.2010
// Дата создания: 8.09.2009

#include <hardflowg.h>
#include <irscpp.h>
#include <mxdata.h>

#if defined(IRS_WIN32) || defined(IRS_LINUX)

#if defined(IRS_WIN32)
bool irs::hardflow::lib_socket_load(WSADATA* ap_wsadata, int a_version_major,
  int a_version_minor)
{
  bool lib_load_success = true;
  WORD version_requested = 0;
  IRS_LOBYTE(version_requested) = static_cast<irs_u8>(a_version_major);
  IRS_HIBYTE(version_requested) = static_cast<irs_u8>(a_version_minor);
  if (WSAStartup(version_requested, ap_wsadata) == 0) {
    // Библиотека удачно загружена
    IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Библиотека сокетов удачно загружена");
  } else {
    // Ошибка при загрузке библиотеки
    IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
      error_str(error_sock_t().get_last_error()));
    lib_load_success = false;
  }     
  return lib_load_success;
}
#endif // defined(IRS_WIN32)

//class udp_channel_list_t
irs::hardflow::udp_channel_list_t::udp_channel_list_t(
  const udp_limit_connections_mode_t a_mode,
  const size_type a_max_size,
  const size_type a_channel_buf_max_size,
  const bool a_limit_lifetime_enabled,
  const double a_max_lifetime_sec,
  const bool a_limit_downtime_enabled,
  const double a_max_downtime_sec
):
  m_mode(a_mode),
  m_max_size(min(a_max_size, static_cast<size_type>(-1))),
  m_channel_id(invalid_channel + 1),
  m_channel_id_overflow(false),
  m_channel_max_count(static_cast<size_type>(-1)),
  m_map_id_channel(),
  //m_map_id_adress(),
  m_map_adress_id(),
  m_id_list(),
  m_buf_max_size(a_channel_buf_max_size),
  m_it_cur_channel(m_map_id_channel.end()),
  m_it_cur_channel_for_check(m_map_id_channel.end()),
  m_max_lifetime_enabled(a_limit_lifetime_enabled),
  m_max_downtime_enabled(a_limit_downtime_enabled),
  m_max_lifetime(irs::make_cnt_s(a_max_lifetime_sec)),
  m_max_downtime(irs::make_cnt_s(a_max_downtime_sec))
{
}

void irs::hardflow::udp_channel_list_t::insert(adress_type a_adress,
  id_type* ap_id, bool* ap_insert_success)
{
  *ap_insert_success = false;
  // Ищем, есть ли уже такой адрес в списке
  map_adress_id_type::iterator it_map_addr_id = m_map_adress_id.find(a_adress);
  if (it_map_addr_id == m_map_adress_id.end()) {
    bool allow_add = false;
    switch (m_mode) {
      case udplc_mode_queue: {
        IRS_ASSERT(m_id_list.size() <= m_max_size);
        if (m_id_list.size() < m_max_size) {
          allow_add = true;
        } else {
          if (m_id_list.size() > 0) {
            erase(m_id_list.front());
            allow_add = true;
          } else {
            // Максимальный размер установлен в ноль
          }
        }
      } break;
      case udplc_mode_limited: {
        IRS_ASSERT(m_id_list.size() <= m_max_size);
        if (m_id_list.size() < m_max_size) {
          allow_add = true;
        } else {
          // Достигнут лимит количества записей
        }
      } break;
      case udplc_mode_unlimited: {
        if (m_id_list.size() <= m_channel_max_count) {
          allow_add = true;
        } else {
          // Достигнут лимит количества записей
        }
      } break;
      default : {
        IRS_ASSERT_MSG("Неизвестный вариант рабочего режима");
      }
    }
    if (allow_add) {
      next_free_channel_id();
      IRS_LIB_ASSERT(m_channel_id != invalid_channel);
      channel_t channel;
      channel.adress = a_adress;
      channel.lifetime.start();
      channel.downtime.start();
      m_map_id_channel.insert(make_pair(m_channel_id, channel));
      m_map_adress_id.insert(make_pair(a_adress, m_channel_id));
      m_id_list.push_back(m_channel_id);
      *ap_id = m_channel_id;
      //m_channel_id++;
      if (m_it_cur_channel == m_map_id_channel.end()) {
        m_it_cur_channel = m_map_id_channel.begin();
      } else {
        // Текущий канал уже установлен
      }
      if (m_it_cur_channel_for_check == m_map_id_channel.end()) {
        m_it_cur_channel_for_check = m_map_id_channel.begin();
      } else {
        // Текущий канал для проверки уже установлен
      }
      *ap_insert_success = true;
    } else {
      // Добавление не разрешено
    }
  } else {
    // Элемент уже присутсвует в списке
    *ap_id = it_map_addr_id->second;
    *ap_insert_success = true;
  }
}

bool irs::hardflow::udp_channel_list_t::id_get(adress_type a_adress,
  id_type* ap_id)
{
  bool find_success = false;
  map_adress_id_type::iterator it_map_addr_id = m_map_adress_id.end();
  it_map_addr_id = m_map_adress_id.find(a_adress);
  if (it_map_addr_id != m_map_adress_id.end()) {
    *ap_id = it_map_addr_id->second;
    find_success = true;
  } else {
    find_success = false;
  }
  return find_success;
}

bool irs::hardflow::udp_channel_list_t::adress_get(id_type a_id,
  adress_type* ap_adress)
{
  bool find_success = true;
  map_id_channel_iterator it_map_id_channel = m_map_id_channel.find(a_id); 
  if (it_map_id_channel != m_map_id_channel.end()) {
    *ap_adress = it_map_id_channel->second.adress;
    find_success = true;
  } else {
    find_success = false;
  }
  return find_success;
}

void irs::hardflow::udp_channel_list_t::erase(id_type a_id)
{
  map_id_channel_iterator it_erase_channel =
    m_map_id_channel.find(a_id);
  if (it_erase_channel != m_map_id_channel.end()) {
    if (m_it_cur_channel == it_erase_channel) {
      if (m_it_cur_channel != m_map_id_channel.begin()) {
        m_it_cur_channel--;
      } else {
        m_it_cur_channel = m_map_id_channel.end();
        if (m_map_id_channel.size() > 1) {
          m_it_cur_channel--;
        } else {
          // Текущий канал оставляем неопределенным,
          // так как удаляется последний канал
        }
      }
    } else {
      // Удаляемый канал не совпадает с текущим
    }
    if (m_it_cur_channel_for_check == it_erase_channel) {
      if (m_it_cur_channel_for_check != m_map_id_channel.begin()) {
        m_it_cur_channel_for_check--;
      } else {
        m_it_cur_channel_for_check = m_map_id_channel.end();
        if (m_map_id_channel.size() > 1) {
          m_it_cur_channel_for_check--;
        } else {
          // Текущий канал оставляем неопределенным,
          // так как удаляется последний канал
        }
      }
    } else {
      // Удаляемый канал не совпадает с текущим
    }
    map_adress_id_type::iterator it_map_adress_id =
      m_map_adress_id.find(it_erase_channel->second.adress);
    queue_id_type::iterator it_id =
      find(m_id_list.begin(), m_id_list.end(), a_id);
    m_map_id_channel.erase(it_erase_channel);
    m_map_adress_id.erase(it_map_adress_id);
    m_id_list.erase(it_id);
  } else {
    IRS_LIB_ASSERT("Канал отсутсвует");
  }
}

bool irs::hardflow::udp_channel_list_t::is_channel_exists(const id_type a_id)
{
  return m_map_id_channel.find(a_id) != m_map_id_channel.end();
}

irs::hardflow::udp_channel_list_t::size_type
irs::hardflow::udp_channel_list_t::channel_buf_max_size_get() const
{
  return m_buf_max_size;
}

void irs::hardflow::udp_channel_list_t::channel_buf_max_size_set(
  size_type a_channel_buf_max_size)
{
  m_buf_max_size = a_channel_buf_max_size;
  map_id_channel_iterator it_channel = m_map_id_channel.begin();
  while (it_channel != m_map_id_channel.end()) {
    it_channel->second.bufer.resize(m_buf_max_size);
  }
}

double irs::hardflow::udp_channel_list_t::lifetime_get(
  const id_type a_channel_id) const
{
  double channel_lifetime = 0.;
  map_id_channel_const_iterator it_channel =
    m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    channel_lifetime = it_channel->second.lifetime.get();
  } else {
    // Канал отсутсвует
  }
  return channel_lifetime;
}

bool irs::hardflow::udp_channel_list_t::limit_lifetime_enabled_get() const
{
  return m_max_lifetime_enabled;
}

double irs::hardflow::udp_channel_list_t::max_lifetime_get() const
{
  return CNT_TO_DBLTIME(m_max_lifetime);
}

void irs::hardflow::udp_channel_list_t::max_lifetime_set(
  double a_max_lifetime_sec)
{
  m_max_lifetime = irs::make_cnt_s(a_max_lifetime_sec);
}

void irs::hardflow::udp_channel_list_t::limit_lifetime_enabled_set(
  bool a_limit_lifetime_enabled)
{
  m_max_lifetime_enabled = a_limit_lifetime_enabled;
}

void irs::hardflow::udp_channel_list_t::downtime_timer_reset(
  const id_type a_channel_id)
{
  map_id_channel_iterator it_channel =
    m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    it_channel->second.downtime.start();
  } else {
    // Канал отсутсвует
  }
}

double irs::hardflow::udp_channel_list_t::downtime_get(
  const id_type a_channel_id) const
{
  double channel_downtime = 0.;
  map_id_channel_const_iterator it_channel =
    m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    channel_downtime = it_channel->second.downtime.get();
  } else {
    // Канал отсутсвует
  }
  return channel_downtime;
}

bool irs::hardflow::udp_channel_list_t::limit_downtime_enabled_get() const
{
  return m_max_downtime_enabled;
}

void irs::hardflow::udp_channel_list_t::limit_downtime_enabled_set(
  bool a_limit_downtime_enabled)
{
  m_max_downtime_enabled = a_limit_downtime_enabled;
}

double irs::hardflow::udp_channel_list_t::max_downtime_get() const
{
  return CNT_TO_DBLTIME(m_max_downtime);
}

void irs::hardflow::udp_channel_list_t::max_downtime_set(
  double a_max_downtime_sec)
{
  m_max_downtime = irs::make_cnt_s(a_max_downtime_sec);
}

void irs::hardflow::udp_channel_list_t::channel_adress_get(
  const id_type a_channel_id, adress_type* ap_adress) 
{
  map_id_channel_iterator it_channel = m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    *ap_adress = it_channel->second.adress;
  } else {
    // Канал не найден
  }
}

void irs::hardflow::udp_channel_list_t::channel_adress_set(
  const id_type a_channel_id, const adress_type& a_adress)
{
  map_id_channel_iterator it_channel = m_map_id_channel.find(a_channel_id);
  if (it_channel != m_map_id_channel.end()) {
    it_channel->second.adress = a_adress;
    it_channel->second.bufer.clear();
  } else {
    // Канал не найден
  }
}  

void irs::hardflow::udp_channel_list_t::clear()
{
  m_map_id_channel.clear();
  m_map_adress_id.clear();
  m_id_list.clear();
  m_it_cur_channel = m_map_id_channel.end();
  m_it_cur_channel_for_check = m_map_id_channel.end();
}

irs::hardflow::udp_channel_list_t::size_type
irs::hardflow::udp_channel_list_t::size()
{
  return m_id_list.size();
}

void irs::hardflow::udp_channel_list_t::mode_set(
  const udp_limit_connections_mode_t a_mode)
{
  m_mode = a_mode;
  switch (m_mode) {
    case udplc_mode_queue: {
      if (m_id_list.size() > m_max_size) {
        size_type adress_count_need_delete = m_id_list.size() - m_max_size;
        for (size_type id_i = 0; id_i < adress_count_need_delete; id_i++) {
          erase(m_id_list.front());
        }
      } else {
        // Удаление объектов не требуется
      }
    } break;
    case udplc_mode_limited: {
      if (m_id_list.size() > m_max_size) {
        size_type adress_count_need_delete = m_id_list.size() - m_max_size;
        for (size_type id_i = 0; id_i < adress_count_need_delete; id_i++) {
          erase(m_id_list.back());
        }
      } else {
        // Удаление объектов не требуется
      }
    } break;
    case udplc_mode_unlimited: {
      // Удаление не требуется
    } break;
    default : {
      IRS_ASSERT_MSG("Неизвестный тип рабочего режима");
    }
  }
}

irs::hardflow::udp_channel_list_t::size_type
irs::hardflow::udp_channel_list_t::max_size_get()
{
  return m_max_size;
}

void irs::hardflow::udp_channel_list_t::max_size_set(size_type a_max_size)
{
  m_max_size = min(a_max_size, m_channel_max_count);
  switch (m_mode) {
    case udplc_mode_queue: {
      if (m_id_list.size() > m_max_size) {
        const size_type channel_erase_count = m_id_list.size() - m_max_size;
        for (size_type channel_i = 0; channel_i < channel_erase_count;
          channel_i++)
        {
          erase(m_id_list.front());
          m_id_list.pop_front();
        }
      } else {
        // Удаление лишних каналов не требуется
      }
    } break;
    case udplc_mode_limited: {
      const size_type channel_erase_count = m_id_list.size() - m_max_size;
      for (size_type channel_i = 0; channel_i < channel_erase_count;
        channel_i++)
      {
        erase(m_id_list.back());
        m_id_list.pop_back();
      }
    } break;
    case udplc_mode_unlimited: {
      // Удаление каналов не требуется
    } break;
    default : {
      IRS_ASSERT_MSG("Неизвестный вариант рабочего режима");
    }
  }
}

irs::hardflow::udp_channel_list_t::size_type
irs::hardflow::udp_channel_list_t::write(const adress_type& a_adress,
  const irs_u8 *ap_buf, size_type a_size)
{
  size_type write_byte_count = 0;
  bool channel_exists = false;
  id_type id = 0;
  // Проверяем наличие канала и создаем, если его не существует
  insert(a_adress, &id, &channel_exists);
  if (channel_exists) {
    map_id_channel_iterator it_map_id_channel =
      m_map_id_channel.find(id);
    if (it_map_id_channel != m_map_id_channel.end()) {
      bufer_type& buf = it_map_id_channel->second.bufer;
      IRS_LIB_ASSERT(buf.size() <= m_buf_max_size);
      write_byte_count = min(a_size, m_buf_max_size - buf.size());
      buf.push_back(ap_buf, ap_buf + write_byte_count);
      it_map_id_channel->second.downtime.start();  
    } else {
      IRS_LIB_ASSERT_MSG("Канал отсутсвует в списке");
    }                   
  } else {
    // Канала с таким адресом не существует и создать его не удалось
  }
  return write_byte_count;
}

irs::hardflow::udp_channel_list_t::size_type
irs::hardflow::udp_channel_list_t::read(size_type a_id,
  irs_u8 *ap_buf, size_type a_size)
{
  size_type read_byte_count = 0;
  map_id_channel_iterator it_map_id_channel =
    m_map_id_channel.find(a_id);
  if (it_map_id_channel != m_map_id_channel.end()) {
    bufer_type& buf = it_map_id_channel->second.bufer;
    read_byte_count = min(buf.size(), a_size);
    if (read_byte_count > 0) {
      buf.copy_to(0, read_byte_count, ap_buf);
      buf.pop_front(read_byte_count);
      it_map_id_channel->second.downtime.start();  
    } else {
      // Нет данных в буфере
    }
  } else {
    // Этого канала не существует
  }
  return read_byte_count;
}

irs::hardflow::udp_channel_list_t::size_type
irs::hardflow::udp_channel_list_t::channel_next()
{
  size_type cur_channel_id = invalid_channel;
  if (m_it_cur_channel != m_map_id_channel.end()) {
    m_it_cur_channel++;
  } else {
    // Текущий канал не установлен
  }
  if (m_it_cur_channel != m_map_id_channel.end()) {
    cur_channel_id = m_it_cur_channel->first;
  } else {
    m_it_cur_channel = m_map_id_channel.begin();
    if (m_it_cur_channel != m_map_id_channel.end()) {
      cur_channel_id = m_it_cur_channel->first;
    } else {
      // Нет ни одного канала
    }
  }
  return cur_channel_id;
}

irs::hardflow::udp_channel_list_t::size_type
irs::hardflow::udp_channel_list_t::cur_channel() const
{
  id_type channel_id = invalid_channel;
  if (m_it_cur_channel != m_map_id_channel.end()) {
    channel_id = m_it_cur_channel->first;
  } else {
    // Текущий канал не установлен
  }
  return channel_id;
}

void irs::hardflow::udp_channel_list_t::tick()
{
  if (m_max_lifetime_enabled || m_max_downtime_enabled) {
    if (m_it_cur_channel_for_check == m_map_id_channel.end()) {
      m_it_cur_channel_for_check = m_map_id_channel.begin();
    } else {
      // Текущий канал правильный
    }
    if (m_it_cur_channel_for_check != m_map_id_channel.end()) {
      bool channel_need_destroy = false;
      if (m_max_lifetime_enabled) {
        if (lifetime_exceeded(m_it_cur_channel_for_check)) {
          channel_need_destroy = true;
        } else {
          // Максимальное время жизни не превышено
        }
      } else {
        // Проверка времени жизни отключена
      }
      if (m_max_downtime_enabled) {
        if (downtime_exceeded(m_it_cur_channel_for_check)) {
          channel_need_destroy = true;
        } else {
          // Максимальное время бездействие не превышено
        }
      } else {
        // Проверка времени бездействия отключена
      }
      if (channel_need_destroy) {
        erase(m_it_cur_channel_for_check->first);
      } else {
        // Время жизни и время бездействия не превышено
        m_it_cur_channel_for_check++;
      }                              
    } else {
      // Нет ни одного канала
      IRS_LIB_ASSERT(m_map_id_channel.empty());
    }
  } else {
    // Проверка времени жизни и времени бездействия отключена
  }
}

bool irs::hardflow::udp_channel_list_t::lifetime_exceeded(
  const map_id_channel_iterator a_it_cur_channel)
{
  bool lifetime_exceeded_status = false;
  if (m_it_cur_channel_for_check != m_map_id_channel.end()) {
    if (m_it_cur_channel_for_check->second.lifetime.get_cnt() > m_max_lifetime)
    {
      lifetime_exceeded_status = true;
    } else {
      // Максимальное время жизни не превышено
    }
  } else {
    // Недопустимый канал
  }
  return lifetime_exceeded_status;
}

bool irs::hardflow::udp_channel_list_t::downtime_exceeded(
  const map_id_channel_iterator a_it_cur_channel)
{
  bool downtime_exceeded_status = false;
  if (m_it_cur_channel_for_check != m_map_id_channel.end()) {
    if (m_it_cur_channel_for_check->second.downtime.get_cnt() > m_max_downtime)
    {
      downtime_exceeded_status = true;
    } else {
      // Максимальное время бездействия не превышено
    }
  } else {
    // Недопустимый канал
  }
  return downtime_exceeded_status;
}

void irs::hardflow::udp_channel_list_t::next_free_channel_id()
{
  if (!m_channel_id_overflow) {
    //IRS_LIB_ASSERT(m_map_id_channel.find(m_channel_id) ==
      //m_map_id_channel.end());
    m_channel_id++;
    if (m_channel_id == invalid_channel) {
      m_channel_id_overflow = true;
    } else {
      // Переполнение не произошло
    }
  } else {
    // Уже было переполнение счетчика
  }
  if (m_channel_id_overflow) {
    if (m_map_id_channel.size() < m_channel_max_count) {
      if(m_channel_id == invalid_channel) {
        m_channel_id++;
      }
      map_id_channel_iterator it_prev =
        m_map_id_channel.find(m_channel_id);
      map_id_channel_iterator it_cur = it_prev;
      if(it_cur != m_map_id_channel.end()) {
        while(true) {
          it_cur++;
          if(it_cur == m_map_id_channel.end()) {
            m_channel_id = it_prev->first + 1;
            if(m_channel_id == invalid_channel) {
              m_channel_id++;
            }
            it_prev = m_map_id_channel.find(m_channel_id);
            it_cur = it_prev;
            if(it_cur == m_map_id_channel.end()) {
              break;
            }
          } else if((it_cur->first - it_prev->first) > 1) {
            m_channel_id = it_prev->first + 1;
            break;
          }
          it_prev = it_cur;
        }
      } else {
        // Текущее значение идентификатора является уникальным
      }

    } else {
      //Нет свободных мест под новый канал
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
        "No place for add new channel" << endl);
      m_channel_id = invalid_channel;
    }
  } else {
    // Переполнения счетчика не было
  }
}

// class error_sock_t
irs::hardflow::error_sock_t::error_sock_t()
{ }

irs::hardflow::error_sock_t::errcode_type
irs::hardflow::error_sock_t::get_last_error()
{
  errcode_type error_code = 0;
  #if defined(IRS_WIN32)
  error_code = WSAGetLastError();
  #elif defined(IRS_LINUX)
  error_code = errno;
  #endif // IRS_WINDOWS IRS_LINUX
  return error_code;
}
      
// class udp_flow_t
irs::hardflow::udp_flow_t::udp_flow_t(
  const string_type& a_local_host_name,
  const string_type& a_local_host_port,
  const string_type& a_remote_host_name,
  const string_type& a_remote_host_port,
  const udp_limit_connections_mode_t a_mode,
  const size_type a_channel_max_count,
  const size_type a_channel_buf_max_size,
  const bool a_limit_lifetime_enabled,
  const double a_max_lifetime_sec,
  const bool a_limit_downtime_enabled,
  const double a_max_downtime_sec
):
  m_error_sock(),
  m_state_info(),
  #if defined(IRS_WIN32)
  m_wsd(),
  #endif // IRS_WIN32
  m_sock(),
  m_local_host_name(a_local_host_name),
  m_local_host_port(a_local_host_port),
  m_func_select_timeout(),
  m_s_kit(),
  m_send_msg_max_size(65000),
  m_channel_list(
    a_mode,
    a_channel_max_count,
    a_channel_buf_max_size,
    a_limit_lifetime_enabled,
    a_max_lifetime_sec,
    a_limit_downtime_enabled,
    a_max_downtime_sec
  ),
  m_read_buf(a_channel_buf_max_size)
{
  m_func_select_timeout.tv_sec = 0;
  m_func_select_timeout.tv_usec = 0;

  sockaddr_in remote_host_addr;
  bool init_success = true;
  addr_init(a_remote_host_name, a_remote_host_port,
    &remote_host_addr, &init_success);
  if (init_success) {
    bool insert_success = false;
    size_type remote_host_id = 0;
    m_channel_list.insert(
      remote_host_addr, &remote_host_id, &insert_success);
    IRS_LIB_ASSERT(insert_success);
  } else {
    // Адресс не задан или задан неправильно
  }
}

irs::hardflow::udp_flow_t::~udp_flow_t()
{
  if (m_state_info.sock_created) {
    close_socket(m_sock);
  } else {
    // Сокет не создан
  }
}

void irs::hardflow::udp_flow_t::start()
{
  if (!m_state_info.lib_socket_loaded) {
    // Загружаем библиотеку сокетов версии 2.2
    #if defined(IRS_WIN32)
    if (lib_socket_load(&m_wsd, 2, 2)) {
      m_state_info.lib_socket_loaded = true;
    } else {
      // Загрузить библиотеку не удалось
    }
    #elif defined(IRS_LINUX)
    m_state_info.lib_socket_loaded = true;
    #endif // IRS_WINDOWS IRS_LINUX
    m_state_info.sock_created = false;
  } else {
    // Библиотека уже загружена
  }
  if (m_state_info.lib_socket_loaded) {
    if (!m_state_info.sock_created) {
      #if defined(IRS_WIN32)
      m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      #elif defined(IRS_LINUX)
      m_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
      #endif // IRS_WINDOWS IRS_LINUX
      if (m_sock != static_cast<socket_type>(m_invalid_socket)) {
        m_state_info.sock_created = true;
        m_state_info.set_io_mode_sock_success = false;
      } else {
        // Сокет создать не удалось
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
          error_str(m_error_sock.get_last_error()));
      }
    } else {
      // Сокет уже создан
    }
  } else {
    // Библиотека сокетов не загружена
  }
  if (m_state_info.sock_created) {
    if (!m_state_info.set_io_mode_sock_success) {
      m_state_info.set_io_mode_sock_success = true;
      #ifdef NOP
      // для включения неблокирующего режима переменная ulblock должна иметь
      // ненулевое значение
      unsigned long ulblock = 1;
      #if defined(IRS_WIN32)
      if (ioctlsocket(m_sock, FIONBIO, &ulblock) == m_socket_error) {
        // функция завершилась неудачей
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
          error_str(m_error_sock.get_last_error()));
      } else {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.bind_sock_and_ladr_success = false;
      }
      #elif defined(IRS_LINUX)
      if (fcntl(m_sock, F_SETFL, O_NONBLOCK) == m_socket_error) {
        // функция завершилась неудачей
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
          error_str(m_error_sock.get_last_error()));
      } else {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.bind_sock_and_ladr_success = false;
      }
      #endif // IRS_WINDOWS IRS_LINUX
      #endif // NOP
    } else {
      // Сокет уже переведен в неблокирующий режим
    }
  } else {
    // Сокет не создан
  }
  if (m_state_info.set_io_mode_sock_success) {
    if (!m_state_info.bind_sock_and_ladr_success) {
      sockaddr_in local_addr;
      bool init_success = true;
      local_addr_init(&local_addr, &init_success);
      if (init_success) {
        if (bind(m_sock, (sockaddr *)&local_addr,
          sizeof(local_addr)) != m_socket_error)
        {
          m_state_info.bind_sock_and_ladr_success = true;    
        } else {
          // Ошибка при связывании сокета с локальным адресом
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
            error_str(m_error_sock.get_last_error()));
        }
      } else {
        // Ошибка инициализации
      }
    } else {
      // Сокет уже привязан к локальному адресу
    }
  } else {
    // Перевод сокета в неблокирующий режим завершился неудачей
  }
}

void irs::hardflow::udp_flow_t::sock_close()
{
  close_socket(m_sock);
  m_state_info.sock_created = false;
  m_state_info.set_io_mode_sock_success = false;
  m_state_info.bind_sock_and_ladr_success = false;
}

void irs::hardflow::udp_flow_t::local_addr_init(
  sockaddr_in* ap_sockaddr, bool* ap_init_success)
{
  *ap_init_success = true;
  if (m_local_host_name != "") {
    if (!adress_str_to_adress_binary(m_local_host_name,
      &ap_sockaddr->sin_addr.s_addr))
    {
      // Преобразовать адрес не удалось
      *ap_init_success = false;
    } else {
      // Адрес успешно получен
    }
  } else {
    // Адрес не задан
    ap_sockaddr->sin_addr.s_addr = htonl(INADDR_ANY);
  }
  if (*ap_init_success) {
    ap_sockaddr->sin_family = AF_INET;
    if (m_local_host_port != "") {
      unsigned short port = 0;
      if (m_local_host_port.to_number(port)) {
        ap_sockaddr->sin_port = htons(port);
      } else {
        // Преобразовать строку в число не удалось
        *ap_init_success = false;
      }
    } else {
      // Порт не указан, выбераем любой порт
    }
  } else {
    // Значение адреса получить не удалось
  }
}

void irs::hardflow::udp_flow_t::addr_init(
  const string_type& a_host_name,
  const string_type& a_host_port,
  sockaddr_in* ap_sockaddr,
  bool* ap_init_success)
{
  *ap_init_success = adress_str_to_adress_binary(
    a_host_name, &ap_sockaddr->sin_addr.s_addr);  
  ap_sockaddr->sin_family = AF_INET;
  if (*ap_init_success) {
    unsigned short port = 0;
    if (a_host_port.to_number(port)) {
      ap_sockaddr->sin_port = htons(port);
    } else {
      // Преобразовать в число не удалось
      *ap_init_success = false;
    }
  } else {
    // Произошла ошибка
  }
}

bool irs::hardflow::udp_flow_t::adress_str_to_adress_binary(
  const string_type& a_adress_str, in_addr_type* ap_adress_binary)
{
  bool adress_convert_success = false;
  if (a_adress_str != "") {
    irs_u32 adress = inet_addr(a_adress_str.c_str());
    if (adress == INADDR_NONE) {
      hostent* p_host = gethostbyname(a_adress_str.c_str());
      if (p_host != NULL) {
        // Берем первый адрес в списке
        memcpy(ap_adress_binary, p_host->h_addr_list[0], p_host->h_length);
        adress_convert_success = true;
      } else {
        // Ошибка получения списка адресов
      }
    } else {
      *ap_adress_binary = adress;
      adress_convert_success = true;
    }
  } else {
    // Адрес не задан
  }
  return adress_convert_success;
}

bool irs::hardflow::udp_flow_t::detect_func_single_arg(
  string_type a_param, string_type* ap_func_name, string_type* ap_arg) const
{
  bool detect_success = false;
  const size_type func_name_begin_pos = a_param.find_first_not_of(irst(" "));
  const size_type func_name_end_pos =
    a_param.find_first_of(irst(" ("), func_name_begin_pos + 1);
  const size_type left_square_bracket_pos = a_param.find(irst("("));
  const size_type right_square_bracket_pos = a_param.find(irst(")"));
  if ((func_name_begin_pos != string_type::npos) &&
      (func_name_end_pos != string_type::npos) &&
      (left_square_bracket_pos != string_type::npos) &&
      (right_square_bracket_pos !=string_type::npos) &&
      (func_name_begin_pos < func_name_end_pos) &&
      (func_name_end_pos <= left_square_bracket_pos) &&
      (left_square_bracket_pos < right_square_bracket_pos))
  {
    *ap_func_name = a_param.substr(func_name_begin_pos,
      func_name_end_pos - func_name_begin_pos);
    *ap_arg = a_param.substr(left_square_bracket_pos + 1,
      right_square_bracket_pos - (left_square_bracket_pos + 1));
    detect_success = true;
  } else {
  }
  return detect_success;
}

irs::string irs::hardflow::udp_flow_t::param(const irs::string &a_param_name)
{
  irs::string param_value;
  if (a_param_name == irst("locale_adress")) {
    param_value = m_local_host_name;
  } else if (a_param_name == irst("locale_port")) {
    param_value = m_local_host_port;
  } else if (a_param_name == irst("remote_adress")) {
    adress_type adress;
    m_channel_list.channel_adress_get(m_channel_list.cur_channel(), &adress);
    param_value = inet_ntoa(adress.sin_addr);
  } else if (a_param_name == irst("remote_port")) {
    adress_type adress;
    m_channel_list.channel_adress_get(m_channel_list.cur_channel(), &adress);
    irs_u16 port =  ntohs(adress.sin_port);
    number_to_string(port, &param_value);
  } else if (a_param_name == irst("read_buf_max_size")) {
    number_to_string(m_channel_list.channel_buf_max_size_get(), &param_value);
  } else if (a_param_name == irst("limit_lifetime_enabled")) {
    number_to_string(m_channel_list.limit_lifetime_enabled_get(), &param_value);
  } else if (a_param_name == irst("max_lifetime")) {
    number_to_string(m_channel_list.max_lifetime_get(), &param_value);
  } else if (a_param_name == irst("limit_downtime_enabled")) {
    number_to_string(m_channel_list.limit_downtime_enabled_get(), &param_value); 
  } else if (a_param_name == irst("max_downtime")) {
    number_to_string(m_channel_list.max_downtime_get(), &param_value);
  } else if (a_param_name == irst("channel_max_count")) {
    number_to_string(m_channel_list.max_size_get(), &param_value);
  } else {
    string_type func_name;
    string_type arg;
    if (detect_func_single_arg(a_param_name, &func_name, &arg)) {
      size_type channel_id = 0;   
      if (string_to_number(arg, &channel_id)) {
        if (func_name == irst("lifetime")) {
          number_to_string(m_channel_list.lifetime_get(channel_id),
            &param_value);
        } else if (func_name == irst("downtime")) {
          number_to_string(m_channel_list.downtime_get(channel_id),
            &param_value);
        } else if (func_name == irst("remote_adress")) {
          adress_type adress;
          m_channel_list.channel_adress_get(channel_id, &adress);
          param_value = inet_ntoa(adress.sin_addr);
        } else if (func_name == irst("remote_port")) {
          adress_type adress;
          m_channel_list.channel_adress_get(channel_id, &adress);
          irs_u16 port =  ntohs(adress.sin_port);
          number_to_string(port, &param_value);
        } else {
          IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE("Неверный формат параметра " <<
            "\"" << a_param_name << "\"");
        }
      } else {
        IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE("Неверный формат параметра" <<
          "\"" << a_param_name << "\"");
      }
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE("Неизвестный параметр " <<
        "\"" << a_param_name << "\"");
    }                                                           
  }
  return param_value;
}

void irs::hardflow::udp_flow_t::set_param(const irs::string &a_param_name,
  const irs::string &a_value)
{ 
  if (a_param_name == irst("remote_adress")) {
    in_addr_type adress_binary = 0;
    if (adress_str_to_adress_binary(a_value, &adress_binary)) {
      adress_type adress;
      m_channel_list.channel_adress_get(m_channel_list.cur_channel(), &adress);
      adress.sin_addr.s_addr = adress_binary;
      m_channel_list.channel_adress_set(m_channel_list.cur_channel(), adress);
    } else {
      // Преобразовать адрес не удалось
    }
  } else if (a_param_name == irst("remote_port")) {
    irs_u16 port = 0;
    if (string_to_number(a_value, &port)) {
      adress_type adress;
      m_channel_list.channel_adress_get(m_channel_list.cur_channel(), &adress);
      adress.sin_port = htons(port);
      m_channel_list.channel_adress_set(m_channel_list.cur_channel(), adress);
    } else {
      // Преобразовать значение не удалось
    }
  } else if (a_param_name == irst("read_buf_max_size")) {
    size_type read_buf_max_size = 0;
    if (string_to_number(a_value, &read_buf_max_size)) {
      m_channel_list.channel_buf_max_size_set(read_buf_max_size);
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE("Недопустимое значение параметра. " <<
        "read_buf_max_size == " << a_value);
    }
  } else if (a_param_name == irst("limit_lifetime_enabled")) {
    bool limit_lifetime_enabled = false;
    if (string_to_number(a_value, &limit_lifetime_enabled)) {
      m_channel_list.limit_lifetime_enabled_set(limit_lifetime_enabled);
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE("Недопустимое значение параметра. " <<
        "limit_lifetime_enabled == " << a_value);
    }
  } else if (a_param_name == irst("max_lifetime")) {
    double max_lifetime = 0.;
    if (string_to_number(a_value, &max_lifetime)) {
      m_channel_list.max_lifetime_set(max_lifetime);
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE("Недопустимое значение параметра. " <<
        "max_lifetime == " << a_value);
    }
  } else if (a_param_name == irst("limit_downtime_enabled")) {
    bool limit_downtime_enabled = false;
    if (string_to_number(a_value, &limit_downtime_enabled)) {
      m_channel_list.limit_downtime_enabled_set(limit_downtime_enabled);
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE("Недопустимое значение параметра. " <<
        "limit_downtime_enabled == " << a_value);
    }
  } else if (a_param_name == irst("max_downtime")) {
    double max_downtime = 0.;
    if (string_to_number(a_value, &max_downtime)) {
      m_channel_list.max_downtime_set(max_downtime);
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE("Недопустимое значение параметра. " <<
        "max_downtime == " << a_value);
    }
  } else if (a_param_name == irst("channel_max_count")) {
    size_type channel_max_count = 0;
    if (string_to_number(a_value, &channel_max_count)) {
      m_channel_list.max_size_set(channel_max_count);
    } else {
      IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE("Недопустимое значение параметра. " <<
        "channel_max_count == " << a_value);
    }
  } else {
    IRS_LIB_HARDFLOWG_DBG_MSG_SRC_BASE("Неизвестный параметр");
  }
}

irs::hardflow::udp_flow_t::size_type irs::hardflow::udp_flow_t::read(
  size_type a_channel_ident, irs_u8 *ap_buf, size_type a_size)
{
  size_type size_rd = 0;
  size_rd = m_channel_list.read(a_channel_ident, ap_buf, a_size);
  return size_rd;
}

irs::hardflow::udp_flow_t::size_type irs::hardflow::udp_flow_t::write(
  size_type a_channel_ident, const irs_u8 *ap_buf, size_type a_size)
{
  size_type size_wr = 0;
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    sockaddr_in remote_host_adr;
    memset(&remote_host_adr, sizeof(sockaddr_in), 0);
    if (m_channel_list.adress_get(a_channel_ident, &remote_host_adr)) {
      size_type msg_size = min(a_size, m_send_msg_max_size);
      const int ret = sendto(m_sock, reinterpret_cast<const char*>(ap_buf),
        msg_size, 0, reinterpret_cast<sockaddr*>(&remote_host_adr),
          sizeof(remote_host_adr));
      if (ret != m_socket_error) {
        size_wr = static_cast<size_type>(ret);
        IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Записано " <<
          static_cast<string_type>(size_wr) << " байт");
      } else {
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Запись: " <<
          error_str(m_error_sock.get_last_error()));
      }
      m_channel_list.downtime_timer_reset(a_channel_ident);
    } else { 
      // Адресс с таким идентификатором отсутсвует в списке
      IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Неизвестный идентификатор");
    }
  } else {
    // Запуск не произошел
  }
  return size_wr;
}

irs::hardflow::udp_flow_t::size_type irs::hardflow::udp_flow_t::channel_next()
{
  return m_channel_list.channel_next();
}

bool irs::hardflow::udp_flow_t::is_channel_exists(size_type a_channel_ident)
{
  return m_channel_list.is_channel_exists(a_channel_ident);
}

void irs::hardflow::udp_flow_t::tick()
{
  bool start_success = m_state_info.get_state_start();
  if (start_success) {
    // Клиент успешно запущен
    FD_ZERO(&m_s_kit);
    FD_SET(m_sock, &m_s_kit);
    int ready_read_sock_count = select(m_sock + 1, &m_s_kit, NULL, NULL,
      &m_func_select_timeout);
    if ((ready_read_sock_count != m_socket_error) &&
      (ready_read_sock_count != 0))
    {
      if (FD_ISSET(m_sock, &m_s_kit)) {
        sockaddr_in sender_addr;
        socklen_type sender_addr_size = sizeof(sender_addr);
        int ret = recvfrom(m_sock, reinterpret_cast<char*>(m_read_buf.data()),
          m_read_buf.size(), 0, (sockaddr*)&sender_addr, &sender_addr_size);
        if (ret != m_socket_error) {
          m_channel_list.write(sender_addr, m_read_buf.data(), ret);
          IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Прочитано " <<
            static_cast<string_type>(ret) << " байт");
        } else {
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL("Чтение: " <<
            error_str(m_error_sock.get_last_error()));
        }
      } else {
        // Нет сокетов для чтения
      }
    } else if (ready_read_sock_count == m_socket_error) {
      IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
        error_str(m_error_sock.get_last_error()));
    } else {
      // Нет сокетов для чтения
    }
  } else {
    start();
  }
  m_channel_list.tick();
}

#if defined(IRS_WIN32) || defined(IRS_LINUX)

irs::hardflow::tcp_server_t::tcp_server_t(
  irs_u16 local_port
):
  #if defined(IRS_WIN32)
  m_wsd(),
  #endif // IRS_WIN32
  m_addr(zero_struct_t<sockaddr_in>::get()),
  m_serv_select_timeout(zero_struct_t<timeval>::get()),
  m_read_fds(zero_struct_t<fd_set>::get()),
  m_write_fds(zero_struct_t<fd_set>::get()),
  m_is_open(true),
  m_local_port(local_port),
  m_server_sock(0),
  m_map_channel_sock(),
  mp_map_channel_sock_it(m_map_channel_sock.begin()),
  m_channel(invalid_channel + 1),
  m_channel_id_overflow(false),
  m_channel_max_count(static_cast<size_type>(-1))
{
  start_server();
}

irs::hardflow::tcp_server_t::~tcp_server_t()
{
  stop_server();
}

void irs::hardflow::tcp_server_t::start_server()
{
  m_is_open = true;
  bool lib_load_success = true;
  #if defined(IRS_WIN32)
  if (!lib_socket_load(&m_wsd, 2, 2)) {
    lib_load_success = false;
  } else {
    // Библиотека удачно загружена
  }
  #endif // defined(IRS_WIN32)
  if (lib_load_success) {
    m_server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(m_server_sock != invalid_socket) {
      m_addr.sin_family = AF_INET;
      m_addr.sin_port = htons(m_local_port);
      m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      if(bind(m_server_sock, (struct sockaddr *)&m_addr,
        sizeof(m_addr)) >= 0) 
      {
        int queue_lenght = 300; //длина очереди
        listen(m_server_sock, queue_lenght);
      } else {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("bind: " <<
          last_error_str() << endl);
        m_is_open = false;
        close_socket(m_server_sock);
      }
    } else {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("socket: " <<
        last_error_str() << endl);
      m_is_open = false;
    }
  } else {
    // Библиотеку загрузить не удалось
    m_is_open = true;
  }
}

irs::hardflow::tcp_server_t::size_type
  irs::hardflow::tcp_server_t::channel_next()
{
  size_type channel = invalid_channel;
  if (!m_map_channel_sock.empty()) {
    IRS_LIB_ASSERT(mp_map_channel_sock_it != m_map_channel_sock.end());
    mp_map_channel_sock_it++;
    if(mp_map_channel_sock_it == m_map_channel_sock.end())
    {
      mp_map_channel_sock_it = m_map_channel_sock.begin();
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("Reach the end" << endl);
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("Go to first socket" << endl);
    }
    channel = mp_map_channel_sock_it->first;
  } else {
    channel = invalid_channel;
  }
  return channel;
}

bool irs::hardflow::tcp_server_t::is_channel_exists(size_type a_channel_ident)
{
  return m_map_channel_sock.find(a_channel_ident) != m_map_channel_sock.end();
}

void irs::hardflow::tcp_server_t::new_channel()
{
  if (!m_channel_id_overflow) {
    m_channel++;
    if (m_channel == invalid_channel) {
      m_channel_id_overflow = true;
    } else {
      // Переполнение не произошло
      IRS_LIB_ASSERT(m_map_channel_sock.find(m_channel) ==
        m_map_channel_sock.end());
    }
  } else {
    // Уже было переполнение счетчика
  }
  if (m_channel_id_overflow)  {
    if (m_map_channel_sock.size() < m_channel_max_count) {
      if(m_channel == invalid_channel) {
        m_channel++;
      }
      map<size_type, int>::iterator it_prev =
        m_map_channel_sock.find(m_channel);
      map<size_type, int>::iterator it_cur = it_prev;
      if(it_cur != m_map_channel_sock.end()) {
        while(true) {
          it_cur++;
          if(it_cur == m_map_channel_sock.end()) {
            m_channel = it_prev->first + 1;
            if(m_channel == invalid_channel) {
              m_channel++;
            }
            it_prev = m_map_channel_sock.find(m_channel);
            it_cur = it_prev;
            if(it_cur == m_map_channel_sock.end()) {
              break;
            }
          } else if((it_cur->first - it_prev->first) > 1) {
            m_channel = it_prev->first + 1;
            break;
          }
          it_prev = it_cur;
        }
      }
    } else {
      //Нет свободных мест под новый канал
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE("No place for add new channel" << endl);
      m_channel = invalid_channel;
    }
  } else {
    // Переполнения счетчика не было
  }
}

irs::hardflow::tcp_server_t::size_type
  irs::hardflow::tcp_server_t::read(size_type a_channel_ident,
  irs_u8 *ap_buf, size_type a_size)
{
  size_type rd_data_size = 0;
  if (m_is_open && is_channel_exists(a_channel_ident)) {
    socket_type sock_rd = m_map_channel_sock[a_channel_ident];
    FD_SET(sock_rd, &m_read_fds);
    socket_type sock_ready = select(sock_rd + 1, &m_read_fds,
      NULL, NULL, &m_serv_select_timeout);
    if((sock_ready != socket_error) && (sock_ready != 0)) {
      if(FD_ISSET(sock_rd, &m_read_fds)) {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("recv start" << endl);
        int server_read = recv(sock_rd, reinterpret_cast<char*>(ap_buf),
          a_size, 0);
        if(server_read > 0){
          rd_data_size =
            static_cast<irs::hardflow::tcp_client_t::size_type>(server_read);
        }
        else {
          if(server_read < 0) {
            IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("recv: " << last_error_str());
          }
          map<size_type, int>::iterator channel_erase_it =
            m_map_channel_sock.find(a_channel_ident);
          if(channel_erase_it == mp_map_channel_sock_it) {
            if (mp_map_channel_sock_it == m_map_channel_sock.begin()) {
              mp_map_channel_sock_it = m_map_channel_sock.end();
              if (m_map_channel_sock.size() > 1) {
                mp_map_channel_sock_it--;
              }
            } else {
              mp_map_channel_sock_it--;
            }
            m_map_channel_sock.erase(a_channel_ident);
          } else {
            m_map_channel_sock.erase(a_channel_ident);
          }
          #if IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_BASE
          irs::mlog() << "Delete channel " << (int)a_channel_ident
            << " by read command" << endl;
          irs::mlog() << "-------------------------------" << endl;
          for(map<size_type, int>::iterator it = m_map_channel_sock.begin();
            it != m_map_channel_sock.end(); it++) {
            irs::mlog() << "m_channel: " << (int)it->first <<
              " socket: " << it->second << endl;
          }
          irs::mlog() << "-------------------------------" << endl;
          #endif //IRS_LIB_HARDFLOWG_DEBUG_BASE
          close_socket(sock_rd);
        }
      }
    } else if(sock_ready == socket_error) {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("select: " << last_error_str())
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE("read" << endl);
      close_socket(sock_rd);
    } else {
      // если select вернула 0, то событие не сработало
    }
    FD_CLR(sock_rd, &m_read_fds);
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE("FD_CLR" << endl);
  }
  return rd_data_size;
}

irs::hardflow::tcp_server_t::size_type 
  irs::hardflow::tcp_server_t::write(size_type a_channel_ident, 
  const irs_u8 *ap_buf, size_type a_size)  
{
  size_type wr_data_size = 0;
  if (m_is_open && is_channel_exists(a_channel_ident)) {
    socket_type sock_wr = m_map_channel_sock[a_channel_ident];
    FD_SET(sock_wr, &m_write_fds); 
    socket_type sock_ready = select(sock_wr + 1, NULL,
      &m_write_fds, NULL, &m_serv_select_timeout);
    if((sock_ready != socket_error) && (sock_ready != 0)) {
      if(FD_ISSET(sock_wr, &m_write_fds)) {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("send start" << endl);
        int server_write = send(sock_wr, reinterpret_cast<const char*>(ap_buf),
          a_size, 0);
        if(server_write > 0){
          wr_data_size = 
            static_cast<irs::hardflow::tcp_client_t::size_type>(server_write);
        }
        else{
          if(server_write < 0) {
            IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("send: " << last_error_str());
          }
          map<size_type, int>::iterator channel_erase_it = 
            m_map_channel_sock.find(a_channel_ident);
          if(channel_erase_it == mp_map_channel_sock_it) {
            if (mp_map_channel_sock_it == m_map_channel_sock.begin()) {
              mp_map_channel_sock_it = m_map_channel_sock.end();
              if (m_map_channel_sock.size() > 1) {                
                mp_map_channel_sock_it--;
              } 
            } else {
              mp_map_channel_sock_it--;
            }
            m_map_channel_sock.erase(a_channel_ident);            
          } else {
            m_map_channel_sock.erase(a_channel_ident);
          }
          
          #if IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_BASE
          irs::mlog() << "Delete channel " << (int)a_channel_ident
            << " by write command" << endl;
          irs::mlog() << "-------------------------------" << endl;
          for(map<size_type, int>::iterator it = m_map_channel_sock.begin();
            it != m_map_channel_sock.end(); it++) {
            irs::mlog() << "m_channel: " << (int)it->first << 
              " socket: " << it->second << endl;
          }
          irs::mlog() << "-------------------------------" << endl;
          #endif //IRS_LIB_DEBUG_BASE
          close_socket(sock_wr);
        }
      }
    } else if(sock_ready == socket_error) {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("select: " <<
        last_error_str() << endl;);
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE("write" << endl);
      close_socket(sock_wr);
    } else {
      // если select вернула 0, то событие не сработало
    }
    FD_CLR(sock_wr, &m_write_fds);
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE("FD_CLR" << endl);
  }
  return wr_data_size;
}

void irs::hardflow::tcp_server_t::tick()
{ 
  if(m_is_open) {
    FD_SET(m_server_sock, &m_read_fds);
    int se_select = select(m_server_sock + 1, &m_read_fds,
      NULL, NULL, &m_serv_select_timeout);
    if((se_select != socket_error) && (se_select != 0)) {
      if(FD_ISSET(m_server_sock, &m_read_fds)) {
        int new_sock = accept(m_server_sock, NULL, NULL);
        if(new_sock >= 0) {
          new_channel();
          if (m_channel != invalid_channel) {
            pair<map<size_type, int>::iterator, bool> insert_channel =
              m_map_channel_sock.insert(make_pair(m_channel, new_sock));
            if(m_map_channel_sock.size() == 1)
              mp_map_channel_sock_it = m_map_channel_sock.begin();
            #if IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_BASE
            if(insert_channel.second) {
              irs::mlog() << "New channel added: " << (int)m_channel << endl;
              irs::mlog() << "-------------------------------" << endl;
              for(map<size_type, int>::iterator it = m_map_channel_sock.begin();
                it != m_map_channel_sock.end(); it++) 
              {
                irs::mlog() << "m_channel: " << (int)it->first << 
                  " socket: " << it->second << endl;
              }
              irs::mlog() << "-------------------------------" << endl;
            }
            else {
              IRS_LIB_HARDFLOW_DBG_RAW_MSG_BASE(
                "Channel already exist" << endl);
            }
            #endif //IRS_LIB_DEBUG_BASE
          }
          else {
            IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE("Invalid channel" << endl);
            close_socket(new_sock);
          }
        } else {
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("accept: " << 
            last_error_str() << endl);
        }
      }
    } else if (socket_error == socket_error) {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("select: " << last_error_str());
    } else {
      // если select вернула 0, то событие не сработало
    }
    FD_CLR(m_server_sock, &m_read_fds);
  } else {
    start_server();
  }
}

void elem_map_close_sock(pair<const size_t, int>& a_map_item)
{
  irs::hardflow::close_socket(a_map_item.second);
}

void irs::hardflow::tcp_server_t::stop_server()
{
  close_socket(m_server_sock);
  for_each(m_map_channel_sock.begin(), m_map_channel_sock.end(),
    elem_map_close_sock);
  m_map_channel_sock.clear();
  m_is_open = false;
}

irs::string irs::hardflow::tcp_server_t::param(const irs::string &a_name)
{
  irs::string param;
  return param;
}

void irs::hardflow::tcp_server_t::set_param(const irs::string &a_name,
  const irs::string &a_value)
{

}

irs::hardflow::tcp_client_t::tcp_client_t(
  mxip_t dest_ip,
  irs_u16 dest_port
):
  m_error_sock(),
  m_state_info(),
  #if defined(IRS_WIN32)
  m_wsd(),
  #endif // IRS_WIN32
  m_addr(zero_struct_t<sockaddr_in>::get()),
  m_client_select_timeout(zero_struct_t<timeval>::get()),
  m_read_fds(zero_struct_t<fd_set>::get()),
  m_write_fds(zero_struct_t<fd_set>::get()),
  m_client_sock(0),
  m_is_open(false),
  m_dest_ip(dest_ip),
  m_dest_port(dest_port),
  m_channel(1)
{
  //start_client();
}

irs::hardflow::tcp_client_t::~tcp_client_t()
{
  if (m_state_info.sock_created) {
    close_socket(m_client_sock);
  } else {
    // Сокет не создан
  }
}

void irs::hardflow::tcp_client_t::start_client()
{
  if (!m_state_info.lib_socket_loaded) {
    // Загружаем библиотеку сокетов версии 2.2
    #if defined(IRS_WIN32)
    if (lib_socket_load(&m_wsd, 2, 2)) {
      m_state_info.lib_socket_loaded = true;
    } else {
      // Загрузить библиотеку не удалось
    }
    #elif defined(IRS_LINUX)
    m_state_info.lib_socket_loaded = true;
    #endif // IRS_WINDOWS IRS_LINUX
    m_state_info.sock_created = false;
  } else {
    // Библиотека уже загружена
  }
  if (m_state_info.lib_socket_loaded) {
    if (!m_state_info.sock_created) {
      #if defined(IRS_WIN32)
      m_client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      #elif defined(IRS_LINUX)
      m_client_sock = socket(PF_INET, SOCK_STREAM, 0);
      #endif // IRS_WINDOWS IRS_LINUX
      if (m_client_sock != static_cast<socket_type>(m_invalid_socket)) {
        m_state_info.sock_created = true;
        m_state_info.set_io_mode_sock_success = false;
      } else {
        // Сокет создать не удалось
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
          error_str(m_error_sock.get_last_error()));
      }
    } else {
      // Сокет уже создан
    }
  } else {
    // Библиотека сокетов не загружена
  }
  if (m_state_info.sock_created) {
    if (!m_state_info.set_io_mode_sock_success) {
      // для включения неблокирующего режима переменная ulblock должна иметь
      // ненулевое значение
      #if defined(IRS_WIN32)
      unsigned long ulblock = 1;
      if (ioctlsocket(m_client_sock, FIONBIO, &ulblock) != m_socket_error) {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.connect_sock_success = false;
      } else {
        // функция завершилась неудачей
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
          error_str(m_error_sock.get_last_error()));
      }
      #elif defined(IRS_LINUX)
      if (fcntl(m_client_sock, F_SETFL, O_NONBLOCK) != m_socket_error) {
        m_state_info.set_io_mode_sock_success = true;
        m_state_info.connect_sock_success = false;
      } else {
        // функция завершилась неудачей
        IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
          error_str(m_error_sock.get_last_error()));
      }
      #endif // IRS_WINDOWS IRS_LINUX
    } else {
      // Сокет уже переведен в неблокирующий режим
    }
  } else {
    // Сокет не создан
  }
  if (m_state_info.set_io_mode_sock_success) {
    if (!m_state_info.connect_sock_success) {
      m_addr.sin_family = AF_INET;
      m_addr.sin_port = htons(m_dest_port);
      m_addr.sin_addr.s_addr = reinterpret_cast<unsigned long&>(m_dest_ip);
      if (connect(m_client_sock, (struct sockaddr *)&m_addr,
        sizeof(m_addr)) != m_socket_error)
      {
        m_state_info.connect_sock_success = true;
      } else {
        const errcode_type error = m_error_sock.get_last_error();
        if (error == error_sock_t::eisconn) {
          // Сокет уже соединен с сервером
          m_state_info.connect_sock_success = true;
          IRS_LIB_HARDFLOWG_DBG_MSG_BASE("Произошло подключение к серверу");
        } else {
          // Ошибка при устоновлении соединения
          IRS_LIB_HARDFLOWG_DBG_MSG_DETAIL(
            error_str(m_error_sock.get_last_error()));
        }
      }
    } else {
      // Соединение уже установлено
    }
  } else {
    // Перевод сокета в неблокирующий режим завершился неудачей
  }
}

void irs::hardflow::tcp_client_t::stop_client()
{
  close_socket(m_client_sock);
  m_is_open = false;
  FD_ZERO(&m_read_fds);
  FD_ZERO(&m_write_fds);
  IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE("Close client" << endl);
}

irs::hardflow::tcp_client_t::size_type 
  irs::hardflow::tcp_client_t::channel_next()
{
  size_type channel = 1;
  return channel;
}

bool irs::hardflow::tcp_client_t::is_channel_exists(size_type a_channel_ident)
{
  return true;
}


irs::hardflow::tcp_client_t::size_type 
  irs::hardflow::tcp_client_t::read(size_type a_channel_ident,
  irs_u8 *ap_buf, size_type a_size)
{
  size_type rd_data_size = 0;
  if (m_state_info.get_state_start()) {
    a_channel_ident = m_channel;
    FD_SET(m_client_sock, &m_read_fds);
    socket_type sock_ready = select(m_client_sock + 1, &m_read_fds, 
      NULL, NULL, &m_client_select_timeout);
    if((sock_ready != socket_error) && (sock_ready != 0)) {
      if(FD_ISSET(m_client_sock, &m_read_fds)) {
        int client_read = recv(m_client_sock, reinterpret_cast<char*>(ap_buf),
          a_size, 0);
        if(client_read > 0) {
          rd_data_size = static_cast<size_type>(client_read);
        } else {
          if(client_read < 0) {
            IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("recv: " << last_error_str());
          }
          close_socket(m_client_sock);
          m_state_info.sock_created = false;
        }
      }
    } else if(sock_ready == socket_error) {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("select: " << last_error_str());
    } else {
      // если select вернула 0, то событие не сработало
    }
    FD_CLR(m_client_sock,&m_read_fds);
  }
  return rd_data_size;
}

irs::hardflow::tcp_client_t::size_type
  irs::hardflow::tcp_client_t::write(size_type /*a_channel_ident*/,
  const irs_u8 *ap_buf, size_type a_size)
{
  size_type wr_data_size = 0;
  if (m_state_info.get_state_start()) {
    socket_type sock_wr = m_client_sock;
    FD_SET(sock_wr, &m_write_fds);
    socket_type sock_ready = select(sock_wr + 1, NULL,
      &m_write_fds, NULL, &m_client_select_timeout);
    if((sock_ready != socket_error) && (sock_ready != 0)) {
      if(FD_ISSET(sock_wr, &m_write_fds)) {
        int client_write = send(sock_wr, reinterpret_cast<const char*>(ap_buf),
          a_size, 0);
        if(client_write > 0){
          wr_data_size =
            static_cast<size_type>(client_write);
        }
        else{
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("send: " << last_error_str());
          close_socket(m_client_sock);
          m_state_info.sock_created = false;
        }
      }
    } else if(sock_ready == socket_error) {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL("select: " << last_error_str());
    }
    else {
      // если select вернула 0, то событие не сработало
    }
    FD_CLR(sock_wr, &m_write_fds);
  }
  return wr_data_size;
}

irs::string irs::hardflow::tcp_client_t::param(const irs::string &a_name)
{
  irs::string param;
  return param;
}

void irs::hardflow::tcp_client_t::set_param(const irs::string &a_name,
  const irs::string &a_value)
{

}

void irs::hardflow::tcp_client_t::tick()
{
  if(!m_state_info.get_state_start()) {
    start_client();
  }
}

#endif // defined(IRS_WIN32) || defined(IRS_LINUX)

#endif //defined(IRS_WIN32) || defined(IRS_LINUX)

irs::hardflow::fixed_flow_t::fixed_flow_t(
  hardflow_t* ap_hardflow
):
  mp_hardflow(ap_hardflow),
  m_read_channel(0),
  mp_read_buf_cur(IRS_NULL),
  mp_read_buf(IRS_NULL),
  m_read_size_need(0),
  m_read_size_rest(0),
  m_read_status(status_success),
  m_read_timeout(make_cnt_s(2)),
  m_write_channel(0),
  mp_write_buf_cur(IRS_NULL),
  mp_write_buf(IRS_NULL),
  m_write_size_need(0),
  m_write_size_rest(0),
  m_write_status(status_success),
  m_write_timeout(make_cnt_s(2))
{
}

void irs::hardflow::fixed_flow_t::connect(hardflow_t* ap_hardflow) 
{
  mp_hardflow = ap_hardflow;
}

irs::hardflow::fixed_flow_t::size_type
  irs::hardflow::fixed_flow_t::read_byte_count() const
{
  return mp_read_buf_cur - mp_read_buf;
}

irs::hardflow::fixed_flow_t::size_type
  irs::hardflow::fixed_flow_t::write_byte_count() const
{
  return mp_write_buf_cur - mp_write_buf;
}

void irs::hardflow::fixed_flow_t::read_timeout(double a_read_timeout_sec)
{
  m_read_timeout.set(make_cnt_s(a_read_timeout_sec));
}

double irs::hardflow::fixed_flow_t::read_timeout() const
{
  return CNT_TO_DBLTIME(m_read_timeout.get());
}

void irs::hardflow::fixed_flow_t::write_timeout(
  double a_write_timeout_sec)
{
  m_write_timeout.set(make_cnt_s(a_write_timeout_sec));
}

double irs::hardflow::fixed_flow_t::write_timeout() const
{
  return CNT_TO_DBLTIME(m_write_timeout.get());
}

void irs::hardflow::fixed_flow_t::read(size_type a_channel_ident,
  irs_u8* ap_buf, size_type a_size)
{
  if (read_status() != status_wait) {
    m_read_status = status_wait;
    m_read_channel = a_channel_ident;
    mp_read_buf_cur = ap_buf;
    mp_read_buf = ap_buf;
    m_read_size_need = a_size;
    m_read_size_rest = a_size;
  }
}

irs::hardflow::fixed_flow_t::status_t
  irs::hardflow::fixed_flow_t::read_status()
{
  return m_read_status;
}

irs::hardflow::fixed_flow_t::size_type 
  irs::hardflow::fixed_flow_t::read_abort()
{
  IRS_LIB_ASSERT(m_read_size_need >= m_read_size_rest);
  size_type m_size_retr = m_read_size_need - m_read_size_rest;
  m_read_status = status_success;
  mp_read_buf_cur = IRS_NULL;
  m_read_size_rest = 0;
  m_read_size_need = 0;
  return m_size_retr;
}

void irs::hardflow::fixed_flow_t::write(size_type a_channel_ident,
  const irs_u8 *ap_buf, size_type a_size)
{
  if (write_status() != status_wait) {
    m_write_status = status_wait;
    m_write_channel = a_channel_ident;
    mp_write_buf_cur = ap_buf;
    mp_write_buf = ap_buf;
    m_write_size_need = a_size;
    m_write_size_rest = a_size;
  }
}

irs::hardflow::fixed_flow_t::status_t 
  irs::hardflow::fixed_flow_t::write_status()
{
  return m_write_status;
}

irs::hardflow::fixed_flow_t::size_type
  irs::hardflow::fixed_flow_t::write_abort()
{
  IRS_LIB_ASSERT(m_write_size_need >= m_write_size_rest);
  size_type m_size_rec = m_write_size_need - m_write_size_rest;
  m_write_status = status_success;
  mp_write_buf_cur = IRS_NULL;
  m_write_size_rest = 0;
  m_write_size_need = 0;
  return m_size_rec;
}

void irs::hardflow::fixed_flow_t::tick()
{
  if(read_status() == status_wait) {
    size_type read_size = mp_hardflow->read(m_read_channel, mp_read_buf_cur,
      m_read_size_rest);
    if (read_size == 0) {
      if (m_read_timeout.stopped()) {
        m_read_timeout.start();
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
          irsm("read timeout start()") << endl);
      }
    } else {
      m_read_timeout.stop();
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(irsm("read timeout stop()") << endl);
    }
    if (!m_read_timeout.check()) {
      if(mp_hardflow->is_channel_exists(m_read_channel)) {
        mp_read_buf_cur += read_size;
        m_read_size_rest -= read_size;
        if(mp_read_buf_cur >= (mp_read_buf + m_read_size_need)) {
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("read end") << endl);
          m_read_status = status_success;
        }
      }
      else {
        m_read_status = status_error;
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(
          irsm("read abort by channel is absent") << endl);
      }
    } else {
      m_read_status = status_error;
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(
        irsm("read abort by timeout") << endl);
    }
  }  
  if(write_status() == status_wait) {
    size_type write_size = mp_hardflow->write(m_write_channel, mp_write_buf_cur,
      m_write_size_rest);
    if(write_size == 0) {
      if(m_write_timeout.stopped()) {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(
          irsm("write timeout start()") << endl);
        m_write_timeout.start();
      }
    } else {
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(irsm("write timeout stop()") << endl);
      m_write_timeout.stop();
    }
    if(!m_write_timeout.check()) {
      if(mp_hardflow->is_channel_exists(m_write_channel)) {
        mp_write_buf_cur += write_size;
        m_write_size_rest -= write_size;
        if(mp_write_buf_cur >= (mp_write_buf + m_write_size_need) ) {
          IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(irsm("write end") << endl);
          m_write_status = status_success;
        }
      }
      else {
        IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(
          irsm("write abort by channel is absent") << endl);
        m_write_status = status_error;
      }
    } else {
      m_write_status = status_error;
      IRS_LIB_HARDFLOWG_DBG_RAW_MSG_DETAIL(
        irsm("write abort by timeout") << endl);
    }
  }
}

irs::hardflow::simple_udp_flow_t::simple_udp_flow_t(
  simple_tcpip_t* ap_simple_udp, 
  mxip_t a_local_ip,
  irs_u16 a_local_port,
  mxip_t a_dest_ip,
  irs_u16 a_dest_port,
  size_type a_channel_max_count
):
  mp_simple_udp(ap_simple_udp),
  m_local_ip(a_local_ip),
  m_local_port(a_local_port),
  m_dest_ip(a_dest_ip),
  m_dest_port(a_dest_port),
  m_channel(invalid_channel),
  m_cur_channel(invalid_channel),
  mp_recv_buf(mp_simple_udp->get_recv_buf()),
  mp_send_buf(mp_simple_udp->get_send_buf()),
  //m_max_channel_downtime(a_channel_life_time),
  m_channel_max_count(a_channel_max_count),
  m_channel_list(m_channel_max_count, udp_channel_t()),
  m_channel_list_it(m_channel_list.begin()),
  m_udp_max_data_size(65000),
  m_cur_dest_ip(mxip_t::zero_ip()),
  m_cur_dest_port(0)
{
  mp_simple_udp->open_udp();
  mp_simple_udp->open_port(m_local_port);
}

irs::hardflow::simple_udp_flow_t::~simple_udp_flow_t()
{
  mp_simple_udp->close_udp();
  mp_simple_udp->close_port(m_local_port);
}

void irs::hardflow::simple_udp_flow_t::view_channel_list()
{
  #if (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_BASE)
  mlog() << irsm("Channel List updated: m_channel = ") <<
    int(m_channel) << endl;
  for (size_t list_idx = 0; list_idx < m_channel_list.size(); list_idx++) {
    mlog() << irsm("Decue index ") << int(list_idx) << irsm(" ip : ") <<
      int(m_channel_list[list_idx].ip.val[0]) << irsm(".") <<
      int(m_channel_list[list_idx].ip.val[1]) << irsm(".") <<
      int(m_channel_list[list_idx].ip.val[2]) << irsm(".") <<
      int(m_channel_list[list_idx].ip.val[3]) << irsm(" port : ") <<
      int(m_channel_list[list_idx].port) << endl;
  }
  #endif // IRS_LIB_HARDFLOWG_DEBUG_TYPE
}

void irs::hardflow::simple_udp_flow_t::new_channel(mxip_t a_ip, irs_u16 a_port)
{
  udp_channel_t channel_content;
  channel_content.ip = a_ip;
  channel_content.port = a_port;
  if (find_if(m_channel_list.begin(), m_channel_list.end(),
    channel_equal_t(a_ip, a_port)) == m_channel_list.end()) 
  {
    m_channel++;
    if (m_channel == invalid_channel) {
      m_channel_list.clear();
      m_channel_list.resize(m_channel_max_count, udp_channel_t());
      m_channel++;
      m_channel_list.pop_back();
      m_channel_list.push_front(channel_content);
    } else {
      if (m_channel <= m_channel_max_count) {
        m_channel_list[m_channel - 1] = channel_content;
        mlog() << irsm("Add channel: ") << int(m_channel) << endl;
      } else {
        m_channel_list.pop_front();
        m_channel_list.push_back(channel_content);
      }
    }
    view_channel_list();
  }
}

irs::hardflow::simple_udp_flow_t::size_type 
  irs::hardflow::simple_udp_flow_t::channel_next()
{
  if (m_channel_list.size()) {
    if (m_channel <= m_channel_max_count) {
      if(m_cur_channel == m_channel) {
        m_cur_channel = 1;
      } else if (m_cur_channel < m_channel) {
        m_cur_channel++;
      }
    } else {
      if (m_cur_channel == m_channel) {
        m_cur_channel -= m_channel_max_count;
      } else if ((m_cur_channel < m_channel) && 
        (m_cur_channel >= (m_channel - m_channel_max_count) )) 
      {
        m_cur_channel++;
      }
    }
  }
  return m_cur_channel;
}

bool irs::hardflow::simple_udp_flow_t::is_channel_exists(
  size_type a_channel_ident)
{
  if(m_channel >= m_channel_max_count) {
    return (a_channel_ident <= m_channel) && 
      (a_channel_ident >= (m_channel - m_channel_list.size()));
  } else {
    return (a_channel_ident <= m_channel) &&
      (a_channel_ident != invalid_channel);
  }
}

irs::hardflow::simple_udp_flow_t::size_type 
  irs::hardflow::simple_udp_flow_t::read(size_type a_channel_ident, 
  irs_u8 *ap_buf, size_type a_size)
{
  size_type read_data_size = 0;
  IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(irsm("a_channel_ident_read = ") << 
    int(a_channel_ident) << endl);
  if (is_channel_exists(a_channel_ident)) {
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(irsm(" ************** read "
      "****************") << endl);
    m_cur_channel = a_channel_ident;
    if (a_size > m_udp_max_data_size) {
      a_size = m_udp_max_data_size;
    }
    mxip_t dest_ip = mxip_t::zero_ip();
    irs_u16 dest_port = 0;
    read_data_size =
      mp_simple_udp->read_udp(&dest_ip, &dest_port, &m_local_port);
    if (read_data_size == a_size) {
      mp_simple_udp->read_udp_complete();
    }
    if ((dest_ip == m_channel_list[m_cur_channel].ip) &&
      (dest_port == m_channel_list[m_cur_channel].port))
    {
      memcpyex(ap_buf, mp_recv_buf + 0x2a, read_data_size);
    } else {
      return 0;
    }
  } else {
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(irsm("Channel don't exist, "
      "read data not posible ") << endl);
  }
  return read_data_size;
}

irs::hardflow::simple_udp_flow_t::size_type 
  irs::hardflow::simple_udp_flow_t::write(size_type a_channel_ident, 
  const irs_u8 *ap_buf, size_type a_size)
{
  size_type write_data_size = 0;
  IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(irsm("a_channel_ident_write = ") <<
    int(a_channel_ident) << endl);
  if (is_channel_exists(a_channel_ident))
  {
    m_cur_channel = a_channel_ident;
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(irsm(" ***************** write "
      "************************") << endl);
    size_t deque_index = 0;
    if (m_channel >= m_channel_max_count) {
      deque_index = m_channel_list.size() - 
        (m_channel - m_cur_channel) - 1;
    } else {
      deque_index = m_cur_channel - 1;
    }
    if (a_size > m_udp_max_data_size) {
      a_size = m_udp_max_data_size;
    }
    if (mp_simple_udp->is_write_udp_complete()) {
      mp_simple_udp->write_udp(m_channel_list[deque_index].ip, 
        m_channel_list[deque_index].port, m_local_port, a_size);
    }
    /*
    mlog() << " m_channel_list.size() = " << int(m_channel_list.size()) << endl;
    mlog() << " m_cur_channel: " << int(m_cur_channel) << endl;
    mlog() << " m_channel: " << int(m_channel) << endl;
    mlog() << " deque_index = " << int(deque_index) << endl;*/
    #if (IRS_LIB_HARDFLOWG_DEBUG_TYPE == IRS_LIB_DEBUG_BASE)
    if (m_channel_list[deque_index].ip != mxip_t::zero_ip()) {
      mlog() << irsm(" write dest_ip = ") << 
        int(m_channel_list[deque_index].ip.val[0]) << irsm(".") <<
        int(m_channel_list[deque_index].ip.val[1]) << irsm(".") << 
        int(m_channel_list[deque_index].ip.val[2]) << irsm(".") <<
        int(m_channel_list[deque_index].ip.val[3]) << irsm(" dest_port = ") <<
        int(m_channel_list[deque_index].port) << endl;
    }
    #endif // IRS_LIB_HARDFLOWG_DEBUG_TYPE
    memcpyex(mp_send_buf + 0x2a, ap_buf, a_size);
  } else {
    IRS_LIB_HARDFLOWG_DBG_RAW_MSG_BASE(irsm("Channel don't exist,"
      "write data not posible ") << endl);
  }
  return write_data_size;
}

irs::string irs::hardflow::simple_udp_flow_t::param(
  const irs::string& /*a_name*/)
{
  irs::string param;
  return param;
}

void irs::hardflow::simple_udp_flow_t::set_param(const irs::string &/*a_name*/,
  const irs::string& /*a_value*/)
{

}

void irs::hardflow::simple_udp_flow_t::tick()
{
  mp_simple_udp->tick();
  if (mp_simple_udp->read_udp(&m_cur_dest_ip, &m_cur_dest_port,
    &m_local_port) > 20)
  {
    mp_simple_udp->read_udp_complete();
  }
  new_channel(m_cur_dest_ip, m_cur_dest_port);
}

