// Дискретные ввод/вывод для ARM Linux Олега Коновалова
// Дата: 12.02.2009

#include <armio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
//#include <iostream.h>
#include <stdarg.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <timer.h>
#include <irscpp.h>
#include <irsdefs.h>
#include <armmap.h>

#define SIZE_OF_BANKS 16

irs::arm::io_t::io_t(irs_bool bank_A,irs_bool bank_B,irs_bool bank_C,irs_bool bank_D)
{
  if((m_fdio = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
  {
    cout << "gpio: Error opening /dev/mem\n";
  }
  /* Map one page */
  io_map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, m_fdio, AT91_SYS & ~MAP_MASK);
  if(io_map_base == (void *) -1)
  {
    close(m_fdio);
  }
    if(bank_A == irs_true)
    {
      idio = PIOA_ID;
      wreval = 1 << idio;
      *((unsigned int*)((irs_u8*)io_map_base + ((PMC_OFFSET + PMC_PCER) & MAP_MASK))) = wreval;
    }
    if(bank_B == irs_true)
    {
      idio = PIOB_ID;
      wreval = 1 << idio;
      *((unsigned int*)((irs_u8*)io_map_base + ((PMC_OFFSET + PMC_PCER) & MAP_MASK))) = wreval;
    }
    if(bank_C == irs_true)
    {
      idio = PIOC_ID;
      wreval = 1 << idio;
      *((unsigned int*)((irs_u8*)io_map_base + ((PMC_OFFSET + PMC_PCER) & MAP_MASK))) = wreval;
      //cout << "It is bank C\n";
    }
    if(bank_D == irs_true)
    {
      idio = PIOD_ID;
      wreval = 1 << idio;
      *((unsigned int*)((irs_u8*)io_map_base + ((PMC_OFFSET + PMC_PCER) & MAP_MASK))) = wreval;
    }

}

irs_uarc irs::arm::io_t::io_tuning(io_bank a_bank,irs_u8 line,irs_bool PER,irs_bool OER,irs_bool IFER,
  irs_bool SODR,irs_bool IER,irs_bool MDER,irs_bool PUER,irs_bool ASR)
{
  return 0;
}

void irs::arm::io_t::set_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  //cout << "a_bank     " << (int)a_bank<<"\n";
  //cout << "line       " << (int)line<<"\n";
  switch(a_index)
  {
    case IOA_ID:
    {
      *((unsigned int*)((irs_u8*)io_map_base + ((PIOA_OFFSET + PIO_SODR) & MAP_MASK))) = (1<<((int)a_bit_index));
    }
    break;
    case IOB_ID:
    {
      *((unsigned int*)((irs_u8*)io_map_base + ((PIOB_OFFSET + PIO_SODR) & MAP_MASK))) = (1<<((int)a_bit_index));
    }
    break;
    case IOC_ID:
    {
      //*((unsigned int*)((irs_u8*)io_map_base + ((PIOC_OFFSET + PIO_PER) & MAP_MASK))) = (1<<((int)line));
      *((unsigned int*)((irs_u8*)io_map_base + ((PIOC_OFFSET + PIO_ODR) & MAP_MASK))) = (1<<((int)a_bit_index));
      *((unsigned int*)((irs_u8*)io_map_base + ((PIOC_OFFSET + PIO_SODR) & MAP_MASK))) = (1<<((int)a_bit_index));
      //cout << "It is bank C\n";
    }
    break;
    case IOD_ID:
    {
      *((unsigned int*)((irs_u8*)io_map_base + ((PIOD_OFFSET + PIO_SODR) & MAP_MASK))) = (1<<((int)a_bit_index));
    }
    break;

  }

}

void irs::arm::io_t::clear_bit(irs_uarc a_index, irs_uarc a_bit_index)
{
  //cout << "a_bank    " << (int)a_bank<<"\n";
  //cout << "line      " << (int)line<<"\n";
  switch(a_index)
  {
    case IOA_ID:
    {
      *((unsigned int*)((irs_u8*)io_map_base + ((PIOA_OFFSET + PIO_CODR) & MAP_MASK))) = (1<<((int)a_bit_index));
    }
    break;
    case IOB_ID:
    {
      *((unsigned int*)((irs_u8*)io_map_base + ((PIOB_OFFSET + PIO_CODR) & MAP_MASK))) = (1<<((int)a_bit_index));
    }
    break;
    case IOC_ID:
    {
      *((unsigned int*)((irs_u8*)io_map_base + ((PIOC_OFFSET + PIO_PER) & MAP_MASK))) = (1<<((int)a_bit_index));
      *((unsigned int*)((irs_u8*)io_map_base + ((PIOC_OFFSET + PIO_OER) & MAP_MASK))) = (1<<((int)a_bit_index));
      *((unsigned int*)((irs_u8*)io_map_base + ((PIOC_OFFSET + PIO_CODR) & MAP_MASK))) = (1<<((int)a_bit_index));
      //cout << "It is bank C\n";
    }
    break;
    case IOD_ID:
    {
      *((unsigned int*)((irs_u8*)io_map_base + ((PIOD_OFFSET + PIO_CODR) & MAP_MASK))) = (1<<((int)a_bit_index));
    }
    break;

  }
}
irs_uarc irs::arm::io_t::size()
{
  return SIZE_OF_BANKS;
}
irs_bool irs::arm::io_t::connected()
{
  return irs_true;
}
void irs::arm::io_t::read(irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size)
{
}
void irs::arm::io_t::write(const irs_u8 *ap_buf, irs_uarc a_index, irs_uarc a_size)
{
}

irs_bool irs::arm::io_t::bit(irs_uarc a_index, irs_uarc a_bit_index)
{
    return irs_false;
}
void irs::arm::io_t::tick()
{
}
