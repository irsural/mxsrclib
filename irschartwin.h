// Базовый класс для окна с графиком
// Дата: 28.09.2009
// Дата создания: 27.09.2009

#ifndef IRSCHARTWINH
#define IRSCHARTWINH

#include <irsdefs.h>
#include <irsstring.h>
#include <irsrect.h>

namespace irs {

// Окно с графиком
class chart_window_t {
public:
  virtual ~chart_window_t() {}
  virtual void show() = 0;
  virtual void hide() = 0;
  virtual rect_t position() const = 0;
  virtual void set_position(const rect_t &a_position) = 0;
  virtual void add_param(const irs::string &a_name) = 0;
  virtual void delete_param(const irs::string &a_name) = 0;
  virtual void clear_param() = 0;
  virtual void set_value(const irs::string &a_name, double a_value) = 0;
  virtual void set_value(const irs::string &a_name, double a_time,
    double a_value) = 0;
  virtual void set_time(double a_time) = 0;
  virtual void add() = 0;
  virtual void add(const irs::string &a_name, double a_time,
    double a_value) = 0;
  virtual void clear() = 0;
  virtual void resize(irs_u32 a_size) = 0;
  virtual irs_u32 size() const = 0;
  virtual void group_all() = 0;
  virtual void ungroup_all() = 0;
};

} //namespace irs

#endif //IRSCHARTWINH
