//! \file
//! \ingroup math_group
//! \brief Калибровка приборов
//!
//! Дата: 04.04.2011\n
//! Ранняя дата: 27.03.2009

#ifndef correct_algH
#define correct_algH

#include <irsdefs.h>

#include <irsdefs.h>
#include <irsstd.h>
#include <timer.h>
#include <mxnetc.h>

#include <irsfinal.h>

namespace irs
{

namespace avr
{

//! \addtogroup math_group
//! @{

class step_correct_t
{
public:
  typedef irs_i32 hor_t;
  typedef irs_i32 ver_t;
  enum correct_error_t
  {
    error_null,
    error_default_map,
    error_life_is_shit
  };
  struct correct_map_t
  {
    irs_u8 m_map_id;  //  not zero
    irs_u8 m_col_count;
    irs_u8 m_row_count;
    hor_t *mp_hor_points;
    ver_t *mp_ver_points;
    float *mp_k_array;
    float *mp_b_array;
  };
  step_correct_t(correct_map_t *ap_map);
  ~step_correct_t();
  correct_error_t get_error();
  ver_t apply(hor_t a_hor_value, ver_t a_ver_value);
  irs_bool apply_map(correct_map_t *ap_map);
private:
  correct_error_t m_current_error;
  correct_map_t *mp_map;
};

//! @}

} //  avr

//! \addtogroup math_group
//! @{

template <class IN_X, class IN_Y, class OUT_Z, class K>
class correct_lin_t
{
public:
  struct correct_map_t
  {
    irs_uarc m_map_id;
    irs_uarc m_x_count;
    irs_uarc m_y_count;
    IN_X *mp_x_points;
    IN_Y *mp_y_points;
    K *mp_k_array;
    K *mp_b_array;
  };
  struct triple_line_data_t
  {
    K x1;
    K x2;
    K y1;
    K y2;
    K z11;
    K z12;
    K z21;
    K z22;
    K x0;
    K y0;
  };
  correct_lin_t(correct_map_t *ap_map);
  ~correct_lin_t();
  OUT_Z apply(IN_X a_x_value, IN_Y a_y_value, OUT_Z a_in_value);
  K triple_line(triple_line_data_t a_data);
  irs_bool apply_map(correct_map_t *ap_map);
  irs_bool test_map(correct_map_t *ap_map);
private:
  correct_map_t *mp_map;
  irs_bool m_error;
};

//------------------------------------------------------------------------------
template <class IN_X, class IN_Y, class OUT_Z, class K>
irs::correct_lin_t<IN_X, IN_Y, OUT_Z, K>::correct_lin_t(correct_map_t *ap_map):
  mp_map(ap_map)
{
  if (test_map(mp_map)) m_error = irs_false;
  else m_error = irs_true;
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
irs::correct_lin_t<IN_X, IN_Y, OUT_Z, K>::~correct_lin_t()
{
  //
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
OUT_Z irs::correct_lin_t<IN_X, IN_Y, OUT_Z, K>::apply(IN_X a_x_value,
                                                      IN_Y a_y_value,
                                                      OUT_Z a_in_value)
{
  OUT_Z out_value = a_in_value;
  irs_uarc x_offset = 0;
  irs_uarc x_index = 0;
  irs_uarc y_index = 0;
  triple_line_data_t m_data;
  K k, b;

  if (m_error)
  {
    if (test_map(mp_map)) m_error = irs_false;
  }
  if (!m_error)
  {
    /*
                 0   1   2   ...   m_x_count-1
              0  k   k   k   ...   k
              1  k   k   k   ...   k
              2  k   k   k   ...   k
            ...  ... ... ... ...  ...
    m_y_count-1  k   k   k   ...   k
    */
    //
    if (a_y_value >= mp_map->mp_y_points[mp_map->m_y_count-1])
    {
      y_index = mp_map->m_y_count - 1;
      a_y_value = mp_map->mp_y_points[y_index];
      x_offset = y_index * mp_map->m_x_count;
    }
    else
    {
      for (y_index = 0; y_index < mp_map->m_y_count; y_index++)
        if (a_y_value < mp_map->mp_y_points[y_index])
        {
          x_offset = y_index * mp_map->m_x_count;
          break;
        }
    }
    if (y_index < 1)
    {
      a_y_value = mp_map->mp_y_points[0];
      y_index = 1;
      x_offset = mp_map->m_x_count;
    }
    if (a_x_value >= mp_map->mp_x_points[mp_map->m_x_count-1])
    {
      out_value = OUT_Z(K(a_in_value)
        * mp_map->mp_k_array[mp_map->m_x_count-1 + x_offset]
        + mp_map->mp_b_array[mp_map->m_x_count-1 + x_offset]);
    }
    else
    {
      for (x_index = 0; x_index < mp_map->m_x_count; x_index++)
        if (a_x_value < mp_map->mp_x_points[x_index])
        {
          if (x_index < 1)
          {
            a_x_value = mp_map->mp_x_points[0];
            x_index = 1;
          }
          m_data.x1 = mp_map->mp_x_points[x_index - 1];
          m_data.x2 = mp_map->mp_x_points[x_index];
          m_data.y1 = mp_map->mp_y_points[y_index - 1];
          m_data.y2 = mp_map->mp_y_points[y_index];
          m_data.z11 = mp_map->mp_k_array[(x_index - 1)
            + (x_offset - mp_map->m_x_count)];
          m_data.z21 = mp_map->mp_k_array[x_index
            + (x_offset - mp_map->m_x_count)];
          m_data.z12 = mp_map->mp_k_array[(x_index - 1)+ x_offset];
          m_data.z22 = mp_map->mp_k_array[x_index + x_offset];
          m_data.x0 = a_x_value;
          m_data.y0 = a_y_value;

          k = triple_line(m_data);

          m_data.z11 = mp_map->mp_b_array[(x_index - 1)
            + (x_offset - mp_map->m_x_count)];
          m_data.z21 = mp_map->mp_b_array[x_index
            + (x_offset - mp_map->m_x_count)];
          m_data.z12 = mp_map->mp_b_array[(x_index - 1)+ x_offset];
          m_data.z22 = mp_map->mp_b_array[x_index + x_offset];

          b = triple_line(m_data);
          out_value = OUT_Z(K(a_in_value) * k + b);
          break;
        }
    }
  }
  return out_value;
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
K irs::correct_lin_t<IN_X, IN_Y, OUT_Z, K>::triple_line(
  triple_line_data_t a_data)
{
  K k, b, z01, z02;
  K delta_x = a_data.x2 - a_data.x1;

  k = (a_data.z21 - a_data.z11) / delta_x;
  b = a_data.z21 - k * a_data.x2;
  z01 = k * a_data.x0 + b;

  k = (a_data.z22 - a_data.z12) / delta_x;
  b = a_data.z22 - k * a_data.x2;
  z02 = k * a_data.x0 + b;

  k = (z02 - z01) / (a_data.y2 - a_data.y1);
  b = z02 - k * a_data.y2;

  return k * a_data.y0 + b;
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
irs_bool irs::correct_lin_t<IN_X, IN_Y, OUT_Z, K>::test_map(correct_map_t*
  ap_map)
{
  if (ap_map->m_map_id == 0) return irs_false;
  if (ap_map->m_x_count < 1) return irs_false;
  if (ap_map->m_y_count < 1) return irs_false;
  for (irs_uarc x_index = 1; x_index < ap_map->m_x_count; x_index++)
    if (ap_map->mp_x_points[x_index] <= ap_map->mp_x_points[x_index-1])
      return irs_false;
  for (irs_uarc y_index = 1; y_index < ap_map->m_y_count; y_index++)
    if (ap_map->mp_y_points[y_index] <= ap_map->mp_y_points[y_index-1])
      return irs_false;

  return irs_true;
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
irs_bool irs::correct_lin_t<IN_X, IN_Y, OUT_Z, K>::apply_map(correct_map_t*
  ap_map)
{
  mp_map = ap_map;
  if (test_map(mp_map))
  {
    m_error = irs_false;
    return irs_true;
  }
  else
  {
    m_error = irs_true;
    return irs_false;
  }
}

//------------------------------------------------------------------------------

template <class IN_X, class IN_Y, class OUT_Z, class K>
struct correct_map_t
{
  correct_map_t();
  conn_data_t<irs_u32> m_map_id;
  conn_data_t<irs_u32> m_x_count;
  conn_data_t<irs_u32> m_y_count;
  array_data_t<IN_X> mp_x_points;
  array_data_t<IN_Y> mp_y_points;
  array_data_t<K> mp_k_array;
  array_data_t<K> mp_b_array;
  irs_uarc connect(mxdata_t *ap_data, irs_uarc a_start_index, bool a_use_b);
};

template <class IN_X, class IN_Y, class OUT_Z, class K>
irs::correct_map_t<IN_X, IN_Y, OUT_Z, K>::correct_map_t():
  m_map_id()
{
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
irs_uarc irs::correct_map_t<IN_X, IN_Y, OUT_Z, K>::connect(mxdata_t *ap_data,
  irs_uarc a_start_index, bool a_use_b)
{
  irs_uarc index = a_start_index;
  index = m_map_id.connect(ap_data, index);
  index = m_x_count.connect(ap_data, index);
  index = m_y_count.connect(ap_data, index);
  index = mp_x_points.connect(ap_data, index, m_x_count);
  index = mp_y_points.connect(ap_data, index, m_y_count);
  irs_uarc k_b_size = m_x_count * m_y_count;
  index = mp_k_array.connect(ap_data, index, k_b_size);
  if (a_use_b) index = mp_b_array.connect(ap_data, index, k_b_size);
  return index;
}


template <class IN_X, class IN_Y, class OUT_Z, class K>
class correct_t
{
public:
  struct triple_line_data_t
  {
    K x1;
    K x2;
    K y1;
    K y2;
    K z11;
    K z12;
    K z21;
    K z22;
    K x0;
    K y0;
    irs_uarc x_offset;
    irs_uarc x_index;
    irs_uarc y_index;
  };

  correct_t(mxdata_t *ap_data, irs_uarc a_start_index, bool a_use_b);
  ~correct_t();
  void preapre_data(IN_X a_x_value, IN_Y a_y_value, OUT_Z a_in_value,
    triple_line_data_t* ap_tl_data);
  OUT_Z apply(IN_X a_x_value, IN_Y a_y_value, OUT_Z a_in_value);
  OUT_Z apply_dac(IN_X a_x_value, IN_Y a_y_value);
  K triple_line(triple_line_data_t a_data);
  irs::correct_map_t<IN_X, IN_Y, OUT_Z, K>* map();
  K k(irs_uarc a_x_index, irs_uarc a_y_index);
  OUT_Z level(irs_uarc a_x_index, irs_uarc a_y_index);
private:
  irs_uarc m_start_index;
  mxdata_t *mp_data;
  irs::correct_map_t<IN_X, IN_Y, OUT_Z, K> m_map;
  irs::correct_map_t<IN_X, IN_Y, OUT_Z, K>* mp_map;
  irs_bool m_error;
  bool m_use_b;

  irs_bool test_map();
};

//------------------------------------------------------------------------------
template <class IN_X, class IN_Y, class OUT_Z, class K>
irs::correct_t<IN_X, IN_Y, OUT_Z, K>::correct_t(mxdata_t *ap_data,
  irs_uarc a_start_index, bool a_use_b):
  m_start_index(a_start_index),
  mp_data(ap_data),
  m_map(),
  mp_map(&m_map),
  m_error(true),
  m_use_b(a_use_b)
{
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
irs::correct_t<IN_X, IN_Y, OUT_Z, K>::~correct_t()
{
  //
}

// Считывание коэффициентов коррекции
// Обычно
// a_x_index - индекс по частоте для переменного тока
// a_y_index - индекс по уровню силы тока или напряжению
template <class IN_X, class IN_Y, class OUT_Z, class K>
K irs::correct_t<IN_X, IN_Y, OUT_Z, K>::
k(irs_uarc a_x_index, irs_uarc a_y_index)
{
  return mp_map->mp_k_array[a_x_index + a_y_index*mp_map->m_x_count];
}

// Считывание уровня значений коррекции (напряжение или сила тока)
// См. также комментарий к функции k
template <class IN_X, class IN_Y, class OUT_Z, class K>
OUT_Z irs::correct_t<IN_X, IN_Y, OUT_Z, K>::
level(irs_uarc a_x_index, irs_uarc a_y_index)
{
  return mp_map->mp_y_points[a_y_index]*static_cast<OUT_Z>(
    mp_map->mp_k_array[a_x_index + a_y_index*mp_map->m_x_count]);
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
irs::correct_map_t<IN_X, IN_Y, OUT_Z, K>*
irs::correct_t<IN_X, IN_Y, OUT_Z, K>::
map()
{
  return mp_map;
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
void irs::correct_t<IN_X, IN_Y, OUT_Z, K>::
preapre_data(IN_X a_x_value, IN_Y a_y_value, OUT_Z a_in_value,
  triple_line_data_t* ap_tl_data)
{
  memsetex(ap_tl_data, sizeof(*ap_tl_data));
  if (mp_data->connected())
  {
    triple_line_data_t& tl_data = *ap_tl_data;
    mp_map->connect(mp_data, m_start_index, m_use_b);
    if (m_error) {
      if (test_map()) m_error = irs_false;
    }
    if (!m_error) {
      irs_uarc x_offset = 0;
      irs_uarc x_index = 0;
      irs_uarc y_index = 0;

      if (a_y_value >= mp_map->mp_y_points[mp_map->m_y_count-1])
      {
        y_index = mp_map->m_y_count - 1;
      }
      else
      {
        for (y_index = 0; y_index < mp_map->m_y_count; y_index++)
          if (a_y_value < mp_map->mp_y_points[y_index]) {
            if (y_index < 1) {
              y_index = 1;
            }
            break;
          }
      }
      x_offset = y_index * mp_map->m_x_count;
      if (a_x_value >= mp_map->mp_x_points[mp_map->m_x_count-1]) {
        x_index = mp_map->m_x_count - 1;
      } else {
        for (x_index = 0; x_index < mp_map->m_x_count; x_index++) {
          if (a_x_value < mp_map->mp_x_points[x_index])
          {
            if (x_index < 1) {
              x_index = 1;
            }
            break;
          }
        }
      }
      tl_data.x1 = mp_map->mp_x_points[x_index - 1];
      tl_data.x2 = mp_map->mp_x_points[x_index];
      tl_data.y1 = mp_map->mp_y_points[y_index - 1];
      tl_data.y2 = mp_map->mp_y_points[y_index];
      tl_data.x0 = a_x_value;
      tl_data.y0 = a_y_value;
      tl_data.x_offset = x_offset;
      tl_data.x_index = x_index;
      tl_data.y_index = y_index;
    }
  }
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
OUT_Z irs::correct_t<IN_X, IN_Y, OUT_Z, K>::
apply(IN_X a_x_value, IN_Y a_y_value, OUT_Z a_in_value)
{
  OUT_Z out_value = a_in_value;

  if (mp_data->connected())
  {
    triple_line_data_t tl_data;
    preapre_data(a_x_value, a_y_value, a_in_value, &tl_data);

    if (!m_error)
    {
      irs_uarc x_offset = tl_data.x_offset;
      irs_uarc x_index = tl_data.x_index;
      irs_uarc y_index = tl_data.y_index;

      if (m_use_b) {
        // Старый алгоритм, который используется только в фазе
        tl_data.z11 = mp_map->mp_k_array[(x_index - 1)
          + (x_offset - mp_map->m_x_count)];
        tl_data.z21 = mp_map->mp_k_array[x_index
          + (x_offset - mp_map->m_x_count)];
        tl_data.z12 = mp_map->mp_k_array[(x_index - 1) + x_offset];
        tl_data.z22 = mp_map->mp_k_array[x_index + x_offset];

        K k = triple_line(tl_data);

        tl_data.z11 = mp_map->mp_b_array[(x_index - 1)
          + (x_offset - mp_map->m_x_count)];
        tl_data.z21 = mp_map->mp_b_array[x_index
          + (x_offset - mp_map->m_x_count)];
        tl_data.z12 = mp_map->mp_b_array[(x_index - 1) + x_offset];
        tl_data.z22 = mp_map->mp_b_array[x_index + x_offset];

        K b = triple_line(tl_data);

        K in = static_cast<K>(a_in_value);

        K out = k*in + b;

        out_value = static_cast<OUT_Z>(out);
      } else {
        // Новый алгоритм, который используется для АЦП (ток и напряжение)
        // Должно быть так: a_y_value == a_in_value
        IN_Y y1 = mp_map->mp_y_points[y_index - 1];
        IN_Y y = mp_map->mp_y_points[y_index];
        tl_data.z11 = y1*mp_map->mp_k_array[(x_index - 1)
          + (x_offset - mp_map->m_x_count)];
        tl_data.z21 = y1*mp_map->mp_k_array[x_index
          + (x_offset - mp_map->m_x_count)];
        tl_data.z12 = y*mp_map->mp_k_array[(x_index - 1) + x_offset];
        tl_data.z22 = y*mp_map->mp_k_array[x_index + x_offset];

        out_value = triple_line(tl_data);
      }
    }
  }
  return out_value;
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
OUT_Z irs::correct_t<IN_X, IN_Y, OUT_Z, K>::
apply_dac(IN_X a_x_value, IN_Y a_y_value)
{
  // a_in_value убрано, так как a_y_value == a_in_value
  IRS_LIB_DBG_MSG_IF(m_use_b, "Функцию OUT_Z irs::correct_t<IN_X, IN_Y, "
    "OUT_Z, K>::apply_dac нельзя вызывать когда включен параметр m_use_b");

  OUT_Z out_value = static_cast<OUT_Z>(a_y_value);

  if (mp_data->connected())
  {
    triple_line_data_t tl_data;
    preapre_data(a_x_value, a_y_value, a_y_value, &tl_data);

    if (!m_error)
    {
      irs_uarc x_offset = tl_data.x_offset;
      irs_uarc x_index = tl_data.x_index;
      irs_uarc y_index = tl_data.y_index;

      if (!m_use_b) {
        // Новый алгоритм, который используется для ЦАП (ток и напряжение)
        IN_Y y1 = mp_map->mp_y_points[y_index - 1];
        IN_Y y = mp_map->mp_y_points[y_index];
        tl_data.z11 = y1/mp_map->mp_k_array[(x_index - 1)
          + (x_offset - mp_map->m_x_count)];
        tl_data.z21 = y1/mp_map->mp_k_array[x_index
          + (x_offset - mp_map->m_x_count)];
        tl_data.z12 = y/mp_map->mp_k_array[(x_index - 1) + x_offset];
        tl_data.z22 = y/mp_map->mp_k_array[x_index + x_offset];

        out_value = triple_line(tl_data);
      }
    }
  }
  return out_value;
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
K irs::correct_t<IN_X, IN_Y, OUT_Z, K>::
triple_line(triple_line_data_t a_data)
{
  K k, b, z01, z02;
  K delta_x = a_data.x2 - a_data.x1;

  k = (a_data.z21 - a_data.z11) / delta_x;
  b = a_data.z21 - k * a_data.x2;
  z01 = k * a_data.x0 + b;

  k = (a_data.z22 - a_data.z12) / delta_x;
  b = a_data.z22 - k * a_data.x2;
  z02 = k * a_data.x0 + b;

  k = (z02 - z01) / (a_data.y2 - a_data.y1);
  b = z02 - k * a_data.y2;

  return k * a_data.y0 + b;
}

template <class IN_X, class IN_Y, class OUT_Z, class K>
irs_bool irs::correct_t<IN_X, IN_Y, OUT_Z, K>::test_map()
{
  if (mp_map->m_map_id == 0) return irs_false;
  if (mp_map->m_x_count < 1) return irs_false;
  if (mp_map->m_y_count < 1) return irs_false;
  for (irs_uarc x_index = 1; x_index < mp_map->m_x_count; x_index++) {
    if (mp_map->mp_x_points[x_index] <= mp_map->mp_x_points[x_index-1]) {
      return irs_false;
    }
  }
  for (irs_uarc y_index = 1; y_index < mp_map->m_y_count; y_index++) {
    if (mp_map->mp_y_points[y_index] <= mp_map->mp_y_points[y_index-1]) {
      return irs_false;
    }
  }
  return irs_true;
}

//------------------------------------------------------------------------------
/*
class mxdata_to_u8_t : public mxdata_t
{
  irs_bool m_is_connected;
  irs_u8 *mp_u8_data;
  irs_uarc m_u8_size;
public:
  mxdata_to_u8_t(irs_u8 *ap_u8_data, irs_uarc a_u8_size);
  ~mxdata_to_u8_t();
  irs_uarc size();
  irs_bool connected();
  void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  void tick();
};
*/
class quasi_eeprom_t : public mxdata_t
{
  irs_u8 *mp_buf;
  irs_uarc m_size;
public:
  quasi_eeprom_t(irs_u8 *ap_buf, irs_uarc a_size);
  ~quasi_eeprom_t();
  irs_uarc size();
  irs_bool connected();
  void read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  void write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size);
  irs_bool bit(irs_uarc a_index, irs_uarc a_bit_index);
  void set_bit(irs_uarc a_index, irs_uarc a_bit_index);
  void clear_bit(irs_uarc a_index, irs_uarc a_bit_index);
  void tick();
};

//! @}

} //  irs

#endif  //correct_algH

