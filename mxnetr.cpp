// Общие утилиты mxnet
// Дата: 18.02.2009

#include <mxnetr.h>
#include <irsalg.h>
//#include <irsavrutil.h>

// Поиск начала в пакете
irs_bool find_begin_in_data(irs_u8 *buf, mxn_sz_t size, mxn_sz_t &pos);

// Идентификатор начала пакета
const irs_i32 beg_pack_array32[] = {
  MXN_CONST_IDENT_BEG_PACK_FIRST,
  MXN_CONST_IDENT_BEG_PACK_SECOND
};
const irs_u8 *const beg_pack_array = (irs_u8 *)(void *)beg_pack_array32;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Поиск начала пакета

irs::mx_beg_pack_proc_t::mx_beg_pack_proc_t(void *a_handle_channel):
  m_mode(mode_start),
  m_handle_channel(a_handle_channel),
  m_buf(IRS_NULL),
  m_buf_end(IRS_NULL),
  m_abort_request(irs_false),
  m_proc(irs_true),
  m_dest(0)
{
}
//---------------------------------------------------------------------------
irs::mx_beg_pack_proc_t::~mx_beg_pack_proc_t()
{
}
//---------------------------------------------------------------------------
void irs::mx_beg_pack_proc_t::operator()(irs_u8 *a_buf, mxifa_dest_t *a_dest)
{
  m_buf = a_buf;
  m_buf_end = m_buf + SIZE_OF_BEG_PACK;
  m_proc = irs_true;
  m_mode = mode_start;
  m_abort_request = irs_false;
  m_dest = a_dest;
}
//---------------------------------------------------------------------------
irs_bool irs::mx_beg_pack_proc_t::tick()
{
  switch (m_mode) {
    case mode_start: {
      mxifa_read_begin(m_handle_channel, m_dest, m_buf, SIZE_OF_BEG_PACK);
      m_mode = mode_read_begin;
    } break;
    case mode_read_begin: {
      if (mxifa_read_end(m_handle_channel, irs_false)) {
        mxifa_read_begin(m_handle_channel, m_dest, m_buf_end, mxn_end_size);
        m_mode = mode_read_end;
      } else if (m_abort_request) {
        mxifa_read_end(m_handle_channel, irs_true);
        m_mode = mode_stop;
      }
    } break;
    case mode_read_end: {
      if (mxifa_read_end(m_handle_channel, irs_false)) {
        mxn_sz_t pos = 0;
        if (find_begin_in_data(m_buf, mxn_header_size, pos)) {
          if (pos) {
            void *src = (void *)(m_buf + pos);
            size_t move_size = mxn_header_size - pos;
            void *dest = (void *)m_buf;
            memmove(dest, src, move_size);

            irs_u8 *buf = m_buf + move_size;
            //mxn_sz_t size = mxn_header_size - move_size;
            mxn_sz_t chunk_size = pos;
            mxifa_read_begin(m_handle_channel, m_dest, buf, chunk_size);

            m_mode = mode_read_chunk;
          } else {
            m_mode = mode_stop;
          }
        } else {
          void *src = (void *)(m_buf + mxn_end_size);
          size_t move_size = SIZE_OF_BEG_PACK;
          void *dest = (void *)m_buf;
          memmove(dest, src, move_size);

          mxifa_read_begin(m_handle_channel, m_dest, m_buf_end, mxn_end_size);
        }
      } else if (m_abort_request) {
        mxifa_read_end(m_handle_channel, irs_true);
        m_mode = mode_stop;
      }
    } break;
    case mode_read_chunk: {
      if (mxifa_read_end(m_handle_channel, irs_false)) {
        m_mode = mode_stop;
      } else if (m_abort_request) {
        mxifa_read_end(m_handle_channel, irs_true);
        m_mode = mode_stop;
      }
    } break;
    case mode_stop: {
      m_proc = irs_false;
      m_abort_request = irs_false;
    } break;
  }

  return m_proc;
}
//---------------------------------------------------------------------------
void irs::mx_beg_pack_proc_t::abort()
{
  m_abort_request = irs_true;
}
//---------------------------------------------------------------------------
// Поиск начала в пакете
irs_bool find_begin_in_data(irs_u8 *buf, mxn_sz_t size, mxn_sz_t &pos)
{
  bool first_byte_compare = true;
  mxn_sz_t buf_ind = 0;
  mxn_sz_t buf_ind_next = 0;
  mxn_sz_t bpa_ind = 0;
  pos = (mxn_sz_t)-1;
  while (buf_ind < size) {
    if (buf[buf_ind] == beg_pack_array[bpa_ind]) {
      if (first_byte_compare) {
        first_byte_compare = false;
        buf_ind_next = buf_ind;
      }
      if (bpa_ind >= SIZE_OF_BEG_PACK - 1) {
        pos = buf_ind - SIZE_OF_BEG_PACK + 1;
        return irs_true;
      }
      bpa_ind++;
    } else {
      if (!first_byte_compare) {
        first_byte_compare = true;
        buf_ind = buf_ind_next;
      }
      bpa_ind = 0;
    }
    buf_ind++;
  }
  return irs_false;
}

namespace {

irs_i32 direct_sum(irs_i32 *vars, mxn_cnt_t count)
{
  irs_i32 sum = 0;
  for (mxn_cnt_t i = 0; i < count; i++) sum += vars[i];
  return sum;
}

} //namespace

// Контрольная сумма для mxnet
irs_i32 irs::checksum_calc(irs::mxn_checksum_t cs_type,
  mxn_packet_t *packet, mxn_cnt_t var_count)
{
  switch(cs_type) {
    case mxncs_reduced_direct_sum: {
      irs_i32 *reduced_packet = reinterpret_cast<irs_i32*>(&packet->code_comm);
      mxn_cnt_t reduced_count =
        mxn_count_of_header - mxn_count_of_beg_pack + var_count;
      return direct_sum(reduced_packet, reduced_count);
    }
    case mxncs_full_direct_sum: {
      irs_i32 *full_packet = reinterpret_cast<irs_i32*>(packet);
      mxn_cnt_t full_count = mxn_count_of_header + var_count;
      return direct_sum(full_packet, full_count);
    }
    case mxncs_crc32: {
      irs_u32 *full_packet = reinterpret_cast<irs_u32*>(packet);
      irs_uarc full_count = mxn_count_of_header + var_count;
      const irs_uarc first_var = 0;
      irs_u32 cs = irs::crc32(full_packet, first_var, full_count);
      irs_i32 cs_ret = static_cast<irs_i32>(cs);
      return cs_ret;
    }
    case mxncs_crc8: {
      irs_u8 *full_packet = reinterpret_cast<irs_u8*>(packet);
      irs_u8 full_count =
        irs_u8((mxn_count_of_header + var_count)*sizeof(irs_i32));
      irs_u8 cs = irs::crc8(full_packet, 0, full_count);
      irs_i32 cs_ret = static_cast<irs_i32>(cs);
      return cs_ret;
    }
  }
  return 0xDEADBEEF;
}
