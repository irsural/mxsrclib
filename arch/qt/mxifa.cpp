// ��������� ���������� ��� ������� ������ (�����������)
// ����: 12.05.2010
// ������ ����: 01.10.2007

#include <irsdefs.h>
#include <mxifa.h>

// ������������� mxifa
void mxifa_init()
{
}
// ��������������� mxifa
void mxifa_deinit()
{
}
// ������������ ��������
void mxifa_tick()
{
}
// ����������� �������� channel
void *mxifa_open(mxifa_ch_t channel, irs_bool is_broadcast)
{
  return IRS_NULL;
}
// ������������� �������� channel
void *mxifa_open_begin(mxifa_ch_t channel, irs_bool is_broadcast)
{
  return IRS_NULL;
}
// �������� ��������� �������� channel
irs_bool mxifa_open_end(void *pchdata, bool abort)
{
  return irs_false;
}
// ����������� �������� channel c ����������������
void *mxifa_open_ex(mxifa_ch_t channel, void *config, irs_bool is_broadcast)
{
  return irs_false;
}
// ������������� �������� channel � �����������������
void *mxifa_open_begin_ex(mxifa_ch_t channel, void *config,
  irs_bool is_broadcast)
{
  return IRS_NULL;
}
// �������� ��������� �������� channel � ����������������
irs_bool mxifa_open_end_ex(void *pchdata, bool abort)
{
  return irs_false;
}
// ����������� �������� channel
irs_bool mxifa_close(void *pchdata)
{
  return irs_false;
}
// ������������� �������� channel
irs_bool mxifa_close_begin(void *pchdata)
{
  return irs_false;
}
// �������� ��������� �������� channel
irs_bool mxifa_close_end(void *pchdata, bool abort)
{
  return irs_false;
}
// ������ ������ � ��������� ��� ��������
irs_bool mxifa_write_begin(void *pchdata, mxifa_dest_t *dest,
  const irs_u8 *buf, mxifa_sz_t size)
{
  return irs_false;
}
// �������� ��������� ������
irs_bool mxifa_write_end(void *pchdata, irs_bool abort)
{
  return irs_false;
}
// ������ �������� ������ �� ���������� � �����
irs_bool mxifa_read_begin(void *pchdata, mxifa_dest_t *dest,
  irs_u8 *buf, mxifa_sz_t size)
{
  return irs_false;
}
// �������� ��������� ������
irs_bool mxifa_read_end(void *pchdata, irs_bool abort)
{
  return irs_false;
}
// ������ �������� ������ ����������
mxifa_sz_t mxifa_fast_read(void *pchdata, mxifa_dest_t *dest, irs_u8 *buf,
  mxifa_sz_t size)
{
  return 0;
}
// ������ ���� ������ �� ������
mxifa_ei_t mxifa_get_channel_type_ex(mxifa_ch_t channel)
{
  return mxifa_ei_unknown;
}
// ������ ���� ������
mxifa_ei_t mxifa_get_channel_type(void *pchdata)
{
  return mxifa_ei_unknown;
}
// ����������� ������ ������ �� �����������
static mxifa_ch_t mxifa_get_channel_num(void *pchdata)
{
  return 0;
}
// ��������� ������������ ������
void mxifa_set_config(void *pchdata, void *config)
{
}
// ������ ������������ ������
void mxifa_get_config(void *pchdata, void *config)
{
}
