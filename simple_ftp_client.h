//! \file
//! \ingroup network_in_out_group
//! \brief  лиент протокола Simple FTP

#ifndef SIMPLE_FTP_CLIENTH
#define SIMPLE_FTP_CLIENTH

#include <irsdefs.h>

#include <irsfs.h>
#include <irscpp.h>
#include <hardflowg.h>

#include <irsfinal.h>

namespace irs {

class simple_ftp_client_t
{
public:
  explicit simple_ftp_client_t(hardflow_t *ap_hardflow, fs_t* ap_fs);
  void show_status() const;
  irs_u32 server_version() const;
  void start_read();
  bool is_done() const;
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
    abort_command = 5,
    error_command = 6,
    read_version_command = 7,
    file_path_response = 8,
  };

  enum status_t {
    st_start_wait,
    st_read_version_command,
    st_read_version_command_wait,
    st_read_version_command_response,
    st_set_file_path_command,
    st_set_file_path_command_wait,
    st_set_file_path_command_response,
    st_set_file_path_ack_wait,
    st_set_file_path_ack_read,
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
      packet_data_max_size = 100,
      checksum_size = 2,
      ack_error_tag = 0xDE,
    };

    irs_u8 command;
    irs_u8 packet_id;
    irs_u8 data_size;
    irs_u8 header_checksum;
    irs_u8 data_checksum;
    irs_u8 data[packet_data_max_size];

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
  irs_u32 m_file_size;
  bool m_is_checksum_error;
  irs_u8 m_packet_id_prev;
  bool m_is_packed_id_prev_exist;
  size_t m_data_offset; // ”казатель на текущую позицию в файле
  vector<irs_u8> m_file;
  timer_t m_trash_data_timer;
  irs_u32 m_server_version;
  irs_string_t m_file_path;
  size_t m_data_offset_prev;
  bool m_start_read;

  static void net_to_u32(irs_u8* ap_data, irs_u32* ap_u32);
  static void u32_to_net(irs_u32 a_u32, irs_u8* ap_data);
};

} // namespace irs

#endif //SIMPLE_FTP_CLIENTH

