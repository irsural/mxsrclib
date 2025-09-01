//! \file
//! \ingroup network_in_out_group
//! \brief Клиент протокола Simple FTP

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#include <fs_cppbuilder.h>
#endif // __BORLANDC__

#include <irserror.h>
#include <irsalg.h>
#include <irstest.h>

#include <simple_ftp_client.h>

#include <irsfinal.h>

namespace irs {

class dir_iterator_simple_ftp_t: public dir_iterator_t
{
public:
  dir_iterator_simple_ftp_t(const vector<irs_u8>& a_dir_info_buf);
  virtual ~dir_iterator_simple_ftp_t();
  virtual fs_result_t next_dir_item(file_info_t* file_info);
  virtual fs_result_t break_iter();
  virtual fs_result_t next_to_begin();
private:
  /// \brief Структура с информацией о файле или папке
  /// \details sf в названии расшифровывается как Simple FTP
  struct file_info_sf_t
  {
    irs_u8 is_dir;
    irs_u32 size;
    irs_u32 name_size;
    char name[0];

    file_info_sf_t():
      is_dir(0),
      size(0),
      name_size(0)
    {
    }
  };

  vector<irs_u8> m_dir_info_buf;
  size_t m_dir_info_pos;
  fs_result_t m_result;
};

dir_iterator_simple_ftp_t::dir_iterator_simple_ftp_t(const vector<irs_u8>& a_dir_info_buf):
  m_dir_info_buf(a_dir_info_buf),
  m_dir_info_pos(0),
  m_result(fsr_success)
{
}
dir_iterator_simple_ftp_t::~dir_iterator_simple_ftp_t()
{
}
fs_result_t dir_iterator_simple_ftp_t::next_dir_item(file_info_t* file_info)
{
  if (m_result != fsr_success) {
    return m_result;
  }
  if (m_dir_info_buf.size() - m_dir_info_pos > sizeof(file_info_sf_t)) {
    const file_info_sf_t& file_info_sf =
      *reinterpret_cast<const file_info_sf_t*>(&m_dir_info_buf[m_dir_info_pos]);
    if (m_dir_info_buf.size() - m_dir_info_pos - sizeof(file_info_sf_t) >= file_info_sf.name_size) {
      file_info->is_dir = file_info_sf.is_dir ? true : false;
      file_info->size = static_cast<size_t>(file_info_sf.size);
      if (file_info_sf.name_size) {
        file_info->name =
          std::string(file_info_sf.name, file_info_sf.name + file_info_sf.name_size);
        m_dir_info_pos += sizeof(file_info_sf_t) + file_info_sf.name_size;
        return fsr_success;
      } else {
        return fsr_end_of_dir;
      }
    } else {
      return fsr_end_of_dir;
    }
  } else {
    return fsr_end_of_dir;
  }
}
fs_result_t dir_iterator_simple_ftp_t::break_iter()
{
  return fsr_success;
}
fs_result_t dir_iterator_simple_ftp_t::next_to_begin()
{
  m_dir_info_pos = 0;
  return fsr_success;
}

simple_ftp_client_t::simple_ftp_client_t(hardflow_t *ap_hardflow, fs_t* ap_fs):
  mp_hardflow(ap_hardflow),
  mp_fs(ap_fs),
  m_status(st_start_wait),
  m_fixed_flow(mp_hardflow),
  m_packet(),
  m_packet_id(0),
  m_oper_return(st_start),
  mp_file(IRS_NULL),
  m_is_file_opened(false),
  m_file_size(0),
  m_is_checksum_error(false),
  m_packet_id_prev(0),
  m_is_packed_id_prev_exist(false),
  m_data_offset(0), // Указатель на текущую позицию в файле
  m_trash_data_timer(make_cnt_ms(100)),
  m_server_version(0),
  m_data_offset_prev(0),
  m_start_read(false),
  #ifdef IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_BASE
  m_measure_time(),
  #endif //IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_BASE
  m_path_local(),
  m_path_remote(),
  m_is_read_version(false),
  m_is_read_dir_to_mem(false),
  m_dir_info_buf(),
  m_is_dir_info_buf_hold(false),
  m_is_dir(false)
{
}
simple_ftp_client_t::~simple_ftp_client_t()
{
  close_file();
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
      IRS_LIB_SIMPLE_FTP_CLN_SHOW_STATUS(st_start);
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
        // Функция show_status выводится внутри #define IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_STATUS
        // Поэтому здесь используется такой макрос
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
void simple_ftp_client_t::start_read_dir()
{
  m_start_read = true;
  m_is_read_dir_to_mem = true;
}
handle_t<dir_iterator_t> simple_ftp_client_t::get_dir_iterator()
{
  return handle_t<dir_iterator_t>(new dir_iterator_simple_ftp_t(m_dir_info_buf));
}
bool simple_ftp_client_t::is_done() const
{
  return !m_start_read;
}
void simple_ftp_client_t::start_read_version()
{
  m_start_read = true;
  m_is_read_version = true;
}
void simple_ftp_client_t::path_local(const std::string& a_path)
{
  m_path_local = a_path;
}
void simple_ftp_client_t::path_remote(const std::string& a_path)
{
  m_path_remote = a_path;
}
void simple_ftp_client_t::tick()
{
  m_fixed_flow.tick();

  #ifdef IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_STATUS
  show_status();
  #endif

  switch (m_status) {
    case st_start: {
      close_file();
      m_dir_info_buf.clear();
      m_is_read_version = false;
      m_is_read_dir_to_mem = false;
    } break;
    case st_start_wait: {
      if (m_start_read) {
        IRS_LIB_SIMP_FTP_CL_DBG_MSG_BASE("Получить файл Запуск");
        #ifdef IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_BASE
        m_measure_time.start();
        #endif IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_BASE
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
        IRS_LIB_SIMP_FTP_CL_DBG_MSG_BASE("simple_ftp m_server_version = " << m_server_version);
        if (m_is_read_version) {
          m_status = st_start;
        } else {
          m_status = st_set_file_path_command;
        }
      } else {
        m_status = st_read_version_command;
      }
    } break;
    case st_set_file_path_command: {
      u32_to_net(m_path_remote.size(), m_packet.data);
      m_data_offset = 0;
      const size_t remaining_size = m_path_remote.size() - m_data_offset;
      const irs_u8 file_path_chunk_size = static_cast<irs_u8>(std::min<size_t>(
        packet_t::data_max_size - sizeof(irs_u32), remaining_size));
      m_packet.data_size = file_path_chunk_size + sizeof(irs_u32);
      copy(
          m_path_remote.begin() + m_data_offset,
          m_path_remote.begin() + m_data_offset + file_path_chunk_size,
          m_packet.data + sizeof(irs_u32)
      );
      m_data_offset += file_path_chunk_size;
      m_packet_id_prev = m_packet_id;
      IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp m_packed_id_prev = " << (int)m_packet_id_prev);

      m_packet.command = set_file_path_command;
      m_status = st_write_packet;
      m_oper_return = st_set_file_path_command_wait;
    } break;
    case st_set_file_path_command_wait: {
      m_status = st_read_header;
      m_oper_return = st_set_file_path_command_response;
    } break;
    case st_set_file_path_command_response: {
      IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp m_packet_id_prev = " << (int)m_packet_id_prev);
      IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp m_packet.packet_id = " <<
        (int)m_packet.packet_id);
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
        if (m_data_offset >= m_path_remote.size()) {
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
        size_t remaining_size = m_path_remote.size() - m_data_offset;
        m_packet.data_size = static_cast<irs_u8>(std::min<size_t>(
          packet_t::data_max_size, remaining_size));
        copy(
            m_path_remote.begin() + m_data_offset,
            m_path_remote.begin() + m_data_offset + m_packet.data_size,
            m_packet.data
        );
        m_data_offset_prev = m_data_offset;
        m_data_offset += m_packet.data_size;
        m_packet_id_prev = m_packet_id;
        IRS_LIB_SIMP_FTP_CL_DBG_MSG_PROGRESS("simple_ftp Передано " << m_data_offset <<
          " из " << m_path_remote.size());

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
        irs_u8& is_dir = m_packet.data[sizeof(irs_u32)];
        m_is_dir = is_dir ? true : false;
        IRS_LIB_SIMP_FTP_CL_DBG_MSG_BASE("simple_ftp m_file_size = " << m_file_size);
        m_status = st_read_command;
      } else {
        m_status = st_read_size_command;
      }
    } break;
    case st_read_command: {
      if (open_file()) {
        m_packet.command = read_command;
        m_packet.data_size = 0;
        m_status = st_write_packet;
        m_oper_return = st_read_command_wait;
      } else {
        m_status = st_start;
      }
    } break;
    case st_read_command_wait: {
      m_status = st_read_header;
      m_oper_return = st_read_command_response;
    } break;
    case st_read_command_response: {
      if (!m_is_checksum_error && (m_packet.command == read_command_response) &&
        (m_packet.data_size <= packet_t::data_max_size))
      {
        m_status = st_read_processing;
        m_is_packed_id_prev_exist = false;
        m_data_offset = 0;
      } else {
        m_status = st_read_command;
      }
    } break;
    case st_read_processing: {

      #ifdef NOP
      static int read_cnt = 0;
      read_cnt++;
      IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp read_cnt = " << read_cnt);
      if (read_cnt > 10) {
        read_cnt = 0;
        m_status = st_start;
        break;
      }
      #endif //NOP

      if (!m_is_checksum_error && (m_packet.command == read_command_response) &&
        (m_packet.data_size <= packet_t::data_max_size))
      {
        // m_is_packed_id_prev_exist, так просто, нельзя объединить под одним if, т. к.
        // иначе последний else будет некорректен
        if (!m_is_packed_id_prev_exist || (m_packet_id_prev == m_packet.packet_id - 1)) {
          if (m_is_dir) {
            size_t size = m_dir_info_buf.size();
            m_dir_info_buf.resize(size + m_packet.data_size);
            memcpy(&m_dir_info_buf[size], m_packet.data, m_packet.data_size);
          } else {
            mp_fs->write(mp_file, m_packet.data, m_packet.data_size);
          }
          m_data_offset += m_packet.data_size;
          m_packet.command = read_command_response;
          m_packet_id = m_packet.packet_id;
          m_packet_id_prev = m_packet.packet_id;
          m_is_packed_id_prev_exist = true;
          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp st_read_processing ok");
        } else if (!m_is_packed_id_prev_exist || (m_packet_id_prev == m_packet.packet_id)) {
          // Сюда попадаем только если мы приняли ранее пакет успешно, а сервер получил
          // неудачное подтверждение. Это повтор пакета от сервера ранее принятого нами
          // успешно. Поэтому ничего не копируем и отправляем подтверждение успеха.
          m_packet.command = read_command_response;
        } else {
          m_packet.command = error_command;
          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp st_read_processing packet_id error");
        }
      } else {

        #ifdef IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_DETAIL
        if (m_is_checksum_error) {
          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp m_is_checksum_error");
        }
        if (m_packet.command != read_command_response) {
          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL(
            "simple_ftp m_packet.command != read_command_response");
          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL(
            "simple_ftp m_packet.command = " << (int)m_packet.command);
        }
        if (!((m_packet.data_size > 0) && (m_packet.data_size <= packet_t::data_max_size)))
        {
          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp Размер пакета не в допуске: " <<
            (int)m_packet.data_size);
        }
        #endif //IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_DETAIL

        m_packet.command = error_command;
        IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp st_read_processing packet error");
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
        if (m_is_dir) {
          dir_info_to_txt_file();
          m_is_dir_info_buf_hold = false;
        }
      } else {
        m_status = st_read_header;
        m_oper_return = st_read_processing;
      }
      IRS_LIB_SIMP_FTP_CL_DBG_MSG_PROGRESS("simple_ftp Принято " << m_data_offset <<
        " из " << m_file_size);
    } break;
    case st_show_data: {
      #ifdef IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_BASE
      double read_time = m_measure_time.get();
      mlog() << "Время чтения файла, с: " << read_time << endl;
      if (read_time) {
        mlog() << "Скорость чтения файла, байт/с: " << m_file_size/read_time << endl;
      }
      #ifdef __BORLANDC__
      // Здесь используется функция utf8_to_wstring, доступная в mxsrclib только для C++ Builder
      std::wstring fn_wstr = utf8_to_wstring(m_path_local);
      std::string fn_str = irs::convert_str_t<wchar_t, char>(fn_wstr.c_str()).get();
      mlog() << sdatetime << "Файл \"" << fn_str << "\" успешно прочитан" << endl;
      #else //__BORLANDC__
      mlog() << sdatetime << "Файл успешно прочитан" << endl;
      #endif //__BORLANDC__
      #endif //IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_BASE

      // Сообщить о завершении всех операций функции is_done
      m_start_read = false;

      m_status = st_start;
    } break;

    case st_write_packet: {
      m_packet.packet_id = m_packet_id;
      m_packet_id++;

      IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp Отправка");
      IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp m_packet.packet_id = " <<
        (int)m_packet.packet_id);
      IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp m_packet.command = " <<
        (int)m_packet.command);
      IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp m_packet.data_size = " <<
        (int)m_packet.data_size);

      size_t header_size = sizeof(packet_t) - packet_t::data_max_size;
      m_packet.header_checksum = crc8(reinterpret_cast<irs_u8*>(&m_packet), 0,
        header_size - packet_t::checksum_size);
      m_packet.data_checksum = 0;
      if (m_packet.data_size) {
        m_packet.data_checksum = irs::crc16_table(reinterpret_cast<irs_u8*>(&m_packet.data),
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
          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp Ошибка записи пакета");
          m_status = st_start;
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

          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp Прием");
          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp m_packet.packet_id = " <<
            (int)m_packet.packet_id);
          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp m_packet.command = " <<
            (int)m_packet.command);
          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp m_packet.data_size = " <<
            (int)m_packet.data_size);

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
            IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp Ошибка контрольной суммы заголовка");

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
          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp Ошибка чтения заголовка");
          m_status = st_start;
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
          packet_t::data_crc_type checksum_calculated =
            irs::crc16_table(reinterpret_cast<irs_u8*>(&m_packet.data), m_packet.data_size);
          if (m_packet.data_checksum == checksum_calculated) {
            m_status = m_oper_return;
          } else {
            IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp Ошибка контрольной суммы данных");
            m_is_checksum_error = true;
            m_status = m_oper_return;
          }
        } break;
        case irs::hardflow::fixed_flow_t::status_error: {
          IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL("simple_ftp Ошибка чтения данных");
          m_status = st_start;
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

bool simple_ftp_client_t::open_file()
{
  if (!m_is_file_opened) {
    mp_file = mp_fs->open(m_path_local, fm_write);
    m_is_file_opened = (mp_file != IRS_NULL);
  }
  return m_is_file_opened;
}

void simple_ftp_client_t::close_file()
{
  if (m_is_file_opened) {
    fs_result_t fsr = mp_fs->close(mp_file);
    if (fsr == fsr_success) {
      mp_file = IRS_NULL;
      m_is_file_opened = false;
    }
  }
}

bool simple_ftp_client_t::write_file(const char* ap_data, size_t a_size, bool a_prev_ok)
{
  if (a_prev_ok) {
    const irs_u8* data = reinterpret_cast<const irs_u8*>(ap_data);
    fs_result_t fsr = mp_fs->write(mp_file, data, a_size);
    return fsr == fsr_success;
  } else {
    return false;
  }
}

bool simple_ftp_client_t::write_file_str(const char* ap_data, bool a_prev_ok)
{
  return write_file(ap_data, strlen(ap_data), a_prev_ok);
}

void simple_ftp_client_t::dir_info_to_txt_file()
{
  dir_iterator_simple_ftp_t dir_it(m_dir_info_buf);
  file_info_t file_info;
  fs_result_t fs_result = fsr_success;
  while ((fs_result = dir_it.next_dir_item(&file_info)) == fsr_success) {
    std::string dir_or_file_s  = wstring_to_utf8(file_info.is_dir ? L"Папка" : L"Файл");
    const char* dir_or_file = dir_or_file_s.c_str();
    bool is_ok = write_file_str(dir_or_file);
    is_ok = write_file_str(";", is_ok);
    stringstream strm;
    strm << file_info.size << wstring_to_utf8(L" байт");
    is_ok = write_file_str(strm.str().c_str(), is_ok);
    is_ok = write_file_str(";", is_ok);
    is_ok = write_file_str(file_info.name.c_str(), is_ok);
    is_ok = write_file_str("\r\n", is_ok);
    if (!is_ok) {
      IRS_LIB_SIMP_FTP_CL_DBG_MSG_BASE("simple_ftp Error convert dir info to txt");
      return;
    }
  }
}

} // namespace irs

