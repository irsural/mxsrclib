//! \file
//! \ingroup single_type_group
//! \brief MTL version 2.0
//!
//! ƒата: 19.05.2010
//! –анн€€ дата: 14.04.2010

#ifndef irsmatrixH
#define irsmatrixH

#include <irsdefs.h>

#include <math.h>
#include <vector>

#include <irsstd.h>

#include <irsfinal.h>

namespace irs {

//! \ingroup single_type_group
template<class TYPE>
class matrix_t;
template<class TYPE>
void meshgrid(
  const matrix_t<TYPE>& a_in_x,
  const matrix_t<TYPE>& a_in_y,
  matrix_t<TYPE>& a_out_x,
  matrix_t<TYPE>& a_out_y);

template<class TYPE>
class index_matrix2_t
{
private:
  vector<TYPE>& m_vec;
  int m_index1;
  int m_index2;
  int& m_col_count;
public:
  index_matrix2_t(vector<TYPE>& a_vec, int& a_col_count);
  //оператор записи
  const TYPE& operator=(const TYPE& a_val);
  //оператор чтени€
  operator TYPE&();
  void set_index(const int a_index1 ,const int a_index2);
};

template<class TYPE>
index_matrix2_t<TYPE>::index_matrix2_t(vector<TYPE>& a_vec,
  int& a_col_count):
  m_vec(a_vec),
  m_index1(0),
  m_index2(0),
  m_col_count(a_col_count)
{
}

template<class TYPE>
const TYPE& index_matrix2_t<TYPE>::operator=(const TYPE& a_val)
{
  m_vec[m_index2 * m_col_count + m_index1] = a_val;
  return a_val;
}

template<class TYPE>
index_matrix2_t<TYPE>::operator TYPE&()
{
  return m_vec[m_index2 * m_col_count + m_index1];
}

template<class TYPE>
void index_matrix2_t<TYPE>::set_index(const int a_index1, const int a_index2)
{
  m_index1 = a_index1;
  m_index2 = a_index2;
}

template<class TYPE>
class index_matrix_t
{
private:
  vector<TYPE>& m_vec;
  index_matrix2_t<TYPE> m_index_matrix2;
  int m_index1;
public:
  index_matrix_t(vector<TYPE>& a_vec, int& a_col_count);
  //оператор записи
  const TYPE& operator=(const TYPE& a_val);
  //оператор чтени€
  operator TYPE();
  index_matrix2_t<TYPE>& operator[](int a_index2);
  void set_index(const int a_index1);
};
template<class TYPE>
index_matrix_t<TYPE>::index_matrix_t(
  vector<TYPE>& a_vec,
  int& a_col_count):
  m_vec(a_vec),
  m_index_matrix2(a_vec, a_col_count),
  m_index1(0)
{
}
template<class TYPE>
const TYPE& index_matrix_t<TYPE>::operator=(const TYPE& a_val)
{
  m_vec[m_index1] = a_val;
  return a_val;
}
template<class TYPE>
index_matrix_t<TYPE>::operator TYPE()
{
  return m_vec[m_index1];
}

template<class TYPE>
index_matrix2_t<TYPE>& index_matrix_t<TYPE>::operator[](int a_index2)
{
  m_index_matrix2.set_index(m_index1, a_index2);
  return m_index_matrix2;
}
template<class TYPE>
void index_matrix_t<TYPE>::set_index(const int a_index1)
{
  m_index1 = a_index1;
}

//! ingroup single_type_group
template<class TYPE>
class matrix_t
{
private:
  vector<TYPE> mv_elem;
  int m_row_count;
  int m_col_count;
  index_matrix_t<TYPE> m_index_matrix;
public:
  matrix_t();
  matrix_t(const int a_col_count, const int a_row_count);
  matrix_t(const int a_col_count, const int a_row_count, const TYPE& a_value);
  matrix_t(const matrix_t<TYPE>& a_matrix);
  matrix_t(const vector<TYPE>& av_elem, int a_col_count);
  inline void size(int& a_col_count, int& a_row_count) const;
  inline void resize(const int a_col_count, const int a_row_count);
  inline int col_count() const;
  inline int row_count() const;
  inline TYPE operator()(int a_i) const;
  inline index_matrix_t<TYPE> operator[](int a_index) const;
  inline void newcols(
    const int a_index_pos,
    const unsigned int a_new_col_count,
    const TYPE& a_value);
  inline void newrows(
    const int a_index_pos,
    const unsigned int a_new_row_count,
    const TYPE& a_value);
  inline void delcols(const int a_index_begin, const int a_index_end);
  inline void delrows(const int a_index_begin, const int a_index_end);
  inline const matrix_t<TYPE>& operator=(const matrix_t<TYPE>& a_matrix);
  inline matrix_t<TYPE> operator+(const matrix_t<TYPE> a_matrix) const;
  inline void operator+=(const matrix_t<TYPE>& a_matrix);
  inline matrix_t<TYPE> operator-(const matrix_t<TYPE> a_matrix) const;
  inline void operator-=(const matrix_t<TYPE>& a_matrix);
  inline matrix_t<TYPE> operator*(const matrix_t<TYPE> a_matrix) const;
  inline matrix_t<TYPE> operator*(const TYPE& a_value) const;
  inline bool operator==(const matrix_t<TYPE>& a_matrix) const;
  inline bool operator!=(const matrix_t<TYPE>& a_matrix) const;
  inline void transp();
  friend void meshgrid<>(
    const matrix_t<TYPE>& a_in_x,
    const matrix_t<TYPE>& a_in_y,
    matrix_t<TYPE>& a_out_x,
    matrix_t<TYPE>& a_out_y);
};

template<class TYPE>
matrix_t<TYPE>::matrix_t():
  mv_elem(),
  m_row_count(0),
  m_col_count(0),
  m_index_matrix(mv_elem, m_col_count)
{
}

template<class TYPE>
matrix_t<TYPE>::matrix_t(const int a_col_count, const int a_row_count):
  mv_elem(a_col_count * a_row_count),
  m_row_count(a_row_count),
  m_col_count(a_col_count),
  m_index_matrix(mv_elem, m_col_count)
{
}

template<class TYPE>
matrix_t<TYPE>::matrix_t(const int a_col_count, const int a_row_count,
  const TYPE& a_value):
  mv_elem(a_col_count * a_row_count, a_value),
  m_row_count(a_row_count),
  m_col_count(a_col_count),
  m_index_matrix(mv_elem, m_col_count)
{
}

template<class TYPE>
matrix_t<TYPE>::matrix_t(const matrix_t<TYPE>& a_matrix):
  mv_elem(a_matrix.mv_elem),
  m_row_count(a_matrix.m_row_count),
  m_col_count(a_matrix.m_col_count),
  m_index_matrix(mv_elem, m_col_count)
{
}

template<class TYPE>
matrix_t<TYPE>::matrix_t(const vector<TYPE>& av_elem, int a_col_count):
  mv_elem(av_elem),
  m_index_matrix(mv_elem, m_col_count)
{
  m_row_count = mv_elem.size() / a_col_count;
  m_col_count = a_col_count;
}

template<class TYPE>
inline void matrix_t<TYPE>::size(int& a_col_count, int& a_row_count) const
{
  a_col_count = m_col_count;
  a_row_count = m_row_count;
}
template<class TYPE>
inline void matrix_t<TYPE>::resize(const int a_col_count, const int a_row_count)
{
  unsigned int size = a_col_count*a_row_count;
  mv_elem.resize(size);
  m_col_count = a_col_count;
  m_row_count = a_row_count;
}

template<class TYPE>
inline int matrix_t<TYPE>::col_count() const
  {return m_col_count;}

template<class TYPE>
inline int matrix_t<TYPE>::row_count() const
  {return m_row_count;}

template<class TYPE>
inline TYPE matrix_t<TYPE>::operator()(int a_i) const
{
  int row = a_i % m_row_count;
  int col = (a_i - row) / m_row_count;
  return mv_elem[mv_elem.col*row+col];
}

template<class TYPE>
inline index_matrix_t<TYPE> matrix_t<TYPE>::operator[](int a_index) const
{
  m_index_matrix.set_index(a_index);
  return m_index_matrix;
}

template<class TYPE>
inline void matrix_t<TYPE>::newcols(
  const int a_index_pos,
  const unsigned int a_new_col_count,
  const TYPE& a_value)
{
  if(a_index_pos <= m_col_count){
    int end_index_new_cols = a_index_pos+a_new_col_count-1;
    m_col_count += a_new_col_count;
    vector<TYPE> vec(m_col_count*m_row_count);
    int i1 = 0, i2 = 0;
    for(int y = 0; y < m_row_count; y++){
      for(int x = 0; x < m_col_count; x++){
        if(x < a_index_pos || x > end_index_new_cols){
          vec[i1] = mv_elem[i2];
          i2++;
        }else{
          vec[i1] = a_value;
        }
        i1++;
      }
    }
    mv_elem = vec;
  }
}

template<class TYPE>
inline void matrix_t<TYPE>::newrows(
  const int a_index_pos,
  const unsigned int a_new_row_count,
  const TYPE& a_value)
{
  if(a_index_pos <= m_row_count){
    mv_elem.insert(
      mv_elem.begin()+m_col_count*a_index_pos,
      m_col_count*a_new_row_count,
      a_value);
    m_row_count += a_new_row_count;
  }
}
template<class TYPE>
inline void matrix_t<TYPE>::delcols(
  const int a_index_begin, const int a_index_end)
{
  if(a_index_begin < m_col_count && a_index_end < m_col_count){
    int col_count = m_col_count-((a_index_end-a_index_begin)+1);
    vector<TYPE> vec(col_count*m_row_count);
    int i1 = 0, i2 = 0;
    for(int y = 0; y < m_row_count; y++){
      for(int x = 0; x < m_col_count; x++){
        if(x < a_index_begin || x > a_index_end){
          vec[i1] = mv_elem[i2];
          i1++;
        }
        i2++;
      }
    }
    mv_elem = vec;
    m_col_count = col_count;
    if(m_col_count == 0)
      m_row_count = 0;
  }
}

template<class TYPE>
inline void matrix_t<TYPE>::delrows(
  const int a_index_begin, const int a_index_end)
{
  if(a_index_begin < m_row_count && a_index_end < m_row_count){
    mv_elem.erase(
      mv_elem.begin()+m_col_count*a_index_begin,
      mv_elem.begin()+m_col_count*(a_index_end+1));
    m_row_count = m_row_count-((a_index_end-a_index_begin)+1);
    if(m_row_count == 0)
      m_col_count = 0;
  }
}

template<class TYPE>
inline const matrix_t<TYPE>& matrix_t<TYPE>::operator=(
  const matrix_t<TYPE>& a_matrix)
{
  mv_elem = a_matrix.mv_elem;
  m_col_count = a_matrix.m_col_count;
  m_row_count = a_matrix.m_row_count;
  return *this;
}

template<class TYPE>
inline matrix_t<TYPE> matrix_t<TYPE>::
  operator+(const matrix_t<TYPE> a_matrix) const
{
  matrix_t<TYPE> matrix(m_col_count,m_row_count);
  if(m_col_count == a_matrix.m_col_count &&
    m_row_count == a_matrix.m_row_count){
    int length = m_col_count*m_row_count;
    for(int i = 0; i < length; i++){
      matrix.mv_elem[i] = mv_elem[i] + a_matrix.mv_elem[i];
    }
  }
  return matrix;
}

template<class TYPE>
inline void matrix_t<TYPE>::operator+=(const matrix_t<TYPE>& a_matrix)
{
  if(m_col_count == a_matrix.m_col_count &&
    m_row_count == a_matrix.m_row_count){
    int length = m_col_count*m_row_count;
    for(int i = 0; i < length; i++)
      mv_elem[i] += a_matrix.mv_elem[i];
  }
}

template<class TYPE>
inline matrix_t<TYPE> matrix_t<TYPE>::
  operator-(const matrix_t<TYPE> a_matrix) const
{
  matrix_t<TYPE> matrix(m_col_count,m_row_count);
  if(m_col_count == a_matrix.m_col_count &&
    m_row_count == a_matrix.m_row_count){
    int length = m_col_count*m_row_count;
    for(int i = 0; i < length; i++)
      matrix.mv_elem[i] = mv_elem[i] - a_matrix.mv_elem[i];
  }
  return matrix;
}

template<class TYPE>
inline void matrix_t<TYPE>::operator-=(const matrix_t<TYPE>& a_matrix)
{
  if(m_col_count == a_matrix.m_col_count &&
    m_row_count == a_matrix.m_row_count){
    int length = m_col_count*m_row_count;
    for(int i = 0; i < length; i++)
      mv_elem[i] -= a_matrix.mv_elem[i];
  }
}

template<class TYPE>
inline matrix_t<TYPE> matrix_t<TYPE>::
  operator*(const matrix_t<TYPE> a_matrix) const
{
  matrix_t<TYPE> matrix(m_row_count, a_matrix.m_col_count);
  TYPE cur_elem = 0;
  if(m_col_count == a_matrix.m_row_count){
    int i = 0;
    int index_row1 = 0;
    for(int y1 = 0; y1 < m_row_count; y1++){
      index_row1 = m_col_count*y1;
      for(int x2 = 0; x2 < a_matrix.m_col_count; x2++){
        for(int x1_y2 = 0; x1_y2 < m_col_count; x1_y2++){
          cur_elem += mv_elem[index_row1+x1_y2]*
            a_matrix.mv_elem[a_matrix.m_col_count*x1_y2+x2];
        }
        matrix.mv_elem[i] = cur_elem;
        cur_elem = 0;
        i++;
      }
    }
  }
  return matrix;
}
template<class TYPE>
inline matrix_t<TYPE> matrix_t<TYPE>::operator*(const TYPE& a_value) const
{
  matrix_t<TYPE> matrix(m_col_count, m_row_count);
  size_t length = mv_elem.size();
  for(size_t i = 0; i < length; i++) {
    matrix.mv_elem[i] = mv_elem[i]*a_value;
  }
  return matrix;
}
template<class TYPE>
inline bool matrix_t<TYPE>::operator==(const matrix_t<TYPE>& a_matrix) const
{
  return ((mv_elem == a_matrix.mv_elem) &&
    (m_col_count == a_matrix.m_col_count) &&
    (m_row_count == a_matrix.m_row_count));
}
template<class TYPE>
inline bool matrix_t<TYPE>::operator!=(const matrix_t<TYPE>& a_matrix) const
{
  return !((mv_elem == a_matrix.mv_elem) &&
    (m_col_count == a_matrix.m_col_count) &&
    (m_row_count == a_matrix.m_row_count));
}
template<class TYPE>
inline void matrix_t<TYPE>::transp()
{
  int index = 0;
  for(int col = 0; col < m_col_count; col++) {
    for(int row = 0; row < m_row_count; row++) {
      mv_elem[index] = mv_elem[m_col_count*row+col];
      index++;
    }
  }
  int new_col_count = m_row_count;
  m_row_count = m_col_count;
  m_col_count = new_col_count;
}

template<class TYPE>
void meshgrid(
  const matrix_t<TYPE>& a_in_x,
  const matrix_t<TYPE>& a_in_y,
  matrix_t<TYPE>& a_out_x,
  matrix_t<TYPE>& a_out_y)
{
  a_out_x.mv_elem.clear();
  a_out_y.mv_elem.clear();
  matrix_t<TYPE> matrix_x;
  matrix_t<TYPE> matrix_y;
  for(int x = 0; x < a_in_x.m_col_count; x++)
    for(int y = 0; y < a_in_x.m_row_count; y++)
      matrix_x.mv_elem.push_back(a_in_x.mv_elem[a_in_x.m_col_count*y+x]);
  matrix_x.m_col_count = matrix_x.mv_elem.size();
  matrix_x.m_row_count = 1;
  for(int x = 0; x < a_in_y.m_col_count; x++)
    for(int y = 0; y < a_in_y.m_row_count; y++)
      matrix_y.mv_elem.push_back(a_in_y.mv_elem[a_in_y.m_col_count*y+x]);
  matrix_y.m_col_count = 1;
  matrix_y.m_row_count = matrix_y.mv_elem.size();
  for(int n = 0; n < matrix_y.m_row_count; n++)
    a_out_x.mv_elem.insert(
      a_out_x.mv_elem.end(),
      matrix_x.mv_elem.begin(),
      matrix_x.mv_elem.end());
  a_out_x.m_col_count = matrix_x.m_col_count;
  a_out_x.m_row_count = matrix_y.m_row_count;
  for(int n = 0; n < matrix_y.m_row_count; n++)
    a_out_y.mv_elem.insert(
      a_out_y.mv_elem.end(),
      matrix_x.m_col_count,
      matrix_y[n]);
  a_out_y.m_col_count = matrix_x.m_col_count;
  a_out_y.m_row_count = matrix_y.m_row_count;
}

template<class TYPE>
inline matrix_t<TYPE> operator*(const TYPE& a_number,
  const matrix_t<TYPE>& a_matrix)
{
  int row_count = a_matrix.row_count();
  int col_count = a_matrix.col_count();
  matrix_t<TYPE> matrix_result(col_count, row_count);
  for (int row = 0; row < row_count; row++) {
    for (int col = 0; col < col_count; col++) {
      matrix_result[col][row] = a_number*a_matrix[col][row];
    }
  }
  return matrix_result;
}

} // namespace irs

#endif //irsmatrixH

