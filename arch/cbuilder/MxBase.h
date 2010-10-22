//! \file
//! \ingroup graphical_user_interface_group
//! \brief Полезные функции для C++ Builder
//!
//! Дата: 14.04.2010\n
//! Дата создания: 8.09.2008

//---------------------------------------------------------------------------
#ifndef MxBaseH
#define MxBaseH
//---------------------------------------------------------------------------
#include <irsdefs.h>

//#include <vcl.h>
#include <mmsystem.h>
#include <winspool.h>

#include <irscpp.h>

#include <irsfinal.h>

//! \ingroup graphical_user_interface_group
#define SAFEOPER(__operator__) try { __operator__; } catch (...) {}
//---------------------------------------------------------------------------
extern void __fastcall Point(TCanvas *Canvas, TPoint P);
//---------------------------------------------------------------------------

namespace irs {

//! \ingroup graphical_user_interface_group
//! \brief Копирование свойств формы
void mxcopy(TForm *dest, TForm *src);

} //namespace irs

namespace Mxbase
{

//! \addtogroup graphical_user_interface_group
//! @{

//---------------------------------------------------------------------------
extern String AppName;
//---------------------------------------------------------------------------
enum TDataType {dtUndef, dtFunction, dtMethod, dtChar, dtShort, dtInt, dtLong,
  dtInt64, dtUChar, dtUShort, dtUInt, dtULong, dtUInt64, dtFloat, dtDouble,
  dtLongDouble, dtComplex};
enum TCompConv {ccReal, ccImag, ccAbs, ccArg};
typedef complex<double> cmp;
typedef complex<float> fcmp;
typedef complex<long double> lcmp;
typedef double (__closure *TClassDblFunc)(double x);
typedef double (*TDblFunc)(double x);
typedef cmp (*TCmpDblFunc)(double x);
typedef cmp (*TCmpFunc)(cmp x);
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
TLine __fastcall Line(TPoint Begin, TPoint End)
{
  TLine L; L.Begin = Begin; L.End = End; return L;
}
TLine __fastcall Line(int x1, int y1, int x2, int y2)
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
class TPointer
{
	union
  {
    signed char *Char;
    signed short *Short;
    signed int *Int;
    signed long *Long;
    unsigned char *UChar;
    unsigned short *UShort;
    unsigned int *UInt;
    unsigned long *ULong;
    signed __int64 *Int64;
    unsigned __int64 *UInt64;
    TClassDblFunc Method;
    TDblFunc Function;
    cmp *Complex;
    float *Float;
    double *Double;
    long double *LongDouble;
  };
  TDataType FType;
  TCompConv FCompConv;
public:
	__property TDataType Type = {read=FType};
	__property TCompConv CompConv = {read=FCompConv, write=FCompConv};
  __fastcall TPointer::TPointer()
  { FType = dtUndef; FCompConv = ccReal; Method = NULL; }
  TPointer &__fastcall operator = (const TPointer &P)
  { Method = P.Method; FType = P.FType; return *this; }
  __fastcall TPointer::TPointer(signed char *Value)
  { Char = Value; FType = dtChar; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(signed short *Value)
  { Short = Value; FType = dtShort; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(signed int *Value)
  { Int = Value; FType = dtInt; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(signed long *Value)
  { Long = Value; FType = dtLong; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(unsigned char *Value)
  { UChar = Value; FType = dtUChar; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(unsigned short *Value)
  { UShort = Value; FType = dtUShort; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(unsigned int *Value)
  { UInt = Value; FType = dtUInt; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(unsigned long *Value)
  { ULong = Value; FType = dtULong; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(signed __int64 *Value)
  { Int64 = Value; FType = dtInt64; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(unsigned __int64 *Value)
  { UInt64 = Value; FType = dtUInt64; FCompConv = ccReal; }
	__fastcall TPointer::TPointer(TClassDblFunc Value)
  { Method = Value; FType = dtMethod; FCompConv = ccReal; }
	__fastcall TPointer::TPointer(TDblFunc Value)
  { Function = Value; FType = dtFunction; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(cmp *Value)
  { Complex = Value; FType = dtComplex; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(float *Value)
  { Float = Value; FType = dtFloat; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(double *Value)
  { Double = Value; FType = dtDouble; FCompConv = ccReal; }
  __fastcall TPointer::TPointer(long double* Value)
  { LongDouble = Value; FType = dtLongDouble; FCompConv = ccReal; }
  __fastcall operator signed char*() const { return Char; }
  __fastcall operator signed short*() const { return Short; }
  __fastcall operator signed int*() const { return Int; }
  __fastcall operator signed long*() const { return Long; }
  __fastcall operator unsigned char*() const { return UChar; }
  __fastcall operator unsigned short*() const { return UShort; }
  __fastcall operator unsigned int*() const { return UInt; }
  __fastcall operator unsigned long*() const { return ULong; }
  __fastcall operator signed __int64*() const { return Int64; }
  __fastcall operator unsigned __int64*() const { return UInt64; }
	__fastcall operator cmp*() const { return Complex; }
  __fastcall operator TDblFunc() const { return Function; }
  __fastcall operator TClassDblFunc() const { return Method; }
  __fastcall operator float*() const { return Float; }
  __fastcall operator double*() const { return Double; }
  __fastcall operator long double*() const { return LongDouble; }
  long double __fastcall operator[](int i) const
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
      case dtMethod: return Method(i);
      default: return NULL;
    }
  }
};

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

//! @}

} // namespace Mxbase

using namespace Mxbase;
#endif
