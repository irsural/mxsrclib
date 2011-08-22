//! \file
//! \ingroup network_in_out_group
//! \brief Клиент и сервер modbus
//!
//! Дата: 18.08.2010\n
//! Ранняя дата: 16.09.2008

#ifndef irsmbusH
#define irsmbusH

#include <irsdefs.h>

#include <mxifa.h>
#include <irsstd.h>
#include <irscpp.h>
#include <mxdata.h>

#include <irsfinal.h>

#ifdef IRS_LIB_IRSMBUS_DEBUG_TYPE
# if (IRS_LIB_IRSMBUS_DEBUG_TYPE == IRS_LIB_DEBUG_BASE)
#   define IRS_LIB_IRSMBUS_DBG_OPERATION_TIME(msg) msg
#   define IRS_LIB_IRSMBUS_DBG_MONITOR(msg)
#   define IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(msg)
#   define IRS_LIB_IRSMBUS_DBG_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg << endl)
#   define IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(msg)
# elif (IRS_LIB_IRSMBUS_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
#   define IRS_LIB_IRSMBUS_DBG_OPERATION_TIME(msg) msg
#   define IRS_LIB_IRSMBUS_DBG_MONITOR(msg) msg
#   define IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(msg)\
      IRS_LIB_DBG_RAW_MSG(msg << endl) 
#   define IRS_LIB_IRSMBUS_DBG_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg << endl) 
#   define IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(msg) msg
# endif
#else // IRS_LIB_IRSMBUS_DEBUG_TYPE
# define IRS_LIB_IRSMBUS_DEBUG_TYPE IRS_LIB_DEBUG_NONE
# define IRS_LIB_IRSMBUS_DBG_OPERATION_TIME(msg)
# define IRS_LIB_IRSMBUS_DBG_MONITOR(msg)
# define IRS_LIB_IRSMBUS_DBG_RAW_MSG_DETAIL(msg) 
# define IRS_LIB_IRSMBUS_DBG_MSG_BASE(msg) 
# define IRS_LIB_IRSMBUS_DBG_RAW_MSG_BLOCK_DETAIL(msg)
#endif // IRS_LIB_IRSMBUS_DEBUG_TYPE

namespace irs {

//! \addtogroup network_in_out_group
//! @{

#pragma pack(push, 1)

struct MBAP_header_t {
  irs_u16       transaction_id;
  irs_u16       proto_id;
  irs_u16       length;
  irs_u8        unit_identifier;
};
struct request_t {
  irs_u8        function_code;
  irs_u16       starting_address;
  irs_u16       quantity;
};
struct request_multiple_write_byte_t {
  irs_u8        function_code;
  irs_u16       starting_address;
  irs_u16       quantity;
  irs_u8        byte_count;
  irs_u8        value[1];
};
struct request_multiple_write_regs_t {
  irs_u8        function_code;
  irs_u16       starting_address;
  irs_u16       quantity;
  irs_u8        byte_count;
  irs_u16       value[1];
};
struct request_exception_t {
  irs_u8        function_code;
};
struct response_read_byte_t {
  irs_u8        function_code;
  irs_u8        byte_count;
  irs_u8        value[1];
};
struct response_read_reg_t {
  irs_u8        function_code;
  irs_u8        byte_count;
  irs_u16       value[1];
};
struct response_single_write_t {
  irs_u8        function_code;
  irs_u16       address;
  irs_u16       value;
};
struct response_exception_t {
  irs_u8        error_code;
  irs_u8        exeption_code;
};

#pragma pack(pop)

void test_bit_copy(ostream& strm, size_t size_data_in, size_t size_data_out, 
  size_t index_data_in, size_t index_data_out, size_t size_data);  

//! \brief Определение границ области пересечения двух областей в составе
//!   одного массива.
//!
//! \param[in] a_index – индекс начала контрольной области;
//! \param[in] a_size – размер контрольной области;
//! \param[in] a_start_range – индекс на начало проверяемой области;
//! \param[in] a_end_range – индекс на конец проверяемой области;
//! \param[out] a_num – размер совпадающей области;
//! \param[out] a_start – индекс на начало совпадающей области.
void range(size_t a_index, size_t a_size, size_t a_start_range, 
  size_t a_end_range, size_t *a_num, size_t *a_start);

//! \brief Сервер протокола Modbus
//! \author Sergeev Sergey
class modbus_server_t : public mxdata_ext_t
{
public:
  typedef hardflow_t::size_type channel_type;
  //! \brief Конструктор
  //!
  //! \param[in] ap_hardflow – указатель на используемый коммуникационный
  //!   протокол;
  //! \param[in] a_discr_inputs_size – размер discret inputs в байтах 0 .. 8192;
  //! \param[in] a_coils_size – размер coils в байтах 0 .. 8192;
  //! \param[in] a_hold_regs_reg – размер holding registers в регистрах
  //!   0 .. 65536;
  //! \param[in] a_input_regs_reg – размер input registers в регистрах
  //!   0 ..65536;
  //! \param[in] double a_disconnect_time_sec – время, через которое сервер
  //!   реагирует на обрыв связи в случае сбоев при попытках совершить обмен
  //!   данными.
  modbus_server_t(
    hardflow_t* ap_hardflow,
    size_t a_discr_inputs_size_byte = 8,
    size_t a_coils_size_byte = 8,
    size_t a_hold_regs_reg = 7,
    size_t a_input_regs_reg = 7,
    counter_t a_disconnect_time = make_cnt_s(2)
  );
  virtual ~modbus_server_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void mark_to_send(irs_uarc a_index, irs_uarc a_size);
  virtual void mark_to_recieve(irs_uarc a_index, irs_uarc a_size);
  virtual void mark_to_send_bit(irs_uarc a_byte_index, irs_uarc a_bit_index);
  virtual void mark_to_recieve_bit(irs_uarc a_byte_index, irs_uarc a_bit_index);
  virtual void update();
  virtual status_t status() const;
  virtual void set_refresh_mode(mode_refresh_t a_refresh_mode);
  virtual void tick();
  virtual void abort();
  
private:
  enum {
    size_of_MBAP = 7,
    size_of_read_header = 4,
    size_of_resp_header = 2,
    size_of_packet = 260,
    size_of_req_excep = 3,
    size_of_req_multi_write = 6,
    size_of_resp_multi_write = 6,
    read_coils = 1,
    read_discrete_inputs = 2,
    read_hold_registers = 3,
    read_input_registers = 4,
    write_single_coil = 5,
    write_single_register = 6,
    read_exception_status = 7,
    write_multiple_coils = 15,
    write_multiple_registers = 16
  };
  //! \brief Режим работы сервера.
  enum condition_t {
    read_header_mode,   //!< \brief Режим чтения заголовка пакета.
    read_request_mode,  //!< \brief Режим приема запроса от клиента.
    send_response_mode, //!< \brief Режим отправки ответа клиенту.
    read_end_mode,      //!< \brief Режим завершения чтения запроса от клиента.
    write_end           //!< \brief Режим завершения отправки ответа клиенту.
  };
  //! \brief Буфер для приема и отправки данных с помощью коммуникационного
  //!   потока fixed_flow.
  raw_data_t<irs_u8>                    mp_buf;
  //! \brief Длина пакета без заголовка.
  size_t                                m_size_byte_end;
  //! \brief Размер discret_inputs в битах.
  size_t                                m_discret_inputs_size_bit;
  //! \brief Размер discret_inputs в битах.
  size_t                                m_coils_size_bit;
  //! \brief Размер holding_registers в регистрах.
  size_t                                m_hold_registers_size_reg;
  //! \brief размер input_registers в регистрах.
  size_t                                m_input_registers_size_reg;
  //! \brief Суммарный размер 4 массивов: discret_inputs, coils,
  //!   holding_registers, input_registers.
  size_t                                m_size_byte;
  //! \brief Конец участка discret_inputs, входящего в запрашиваемый диапазон
  //!   при выполнении операций.
  size_t                                m_discret_inputs_end_byte;
  //! \brief Конец участка coils, входящего в запрашиваемый диапазон при
  //!   выполнении операций.
  size_t                                m_coils_end_byte;
  //! \brief Конец участка holding_registers, входящего в запрашиваемый
  //!   диапазон при выполнении операций.
  size_t                                m_hold_registers_end_byte;
  //! \brief Конец участка input_register, входящего в запрашиваемый диапазон
  //!   при выполнении операций.
  size_t                                m_input_registers_end_byte;
  //! \brief Режим работы сервера
  condition_t                           m_mode;
  //! \brief Байтовый массив discret_inputs, используемый в операциях чтения и
  //!   записи.
  raw_data_t<irs_u8>                    m_discr_inputs_byte;
  //! \brief Байтовый массив coils, используемый в операциях чтения и записи.
  raw_data_t<irs_u8>                    m_coils_byte;
  //! \brief Регистровый массив input_registers, используемый в
  //!   операциях чтения и записи.
  raw_data_t<irs_u16>                   m_input_regs_reg;
  //! \brief Регистровый массив holding_registers, используемый в операциях
  //!   чтения и записи.
  raw_data_t<irs_u16>                   m_hold_regs_reg;
  //! \brief Размер в байтах discret_inputs, входящих в запрашиваемый диапазон
  //!   при выполнении операций.
  size_t                                m_discret_inputs_size_byte;
  //! \brief Начало области массива discret_inputs, входящей в запрашиваемый
  //!   диапазон.
  size_t                                m_discret_inputs_start_byte;
  //! \brief Размер в байтах coils, входящих в запрашиваемый диапазон при
  //!   выполнении операций.
  size_t                                m_coils_size_byte;
  //! \brief Начало области массива coils, входящей в запрашиваемый диапазон.
  size_t                                m_coils_start_byte;
  //! \brief Размер в байтах holding_registers, входящих в запрашиваемый
  //! диапазон при выполнении операций.
  size_t                                m_hold_registers_size_byte;
  //! \brief Начало области массива holding_registers, входящей в запрашиваемый
  //!   диапазон.
  size_t                                m_hold_registers_start_byte;
  //! \brief Размер в байтах input_registers, входящих в запрашиваемый диапазон
  //!   при выполнении операций.
  size_t                                m_input_registers_size_byte;
  //! \brief Начало области массива input_registers, входящей в запрашиваемый
  //!   диапазон.
  size_t                                m_input_registers_start_byte;
  //! \brief Номер канала, отведенного для общения с клиентом.
  channel_type                          m_channel;
  //! \brief Коммуникационный протокол, на базе которого работает сервер.
  hardflow_t*                           mp_hardflow_server;
  //! \brief Вспомогательный класс, предназначенный для чтения/записи
  //!   известного(фиксированного) объема данных.
  hardflow::fixed_flow_t                m_fixed_flow;
  //! \brief Статус операции update().
  status_t                              m_operation_status;
  static char const IRS_CSTR_NONVOLATILE       m_read_header_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_read_request_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_send_response_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_read_end_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_write_end[];
  static IRS_CSTR_NONVOLATILE char const IRS_CSTR_NONVOLATILE* const
    m_ident_name_list[];
  
  void error_response(irs_u8 error_code);
  void modbus_pack_request_monitor(irs_u8 *ap_buf);
  void modbus_pack_response_monitor(irs_u8 *ap_buf);
  void view_mode();
};


//! \brief Клиент протокола Modbus
//! \author Sergeev Sergey
class modbus_client_t : public mxdata_ext_t
{
public:
  typedef hardflow_t::size_type channel_type;
  //! \brief Конструктор
  //!
  //! \param[in] ap_hardflow – указатель на используемый коммуникационный
  //!   протокол
  //! \param[in] a_refresh_mode – режим работы клиента: mode_refresh_auto или
  //!   mode_refresh_manual
  //! \param[in] a_discr_inputs_size_byte – количество discret inputs в
  //!   байтах 0 .. 8192
  //! \param[in] a_coils_size_byte – размер coils в байтах 0 .. 8192
  //! \param[in] a_hold_regs_reg – размер holding registers в
  //!   регистрах 0 .. 65536
  //! \param[in] a_input_regs_reg – размер input registers в
  //!   регистрах 0 ..65536
  //! \param[in] a_update_time – период чтения полного массива данных
  //!   (для автоматического режима)
  //! \param[in] a_error_count_max – максимальное количество попыток передачи
  //!   запроса до выставления status_error. Попытка считается неудачной,
  //!   если получены данные об ошибке или не получен ответ на запрос
  //! \param[in] a_disconnect_time – время, через которое сервер
  //!   реагирует на обрыв связи в случае сбоев при попытках совершить
  //!   обмен данными
  modbus_client_t(
    hardflow_t* ap_hardflow,
    mode_refresh_t a_refresh_mode = mode_refresh_auto,
    size_t a_discr_inputs_size_byte = 8,
    size_t a_coils_size_byte = 8,
    size_t a_hold_regs_reg = 7,
    size_t a_input_regs_reg = 7,
    counter_t a_update_time = make_cnt_ms(200),
    irs_u8 a_error_count_max = 3,
    counter_t a_disconnect_time = make_cnt_s(2),
    irs_u16 a_size_of_packet = 260,
    irs_u8 a_unit_id = 0
  );
  void set_delay_time(double time);
  virtual ~modbus_client_t();
  virtual irs_uarc size();
  virtual irs_bool connected();
  virtual void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  virtual irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  virtual void mark_to_send(irs_uarc a_index, irs_uarc a_size);
  virtual void mark_to_recieve(irs_uarc a_index, irs_uarc a_size);
  virtual void mark_to_send_bit(irs_uarc a_byte_index, irs_uarc a_bit_index);
  virtual void mark_to_recieve_bit(irs_uarc a_byte_index, irs_uarc a_bit_index);
  virtual void update();
  virtual status_t status() const;
  virtual void set_refresh_mode(mode_refresh_t a_refresh_mode);
  virtual void abort();
  virtual void tick();
  
private:
  enum {
    size_of_MBAP = 7,
    size_of_read_header = 4,
    size_of_resp_header = 2,
    size_of_req_excep = 1,
    size_of_req_multi_write = 6,
    size_of_resp_multi_write = 6,
    packet_size_max = 260,
    packet_size_min = 16
  };
  enum condition_t {
    //! \brief Режим чтения полного массива данных для автоматического режима.
    request_start = 0,
    //! \brief чтение coils.
    read_coils = 1,
    //! \brief Чтение discret_inputs.
    read_discrete_inputs = 2,
    //! \brief Чтение holding_registers.
    read_hold_registers = 3,
    //! \brief чтение input_registers.
    read_input_registers = 4,
    //! \brief Запись одиночного coils.
    write_single_coil = 5,
    //! \brief Запись одиночного регистра.
    write_single_register = 6,
    //! \brief Чтение ошибочного статуса.
    read_exception_status = 7,
    //! \brief Запись множественных coils.
    write_multiple_coils = 15,
    //! \brief Запись множественных регистров.
    write_multiple_registers = 16
  };
  enum mode_t {
    //! \brief Ожидание команды update().
    wait_command_mode,
    //! \brief Поиск данных на запись.
    search_write_data_mode,
    //! \brief Формирование запросов на запись.
    request_write_data_mode,
    convert_request_mode,
    //! \brief Отправка запроса.
    send_request_mode,
    //! \brief Чтение заголовка ответа от сервера.
    read_header_mode,
    //! \brief Чтение тела ответа от сервера.
    read_response_mode,
    //! \brief Обработка ответа.
    treatment_response_mode,
    //! \brief Поиск данных для чтения.
    search_read_data_mode,
    //! \brief Формирование запросов на чтение.
    request_read_data_mode,
    //! \brief Формирование запросов на чтение полного массива
    //!   (для автоматического режима).
    make_request_mode
  };
  
  irs_u16                               m_size_of_packet;
  irs_u16                               m_size_of_data_write_byte;
  irs_u16                               m_size_of_data_read_byte;
  irs_u16                               m_size_of_data_write_reg;
  irs_u16                               m_size_of_data_read_reg;
  //! \brief Индекс, использующийся для формирования запросов по чтению.
  size_t                                m_global_read_index;
  //! \brief Размер discret_inputs в битах.
  size_t                                m_discret_inputs_size_bit;
  //! \brief Размер coils в битах.
  size_t                                m_coils_size_bit;
  //! \brief Размер holding_registers в регистрах.
  size_t                                m_hold_registers_size_reg;
  //! \brief Размер input_registers в регистрах.
  size_t                                m_input_registers_size_reg;
  //! \brief Конец участка discret_inputs, входящего в запрашиваемый диапазон
  //!   при выполнении операций.
  size_t                                m_discret_inputs_end_byte;
  //! \brief Конец участка coils, входящего в запрашиваемый диапазон при
  //!   выполнении операций.
  size_t                                m_coils_end_byte;
  //! \brief – Конец участка holding_registers, входящего в запрашиваемый
  //!   диапазон при выполнении операций.
  size_t                                m_hold_registers_end_byte;
  //! \brief Конец учаска input_registers, входящего в запрашиваемый диапазон
  //!   при выполни операций.
  size_t                                m_input_registers_end_byte;
  //! \brief Массив данных, используемый при отправке запроса серверу.
  raw_data_t<irs_u8>                    m_spacket;
  //! \brief Массив данных, используемый при приеме ответа от сервера.
  raw_data_t<irs_u8>                    m_rpacket;
  //! \brief Длина пакета без заголовка.
  size_t                                m_size_byte_end;
  //! \brief Флаг, показывающий состояние операции чтения.
  bool                                  m_read_table;
  //! \brief флаг, показывающий состояние операции записи.
  bool                                  m_write_table;
  bool                                  m_write_complete;
  //! \brief Номер канала клиента.
  channel_type                          m_channel;
  //! \brief Положение начала диапазона в массиве флагов на
  //!   чтение/запись данных.
  size_t                                m_start_block;
  //! \brief Поиска флага в массиве флагов на чтение/запись.
  size_t                                m_search_index;
  //! \brief Размер в байтах discret_inputs, входящих в запрашиваемый диапазон
  //!   при выполнении операций.
  size_t                                m_discret_inputs_size_byte;
  //! \brief Начало области массива discret_inputs, входящей в запрашиваемый
  //!   диапазон.
  size_t                                m_discret_inputs_start_byte;
  //! \brief Размер в байтах coils, входящих в запрашиваемый диапазон при
  //!   выполнении операций.
  size_t                                m_coils_size_byte;
  //! \brief Начало области массива coils, входящей в запрашиваемый диапазон.
  size_t                                m_coils_start_byte;
  //! \brief Размер в байтах holding_registers, входящих в запрашиваемый
  //!   диапазон при выполнении операций.
  size_t                                m_hold_registers_size_byte;
  //! \brief Начало области массива holding_registers, входящей в
  //!   запрашиваемый диапазон.
  size_t                                m_hold_registers_start_byte;
  //! \brief Размер в байтах input_register, входящих в запрашиваемый диапазон
  //!   при выполнении операций.
  size_t                                m_input_registers_size_byte;
  //! \brief Начало области массива inputs_registers, входящей в
  //!   запрашиваемый диапазон.
  size_t                                m_input_registers_start_byte;
  //! \brief Массив для хранения флагов, помечающих данные для чтения.
  vector<bool>                          m_need_read;
  //! \brief Массив для хранения флагов, помечающих данные для записи.
  vector<bool>                          m_need_writes;
  vector<bool>                          m_need_writes_reserve;
  //! \brief Байтовый массив discret_inputs, используемый в операциях чтения.
  raw_data_t<irs_u8>                    m_discr_inputs_byte_read;
  //! \brief Байтовый массив coils, используемый в операциях чтения.
  raw_data_t<irs_u8>                    m_coils_byte_read;
  //! \brief Байтовый массив coils, используемый в операциях записи.
  raw_data_t<irs_u8>                    m_coils_byte_write;
  //! \brief Регистровый массив input_registers, используемый в
  //!   операциях чтения.
  raw_data_t<irs_u16>                   m_input_regs_reg_read;
  //! \brief Регистровый массив holding_registers, используемый в
  //!   операциях чтения.
  raw_data_t<irs_u16>                   m_hold_regs_reg_read;
  //! \brief Регистровый массив holding_registers, используемый в операциях
  //!   записи.
  raw_data_t<irs_u16>                   m_hold_regs_reg_write;
  //! \brief Вид запроса.
  condition_t                           m_command;
  //! \brief режим работы клиента при формировании запросов на чтение.
  condition_t                           m_request_type;
  //! \brief Режим работы клиента по основной ветви.
  mode_t                                m_mode;
  //! \brief Циклический таймер, определяющий интервал повторного прочтения
  //!   полного массива данных.
  loop_timer_t                          m_loop_timer;
  //! \brief Коммуникационный протокол, на базе которого работает клиент.
  hardflow_t*                           mp_hardflow_client;
  //! \brief Вспомогательный класс, предназначенный для чтения/записи
  //!   известного(фиксированного) объема данных.
  hardflow::fixed_flow_t                m_fixed_flow;
  //! \brief Стартовый адрес для читаемой/записываемой области данных.
  size_t                                m_start_addr;
  //! \brief Значение записываемого бита для coils.
  irs_u16                               m_coil_write_bit;
  //! \brief Индекс записываемого бита для coils.
  size_t                                m_coil_bit_index;
  //! \brief Измерительный таймер.
  mx_time_int_t                         m_send_measure_time;
  //! \brief Флаг, выставляемый при первом полном прочтении массива серверафлаг,
  //!   выставляемый при первом полном прочтении массива сервера.
  bool                                  m_first_read;
  //! \brief Измерительный таймер.
  measure_time_t                        m_measure_time;
  //! \brief Время, затрачиваемое на операции чтения/записи.
  double                                m_measure_int_time;
  //! \brief Состояние операции update().
  status_t                              m_operation_status;
  //! \brief Режим работы клиента (ручной или автоматический).
  mode_refresh_t                        m_refresh_mode;
  //! \brief Флаг, разрешающий старт работы клиента  в ручном режиме.
  bool                              m_start;
  //! \brief Количество данных для записи, находящихся  в одном
  //!   непрерывном блоке.
  irs_u16                               m_write_quantity;
  //! \brief Количество данных для чтения, находящихся в одном
  //!   непрерывном блоке.
  irs_u16                               m_read_quantity;
  //! \brief Максимальное количество попыток передачи запроса до выставления
  //!   status_error.
  //!
  //! Попытка считается неудачной, если получены данные об ошибке или не
  //!   получен ответ на запрос.
  irs_u8                                m_error_count_max;
  irs_u16                               m_transaction_id;
  irs_u16                               m_request_quantity_discr_inputs_bit;
  irs_u16                               m_request_quantity_coils_bit;
  //! \brief Текущее количество неудачных попыток передачи запроса
  irs_u8                                m_error_count;
  irs_u8                                m_unit_id;
  timer_t                               m_send_request_timer;
  static char const IRS_CSTR_NONVOLATILE       m_wait_command_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_search_write_data_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_request_write_data_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_convert_request_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_send_request_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_read_header_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_read_response_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_treatment_response_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_search_read_data_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_request_read_data_mode[];
  static char const IRS_CSTR_NONVOLATILE       m_make_request_mode[];
  static IRS_CSTR_NONVOLATILE char const IRS_CSTR_NONVOLATILE* const
    m_ident_name_list[];
  //! \brief Формирование запроса от клиента серверу.
  //!
  //! \param[in] a_index – адрес начала запрашиваемой позиции;
  //! \param[in] a_size – размер данных для записи или чтения.
  void make_packet(size_t a_index, irs_u16 a_size);
  void modbus_pack_request_monitor(irs_u8 *ap_buf);
  void modbus_pack_response_monitor(irs_u8 *ap_buf);
  void view_mode();
  size_t get_packet_number();
  void reconnect();
};

//! @}

} //namespace irs

#endif //irsmbusH
