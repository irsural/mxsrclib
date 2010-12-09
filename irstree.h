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

#ifdef NOP
#if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER6) &&\
  (__BORLANDC__ <= (IRS_CPP_BUILDER6 + 4))) ||\
  defined(IRS_LINUX) ||\
  defined(_MSC_VER)

template<class T>
class tree_t;

template<class T>
class tree_node_t;

template<class T, class NODE_PTR_T, class NODE_REF_T>
class tree_node_child_iterator_t: public iterator<
  random_access_iterator_tag,
  typename tree_node_t<T>::node_type,
  typename tree_t<T>::difference_type,
  NODE_PTR_T,
  NODE_REF_T>
{
public:
  typedef size_t size_type;
  typedef tree_node_t<T> node_type;
  typedef ptrdiff_t difference_type;
  typedef NODE_PTR_T pointer;
  typedef NODE_REF_T reference;
  typedef typename tree_node_t<T>::children_type::iterator local_iterator_type;
  tree_node_child_iterator_t(
  ):
    m_local_it_child()
  {
  }
  tree_node_child_iterator_t(local_iterator_type a_local_it_child
  ):
    m_local_it_child(a_local_it_child)
  {
  }
  inline reference operator*() const
  {
    return **m_local_it_child;
  }
  inline pointer operator->() const
  {
    return &(**m_local_it_child);
  }
  inline tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T>& operator++()
  {
    m_local_it_child++;
    return *this;
  }
  inline tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T> operator++(int)
  {
    tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T> cur_it = *this;
    m_local_it_child++;
    return cur_it;
  }
  inline tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T>& operator--()
  {
    m_local_it_child--;
    return *this;
  }
  inline tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T> operator--(int)
  {
    tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T> cur_it = *this;
    m_local_it_child--;
    return cur_it;
  }
  inline tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T> operator+(
    difference_type a_n) const
  {
    return tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T>(
      m_local_it_child + a_n);
  }
  inline tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T>& operator+=(
    difference_type a_n) const
  {
    m_local_it_child += a_n;
    return *this;
  }
  inline tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T> operator-(
    difference_type a_n) const
  {
    return tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T>(
      m_local_it_child - a_n);
  }
  inline tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T>& operator-=(
    difference_type a_n) const
  {
    m_local_it_child -= a_n;
    return *this;
  }
  inline reference operator[](difference_type a_n) const
  { 
    return *(m_local_it_child + a_n);
  }
  inline bool operator>(const tree_node_child_iterator_t<T, NODE_PTR_T,
    NODE_REF_T> a_it_child) const
  {
    return (m_local_it_child > a_it_child.m_local_it_child);
  }
  inline bool operator>=(
    const tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T>
      a_it_child) const
  {
    return (m_local_it_child >= a_it_child.m_local_it_child);
  }
  inline bool operator<(
    const tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T>
      a_it_child) const
  {
    return (m_local_it_child < a_it_child.m_local_it_child);
  }
  inline bool operator<=(
    const tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T>
      a_it_child) const
  {
    return (m_local_it_child <= a_it_child.m_local_it_child);
  }
  inline bool operator==(
    const tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T>
      a_it_child) const
  {
    return (m_local_it_child == a_it_child.m_local_it_child);
  }
  inline bool operator!=(
    const tree_node_child_iterator_t<T, NODE_PTR_T, NODE_REF_T>
      a_it_child) const
  {
    return (m_local_it_child != a_it_child.m_local_it_child);
  }
private:
  local_iterator_type m_local_it_child;
  friend class tree_node_t<T>;
}; // class tree_node_child_iterator_t

template<class T>
class tree_node_t
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* pointer_type;
  typedef const T* const_pointer;
  typedef tree_node_t<T> node_type;
  typedef tree_node_t<T>& node_reference;
  typedef const tree_node_t<T>& node_const_reference;
  typedef tree_node_t<T>* node_pointer;
  typedef const tree_node_t<T>* node_const_pointer;
  //typedef list<tree_node_t<T> > list_tree_node;
  //typedef typename list_tree_node::iterator node_iterator;
  typedef typename vector<tree_node_t<T> >:: iterator test_node_iterator;
  typedef typename list<tree_node_t<T> >:: iterator node_iterator;
  typedef node_iterator parent_type;
  typedef node_iterator child_type;   
  typedef vector<child_type> children_type;
  typedef typename vector<child_type>::iterator children_iterator;
  typedef typename vector<child_type>::const_iterator children_const_iterator;
  typedef tree_node_child_iterator_t<T, node_pointer, node_reference>
    iterator;
  typedef tree_node_child_iterator_t<T, node_const_pointer,
    node_const_reference> const_iterator;
public:
  tree_node_t():
    mp_tree(IRS_NULL),
    m_it_this_node(),
    m_parent(),
    m_children(),
    m_value()
  {     
  }
  tree_node_t(tree_t<T>* ap_tree, node_iterator a_it_this_node,
    child_type a_parent
  ):
    mp_tree(ap_tree),
    m_it_this_node(a_it_this_node),
    m_parent(a_parent),
    m_children(),
    m_value()
  {
  }
  tree_node_t(tree_t<T>* ap_tree,
     node_iterator a_it_this_node,
    child_type a_parent,
    const_reference a_value
  ):
    mp_tree(ap_tree),
    m_it_this_node(a_it_this_node),
    m_parent(a_parent),
    m_children(),
    m_value(a_value)
  {
  }
  ~tree_node_t()
  {
  }
  inline void push_front(const_reference a_object)
  {        
    node_iterator it_node = create_node(a_object);
    m_children.insert(m_children.begin(), it_node);
  }
  inline void pop_front()
  {
    IRS_LIB_TREE_ASSERT(size() > 0);
    mp_tree->m_nodes.erase(m_children.front());
    m_children.erase(m_children.begin());
  }
  inline void push_back(const_reference a_object)
  {
    node_iterator it_node = create_node(a_object);
    m_children.insert(m_children.end(), it_node);
  }
  inline void pop_back()
  {
    IRS_LIB_TREE_ASSERT(size() > 0);
    children_iterator it_children = m_children.end();
    it_children--;
    mp_tree->m_nodes.erase(*it_children);
    m_children.erase(it_children);
  }
  inline void insert(iterator a_it_pos, const_reference a_object)
  {
    node_iterator it_node = create_node(a_object);
    m_children.insert(a_it_pos.m_local_it_child, it_node);
  }
  inline void erase(iterator a_it_pos)
  {
    const tree_node_t<T> nod = *a_it_pos;
    mp_tree->m_nodes.erase(*(a_it_pos.m_local_it_child));
    m_children.erase(a_it_pos.m_local_it_child);
  }
  inline void clear()
  {
    children_iterator it_child = m_children.begin();
    while (it_child != m_children.end()) {
      mp_tree->m_nodes.erase(*it_child);
      it_child++;
    }
    m_children.clear();
  }
  inline iterator begin()
  {
    return iterator(m_children.begin());
  }
  inline const_iterator begin() const
  {
    return const_iterator(m_children.begin());
  }
  inline iterator end()
  {
    return iterator(m_children.end());
  }
  inline const_iterator end() const
  {
    return const_iterator(m_children.end());
  }
  inline node_reference front()
  {
    IRS_LIB_TREE_ASSERT(size() > 0);
    return *(m_children.front());
  }
  inline node_const_reference front() const
  {
    IRS_LIB_TREE_ASSERT(size() > 0);
    return *(m_children.front());
  }
  inline node_reference back()
  {
    IRS_LIB_TREE_ASSERT(size() > 0);
    return *(m_children.back());
  }
  inline node_const_reference back() const
  {
    IRS_LIB_TREE_ASSERT(size() > 0);
    return *(m_children.back());
  }
  inline node_reference parent()
  {
    return *m_parent;
  }
  inline node_reference operator[](size_type a_index)
  {
    children_iterator it_children = m_children.begin();
    advance(it_children, a_index);
    return *(*it_children);
  }
  inline node_const_reference operator[](size_type a_index) const
  {
    children_const_iterator it_children = m_children.begin();
    advance(it_children, a_index);
    return *(*it_children);
  }
  inline reference value()
  {
    return m_value;
  }
  inline const_reference value() const
  {
    return m_value;
  }
  inline void value(const_reference a_value)
  {
    m_value = a_value;
  }
  inline size_type size() const
  {
    return m_children.size();
  }
  inline bool empty() const
  {
    return m_children.empty();
  }
private:
  tree_t<value_type>* mp_tree;
  node_iterator m_it_this_node;
  parent_type m_parent;
  children_type m_children;
  value_type m_value;
  friend class tree_t<T>;
  node_iterator create_node(const_reference a_value)
  {
    tree_node_t<T> child_node;
    child_node.m_value = a_value;
    child_node.mp_tree = mp_tree;
    child_node.m_parent = m_it_this_node;
    mp_tree->m_nodes.push_back(child_node);
    node_iterator it_node = mp_tree->m_nodes.end();
    it_node--;
    return it_node;
  }
}; // class tree_node_t

template<class T>
class tree_t
{
public:
  typedef T value_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef list<tree_node_t<T> > nodes_type;
  typedef typename nodes_type::iterator iterator;
  typedef typename nodes_type::const_iterator const_iterator;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef tree_node_t<T> node_type;
  typedef tree_node_t<T>* node_pointer;
  typedef tree_node_t<T>& node_reference;
  typedef const tree_node_t<T>& node_const_reference;

public:
  tree_t(): m_nodes()
  {
  }
  inline size_type size() const
  {
    return m_nodes.size();
  }
  inline bool empty() const
  {
    return m_nodes.empty();
  }
  inline node_reference top()
  {
    IRS_LIB_TREE_ASSERT(m_nodes.size() > 0);
    return m_nodes.front();
  }
  inline node_const_reference top() const
  {
    IRS_LIB_TREE_ASSERT(m_nodes.size() > 0);
    return m_nodes.front();
  }
  void create_top(const_reference a_value)
  {
    #ifdef IRS_LIB_TREE_DEBUG
    if (!m_nodes.empty()) {
      IRS_LIB_TREE_ASSERT_MSG("Дерево должно быть пустым");
    } else {
      // В дереве нет элементов
    }
    #endif
    tree_node_t<T> top_node;
    top_node.m_value = a_value;
    top_node.mp_tree = this;
    top_node.m_parent = m_nodes.end();
    m_nodes.push_back(top_node);
    iterator it_node = m_nodes.end();
    it_node--;
    it_node->m_it_this_node = it_node;
  }
  inline void clear()
  {
    m_nodes.clear();
  }
  inline iterator begin()
  {
    return m_nodes.begin();
  }
  inline const_iterator begin() const
  {
    return m_nodes.begin();
  }
  inline iterator end()
  {
    m_nodes.end();
  }
  inline const_iterator end() const
  {
    m_nodes.end();
  }
private:
  nodes_type m_nodes;
  friend class tree_node_t<T>;
};

void test_tree()
{
  irs::tree_t<double> tree;
  int tree_size = tree.size();
  tree.create_top(IRS_NULL);
  tree_size = tree.size();
  irs::tree_node_t<double> p_node = tree.top();
  p_node.value(9.09);
  p_node.push_back(9.10);
  tree_size = tree.size();
  p_node.push_front(9.11);
  tree_size = tree.size();
  p_node.pop_front();
  tree_size = tree.size();
  p_node.pop_back();
  p_node.insert(p_node.begin(), 10.1);
  tree_size = tree.size();
  //int node_child_size = p_node.size();
  irs::tree_node_t<double>::iterator it_child_node_beg = p_node.begin();
  irs::tree_node_t<double>::iterator it_child_node_end = p_node.end();
  p_node.erase(p_node.begin());
  p_node.insert(p_node.end(), 10.2);
  p_node.erase(p_node.end());
  p_node.push_front(10.1);
  p_node.push_front(10.2);
  p_node.push_front(10.3);
  p_node.push_back(10.4);
  tree_size = tree.size();
  irs::tree_node_t<double> p_front_node = p_node.front();
  tree_size = tree.size();
  irs::tree_node_t<double> p_back_node = p_node.back();
  tree_size = tree.size();
  irs::tree_node_t<double> node1 = p_node.front();
  tree_size = tree.size();
  //const int i;
  const irs::tree_node_t<double> node = p_node.back();
  //irs::tree_node_reference_t<double> node_ref3(IRS_NULL);
  //node_ref = p_node.back();
  tree_size = tree.size();
  tree_size = tree.size();
  irs::tree_node_t<double>::iterator it = p_node.begin();

  while (it != p_node.end()) {
    double d = it->value();
    d *= 10;
    it->value(d);
    it++;                   
  }
  irs::tree_node_t<double> ref2_node = tree.top();
  irs::tree_t<double>::iterator it_node = tree.begin();
  it_node++;
  irs::tree_node_t<double>::iterator child_iterator;
  child_iterator++;
  ++child_iterator;
  child_iterator--;
  --child_iterator;
  irs::tree_node_t<double>::const_iterator child_const_iterator;
  /*while (ref2_node != IRS_NULL) {
    ref2_node = ref2_node->next();
  }*/
  tree_size = tree.size();
}

#endif //Compilers
#endif // NOP

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
  p_children_end(ap_children_end)
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
  typedef random_access_iterator_tag iterator_category;
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
  typedef random_access_iterator_tag iterator_category;
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
class basic_tree_iterator_t
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
public:
  basic_tree_iterator_t(node_pointer ap_node  = IRS_NULL);
  basic_tree_iterator_t(const basic_tree_iterator_t& a_basic_tree_iterator);
  void swap(basic_tree_iterator_t& a_basic_tree_iterator);
  basic_tree_iterator_t operator=(
    const basic_tree_iterator_t& a_basic_tree_iterator);
  bool operator==(const basic_tree_iterator_t& a_basic_tree_iterator) const;
  bool operator!=(const basic_tree_iterator_t& a_basic_tree_iterator) const;
  reference operator*();
  basic_tree_iterator_t& operator--();
  basic_tree_iterator_t operator--(int);
  basic_tree_iterator_t& operator++();
  basic_tree_iterator_t operator++(int);
  void go_root();
  void go_begin();
  void go_end();
  void go_next();
  void go_prev();
  void go_parent();
  void go_children_begin();
  void go_children_end();
  void go_prev_sibling();
  void go_next_sibling();
  void go_siblings_begin();
  void go_siblings_end();
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
basic_tree_iterator_t<T> basic_tree_iterator_t<T>::operator=(
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
  *this = mp_node->mp_tree->root();
}

template <class T>
void basic_tree_iterator_t<T>::go_begin()
{
  *this = mp_node->mp_tree->begin();
}

template <class T>
void basic_tree_iterator_t<T>::go_end()
{
  *this = mp_node->mp_tree->end();
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
  tree_iterator_t(node_pointer ap_node_pointer = IRS_NULL);
  tree_iterator_t(const tree_iterator_t& a_tree_iterator);
  tree_iterator_t& operator=(const tree_iterator_t& a_tree_iterator);
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
tree_iterator_t<T>& tree_iterator_t<T>::operator=(
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
class basic_tree_child_iterator_t: public basic_tree_iterator_t<IteratorTag>
{
private:
  typedef basic_tree_iterator_t<IteratorTag> basic_tree_iterator;
protected:
  typedef typename IteratorTag::node_pointer node_pointer;
public:
  basic_tree_child_iterator_t(const node_pointer ap_node_pointer = IRS_NULL);
  basic_tree_iterator_t<IteratorTag> tree_iterator();
  basic_tree_child_iterator_t& operator--();
  basic_tree_child_iterator_t operator--(int);
  basic_tree_child_iterator_t& operator++();
  basic_tree_child_iterator_t operator++(int);
  void go_begin();
  void go_end();
  void go_next();
  void go_prev();
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

template <class IteratorTag>
void basic_tree_child_iterator_t<IteratorTag>::go_begin()
{
  if (mp_node != mp_node->p_tree->root().mp_node) {
    mp_node = mp_node->p_parent->p_children_begin;
  }
}

template <class IteratorTag>
void basic_tree_child_iterator_t<IteratorTag>::go_end()
{
  if (mp_node != mp_node->p_tree->root().mp_node) {
    mp_node = mp_node->p_parent->p_children_begin;
  } else {
    mp_node = mp_node->p_tree->end().mp_node;
  }
}

template <class IteratorTag>
void basic_tree_child_iterator_t<IteratorTag>::go_next()
{
  go_next_sibling();
}

template <class IteratorTag>
void basic_tree_child_iterator_t<IteratorTag>::go_prev()
{
  go_prev_sibling();
}

template <class T>
class tree_child_iterator_t:
  public basic_tree_child_iterator_t<tree_iterator_tags<T*> >
{
private:
  typedef basic_tree_child_iterator_t<tree_iterator_tags<T*> > base_iterator;
public:
  typedef typename base_iterator::node_pointer node_pointer;
  tree_child_iterator_t(node_pointer ap_node_pointer = IRS_NULL);
  tree_child_iterator_t(const tree_child_iterator_t& a_tree_child_iterator);
  tree_child_iterator_t(const tree_iterator_t<T>& a_tree_iterator);
  tree_child_iterator_t& operator=(
    const tree_child_iterator_t& a_tree_child_iterator);
  tree_child_iterator_t& operator=(const tree_iterator_t<T>& a_tree_iterator);
  using base_iterator::swap;
};

template <class T>
tree_child_iterator_t<T>::tree_child_iterator_t(
  node_pointer ap_node_pointer
):
  base_iterator(ap_node_pointer)
{
}

template <class T>
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
  typedef basic_tree_child_iterator_t<tree_iterator_tags<const T*> >
    base_iterator;
public:
  typedef typename base_iterator::node_pointer node_pointer;
  const_tree_child_iterator_t(node_pointer ap_node_pointer = IRS_NULL);
  const_tree_child_iterator_t(
    const const_tree_child_iterator_t<T>& a_const_tree_child_iterator);
  const_tree_child_iterator_t(
    const tree_child_iterator_t<T>& a_tree_child_iterator);
  const_tree_child_iterator_t& operator=(
    const const_tree_child_iterator_t& a_const_tree_child_iterator);
};

template <class T>
const_tree_child_iterator_t<T>::const_tree_child_iterator_t(
  node_pointer ap_node_pointer
):
  base_iterator(ap_node_pointer)
{
}

template <class T>
const_tree_child_iterator_t<T>::const_tree_child_iterator_t(
  const const_tree_child_iterator_t<T>& a_const_tree_child_iterator
):
  base_iterator(a_const_tree_child_iterator.mp_node)
{
}

template <class T>
const_tree_child_iterator_t<T>::
const_tree_child_iterator_t(
  const tree_child_iterator_t<T>& a_tree_child_iterator
):
  base_iterator(a_tree_child_iterator.mp_node)
{
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
  void push_front(iterator a_sibling, const value_type& a_value);
  //! \brief Удаление узла из начала списка узлов,
  //!   в котором находится a_sibling.
  void pop_front(iterator a_sibling);
  //! \brief Вставка узла в конец списка узлов, в котором находится a_sibling.
  void push_back(iterator a_sibling, const value_type& a_value);
  //! \brief Удаление узла из конца списка узлов, в котором находится a_sibling.
  void pop_back(iterator a_sibling);
  //! \brief Вставка узла в позицию перед узлом a_sibling.
  iterator insert(iterator a_pos, const value_type& a_value);
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
void tree_t<T>::push_front(iterator a_sibling, const value_type& a_value)
{
  IRS_LIB_ASSERT(a_sibling != iterator(mp_root));
  push_front_child(a_sibling.mp_node->p_parent, a_value);
}

template <class T>
void tree_t<T>::pop_front(iterator a_sibling)
{
  IRS_LIB_ASSERT(a_sibling != iterator(mp_root));
  pop_front_child(a_sibling.mp_node->p_parent);
}

template <class T>
void tree_t<T>::push_back(iterator a_sibling, const value_type& a_value)
{
  push_back_child(iterator(a_sibling.mp_node->p_parent), a_value);
}

template <class T>
void tree_t<T>::pop_back(iterator a_sibling)
{
  IRS_LIB_ASSERT(a_sibling != iterator(mp_root));
  pop_back_child(a_sibling.mp_node->p_parent);
}

template <class T>
typename tree_t<T>::iterator tree_t<T>::insert(iterator a_pos,
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
  insert(iterator(a_parent.mp_node->p_children_begin), a_value);
}

template <class T>
void tree_t<T>::pop_front_child(iterator a_parent)
{
  erase(iterator(a_parent.mp_node->p_children_begin));
}

template <class T>
void tree_t<T>::push_back_child(iterator a_parent, const value_type& a_value)
{
  insert(iterator(a_parent.mp_node->p_children_end), a_value);
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
  tree.push_back(tree.begin(), 0.5);
  tree.push_front(tree.begin(), 0.6);
  tree.pop_back(tree.begin());
  tree.pop_front(tree.begin());
  IRS_LIB_ASSERT(tree.size() == 3);
  tree_t<double>::iterator it = tree.begin();
  tree_t<double>::child_iterator child_it1(it);
  tree_t<double>::child_iterator child_it2(child_it1);
  child_it2 = child_it1;
  child_it2 = it;
  tree_t<double>::const_child_iterator const_child_it1(it);
  tree_t<double>::const_child_iterator const_child_it2(child_it1);
  tree_t<double>::const_child_iterator const_child_it3(const_child_it1);
  const_child_it3.go_next_sibling();
  const_child_it3.go_prev_sibling();
  const_child_it3.go_end();
  const_child_it3.go_begin();
  const_child_it3.go_next();
  const_child_it3.go_prev();
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



