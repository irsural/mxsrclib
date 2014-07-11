//! \file
//! \ingroup single_type_group
//! \brief Сохранение и загрузка графиков
//!
//! Дата создания: 03.07.2014

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <string.h>

#include <csvwork.h>

#include <irs_chart_data.h>

#include <irsfinal.h>
//---------------------------------------------------------------------------

// struct chart_t
vector<double> irs::chart_data::chart_t::get_x_array() const
{
  vector<double> x_array;
  for (size_type i = 0; i < points.size(); i++) {
    x_array.push_back(points[i].x);
  }
  return x_array;
}

vector<double> irs::chart_data::chart_t::get_y_array() const
{
  vector<double> y_array;
  for (size_type i = 0; i < points.size(); i++) {
    y_array.push_back(points[i].y);
  }
  return y_array;
}

void irs::chart_data::chart_t::set_x_y_arrays(
  const vector<double>& a_x_array,
  const vector<double>& a_y_array)
{
  if (a_x_array.size() != a_y_array.size()) {
    throw logic_error("Не совпадают размеры массивов");
  }
  set_x_y_arrays(a_x_array, a_y_array, a_x_array.size());
}

void irs::chart_data::chart_t::set_x_y_arrays(
  const vector<double>& a_x_array,
  const vector<double>& a_y_array, size_type a_size)
{
  if ((a_x_array.size() < a_size) || (a_y_array.size() < a_size)) {
    throw logic_error("Недостаточно элементов в массивах");
  }
  points.clear();
  for (size_type i = 0; i < a_size; i++) {
    point_type point;
    point.x = a_x_array[i];
    point.y = a_y_array[i];
    points.push_back(point);
  }
}

// class charts_csv_file_t
irs::chart_data::charts_csv_file_t::charts_csv_file_t():
  m_x_suffix(irst("_x")),
  m_y_suffix(irst("_y")),
  m_data_row_start_index(1)
{
}

irs::chart_data::charts_t irs::chart_data::charts_csv_file_t::load(
  const string_type& a_file_name) const
{
  csvwork::csv_file_synchro_t csv_file;
  csv_file.set_file_name(a_file_name);
  table_string_t table_string;
  if (!csv_file.load(&table_string)) {
    throw runtime_error("Не удалось загрузить файл");
  }

  const size_type col_count =
    table_string.get_col_count() - table_string.get_col_count()%2;
  size_type row_count = table_string.get_row_count();
  size_type row_index = 0;

  vector<string_type> names;

  size_type col_index = 0;
  while (col_index < col_count) {
    string_type name = table_string.read_cell(col_index, 0);
    if (name.size() >= m_x_suffix.size()) {
      const size_type pos = name.size()- m_x_suffix.size();
      string_type suffix = name.substr(pos);
      if (suffix == m_x_suffix) {
        name = name.substr(0, pos);
      }
    }
    if (find(names.begin(), names.end(), name) == names.end()) {
      names.push_back(name);
    }
    col_index += 2;
  }

  charts_t charts;
  size_type x_col_index = 0;
  for (size_type chart_i = 0; chart_i < names.size(); chart_i++) {
    chart_t chart;
    for (size_type row_index = m_data_row_start_index; row_index < row_count;
        row_index++) {
      chart_t::point_type point;
      double x = 0;
      double y = 0;
      string_type x_str = table_string.read_cell(x_col_index, row_index);
      string_type y_str = table_string.read_cell(x_col_index + 1, row_index);
      if (str_to_num(x_str, &point.x) && str_to_num(y_str, &point.y)) {
        chart.points.push_back(point);
      }
    }
    chart.index = chart_i;
    charts.insert(make_pair(names[chart_i], chart));
    x_col_index += 2;
  }
  return charts;
}

void irs::chart_data::charts_csv_file_t::save(
  const charts_t& a_charts, const string_type& a_file_name) const
{
  table_string_t table_string;
  table_string.set_col_count(a_charts.size()*2);
  size_type row_count = 0;
  charts_t::const_iterator it = a_charts.begin();
  while (it != a_charts.end()) {
    row_count = std::max(it->second.points.size(), row_count);
    ++it;
  }
  // На 1 больше для названия столбцов
  table_string.set_row_count(row_count + 1);

  it = a_charts.begin();
  size_type col_index = 0;
  while (it != a_charts.end()) {
    const string_type name = it->first;
    table_string.write_cell(col_index, 0, name + m_x_suffix);
    col_index++;
    table_string.write_cell(col_index, 0, name + m_y_suffix);
    col_index++;
    ++it;
  }

  it = a_charts.begin();
  col_index = 0;
  while (it != a_charts.end()) {
    size_type row_index = m_data_row_start_index;
    const size_type chart_size = it->second.points.size();
    for (size_type i = 0; i < chart_size; i++) {
      const point_t<double> point = it->second.points[i];
      string_type x_cell;
      num_to_str(point.x, &x_cell);
      table_string.write_cell(col_index, row_index, x_cell);
      string_type y_cell;
      num_to_str(point.y, &y_cell);
      table_string.write_cell(col_index + 1, row_index, y_cell);
      row_index++;
    }
    col_index += 2;
    row_index = m_data_row_start_index;
    ++it;
  }

  csvwork::csv_file_synchro_t csv_file;
  csv_file.set_file_name(a_file_name);
  if (!csv_file.save(table_string)) {
    throw runtime_error("Не удалось сохранить файл");
  }
}

