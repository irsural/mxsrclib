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

#if (defined(__BORLANDC__) && (__BORLANDC__ >= IRS_CPP_BUILDER6) &&\
  (__BORLANDC__ <= (IRS_CPP_BUILDER6 + 4))) ||\
  defined(IRS_LINUX) ||\
  defined(_MSC_VER) //||
  // defined(__ICCAVR__)

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


}; // namespace irs

template<class T>
class obj_t
{
public:
  typedef typename list<obj_t<T> >::iterator iterator_list_var;
private:
   T m_value;
};

#endif //irstreeH



