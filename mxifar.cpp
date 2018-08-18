//! \file
//! \ingroup network_in_out_group
//! \brief Функции для mxifa (mxifa routines)
//!
//! Дата: 01.04.2010\n
//! Ранняя дата: 17.09.2009

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <mxifar.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <irscpp.h>

#include <irsfinal.h>
//---------------------------------------------------------------------------
// Нулевой IP
const mxip_t zero_ip = {{{0, 0, 0, 0}}};
//---------------------------------------------------------------------------

