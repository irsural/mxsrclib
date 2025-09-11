//! \file
//! \ingroup system_utils_group
//! \brief Функции для работы с файловой системой для C++Builder

#include <irspch.h>
#pragma hdrstop

#include <stringapiset.h>

#include <irserror.h>

#include <fs_cppbuilder.h>

#include <irsfinal.h>

namespace irs {

// Конвертация UTF-8 в wstring (Windows)
std::wstring utf8_to_wstring(const std::string& utf8)
{
  if (utf8.empty()) return L"";

  // Получаем необходимый размер буфера
  int wchars_count = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
  if (wchars_count == 0) {
    throw runtime_error("Failed to convert UTF-8 to wchar_t (size calculation)");
  }

  // Выделяем буфер
  vector<wchar_t> buffer(wchars_count);

  // Выполняем преобразование
  if (MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &buffer[0], wchars_count) == 0) {
    throw runtime_error("Failed to convert UTF-8 to wchar_t");
  }

  return std::wstring(&buffer[0]);
}

// Конвертация wstring в UTF-8 (Windows)
std::string wstring_to_utf8(const std::wstring& wstr)
{
  if (wstr.empty()) return "";

  // Получаем необходимый размер буфера
  int utf8_count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
  if (utf8_count == 0) {
    throw runtime_error("Failed to convert wchar_t to UTF-8 (size calculation)");
  }

  // Выделяем буфер
  vector<char> buffer(utf8_count);

  // Выполняем преобразование
  if (WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &buffer[0], utf8_count, NULL, NULL) == 0) {
    throw runtime_error("Failed to convert wchar_t to UTF-8");
  }

  return std::string(&buffer[0]);
}

class dir_iterator_dummy_t: public dir_iterator_t
{
public:
  dir_iterator_dummy_t() {}
  virtual ~dir_iterator_dummy_t() {}
  virtual fs_result_t next_dir_item(file_info_t*)
  {
    return fs_result_t::fsr_end_of_dir;
  }
  virtual fs_result_t break_iter()
  {
    return fs_result_t::fsr_success;
  }
};

class fs_cppbuilder_t: public fs_t
{
public:
  virtual ~fs_cppbuilder_t() {};

  virtual bool is_mounted();
  virtual fs_result_t mkfs(const std::string& a_path);

  virtual file_t* open(const std::string& a_filename, file_mode_t a_mode);
  virtual fs_result_t close(file_t* a_file);
  virtual size_t read(
    file_t* a_file, uint8_t* a_buffer, size_t a_count, fs_result_t* ap_result
  );
  virtual fs_result_t write(file_t* a_file, const uint8_t* a_buffer, size_t a_count);

  virtual fs_result_t rename(const std::string& a_old_name, const std::string& a_new_name);
  virtual size_t get_file_size(file_t* a_file, fs_result_t* ap_result);

  virtual fs_result_t truncate_to_zero(const std::string& a_filename);

  virtual bool is_dir_exists(const std::string& a_dir_name, fs_result_t* ap_result);
  virtual bool is_file_exists(const std::string& a_file_name, fs_result_t* ap_result);

  virtual fs_result_t unlink(const std::string& a_file_name);

  virtual fs_result_t mkdir(const std::string& a_dir_name);
  virtual handle_t<dir_iterator_t> get_dir_iterator(const std::string& a_dir_name);

private:
  // fc - file convert. Преобразование указателя на файл.
  static FILE* fc(file_t* ap_vfile);
};

bool fs_cppbuilder_t::is_mounted()
{
  return fs_result_t::fsr_success;
}

fs_result_t fs_cppbuilder_t::mkfs(const std::string& a_path)
{
  return fs_result_t::fsr_error;
}

fs_cppbuilder_t::file_t* fs_cppbuilder_t::open(const std::string& a_filename,
  file_mode_t a_mode)
{
  std::string mode = "";
  switch (a_mode) {
    case (file_mode_t::fm_read): {
      mode = "rb";
    } break;
    case (file_mode_t::fm_write): {
      mode = "wb";
    } break;
    case (file_mode_t::fm_readwrite): {
      mode = "w+b";
    } break;
    case (file_mode_t::fm_append): {
      mode = "a+b";
    } break;
  }
  return _wfopen(utf8_to_wstring(a_filename).c_str(), utf8_to_wstring(mode).c_str());
}

fs_result_t fs_cppbuilder_t::close(file_t* a_file)
{
  if (fclose(fc(a_file)) == 0) {
    return fs_result_t::fsr_success;
  } else {
    return fs_result_t::fsr_error;
  }
}

size_t fs_cppbuilder_t::read(
  file_t* a_file, uint8_t* a_buffer, size_t a_count, fs_result_t* ap_result
)
{
  size_t read_count =
    fread(static_cast<void*>(a_buffer), sizeof(uint8_t), a_count, fc(a_file));
  if (read_count == a_count || feof(fc(a_file))) {
    *ap_result = fs_result_t::fsr_success;
    return read_count;
  } else {
    *ap_result = fs_result_t::fsr_error;
    return read_count;
  }
}

fs_result_t fs_cppbuilder_t::write(file_t* a_file, const uint8_t* a_buffer, size_t a_count)
{
  size_t written =
    fwrite(reinterpret_cast<const void*>(a_buffer), sizeof(uint8_t), a_count, fc(a_file));
  fflush(fc(a_file));
  if (written == a_count) {
    return fs_result_t::fsr_success;
  } else {
    return fs_result_t::fsr_error;
  }
}

size_t fs_cppbuilder_t::get_file_size(file_t* a_file, fs_result_t* ap_result)
{
  *ap_result = fs_result_t::fsr_success;
  fseek(fc(a_file), 0L, SEEK_END);
  return ftell(fc(a_file));
}

fs_result_t fs_cppbuilder_t::truncate_to_zero(const std::string& a_filename)
{
  fs_cppbuilder_t::file_t* file_handle = open(a_filename, file_mode_t::fm_write);
  if (file_handle == NULL) {
    return fs_result_t::fsr_error;
  }

  fs_result_t res = close(file_handle);
  return res;
}

fs_result_t fs_cppbuilder_t::rename(const std::string& a_old_name,
  const std::string& a_new_name)
{
  fs_result_t res = fs_result_t::fsr_success;
  if (is_file_exists(a_new_name, &res)) {
    if (res != fs_result_t::fsr_success) {
      return res;
    }

    res = unlink(a_new_name);
    if (res != fs_result_t::fsr_success) {
      return res;
    }
  }
  int result = _wrename(utf8_to_wstring(a_old_name).c_str(), utf8_to_wstring(a_new_name).c_str());
  return result == 0 ? fs_result_t::fsr_success : fs_result_t::fsr_error;
}

bool fs_cppbuilder_t::is_dir_exists(const std::string&, fs_result_t* ap_result)
{
  *ap_result = fs_result_t::fsr_error;
  IRS_LIB_ERROR(ec_standard, "Операция fs_cppbuilder_t::is_dir_exists не поддерживается");
  return false;
}

bool fs_cppbuilder_t::is_file_exists(const std::string& a_file_name, fs_result_t* ap_result)
{
  *ap_result = fs_result_t::fsr_error;
  IRS_LIB_ERROR(ec_standard, "Операция fs_cppbuilder_t::is_file_exists не поддерживается");
  return false;
}

fs_result_t fs_cppbuilder_t::unlink(const std::string& a_file_name)
{
  const wchar_t* file_name = utf8_to_wstring(a_file_name).c_str();

  DWORD attributes = GetFileAttributesW(file_name);
  if (attributes == INVALID_FILE_ATTRIBUTES) {
    return fsr_error; // Путь не существует
  }

  if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
    // Это папка - удаляем как папку
    return RemoveDirectoryW(file_name) ? irs::fsr_success : irs::fsr_error;
  } else {
    // Это файл - удаляем как файл
    return DeleteFileW(file_name) ? irs::fsr_success : irs::fsr_error;
  }
}

fs_result_t fs_cppbuilder_t::mkdir(const std::string&)
{
  IRS_LIB_ERROR(ec_standard, "Операция fs_cppbuilder_t::mkdir не поддерживается");
  return fs_result_t::fsr_error;
}

handle_t<dir_iterator_t> fs_cppbuilder_t::get_dir_iterator(const std::string&)
{
  IRS_LIB_ERROR(ec_standard, "Операция fs_cppbuilder_t::get_dir_iterator не поддерживается");
  return handle_t<dir_iterator_t>(new dir_iterator_dummy_t());
}

FILE* fs_cppbuilder_t::fc(file_t* ap_vfile)
{
  return reinterpret_cast<FILE*>(ap_vfile);
}


fs_t* fs_cppbuilder()
{
  static fs_cppbuilder_t fs_cppbuilder_i;
  return &fs_cppbuilder_i;
}

} // namespace irs

