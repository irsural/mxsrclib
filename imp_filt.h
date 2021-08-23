#ifndef imp_filtH
#define imp_filtH

#include <irsdefs.h>

#include <irscpp.h>

#include <irsfinal.h>

#define cont_out(cont)\
{\
  irs::mlog() << #cont " = ";\
  show_elem(cont, irs::mlog());\
}

namespace irs {

extern bool is_dbg;

typedef double impf_value_t;
  
template <class I>
void show_elem(I it_begin, I it_end, ostream& a_stream)
{
  a_stream << "{ ";
  I it_penult = it_end;
  --it_penult;
  for (I it = it_begin; it != it_penult; ++it) {
    a_stream << (*it) << "; ";
  }
  a_stream << (*it_penult) << " }" << endl;
}
template <class C>
void show_elem(C a_cont, ostream& a_stream)
{
  show_elem(a_cont.begin(), a_cont.end(), a_stream);
}

class filt_imp_noise_t
{
public:
  typedef double value_type;
  typedef vector<value_type> signal_type;
  typedef signal_type::iterator signal_it_type;

  filt_imp_noise_t(size_t a_max_size = 0);
  filt_imp_noise_t(const vector<value_type>& a_samples);
  size_t size();
  // Класс работает при a_max_size >= 3
  void max_size(size_t a_max_size);
  size_t max_size();
  void clear();
  void add(value_type a_sample);
  void assign(const vector<value_type>& a_samples);
  //Выдает корректные значения, только когда size() == max_size()
  value_type get();
  void signal_on(bool a_on);
  bool signal_on();
  void signal_get(signal_type* ap_signal);
private:
  typedef multiset<value_type> sort_cont_type;
  typedef sort_cont_type::iterator sort_it_type;
  typedef pair<sort_it_type, sort_it_type> range_type;
  typedef deque<sort_it_type> unsort_cont_type;
  //typedef unsort_cont_type::iterator unsort_it_type;
  typedef multimap<value_type, size_t> signal_sort_cont_type;
  typedef signal_sort_cont_type::iterator signal_sort_it_type;

  struct rep_t
  {
    rep_t();

    sort_cont_type sort_samples;
    unsort_cont_type unsort_samples;
    sort_cont_type sort_samples_work;
    size_t max_size;
    value_type result;
    value_type sum;
    value_type sum_pos_prev;
    bool is_first_calc;
    value_type avg_prev;
    value_type left_pos_prev_val;
    value_type back_val;
    size_t P_prev;
    signal_sort_cont_type signal_sort_samples;
    bool signal_on;
  };

  rep_t m_rep;

  void add_internal(value_type a_sample);
  void calc();
  value_type find_avg_rectification(value_type a_avg);
};

impf_value_t calc_impf(vector<impf_value_t>* ap_impf_vector);

class calc_impf_shift_t
{
public:
  calc_impf_shift_t(size_t a_max_size);
  void add(impf_value_t a_value);
  impf_value_t get();
private:
  size_t m_max_size;
  vector<impf_value_t> m_value_list;
  impf_value_t m_result;
};

} //  irs

#endif  //  imp_filtH
