template <class T>
class my_t
{
public:
  template <class Y>
  void func(T t, Y y);
};

template <class T>
template <class Y>
void my_t<T>::func(T t, Y y)
{
}
