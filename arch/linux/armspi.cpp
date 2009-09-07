// SPI для ARM Linux Олега Коновалова
// Дата: 12.02.2009

#include <armspi.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <memory.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <armmap.h>
//#include <at91_spimode.h>

#define OUTDBG(out) (out)
//#define OUTDBG(out)

ostream &hex_u322(ostream &s)
{
  s.setf(ios::showbase);
  s.setf(ios::hex , ios::basefield);
  s<<setw(8)<<setfill('0');
  return s;
}
irs::armspi_t::~armspi_t()
{
}
irs::armspi_t::armspi_t(irs_uarc MCK_div,irs_u8 SPI_mode,irs_uarc width)
{
  // irs_uarc stat     = 0;
  m_spimode         = spi_wait;
  m_is_read         = irs_false;
  m_is_write        = irs_false;
  m_msbset          = irs_false;
  memset(m_wbuf,0,sizeof(m_wbuf));
  memset(m_rbuf,0,sizeof(m_wbuf));
  m_device          = 255;
  m_line            = 255;
  m_rbufsize        = 0;
  m_wbufsize        = 0;
  m_spi_fd          = 65535;
  inspi.Bits_per_trans = width;      //Must be in range 0..8 0 - 8 bits, 8 - 16 bits;
  inspi.SPI_BRate = MCK_div;         //Must be in range 2..255

  switch(SPI_mode)
  {
    case 0:
    {
      inspi.CPOL = 0;
      inspi.NCPHA = 1;
    }
    break;
    case 1:
    {
      inspi.CPOL = 0;
      inspi.NCPHA = 0;
    }
    break;
    case 2:
    {
      inspi.CPOL = 1;
      inspi.NCPHA = 1;
    }
    break;
    case 3:
    {
      inspi.CPOL = 1;
      inspi.NCPHA = 0;
    }
    break;
  }
   /* select_of_cs(2);
    m_line = 2;
    spiPtr = &inspi;
    stat = ::ioctl(m_spi_fd,2,(void *)spiPtr);
    if(stat == -1)
      perror("ioctl");*/
  OUTDBG(cout<<"*****************STATUS*************\n");
  OUTDBG(cout<<"CPOL    : "<<(int)inspi.CPOL<<"\n");
  OUTDBG(cout<<"NCPHA   : "<<(int)inspi.NCPHA<<"\n");
  OUTDBG(cout<<"BPT     : "<<(int)inspi.Bits_per_trans<<"\n");
  OUTDBG(cout<<"BRate   : "<<(int)inspi.SPI_BRate<<"\n");
  OUTDBG(cout<<"DBSPCK  : "<<(int)inspi.SPI_DBSPCK<<"\n");
  OUTDBG(cout<<"DBTrans : "<<(int)inspi.SPI_DBTransfers<<"\n");
  m_cpol    = (irs_u8)(inspi.CPOL);
  m_ncpha   = (irs_u8)(inspi.NCPHA);
  m_bpt     = (irs_u8)(inspi.Bits_per_trans);
  m_brate   = (irs_u8)(inspi.SPI_BRate);
  m_dbspck  = (irs_u8)(inspi.SPI_DBSPCK);
  m_dbtrans = (irs_u8)(inspi.SPI_DBTransfers);

}

irs::spi_t::status_t irs::armspi_t::get_status()
{
  if((m_is_read)||(m_is_write))
    return BUSY;
  else
    return FREE;

}
void irs::armspi_t::abort()
{
  m_is_read = irs_false;
  m_is_write = irs_false;
  m_spimode = spi_wait;
}

irs_uarc irs::armspi_t::fast_read(irs_u8 *ap_buf,irs_uarc a_size)
{
  m_is_read = irs_true;
  int is_read = ::read(m_spi_fd, ((char *)ap_buf), a_size);
  if(is_read == -1)
  {
    perror("read");
  }
  else
  {
    for(int i = 0; i < is_read; i++)
      OUTDBG(cout << "After read buffer contein: "<<(int)ap_buf[i]<<"\n");
  }
  m_is_read = irs_false;
  return is_read;
}

irs_uarc irs::armspi_t::fast_write(irs_u8 *ap_buf,irs_uarc a_size)
{
  m_is_write = irs_true;
  //m_wbuf = ap_buf;
  int is_write = ::write(m_spi_fd, ((char *)m_wbuf), a_size);
  if(is_write == -1)
  {
    perror("read");
  }
  else
  {
    for(int i=0; i < is_write; i++)
      OUTDBG(cout << "After write buffer contein: "<<(int)m_wbuf[i]<<"\n");
  }
  m_is_write = irs_false;
  return is_write;
}
void irs::armspi_t::read(irs_u8 *ap_buf,irs_uarc a_size)
{
  m_is_read = irs_true;
  *m_rbuf = *ap_buf;
  m_rbufsize = a_size;
  m_spimode = spi_read;
}

void irs::armspi_t::write(const irs_u8 *ap_buf,irs_uarc a_size)
{
  m_is_write = irs_true;
  //m_wbuf = ap_buf;
  //for(int i ; i < a_size;i++)
   //m_wbuf[i] = ap_buf[i];
  if(a_size < SIZE_OF_BUFFER)
  {
    memcpy(m_wbuf,ap_buf,a_size);
    //strcpy((char *)m_wbuf,(char *)ap_buf);
    OUTDBG(cout << "\nBuffer contein after copy: ");
    for(irs_uarc i = 0; i < a_size; i++)
      OUTDBG(cout << (int)m_wbuf[i]<<" ");
    OUTDBG(cout <<"\n ");
    m_wbufsize = a_size;
    m_spimode = spi_write;
  }
  else
    OUTDBG(cout << "Buffer is too big\n");
}

void irs::armspi_t::set_cs(irs_uarc line_num)
{
  if((line_num >= 0)&&(line_num < 4 ))
  {
    if(get_status() == FREE)
    {
      if(m_spi_fd != 65535)
        close(m_spi_fd);
      select_of_cs(line_num);
      m_line = line_num;
      //m_spimode = spi_cs;
    }
  }
  else
  {
    OUTDBG(cout << "We haven't such line in this device!\n");
    OUTDBG(cout << "Line number: "<<line_num<<"\n");
  }
}
void irs::armspi_t::reset_cs()
{
  if(m_spi_fd != 65535)
    close(m_spi_fd);
  m_spi_fd = 65535;
}
irs_bool irs::armspi_t::set_bpt(irs_u8 a_bpt)
{
  if(get_status() == FREE)
  {
    //int stat = 0;
    inspi.Bits_per_trans = (unsigned char)a_bpt;
    spiPtr = &inspi;

    int stat = ::ioctl(m_spi_fd,sizeof(spiPtr),(void *)spiPtr);
    if(stat == -1)
    {
      perror("ioctl");
      OUTDBG(cout <<"Can't call ioctl : BITRATE\n");
    }
    else
    {
      OUTDBG(cout << "All Okey!!!\n");
    }
  }
  return irs_false;
}
bool irs::armspi_t::set_bitrate(irs_u32 a_bitrate)
{
  //inspi.CPOL           = (unsigned char)m_cpol;
  //inspi.NCPHA          = (unsigned char)m_ncpha;
  //inspi.Bits_per_trans = (unsigned char)m_bpt;
  //inspi.SPI_BRate      = (unsigned char)m_brate;
  //inspi.SPI_DBSPCK     = (unsigned char)m_dbspck;
  //inspi.SPI_DBTransfers= (unsigned char)m_dbtrans;
  if(get_status() == FREE)
  {
    inspi.SPI_BRate = 255;
    spiPtr = &inspi;
    int stat = ::ioctl(m_spi_fd,sizeof(spiPtr),(void *)spiPtr);
    if(stat == -1)
    {
      perror("ioctl");
      OUTDBG(cout <<"Can't call ioctl : BITRATE\n");
    }
    else
    {
      OUTDBG(cout << "All Okey!!!\n");
    }
    OUTDBG(cout << "We ready to set bitrate to ne value\n");
    inspi.SPI_BRate = (unsigned char)(44928000/(2*a_bitrate));
    OUTDBG(cout << "We have calculate new bitrate "<< (int)inspi.SPI_BRate<<"\n");
    spiPtr = &inspi;
    stat = ::ioctl(m_spi_fd,sizeof(spiPtr),(void *)spiPtr);
    if(stat == -1)
    {
      perror("ioctl");
      OUTDBG(cout <<"Can't call ioctl : BITRATE\n");
      return irs_false;
    }
    else
    {
      return irs_true;
      OUTDBG(cout << "All Okey!!!\n");
    }
  }
  else
    return irs_false;
}
bool irs::armspi_t::set_polarity(spi_t::polarity_t a_polarity)
{
  //inspi.CPOL           = (unsigned char)m_cpol;
  //inspi.NCPHA          = (unsigned char)m_ncpha;
  //inspi.Bits_per_trans = (unsigned char)m_bpt;
  //inspi.SPI_BRate      = (unsigned char)m_brate;
  //inspi.SPI_DBSPCK     = (unsigned char)m_dbspck;
  //inspi.SPI_DBTransfers= (unsigned char)m_dbtrans;
  if(get_status() == FREE)
  {
    OUTDBG(cout<<"System free\n");
    if(a_polarity == RISING_EDGE)
    {

      inspi.CPOL = 1;
      spiPtr = &inspi;
      int stat = ::ioctl(m_spi_fd,sizeof(spiPtr),(void *)spiPtr);
      if(stat == -1)
      {
        perror("ioctl");
        OUTDBG(cout <<"Can't call ioctl : POSITIVE\n");
        return irs_false;
      }
      else
      {
        OUTDBG(cout << "All Okey!!!\n");
        return irs_true;
      }
    }

    if(a_polarity == FALLING_EDGE)
    {
      // irs_uarc stat = 0;
      inspi.CPOL = 0;
      spiPtr = &inspi;
      int stat = ::ioctl(m_spi_fd,sizeof(spiPtr),(void *)spiPtr);
      if(stat == -1)
      {
        perror("ioctl");
        OUTDBG(cout <<"Can't call ioctl: ZERO\n");
        return irs_false;
      }
      else
      {
        OUTDBG(cout << "All Okey!!!\n");
        return irs_true;
      }
    }
  }
  else
  {
    OUTDBG(cout << "Please wait...I'am busy now\n");
    return irs_false;
  }
  return irs_false;
}
bool irs::armspi_t::set_phase(spi_t::phase_t a_phase)
{
  //inspi.CPOL           = (unsigned char)m_cpol;
  //inspi.NCPHA          = (unsigned char)m_ncpha;
  //inspi.Bits_per_trans = (unsigned char)m_bpt;
  //inspi.SPI_BRate      = (unsigned char)m_brate;
  //inspi.SPI_DBSPCK     = (unsigned char)m_dbspck;
  //inspi.SPI_DBTransfers= (unsigned char)m_dbtrans;
  if(get_status() == FREE)
  {
    if(a_phase == LEAD_EDGE)
    {
      // irs_uarc stat = 0;
      inspi.NCPHA = 1;
      spiPtr = &inspi;
      int stat = ::ioctl(m_spi_fd,sizeof(spiPtr),(void *)spiPtr);
      if(stat == -1)
      {
        perror("ioctl");
        return irs_false;
      }
      else
        return irs_true;
    }

    if(a_phase == TRAIL_EDGE)
    {
      // irs_uarc stat = 0;
      inspi.NCPHA = 0;
      spiPtr = &inspi;
      int stat = ::ioctl(m_spi_fd,sizeof(spiPtr),(void *)spiPtr);
      if(stat == -1)
      {
        perror("ioctl");
        return irs_false;
      }
      else
        return irs_true;
    }
  }
  else
    return irs_false;
  return irs_false;
}
bool irs::armspi_t::set_order(spi_t::order_t a_order)
{
  if(get_status() == FREE)
  {
    if(a_order == LSB)
      return irs_true;
    if(a_order == MSB)
    {
      m_msbset = irs_true;
      return irs_true;
    }
  }
  else
    return irs_false;
  return irs_false;
}
void irs::armspi_t::status()
{
  //spipar.cpol       = (unsigned char)m_cpol;
  //spipar.ncpha      = (unsigned char)m_ncpha;
  //spipar.bpt        = (unsigned char)m_bpt;
  //spipar.brate      = (unsigned char)m_brate;
  //spipar.dbspck     = (unsigned char)m_dbspck;
  //spipar.dbtrans    = (unsigned char)m_dbtrans;
  int fd = 0;
  void *map_base;
  if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1)
  {
    OUTDBG(cout<<"gpio: Error opening /dev/mem\n");
  }
  map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, AT91_SPI & ~MAP_16_MASK);
  if(map_base == (void *) -1)
  {
    OUTDBG(cout<<"gpio: Error mapping memory\n");
    close(fd);
  }
  irs_u32 CSR = 0 ,MR = 0;
  CSR = *((unsigned int*)((irs_u8*)map_base + ((0 + SPI_CSR2) & MAP_16_MASK)));
  MR = *((unsigned int*)((irs_u8*)map_base + ((0 + SPI_MR) & MAP_16_MASK)));

  OUTDBG(cout<<"*****************STATUS*************\n");
  OUTDBG(cout<<"SPI Chip select registr: "<<hex_u322<< CSR<<"\n");
  OUTDBG(cout<<"SPI Mode registr       : "<<hex_u322<< MR<<"\n");
  OUTDBG(cout<<"CPOL    : "<<(int)inspi.CPOL<<"\n");
  OUTDBG(cout<<"NCPHA   : "<<(int)inspi.NCPHA<<"\n");
  OUTDBG(cout<<"BPT     : "<<(int)inspi.Bits_per_trans<<"\n");
  OUTDBG(cout<<"BRate   : "<<(int)inspi.SPI_BRate<<"\n");
  OUTDBG(cout<<"DBSPCK  : "<<(int)inspi.SPI_DBSPCK<<"\n");
  OUTDBG(cout<<"DBTrans : "<<(int)inspi.SPI_DBTransfers<<"\n");

  //OUTDBG(cout<<"*****************STATUS*************\n");
  //OUTDBG(cout<<"CPOL    : "<<(int)m_cpol<<"\n");
  //OUTDBG(cout<<"NCPHA   : "<<(int)m_ncpha<<"\n");
  //OUTDBG(cout<<"BPT     : "<<(int)m_bpt<<"\n");
  //OUTDBG(cout<<"BRate   : "<<(int)m_brate<<"\n");
  //OUTDBG(cout<<"DBSPCK  : "<<(int)m_dbspck<<"\n");
  //OUTDBG(cout<<"DBTrans : "<<(int)m_dbtrans<<"\n");
  if(munmap(map_base, MAP_SIZE) == -1)
  {
    OUTDBG(cout<<"Error unmapping memory\n");
    //close(fd);
  }
  close(fd);

}
void irs::armspi_t::reverse()
{
  irs_u8 var = 0;
  for(irs_uarc i = 0; i < (m_wbufsize);i+=2)
  {
    var = m_wbuf[i+1];
    m_wbuf[i+1] = m_wbuf[i];
    m_wbuf[i] = var;
  }
}
irs_bool irs::armspi_t::select_of_cs(irs_uarc cs)
{
  switch(cs)
  {
    case 0:
    {
      m_device = 0;
      m_spi_fd = open("/dev/spi/0",O_RDWR);
      if(m_spi_fd == -1)
      {
        perror("open");
        return irs_false;
      }
      else
        return irs_true;
    }
    break;
    case 1:
    {
      m_device = 1;
      m_spi_fd = open("/dev/spi/1",O_RDWR);
      if(m_spi_fd == -1)
      {
        perror("open");
        return irs_false;
      }
      else
        return irs_true;
    }
    break;
    case 2:
    {
      m_device = 2;
      m_spi_fd = open("/dev/spi/2",O_RDWR);
      if(m_spi_fd == -1)
      {
        perror("open");
        return irs_false;
      }
      else
        return irs_true;
    }
    break;
    case 3:
    {
      m_device = 3;
      m_spi_fd = open("/dev/spi/3",O_RDWR);
      if(m_spi_fd == -1)
      {
        perror("open");
        return irs_false;
      }
      else
        return irs_true;
    }
    break;
  }
  return irs_false;
}

void irs::armspi_t::set_cpuclock(char var)
{
    // irs_uarc stat = 0;
    inspi.SPI_DBTransfers = var;
    spiPtr = &inspi;
    int stat = ::ioctl(m_spi_fd,sizeof(spiPtr),(void *)spiPtr);
    if(stat == -1)
    {
      perror("ioctl");
      OUTDBG(cout <<"Can't call ioctl : Cpu clock\n");
      //return irs_false;
    }
    else
    {
      OUTDBG(cout << "All Okey!!!\n");
      //return irs_true;
    }

}
void irs::armspi_t::tick()
{
  switch(m_spimode)
  {
    case spi_write:
    {
      m_is_write = irs_true;
      if(m_spi_fd != 65535)
      {
        if(m_msbset == irs_true)
          reverse();
        // irs_uarc is_write = 0;
        OUTDBG(cout << "FD    - "<<m_spi_fd<<"\n");
        OUTDBG(cout << "SIZE  - "<<(int)m_wbufsize<<"\n");
        OUTDBG(cout << "Buf   - "<<(int)m_wbuf[0]<<"\n");
        int is_write = ::write(m_spi_fd, ((char *)m_wbuf), m_wbufsize);
        OUTDBG(cout << "WRITE  - "<<(int)errno<<"\n");
        OUTDBG(cout << "WRITE  - "<<(int)is_write<<"\n");
        if(is_write == -1)
        {
          perror("write");
        }
        else
        {
          for(int i=0; i < is_write; i++)
            OUTDBG(cout << "After write buffer contein: "<<
              (int)m_wbuf[i]<<"\n");
        }
        m_is_read = irs_false;
      }
      else
        OUTDBG(cout << "Cann't write!!!\nWe have no open devices!\n");
      m_spimode = spi_wait;
    }
    break;
    case spi_read:
    {
      m_is_read = irs_true;
      if(m_spi_fd != 65535)
      {
        // irs_uarc is_read = 0;
        OUTDBG(cout << "FD    - "<<m_spi_fd<<"\n");
        int is_read = ::read(m_spi_fd, ((char *)m_rbuf), m_rbufsize);
        OUTDBG(cout << "read  - "<<(int)is_read<<"\n");
        if(is_read == -1)
        {
          perror("read");
        }
        else
        {
          for(int i=0; i < is_read; i++)
            OUTDBG(cout << "After read buffer contein: "<<(int)m_rbuf[i]<<"\n");
        }

        m_is_read = irs_false;
      }
      else
        OUTDBG(cout << "Cann't read!!!\nWe have no open devices!\n");
      m_spimode = spi_wait;
    }
    break;
    case spi_cs:
    {
      if(m_line != m_device)
      {
        if(m_spi_fd != 65535)
          close(m_spi_fd);
        //irs_bool result = irs_false;
        irs_bool result = select_of_cs(m_line);
        if(result)
          OUTDBG(cout << "We open device \n");
        else
          OUTDBG(cout << "Fail set chip select!\n");
      }
      else
        OUTDBG(cout << "This device is already open!\n");
    }
    break;
    case spi_wait:
    {
    }
    break;
  }
}
