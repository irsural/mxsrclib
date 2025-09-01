//! \file
//! \ingroup network_in_out_group
//! \brief Клиент протокола Simple FTP

#ifndef SIMPLE_FTP_CLIENTH
#define SIMPLE_FTP_CLIENTH

#include <irsdefs.h>

#include <irsfs.h>
#include <irscpp.h>
#include <hardflowg.h>

#include <irsfinal.h>

#define IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG

#ifdef IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG
//#define IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_STATUS
#define IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_BASE
//#define IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_DETAIL
//#define IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_PROGRESS
#endif //IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG

#ifdef IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_BASE
#define IRS_LIB_SIMP_FTP_CL_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG(msg)
#else //IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_BASE
#define IRS_LIB_SIMP_FTP_CL_DBG_MSG_BASE(msg)
#endif //IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_BASE

#ifdef IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_DETAIL
#define IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL(msg) IRS_LIB_DBG_MSG(msg)
#else //IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_DETAIL
#define IRS_LIB_SIMP_FTP_CL_DBG_MSG_DETAIL(msg)
#endif //IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_DETAIL

#ifdef IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_PROGRESS
#define IRS_LIB_SIMP_FTP_CL_DBG_MSG_PROGRESS(msg) IRS_LIB_DBG_MSG(msg)
#else //IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_PROGRESS
#define IRS_LIB_SIMP_FTP_CL_DBG_MSG_PROGRESS(msg)
#endif //IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_PROGRESS

namespace irs {

class simple_ftp_client_t
{
public:
  explicit simple_ftp_client_t(hardflow_t *ap_hardflow, fs_t* ap_fs);
  ~simple_ftp_client_t();
  irs_u32 server_version() const;
  void start_read();
  void start_read_dir();
  handle_t<dir_iterator_t> get_dir_iterator();
//  void start_read_binary();
  bool is_done() const;
  void start_read_version();
  void path_local(const std::string& a_path);
  void path_remote(const std::string& a_path);
  void tick();

private:
  enum {
    channel = 2,
  };

  enum {
    set_file_path_command = 1,
    // При чтении размера также считывается признак - это размер папки или файла
    read_size_command = 2,
    read_command = 3,
    read_command_response = 4,
    abort_command = 5, // Не используется. Может быть заменена.
    error_command = 6,
    read_version_command = 7,
    file_path_response = 8,
  };

  enum status_t {
    st_start,
    st_start_wait,
    st_read_version_command,
    st_read_version_command_wait,
    st_read_version_command_response,
    st_set_file_path_command,
    st_set_file_path_command_wait,
    st_set_file_path_command_response,
    st_set_file_path_ack_wait,
    st_set_file_path_ack_read,
    // При чтении размера также считывается признак - это размер папки или файла
    st_read_size_command,
    st_read_size_command_wait,
    st_read_size_command_response,
    st_read_command,
    st_read_command_wait,
    st_read_command_response,
    st_read_processing,
    st_send_ack_wait,
    st_show_data,

    st_write_packet,
    st_write_packet_wait,
    st_read_header,
    st_read_header_wait,
    st_read_trash_data_wait,
    st_read_data,
    st_read_data_wait
  };

  #pragma pack(push, 1)
  struct packet_t
  {
    typedef irs_u32 file_size_type;

    enum {
      data_max_size = 100,
      checksum_size = 2,
      ack_error_tag = 0xDE,
    };

    irs_u8 command;
    irs_u8 packet_id;
    irs_u8 data_size;
    irs_u8 header_checksum;
    irs_u8 data_checksum;
    irs_u8 data[data_max_size];

    packet_t():
      command(0),
      packet_id(0),
      data_size(0),
      header_checksum(0),
      data_checksum(0),
      data()
    {
    }
  };
  #pragma pack(pop)

  irs::hardflow_t *mp_hardflow;
  fs_t* mp_fs;
  status_t m_status;
  irs::hardflow::fixed_flow_t m_fixed_flow;
  packet_t m_packet;
  irs_u8 m_packet_id;
  status_t m_oper_return;
  fs_t::file_t* mp_file;
  bool m_is_file_opened;
  irs_u32 m_file_size;
  bool m_is_checksum_error;
  irs_u8 m_packet_id_prev;
  bool m_is_packed_id_prev_exist;
  size_t m_data_offset; // Указатель на текущую позицию в файле
  timer_t m_trash_data_timer;
  irs_u32 m_server_version;
  size_t m_data_offset_prev;
  bool m_start_read;
  #ifdef IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_BASE
  measure_time_t m_measure_time;
  #endif //IRS_LIB_SIMPLE_FTP_CLIENT_DEBUG_BASE
  std::string m_path_local;
  std::string m_path_remote;
  bool m_is_read_version;
  bool m_is_read_dir_to_mem;
  /// @brief Буфер для временного хранения информации о прочитанном содержимом папки
  vector<irs_u8> m_dir_info_buf;
  bool m_is_dir_info_buf_hold;
  bool m_is_dir;

  static void net_to_u32(irs_u8* ap_data, irs_u32* ap_u32);
  static void u32_to_net(irs_u32 a_u32, irs_u8* ap_data);

  void show_status() const;
  bool open_file();
  void close_file();
  bool write_file(const char* ap_data, size_t a_size, bool a_prev_ok = true);
  bool write_file_str(const char* ap_data, bool a_prev_ok = true);
  void dir_info_to_txt_file();
};

} // namespace irs

#endif //SIMPLE_FTP_CLIENTH

