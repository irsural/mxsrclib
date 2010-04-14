// Класс для расширения количества переменнных в mxnet "Воронка"
// Дата 13.00.2010

#ifndef irsfunnelH
#define irsfunnelH

#include <irsdefs.h>

#include <irsstd.h>
#include <timer.h>

#include <irsfinal.h>

namespace irs
{
enum fannel_client_mode_t {fcm_full_size, fcm_reduce};
class funnel_client_t : public mxdata_t
{
  enum status_t
  {
    FREE,
    WRITE,
    WRITE_WAIT,
    WRITE_PROTECT,
    READ
  };
  fannel_client_mode_t m_fannel_client_mode;
  status_t m_status;
  mxdata_t *mp_data;
  irs_bool m_is_connected;
  irs_bool m_indexes_are_created;
  irs_u8 *mp_buf;
  irs_uarc m_size;
  irs_uarc m_area_index;
  irs_uarc m_area_size;
  irs_uarc m_current_index;
  vector<bool> m_write_vector;
  irs_uarc m_start_index;
  counter_t m_read_interval;
  counter_t m_read_counter;
  conn_data_t<irs_i32> m_size_index;
  conn_data_t<irs_i32> m_read_index;
  conn_data_t<irs_i32> m_read_value;
  conn_data_t<irs_i32> m_write_index;
  conn_data_t<irs_i32> m_write_value;
  timer_t m_protect_timer;
  bool m_is_buf_created;
  bool m_on_read_complete;
  bool m_on_reset_stat_read_complete;
  irs_iarc m_count_read_byte;
  irs_uarc m_read_start_index;
  irs_uarc m_need_write_byte;
public:
  funnel_client_t(
    mxdata_t *ap_data,
    irs_uarc a_start_index,
    counter_t a_read_interval,
    const irs_uarc a_area_index = 0,
    const irs_uarc a_area_size = 0,
    const fannel_client_mode_t a_fannel_client_mode = fcm_full_size);
  ~funnel_client_t();
  irs_uarc size();
  irs_bool connected();
  void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  irs_i32 get_write_size();
  bool read_complete();
  void reset_stat_read_complete();
  irs_i32 get_read_size();
  void tick();
};

class funnel_server_t
{
  mxdata_t *mp_data;
  mxdata_t *mp_trans_data;
  irs_uarc m_size;
  irs_i32 m_current_index;
  irs_i32 m_current_value;
  irs_bool m_is_connected;
  irs_uarc m_start_index;
  irs_bool m_prev_operation_read;
  irs_uarc m_data_shift;
  conn_data_t<irs_i32> m_size_index;
  conn_data_t<irs_i32> m_write_index;
  conn_data_t<irs_i32> m_write_value;
  conn_data_t<irs_i32> m_read_index;
  conn_data_t<irs_i32> m_read_value;
public:
  funnel_server_t(mxdata_t *ap_data, irs_uarc a_data_shift,
  mxdata_t *ap_trans_data, irs_uarc a_size, irs_uarc a_start_index);
  irs_uarc size();
  irs_bool connected();
  ~funnel_server_t();
  void tick();
};

} //  irs

#endif // irsfunnelH
