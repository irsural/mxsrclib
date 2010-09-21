// ������� ����� ��� ���� � ��������
// ����: 28.09.2009
// ���� ��������: 27.09.2009

#ifndef IRSCHARTWINH
#define IRSCHARTWINH

#include <irsdefs.h>

#include <irsstrdefs.h>
#include <irsstring.h>
#include <irsrect.h>

#include <irsfinal.h>

namespace irs {

// ���� � ��������
class chart_window_t {
public:
  typedef string_t string_type;
  virtual ~chart_window_t() {}
  virtual void show() = 0;
  virtual void hide() = 0;
  virtual rect_t position() const = 0;
  virtual void set_position(const rect_t &a_position) = 0;
  virtual void add_param(const string_type &a_name) = 0;
  virtual void delete_param(const string_type &a_name) = 0;
  virtual void clear_param() = 0;
  virtual void set_value(const string_type &a_name, double a_value) = 0;
  virtual void set_value(const string_type &a_name, double a_time,
    double a_value) = 0;
  virtual void set_time(double a_time) = 0;
  virtual void add() = 0;
  virtual void add(const string_type &a_name, double a_time,
    double a_value) = 0;
  virtual void clear() = 0;
  virtual void set_refresh_time(irs_i32 a_refresh_time_ms) = 0; 
  virtual void resize(irs_u32 a_size) = 0;
  virtual irs_u32 size() const = 0;
  virtual void group_all() = 0;
  virtual void ungroup_all() = 0;
};

} //namespace irs

#endif //IRSCHARTWINH
