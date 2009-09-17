// Абстакция интерфейса для каналов обмена (интерфейсов)
// Max Interface Abstraction
// Дата: 16.09.2009

#include <mxifa.h>
#include <unistd.h>
#include <fcntl.h>
#include <irsdefs.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
//#include <UDP_stack.h>
//#include <assert.h>
#include <timer.h>
#include <string.h>
//#include <list>
#include <irscpp.h>
#include <gdefs.h>
#ifdef DBGMODE
//#include <iom128.h>
#endif //DBGMODE

using namespace std;
//#define OUTDBG(out) out
#define OUTDBG(out)

// Таймаут открытия канала
#define TIMEOUT_CHANNEL TIME_TO_CNT(1, 1)
#define addrmass_size 10
// Специальная пустая функция
static void mxifa_nulf(void *pchdata);
// Выполнение операции чтения для avr128_ether
//static void avr128_ether_read(void *pchdata);
// Выполнение операции записи для avr128_ether
//static void avr128_ether_write(void *pchdata);
// Элементарная функция канала
typedef void (*mxifa_tick_func_t)(void *);
// Тип текущего режима записи
typedef enum _mxifa_write_mode_t {
  mxifa_wm_free,
  mxifa_wm_write
} mxifa_write_mode_t;
// Тип текущего режима записи
typedef enum _mxifa_read_mode_t {
  mxifa_rm_free,
  mxifa_rm_read
} mxifa_read_mode_t;
//
typedef struct _mxifa_linux_host_data_t{
    //file descriptor
    int host_fd;
    //destanation addres of host
    struct sockaddr_in address;
    //flag, that show close or open socket
    irs_bool opened;

}mxifa_linux_host_data_t;

class Hdata{

  private:
    int host_fd;
    struct sockaddr_in haddr;
  public:
    Hdata();
    void set_host_fd(int);
    void set_host_addr(struct sockaddr_in);
    void printData();
    int get_hfd();
    unsigned long get_haddr();
    unsigned short get_hport();
};
Hdata::Hdata()
{
  host_fd = -1;

}

void Hdata::set_host_fd(int fd)
{
  host_fd = fd;
}
void Hdata::set_host_addr(struct sockaddr_in address)
{
  haddr = address;
}
int Hdata::get_hfd()
{
  return host_fd;
}
unsigned long Hdata::get_haddr()
{
  return haddr.sin_addr.s_addr;
}
unsigned short Hdata::get_hport()
{
  return haddr.sin_port;
}

// Структура данных канала типа mxifa_ei_linux_tcpip
typedef struct _mxifa_linux_tcpip_t {
  // Удаленный IP
  mxip_t dest_ip;
  mxip_t null_ip;
  // Включение широковещания
  irs_bool is_broadcast;
  //
  irs_bool can_read;
  irs_bool can_write;
  irs_bool can_rany;
  irs_bool is_empty;
  irs_bool sock_finded;
  // File descriptor
  int socket_fd;
  fd_set master;
  fd_set read_fds;
  fd_set write_fds;
  int maxfd;
  int newfd;
  int cur_rfd;
  //
  int cur_ac_sock;
  int cur_rw_sock;
//  socklen_t addrlen;
  // Локальный IP
//  mxip_t local_ip;
  //
  struct sockaddr_in local_addr;
  struct sockaddr_in dest_addr;
  struct sockaddr_in acc_addr;
  struct sockaddr_in read_addr;
  struct sockaddr_in null_addr;
  mxifa_dest_t *read_address;
  // Локальный порт

  irs_u16 local_port;
  irs_u16 null_port;
  //
  irs_u16 dest_port;
  //
  irs_u32 buf_lenth;

  // Буфер чтения
  irs_u8 *rd_buf,*rd_buf_bu;
  // Размер буфера чтения
  mxifa_sz_t rd_size, rd_size_bu;
  // Буфер записи
  irs_u8 *wr_buf;
  // Размер буфера записи
  mxifa_sz_t wr_size;
  struct timeval tv_zero;
  mxifa_linux_host_data_t addr_mass[addrmass_size];
  list<Hdata> HD;
  list<Hdata>::iterator it;
  list<Hdata>::iterator r_it;
  list<Hdata>::iterator cur_it;
  mxip_t dest_ip_def;
  irs_u16 dest_port_def;
  mxip_t dest_ip_cur;
  irs_u16 dest_port_cur;
  irs_bool first_read;
  int wr_socket;

  #ifdef NOP

  // Локальная маска
  mxip_t local_mask;
  // Удаленный порт
  irs_u16 dest_port;
  // Текущий режим записи
  mxifa_write_mode_t write_mode;
  // Описание удаленной системы для записи
  mxifa_dest_t *wr_dest;

  // Текущий режим записи
  mxifa_read_mode_t read_mode;
  // Описание удаленной системы для чтения
  mxifa_dest_t *rd_dest;

  // Начало буфера чтения UDP_stack
  irs_u16 buf_begin;
  // Размер буфера чтения UDP_stack
  irs_u16 buf_size;
  #endif //NOP
} mxifa_linux_tcpip_t;

typedef struct _mxifa_linux_tcpip_cl_t {
  // Удаленный IP
  mxip_t dest_ip;
  // Включение широковещания
  irs_bool is_broadcast;
  irs_bool can_read;
  irs_bool can_write;
  // File descriptor
  int socket_fd;
  fd_set master;
  fd_set read_fds;
  fd_set write_fds;
  struct sockaddr_in local_addr;
  struct sockaddr_in dest_addr;
  // Локальный порт
  irs_u16 local_port;
  //
  irs_u16 dest_port;
  //
  irs_u32 buf_lenth;
  // Буфер чтения
  irs_u8 *rd_buf;
  // Размер буфера чтения
  mxifa_sz_t rd_size;
  // Буфер записи
  irs_u8 *wr_buf;
  // Размер буфера записи
  mxifa_sz_t wr_size;
  struct timeval tv_zero;
  //irs_bool;
  //irs_bool write_end;
  irs_bool sock_open;
  mxip_t dest_ip_def;
  irs_u16 dest_port_def;
  mxip_t dest_ip_cur;
  irs_u16 dest_port_cur;

} mxifa_linux_tcpip_cl_t;

// Обобщенная структура данных канала
typedef struct _mxifa_chdata_t {
  // Тип интерфейса
  mxifa_ei_t enum_iface;
  // Фукции обработки
  mxifa_tick_func_t tick;
  // Истина, если устройство не занято операциями
  irs_bool mode_free;
  // Канал открыт
  irs_bool opened;
  // Специфические данные канала
  void *ch_spec;
} mxifa_chdata_t;



// Порт протокола mxnet
#define MXNET_PORT 5002
#define NULL_PORT  0
// Ethernet (MAC) адрес
//irs_u8 local_mac[MXIFA_MAC_SIZE] = {0x23, 0x01, 0xF1, 0x5C, 0xAF, 0x11};
// Локальный IP
//char number_of_cl = 0;
//mxip_t local_ip = {{192, 168, 0, 38}};
// Широковещательный IP
const mxip_t broadcast_ip = {{255, 255, 255, 255}};
const mxip_t se_dest_ip = {{127,0,0,1}};
const mxip_t NULL_IP = {{0,0,0,0}};
const irs_u16 se_dest_port = 5003;
const mxip_t cl_dest_ip ={{127,0,0,1}};
const irs_u16 cl_dest_port = 5002;
// Удаленный IP для канала 5
//#define dest_ip_5  "192.168.0.16"
//#define dest_ip_5  "127.0.0.1"
// Маска подсети (не используется)
//mxip_t local_mask = {{255, 255, 255, 0}};

// Данные канала 5
mxifa_linux_tcpip_t mxifa_linux_tcpip_5;
// Данные канала 6
mxifa_linux_tcpip_cl_t mxifa_linux_tcpip_6;
// Массив данных всех каналов
mxifa_chdata_t mxifa_chdata[] = {
  // Зарезервированный нулевой порт
  {
    mxifa_ei_unknown,
    mxifa_nulf,
    true,
    false,
    IRS_NULL
  },
  {
    mxifa_ei_unknown,
    mxifa_nulf,
    true,
    false,
    IRS_NULL
  },
  {
    mxifa_ei_unknown,
    mxifa_nulf,
    true,
    false,
    IRS_NULL
  },
  {
    mxifa_ei_unknown,
    mxifa_nulf,
    true,
    false,
    IRS_NULL
  },
  {
    mxifa_ei_unknown,
    mxifa_nulf,
    true,
    false,
    IRS_NULL
  },
  // Chanel 5 на linux
  {
    mxifa_ei_linux_tcpip, // Тип интерфейса
    mxifa_nulf,             // Фукции обработки
    true,                   // Истина, если устройство не занято операциями
    false,                  // Канал открыт
    (void *)&mxifa_linux_tcpip_5 // Специфические данные канала
  },
  // Chanel 6 на linux
  {
    mxifa_ei_linux_tcpip_cl, // Тип интерфейса
    mxifa_nulf,             // Фукции обработки
    true,                   // Истина, если устройство не занято операциями
    false,                  // Канал открыт
    (void *)&mxifa_linux_tcpip_6 // Специфические данные канала
  }

};
// Обобщенная структура данных канала 1
//mxifa_chdata_t mxifa_chdata_1 = {mxifa_ei_linux_tcpip, mxifa_avr128_ether_1};
// Массив указателей на данные всех каналов
//mxifa_chdata_t *mxifa_chdata[] = {IRS_NULL, &mxifa_chdata_1};
// Размер массива указателей на данные всех каналов
#define MXIFA_CHD_SIZE (IRS_ARRAYOFSIZE(mxifa_chdata))
// Номер текущего канала
mxifa_ch_t channel_cur = 1;
// Указатель на первый элемент массива с данными каналов
mxifa_chdata_t *mxifa_chdata_begin = &mxifa_chdata[1];
//mxifa_chdata_t *mxifa_chdata_end = &mxifa_chdata[MXIFA_CHD_SIZE - 1];
// Указатель на текущий элемент массива с данными каналов
mxifa_chdata_t *mxifa_chdata_cur = mxifa_chdata_begin;
// Данные текущей удаленной системы
//mxifa_dest_t curr_dest;
// Текущий IP удаленной системы
mxip_t linux_tcpip_default_ip_client = {{127,0,0,1}};
mxip_t linux_tcpip_default_ip_server = {{192,168,0,16}};
static irs_u8 count_init = 0;

// Инициализация mxifa
void mxifa_init()
{
  if (!count_init)
  {
    init_to_cnt();
    mxifa_linux_tcpip_5.r_it = mxifa_linux_tcpip_5.HD.begin();
    mxifa_linux_tcpip_5.dest_ip_def = se_dest_ip;
    mxifa_linux_tcpip_5.dest_port_def = se_dest_port;
    mxifa_linux_tcpip_5.dest_ip_cur = se_dest_ip;
    mxifa_linux_tcpip_5.dest_port_cur = se_dest_port;
    mxifa_linux_tcpip_5.dest_ip = se_dest_ip;
    mxifa_linux_tcpip_5.null_ip = NULL_IP;
    // Включение широковещания
    mxifa_linux_tcpip_5.is_broadcast = irs_false;
    mxifa_linux_tcpip_5.can_read = irs_false;
    mxifa_linux_tcpip_5.can_write = irs_false;
    mxifa_linux_tcpip_5.can_rany = irs_false;
    mxifa_linux_tcpip_5.is_empty = irs_true;
    mxifa_linux_tcpip_5.sock_finded = irs_false;
    mxifa_linux_tcpip_5.socket_fd = -1;
    FD_ZERO(&mxifa_linux_tcpip_5.master);
    FD_ZERO(&mxifa_linux_tcpip_5.read_fds);
    FD_ZERO(&mxifa_linux_tcpip_5.write_fds);
    mxifa_linux_tcpip_5.maxfd = 0;
    mxifa_linux_tcpip_5.newfd = 0;
    mxifa_linux_tcpip_5.cur_rfd = -1;
    mxifa_linux_tcpip_5.cur_ac_sock = 0;
    mxifa_linux_tcpip_5.cur_rw_sock = 0;
    mxifa_linux_tcpip_5.local_port = MXNET_PORT;
    mxifa_linux_tcpip_5.null_port = NULL_PORT;
    mxifa_linux_tcpip_5.dest_port = se_dest_port;
    mxifa_linux_tcpip_5.null_addr.sin_family = AF_INET;
    mxifa_linux_tcpip_5.acc_addr.sin_family = AF_INET;
    mxifa_linux_tcpip_5.acc_addr.sin_port =
      htons(mxifa_linux_tcpip_5.local_port);
    mxifa_linux_tcpip_5.null_addr.sin_port =
      htons(mxifa_linux_tcpip_5.null_port);
    mxifa_linux_tcpip_5.acc_addr.sin_addr.s_addr  = INADDR_ANY;
    mxifa_linux_tcpip_5.null_addr.sin_addr.s_addr =
      *(unsigned long *)&(mxifa_linux_tcpip_5.null_ip);
    memset(&(mxifa_linux_tcpip_5.acc_addr.sin_zero),'\0',8);
    memset(&(mxifa_linux_tcpip_5.null_addr.sin_zero),'\0',8);
    mxifa_linux_tcpip_5.local_addr.sin_family = AF_INET;
    mxifa_linux_tcpip_5.local_addr.sin_port =
      htons(mxifa_linux_tcpip_5.local_port);
    mxifa_linux_tcpip_5.local_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(mxifa_linux_tcpip_5.local_addr.sin_zero),'\0',8);
    mxifa_linux_tcpip_5.dest_addr.sin_family = AF_INET;
    mxifa_linux_tcpip_5.dest_addr.sin_port =
      htons(mxifa_linux_tcpip_5.dest_port );

    mxifa_linux_tcpip_5.dest_addr.sin_addr.s_addr =
      *(unsigned long *)&(mxifa_linux_tcpip_5.dest_ip);
    memset(&(mxifa_linux_tcpip_5.dest_addr.sin_zero),'\0',8);

    mxifa_linux_tcpip_5.read_addr.sin_family = AF_INET;
    memset(&(mxifa_linux_tcpip_5.read_addr.sin_zero),'\0',8);
    mxifa_linux_tcpip_5.read_addr.sin_port =
      htons(mxifa_linux_tcpip_5.null_port );
    mxifa_linux_tcpip_5.read_addr.sin_addr.s_addr =
      *(unsigned long *)&(mxifa_linux_tcpip_5.null_ip);

    mxifa_linux_tcpip_5.buf_lenth = 0;
    // Буфер чтения
    mxifa_linux_tcpip_5.rd_buf = IRS_NULL;
    mxifa_linux_tcpip_5.rd_buf_bu = IRS_NULL;
    mxifa_linux_tcpip_5.read_address = IRS_NULL;
    // Размер буфера чтения
    mxifa_linux_tcpip_5.rd_size = 0;
    mxifa_linux_tcpip_5.rd_size_bu = 0;
    mxifa_linux_tcpip_5.wr_buf = IRS_NULL;
    mxifa_linux_tcpip_5.wr_size = 0;
    mxifa_linux_tcpip_5.tv_zero.tv_sec = 0;
    mxifa_linux_tcpip_5.tv_zero.tv_usec = 0;
    mxifa_linux_tcpip_5.it = mxifa_linux_tcpip_5.HD.begin();
    for(int i=0; i < addrmass_size; i++)
    {
      mxifa_linux_tcpip_5.addr_mass[i].host_fd = -1;
      mxifa_linux_tcpip_5.addr_mass[i].opened = irs_false;
      socklen_t rlen = sizeof(mxifa_linux_tcpip_5.addr_mass[i]);
      memset(&(mxifa_linux_tcpip_5.addr_mass[i]),0,rlen);
    }
    mxifa_linux_tcpip_5.first_read = irs_false;
    mxifa_linux_tcpip_5.wr_socket = -1;

    mxifa_linux_tcpip_6.dest_ip_def = cl_dest_ip;
    mxifa_linux_tcpip_6.dest_port_def = cl_dest_port;
    mxifa_linux_tcpip_6.dest_ip_cur = cl_dest_ip;
    mxifa_linux_tcpip_6.dest_port_cur = cl_dest_port;
    mxifa_linux_tcpip_6.dest_ip = mxifa_linux_tcpip_6.dest_ip_def;
    mxifa_linux_tcpip_6.is_broadcast = irs_false;
    mxifa_linux_tcpip_6.can_read = irs_false;
    mxifa_linux_tcpip_6.can_write = irs_false;
    mxifa_linux_tcpip_6.socket_fd = -1;
    FD_ZERO(&mxifa_linux_tcpip_6.master);
    FD_ZERO(&mxifa_linux_tcpip_6.read_fds);
    FD_ZERO(&mxifa_linux_tcpip_5.write_fds);
    mxifa_linux_tcpip_6.local_port = se_dest_port;;
    mxifa_linux_tcpip_6.dest_port = cl_dest_port;
    mxifa_linux_tcpip_6.local_addr.sin_family = AF_INET;
    mxifa_linux_tcpip_6.local_addr.sin_port =
      htons(mxifa_linux_tcpip_6.local_port);
    mxifa_linux_tcpip_6.local_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(mxifa_linux_tcpip_6.local_addr.sin_zero),'\0',8);
    mxifa_linux_tcpip_6.dest_addr.sin_family = AF_INET;
    mxifa_linux_tcpip_6.dest_addr.sin_port =
      htons(mxifa_linux_tcpip_6.dest_port );
    mxifa_linux_tcpip_6.dest_addr.sin_addr.s_addr =
      *(unsigned long *)&(mxifa_linux_tcpip_6.dest_ip);
    memset(&(mxifa_linux_tcpip_6.dest_addr.sin_zero),'\0',8);
    mxifa_linux_tcpip_6.buf_lenth = 0;
    mxifa_linux_tcpip_6.rd_buf = IRS_NULL;
    mxifa_linux_tcpip_6.rd_size = 0;
    mxifa_linux_tcpip_6.wr_buf = IRS_NULL;
    mxifa_linux_tcpip_6.wr_size = 0;
    mxifa_linux_tcpip_6.tv_zero.tv_sec = 0;
    mxifa_linux_tcpip_6.tv_zero.tv_usec = 0;
    //mxifa_linux_tcpip_6.read_end = irs_true;
    //mxifa_linux_tcpip_6.write_end = irs_true;
    mxifa_linux_tcpip_6.sock_open = irs_false;

  }
  count_init++;
}
// Деинициализация mxifa
void mxifa_deinit()
{
  count_init--;
  if (!count_init) {
    deinit_to_cnt();
  }
}

static void mxifa_open_cl_socket(mxifa_chdata_t *cur_chanel)
{
  mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
  (mxifa_linux_tcpip_cl_t *)cur_chanel->ch_spec;

  FD_ZERO(&(linux_tcpip_cl->master));
  FD_ZERO(&(linux_tcpip_cl->read_fds));
  FD_ZERO(&(linux_tcpip_cl->write_fds));
  linux_tcpip_cl->socket_fd = 0;
  linux_tcpip_cl->socket_fd = socket(PF_INET,SOCK_STREAM,0);
  //puts("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  //if(linux_tcpip_cl->socket_fd == -1)
    //OUTDBG(cout"400 CLIENT - Can't open socket!\n");
 // else
   // puts("402 CLIENT - Socket:            OK");
  int ye = 1;
  int w = setsockopt(linux_tcpip_cl->socket_fd,
            SOL_SOCKET, SO_REUSEADDR, &ye, sizeof(ye));
  if(w == -1)
    perror("setsockopt");
 // else
   // OUTDBG(cout"409 CLIENT - Setsockopt:        OK\n");

  int d = bind(linux_tcpip_cl->socket_fd,
          (struct sockaddr *)&(linux_tcpip_cl->local_addr),
          sizeof(struct sockaddr));
  if(d == -1)
    perror("Bind");
  //else
   // OUTDBG(cout"417 CLIENT - Bind:              OK\n");
  // int yes = 1;
  //OUTDBG(cout"419 CLIENT - Socket_fd:%d\n",
      //linux_tcpip_cl->socket_fd);
  socklen_t addrl_1  = sizeof(linux_tcpip_cl->dest_addr);
  int er = connect(linux_tcpip_cl->socket_fd,
    (struct sockaddr *)&(linux_tcpip_cl->dest_addr),addrl_1);
  if(er == -1)
  {
    //perror("connect");
    //OUTDBG(cout"427 Client - number of error %d\n",errno);
    //OUTDBG(cout"428 CLIENT - dest addr %s\n",
     // inet_ntoa(linux_tcpip_cl->dest_addr.sin_addr));
    close(linux_tcpip_cl->socket_fd);
    linux_tcpip_cl->sock_open = irs_false;
  }
  else
  {
    //OUTDBG(cout"435 CLIENT - Connect: OK\n");
    linux_tcpip_cl->sock_open = irs_true;
    FD_SET(linux_tcpip_cl->socket_fd, &(linux_tcpip_cl->master));
  }
  cur_chanel->opened = irs_true;
}
static void mxifa_read_cl_socket(mxifa_chdata_t *cur_chanel)
{
  mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
    (mxifa_linux_tcpip_cl_t *)cur_chanel->ch_spec;
  //puts("445 CLIENT - read");
  //linux_tcpip_cl->read_end = irs_false;
  mxifa_sz_t nbytes_r = recv(linux_tcpip_cl->socket_fd,
    (void *)linux_tcpip_cl->rd_buf,
    (size_t)linux_tcpip_cl->rd_size,0);
  if(nbytes_r <= 0)
  {
    if(nbytes_r == 0)
    {
      //OUTDBG(cout"454 CLIENT - Socket %d hung up when reading\n",
      //linux_tcpip_cl->socket_fd);
    }
    else
    {
      perror("recv");
      //OUTDBG(cout"460 Client - number of error %d\n",errno);
    }
    close(linux_tcpip_cl->socket_fd);
    linux_tcpip_cl->sock_open = irs_false;
    FD_CLR(linux_tcpip_cl->socket_fd,
    &(linux_tcpip_cl->master));
    FD_CLR(linux_tcpip_cl->socket_fd,
    &(linux_tcpip_cl->read_fds));
    FD_CLR(linux_tcpip_cl->socket_fd,
    &(linux_tcpip_cl->write_fds));
    linux_tcpip_cl->can_read = irs_false;
  }
  else
  {
    #ifdef NOP
    OUTDBG(cout"475 CLIENT - Get message\n");
    OUTDBG(cout"476 CLIENT - Number recivied bytes %d\n", nbytes_r);
    OUTDBG(cout"477 CLIENT - File descriptor %d\n", linux_tcpip_cl->socket_fd);
    OUTDBG(cout"478 CLIENT - bufer size: %d\n", (int)linux_tcpip_cl->rd_size);

    #endif
   // OUTDBG(cout"481 CLIENT - bufer %s\n", linux_tcpip_cl->rd_buf);
    linux_tcpip_cl->rd_buf += nbytes_r;
    linux_tcpip_cl->rd_size -= nbytes_r;
    if(linux_tcpip_cl->rd_size <= 0)
    {
      linux_tcpip_cl->can_read = irs_false;
     // OUTDBG(cout"487 CLIENT - Get all parts of the message\n");
    }
  }
}
static void mxifa_write_cl_socket(mxifa_chdata_t *cur_chanel)
{
  mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
  (mxifa_linux_tcpip_cl_t *)cur_chanel->ch_spec;
 // OUTDBG(cout"495 CLIENT - SEND:bufer size %d\n",
  //(int)linux_tcpip_cl->wr_size);
  int nbytes_wr =
      send(linux_tcpip_cl->socket_fd,
        (void *)linux_tcpip_cl->wr_buf,
        (size_t)linux_tcpip_cl->wr_size,0);
  if(nbytes_wr <= 0)
  {
    if(nbytes_wr == 0)
    {
      //OUTDBG(cout"505 CLIENT - Socket %d hung up wen sending\n",
        //linux_tcpip_cl->socket_fd);
    }
    else
    {
      perror("send");
      //OUTDBG(cout"511 Client - number of error %d\n",errno);
    }
    close(linux_tcpip_cl->socket_fd);
    linux_tcpip_cl->sock_open = irs_false;
    FD_CLR(linux_tcpip_cl->socket_fd,
    &(linux_tcpip_cl->master));
    FD_CLR(linux_tcpip_cl->socket_fd,
    &(linux_tcpip_cl->read_fds));
    FD_CLR(linux_tcpip_cl->socket_fd,
    &(linux_tcpip_cl->write_fds));
    linux_tcpip_cl->can_write = irs_false;
  }
  else
  {
    #ifdef NOP
    OUTDBG(cout"526 CLIENT - SEND:Number send bytes %d\n", nbytes_wr);
    OUTDBG(cout"527 CLIENT - SEND:File descriptor %d\n",
      linux_tcpip_cl->socket_fd);
    OUTDBG(cout"529 CLIENT - SEND:bufer size %d\n",
      (int)linux_tcpip_cl->wr_size);
    OUTDBG(cout"531 CLIENT - SEND:bufer %s\n", linux_tcpip_cl->wr_buf);
    #endif //NOP
    linux_tcpip_cl->wr_buf += nbytes_wr;
    linux_tcpip_cl->wr_size -= nbytes_wr;
    if(linux_tcpip_cl->wr_size <= 0)
    {
      linux_tcpip_cl->can_write = irs_false;
    //  OUTDBG(cout"538 CLIENT - Send all parts of the message\n");
    }
    //linux_tcpip_cl->can_write == irs_false;
  }
}

static void mxifa_reconect_cl_socket(mxifa_chdata_t *cur_chanel)
{
  mxifa_chdata_t *pchdatas = mxifa_chdata_cur;
  mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
   (mxifa_linux_tcpip_cl_t *)cur_chanel->ch_spec;
  if((linux_tcpip_cl->dest_addr.sin_addr.s_addr !=
  *(unsigned long *)&(linux_tcpip_cl->dest_ip_cur))||
  (linux_tcpip_cl->dest_addr.sin_port !=
    htons(linux_tcpip_cl->dest_port_cur)))
  {
    puts("554 CLIENT - Reconnect socket");
    if(linux_tcpip_cl->sock_open == irs_true)
    {
      close(linux_tcpip_cl->socket_fd);
      linux_tcpip_cl->sock_open = irs_false;
    }
    linux_tcpip_cl->dest_addr.sin_addr.s_addr =
      *(unsigned long *)&(linux_tcpip_cl->dest_ip_cur);
    linux_tcpip_cl->dest_addr.sin_port =
      htons(linux_tcpip_cl->dest_port_cur);
    mxifa_open_cl_socket(pchdatas);
  }
}

static void mxifa_open_se_socket(mxifa_chdata_t *cur_chanel)
{
  mxifa_linux_tcpip_t *linux_tcpip =
  (mxifa_linux_tcpip_t *)cur_chanel->ch_spec;
  FD_ZERO(&(linux_tcpip->master));
  FD_ZERO(&(linux_tcpip->read_fds));
  FD_ZERO(&(linux_tcpip->write_fds));
  linux_tcpip->socket_fd = socket(PF_INET,SOCK_STREAM,0);
  if(linux_tcpip->socket_fd == -1)
    OUTDBG(cout<<"577 SERVER - Can't open socket!\n");
  //else
    //puts("579 SERVER - Socket:            OK");
  int yes = 1;
  //OUTDBG(cout"581 SERVER - Socket_fd:         %d\n",
      //linux_tcpip->socket_fd);
  int w = setsockopt(linux_tcpip->socket_fd,
            SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  if(w == -1)
    perror("setsockopt");
  //else
    //OUTDBG(cout"588 SERVER - Setsockopt:        OK\n");
  int d = bind(linux_tcpip->socket_fd,
          (struct sockaddr *)&(linux_tcpip->local_addr),
          sizeof(struct sockaddr));
  if(d == -1)
    perror("Bind");
  //else
    //OUTDBG(cout"595 SERVER - Bind:              OK\n");

  int a = listen(linux_tcpip->socket_fd, addrmass_size);
  if(a == -1)
    perror("listen");
  //else
    //OUTDBG(cout"601 SERVER - Listen:            OK\n");
  FD_SET(linux_tcpip->socket_fd, &(linux_tcpip->master));
  FD_SET(linux_tcpip->socket_fd, &(linux_tcpip->read_fds));
  FD_SET(linux_tcpip->socket_fd, &(linux_tcpip->write_fds));
  linux_tcpip->maxfd = linux_tcpip->socket_fd;
  linux_tcpip->cur_rw_sock = 0;
  linux_tcpip->cur_ac_sock = 0;
}

static void mxifa_close_se_socket(mxifa_chdata_t *cur_chanel)
{
  mxifa_linux_tcpip_t *linux_tcpip =
   (mxifa_linux_tcpip_t *)cur_chanel->ch_spec;
  //int j = 0;
  list<Hdata>::iterator remove_it;
  for(remove_it = linux_tcpip->HD.begin();
    remove_it != linux_tcpip->HD.end();
    remove_it++)
  {
    close((*(remove_it)).get_hfd());
  }
  close(linux_tcpip->socket_fd);
}

// Блокирующее открытие channel
void *mxifa_open(mxifa_ch_t channel, irs_bool is_broadcast)
{
  mxifa_chdata_t *pchdatas = mxifa_chdata_cur;
  if(pchdatas->opened)return IRS_NULL;
  //if(linux_tcpip_cl->sock_open == irs_false) break;
  void *pchdata = mxifa_open_begin(channel, is_broadcast);
  counter_t var_timeout_channel_open;
  set_to_cnt(var_timeout_channel_open, TIMEOUT_CHANNEL);
  for (;;) {
    if (mxifa_open_end(pchdata, irs_false)) return pchdata;
    if (test_to_cnt(var_timeout_channel_open)) {
      mxifa_open_end(pchdata, irs_true);
      break;
    }
  }
  return IRS_NULL;
}
// Неблокирующее открытие channel
void *mxifa_open_begin(mxifa_ch_t channel, irs_bool is_broadcast)
{
  if ((channel < 1) || (channel >= MXIFA_CHD_SIZE)) return IRS_NULL;
  mxifa_chdata_t *cur_ch = &mxifa_chdata[channel];
  //OUTDBG(cout"648 Channel %d \n", (int) channel);
  switch (cur_ch->enum_iface) {
    case mxifa_ei_linux_tcpip:
    {
      //puts("652 SERVER - Mxifa_open_begin:  OK");
      mxifa_linux_tcpip_t *linux_tcpip =
        (mxifa_linux_tcpip_t *)cur_ch->ch_spec;
      const mxip_t *dest_ip = &linux_tcpip->dest_ip;
      linux_tcpip->is_broadcast = is_broadcast;
      if(is_broadcast)
      {
        dest_ip = &broadcast_ip;
      }
      mxifa_open_se_socket(cur_ch);
      cur_ch->opened = irs_true;
    }
    break;
    case mxifa_ei_linux_tcpip_cl:
    {
      //puts("667 CLIENT - Mxifa_open_begin:  OK");
      mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
        (mxifa_linux_tcpip_cl_t *)cur_ch->ch_spec;
      const mxip_t *dest_ip = &linux_tcpip_cl->dest_ip;
      linux_tcpip_cl->is_broadcast = is_broadcast;
      if (is_broadcast)
      {
        dest_ip = &broadcast_ip;
      }
      mxifa_open_cl_socket(cur_ch);
    }
    break;
    default : {
    }
  }
  return cur_ch;
}
// Проверка окончания открытия channel
irs_bool mxifa_open_end(void *pchdata, bool abort)
{
  if (pchdata == IRS_NULL) return irs_false;
  irs_bool opened = irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_linux_tcpip: {
      abort = abort;
      opened = irs_true;
    } break;
    case mxifa_ei_linux_tcpip_cl: {
      abort = abort;
      opened = irs_true;
      //return mxifa_linux_tcpip_6.sock_open;
    } break;
    default : {
    }
  }
  return opened;
}
// Блокирующее закрытие channel
irs_bool mxifa_close(void *pchdata)
{
  if (pchdata == IRS_NULL) return irs_false;
  if (mxifa_close_begin(pchdata)) return irs_true;
  counter_t var_timeout_channel_close;
  set_to_cnt(var_timeout_channel_close, TIMEOUT_CHANNEL);
  for (;;) {
    if (mxifa_close_end(pchdata, irs_false)) return irs_true;
    if (test_to_cnt(var_timeout_channel_close)) {
      mxifa_close_end(pchdata, irs_true);
      break;
    }
  }
  return irs_false;
}
// Неблокирующее закрытие channel
irs_bool mxifa_close_begin(void *pchdata)
{
  if (pchdata == IRS_NULL) return irs_false;
  irs_bool closed = irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;

  switch (pchdatas->enum_iface) {
    case mxifa_ei_linux_tcpip:
    {
      #ifdef NOP
      mxifa_linux_tcpip_t *linux_tcpip =
        (mxifa_linux_tcpip_t *)pchdatas->ch_spec;
        mxifa_close_se_socket(pchdatas);
        pchdatas->opened = irs_false;
      #endif //NOP
    }
    break;
    case mxifa_ei_linux_tcpip_cl:
    {
      mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
       (mxifa_linux_tcpip_cl_t *)pchdatas->ch_spec;
        close(linux_tcpip_cl->socket_fd);
        linux_tcpip_cl->sock_open = irs_false;
        pchdatas->opened = irs_false;
    }
    break;
    default : {
    }
  }
  return closed;
}
// Проверка окончания закрытия channel
irs_bool mxifa_close_end(void *pchdata, bool abort)
{
  if (pchdata == IRS_NULL) return irs_false;
  irs_bool closed = irs_false;
  mxifa_chdata_t
  *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_linux_tcpip: {
     // mxifa_avr128_ether_t *avr128_ether =
     //   (mxifa_avr128_ether_t *)pchdatas->ch_spec;
     // if (abort) {
    } break;
    case mxifa_ei_linux_tcpip_cl: {
     // mxifa_avr128_ether_t *avr128_ether =
     //   (mxifa_avr128_ether_t *)pchdatas->ch_spec;
     // if (abort) {
    } break;
    default : {
    }
  }
  return closed;
}
// Запись буфера в интерфейс для передачи
irs_bool mxifa_write_begin(void *pchdata, mxifa_dest_t *dest,
  irs_u8 *buf, mxifa_sz_t size)
{
  //OUTDBG(cout"Write_begin Start\n");
  if (pchdata == IRS_NULL) return irs_false;
  if (buf == IRS_NULL) return irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  //OUTDBG(cout"Write_begin Start - 1\n");
  switch (pchdatas->enum_iface) {
    case mxifa_ei_linux_tcpip:
    {
      pchdatas->mode_free = false;
      mxifa_linux_tcpip_t *linux_tcpip =
       (mxifa_linux_tcpip_t *)pchdatas->ch_spec;
      linux_tcpip->wr_buf = buf;
      linux_tcpip->wr_size = size;
      //linux_tcpip->can_write = irs_true;
      //OUTDBG(cout"809 SERVER - write begin:bufer size %d\n", size);
      if(dest == IRS_NULL)
      {
        if(linux_tcpip->is_broadcast == irs_true)
        {
          linux_tcpip->dest_addr.sin_addr.s_addr =
            inet_addr("255.255.255.255");
          linux_tcpip->dest_addr.sin_port =
            htons(linux_tcpip->dest_port_cur);
        }
        else
        {
          linux_tcpip->dest_ip_cur = linux_tcpip->dest_ip_def;
          linux_tcpip->dest_port_cur = linux_tcpip->dest_port_def;
          linux_tcpip->dest_addr.sin_addr.s_addr =
          *(unsigned long *)&(linux_tcpip->dest_ip_cur);
          linux_tcpip->dest_addr.sin_port =
            htons(linux_tcpip->dest_port_cur);
          //irs_bool is_finded = irs_false;
          if(linux_tcpip->it == linux_tcpip->HD.end())
            linux_tcpip->it = linux_tcpip->HD.begin();
          if(linux_tcpip->HD.size() == 0)
            linux_tcpip->can_write = irs_false;
          else
            linux_tcpip->can_write = irs_true;
          }
      }
      else
      {
        linux_tcpip->dest_ip_cur = dest->mxifa_linux_dest.ip;
        linux_tcpip->dest_port_cur = dest->mxifa_linux_dest.port;
        linux_tcpip->dest_addr.sin_addr.s_addr =
          *(unsigned long *)&(linux_tcpip->dest_ip_cur);
        linux_tcpip->dest_addr.sin_port =
          htons(linux_tcpip->dest_port_cur);
        if(linux_tcpip->it == linux_tcpip->HD.end())
            linux_tcpip->it = linux_tcpip->HD.begin();
        if(linux_tcpip->HD.size() == 0)
          linux_tcpip->can_write = irs_false;
        else
          linux_tcpip->can_write = irs_true;
        }
    } break;
    case mxifa_ei_linux_tcpip_cl:
    {
      pchdatas->mode_free = false;
      mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
       (mxifa_linux_tcpip_cl_t *)pchdatas->ch_spec;
      linux_tcpip_cl->wr_buf = buf;
      linux_tcpip_cl->wr_size = size;
      linux_tcpip_cl->can_write = irs_true;
      if(dest == IRS_NULL)
      {
        if(linux_tcpip_cl->is_broadcast)
        {
          linux_tcpip_cl->dest_addr.sin_addr.s_addr =
            inet_addr("255.255.255.255");
          linux_tcpip_cl->dest_addr.sin_port =
            htons(linux_tcpip_cl->dest_port_cur);
        }
        else
        {
          linux_tcpip_cl->dest_ip_cur = linux_tcpip_cl->dest_ip_def;
          linux_tcpip_cl->dest_port_cur = linux_tcpip_cl->dest_port_def;
        }
      }
      else
      {
        linux_tcpip_cl->dest_ip_cur = dest->mxifa_linux_cl_dest.ip;
        linux_tcpip_cl->dest_port_cur = dest->mxifa_linux_cl_dest.port;
      }
    } break;
    default : {
    }
  }
  return irs_false;
}
// Проверка окончания записи
irs_bool mxifa_write_end(void *pchdata, irs_bool abort)
{
  if (pchdata == IRS_NULL) return irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_linux_tcpip: {
      mxifa_linux_tcpip_t *linux_tcpip =
        (mxifa_linux_tcpip_t *)pchdatas->ch_spec;
      if (abort)
      {
        linux_tcpip->can_write = irs_false;
        return irs_true;
      }
      if (linux_tcpip->can_write)
        return irs_false;
      else
        return irs_true;
    } break;
    case mxifa_ei_linux_tcpip_cl:
    {
      mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
        (mxifa_linux_tcpip_cl_t *)pchdatas->ch_spec;
      if (abort)
      {
        linux_tcpip_cl->can_write = irs_false;
        return irs_true;
      }
      if (linux_tcpip_cl->can_write)
        return irs_false;
      else
        return irs_true;
    } break;
    default : {
    }
  }
  return irs_false;
}
// Чтение принятых данных из интерфейса в буфер
irs_bool mxifa_read_begin(void *pchdata, mxifa_dest_t *dest,
  irs_u8 *buf, mxifa_sz_t size)
{
  if (pchdata == IRS_NULL) return irs_false;
  if (buf == IRS_NULL) return irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_linux_tcpip: {
      mxifa_linux_tcpip_t *linux_tcpip =
        (mxifa_linux_tcpip_t *)pchdatas->ch_spec;
      linux_tcpip->rd_buf = buf;
      linux_tcpip->rd_buf_bu = buf;
      linux_tcpip->rd_size = size;
      linux_tcpip->rd_size_bu = size;
      linux_tcpip->can_read = irs_true;
      linux_tcpip->first_read = irs_true;
      //linux_tcpip->can_rany = irs_true;
      linux_tcpip->read_address = dest;

      bool port_null =
        dest->mxifa_linux_dest.port ==
        ntohs(linux_tcpip->null_addr.sin_port);
      unsigned long var_ip = linux_tcpip->null_addr.sin_addr.s_addr;
      bool ip_null = dest->mxifa_linux_dest.ip ==
          *(mxip_t *)&(var_ip);

      if (port_null && ip_null)
      {
        linux_tcpip->can_rany = irs_true;
      }
      else
      {
        linux_tcpip->can_rany = irs_false;
        //linux_tcpip->r_it = linux_tcpip->HD.begin();
      }

    } break;
    case mxifa_ei_linux_tcpip_cl:
    {
      //puts("CLIENT - function read_begin");
      mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
        (mxifa_linux_tcpip_cl_t *)pchdatas->ch_spec;
      linux_tcpip_cl->rd_buf = buf;
      linux_tcpip_cl->rd_size = size;
      linux_tcpip_cl->can_read = irs_true;
      if(dest != IRS_NULL)
      {
        dest->mxifa_linux_cl_dest.port =
          ntohs(linux_tcpip_cl->dest_addr.sin_port);
        dest->mxifa_linux_cl_dest.ip = *(mxip_t *)
          &linux_tcpip_cl->dest_addr.sin_addr.s_addr;
      }
    } break;
    default : {
    }
  }
  return irs_false;
}
// Проверка окончания чтения
irs_bool mxifa_read_end(void *pchdata, irs_bool abort)
{
  if (pchdata == IRS_NULL) return irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_linux_tcpip: {
      mxifa_linux_tcpip_t *linux_tcpip =
        (mxifa_linux_tcpip_t *)pchdatas->ch_spec;
      if (mxifa_linux_tcpip_5.HD.size() == 0) return irs_false;
      if(linux_tcpip->can_read == irs_false )
       return irs_true;
      else
       return irs_false;
      if(abort)
      {
        linux_tcpip->can_read = irs_false;
        return irs_true;
      }
    } break;
    case mxifa_ei_linux_tcpip_cl:
    {
      mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
        (mxifa_linux_tcpip_cl_t *)pchdatas->ch_spec;
      //puts("CLIENT - read end");
      if (linux_tcpip_cl->can_read == irs_false)
       return irs_true;
      else
       return irs_false;
      if(abort)
      {
         linux_tcpip_cl->can_read = irs_false;
         return irs_true;
      }
    } break;
    default : {
    }
  }
  return irs_false;
}
void mxifa_add_sock(int fd, struct sockaddr_in addr)
{
  mxifa_chdata_t *pchdatas = mxifa_chdata_cur;
  switch (mxifa_chdata_cur->enum_iface)
  {
    case mxifa_ei_linux_tcpip:
    {
      mxifa_linux_tcpip_t *linux_tcpip =
        (mxifa_linux_tcpip_t *)pchdatas->ch_spec;
      bool is_empty = linux_tcpip->HD.empty();
      linux_tcpip->HD.push_front(Hdata());
      linux_tcpip->HD.front().set_host_fd(fd);
      linux_tcpip->HD.front().set_host_addr(addr);
      if (is_empty) linux_tcpip->r_it = linux_tcpip->HD.begin();
    }
    break;
    default : {
    }
  }
}
void mxifa_remove_sock(list<Hdata>::iterator it_cur)
{
  mxifa_chdata_t *pchdatas = mxifa_chdata_cur;
  switch (mxifa_chdata_cur->enum_iface)
  {
    case mxifa_ei_linux_tcpip:
    {
      mxifa_linux_tcpip_t *linux_tcpip =
       (mxifa_linux_tcpip_t *)pchdatas->ch_spec;

      if (linux_tcpip->r_it == it_cur) {
        linux_tcpip->r_it++;
        if(linux_tcpip->r_it == linux_tcpip->HD.end())
          linux_tcpip->r_it = linux_tcpip->HD.begin();
      }
      linux_tcpip->HD.erase(it_cur);
    }
    break;
    default : {
    }
  }
}
void  mxifa_find_read_sock()
{
  mxifa_chdata_t *pchdatas = mxifa_chdata_cur;
  switch (mxifa_chdata_cur->enum_iface)
  {
    case mxifa_ei_linux_tcpip:
    {
      mxifa_linux_tcpip_t *linux_tcpip =
       (mxifa_linux_tcpip_t *)pchdatas->ch_spec;
      linux_tcpip->sock_finded = irs_false;
      if (linux_tcpip->first_read == irs_true) {
        if (linux_tcpip->can_rany == irs_true)
        {
          for(size_t i = 0;i < linux_tcpip->HD.size();i++)
          {
            if(FD_ISSET((*(linux_tcpip->r_it)).get_hfd(),
              &(linux_tcpip->read_fds)))
            {
              linux_tcpip->sock_finded = irs_true;
              break;
            }
            linux_tcpip->r_it++;
            if(linux_tcpip->r_it == linux_tcpip->HD.end())
            {
              linux_tcpip->r_it = linux_tcpip->HD.begin();
            }
          }
        }
        else
        {
          for(size_t i = 0;i<linux_tcpip->HD.size();i++)
          {
            bool port_valid =
              linux_tcpip->read_address->mxifa_linux_dest.port ==
              ntohs((*(linux_tcpip->r_it)).get_hport());
            unsigned long var_ip = (*(linux_tcpip->r_it)).get_haddr();
            bool ip_valid = linux_tcpip->read_address->mxifa_linux_dest.ip ==
                *(mxip_t *)&(var_ip);
            if (port_valid && ip_valid)
            if (FD_ISSET((*(linux_tcpip->r_it)).get_hfd(),
              &(linux_tcpip->read_fds)))
            {
              linux_tcpip->sock_finded = irs_true;
              break;
            }
            linux_tcpip->r_it++;
            if(linux_tcpip->r_it == linux_tcpip->HD.end())
            {
              linux_tcpip->r_it = linux_tcpip->HD.begin();
            }
          }
        }
      }
    }
    break;
    default : {
    }
  }
}
// Элементарное действие
void mxifa_tick()
{
  //OUTDBG(cout"Start tick() - 0\n");
  mxifa_chdata_t *pchdatas = mxifa_chdata_cur;
  //OUTDBG(cout"Start tick() - 1\n");
  switch (mxifa_chdata_cur->enum_iface)
  {
    case mxifa_ei_linux_tcpip:
    {
      if(!pchdatas->opened)break;
      //OUTDBG(cout"Channel number - %d\n",channel_cur);
      //OUTDBG(cout"Start tick() - 2\n");
      mxifa_linux_tcpip_t *linux_tcpip =
       (mxifa_linux_tcpip_t *)pchdatas->ch_spec;
      linux_tcpip->read_fds = linux_tcpip->master;
      linux_tcpip->write_fds = linux_tcpip->master;
      int se = select(linux_tcpip->maxfd+1,&(linux_tcpip->read_fds),
        &(linux_tcpip->write_fds),NULL,&(linux_tcpip->tv_zero));
      if(se < 0)
        perror("select");
      if(se > 0)
      {
        int qwe = FD_ISSET(linux_tcpip->socket_fd,&(linux_tcpip->read_fds));
        if(qwe)
        {
          FD_SET(linux_tcpip->socket_fd, &(linux_tcpip->read_fds));
          socklen_t addrlen = sizeof(linux_tcpip->acc_addr);
          linux_tcpip->newfd = accept(linux_tcpip->socket_fd,
            (struct sockaddr *)&linux_tcpip->acc_addr,
            &(addrlen));
          if( linux_tcpip->newfd == -1)
          {
            perror("accept");
          }
          else
          {
            OUTDBG(cout<<"1058 SERVER - Accept: OK\nNew file descriptor: %d\n"
              << linux_tcpip->newfd<<"\n" );
            FD_SET(linux_tcpip->newfd,&(linux_tcpip->master));
            FD_SET(linux_tcpip->newfd, &(linux_tcpip->read_fds));
            FD_SET(linux_tcpip->newfd, &(linux_tcpip->write_fds));
            if(linux_tcpip->newfd > linux_tcpip->maxfd)
              linux_tcpip->maxfd = linux_tcpip->newfd;
            mxifa_add_sock(linux_tcpip->newfd, linux_tcpip->acc_addr);
            // int r = linux_tcpip->HD.front().get_hfd();/*here*/
            OUTDBG(cout<<"1074 SERVER - port dest: "<< r <<"\n");
          }
        }
        if((linux_tcpip->can_read)&&(!linux_tcpip->HD.empty()))
        {
          mxifa_find_read_sock();
          if(linux_tcpip->sock_finded)
          {
            mxifa_sz_t nbytes_r =
              recv((*(linux_tcpip->r_it)).get_hfd(),
              (void *)linux_tcpip->rd_buf,
              (size_t)linux_tcpip->rd_size,MSG_NOSIGNAL);

            #ifdef NOP
            OUTDBG(cout"1122 SERVER - File descriptor %d\n",
              (*(linux_tcpip->r_it)).get_hfd());
            OUTDBG(cout"1125 SERVER - bufer size %d\n",
              (int)linux_tcpip->rd_size);
            #endif //NOP
            if(nbytes_r <= 0)
            {
              #ifdef NOP
              OUTDBG(cout"1133 SERVER - Recv: socket %d hung up\n",
                (*(linux_tcpip->r_it)).get_hfd());
              #endif //NOP
              puts("1142 SERVER - recv close socket");
              close((*(linux_tcpip->r_it)).get_hfd());
              FD_CLR((*(linux_tcpip->r_it)).get_hfd(),
                &(linux_tcpip->master));
              FD_CLR((*(linux_tcpip->r_it)).get_hfd(),
                &(linux_tcpip->read_fds));
              FD_CLR((*(linux_tcpip->r_it)).get_hfd(),
                &(linux_tcpip->write_fds));
              linux_tcpip->rd_size = linux_tcpip->rd_size_bu;
              linux_tcpip->rd_buf = linux_tcpip->rd_buf_bu;
              linux_tcpip->can_read = irs_false;
              linux_tcpip->first_read = irs_true;
              mxifa_remove_sock(linux_tcpip->r_it);
            }
            else
            {
              if((linux_tcpip->first_read)&&(linux_tcpip->can_rany))
              {
                linux_tcpip->read_address->mxifa_linux_dest.port =
                  ntohs((*(linux_tcpip->r_it)).get_hport());
                unsigned long var_ip = (*(linux_tcpip->r_it)).get_haddr();
                linux_tcpip->read_address->mxifa_linux_dest.ip =
                *(mxip_t *)&(var_ip);
              }
              linux_tcpip->first_read = irs_false;
              if(nbytes_r == linux_tcpip->rd_size)
              {
                linux_tcpip->can_read = irs_false;
                linux_tcpip->can_rany = irs_true;
                linux_tcpip->r_it++;
                if(linux_tcpip->r_it == linux_tcpip->HD.end())
                  linux_tcpip->r_it = linux_tcpip->HD.begin();
              }
              if(nbytes_r < linux_tcpip->rd_size)
              {
                linux_tcpip->rd_size -= nbytes_r;
                linux_tcpip->rd_buf += nbytes_r;
                linux_tcpip->cur_rfd = (*(linux_tcpip->r_it)).get_hfd();
              }
            }
          }
        }
        if(linux_tcpip->can_write)
        {
          //cout << "can_write = true" << endl;
          mxifa_sz_t nbytes_wr = 0;
          if(linux_tcpip->is_broadcast)
          {
            //bad_sock = irs_true;
          }
          else
          {

            for(linux_tcpip->it = linux_tcpip->HD.begin();
              linux_tcpip->it != linux_tcpip->HD.end();)
            {
              if(((*(linux_tcpip->it)).get_haddr() ==
                linux_tcpip->dest_addr.sin_addr.s_addr)&&
                (((*(linux_tcpip->it)).get_hport() ==
                linux_tcpip->dest_addr.sin_port)))
              {
                break;
              }
              linux_tcpip->it++;
              if(linux_tcpip->it == linux_tcpip->HD.end())
              {
                linux_tcpip->it--;
                break;
              }
            }
            int t_fd = (*(linux_tcpip->it)).get_hfd();
            if((FD_ISSET(t_fd,
              &(linux_tcpip->write_fds))))
            {
              FD_SET(t_fd,&(linux_tcpip->write_fds));
              nbytes_wr =
              send((*(linux_tcpip->it)).get_hfd(),
                (void *)linux_tcpip->wr_buf,
                (size_t)linux_tcpip->wr_size,MSG_NOSIGNAL);
              #ifdef NOP
              OUTDBG(cout"1234 SERVER - File descriptor %d\n",
                (*(linux_tcpip->it)).get_hfd());
              OUTDBG(cout"1236 SERVER - bufer size %d\n",
                (int)linux_tcpip->rd_size);
              #endif //NOP
              //OUTDBG(cout"1239 SERVER - bufer %s\n", linux_tcpip->wr_buf);
              if(nbytes_wr <= 0)
              {
                OUTDBG(cout<<"1244 SERVER - Send: socket  hung up"
                  <<((*(linux_tcpip->it)).get_hfd())<<"\n");
                puts("1250 ERROR!!!");
                //perror("send");
                close((*(linux_tcpip->it)).get_hfd());
                FD_CLR((*(linux_tcpip->it)).get_hfd(),
                  &(linux_tcpip->master));
                FD_CLR((*(linux_tcpip->it)).get_hfd(),
                  &(linux_tcpip->read_fds));
                FD_CLR((*(linux_tcpip->it)).get_hfd(),
                  &(linux_tcpip->write_fds));
                linux_tcpip->can_write = irs_false;
                mxifa_remove_sock(linux_tcpip->it);
              }
              else
              {
                if(nbytes_wr == (linux_tcpip->wr_size))
                {
                  linux_tcpip->can_write = irs_false;
                }
                if(nbytes_wr < (linux_tcpip->wr_size))
                {
                  linux_tcpip->wr_size -= nbytes_wr;
                  linux_tcpip->wr_buf += nbytes_wr;
                }
              }
            }
          }
        }
      }
    }
    break;
    case mxifa_ei_linux_tcpip_cl:
    {
      if(!pchdatas->opened)break;
      mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
       (mxifa_linux_tcpip_cl_t *)pchdatas->ch_spec;
      mxifa_reconect_cl_socket(pchdatas);
      if(!linux_tcpip_cl->sock_open)
      {
        mxifa_open_cl_socket(pchdatas);
      }
      else
      {
        //puts("1309 Client test point 2");
        linux_tcpip_cl->read_fds = linux_tcpip_cl->master;
        linux_tcpip_cl->write_fds = linux_tcpip_cl->master;
        FD_SET(linux_tcpip_cl->socket_fd, &(linux_tcpip_cl->read_fds));
        FD_SET(linux_tcpip_cl->socket_fd, &(linux_tcpip_cl->write_fds));
        int CL_SE = select(linux_tcpip_cl->socket_fd+1,
                      &(linux_tcpip_cl->read_fds),
                      &(linux_tcpip_cl->write_fds),NULL,
                      &(linux_tcpip_cl->tv_zero));
        //puts("1318 Client test point 2.5");
        if(CL_SE == -1)
          perror("select");
        //puts("1321 Client test point 2.75");
        if(CL_SE > 0)
        {
        //  puts("1324 Client test point 3");
        int wer = FD_ISSET(linux_tcpip_cl->socket_fd,
              &(linux_tcpip_cl->read_fds));
        FD_SET(linux_tcpip_cl->socket_fd,
              &(linux_tcpip_cl->read_fds));
        //OUTDBG(cout"1329 CLIENT - test socket %d\n", wer);
        //puts("1330 Client test point 4");
        //OUTDBG(cout"1331 Socket fd: %d\n",linux_tcpip_cl->socket_fd);
        if(wer)
        {
          if(linux_tcpip_cl->can_read)
          {
            // OUTDBG(cout"1336 CLIENT - read data\n");
            mxifa_read_cl_socket(pchdatas);
          }
        }
        if(FD_ISSET(linux_tcpip_cl->socket_fd,
          &(linux_tcpip_cl->write_fds)))
        {
          if(linux_tcpip_cl->can_write)
          {
            //puts("1346 Client test point 5");
            //OUTDBG(cout"1347 CLIENT - send data\n");
            mxifa_write_cl_socket(pchdatas);
            //cout << "can_write = true" << endl;
          }
        }
       }
        //puts("1352 Client test point end!");
      }
    }
    break;
    default : {
    }
  }
 // puts("1357 Dop test point 0");
  if (MXIFA_CHD_SIZE > 2)
  {
//    OUTDBG(cout"1360 MXIFA_CHD_SIZE  = %d\n",(int)MXIFA_CHD_SIZE);
    for(unsigned int i = 1; i < MXIFA_CHD_SIZE;i++)
    {
      channel_cur++;
      mxifa_chdata_cur++;
      if (channel_cur > MXIFA_CHD_SIZE - 1)
      {
        channel_cur = 1;
        mxifa_chdata_cur = mxifa_chdata_begin;
      }
      irs_bool known_ch =
        (mxifa_chdata_cur->enum_iface != mxifa_ei_unknown);
      irs_bool opened_ch =  mxifa_chdata_cur->opened;
      if(known_ch && opened_ch) break;
    }
  //  OUTDBG(cout"1375 Current channel - %d\n", channel_cur);
  }
//  puts("1377 Dop test point end of tick()");
}
// Чтение типа канала
mxifa_ei_t mxifa_get_channel_type(void *pchdata)
{
  if (pchdata == IRS_NULL) return mxifa_ei_unknown;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  return pchdatas->enum_iface;
}
// Установка конфигурации канала
void mxifa_set_config(void *pchdata, void *config)
{
  if (pchdata == IRS_NULL) return;
  if (config == IRS_NULL) return;
  //mxip_t ip, mxip_t mask
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_linux_tcpip:
    {
      mxifa_linux_tcpip_t *linux_tcpip =
            (mxifa_linux_tcpip_t *)pchdatas->ch_spec;
      mxifa_linux_tcp_ip_cfg *cfg = (mxifa_linux_tcp_ip_cfg *)config;
      irs_bool is_noteq = irs_false;
      if(linux_tcpip->local_addr.sin_port != htons(cfg->local_port))
      {
        mxifa_close_se_socket(pchdatas);
        is_noteq = irs_true;
      }
      puts("1405 SERVER - changing configuration: OK");
      //fcntl(linux_tcpip->socket_fd,F_SETFL, O_NONBLOCK);
      linux_tcpip->local_addr.sin_family = AF_INET;
      linux_tcpip->local_addr.sin_port = htons(cfg->local_port);
      linux_tcpip->local_addr.sin_addr.s_addr = INADDR_ANY;
      memset(&(linux_tcpip->local_addr.sin_zero),'\0',8);

      linux_tcpip->dest_addr.sin_family = AF_INET;
      linux_tcpip->dest_addr.sin_port = htons(cfg->dest_port);
      linux_tcpip->dest_addr.sin_addr.s_addr =
        *(unsigned long *)&(cfg->dest_ip);
      memset(&(linux_tcpip->dest_addr.sin_zero),'\0',8);
      if(is_noteq == irs_true )
      {
        mxifa_open_se_socket(pchdatas);
        //is_noteq == irs_false;
      }
    } break;
    case mxifa_ei_linux_tcpip_cl:
    {
      mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
            (mxifa_linux_tcpip_cl_t *)pchdatas->ch_spec;
      mxifa_linux_tcp_ip_cl_cfg *cfg_cl =
       (mxifa_linux_tcp_ip_cl_cfg *)config;
      irs_bool is_noteq = irs_false;
      if(linux_tcpip_cl->dest_addr.sin_port != htons(cfg_cl->dest_port))
      {
        is_noteq = irs_true;
        if(linux_tcpip_cl->sock_open)
        {
          close(linux_tcpip_cl->socket_fd);
          linux_tcpip_cl->sock_open = irs_false;
        }
        //OUTDBG(cout"1438 CLIENT - close socket: %d\n",
          //linux_tcpip_cl->socket_fd);
      }
      FD_ZERO(&(linux_tcpip_cl->master));
      FD_ZERO(&(linux_tcpip_cl->read_fds));
      FD_ZERO(&(linux_tcpip_cl->write_fds));
      /*linux_tcpip_cl->socket_fd = socket(PF_INET,SOCK_STREAM,0);
      if(linux_tcpip_cl->socket_fd == -1)
        OUTDBG(cout"1446 CLIENT - Can't open socket!\n");
      else
      {
        puts("1449 CLIENT - Changing configuration: OK");
        OUTDBG(cout"1450 CLIENT - Get new file descriptor: %d\n",
          linux_tcpip_cl->socket_fd);
      }*/
      //puts("1453 CLIENT - Changing configuration: OK");
      linux_tcpip_cl->local_addr.sin_family = AF_INET;
      linux_tcpip_cl->local_addr.sin_port = htons(cfg_cl->local_port);
      linux_tcpip_cl->local_addr.sin_addr.s_addr = INADDR_ANY;
      memset(&(linux_tcpip_cl->local_addr.sin_zero),'\0',8);

      linux_tcpip_cl->dest_addr.sin_family = AF_INET;
      linux_tcpip_cl->dest_addr.sin_port = htons(cfg_cl->dest_port);
      linux_tcpip_cl->dest_addr.sin_addr.s_addr =
        *(unsigned long *)&(cfg_cl->dest_ip);
      memset(&(linux_tcpip_cl->dest_addr.sin_zero),'\0',8);


      linux_tcpip_cl->dest_port_cur = cfg_cl->dest_port;
      linux_tcpip_cl->dest_ip_cur = cfg_cl->dest_ip;

      linux_tcpip_cl->dest_port_def = cfg_cl->dest_port;
      linux_tcpip_cl->dest_ip_def = cfg_cl->dest_ip;


      //socklen_t addrlen  = sizeof(linux_tcpip_cl->dest_addr);

      if(is_noteq)
      {
        mxifa_open_cl_socket(pchdatas);
        is_noteq = irs_false;
      }
    } break;
    default : {
    }
  }
}
// Чтение конфигурации канала
void mxifa_get_config(void *pchdata, void *config)
{
  if (pchdata == IRS_NULL) return;
  if (config == IRS_NULL) return;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface) {
    case mxifa_ei_linux_tcpip:
    {
      mxifa_linux_tcp_ip_cfg *cfg = (mxifa_linux_tcp_ip_cfg *)config;
      mxifa_linux_tcpip_t *linux_tcpip =
       (mxifa_linux_tcpip_t *)pchdatas->ch_spec;
      cfg->dest_ip =
        *(mxip_t *)&(linux_tcpip->dest_addr.sin_addr.s_addr);
      cfg->local_port = ntohs(linux_tcpip->local_addr.sin_port);
      cfg->dest_port = ntohs(linux_tcpip->dest_addr.sin_port );
    } break;
    case mxifa_ei_linux_tcpip_cl:
    {
      mxifa_linux_tcp_ip_cl_cfg *cfg_cl =
        (mxifa_linux_tcp_ip_cl_cfg *)config;
      mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
       (mxifa_linux_tcpip_cl_t *)pchdatas->ch_spec;
      cfg_cl->dest_ip =
        *(mxip_t *)&(linux_tcpip_cl->dest_addr.sin_addr.s_addr);
      cfg_cl->local_port = ntohs(linux_tcpip_cl->local_addr.sin_port);
      cfg_cl->dest_port = ntohs(linux_tcpip_cl->dest_addr.sin_port );
    } break;
    default : {
    }
  }
}

void *mxifa_open_ex(mxifa_ch_t channel,void *config,
                                            irs_bool is_broadcast)
{
  if ((channel < 1) || (channel >= MXIFA_CHD_SIZE)) return IRS_NULL;
  mxifa_chdata_t *cur_ch = &mxifa_chdata[channel];
  switch (cur_ch->enum_iface)
  {
    case mxifa_ei_linux_tcpip:
    {
      mxifa_linux_tcpip_t *linux_tcpip =
        (mxifa_linux_tcpip_t *)cur_ch->ch_spec;
      mxifa_linux_tcp_ip_cfg *cfg = (mxifa_linux_tcp_ip_cfg *)config;
      const mxip_t *dest_ip = &linux_tcpip->dest_ip;
      linux_tcpip->is_broadcast = is_broadcast;
      if (is_broadcast)
      {
        dest_ip = &broadcast_ip;
      }
      close(linux_tcpip->socket_fd);
      linux_tcpip->local_addr.sin_family = AF_INET;
      linux_tcpip->local_addr.sin_port = htons(cfg->local_port);
      linux_tcpip->local_addr.sin_addr.s_addr = INADDR_ANY;
      memset(&(linux_tcpip->local_addr.sin_zero),'\0',8);
      linux_tcpip->dest_addr.sin_family = AF_INET;
      linux_tcpip->dest_addr.sin_port = htons(cfg->dest_port);
      memset(&(linux_tcpip->dest_addr.sin_zero),'\0',8);
      linux_tcpip->dest_addr.sin_addr.s_addr =
        *(unsigned long *)&(cfg->dest_ip);
      mxifa_open_se_socket(cur_ch);
      cur_ch->opened = irs_true;
    } break;
    case mxifa_ei_linux_tcpip_cl:
    {
      mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
        (mxifa_linux_tcpip_cl_t *)cur_ch->ch_spec;
      mxifa_linux_tcp_ip_cl_cfg *cfg_cl =
        (mxifa_linux_tcp_ip_cl_cfg *)config;
      const mxip_t *dest_ip = &linux_tcpip_cl->dest_ip;
      linux_tcpip_cl->is_broadcast = is_broadcast;
      if (is_broadcast)
      {
        dest_ip = &broadcast_ip;
      }
      close(linux_tcpip_cl->socket_fd);
      linux_tcpip_cl->sock_open = irs_false;
      linux_tcpip_cl->local_addr.sin_family = AF_INET;
      linux_tcpip_cl->local_addr.sin_port = htons(cfg_cl->local_port);
      linux_tcpip_cl->local_addr.sin_addr.s_addr = INADDR_ANY;
      memset(&(linux_tcpip_cl->local_addr.sin_zero),'\0',8);
      linux_tcpip_cl->dest_addr.sin_family = AF_INET;
      linux_tcpip_cl->dest_addr.sin_port = htons(cfg_cl->dest_port);
      memset(&(linux_tcpip_cl->dest_addr.sin_zero),'\0',8);
      linux_tcpip_cl->dest_addr.sin_addr.s_addr =
        *(unsigned long *)&(cfg_cl->dest_ip);
      mxifa_open_cl_socket(cur_ch);
    } break;
    default : {
    }
  }
  return cur_ch;
}
mxifa_sz_t mxifa_fast_read(void *pchdata,
      mxifa_dest_t *dest, irs_u8 *buf,mxifa_sz_t size)
{
     int length;
  if (pchdata == IRS_NULL) return irs_false;
  if (buf == IRS_NULL) return irs_false;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  switch (pchdatas->enum_iface)
  {
    case mxifa_ei_linux_tcpip:
    {
      mxifa_linux_tcpip_t *linux_tcpip =
        (mxifa_linux_tcpip_t *)pchdatas->ch_spec;
      int status = select(linux_tcpip->maxfd + 1, &(linux_tcpip->read_fds),
        NULL,NULL,&(linux_tcpip->tv_zero));
      if(status <= 0)
        return 0;
      else
      {
        bool port_null =
          dest->mxifa_linux_dest.port ==
          ntohs(linux_tcpip->null_addr.sin_port);
        unsigned long var_ip = linux_tcpip->null_addr.sin_addr.s_addr;
        bool ip_null = dest->mxifa_linux_dest.ip ==
            *(mxip_t *)&(var_ip);
        if (port_null && ip_null)
        {
          linux_tcpip->can_rany = irs_true;
        }
        else
        {
          linux_tcpip->can_rany = irs_false;
        }
        linux_tcpip->first_read = irs_true;
        mxifa_find_read_sock();
        if(linux_tcpip->sock_finded)
        {
          length = recv((*(linux_tcpip->r_it)).get_hfd(),
            buf,size,0);
          if(length <= 0)
          {
            if(length == 0)
            {
              #ifdef NOP
              OUTDBG(cout"1610 SERVER - Recv: socket %d hung up\n",
                (*(linux_tcpip->r_it)).get_hfd());
              #endif //NOP
              close((*(linux_tcpip->r_it)).get_hfd());
              FD_CLR((*(linux_tcpip->r_it)).get_hfd(),
                &(linux_tcpip->master));
              FD_CLR((*(linux_tcpip->r_it)).get_hfd(),
                &(linux_tcpip->read_fds));
              FD_CLR((*(linux_tcpip->r_it)).get_hfd(),
                &(linux_tcpip->write_fds));
              linux_tcpip->can_read = irs_false;
              linux_tcpip->first_read = irs_true;
              mxifa_remove_sock(linux_tcpip->r_it);
              return 0;
            }
          }
          else
          {
            //OUTDBG(cout"1645 SERVER - Get message\n");
          }
        }
      }
    }
    break;
    case mxifa_ei_linux_tcpip_cl:
    {
      mxifa_linux_tcpip_cl_t *linux_tcpip_cl =
        (mxifa_linux_tcpip_cl_t *)pchdatas->ch_spec;
      if(linux_tcpip_cl->sock_open == irs_false)
      {
        //mxifa_open_cl_socket(pchdatas);
        return IRS_NULL;
      }
      else
      {
        int res = select(linux_tcpip_cl->socket_fd + 1,
          &(linux_tcpip_cl->read_fds),NULL,NULL,&(linux_tcpip_cl->tv_zero));
        if(res == -1)
        {
          perror("select");
          return IRS_NULL;
        }
        if(res > 0)
        {
          if(FD_ISSET(linux_tcpip_cl->socket_fd,
            &(linux_tcpip_cl->read_fds)))
          {
            length = recv(linux_tcpip_cl->socket_fd,
              buf,size,0);
            if(length <= 0)
            {
              if(length == 0)
              {
                //OUTDBG(cout"1707 CLIENT - Socket %d hung up when fast reading\n",
                  //linux_tcpip_cl->socket_fd);
                return IRS_NULL;
              }
              else
              {
                perror("recv");
              }
              close(linux_tcpip_cl->socket_fd);
              linux_tcpip_cl->sock_open = irs_false;
              FD_CLR(linux_tcpip_cl->socket_fd,
              &(linux_tcpip_cl->master));
              FD_CLR(linux_tcpip_cl->socket_fd,
              &(linux_tcpip_cl->read_fds));
              FD_CLR(linux_tcpip_cl->socket_fd,
              &(linux_tcpip_cl->write_fds));
              return IRS_NULL;
            }
            else
            {
              #ifdef NOP
              OUTDBG(cout"1727 CLIENT - Get message\n");
              OUTDBG(cout"1728 CLIENT - Number recivied bytes %d\n", length);
              OUTDBG(cout"1729 CLIENT - File descriptor %d\n", linux_tcpip_cl->socket_fd);
              OUTDBG(cout"1730 CLIENT - bufer size: %d\n", (int)linux_tcpip_cl->rd_size);
            // OUTDBG(cout"1733 CLIENT - bufer %s\n", linux_tcpip_cl->rd_buf);
              linux_tcpip_cl->rd_buf += length;
              linux_tcpip_cl->rd_size -= length;
              if(linux_tcpip_cl->rd_size <= 0)
              {
              // OUTDBG(cout"1739 CLIENT - Get all parts of the message\n");
              }
              #endif
            }
          }
        }
        else
          return IRS_NULL;
      }
    }
    break;
    default : {
    }
  }
  return length;
}
mxifa_ei_t mxifa_get_channel_type_ex(mxifa_ch_t channel)
{
  //if ((channel < 1) || (channel > MXIFA_CHD_SIZE)) return IRS_NULL;
  mxifa_chdata_t *cur_ch = &mxifa_chdata[channel];
  return cur_ch->enum_iface;

}
//---------------------------------------------------------------------------
// Специальная пустая функция
static void mxifa_nulf(void *pchdata)
{
  if (pchdata == IRS_NULL) return;
}
#ifdef NOP
// Выполнение операции чтения для avr128_ether
static void avr128_ether_read(void *pchdata)
{
  if (pchdata == IRS_NULL) return;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  mxifa_avr128_ether_t *avr128_ether =
    (mxifa_avr128_ether_t *)pchdatas->ch_spec;
  if (avr128_ether->read_mode == mxifa_rm_read) {
    if (avr128_ether->buf_size) {
      irs_u16 size = avr128_ether->buf_size;
      if (size > avr128_ether->rd_size) size = avr128_ether->rd_size;
      memcpy((void *)avr128_ether->rd_buf, (void *)(user_rx_buf +
        avr128_ether->buf_begin), size);
      avr128_ether->buf_size -= size;
      avr128_ether->buf_begin += size;
      avr128_ether->read_mode = mxifa_rm_free;
      irs_bool read_mode_free = (avr128_ether->read_mode == mxifa_rm_free);
      irs_bool write_mode_free = (avr128_ether->write_mode == mxifa_wm_free);
      pchdatas->mode_free = (read_mode_free && write_mode_free);
      if (!avr128_ether->buf_size) ReadUDP_end();
    } else {
      irs_u8 *p_ip = IRS_NULL;
      irs_u16 *p_port = IRS_NULL;
      if (avr128_ether->rd_dest) {
        p_ip = (irs_u8 *)&avr128_ether->rd_dest->avr128_ether.ip;
        p_port = &avr128_ether->rd_dest->avr128_ether.port;
      }
      irs_u16 bytes_recieved =  ReadUDP_begin(p_ip, p_port);
      if (bytes_recieved < MXIFA_SZ_T_MAX) {
        avr128_ether->buf_size = bytes_recieved;
      } else {
        avr128_ether->buf_size = MXIFA_SZ_T_MAX;
      }
      avr128_ether->buf_begin = 0;
    }
  }
  pchdatas->tick = avr128_ether_write;
}


// Выполнение операции записи для avr128_ether
static void avr128_ether_write(void *pchdata)
{
  if (pchdata == IRS_NULL) return;
  mxifa_chdata_t *pchdatas = (mxifa_chdata_t *)pchdata;
  mxifa_avr128_ether_t *avr128_ether =
    (mxifa_avr128_ether_t *)pchdatas->ch_spec;
  if (avr128_ether->write_mode == mxifa_wm_write) {
    if (WriteUDP_begin()) {
      mxifa_sz_t size = avr128_ether->wr_size;
      #if (MXIFA_SZ_T_MAX > UDP_BUFFER_SIZE_TX)
      if (size > UDP_BUFFER_SIZE_TX) size = UDP_BUFFER_SIZE_TX;
      #endif //(MXIFA_SZ_T_MAX > UDP_BUFFER_SIZE_TX)
      memcpy((void *)user_tx_buf, (void *)avr128_ether->wr_buf, size);
      avr128_ether->write_mode = mxifa_wm_free;
      irs_bool read_mode_free = (avr128_ether->read_mode == mxifa_rm_free);
      irs_bool write_mode_free = (avr128_ether->write_mode == mxifa_wm_free);
      pchdatas->mode_free = (read_mode_free && write_mode_free);
      if (avr128_ether->wr_dest) {
        WriteUDP_end((irs_u8 *)&avr128_ether->wr_dest->avr128_ether.ip,
          &avr128_ether->wr_dest->avr128_ether.port, size);
      } else {
        WriteUDP_end(IRS_NULL, IRS_NULL, size);
      }
    }
  }
  pchdatas->tick = avr128_ether_read;
}
#endif //NOP
