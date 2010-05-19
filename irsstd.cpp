// Стандартаная библиотека ИРС
// Дата: 19.05.2010
// Ранняя дата: 25.11.2007

// Номер файла
#define IRSSTDCPP_IDX 15

#include <irsdefs.h>

#ifdef __BORLANDC__
#include <vcl.h>
#pragma hdrstop
#endif //__BORLANDC__

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <irserror.h>

#include <irsstd.h>
//#include <irscpp.h>

#include <irsfinal.h>

// Конструктор по умолчанию
mxconsole_t::mxconsole_t()
{
}
// Конструктор
mxconsole_t::mxconsole_t(mxcondrv_t *a_condrv):
  f_condrv(a_condrv)
{
}
// Деструктор
mxconsole_t::~mxconsole_t()
{
}
// Вывод отладочной информации по типу puts
void mxconsole_t::puts(const char *a_msg)
{
  const irs_u8 *msg = (const irs_u8 *)a_msg;
  if (!f_condrv) return;
  if (!msg) return;
  irs_i32 len = strlen((char *)msg);
  irs_u8 *msg_crlf = new irs_u8[len + 2];
  if (!msg_crlf) return;
  ::strcpy((char *)msg_crlf, (char *)msg);
  msg_crlf[len] = '\n';
  msg_crlf[len + 1] = 0;
  f_condrv->out((char *)msg_crlf);
  delete []msg_crlf;
}
// Вывод отладочной информации по типу printf
void mxconsole_t::printf(const char *a_format, ...)
{
  const irs_u8 *format = (const irs_u8 *)a_format;
  if (!f_condrv) return;
  if (!format) return;
  irs_u8 *msgbuf;
  irs_i32 len;
  va_list arglist;
  va_start(arglist, a_format);
  len = vsnprintf(NULL, 0, (char *)format, arglist);
  len++;
  va_end(arglist);
  msgbuf = new irs_u8[len];
  if (!msgbuf) return;
  va_start(arglist, a_format);
  vsnprintf((char *)msgbuf, len, (char *)format, arglist);
  msgbuf[len - 1] = 0;
  va_end(arglist);
  f_condrv->out((char *)msgbuf);
  delete []msgbuf;
}

// Класс для приложения
mxapplication_t::mxapplication_t():
  first_exec(irs_true),
  stop_exec(irs_false),
  f_init(IRS_NULL),
  f_deinit(IRS_NULL),
  f_tick(IRS_NULL),
  f_stop(IRS_NULL),
  f_stopped(IRS_NULL)
{
}
mxapplication_t::~mxapplication_t()
{
}
irs_bool mxapplication_t::run(irs_bool exec)
{
  if (exec)
  if (first_exec) {
    first_exec = irs_false;
    init();
  }
  if (!first_exec) {
    if (!exec && !stop_exec) {
      stop_exec = irs_true;
      stop();
    }
    tick();
  }
  if (stopped() && !first_exec) {
    deinit();
    first_exec = irs_true;
    stop_exec = irs_false;
    return irs_false;
  } else {
    return irs_true;
  }
}
void mxapplication_t::set_init_event(void (*a_init)())
{
  f_init = a_init;
}
void mxapplication_t::set_denit_event(void (*a_deinit)())
{
  f_deinit = a_deinit;
}
void mxapplication_t::set_tick_event(void (*a_tick)())
{
  f_tick = a_tick;
}
void mxapplication_t::set_stop_event(void (*a_stop)())
{
  f_stop = a_stop;
}
void mxapplication_t::set_stopped_event(irs_bool (*a_stopped)())
{
  f_stopped = a_stopped;
}
void mxapplication_t::init()
{
  if (f_init) f_init();
}
void mxapplication_t::deinit()
{
  if (f_deinit) f_deinit();
}
void mxapplication_t::tick()
{
  if (f_tick) f_tick();
}
void mxapplication_t::stop()
{
  if (f_stop) f_stop();
}
irs_bool mxapplication_t::stopped()
{
  if (f_stopped) return f_stopped();
  else return irs_true;
}

// Перераспределение памяти с сохранением данных
void *irs_renew(void *pointer, size_t old_size, size_t new_size)
{
  if (old_size == new_size) return pointer;

  void *new_pointer = IRS_NULL;
  if (new_size) {
    new_pointer =
      (void*)IRS_LIB_NEW_ASSERT(irs_u8 *[new_size], IRSSTDCPP_IDX);
    if (!new_pointer) return new_pointer;
  }
  if (pointer) {
    if (old_size && new_size) {
      memcpy(new_pointer, pointer, irs_min(old_size, new_size));
    }
    IRS_LIB_ARRAY_DELETE_ASSERT((irs_u8*&)pointer);
  }
  return new_pointer;
}
mxapplication_t *mxapplication = IRS_NULL;

void afloat_to_str(char *str, double N, size_t len, size_t accur)
{
  ostrstream strm(str, len + 1);
  strm << setiosflags(ios::fixed) << setw(len) << setprecision(accur) << N;
  strm << '\0';
  str[len] = '\0';
  //str[strm.pcount()] = 0;
}

#ifdef __ICCAVR__
// Карта преобразования номеров клавиш в коды
// для вертикального расположения клавиатуры
__flash irskey_t irskbd_table_vertical[] =
{
  irskey_none,      // 0
  irskey_0,         // 1
  irskey_1,         // 2
  irskey_2,         // 3
  irskey_3,         // 4
  irskey_4,         // 5
  irskey_5,         // 6
  irskey_6,         // 7
  irskey_7,         // 8
  irskey_8,         // 9
  irskey_9,         // 10
  irskey_point,     // 11
  irskey_enter,     // 12
  irskey_up,        // 13
  irskey_down,      // 14
  irskey_backspace, // 15
  irskey_escape     // 16
};

// Карта преобразования номеров клавиш в коды
// для горизонтального расположения клавиатуры
__flash irskey_t irskbd_table_horisontal[] =
{
  irskey_none,      // 0
  irskey_enter,     // 1
  irskey_backspace, // 2
  irskey_3,         // 3
  irskey_6,         // 4
  irskey_point,     // 5
  irskey_2,         // 6
  irskey_5,         // 7
  irskey_0,         // 8
  irskey_1,         // 9
  irskey_4,         // 10
  irskey_down,      // 11
  irskey_escape,    // 12
  irskey_8,         // 13
  irskey_9,         // 14
  irskey_up,        // 15
  irskey_7          // 16
};

// Карта преобразования номеров клавиш в коды
// для расположения клавиатуры для ГТЧ rev 0 (макетка edition)
__flash irskey_t irskbd_table_gtch_rev_0[] =
{
  irskey_none,      // 0
  irskey_none,      // 1
  irskey_none,      // 2
  irskey_escape,    // 3
  irskey_6,         // 4
  irskey_none,      // 5
  irskey_2,         // 6
  irskey_5,         // 7
  irskey_none,      // 8
  irskey_1,         // 9
  irskey_4,         // 10
  irskey_enter,     // 11
  irskey_up,        // 12
  irskey_8,         // 13
  irskey_9,         // 14
  irskey_down,      // 15
  irskey_7          // 16
};

// Карта преобразования номеров клавиш в коды
// для расположения клавиатуры для ГТЧ rev 1 (чемодан edition)
__flash irskey_t irskbd_table_gtch_rev_1[] =
{
  irskey_none,      // 0
  irskey_none,      // 1
  irskey_none,      // 2
  irskey_2,         // 3
  irskey_5,         // 4
  irskey_none,      // 5
  irskey_escape,    // 6
  irskey_6,         // 7
  irskey_none,      // 8
  irskey_enter,     // 9
  irskey_down,      // 10
  irskey_1,         // 11
  irskey_7,         // 12
  irskey_9,         // 13
  irskey_8,         // 14
  irskey_4,         // 15
  irskey_up         // 16
};

const irs_avr_port_map_t avr_port_map[] = {
  {&PORTA, &PINA, &DDRA},
  {&PORTB, &PINB, &DDRB},
  {&PORTC, &PINC, &DDRC},
  {&PORTD, &PIND, &DDRD},
  {&PORTE, &PINE, &DDRE},
  {&PORTF, &PINF, &DDRF},
  {&PORTG, &PING, &DDRG}
};
//#define avr_port_map_size IRS_ARRAYOFSIZE(avr_port_map);

//__flash irs_u8 lcd_table[] =

mxkey_drv_avr_t::mxkey_drv_avr_t(irskbd_t type, irskbd_map_t map_type,
  irs_avr_port_t a_port):
  m_high_set_bit(7),
  m_high_get_bit(3),
  mp_set(avr_port_map[a_port].set),
  mp_get(avr_port_map[a_port].get),
  mp_dir(avr_port_map[a_port].dir)
{
  switch (type) {
    case irskbd_high_pull_up: {
      m_high_set_bit = 3;
      m_high_get_bit = 7;
      (*mp_dir) = 0x0F;
    } break;

    default:
    case irskbd_low_pull_up: {
      m_high_set_bit = 7;
      m_high_get_bit = 3;
      (*mp_dir) = 0xF0;
    } break;
  }
  switch(map_type) {
    default:
    case irskbd_map_vertical: {
      mp_key_map = irskbd_table_vertical;
    } break;

    case irskbd_map_horisontal: {
      mp_key_map = irskbd_table_horisontal;
    } break;

    case irskbd_map_gtch_rev_1: {
      mp_key_map = irskbd_table_gtch_rev_1;
    } break;
  }
  (*mp_set) = (*mp_dir)^0xFF;
}
mxkey_drv_avr_t::~mxkey_drv_avr_t()
{
  (*mp_dir) = 0x00;
  (*mp_set) = 0x00;
}
irskey_t mxkey_drv_avr_t::operator()()
{
  reshor(0);
  sethor(1);
  sethor(2);
  sethor(3);
  if (!chkver(0)) return mp_key_map[8];
  if (!chkver(1)) return mp_key_map[9];
  if (!chkver(2)) return mp_key_map[10];
  if (!chkver(3)) return mp_key_map[16];
  sethor(0);
  reshor(1);
  sethor(2);
  sethor(3);
  if (!chkver(0)) return mp_key_map[5];
  if (!chkver(1)) return mp_key_map[6];
  if (!chkver(2)) return mp_key_map[7];
  if (!chkver(3)) return mp_key_map[13];
  sethor(0);
  sethor(1);
  reshor(2);
  sethor(3);
  if (!chkver(0)) return mp_key_map[2];
  if (!chkver(1)) return mp_key_map[3];
  if (!chkver(2)) return mp_key_map[4];
  if (!chkver(3)) return mp_key_map[14];
  sethor(0);
  sethor(1);
  sethor(2);
  reshor(3);
  if (!chkver(0)) return mp_key_map[1];
  if (!chkver(1)) return mp_key_map[11];
  if (!chkver(2)) return mp_key_map[15];
  if (!chkver(3)) return mp_key_map[12];

  return mp_key_map[0];
}

// Класс драйвера дисплея AVR

#define LCD_INIT_DELAY_MS 15
#define LCD_TICK_DELAY_US 100 //100
#define LCD_CYCLE_DELAY_US 40

//#define LCD_SIZE 80 //symbols
#define LINE0 0
#define LINE1 20
#define LINE2 40
#define LINE3 60
#define LINE0_ADR 0x00
#define LINE1_ADR 0x40
#define LINE2_ADR 0x14
#define LINE3_ADR 0x54
//#define LINE_LEN 20

#define IMAGE_ON 0x0C   //  Включение изображения, курсоров есть
#define IMAGE_OFF 0x08  //  Выключение изображения
#define BUS_8BIT 0x30   //  Инициализационная команда - выбор ширины шины
                        // данных = 8 бит
#define TWO_LINES 0x38  //  8 бит шина данных, режим отображения 2 строки,
                        // размер матрицы символа 5 х 8 точек
#define ADR_INC 0x06    //  Счетчик адреса инкрементируется
#define LCD_NULL 0      //  Ничо

#define lcd_delay_mcs(delay)\
{\
  set_to_cnt(lcd_timer, MCS_TO_CNT(delay));\
  while (!test_to_cnt(lcd_timer));\
}
#define lcd_delay_ms(delay)\
{\
  set_to_cnt(lcd_timer, MS_TO_CNT(delay));\
  while (!test_to_cnt(lcd_timer));\
}
#define lcd_init_command(command)\
{\
  lcd_IR_enable();\
  set_to_cnt(lcd_timer, MS_TO_CNT(LCD_INIT_DELAY_MS));\
  while (!test_to_cnt(lcd_timer));\
  lcd_enable();\
  set_to_cnt(lcd_timer, MS_TO_CNT(LCD_INIT_DELAY_MS));\
  while (!test_to_cnt(lcd_timer));\
  lcd_out(command);\
  set_to_cnt(lcd_timer, MS_TO_CNT(LCD_INIT_DELAY_MS));\
  while (!test_to_cnt(lcd_timer));\
  lcd_disable();\
  set_to_cnt(lcd_timer, MS_TO_CNT(LCD_INIT_DELAY_MS));\
  while (!test_to_cnt(lcd_timer));\
  lcd_out(LCD_NULL);\
}

//  Перекодировка символов Windows-1251 в коды контроллера Epson на базе
// HD44780 с разными прибамбасами
__flash irs_u8 lcd_table[] =
{
  0x20, // '~':0
  0xFF, // '~':1 //Курсор квадратом
  0x20, // '~':2
  0x20, // '~':3
  0x20, // '~':4
  0x20, // '~':5
  0x20, // '~':6
  0x20, // '~':7
  0x20, // '~':8
  0x20, // '~':9
  0x20, // '~':10
  0x20, // '~':11
  0x20, // '~':12
  0x20, // '~':13
  0x20, // '~':14
  0x20, // '~':15
  0x20, // '~':16
  0x20, // '~':17
  0x20, // '~':18
  0x20, // '~':19
  0x20, // '~':20
  0x20, // '~':21
  0x20, // '~':22
  0x20, // '~':23
  0x20, // '~':24
  0x20, // '~':25
  0x20, // '~':26
  0x20, // '~':27
  0x20, // '~':28
  0x20, // '~':29
  0x20, // '~':30
  0x20, // '~':31
  0x20, // ' ':32
  0x21, // '!':33
  0x22, // '"':34
  0xEF,//0x23, // '#':35-------led on
  0xEE, // '$':36--------------led off
  0x25, // '%':37
  0x26, // '&':38
  0x27, // ''':39
  0x28, // '(':40
  0x29, // ')':41
  0x2A, // '*':42
  0x2B, // '+':43
  0x2C, // ',':44
  0x2D, // '-':45
  0x2E, // '.':46
  0x2F, // '/':47
  0x30, // '0':48
  0x31, // '1':49
  0x32, // '2':50
  0x33, // '3':51
  0x34, // '4':52
  0x35, // '5':53
  0x36, // '6':54
  0x37, // '7':55
  0x38, // '8':56
  0x39, // '9':57
  0x3A, // ':':58
  0x3B, // ';':59
  0x3C, // '<':60
  0x3D, // '=':61
  0x3E, // '>':62
  0x3F, // '?':63
  0x40, // '@':64
  0x41, // 'A':65
  0x42, // 'B':66
  0x43, // 'C':67
  0x44, // 'D':68
  0x45, // 'E':69
  0x46, // 'F':70
  0x47, // 'G':71
  0x48, // 'H':72
  0x49, // 'I':73
  0x4A, // 'J':74
  0x4B, // 'K':75
  0x4C, // 'L':76
  0x4D, // 'M':77
  0x4E, // 'N':78
  0x4F, // 'O':79
  0x50, // 'P':80
  0x51, // 'Q':81
  0x52, // 'R':82
  0x53, // 'S':83
  0x54, // 'T':84
  0x55, // 'U':85
  0x56, // 'V':86
  0x57, // 'W':87
  0x58, // 'X':88
  0x59, // 'Y':89
  0x5A, // 'Z':90
  0x5B, // '[':91
  0x5C, // '\':92
  0x5D, // ']':93
  0xD9, // '^':94 стрелка вверх
  0x5F, // '_':95
  0x60, // '`':96
  0x61, // 'a':97
  0x62, // 'b':98
  0x63, // 'c':99
  0x64, // 'd':100
  0x65, // 'e':101
  0x66, // 'f':102
  0x67, // 'g':103
  0x68, // 'h':104
  0x69, // 'i':105
  0x6A, // 'j':106
  0x6B, // 'k':107
  0x6C, // 'l':108
  0x6D, // 'm':109
  0x6E, // 'n':110
  0x6F, // 'o':111
  0x70, // 'p':112
  0x71, // 'q':113
  0x72, // 'r':114
  0x73, // 's':115
  0x74, // 't':116
  0x75, // 'u':117
  0x76, // 'v':118
  0x77, // 'w':119
  0x78, // 'x':120
  0x79, // 'y':121
  0x7A, // 'z':122
  0x20, // '{':123
  0x20, // '|':124
  0x20, // '}':125
  0xDA, // '~':126 стрелка вниз
  0x20, // '':127
  0x20, // 'Ђ':128
  0x20, // 'Ѓ':129
  0x20, // '‚':130
  0x20, // 'ѓ':131
  0xCA, // '„':132
  0x20, // '…':133
  0x20, // '†':134
  0x20, // '‡':135
  0x7E, // '€':136 'enter'
  0xD4, // '‰':137
  0x20, // 'Љ':138
  0x20, // '‹':139
  0x20, // 'Њ':140
  0x20, // 'Ќ':141
  0x20, // 'Ћ':142
  0x20, // 'Џ':143
  0x20, // 'ђ':144
  0x27, // '‘':145
  0x27, // '’':146
  0xCB, // '“':147
  0xCB, // '”':148
  0xDF, // '•':149
  0x2D, // '–':150
  0x2D, // '—':151
  0x20, // '':152
  0x20, // '™':153
  0x20, // 'љ':154
  0x3E, // '›':155
  0x20, // 'њ':156
  0x20, // 'ќ':157
  0x20, // 'ћ':158
  0x20, // 'џ':159
  0x20, // ' ':160
  0x20, // 'Ў':161
  0x20, // 'ў':162
  0x4A, // 'Ј':163
  0x20, // '¤':164
  0x20, // 'Ґ':165
  0x20, // '¦':166
  0xFD, // '§':167
  0xA2, // 'Ё':168
  0x20, // '©':169
  0x20, // 'Є':170
  0xC8, // '«':171
  0x20, // '¬':172
  0x2D, // '­':173
  0x20, // '®':174
  0x20, // 'Ї':175
  0xEF, // '°':176
  0x20, // '±':177
  0x49, // 'І':178
  0x69, // 'і':179
  0xB4, // 'ґ':180
  0x75, // 'µ':181
  0xFE, // '¶':182
  0xDF, // '·':183
  0xB5, // 'ё':184
  0x23, // '№':185
  0x20, // 'є':186
  0xC9, // '»':187
  0x6A, // 'ј':188
  0x53, // 'Ѕ':189
  0x73, // 'ѕ':190
  0x69, // 'ї':191
  0x41, // 'А':192
  0xA0, // 'Б':193
  0x42, // 'В':194
  0xA1, // 'Г':195
  0xE0, // 'Д':196
  0x45, // 'Е':197
  0xA3, // 'Ж':198
  0x33, // 'З':199
  0xA5, // 'И':200
  0xA6, // 'Й':201
  0x4B, // 'К':202
  0xA7, // 'Л':203
  0x4D, // 'М':204
  0x48, // 'Н':205
  0x4F, // 'О':206
  0xA8, // 'П':207
  0x50, // 'Р':208
  0x43, // 'С':209
  0x54, // 'Т':210
  0xA9, // 'У':211
  0xAA, // 'Ф':212
  0x58, // 'Х':213
  0xE1, // 'Ц':214
  0xAB, // 'Ч':215
  0xAC, // 'Ш':216
  0xE2, // 'Щ':217
  0xAD, // 'Ъ':218
  0xAE, // 'Ы':219
  0xAD, // 'Ь':220 = 'Ъ'
  0xAF, // 'Э':221
  0xB0, // 'Ю':222
  0xB1, // 'Я':223
  0x61, // 'а':224
  0xB2, // 'б':225
  0xB3, // 'в':226
  0xB4, // 'г':227
  0xE3, // 'д':228
  0x65, // 'е':229
  0xB6, // 'ж':230
  0xB7, // 'з':231
  0xB8, // 'и':232
  0xB9, // 'й':233
  0xBA, // 'к':234
  0xBB, // 'л':235
  0xBC, // 'м':236
  0xBD, // 'н':237
  0x6F, // 'о':238
  0xBE, // 'п':239
  0x70, // 'р':240
  0x63, // 'с':241
  0xBF, // 'т':242
  0x79, // 'у':243
  0xE4, // 'ф':244
  0x78, // 'х':245
  0xE5, // 'ц':246
  0xC0, // 'ч':247
  0xC1, // 'ш':248
  0xE6, // 'щ':249
  0xC2, // 'ъ':250
  0xC3, // 'ы':251
  0xC4, // 'ь':252
  0xC5, // 'э':253
  0xC6, // 'ю':254
  0xC7  // 'я':255
};

mxdisplay_drv_avr_t::mxdisplay_drv_avr_t(
  irslcd_t a_type,
  irs_avr_port_t a_data_port,
  irs_avr_port_t a_control_port,
  irs_u8 a_control_pin
):
  mp_set_data(avr_port_map[a_data_port].set),
  mp_get_data(avr_port_map[a_data_port].get),
  mp_dir_data(avr_port_map[a_data_port].dir),
  mp_set_control(avr_port_map[a_control_port].set),
  mp_get_control(avr_port_map[a_control_port].get),
  mp_dir_control(avr_port_map[a_control_port].dir),
  m_LCD_E(a_control_pin + 0),
  m_LCD_RS(a_control_pin + 1),
  LINE_COUNT(4),
  LINE_LEN(20),
  LCD_SIZE(LINE_LEN*LINE_COUNT),
  lcd_status(LCD_BEGIN),
  lcd_ram(IRS_NULL),
  lcd_current_symbol(0),
  lcd_current_symbol_address(0),
  lcd_init_counter(0),
  lcd_timer(0),
  lcd_address_jump(irs_false)
{
  init_to_cnt();

  if (a_type == irslcd_4x20) {
    LINE_COUNT = 4;
    LINE_LEN = 20;
    LCD_SIZE = LINE_LEN*LINE_COUNT;
  }

  lcd_ram = new irs_u8[LCD_SIZE];
  memset(lcd_ram, ' ', LCD_SIZE);

  //IRS_PAUSE(TIME_TO_CNT(1, 1));

  set_to_cnt(lcd_timer, MS_TO_CNT(200));
  while (!test_to_cnt(lcd_timer));

  //  Инициализация портов ввода/вывода
#ifndef DBGREG
  (*mp_dir_data) = 0xFF;  //  Шина данных LCD
  (*mp_set_data) = 0x00;
  (*mp_dir_control) |= ((1 << m_LCD_RS)|(1 << m_LCD_E)); //  Линии E и RS LCD
  (*mp_set_control) &= (((1 << m_LCD_RS)|(1 << m_LCD_E))^0xFF);
#else //DBGREG
  DDRA = 0xFF;  //  Шина данных LCD
  PORTA = 0x00;
  DDRE |= ((1 << m_LCD_RS)|(1 << m_LCD_E)); //  Линии E и RS LCD
  PORTE &= (((1 << m_LCD_RS)|(1 << m_LCD_E))^0xFF);
#endif //DBGREG

  for (irs_u8 i = 0; i < 3; i++)
  {
    //  Выдержка
    lcd_delay_ms(LCD_INIT_DELAY_MS);
    //  Команда
    lcd_init_command(BUS_8BIT);
    //  И так - 3 раза
  }
  lcd_delay_ms(LCD_INIT_DELAY_MS);
  lcd_init_command(TWO_LINES);
  lcd_delay_ms(LCD_INIT_DELAY_MS);
  lcd_init_command(IMAGE_ON);
  lcd_delay_ms(LCD_INIT_DELAY_MS);
  lcd_init_command(ADR_INC);
}
mxdisplay_drv_avr_t::~mxdisplay_drv_avr_t()
{
  //  Гашение индикатора
  lcd_delay_mcs(LCD_TICK_DELAY_US);
  lcd_init_command(IMAGE_OFF);
  //  Деинициализация портов ввода/вывода
  //  Шина данных LCD
  (*mp_dir_data) = 0x0;
  (*mp_set_data) = 0x00;
  //  Линии E и RS LCD
  (*mp_dir_control) &= (((1 << m_LCD_RS)&(1 << m_LCD_E))^0xFF);
  (*mp_set_control) &= (((1 << m_LCD_RS)&(1 << m_LCD_E))^0xFF);

  delete []lcd_ram;

  deinit_to_cnt();
}
mxdisp_pos_t mxdisplay_drv_avr_t::get_height()
{
  return LINE_COUNT;
}
mxdisp_pos_t mxdisplay_drv_avr_t::get_width()
{
  return LINE_LEN;
}
void mxdisplay_drv_avr_t::outtextpos(mxdisp_pos_t a_left, mxdisp_pos_t a_top,
  const char *text)
{
  irs_u8 begin = (irs_u8)(a_top*LINE_LEN) + a_left;
  irs_u8 delta = LCD_SIZE - begin;
  for (irs_u8 i = 0; (i < strlen(text))&&(i < delta); i++) {
    lcd_ram[begin + i] = lcd_table[(irs_u8)text[i]];
  }
}
void mxdisplay_drv_avr_t::tick()
{
  switch (lcd_status)
  {
  case LCD_BEGIN:
    {
      set_to_cnt(lcd_timer, MCS_TO_CNT(LCD_TICK_DELAY_US));
      lcd_status = LCD_RS;
      break;
    }
  case LCD_RS:
    {
      if (test_to_cnt(lcd_timer))
      {
        set_to_cnt(lcd_timer, MCS_TO_CNT(LCD_TICK_DELAY_US));
        if (lcd_current_symbol > LCD_SIZE-1) lcd_current_symbol = 0;
        if (lcd_address_jump == irs_false)
        {
          switch (lcd_current_symbol)
          {
          case LINE0:
            {
              lcd_current_symbol_address = LINE0_ADR;
              lcd_status = LCD_ADDRESS_OUT;
              lcd_address_jump = irs_true;
              lcd_IR_enable();
              break;
            }
          case /*(irs_u8)(LCD_SIZE/2)*/LINE1:
            {
              //  Переход на вторую строку. Адрес записывается в индикатор
              // только при переходе на новую строку
              lcd_current_symbol_address = LINE1_ADR;
              lcd_status = LCD_ADDRESS_OUT;
              lcd_address_jump = irs_true;
              lcd_IR_enable();
              break;
            }
          case LINE2:
            {
              //  Переход на третью строку. Адрес записывается в индикатор
              // только при переходе на новую строку
              lcd_current_symbol_address = LINE2_ADR;
              lcd_status = LCD_ADDRESS_OUT;
              lcd_address_jump = irs_true;
              lcd_IR_enable();
              break;
            }
          case LINE3:
            {
              //  Переход на четвертую строку. Адрес записывается в индикатор
              // только при переходе на новую строку
              lcd_current_symbol_address = LINE3_ADR;
              lcd_status = LCD_ADDRESS_OUT;
              lcd_address_jump = irs_true;
              lcd_IR_enable();
              break;
            }
          default:
            {
              lcd_status = LCD_DATA_OUT;
              lcd_DR_enable();
              break;
            }
          }
        }
        else
        {
          lcd_status = LCD_DATA_OUT;
          lcd_DR_enable();
          lcd_address_jump = irs_false;
        }
      }
      break;
    }
  case LCD_DATA_OUT:
    {
      if (test_to_cnt(lcd_timer))
      {
        lcd_status = LCD_E_RISE;
        lcd_out(lcd_ram[lcd_current_symbol]);
        lcd_current_symbol++;
        set_to_cnt(lcd_timer, MCS_TO_CNT(LCD_TICK_DELAY_US));
      }
      break;
    }
  case LCD_ADDRESS_OUT:
    {
      if (test_to_cnt(lcd_timer))
      {
        set_to_cnt(lcd_timer, MCS_TO_CNT(LCD_TICK_DELAY_US));
        lcd_status = LCD_E_RISE;
        lcd_out(lcd_current_symbol_address|0x80);
      }
      break;
    }
  case LCD_E_RISE:
    {
      if (test_to_cnt(lcd_timer))
      {
        set_to_cnt(lcd_timer, MCS_TO_CNT(LCD_TICK_DELAY_US));
        lcd_status = LCD_E_FALL;
        lcd_enable();
      }
      break;
    }
  case LCD_E_FALL:
    {
      if (test_to_cnt(lcd_timer))
      {
        set_to_cnt(lcd_timer, MCS_TO_CNT(LCD_CYCLE_DELAY_US));
        lcd_status = LCD_RS;
        lcd_disable();
      }
      break;
    }
  case LCD_FREE:
    {
      lcd_disable();
      lcd_out(0);
      break;
    }
  }
}
#endif //__ICCAVR__

#ifdef __BORLANDC__
// Вспомогательный класс драйвера клавиатуры С++ Builder
class mxkdb_keystate_t: TObject
{
  Word f_key;
  void __fastcall KeyDown(System::TObject* Sender, Word &Key,
    Classes::TShiftState Shift);
  void __fastcall KeyUp(System::TObject* Sender, Word &Key,
    Classes::TShiftState Shift);
public:
  mxkdb_keystate_t();
  virtual __fastcall ~mxkdb_keystate_t();
  void connect(TMemo *input);
  Word operator()();
};

// Класс драйвера консоли для объекта TStrings C++ Builder
mxbuilder_condrv_t::mxbuilder_condrv_t()
{
}
mxbuilder_condrv_t::mxbuilder_condrv_t(TStrings *Console):
  f_console(Console)
{
}
void mxbuilder_condrv_t::out(const char *str)
{
  int len_s = strlen((char *)str);
  //char *message = (char *)malloc(len_s + 1);
  char *message = new char[len_s + 1];
  if (!message) return;
  ::strcpy(message, (char *)str);
  char *message_line = message;
  //TStrings *Console = MainForm->Memo1->Lines;
  for (int i_msg_ch = 0; i_msg_ch < len_s; i_msg_ch++) {
    if ('\n' == message[i_msg_ch]) {
      message[i_msg_ch] = 0;
      if (f_console->Count <= 0) f_console->Add("");
      String ConsoleLine = f_console->Strings[f_console->Count - 1];
      ConsoleLine += String(message_line);
      f_console->Strings[f_console->Count - 1] = ConsoleLine;
      f_console->Add("");
      message_line = &message[i_msg_ch + 1];
    }
  }
  if (len_s > 0)
  if (message[len_s - 1] != '\n') {
    if (f_console->Count <= 0) f_console->Add("");
    String ConsoleLine = f_console->Strings[f_console->Count - 1];
    ConsoleLine += String(message_line);
    f_console->Strings[f_console->Count - 1] = ConsoleLine;
  }
  delete []message;
}

// Класс для автоподключения mainprg к приложению на C++ Builder
__fastcall mpc_builder_t::mpc_builder_t():
  m_timer(IRS_NULL),
  m_SavedCloseEvent(IRS_NULL),
  m_form(IRS_NULL),
  m_app_proc(IRS_NULL),
  m_mxapplication(IRS_NULL)
{
}
__fastcall mpc_builder_t::~mpc_builder_t()
{
  if (m_timer) delete m_timer;
  m_timer = IRS_NULL;
  if (m_mxapplication) delete m_mxapplication;
  m_mxapplication = IRS_NULL;
}
void mpc_builder_t::TimerInit(TNotifyEvent Event)
{
  m_timer = new TTimer(IRS_NULL);
  m_timer->OnTimer = Event;
  m_timer->Interval = 10;
  m_timer->Enabled = true;
}

void mpc_builder_t::Connect(TForm *Form, void (*a_init)(), void (*a_deinit)(),
  void (*a_tick)(), void (*a_stop)(), irs_bool (*a_stopped)())
{
  m_form = Form;
  m_SavedCloseEvent = m_form->OnClose;
  m_form->OnClose = MainFormClose;

  m_mxapplication = new mxapplication_t();
  m_mxapplication->set_init_event(a_init);
  m_mxapplication->set_denit_event(a_deinit);
  m_mxapplication->set_tick_event(a_tick);
  m_mxapplication->set_stop_event(a_stop);
  m_mxapplication->set_stopped_event(a_stopped);
  m_mxapplication->run(irs_true);

  TimerInit(TimerEvent);
}
void __fastcall mpc_builder_t::MainFormClose(System::TObject* Sender,
  TCloseAction &Action)
{
  if (m_SavedCloseEvent) m_SavedCloseEvent(Sender, Action);
  while (m_mxapplication->run(irs_false));
}
void __fastcall mpc_builder_t::TimerEvent(System::TObject* Sender)
{
  if (!m_mxapplication->run(irs_true)) {
    m_timer->Enabled = false;
    m_form->Close();
    return;
  }
}
void mpc_builder_t::Connect(TForm *a_Form, mx_proc_t *a_app_proc)
{
  m_form = a_Form;
  m_SavedCloseEvent = m_form->OnClose;
  m_form->OnClose = ProcClose;

  m_app_proc = a_app_proc;

  TimerInit(ProcTimer);
}
void __fastcall mpc_builder_t::ProcClose(System::TObject* Sender,
  TCloseAction &Action)
{
  if (m_SavedCloseEvent) m_SavedCloseEvent(Sender, Action);
  m_app_proc->abort();
  while (m_app_proc->tick());
}
void __fastcall mpc_builder_t::ProcTimer(System::TObject* Sender)
{
  if (!m_app_proc->tick()) {
    m_timer->Enabled = false;
    m_form->Close();
    return;
  }
}

// Класс драйвера клавиатуры С++ Builder
mxkey_drv_builder_t::mxkey_drv_builder_t():
  keystate(IRS_NULL)
{
  keystate = (void *)new mxkdb_keystate_t();
}
mxkey_drv_builder_t::~mxkey_drv_builder_t()
{
  delete (mxkdb_keystate_t *)keystate;
}
void mxkey_drv_builder_t::connect(TMemo *a_display)
{
  ((mxkdb_keystate_t *)keystate)->connect(a_display);
}
irskey_t mxkey_drv_builder_t::operator()()
{
  typedef struct _key_map_t {
    Word win_key;
    irskey_t irs_key;
  } key_map_t;
  key_map_t key_map[] = {
    {VK_RETURN,     irskey_enter},
    {VK_SEPARATOR,  irskey_enter},
    {VK_ESCAPE,     irskey_escape},
    {VK_UP,         irskey_up},
    {VK_DOWN,       irskey_down},
    {'0',           irskey_0},
    {'1',           irskey_1},
    {'2',           irskey_2},
    {'3',           irskey_3},
    {'4',           irskey_4},
    {'5',           irskey_5},
    {'6',           irskey_6},
    {'7',           irskey_7},
    {'8',           irskey_8},
    {'9',           irskey_9},
    {VK_NUMPAD0,    irskey_0},
    {VK_NUMPAD1,    irskey_1},
    {VK_NUMPAD2,    irskey_2},
    {VK_NUMPAD3,    irskey_3},
    {VK_NUMPAD4,    irskey_4},
    {VK_NUMPAD5,    irskey_5},
    {VK_NUMPAD6,    irskey_6},
    {VK_NUMPAD7,    irskey_7},
    {VK_NUMPAD8,    irskey_8},
    {VK_NUMPAD9,    irskey_9},
    {VK_DECIMAL,    irskey_point},
    {VK_BACK,       irskey_backspace}
  };
  const irs_u8 key_map_size = sizeof(key_map)/sizeof(key_map_t);

  for (irs_u8 key_map_index = 0; key_map_index < key_map_size;
    key_map_index++) {
    if (key_map[key_map_index].win_key == (*(mxkdb_keystate_t *)keystate)())
    {
      return key_map[key_map_index].irs_key;
    }
  }
  return irskey_none;
}

// Вспомогательный класс драйвера клавиатуры С++ Builder
mxkdb_keystate_t::mxkdb_keystate_t():
  f_key(0)
{
}
__fastcall mxkdb_keystate_t::~mxkdb_keystate_t()
{
}
Word mxkdb_keystate_t::operator()()
{
  return f_key;
}
void __fastcall mxkdb_keystate_t::KeyDown(System::TObject* Sender, Word &Key,
  Classes::TShiftState Shift)
{
  f_key = Key;
}
void __fastcall mxkdb_keystate_t::KeyUp(System::TObject* Sender, Word &Key,
  Classes::TShiftState Shift)
{
  f_key = 0;
}
void mxkdb_keystate_t::connect(TMemo *input)
{
  input->OnKeyDown = KeyDown;
  input->OnKeyUp = KeyUp;
}


// Класс драйвера дисплея С++ Builder

// Время интервала повтора по умолчанию
#define mxdisplay_refresh_time_def TIME_TO_CNT(1, 20)

mxdisplay_drv_builder_t::mxdisplay_drv_builder_t(mxdisp_pos_t a_height,
  mxdisp_pos_t a_width):
  mp_display(IRS_NULL),
  mp_console(IRS_NULL),
  m_height(a_height),
  m_width(a_width),
  m_length(mxdisp_pos_t(m_height*m_width)),
  mp_display_ram(new char[m_length]),
  m_display_ram_changed(irs_true),
  m_refresh_time(mxdisplay_refresh_time_def),
  m_refresh_to(0)
{
  //lcd_clear();
  init_to_cnt();
  set_to_cnt(m_refresh_to, m_refresh_time);
}
mxdisplay_drv_builder_t::~mxdisplay_drv_builder_t()
{
  deinit_to_cnt();
  delete []mp_display_ram;
}
mxdisp_pos_t mxdisplay_drv_builder_t::get_height()
{
  return m_height;
}
mxdisp_pos_t mxdisplay_drv_builder_t::get_width()
{
  return m_width;
}
void mxdisplay_drv_builder_t::outtextpos(
  mxdisp_pos_t a_left, mxdisp_pos_t a_top, const char *ap_text)
{
  if ((a_left < m_width) && (a_top < m_height)) {
    m_display_ram_changed = irs_true;
    mxdisp_pos_t text_len = (mxdisp_pos_t)strlen(ap_text);
    if (a_left + text_len > m_width) text_len = mxdisp_pos_t(m_width - a_left);
    memcpy((void *)(mp_display_ram + a_top*m_width + a_left),
      (void *)ap_text, text_len);
  }
}
void mxdisplay_drv_builder_t::tick()
{
  if (test_to_cnt(m_refresh_to)) {
    set_to_cnt(m_refresh_to, m_refresh_time);
    if (m_display_ram_changed)
    if (mp_console) {
      m_display_ram_changed = irs_false;
      mp_console->BeginUpdate();
      mp_console->Clear();
      const irs_u32 add_chr_cnt = 2;
      const irs_u32 line_cur_size = m_width + add_chr_cnt + 1;
      char *line_cur = new char [line_cur_size];
      memset((void *)line_cur, '-', line_cur_size - 1);
      line_cur[line_cur_size - 1] = 0;
      mp_console->Add(line_cur);
      for (mxdisp_pos_t line_ind = 0; line_ind < m_height; line_ind++) {
        line_cur[0] = '|';
        memcpy((void *)&line_cur[1], mp_display_ram + line_ind*m_width,
          m_width);
        line_cur[line_cur_size - 2] = '|';
        line_cur[line_cur_size - 1] = 0;
        mp_console->Add(line_cur);
      }
      memset((void *)line_cur, '-', line_cur_size - 1);
      line_cur[line_cur_size - 1] = 0;
      mp_console->Add(line_cur);
      mp_console->EndUpdate();
      delete []line_cur;
    }
  }
}
void mxdisplay_drv_builder_t::connect(TMemo *ap_display)
{
  mp_display = ap_display;
  mp_console = mp_display->Lines;
}
void mxdisplay_drv_builder_t::set_refresh_time(counter_t a_refresh_time)
{
  m_refresh_time = a_refresh_time;
}


//Класс драйвер для консоли Windows
irs_win32_console_display_t::irs_win32_console_display_t():
  m_Written(0),
  m_top_displey(0)
{
  m_Coord.X=0;   // координаты вывода(x,y)
  m_Coord.Y=0;   // координаты вывода(x,y)
  //просим десриптор стандартного ввода
  m_InpHandle=GetStdHandle(STD_INPUT_HANDLE);
  //просим дескрипртор стандартного вывода
  m_OutHandle=GetStdHandle(STD_OUTPUT_HANDLE);
  //считываем параметры буфера
  GetConsoleScreenBufferInfo(m_OutHandle, &m_BufInfo);
  //щас погасим курсор
  CONSOLE_CURSOR_INFO lpConsoleCursorInfo;
  GetConsoleCursorInfo(m_OutHandle,&lpConsoleCursorInfo);
  lpConsoleCursorInfo.bVisible=false;   //гасим курсор
  SetConsoleCursorInfo(m_OutHandle,&lpConsoleCursorInfo);
  //m_textbuf=newx char[get_width()];
  //if (SetConsoleCP(1252));
}

mxdisp_pos_t irs_win32_console_display_t::get_height()
{
  return static_cast<mxdisp_pos_t>(m_BufInfo.srWindow.Bottom+1);//.dwSize.Y;
}
mxdisp_pos_t irs_win32_console_display_t::get_width()
{
  return static_cast<mxdisp_pos_t>(m_BufInfo.srWindow.Right+1);//.dwSize.X;
}
void irs_win32_console_display_t::outtextpos(
  mxdisp_pos_t a_left,
  mxdisp_pos_t a_top,
  const char *text)
{
  if((a_left < static_cast<mxdisp_pos_t>(m_BufInfo.srWindow.Right+1)) &&
    (a_top < static_cast<mxdisp_pos_t>(m_BufInfo.srWindow.Bottom+1)))
  {
    irs_u32 width=get_width();
	  m_Coord.X=static_cast<short>(a_left);
	  m_Coord.Y=static_cast<short>(a_top+m_top_displey);
    irs_u32 lentext=min(strlen(text), static_cast<size_t>(width-a_left));
    irs::auto_arr<wchar_t> unicode_text(new wchar_t[lentext+1]);
    MultiByteToWideChar(
      1251/*UINT CodePage*/,
      MB_PRECOMPOSED /*DWORD dwFlags*/,
      text/*LPCSTR lpMultiByteStr*/,
      lentext/*int cbMultiByte*/,
      unicode_text.get()/*LPWSTR lpWideCharStr*/,
      lentext/*int cchWideChar*/);
    WriteConsoleOutputCharacterW(
      m_OutHandle,              // screen buffer handle
      unicode_text.get(),        // pointer to source string
      lentext,                  // length of string
      m_Coord,                  // first cell to write to
      &m_Written);              // actual number written
  }
}
void irs_win32_console_display_t::tick()
{
}


// Класс драйвер для влавиатуры
irs_win32_console_key_drv_t::irs_win32_console_key_drv_t():
  m_flag_event_KeyDown(false),
  m_temporary_irs_key(irskey_none),
  m_Written(0)
{ 
  //просим десриптор стандартного ввода
  m_InpHandle=GetStdHandle(STD_INPUT_HANDLE);
  m_fdwMode = ENABLE_WINDOW_INPUT;
  SetConsoleMode(m_InpHandle, m_fdwMode);
}
irskey_t irs_win32_console_key_drv_t::operator()()
{

  typedef struct _key_map_t {
    irs_i16 win_key;
    irskey_t irs_key;
  } key_map_t;
  key_map_t key_map[] = {
    {VK_RETURN,     irskey_enter},
    {VK_SEPARATOR,  irskey_enter},
    {VK_ESCAPE,     irskey_escape},
    {VK_UP,         irskey_up},
    {VK_DOWN,       irskey_down},
    {'0',           irskey_0},
    {'1',           irskey_1},
    {'2',           irskey_2},
    {'3',           irskey_3},
    {'4',           irskey_4},
    {'5',           irskey_5},
    {'6',           irskey_6},
    {'7',           irskey_7},
    {'8',           irskey_8},
    {'9',           irskey_9},
    {VK_NUMPAD0,    irskey_0},
    {VK_NUMPAD1,    irskey_1},
    {VK_NUMPAD2,    irskey_2},
    {VK_NUMPAD3,    irskey_3},
    {VK_NUMPAD4,    irskey_4},
    {VK_NUMPAD5,    irskey_5},
    {VK_NUMPAD6,    irskey_6},
    {VK_NUMPAD7,    irskey_7},
    {VK_NUMPAD8,    irskey_8},
    {VK_NUMPAD9,    irskey_9},
    {VK_DECIMAL,    irskey_point},
    {VK_BACK,       irskey_backspace}
  };
  const irs_u8 key_map_size = sizeof(key_map)/sizeof(key_map_t);

  GetNumberOfConsoleInputEvents(m_InpHandle,&m_cNumRead);
  if(m_cNumRead>0)
  {
    ReadConsoleInput(m_InpHandle, m_irInBuf, m_len_buf, &m_cNumRead);
    for(irs_u32 i=0;i< m_cNumRead; i++)
      if(m_irInBuf[i].EventType==KEY_EVENT)    //если событие с клавиатуры
      {
        //если событие при нажатии клавиши и клавиша не была нажата
        if(m_irInBuf[i].Event.KeyEvent.bKeyDown==true
          && m_flag_event_KeyDown==false)
        {
          //смотрим, совпадает ли с одной из кнопок
          for (irs_u8 key_map_index = 0; key_map_index < key_map_size;
          key_map_index++)
          {
            //если совпадает, отправляем ее
            if(key_map[key_map_index].win_key==
              m_irInBuf[i].Event.KeyEvent.wVirtualKeyCode)  //виртуальный код
            {
              m_flag_event_KeyDown=true;
              m_temporary_irs_key=key_map[key_map_index].irs_key;
              return m_temporary_irs_key;
            }
          }
        }
        //если произошло отпускание кнопки, и клавиша была нажата
        else  if(m_irInBuf[i].Event.KeyEvent.bKeyDown==false &&
          m_flag_event_KeyDown==true)
        {
          m_flag_event_KeyDown=false;
          m_temporary_irs_key=irskey_none;
          return m_temporary_irs_key;
        }
        else  if(m_flag_event_KeyDown==true)  //если все еще клавиша нажата
        {
          return m_temporary_irs_key;
        }             
      }
  }
  else  if(m_flag_event_KeyDown==true)        //если все еще клавиша нажата
  {
    return m_temporary_irs_key;
  }

  m_temporary_irs_key=irskey_none;
  return m_temporary_irs_key;
}

// Конфигурация консоли для C++ Builder
irs::conio_cfg_t& irs::arch_conio_cfg::def()
{
  static irs_win32_console_key_drv_t key_drv;
  static irs_win32_console_display_t display;
  static conio_cfg_t conio_cfg_i(key_drv, display);
  return conio_cfg_i;
}
// Конфигурация консоли для Memo C++ Builder
irs::conio_cfg_t& irs::arch_conio_cfg::memo(TMemo *ap_memo,
  mxdisp_pos_t a_height, mxdisp_pos_t a_width)
{
  static mxkey_drv_builder_t key_drv;
  static mxdisplay_drv_builder_t display(a_height, a_width);
  static conio_cfg_t conio_cfg_i(key_drv, display);
  key_drv.connect(ap_memo);
  display.connect(ap_memo);
  return conio_cfg_i;
}

// Буфер стандартных потоков для Memo
irs::memobuf::memobuf(const memobuf& a_buf):
  m_outbuf_size(a_buf.m_outbuf_size),
  m_outbuf(new char[m_outbuf_size + 1]),
  mp_memo(a_buf.mp_memo)
{
  memset(m_outbuf.get(), 0, m_outbuf_size);
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
}
irs::memobuf::memobuf(TMemo *ap_memo, int a_outbuf_size):
  m_outbuf_size(a_outbuf_size),
  m_outbuf(new char[m_outbuf_size + 1]),
  mp_memo(ap_memo)
{
  memset(m_outbuf.get(), 0, m_outbuf_size);
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
}
irs::memobuf::~memobuf()
{
}
void irs::memobuf::connect(TMemo *ap_memo)
{
  mp_memo = ap_memo;
}
int irs::memobuf::overflow(int c)
{
  if (!mp_memo) return 0;
  
  int len_s = pptr() - pbase();
  TStrings *Console = mp_memo->Lines;
  if (len_s > 0) {
    *pptr() = 0;

    char *message = pbase();
    char *message_line = message;
    for (int i_msg_ch = 0; i_msg_ch < len_s; i_msg_ch++) {
      if ('\n' == message[i_msg_ch]) {
        message[i_msg_ch] = 0;
        if (Console->Count <= 0) Console->Add("");
        String ConsoleLine = Console->Strings[Console->Count - 1];
        ConsoleLine += String(message_line);
        Console->Strings[Console->Count - 1] = ConsoleLine;
        Console->Add("");
        message_line = &message[i_msg_ch + 1];
      }
    }
    if (message[len_s - 1] != '\n') {
      if (Console->Count <= 0) Console->Add("");
      String ConsoleLine = Console->Strings[Console->Count - 1];
      ConsoleLine += String(message_line);
      Console->Strings[Console->Count - 1] = ConsoleLine;
    }
  }
  if (c != EOF)
  {
    if (c != '\n') {
      if (Console->Count <= 0) Console->Add("");
      String ConsoleLine = Console->Strings[Console->Count - 1];
      ConsoleLine += String((char)c);
      Console->Strings[Console->Count - 1] = ConsoleLine;
    } else {
      Console->Add("");
    }
  }
  setp(m_outbuf.get(), m_outbuf.get() + m_outbuf_size);
  return 0;
}
int irs::memobuf::sync()
{
  return overflow();
}
#endif //__BORLANDC__

#ifdef __WATCOMC__
#include <irsstdl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Преобразование числа double в строку в %g формате
irs_u8 *irs_gcvt(double value, int ndigits, irs_u8 *buffer)
{
  return (irs_u8 *)gcvt(value, ndigits, (char *)buffer);
}

//Класс драйвер для консоли Windows
irs_win32_console_display_t::irs_win32_console_display_t():
  m_Written(0),
  m_top_displey(0)
{
  m_Coord.X=0;   // координаты вывода(x,y)
  m_Coord.Y=0;   // координаты вывода(x,y)
  //просим десриптор стандартного ввода
  m_InpHandle=GetStdHandle(STD_INPUT_HANDLE);
  //просим дескрипртор стандартного вывода
  m_OutHandle=GetStdHandle(STD_OUTPUT_HANDLE);
  //считываем параметры буфера
  GetConsoleScreenBufferInfo(m_OutHandle, &m_BufInfo);
  //щас погасим курсор
  CONSOLE_CURSOR_INFO lpConsoleCursorInfo;
  GetConsoleCursorInfo(m_OutHandle,&lpConsoleCursorInfo);
  lpConsoleCursorInfo.bVisible=false;   //гасим курсор
  SetConsoleCursorInfo(m_OutHandle,&lpConsoleCursorInfo);
  m_textbuf=new char[get_width()];           
}

mxdisp_pos_t irs_win32_console_display_t::get_height()
{
  return static_cast<mxdisp_pos_t>(m_BufInfo.srWindow.Bottom+1);//.dwSize.Y;
}
mxdisp_pos_t irs_win32_console_display_t::get_width()
{
  return static_cast<mxdisp_pos_t>(m_BufInfo.srWindow.Right+1);//.dwSize.X;
}
void irs_win32_console_display_t::outtextpos(
  mxdisp_pos_t a_left,
  mxdisp_pos_t a_top,
  const char *text)
{
  if((a_left < m_BufInfo.srWindow.Right+1) &&
    (a_top < m_BufInfo.srWindow.Bottom+1))
  {
    irs_u32 width=get_width();
    m_Coord.X = static_cast<SHORT>(a_left);
    m_Coord.Y = static_cast<SHORT>(a_top+m_top_displey);
    irs_u32 lentext=min(strlen(text), static_cast<unsigned int>(width-a_left));
    WriteConsoleOutputCharacter(
      m_OutHandle,              // screen buffer handle
      text,                     // pointer to source string
      lentext,                  // length of string
      m_Coord,                  // first cell to write to
      &m_Written);              // actual number written
  }
}

void irs_win32_console_display_t::tick()
{
}

// Класс драйвер для влавиатуры
irs_win32_console_key_drv_t::irs_win32_console_key_drv_t():
  m_flag_event_KeyDown(irs_false),
  m_temporary_irs_key(irskey_none),
  m_Written(0)
{ 
  //просим десриптор стандартного ввода
  m_InpHandle=GetStdHandle(STD_INPUT_HANDLE);
  m_fdwMode = ENABLE_WINDOW_INPUT;
  SetConsoleMode(m_InpHandle, m_fdwMode);
}
irskey_t irs_win32_console_key_drv_t::operator()()
{

  typedef struct _key_map_t {
    irs_i16 win_key;
    irskey_t irs_key;
  } key_map_t;
  key_map_t key_map[] = {
    {VK_RETURN,     irskey_enter},
    {VK_SEPARATOR,  irskey_enter},
    {VK_ESCAPE,     irskey_escape},
    {VK_UP,         irskey_up},
    {VK_DOWN,       irskey_down},
    {'0',           irskey_0},
    {'1',           irskey_1},
    {'2',           irskey_2},
    {'3',           irskey_3},
    {'4',           irskey_4},
    {'5',           irskey_5},
    {'6',           irskey_6},
    {'7',           irskey_7},
    {'8',           irskey_8},
    {'9',           irskey_9},
    {VK_NUMPAD0,    irskey_0},
    {VK_NUMPAD1,    irskey_1},
    {VK_NUMPAD2,    irskey_2},
    {VK_NUMPAD3,    irskey_3},
    {VK_NUMPAD4,    irskey_4},
    {VK_NUMPAD5,    irskey_5},
    {VK_NUMPAD6,    irskey_6},
    {VK_NUMPAD7,    irskey_7},
    {VK_NUMPAD8,    irskey_8},
    {VK_NUMPAD9,    irskey_9},
    {VK_DECIMAL,    irskey_point},
    {VK_BACK,       irskey_backspace}
  };
  const irs_u8 key_map_size = sizeof(key_map)/sizeof(key_map_t);

  GetNumberOfConsoleInputEvents(m_InpHandle,&m_cNumRead);
  if(m_cNumRead>0)
  {
    //irInBuf-buffer to read into
    //128-size of read buffer
    //cNumRead-number of records read
    ReadConsoleInput(m_InpHandle, m_irInBuf, m_len_buf, &m_cNumRead);
    for(irs_u32 i=0;i< m_cNumRead; i++)
      if(m_irInBuf[i].EventType==KEY_EVENT)    //если событие с клавиатуры
      {
        //если событие при нажатии клавиши и клавиша не была нажата
        if(m_irInBuf[i].Event.KeyEvent.bKeyDown==irs_true
          && m_flag_event_KeyDown==irs_false)
        {
          //смотрим, совпадает ли с одной из кнопок
          for (irs_u8 key_map_index = 0; key_map_index < key_map_size;
          key_map_index++)
          {
            //если совпадает, отправляем ее
            if(key_map[key_map_index].win_key==
              m_irInBuf[i].Event.KeyEvent.wVirtualKeyCode)  //виртуальный код
            {
              m_flag_event_KeyDown=irs_true;
              m_temporary_irs_key=key_map[key_map_index].irs_key;
              return m_temporary_irs_key;
            }
          }
        }
        //если произошло отпускание кнопки, и клавиша была нажата
        else  if(m_irInBuf[i].Event.KeyEvent.bKeyDown==irs_false &&
          m_flag_event_KeyDown==irs_true)
        {
          m_flag_event_KeyDown=irs_false;
          m_temporary_irs_key=irskey_none;
          return m_temporary_irs_key;
        }
        else  if(m_flag_event_KeyDown==irs_true)  //если все еще клавиша нажата
        {
          return m_temporary_irs_key;
        }             
      }
  }
  else  if(m_flag_event_KeyDown==irs_true)        //если все еще клавиша нажата
  {
    return m_temporary_irs_key;
  }

  m_temporary_irs_key=irskey_none;
  return m_temporary_irs_key;
}
#endif //__WATCOMC__
