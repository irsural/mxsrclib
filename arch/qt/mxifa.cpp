// Абстакция интерфейса для каналов обмена (интерфейсов)
// Дата: 12.05.2010
// Ранняя дата: 01.10.2007

#include <irsdefs.h>
#include <mxifa.h>

// Инициализация mxifa
void mxifa_init()
{
}
// Деинициализация mxifa
void mxifa_deinit()
{
}
// Элементарное действие
void mxifa_tick()
{
}
// Блокирующее открытие channel
void *mxifa_open(mxifa_ch_t channel, irs_bool is_broadcast)
{
  return IRS_NULL;
}
// Неблокирующее открытие channel
void *mxifa_open_begin(mxifa_ch_t channel, irs_bool is_broadcast)
{
  return IRS_NULL;
}
// Проверка окончания открытия channel
irs_bool mxifa_open_end(void *pchdata, bool abort)
{
  return irs_false;
}
// Блокирующее открытие channel c кофигурированием
void *mxifa_open_ex(mxifa_ch_t channel, void *config, irs_bool is_broadcast)
{
  return irs_false;
}
// Неблокирующее открытие channel с конфигурированием
void *mxifa_open_begin_ex(mxifa_ch_t channel, void *config,
  irs_bool is_broadcast)
{
  return IRS_NULL;
}
// Проверка окончания открытия channel с кофигурированием
irs_bool mxifa_open_end_ex(void *pchdata, bool abort)
{
  return irs_false;
}
// Блокирующее закрытие channel
irs_bool mxifa_close(void *pchdata)
{
  return irs_false;
}
// Неблокирующее закрытие channel
irs_bool mxifa_close_begin(void *pchdata)
{
  return irs_false;
}
// Проверка окончания закрытия channel
irs_bool mxifa_close_end(void *pchdata, bool abort)
{
  return irs_false;
}
// Запись буфера в интерфейс для передачи
irs_bool mxifa_write_begin(void *pchdata, mxifa_dest_t *dest,
  const irs_u8 *buf, mxifa_sz_t size)
{
  return irs_false;
}
// Проверка окончания записи
irs_bool mxifa_write_end(void *pchdata, irs_bool abort)
{
  return irs_false;
}
// Чтение принятых данных из интерфейса в буфер
irs_bool mxifa_read_begin(void *pchdata, mxifa_dest_t *dest,
  irs_u8 *buf, mxifa_sz_t size)
{
  return irs_false;
}
// Проверка окончания чтения
irs_bool mxifa_read_end(void *pchdata, irs_bool abort)
{
  return irs_false;
}
// Чтение принятых данных немедленно
mxifa_sz_t mxifa_fast_read(void *pchdata, mxifa_dest_t *dest, irs_u8 *buf,
  mxifa_sz_t size)
{
  return 0;
}
// Чтение типа канала по номеру
mxifa_ei_t mxifa_get_channel_type_ex(mxifa_ch_t channel)
{
  return mxifa_ei_unknown;
}
// Чтение типа канала
mxifa_ei_t mxifa_get_channel_type(void *pchdata)
{
  return mxifa_ei_unknown;
}
// Определение номера канала по дескриптору
static mxifa_ch_t mxifa_get_channel_num(void *pchdata)
{
  return 0;
}
// Установка конфигурации канала
void mxifa_set_config(void *pchdata, void *config)
{
}
// Чтение конфигурации канала
void mxifa_get_config(void *pchdata, void *config)
{
}
