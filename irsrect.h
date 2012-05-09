//! \file
//! \ingroup single_type_group
//! \brief Rect
//!
//! Дата: 06.05.2011\n
//! Дата создания: 15.04.2010

#ifndef IRSRECTH
#define IRSRECTH

#include <irsdefs.h>

#include <irscpp.h>
#include <irslimits.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup single_type_group
//! @{

struct point_t {
  typedef irs_size_t size_type;

  size_type left;
  size_type top;

  point_t(
    size_type a_left = 0,
    size_type a_top = 0
  ):
    left(a_left),
    top(a_top)
  {}
  inline bool operator==(const point_t& a_point);
  inline bool operator!=(const point_t& a_point);
};

inline bool point_t::operator==(const point_t& a_point)
{
  return (left == a_point.left) && (top == a_point.top);
}

inline bool point_t::operator!=(const point_t& a_point)
{
  return !operator==(a_point);
}

struct rect_t {
  typedef irs_size_t size_type;
  typedef irs_ptrdiff_t difference_type;

  size_type left;
  size_type top;
  size_type right;
  size_type bottom;

  rect_t(
    size_type a_left = 0,
    size_type a_top = 0,
    size_type a_right = 0,
    size_type a_bottom = 0
  ):
    left(a_left),
    top(a_top),
    right(a_right),
    bottom(a_bottom)
  {}
  rect_t(const rect_t& a_rect):
    left(a_rect.left),
    top(a_rect.top),
    right(a_rect.right),
    bottom(a_rect.bottom)
  {}
  inline bool operator==(const rect_t& a_rect) const;
  inline bool operator!=(const rect_t& a_rect) const;
  inline size_type width() const;
  inline size_type height() const;
  //! \brief Перемещает диапазон в точку с координатами (a_left, a_top).
  inline void move_to(const size_type a_left, const size_type a_top);
  //! \brief Перемещает дипазон в точку a_left_top.
  inline void move_to(const point_t& a_left_top);
  inline void move_left(const size_type a_left);
  inline void move_top(const size_type a_top);
  inline void move_right(const size_type a_right);
  inline void move_bottom(const size_type a_bottom);
  inline void offset(const difference_type a_x_diff,
    const difference_type a_y_diff);
  inline void union_with(const rect_t& a_rect);
  inline rect_t get_united(const rect_t& a_rect) const;
  //! \brief Возвращает true, если точка с координатами (a_left, a_top)
  //!   входит в диапазон.
  inline bool contains(const size_type a_left, const size_type a_top) const;
  //! \brief Возвращает true, если точка a_point входит в диапазон.
  inline bool contains(const point_t& a_point) const;
  //! \brief Возвращает true, если диапазон a_rect является поддиапазоном.
  inline bool contains(const rect_t& a_rect) const;
  //intersect
  //get_intersect
  inline bool intersects_with(const rect_t& a_rect) const;
};

// rect_intersect & | &= |= == != += -=

inline rect_t rect_union(const rect_t& a_first_rect,
  const rect_t& a_second_rect)
{
  return a_first_rect.get_united(a_second_rect);
}

inline bool rect_t::operator==(const rect_t& a_rect) const
{
  return (left == a_rect.left) &&
    (top == a_rect.top) &&
    (right == a_rect.right) &&
    (bottom == a_rect.bottom);
}

inline bool rect_t::operator!=(const rect_t& a_rect) const
{
  return !operator==(a_rect);
}

inline rect_t::size_type rect_t::width() const
{
  return right - left + 1;
}

inline rect_t::size_type rect_t::height() const
{
  return bottom - top + 1;
}

inline void rect_t::move_to(const size_type a_left, const size_type a_top)
{
  right += (a_left - left);
  bottom += (a_top - bottom);
  left = a_left;
  top = a_top;
}

inline void rect_t::move_to(const point_t& a_left_top)
{
  right += (a_left_top.left - left);
  bottom += (a_left_top.top - bottom);
  left = a_left_top.left;
  top = a_left_top.top;
}

inline void rect_t::move_left(const size_type a_left)
{
  right += (a_left - left);
  left = a_left;
}

inline void rect_t::move_top(const size_type a_top)
{
  bottom += (a_top - top);
  top = a_top;
}

inline void rect_t::move_right(const size_type a_right)
{
  left += (a_right - right);
  right = a_right;
}

inline void rect_t::move_bottom(const size_type a_bottom)
{
  top += (a_bottom - bottom);
  bottom = a_bottom;
}

inline void rect_t::offset(const difference_type a_x_diff,
  const difference_type a_y_diff)
{
  left += a_x_diff;
  right += a_x_diff;
  top += a_y_diff;
  bottom += a_y_diff;
}

inline void rect_t::union_with(const rect_t& a_rect)
{
  left = min(left, a_rect.left);
  top = min(top, a_rect.top);
  right = max(right, a_rect.right);
  bottom = max(bottom, a_rect.bottom);
}

inline rect_t rect_t::get_united(const rect_t& a_rect) const
{
  rect_t rect_result(*this);
  rect_result.union_with(a_rect);
  return rect_result;
}

inline bool rect_t::contains(const size_type a_left,
  const size_type a_top) const
{
  return (a_left >= left) && (a_left <= right) && (a_top >= top) &&
    (a_top <= bottom);
}

inline bool rect_t::contains(const point_t& a_point) const
{
  return contains(a_point.left, a_point.top);
}

inline bool rect_t::contains(const rect_t& a_rect) const
{
  return (a_rect.left >= left) && (a_rect.right <= right) &&
    (a_rect.top >= top) && (a_rect.bottom <= bottom);
}

inline bool rect_t::intersects_with(const rect_t& a_rect) const
{
  return
    (
      ((a_rect.left <= left) && (left <= a_rect.right)) ||
      ((left <= a_rect.left) && (a_rect.left <= right))
    ) && (
      ((a_rect.top <= top) && (top <= a_rect.bottom)) ||
      ((top <= a_rect.top) && (a_rect.top <= bottom))
    );
}



template <class T>
struct mx_point_t {
  typedef T value_type;

  value_type left;
  value_type top;

  mx_point_t(
    value_type a_left = value_type(),
    value_type a_top = value_type()
  ):
    left(a_left),
    top(a_top)
  {
  }
};

template <class T>
bool operator==(const mx_point_t<T>& a_first,
  const mx_point_t<T>& a_second)
{
  return (a_first.left == a_second.left) && (a_first.top == a_second.top);
}

template <class T>
bool operator!=(const mx_point_t<T>& a_first,
  const mx_point_t<T>& a_second)
{
  return !(a_first == a_second);
}

template <class T>
struct mx_rect_t {
  typedef T value_type;

  value_type left;
  value_type top;
  value_type right;
  value_type bottom;

  mx_rect_t(
    value_type a_left = value_type(),
    value_type a_top = value_type(),
    value_type a_right = value_type(),
    value_type a_bottom = value_type()
  ):
    left(a_left),
    top(a_top),
    right(a_right),
    bottom(a_bottom)
  {
  }

  value_type width() const;
  value_type height() const;
};

template <class T>
typename mx_rect_t<T>::value_type mx_rect_width(
  const mx_rect_t<T>& a_rect, integral_type_tag)
{
  return a_rect.right - a_rect.left + 1;
}
template <class T>
typename mx_rect_t<T>::value_type mx_rect_width(
  const mx_rect_t<T>& a_rect, floating_point_type_tag)
{
  return a_rect.right - a_rect.left;
}
template <class T>
typename mx_rect_t<T>::value_type mx_rect_height(
  const mx_rect_t<T>& a_rect, integral_type_tag)
{
  return a_rect.bottom - a_rect.top + 1;
}
template <class T>
typename mx_rect_t<T>::value_type mx_rect_height(
  const mx_rect_t<T>& a_rect, floating_point_type_tag)
{
  // top - bottom сделано намерено. Считаем, что направления увеличения
  // по оси Y как в графиках вверх, а не как в формах и окнах вниз
  return a_rect.top - a_rect.bottom;
}
template <class T>
typename mx_rect_t<T>::value_type mx_rect_t<T>::width() const
{
  return mx_rect_width(*this, type_detect_t<T>::tag_type);
}
template <class T>
typename mx_rect_t<T>::value_type mx_rect_t<T>::height() const
{
  return mx_rect_height(*this, type_detect_t<T>::tag_type);
}

template <class T>
bool operator==(const mx_rect_t<T>& a_first,
  const mx_rect_t<T>& a_second)
{
  return (a_first.left == a_second.left) && (a_first.top == a_second.top) &&
    (a_first.right == a_second.right) && (a_first.bottom == a_second.bottom);
}

template <class T>
bool operator!=(const mx_rect_t<T>& a_first,
  const mx_rect_t<T>& a_second)
{
  return !(a_first == a_second);
}

template <class T>
struct mx_bounds_t {
  typedef T value_type;

  value_type begin;
  value_type end;

  mx_bounds_t(
    value_type a_begin = value_type(),
    value_type a_end = value_type()
  ):
    begin(a_begin),
    end(a_end)
  {
  }
};

template <class T>
bool operator==(const mx_bounds_t<T>& a_first,
  const mx_bounds_t<T>& a_second)
{
  return (a_first.begin == a_second.begin) && (a_first.end == a_second.end);
}

template <class T>
bool operator!=(const mx_bounds_t<T>& a_first,
  const mx_bounds_t<T>& a_second)
{
  return !(a_first == a_second);
}

//! @}

} //namespace irs

#endif //IRSDEFSH
