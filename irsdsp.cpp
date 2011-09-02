//! \file
//! \ingroup signal_processing_group
//! \brief Цифровая обработка сигналов
//!
//! Дата: 20.09.2009\n
//! Ранняя Дата: 27.08.2009

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <math.h>

#include <irsdsp.h>
#include <irserror.h>

#include <irsfinal.h>

#ifndef NEW
// ПИД-регулятор
#define NEW_REG_02092011
double irs::pid_reg(pid_data_t *pd, double error_in)
{
  double dif_int = pd->prev_e;
  double res = 0;
  if (pd->block_int == 1) if (pd->prev_e > 0) dif_int = 0;
  if (pd->block_int == -1) if (pd->prev_e < 0) dif_int = 0;
  if (pd->block_int_ext == 1) if (pd->prev_e > 0) dif_int = 0;
  if (pd->block_int_ext == -1) if (pd->prev_e < 0) dif_int = 0;
  pd->int_val += dif_int;
  double d_pid = pd->k_d_pid*(pd->max - pd->min);
  if ((pd->k != 0.) && (pd->ki != 0.)) {
    double imin = (pd->min - d_pid)/pd->k/pd->ki;
    double imax = (pd->max + d_pid)/pd->k/pd->ki;
    pd->int_val = bound(pd->int_val, imin, imax);
  }
  double Ires = pd->k*pd->ki*pd->int_val;
  double Pres = pd->k*error_in;
  double Dres = pd->k*pd->kd*(error_in - pd->prev_e);
  double PIres = Pres + Ires;
  //double PDres = Pres + Dres;
  //res = Ires + PDres;
  res = PIres + Dres;
  #ifdef NEW_REG_02092011
  if (pd->int_val >= 0) {
  #endif //NEW_REG_02092011
    if (PIres > pd->max + d_pid) {
      if (Pres > pd->max || (pd->k == 0.) || (pd->ki == 0.)) {
        pd->int_val = 0.;
        res = Pres;
      } else {
        pd->int_val = (pd->max - Pres)/pd->k/pd->ki;
        res = pd->max;
      }
    }
  #ifdef NEW_REG_02092011
  }
  #endif //NEW_REG_02092011
  #ifdef NEW_REG_02092011
  if (pd->int_val <= 0) {
    if (PIres < pd->min - d_pid) {
      if (Pres < pd->min || (pd->k == 0.) || (pd->ki == 0.)) {
        pd->int_val = 0.;
        res = Pres;
      } else {
        pd->int_val = (pd->min - Pres)/pd->k/pd->ki;
        res = pd->min;
      }
    }
  }
  #endif //NEW_REG_02092011
  #ifdef NEW_REG_02092011
  if (PIres > pd->max + d_pid) {
  #else //NEW_REG_02092011
  if (PIres > pd->max - d_pid) {
  #endif //NEW_REG_02092011
    pd->block_int = 1;
  #ifdef NEW_REG_02092011
  #else //NEW_REG_02092011
  } else if (PIres < pd->min) {
  #endif //NEW_REG_02092011
    pd->block_int = -1;
  } else {
    pd->block_int = 0;
  }
  res = bound(res, pd->min, pd->max);
  pd->pp_e = pd->prev_e;
  pd->prev_e = error_in;
  pd->prev_out = res;
  return res;
}
/*
double irs::pid_reg(pid_data_t *pd, double e)
{
  double dif_int = pd->prev_e;
  double res = 0;
  if (pd->block_int == 1) if (pd->prev_e > 0) dif_int = 0;
  if (pd->block_int == -1) if (pd->prev_e < 0) dif_int = 0;
  if (pd->block_int_ext == 1) if (pd->prev_e > 0) dif_int = 0;
  if (pd->block_int_ext == -1) if (pd->prev_e < 0) dif_int = 0;
  pd->int_val += dif_int;
  double d_pid = pd->k_d_pid*(pd->max - pd->min);
  if ((pd->k != 0.) && (pd->ki != 0.)) {
    double imin = (pd->min - d_pid)/pd->k/pd->ki;
    double imax = (pd->max + d_pid)/pd->k/pd->ki;
    pd->int_val = bound(pd->int_val, imin, imax);
  }
  double Ires = pd->k*pd->ki*pd->int_val;
  double PDres = pd->k*(e + pd->kd*(e - pd->prev_e));
  res = Ires + PDres;
  if (res > pd->max + d_pid) {
    if (PDres > pd->max || (pd->k == 0.) || (pd->ki == 0.)) {
      pd->int_val = 0.;
      res = PDres;
    } else {
      pd->int_val = (pd->max - PDres)/pd->k/pd->ki;
      res = pd->max;
    }
  }
  if (PDres < -d_pid) {
    if (pd->int_val < 0.) pd->int_val = 0.;
  }
  if (res > pd->max) {
    res = pd->max;
    pd->block_int = 1;
  } else if (res < pd->min) {
    res = pd->min;
    pd->block_int = -1;
  } else {
    pd->block_int = 0;
  }
  res = bound(res, pd->min, pd->max);
  pd->pp_e = pd->prev_e;
  pd->prev_e = e;
  pd->prev_out = res;
  return res;
}
*/
// Синхронизация ПИД-регулятора на текущее значение регулируемого параметра
// при выключенном регуляторе
// pd - Параметры ПИД-регулятора,
// e - ошибка на входе регулятора
// rp - значение выдаваемое на исполнительный механизм на который
// осуществляется синхронизация
void irs::pid_reg_sync(pid_data_t *pd, double e, double rp)
{
  if ((pd->k == 0.) || (pd->ki == 0.)) {
    pd->int_val = 0.;
  } else {
    double Pres = pd->k*pd->prev_e;
    double d_pid = pd->k_d_pid*(pd->max - pd->min);
    if (fabs(Pres) < d_pid) {
      pd->int_val = (rp - pd->k*(e/* + pd->kd*(e - pd->prev_e)*/))
        /(pd->k*pd->ki);
    } else {
      pd->int_val = 0.;
    }
  }
  pd->pp_e = e;
  pd->prev_e = e;
  pd->prev_out = rp;
}
// Синхронизация ПИД-регулятора на текущее значение регулируемого параметра
// при включенном регуляторе
// pd - Параметры ПИД-регулятора,
void irs::pid_reg_sync(pid_data_t *pd)
{
  if ((pd->k == 0.) || (pd->ki == 0.)) {
    pd->int_val = 0.;
  } else {
    double PDres = pd->k*(pd->prev_e + pd->kd*(pd->prev_e - pd->pp_e));
    double d_pid = pd->k_d_pid*(pd->max - pd->min);
    if ((PDres > -d_pid) && (PDres < pd->max)) {
      pd->int_val = (pd->prev_out - pd->k*(pd->prev_e +
        pd->kd*(pd->prev_e - pd->pp_e)))
        /(pd->k*pd->ki);
    } else {
      pd->int_val = 0.;
    }
  }
}

// Колебательное звено
double irs::osc(osc_data_t *od, double x)
{
  double k = sqr(1/od->nf) + sqr(2*IRS_PI/od->no);
  double y = (k*(x + 2*od->x1 + od->x2)
    - (2*k - 8)*od->y1
    - (k + 4*(1 - 1/od->nf))*od->y2)/(k + 4*(1 + 1/od->nf));
  od->x2 = od->x1; od->x1 = x;
  od->y2 = od->y1; od->y1 = y;
  return y;
}

// Инерционное звено
double irs::fade(fade_data_t *fd, double x)
{
  double y = (x + fd->x1 - (1 - 2*fd->t)*fd->y1)/(1 + 2*fd->t);
  fd->x1 = x;
  fd->y1 = y;
  return y;
}

// Изодромное звено
// id - структура данных изодромного звена
// x - входное значение
double irs::isodr(isodr_data_t *id, double x)
{
  return id->k*x + (1 - id->k)*fade(&id->fd, x);
}
#endif //NEW
