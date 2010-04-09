// Абстракция таблицы Лящова Максима
// Дата: 15.03.2010
// Ранняя дата: 17.09.2009

#ifndef tablegH
#define tablegH

#include <irscpp.h>
#include <irsstdg.h>
#include <irserror.h>
#include <irsmath.h>
#include <irssysutils.h>
#include <irsmatrix.h>
//---------------------------------------------------------------------------

namespace irs {

#ifndef __WATCOMC__

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

// Class table_t
// Примеры использования
// ex1: table_t<int, vector<int>, vector<vector<int> > > table;
// ex2: table_t<int, deque<int>, list<deque<int> > > table;
template<
  class cell_type_t,
  class column_type_t = deque<cell_type_t>,
  class container_t = deque<deque<cell_type_t> > >
class table_t: public table_size_t
{
public:
  //typedef typename container_t::size_type size_type;
  typedef typename container_t::iterator it_container_t;
  typedef typename container_t::const_iterator it_const_container_t;
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
  inline cell_type_t& read_cell(
    const size_type a_col_index, const size_type a_row_index);
  inline void write_cell(
    const size_type a_col_index,
    const size_type a_row_index,
    const cell_type_t& a_cell);
  virtual size_type get_col_count() const;
  virtual size_type get_row_count() const;
  virtual inline void set_col_count(const size_type a_col_count);
  virtual inline void set_row_count(const size_type a_row_count);
  template<class it_col_type_t>
  void col_push_front(it_col_type_t a_first_elem);
  template<class it_col_type_t>
  void col_push_back(it_col_type_t a_first_elem);
  template<class it_row_type_t>
  void row_push_front(it_row_type_t a_first_elem);
  template<class it_row_type_t>
  void row_push_back(it_row_type_t a_first_elem);
  // Возвращает первый столбец
  template<class col_type_t>
  inline col_type_t col_front() const;
  // Возвращает последний столбец
  template<class col_type_t>
  inline col_type_t col_back() const;
  // Возвращает первую строку
  template<class row_type_t>
  inline row_type_t row_front() const;
  // Возвращает последнюю строку
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
  it_const_container_t it_col = m_container.begin();
  advance(it_col, a_col_index);
  it_const_column_t it_elem_col = it_col->begin();
  advance(it_elem_col, a_row_index);
  return *it_elem_col;
}

template<class cell_type_t, class column_type_t, class container_t>
inline cell_type_t& table_t<cell_type_t, column_type_t, container_t>::
read_cell(const size_type a_col_index, const size_type a_row_index)
{
  IRS_LIB_ASSERT(a_col_index < m_col_count);
  IRS_LIB_ASSERT(a_row_index < m_row_count);
  it_container_t it_col = m_container.begin();
  advance(it_col, a_col_index);
  it_column_t it_elem_col = it_col->begin();
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
table_size_t::size_type
table_t<cell_type_t, column_type_t, container_t>::get_col_count() const
{
  IRS_LIB_ASSERT(m_col_count == m_container.size());
  return m_col_count;
}

template<class cell_type_t, class column_type_t, class container_t>
table_size_t::size_type
table_t<cell_type_t, column_type_t, container_t>::get_row_count() const
{
  #ifdef IRS_LIB_DEBUG
  if (m_container.size() > 0) {
    IRS_LIB_ASSERT(m_row_count == m_container.begin()->size());
  } else {
    // Количество строк может быть любым
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
  //  добавлено "int"
  const int index_first_elem = 0;
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
  //  добавлено "int"
  const int index_first_elem = 0;
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
  it_const_container_t it_col = m_container.begin();
  it_const_container_t it_end_col = m_container.end();
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
  swap(*it_col1, *it_col2);
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
  copy_n(
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
  copy_n(a_first_elem, m_row_count, inserter(*it_col, it_col->begin()));
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
  deque<irs::string>,
  deque<deque<irs::string> > > table_string_t;

template <class cell_type_t>
class composite_table_t : public table_size_t
{
public:
  typedef table_size_t::size_type size_type;
  typedef irs::rect_t diapason_type;
  struct cell_param_t
  {
    bool autonomous;
    diapason_type diapason;
    cell_param_t():
      autonomous(true),
      diapason()
    {
    }
  };
  struct cell_t
  {
    cell_type_t value;
    cell_param_t param;
  };
  composite_table_t();
  composite_table_t(const size_type a_col_count, const size_type a_row_count);
  inline const cell_type_t& read_cell(
    const size_type a_col_index, const size_type a_row_index) const;
  inline void write_cell(
    const size_type a_col_index,
    const size_type a_row_index,
    const cell_type_t& a_cell);
  virtual size_type get_col_count() const;
  virtual size_type get_row_count() const;
  virtual inline void set_col_count(const size_type a_col_count);
  virtual inline void set_row_count(const size_type a_row_count);
  inline void resize(size_type a_new_col_count, size_type a_new_row_count);
  inline void union_on(const diapason_type& a_diapason);
  inline void union_off(const diapason_type& a_diapason);
  inline bool is_diapason_for_union_valid(
    const diapason_type& a_diapason) const;
  inline bool is_cell_united(const size_type a_col_index,
    const size_type a_row_index) const;
  inline diapason_type get_cell_diapason(const size_type a_col_index,
    const size_type a_row_index) const;
private:
  // Первый диапазон является поддиапазоном второго
  inline bool first_subdiapason_second(const diapason_type& a_first_diapason,
    const diapason_type& a_second_diapason) const;
  irs::table_t<cell_t> m_table;
};

template <class cell_type_t>
composite_table_t<cell_type_t>::composite_table_t():
  m_table()
{
}

template <class cell_type_t>
composite_table_t<cell_type_t>::composite_table_t(
  const size_type a_col_count,
  const size_type a_row_count
):
  m_table(a_col_count, a_row_count)
{
}

template <class cell_type_t>
inline const cell_type_t&
composite_table_t<cell_type_t>::read_cell(const size_type a_col_index,
  const size_type a_row_index) const
{
  const cell_t& cell = m_table.read_cell(a_col_index, a_row_index);
  size_type final_col_index = a_col_index;
  size_type final_row_index = a_row_index;
  if (!cell.param.autonomous) {
    final_col_index = cell.param.diapason.left;
    final_row_index = cell.param.diapason.top;
  } else {
    // Оставляем прежние координаты ячейки
  }
  return m_table.read_cell(final_col_index, final_row_index).value;
}

template <class cell_type_t>
inline void composite_table_t<cell_type_t>::
write_cell(
  const size_type a_col_index,
  const size_type a_row_index,
  const cell_type_t& a_cell)
{
  cell_t cell = m_table.read_cell(a_col_index, a_row_index);
  cell.value = a_cell;
  size_type final_col_index = a_col_index;
  size_type final_row_index = a_row_index;
  if (!cell.param.autonomous) {
    final_col_index = cell.param.diapason.left;
    final_row_index = cell.param.diapason.top;
  } else {
    // Оставляем прежние координаты ячейки
  }
  m_table.write_cell(final_col_index, final_row_index, cell);
}

template <class cell_type_t>
typename composite_table_t<cell_type_t>::size_type
composite_table_t<cell_type_t>::get_col_count() const
{
  return m_table.get_col_count();
}

template <class cell_type_t>
typename composite_table_t<cell_type_t>::size_type
composite_table_t<cell_type_t>::get_row_count() const
{
  return m_table.get_row_count();
}

template <class cell_type_t>
inline void composite_table_t<cell_type_t>::
set_col_count(const size_type a_col_count)
{
  const size_type col_count = get_col_count();
  const size_type row_count = get_row_count();
  if (a_col_count < col_count) {
    for (size_type col_i = 0; col_i < a_col_count; col_i++) {
      for (size_type row_i = 0; row_i < row_count; row_i++) {
        cell_t& cell = m_table.read_cell(col_i, row_i);
        if (!cell.param.autonomous) {
          if (cell.param.diapason.right >= a_col_count)
          {
            cell.param.diapason.right = a_col_count - 1;
          } else {
            // Корректировать границы диапазона не требуется
          }
        } else {
          // Для автономной ячейки не требуется изменять диапазон
        }
      }
    }
  } else {
    // Никаких дополнительных действий не требуется
  }
  m_table.set_col_count(a_col_count);
}

template <class cell_type_t>
inline void composite_table_t<cell_type_t>::set_row_count(
  const size_type a_row_count)
{
  const size_type col_count = get_col_count();
  const size_type row_count = get_row_count();
  if (a_row_count < row_count) {
    for (size_type col_i = 0; col_i < col_count; col_i++) {
      for (size_type row_i = 0; row_i < a_row_count; row_i++) {
        cell_t& cell = m_table.read_cell(col_i, row_i);
        if (!cell.param.autonomous) {
          if (cell.param.diapason.bottom >= a_row_count)
          {
            cell.param.diapason.bottom = a_row_count - 1;
          } else {
            // Корректировать границы диапазона не требуется
          }
        } else {
          // Для автономной ячейки не требуется изменять диапазон
        }
      }
    }
  } else {
    // Никаких дополнительных действий не требуется
  }
  m_table.set_row_count(a_row_count);
}

template <class cell_type_t>
inline void composite_table_t<cell_type_t>::resize(
  size_type a_new_col_count, size_type a_new_row_count)
{
  set_row_count(a_new_col_count);
  set_col_count(a_new_row_count);
}

template <class cell_type_t>
inline void composite_table_t<cell_type_t>::union_on(
  const diapason_type& a_diapason)
{
  if (is_diapason_for_union_valid(a_diapason)) {
    for (size_type col_i = a_diapason.left; col_i <= a_diapason.right; col_i++)
    {
      for (size_type row_i = a_diapason.top; row_i <= a_diapason.bottom;
        row_i++)
      {
        cell_t cell = m_table.read_cell(col_i, row_i);
        cell.param.autonomous = false;
        cell.param.diapason = a_diapason;
        m_table.write_cell(col_i, row_i, cell);
      }
    }
  } else {
    IRS_LIB_ASSERT_MSG("Указан недопустимый диапазон");
  }
}

template <class cell_type_t>
inline void composite_table_t<cell_type_t>::union_off(
  const diapason_type& a_diapason)
{
  cell_param_t param_for_autonomous;
  param_for_autonomous.autonomous = true;
  for (size_type col_i = a_diapason.left; col_i <= a_diapason.right; col_i++)
  {
    for (size_type row_i = a_diapason.top; row_i <= a_diapason.bottom; row_i++)
    {
      const cell_t& cell = m_table.read_cell(col_i, row_i);
      if (!cell.param.autonomous) {
        diapason_type diapason = cell.param.diapason;
        for (size_type diapason_col_i = diapason.left;
          diapason_col_i <= diapason.right;
          diapason_col_i++)
        {
          for (size_type diapason_row_i = diapason.top;
            diapason_row_i <= diapason.bottom;
            diapason_row_i++)
          {
            cell_t& cell_from_subdiapason =
              m_table.read_cell(diapason_col_i, diapason_row_i);
            cell_from_subdiapason.param.autonomous = true;
          }
        }
      } else {
        // Ячейка уже автономная
      }
    }
  }
}

template <class cell_type_t>
inline bool composite_table_t<cell_type_t>::is_diapason_for_union_valid(
  const diapason_type& a_diapason) const
{
  bool diapason_success = true;
  if ((a_diapason.width() <= 1) && (a_diapason.height() <= 1)) {
    diapason_success = false;
  } else {
    // Диапазон охватывает более одной ячеки
  }
  for (size_type col_i = a_diapason.left; col_i <= a_diapason.right; col_i++)
  {
    for (size_type row_i = a_diapason.top; row_i <= a_diapason.bottom; row_i++)
    {
      const cell_t& cell = m_table.read_cell(col_i, row_i);
      if (!cell.param.autonomous) {
        if (!first_subdiapason_second(cell.param.diapason, a_diapason)) {
          diapason_success = false;
          break;
        } else {
          // Диапазон ячейки входит в диапазон для текущего объединения ячеек
        }
      } else {
        // Автономная ячейка не требует проверки диапазона
      }
    }
  }
  return diapason_success;
}

template <class cell_type_t>
inline bool composite_table_t<cell_type_t>::is_cell_united(
  const size_type a_col_index,
  const size_type a_row_index) const
{
  const cell_t& cell = m_table.read_cell(a_col_index, a_row_index);
  return !cell.param.autonomous;
}

template <class cell_type_t>
inline typename composite_table_t<cell_type_t>::diapason_type
composite_table_t<cell_type_t>::get_cell_diapason(
  const size_type a_col_index,
  const size_type a_row_index) const
{
  diapason_type diapason(a_col_index, a_row_index, 1, 1);
  const cell_t& cell = m_table.read_cell(a_col_index, a_row_index);
  if (!cell.param.autonomous) {
    diapason = cell.param.diapason;
  } else {
    IRS_LIB_ASSERT_MSG("Ячейка не объединена");
  }
  return diapason;
}

template <class cell_type_t>
inline bool composite_table_t<cell_type_t>::first_subdiapason_second(
  const diapason_type& a_first_diapason,
  const diapason_type& a_second_diapason) const
{
  bool statement_true = false;
  //const size_type first_right = a_first_diapason.left + a_first_diapason.width;
  //const size_type second_right = a_second_diapason.left +
    //a_second_diapason.width;
  //const size_type first_bottom = a_first_diapason.top + a_first_diapason.height;
  //const size_type second_bottom = a_second_diapason.top +
    //a_second_diapason.height;
  if ((a_first_diapason.left >= a_second_diapason.left) &&
    (a_first_diapason.right <= a_second_diapason.right) &&
    (a_first_diapason.top >= a_second_diapason.top) &&
    (a_first_diapason.bottom <= a_second_diapason.bottom))
  {
    statement_true = true;
  } else {
    statement_true = false;
  }
  return statement_true;
}

/*class table_string_t:public
  table_t<
    irs::string,
    deque<irs::string>,
    deque<deque<irs::string> > >
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
  //virtual void write_cell(
    //const T& a_cell, const int a_col, const int a_row) = 0;
  //domen_read_write_t<test_table_t, int> ::
  //  property_read_write_t<aux_get_col_count, aux_set_col_count> col_count;
  //domen_read_write_t<table_t<T>, int> ::
    property_read_write_t<aux_get_row_count, aux_set_row_count> row_count;
  //test_table_t()//:col_count(*this)//, row_count(*this)
  {
  }
};  */



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
/*
class vsample_t
{
//protected:
public:


 	virtual int vget_val() = 0;
  virtual void vset_val(const int& a_val) = 0;
  int get_val();
  #ifdef NOP
  int get_val()
  {
    return vget_val();
  }
  #endif //NOP
  void set_val(const int& a_val)
  {
    vset_val(a_val);
  }
public:
  //int m_val;
 	domen<vsample_t,int> :: property<get_val,set_val> val;
 	vsample_t():val(*this)
  {}
};
inline int vsample_t::get_val()
{
  return vget_val();
}

*/
template<class T>
class ex_t
{
  T val;
public:
  void set_metod (const T a_val);
};

//template<class T>
//set_metod_t<T> ptr_set_metod = ex_t<T>::set_metod;
template<class T>
class test2_t
{
  typedef void (ex_t<T>::*set_metod_t) (const T);

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
  //typedef property_read_only_t property_t;
};

//test2_t<ex_t<int>, ex_t<int>::set_metod> test2;

#endif //__WATCOMC__

}; // namespace irs

#endif //tablegH

