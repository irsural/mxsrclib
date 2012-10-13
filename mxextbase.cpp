//! \file
//! \ingroup graphical_user_interface_group
//! \brief Полезные функции для C++ Builder
//!
//! Дата: 14.04.2010\n
//! Дата создания: 9.09.2009
//---------------------------------------------------------------------------
#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <mxextbase.h>
#include <irsstrdefs.h>

#include <irsfinal.h>

#ifndef MXEXT_OFF
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
#if defined(QT_CORE_LIB) || defined(__BORLANDC__)


namespace irs {

struct color_const_to_rgba_collation_t {
  vector<irs_u32> collation;
  color_const_to_rgba_collation_t(): collation(cl_size) {
    collation[cl_white]        = 0xFFFFFFFF;
    collation[cl_black]        = 0xFF000000;
    collation[cl_red]          = 0xFFff0000;
    collation[cl_dark_red]     = 0xFF800000;
    collation[cl_green]        = 0xFF00ff00;
    collation[cl_dark_green]   = 0xFF008000;
    collation[cl_blue]         = 0xFF0000ff;
    collation[cl_dark_blue]    = 0xFF000080;
    collation[cl_cyan]         = 0xFF00ffff;
    collation[cl_dark_cyan]    = 0xFF008080;
    collation[cl_magenta]      = 0xFFff00ff;
    collation[cl_dark_magenta] = 0xFF800080;
    collation[cl_yellow]       = 0xFFffff00;
    collation[cl_dark_yellow]  = 0xFF808000;
    collation[cl_gray]         = 0xFFa0a0a4;
    collation[cl_dark_gray]    = 0xFF808080;
    collation[cl_light_gray]   = 0xFFc0c0c0;
    collation[cl_transparent]  = 0x00000000;
    collation[cl_cream]        = 0xFFFFFBF0;
    collation[cl_money_green]  = 0xFFC0DCC0;
    collation[cl_sky_blue]     = 0xFFA6CAF0;
  }
};

mx_ext_chart_types::size_type mx_ext_chart_last()
{
  return static_cast<mx_ext_chart_types::size_type>(-1);
}

} //namespace irs

color_union_t color_union_from_color_const(color_const_t a_color_const)
{
  static color_const_to_rgba_collation_t col;
  color_union_t color_union = zero_struct_t<color_union_t>::get();
  color_union->color = col.collation[a_color_const];
  return color_union;
}
color_union_t color_union_from_rgba(irs_u8 a_red = 255, irs_u8 a_green = 255,
  irs_u8 a_blue = 255, irs_u8 a_alpha = 255)
{
  color_union_t color_union = zero_struct_t<color_union_t>::get();
  color_union.red = a_red;
  color_union.green = a_green;
  color_union.blue = a_blue;
  color_union.alpha = a_alpha;
  return color_union;
}

irs::color_t::color_t(irs_u8 a_red = 255, irs_u8 a_green = 255,
  irs_u8 a_blue = 255, irs_u8 a_alpha = 255
):
  m_color_union(color_union_from_rgba(a_red, a_green, a_blue, a_alpha))
{
}
irs::color_t::color_t(color_const_t a_color_const):
  m_color_union(color_union_from_color_const(a_color_const))
{
}
#ifdef NOP
irs::color_t::color_t(const native_color_type& a_native_color):
  m_color_union(native_gui_lib_stuff_t::color_from_native(a_native_color)),
  mp_native_color(new native_color_type(a_native_color))
{
}
#endif //NOP
void assign_rgba(irs_u8 a_red = 255, irs_u8 a_green = 255,
  irs_u8 a_blue = 255, irs_u8 a_alpha = 255)
{
  color_t color(a_red, a_green, a_blue, a_alpha);
  swap(*this, color);
}
void irs::color_t::operator=(color_const_t a_color_const)
{
  color_t color(a_color_const);
  swap(*this, color);
}
#ifdef NOP
void irs::color_t::operator=(const native_color_type& a_native_color)
{
  color_t color(a_native_color);
  swap(*this, color);
}
#endif //NOP
void irs::color_t::swap(color_t& a_color)
{
  ::swap(m_color_union, a_color.m_color_union);
}
void swap(color_t& a_color_left, color_t& a_color_right)
{
  a_color_left.swap(a_color_right);
}
#ifdef NOP
void irs::color_t::get_native_color(native_color_type* ap_native_color)
{
  ap_native_color = mp_native_color.get();
}
#endif //NOP
irs_u8 irs::color_t::red() const
{
  return m_color_union.red;
}
irs_u8 irs::color_t::green() const
{
  return m_color_union.green;
}
irs_u8 irs::color_t::blue() const
{
  return m_color_union.blue;
}
irs_u8 irs::color_t::alpha() const
{
  return m_color_union.alpha;
}
void irs::color_t::red(irs_u8 a_red)
{
  m_color_union.red = a_red;
}
void irs::color_t::green(irs_u8 a_green)
{
  m_color_union.green = a_green;
}
void irs::color_t::blue(irs_u8 a_blue)
{
  m_color_union.blue = a_blue;
}
void irs::color_t::alpha(irs_u8 a_alpha)
{
  m_color_union.blue = a_alpha;
}
#ifdef NOP
void irs::color_t::update_native()
{
  native_gui_lib_stuff_t::color_to_native(m_color_union,
    mp_native_color.get());
}
#endif //NOP

irs::brush_t::brush_t(const color_t& a_color = color_t()):
  m_color(a_color)
{
}
irs::color_t irs::brush_t::color()
{
  return m_color;
}
void irs::brush_t::color(const color_t& a_color)
{
  m_color = a_color;
}

irs::pen_t::pen_t(const color_t& a_color, pen_style_t a_style):
  m_color(a_color),
  m_style(a_style),
  mp_native_pen(IRS_NULL)
{
}
irs::color_t irs::pen_t::color()
{
  return m_color;
}
void irs::pen_t::color(const color_t& a_color)
{
  m_color = a_color;
}
irs::pen_style_t irs::pen_t::style()
{
  return m_style;
}
void irs::pen_t::style(pen_style_t a_style)
{
  m_style = a_style;
}

#ifdef __BORLANDC__
irs::cbuilder_canvas_t::cbuilder_canvas_t(TCanvas* ap_canvas):
  mp_canvas(ap_canvas)
{
}
//! \brief Чтение текущего пера
irs::pen_t irs::cbuilder_canvas_t::pen()
{
  color_t color = color_from_native(mp_canvas->Pen->Color);
  pen_style_t style = pen_style_from_native(mp_canvas->Pen->Style);
  pen_t pen(color, style);
  return pen;
}
//! \brief Установка текущего пера
void irs::cbuilder_canvas_t::pen(const pen_t& a_pen)
{
  mp_canvas->Pen->Color = color_to_native(a_pen.color());
  mp_canvas->Pen->Style = pen_style_to_native(a_pen.style());
}
//! \brief Чтение текущей кисти
irs::brush_t irs::cbuilder_canvas_t::brush()
{
  color_t color = color_from_native(mp_canvas->Brush->Color);
  brush_t brush(color);
  return brush;
}
//! \brief Установка текущей кисти
void irs::cbuilder_canvas_t::brush(const brush_t& a_brush)
{
  mp_canvas->Brush->Color = color_to_native(a_brush.color());
  mp_canvas->Pen->Brush->Color = mp_canvas->Brush->Color;
}
//! \brief Рисует линию текущим пером (pen)
void irs::cbuilder_canvas_t::line(const line_type& a_chart_line)
{
  mp_canvas->MoveTo(a_chart_line.begin.left, a_chart_line.begin.top);
  mp_canvas->LineTo(L.End.x, L.End.y);
  mp_canvas->Pixels[P.x][P.y] = Canvas->Pen->Color;
}
//! \brief Заполняет холст текущей кистью (brush)
void irs::cbuilder_canvas_t::clear()
{
}
struct builder_color_union_t {
  struct {
    irs_u8 red;
    irs_u8 green;
    irs_u8 blue;
    irs_u8 spec;
  };
  irs_u32 color;
};
irs::color_t irs::cbuilder_canvas_t::color_from_native(TColor a_native_color)
{
  const builder_color_union_t& native_color =
    reinterpret_cast<const builder_color_union_t&>(a_native_color);
  color_t color_ret(native_color.red, native_color.green,
    native_color.blue);
  return color_ret;
}
TColor irs::cbuilder_canvas_t::color_to_native(color_t a_color)
{
  builder_color_union_t native_color =
    irs::zero_struct_t<builder_color_union_t>::get();
  native_color.red= a_color.red;
  native_color.green = a_color.green;
  native_color.blue = a_color.blue;
  return reinterpret_cast<TColor&>(*ap_native_color);
}
irs::pen_style_t irs::cbuilder_canvas_t::pen_style_from_native(
  TPenStyle a_native_style)
{
  pen_style_t style = ps_solid;
  switch(a_native_style) {
    case psSolid: style = ps_solid; break;
    case psDash: style = ps_dash; break;
    case psDot: style = ps_dot; break;
    case psDashDot: style = ps_dash_dot; break;
    default: style = ps_solid; break;
  }
  return style;
}
TPenStyle irs::cbuilder_canvas_t::pen_style_to_native(pen_style_t a_style)
{
  pen_style_t native_style = ps_solid;
  switch(a_style) {
    case ps_solid: native_style = psSolid; break;
    case ps_dash: native_style = psDash; break;
    case ps_dot: native_style = psDot; break;
    case ps_dash_dot: native_style = psDashDot; break;
    default: native_style = psSolid; break;
  }
  return native_style;
}
#endif //__BORLANDC__

#ifdef NOP
// Копирование свойств формы
void irs::mxcopy(TForm *dest, TForm *src)
{
  dest->Left = src->Left;
  dest->Top = src->Top;
  dest->Width = src->Width;
  dest->Height = src->Height;
  dest->Position = src->Position;
  dest->Caption = src->Caption;
}


//***************************************************************************
// Глобальные переменные

String AppName = Application->Title;

//***************************************************************************
// Глобальные функции

//--------------------------------------------------------------------------
void __fastcall Point(TCanvas *Canvas, TPoint P)
{
  Canvas->Pixels[P.x][P.y] = Canvas->Pen->Color;
}
//---------------------------------------------------------------------------
// Простая линия
void __fastcall Line(TCanvas *Canvas, TPoint P1, TPoint P2)
{
  TRect ClipRect = Canvas->ClipRect;
  if (((P1.x < ClipRect.Left) && (P2.x < ClipRect.Left)) ||
    ((P1.x >= ClipRect.Right) && (P2.x >= ClipRect.Right)) ||
       ((P1.y < ClipRect.Top) && (P2.y < ClipRect.Top)) ||
   ((P1.y >= ClipRect.Bottom) && (P2.y >= ClipRect.Bottom)))
    return;
  Canvas->MoveTo(P1.x, P1.y); Canvas->LineTo(P2.x, P2.y);
  Canvas->Pixels[P2.x][P2.y] = Canvas->Pen->Color;
}
//---------------------------------------------------------------------------
// Прерывистая линия. Пространство между черточками не заполняется.
void __fastcall DashLine(TCanvas *Canvas, TPoint P1, TPoint P2, int Dash,
  int Space)
{
  if (!Dash) return;
  TRect ClipRect = Canvas->ClipRect;
  if (((P1.x < ClipRect.Left) && (P2.x < ClipRect.Left)) ||
    ((P1.x >= ClipRect.Right) && (P2.x >= ClipRect.Right)) ||
       ((P1.y < ClipRect.Top) && (P2.y < ClipRect.Top)) ||
   ((P1.y >= ClipRect.Bottom) && (P2.y >= ClipRect.Bottom)))
    return;
  if (!Space) Line(Canvas, P1, P2);
  int x1 = P1.x, x2 = P2.x, y1 = P1.y, y2 = P2.y;
  int Period = Dash + Space;
  int rx = abs(x2 - x1), ry = abs(y2 - y1);
  if (rx == 0 && ry == 0) {
    Canvas->MoveTo(x1, y1); Canvas->LineTo(x1 + 1, y1);
  } else if (rx > ry) {
    double k = (y2 - y1)/double(x2 - x1);
    int s = (x2 - x1 >= 0)?1:-1;
    int n = rx/Period;
    int m = rx%Period;
    for (int i=0; i<n; i++)
    {
      Canvas->MoveTo(x1 + s*i*Period, static_cast<int>(y1 + s*i*Period*k));
      Canvas->LineTo(x1 + s*(i*Period + Dash),
        static_cast<int>(y1 + s*(i*Period + Dash)*k));
    }
    int Frec = (m > Dash)?Dash:m;
    Canvas->MoveTo(x1 + s*n*Period, static_cast<int>(y1 + s*n*Period*k));
    Canvas->LineTo(x1 + s*(n*Period + Frec),
      static_cast<int>(y1 + s*(n*Period + Frec)*k));
  }
  else
  {
    double k = (x2 - x1)/double(y2 - y1);
    int s = (y2 - y1 >= 0)?1:-1;
    int n = ry/Period;
    int m = ry%Period;
    for (int i=0; i<n; i++)
    {
      Canvas->MoveTo(static_cast<int>(x1 + s*i*Period*k), y1 + s*i*Period);
      Canvas->LineTo(static_cast<int>(x1 + s*(i*Period + Dash)*k),
        y1 + s*(i*Period + Dash));
    }
    int Frec = (m > Dash)?Dash:m;
    Canvas->MoveTo(static_cast<int>(x1 + s*n*Period*k), y1 + s*n*Period);
    Canvas->LineTo(static_cast<int>(x1 + s*(n*Period + Frec)*k),
      y1 + s*(n*Period + Frec));
  }
  // Рисование по точкам
  /*if (!Dash) return;
  TColor Color = Canvas->Pen->Color;
  int x1 = P1.x, x2 = P2.x, y1 = P1.y, y2 = P2.y;
  int D = 0, S = 0, s; double k;
  bool Draw = true;
  if (x2 - x1 > y2 - y1)
  {
    k = (y2 - y1)/double(x2 - x1);
    s = (x2 - x1 > 0)?1:-1;
    for (int i=0; i<=abs(x2 - x1); i++)
    {
      if (D > Dash) { D = 0; Draw = false; }
      if (S > Space) { S = 0; Draw = true; }
      if (Draw) Canvas->Pixels[int(x1 + s*i)][int(y1 + s*i*k)] = Color;
      if (Draw) D++; else S++;
    }
  }
  else
  {
    k = (x2 - x1)/double(y2 - y1);
    s = (y2 - y1 > 0)?1:-1;
    for (int i=0; i<=abs(y2 - y1); i++)
    {
      if (D > Dash) { D = 0; Draw = false; }
      if (S > Space) { S = 0; Draw = true; }
      if (Draw) Canvas->Pixels[int(x1 + s*i*k)][int(y1 + s*i)] = Color;
      if (Draw) D++; else S++;
    }
  }*/
}
//---------------------------------------------------------------------------
// Ломанная линия
void __fastcall PolyLine(TCanvas *Canvas, TPoint *Points, int LastPoint)
{
  for (int i = 0; i < LastPoint; i++) Line(Canvas, Points[i], Points[i + 1]);
}
//---------------------------------------------------------------------------
// Прерывистая ломанная линия
void __fastcall DashPolyLine(TCanvas *Canvas, TPoint *Points, int LastPoint,
  int Dash, int Space)
{
  for (int i = 0; i < LastPoint; i++)
    DashLine(Canvas, Points[i], Points[i + 1], Dash, Space);
}
//---------------------------------------------------------------------------
// Прямоугольная рамка
void __fastcall RectLine(TCanvas *Canvas, TRect Rect)
{
  TPoint Points[] = {Point(Rect.Left, Rect.Top),
    Point(Rect.Right - 1, Rect.Top),
    Point(Rect.Right - 1, Rect.Bottom - 1), Point(Rect.Left, Rect.Bottom - 1),
    Point(Rect.Left, Rect.Top)};
  PolyLine(Canvas, Points, 4);
}
//---------------------------------------------------------------------------
// Прерывистая прямоугольная рамка
void __fastcall DashRectLine(TCanvas *Canvas, TRect Rect, int Dash, int Space)
{
  TPoint Points[] = {Point(Rect.Left, Rect.Top),
    Point(Rect.Right - 1, Rect.Top),
    Point(Rect.Right - 1, Rect.Bottom - 1), Point(Rect.Left, Rect.Bottom - 1),
    Point(Rect.Left, Rect.Top)};
  DashPolyLine(Canvas, Points, 4, Dash, Space);
}
//---------------------------------------------------------------------------
// Создает прямоугольную отсекающую область
void __fastcall SetClipRect(TCanvas *Canvas, TRect Rect)
{
  HRGN RectRgn = CreateRectRgn(Rect.Left, Rect.Top, Rect.Right, Rect.Bottom);
  SelectClipRgn(Canvas->Handle, RectRgn);
  DeleteObject(RectRgn);
}
//---------------------------------------------------------------------------
// Удаляет отсекающую область
void __fastcall DeleteClipRect(TCanvas *Canvas)
{
  SelectClipRgn(Canvas->Handle, NULL);
}
//---------------------------------------------------------------------------
TSize __fastcall TextExtent(TCanvas *Canvas, String Text)
{
  TSize Size;
  GetTextExtentPoint32(Canvas->Handle, Text.c_str(),
    Text.Length(), (LPSIZE)&Size);
  return Size;
}
//---------------------------------------------------------------------------
int __fastcall TextWidth(TCanvas *Canvas, String Text)
{
  return TextExtent(Canvas, Text).cx;
}
//---------------------------------------------------------------------------
int __fastcall TextHeight(TCanvas *Canvas, String Text)
{
  return TextExtent(Canvas, Text).cy;
}
//---------------------------------------------------------------------------
TDblPoint __fastcall DblPoint(double x, double y)
{ TDblPoint Temp; Temp.x = x; Temp.y = y; return Temp; }
//---------------------------------------------------------------------------
TDblRect __fastcall DblRect(double Left, double Top, double Right,
  double Bottom)
{
  TDblRect Temp;
  Temp.Left = Left; Temp.Top = Top; Temp.Right = Right; Temp.Bottom = Bottom;
  return Temp;
}
//---------------------------------------------------------------------------
bool __fastcall GetWaveFormat(String FileName, TWaveFormat &WaveFormat)
{
  HMMIO       hmmio;              // дескриптор файла для открытия
  MMCKINFO    mmckinfoParent;     // информация о родительском куске
  MMCKINFO    mmckinfoSubchunk;   // структура с информацией о подкуске
  int         FmtSize;            // размер куска "fmt"
  WAVEFORMATEX *Format;           // стуктура содержащая формат
  hmmio = mmioOpen(FileName.c_str(), NULL,
    MMIO_READ | MMIO_ALLOCBUF);
  if(!hmmio) return false;
  mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
  if (mmioDescend(hmmio, (LPMMCKINFO) &mmckinfoParent, NULL, MMIO_FINDRIFF))
  {
    mmioClose(hmmio, 0);
    return false;
  }
  mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
  if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, MMIO_FINDCHUNK))
  {
    mmioClose(hmmio, 0);
    return false;
  }
  FmtSize = mmckinfoSubchunk.cksize;
  Format = (WAVEFORMATEX *)malloc(FmtSize);
  if (mmioRead(hmmio, (HPSTR)Format, FmtSize) != FmtSize)
  {
    mmioClose(hmmio, 0);
    return false;
  }
  WaveFormat.FormatTag = Format->wFormatTag;
  WaveFormat.Channels = Format->nChannels;
  WaveFormat.Bits = Format->wBitsPerSample;
  WaveFormat.Frecuency = Format->nSamplesPerSec;
  WaveFormat.Speed = Format->nAvgBytesPerSec;
  WaveFormat.Sample = WaveFormat.Channels*WaveFormat.Bits/8;
  mmioAscend(hmmio, &mmckinfoSubchunk, 0);
  mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
  if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, MMIO_FINDCHUNK))
  {
    mmioClose(hmmio, 0);
    return false;
  }
  WaveFormat.Size = mmckinfoSubchunk.cksize;;
  WaveFormat.Pos = mmckinfoSubchunk.dwDataOffset;
  return true;
}
//---------------------------------------------------------------------------
bool __fastcall GetPCMWaveData(String FileName, TWaveFormat &WaveFormat,
  short *&Data, unsigned &Begin, unsigned &Size, int Channel)
{
  HMMIO       hmmio;              // дескриптор файла для открытия
  DWORD DataSize, WaveSize;
  if (!Size)
  {
    Data = (short *)realloc(Data, 0);
    return true;
  }
  if ((WaveFormat.FormatTag != 1) || ((WaveFormat.Bits != 8) &&
    (WaveFormat.Bits != 16))) return false;
  WaveSize = WaveFormat.Size/WaveFormat.Sample;
  if (Begin + Size > WaveSize)
    if (WaveSize > Size) Begin = WaveSize - Size;
    else { Begin = 0; Size = WaveSize; }
  Data = (short *)realloc(Data, 0);
  Data = (short *)realloc(Data, Size*sizeof(short));
  DataSize = Size*WaveFormat.Sample;
  hmmio = mmioOpen(FileName.c_str(), NULL,
    MMIO_READ | MMIO_ALLOCBUF);
  if(!hmmio) return false;

  unsigned BufSize = 256*1024;
  unsigned char *Buf = new unsigned char[BufSize];
  unsigned Frac = DataSize%BufSize;
  unsigned Count = DataSize/BufSize;
  mmioSeek(hmmio, WaveFormat.Pos, SEEK_SET);
  for (unsigned i = 0; i <= Count; i++)
  {
    unsigned PartSize = (i != Count)?BufSize:Frac;
    mmioRead(hmmio, (HPSTR)Buf, PartSize);
    unsigned Sample = WaveFormat.Sample;
    int j0 = Channel*WaveFormat.Bits/8;
    if (WaveFormat.Bits == 8)
      for (unsigned j = j0; j < PartSize; j += Sample)
        Data[(j + i*BufSize)/Sample] = short(Buf[j] - 128);
    else
      for (unsigned j = j0; j < PartSize; j += Sample)
        Data[(j + i*BufSize)/Sample] = short(Buf[j] | (Buf[j + 1] << 8));
  }
  delete []Buf;

  mmioClose(hmmio, 0);
  return true;
}
//---------------------------------------------------------------------------
// Центрирование указанной формы относительно главной
void __fastcall FormCentered(TForm *Form)
{
  TForm *MainForm = Application->MainForm;
  if (!MainForm || !Form) return;
  int LeftMain = MainForm->Left, TopMain = MainForm->Top;
  int WidthMain = MainForm->Width, HeightMain = MainForm->Height;
  Form->Left = LeftMain + WidthMain/2 - Form->Width/2;
  Form->Top = TopMain + HeightMain/2 - Form->Height/2;
}
//---------------------------------------------------------------------------
// Центрирование указанной формы относительно активной
void __fastcall CenteredOfActiveForm(TForm *Form)
{
  if (!Form) return;
  bool Desktop = false;
  TForm *ActForm = Screen->ActiveForm;
  if (Form == ActForm) return;
  int Left, Top, Width, Height;
  if (!ActForm) Desktop = true;
  if (Desktop)
  {
    Left = Screen->DesktopLeft; Top = Screen->DesktopTop;
    Width = Screen->DesktopWidth; Height = Screen->DesktopHeight;
  }
  else
  {
    Left = ActForm->Left; Top = ActForm->Top;
    Width = ActForm->Width; Height = ActForm->Height;
  }
  Form->Left = Left + Width/2 - Form->Width/2;
  Form->Top = Top + Height/2 - Form->Height/2;
}
//---------------------------------------------------------------------------
TModalResult __fastcall ShowMsg(String Caption, String Text,
  TMsgDlgType DlgType, TMsgDlgButtons Buttons)
{
  TModalResult ModalResult = mrNone; // Если произошло исключение
  TForm *MsgDlg = CreateMessageDialog(Text, DlgType, Buttons);
  if (!MsgDlg) return ModalResult;
  try
  {
    MsgDlg->Caption = Caption;
    CenteredOfActiveForm(MsgDlg);
    ModalResult = MsgDlg->ShowModal();
  }
  catch (...) {}
  delete MsgDlg;
  return ModalResult;
}
//---------------------------------------------------------------------------
TModalResult __fastcall ShowMsg(String Text, TMsgDlgButtons Buttons)
{
  if (!AppName.IsEmpty()) return ShowMsg(AppName, Text, mtCustom, Buttons);
  else return ShowMsg(Application->Title, Text, mtCustom, Buttons);
}
//---------------------------------------------------------------------------
void __fastcall ShowMsg(String Text)
{
  if (!AppName.IsEmpty())
    ShowMsg(AppName, Text, mtCustom, TMsgDlgButtons () << mbOK);
  else ShowMsg(Application->Title, Text, mtCustom, TMsgDlgButtons () << mbOK);
}
//---------------------------------------------------------------------------
void __fastcall Line(TCanvas *Canvas, TLine L)
{
  Canvas->MoveTo(L.Begin.x, L.Begin.y);
  Canvas->LineTo(L.End.x, L.End.y);
}
//---------------------------------------------------------------------------
void __fastcall LineP(TCanvas *Canvas, TLine L)
{
  Line(Canvas, L); Point(Canvas, L.End);
}
//---------------------------------------------------------------------------
bool FltEqual(long double x1, long double x2, long double eps)
{
  long double y1 = min(x1, x2);
  long double y2 = max(x1, x2);
  bool Equal;
  if (y2 == 0) Equal = y1 == 0;
  else Equal = (y2 - y1)/y2 < eps;
  return Equal;
}
//---------------------------------------------------------------------------
int __fastcall Align2(int Number, int Shift)
{
  if (Number <= 0) return 0;
  int k = 0; for (int i = Number - 1; i >>= 1;) k++;
  return 1 << (k + Shift);
}
//---------------------------------------------------------------------------

//***************************************************************************
// TPrinterDC - создает контекст устройства принтера по умолчанию

//---------------------------------------------------------------------------
// Переведено с Delphi на C++Builder
// Предположительно возвращает указатель первый элемент из списка разделенного
// запятыми на который уазывает Str без лидирующих пробелов и модифицирует Str
// так, чтобы она указывала на следующий элемент.
irs::char_t *__fastcall FetchStr(irs::char_t *&Str)
{
  irs::char_t *P, *Result;
  Result = Str;
  if (Str == NULL) return(Result);
  P = Str;
  while (*P == irst(' ')) P++;
  Result = P;
  while ((*P != 0) && (*P != irst(','))) P++;
  if (*P == irst(',')) { *P = 0; P++; }
  Str = P;
  return Result;
}
//---------------------------------------------------------------------------
TPrinterDC::TPrinterDC()
{
  DWORD ByteCnt, StructCnt;
  irs::char_t DefaultPrinter[79];
  irs::char_t *Cur, *Device;
  PPRINTER_INFO_5 PrinterInfo;
  ByteCnt = 0;
  StructCnt = 0;
  if (!EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 5, NULL, 0, &ByteCnt,
    &StructCnt) && (GetLastError() != ERROR_INSUFFICIENT_BUFFER)) return;
  PrinterInfo = (PPRINTER_INFO_5)AllocMem(ByteCnt);
  EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 5, (LPBYTE)PrinterInfo, ByteCnt,
    &ByteCnt, &StructCnt);
  if (StructCnt > 0)
    Device = PrinterInfo->pPrinterName;
  else
  {
    GetProfileString(irst("windows"), irst("device"), irst(""), DefaultPrinter,
      sizeof(DefaultPrinter) - 1);
    Cur = DefaultPrinter;
    Device = FetchStr(Cur);
  }
  SysFreeMem(PrinterInfo);
  FDevice = Device;
  PDC = CreateDC(NULL, Device, NULL, NULL);
}
//---------------------------------------------------------------------------
TPrinterDC::~TPrinterDC()
{
  if (PDC) DeleteDC(PDC);
}
//---------------------------------------------------------------------------

#endif //NOP

#endif //defined(QT_CORE_LIB) || defined(__BORLANDC__)
#endif //IRS_FULL_STDCPPLIB_SUPPORT
#endif //MXEXT_OFF
