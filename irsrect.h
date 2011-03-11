//! \file
//! \ingroup single_type_group
//! \brief Rect
//!
//! ����: 11.03.2011\n
//! ���� ��������: 15.04.2010

#ifndef IRSRECTH
#define IRSRECTH

#include <irsdefs.h>

#include <irscpp.h>

#include <irsfinal.h>

namespace irs {

typedef size_t sizens_t;

//! \addtogroup single_type_group
//! @{

struct point_t {
  typedef sizens_t size_type;
  int left;
  int top;
  point_t(
    sizens_t a_left = 0,
    sizens_t a_top = 0
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
  typedef sizens_t size_type;
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
  inline size_type width() const;
  inline size_type height() const;
  //! \brief ���������� �������� � ����� � ������������ (a_left, a_top).
  inline void move_to(const size_type a_left, const size_type a_top);
  //! \brief ���������� ������� � ����� a_left_top.
  inline void move_to(const point_t& a_left_top);  
  inline void move_left(const size_type a_left);
  inline void move_top(const size_type a_top);
  inline void move_right(const size_type a_right);
  inline void move_bottom(const size_type a_bottom);
  inline void offset(const size_type a_x_diff, const size_type a_y_diff);
  inline void union_with(const rect_t& a_rect);
  inline rect_t get_united(const rect_t& a_rect) const;
  //! \brief ���������� true, ���� ����� � ������������ (a_left, a_top)
  //!   ������ � ��������.
  inline bool contains(const size_type a_left, const size_type a_top) const;
  //! \brief ���������� true, ���� ����� a_point ������ � ��������.
  inline bool contains(const point_t& a_point) const;
  //! \brief ���������� true, ���� �������� a_rect �������� �������������.
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

inline void rect_t::offset(const size_type a_x_diff, const size_type a_y_diff)
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

//! @}

} //namespace irs

#endif //IRSDEFSH
