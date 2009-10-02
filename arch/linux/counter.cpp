// Модуль счетчика
// Linux
// Дата: 01.10.2009
// Ранняя дата: 13.09.2009

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
#include <irscpp.h>
// Заголовок limits включается из-за того что это аппаратно-зависимый код
#include <limits>
#include <stdarg.h>
#include <irsdefs.h>

// Максимальное время которое можно измерить
counter_t COUNTER_MAX = std::numeric_limits<counter_t>::max();
// Число секунд в интервале
counter_t SECONDS_PER_INTERVAL = 1;
// Количество отсчетов в секунде
counter_t COUNTER_PER_INTERVAL = CLOCKS_PER_SEC;

// Разрядность счетчика ARM
const int arm_counter_digits = 20;
// Необходимая величина сдвига для значения счетчика ARM
const int arm_counter_shift =
  std::numeric_limits<counter_t>::digits - arm_counter_digits;

int init_cnt = 0;

#define OUTDBG(out) out
//#define OUTDBG(out)

// Регистр AT91
#define AT91_REG(BASE, OFFSET, REG) \
  (*((unsigned*)((irs_u8*)BASE + ((OFFSET + REG) & MAP_MASK))))

ostream &hex_u32(ostream &s)
{
  s.setf(ios::hex, ios::basefield);
  s << setw(8) << setfill('0');
  return s;
}

void *map_base = IRS_NULL;
irs_bool is_arm = irs_false;

// Инициализация счетчика
void counter_init()
{
  if (init_cnt == 0)
  {
    OUTDBG(cout << "Init counter\n");
    struct utsname buf[1000];
    const int uname_ok = 0;
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
    }
    if (is_arm)
    {
      COUNTER_MAX = 0x7FFFFFFF & (~((1 << 12) - 1));
      SECONDS_PER_INTERVAL = 1;
      COUNTER_PER_INTERVAL = 32768*4096;
      OUTDBG(cout<<"init timer counter 0\n");
      int fd = open("/dev/mem", O_RDWR | O_SYNC);
      if(fd == -1)
      {
        OUTDBG(cout<<"gpio: Error opening /dev/mem\n");
      }
      map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
        fd, AT91_SYS & ~MAP_MASK);
      if(map_base == (void *)(-1))
      {
        OUTDBG(cout<< "gpio: Error mapping memory\n");
        close(fd);
      }
      AT91_REG(map_base, PMC_OFFSET, PMC_PCER) = (1 << TC0_ID);
      OUTDBG(
        unsigned pmc_pcsr = AT91_REG(map_base, PMC_OFFSET, PMC_PCSR);
        cout<<"Peripheral status registr:"<<hex_u32<<pmc_pcsr<<"\n"
      );
    } else {
      SECONDS_PER_INTERVAL = 1;
      COUNTER_PER_INTERVAL = CLOCKS_PER_SEC;
    }
  }
  init_cnt++;
}
// Чтение счетчика
counter_t counter_get()
{
  if (is_arm) {
    unsigned st_crtr = AT91_REG(map_base, ST_OFFSET, ST_CRTR);
    #ifndef NOP
    counter_t counter_ret = 0;
    if (std::numeric_limits<counter_t>::digits > arm_counter_digits) {
      counter_ret = static_cast<counter_t>(st_crtr);
      typedef irs::type_relative_t<counter_t>::unsigned_type
        unsigned_counter_t;
      reinterpret_cast<unsigned_counter_t&>(counter_ret) <<=
        arm_counter_shift;
    } else {
      st_crtr <<= arm_counter_shift;
      counter_ret = reinterpret_cast<counter_t&>(st_crtr);
    }
    return counter_ret;
    #else //NOP
    return st_crtr << arm_counter_shift;
    #endif //NOP
  } else {
    return clock();    //  Отсчитанное количество тактов
  }
}
// Деинициализация счетчика
void counter_deinit()
{
  init_cnt--;
  if(init_cnt == 0)
  {
  }
}
