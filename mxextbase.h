//! \file
//! \ingroup graphical_user_interface_group
//! \brief Классы и функции для mx_ext_chart_t
//!
//! Дата: 14.04.2010\n
//! Дата создания: 8.09.2008

#ifndef MXEXTBASEH
#define MXEXTBASEH

#define MXEXT_OFF

#ifndef MXEXT_OFF
#include <irsdefs.h>

#ifdef IRS_WIN32
#include <mmsystem.h>
#include <winspool.h>
#endif //IRS_WIN32
//#include <Dialogs.hpp>
#ifdef __BORLANDC__
#include <Graphics.hpp>
#endif //__BORLANDC__

//#include <vcl.h>
#include <irscpp.h>
#include <irslimits.h>
#include <irsrect.h>

#include <irsfinal.h>

//! \addtogroup graphical_user_interface_group
//! @{

#ifdef IRS_FULL_STDCPPLIB_SUPPORT
#if defined(QT_CORE_LIB) || defined(__BORLANDC__)

#define SAFEOPER(__operator__) try { __operator__; } catch (...) {}
//---------------------------------------------------------------------------
//extern void __fastcall Point(TCanvas *Canvas, TPoint P);
//extern String AppName;
//---------------------------------------------------------------------------
namespace irs {

//! \brief Подключение функции члена класса вида T fn(T x)
//! \brief Интерфейс
template <class T>
class value_fn_value_t
{
public:
  typedef T value_type;

  virtual ~value_fn_value_t() {}
  virtual value_type fn(value_type x) = 0;
private:
};
//! \brief Подключение функции члена класса вида T fn(T x)
//! \brief Реализация
template <class class_type, class value_type,
  class base_type = value_fn_value_t<value_type> >
class value_method_value_t: public base_type
{
public:
  typedef value_type (class_type::*member_type)(value_type);

  value_method_value_t(class_type* ap_object = IRS_NULL,
    member_type ap_member = IRS_NULL);
  virtual value_type fn(value_type x)
  {
    return (mp_object->*mp_member)(x);
  }
private:
  class_type* mp_object;
  member_type mp_member;
};

template <class T>
struct mx_chart_line_t {
  typedef T value_type;
  typedef mx_point_t<value_type> point_type;

  point_type begin;
  point_type end;

  mx_chart_line_t(
    point_type a_begin = point_type(),
    point_type a_end = point_type()
  ):
    begin(a_begin),
    end(a_end)
  {
  }
  mx_chart_line_t(
    value_type a_x1,
    value_type a_y1,
    value_type a_x2,
    value_type a_y2
  ):
    begin(point_type(a_x1, a_y1)),
    end(point_type(a_x2, a_y2))
  {
  }
};

struct mx_ext_chart_types
{
  typedef long double float_type;
  typedef int int_type;
  typedef size_t size_type;
  typedef mx_point_t<float_type> point_float_type;
  typedef mx_rect_t<float_type> rect_float_type;
  typedef mx_point_t<int_type> point_int_type;
  typedef mx_rect_t<int_type> rect_int_type;
  typedef mx_bounds_t<float_type> bounds_float_type;
  typedef mx_chart_line_t<int_type> line_type;
  enum paint_style_t { ps_display_paint, ps_black_print, ps_color_print };
  enum item_change_t { ic_delete = 0, ic_clear = 1, ic_insert = 2,
    ic_comp_conv_x = 3, ic_comp_conv_y = 4, ic_set_base_item = 5 };

  static size_type last_index_mark();
};

union color_union_t {
  struct {
    irs_u8 blue;
    irs_u8 green;
    irs_u8 red;
    irs_u8 alpha;
  };
  irs_u32 color;
};

enum color_const_t {
  cl_first,
  cl_white = cl_first,
  cl_black,
  cl_red,
  cl_dark_red,
  cl_green,
  cl_dark_green,
  cl_blue,
  cl_dark_blue,
  cl_cyan,
  cl_dark_cyan,
  cl_magenta,
  cl_dark_magenta,
  cl_yellow,
  cl_dark_yellow,
  cl_gray,
  cl_dark_gray,
  cl_light_gray,
  cl_transparent,
  cl_cream,
  cl_money_green,
  cl_sky_blue,
  cl_size
};

color_union_t color_union_from_color_const(color_const_t a_color_const);
color_union_t color_union_from_rgba(irs_u8 a_red = 255, irs_u8 a_green = 255,
  irs_u8 a_blue = 255, irs_u8 a_alpha = 255);

class color_t
{
public:
  color_t(irs_u8 a_red = 255, irs_u8 a_green = 255, irs_u8 a_blue = 255,
    irs_u8 a_alpha = 255);
  color_t(color_const_t a_color_const);
  color_t(const color_t& a_color);

  void assign_rgba(irs_u8 a_red = 255, irs_u8 a_green = 255,
    irs_u8 a_blue = 255, irs_u8 a_alpha = 255);
  void operator=(color_const_t a_color_const);
  void operator=(const color_t& a_color);

  void swap(color_t& a_color);
  irs_u8 red() const;
  irs_u8 green() const;
  irs_u8 blue() const;
  irs_u8 alpha() const;
  void red(irs_u8 a_red);
  void green(irs_u8 a_green);
  void blue(irs_u8 a_blue);
  void alpha(irs_u8 a_alpha);
private:
  color_union_t m_color_union;
};
void swap(color_t& a_color_left, color_t& a_color_right);

enum pen_style_t { ps_solid, ps_dash, ps_dot, ps_dash_dot };

// В brush_t есть только цвет (color_t)
// В будущем, возможно, появятся другие параметры
class brush_t
{
public:
  brush_t(const color_t& a_color = color_t());

  color_t color();
  void color(const color_t& a_color);
private:
  color_t m_color;
};

class pen_t
{
public:
  pen_t(const color_t& a_color = color_t(), pen_style_t a_style = ps_solid);

  color_t color();
  void color(const color_t& a_color);
  pen_style_t style();
  void style(pen_style_t a_style);
private:
  color_t m_color;
  pen_style_t m_style;
};

//! \brief Холст (Canvas)
//! \brief Интерфейс
class canvas_t: public mx_ext_chart_types
{
public:
  //typedef mx_ext_chart_types::rect_int_type rect_int_type;

  //! \brief Чтение текущего пера
  virtual pen_t pen() = 0;
  //! \brief Установка текущего пера
  virtual void pen(const pen_t& a_pen) = 0;
  //! \brief Чтение текущей кисти
  virtual brush_t brush() = 0;
  //! \brief Установка текущей кисти
  virtual void brush(const brush_t& a_brush) = 0;
  //! \brief Рисует линию текущим пером (pen)
  virtual void line(const line_type& a_chart_line) = 0;
  //! \brief Заполняет прямоуголник текущей кистью (brush)
  //virtual void fill_rect(const rect_int_type& a_rect) = 0;
  //! \brief Заполняет прямоуголник текущей кистью (brush)
  virtual void clear() = 0;
};

#ifdef __BORLANDC__
//! \brief Холст (Canvas)
//! \brief Реализация для C++ Builder
class cbuilder_canvas_t: public canvas_t
{
public:
  cbuilder_canvas_t(TCanvas* ap_canvas);

  //! \brief Чтение текущего пера
  virtual pen_t pen();
  //! \brief Установка текущего пера
  virtual void pen(const pen_t& a_pen);
  //! \brief Чтение текущей кисти
  virtual brush_t brush();
  //! \brief Установка текущей кисти
  virtual void brush(const brush_t& a_brush);
  //! \brief Рисует линию текущим пером (pen)
  virtual void line(const line_type& a_chart_line);
  //! \brief Заполняет холст текущей кистью (brush)
  virtual void clear();

private:
  TCanvas* mp_canvas;

  static color_t color_from_native(TColor a_native_color);
  static TColor color_to_native(color_t a_color);
  static pen_style_t pen_style_from_native(TPenStyle a_native_style);
  static TPenStyle pen_style_to_native(pen_style_t a_style);
};
#endif //__BORLANDC__

} //namespace irs

#ifdef NOP
typedef struct tagDblRect { double Left, Top, Right, Bottom; } TDblRect;
bool operator == (TDblRect R1, TDblRect R2)
{
  return R1.Left == R2.Left && R1.Top == R2.Top && R1.Right == R2.Right &&
    R1.Bottom == R2.Bottom;
}
bool operator != (TDblRect R1, TDblRect R2)
{
  return R1.Left != R2.Left || R1.Top != R2.Top || R1.Right != R2.Right ||
    R1.Bottom != R2.Bottom;
}
typedef struct tagDblPoint { double x, y; } TDblPoint;
typedef struct tagDblBounds {
  double Begin, End;
  friend bool operator == (tagDblBounds ,tagDblBounds);
  friend bool operator != (tagDblBounds ,tagDblBounds);
} TDblBounds;
inline bool operator == (tagDblBounds A1, tagDblBounds A2)
{ return  A1.Begin == A2.Begin && A1.End == A2.End; }
inline bool operator != (tagDblBounds A1, tagDblBounds A2)
{ return  A1.Begin != A2.Begin || A1.End != A2.End; }
TDblBounds __fastcall DblBounds(double Begin, double End)
{
  TDblBounds Temp = {Begin, End};
  return Temp;
}
class TPrinterDC;
typedef unsigned char uchar;

struct TLine { TPoint Begin, End; };
TLine Line(TPoint Begin, TPoint End)
{
  TLine L; L.Begin = Begin; L.End = End; return L;
}
TLine Line(int x1, int y1, int x2, int y2)
{
  return Line(Point(x1, y1), Point(x2, y2));
}
//---------------------------------------------------------------------------
struct TWaveFormat
{
  int FormatTag;          // Метка фомата
  int Channels;           // Число каналов
  int Bits;               // Разрядность канала в битах
  int Sample;             // Размер отсчета в байтах (для ИКМ /PCM/)
  unsigned Frecuency;     // Частота дискретизации в Гц (отсчет/с)
  unsigned Speed;         // Скорость данных в байт/с
  unsigned Size;          // Размер данных в байтах
  unsigned Pos;           // Начало данных
};
//---------------------------------------------------------------------------
template <class T> T sqr(T x) { return x*x; }
extern void __fastcall Line(TCanvas *Canvas, TPoint P1, TPoint P2);
extern void __fastcall DashLine(TCanvas *Canvas, TPoint P1, TPoint P2, int Dash,
  int Space);
extern void __fastcall PolyLine(TCanvas *Canvas, TPoint *Points, int LastPoint);
extern void __fastcall DashPolyLine(TCanvas *Canvas, TPoint *Points,
  int LastPoint, int Dash, int Space);
extern void __fastcall RectLine(TCanvas *Canvas, TRect Rect);
extern void __fastcall DashRectLine(TCanvas *Canvas, TRect Rect, int Dash,
  int Space);
extern TDblPoint __fastcall DblPoint(double x, double y);
extern TDblRect __fastcall DblRect(double Left, double Top, double Right,
  double Bottom);
extern TSize __fastcall TextExtent(TCanvas *Canvas, String Text);
extern int __fastcall TextWidth(TCanvas *Canvas, String Text);
extern int __fastcall TextHeight(TCanvas *Canvas, String Text);
extern void __fastcall SetClipRect(TCanvas *Canvas, TRect Rect);
extern void __fastcall DeleteClipRect(TCanvas *Canvas);
extern bool __fastcall GetWaveFormat(String FileName, TWaveFormat &WaveFormat);
extern bool __fastcall GetPCMWaveData(String FileName, TWaveFormat &WaveFormat,
  short *&Data, unsigned &Begin, unsigned &Size, int Channel);
extern void __fastcall FormCentered(TForm *Form);
extern void __fastcall CenteredOfActiveForm(TForm *Form);
extern void __fastcall ShowMsg(String Text);
extern TModalResult __fastcall ShowMsg(String Text, TMsgDlgButtons Buttons);
extern TModalResult __fastcall ShowMsg(String Caption, String Text,
  TMsgDlgType DlgType, TMsgDlgButtons Buttons);
extern void __fastcall Line(TCanvas *Canvas, TLine L);
extern void __fastcall LineP(TCanvas *Canvas, TLine L);
extern bool FltEqual(long double x1, long double x2, long double eps = 1e-10);
extern int __fastcall Align2(int Number, int Shift);
//---------------------------------------------------------------------------
template <class T1>
String __fastcall Printf(const String& format, T1 arg1)
{
  return Format(format, ARRAYOFCONST((arg1)));
}
template <class T1, class T2>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2)
{
  return Format(format, ARRAYOFCONST((arg1, arg2)));
}
template <class T1, class T2, class T3>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3)));
}
template <class T1, class T2, class T3, class T4>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4)));
}
template <class T1, class T2, class T3, class T4, class T5>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7,
  class T8>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7,
    arg8)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7,
  class T8, class T9>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7,
    arg8, arg9)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7,
  class T8, class T9, class T10>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7,
    arg8, arg9, arg10)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7,
  class T8, class T9, class T10, class T11>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7,
    arg8, arg9, arg10, arg11)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7,
  class T8, class T9, class T10, class T11, class T12>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11,
  T12 arg12)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7,
    arg8, arg9, arg10, arg11, arg12)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7,
  class T8, class T9, class T10, class T11, class T12, class T13>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11,
  T12 arg12, T13 arg13)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7,
    arg8, arg9, arg10, arg11, arg12, arg13)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7,
  class T8, class T9, class T10, class T11, class T12, class T13, class T14>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11,
  T12 arg12, T13 arg13, T14 arg14)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7,
    arg8, arg9, arg10, arg11, arg12, arg13, arg14)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7,
  class T8, class T9, class T10, class T11, class T12, class T13, class T14,
  class T15>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11,
  T12 arg12, T13 arg13, T14 arg14, T15 arg15)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7,
    arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7,
  class T8, class T9, class T10, class T11, class T12, class T13, class T14,
  class T15, class T16>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11,
  T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7,
    arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7,
  class T8, class T9, class T10, class T11, class T12, class T13, class T14,
  class T15, class T16, class T17>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11,
  T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7,
    arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7,
  class T8, class T9, class T10, class T11, class T12, class T13, class T14,
  class T15, class T16, class T17, class T18>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11,
  T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7,
    arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7,
  class T8, class T9, class T10, class T11, class T12, class T13, class T14,
  class T15, class T16, class T17, class T18, class T19>
String __fastcall Printf(const String& format, T1 arg1, T2 arg2, T3 arg3,
  T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11,
  T12 arg12, T13 arg13, T14 arg14, T15 arg15, T16 arg16, T17 arg17, T18 arg18,
  T19 arg19)
{
  return Format(format, ARRAYOFCONST((arg1, arg2, arg3, arg4, arg5, arg6, arg7,
    arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, arg17, arg18,
    arg19)));
}
//---------------------------------------------------------------------------
#endif //NOP
enum TDataType {dtUndef, dtFunction, dtMethod, dtChar, dtShort, dtInt, dtLong,
  dtInt64, dtUChar, dtUShort, dtUInt, dtULong, dtUInt64, dtFloat, dtDouble,
  dtLongDouble, dtComplex};
enum TCompConv {ccReal, ccImag, ccAbs, ccArg};
typedef complex<double> cmp;
typedef complex<float> fcmp;
typedef complex<long double> lcmp;
//typedef double (__closure *TClassDblFunc)(double x);
typedef double (*TDblFunc)(double x);
typedef cmp (*TCmpDblFunc)(double x);
typedef cmp (*TCmpFunc)(cmp x);

class TPointer
{
  typedef irs::value_fn_value_t<double>* method_type;

  union
  {
    signed char* Char;
    signed short* Short;
    signed int* Int;
    signed long* Long;
    unsigned char* UChar;
    unsigned short* UShort;
    unsigned int* UInt;
    unsigned long* ULong;
    signed __int64* Int64;
    unsigned __int64* UInt64;
    method_type Method;
    TDblFunc Function;
    cmp* Complex;
    float* Float;
    double* Double;
    long double* LongDouble;
  };
  TDataType FType;
  TCompConv FCompConv;
public:
  TDataType type() { return FType; }
  TCompConv comp_conv() { return FCompConv; }
  void comp_conv(TCompConv a_comp_conv) { FCompConv = a_comp_conv; }

  TPointer(): Method(NULL), FType(dtUndef), FCompConv(ccReal) {}
  TPointer(signed char* Value):
    Char(Value), FType(dtChar), FCompConv(ccReal) {}
  TPointer(signed short* Value):
    Short(Value), FType(dtShort), FCompConv(ccReal) {}
  TPointer(signed int* Value):
    Int(Value), FType(dtInt), FCompConv(ccReal) {}
  TPointer(signed long* Value):
    Long(Value), FType(dtLong), FCompConv(ccReal) {}
  TPointer(unsigned char* Value):
    UChar(Value), FType(dtUChar), FCompConv(ccReal) {}
  TPointer(unsigned short* Value):
    UShort(Value), FType(dtUShort), FCompConv(ccReal) {}
  TPointer(unsigned int* Value):
    UInt(Value), FType(dtUInt), FCompConv(ccReal) {}
  TPointer(unsigned long* Value):
    ULong(Value), FType(dtULong), FCompConv(ccReal) {}
  TPointer(signed __int64* Value):
    Int64(Value), FType(dtInt64), FCompConv(ccReal) {}
  TPointer(unsigned __int64* Value):
    UInt64(Value), FType(dtUInt64), FCompConv(ccReal) {}
  TPointer(method_type Value):
    Method(Value), FType(dtMethod), FCompConv(ccReal) {}
  TPointer(TDblFunc Value):
    Function(Value), FType(dtFunction), FCompConv(ccReal) {}
  TPointer(cmp* Value):
    Complex(Value), FType(dtComplex), FCompConv(ccReal) {}
  TPointer(float* Value):
    Float(Value), FType(dtFloat), FCompConv(ccReal) {}
  TPointer(double* Value):
    Double(Value), FType(dtDouble), FCompConv(ccReal) {}
  TPointer(long double* Value):
    LongDouble(Value), FType(dtLongDouble), FCompConv(ccReal) {}

  operator signed char*() const { return Char; }
  operator signed short*() const { return Short; }
  operator signed int*() const { return Int; }
  operator signed long*() const { return Long; }
  operator unsigned char*() const { return UChar; }
  operator unsigned short*() const { return UShort; }
  operator unsigned int*() const { return UInt; }
  operator unsigned long*() const { return ULong; }
  operator signed __int64*() const { return Int64; }
  operator unsigned __int64*() const { return UInt64; }
  operator cmp*() const { return Complex; }
  operator TDblFunc() const { return Function; }
  operator method_type() const { return Method; }
  operator float*() const { return Float; }
  operator double*() const { return Double; }
  operator long double*() const { return LongDouble; }
  operator bool() { return irs::to_bool(Int); }

  long double operator[](int i) const
  {
    switch (FType)
    {
      case dtChar: return Char[i];
      case dtShort: return Short[i];
      case dtInt: return Int[i];
      case dtLong: return Long[i];
      case dtUChar: return UChar[i];
      case dtUShort: return UShort[i];
      case dtUInt: return UInt[i];
      case dtULong: return ULong[i];
      case dtInt64: return Int64[i];
      case dtUInt64: return UInt64[i];
      case dtFloat: return Float[i];
      case dtDouble: return Double[i];
      case dtLongDouble: return LongDouble[i];
      case dtComplex:
        switch (FCompConv)
        {
          case ccReal: return real(Complex[i]);
          case ccImag: return imag(Complex[i]);
          case ccAbs: return abs(Complex[i]);
          case ccArg: return arg(Complex[i]);
          default:  return real(Complex[i]);
        }
      case dtFunction: return Function(i);
      case dtMethod: return Method->fn(i);
      default: return 0;
    }
  }
};

#ifdef NOP
class TPrinterDC
{
  HDC PDC;
  String FDevice;
public:
  __property String Device = {read=FDevice};
  operator HDC() { return PDC; }
  TPrinterDC();
  ~TPrinterDC();
};
#endif //NOP

//} // namespace Mxbase

//using namespace Mxbase;

#endif //defined(QT_CORE_LIB) || defined(__BORLANDC__)
#endif //IRS_FULL_STDCPPLIB_SUPPORT
#endif //MXEXT_OFF

//! @}

#endif //MXEXTBASEH
