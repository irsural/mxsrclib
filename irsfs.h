#ifndef IRS_FS_H
#define IRS_FS_H

#include <cstdint>
#include <cstdio>
#include <mxdata.h>
#include <string>

namespace irs {

enum fs_result_t {
  fsr_success,
  fsr_error,
  fsr_file_exists,
  fsr_file_not_exists,
  fsr_end_of_dir
};

enum file_mode_t {
  fm_read,
  fm_write,
  fm_readwrite,
  fm_append
};

struct file_info_t
{
  std::string name;
  uint64_t size;
  bool is_dir;
};

class dir_iterator_t
{
public:
  virtual ~dir_iterator_t() {};
  virtual fs_result_t next_dir_item(file_info_t* file_info) = 0;
  virtual fs_result_t break_iter() = 0;
};

class fs_t
{
public:
  typedef void file_t;

  virtual ~fs_t() {};
  virtual bool is_mounted() = 0;
  virtual fs_result_t mkfs(const std::string& a_path) = 0;

  virtual file_t* open(const std::string& a_filename, file_mode_t a_mode) = 0;
  virtual fs_result_t close(file_t* a_file) = 0;
  virtual size_t read(
    file_t* a_file, uint8_t* a_buffer, size_t a_count, fs_result_t* ap_result
  ) = 0;
  virtual fs_result_t write(file_t* a_file, uint8_t* a_buffer, size_t a_count) = 0;

  virtual fs_result_t rename(const std::string& a_old_name, const std::string& a_new_name) = 0;
  virtual size_t get_file_size(file_t* a_file, fs_result_t* ap_result) = 0;

  virtual fs_result_t truncate_to_zero(const std::string& a_filename) = 0;

  virtual bool is_dir_exists(const std::string& a_dir_name, fs_result_t* ap_result) = 0;
  virtual bool is_file_exists(const std::string& a_file_name, fs_result_t* ap_result) = 0;

  virtual fs_result_t mkdir(const std::string& a_dir_name) = 0;
  virtual irs::handle_t<dir_iterator_t> get_dir_iterator(const std::string& a_dir_name) = 0;

  /// \brief Удаляет файл или пустую папку
  /// \details При попытке удалить не пустую папку, метод вернет ошибку
  /// \param a_file_name Имя файла/папки
  virtual fs_result_t unlink(const std::string& a_file_name) = 0;
};

} // namespace irs

#endif // IRS_FS_H
