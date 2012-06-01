#ifndef IRSINIT_H
#define IRSINIT_H

#include <irsdefs.h>

#include <mxdata.h>
#include <irsint.h>

#include <irsfinal.h>

//! \addtogroup configuration_group
//! @{

namespace irs {

class init_t
{
public:
  virtual void dummy() = 0;
};

init_t& init();

} //namespace irs

//! @}

#endif // IRSINIT_H
