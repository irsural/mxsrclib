//! \file
//! \ingroup network_in_out_group
//! \brief Сервер протокола Simple FTP

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irsalg.h>
#include <irserror.h>
#include <simple_ftp_server.h>

#include <irsfinal.h>

namespace irs {

simple_ftp_server_t::simple_ftp_server_t(hardflow_t* ap_hardflow, fs_t* ap_fs):
  m_server_version(1),
  mp_hardflow(ap_hardflow),
  mp_fs(ap_fs),
  m_status(st_start_wait),
  m_fixed_flow(mp_hardflow),
  m_packet(),
  m_packet_id(0),
  m_packet_id_prev(0),
  m_oper_return(st_start_wait),
  m_is_read_time_inf(false),
  m_file(2000),
  m_data_offset(0), // Указатель на текущую позицию в файле
  m_check_ack_packet_id(0),
  m_is_checksum_error(false),
  m_trash_data_timer(make_cnt_ms(100)),
  m_file_path(),
  m_file_path_size(0)
{
  // IRS_LIB_DBG_MSG("simple_ftp Конструктор simple_ftp_server_t");
  irs_u8 value = 0;
  for (size_t i = 0; i < m_file.size(); i++) {
    m_file[i] = value++;
  }
}

void simple_ftp_server_t::show_status() const
{
  static status_t status_prev = st_write_packet;

#define IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(status) \
  case status: { \
    IRS_LIB_DBG_MSG(#status); \
  } break;

  if (m_status != status_prev) {
    status_prev = m_status;
    switch (m_status) {
      // IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_start_wait);
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_command_processing);
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_set_file_path_ack_wait);
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_set_file_path_processing);
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_show_file_path);
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_read_processing);
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_read_ack);
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_check_ack);
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_write_packet);
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_write_packet_wait);
      // IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_read_header);
      // IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_read_header_wait);
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_read_trash_data_wait);
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_read_data);
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_read_data_wait);
    }
  }
}

void simple_ftp_server_t::tick()
{
  m_fixed_flow.tick();

  show_status();

  switch (m_status) {
    case st_start_wait: {
      m_status = st_read_header;
      m_oper_return = st_command_processing;
      m_is_read_time_inf = true;
      m_data_offset = 0;
    } break;
    case st_command_processing: {
      if (m_is_checksum_error) {
        m_packet.command = error_command;
        m_packet.data_size = 0;
        m_status = st_write_packet;
        m_oper_return = st_start_wait;
      } else {
        switch (m_packet.command) {
          case read_version_command: {
            if (!m_packet.data_size) {
              IRS_LIB_DBG_MSG("simple_ftp Команда чтения версии");
              m_packet.data_size = 4;
              irs_u32 version = m_server_version;
              u32_to_net(version, m_packet.data);
              m_status = st_write_packet;
              m_oper_return = st_start_wait;
            } else {
              m_packet.command = error_command;
              m_packet.data_size = 0;
              m_status = st_write_packet;
              m_oper_return = st_start_wait;
            }
          } break;
          case set_file_path_command: {
            if ((m_packet.data_size >= sizeof(irs_u32)) &&
                (m_packet.data_size <= packet_t::data_max_size))
            {
              IRS_LIB_DBG_MSG("simple_ftp Команда записи пути к файлу");
              m_data_offset = 0;
              m_file_path.clear();
              net_to_u32(m_packet.data, &m_file_path_size);
              copy(
                m_packet.data + sizeof(irs_u32),
                m_packet.data + m_packet.data_size,
                back_inserter(m_file_path)
              );
              m_data_offset += (m_packet.data_size - sizeof(irs_u32));
              m_packet_id = m_packet.packet_id;
              m_packet_id_prev = m_packet.packet_id;

              m_packet.command = file_path_response;
              m_packet.data_size = 0;
              m_status = st_write_packet;
              m_oper_return = st_set_file_path_ack_wait;
            } else {
              m_packet.command = error_command;
              m_packet.data_size = 0;
              m_status = st_write_packet;
              m_oper_return = st_start_wait;
            }
          } break;
          case read_size_command: {
            if (!m_packet.data_size) {
              IRS_LIB_DBG_MSG("simple_ftp Команда чтения размера");
              m_packet.data_size = 4;
              irs_u32 size = m_file.size();
              u32_to_net(size, m_packet.data);
              m_status = st_write_packet;
              m_oper_return = st_start_wait;
            } else {
              m_packet.command = error_command;
              m_packet.data_size = 0;
              m_status = st_write_packet;
              m_oper_return = st_start_wait;
            }
          } break;
          case read_command: {
            if (!m_packet.data_size) {
              IRS_LIB_DBG_MSG("simple_ftp Команда чтения данных");
              m_status = st_read_processing;
            } else {
              m_packet.command = error_command;
              m_packet.data_size = 0;
              m_status = st_write_packet;
              m_oper_return = st_start_wait;
            }
          } break;
          default: {
            m_status = st_start_wait;
          } break;
        }
      };
      ;
    } break;
    case st_set_file_path_ack_wait: {
      if (m_data_offset >= m_file_path_size) {
        // Если весь файл принят, переходим в состояние ожидания
        // В отладочном коде переходим к отображению пути к файлу
        m_status = st_show_file_path;
        if (m_file_path_size == 0) {
          m_file_path = "";
        }
      } else {
        m_status = st_read_header;
        m_oper_return = st_set_file_path_processing;
      }
    } break;
    case st_set_file_path_processing: {
      if (!m_is_checksum_error && (m_packet.command == set_file_path_command)) {
        m_status = st_command_processing;
        break;
      }
      if (!m_is_checksum_error && (m_packet.command == file_path_response) &&
          (m_packet.data_size > 0) && (m_packet.data_size <= packet_t::data_max_size))
      {
        if (m_packet_id_prev == m_packet.packet_id - 1) {
          std::copy(
            m_packet.data, m_packet.data + m_packet.data_size, std::back_inserter(m_file_path)
          );
          m_data_offset += m_packet.data_size;
          m_packet.command = file_path_response;
          m_packet_id = m_packet.packet_id;
          m_packet_id_prev = m_packet.packet_id;
          IRS_LIB_DBG_MSG(
            "simple_ftp m_file_path.size() = " << m_file_path.size() << " from " << m_file_path_size
          );
          IRS_LIB_DBG_MSG("simple_ftp st_set_file_path_processing ok");
        } else if (m_packet_id_prev == m_packet.packet_id) {
          // Сюда попадаем только если мы приняли ранее пакет успешно, а клиент получил
          // неудачное подтверждение. Это повтор пакета от клиента ранее принятого нами
          // успешно. Поэтому ничего не копируем и отправляем подтверждение успеха.
          m_packet.command = file_path_response;
        } else {
          m_packet.command = error_command;
          IRS_LIB_DBG_MSG("simple_ftp st_set_file_path_processing packet_id error");
        }
      } else {
        if (m_is_checksum_error) {
          IRS_LIB_DBG_MSG("simple_ftp m_is_checksum_error");
        }
        if (m_packet.command != file_path_response) {
          IRS_LIB_DBG_MSG("simple_ftp m_packet.command != file_path_response");
          IRS_LIB_DBG_MSG("simple_ftp m_packet.command = " << (int)m_packet.command);
        }
        if (!((m_packet.data_size > 0) && (m_packet.data_size <= packet_t::data_max_size))) {
          IRS_LIB_DBG_MSG("simple_ftp Размер пакета не в допуске: " << (int)m_packet.data_size);
        }

        m_packet.command = error_command;
        IRS_LIB_DBG_MSG("simple_ftp st_read_processing packet error");
      }

      m_packet.data_size = 0;
      m_status = st_write_packet;
      m_oper_return = st_set_file_path_ack_wait;
    } break;
    case st_show_file_path: {
      IRS_LIB_DBG_MSG("simple_ftp Размер пути к файлу: " << m_file_path_size);
      IRS_LIB_DBG_MSG("simple_ftp m_file_path.size() = " << m_file_path.size());
      IRS_LIB_DBG_MSG("simple_ftp Путь к файлу: \"" << m_file_path << "\"");
      m_status = st_start_wait;
    } break;
    case st_read_processing: {
      m_packet.command = read_command_response;
      size_t remaining_size = m_file.size() - m_data_offset;
      m_packet.data_size = std::min<size_t>(packet_t::data_max_size, remaining_size);
      memcpy(&m_packet.data[0], &m_file[m_data_offset], m_packet.data_size);
      // irs::memcpyex(m_packet.data, &m_file[m_data_offset], m_packet.data_size);

      // Инкрементируем смещение для следующего пакета
      m_data_offset += m_packet.data_size;

      IRS_LIB_DBG_MSG("simple_ftp Отправка пакета");
      m_check_ack_packet_id = m_packet_id;
      m_status = st_write_packet;
      m_oper_return = st_read_ack;
    } break;
    case st_read_ack: {
      IRS_LIB_DBG_MSG("simple_ftp Прием пакета");

      m_status = st_read_header;
      m_oper_return = st_check_ack;
      m_is_read_time_inf = false;
    } break;
    case st_check_ack: {
      if (!m_is_checksum_error && (m_packet.command == read_command)) {
        m_status = st_command_processing;
        break;
      }
      if (!m_is_checksum_error && !m_packet.data_size &&
          (m_packet.command == read_command_response) &&
          (m_packet.packet_id == m_check_ack_packet_id))
      {
        if (m_data_offset >= m_file.size()) {
          // Если весь файл отправлен, переходим в состояние ожидания
          m_status = st_start_wait;
        } else {
          m_status = st_read_processing;
        }
      } else {
        IRS_LIB_DBG_MSG("simple_ftp Ошибка подтверждения. Повтор пакета.");
        m_data_offset -= m_packet.data_size;
        m_packet_id--;
        m_status = st_read_processing;
      }
    } break;

    case st_write_packet: {
      m_packet.packet_id = m_packet_id;
      m_packet_id++;

      IRS_LIB_DBG_MSG("simple_ftp Отправка");
      IRS_LIB_DBG_MSG("simple_ftp m_packet.packet_id = " << (int)m_packet.packet_id);
      IRS_LIB_DBG_MSG("simple_ftp m_packet.command = " << (int)m_packet.command);
      IRS_LIB_DBG_MSG("simple_ftp m_packet.data_size = " << (int)m_packet.data_size);

      size_t header_size = sizeof(packet_t) - packet_t::data_max_size;
      m_packet.header_checksum =
        irs::crc8(reinterpret_cast<irs_u8*>(&m_packet), 0, header_size - packet_t::checksum_size);
      m_packet.data_checksum = 0;
      if (m_packet.data_size) {
        m_packet.data_checksum =
          irs::crc8(reinterpret_cast<irs_u8*>(&m_packet.data), 0, m_packet.data_size);
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
      size_t header_size = sizeof(packet_t) - packet_t::data_max_size;
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

          size_t header_size = sizeof(packet_t) - packet_t::data_max_size;
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
              channel, reinterpret_cast<irs_u8*>(&m_packet.data), packet_t::data_max_size
            );

            m_is_checksum_error = true;
            m_trash_data_timer.start();
            m_status = st_read_trash_data_wait;
          }
        } break;
        case irs::hardflow::fixed_flow_t::status_error: {
          if (m_is_read_time_inf) {
            m_status = st_read_header;
          } else {
            IRS_LIB_DBG_MSG("simple_ftp Ошибка чтения заголовка");
            m_status = st_start_wait;
          }
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

void simple_ftp_server_t::net_to_u32(irs_u8* ap_data, irs_u32* ap_u32)
{
  *ap_u32 = (static_cast<irs_u32>(ap_data[0]) << 24) | (static_cast<irs_u32>(ap_data[1]) << 16) |
    (static_cast<irs_u32>(ap_data[2]) << 8) | static_cast<irs_u32>(ap_data[3]);
}

void simple_ftp_server_t::u32_to_net(irs_u32 a_u32, irs_u8* ap_data)
{
  ap_data[0] = static_cast<irs_u8>((a_u32 >> 24) & 0xFF);
  ap_data[1] = static_cast<irs_u8>((a_u32 >> 16) & 0xFF);
  ap_data[2] = static_cast<irs_u8>((a_u32 >> 8) & 0xFF);
  ap_data[3] = static_cast<irs_u8>(a_u32 & 0xFF);
}

} // namespace irs
