//! \file
//! \ingroup graphical_user_interface_group
//! \brief Тест сети 4 - абстрактный базовый класс

//! Дата: 14.04.2010\n
// Дата создания: 14.05.2008

#ifndef tstlan4absH
#define tstlan4absH

#include <irsdefs.h>

#include <irsstd.h>

#include <irsfinal.h>

namespace irs {

//! \addtogroup graphical_user_interface_group
//! @{

class tstlan4_base_t
{
public:
  virtual ~tstlan4_base_t() {}
  virtual void tick() = 0;
  virtual void show() = 0;
  virtual void hide() = 0;
  virtual rect_t position() const = 0;
  virtual void set_position(const rect_t &a_position) = 0;
  virtual void connect(mxdata_t *ap_data) = 0;
  virtual void save_conf() = 0;
  virtual void update_time(const irs_i32 a_update_time) = 0;
  virtual void resize_chart(const irs_u32 a_size) = 0;
  virtual void options_event_connect(event_t* ap_event) = 0;
  virtual void options_event_clear() = 0;
};

//! @}

} //namespace irs

#endif //tstlan4absH

