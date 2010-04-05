// Общие утилиты mxnet
// Дата: 16.04.2008

#ifndef MXNETRH
#define MXNETRH

#include <irsdefs.h>
#include <mxifa.h>
#include <mxnetd.h>
#include <mxnetda.h>

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
