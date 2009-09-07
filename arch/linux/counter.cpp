// Модуль счетчика
// Дата: 12.02.2009

#include <counter.h>
#include <armmap.h>

#include <irscpp.h>
#include <sys/utsname.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
//#include <iostream.h>
#include <irscpp.h>
#include <stdarg.h>
#include <irsdefs.h>


//#include <time.h>

// Структура в которую считывается счетчик процессора
//typedef long long counter_t;

// Время калибровки в тиках
//const clock_t time_int_calibr = 18;

counter_t COUNTER_MAX = IRS_I32_MAX;

// Число секунд в интервале
counter_t SECONDS_PER_INTERVAL = 1;
// Количество отсчетов в секунде
counter_t COUNTER_PER_INTERVAL = 1000000;

//#define OUTDBG(out) out
#define OUTDBG(out)

// Регистр AT91
#define AT91_REG(BASE, OFFSET, REG) \
  (*((unsigned*)((irs_u8*)BASE + ((OFFSET + REG) & MAP_MASK))))

ostream &hex_u32(ostream &s)
{
  s.setf(ios::hex, ios::basefield);
  s<<setw(8)<<setfill('0');
  return s;
}

//void *map_base, *map_tc;
void *map_base = IRS_NULL;
//unsigned int readval,r0, writeval, data,fd, p_id;
irs_bool is_arm = irs_false;

// Инициализация счетчика
void counter_init()
{
  //char id[] = "armv4l";
  //irs_u8 *s;
  OUTDBG(cout << "Init counter\n");
  struct utsname buf[1000];
  const int uname_ok = 0;
  //int uname_status = uname(buf);
  if (uname(buf) == uname_ok) {
    OUTDBG(cout << "All OK\n");
    OUTDBG(cout << "Now we will init counter OK\n");
    OUTDBG(cout << "System name "<<buf->sysname <<"\n");
    OUTDBG(cout << "Node name   "<<buf->nodename<<"\n");
    OUTDBG(cout << "Release     "<<buf->release <<"\n");
    OUTDBG(cout << "Version     "<<buf->version <<"\n");
    OUTDBG(cout << "Machine     "<<buf->machine <<"\n");
    char* machine = strstr(buf->machine, "armv4l");
    if (machine != IRS_NULL)
    {
      OUTDBG(cout << "It is ARM, we can work!!!\n");
      is_arm = irs_true;
    }
  } else {
    OUTDBG(cout << "ERROR\n");
    //perror("uname");
  }
  if (is_arm)
  {
    COUNTER_MAX = 0x7FFFFFFF & (~((1 << 12) - 1));
    //IRS_HIWORD(COUNTER_MAX) = IRS_I16_MAX;
    SECONDS_PER_INTERVAL = 1;
    COUNTER_PER_INTERVAL = 32768*4096;
    OUTDBG(cout<<"init timer counter 0\n");
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd == -1)
    {
      OUTDBG(cout<<"gpio: Error opening /dev/mem\n");
      //return -1;
    }
    map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
      fd, AT91_SYS & ~MAP_MASK);
    //map_tc = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
      //fd, AT91_TC0 & ~MAP_16_MASK);
    if(map_base == (void *)(-1))
    {
      OUTDBG(cout<< "gpio: Error mapping memory\n");
      close(fd);
      //return -1;
    }
    //p_id = TC0_ID;
    //writeval = 1 << p_id;
    //unsigned& pmc_pcer = *((unsigned*)((irs_u8*)map_base +
      //((PMC_OFFSET + PMC_PCER) & MAP_MASK)));
    AT91_REG(map_base, PMC_OFFSET, PMC_PCER) = (1 << TC0_ID);
    //unsigned pmc_pcsr = *((unsigned*)((irs_u8*)map_base +
      //((PMC_OFFSET + PMC_PCSR) & MAP_MASK)));
    OUTDBG(
      unsigned pmc_pcsr = AT91_REG(map_base, PMC_OFFSET, PMC_PCSR);
      cout<<"Peripheral status registr:"<<hex_u32<<pmc_pcsr<<"\n"
    );
  } else {
    SECONDS_PER_INTERVAL = 1;
    COUNTER_PER_INTERVAL = CLOCKS_PER_SEC;
  }
}
// Чтение счетчика
counter_t counter_get()
{
  // irs_u16 cnt = 0;
  // counter_t cnt32 = 0;
  //считывание cnt
  if(is_arm)
  {
    //r0 = *((unsigned int*)((irs_u8*)map_base +
      //((ST_OFFSET + ST_CRTR) & MAP_16_MASK)));
    unsigned st_crtr = AT91_REG(map_base, ST_OFFSET, ST_CRTR);
    //OUTDBG(cout << r0<<"\n");
    //cnt = r0<<12;
    //IRS_HIWORD(cnt32) = cnt;
    return st_crtr << 12;
  }
  else
    return clock();    //  Отсчитанное количество тактов
}
// Деинициализация счетчика
void counter_deinit()
{
}
