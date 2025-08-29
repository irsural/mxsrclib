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
  mp_file(IRS_NULL),
  m_is_file_opened(false),
  m_file_size(0),
  m_data_offset(0), // Указатель на текущую позицию в файле
  mp_dir_iterator(IRS_NULL),
  m_check_ack_packet_id(0),
  m_is_checksum_error(false),
  m_trash_data_timer(make_cnt_ms(100)),
  m_is_file(false),
  m_is_dir(false),
  m_dir_info_buf(),
  m_fs_result(fsr_success),
  m_file_path(),
  m_file_path_size(0)
{
  m_dir_info_buf.reserve(m_dir_info_buf_reserve);
}

simple_ftp_server_t::~simple_ftp_server_t()
{
  close_file();
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
      IRS_LIB_SIMPLE_FTP_SRV_SHOW_STATUS(st_get_dir_size);
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

#ifdef IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_STATUS
  show_status();
#endif //IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_STATUS

  switch (m_status) {
    case st_start_wait: {
      m_status = st_read_header;
      m_oper_return = st_command_processing;
      m_is_read_time_inf = true;
      m_data_offset = 0;
      close_file();
      mp_dir_iterator.reset();
      if (!m_dir_info_buf.empty()) {
        vector<irs_u8> dir_info_reset_buf;
        dir_info_reset_buf.reserve(m_dir_info_buf_reserve);
        swap(m_dir_info_buf, dir_info_reset_buf);
      }
      memsetex(&m_packet, 1);
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
              IRS_LIB_SIMP_FTP_SR_DBG_MSG_BASE("simple_ftp Команда чтения версии");
              m_packet.data_size = 4;
              irs_u32 version = m_server_version;
              u32_to_net(version, m_packet.data);
              m_status = st_write_packet;
              m_oper_return = st_start_wait;
            } else {
              error_response();
            }
          } break;
          case set_file_path_command: {
            if ((m_packet.data_size >= sizeof(irs_u32)) &&
                (m_packet.data_size <= packet_t::data_max_size))
            {
              IRS_LIB_SIMP_FTP_SR_DBG_MSG_BASE("simple_ftp Команда записи пути к файлу");
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
              error_response();
            }
          } break;
          case read_size_command: {
            bool is_ok = (m_packet.data_size == 0);
            m_is_file = false;
            if (is_ok) {
              fs_result_t fs_result = irs::fsr_success;
              m_is_file = mp_fs->is_file_exists(m_file_path, &fs_result);
            }
            m_is_dir = !m_is_file;
            if (is_ok) {
              IRS_LIB_SIMP_FTP_SR_DBG_MSG_BASE("simple_ftp Команда чтения размера");
              if (m_is_file) {
                is_ok = open_file();
              } else if (m_is_dir) {
                m_file_size = 0;
                mp_dir_iterator = mp_fs->get_dir_iterator(m_file_path);
                m_status = st_get_dir_size;
              } else {
                is_ok = false;
              }
            }
            if (m_is_file) {
              if (is_ok) {
                is_ok = get_file_size(&m_file_size);
              }
              if (is_ok) {
                file_size_response();
              }
            }
            if (!is_ok) {
              error_response();
            }
          } break;
          case read_command: {
            IRS_LIB_SIMP_FTP_SR_DBG_MSG_BASE("simple_ftp Команда чтения данных");
            bool is_ok = (m_packet.data_size == 0);
            if (m_is_file) {
              if (is_ok) {
                is_ok = open_file();
              }
              if (is_ok) {
                irs_u32 file_size = 0;
                is_ok = get_file_size(&file_size);
                is_ok = is_ok && (file_size == m_file_size);
              }
            }
            if (m_is_dir) {
              mp_dir_iterator.reset();
              mp_dir_iterator = mp_fs->get_dir_iterator(m_file_path);
              m_fs_result = fsr_success;
              // dir_info_size = 0;
            }
            if (is_ok) {
              m_status = st_read_processing;
            }
            if (!is_ok) {
              error_response();
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
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL(
            "simple_ftp m_file_path.size() = " << m_file_path.size() << " from " << m_file_path_size
          );
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp st_set_file_path_processing ok");
        } else if (m_packet_id_prev == m_packet.packet_id) {
          // Сюда попадаем только если мы приняли ранее пакет успешно, а клиент получил
          // неудачное подтверждение. Это повтор пакета от клиента ранее принятого нами
          // успешно. Поэтому ничего не копируем и отправляем подтверждение успеха.
          m_packet.command = file_path_response;
        } else {
          m_packet.command = error_command;
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL(
            "simple_ftp st_set_file_path_processing packet_id error"
          );
        }
      } else {
        if (m_is_checksum_error) {
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp m_is_checksum_error");
        }
        if (m_packet.command != file_path_response) {
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp m_packet.command != file_path_response");
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL(
            "simple_ftp m_packet.command = " << (int)m_packet.command
          );
        }
        if (!((m_packet.data_size > 0) && (m_packet.data_size <= packet_t::data_max_size))) {
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL(
            "simple_ftp Размер пакета не в допуске: " << (int)m_packet.data_size
          );
        }

        m_packet.command = error_command;
        IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp st_read_processing packet error");
      }

      m_packet.data_size = 0;
      m_status = st_write_packet;
      m_oper_return = st_set_file_path_ack_wait;
    } break;
    case st_show_file_path: {
      IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Размер пути к файлу: " << m_file_path_size);
      IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp m_file_path.size() = " << m_file_path.size());
      IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Путь к файлу: \"" << m_file_path << "\"");
      m_status = st_start_wait;
    } break;
    case st_get_dir_size: {
      file_info_t file_info;
      fs_result_t fs_result = mp_dir_iterator->next_dir_item(&file_info);
      switch (fs_result) {
        case fsr_success: {
          m_file_size += sizeof(file_info_sf_t) + file_info.name.size();
        } break;
        case fsr_end_of_dir: {
          file_size_response();
        } break;
        default: {
          error_response();
        } break;
      }
    } break;
    case st_read_processing: {
      m_packet.command = read_command_response;
      const size_t remaining_size = m_file_size - m_data_offset;
      m_packet.data_size = std::min<size_t>(packet_t::data_max_size, remaining_size);
      if (m_is_file) {
        mp_fs->read(mp_file, m_packet.data, m_packet.data_size, &m_fs_result);
      }
      if (m_is_dir) {
        memsetex(m_packet.data, m_packet.data_size);
        if (m_fs_result != fsr_end_of_dir) {
          m_fs_result = dir_info_to_packet();
        }
      }
      if (m_fs_result == fsr_success) {
        // Инкрементируем смещение для следующего пакета
        m_data_offset += m_packet.data_size;

        IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Отправка пакета");
        m_check_ack_packet_id = m_packet_id;
        m_status = st_write_packet;
        m_oper_return = st_read_ack;
      } else {
        IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Ошибка чтения файла: ");
        error_response();
      }
    } break;
    case st_read_ack: {
      IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Прием пакета");

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
        if (m_data_offset >= m_file_size) {
          // Если весь файл отправлен, переходим в состояние ожидания
          m_status = st_start_wait;
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_BASE("simple_ftp The file was transferred in full");
        } else {
          m_status = st_read_processing;
        }
      } else {
        IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Ошибка подтверждения. Повтор пакета.");
        m_data_offset -= m_packet.data_size;
        m_packet_id--;
        m_status = st_read_processing;
      }
    } break;

    case st_write_packet: {
      m_packet.packet_id = m_packet_id;
      m_packet_id++;

      IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Отправка");
      IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL(
        "simple_ftp m_packet.packet_id = " << (int)m_packet.packet_id
      );
      IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp m_packet.command = " << (int)m_packet.command);
      IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL(
        "simple_ftp m_packet.data_size = " << (int)m_packet.data_size
      );

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
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Ошибка записи пакета");
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

          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Прием");
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL(
            "simple_ftp m_packet.packet_id = " << (int)m_packet.packet_id
          );
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL(
            "simple_ftp m_packet.command = " << (int)m_packet.command
          );
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL(
            "simple_ftp m_packet.data_size = " << (int)m_packet.data_size
          );

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
            IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Ошибка контрольной суммы заголовка");

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
            IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Ошибка чтения заголовка");
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
            IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Ошибка контрольной суммы данных");
            m_is_checksum_error = true;
            m_status = m_oper_return;
          }
        } break;
        case irs::hardflow::fixed_flow_t::status_error: {
          IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL("simple_ftp Ошибка чтения данных");
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

bool simple_ftp_server_t::open_file()
{
  if (!m_is_file_opened) {
    mp_file = mp_fs->open(m_file_path, fm_read);
    m_is_file_opened = (mp_file != IRS_NULL);
  }
  return m_is_file_opened;
}

void simple_ftp_server_t::close_file()
{
  if (m_is_file_opened) {
    fs_result_t fsr = mp_fs->close(mp_file);
    if (fsr == fsr_success) {
      mp_file = IRS_NULL;
      m_is_file_opened = false;
    }
  }
}

bool simple_ftp_server_t::get_file_size(irs_u32* ap_file_size) const
{
  fs_result_t fs_result = irs::fsr_success;
  *ap_file_size = static_cast<irs_u32>(mp_fs->get_file_size(mp_file, &fs_result));
  return (fs_result == irs::fsr_success);
}

void simple_ftp_server_t::error_response()
{
  m_packet.command = error_command;
  m_packet.data_size = 0;
  m_status = st_write_packet;
  m_oper_return = st_start_wait;
}

void simple_ftp_server_t::file_size_response()
{
  m_packet.data_size = sizeof(irs_u32) + sizeof(irs_u8);
  u32_to_net(m_file_size, m_packet.data);
  irs_u8& is_dir = m_packet.data[sizeof(irs_u32)];
  is_dir = m_is_dir ? 1 : 0;
  m_status = st_write_packet;
  m_oper_return = st_start_wait;
}

fs_result_t simple_ftp_server_t::dir_info_to_packet()
{
  fs_result_t fs_result = fsr_success;

  const size_t packet_data_size = m_packet.data_size;
  if (m_dir_info_buf.size() >= packet_data_size) {
    memcpy(m_packet.data, &m_dir_info_buf[0], packet_data_size);
    m_dir_info_buf.resize(m_dir_info_buf.size() - packet_data_size);
  } else { //if (m_dir_info_buf.size() >= packet_data_max_size) {
    memcpyex(m_packet.data, &m_dir_info_buf[0], m_dir_info_buf.size());
    size_t dir_info_size = m_dir_info_buf.size();
    m_dir_info_buf.clear();

    file_info_t file_info;
    while ((fs_result = mp_dir_iterator->next_dir_item(&file_info)) != fsr_end_of_dir) {
      if (fs_result == fsr_success) {
        file_info_sf_t file_info_sf;
        file_info_sf.is_dir = file_info.is_dir ? 1 : 0;
        file_info_sf.size = static_cast<irs_u32>(file_info.size);
        file_info_sf.name_size = static_cast<irs_u32>(file_info.name.size());
        if (sizeof(file_info_sf_t) + dir_info_size < packet_data_size) {
          memcpy(m_packet.data + dir_info_size, &file_info_sf, sizeof(file_info_sf_t));
          dir_info_size += sizeof(file_info_sf_t);
          if (dir_info_size + file_info_sf.name_size < packet_data_size) {
            copy(file_info.name.begin(), file_info.name.end(), m_packet.data + dir_info_size);
            dir_info_size += file_info_sf.name_size;
            // Уходим на следующую итерацию, т. к. пакет не заполнен
          } else {
            const size_t file_name_chunk_size = packet_data_size - dir_info_size;
            copy(
              file_info.name.begin(),
              file_info.name.begin() + file_name_chunk_size,
              m_packet.data + dir_info_size
            );
            dir_info_size += file_name_chunk_size;

            m_dir_info_buf.resize(file_info_sf.name_size - file_name_chunk_size);
            copy(
              file_info.name.begin() + file_name_chunk_size,
              file_info.name.end(),
              m_dir_info_buf.begin()
            );

            // Завершаем цикл, т. к. пакет заполнен
            break;
          }
        } else { //if (sizeof(file_info_sf_t) + dir_info_size < packet_data_size) {} else
          const size_t file_info_sf_chunk_size = packet_data_size - dir_info_size;
          memcpy(m_packet.data + dir_info_size, &file_info_sf, file_info_sf_chunk_size);
          dir_info_size += file_info_sf_chunk_size;

          const size_t file_info_sf_rest_size = sizeof(file_info_sf_t) - file_info_sf_chunk_size;
          m_dir_info_buf.resize(file_info_sf.name_size + file_info_sf_rest_size);
          const irs_u8* file_info_sf_pos = reinterpret_cast<irs_u8*>(&file_info_sf);
          file_info_sf_pos += file_info_sf_chunk_size;
          memcpy(&m_dir_info_buf[0], file_info_sf_pos, file_info_sf_rest_size);

          copy(
            file_info.name.begin(),
            file_info.name.end(),
            m_dir_info_buf.begin() + static_cast<ptrdiff_t>(file_info_sf_rest_size)
          );

          // Завершаем цикл, т. к. пакет заполнен
          break;
        } //if (sizeof(file_info_sf_t) + dir_info_size < packet_data_size) {} else
      } else { //if (fs_result == irs::fsr_success) {
        return fsr_error;
      } //if (fs_result == irs::fsr_success) {} else
    } //while (fs_result != irs::fsr_end_of_dir) {
  } //if (m_dir_info_buf.size() >= packet_data_max_size) {} else

#ifdef IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_DIR
  mlog() << "Packet:" << endl;
  c_array_view_t<irs_u8> data_dbgdir(m_packet.data, m_packet.data_size);
  out_data_hex(&data_dbgdir);
  mlog() << endl;
  mlog() << "Text:" << endl;
  for (int i = 0; i < m_packet.data_size; ++i) {
    if (m_packet.data[i] == 0) {
      mlog().put(' ');
    } else {
      mlog().put(static_cast<char>(m_packet.data[i]));
    }
  }
  mlog() << endl;
#endif //IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_DIR

  return fs_result;
}

} // namespace irs
