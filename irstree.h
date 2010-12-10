//  Component Tree
//  Data: 15.09.2009
//
//  Copyright (c) 2009
//  IRS Company
//
//  Copyright (c) 2009
//  Maksim Lyashchov
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.

#ifndef irstreeH
#define irstreeH

#include <irsdefs.h>

#include <irscpp.h>
#include <mxdata.h>
#include <irserror.h>
#include <list>

#include <irsfinal.h>

#ifdef IRS_LIB_DEBUG
// Включение отладочного режима компонента
#define IRS_LIB_TREE_DEBUG
#endif // IRS_LIB_DEBUG

#ifdef IRS_LIB_TREE_DEBUG
#define IRS_LIB_TREE_ASSERT(msg) IRS_LIB_ASSERT(msg)
#define IRS_LIB_TREE_ASSERT_MSG(msg) IRS_LIB_ASSERT_MSG(msg)
#else // IRS_LIB_TREE_DEBUG
#define IRS_LIB_TREE_ASSERT(msg)
#define IRS_LIB_TREE_ASSERT_MSG(msg)
#endif // IRS_LIB_TREE_DEBUG

namespace irs {

#ifdef NEW_TREE_ENABLED

template <class T>
class tree_t;

template <class T>
struct tree_node_t
{
  typedef T value_type;
  typedef tree_t<T>* tree_pointer;
  typedef tree_node_t* parent_pointer;
  typedef tree_node_t* node_pointer;
  typedef list<tree_node_t*> children_type;
  typedef irs_size_t size_type;
  tree_node_t(const value_type& a_value,
    tree_pointer ap_tree,
    parent_pointer ap_parent,
    node_pointer ap_prev,
    node_pointer ap_next,
    node_pointer ap_children_begin,
    node_pointer ap_children_end);
  value_type value;
  tree_pointer p_tree;
  node_pointer p_parent;
  node_pointer p_prev;
  node_pointer p_next;
  node_pointer p_children_begin;
  node_pointer p_children_end;
  size_type child_count;
};

template <class T>
tree_node_t<T>::tree_node_t(
  const value_type& a_value,
  tree_pointer ap_tree,
  parent_pointer ap_parent,
  node_pointer ap_prev,
  node_pointer ap_next,
  node_pointer ap_children_begin,
  node_pointer ap_children_end
):
  value(a_value),
  p_tree(ap_tree),
  p_parent(ap_parent),
  p_prev(ap_prev),
  p_next(ap_next),
  p_children_begin(ap_children_begin),
  p_children_end(ap_children_end),
  child_count(0)
{
}

template <class Iterator>
struct tree_iterator_tags
{
  typedef typename Iterator::difference_type difference_type;
  typedef typename Iterator::value_type value_type;
  typedef typename Iterator::pointer pointer;
  typedef typename Iterator::reference reference;
  typedef typename Iterator::iterator_category iterator_category;
  typedef typename Iterator::tree_pointer tree_pointer;
  typedef typename Iterator::node_pointer node_pointer;
};

template <class T>
struct tree_iterator_tags<T*>
{
  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef T* pointer;
  typedef T& reference;
  typedef bidirectional_iterator_tag iterator_category;
  typedef tree_t<T>* tree_pointer;
  typedef tree_node_t<T>* node_pointer;
};

template <class T>
struct tree_iterator_tags<const T*>
{
  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef const T* pointer;
  typedef const T& reference;
  typedef bidirectional_iterator_tag iterator_category;
  typedef const tree_t<T>* tree_pointer;
  typedef const tree_node_t<T>* node_pointer;
};

template <class T>
class tree_iterator_t;
template <class T>
class const_tree_iterator_t;
template <class IteratorTag>
class basic_tree_child_iterator_t;
template <class T>
class tree_child_iterator_t;
template <class T>
class const_tree_child_iterator_t;

template <class IteratorTag>
class basic_tree_iterator_t:
  public iterator<
    typename IteratorTag::iterator_category,
    typename IteratorTag::value_type,
    typename IteratorTag::difference_type,
    typename IteratorTag::pointer,
    typename IteratorTag::reference>
{
public:
  typedef typename IteratorTag::difference_type difference_type;
  typedef typename IteratorTag::value_type value_type;
  typedef typename IteratorTag::pointer pointer;
  typedef typename IteratorTag::reference reference;
  typedef typename IteratorTag::iterator_category iterator_category;
public:
  typedef typename IteratorTag::tree_pointer tree_pointer;
  typedef typename IteratorTag::node_pointer node_pointer;
  typedef irs_size_t size_type;
public:
  basic_tree_iterator_t(node_pointer ap_node  = IRS_NULL);
  basic_tree_iterator_t(const basic_tree_iterator_t& a_basic_tree_iterator);
  void swap(basic_tree_iterator_t& a_basic_tree_iterator);
  const basic_tree_iterator_t& operator=(
    const basic_tree_iterator_t& a_basic_tree_iterator);
  bool operator==(const basic_tree_iterator_t& a_basic_tree_iterator) const;
  bool operator!=(const basic_tree_iterator_t& a_basic_tree_iterator) const;
  reference operator*();
  //! \brief Перейти к предыдущему элементу дерева.
  basic_tree_iterator_t& operator--();
  //! \brief Перейти к предыдущему элементу дерева.
  basic_tree_iterator_t operator--(int);
  //! \brief Перейти к следующему элементу дерева.
  basic_tree_iterator_t& operator++();
  //! \brief Перейти к следующему элементу дерева.
  basic_tree_iterator_t operator++(int);
  //! \brief Перейти к корневому элементу дерева. Тоже что и go_end.
  void go_root();
  //! \brief Перейти в первому элементу дерева.
  void go_begin();
  //! \brief Перейти к следующему за последним элементу дерева.
  //!   Тоже что и go_root.
  void go_end();
  //! \brief Перейти к следующему элементу дерева.
  void go_next();
  //! \brief Перейти к предыдущему элементу дерева.
  void go_prev();
  //! \brief Перейти к родительскому элементу.
  void go_parent();
  //! \brief Перейти к первому дочернему элементу.
  void go_children_begin();
  //! \brief Перейти к следующему за последним дочернему элементу.
  void go_children_end();
  //! \brief Перейти к предыдущему элементу, который имеет того же родителя.
  void go_prev_sibling();
  //! \brief Перейти к следующему элементу, который имеет того же родителя.
  void go_next_sibling();
  //! \brief Перейти к первому элементу, который имеет того же родителя.
  void go_siblings_begin();
  //! \brief Перейти к элементу, следующему за последним,
  //!   имеющего того же родителя.
  void go_siblings_end();
  basic_tree_iterator_t children_begin();
  basic_tree_iterator_t children_end();
  size_type child_count() const;
protected:
  node_pointer get_next_node(node_pointer ap_cur_node,
    const bool a_children_allowed = true);
  node_pointer mp_node;
  friend class tree_t<value_type>;
  friend class const_tree_iterator_t<value_type>;
  friend class basic_tree_iterator_t<tree_iterator_tags<value_type*> >;
  friend class basic_tree_iterator_t<tree_iterator_tags<const value_type*> >;
  friend class basic_tree_child_iterator_t<tree_iterator_tags<value_type*> >;
  friend class basic_tree_child_iterator_t<
    tree_iterator_tags<const value_type*> >;
  friend class tree_child_iterator_t<value_type>;
  friend class const_tree_child_iterator_t<value_type>;
};

template <class T>
basic_tree_iterator_t<T>::basic_tree_iterator_t(node_pointer ap_node):
  mp_node(ap_node)
{
}

template <class T>
basic_tree_iterator_t<T>::basic_tree_iterator_t(
  const basic_tree_iterator_t& a_basic_tree_iterator
):
  mp_node(a_basic_tree_iterator.mp_node)
{
}

template <class T>
void basic_tree_iterator_t<T>::swap(
  basic_tree_iterator_t& a_basic_tree_iterator)
{
  ::swap(mp_node, a_basic_tree_iterator.mp_node);
}

template <class T>
const basic_tree_iterator_t<T>& basic_tree_iterator_t<T>::operator=(
  const basic_tree_iterator_t& a_basic_tree_iterator)
{
  basic_tree_iterator_t basic_tree_iterator(a_basic_tree_iterator);
  swap(basic_tree_iterator);
  return *this;
}

template <class T>
bool basic_tree_iterator_t<T>::operator==(
  const basic_tree_iterator_t& a_basic_tree_iterator) const
{
  return mp_node == a_basic_tree_iterator.mp_node;
}

template <class T>
bool basic_tree_iterator_t<T>::operator!=(
  const basic_tree_iterator_t& a_basic_tree_iterator) const
{
  return mp_node != a_basic_tree_iterator.mp_node;
}

template <class T>
typename basic_tree_iterator_t<T>::reference
basic_tree_iterator_t<T>::operator*()
{
  return mp_node->value;
}

template <class T>
basic_tree_iterator_t<T>& basic_tree_iterator_t<T>::operator--()
{
  go_prev();
  return *this;
}

template <class T>
basic_tree_iterator_t<T> basic_tree_iterator_t<T>::operator--(int)
{
  basic_tree_iterator_t<T> it(*this);
  go_prev();
  return it;
}

template <class T>
basic_tree_iterator_t<T>& basic_tree_iterator_t<T>::operator++()
{
  go_next();
  return *this;
}

template <class T>
basic_tree_iterator_t<T> basic_tree_iterator_t<T>::operator++(int)
{
  basic_tree_iterator_t<T> it(*this);
  go_next();
  return it;
}

template <class T>
void basic_tree_iterator_t<T>::go_root()
{
  IRS_LIB_ASSERT(mp_node->mp_tree);
  *this = mp_node->p_tree->root();
}

template <class T>
void basic_tree_iterator_t<T>::go_begin()
{
  tree_iterator_t<value_type> tree_it = mp_node->p_tree->end();
  *this = *reinterpret_cast<basic_tree_iterator_t*>(&tree_it);
}

template <class T>
void basic_tree_iterator_t<T>::go_end()
{
  tree_iterator_t<value_type> tree_it = mp_node->p_tree->end();
  *this = *reinterpret_cast<basic_tree_iterator_t*>(&tree_it);
}

template <class T>
void basic_tree_iterator_t<T>::go_next()
{
  mp_node = get_next_node(mp_node);
}

template <class T>
void basic_tree_iterator_t<T>::go_prev()
{
  if (mp_node == mp_node->p_tree->begin().mp_node) {
    mp_node = mp_node->p_tree->root().mp_node;
  } else if (mp_node == mp_node->p_parent->p_children_begin) {
    mp_node = mp_node->p_parent;
  } else {
    mp_node = mp_node->p_prev;
    while (mp_node->p_children_begin != mp_node->p_children_end) {
      mp_node = mp_node->p_children_end;
      mp_node = mp_node->p_prev;
    }
  }
}

template <class T>
void basic_tree_iterator_t<T>::go_parent()
{
  mp_node = mp_node->p_parent;
}

template <class T>
void basic_tree_iterator_t<T>::go_children_begin()
{
  mp_node = mp_node->p_children_begin;
}

template <class T>
void basic_tree_iterator_t<T>::go_children_end()
{
  mp_node = mp_node->p_children_end;
}

template <class T>
void basic_tree_iterator_t<T>::go_prev_sibling()
{
  mp_node = mp_node->p_prev;
}

template <class T>
void basic_tree_iterator_t<T>::go_next_sibling()
{
  mp_node = mp_node->p_next;
}

template <class T>
void basic_tree_iterator_t<T>::go_siblings_begin()
{
  IRS_LIB_ASSERT(mp_node != mp_node->p_tree->root().mp_node);
  mp_node = mp_node->p_parent->p_children_begin;
}

template <class T>
void basic_tree_iterator_t<T>::go_siblings_end()
{
  IRS_LIB_ASSERT(mp_node != mp_node->p_tree->root().mp_node);
  mp_node = mp_node->p_parent->p_children_end;
}

template <class T>
basic_tree_iterator_t<T> basic_tree_iterator_t<T>::children_begin()
{
  return basic_tree_iterator_t<T>(mp_node->p_children_begin);
}

template <class T>
basic_tree_iterator_t<T> basic_tree_iterator_t<T>::children_end()
{
  return basic_tree_iterator_t<T>(mp_node->p_children_end);
}

template <class T>
typename basic_tree_iterator_t<T>::size_type
basic_tree_iterator_t<T>::child_count() const
{
  return mp_node->child_count;
}

template <class T>
typename basic_tree_iterator_t<T>::node_pointer
basic_tree_iterator_t<T>::get_next_node(node_pointer ap_cur_node,
  const bool a_children_allowed)
{
  node_pointer p_next_node = ap_cur_node;
  if (ap_cur_node != ap_cur_node->p_tree->root().mp_node) {
    if (a_children_allowed &&
      (ap_cur_node->p_children_begin != ap_cur_node->p_children_end)) {
      p_next_node = ap_cur_node->p_children_begin;
    } else if (ap_cur_node->p_next != ap_cur_node->p_parent->p_children_end) {
      p_next_node = ap_cur_node->p_next;
    } else {
      const bool children_allowed = false;
      p_next_node = get_next_node(ap_cur_node->p_parent, children_allowed);
    }
  }
  return p_next_node;
}

template <class T>
class tree_iterator_t: public basic_tree_iterator_t<tree_iterator_tags<T*> >
{
private:
  typedef basic_tree_iterator_t<tree_iterator_tags<T*> > base_iterator;
public:
  typedef typename base_iterator::node_pointer node_pointer;
  typedef typename base_iterator::iterator_category iterator_category;
  tree_iterator_t(node_pointer ap_node_pointer = IRS_NULL);
  tree_iterator_t(const tree_iterator_t& a_tree_iterator);
  const tree_iterator_t& operator=(const tree_iterator_t& a_tree_iterator);
private:
  friend class basic_tree_iterator_t<tree_iterator_tags<T*> >;
  //friend class const_tree_iterator_t<T>;
};

template <class T>
tree_iterator_t<T>::tree_iterator_t(node_pointer ap_node_pointer):
  base_iterator(ap_node_pointer)
{
}

template <class T>
tree_iterator_t<T>::tree_iterator_t(const tree_iterator_t& a_tree_iterator):
  base_iterator(a_tree_iterator.mp_node)
{
}

template <class T>
const tree_iterator_t<T>& tree_iterator_t<T>::operator=(
  const tree_iterator_t& a_tree_iterator)
{
  tree_iterator_t tree_iterator(a_tree_iterator);
  base_iterator::swap(tree_iterator);
  return *this;
}

template <class T>
class const_tree_iterator_t:
  //public tree_iterator_t<T>
  public basic_tree_iterator_t<tree_iterator_tags<const T*> >
{
private:
  typedef basic_tree_iterator_t<tree_iterator_tags<const T*> > base_iterator;
  typedef basic_tree_iterator_t<tree_iterator_tags<T*> > iterator;
public:
  typedef typename base_iterator::node_pointer node_pointer;
  const_tree_iterator_t(node_pointer ap_node_pointer = IRS_NULL);
  const_tree_iterator_t(const const_tree_iterator_t& a_const_tree_iterator);
  const_tree_iterator_t(const tree_iterator_t<T>& a_tree_iterator);
  const_tree_iterator_t& operator=(
    const const_tree_iterator_t& a_const_tree_iterator);
private:
  friend class basic_tree_iterator_t<tree_iterator_tags<const T*> >;
};

template <class T>
const_tree_iterator_t<T>::const_tree_iterator_t(node_pointer ap_node_pointer
):
  base_iterator(ap_node_pointer)
{
}

template <class T>
const_tree_iterator_t<T>::const_tree_iterator_t(
  const const_tree_iterator_t<T>& a_const_tree_iterator
):
  base_iterator(a_const_tree_iterator.mp_node)
{
}

template <class T>
const_tree_iterator_t<T>::const_tree_iterator_t(
  const tree_iterator_t<T>& a_tree_iterator
):
  //base_iterator(static_cast<const iterator*>(&a_tree_iterator)->mp_node)
  base_iterator(a_tree_iterator.mp_node)
{
}

template <class T>
const_tree_iterator_t<T>& const_tree_iterator_t<T>::operator=(
  const const_tree_iterator_t& a_const_tree_iterator)
{
  const_tree_iterator_t const_tree_iterator(a_const_tree_iterator);
  swap(const_tree_iterator);
  return *this;
}

template <class IteratorTag>
class basic_tree_child_iterator_t: protected basic_tree_iterator_t<IteratorTag>
{
private:
  typedef basic_tree_iterator_t<IteratorTag> basic_tree_iterator;
protected:
  typedef typename IteratorTag::node_pointer node_pointer;
public:
  typedef typename IteratorTag::reference reference;
  basic_tree_child_iterator_t(const node_pointer ap_node_pointer = IRS_NULL);
  basic_tree_iterator_t<IteratorTag> tree_iterator();
  bool operator==(const basic_tree_child_iterator_t& a_iterator) const;
  bool operator!=(const basic_tree_child_iterator_t& a_iterator) const;
  reference operator*();
  //! \brief Перейти к предыдущему элементу, который имеет того же родителя.
  basic_tree_child_iterator_t& operator--();
  //! \brief Перейти к предыдущему элементу, который имеет того же родителя.
  basic_tree_child_iterator_t operator--(int);
  //! \brief Перейти к следующему элементу, который имеет того же родителя.
  basic_tree_child_iterator_t& operator++();
  //! \brief Перейти к следующему элементу, который имеет того же родителя.
  basic_tree_child_iterator_t operator++(int);
protected:
  using basic_tree_iterator::swap;
  using basic_tree_iterator::go_prev_sibling;
  using basic_tree_iterator::go_next_sibling;
private:
  using basic_tree_iterator::mp_node;
};

template <class IteratorTag>
basic_tree_child_iterator_t<IteratorTag>::basic_tree_child_iterator_t(
  const node_pointer ap_node_pointer
):
  basic_tree_iterator(ap_node_pointer)
{
}

template <class IteratorTag>
basic_tree_iterator_t<IteratorTag>
basic_tree_child_iterator_t<IteratorTag>::tree_iterator()
{
  return basic_tree_iterator_t<IteratorTag>(mp_node);
}

template <class IteratorTag>
bool basic_tree_child_iterator_t<IteratorTag>::operator==(
  const basic_tree_child_iterator_t& a_iterator) const
{
  return basic_tree_iterator::operator==(a_iterator);
}

template <class IteratorTag>
bool basic_tree_child_iterator_t<IteratorTag>::operator!=(
  const basic_tree_child_iterator_t& a_iterator) const
{
  return basic_tree_iterator::operator!=(a_iterator);
}

template <class IteratorTag>
typename basic_tree_child_iterator_t<IteratorTag>::reference
basic_tree_child_iterator_t<IteratorTag>::operator*()
{
  return basic_tree_iterator::operator*();
}

template <class IteratorTag>
basic_tree_child_iterator_t<IteratorTag>&
basic_tree_child_iterator_t<IteratorTag>::operator--()
{
  go_prev_sibling();
  return *this;
}

template <class IteratorTag>
basic_tree_child_iterator_t<IteratorTag>
basic_tree_child_iterator_t<IteratorTag>::operator--(int)
{
  basic_tree_child_iterator_t basic_tree_child_iterator(*this);
  go_prev_sibling();
  return basic_tree_child_iterator;
}

template <class IteratorTag>
basic_tree_child_iterator_t<IteratorTag>&
basic_tree_child_iterator_t<IteratorTag>::operator++()
{
  go_next_sibling();
  return *this;
}

template <class IteratorTag>
basic_tree_child_iterator_t<IteratorTag>
basic_tree_child_iterator_t<IteratorTag>::operator++(int)
{
  basic_tree_child_iterator_t basic_tree_child_iterator(*this);
  go_next_sibling();
  return basic_tree_child_iterator;
}

template <class T>
class tree_child_iterator_t:
  public basic_tree_child_iterator_t<tree_iterator_tags<T*> >
{
private:
  //typedef basic_tree_child_iterator_t<tree_iterator_tags<T*> > base_iterator;
  typedef basic_tree_iterator_t<tree_iterator_tags<T*> > base_iterator;
  typedef basic_tree_child_iterator_t<tree_iterator_tags<T*> >
    base_child_iterator;
public:
  typedef typename base_iterator::node_pointer node_pointer;
  tree_child_iterator_t(node_pointer ap_node_pointer = IRS_NULL);
  //tree_child_iterator_t(const tree_child_iterator_t& a_tree_child_iterator);
  //tree_child_iterator_t(const tree_iterator_t<T>& a_tree_iterator);
  tree_child_iterator_t(const base_iterator& a_base_iterator);
  tree_child_iterator_t& operator=(
    const tree_child_iterator_t& a_tree_child_iterator);
  tree_child_iterator_t& operator=(const tree_iterator_t<T>& a_tree_iterator);
  using base_child_iterator::swap;
};

template <class T>
tree_child_iterator_t<T>::tree_child_iterator_t(
  node_pointer ap_node_pointer
):
  base_iterator(ap_node_pointer)
{
}

/*template <class T>
tree_child_iterator_t<T>::tree_child_iterator_t(
  const tree_child_iterator_t& a_tree_child_iterator
):
  base_iterator(a_tree_child_iterator.mp_node)
{
}

template <class T>
tree_child_iterator_t<T>::tree_child_iterator_t(
  const tree_iterator_t<T>& a_tree_iterator
):
  base_iterator(a_tree_iterator.mp_node)
{
}*/

template <class T>
tree_child_iterator_t<T>::tree_child_iterator_t(
  const base_iterator& a_base_iterator
):
  base_child_iterator(a_base_iterator.mp_node)
{
}

template <class T>
tree_child_iterator_t<T>& tree_child_iterator_t<T>::operator=(
  const tree_child_iterator_t<T>& a_tree_child_iterator)
{
  tree_child_iterator_t<T> tree_child_iterator(a_tree_child_iterator);
  swap(tree_child_iterator);
  return *this;
}

template <class T>
tree_child_iterator_t<T>& tree_child_iterator_t<T>::operator=(
  const tree_iterator_t<T>& a_tree_iterator)
{
  tree_child_iterator_t<T> tree_child_iterator(a_tree_iterator);
  swap(tree_child_iterator);
  return *this;
}

template <class T>
class const_tree_child_iterator_t:
  public basic_tree_child_iterator_t<tree_iterator_tags<const T*> >
{
private:
  typedef basic_tree_iterator_t<tree_iterator_tags<T*> > base_iterator;
  typedef basic_tree_iterator_t<tree_iterator_tags<const T*> >
    const_base_iterator;
  typedef basic_tree_child_iterator_t<tree_iterator_tags<const T*> >
    base_child_iterator;
public:
  typedef typename base_child_iterator::node_pointer node_pointer;
  const_tree_child_iterator_t(node_pointer ap_node_pointer = IRS_NULL);
  const_tree_child_iterator_t(
    const const_tree_child_iterator_t<T>& a_const_tree_child_iterator);
  const_tree_child_iterator_t(
    const tree_child_iterator_t<T>& a_tree_child_iterator);
  const_tree_child_iterator_t(const base_iterator& a_base_iterator);
  const_tree_child_iterator_t(const const_base_iterator& a_const_base_iterator);
  const_tree_child_iterator_t& operator=(
    const tree_child_iterator_t<T>& a_tree_child_iterator);
  const_tree_child_iterator_t& operator=(
    const const_tree_child_iterator_t& a_const_tree_child_iterator);
  using base_child_iterator::swap;
};

template <class T>
const_tree_child_iterator_t<T>::const_tree_child_iterator_t(
  node_pointer ap_node_pointer
):
  base_child_iterator(ap_node_pointer)
{
}

template <class T>
const_tree_child_iterator_t<T>::const_tree_child_iterator_t(
  const const_tree_child_iterator_t<T>& a_const_tree_child_iterator
):
  base_child_iterator(a_const_tree_child_iterator.mp_node)
{
}

template <class T>
const_tree_child_iterator_t<T>::
const_tree_child_iterator_t(
  const tree_child_iterator_t<T>& a_tree_child_iterator
):
  base_child_iterator(a_tree_child_iterator.mp_node)
{
}

template <class T>
const_tree_child_iterator_t<T>::
const_tree_child_iterator_t(const base_iterator& a_base_iterator
):
  base_child_iterator(a_base_iterator.mp_node)
{
}

template <class T>
const_tree_child_iterator_t<T>::
const_tree_child_iterator_t(const const_base_iterator& a_const_base_iterator
):
  base_child_iterator(a_const_base_iterator.mp_node)
{
}

template <class T>
const_tree_child_iterator_t<T>& const_tree_child_iterator_t<T>::operator=(
  const tree_child_iterator_t<T>& a_tree_child_iterator)
{
  return operator=(const_tree_child_iterator_t<T>(a_tree_child_iterator));
  /*const_tree_child_iterator_t const_tree_child_iterator(
    a_tree_child_iterator);
  swap(const_tree_child_iterator);
  return *this;*/
}

template <class T>
const_tree_child_iterator_t<T>& const_tree_child_iterator_t<T>::operator=(
  const const_tree_child_iterator_t& a_const_tree_child_iterator)
{
  const_tree_child_iterator_t const_tree_child_iterator(
    a_const_tree_child_iterator);
  swap(const_tree_child_iterator);
  return *this;
}

template <class T>
class tree_t
{
public:
  typedef irs_size_t size_type;
  typedef T value_type;
  //typedef tree_node_t<T> node_type;
  typedef tree_node_t<T>* node_pointer;
  typedef tree_iterator_t<T> iterator;
  typedef const_tree_iterator_t<T> const_iterator;
  typedef tree_child_iterator_t<T> child_iterator;
  typedef const_tree_child_iterator_t<T> const_child_iterator;
  tree_t();
  ~tree_t();
  //! \brief Удаление всех узлов.
  void clear();
  //! \brief Вставка узла в начало списка узлов, в котором находится a_sibling.
  void push_front_sibling(iterator a_sibling, const value_type& a_value);
  //! \brief Удаление узла из начала списка узлов,
  //!   в котором находится a_sibling.
  void pop_front_sibling(iterator a_sibling);
  //! \brief Вставка узла в конец списка узлов, в котором находится a_sibling.
  void push_back_sibling(iterator a_sibling, const value_type& a_value);
  //! \brief Удаление узла из конца списка узлов, в котором находится a_sibling.
  void pop_back_sibling(iterator a_sibling);
  //! \brief Вставка узла в позицию перед узлом a_sibling.
  iterator insert_sibling(iterator a_pos, const value_type& a_value);
  //! \brief Удаление узла a_pos.
  iterator erase(iterator a_pos);
  //! \brief Вставка узла в начало списка дочерних узлов узла a_parent.
  void push_front_child(iterator a_parent, const value_type& a_value);
  //! \brief Удаление узла из начала списка дочерних узлов узла a_parent.
  void pop_front_child(iterator a_parent);
  //! \brief Вставка узла в конец списка дочерних узлов узла a_sibling.
  void push_back_child(iterator a_parent, const value_type& a_value);
  //! \brief Удаление узла из конца списка дочерних узлов узла a_sibling.
  void pop_back_child(iterator a_parent);
  //! \brief Возвращает итератор корневого элемента.
  iterator root();
  //! \brief Возвращает константный итератор корневого элемента.
  const_iterator root() const;
  //! \brief Возвращает итератор первого элемента.
  iterator begin();
  //! \brief Возвращает константный итератор первого элемента.
  const_iterator begin() const;
  //! \brief Возвращает итератор узла, следующего за последним элементом.
  iterator end();
  //! \brief Возвращает константный итератор узла,
  //!   следующего за последним элементом.
  const_iterator end() const;
  //! \brief Возвращает колечество узлов.
  size_type size() const;
  //! \brief Возвращает true, если в дереве нет узлов.
  bool empty() const;
private:
  enum { single_root_node = 1 };
  typedef tree_node_t<T> node_type;
  //typedef tree_node_t<T>* child_node_iterator;
  node_pointer create_node(const value_type& a_value = value_type()); 
  void connect_node_before(node_pointer ap_pos, node_pointer ap_node);
  void delete_node(node_pointer ap_node);
  void disconnect_node(node_pointer ap_node);
  void release_node(node_pointer ap_node);
  node_pointer mp_root;
  size_type m_size;
};

template <class T>
tree_t<T>::tree_t():
  mp_root(create_node()),
  m_size(0)
{
}

template <class T>
typename tree_t<T>::node_pointer
tree_t<T>::create_node(const value_type& a_value)
{
  node_pointer p_new_node = new node_type(a_value, this, IRS_NULL, IRS_NULL,
    IRS_NULL, IRS_NULL, IRS_NULL);
  node_pointer p_children_end = new node_type(a_value, this, p_new_node,
    IRS_NULL, IRS_NULL, IRS_NULL, IRS_NULL);
  p_children_end->p_prev = p_children_end;
  p_children_end->p_next = p_children_end;
  p_new_node->p_children_begin = p_children_end;
  p_new_node->p_children_end = p_children_end;
  m_size++;
  return p_new_node;
}  

template <class T>
void tree_t<T>::connect_node_before(node_pointer ap_pos, node_pointer ap_node)
{
  IRS_LIB_ASSERT(ap_pos);
  IRS_LIB_ASSERT(ap_pos->p_prev != IRS_NULL);
  IRS_LIB_ASSERT(ap_pos->p_next != IRS_NULL);
  IRS_LIB_ASSERT(ap_node);
  ap_node->p_parent = ap_pos->p_parent;
  node_pointer p_prev_node = ap_pos->p_prev;
  p_prev_node->p_next = ap_node;
  ap_node->p_prev = p_prev_node;
  ap_pos->p_prev = ap_node;
  ap_node->p_next = ap_pos;
  if (ap_pos->p_parent->p_children_begin == ap_pos) {
    ap_pos->p_parent->p_children_begin = ap_node;
  }
  ap_node->p_parent->child_count++;
}

template <class T>
void tree_t<T>::delete_node(node_pointer ap_node)
{
  IRS_LIB_ASSERT(ap_node);
  disconnect_node(ap_node);
  release_node(ap_node);
}

template <class T>
void tree_t<T>::disconnect_node(node_pointer ap_node)
{
  node_pointer p_prev_node = ap_node->p_prev;
  node_pointer p_next_node = ap_node->p_next;
  if (ap_node->p_parent->p_children_begin == ap_node) {
    ap_node->p_parent->p_children_begin = p_next_node;
  }
  p_prev_node->p_next = p_next_node;
  p_next_node->p_prev = p_prev_node;
  ap_node->p_parent->child_count--;
}

template <class T>
void tree_t<T>::release_node(node_pointer ap_node)
{
  node_pointer p_node = ap_node->p_children_begin;
  const bool is_children_end = (p_node == IRS_NULL);
  while (p_node) {
    node_pointer p_next_node = (p_node != ap_node->p_children_end) ?
      p_node->p_next : IRS_NULL;
    release_node(p_node);
    p_node = p_next_node;
  }

  delete ap_node;
  if (!is_children_end) {
    m_size--;
  }
}

template <class T>
tree_t<T>::~tree_t()
{
  release_node(mp_root);
}

template <class T>
void tree_t<T>::clear()
{
  node_pointer p_node = mp_root->p_children_begin;
  while (p_node != mp_root->p_children_end) {
    node_pointer p_next_node = p_node->p_next;
    delete_node(p_node);
    p_node = p_next_node;
  }
}

template <class T>
void tree_t<T>::push_front_sibling(iterator a_sibling,
  const value_type& a_value)
{
  IRS_LIB_ASSERT(a_sibling != iterator(mp_root));
  push_front_child(a_sibling.mp_node->p_parent, a_value);
}

template <class T>
void tree_t<T>::pop_front_sibling(iterator a_sibling)
{
  IRS_LIB_ASSERT(a_sibling != iterator(mp_root));
  pop_front_child(a_sibling.mp_node->p_parent);
}

template <class T>
void tree_t<T>::push_back_sibling(iterator a_sibling, const value_type& a_value)
{
  push_back_child(iterator(a_sibling.mp_node->p_parent), a_value);
}

template <class T>
void tree_t<T>::pop_back_sibling(iterator a_sibling)
{
  IRS_LIB_ASSERT(a_sibling != iterator(mp_root));
  pop_back_child(a_sibling.mp_node->p_parent);
}

template <class T>
typename tree_t<T>::iterator tree_t<T>::insert_sibling(iterator a_pos,
  const value_type& a_value)
{ 
  node_pointer p_new_node = create_node(a_value);
  connect_node_before(a_pos.mp_node, p_new_node);
  return iterator(p_new_node);
}

template <class T>
typename tree_t<T>::iterator tree_t<T>::erase(iterator a_pos)
{
  node_pointer p_next_node = a_pos.mp_node->p_next;
  delete_node(a_pos.mp_node);
  return iterator(p_next_node);
}

template <class T>
void tree_t<T>::push_front_child(iterator a_parent, const value_type& a_value)
{
  insert_sibling(iterator(a_parent.mp_node->p_children_begin), a_value);
}

template <class T>
void tree_t<T>::pop_front_child(iterator a_parent)
{
  erase(iterator(a_parent.mp_node->p_children_begin));
}

template <class T>
void tree_t<T>::push_back_child(iterator a_parent, const value_type& a_value)
{
  insert_sibling(iterator(a_parent.mp_node->p_children_end), a_value);
}

template <class T>
void tree_t<T>::pop_back_child(iterator a_parent)
{
  erase(iterator(a_parent.mp_node->p_children_end->p_prev));
}

template <class T>
typename tree_t<T>::iterator tree_t<T>::root()
{
  return mp_root;
}

template <class T>
typename tree_t<T>::const_iterator tree_t<T>::root() const
{
  return mp_root;
}

template <class T>
typename tree_t<T>::iterator tree_t<T>::begin()
{
  if (empty()) {
    return iterator(mp_root);
  } else {
    return iterator(mp_root->p_children_begin);
  }
}

template <class T>
typename tree_t<T>::const_iterator tree_t<T>::begin() const
{
  if (empty()) {
    return const_iterator(mp_root);
  } else {
    return const_iterator(mp_root->p_children_begin);
  }
}

template <class T>
typename tree_t<T>::iterator tree_t<T>::end()
{
  return mp_root;
}

template <class T>
typename tree_t<T>::const_iterator tree_t<T>::end() const
{
  return mp_root;
}

template <class T>
typename tree_t<T>::size_type tree_t<T>::size() const
{
  return m_size;
}

template <class T>
bool tree_t<T>::empty() const
{
  return (mp_root->p_children_begin == mp_root->p_children_end);
}

inline void test_tree()
{
  tree_t<double> tree;
  tree.push_back_child(tree.begin(), 0.0);
  tree.push_back_child(tree.begin(), 0.1);
  tree.push_back_child(tree.end(), 0.2);
  tree.push_back_child(tree.end(), 0.3);
  tree.push_front_child(tree.end(), 0.4);
  tree.pop_back_child(tree.root());
  tree.pop_front_child(tree.root());
  tree.push_back_sibling(tree.begin(), 0.5);
  tree.push_front_sibling(tree.begin(), 0.6);
  tree.pop_back_sibling(tree.begin());
  tree.pop_front_sibling(tree.begin());
  IRS_LIB_ASSERT(tree.size() == 3);
  tree_t<double>::iterator it = tree.begin();
  max_element(tree.begin(), tree.end());
  tree_t<double>::child_iterator child_begin(it.children_begin());
  tree_t<double>::child_iterator child_end = it.children_end();
  tree_t<double>::child_iterator max_value_it = max_element(child_begin,
    child_end);
  tree_t<double>::const_child_iterator child_begin2(child_begin);
  tree_t<double>::const_child_iterator child_end2(it.children_end());
  child_begin2 = child_begin;
  //child_end2 = it.children_end();
  tree_t<double>::const_child_iterator max_value_it2 = max_element(child_begin2,
    child_end2);

  list<double> l;
  max_element(l.begin(), l.end());

  tree_t<double>::child_iterator child_it1(it);
  tree_t<double>::child_iterator child_it2(child_it1);
  child_it2 = child_it1;
  child_it2 = it;
  tree_t<double>::const_child_iterator const_child_it1(it);
  tree_t<double>::const_child_iterator const_child_it2(child_it1);
  tree_t<double>::const_child_iterator const_child_it3(const_child_it1);
  /*const_child_it3.go_next_sibling();
  const_child_it3.go_prev_sibling();
  const_child_it3.go_siblings_end();
  const_child_it3.go_siblings_begin();
  const_child_it3.go_next();
  const_child_it3.go_prev();*/
  const_child_it3++;
  ++const_child_it3;
  const_child_it3--;
  --const_child_it3;

  it = tree.begin();
  *it = 10;
  double d = *it;
  d = 0;
  it++;
  ++it;
  --it;
  it--;
  tree.clear();
  IRS_LIB_ASSERT(tree.size() == 0);
}
#endif // NEW_TREE_ENABLED

}; // namespace irs

#endif //irstreeH



