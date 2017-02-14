#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <irscpp.h>

#include <irsalg.h>

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
# if defined(_MSC_VER)
#   include <direct.h>
# elif (defined(__MINGW32__) || defined(__BORLANDC__))
#   include <dir.h>
# endif // defined(__MINGW32__)
#endif // IRS_FULL_STDCPPLIB_SUPPORT

#include <irshfftp.h>

#include <irsfinal.h>

#if (defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__))

irs::hfftp::error_t
irs::hfftp::file_error_to_error(file_error_t a_file_error)
{
  switch (a_file_error) {
    case file_error_file_not_found: {
      return error_file_not_found;
    } break;
    case file_error_access_violation: {
      return error_access_violation;
    } break;
    case file_error_disk_full: {
      return error_disk_full;
    } break;
    case file_error_file_already_exists: {
      return error_file_already_exists;
    } break;
    case file_error_no_error: {
      return error_no_error;
    } break;
    default : {
      return error_not_defined;
    }
  }
  #ifdef __BORLANDC__
  return error_not_defined;
  #endif // __BORLANDC__
}

// class memory_file_read_only_t
irs::hfftp::memory_file_read_only_t::memory_file_read_only_t(
  const irs_u8* ap_buf, size_type a_size):
  mp_buf(ap_buf),
  m_size(a_size),
  m_pos(0),
  m_end_of_file(false)
{
}

irs::hfftp::memory_file_read_only_t::size_type
irs::hfftp::memory_file_read_only_t::read(irs_u8* ap_buf, size_type a_size)
{
  const size_type count = min(m_size - m_pos, a_size);
  if (count < a_size) {
    m_end_of_file = true;
  }
  memcpyex(ap_buf, &mp_buf[m_pos], count);
  m_pos += count;
  return count;
}

bool irs::hfftp::memory_file_read_only_t::end_of_file() const
{
  return m_end_of_file;
}

irs::hfftp::file_error_t irs::hfftp::memory_file_read_only_t::error() const
{
  return file_error_no_error;
}

bool irs::hfftp::memory_file_read_only_t::size_is_known() const
{
  return true;
}

irs::hfftp::memory_file_read_only_t::size_type
irs::hfftp::memory_file_read_only_t::size() const
{
  return m_size;
}

// class memory_file_write_only_t
irs::hfftp::memory_file_write_only_t::memory_file_write_only_t(
  size_type a_file_max_size
):
  m_buf(),
  m_max_size(a_file_max_size),
  m_last_error(file_error_no_error)
{
}

irs::hfftp::memory_file_write_only_t::size_type
irs::hfftp::memory_file_write_only_t::write(
  const irs_u8* ap_buf, size_type a_size)
{
  const size_type pos = m_buf.size();
  const size_type count = min(m_max_size - m_buf.size(), a_size);
  if (count < a_size) {
    m_last_error = file_error_file_full;
  }
  m_buf.resize(m_buf.size() + count);
  memcpyex(vector_data(m_buf, pos), ap_buf, count);
  return count;
}

void irs::hfftp::memory_file_write_only_t::write_abort()
{
  m_buf.clear();
}

irs::hfftp::file_error_t
irs::hfftp::memory_file_write_only_t::error() const
{
  return m_last_error;
}

irs::hfftp::memory_file_write_only_t::size_type
irs::hfftp::memory_file_write_only_t::size() const
{
  return m_buf.size();
}

bool irs::hfftp::memory_file_write_only_t::max_size_is_known() const
{
  return true;
}

irs::hfftp::memory_file_write_only_t::size_type
irs::hfftp::memory_file_write_only_t::max_size() const
{
  return m_max_size;
}

irs::hfftp::memory_file_write_only_t::size_type
irs::hfftp::memory_file_write_only_t::read(irs_u8* ap_buf,
  size_type a_size) const
{
  const size_type count = min(m_buf.size(), a_size);
  memcpyex(ap_buf, vector_data(m_buf), count);
  return count;
}

#ifndef IRS_MICROCONTROLLER
// class ifstream_file_t
irs::hfftp::ifstream_file_t::ifstream_file_t(const string_type& a_file_name):
  m_ifile(IRS_SIMPLE_FROM_TYPE_STR(a_file_name.c_str()), ios::binary|ios::in),
  m_size(0),
  ///m_pos(0),
  m_eof(false)
{
  if (!m_ifile.fail()) {
    m_ifile.seekg(0, ios::end);
    m_size = m_ifile.tellg();
    m_ifile.seekg(0, ios::beg);
  }/* else {
    m_success = false;
  }*/
}

irs::hfftp::ifstream_file_t::~ifstream_file_t()
{
  m_ifile.close();
}

irs::hfftp::ifstream_file_t::size_type
irs::hfftp::ifstream_file_t::read(irs_u8* ap_buf, size_type a_size)
{
  if (!m_ifile.good() || m_eof) {
    return 0;
  }
  const int pos = m_ifile.tellg();
  const size_type size = min(a_size, m_size - pos);
  if (a_size > size) {
    m_eof = true;
  }
  m_ifile.read(reinterpret_cast<char*>(ap_buf), size);
  if (!m_ifile.fail()) {
    return size;
  } else {
    return 0;
  }
}

bool irs::hfftp::ifstream_file_t::end_of_file() const
{
  return m_eof;
}

irs::hfftp::file_error_t
irs::hfftp::ifstream_file_t::error() const
{
  if (m_ifile.fail()) {
    return file_error_unknown;
  } else {
    return file_error_no_error;
  }
}

bool irs::hfftp::ifstream_file_t::size_is_known() const
{
  return true;
}

irs::hfftp::ifstream_file_t::size_type
irs::hfftp::ifstream_file_t::size() const
{
  return m_size;
}

// class ofstream_file_t
irs::hfftp::ofstream_file_t::ofstream_file_t(const string_type& a_file_name):
  m_ofile(IRS_SIMPLE_FROM_TYPE_STR(a_file_name.c_str()),
    ios::binary|ios::out|ios::trunc),
  m_file_name(a_file_name),
  m_size(0)
{
}

irs::hfftp::ofstream_file_t::~ofstream_file_t()
{
  m_ofile.close();
}

irs::hfftp::ofstream_file_t::size_type
irs::hfftp::ofstream_file_t::write(const irs_u8* ap_buf, size_type a_size)
{
  if (!m_ofile.good()) {
    return 0;
  }
  m_ofile.write(reinterpret_cast<const char*>(ap_buf), a_size);
  if (!m_ofile.good()) {
    return 0;
  }
  m_size += a_size;
  return a_size;
}

void irs::hfftp::ofstream_file_t::write_abort()
{
  m_ofile.close();
  #ifdef __GNUC__
  rmdir(irs::str_conv<irs::irs_string_t>(m_file_name).c_str());
  #else // !__GNUC__
  _rmdir(irs::str_conv<irs::irs_string_t>(m_file_name).c_str());
  #endif // !__GNUC__
  m_size = 0;
}

irs::hfftp::file_error_t irs::hfftp::ofstream_file_t::error() const
{
  if (m_ofile.fail()) {
    return file_error_unknown;
  } else {
    return file_error_no_error;
  }
}

irs::hfftp::ofstream_file_t::size_type
irs::hfftp::ofstream_file_t::size() const
{
  return m_size;
}

bool irs::hfftp::ofstream_file_t::max_size_is_known() const
{
  return true;
}

irs::hfftp::ofstream_file_t::size_type
irs::hfftp::ofstream_file_t::max_size() const
{
  return std::numeric_limits<size_type>::max();
}

#endif // !IRS_MICROCONTROLLER

// class various_page_mem_file_write_only_t
irs::hfftp::various_page_mem_file_write_only_t::
various_page_mem_file_write_only_t(various_page_mem_t* ap_various_page_mem,
  irs_u8* ap_begin,
  size_type a_size,
  bool a_save_size_enabled,
  size_type a_buf_max_size
):
  mp_various_page_mem(ap_various_page_mem),
  m_first_page_index(ap_various_page_mem->page_index(ap_begin)),
  m_last_page_index(ap_various_page_mem->page_index(ap_begin + a_size - 1)),
  m_buf(),
  m_save_size_enabled(a_save_size_enabled),
  m_buf_max_size(a_buf_max_size),
  mp_pos(NULL),
  mp_begin(ap_begin),
  mp_end(ap_begin + a_size),
  m_size(0),
  m_current_page_index(m_first_page_index),
  m_process(process_erase),
  m_max_size(0),
  m_last_error(irs::hfftp::file_error_no_error),
  m_size_saved(false)
{
  IRS_LIB_ASSERT(ap_various_page_mem);
  IRS_LIB_ASSERT(m_first_page_index <= m_last_page_index);
  IRS_LIB_ASSERT(m_last_page_index < mp_various_page_mem->page_count());

  mp_pos = mp_begin;
  if (m_save_size_enabled) {
    mp_pos += sizeof(file_size_t);
  }

  if (mp_end >= mp_pos) {
    m_max_size = mp_end - mp_pos;
  }
}

irs::hfftp::various_page_mem_file_write_only_t::size_type
irs::hfftp::various_page_mem_file_write_only_t::write(
  const irs_u8* ap_buf, size_type a_size)
{
  if (m_size_saved) {
    throw std::logic_error("Размер файла уже сохранен.");
  }
  if ((m_process != process_wait_command) ||
    (m_last_error != irs::hfftp::file_error_no_error)) {
    return 0;
  }
  size_type size = 0;
  if (mp_various_page_mem->status() == irs_st_ready) {
    size = std::min(m_buf_max_size, a_size);
    size = std::min(size, static_cast<size_type>(mp_end - mp_pos));
    if (static_cast<size_type>(mp_end - mp_pos) < a_size) {
      m_last_error = irs::hfftp::file_error_file_full;
      m_size = 0;
    } else {
      m_buf.resize(size);
      memcpyex(irs::vector_data(m_buf), ap_buf, size);
      mp_various_page_mem->write(mp_pos, irs::vector_data(m_buf), size);
      m_process = process_write;
    }
  } else if (mp_various_page_mem->status() == irs_st_error) {
    m_last_error = irs::hfftp::file_error_unknown;
  }
  return size;
}

void irs::hfftp::various_page_mem_file_write_only_t::write_abort()
{
  m_size = 0;
}

irs::hfftp::file_error_t
irs::hfftp::various_page_mem_file_write_only_t::error() const
{
  return m_last_error;
}

bool irs::hfftp::various_page_mem_file_write_only_t::max_size_is_known() const
{
  return true;
}

irs::hfftp::various_page_mem_file_write_only_t::size_type
irs::hfftp::various_page_mem_file_write_only_t::size() const
{
  return m_size;
}

irs::hfftp::various_page_mem_file_write_only_t::size_type
irs::hfftp::various_page_mem_file_write_only_t::max_size() const
{
  return m_max_size;
}

bool irs::hfftp::various_page_mem_file_write_only_t::save_size_enabled() const
{
  return m_save_size_enabled;
}

void irs::hfftp::various_page_mem_file_write_only_t::save_size()
{
  if ((m_last_error == irs::hfftp::file_error_no_error) &&
    (m_process == process_wait_command) &&
    (mp_various_page_mem->status() == irs_st_ready) &&
    m_save_size_enabled &&
    !m_size_saved) {
    file_size_t file_size = static_cast<file_size_type>(m_size);
    m_buf.resize(sizeof(file_size_type));
    *reinterpret_cast<file_size_t*>(irs::vector_data(m_buf)) = file_size;
    //memcpyex(irs::vector_data(m_buf), reinterpret_cast<irs_u8*>(&file_size),
      //m_buf.size());
    //irs_u8* pos = mp_various_page_mem->page_begin(m_first_page_index);
    mp_various_page_mem->write(mp_begin, irs::vector_data(m_buf), m_buf.size());
    m_process = process_save_size;
  }
}

bool irs::hfftp::various_page_mem_file_write_only_t::size_saved() const
{
  return m_size_saved;
}

irs_status_t irs::hfftp::various_page_mem_file_write_only_t::status() const
{
  if ((m_last_error == irs::hfftp::file_error_no_error) &&
    (m_process == process_wait_command)) {
    return irs_st_ready;
  } else if (m_last_error != irs::hfftp::file_error_no_error) {
    return irs_st_error;
  } else {
    return irs_st_busy;
  }
}

void irs::hfftp::various_page_mem_file_write_only_t::tick()
{
  if (m_process != process_wait_command) {
    if (mp_various_page_mem->status() == irs_st_error) {
      m_size = 0;
      m_last_error = file_error_unknown;
      m_process = process_wait_command;
    }
  }
  if (mp_various_page_mem->status() == irs_st_ready) {
    switch (m_process) {
      case process_erase: {
        if (m_current_page_index <= m_last_page_index) {
          mp_various_page_mem->erase_page(m_current_page_index);
          m_current_page_index++;
        } else {
          m_process = process_wait_command;
        }
      } break;
      case process_write: {
        mp_pos += m_buf.size();
        m_size += m_buf.size();
        m_process = process_wait_command;
      } break;
      case process_save_size: {
        m_size_saved = true;
        m_process = process_wait_command;
      } break;
      case process_wait_command: {
        // Ожидаем команд
      } break;
    }
  }
}

#ifndef IRS_MICROCONTROLLER

// class fstream_files_t
irs::hfftp::fstream_files_t::fstream_files_t():
  m_files()
{
}

irs::hfftp::file_read_only_t* irs::hfftp::fstream_files_t::open_for_read(
  const string_type& a_file_name,
  file_error_t* ap_error_code)
{
  if (m_files.find(a_file_name) == m_files.end()) {
    file_read_only_t* file = new ifstream_file_t(a_file_name);
    m_files.insert(make_pair(a_file_name, file));
    *ap_error_code = file_error_no_error;
    return file;
  } else {
    *ap_error_code = file_error_access_violation;
  }
  return IRS_NULL;
}

irs::hfftp::file_write_only_t* irs::hfftp::fstream_files_t::open_for_write(
  const string_type& a_file_name,
  file_error_t* ap_error_code)
{
  if (m_files.find(a_file_name) == m_files.end()) {
    file_write_only_t* file = new ofstream_file_t(a_file_name);
    m_files.insert(make_pair(a_file_name, file));
    *ap_error_code = file_error_no_error;
    return file;
  } else {
    *ap_error_code = file_error_access_violation;
  }
  return IRS_NULL;
}

void irs::hfftp::fstream_files_t::close(file_base_t* ap_file)
{
  files_map_type::iterator it = m_files.begin();
  bool file_not_found = true;
  while (it != m_files.end()) {
    if (it->second.get() == ap_file) {
      file_not_found = false;
      m_files.erase(it);
      break;
    }
    ++it;
  }
  if (file_not_found) {
    throw std::logic_error(
      "Указанный файл не был открыт этим экземпляром.");
  }
}

#endif // !IRS_MICROCONTROLLER

const std::size_t irs::hfftp::data_min_size = 10;
const double irs::hfftp::min_timeout = 0.001;

namespace {

char packet_id[] = {'h', 'f', 'f', 't', 'p'};

} // unnamed namespace

// class field_data_t
irs::hfftp::field_data_t::field_data_t(vector<irs_u8> *ap_data):
  mp_data(ap_data)
{
}

bool irs::hfftp::field_data_t::read(const vector<irs_u8> &a_buf,
  size_type *ap_pos)
{
  if ((a_buf.size() - *ap_pos) < sizeof(data_size_t)) {
    return false;
  }
  const data_size_t data_size =
    *reinterpret_cast<const data_size_t*>(vector_data(a_buf, *ap_pos));
  (*ap_pos) += sizeof(data_size_t);
  if ((a_buf.size() - *ap_pos) < static_cast<size_type>(data_size)) {
    return false;
  }
  mp_data->resize(data_size);
  memcpyex(reinterpret_cast<irs_u8*>(vector_data(*mp_data)),
    vector_data(a_buf, *ap_pos), mp_data->size());
  (*ap_pos) += data_size;
  return true;
}

void irs::hfftp::field_data_t::push_back(vector<irs_u8>* ap_buf)
{
  size_type pos = ap_buf->size();
  ap_buf->resize(ap_buf->size() + sizeof(data_size_t));
  *reinterpret_cast<data_size_t*>(vector_data(*ap_buf, pos)) =
    static_cast<data_size_t>(mp_data->size());
  pos = ap_buf->size();
  ap_buf->resize(ap_buf->size() + mp_data->size());
  memcpyex(reinterpret_cast<irs_u8*>(vector_data(*ap_buf, pos)),
    vector_data(*mp_data), mp_data->size());
}

// class field_string_t
irs::hfftp::field_string_t::field_string_t(string_type *ap_string):
  mp_string(ap_string)
{
}

bool irs::hfftp::field_string_t::read(const vector<irs_u8>& a_buf,
  size_type *ap_pos)
{
  vector<irs_u8> buf;
  field_data_t field_data(&buf);
  bool success = field_data.read(a_buf, ap_pos);
  if (success) {
    std::string s(reinterpret_cast<char*>(vector_data(buf)), buf.size());
    *mp_string = irs::str_conv<string_type>(s);
  }
  return success;
}

void irs::hfftp::field_string_t::push_back(vector<irs_u8>* ap_buf)
{
  const std::string s = irs::str_conv<std::string>(*mp_string);
  vector<irs_u8> buf;
  buf.resize(s.size());
  memcpyex(reinterpret_cast<irs_u8*>(vector_data(buf)),
    reinterpret_cast<const irs_u8*>(s.c_str()), s.size());
  field_data_t field_data(&buf);
  field_data.push_back(ap_buf);
}


irs::handle_t<irs::hfftp::option_value_t>
irs::hfftp::make_option_value(option_name_t a_option_name)
{
  handle_t<option_value_t> option_value;
  switch (a_option_name) {
    case option_get_file_size: {
      break;
    }
    case option_file_size: {
      option_value.reset(new option_value_simple_type_t<file_size_t>());
    } break;
  }
  return option_value;
}

// class field_options_t
irs::hfftp::field_options_t::field_options_t(
  std::map<option_name_t, handle_t<option_value_t> >* ap_options
):
  mp_options(ap_options)
{
}

bool irs::hfftp::field_options_t::read(const vector<irs_u8>& a_buf,
  size_type* ap_pos)
{
  mp_options->clear();
  option_count_t options_count = 0;
  field_simple_type_t<option_count_t> field_option_count(&options_count);
  if (!field_option_count.read(a_buf, ap_pos)) {
    return false;
  }
  for (size_type i = 0; i < static_cast<size_type>(options_count); i++) {
    option_code_t option_code;
    field_simple_type_t<option_code_t> field_option_name(&option_code);
    if (!field_option_name.read(a_buf, ap_pos)) {
      return false;
    }
    option_name_t option_name = static_cast<option_name_t>(option_code);
    handle_t<option_value_t> option_value = make_option_value(option_name);
    if (!option_value.is_empty()) {
      irs::handle_t<field_t> field = option_value->make_field();
      if (!field->read(a_buf, ap_pos)) {
        return false;
      }
    }
    mp_options->insert(std::make_pair(option_name, option_value));
  }
  return true;
}

void irs::hfftp::field_options_t::push_back(vector<irs_u8>* ap_buf)
{
  size_type pos = ap_buf->size();
  ap_buf->resize(ap_buf->size() + sizeof(option_count_t));
  const option_count_t count = static_cast<option_count_t>(mp_options->size());
  *reinterpret_cast<option_count_t*>(vector_data(*ap_buf, pos)) = count;
  std::map<option_name_t, handle_t<option_value_t> >::const_iterator it =
    mp_options->begin();
  while (it != mp_options->end()) {
    pos = ap_buf->size();
    ap_buf->resize(ap_buf->size() + sizeof(option_code_t));
    const option_name_t name = it->first;
    *reinterpret_cast<option_code_t*>(vector_data(*ap_buf, pos)) =
      static_cast<option_code_t>(name);
    if (!it->second.is_empty()) {
      irs::handle_t<field_t> field = it->second->make_field();
      field->push_back(ap_buf);
    }
    ++it;
  }
}

// class fields_flow_t
irs::hfftp::fields_flow_t::fields_flow_t(opcode_t a_opcode,
  irs::hardflow_t* ap_hardflow, size_type a_channel
):
  m_opcode(a_opcode),
  mp_hardflow(ap_hardflow),
  m_fixed_flow(ap_hardflow),
  m_channel(a_channel),
  m_packet_max_size(1400),
  m_status(irs_st_ready),
  m_process(process_wait_command),
  m_buf(),
  m_fields(),
  m_current_field(0),
  m_packet_crc32()
{
  // Выставляем максимальный тайм-аут, так как нам не нужно,
  //   чтобы он срабатывал
  m_fixed_flow.write_timeout(std::numeric_limits<counter_t>::max());
  m_fixed_flow.read_timeout(std::numeric_limits<counter_t>::max());

}

void irs::hfftp::fields_flow_t::set_channel(size_type a_channel)
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_channel = a_channel;
}

void irs::hfftp::fields_flow_t::add_string(string_type* ap_string)
{
  irs::handle_t<field_t> field(new field_string_t(ap_string));
  m_fields.push_back(field);
}

void irs::hfftp::fields_flow_t::add_data(vector<irs_u8>* ap_data)
{
  irs::handle_t<field_t> field(new field_data_t(ap_data));
  m_fields.push_back(field);
}

void irs::hfftp::fields_flow_t::erase_fields()
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_fields.clear();
}

void irs::hfftp::fields_flow_t::read()
{
  vector<irs_u8> buf;
  push_back_id_and_opcode(&buf);
  m_packet_crc32.reset();
  m_packet_crc32.put(vector_data(buf), buf.size());

  m_buf.resize(sizeof(packet_size_t));
  m_fixed_flow.read(m_channel, vector_data(m_buf), m_buf.size());
  m_status = irs_st_busy;
  m_process = process_read_packet_size;
}

void irs::hfftp::fields_flow_t::write()
{
  m_buf.clear();
  const size_type pos = m_buf.size();
  m_buf.resize(m_buf.size() + IRS_ARRAYSIZE(packet_id));
  memcpyex(reinterpret_cast<char*>(vector_data(m_buf, pos)), packet_id,
    IRS_ARRAYSIZE(packet_id));
  field_simple_type_t<opcode_t> opcode_field(&m_opcode);
  opcode_field.push_back(&m_buf);

  packet_size_t packet_size = 0;
  const size_type size_pos = m_buf.size();
  field_simple_type_t<packet_size_t> packet_size_field(&packet_size);
  packet_size_field.push_back(&m_buf);

  irs_u32 crc32 = 0;
  const size_type crc_pos = m_buf.size();
  field_simple_type_t<irs_u32> crc32_field(&crc32);
  crc32_field.push_back(&m_buf);

  for (size_type i = 0; i < m_fields.size(); i++) {
    m_fields[i]->push_back(&m_buf);
  }
  packet_size = m_buf.size();
  packet_size_field.write(&m_buf, size_pos);
  crc32 = crc32_table(vector_data(m_buf), m_buf.size());
  crc32_field.write(&m_buf, crc_pos);

  m_fixed_flow.write(m_channel, vector_data(m_buf), m_buf.size());
  m_status = irs_st_busy;
  m_process = process_write;
}

void irs::hfftp::fields_flow_t::push_back_id_and_opcode(vector<irs_u8>* ap_buf)
{
  const size_type pos = ap_buf->size();
  ap_buf->resize(ap_buf->size() + IRS_ARRAYSIZE(packet_id));
  memcpyex(reinterpret_cast<char*>(vector_data(*ap_buf, pos)), packet_id,
    IRS_ARRAYSIZE(packet_id));
  field_simple_type_t<opcode_t> opcode_field(&m_opcode);
  opcode_field.push_back(ap_buf);
}

irs_status_t irs::hfftp::fields_flow_t::status() const
{
  return m_status;
}

void irs::hfftp::fields_flow_t::abort()
{
  m_fixed_flow.write_abort();
  m_fixed_flow.read_abort();
  m_status = irs_st_ready;
}

void irs::hfftp::fields_flow_t::set_data_max_size(data_size_t a_data_max_size)
{
  const size_type file_name_min_size = 1;
  const size_type packet_request_min_size = IRS_ARRAYSIZE(packet_id) +
    sizeof(opcode_t) + sizeof(packet_size_t) +sizeof(crc32_t) +
    sizeof(block_index_t) + sizeof(data_size_t) + sizeof(timeout_t) +
    sizeof(retransmit_count_t) + file_name_min_size;


  const size_type packet_data_header_size = IRS_ARRAYSIZE(packet_id) +
    sizeof(opcode_t) + sizeof(packet_size_t) + sizeof(crc32_t) +
    sizeof(block_index_t) + sizeof(data_size_t);

  m_packet_max_size = std::max(packet_request_min_size,
    packet_data_header_size + a_data_max_size);
}

void irs::hfftp::fields_flow_t::tick()
{
  m_fixed_flow.tick();
  switch (m_process) {
    case process_write: {
      if (m_fixed_flow.write_status() == fixed_flow_type::status_success) {
        m_status = irs_st_ready;
        m_process = process_wait_command;
      }
    } break;
    case process_read_packet_size: {
      if (m_fixed_flow.read_status() ==
        irs::hardflow::fixed_flow_t::status_success) {
        m_packet_crc32.put(vector_data(m_buf), sizeof(packet_size_t));
        packet_size_t size = 0;
        field_simple_type_t<packet_size_t> size_field(&size);

        const size_type header_size = IRS_ARRAYSIZE(packet_id) +
          sizeof(opcode_t) + sizeof(packet_size_t) /*+ sizeof(irs_u32)*/;
        size_type pos = 0;
        const bool read_size_status = size_field.read(m_buf, &pos);
        if (read_size_status && (size >= header_size) &&
          (size <= m_packet_max_size)) {
          m_buf.resize(size - header_size);
          m_fixed_flow.read(m_channel, vector_data(m_buf), m_buf.size());
          m_process = process_read;
        } else {
          IRS_LIB_HFFTP_DBG_MSG_DETAIL("Пакет имеет недопустимый размер");
          m_status = irs_st_error;
          m_process = process_wait_command;
        }
      }
    } break;
    case process_read: {
      if (m_fixed_flow.read_status() ==
        irs::hardflow::fixed_flow_t::status_success) {
        size_type pos = 0;
        irs_u32 crc32 = 0;
        irs_u32 crc32_helper = 0;
        field_simple_type_t<irs_u32> crc32_field(&crc32_helper);
        crc32_field.read(m_buf, &pos);
        crc32 = crc32_helper;
        crc32_helper = 0;
        crc32_field.write(&m_buf, 0);
        m_packet_crc32.put(vector_data(m_buf), m_buf.size());
        if (crc32 == m_packet_crc32.get_crc()) {
          for (size_type i = 0; i < m_fields.size(); i++) {
            if (!m_fields[i]->read(m_buf, &pos)) {
              m_status = irs_st_error;
              m_process = process_wait_command;
              break;
            }
          }
          m_status = irs_st_ready;
        } else {
          IRS_LIB_HFFTP_DBG_MSG_DETAIL("Не сходится контрольная сумма пакета");
          m_status = irs_st_error;
        }
        m_process = process_wait_command;
      }
    } break;
    case process_wait_command: {
      // Ожидание команды
    } break;
  }
}

// class packet_t
irs::hfftp::packet_t::packet_t(packet_type_t a_packet_type,
  irs::hardflow_t* ap_hardflow
):
  m_packet_type(a_packet_type),
  m_status(irs_st_ready),
  m_process(process_wait_command),
  m_channel(),
  m_fields_flow(a_packet_type, ap_hardflow, 0)
{
}

irs::hfftp::packet_type_t irs::hfftp::packet_t::get_type() const
{
  return m_packet_type;
}

irs::hfftp::fields_flow_t* irs::hfftp::packet_t::fields_flow()
{
  return &m_fields_flow;
}

void irs::hfftp::packet_t::read(size_type a_channel)
{
  m_fields_flow.set_channel(a_channel);
  m_channel = a_channel;
  m_fields_flow.read();
  m_status = irs_st_busy;
  m_process = process_read_or_write;
}

void irs::hfftp::packet_t::write(size_type a_channel)
{
  m_fields_flow.set_channel(a_channel);
  m_fields_flow.write();
  m_status = irs_st_busy;
  m_process = process_read_or_write;
}

void irs::hfftp::packet_t::abort()
{
  m_fields_flow.abort();
  m_status = irs_st_ready;
  m_process = process_wait_command;
}

void irs::hfftp::packet_t::tick()
{
  m_fields_flow.tick();
  switch (m_process) {
    case process_read_or_write: {
      if (m_fields_flow.status() == irs_st_ready) {
        m_status = irs_st_ready;
        m_process = process_wait_command;
      }
    } break;
    case process_wait_command: {
      // Ожидаем команд
    } break;
  }
}

irs_status_t irs::hfftp::packet_t::status() const
{
  return m_status;
}

void irs::hfftp::packet_t::set_data_max_size(data_size_t a_data_max_size)
{
  m_fields_flow.set_data_max_size(a_data_max_size);
}

// class packet_read_request_t
irs::hfftp::packet_read_request_t::packet_read_request_t(
  irs::hardflow_t* ap_hardflow
):
  packet_t(pt_read_request, ap_hardflow),
  m_fields()
{
  fields_flow()->add_string(&m_fields.file_name);
  fields_flow()->add_simple_type<data_size_t>(&m_fields.data_max_size);
  fields_flow()->add_simple_type<timeout_t>(&m_fields.timeout_ms);
  fields_flow()->add_simple_type<retransmit_count_t>(
    &m_fields.retransmit_count);
}

const irs::hfftp::packet_read_request_t::fields_t&
irs::hfftp::packet_read_request_t::get_fields() const
{
  return m_fields;
}

void irs::hfftp::packet_read_request_t::set_fields(const fields_t& a_fields)
{
  m_fields = a_fields;
}

// class packet_write_request_t
irs::hfftp::packet_write_request_t::packet_write_request_t(
  irs::hardflow_t* ap_hardflow
):
  packet_t(pt_write_request, ap_hardflow),
  m_fields()
{
  fields_flow()->add_string(&m_fields.file_name);
  fields_flow()->add_simple_type<bool_t>(&m_fields.file_size_is_known);
  fields_flow()->add_simple_type<file_size_t>(&m_fields.file_size);
  fields_flow()->add_simple_type<data_size_t>(&m_fields.data_max_size);
  fields_flow()->add_simple_type<timeout_t>(&m_fields.timeout_ms);
  fields_flow()->add_simple_type<retransmit_count_t>(
    &m_fields.retransmit_count);
}

const irs::hfftp::packet_write_request_t::fields_t&
irs::hfftp::packet_write_request_t::get_fields() const
{
  return m_fields;
}

void irs::hfftp::packet_write_request_t::set_fields(const fields_t& a_fields)
{
  m_fields = a_fields;
}

// class packet_data_t
irs::hfftp::packet_data_t::packet_data_t(irs::hardflow_t* ap_hardflow):
  packet_t(pt_data, ap_hardflow),
  m_fields()
{
  fields_flow()->add_simple_type<block_index_t>(&m_fields.block_index);
  fields_flow()->add_data(&m_fields.data);
}

const irs::hfftp::packet_data_t::fields_t&
irs::hfftp::packet_data_t::get_fields() const
{
  return m_fields;
}

void irs::hfftp::packet_data_t::set_fields(const fields_t& a_fields)
{
  m_fields = a_fields;
}

// class packet_acknowledgment_t
irs::hfftp::packet_acknowledgment_t::packet_acknowledgment_t(
  irs::hardflow_t* ap_hardflow
):
  packet_t(pt_acknowledgment, ap_hardflow),
  m_fields()
{
  fields_flow()->add_simple_type<block_index_t>(&m_fields.block_index);
}

const irs::hfftp::packet_acknowledgment_t::fields_t&
irs::hfftp::packet_acknowledgment_t::get_fields() const
{
  return m_fields;
}

void irs::hfftp::packet_acknowledgment_t::set_fields(
  const fields_t& a_fields)
{
  m_fields = a_fields;
}

// class packet_acknowledgment_options_t
irs::hfftp::packet_acknowledgment_options_t::packet_acknowledgment_options_t(
  irs::hardflow_t* ap_hardflow
):
  packet_t(pt_acknowledgment_options, ap_hardflow),
  m_fields()
{
  fields_flow()->add_simple_type<bool_t>(&m_fields.file_size_is_known);
  fields_flow()->add_simple_type<file_size_t>(&m_fields.file_size);
  fields_flow()->add_simple_type<data_size_t>(&m_fields.data_max_size);
  fields_flow()->add_simple_type<timeout_t>(&m_fields.timeout_ms);
  fields_flow()->add_simple_type<retransmit_count_t>(
    &m_fields.retransmit_count);
}

const irs::hfftp::packet_acknowledgment_options_t::fields_t&
irs::hfftp::packet_acknowledgment_options_t::get_fields() const
{
  return m_fields;
}

void irs::hfftp::packet_acknowledgment_options_t::set_fields(
  const fields_t& a_fields)
{
  m_fields = a_fields;
}

// class packet_error_t
irs::hfftp::packet_error_t::packet_error_t(
  irs::hardflow_t* ap_hardflow
):
  packet_t(pt_error, ap_hardflow),
  m_fields()
{
  fields_flow()->add_simple_type<error_code_t>(&m_fields.error_code);
  fields_flow()->add_string(&m_fields.error_message);
}

const irs::hfftp::packet_error_t::fields_t&
irs::hfftp::packet_error_t::get_fields() const
{
  return m_fields;
}

void irs::hfftp::packet_error_t::set_fields(
  const fields_t& a_fields)
{
  m_fields = a_fields;
}

// class packet_flow_t
irs::hfftp::packet_flow_t::packet_flow_t(irs::hardflow_t* ap_hardflow):
  mp_hardflow(ap_hardflow),
  m_fixed_flow(ap_hardflow),
  mp_send_packet(IRS_NULL),
  mp_receive_packet(IRS_NULL),
  m_send_timeout(make_cnt_s(3)),
  m_send_timeout_enabled(false),
  m_receive_timeout(make_cnt_s(7)),
  m_receive_timeout_enabled(false),
  m_retransmit_count(0),
  m_status(irs_st_ready),
  m_write_buf(),
  m_read_buf(),
  m_channel(0),
  m_process(process_wait_command),
  m_data_max_size(512)
{
  // Выставляем максимальный тайм-аут, так как нам не нужно, чтобы он срабатывал
  m_fixed_flow.read_timeout(std::numeric_limits<counter_t>::max());
}

void irs::hfftp::packet_flow_t::send_read_request(size_type a_channel,
  const packet_read_request_t::fields_t& a_fields)
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_retransmit_count = 0;
  irs::hfftp::packet_read_request_t* packet =
    new packet_read_request_t(mp_hardflow);
  packet->set_fields(a_fields);
  mp_send_packet.reset(packet);
  mp_send_packet->write(a_channel);
  send_timeout_start(m_send_timeout_enabled);
  m_status = irs_st_busy;
  m_process = process_wait_send_packet;
}

void irs::hfftp::packet_flow_t::send_write_request(size_type a_channel,
  const packet_write_request_t::fields_t& a_fields)
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_retransmit_count = 0;
  packet_write_request_t* packet = new packet_write_request_t(mp_hardflow);
  packet->set_fields(a_fields);
  mp_send_packet.reset(packet);
  mp_send_packet->write(a_channel);
  send_timeout_start(m_send_timeout_enabled);
  m_status = irs_st_busy;
  m_process = process_wait_send_packet;
}

void irs::hfftp::packet_flow_t::send_data(size_type a_channel,
  block_index_t a_block_index,
  const vector<irs_u8>& a_data)
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_retransmit_count = 0;
  packet_data_t* packet = new packet_data_t(mp_hardflow);
  packet_data_t::fields_t fields;
  fields.data = a_data;
  fields.block_index = a_block_index;
  packet->set_fields(fields);
  mp_send_packet.reset(packet);
  mp_send_packet->write(a_channel);
  send_timeout_start(m_send_timeout_enabled);
  m_status = irs_st_busy;
  m_process = process_wait_send_packet;
}

void irs::hfftp::packet_flow_t::send_acknowledgment(size_type a_channel,
  size_type a_block_index)
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_retransmit_count = 0;
  packet_acknowledgment_t* packet = new packet_acknowledgment_t(mp_hardflow);
  packet_acknowledgment_t::fields_t fields;
  fields.block_index = a_block_index;
  packet->set_fields(fields);
  mp_send_packet.reset(packet);
  mp_send_packet->write(a_channel);
  send_timeout_start(m_send_timeout_enabled);
  m_status = irs_st_busy;
  m_process = process_wait_send_packet;
}

void irs::hfftp::packet_flow_t::send_acknowledgment_options(size_type a_channel,
  const packet_acknowledgment_options_t::fields_t& a_fields)
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_retransmit_count = 0;
  packet_acknowledgment_options_t* packet =
    new packet_acknowledgment_options_t(mp_hardflow);
  packet->set_fields(a_fields);
  mp_send_packet.reset(packet);
  mp_send_packet->write(a_channel);
  send_timeout_start(m_send_timeout_enabled);
  m_status = irs_st_busy;
  m_process = process_wait_send_packet;
}

void irs::hfftp::packet_flow_t::send_error(size_type a_channel,
  error_t a_error,
  const string_type& a_error_message)
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  m_retransmit_count = 0;
  packet_error_t* packet = new packet_error_t(mp_hardflow);
  packet_error_t::fields_t fields;
  fields.error_code = a_error;
  fields.error_message = a_error_message;
  packet->set_fields(fields);
  mp_send_packet.reset(packet);
  mp_send_packet->write(a_channel);
  send_timeout_start(m_send_timeout_enabled);
  m_status = irs_st_busy;
  m_process = process_wait_send_packet;
}

void irs::hfftp::packet_flow_t::retransmit_last_packet(size_type a_channel)
{
  IRS_LIB_ASSERT(m_status != irs_st_busy);
  IRS_LIB_ASSERT(!mp_send_packet.is_empty());
  m_retransmit_count++;
  mp_send_packet->write(a_channel);
  send_timeout_start(m_send_timeout_enabled);
  m_status = irs_st_busy;
  m_process = process_wait_send_packet;
}

irs::hfftp::packet_flow_t::size_type
irs::hfftp::packet_flow_t::get_retransmit_count() const
{
  return m_retransmit_count;
}

void irs::hfftp::packet_flow_t::receive_packet(size_type a_channel)
{
  m_channel = a_channel;
  receive_timeout_start(m_receive_timeout_enabled);
  m_status = irs_st_busy;
  m_process = process_find_packet_begin;
}

void irs::hfftp::packet_flow_t::receive_packet_without_reset_timeout(
  size_type a_channel)
{
  m_channel = a_channel;
  m_status = irs_st_busy;
  m_process = process_find_packet_begin;
}

irs::hfftp::packet_t* irs::hfftp::packet_flow_t::get_packet()
{
  return mp_receive_packet.get();
}

void irs::hfftp::packet_flow_t::set_data_max_size(size_type a_max_size)
{
  m_data_max_size = a_max_size;
}

irs_status_t irs::hfftp::packet_flow_t::status() const
{
  return m_status;
}

void irs::hfftp::packet_flow_t::enable_send_timeout(double a_seconds)
{
  m_send_timeout.set(make_cnt_s(a_seconds));
  m_send_timeout_enabled = true;
}

void irs::hfftp::packet_flow_t::disable_send_timeout()
{
  m_send_timeout_enabled = false;
}

void irs::hfftp::packet_flow_t::enable_receive_timeout(double a_seconds)
{
  m_receive_timeout.set(make_cnt_s(a_seconds));
  m_receive_timeout_enabled = true;
}

void irs::hfftp::packet_flow_t::disable_receive_timeout()
{
  m_receive_timeout_enabled = false;
}

void irs::hfftp::packet_flow_t::abort()
{
  if (!mp_send_packet.is_empty()) {
    mp_send_packet->abort();
  }
  if (!mp_receive_packet.is_empty()) {
    mp_receive_packet->abort();
  }
  m_status = irs_st_ready;
  m_process = process_wait_command;
}

void irs::hfftp::packet_flow_t::tick()
{
  m_fixed_flow.tick();
  if (!mp_send_packet.is_empty()) {
    mp_send_packet->tick();
  }
  if (!mp_receive_packet.is_empty()) {
    mp_receive_packet->tick();
  }
  switch (m_process) {
    case process_wait_send_packet: {
      if (mp_send_packet->status() == irs_st_ready) {
        m_status = irs_st_ready;
        m_process = process_wait_command;
      } else if (m_send_timeout.check() ||
        (mp_send_packet->status() == irs_st_error)) {
        mp_send_packet->abort();
        m_status = irs_st_error;
        m_process = process_wait_command;
      }
    } break;
    case process_find_packet_begin: {
      IRS_LIB_ASSERT(m_read_buf.size() < IRS_ARRAYSIZE(packet_id));
      size_type pos = m_read_buf.size();
      m_read_buf.resize(IRS_ARRAYSIZE(packet_id));
      const size_type size = mp_hardflow->read(m_channel,
        vector_data(m_read_buf, pos), m_read_buf.size() - pos);
      m_read_buf.resize(pos + size);
      bool equal = false;
      if (m_read_buf.size() == IRS_ARRAYSIZE(packet_id)) {
        if (memcmp(vector_data(m_read_buf), packet_id,
          m_read_buf.size()) == 0) {
          equal = true;
        }
      }
      if (equal) {
        m_read_buf.resize(sizeof(opcode_t));
        m_fixed_flow.read(m_channel, vector_data(m_read_buf),
          m_read_buf.size());
        m_process = process_wait_opcode;
      } else if (m_receive_timeout.check()) {
        m_status = irs_st_error;
        m_process = process_wait_command;
      } else {
        if (m_read_buf.size() == IRS_ARRAYSIZE(packet_id)) {
          m_read_buf.erase(m_read_buf.begin());
        }
      }
    } break;
    case process_wait_opcode: {
      if (m_fixed_flow.read_status() == fixed_flow_type::status_success) {
        const opcode_t opcode =
          *reinterpret_cast<opcode_t*>(vector_data(m_read_buf));
        make_packet(opcode);
        m_read_buf.clear();
        mp_receive_packet->read(m_channel);
        m_process = process_receive_answer;
      } else if (m_receive_timeout.check() ||
        (m_fixed_flow.read_status() == fixed_flow_type::status_error)) {
        m_fixed_flow.read_abort();
        m_status = irs_st_error;
        m_process = process_wait_command;
        //mp_hardflow->read(m_channel, &m_read_buf.front(), m_read_buf.size());
      }
    } break;
    case process_receive_answer: {
      if (mp_receive_packet->status() == irs_st_ready) {
        m_status = irs_st_ready;
        m_process = process_wait_command;
      } else if (m_receive_timeout.check() ||
        (mp_receive_packet->status() == irs_st_error)) {
        mp_receive_packet->abort();
        m_status = irs_st_error;
        m_process = process_wait_command;
      }
    } break;
    case process_wait_command: {
      // Ожидание команд
    } break;
  }
}

void irs::hfftp::packet_flow_t::make_packet(opcode_t a_opcode)
{
  switch (a_opcode) {
    case pt_read_request: {
      mp_receive_packet.reset(new packet_read_request_t(mp_hardflow));
    } break;
    case pt_write_request: {
      mp_receive_packet.reset(new packet_write_request_t(mp_hardflow));
    } break;
    case pt_data: {
      mp_receive_packet.reset(new packet_data_t(mp_hardflow));
    } break;
    case pt_acknowledgment: {
      mp_receive_packet.reset(new packet_acknowledgment_t(mp_hardflow));
    } break;
    case pt_acknowledgment_options: {
      mp_receive_packet.reset(new packet_acknowledgment_options_t(mp_hardflow));
    } break;
    case pt_error: {
      mp_receive_packet.reset(new packet_error_t(mp_hardflow));
    } break;
  }
  if (!mp_receive_packet.is_empty()) {
    mp_receive_packet->set_data_max_size(
      static_cast<data_size_t>(m_data_max_size));
  }
}

void irs::hfftp::packet_flow_t::send_timeout_start(bool a_enabled)
{
  if (a_enabled) {
    m_send_timeout.start();
  } else {
    m_send_timeout.stop();
  }
}

void irs::hfftp::packet_flow_t::receive_timeout_start(bool a_enabled)
{
  if (a_enabled) {
    m_receive_timeout.start();
  } else {
    m_receive_timeout.stop();
  }
}

irs::hfftp::timeout_t irs::hfftp::timeout_to_timeout_ms(double a_timeout)
{
  return static_cast<timeout_t>(irs::bound<double>(
    a_timeout*1000., 1,
    std::numeric_limits<timeout_t>::max()));
}

double irs::hfftp::timeout_ms_to_timeout(timeout_t a_timeout_ms)
{
  return a_timeout_ms/1000.;
}

// class client_t
irs::hfftp::client_t::client_t(irs::hardflow_t* ap_transport,
  const configuration_type& a_configuration
):
  mp_transport(ap_transport),
  m_configuration(normalize_configuration(a_configuration)),
  m_protocol_config(extract_protocol_config(m_configuration)),
  m_connection_is_established(false),
  m_packet_flow(ap_transport),
  m_retransmit_count(0),
  m_send_timeout(7),
  m_receive_timeout(7),
  m_channel(ap_transport->channel_next()),
  m_read_buf(),
  m_write_buf(),
  m_buf_pos(0),
  m_block_index(0),
  mp_file_write_only(IRS_NULL),
  mp_file_read_only(IRS_NULL),
  m_remote_file_name(),
  m_process(process_wait_command),
  m_mode(mode_write),
  m_status(irs_st_ready),
  m_last_error(error_no_error),
  m_last_error_message(),
  m_file_size_is_known(false),
  m_file_size(0),
  m_transferred_bytes(0),
  m_timeout_timer(make_cnt_s(m_protocol_config.timeout)),
  m_changed_event()
{
  m_packet_flow.disable_receive_timeout();
  m_packet_flow.disable_send_timeout();
  m_packet_flow.set_data_max_size(m_protocol_config.data_size);
}

void irs::hfftp::client_t::get(const string_type& a_remote_file_name,
  file_write_only_t* ap_file)
{
  if (m_status != irs_st_busy) {
    IRS_LIB_ASSERT(ap_file);
    m_connection_is_established = false;
    m_remote_file_name = a_remote_file_name;
    mp_file_write_only = ap_file;
    m_protocol_config = extract_protocol_config(m_configuration);
    m_packet_flow.set_data_max_size(m_protocol_config.data_size);
    m_transferred_bytes = 0;
    m_block_index = 0;
    m_file_size_is_known = false;
    m_file_size = 0;
    m_status = irs_st_busy;
    m_mode = mode_write;
    protocol_configuration_t protocol_configuration;

    irs::hfftp::packet_read_request_t::fields_t fields;
    fields.file_name = m_remote_file_name;
    fields.data_max_size =
      static_cast<data_size_t>(m_configuration.data_size);
    fields.timeout_ms = timeout_to_timeout_ms(m_configuration.timeout);
    fields.retransmit_count =
      static_cast<retransmit_count_t>(m_configuration.retransmit_max_count);

    m_packet_flow.send_read_request(m_channel, fields);
    m_retransmit_count = 0;
    m_timeout_timer.set(make_cnt_s(m_protocol_config.timeout));
    m_timeout_timer.start();
    IRS_LIB_HFFTP_DBG_MSG_BASE("Отправляем запрос на чтение файла " <<
      irs::str_conv<std::string>(m_remote_file_name));
    exec_changed_event();
    set(process_wait_send_request);
  } else {
    IRS_ASSERT_MSG("Еще не завершена предыдущая операция");
  }
}

void irs::hfftp::client_t::put(const string_type& a_remote_file_name,
  file_read_only_t* ap_file)
{
  if (m_status != irs_st_busy) {
    IRS_LIB_ASSERT(ap_file);
    m_connection_is_established = false;
    m_remote_file_name = a_remote_file_name;
    mp_file_read_only = ap_file;
    m_protocol_config = extract_protocol_config(m_configuration);
    m_packet_flow.set_data_max_size(m_protocol_config.data_size);
    m_block_index = 0;
    m_transferred_bytes = 0;
    m_file_size_is_known = mp_file_read_only->size_is_known();
    m_file_size = m_file_size_is_known ? mp_file_read_only->size() : 0;
    m_status = irs_st_busy;
    m_mode = mode_read;
    packet_write_request_t::fields_t fields;
    fields.file_name = m_remote_file_name;
    fields.file_size_is_known = m_file_size_is_known;
    fields.file_size = m_file_size;
    fields.data_max_size = static_cast<data_size_t>(
      m_protocol_config.data_size);
    fields.timeout_ms = timeout_to_timeout_ms(m_protocol_config.timeout);
    fields.retransmit_count = static_cast<retransmit_count_t>(
      m_protocol_config.retransmit_max_count);
    m_packet_flow.send_write_request(m_channel, fields);
    m_retransmit_count = 0;
    m_timeout_timer.set(make_cnt_s(m_protocol_config.timeout));
    m_timeout_timer.start();
    IRS_LIB_HFFTP_DBG_MSG_BASE("Отправляем запрос на запись файла " <<
      irs::str_conv<std::string>(m_remote_file_name));
    exec_changed_event();
    set(process_wait_send_request);
  } else {
    IRS_ASSERT_MSG("Еще не завершена предыдущая операция");
  }
}

bool irs::hfftp::client_t::connection_is_established() const
{
  return m_connection_is_established;
}

const irs::hfftp::protocol_configuration_t&
irs::hfftp::client_t::get_protocol_configuration() const
{
  return m_protocol_config;
}

irs_status_t irs::hfftp::client_t::status() const
{
  return m_status;
}

irs::hfftp::client_t::size_type
irs::hfftp::client_t::transferred_bytes() const
{
  return m_transferred_bytes;
}

irs::hfftp::client_t::size_type
irs::hfftp::client_t::file_size_is_known() const
{
  return m_file_size_is_known;
}

irs::hfftp::client_t::size_type
irs::hfftp::client_t::get_file_size() const
{
  return m_file_size;
}

void irs::hfftp::client_t::get_last_error(
  error_t* ap_error, string_type* ap_error_message)
{
  *ap_error = m_last_error;
  *ap_error_message = m_last_error_message;
}

void irs::hfftp::client_t::exec_changed_event()
{
  if (!m_changed_event.is_empty()) {
    m_changed_event->exec(*this);
  }
}

void irs::hfftp::client_t::set_error(
  error_t a_error, const string_type& a_error_message)
{
  m_last_error = a_error;
  m_last_error_message = a_error_message;
  IRS_LIB_HFFTP_DBG_MSG_BASE(irs::str_conv<std::string>(a_error_message));
  m_connection_is_established = false;
  m_status = irs_st_error;
}

bool irs::hfftp::client_t::agree_configuration(
  const packet_acknowledgment_options_t::fields_t& a_fields)
{
  const size_type data_size = a_fields.data_max_size;
  const double timeout = a_fields.timeout_ms/1000.;
  const size_type retransmit_max_count = a_fields.retransmit_count;

  if (!is_in_range(data_size, data_min_size,
    m_configuration.data_size)) {
    return false;
  }
  if (!is_in_range(timeout, min_timeout, m_configuration.timeout)) {
    return false;
  }
  if (!is_in_range<size_type>(retransmit_max_count, 0,
    m_configuration.retransmit_max_count)) {
    return false;
  }
  if (m_mode == mode_write) {
    m_file_size_is_known = a_fields.file_size_is_known;
    m_file_size = a_fields.file_size;
    if (m_file_size_is_known && mp_file_write_only->max_size_is_known()) {
      if (m_file_size > mp_file_write_only->max_size()) {
        return false;
      }
    }
  }
  m_protocol_config.data_size = data_size;
  m_protocol_config.timeout = timeout;
  m_protocol_config.retransmit_max_count = retransmit_max_count;
  return true;
}

void irs::hfftp::client_t::abort_writing_file_if_write_mode()
{
  if (mp_file_write_only) {
    mp_file_write_only->write_abort();
    mp_file_write_only = NULL;
  }
}

irs::hfftp::protocol_configuration_t
irs::hfftp::client_t::extract_protocol_config(
  const configuration_t& a_config)
{
  protocol_configuration_t protocol_config;
  protocol_config.data_size = a_config.data_size;
  protocol_config.timeout = a_config.timeout;
  protocol_config.retransmit_max_count = a_config.retransmit_max_count;
  return protocol_config;
}

void irs::hfftp::client_t::tick()
{
  m_packet_flow.tick();
  switch (m_process) {
    case process_wait_send_request: {
      if (m_packet_flow.status() == irs_st_ready) {
        m_packet_flow.receive_packet(m_channel);
        m_timeout_timer.start();
        set(process_wait_receive_acknowledgment_options);
      } else if ((m_packet_flow.status() == irs_st_error) ||
        m_timeout_timer.check()) {
        m_packet_flow.abort();
        abort_writing_file_if_write_mode();
        set_error(error_not_defined, irst("Отправить запрос не удалось."));
        exec_changed_event();
        set(process_wait_command);
      }
    } break;
    case process_wait_receive_acknowledgment_options: {
      if (m_packet_flow.status() == irs_st_ready) {
        const packet_t* packet = m_packet_flow.get_packet();
        if (packet->get_type() == pt_acknowledgment_options) {
          const packet_acknowledgment_options_t* packet_ack_opt =
            static_cast<const packet_acknowledgment_options_t*>(packet);
          if (agree_configuration(packet_ack_opt->get_fields())) {
            m_packet_flow.set_data_max_size(
              m_protocol_config.data_size);
            m_timeout_timer.set(make_cnt_s(m_protocol_config.timeout));
            m_connection_is_established = true;
            exec_changed_event();
            if (m_mode == mode_write) {
              packet_acknowledgment_options_t::fields_t fields;
              fields.data_max_size =
                static_cast<data_size_t>(m_protocol_config.data_size);
              fields.timeout_ms = bound(
                static_cast<timeout_t>(m_protocol_config.timeout*1000),
                static_cast<timeout_t>(1),
                std::numeric_limits<timeout_t>::max());
              fields.retransmit_count = static_cast<retransmit_count_t>(
                m_protocol_config.retransmit_max_count);
              m_packet_flow.send_acknowledgment_options(m_channel, fields);
              m_retransmit_count = 0;
              set(process_wait_send_acknowledgment_options);
            } else {
              m_buf_pos = 0;
              m_block_index = 0;
              m_read_buf.resize(m_protocol_config.data_size);
              set(process_read_data_from_file);
            }
            m_timeout_timer.start();
          } else {
            abort_writing_file_if_write_mode();
            set_error(error_not_defined,
              irst("От сервера получены недопустимые опции"));
            exec_changed_event();
            set(process_wait_command);
          }
        } else if (packet->get_type() == pt_error) {
          const packet_error_t* packet_error =
            static_cast<const packet_error_t*>(packet);
          abort_writing_file_if_write_mode();
          set_error(static_cast<error_t>(packet_error->get_fields().error_code),
            packet_error->get_fields().error_message);
          exec_changed_event();
          set(process_wait_command);
        } else {
          IRS_LIB_HFFTP_DBG_MSG_BASE("Получен пакет недопустимого типа. "
            "Игнорируем его");
          m_packet_flow.receive_packet_without_reset_timeout(m_channel);
        }
      } else if ((m_packet_flow.status() == irs_st_error) ||
        m_timeout_timer.check()) {
        m_packet_flow.abort();
        IRS_LIB_HFFTP_DBG_MSG_BASE(
          "Произошел тайм-аут получения пакета подтверждения опций");
        if (m_retransmit_count < m_protocol_config.retransmit_max_count) {
          m_retransmit_count++;
          m_packet_flow.retransmit_last_packet(m_channel);
          m_timeout_timer.start();
          set(process_wait_send_request);
        } else {
          //m_packet_flow.receive_packet(m_channel);
          abort_writing_file_if_write_mode();
          set_error(error_not_defined,
            irst("Достигнут лимит отправки пакета подтвреждения опций"));
          exec_changed_event();
          set(process_wait_command);
        }
      }
    } break;
    case process_wait_send_acknowledgment_options: {
      if (m_packet_flow.status() == irs_st_ready) {
        m_packet_flow.receive_packet(m_channel);
        set(process_wait_receive_data);
      } else if ((m_packet_flow.status() == irs_st_error) ||
        m_timeout_timer.check()) {
        m_packet_flow.abort();
        abort_writing_file_if_write_mode();
        set_error(error_not_defined,
          irst("Отправить пакет подтверждения опций не удалось"));
        exec_changed_event();
        set(process_wait_command);
      }
    } break;
    case process_wait_send_acknowledgment: {
      if (m_packet_flow.status() == irs_st_ready) {
        if (m_write_buf.size() < m_protocol_config.data_size) {
          m_connection_is_established = false;
          m_status = irs_st_ready;
          IRS_LIB_HFFTP_DBG_MSG_BASE("Получение файла завершено");
          exec_changed_event();
          set(process_wait_command);
        } else {
          //m_block_index++;
          m_packet_flow.receive_packet(m_channel);
          m_timeout_timer.start();
          set(process_wait_receive_data);
        }
      } else if ((m_packet_flow.status() == irs_st_error) ||
        m_timeout_timer.check()) {
        m_packet_flow.abort();
        if (m_write_buf.size() == m_protocol_config.data_size) {
          abort_writing_file_if_write_mode();
        }
        set_error(error_not_defined,
          irst("Отправить пакет подтверждения опций не удалось"));
        exec_changed_event();
        set(process_wait_command);
      }
    } break;
    case process_receive_acknowledgment: {
      if (m_packet_flow.status() == irs_st_ready) {
        const packet_t* packet = m_packet_flow.get_packet();
        if (packet->get_type() == pt_acknowledgment) {
          const packet_acknowledgment_t* packet_ack =
            static_cast<const packet_acknowledgment_t*>(packet);
          if (m_block_index == packet_ack->get_fields().block_index) {
            m_transferred_bytes += m_read_buf.size();
            if (mp_file_read_only->end_of_file()) {
              IRS_LIB_HFFTP_DBG_MSG_BASE("Отправка файла завершена");
              m_connection_is_established = false;
              m_status = irs_st_ready;
              exec_changed_event();
              set(process_wait_command);
            } else {
              m_block_index++;
              m_buf_pos = 0;
              m_read_buf.resize(m_protocol_config.data_size);
              m_timeout_timer.start();
              set(process_read_data_from_file);
            }
          } else {
            IRS_LIB_HFFTP_DBG_MSG_BASE(
              "Недопустимый индекс блока данных в пакете подтверждения"
              "Игнорируем пакет");
            m_packet_flow.receive_packet_without_reset_timeout(m_channel);
          }
        } else if (packet->get_type() == pt_error) {
          const packet_error_t* packet_error =
            static_cast<const packet_error_t*>(packet);
          set_error(static_cast<error_t>(packet_error->get_fields().error_code),
            packet_error->get_fields().error_message);
          exec_changed_event();
          set(process_wait_command);
        } else {
          IRS_LIB_HFFTP_DBG_MSG_BASE("Получен пакет недопустимого типа. "
            "Игнорируем его");
          m_packet_flow.receive_packet_without_reset_timeout(m_channel);
        }
      } else if ((m_packet_flow.status() == irs_st_error) ||
        m_timeout_timer.check()) {
        m_packet_flow.abort();
        IRS_LIB_HFFTP_DBG_MSG_BASE(
          "Произошел тайм-аут получения пакета подтверждения");
        if (m_retransmit_count < m_protocol_config.retransmit_max_count) {
          m_retransmit_count++;
          m_packet_flow.retransmit_last_packet(m_channel);
          IRS_LIB_HFFTP_DBG_MSG_DETAIL(
            "Посылаем опять пакет данных. Попытка №" << m_retransmit_count);
          m_timeout_timer.start();
          set(process_send_data);
        } else {
          mp_file_read_only = IRS_NULL;
          m_connection_is_established = false;
          m_status = irs_st_error;
          set_error(error_not_defined,
            irst("Достигнут лимит отправки пакета данных"));
          exec_changed_event();
          set(process_wait_command);
        }
      }
    } break;
    case process_wait_receive_data: {
      if (m_packet_flow.status() == irs_st_ready) {
        const packet_t* packet = m_packet_flow.get_packet();
        if (packet->get_type() == pt_data) {
          const packet_data_t* packet_data =
            static_cast<const packet_data_t*>(packet);
          if (m_block_index == packet_data->get_fields().block_index) {
            m_write_buf = packet_data->get_fields().data;
            m_buf_pos = 0;
            m_timeout_timer.start();
            set(process_write_data_to_file);
          } else {
            IRS_LIB_HFFTP_DBG_MSG_BASE(
              "Недопустимый индекс блока данных в пакете данных");
            m_packet_flow.receive_packet_without_reset_timeout(m_channel);
          }
        } else {
          IRS_LIB_HFFTP_DBG_MSG_BASE("Получен пакет недопустимого типа. "
            "Игнорируем его");
          m_packet_flow.receive_packet_without_reset_timeout(m_channel);
        }
      } else if ((m_packet_flow.status() == irs_st_error) ||
        m_timeout_timer.check()) {
        m_packet_flow.abort();
        IRS_LIB_HFFTP_DBG_MSG_BASE("Произошел тайм-аут получения пакета данных");
        if (m_retransmit_count < m_protocol_config.retransmit_max_count) {
          m_retransmit_count++;
          m_packet_flow.retransmit_last_packet(m_channel);
          m_timeout_timer.start();
          IRS_LIB_HFFTP_DBG_MSG_DETAIL(
            "Посылаем опять последний отправленный пакет. "
              "Попытка №" << m_retransmit_count);
          if (m_block_index == 0) {
            set(process_wait_send_acknowledgment_options);
          } else {
            set(process_wait_send_acknowledgment);
          }
        } else {
          //m_packet_flow.receive_packet(m_channel);
          abort_writing_file_if_write_mode();
          set_error(error_not_defined, irst("Достигнут лимит отправки пакета"));
          exec_changed_event();
          set(process_wait_command);
        }
      }
    } break;
    case process_send_data: {
      if (m_packet_flow.status() == irs_st_ready) {
        m_packet_flow.receive_packet(m_channel);
        m_timeout_timer.start();
        set(process_receive_acknowledgment);
      } else if ((m_packet_flow.status() == irs_st_error) ||
        m_timeout_timer.check()) {
        m_packet_flow.abort();
        set_error(error_not_defined,
          irst("Отправить пакет с данными не удалось"));
        exec_changed_event();
        set(process_wait_command);
      }
    } break;
    case process_write_data_to_file: {
      if (m_timeout_timer.check()) {
        abort_writing_file_if_write_mode();
        set_error(error_not_defined,
          irst("Произошел тайм-аут получения/записи данных"));
        exec_changed_event();
        set(process_wait_command);
        break;
      }
      const size_type write_byte_count = mp_file_write_only->write(
        vector_data(m_write_buf, m_buf_pos), m_write_buf.size());
      m_transferred_bytes += write_byte_count;
      m_buf_pos += write_byte_count;
      if (!mp_file_write_only->fail()) {
        if (m_buf_pos == m_write_buf.size()) {
          m_packet_flow.send_acknowledgment(m_channel, m_block_index);
          m_block_index++;
          m_retransmit_count = 0;
          m_timeout_timer.start();
          exec_changed_event();
          set(process_wait_send_acknowledgment);
        }
      } else {
        set_error(error_not_defined,
          irst("Произошла ошибка при записи в файл"));
        exec_changed_event();
        set(process_wait_command);
      }
    } break;
    case process_read_data_from_file: {
      if (m_timeout_timer.check()) {
        set_error(error_not_defined,
          irst("Произошел тайм-аут чтения данных из файла"));
        exec_changed_event();
        set(process_wait_command);
        break;
      }
      m_buf_pos += mp_file_read_only->read(vector_data(m_read_buf, m_buf_pos),
        m_read_buf.size() - m_buf_pos);
      if (!mp_file_read_only->fail()) {
        if ((m_buf_pos == m_read_buf.size()) ||
          mp_file_read_only->end_of_file()) {
          m_read_buf.resize(m_buf_pos);
          m_packet_flow.send_data(m_channel, m_block_index, m_read_buf);
          m_retransmit_count = 0;
          set(process_send_data);
        }
      } else {
        set_error(error_not_defined,
          irst("Произошла ошибка при чтении файла"));
        exec_changed_event();
        set(process_wait_command);
      }
    } break;
    case process_wait_command: {
      // Ожидание команды
    } break;
  }
}

void irs::hfftp::client_t::set_changed_event(
  handle_t<event_1_t<const client_t&> > a_event)
{
  m_changed_event = a_event;
}

void irs::hfftp::client_t::set(process_t a_process)
{
  m_process = a_process;
  switch (m_process) {
    case process_wait_send_request: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL(
        "Посылаем опять пакет запроса. Попытка №" << m_retransmit_count);
    } break;
    case process_wait_receive_acknowledgment_options: {
      IRS_LIB_HFFTP_DBG_MSG_BASE("Ожидаем подтверждения");
    } break;
    case process_wait_send_acknowledgment_options: {
      IRS_LIB_HFFTP_DBG_MSG_BASE("Отправляем подтверждение опций");
    } break;
    case process_wait_send_acknowledgment: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Отправка подтверждения");
    } break;
    case process_receive_acknowledgment: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Получаем пакет подтверждения данных");
    } break;
    case process_wait_receive_data: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Получение данных");
    } break;
    case process_send_data: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Отправка данных");
    } break;
    case process_write_data_to_file: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Записываем данные в файл");
    } break;
    case process_read_data_from_file: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Читаем данные из файла");
    } break;
    case process_wait_command: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Ожидаем команд");
    } break;
  }
}

irs::hfftp::client_t::configuration_type
irs::hfftp::client_t::normalize_configuration(
  const configuration_type& a_configuration)
{
  configuration_type configuration;
  configuration.data_size = bound<size_type>(a_configuration.data_size,
    data_min_size, std::numeric_limits<data_size_t>::max());
  configuration.timeout = bound<double>(a_configuration.timeout,
    min_timeout, std::numeric_limits<timeout_t>::max()/1000.);
  configuration.retransmit_max_count = bound<size_type>(
    a_configuration.retransmit_max_count, 0,
    std::numeric_limits<retransmit_count_t>::max());
  return configuration;
}

// class server_state_t
/*irs::hfftp::server_state_t::server_state_t(
  const server_channels_map_t* ap_channels_map):
  mp_channels_map(ap_channels_map)
{
}

irs::hfftp::server_state_t::size_type
irs::hfftp::server_state_t::channel_count() const
{
  return mp_channels_map->size();
}*/

// class server_channel_t
irs::hfftp::server_channel_t::server_channel_t(hardflow_t* ap_hardflow,
  size_type a_channel, files_t *ap_files,
  const configuration_type* ap_configuration
):
  mp_hardflow(ap_hardflow),
  m_channel(a_channel),
  mp_configuration(ap_configuration),
  m_protocol_config(extract_protocol_config(*ap_configuration)),
  m_connection_is_established(false),
  m_destroyed(false),
  m_packet_flow(ap_hardflow),
  m_send_timeout(mp_configuration->timeout),
  m_receive_timeout(mp_configuration->timeout),
  mp_files(ap_files),
  m_local_file_name(),
  mp_file_write_only(IRS_NULL),
  mp_file_read_only(IRS_NULL),
  m_read_buf(),
  m_write_buf(),
  m_buf_pos(0),
  m_block_index(0),
  m_process(process_receive_request),
  m_file_transfer_status(irs_st_ready),
  m_mode(mode_read),
  m_last_error(error_no_error),
  m_last_error_message(),
  m_file_name(),
  m_file_size_is_known(false),
  m_file_size(0),
  m_data_min_size(10),
  m_min_timeout(0.001),
  m_retransmit_min_count(0),
  m_timeout_timer(make_cnt_s(m_protocol_config.timeout)),
  m_changed_event()
{
  m_packet_flow.disable_receive_timeout();
  m_packet_flow.disable_send_timeout();
  m_packet_flow.receive_packet(m_channel);
  m_packet_flow.set_data_max_size(m_protocol_config.data_size);
}

irs::hfftp::server_channel_t::~server_channel_t()
{
  if (mp_file_read_only) {
    mp_files->close(mp_file_read_only);
    mp_file_read_only = IRS_NULL;
  }
  if (mp_file_write_only) {
    mp_file_write_only->write_abort();
    mp_files->close(mp_file_write_only);
    mp_file_write_only = IRS_NULL;
  }
}

irs::hfftp::server_channel_t::size_type
irs::hfftp::server_channel_t::get_channel_id() const
{
  return m_channel;
}

bool irs::hfftp::server_channel_t::connection_is_established() const
{
  return m_connection_is_established;
}

const irs::hfftp::protocol_configuration_t&
irs::hfftp::server_channel_t::get_protocol_configuration() const
{
  return m_protocol_config;
}

bool irs::hfftp::server_channel_t::destroyed() const
{
  return m_destroyed;
}

irs_status_t irs::hfftp::server_channel_t::get_file_transfer_status() const
{
  return m_file_transfer_status;
}

irs::hfftp::mode_t irs::hfftp::server_channel_t::get_mode() const
{
  return m_mode;
}

irs::hfftp::server_channel_t::size_type
irs::hfftp::server_channel_t::transferred_bytes() const
{
  return m_transferred_bytes;
}

const irs::hfftp::server_channel_t::string_type&
irs::hfftp::server_channel_t::get_file_name() const
{
  return m_file_name;
}

irs::hfftp::server_channel_t::size_type
irs::hfftp::server_channel_t::file_size_is_known() const
{
  return m_file_size_is_known;
}

irs::hfftp::server_channel_t::size_type
irs::hfftp::server_channel_t::get_file_size() const
{
  return m_file_size;
}

void irs::hfftp::server_channel_t::get_last_error(error_t* ap_error,
  string_type* ap_error_message)
{
  *ap_error = m_last_error;
  *ap_error_message = m_last_error_message;
}

void irs::hfftp::server_channel_t::set_changed_event(
  handle_t<event_1_t<const server_channel_t&> > a_event)
{
  m_changed_event = a_event;
}

void irs::hfftp::server_channel_t::tick()
{
  if (m_destroyed) {
    return;
  }
  if (!mp_hardflow->is_channel_exists(m_channel)) {
    IRS_LIB_HFFTP_DBG_MSG_BASE("Канал №" << m_channel << " уничтожен");
    m_destroyed = true;
    return;
  }
  m_packet_flow.tick();
  switch (m_process) {
    case process_receive_request: {
      if (m_packet_flow.status() == irs_st_ready) {
        const packet_t* packet = m_packet_flow.get_packet();
        bool error = false;
        if (packet->get_type() == pt_read_request) {
          m_mode = mode_read;
          const packet_read_request_t* read_request =
            static_cast<const packet_read_request_t*>(packet);
          m_local_file_name = read_request->get_fields().file_name;
          IRS_LIB_HFFTP_DBG_MSG_BASE("Принят запрос на чтение файла " <<
            irs::str_conv<std::string>(m_local_file_name));
          file_error_t error_code = file_error_no_error;
          m_file_name = read_request->get_fields().file_name;
          mp_file_read_only = mp_files->open_for_read(
            m_file_name, &error_code);
          if (mp_file_read_only) {
            m_file_size_is_known = mp_file_read_only->size_is_known();
            m_file_size = m_file_size_is_known ? mp_file_read_only->size() : 0;
          } else {
            error = true;
            m_packet_flow.send_error(m_channel,
              file_error_to_error(error_code),
              irst("Открыть файл на чтение не удалось"));
            set_error(file_error_to_error(error_code),
              irst("Открыть файл \"") + m_local_file_name +
              irst("\" на чтение не удалось"));
            m_file_transfer_status = irs_st_error;
            m_timeout_timer.start();
            exec_changed_event();
            set(process_send_error);
          }
        } else if (packet->get_type() == pt_write_request) {
          m_mode = mode_write;
          const packet_write_request_t* write_request =
            static_cast<const packet_write_request_t*>(packet);
          m_local_file_name = write_request->get_fields().file_name;
          IRS_LIB_HFFTP_DBG_MSG_BASE("Принят запрос на запись файла " <<
            irs::str_conv<std::string>(m_local_file_name));
          file_error_t error_code = file_error_no_error;
          m_file_name = write_request->get_fields().file_name;
          mp_file_write_only = mp_files->open_for_write(
            m_file_name, &error_code);
          if (mp_file_write_only) {
            m_file_size_is_known =
              write_request->get_fields().file_size_is_known;
            m_file_size = write_request->get_fields().file_size;
            if (m_file_size_is_known &&
              mp_file_write_only->max_size_is_known()) {
              if (m_file_size > mp_file_write_only->max_size()) {
                error = true;
                mp_file_write_only->write_abort();
                mp_files->close(mp_file_write_only);
                mp_file_write_only = IRS_NULL;
                m_packet_flow.send_error(m_channel, error_not_defined,
                  irst("Файл имеет слишком большой размер"));
                set_error(error_not_defined,
                  irst("Файл имеет слишком большой размер"));
                m_file_transfer_status = irs_st_error;
                m_timeout_timer.start();
                exec_changed_event();
                set(process_send_error);
              }
            }
          } else {
            error = true;
            m_packet_flow.send_error(m_channel, file_error_to_error(error_code),
              irst("Открыть файл на запись не удалось"));
            set_error(file_error_to_error(error_code),
              irst("Открыть файл \"") + m_local_file_name +
              irst("\" на запись не удалось"));
            m_file_transfer_status = irs_st_error;
            m_timeout_timer.start();
            exec_changed_event();
            set(process_send_error);
          }
        } else {
          error = true;
          m_packet_flow.send_error(m_channel, error_illegal_hfftp_operation,
            irst("Недопустимый пакет"));
          m_timeout_timer.start();
          IRS_LIB_HFFTP_DBG_MSG_DETAIL("Получен пакет, не являющийся пакетом "
            "запроса");
          set(process_send_error);
        }
        if (!error) {
          agree_configuration(packet);
          m_timeout_timer.set(make_cnt_s(m_protocol_config.timeout));
          m_packet_flow.set_data_max_size(m_protocol_config.data_size);
          packet_acknowledgment_options_t::fields_t fields;
          fields.data_max_size =
            static_cast<data_size_t>(m_protocol_config.data_size);
          fields.timeout_ms = bound(
            static_cast<timeout_t>(m_protocol_config.timeout*1000),
            static_cast<timeout_t>(1),
            std::numeric_limits<timeout_t>::max());
          fields.retransmit_count = static_cast<retransmit_count_t>(
            m_protocol_config.retransmit_max_count);
          fields.file_size_is_known = m_file_size_is_known;
          fields.file_size = m_file_size;
          m_packet_flow.send_acknowledgment_options(m_channel, fields);
          m_retransmit_count = 0;
          m_transferred_bytes = 0;
          m_block_index = 0;
          m_file_transfer_status = irs_st_busy;
          m_timeout_timer.start();
          exec_changed_event();
          set(process_send_acknowledgment_options);
        }
      } else if (m_packet_flow.status() == irs_st_error) {
        m_packet_flow.disable_receive_timeout();
        m_packet_flow.receive_packet(m_channel);
        IRS_LIB_HFFTP_DBG_MSG_DETAIL("Ошибка при получении пакета запроса");
      }
    } break;
    case process_send_acknowledgment_options: {
      if (m_packet_flow.status() == irs_st_ready) {
        m_packet_flow.enable_receive_timeout(m_receive_timeout);
        m_packet_flow.receive_packet(m_channel);
        m_timeout_timer.start();
        if (m_mode == mode_read) {
          set(process_receive_acknowledgment_option);
        } else {
          set(process_receive_data);
        }
      } else if ((m_packet_flow.status() == irs_st_error) ||
        m_timeout_timer.check()) {
        m_packet_flow.abort();
        if (mp_file_write_only) {
          mp_file_write_only->write_abort();
          mp_files->close(mp_file_write_only);
          mp_file_write_only = IRS_NULL;
        } else if (mp_file_read_only) {
          mp_files->close(mp_file_read_only);
          mp_file_read_only = IRS_NULL;
        }
        set_error(error_not_defined,
          irst("Не удалось отправить пакет подтверждения опций"));
        m_file_transfer_status = irs_st_error;
        exec_changed_event();
        //m_packet_flow.disable_receive_timeout();
        m_packet_flow.receive_packet(m_channel);
        set(process_receive_request);
      }
    } break;
    case process_receive_acknowledgment_option: {
      if (m_packet_flow.status() == irs_st_ready) {
        const packet_t* packet = m_packet_flow.get_packet();
        if (packet->get_type() == pt_acknowledgment_options) {
          m_buf_pos = 0;
          m_connection_is_established = true;
          exec_changed_event();
          m_read_buf.resize(m_protocol_config.data_size);
          m_timeout_timer.start();
          set(process_read_data_from_file);
        } else {
          IRS_LIB_HFFTP_DBG_MSG_BASE("Получен пакет недопустимого типа. "
            "Игнорируем его");
          m_packet_flow.receive_packet_without_reset_timeout(m_channel);
        }
      } else if ((m_packet_flow.status() == irs_st_error) ||
        m_timeout_timer.check()) {
        m_packet_flow.abort();
        IRS_LIB_HFFTP_DBG_MSG_BASE(
          "Произошел тайм-аут получения пакета подтверждения опций");
        if (m_retransmit_count < m_protocol_config.retransmit_max_count) {
          m_retransmit_count++;
          m_packet_flow.retransmit_last_packet(m_channel);
          m_timeout_timer.start();
          IRS_LIB_HFFTP_DBG_MSG_DETAIL(
            "Посылаем опять пакет подтверждения опций. Попытка №" <<
            m_retransmit_count);
          set(process_send_acknowledgment_options);
        } else {
          //m_packet_flow.disable_receive_timeout();
          m_packet_flow.receive_packet(m_channel);

          IRS_LIB_ASSERT(mp_file_read_only);
          mp_files->close(mp_file_read_only);
          mp_file_read_only = IRS_NULL;
          set_error(error_not_defined,
            irst("Достигнут лимит отправки пакета подтвреждения опций"));
          m_file_transfer_status = irs_st_error;
          exec_changed_event();
          set(process_receive_request);
        }
      }
    } break;
    case process_read_data_from_file: {
      if (m_timeout_timer.check()) {
        IRS_LIB_ASSERT(mp_file_read_only);
        mp_files->close(mp_file_read_only);
        mp_file_read_only = IRS_NULL;
        set_error(error_not_defined,
          irst("Произошел тайм-аут при чтении файла/передаче данных"));
        m_file_transfer_status = irs_st_error;
        exec_changed_event();
        set(process_receive_request);
        break;
      }
      const size_type read_count = mp_file_read_only->read(
        vector_data(m_read_buf, m_buf_pos), m_read_buf.size() - m_buf_pos);
      m_buf_pos += read_count;
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Прочитано из файла " << read_count << " байт");
      if (!mp_file_read_only->fail()) {
        if ((m_buf_pos == m_read_buf.size()) ||
          mp_file_read_only->end_of_file()) {
          if (mp_file_read_only->end_of_file()) {
            m_file_size_is_known = true;
            m_file_size = m_transferred_bytes + m_buf_pos;
            mp_files->close(mp_file_read_only);
            mp_file_read_only = IRS_NULL;
          }
          m_read_buf.resize(m_buf_pos);
          m_packet_flow.send_data(m_channel, m_block_index, m_read_buf);
          m_retransmit_count = 0;
          set(process_send_data);
        }
      } else {
        const file_error_t file_error = mp_file_read_only->error();
        mp_files->close(mp_file_read_only);
        mp_file_read_only = IRS_NULL;
        m_file_transfer_status = irs_st_error;
        set_error(file_error_to_error(file_error),
          irst("Произошла ошибка при чтении файла \"") +
          m_local_file_name + irst("\""));
        exec_changed_event();
        m_packet_flow.send_error(m_channel,
          file_error_to_error(file_error),
          irst("Произошла ошибка при чтении файла"));
        m_timeout_timer.start();
        set(process_send_error);
      }
    } break;
    case process_write_data_to_file: {
      if (m_timeout_timer.check()) {
        IRS_LIB_ASSERT(mp_file_write_only);
        mp_file_write_only->write_abort();
        mp_files->close(mp_file_write_only);
        mp_file_write_only = IRS_NULL;
        set_error(error_not_defined,
          irst("Произошел тайм-аут при получении данных/записи файла"));
        m_file_transfer_status = irs_st_error;
        exec_changed_event();
        set(process_receive_request);
        IRS_DBG_MSG("DEBUG process_write_data_to_file end");
        break;
      }
      const size_type write_byte_count =
        mp_file_write_only->write(vector_data(m_write_buf, m_buf_pos),
        m_write_buf.size() - m_buf_pos);
      m_buf_pos += write_byte_count;
      m_transferred_bytes += write_byte_count;
      exec_changed_event();
      if (!mp_file_write_only->fail()) {
        if (m_buf_pos == m_write_buf.size()) {
          if (m_write_buf.size() < m_protocol_config.data_size) {
            m_file_size_is_known = true;
            m_file_size = m_transferred_bytes;
          }
          m_packet_flow.send_acknowledgment(m_channel, m_block_index);
          m_block_index++;
          m_retransmit_count = 0;
          set(process_send_acknowledgment);
        }
      } else {
        const file_error_t file_error = mp_file_write_only->error();
        mp_files->close(mp_file_write_only);
        mp_file_write_only = IRS_NULL;
        m_file_transfer_status = irs_st_error;
        set_error(file_error_to_error(file_error),
          irst("Произошла ошибка при записи файла \"") +
          m_local_file_name + irst("\""));
        exec_changed_event();
        m_packet_flow.send_error(m_channel,
          file_error_to_error(file_error),
          irst("Произошла ошибка при записи в файл"));
        m_timeout_timer.start();
        set(process_send_error);
      }
    } break;
    case process_receive_data: {
      if (m_packet_flow.status() == irs_st_ready) {
        const packet_t* packet = m_packet_flow.get_packet();
        if (packet->get_type() == pt_data) {
          const packet_data_t* packet_data =
            static_cast<const packet_data_t*>(packet);
          if (m_block_index == 0) {
            m_connection_is_established = true;
          }
          if (m_block_index == packet_data->get_fields().block_index) {
            m_write_buf = packet_data->get_fields().data;
            m_buf_pos = 0;
            set(process_write_data_to_file);
          } else {
            IRS_LIB_HFFTP_DBG_MSG_BASE(
              "Недопустимый индекс блока данных в пакете данных");
            m_packet_flow.receive_packet_without_reset_timeout(m_channel);
          }
        } else {
          IRS_LIB_HFFTP_DBG_MSG_BASE("Получен пакет недопустимого типа. "
            "Игнорируем его");
          m_packet_flow.receive_packet_without_reset_timeout(m_channel);
        }
      } else if ((m_packet_flow.status() == irs_st_error) ||
        m_timeout_timer.check()) {
        m_packet_flow.abort();
        IRS_LIB_HFFTP_DBG_MSG_BASE(
          "Произошел тайм-аут получения пакета данных");
        if (m_retransmit_count < m_protocol_config.retransmit_max_count) {
          m_retransmit_count++;
          m_packet_flow.retransmit_last_packet(m_channel);
          IRS_LIB_HFFTP_DBG_MSG_DETAIL(
            "Посылаем опять последний отправленный пакет. Попытка №" <<
            m_retransmit_count);
          if (m_block_index == 0) {
            set(process_send_acknowledgment_options);
          } else {
            set(process_send_acknowledgment);
          }
        } else {
          IRS_LIB_ASSERT(mp_file_write_only);
          mp_file_write_only->write_abort();
          mp_files->close(mp_file_write_only);
          mp_file_write_only = IRS_NULL;
          m_file_transfer_status = irs_st_error;
          m_connection_is_established = false;
          set_error(error_not_defined,
            irst("Достигнут лимит отправки последнего пакета"));
          exec_changed_event();
          //m_packet_flow.disable_receive_timeout();
          m_packet_flow.receive_packet(m_channel);
          set(process_receive_request);
        }
      }
    } break;
    case process_send_data: {
      if (m_packet_flow.status() == irs_st_ready) {
        m_packet_flow.enable_receive_timeout(m_receive_timeout);
        m_packet_flow.receive_packet(m_channel);
        set(process_receive_acknowledgment);
      } else if ((m_packet_flow.status() == irs_st_error) ||
        m_timeout_timer.check()) {
        m_packet_flow.abort();
        IRS_LIB_HFFTP_DBG_MSG_BASE("Отправить пакет данных не удалось");
        mp_files->close(mp_file_read_only);
        mp_file_read_only = IRS_NULL;
        m_file_transfer_status = irs_st_error;
        m_connection_is_established = false;
        set_error(error_not_defined,
          irst("Отправить пакет данных не удалось"));
        exec_changed_event();
        //m_packet_flow.disable_receive_timeout();
        m_packet_flow.receive_packet(m_channel);
        set(process_receive_request);
      }
    } break;
    case process_send_acknowledgment: {
      if (m_packet_flow.status() != irs_st_busy) {
        if (m_write_buf.size() < m_protocol_config.data_size) {
          mp_files->close(mp_file_write_only);
          mp_file_write_only = IRS_NULL;
          IRS_LIB_HFFTP_DBG_MSG_BASE("Получение файла завершено");
          m_file_transfer_status = irs_st_ready;
          m_connection_is_established = false;
          exec_changed_event();
          m_packet_flow.disable_receive_timeout();
          m_packet_flow.receive_packet(m_channel);
          set(process_receive_request);
        } else {
          if (m_packet_flow.status() == irs_st_ready) {
            //m_block_index++;
            m_packet_flow.enable_receive_timeout(m_receive_timeout);
            m_packet_flow.receive_packet(m_channel);
            m_timeout_timer.start();
            set(process_receive_data);
          } else if ((m_packet_flow.status() == irs_st_error) ||
            m_timeout_timer.check()) {
            m_packet_flow.abort();
            mp_file_write_only->write_abort();
            mp_files->close(mp_file_write_only);
            mp_file_write_only = IRS_NULL;
            m_file_transfer_status = irs_st_error;
            m_connection_is_established = false;
            set_error(error_not_defined,
              irst("Отправить пакет подтверждения не удалось"));
            exec_changed_event();
            //m_packet_flow.disable_receive_timeout();
            m_packet_flow.receive_packet(m_channel);
            set(process_receive_request);
          }
        }
      }
    } break;
    case process_receive_acknowledgment: {
      if (m_packet_flow.status() == irs_st_ready) {
        const packet_t* packet = m_packet_flow.get_packet();
        if (packet->get_type() == pt_acknowledgment) {
          const packet_acknowledgment_t* acknowledgment =
            static_cast<const packet_acknowledgment_t*>(packet);
          if (acknowledgment->get_fields().block_index == m_block_index) {
            m_transferred_bytes += m_read_buf.size();
            exec_changed_event();
            if (m_read_buf.size() == m_protocol_config.data_size) {
              m_block_index++;
              m_buf_pos = 0;
              m_timeout_timer.start();
              set(process_read_data_from_file);
            } else {
              m_file_transfer_status = irs_st_ready;
              m_connection_is_established = false;
              exec_changed_event();
              IRS_LIB_HFFTP_DBG_MSG_BASE("Отправка файла завершена");
              m_packet_flow.disable_receive_timeout();
              m_packet_flow.receive_packet(m_channel);
              set(process_receive_request);
            }
          } else {
            IRS_LIB_HFFTP_DBG_MSG_BASE(
              "Недопустимый индекс блока данных в пакете подтверждения"
              "Игнорируем пакет");
            m_packet_flow.receive_packet_without_reset_timeout(m_channel);
          }
        } else {
          IRS_LIB_HFFTP_DBG_MSG_BASE("Получен пакет недопустимого типа. "
            "Игнорируем его");
          m_packet_flow.receive_packet_without_reset_timeout(m_channel);
        }
      } else if ((m_packet_flow.status() == irs_st_error) ||
        m_timeout_timer.check()) {
        m_packet_flow.abort();
        IRS_LIB_HFFTP_DBG_MSG_BASE(
          "Произошел тайм-аут получения пакета подтверждения");
        if (m_retransmit_count < m_protocol_config.retransmit_max_count) {
          m_retransmit_count++;
          m_packet_flow.retransmit_last_packet(m_channel);
          IRS_LIB_HFFTP_DBG_MSG_DETAIL(
            "Посылаем опять пакет данных. Попытка №" <<
            m_retransmit_count);
          m_timeout_timer.start();
          set(process_send_data);
        } else {
          mp_files->close(mp_file_read_only);
          mp_file_read_only = IRS_NULL;
          m_file_transfer_status = irs_st_error;
          m_connection_is_established = false;
          set_error(error_not_defined,
            irst("Достигнут лимит отправки пакета данных"));
          exec_changed_event();
          //m_packet_flow.disable_receive_timeout();
          m_packet_flow.receive_packet(m_channel);
          set(process_receive_request);
        }
      }
    } break;
    case process_send_error: {
      if (m_packet_flow.status() != irs_st_busy) {
        if ((m_packet_flow.status() == irs_st_error) ||
          m_timeout_timer.check()) {
          m_packet_flow.abort();
          IRS_LIB_HFFTP_DBG_MSG_BASE("Отправить пакет ошибки не удалось");
        }
        //m_packet_flow.disable_receive_timeout();
        m_packet_flow.receive_packet(m_channel);
        m_connection_is_established = false;
        exec_changed_event();
        set(process_receive_request);
      }
    } break;
  }
}

void irs::hfftp::server_channel_t::set(process_t a_process)
{
  m_process = a_process;
  switch (m_process) {
    case process_receive_request: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Ожидаем пакет запроса");
    } break;
    case process_send_acknowledgment_options: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Посылаем пакет подтверждения опций");
    } break;
    case process_receive_acknowledgment_option: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Ожидаем пакет подтверждения опций");
    } break;
    case process_read_data_from_file: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Читаем данные из файла");
    } break;
    case process_write_data_to_file: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Записываем данные в файл");
    } break;
    case process_send_data: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Отправляем данные");
    } break;
    case process_receive_data: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Ожидаем пакет данных");
    } break;
    case process_send_acknowledgment: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Посылаем подтверждение данных");
    } break;
    case process_receive_acknowledgment: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Ожидаем пакет подтверждения данных");
    } break;
    case process_send_error: {
      IRS_LIB_HFFTP_DBG_MSG_DETAIL("Посылаем сообщение об ошибке");
    } break;
  }
}

void irs::hfftp::server_channel_t::exec_changed_event()
{
  if (!m_changed_event.is_empty()) {
    m_changed_event->exec(*this);
  }
}

void irs::hfftp::server_channel_t::set_error(
  error_t a_error, const string_type& a_error_message)
{
  m_last_error = a_error;
  m_last_error_message = a_error_message;
  IRS_LIB_HFFTP_DBG_MSG_BASE(irs::str_conv<std::string>(a_error_message));
}

void irs::hfftp::server_channel_t::agree_configuration(
  const packet_t* ap_packet)
{
  IRS_LIB_ASSERT((ap_packet->get_type() == pt_read_request) ||
    (ap_packet->get_type() == pt_write_request));

  data_size_t data_size = 0;
  timeout_t timeout_ms = 0;
  retransmit_count_t retransmit_count = 0;
  if (ap_packet->get_type() == pt_read_request) {
    const packet_read_request_t* read_request =
      static_cast<const packet_read_request_t*>(ap_packet);
    data_size = read_request->get_fields().data_max_size;
    timeout_ms = read_request->get_fields().timeout_ms;
    retransmit_count = read_request->get_fields().retransmit_count;
  } else if (ap_packet->get_type() == pt_write_request) {
    const packet_write_request_t* write_request =
      static_cast<const packet_write_request_t*>(ap_packet);
    data_size = write_request->get_fields().data_max_size;
    timeout_ms = write_request->get_fields().timeout_ms;
    retransmit_count = write_request->get_fields().retransmit_count;
  }
  m_protocol_config.data_size = bound<size_type>(
    data_size, m_data_min_size,
    mp_configuration->data_size);
  m_protocol_config.timeout = bound<double>(
    timeout_ms_to_timeout(timeout_ms), m_min_timeout,
    mp_configuration->timeout);
  m_protocol_config.retransmit_max_count = bound<size_type>
    (retransmit_count, m_retransmit_min_count,
    mp_configuration->retransmit_max_count);
}
irs::hfftp::protocol_configuration_t
irs::hfftp::server_channel_t::extract_protocol_config(
  const configuration_type& a_config)
{
  protocol_configuration_t protocol_config;
  protocol_config.data_size = a_config.data_size;
  protocol_config.timeout = a_config.timeout;
  protocol_config.retransmit_max_count = a_config.retransmit_max_count;
  return protocol_config;
}

// class server_t
irs::hfftp::server_t::server_t(irs::hardflow_t* ap_hardflow, files_t* ap_files,
  size_type a_channel_max_count, const configuration_type &a_configuration
):
  m_data_min_size(10),
  m_min_timeout(0.001),
  m_retransmit_min_count(0),
  mp_hardflow(ap_hardflow),
  mp_files(ap_files),
  m_channels(),
  m_channel_configuration(normalize_configuration(a_configuration)),
  m_channel_max_count(a_channel_max_count),
  m_channel_it(m_channels.end()),
  mp_created_channel_event(),
  mp_destroyed_channel_event(),
  mp_channel_changed_event(),
  mp_changed_event()
{
}

void irs::hfftp::server_t::set_created_channel_event(
  handle_t<event_1_t<size_type> > a_event)
{
  mp_created_channel_event = a_event;
}

void irs::hfftp::server_t::set_destroyed_channel_event(
  handle_t<event_1_t<size_type> > a_event)
{
  mp_destroyed_channel_event = a_event;
}

void irs::hfftp::server_t::set_changed_event(
  handle_t<event_1_t<const server_channels_map_t&> > a_event)
{
  mp_changed_event = a_event;
}

void irs::hfftp::server_t::set_channel_changed_event(
  handle_t<event_1_t<const server_channel_t&> > a_event)
{
  mp_channel_changed_event = a_event;
  channels_map_type::iterator it = m_channels.begin();
  while (it != m_channels.end()) {
    it->second->set_changed_event(mp_channel_changed_event);
  }
}

void irs::hfftp::server_t::tick()
{
  if (m_channel_it == m_channels.end()) {
    size_type channel = mp_hardflow->channel_next();
    if (mp_hardflow->is_channel_exists(channel) &&
      (m_channels.find(channel) == m_channels.end()) &&
      (m_channels.size() < m_channel_max_count)) {
      pair<channels_map_type::iterator, bool> res =
        m_channels.insert(make_pair(channel, new server_channel_t(mp_hardflow,
        channel, mp_files, &m_channel_configuration)));
      //res.first->second->set_changed_event(irs::make_event(this,
        //&server_t::channel_changed_event));
      res.first->second->set_changed_event(mp_channel_changed_event);
      exec_created_channel_event(channel);
    }
    m_channel_it = m_channels.begin();
  } else {
    m_channel_it->second->tick();
    if (m_channel_it->second->destroyed()) {
      channels_map_type::iterator it = m_channel_it;
      ++m_channel_it;
      const size_type channel_id = it->first;
      m_channels.erase(it);
      exec_destroyed_channel_event(channel_id);
    } else {
      ++m_channel_it;
    }
  }
}

irs::hfftp::server_t::configuration_type
irs::hfftp::server_t::normalize_configuration(
  const configuration_type& a_configuration)
{
  configuration_type configuration;
  configuration.data_size = bound<size_type>(a_configuration.data_size,
    m_data_min_size, std::numeric_limits<data_size_t>::max());
  configuration.timeout = bound<double>(a_configuration.timeout,
    m_min_timeout, std::numeric_limits<timeout_t>::max()/1000.);
  configuration.retransmit_max_count = bound<size_type>(
    a_configuration.retransmit_max_count, m_retransmit_min_count,
    std::numeric_limits<retransmit_count_t>::max());
  return configuration;
}

void irs::hfftp::server_t::exec_channel_changed_event(
  const server_channel_t& a_channel)
{
  if (!mp_channel_changed_event.is_empty()) {
    mp_channel_changed_event->exec(a_channel);
  }
}

void irs::hfftp::server_t::exec_created_channel_event(size_type a_channel_id)
{
  if (!mp_created_channel_event.is_empty()) {
    mp_created_channel_event->exec(a_channel_id);
  }
}

void irs::hfftp::server_t::exec_destroyed_channel_event(size_type a_channel_id)
{
  if (!mp_destroyed_channel_event.is_empty()) {
    mp_destroyed_channel_event->exec(a_channel_id);
  }
}

#endif // defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__))
