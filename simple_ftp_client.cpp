//! \file
//! \ingroup network_in_out_group
//! \brief Клиент протокола Simple FTP

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irserror.h>
#include <irsalg.h>

#include <simple_ftp_client.h>

#include <irsfinal.h>

namespace irs {

simple_ftp_client_t::simple_ftp_client_t(hardflow_t *ap_hardflow, fs_t* ap_fs):
  mp_hardflow(ap_hardflow),
  mp_fs(ap_fs),
  m_status(st_start_wait),
  m_fixed_flow(mp_hardflow),
  m_packet(),
  m_packet_id(0),
  m_oper_return(st_start_wait),
  m_file_size(0),
  m_is_checksum_error(false),
  m_packet_id_prev(0),
  m_is_packed_id_prev_exist(false),
  m_data_offset(0), // Указатель на текущую позицию в файле
  m_file(),
  m_trash_data_timer(make_cnt_ms(100)),
  m_server_version(0),
  m_file_path(2000, '\0'),
  m_data_offset_prev(0),
  m_start_read(false)
{
  char value = 35;
  for (size_t i = 0; i < m_file_path.size(); i++) {
    m_file_path[i] = value++;
    if (static_cast<irs_u8>(value) >= 127) {
      value = 35;
    }
  }
}
void simple_ftp_client_t::show_status() const
{
  static status_t status_prev = st_write_packet;

#define IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(status) \
case status: { \
  IRS_LIB_DBG_MSG(#status); \
} break;

  if (m_status != status_prev) {
    status_prev = m_status;
    switch (m_status) {
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_start_wait);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_version_command);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_version_command_wait);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_version_command_response);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_set_file_path_command);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_set_file_path_command_wait);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_set_file_path_command_response);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_set_file_path_ack_wait);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_set_file_path_ack_read);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_size_command);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_size_command_wait);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_size_command_response);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_command);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_command_wait);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_command_response);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_processing);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_send_ack_wait);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_show_data);

      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_write_packet);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_write_packet_wait);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_header);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_header_wait);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_trash_data_wait);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_data);
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_read_data_wait);
      default: {
        IRS_LIB_DBG_MSG("Неизвестный статус. Возможно забыли добавить этот статус.");
      }
    }
  }
}
irs_u32 simple_ftp_client_t::server_version() const
{
  return m_server_version;
}
void simple_ftp_client_t::start_read()
{
  m_start_read = true;
}
bool simple_ftp_client_t::is_done() const
{
  return !m_start_read;
}
void simple_ftp_client_t::tick()
{
  m_fixed_flow.tick();

  show_status();

  switch (m_status) {
    case st_start_wait: {
      if (m_start_read) {
        irs::mlog() << "Получить файл Запуск" << endl;
        m_status = st_read_version_command;
      }
    } break;
    case st_read_version_command: {
      m_packet.command = read_version_command;
      m_packet.data_size = 0;
      m_status = st_write_packet;
      m_oper_return = st_read_version_command_wait;
    } break;
    case st_read_version_command_wait: {
      m_status = st_read_header;
      m_oper_return = st_read_version_command_response;
    } break;
    case st_read_version_command_response: {
      if (!m_is_checksum_error && (m_packet.command == read_version_command) &&
        m_packet.data_size)
      {
        net_to_u32(m_packet.data, &m_server_version);
        IRS_LIB_DBG_MSG("simple_ftp m_server_version = " << m_server_version);
        m_status = st_set_file_path_command;
      } else {
        m_status = st_read_version_command;
      }
    } break;
    case st_set_file_path_command: {
      u32_to_net(m_file_path.size(), m_packet.data);
      m_data_offset = 0;
      const size_t remaining_size = m_file_path.size() - m_data_offset;
      const irs_u8 file_path_chunk_size = static_cast<irs_u8>(std::min<size_t>(
        packet_t::packet_data_max_size - sizeof(irs_u32), remaining_size));
      m_packet.data_size = file_path_chunk_size + sizeof(irs_u32);
      copy(
          m_file_path.begin() + m_data_offset,
          m_file_path.begin() + m_data_offset + file_path_chunk_size,
          m_packet.data + sizeof(irs_u32)
      );
      m_data_offset += file_path_chunk_size;
      m_packet_id_prev = m_packet_id;
      IRS_LIB_DBG_MSG("simple_ftp m_packed_id_prev = " << (int)m_packet_id_prev);

      m_packet.command = set_file_path_command;
      m_status = st_write_packet;
      m_oper_return = st_set_file_path_command_wait;
    } break;
    case st_set_file_path_command_wait: {
      m_status = st_read_header;
      m_oper_return = st_set_file_path_command_response;
    } break;
    case st_set_file_path_command_response: {
      IRS_LIB_DBG_MSG("simple_ftp m_packet_id_prev = " << (int)m_packet_id_prev);
      IRS_LIB_DBG_MSG("simple_ftp m_packet.packet_id = " << (int)m_packet.packet_id);
      if (!m_is_checksum_error && (m_packet.command == file_path_response) &&
        !m_packet.data_size && (m_packet_id_prev == m_packet.packet_id))
      {
        m_status = st_set_file_path_ack_wait;
      } else {
        m_status = st_set_file_path_command;
      }
    } break;
    case st_set_file_path_ack_wait: {
      bool is_send_packet_needed = false;
      if (!m_is_checksum_error && (m_packet.command == file_path_response) &&
        !m_packet.data_size && (m_packet_id_prev == m_packet.packet_id))
      {
        if (m_data_offset >= m_file_path.size()) {
          m_status = st_read_size_command;
        } else {
          is_send_packet_needed = true;
        }
      } else {
        is_send_packet_needed = true;
        m_packet_id = m_data_offset_prev;
        m_data_offset = m_data_offset_prev;
      }
      if (is_send_packet_needed) {
        size_t remaining_size = m_file_path.size() - m_data_offset;
        m_packet.data_size = static_cast<irs_u8>(std::min<size_t>(
          packet_t::packet_data_max_size, remaining_size));
        copy(
            m_file_path.begin() + m_data_offset,
            m_file_path.begin() + m_data_offset + m_packet.data_size,
            m_packet.data
        );
        m_data_offset_prev = m_data_offset;
        m_data_offset += m_packet.data_size;
        m_packet_id_prev = m_packet_id;
        IRS_LIB_DBG_MSG("simple_ftp Передано " << m_data_offset << " из " << m_file_path.size());

        m_packet.command = file_path_response;
        m_status = st_write_packet;
        m_oper_return = st_set_file_path_ack_read;
      }
    } break;
    case st_set_file_path_ack_read: {
      m_status = st_read_header;
      m_oper_return = st_set_file_path_ack_wait;
    } break;
    case st_read_size_command: {
      m_packet.command = read_size_command;
      m_packet.data_size = 0;
      m_status = st_write_packet;
      m_oper_return = st_read_size_command_wait;
    } break;
    case st_read_size_command_wait: {
      m_status = st_read_header;
      m_oper_return = st_read_size_command_response;
    } break;
    case st_read_size_command_response: {
      if (!m_is_checksum_error && (m_packet.command == read_size_command) && m_packet.data_size) {
        net_to_u32(m_packet.data, &m_file_size);
        IRS_LIB_DBG_MSG("simple_ftp m_file_size = " << m_file_size);
        m_status = st_read_command;
      } else {
        m_status = st_read_size_command;
      }
    } break;
    case st_read_command: {
      m_packet.command = read_command;
      m_packet.data_size = 0;
      m_status = st_write_packet;
      m_oper_return = st_read_command_wait;
    } break;
    case st_read_command_wait: {
      IRS_LIB_DBG_MSG("simple_ftp Прием пакета");
      m_status = st_read_header;
      m_oper_return = st_read_command_response;
    } break;
    case st_read_command_response: {
      if (!m_is_checksum_error && (m_packet.command == read_command_response) &&
        (m_packet.data_size > 0) && (m_packet.data_size <= packet_t::packet_data_max_size))
      {
        m_status = st_read_processing;
        m_is_packed_id_prev_exist = false;
        m_data_offset = 0;
      } else {
        m_status = st_read_command;
      }
    } break;
    case st_read_processing: {
//        static int read_cnt = 0;
//        read_cnt++;
//        IRS_LIB_DBG_MSG("simple_ftp read_cnt = " << read_cnt);
//        if (read_cnt > 10) {
//          read_cnt = 0;
//          m_status = st_start_wait;
//          break;
//        }

      if (!m_is_checksum_error && (m_packet.command == read_command_response) &&
        (m_packet.data_size > 0) && (m_packet.data_size <= packet_t::packet_data_max_size))
      {
        // m_is_packed_id_prev_exist, так просто, нельзя объединить под одним if, т. к.
        // иначе последний else будет некорректен
        if (!m_is_packed_id_prev_exist || (m_packet_id_prev == m_packet.packet_id - 1)) {
          std::copy(m_packet.data, m_packet.data + m_packet.data_size,
            std::back_inserter(m_file));
          m_data_offset += m_packet.data_size;
          m_packet.command = read_command_response;
          m_packet_id = m_packet.packet_id;
          m_packet_id_prev = m_packet.packet_id;
          m_is_packed_id_prev_exist = true;
          IRS_LIB_DBG_MSG("simple_ftp st_read_processing ok");
        } else if (!m_is_packed_id_prev_exist || (m_packet_id_prev == m_packet.packet_id)) {
          // Сюда попадаем только если мы приняли ранее пакет успешно, а сервер получил
          // неудачное подтверждение. Это повтор пакета от сервера ранее принятого нами
          // успешно. Поэтому ничего не копируем и отправляем подтверждение успеха.
          m_packet.command = read_command_response;
        } else {
          m_packet.command = error_command;
          IRS_LIB_DBG_MSG("simple_ftp st_read_processing packet_id error");
        }
      } else {

        if (m_is_checksum_error) {
          IRS_LIB_DBG_MSG("simple_ftp m_is_checksum_error");
        }
        if (m_packet.command != read_command_response) {
          IRS_LIB_DBG_MSG("simple_ftp m_packet.command != read_command_response");
          IRS_LIB_DBG_MSG("simple_ftp m_packet.command = " << (int)m_packet.command);
        }
        if (!((m_packet.data_size > 0) && (m_packet.data_size <= packet_t::packet_data_max_size)))
        {
          IRS_LIB_DBG_MSG("simple_ftp Размер пакета не в допуске: " << (int)m_packet.data_size);
        }

        m_packet.command = error_command;
        IRS_LIB_DBG_MSG("simple_ftp st_read_processing packet error");
      }

      m_packet.data_size = 0;
      m_status = st_write_packet;
      m_oper_return = st_send_ack_wait;
    } break;
    case st_send_ack_wait: {
      // Если весь файл принят, переходим в состояние ожидания
      // В отладочном коде переходим к отображению принятого файла
      if (m_data_offset >= m_file_size) {
        m_status = st_show_data;
      } else {
        m_status = st_read_header;
        m_oper_return = st_read_processing;
      }
    } break;
    case st_show_data: {
      #ifdef IRS_LIB_DEBUG
      size_t size = m_file.size();
      irs::mlog() << "m_file.size() = " << size << endl;
      for (size_t i = 0; i < size; i++) {
        irs::mlog() << static_cast<int>(m_file[i]) << ' ';
      }
      irs::mlog() << endl;
      #endif //IRS_LIB_DEBUG

      // Сообщить о завершении всех операций функции is_done
      m_start_read = false;

      m_status = st_start_wait;
    } break;

    case st_write_packet: {
      m_packet.packet_id = m_packet_id;
      m_packet_id++;

      IRS_LIB_DBG_MSG("simple_ftp Отправка");
      IRS_LIB_DBG_MSG("simple_ftp m_packet.packet_id = " << (int)m_packet.packet_id);
      IRS_LIB_DBG_MSG("simple_ftp m_packet.command = " << (int)m_packet.command);
      IRS_LIB_DBG_MSG("simple_ftp m_packet.data_size = " << (int)m_packet.data_size);

      size_t header_size = sizeof(packet_t) - packet_t::packet_data_max_size;
      m_packet.header_checksum = crc8(reinterpret_cast<irs_u8*>(&m_packet), 0,
        header_size - packet_t::checksum_size);
      m_packet.data_checksum = 0;
      if (m_packet.data_size) {
        m_packet.data_checksum = irs::crc8(reinterpret_cast<irs_u8*>(&m_packet.data), 0,
          m_packet.data_size);
      }
      size_t packet_size = header_size + m_packet.data_size;
      m_fixed_flow.write(channel, reinterpret_cast<irs_u8*>(&m_packet), packet_size);
      m_status = st_write_packet_wait;
    } break;
    case st_write_packet_wait: {
      irs::hardflow::fixed_flow_t::status_t status = m_fixed_flow.write_status();
      switch (status) {
        case irs::hardflow::fixed_flow_t::status_success: {
          m_status = m_oper_return;
        } break;
        case irs::hardflow::fixed_flow_t::status_error: {
          IRS_LIB_DBG_MSG("simple_ftp Ошибка записи пакета");
          m_status = st_start_wait;
        } break;
      }
    } break;

    case st_read_header: {
      size_t header_size = sizeof(packet_t) - packet_t::packet_data_max_size;
      m_fixed_flow.read(channel, reinterpret_cast<irs_u8*>(&m_packet), header_size);
      m_status = st_read_header_wait;
    } break;
    case st_read_header_wait: {
      irs::hardflow::fixed_flow_t::status_t status = m_fixed_flow.read_status();
      switch (status) {
        case irs::hardflow::fixed_flow_t::status_success: {

          IRS_LIB_DBG_MSG("simple_ftp Прием");
          IRS_LIB_DBG_MSG("simple_ftp m_packet.packet_id = " << (int)m_packet.packet_id);
          IRS_LIB_DBG_MSG("simple_ftp m_packet.command = " << (int)m_packet.command);
          IRS_LIB_DBG_MSG("simple_ftp m_packet.data_size = " << (int)m_packet.data_size);

          size_t header_size = sizeof(packet_t) - packet_t::packet_data_max_size;
          irs_u8 checksum_calculated = irs::crc8(
            reinterpret_cast<irs_u8*>(&m_packet), 0, header_size - packet_t::checksum_size
          );
          if (m_packet.header_checksum == checksum_calculated) {
            if (m_packet.data_size) {
              m_status = st_read_data;
            } else {
              m_status = m_oper_return;
            }
          } else {
            IRS_LIB_DBG_MSG("simple_ftp Ошибка контрольной суммы заголовка");

            // Очистка недочитанных данных
            m_fixed_flow.read(
              channel, reinterpret_cast<irs_u8*>(&m_packet.data), packet_t::packet_data_max_size
            );

            m_is_checksum_error = true;
            m_trash_data_timer.start();
            m_status = st_read_trash_data_wait;
          }
        } break;
        case irs::hardflow::fixed_flow_t::status_error: {
          IRS_LIB_DBG_MSG("simple_ftp Ошибка чтения заголовка");
          m_status = st_start_wait;
        } break;
      }
    } break;
    case st_read_trash_data_wait: {
      if (m_trash_data_timer.check()) {
        m_fixed_flow.read_abort();
        m_status = m_oper_return;
      }
    } break;
    case st_read_data: {
      m_fixed_flow.read(channel, reinterpret_cast<irs_u8*>(m_packet.data), m_packet.data_size);
      m_status = st_read_data_wait;
    } break;
    case st_read_data_wait: {
      irs::hardflow::fixed_flow_t::status_t status = m_fixed_flow.read_status();
      switch (status) {
        case irs::hardflow::fixed_flow_t::status_success: {
          irs_u8 checksum_calculated =
            irs::crc8(reinterpret_cast<irs_u8*>(&m_packet.data), 0, m_packet.data_size);
          if (m_packet.data_checksum == checksum_calculated) {
            m_status = m_oper_return;
          } else {
            IRS_LIB_DBG_MSG("simple_ftp Ошибка контрольной суммы данных");
            m_is_checksum_error = true;
            m_status = m_oper_return;
          }
        } break;
        case irs::hardflow::fixed_flow_t::status_error: {
          IRS_LIB_DBG_MSG("simple_ftp Ошибка чтения данных");
          m_status = st_start_wait;
        } break;
      }
    } break;
  }
}

void simple_ftp_client_t::net_to_u32(irs_u8* ap_data, irs_u32* ap_u32)
{
  *ap_u32 =
    (static_cast<irs_u32>(ap_data[0]) << 24) |
    (static_cast<irs_u32>(ap_data[1]) << 16) |
    (static_cast<irs_u32>(ap_data[2]) << 8)  |
     static_cast<irs_u32>(ap_data[3]);
}
void simple_ftp_client_t::u32_to_net(irs_u32 a_u32, irs_u8* ap_data)
{
  ap_data[0] = static_cast<irs_u8>((a_u32 >> 24) & 0xFF);
  ap_data[1] = static_cast<irs_u8>((a_u32 >> 16) & 0xFF);
  ap_data[2] = static_cast<irs_u8>((a_u32 >> 8) & 0xFF);
  ap_data[3] = static_cast<irs_u8>(a_u32 & 0xFF);
}

} // namespace irs

