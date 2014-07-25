//! \file
//! \ingroup single_type_group
//! \brief Сохранение и загрузка графиков
//!
//! Дата создания: 03.07.2014

#ifndef irs_chart_dataH
#define irs_chart_dataH

#include <irsdefs.h>

#include <irscpp.h>
#include <string.h>

#include <irsfinal.h>

namespace irs {

namespace chart_data {

template <class T>
struct point_t
{
  T x;
  T y;
  point_t(T a_x = 0, T a_y = 0):
    x(a_x),
    y(a_y)
  {
  };
};

struct chart_t
{
  typedef irs_size_t size_type;
  typedef point_t<double> point_type;
  typedef vector<point_type> points_type;
  vector<point_type> points;
  size_type index;
  bool visible;
  chart_t():
    points(),
    index(0),
    visible(true)
  {
  }
  chart_t(const vector<double>& a_x_array, const vector<double>& a_y_array):
    points()
  {
    const size_type size = min(a_x_array.size(), a_y_array.size());
    for (size_type i = 0; i < size; i++) {
      points.push_back(point_type(a_x_array[i], a_y_array[i]));
    }
  }
  vector<double> get_x_array() const;
  vector<double> get_y_array() const;
  void set_x_y_arrays(const vector<double>& a_x_array,
    const vector<double>& a_y_array);
  void set_x_y_arrays(const vector<double>& a_x_array,
    const vector<double>& a_y_array, size_type a_size);
};

typedef map<irs::string_t, chart_t> charts_t;

class charts_csv_file_t
{
public:
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;
  charts_csv_file_t();
  charts_t load(const string_type& a_file_name) const;
  void save(const charts_t& a_charts, const string_type& a_file_name) const;
private:
  const string_type m_x_suffix;
  const string_type m_y_suffix;
  const size_type m_data_row_start_index;
};

#ifdef IRS_USE_JSON_CPP
class charts_json_file_t
{
public:
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;
  charts_json_file_t();
  charts_t load(const string_type& a_file_name) const;
  void save(const charts_t& a_charts, const string_type& a_file_name) const;
private:
};
#endif // IRS_USE_JSON_CPP

} // namespace chart_data

} // namespace irs
//---------------------------------------------------------------------------
#endif
