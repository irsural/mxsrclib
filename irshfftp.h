#ifndef IRSTFTPH
#define IRSTFTPH

#include <irsdefs.h>

#include <hardflowg.h>
#include <mxdata.h>
#include <irsalg.h>

#include <irsfinal.h>

namespace irs {

#ifndef IRS_LIB_HFFTP_DEBUG_TYPE
# define IRS_LIB_HFFTP_DEBUG_TYPE IRS_LIB_DEBUG_NONE
#endif // !IRS_LIB_HARDFLOWG_DEBUG_TYPE

# if (IRS_LIB_HFFTP_DEBUG_TYPE == IRS_LIB_DEBUG_BASE)
# define IRS_LIB_HFFTP_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
# define IRS_LIB_HFFTP_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG(msg)
# define IRS_LIB_HFFTP_DBG_MSG_SRC_BASE(msg) IRS_LIB_DBG_MSG_SRC(msg)
# define IRS_LIB_HFFTP_DBG_RAW_MSG_DETAIL(msg)
# define IRS_LIB_HFFTP_DBG_MSG_DETAIL(msg)
# define IRS_LIB_HFFTP_DBG_RAW_MSG_BLOCK_BASE(msg) msg
# define IRS_LIB_HFFTP_DBG_RAW_MSG_BLOCK_DETAIL(msg)
#elif (IRS_LIB_HFFTP_DEBUG_TYPE == IRS_LIB_DEBUG_DETAIL)
# define IRS_LIB_HFFTP_DBG_RAW_MSG_BASE(msg) IRS_LIB_DBG_RAW_MSG(msg)
# define IRS_LIB_HFFTP_DBG_MSG_BASE(msg) IRS_LIB_DBG_MSG(msg)
# define IRS_LIB_HFFTP_DBG_MSG_SRC_BASE(msg) IRS_LIB_DBG_MSG_SRC(msg)
# define IRS_LIB_HFFTP_DBG_RAW_MSG_BLOCK_BASE(msg)
# define IRS_LIB_HFFTP_DBG_RAW_MSG_DETAIL(msg) IRS_LIB_DBG_RAW_MSG(msg)
# define IRS_LIB_HFFTP_DBG_MSG_DETAIL(msg) IRS_LIB_DBG_MSG(msg)
# define IRS_LIB_HFFTP_DBG_RAW_MSG_BLOCK_DETAIL(msg) msg
#else // IRS_LIB_HFFTP_DEBUG_TYPE == IRS_LIB_DEBUG_NONE
# define IRS_LIB_HFFTP_DBG_RAW_MSG_BASE(msg)
# define IRS_LIB_HFFTP_DBG_MSG_BASE(msg)
# define IRS_LIB_HFFTP_DBG_MSG_SRC_BASE(msg)
# define IRS_LIB_HFFTP_DBG_RAW_MSG_BLOCK_BASE(msg)
# define IRS_LIB_HFFTP_DBG_RAW_MSG_DETAIL(msg)
# define IRS_LIB_HFFTP_DBG_MSG_DETAIL(msg)
# define IRS_LIB_HFFTP_DBG_MSG_SRC_DETAIL(msg)
# define IRS_LIB_HFFTP_DBG_RAW_MSG_BLOCK_DETAIL(msg)
#endif // IRS_LIB_HFFTP_DEBUG_TYPE == IRS_LIB_DEBUG_NONE

#if (defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__))

namespace hfftp {

enum error_t {
  error_not_defined,
  error_file_not_found,
  error_access_violation,
  error_disk_full,
  error_illegal_hfftp_operation,
  error_unknown_transfer_id,
  error_file_already_exists,
  error_no_such_user,
  error_no_error
  //error_abort_io_operation
};

enum file_error_t {
  file_error_file_not_found,
  file_error_access_violation,
  file_error_disk_full,
  file_error_file_full,
  file_error_file_already_exists,
  file_error_unknown,
  file_error_no_error
};

error_t file_error_to_error(file_error_t a_file_error);

class file_base_t
{
public:
  typedef irs_size_t size_type;
  virtual size_type size() const = 0;
  virtual ~file_base_t()
  { }
};

class file_read_only_t: public file_base_t
{
public:
  virtual ~file_read_only_t()
  { }
  virtual size_type read(irs_u8* ap_buf, size_type a_size) = 0;
  //! \brief Устанавливается при попытке прочитать байты,
  //!   расположенные после конца
  virtual bool end_of_file() const = 0;
  virtual file_error_t error() const = 0;
  virtual bool fail() const
  {
    return error() != file_error_no_error;
  }
  virtual bool size_is_known() const
  {
    return false;
  }
  virtual size_type size() const
  {
    return 0;
  }
};

class file_write_only_t: public file_base_t
{
public:
  virtual ~file_write_only_t()
  { }
  virtual size_type write(const irs_u8* ap_buf, size_type a_size) = 0;
  virtual void write_abort() = 0;
  virtual file_error_t error() const = 0;
  virtual bool fail() const
  {
    return error() != file_error_no_error;
  }
  virtual bool max_size_is_known() const
  {
    return false;
  }
  virtual size_type max_size() const
  {
    return std::numeric_limits<size_type>::max();
  }
};

class file_t: public file_read_only_t, public file_write_only_t
{
public:
  typedef irs_size_t size_type;
};

class memory_file_read_only_t: public file_read_only_t
{
public:
  memory_file_read_only_t(const irs_u8* ap_buf, size_type a_size);
  virtual size_type read(irs_u8* ap_buf, size_type a_size);
  virtual bool end_of_file() const;
  virtual file_error_t error() const;
  virtual bool size_is_known() const;
  virtual size_type size() const;
private:
  const irs_u8* mp_buf;
  size_type m_size;
  size_type m_pos;
  bool m_end_of_file;
};

class memory_file_write_only_t: public file_write_only_t
{
public:
  memory_file_write_only_t(size_type a_file_max_size =
    std::numeric_limits<size_type>::max());
  virtual size_type write(const irs_u8* ap_buf, size_type a_size);
  virtual void write_abort();
  virtual file_error_t error() const;
  virtual size_type size() const;
  virtual bool max_size_is_known() const;
  virtual size_type max_size() const;
  //! \brief Чтение от начала.
  size_type read(irs_u8* ap_buf, size_type a_size) const;
private:
  vector<irs_u8> m_buf;
  size_type m_max_size;
  file_error_t m_last_error;
};

#ifndef IRS_MICROCONTROLLER

class ifstream_file_t: public file_read_only_t
{
public:
  typedef irs::char_t char_type;
  typedef irs::string_t string_type;
  typedef std::ifstream ifstream_type;
  ifstream_file_t(const string_type& a_file_name);
  virtual ~ifstream_file_t();
  virtual size_type read(irs_u8* ap_buf, size_type a_size);
  virtual bool end_of_file() const;
  virtual file_error_t error() const;
  virtual bool size_is_known() const;
  virtual size_type size() const;
private:
  ifstream_file_t();
  ifstream_type m_ifile;
  //bool m_success;
  size_type m_size;
  //size_type m_pos;
  bool m_eof;
};

class ofstream_file_t: public file_write_only_t
{
public:
  typedef irs::char_t char_type;
  typedef irs::string_t string_type;
  typedef std::ofstream ofstream_type;
  ofstream_file_t(const string_type& a_file_name);
  virtual ~ofstream_file_t();
  virtual size_type write(const irs_u8* ap_buf, size_type a_size);
  virtual void write_abort();
  virtual file_error_t error() const;
  virtual size_type size() const;
  virtual bool max_size_is_known() const;
  virtual size_type max_size() const;
public:
  ofstream_file_t();
  ofstream_type m_ofile;
  const string_type m_file_name;
  size_type m_size;
};

#endif // !IRS_MICROCONTROLLER

class various_page_mem_file_write_only_t: public irs::hfftp::file_write_only_t
{
public:
  typedef irs_u32 file_size_type;
  various_page_mem_file_write_only_t(various_page_mem_t* ap_various_page_mem,
    irs_u8* ap_begin,
    size_type a_size,
    bool a_save_size_enabled = false,
    size_type a_buf_max_size = 1024);
  virtual size_type write(const irs_u8* ap_buf, size_type a_size);
  virtual void write_abort();
  virtual irs::hfftp::file_error_t error() const;
  virtual size_type size() const;
  virtual bool max_size_is_known() const;
  virtual size_type max_size() const;
  //! \brief Функция выполняет сохранение размера файла.
  //! \details Размер файла можно сохранить только один раз.
  //!   Размер сохраняется перед данными, в начало первой страницы указанного
  //!   диапазона
  bool save_size_enabled() const;
  void save_size();
  bool size_saved() const;
  irs_status_t status() const;
  void tick();
private:
  various_page_mem_file_write_only_t();
  enum process_t {
    process_erase,
    process_wait_for_erase_operation,
    process_write,
    process_save_size,
    process_wait_for_write_operation,
    process_wait_command
  };
  various_page_mem_t* mp_various_page_mem;
  size_type m_first_page_index;
  size_type m_last_page_index;
  std::vector<irs_u8> m_buf;
  bool m_save_size_enabled;
  size_type m_buf_max_size;
  irs_u8* mp_pos;
  irs_u8* mp_begin;
  irs_u8* mp_end;
  size_type m_size;
  size_type m_current_page_index;
  process_t m_process;
  size_type m_max_size;
  irs::hfftp::file_error_t m_last_error;
  bool m_size_saved;
};

class files_t
{
public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  virtual ~files_t()
  { }
  virtual file_read_only_t* open_for_read(
    const string_type& a_file_name,
    file_error_t* ap_error_code) = 0;
  virtual file_write_only_t* open_for_write(
    const string_type& a_file_name,
    file_error_t* ap_error_code) = 0;
  virtual void close(file_base_t* ap_file) = 0;
};

#ifndef IRS_MICROCONTROLLER
class fstream_files_t: public files_t
{
public:
  typedef irs::string_t string_type;
  fstream_files_t();
  virtual file_read_only_t* open_for_read(
    const string_type& a_file_name,
    file_error_t* ap_error_code);
  virtual file_write_only_t* open_for_write(
    const string_type& a_file_name,
    file_error_t* ap_error_code);
  virtual void close(file_base_t* ap_file);
private:
  typedef std::map<string_type, irs::handle_t<file_base_t> >
    files_map_type;
  files_map_type m_files;
};
#endif // !IRS_MICROCONTROLLER

// RFC1350
enum packet_type_t {
  pt_read_request = 1,
  pt_write_request = 2,
  pt_data = 3,
  pt_acknowledgment = 4,
  pt_acknowledgment_options = 5,
  pt_error = 6
};

enum {
  header_size = 6,
  field_opcode_size = 1,
  field_block_index_size = 4,
  field_data_size_size = 4
};

typedef irs_u8 opcode_t;
typedef irs_u32 packet_size_t;
typedef irs_u32 crc32_t;
typedef irs_u8 bool_t;
typedef irs_u16 data_size_t;
typedef irs_u16 option_count_t;
typedef irs_u16 option_code_t;
typedef irs_u32 timeout_t;
typedef irs_u16 retransmit_count_t;
typedef irs_u32 block_index_t;
typedef irs_u32 error_code_t;
typedef irs_u32 file_size_t;

extern const std::size_t data_min_size;
extern const double min_timeout;

//! Пакет запроса чтения
//! 5 байт    2 байта       4 байта    1 байта       2 байта
//! ----------------------------------------------------------------
//! |  ID  | Размер пакета  | crc32 | Код пакета | Длина имени файла |
//! ----------------------------------------------------------------
//!
//!   1 байт на символ    4 байта         2 байта
//!   -------------------------------------------------
//!   | Имя файла     | Размер данных | Тайм-аут, мсек |
//!   -------------------------------------------------
//!
//!     2 байта
//!   ------------------------------
//!   | Количество повторных передач |
//!   ------------------------------

class field_t
{
public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  virtual ~field_t()
  { }
  virtual bool read(const vector<irs_u8>& a_buf, size_type* ap_pos) = 0;
  virtual void push_back(vector<irs_u8>* ap_buf) = 0;
};

template <class T>
class field_simple_type_t: public field_t
{
public:
  field_simple_type_t(T* ap_opcode);
  virtual bool read(const vector<irs_u8>& a_buf, size_type* ap_pos);
  virtual void push_back(vector<irs_u8>* ap_buf);
  void write(vector<irs_u8>* ap_buf, size_type a_pos);
private:
  typedef irs::hardflow::fixed_flow_t fixed_flow_type;
  field_simple_type_t();
  T* mp_value;
};

template <class T>
irs::hfftp::field_simple_type_t<T>::field_simple_type_t(T* ap_value):
  mp_value(ap_value)
{
}

template <class T>
bool irs::hfftp::field_simple_type_t<T>::read(const vector<irs_u8>& a_buf,
  size_type* ap_pos)
{
  IRS_LIB_ASSERT(ap_pos);
  if ((a_buf.size() - *ap_pos) < sizeof(T)) {
    return false;
  }
  *mp_value = *reinterpret_cast<const T*>(vector_data(a_buf, *ap_pos));
  (*ap_pos) += sizeof(T);
  return true;
}

template <class T>
void irs::hfftp::field_simple_type_t<T>::push_back(vector<irs_u8>* ap_buf)
{
  const size_type pos = ap_buf->size();
  ap_buf->resize(ap_buf->size() + sizeof(T));
  write(ap_buf, pos);
}

template <class T>
void irs::hfftp::field_simple_type_t<T>::write(
  vector<irs_u8>* ap_buf, size_type a_pos)
{
  *reinterpret_cast<T*>(&(*ap_buf)[a_pos]) = *mp_value;
}

class field_data_t: public field_t
{
public:
  field_data_t(vector<irs_u8>* ap_data);
  virtual bool read(const vector<irs_u8>& a_buf, size_type* ap_pos);
  virtual void push_back(vector<irs_u8>* ap_buf);
private:
  field_data_t();
  vector<irs_u8>* mp_data;
};

class field_string_t: public field_t
{
public:
  field_string_t(string_type* ap_string);
  virtual bool read(const vector<irs_u8>& a_buf, size_type* ap_pos);
  virtual void push_back(vector<irs_u8>* ap_buf);
private:
  typedef irs::hardflow::fixed_flow_t fixed_flow_type;
  enum { m_string_max_size = 1024 };
  field_string_t();
  string_type* mp_string;
};

enum option_name_t {
  option_get_file_size = 0,
  option_file_size = 1
};

class option_value_t
{
public:
  virtual ~option_value_t()
  {
  }
  virtual irs::handle_t<field_t> make_field() = 0;
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  virtual bool equals(const option_value_t* ap_option_value) const = 0;
  #endif // IRS_FULL_STDCPPLIB_SUPPORT
};

template <class T>
class option_value_simple_type_t: public option_value_t
{
public:
  option_value_simple_type_t():
    m_value(0)
  {
  }
  virtual irs::handle_t<field_t> make_field()
  {
    return handle_t<field_t>(new field_simple_type_t<T>(&m_value));
  }
  #ifdef IRS_FULL_STDCPPLIB_SUPPORT
  virtual bool equals(const option_value_t* ap_option_value) const
  {
    const option_value_simple_type_t<T>* option_value =
      dynamic_cast<const option_value_simple_type_t<T>* >(ap_option_value);
    if (!option_value) {
      return false;
    }
    return m_value == option_value->m_value;
  }
  #endif // IRS_FULL_STDCPPLIB_SUPPORT
  T get() const
  {
    return m_value;
  }
  void set(T a_value)
  {
    m_value = a_value;
  }
private:
  T m_value;
};

handle_t<option_value_t> make_option_value(option_name_t a_option_name);

class field_options_t: public field_t
{
public:
  field_options_t(
    std::map<option_name_t, handle_t<option_value_t> >* ap_options);
  virtual bool read(const vector<irs_u8>& a_buf, size_type* ap_pos);
  virtual void push_back(vector<irs_u8>* ap_buf);
private:
  typedef irs::hardflow::fixed_flow_t fixed_flow_type;
  enum { m_string_max_size = 1024 };
  field_options_t();
  std::map<option_name_t, handle_t<option_value_t> >* mp_options;
};

class fields_flow_t
{
public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  fields_flow_t(opcode_t a_opcode,
    irs::hardflow_t* ap_hardflow, size_type a_channel);
  void set_channel(size_type a_channel);
  //void add_id_and_opcode_write_only(opcode_t a_opcode);
  template <class T>
  void add_simple_type(T* ap_value);
  void add_string(string_type* ap_string);
  void add_data(vector<irs_u8>* ap_data);
  void erase_fields();
  void read();
  void write();
  irs_status_t status() const;
  void abort();
  void set_data_max_size(data_size_t a_data_max_size);
  void tick();
private:
  typedef irs::hardflow::fixed_flow_t fixed_flow_type;
  enum process_t {
    process_write,
    process_read_packet_size,
    process_read,
    process_wait_command
  };

  fields_flow_t();
  void push_back_id_and_opcode(vector<irs_u8>* ap_buf);
  opcode_t m_opcode;
  irs::hardflow_t* mp_hardflow;
  fixed_flow_type m_fixed_flow;
  size_type m_channel;
  packet_size_t m_packet_max_size;
  irs_status_t m_status;
  process_t m_process;
  vector<irs_u8> m_buf;
  vector<irs::handle_t<field_t> > m_fields;
  size_type m_current_field;
  crc32_table_stream_t m_packet_crc32;
};

template <class T>
void fields_flow_t::add_simple_type(T* ap_value)
{
  irs::handle_t<field_t> field(new field_simple_type_t<T>(ap_value));
  m_fields.push_back(field);
}

class packet_t
{
public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  packet_t(packet_type_t a_packet_type, irs::hardflow_t* ap_hardflow);
  virtual ~packet_t()
  { }
  virtual packet_type_t get_type() const;
  virtual void read(size_type a_channel);
  virtual void write(size_type a_channel);
  virtual void abort();
  virtual void tick();
  virtual irs_status_t status() const;
  void set_data_max_size(data_size_t a_data_max_size);
protected:
  fields_flow_t* fields_flow();
private:
  packet_t();
  enum process_t {
    process_read_or_write,
    process_wait_command
  };
  packet_type_t m_packet_type;
  irs_status_t m_status;
  process_t m_process;
  size_type m_channel;
  fields_flow_t m_fields_flow;
};

//! \brief Пакет запроса чтения
class packet_read_request_t: public packet_t
{
public:
  struct fields_t {
    string_type file_name;
    data_size_t data_max_size;
    timeout_t timeout_ms;
    retransmit_count_t retransmit_count;
    fields_t():
      file_name(),
      data_max_size(0),
      timeout_ms(1000),
      retransmit_count(3)
    {
    }
    bool operator==(const fields_t& a_fields) const
    {
      return (file_name == a_fields.file_name) &&
        (data_max_size == a_fields.data_max_size) &&
        (timeout_ms == a_fields.timeout_ms) &&
        (retransmit_count == a_fields.retransmit_count);
    }
  };
  packet_read_request_t(irs::hardflow_t* ap_hardflow);
  const fields_t& get_fields() const;
  void set_fields(const fields_t& a_fields);
private:
  packet_read_request_t();
  fields_t m_fields;
};

//! \brief Пакет запроса записи
class packet_write_request_t: public packet_t
{
public:
  struct fields_t {
    string_type file_name;
    bool_t file_size_is_known;
    file_size_t file_size;
    data_size_t data_max_size;
    timeout_t timeout_ms;
    retransmit_count_t retransmit_count;
    fields_t():
      file_name(),
      file_size_is_known(false),
      file_size(0),
      data_max_size(0),
      timeout_ms(1000),
      retransmit_count(3)
    {
    }
  };
  packet_write_request_t(irs::hardflow_t* ap_hardflow);
  const fields_t& get_fields() const;
  void set_fields(const fields_t& a_fields);
private:
  packet_write_request_t();
  fields_t m_fields;
};

class packet_data_t: public packet_t
{
public:
  struct fields_t {
    block_index_t block_index;
    vector<irs_u8> data;
    fields_t():
      block_index(0),
      data()
    {
    }
  };
  packet_data_t(irs::hardflow_t* ap_hardflow);
  const fields_t& get_fields() const;
  void set_fields(const fields_t& a_fields);
private:
  packet_data_t();
  fields_t m_fields;
};

class packet_acknowledgment_t: public packet_t
{
public:
  struct fields_t
  {
    block_index_t block_index;
    fields_t():
      block_index(0)
    {
    }
  };
  packet_acknowledgment_t(irs::hardflow_t* ap_hardflow);
  const fields_t& get_fields() const;
  void set_fields(const fields_t& a_fields);
private:
  packet_acknowledgment_t();
  fields_t m_fields;
};

class packet_acknowledgment_options_t: public packet_t
{
public:
  struct fields_t
  {
    bool_t file_size_is_known;
    file_size_t file_size;
    data_size_t data_max_size;
    timeout_t timeout_ms;
    retransmit_count_t retransmit_count;
    fields_t():
      file_size_is_known(),
      file_size(),
      data_max_size(0),
      timeout_ms(1000),
      retransmit_count(3)
    {
    }
  };
  packet_acknowledgment_options_t(irs::hardflow_t* ap_hardflow);
  const fields_t& get_fields() const;
  void set_fields(const fields_t& a_fields);
private:
  packet_acknowledgment_options_t();
  fields_t m_fields;
};

class packet_error_t: public packet_t
{
public:
  struct fields_t
  {
    error_code_t error_code;
    string_type error_message;
    fields_t():
      error_code(0),
      error_message()
    {
    }
  };
  packet_error_t(irs::hardflow_t* ap_hardflow);
  const fields_t& get_fields() const;
  void set_fields(const fields_t& a_fields);
private:
  packet_error_t();
  fields_t m_fields;
};

struct protocol_configuration_t
{
  typedef std::size_t size_type;
  size_type data_size;
  double timeout;
  size_type retransmit_max_count;
  protocol_configuration_t():
    data_size(1300),
    timeout(3),
    retransmit_max_count(3)
  {
  }
};

timeout_t timeout_to_timeout_ms(double a_timeout);
double timeout_ms_to_timeout(timeout_t a_timeout_ms);

class packet_flow_t
{
public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  packet_flow_t(irs::hardflow_t* ap_hardflow);
  void send_read_request(size_type a_channel,
    const packet_read_request_t::fields_t& a_fields);
  void send_write_request(size_type a_channel,
    const packet_write_request_t::fields_t& a_fields);
  void send_data(size_type a_channel, block_index_t a_block_index,
    const vector<irs_u8>& a_data);
  void send_acknowledgment(size_type a_channel, size_type a_block_index);
  void send_acknowledgment_options(size_type a_channel,
    const packet_acknowledgment_options_t::fields_t& a_fields);
  void send_error(size_type a_channel, error_t a_error,
    const string_type& a_error_message);
  void retransmit_last_packet(size_type a_channel);
  size_type get_retransmit_count() const;
  void receive_packet(size_type a_channel);
  void receive_packet_without_reset_timeout(size_type a_channel);
  packet_t* get_packet();
  void set_data_max_size(size_type a_max_size);
  irs_status_t status() const;
  void enable_send_timeout(double a_seconds);
  void disable_send_timeout();
  void enable_receive_timeout(double a_seconds);
  void disable_receive_timeout();
  void abort();
  void tick();
private:
  typedef irs::hardflow::fixed_flow_t fixed_flow_type;
  enum process_t {
    process_wait_send_packet,
    process_find_packet_begin,
    process_wait_opcode,
    process_receive_answer,
    process_wait_command
  };
  packet_flow_t();
  void make_packet(opcode_t a_opcode);
  void send_timeout_start(bool a_enabled);
  void receive_timeout_start(bool a_enabled);
  irs::hardflow_t* mp_hardflow;
  fixed_flow_type m_fixed_flow;
  irs::handle_t<packet_t> mp_send_packet;
  irs::handle_t<packet_t> mp_receive_packet;
  timer_t m_send_timeout;
  bool m_send_timeout_enabled;
  timer_t m_receive_timeout;
  bool m_receive_timeout_enabled;
  size_type m_retransmit_count;
  irs_status_t m_status;
  vector<irs_u8> m_write_buf;
  vector<irs_u8> m_read_buf;
  //timer_t m_read_timeout_timer;
  size_type m_channel;
  process_t m_process;
  size_type m_data_max_size;
};

enum mode_t {
  mode_read,
  mode_write
};

class client_t
{
public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  struct configuration_t
  {
    typedef std::size_t size_type;
    size_type data_size;
    double timeout;
    size_type retransmit_max_count;
    configuration_t():
      data_size(1300),
      timeout(3),
      retransmit_max_count(3)
    {
    }
  };
  typedef configuration_t configuration_type;
  client_t(irs::hardflow_t* ap_transport,
    const configuration_type& a_configuration = configuration_t());
  void get(const string_type& a_remote_file_name,
    file_write_only_t* ap_file);
  void put(const string_type& a_remote_file_name,
    file_read_only_t* ap_file);
  //void get(irs_u8* ap_buf, size_type a_size);
  //void put(const irs_u8* ap_buf, size_type a_size);
  //! \brief Возвращает \c true, когда соединение установлено
  bool connection_is_established() const;
  //! \brief Функция возвращает текущие настройки протокола. Значение, возвращаемое
  //!   функцией меняется, когда устанавливается соединение и
  //!   функция connection_is_established возвращает \c true
  const protocol_configuration_t& get_protocol_configuration() const;
  irs_status_t status() const;
  size_type transferred_bytes() const;
  size_type file_size_is_known() const;
  size_type get_file_size() const;
  void get_last_error(error_t* ap_error, string_type* ap_error_message);
  void tick();
  void set_changed_event(
    handle_t<event_1_t<const client_t&> > a_event);
private:
  void exec_changed_event();
  void set_error(error_t a_error, const string_type& a_error_message);
  bool agree_configuration(
    const packet_acknowledgment_options_t::fields_t& a_fields);
  void abort_writing_file_if_write_mode();
  protocol_configuration_t extract_protocol_config(
    const configuration_t& a_config);
  typedef irs::hardflow::fixed_flow_t fixed_flow_type;
  enum process_t {
    //process_send_read_request,
    process_wait_send_request,
    process_wait_receive_acknowledgment_options,
    process_wait_send_acknowledgment_options,
    process_wait_send_acknowledgment,
    process_receive_acknowledgment,
    process_wait_receive_data,
    process_send_data,
    process_write_data_to_file,
    process_read_data_from_file,
    //process_send_error,
    process_wait_command
  };
  //void send_packet_read_request();
  void set(process_t a_process);
  configuration_type normalize_configuration(
    const configuration_type& a_configuration);
  irs::hardflow_t* mp_transport;
  const configuration_type m_configuration;
  protocol_configuration_t m_protocol_config;
  bool m_connection_is_established;
  packet_flow_t m_packet_flow;
  size_type m_retransmit_count;
  double m_send_timeout;
  double m_receive_timeout;
  size_type m_channel;
  vector<irs_u8> m_read_buf;
  vector<irs_u8> m_write_buf;
  size_type m_buf_pos;
  size_type m_block_index;
  //const size_type m_data_max_size;
  //size_type m_current_data_max_size;
  file_write_only_t* mp_file_write_only;
  file_read_only_t* mp_file_read_only;
  string_type m_remote_file_name;
  process_t m_process;
  mode_t m_mode;
  irs_status_t m_status;
  error_t m_last_error;
  string_type m_last_error_message;
  bool m_file_size_is_known;
  size_type m_file_size;
  size_type m_transferred_bytes;
  //fixed_flow_type m_fixed_flow;
  timer_t m_timeout_timer;
  handle_t<event_1_t<const client_t&> > m_changed_event;
};

struct server_channel_configuration_t
{
  typedef std::size_t size_type;
  size_type data_size;
  double timeout;
  size_type retransmit_max_count;
  server_channel_configuration_t(
    size_type a_data_size = 512,
    double a_timeout = 1,
    size_type a_retransmit_max_count = 3
  ):
    data_size(a_data_size),
    timeout(a_timeout),
    retransmit_max_count(a_retransmit_max_count)
  {
  }
};

class server_channel_t
{
public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  typedef server_channel_configuration_t configuration_type;
  server_channel_t(irs::hardflow_t* ap_hardflow, size_type a_channel,
    files_t* ap_files, const configuration_type* ap_configuration);
  ~server_channel_t();
  size_type get_channel_id() const;
  //! \brief Возвращает \c true, когда соединение установлено
  bool connection_is_established() const;
  //! \brief Функция возвращает согласованные настройки. Значение, возвращаемое
  //!   функцией актуально только, когда функция connection_is_established
  //!   возвращает \c true
  const protocol_configuration_t& get_protocol_configuration() const;
  bool destroyed() const;
  irs_status_t get_file_transfer_status() const;
  mode_t get_mode() const;
  size_type transferred_bytes() const;
  const string_type& get_file_name() const;
  size_type file_size_is_known() const;
  size_type get_file_size() const;
  void get_last_error(error_t* ap_error, string_type* ap_error_message);
  void set_changed_event(
    handle_t<event_1_t<const server_channel_t&> > a_event);
  void tick();
private:
  enum process_t {
    process_receive_request,
    process_send_acknowledgment_options,
    process_receive_acknowledgment_option,
    process_read_data_from_file,
    process_write_data_to_file,
    process_send_data,
    process_receive_data,
    process_send_acknowledgment,
    process_receive_acknowledgment,
    process_send_error
  };
  server_channel_t();
  void set(process_t a_process);
  void exec_changed_event();
  void set_error(error_t a_error, const string_type& a_error_message);
  void agree_configuration(const packet_t* ap_packet);
  protocol_configuration_t extract_protocol_config(
    const configuration_type& a_config);
  irs::hardflow_t* mp_hardflow;
  const size_type m_channel;
  const configuration_type* mp_configuration;
  protocol_configuration_t m_protocol_config;
  bool m_connection_is_established;
  bool m_destroyed;
  packet_flow_t m_packet_flow;
  size_type m_retransmit_count;
  double m_send_timeout;
  double m_receive_timeout;
  files_t* mp_files;
  string_type m_local_file_name;
  file_write_only_t* mp_file_write_only;
  file_read_only_t* mp_file_read_only;
  vector<irs_u8> m_read_buf;
  vector<irs_u8> m_write_buf;
  size_type m_buf_pos;
  size_type m_block_index;
  process_t m_process;
  irs_status_t m_file_transfer_status;
  mode_t m_mode;
  error_t m_last_error;
  string_type m_last_error_message;
  string_type m_file_name;
  bool m_file_size_is_known;
  size_type m_file_size;
  size_type m_transferred_bytes;
  const size_type m_data_min_size;
  const double m_min_timeout;
  const size_type m_retransmit_min_count;
  timer_t m_timeout_timer;
  //size_type m_current_data_max_size;
  handle_t<event_1_t<const server_channel_t&> > m_changed_event;
};

typedef map<std::size_t, handle_t<server_channel_t> > server_channels_map_t;

class server_t
{
public:
  typedef std::size_t size_type;
  typedef server_channel_configuration_t configuration_type;
  server_t(irs::hardflow_t* ap_hardflow, files_t* ap_files,
    size_type a_channel_max_count,
    const configuration_type& a_configuration = configuration_type());
  void set_created_channel_event(handle_t<event_1_t<size_type> > a_event);
  void set_destroyed_channel_event(handle_t<event_1_t<size_type> > a_event);
  void set_changed_event(
    handle_t<event_1_t<const server_channels_map_t&> > a_event);
  void set_channel_changed_event(
    handle_t<event_1_t<const server_channel_t&> > a_event);
  void tick();
private:
  typedef server_channels_map_t channels_map_type;
  server_t();
  configuration_type normalize_configuration(
    const configuration_type& a_configuration);
  void exec_channel_changed_event(const server_channel_t& a_channel);
  void exec_created_channel_event(size_type a_channel_id);
  void exec_destroyed_channel_event(size_type a_channel_id);
  const size_type m_data_min_size;
  const double m_min_timeout;
  const size_type m_retransmit_min_count;
  irs::hardflow_t* mp_hardflow;
  files_t* mp_files;
  channels_map_type m_channels;
  configuration_type m_channel_configuration;
  size_type m_channel_max_count;
  channels_map_type::iterator m_channel_it;
  handle_t<event_1_t<size_type> > mp_created_channel_event;
  handle_t<event_1_t<size_type> > mp_destroyed_channel_event;
  handle_t<event_1_t<const server_channel_t&> > mp_channel_changed_event;
  handle_t<event_1_t<const channels_map_type&> > mp_changed_event;
};

} // namespace hfftp

#endif // defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__))

} // namespace irs

#endif // IRSTFTPH
