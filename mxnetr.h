// Общие утилиты mxnet
// Дата: 13.04.2010

#ifndef MXNETRH
#define MXNETRH

#include <irsdefs.h>

#include <mxifa.h>
#include <mxnetd.h>
#include <mxnetda.h>

#include <irsfinal.h>

namespace irs {
  
// Поиск начала пакета
class mx_beg_pack_proc_t: public mx_proc_t
{
  typedef enum _mode_t {
    mode_start,
    mode_read_begin,
    mode_read_end,
    mode_read_chunk,
    mode_stop
  } mode_t;

  mode_t m_mode;
  void *m_handle_channel;
  irs_u8 *m_buf;
  irs_u8 *m_buf_end;
  irs_bool m_abort_request;
  irs_bool m_proc;
  mxifa_dest_t *m_dest;
public:
  mx_beg_pack_proc_t(void *a_handle_channel);
  virtual ~mx_beg_pack_proc_t();
  void operator()(irs_u8 *a_buf, mxifa_dest_t *a_dest = 0);
  virtual irs_bool tick();
  virtual void abort();
};

// Поиск начала пакета через hardflow
class mx_beg_pack_proc_fix_flow_t
{
public:
  typedef hardflow_t::size_type channel_t;
    
  mx_beg_pack_proc_fix_flow_t(hardflow::fixed_flow_t& a_fixed_flow);
  ~mx_beg_pack_proc_fix_flow_t();
  void start(irs_u8* ap_buf, channel_t a_channel);
  void abort();
  bool busy();
  void tick();
private:
  enum status_t 
  {
    WAIT,
    START,
    READ_BEGIN,
    READ_END,
    READ_CHUNK,
    STOP
  };
  enum
  {
    invalid_channel = irs::hardflow_t::invalid_channel
  };

  status_t m_status;
  hardflow::fixed_flow_t& m_fixed_flow;
  irs_u8* mp_buf;
  irs_u8* mp_buf_end;
  bool m_abort_request;
  bool m_busy;
  channel_t m_channel;
};


// Размер заголовка в 4-байтных перменных
const mxn_cnt_t mxn_count_of_header = MXN_SIZE_OF_HEADER;
// Размер начала пакета в 4-байтных переменных
const mxn_cnt_t mxn_count_of_beg_pack = MXN_SIZE_OF_BEG_PACK/sizeof(irs_i32);
// Размер контрольной суммы в 4-байтных переменных
const mxn_cnt_t mxn_count_of_checksum = 1;
// Тип контрольной суммы
enum mxn_checksum_t {
  mxncs_reduced_direct_sum,   // прямая сумма без начала пакета
  mxncs_full_direct_sum,      // прямая сумма вместе с началом пакета
  mxncs_crc32,                // CRC32
  mxncs_crc8                  // CRC8
};
// Контрольная сумма для mxnet
irs_i32 checksum_calc(mxn_checksum_t cs_type, mxn_packet_t *packet, 
  mxn_cnt_t var_count);

} //namespace irs

#endif //MXNETRH
