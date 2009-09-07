
#ifndef tablegH
#define tablegH

#include <string>
#include <vector>
#include <algorithm>
#include <irsstdg.h>
#include <irserror.h>
#include <irsmath.h>
#include <irssysutils.h>
#include <irsmatrix.h>
#include <strstream> 
#include <string>
//---------------------------------------------------------------------------

class table_size_read_only_t
{
public:
  typedef size_t size_type;
  virtual size_type get_col_count() const = 0;
  virtual size_type get_row_count() const = 0;
protected:
  table_size_read_only_t()
  {}
public:
};


class table_size_t: public table_size_read_only_t
{
public:
  virtual void set_col_count(const size_type a_col_count) = 0;
  virtual void set_row_count(const size_type a_row_count) = 0;
  table_size_t()
  {}
};
/*
template<class T>
class table_data_t: public table_size_t
{
private:
  std::vector<T> m_cells;
  size_type m_col_count;
  size_type m_row_count;
public:
  table_data_t();
  table_data_t(const size_type a_col_count, const size_type a_row_count);
  table_data_t(
    const size_type a_col_count, const size_type a_row_count, const T& a_value);
  virtual ~table_data_t();
  inline const T& read_cell(
    const size_type a_col, const size_type a_row) const;
  inline void write_cell(
    const size_type a_col, const size_type a_row, const T& a_cell);
  virtual inline size_type get_col_count() const;
  virtual inline size_type get_row_count() const;
  virtual inline void set_col_count(const size_type a_col_count);
  virtual inline void set_row_count(const size_type a_row_count);
  inline void size(size_type* ap_col_count, size_type* ap_row_count) const;
  inline void resize(const size_type a_col_count, const size_type a_row_count);
  void newcols(
    const size_type a_index_pos,
    const size_type a_new_col_count,
    const T& a_value);
  void newrows(
    const size_type a_index_pos,
    const size_type a_new_row_count,
    const T& a_value);
  void delcols(const size_type a_index_begin, const size_type a_index_end);
  void delrows(const size_type a_index_begin, const size_type a_index_end);
  inline const table_data_t<T>& operator=(const table_data_t<T>& a_table_data);
  inline bool operator==(const table_data_t<T>& a_table_data) const;
  inline bool operator!=(const table_data_t<T>& a_table_data) const;
  void swap_cols(const size_type a_index_col_1, const size_type a_index_col_2);
  void swap_rows(const size_type a_index_row_1, const size_type a_index_row_2);
  void clear();
};

template<class T>
table_data_t<T>::table_data_t():
  m_cells(0),
  m_col_count(0),
  m_row_count(0)
{
}

template<class T>
table_data_t<T>::table_data_t(
  const size_type a_col_count, const size_type a_row_count):
  m_cells(a_col_count * a_row_count),
  m_col_count(a_col_count),
  m_row_count(a_row_count)
{
}

template<class T>
table_data_t<T>::table_data_t(
  const size_type a_col_count, const size_type a_row_count, const T& a_value):
  m_cells(a_col_count * a_row_count, a_value),
  m_col_count(a_col_count),
  m_row_count(a_row_count)
{
}

template<class T>
table_data_t<T>::~table_data_t()
{
}

template<class T>
inline const T& table_data_t<T>::read_cell(
  const size_type a_col, const size_type a_row) const
{
  return *(m_cells.begin()+(m_col_count * a_row+a_col));
}     

template<class T>
inline void table_data_t<T>::write_cell(
  const size_type a_col, const size_type a_row, const T& a_cell)
{ 
  m_cells[m_col_count * a_row+a_col] = a_cell;
}

template<class T>
inline table_data_t<T>::size_type table_data_t<T>::get_col_count() const
{
  return m_col_count;
}

template<class T>
inline table_data_t<T>::size_type table_data_t<T>::get_row_count() const
{
  return m_row_count;
}

template<class T>
inline void table_data_t<T>::set_col_count(const size_type a_col_count)
{
  resize(a_col_count, m_row_count);
}

template<class T>
inline void table_data_t<T>::set_row_count(const size_type a_row_count)
{
  m_row_count = a_row_count;
  const int new_size = m_col_count * a_row_count;
  m_cells.resize(new_size);
}

template<class T>
inline void table_data_t<T>::size(
  size_type* ap_col_count, size_type* ap_row_count) const
{
  *ap_col_count = m_col_count;
  *ap_row_count = m_row_count;
}

template<class T>
inline void table_data_t<T>::resize(
  const size_type a_col_count, const size_type a_row_count)
{
  // ѕеределать на более эффективный вариант
  size_type col_count_min = min(m_col_count, a_col_count);
  size_type row_count_min = min(m_row_count, a_row_count);
  const size_type new_size = a_col_count * a_row_count;
  if ((m_col_count < a_col_count) && (a_row_count != 0)) {
    const size_type col_empty_count = a_col_count - m_col_count;
    m_cells.resize(new_size);
    const std::vector<T>::iterator it_cells_beg = m_cells.begin();
    std::vector<T>::iterator it_cur_row_beg = IRS_NULL;
    std::vector<T>::iterator it_cur_row_end = IRS_NULL;
    std::vector<T>::iterator it_cur_new_row_beg = IRS_NULL;
    std::vector<T>::iterator it_cur_new_row_end = IRS_NULL;

    for (size_type row = row_count_min-1; row > 0; row--) {
      it_cur_row_beg = it_cells_beg+m_col_count*row;
      it_cur_new_row_beg = it_cells_beg+a_col_count*row;
      it_cur_row_end = it_cur_row_beg + m_col_count;
      it_cur_new_row_end = it_cur_new_row_beg + m_col_count;
      copy_backward(it_cur_row_beg, it_cur_row_end, it_cur_new_row_end);
      fill_n(it_cur_new_row_end, col_empty_count, T());
    }
    it_cur_row_end = it_cells_beg + m_col_count;
    fill_n(it_cur_row_end, col_empty_count, T());
  } else if ((m_col_count > a_col_count) && (a_row_count != 0)) {
    const std::vector<T>::iterator it_cells_beg = m_cells.begin();
    for (size_type row = 1; row < row_count_min; row++) {
      const std::vector<T>::iterator it_cur_row =
        it_cells_beg+m_col_count*row;
      const std::vector<T>::iterator it_cur_new_row =
        it_cells_beg+a_col_count*row;
      copy(
        it_cur_row,
        it_cur_row + m_col_count,
        it_cur_new_row);
    }
    m_cells.resize(new_size);
  } else if (m_row_count != a_row_count) {
    m_cells.resize(new_size);
  }
  m_col_count = a_col_count;
  m_row_count = a_row_count;
}

template<class T>
void table_data_t<T>::newcols(
  const size_type a_index_pos,
  const size_type a_new_col_count,
  const T& a_value)
{
  size_type end_index_new_cols = a_index_pos+a_new_col_count-1;
  m_col_count += a_new_col_count;
  vector<T> vec(m_col_count * m_row_count);
  size_type i1 = 0, i2 = 0;
  for(size_type y = 0; y < m_row_count; y++){
    for(size_type x = 0; x < m_col_count; x++){
      if(x < a_index_pos || x > end_index_new_cols){
        vec[i1] = m_cells[i2];
        i2++;
      }else{
        vec[i1] = a_value;
      }
      i1++;
    }
  }
  m_cells = vec;
}

template<class T>
void table_data_t<T>::newrows(
  const size_type a_index_pos,
  const size_type a_new_row_count,
  const T& a_value)
{
  m_cells.insert(
    m_cells.begin() + m_col_count * a_index_pos,
    m_col_count * a_new_row_count,
    a_value);
  m_row_count += a_new_row_count;
}

template<class T>
void table_data_t<T>::delcols(
  const size_type a_index_begin, const size_type a_index_end)
{
  size_type col_count = m_col_count-((a_index_end-a_index_begin)+1);
  vector<T> vec(col_count*m_row_count);
  size_type i1 = 0, i2 = 0;
  for(size_type y = 0; y < m_row_count; y++){
    for(size_type x = 0; x < m_col_count; x++){
      if(x < a_index_begin || x > a_index_end){
        vec[i1] = m_cells[i2];
        i1++;
      }
      i2++;
    }
  }
  m_cells = vec;
  m_col_count = col_count;
  if(m_col_count == 0)
    m_row_count = 0;
}

template<class T>
void table_data_t<T>::delrows(
  const size_type a_index_begin, const size_type a_index_end)
{
  m_cells.erase(
    m_cells.begin()+m_col_count*a_index_begin,
    m_cells.begin()+m_col_count*(a_index_end+1));
  m_row_count = m_row_count-((a_index_end-a_index_begin)+1);
  if(m_row_count == 0)
    m_col_count = 0;
}

template<class T>
inline const table_data_t<T>& table_data_t<T>::operator=(
  const table_data_t<T>& a_table_data)
{
  m_cells = a_table_data.m_cells;
  m_col_count = a_table_data.m_col_count;
  m_row_count = a_table_data.m_row_count;
  return *this;
}

template<class T>
inline bool table_data_t<T>::operator==(
  const table_data_t<T>& a_table_data) const
{
  return ((m_cells == a_table_data.m_cells) &&
    (m_col_count == a_table_data.m_col_count) &&
    (m_row_count == a_table_data.m_row_count));
}

template<class T>
inline bool table_data_t<T>::operator!=(
  const table_data_t<T>& a_table_data) const
{
  return !((m_cells == a_table_data.m_cells) &&
    (m_col_count == a_table_data.m_col_count) &&
    (m_row_count == a_table_data.m_row_count));
}

template<class T>
void table_data_t<T>::swap_cols(
  const size_type a_index_col_1, const size_type a_index_col_2)
{
  if (a_index_col_1 != a_index_col_2) {
    for (size_type row = 0; row < m_row_count; row++) {
      const size_type index_cur_row = m_col_count * row;
      const size_type index_cur_cell_col_1 = index_cur_row + a_index_col_1;
      const size_type index_cur_cell_col_2 = index_cur_row + a_index_col_2;
      T cell = m_cells[index_cur_cell_col_1];
      m_cells[index_cur_cell_col_1] = m_cells[index_cur_cell_col_2];
      m_cells[index_cur_cell_col_2] = cell;
    }
  }
}

template<class T>
void table_data_t<T>::swap_rows(
  const size_type a_index_row_1, const size_type a_index_row_2)
{
  if (a_index_row_1 != a_index_row_2) {
    std::vector<T>::iterator index_begin_subdiapason_1 =
      m_cells.begin() + m_col_count * a_index_row_1;
    std::vector<T>::iterator index_end_subdiapason_1 =
      m_cells.begin() + m_col_count * a_index_row_1 + m_col_count;
    std::vector<T>::iterator index_begin_subdiapason_2 =
      m_cells.begin() + m_col_count * a_index_row_2;
    swap_ranges(
      index_begin_subdiapason_1,
      index_end_subdiapason_1,
      index_begin_subdiapason_2);
  }
}

template<class T>
void table_data_t<T>::clear()
{
  m_cells.clear();
  m_col_count = 0;
  m_row_count = 0;
}
*/



/*template<class container_type_t, class elem_type_t>
class test
{
  container_type_t<elem_type_t> m_var;
};*/

// Class table_t
// ѕримеры использовани€
// ex1: table_t<int, std::vector<int>, std::vector<std::vector<int> > > table;
// ex2: table_t<int, std::deque<int>, std::list<std::deque<int> > > table;

template<
  class cell_type_t,
  class column_type_t = std::deque<cell_type_t>,
  class container_t = std::deque<std::deque<cell_type_t> > >
class table_t: public table_size_t
{
public:
  typedef typename container_t::size_type size_type;
  typedef typename container_t::iterator it_container_t;
  typedef typename container_t::const_iterator it_const_container;
  typedef typename column_type_t::iterator it_column_t;
  typedef typename column_type_t::const_iterator it_const_column_t;
private:
  container_t m_container;
  size_type m_col_count;
  size_type m_row_count;
public:
  table_t();
  table_t(size_type a_new_col_count, size_type a_new_row_count);
  inline const cell_type_t& read_cell(
    const size_type a_col_index, const size_type a_row_index) const;
  inline void write_cell(
    const size_type a_col_index,
    const size_type a_row_index,
    const cell_type_t& a_cell);
  virtual inline size_type get_col_count() const;
  virtual inline size_type get_row_count() const;
  virtual inline void set_col_count(size_type a_col_count);
  virtual inline void set_row_count(size_type a_row_count);
  template<class it_col_type_t>
  void col_push_front(it_col_type_t a_first_elem);
  template<class it_col_type_t>
  void col_push_back(it_col_type_t a_first_elem);
  template<class it_row_type_t>
  void row_push_front(it_row_type_t a_first_elem);
  template<class it_row_type_t>
  void row_push_back(it_row_type_t a_first_elem);
  // ¬озвращает первый столбец
  template<class col_type_t>
  inline col_type_t col_front() const;
  // ¬озвращает последний столбец
  template<class col_type_t>
  inline col_type_t col_back() const;
  // ¬озвращает первую строку
  template<class row_type_t>
  inline row_type_t row_front() const;
  // ¬озвращает последнюю строку
  template<class row_type_t>
  inline row_type_t row_back() const;
  void col_pop_front();
  void col_pop_back();
  void row_pop_front();
  void row_pop_back();
  void col_swap(size_type a_col1_index, size_type a_col2_index);
  void row_swap(size_type a_row1_index, size_type a_row2_index);
  void col_erase(size_type a_col_index);
  void col_erase(size_type a_col_index, size_type a_count);
  void row_erase(size_type a_row_index);
  void row_erase(size_type a_row_index, size_type a_count);
  template<class it_col_type_t>
  void col_insert(size_type a_col_index, it_col_type_t a_first_elem);
  template<class it_col_type_t>
  void col_insert(
    size_type a_col_index, size_type a_count, it_col_type_t a_first_elem);
  template<class it_row_type_t>
  void row_insert(size_type a_col_index, it_row_type_t a_first_elem);
  template<class it_row_type_t>
  void row_insert(
    size_type a_col_index, size_type a_count, it_row_type_t a_first_elem);
  inline void resize(size_type a_new_col_count, size_type a_new_row_count);
  inline void clear();
};

template<class cell_type_t, class column_type_t, class container_t>
table_t<cell_type_t, column_type_t, container_t>::table_t():
  m_container(),
  m_col_count(0),
  m_row_count(0)
{
}

template<class cell_type_t, class column_type_t, class container_t>
table_t<cell_type_t, column_type_t, container_t>::
table_t(size_type a_new_col_count, size_type a_new_row_count):
  m_container(),
  m_col_count(0),
  m_row_count(0)
{
  resize(a_new_col_count, a_new_row_count);
}

template<class cell_type_t, class column_type_t, class container_t>
inline const cell_type_t& table_t<cell_type_t, column_type_t, container_t>::
read_cell(const size_type a_col_index, const size_type a_row_index) const
{
  IRS_LIB_ASSERT(a_col_index < m_col_count);
  IRS_LIB_ASSERT(a_row_index < m_row_count);
  it_const_container it_col = m_container.begin();
  advance(it_col, a_col_index);
  it_const_column_t it_elem_col = it_col->begin();
  advance(it_elem_col, a_row_index);
  return *it_elem_col;
}

template<class cell_type_t, class column_type_t, class container_t>
inline void table_t<cell_type_t, column_type_t, container_t>::
write_cell(
  const size_type a_col_index,
  const size_type a_row_index,
  const cell_type_t& a_cell)
{
  IRS_LIB_ASSERT(a_col_index < m_col_count);
  IRS_LIB_ASSERT(a_row_index < m_row_count);
  it_container_t it_col = m_container.begin();
  advance(it_col, a_col_index);
  it_column_t it_elem_col = it_col->begin();
  advance(it_elem_col, a_row_index);
  *it_elem_col = a_cell;
}

template<class cell_type_t, class column_type_t, class container_t>
inline table_t<cell_type_t, column_type_t, container_t>::size_type
table_t<cell_type_t, column_type_t, container_t>::get_col_count() const
{
  IRS_LIB_ASSERT(m_col_count == m_container.size());
  return m_col_count;
}

template<class cell_type_t, class column_type_t, class container_t>
inline table_t<cell_type_t, column_type_t, container_t>::size_type
table_t<cell_type_t, column_type_t, container_t>::get_row_count() const
{
  #ifdef IRS_LIB_DEBUG
  if (m_container.size() > 0) {
    IRS_LIB_ASSERT(m_row_count == m_container.begin()->size());
  } else {
    IRS_LIB_ASSERT(m_row_count == 0);
  }
  #endif
  return m_row_count;
}

template<class cell_type_t, class column_type_t, class container_t>
inline void table_t<cell_type_t, column_type_t, container_t>::
set_col_count(size_type a_col_count)
{
  resize(a_col_count, m_row_count);
}

template<class cell_type_t, class column_type_t, class container_t>
inline void table_t<cell_type_t, column_type_t, container_t>::
  set_row_count(size_type a_row_count)
{
  resize(m_col_count, a_row_count);
}

template<class cell_type_t, class column_type_t, class container_t>
template<class it_col_type_t>
void table_t<cell_type_t, column_type_t, container_t>::
col_push_front(it_col_type_t a_first_elem)
{
  const index_first_elem = 0;
  col_insert(index_first_elem, a_first_elem);
}

template<class cell_type_t, class column_type_t, class container_t>
template<class it_col_type_t>
void table_t<cell_type_t, column_type_t, container_t>::
col_push_back(it_col_type_t a_first_elem)
{
  col_insert(m_col_count, a_first_elem);
}

template<class cell_type_t, class column_type_t, class container_t>
template<class it_row_type_t>
void table_t<cell_type_t, column_type_t, container_t>::
row_push_front(it_row_type_t a_first_elem)
{
  const index_first_elem = 0;
  row_insert(index_first_elem, a_first_elem);
}

template<class cell_type_t, class column_type_t, class container_t>
template<class it_row_type_t>
void table_t<cell_type_t, column_type_t, container_t>::
row_push_back(it_row_type_t a_first_elem)
{
  row_insert(m_row_count, a_first_elem);
}

template<class cell_type_t, class column_type_t, class container_t>
template<class col_type_t>
inline col_type_t table_t<cell_type_t, column_type_t, container_t>::
col_front() const
{
  col_type_t col;
  col.rezerve(m_row_count);
  it_const_container_t it_beg_col = m_container.begin();
  it_const_column_t it_elem_col = it_beg_col->begin();
  it_const_column_t it_end_elem_col = it_beg_col->end();
  while (it_elem_col != it_end_elem_col) {
    col.push_back(*it_elem_col);
    it_elem_col++;
  }
  return col;
}

template<class cell_type_t, class column_type_t, class container_t>
template<class col_type_t>
inline col_type_t table_t<cell_type_t, column_type_t, container_t>::
col_back() const
{
  col_type_t col;
  col.rezerve(m_row_count);
  it_const_container_t it_end_col = m_container.end();
  it_const_column_t it_elem_col = it_end_col->begin();
  it_const_column_t it_end_elem_col = it_end_col->end();
  while (it_elem_col != it_end_elem_col) {
    col.push_back(*it_elem_col);
    it_elem_col++;
  }
  return col;
}

template<class cell_type_t, class column_type_t, class container_t>
template<class row_type_t>
inline row_type_t table_t<cell_type_t, column_type_t, container_t>::
row_front() const
{
  row_type_t row;
  row.reserve(m_col_count);
  it_const_container it_col = m_container.begin();
  it_const_container it_end_col = m_container.end();
  while (it_col != it_end_col) {
    row.push_back(it_col->front());
    it_col++;
  }
  return row;
}

template<class cell_type_t, class column_type_t, class container_t>
template<class row_type_t>
inline row_type_t table_t<cell_type_t, column_type_t, container_t>::
row_back() const
{
  row_type_t row;
  row.rezerve(m_col_count);
  it_const_container_t it_col = m_container.begin();
  it_const_container_t it_end_col = m_container.end();
  while (it_col != it_end_col) {
    row.push_back(it_col->back());
  }
  return row;
}

template<class cell_type_t, class column_type_t, class container_t>
void table_t<cell_type_t, column_type_t, container_t>::
  col_pop_front()
{
  col_erase(0);
}

template<class cell_type_t, class column_type_t, class container_t>
void table_t<cell_type_t, column_type_t, container_t>::
col_pop_back()
{
  col_erase(m_col_count - 1);
}

template<class cell_type_t, class column_type_t, class container_t>
void table_t<cell_type_t, column_type_t, container_t>::
row_pop_front()
{
  row_erase(0);
}

template<class cell_type_t, class column_type_t, class container_t>
void table_t<cell_type_t, column_type_t, container_t>::
row_pop_back()
{
  row_erase(m_row_count - 1);
}

template<class cell_type_t, class column_type_t, class container_t>
void table_t<cell_type_t, column_type_t, container_t>::
col_swap(size_type a_col1_index, size_type a_col2_index)
{
  IRS_LIB_ASSERT(a_col1_index < m_col_count);
  IRS_LIB_ASSERT(a_col2_index < m_col_count);
  it_container_t it_col1 = m_container.begin();
  it_container_t it_col2 = m_container.begin();
  advance(it_col1, a_col1_index);
  advance(it_col2, a_col2_index);
  std::swap(*it_col1, *it_col2);
}

template<class cell_type_t, class column_type_t, class container_t>
void table_t<cell_type_t, column_type_t, container_t>::
row_swap(size_type a_row1_index, size_type a_row2_index)
{
  IRS_LIB_ASSERT(a_row1_index < m_row_count);
  IRS_LIB_ASSERT(a_row2_index < m_row_count);
  it_container_t it_col = m_container.begin();
  for (size_type col = 0; col < m_col_count; col++) {
    it_column_t it_elem_col1 = it_col->begin();
    it_column_t it_elem_col2 = it_col->begin();
    advance(it_elem_col1, a_row1_index);
    advance(it_elem_col2, a_row2_index);
    iter_swap(it_elem_col1, it_elem_col2);
    it_col++;
  }
}

template<class cell_type_t, class column_type_t, class container_t>
void table_t<cell_type_t, column_type_t, container_t>::
col_erase(size_type a_col_index)
{
  IRS_LIB_ASSERT(a_col_index < m_col_count);
  it_container_t it_col = m_container.begin();
  advance(it_col, a_col_index);
  m_container.erase(it_col);
  m_col_count--;
}

template<class cell_type_t, class column_type_t, class container_t>
void table_t<cell_type_t, column_type_t, container_t>::
col_erase(size_type a_col_index, size_type a_count)
{
  IRS_LIB_ASSERT((a_col_index + a_count) <= m_col_count);
  it_container_t it_first_col = m_container.begin();
  advance(it_first_col, a_col_index);
  it_container_t it_last_col = it_first_col;
  advance(it_last_col, a_count);
  m_container.erase(it_first_col, it_last_col);
  m_col_count -= a_count;
}

template<class cell_type_t, class column_type_t, class container_t>
void table_t<cell_type_t, column_type_t, container_t>::
row_erase(size_type a_row_index)
{
  IRS_LIB_ASSERT(a_row_index < m_row_count);
  it_container_t it_col = m_container.begin();
  for (size_type col = 0; col < m_col_count; col++) {
    it_column_t it_elem_col = it_col->begin();
    advance(it_elem_col, a_row_index);
    it_col->erase(it_elem_col);
    it_col++;
  }
  m_row_count--;
}

template<class cell_type_t, class column_type_t, class container_t>
void table_t<cell_type_t, column_type_t, container_t>::
row_erase(size_type a_row_index, size_type a_count)
{
  IRS_LIB_ASSERT((a_row_index + a_count) <= m_row_count);
  it_container_t it_col = m_container.begin();
  for (size_type col = 0; col < m_col_count; col++) {
    it_column_t it_first_elem_col = it_col->begin();
    advance(it_first_elem_col, a_row_index);
    it_column_t it_last_elem_col = it_first_elem_col;
    advance(it_last_elem_col, a_count);
    it_col->erase(it_first_elem_col, it_last_elem_col);
    it_col++;
  }
  m_row_count -= a_count;
}


template<class cell_type_t, class column_type_t, class container_t>
template<class it_col_type_t>
void table_t<cell_type_t, column_type_t, container_t>::
  col_insert(size_type a_col_index, it_col_type_t a_first_elem)
{
  IRS_LIB_ASSERT(a_col_index <= m_col_count);
  typedef typename container_t::iterator cont_it_t;
  cont_it_t it_col = m_container.begin();
  advance(it_col, a_col_index);
  cont_it_t new_it_col =
    m_container.insert(it_col, column_type_t());
  std::copy_n(
    a_first_elem, m_row_count, inserter(*new_it_col, new_it_col->begin()));
  m_col_count++;
}

template<class cell_type_t, class column_type_t, class container_t>
template<class it_col_type_t>
void table_t<cell_type_t, column_type_t, container_t>::
  col_insert(
    size_type a_col_index, size_type a_count, it_col_type_t a_first_elem)
{
  IRS_LIB_ASSERT(a_col_index <= m_col_count);
  it_container_t it_col = m_container.begin();
  advance(it_col, a_col_index);
  m_container.insert(it_col, a_count, column_type_t());
  it_col = m_container.begin();
  advance(it_col, a_col_index);
  std::copy_n(a_first_elem, m_row_count, inserter(*it_col, it_col->begin()));
  it_container_t first_new_col = it_col;
  it_col++;
  for (size_type cur_count = 1; cur_count < a_count; cur_count++) {
    *it_col = *first_new_col;
    it_col++;
  }
  m_col_count += a_count;
}

template<class cell_type_t, class column_type_t, class container_t>
template<class it_col_type_t>
void table_t<cell_type_t, column_type_t, container_t>::
  row_insert(size_type a_row_index, it_col_type_t a_first_elem)
{
  IRS_LIB_ASSERT(a_row_index <= m_row_count);
  it_container_t it_col = m_container.begin();
  for (size_type col = 0; col < m_col_count; col++) {
    it_column_t it_pos_elem_col = it_col->begin();
    advance(it_pos_elem_col, a_row_index);
    it_col->insert(it_pos_elem_col, *a_first_elem);
    it_col++;
    a_first_elem++;
  }
  m_row_count++;
}

template<class cell_type_t, class column_type_t, class container_t>
template<class it_col_type_t>
void table_t<cell_type_t, column_type_t, container_t>::
  row_insert(
    size_type a_row_index, size_type a_count, it_col_type_t a_first_elem)
{
  IRS_LIB_ASSERT(a_row_index <= m_row_count);
  it_container_t it_col = m_container.begin();
  for (size_type col = 0; col < m_col_count; col++) {
    it_column_t it_pos_elem_col = it_col->begin();
    advance(it_pos_elem_col, a_row_index);
    it_col->insert(it_pos_elem_col, a_count, *a_first_elem);
    it_col++;
    a_first_elem++;
  }
  m_row_count += a_count;
}

template<class cell_type_t, class column_type_t, class container_t>
inline void table_t<cell_type_t, column_type_t, container_t>::
resize(size_type a_new_col_count, size_type a_new_row_count)
{
  m_container.resize(a_new_col_count);
  if (a_new_row_count == m_row_count) {
    if ((m_row_count > 0) && (a_new_col_count > m_col_count)) {
      it_container_t it_col = m_container.begin();
      advance(it_col, m_col_count);
      for (size_type col = m_col_count; col < a_new_col_count; col++) {
        it_col->resize(m_row_count);
        it_col++;
      }
    }
  } else {
    it_container_t it_col = m_container.begin();
    for (size_type col = 0; col < a_new_col_count; col++) {
      it_col->resize(a_new_row_count);
      it_col++;
    }
  }
  m_col_count = a_new_col_count;
  m_row_count = a_new_row_count;
}

template<class cell_type_t, class column_type_t, class container_t>
inline void table_t<cell_type_t, column_type_t, container_t>::
  clear()
{
  m_container.clear();
  m_col_count = 0;
  m_row_count = 0;
}



typedef table_t<
  irs::string,
  std::deque<irs::string>,
  std::deque<std::deque<irs::string> > > table_string_t;

/*class table_string_t:public
  table_t<
    irs::string,
    std::deque<irs::string>,
    std::deque<std::deque<irs::string> > >
{
private:
  char m_delimiter_col;
  static const char m_delimiter_row = '\n';
public:
  table_string_t();
  table_string_t(const int a_col_count, const int a_row_count);
  table_string_t(
    const int a_col_count, const int a_row_count, const irs::string& a_value);
  virtual ~table_string_t();
  inline void set_delimiter_col(const char a_delimiter_col);
  bool save_to_file_csv(const irs::string& a_file_name) const;
  bool load_from_file_csv(const irs::string& a_file_name);
};*/

/*inline void table_string_t::set_delimiter_col(const char a_delimiter_col)
{
  m_delimiter_col = a_delimiter_col;
}*/


/*template<
  template< class > class Container,
  template< class > class Container2,
  class Type >
class CrossReferences
{
  Container< Container2<Type> > mems;
  Container< Type* > refs;

};*/


/*template<
  template<class> class column_type_t,
  template<class> class row_type_t,
  class cell_type_t>
class table_t
{
public:
  typedef column_type_t::size_type size_type;
private:
  column_type_t<row_type_t<cell_type_t> > m_container;
  size_type m_col_count;
  size_type m_row_count;
public:
  table_t();

  inline size_type get_col_count() const;
  inline size_type get_row_count() const;
  void set_col_count(size_type a_col_count);
  void set_row_count(size_type a_row_count);
  template<class T>
  void col_push_front(const T& a_obj);
  void col_push_back(const type_t& a_obj);
  void row_push_front(const type_t& a_obj);
  void row_push_back(const type_t& a_obj);
  void col_swap(size_type a_col1_index, size_type a_col2_index);
  void row_swap(size_type a_row1_index, size_type a_row2_index);
  void col_erase(size_type a_col_index);
  void row_erase(size_type a_row_index);
  void col_insert(size_type a_col_index);
  void row_insert(size_type a_col_index);
  void resize(size_type a_new_col_count, size_type a_new_row_count);
};
//typedef typename table_t<container_t, type_t>::size_type __type_size

template<class container_t, class type_t>
table_t<container_t, type_t>::table_t():
  m_container(),
  m_col_count(0),
  m_row_count(0)
{
}

template<class container_t, class type_t>
inline table_t<container_t, type_t>::size_type
  table_t<container_t, type_t>::get_col_count() const
{
  return m_col_count;
}

template<class container_t, class type_t>
inline table_t<container_t, type_t>::size_type
  table_t<container_t, type_t>::get_row_count() const
{
  return m_row_count;
}

template<class container_t, class type_t>
void table_t<container_t, type_t>::set_col_count(size_type a_col_count)
{
  m_container.resize(a_col_count);
  if ((m_row_count > 0) && (m_col_count < a_col_count)) {
    for (size_type col = m_col_count; col < a_col_count; col++) {
      m_container[col].resize(m_row_count);
    }
  }
  m_col_count = a_col_count;
}

template<class container_t, class type_t>
void table_t<container_t, type_t>::set_row_count(size_type a_row_count)
{
  for (size_type col = 0; col < m_col_count; col++) {
    m_container[col].resize(a_row_count);
  }
  m_row_count = a_row_count;
}

template<class container_t, class type_t>
template<class T>
void table_t<container_t, type_t>::col_push_front(const T& a_obj)
{

} */




//template<class T>
/*class test_table_t:public table_size_t
{
public:
  inline int aux_get_col_count()
  {
    return get_col_count();
  }
  inline void aux_set_col_count(const int a_col_count)
  {
    set_col_count(a_col_count);
  }
  inline int aux_get_row_count()
  {
    return get_row_count();
  }
  inline void aux_set_row_count(const int a_row_count)
  {
    set_row_count(a_row_count);
  }
public:
  //virtual int get_col_count() = 0;
  //virtual int get_row_count() = 0;
  virtual void set_col_count(const int a_col_count) = 0;
  virtual void set_row_count(const int a_row_count) = 0;
  //virtual void read_cell(T& a_cell, const int a_col, const int a_row) = 0;
  /*virtual void write_cell(
    const T& a_cell, const int a_col, const int a_row) = 0;*/
  //domen_read_write_t<test_table_t, int> ::
  //  property_read_write_t<aux_get_col_count, aux_set_col_count> col_count;
  /*domen_read_write_t<table_t<T>, int> ::
    property_read_write_t<aux_get_row_count, aux_set_row_count> row_count;*/
  /*test_table_t()//:col_count(*this)//, row_count(*this)
  {
  }
};  */


struct coord_cell_t
{
  int col;
  int row;
  coord_cell_t():col(0), row(0)
  {
  }
  bool operator== (coord_cell_t a_coord_cell)
  {
    if((col == a_coord_cell.col) && (row == a_coord_cell.row))
      return true;
    else
      return false;
  }
  bool operator!= (coord_cell_t a_coord_cell)
  {
    if((col != a_coord_cell.col) && (row != a_coord_cell.row))
      return true;
    else
      return false;
  }
};

class cell_double_t
{
private:
  static const double m_epsilon;
  double m_value;
  bool m_init;
public:
  cell_double_t();
  cell_double_t(double a_value, bool a_init);
  bool init();
  double value();
  irs::string str_value(
    const int a_precision = 5,
    const irs::num_mode_t a_num_mode = irs::num_mode_general,
    const irs::num_base_t a_num_base = irs::num_base_dec);
  const cell_double_t& operator=(const cell_double_t& a_cell);
  void operator=(const double& a_value);
  void operator=(const irs::string& a_str_value);
  bool operator==(const cell_double_t& a_cell) const;
  bool operator!=(const cell_double_t& a_cell) const;
  bool operator<(const cell_double_t& a_cell) const;
};

struct cell_t
{
  static const double m_epsilon;
  double value;
  bool init;
  cell_t():value(0.0), init(false)
  {}
  cell_t(double a_value, bool a_init
  ):
    value(a_value),
    init(a_init)
  {}
  const cell_t& operator=(const cell_t& a_cell)
  {
    value = a_cell.value;
    init = a_cell.init;
    return *this;
  }
  bool operator==(const cell_t& a_cell) const
  {
    if(init != a_cell.init)
      return false;
    else if((init == true) &&
      (irs::compare_value(value, a_cell.value, m_epsilon) == irs::equals_value))
      return true;
    else if (init == false)
      return true;
    else
      return false;
  }
  bool operator!=(const cell_t& a_cell) const
  {
    return !operator==(a_cell);
  }
  bool operator<(const cell_t& a_cell) const
  {
    if((init == true) && (a_cell.init == true) &&
      (irs::compare_value(value, a_cell.value, m_epsilon)
        == irs::less_than_value)){
      return true;
    }else{
      return false;
    }
  }
};

//тип перехода к следующему элементу
enum type_jump_next_elem_t{
  HORIZONTAL_DOWN,
  HORIZONTAL_DOWN_SMOOTH,
  HORIZONTAL_UP,
  HORIZONTAL_UP_SMOOTH,
  VERTICAL_FORWARD,
  VERTICAL_FORWARD_SMOOTH,
  VERTICAL_BACK,
  VERTICAL_BACK_SMOOTH
};
irs::string type_jump_next_elem_to_str(
  const type_jump_next_elem_t a_type_jump_next_elem);
type_jump_next_elem_t str_to_type_jump_next_elem(
  const irs::string a_type_jump_next_elem_str);

struct illegal_cells_t
{
  std::vector<coord_cell_t> cells;
  std::vector<int> cols;
  std::vector<int> rows;
  illegal_cells_t(
  ):
    cells(),
    cols(),
    rows()
  {
  }
};
class manager_traffic_cell_t
{
private:
  table_size_read_only_t* mp_table;
  type_jump_next_elem_t m_type_jump_next_elem;
  coord_cell_t m_current_cell;
  coord_cell_t m_start_cell;
  int min_col;
  int min_row;
  int max_col;
  int max_row;
  illegal_cells_t m_illegal_cells;
  manager_traffic_cell_t();
  enum type_border_table_t{TBT_DEFAULT, TBT_USER};
  type_border_table_t m_type_min_col;
  type_border_table_t m_type_min_row;
  type_border_table_t m_type_max_col;
  type_border_table_t m_type_max_row;
public:
  manager_traffic_cell_t(table_size_read_only_t* ap_table);
  inline void set_type_traffic(
    const type_jump_next_elem_t a_type_jump_next_elem);
  inline void set_current_cell(
    const unsigned int a_col, const unsigned int a_row);
  //установка типа движени€
  void set_traffic_smooth();
  void unset_traffic_smooth();
  bool next_cell();
  inline coord_cell_t get_coord_cell();
  void set_illegal_cell(const illegal_cells_t& a_illegal_cells);
  void set_min_col(const unsigned int a_col);
  void unset_min_col();
  void set_min_row(const unsigned int a_row);
  void unset_min_row();
  void set_max_col(const unsigned int a_col);
  void unset_max_col();
  void set_max_row(const unsigned int a_row);
  void unset_max_row();
  // ¬озвращает оставшеес€ количество €чеек относительно текущей
  // и до конца таблицы
  int get_cell_count_end();
private:
  void init_inf_table();
  // по горизонтали вниз, обычный переход
  bool next_cell_horizontal_down();
  // плавный переход
  bool next_cell_horizontal_down_smooth();
  // по горизонтали вверх, обычный переход
  bool next_cell_horizontal_up();
  // плавный переход
  bool next_cell_horizontal_up_smooth();
  // по вертикали вниз, обычный переход
  bool next_cell_vertical_forward();
  // плавный переход
  bool next_cell_vertical_forward_smooth();
  // по вертикали вверх, обычный переход
  bool next_cell_vertical_back();
  // плавный переход
  bool next_cell_vertical_back_smooth();
  // проверка на запрещенную €чейку
  bool illegal_cell(const coord_cell_t a_cell);
};
inline void manager_traffic_cell_t::set_type_traffic(
  const type_jump_next_elem_t a_type_jump_next_elem)
{
  m_type_jump_next_elem = a_type_jump_next_elem;
}
inline void manager_traffic_cell_t::set_current_cell(
  const unsigned int a_col, const unsigned int a_row)
{
  m_current_cell.col = a_col;
  m_current_cell.row = a_row;
}
inline coord_cell_t manager_traffic_cell_t::get_coord_cell()
{
  return m_current_cell;
}


class table_display_t: public table_size_t
{
public:
  virtual void add_event(
    const irs::string& a_event_name, mxfact_event_t *ap_event) = 0;
  virtual void set_col(const int a_col) = 0;
  virtual void set_row(const int a_row) = 0;
  virtual size_type get_col() = 0;
  virtual size_type get_row() = 0;
  virtual void read_cell(
    const size_type a_col,
    const size_type a_row,
    const irs::string* ap_cell) const = 0;
  virtual void write_cell(
    const size_type a_col,
    const size_type a_row,
    const irs::string& a_cell) = 0;
};

/*
template<class TYPE_1, class TYPE_2>
class short_value_cell_t
{
public:
  virtual void full_value_to_short_value(
  const TYPE_1& a_full_value, TYPE_2& a_short_value) = 0;
};    */
/*
template<class TYPE_1, class TYPE_2>
class converter_cell_t
{
  virtual TYPE_2 tdisp_cell_to_tdata_cell(const TYPE_1& a_tdisp_cell) = 0;
  virtual TYPE_1 tdata_cell_to_tdisp_cell(const TYPE_2& a_tdata_cell) = 0;
};

template<class TYPE_1, class TYPE_2>
class virtual_table_t
{
private:
  class event_out_value_cell_t: public mxfact_event_t
  {
  public:
    void exec()
    {
      mxfact_event_t::exec();
      out_value_cell();
    }
  } mp_event_out_value_cell;
  class event_out_short_value_cell_t: public mxfact_event_t
  {
  public:
    void exec()
    {
      mxfact_event_t::exec();
      out_short_value_cell();
    }
  } m_event_out_short_value_cell;
  class event_in_value_cell_t: public mxfact_event_t
  {
  public:
    void exec()
    {
      mxfact_event_t::exec();
      in_value_cell();
    }
  } m_event_in_value_cell;
  table_t<TYPE_1>* mp_table_data;
  table_display_t<TYPE_2>* mp_table_display;
  converter_cell_t<TYPE_1, TYPE_2>* mp_converter_type;
  short_value_cell_t<TYPE_1, TYPE_2>* mp_short_value;
  virtual_table_t();
public:
  virtual_table_t(
    table_t<TYPE_1>* ap_table_data,
    table_display_t<TYPE_2>* ap_table_display,
    converter_cell_t<TYPE_1, TYPE_2>* ap_converter_type,
    short_value_cell_t<TYPE_1, TYPE_2>* ap_short_value);
  void connect_short_value(
    const short_value_cell_t<TYPE_1, TYPE_2>* ap_short_value);
  void set_col_count(const int a_col_count);
  void set_row_count(const int a_row_count);
  int col_count();
  int row_count();
  void set_col(const int a_col);
  void set_row(const int a_row);
  int col();
  int row();
  void read_cell(TYPE_1& a_cell, const int a_col, const int a_row);
  void write_cell(TYPE_1& const a_cell, const int a_col, const int a_row);
private:
  void out_value_cell();
  void out_short_value_cell();
  void in_value_cell();
public:
  void out_value_cell(const int a_col, const int a_row);
  void out_short_value_cell(const int a_col, const int a_row);
  void in_value_cell(const int a_col, const int a_row);
};
template<class TYPE_1, class TYPE_2>
virtual_table_t<TYPE_1, TYPE_2>::virtual_table_t(
  table_t<TYPE_1>* ap_table_data,
  table_display_t<TYPE_2>* ap_table_display,
  converter_cell_t<TYPE_1, TYPE_2>* ap_converter_type,
  short_value_cell_t<TYPE_1, TYPE_2>* ap_short_value
):
  mp_table_data(ap_table_data),
  mp_table_display(ap_table_display),
  mp_short_value(ap_short_value)
{
  mp_table_data->add_ev_out_val_cell(&m_event_out_value_cell);
  mp_table_data->add_ev_out_short_val_cell(&m_event_out_short_value_cell);
  mp_table_data->add_ev_in_val_cell(&m_event_in_value_cell);
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::connect_short_value(
  const short_value_cell_t<TYPE_1, TYPE_2>* ap_short_value)
{
  mp_short_value(ap_short_value);
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::set_col_count(const int a_col_count)
{
  mp_table_data->set_col_count(a_col_count);
  mp_table_display->set_col_count(a_col_count);
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::set_row_count(const int a_row_count)
{
  mp_table_data->set_row_count(a_row_count);
  mp_table_display->set_row_count(a_row_count);
}

template<class TYPE_1, class TYPE_2>
int virtual_table_t<TYPE_1, TYPE_2>::col_count()
{
  int col_count_tdata = 0, col_count_t_display = 0;
  col_count_tdata = mp_table_data->col_count();
  col_count_t_display = mp_table_display->col_count();
  IRS_ASSERT(col_count_tdata == col_count_t_display);
  return col_count_tdata;
}

template<class TYPE_1, class TYPE_2>
int virtual_table_t<TYPE_1, TYPE_2>::row_count()
{
  int row_count_tdata = 0, row_count_t_display = 0;
  row_count_tdata = mp_table_data->row_count();
  row_count_t_display = mp_table_display->row_count();
  IRS_ASSERT(row_count_tdata == row_count_t_display);
  return row_count_tdata;
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::set_col(const int a_col)
{
  ap_table_display->set_col(a_col);
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::set_row(const int a_row)
{
  ap_table_display->set_row(a_row);
}

template<class TYPE_1, class TYPE_2>
int virtual_table_t<TYPE_1, TYPE_2>::col()
{
  int col = ap_table_display->col();
  return col;
}

template<class TYPE_1, class TYPE_2>
int virtual_table_t<TYPE_1, TYPE_2>::row()
{
  int row = ap_table_display->row();
  return row;
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::read_cell(
  TYPE_1& a_cell, const int a_col, const int a_row)
{
  mp_table_data->read_cell(&a_cell, a_col, a_row);
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::write_cell(
  TYPE_1& const a_cell, const int a_col, const int a_row)
{
  mp_table_data->write_cell(&a_cell, a_col, a_row);
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::out_value_cell()
{
  TYPE_1 cell_tdata;
  TYPE_2 cell_tdisplay;
  int col = mp_table_display->col();
  int row = mp_table_display->row();
  mp_table_data->read_cell(cell_tdata, col, row);
  cell_tdisplay = mp_converter_type->data_type_to_disp_type(cell_tdata);
  mp_table_display->write_cell(cell_tdisplay, col, row);
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::out_short_value_cell()
{
  TYPE_1 full_value_cell_tdata;
  TYPE_2 full_value_cell_tdisp;
  TYPE_2 short_value_cell_tdisp;
  int col = mp_table_display->col();
  int row = mp_table_display->row();
  mp_table_data->read_cell(full_value_cell_tdata, col, row);
  full_value_cell_tdisp =
    mp_converter_type->data_type_to_disp_type(full_value_cell_tdata);
  mp_short_value->full_value_to_short_value(
    full_value_cell_tdisp, short_value_cell_tdisp);
  mp_table_display->write_cell(short_value_cell_tdisp, col, row);
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::in_value_cell()
{
  TYPE_1 cell_tdata;
  TYPE_2 cell_tdisplay;
  int col = mp_table_display->col();
  int row = mp_table_display->row();
  mp_table_display->read_cell(cell_tdisplay, col, row);
  cell_tdata = mp_converter_type->disp_type_to_data_type(cell_tdisplay);
  mp_table_data->write_cell(cell_tdata, col, row);
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::out_value_cell(
  const int a_col, const int a_row)
{
  TYPE_1 cell_tdata;
  TYPE_2 cell_tdisplay;
  mp_table_data->read_cell(cell_tdata, a_col, a_row);
  cell_tdisplay = mp_converter_type->data_type_to_disp_type(cell_tdata);
  mp_table_display->write_cell(cell_tdisplay, a_col, a_row);
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::out_short_value_cell(
  const int a_col, const int a_row)
{
  TYPE_1 full_value_cell_tdata;
  TYPE_2 short_value_cell_tdisp;
  mp_table_data->read_cell(full_value_cell_tdata, a_col, a_row);
  mp_short_value->full_value_to_short_value(
    full_value_cell_tdisp, short_value_cell_tdisp);
  mp_table_display->write_cell(short_value_cell_tdisp, a_col, a_row);
}

template<class TYPE_1, class TYPE_2>
void virtual_table_t<TYPE_1, TYPE_2>::in_value_cell(
  const int a_col, const int a_row)
{
  TYPE_1 cell_tdata;
  TYPE_2 cell_tdisplay;
  mp_table_display->read_cell(cell_tdisplay, a_col, a_row);
  cell_tdata = mp_converter_type->disp_type_to_data_type(cell_tdisplay);
  mp_table_data->write_cell(cell_tdata, a_col, a_row);
}

class table_data_t:public table_t<cell_t>
{
private:
  irs::matrix_t<cell_t> m_table;
public:
  table_data_t();
  inline virtual void set_col_count(const int a_col_count);
  inline virtual void set_row_count(const int a_row_count);
  inline virtual int col_count();
  inline virtual int row_count();
  inline virtual void read_cell(cell_t& a_cell, const int col, const int row);
  inline virtual void write_cell(
    cell_t& const a_cell, const int col, const int row);
};
table_data_t::table_data_t(
):
  m_table()
{
}
inline void table_data_t::set_col_count(const int a_col_count)
{
  int col_count = 0;
  int row_count = 0;
  m_table.size(col_count, row_count);
  m_table.resize(a_col_count, row_count);
}
inline void table_data_t::set_row_count(const int a_row_count)
{
  int col_count = 0;
  int row_count = 0;
  m_table.size(col_count, row_count);
  m_table.resize(col_count, a_row_count);
}
inline int table_data_t::col_count()
{
  int col_count = 0;
  int row_count = 0;
  m_table.size(col_count, row_count);
  return col_count;
}
inline int table_data_t::row_count()
{
  int col_count = 0;
  int row_count = 0;
  m_table.size(col_count, row_count);
  return row_count;
}
inline void table_data_t::read_cell(
  cell_t& a_cell, const int col, const int row)
{
  a_cell = m_table[col][row];
}
inline void table_data_t::write_cell(
  cell_t& const a_cell, const int col, const int row)
{
  m_table[col][row] = a_cell;
}
 */





/*class sample_t
{
 	int get_method_1() { return 1; }
 	void set_method_1(const int &) { ; }
 	int get_val() { return m_val; }
 	void set_val(const int& a_val)
  {
    m_val = a_val;
  }
 public:
  int m_val;
 	domen<sample_t,int> :: property<get_method_1,set_method_1> fake_int_1;
 	domen<sample_t,int> :: property<get_val,set_val> val;

 	sample_t() : m_val(0), fake_int_1(*this), val(*this) { ; }
};

*/
template<typename owner_t, typename value_t>
struct domen
{
        typedef value_t (owner_t::*get_method_t) (void);
        typedef void (owner_t::*set_method_t) (const value_t &);

	template<get_method_t get_method, set_method_t set_method>
	class property
	{
	 	owner_t & owner;
	 public:
  	 	property(owner_t & _owner) : owner(_owner) { ; }
	 	operator value_t () const { return (owner.*get_method)();}
	 	value_t operator = (const value_t & value)
	 	{
	 		(owner.*set_method)(value);
	 		return (owner.*get_method)();
	 	}
	};
};
class vsample_t
{
//protected:
public:


 	virtual int vget_val() = 0;
  virtual void vset_val(const int& a_val) = 0;
  inline int get_val()
  {
    return vget_val();
  }
  inline void set_val(const int& a_val)
  {
    vset_val(a_val);
  }
public:
  //int m_val;
 	domen<vsample_t,int> :: property<get_val,set_val> val;
 	vsample_t():val(*this)
  {}
};

template<class T>
class ex_t
{
  T val;
public:
  void set_metod (const T a_val);
};

template<class T>
typedef void (ex_t<T>::*set_metod_t) (const T);
//template<class T>
//set_metod_t<T> ptr_set_metod = ex_t<T>::set_metod;
template<class T>
class test2_t
{
  set_metod_t m_set_metod;
public:

  test2_t()
  {}
  void m()
  {
    //T.*a_set_metod(1);
  }
};

template<class owner_t, class value_t>
struct test3_t
{
  typedef value_t (owner_t::*get_method_t) (void);
	template<get_method_t get_method>
	class property_read_only_t
	{
	 	owner_t& m_owner;
	 public:
  	property_read_only_t(owner_t& a_owner) : m_owner(a_owner) { ; }
	 	operator value_t () const { return (m_owner.*get_method)();}
	};
  typedef property_read_only_t property_t;
};

//test2_t<ex_t<int>, ex_t<int>::set_metod> test2;












#endif

