#include <stdio.h>
#include <fcntl.h>
#include <sys/termios.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <irsdefs.h>
#include <iostream.h>

  int termin = 0;
  int havec = 0, h = 0;
  struct termios oldterm;
  struct termios newterm;


void kbd_init()
{

  termin=open("/dev/tty",O_RDONLY);

  tcgetattr(termin,&oldterm);
  tcgetattr(termin,&newterm);

  newterm.c_lflag=0;
  newterm.c_iflag&=~(ICRNL|IGNCR|INLCR|IXON|IXOFF);
  //newterm.c_oflag=0;
  newterm.c_cc[VMIN]=1;
  newterm.c_cc[VTIME]=0;
  tcsetattr(termin,TCSADRAIN,&newterm);

  fcntl(termin, F_SETFL, O_NDELAY);

}

void kbd_deinit()
{
  tcsetattr(termin,TCSADRAIN,&oldterm);
  fcntl(termin, F_SETFL, 0);

}
irs_u8 key_cods()
{

    //while(havec!=134518555)
    //{
    h = read(termin,&havec,1);
    //cout << h;
    if(h==1)
    {
      //printf("[%08x]\n", havec);
      //printf("OK\n\r");
      //fflush(stdout);
      //printf("LOW byte: %d\n",IRS_LOBYTE(havec));
      return IRS_LOBYTE(havec);
    }
    else
    {
      return 0;
    }
    //printf("1\n");
    //}

    //tcsetattr(termin,TCSADRAIN,&oldterm);
    //fcntl(termin, F_SETFL, 0);

    //return 0;
}
