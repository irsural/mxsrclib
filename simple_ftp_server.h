//! \file
//! \ingroup network_in_out_group
//! \brief Сервер протокола Simple FTP

#ifndef SIMPLE_FTP_SERVERH
#define SIMPLE_FTP_SERVERH

#include <irsdefs.h>

#include <hardflowg.h>
#include <irscpp.h>
#include <irsfs.h>

#include <irsfinal.h>

// #define IRS_LIB_SIMPLE_FTP_SERVER_DEBUG

#ifdef IRS_LIB_SIMPLE_FTP_SERVER_DEBUG
// #define IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_STATUS
#define IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_BASE
// #define IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_DETAIL
#define IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_DIR
#endif //IRS_LIB_SIMPLE_FTP_SERVER_DEBUG

#ifdef IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_BASE
#define IRS_LIB_SIMP_FTP_SR_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG(msg)
#else //IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_BASE
#define IRS_LIB_SIMP_FTP_SR_DBG_MSG_BASE(msg)
#endif //IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_BASE

#ifdef IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_DETAIL
#define IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL(msg) IRS_LIB_DBG_MSG(msg)
#else //IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_DETAIL
#define IRS_LIB_SIMP_FTP_SR_DBG_MSG_DETAIL(msg)
#endif //IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_DETAIL

#ifdef IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_DIR
#define IRS_LIB_SIMP_FTP_SR_DBG_MSG_DIR(msg) IRS_LIB_DBG_MSG(msg)
#else //IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_DIR
#define IRS_LIB_SIMP_FTP_SR_DBG_MSG_DIR(msg)
#endif //IRS_LIB_SIMPLE_FTP_SERVER_DEBUG_DIR

namespace irs {

class simple_ftp_server_t
{
public:
  explicit simple_ftp_server_t(hardflow_t* ap_hardflow, fs_t* ap_fs);
  ~simple_ftp_server_t();
  void show_status() const;
  void tick();

private:
  enum {
    channel = 2,
  };

  enum {
    set_file_path_command = 1,
    read_size_command = 2,
    read_command = 3,
    read_command_response = 4,
    abort_command = 5, // Не используется. Может быть заменена.
    error_command = 6,
    read_version_command = 7,
    file_path_response = 8,
  };

  enum status_t {
    st_start_wait,
    st_command_processing,
    st_set_file_path_ack_wait,
    st_set_file_path_processing,
    st_show_file_path,
    st_read_processing,
    st_read_ack,
    st_check_ack,
    st_get_dir_size,

    st_write_packet,
    st_write_packet_wait,
    st_read_header,
    st_read_header_wait,
    st_read_trash_data_wait,
    st_read_data,
    st_read_data_wait,
  };

#pragma pack(push, 1)

  struct packet_t
  {
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

#pragma pack(pop)

  static void net_to_u32(irs_u8* ap_data, irs_u32* ap_u32);
  static void u32_to_net(irs_u32 a_u32, irs_u8* ap_data);

  static const size_t m_dir_info_buf_reserve = 20;

  irs_u32 m_server_version;
  hardflow_t* mp_hardflow;
  fs_t* mp_fs;
  status_t m_status;
  hardflow::fixed_flow_t m_fixed_flow;
  packet_t m_packet;
  irs_u8 m_packet_id;
  irs_u8 m_packet_id_prev;
  status_t m_oper_return;
  bool m_is_read_time_inf;
  fs_t::file_t* mp_file;
  bool m_is_file_opened;
  irs_u32 m_file_size;
  //// @brief Указатель на текущую позицию в файле
  size_t m_data_offset;
  irs::handle_t<irs::dir_iterator_t> mp_dir_iterator;
  irs_u8 m_check_ack_packet_id;
  bool m_is_checksum_error;
  timer_t m_trash_data_timer;
  bool m_is_file;
  bool m_is_dir;
  /// @brief Буфер для хранения остатка информации о содержимом папки
  vector<irs_u8> m_dir_info_buf;
  fs_result_t m_fs_result;
  irs_string_t m_file_path;
  irs_u32 m_file_path_size;

  bool open_file();
  void close_file();
  bool get_file_size(irs_u32* ap_file_size) const;
  void error_response();
  void file_size_response();
  fs_result_t dir_info_to_packet();
};

} // namespace irs

#endif //SIMPLE_FTP_SERVERH
