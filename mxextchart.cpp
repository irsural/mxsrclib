//! \file
//! \ingroup graphical_user_interface_group
//! \brief Построение графиков
//!
//! Дата: 02.03.2011\n
//! Дата создания: 8.09.2008

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <timer.h>
#include <mxdata.h>
#include <mxextchart.h>

#include <irsfinal.h>

#ifndef MXEXT_OFF
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
#if defined(QT_CORE_LIB) || defined(__BORLANDC__)

namespace irs {


const mx_ext_chart_types::float_type mx_ext_chart_inf =
  numeric_limits<mx_ext_chart_types::float_type>::infinity();
const int mx_ext_chart_min_exp =
  numeric_limits<mx_ext_chart_types::float_type>::min_exponent10;
const mx_ext_chart_types::float_type mx_ext_chart_dbl_error = 1e-9;

mx_ext_chart_types::size_type mx_ext_chart_types::last_index_mark()
{
  return static_cast<size_type>(-1);
}

//***************************************************************************
// Глобальные функции

mx_ext_chart_types::size_type BeginChartRange(mx_ext_chart_t *Chart,
  mx_ext_chart_types::size_type Index)
{
  rect_float_type A = Chart->Area;
  mx_ext_chart_item_t *Item = IRS_NULL;
  if (Index == mx_ext_chart_last()) {
    Item = Chart->Last;
  } else {
    Item = Chart->Items[Index];
  }
  bounds_float_type B = Item->Bounds;
  float_type a = Item->ScaleX;
  float_type b = Item->ShiftX;
  float_type L = 0;
  typedef mx_ext_chart_types::size_type fn_size_t;
  if (Item->DataX)
  {
    fn_size_t pixel_begin = static_cast<fn_size_t>(B.Begin);
    fn_size_t pixel_end = static_cast<fn_size_t>(B.End);
    for (fn_size_t i = pixel_begin; i <= pixel_end; i++) {
      if ((Item->DataX[i] - b)/a > A.Left) {
        L = i;
        break;
      } else if (FltEqual((Item->DataX[i] - b)/a, A.Left)) {
        L = i;
        break;
      }
    }
  } else {
    L = (A.Left - b)/a;
  }
  if (L < B.Begin) {
    L = B.Begin;
  }
  return static_cast<fn_size_t>(ceil(L));
}

mx_ext_chart_types::size_type EndChartRange(mx_ext_chart_t *Chart,
  mx_ext_chart_types::size_type Index)
{
  rect_float_type A = Chart->Area;
  mx_ext_chart_item_t *Item = IRS_NULL;
  if (Index == mx_ext_chart_last()) {
    Item = Chart->Last;
  } else {
    Item = Chart->Items[Index];
  }
  bounds_float_type B = Item->Bounds;
  float_type a = Item->ScaleX;
  float_type b = Item->ShiftX;
  float_type L = 0;
  typedef mx_ext_chart_types::size_type fn_size_t;
  if (Item->DataX)
  {
    fn_size_t pixel_begin = static_cast<fn_size_t>(B.Begin);
    fn_size_t pixel_end = static_cast<fn_size_t>(B.End);
    for (fn_size_t i = pixel_begin; i <= pixel_end; i++) {
      if ((Item->DataX[i] - b)/a < A.Right) {
        L = i;
        break;
      } else if (FltEqual((Item->DataX[i] - b)/a, A.Right)) {
        L = i;
        break;
      }
    }
  } else {
    L = (A.Right - b)/a;
  }
  if (L > B.End) {
    L = B.End;
  }
  return static_cast<fn_size_t>(floor(L));
}

#ifdef NOP
double RoundTo2(double x, int Dig2 = 5)
{
  union { double y; __int64 i64; };
  y = x; i64 >> Dig2; i64 << Dig2;
  return y;
}

double AddMant(double x, double am = mx_ext_chart_dbl_error)
{
  int e; double m = frexp(x, &e) + am;
  return ldexp(m, e);
}

double SubMant(double x, double am = mx_ext_chart_dbl_error)
{
  return AddMant(x, -am);
}

bool KeyPress(int VirtKey)
{
  return GetAsyncKeyState(VirtKey) & 0x8000;
}

bool AnyKeyPress()
{
  for(register int i = 0; i < 256; i++) if (KeyPress(i)) return true;
  return false;
}

#if defined(QT_CORE_LIB)
static color_union_t native_gui_lib_stuff_t::color_from_native(
  const native_color_type& a_native_color)
{
  color_union_t color_ret = zero_struct_t<color_union_t>::get();
  color_ret.red = static_cast<irs_u8>(a_native_color.red());
  color_ret.green = static_cast<irs_u8>(a_native_color.green());
  color_ret.blue = static_cast<irs_u8>(a_native_color.blue());
  color_ret.alpha = static_cast<irs_u8>(a_native_color.alpha());
  return color_ret;
}
static void native_gui_lib_stuff_t::color_to_native(color_union_t a_color,
  native_color_type* ap_native_color)
{
  ap_native_color->setRgb(a_color.red, a_color.green, a_color.blue,
    a_color.alpha);
}
static void pen_general_to_native(const pen_t& a_general_pen,
  native_pen_type* ap_native_pen)
{
}
static void pen_native_to_general(const native_pen_type& ap_native_pen,
  pen_t* a_general_pen)
{
}
#elif defined(__BORLANDC__)
struct builder_color_union_t {
  struct {
    irs_u8 red;
    irs_u8 green;
    irs_u8 blue;
    irs_u8 spec;
  };
  irs_u32 color;
};
static color_union_t native_gui_lib_stuff_t::color_from_native(
  const native_color_type* a_native_color)
{
  const builder_color_union_t& native_color =
    reinterpret_cast<const builder_color_union_t&>(a_native_color);
  color_union_t color_ret = zero_struct_t<color_union_t>::get();
  color_ret.red = native_color.red;
  color_ret.green = native_color.green;
  color_ret.blue = native_color.blue;
  color_ret.alpha = 255;
  return color_ret;
}
static void native_gui_lib_stuff_t::color_to_native(color_union_t a_color,
  native_color_type* ap_native_color)
{
  builder_color_union_t& native_color =
    reinterpret_cast<builder_color_union_t&>(*ap_native_color);
  native_color.red= a_color.red;
  native_color.green = a_color.green;
  native_color.blue = a_color.blue;
}
#else //GUI Libs
static color_union_t native_gui_lib_stuff_t::color_from_native(
  const native_color_type*)
{
}
static void native_gui_lib_stuff_t::color_to_native(
  color_union_t, native_color_type*)
{
}
#endif //GUI Libs
#endif //NOP


//***************************************************************************
// mx_ext_chart_item_t - Компонент на mx_ext_chart_t


// Конструктор компонета
mx_ext_chart_item_t::mx_ext_chart_item_t()
  :TComponent(AOwner),
  FHide(true),
  NeedCalculate(true),
  NeedAutoScale(true),
  FError(false),
  IndexMarkerX(0),
  IndexMarkerY(0),
  FMarkerX(NULL),
  FMarkerY(NULL),
  ClipArea(DblRect(-1., 1., -1., 1.)),
  Lines(),
  FCanvas(NULL),
  //FFunc[2]
  //FData[2]
  FArea(DblRect(-1., 1., -1., 1.)),
  FBounds(DblBounds(0., 0.)),
  FBoundsRect(Rect(0, 0, 0, 0)),
  FPrevBoundsRect(Rect(0, 0, 0, 0)),
  FStep(1),
  //bool FAutoScale[2];
  FOnChange(NULL),
  FOnError(NULL),
  //size_type FGroup[2];
  FShiftX(0.),
  FShiftY(0.),
  FScaleX(1.),
  FScaleY(1.),
  FPen(new TPen()),
  FMarkerPen(new TPen()),
  m_on_change(),
  m_data_of_change_event(cctArea)
{
  for (size_type i = 0; i < 2; i++) {
    FFunc[i] = DefFunc;
    FAutoScale[i] = true;
    FData[i] = IRS_NULL;
    FGroup[i] = 0;
  }
}

// Деструктор компонета
mx_ext_chart_item_t::~mx_ext_chart_item_t()
{
  delete FPen;
  FPen = NULL;
  delete FMarkerPen;
  FMarkerPen = NULL;
}

point_int_type mx_ext_chart_item_t::ConvCoor(point_float_type P)
{
  float_type k = 0;
  int_type x = 0;
  int_type y = 0;
  try
  {
    if (FArea.Right != FArea.Left)
      k = (P.x - FArea.Left)/(FArea.Right - FArea.Left);
    x = static_cast<int_type>(BoundsRect.Left + (Width - 1)*k);
  }
  catch (Exception &e)
  { FError = true; if (FOnError) FOnError(this, e, P, 0, cetConvX); }
  try
  {
    if (FArea.Top != FArea.Bottom)
      k = (P.y - FArea.Bottom)/(FArea.Top - FArea.Bottom);
    y = static_cast<int_type>(BoundsRect.Bottom - (Height - 1)*k - 1);
  }
  catch (Exception &e)
  { FError = true; if (FOnError) FOnError(this, e, P, 0, cetConvY); }
  return Point(x, y);
}

void mx_ext_chart_item_t::invalidate()
{
  NeedCalculate = NeedAutoScale = true;
}

point_float_type mx_ext_chart_item_t::XYFunc(float_type t) const
{
  return DblPoint(FFunc[0](t), FFunc[1](t));
}

bool mx_ext_chart_item_t::IntoArea(point_float_type P) const
{
  return (P.x >= ClipArea.Left) && (P.x <= ClipArea.Right) &&
    (P.y >= ClipArea.Bottom) && (P.y <= ClipArea.Top);
}

bool mx_ext_chart_item_t::OutArea(point_float_type P1,
  point_float_type P2) const
{
  return (P1.x < ClipArea.Left) && (P2.x < ClipArea.Left) ||
         (P1.x > ClipArea.Right) && (P2.x > ClipArea.Right) ||
         (P1.y < ClipArea.Bottom) && (P2.y < ClipArea.Bottom) ||
         (P1.y > ClipArea.Top && P2.y > ClipArea.Top);
}

bool mx_ext_chart_item_t::ClipLine(point_float_type &P1, point_float_type &P2) const
{
  if (OutArea(P1, P2)) return false;
  bool P1Into = IntoArea(P1), P2Into = IntoArea(P2);
  if (P1Into && P2Into) return true;
  vector<point_float_type> P;
  if (P1Into) P.push_back(P1);
  if (P2Into) P.push_back(P2);
  float_type x1 = ClipArea.Left;
  float_type x2 = ClipArea.Right;
  float_type y1 = ClipArea.Bottom;
  float_type y2 = ClipArea.Top;
  float_type xs = min(P1.x, P2.x);
  float_type xe = max(P1.x, P2.x);
  float_type ys = min(P1.y, P2.y);
  float_type ye = max(P1.y, P2.y);
  if (P2.x == P1.x && P1.x >= x1 && P1.x <= x2)
  {
    if (P.size() < 2 && y1 >= ys && y1 <= ye) P.push_back(DblPoint(P1.x, y1));
    if (P.size() < 2 && y2 >= ys && y2 <= ye) P.push_back(DblPoint(P1.x, y2));
  }
  else
  if (P2.y == P1.y && P1.y >= y1 && P1.y <= y2)
  {
    if (P.size() < 2 && x1 >= xs && x1 <= xe) P.push_back(DblPoint(x1, P1.y));
    if (P.size() < 2 && x2 >= xs && x2 <= xe) P.push_back(DblPoint(x2, P2.y));
  }
  else
  {
    float_type a = (P2.y - P1.y)/(P2.x - P1.x), b = P1.y - a*P1.x;
    if (P.size() < 2 && y1 >= ys && y1 <= ye)
    {
      P.push_back(DblPoint((y1 - b)/a, y1));
      if (!(P.back().x >= x1 && P.back().x <= x2)) P.pop_back();
    }
    if (P.size() < 2 && y2 >= ys && y2 <= ye)
    {
      P.push_back(DblPoint((y2 - b)/a, y2));
      if (!(P.back().x >= x1 && P.back().x <= x2)) P.pop_back();
    }
    if (P.size() < 2 && x1 >= xs && x1 <= xe)
    {
      P.push_back(DblPoint(x1, a*x1 + b));
      if (!(P.back().y >= y1 && P.back().y <= y2)) P.pop_back();
    }
    if (P.size() < 2 && x2 >= xs && x2 <= xe)
    {
      P.push_back(DblPoint(x2, a*x2 + b));
      if (!(P.back().y >= y1 && P.back().y <= y2)) P.pop_back();
    }
  }
  if (P.size() == 1) { P1 = P[0]; P2 = P[0]; return true; }
  if (P.size() == 2) { P1 = P[0]; P2 = P[1]; return true; }
  return false;
}

void mx_ext_chart_item_t::DoCalculate()
{
  point_int_type P = point_int_type();
  point_int_type OldP = point_int_type();
  point_float_type DP = point_float_type();
  point_int_type OldDP = point_float_type();
  float_type Begin = 0;
  float_type End = 0;
  float_type AW = FArea.Right - FArea.Left;
  float_type AH = FArea.Top - FArea.Bottom;
  int_type B = 0;
  int_type E = 0;
  int_type L = 0;
  ClipArea = DblRect(FArea.Left - 0.1*AW, FArea.Top + 0.1*AH,
    FArea.Right + 0.1*AW, FArea.Bottom - 0.1*AH);
  Lines.clear();
  if (!DataX)
  {
    Begin = max(FStep*floor(FArea.Left/FStep), Bounds.Begin);
    End = min(FStep*ceil(FArea.Right/FStep), Bounds.End);
    B = ConvCoor(DblPoint(Begin, 0)).x;
    E = ConvCoor(DblPoint(End, 0)).x;
    L = E - B + 1;
  }
  else if (!DataY)
  {
    Begin = max(FStep*floor(FArea.Bottom/FStep), Bounds.Begin);
    End = min(FStep*ceil(FArea.Top/FStep), Bounds.End);
    B = ConvCoor(DblPoint(0, Begin)).y;
    E = ConvCoor(DblPoint(0, End)).y;
    L = E - B + 1;
  }
  else
  {
    Begin = Bounds.Begin; End = Bounds.End;
  }
  if (Begin > End) return;
  if (!DataX && (End - Begin)/Step > 2*L)
  {
    float_type dA = (End - Begin)/L;
    float_type t = Begin, y = 0.;
    for(size_type i = 0; i < L; i++)
    {
      point_int_type P1, P2;
      float_type t2 = Begin + (i + 1)*dA - Step/4;
      FError = false;
      float_type Max = FFunc[1](t);
      float_type Min = Max;
      t += Step;
      if (FError) break;
      for(; t < t2; t += Step)
      {
        FError = false; y = FFunc[1](t); if (FError) break;
        Max = max(Max, y); Min = min(Min, y);
      }
      if (FError) break;
      if (!(Max > FArea.Top && Min > FArea.Top || Max < FArea.Bottom &&
        Min < FArea.Bottom))
      {
        Max = min(Max, FArea.Top); Min = max(Min, FArea.Bottom);
        FError = false;
        P1 = ConvCoor(DblPoint(FArea.Left, Max));
        P2 = ConvCoor(DblPoint(FArea.Left, Min));
        if (FError) break;
        P1.x = B + i; P2.x = B + i;
        Lines.push_back(Line(P1, P2));
      }
      float_type s = y;
      float_type e = FFunc[1](t);
      if (!(s > ClipArea.Top && e > ClipArea.Top || s < ClipArea.Bottom &&
        e < ClipArea.Bottom) && i != L - 1)
      {
        s = max(min(s, FArea.Top), FArea.Bottom);
        e = max(min(e, FArea.Top), FArea.Bottom);
        FError = false;
        P1 = ConvCoor(DblPoint(FArea.Left, s));
        P2 = ConvCoor(DblPoint(FArea.Left, e));
        if (FError) break;
        P1.x = B + i; P2.x = B + i + 1;
        Lines.push_back(Line(P1, P2));
      }
    }
  }
  else if (!DataY && (End - Begin)/Step > 2*L)
  {
    float_type dA = (End - Begin)/L;
    float_type t = Begin, x = 0.;
    for(size_type i = 0; i < L; i++)
    {
      point_int_type P1, P2;
      float_type t2 = Begin + (i + 1)*dA - Step/4;
      FError = false;
      float_type Max = FFunc[0](t);
      float_type Min = Max;
      if (FError) break;
      for(; t < t2; t += Step)
      {
        FError = false; x = FFunc[0](t); if (FError) break;
        Max = max(Max, x); Min = min(Min, x);
      }
      if (FError) break;
      if (!(Max > ClipArea.Right && Min > ClipArea.Right || Max < ClipArea.Left &&
        Min < ClipArea.Left))
      {
        Max = min(Max, FArea.Right); Min = max(Min, FArea.Left);
        FError = false;
        P1 = ConvCoor(DblPoint(Max, FArea.Bottom));
        P2 = ConvCoor(DblPoint(Min, FArea.Bottom));
        if (FError) break;
        P1.x = E + i; P2.x = E + i;
        Lines.push_back(Line(P1, P2));
      }
      float_type s = x;
      float_type e = FFunc[0](t);
      if (!(s > ClipArea.Right && e > ClipArea.Right || s < ClipArea.Left &&
        e < ClipArea.Left) && i != L - 1)
      {
        s = max(min(s, FArea.Right), FArea.Left);
        e = max(min(e, FArea.Right), FArea.Left);
        FError = false;
        P1 = ConvCoor(DblPoint(s, FArea.Bottom));
        P2 = ConvCoor(DblPoint(s, FArea.Bottom));
        if (FError) break;
        P1.x = E - i; P2.x = E - i - 1;
        Lines.push_back(Line(P1, P2));
      }
    }
  }
  else
  {
    FError = false;
    OldDP = XYFunc(Begin);
    if (FError) return;
    for(float_type t = Begin + FStep; t < End; t += FStep)
    {
      FError = false;
      DP = XYFunc(t);
      point_float_type OldDP1 = OldDP, DP1 = DP;
      if (!ClipLine(OldDP1, DP1)) { OldDP = DP; continue; }
      P = ConvCoor(DP1); OldP = ConvCoor(OldDP1);
      if (FError) return;
      Lines.push_back(Line(OldP, P));
      OldDP = DP;
    }
    FError = false;
    DP = XYFunc(End);
    if (ClipLine(OldDP, DP))
    {
      P = ConvCoor(DP); OldP = ConvCoor(OldDP);
      Lines.push_back(Line(OldP, P));
    }
    if (FError) return;
  }
}

void mx_ext_chart_item_t::Paint(paint_style_t a_paint_style)
{
  if (!ValidRect()) return;
  TCanvas *Dev = FCanvas;
  TPen *OldPen = new TPen(); OldPen->Assign(Dev->Pen);
  // Настройка инструментов для рисования
  Dev->Pen = FPen;
  switch (a_paint_style)
  {
    case ps_black_print:
      Dev->Pen->Color = clBlack;
    case ps_color_print:
      Dev->Pen->Style = psSolid;
      break;
  }
  // Отображение графика
  if ((Lines.size() == 1) && (Lines[0].Begin == Lines[0].End))
  {
    LineP(Dev, Line(Lines[0].End.x - 3, Lines[0].End.y - 3,
      Lines[0].End.x + 3, Lines[0].End.y + 3));
    LineP(Dev, Line(Lines[0].End.x - 3, Lines[0].End.y + 3,
      Lines[0].End.x + 3, Lines[0].End.y - 3));
  }
  else {
    for (size_type i = 0; i < Lines.size(); i++) {
      LineP(Dev, Lines[i]);
    }
  }
  // Отображение маркеров
  for(size_type i = 0; i < IndexMarkerX; i++) {
    PaintMarkerX(i);
  }
  for(size_type i = 0; i < IndexMarkerY; i++) {
    PaintMarkerY(i);
  }
  Dev->Pen = OldPen;
  delete OldPen;
  OldPen = NULL;
}

void mx_ext_chart_item_t::DoPaint()
{
  Paint(0);
}

void mx_ext_chart_item_t::DoBlackPrint()
{
  Paint(1);
}

void mx_ext_chart_item_t::DoColorPrint()
{
  Paint(2);
}

canvas_t& mx_ext_chart_item_t::canvas()
{
  return m_canvas;
}
void mx_ext_chart_item_t::canvas(const canvas_t &a_canvas)
{
  m_canvas = a_canvas;
  if (FOnChange) FOnChange(this, cctCanvas);
}
rect_int_type mx_ext_chart_item_t::bounds_rect() const
{
  ((mx_ext_chart_t *)Owner)->PreCalc();
  return FBoundsRect;
}
void mx_ext_chart_item_t::bounds_rect(const rect_int_type& Value)
{
  if ((Value.right - Value.left != FBoundsRect.right - FBoundsRect.left) ||
      (Value.bottom - Value.top != FBoundsRect.bottom - FBoundsRect.top))
    NeedCalculate = true;
  FPrevBoundsRect = FBoundsRect;
  FBoundsRect = Value;
  if (FOnChange) FOnChange(this, cctBoundsRect);
}
generator_events_t* mx_ext_chart_item_t::on_change()
{
  return &m_on_change;
}
void mx_ext_chart_item_t::data_of_change_event(TChartChangeType a_change_type)
{
  m_data_of_change_event = a_change_type;
}

void mx_ext_chart_item_t::PaintMarkerX(size_type i)
{
  if (!ValidRect()) return;
  mx_ext_chart_t *OwnerChart = (mx_ext_chart_t *)Owner;
  OwnerChart->PreCalc();
  TCanvas *Dev = FCanvas;
  TPen *OldPen = new TPen(); OldPen->Assign(Dev->Pen);
  Dev->Pen = FMarkerPen;
  Dev->Pen->Style = psSolid;
  Dev->Pen->Mode = pmNotXor;
  float_type XCoor = (FMarkerX[i] - FShiftX)/FScaleX;
  if (XCoor < FArea.Left && XCoor > FArea.Right)
    return;
  point_int_type P1 = ConvCoor(DblPoint(XCoor, FArea.Top));
  if (GroupX == OwnerChart->Items[OwnerChart->BaseItem]->GroupX)
  { OwnerChart->PaintChildMarkerX(this, P1.x); return; }
  point_int_type P2 = ConvCoor(DblPoint(XCoor, FArea.Bottom));
  DashLine(Dev, P1, P2, 5, 5);
  Dev->Pen = OldPen;
  delete OldPen;
  OldPen = NULL;
}

void mx_ext_chart_item_t::PaintMarkerY(size_type i)
{
  if (!ValidRect()) return;
  mx_ext_chart_t *OwnerChart = (mx_ext_chart_t *)Owner;
  OwnerChart->PreCalc();
  TCanvas *Dev = FCanvas;
  TPen *OldPen = new TPen(); OldPen->Assign(Dev->Pen);
  Dev->Pen = FMarkerPen;
  Dev->Pen->Style = psSolid;
  Dev->Pen->Mode = pmNotXor;
  float_type YCoor = (FMarkerY[i] - FShiftY)/FScaleY;
  if (YCoor < FArea.Bottom && YCoor > FArea.Top)
    return;
  point_int_type P1 = ConvCoor(DblPoint(FArea.Left, YCoor));
  if (GroupY == OwnerChart->Items[OwnerChart->BaseItem]->GroupY)
  { OwnerChart->PaintChildMarkerY(this, P1.y); return; }
  point_int_type P2 = ConvCoor(DblPoint(FArea.Right, YCoor));
  DashLine(Dev, P1, P2, 5, 5);
  Dev->Pen = OldPen;
  delete OldPen;
  OldPen = NULL;
}

// Представление массивов в виде функций
float_type mx_ext_chart_item_t::Func(idx_t i, float_type x) const
{
  return static_cast<float_type>(FData[i][static_cast<size_type>(x + 0.5)]);
}

// Вспомогательная функция по X
float_type mx_ext_chart_item_t::FuncX(float_type x)
{
  float_type f = 0;
  try { f = Func(0,x); }
  catch (Exception &e)
  {
    FError = true;
    if (FOnError) FOnError(this, e, DblPoint(0, 0), x, cetCalcX);
  }
  return f;
}

// Вспомогательная функция по Y
float_type mx_ext_chart_item_t::FuncY(float_type x)
{
  float_type f = 0;
  try { f = Func(1,x); }
  catch (Exception &e)
  {
    FError = true;
    if (FOnError) FOnError(this, e, DblPoint(0, 0), x, cetCalcY);
  }
  return f;
}

// Установка данных (массивов или функций) для построения
// Выравнивание под массив
void mx_ext_chart_item_t::FloorAxis()
{
  if ((FData[0].Type != dtFunction) || (FData[1].Type != dtFunction) ||
        (FData[0].Type != dtMethod) || (FData[1].Type != dtMethod))
  {
    FBounds.Begin = floor(FBounds.Begin); FBounds.End = floor(FBounds.End);
    FStep = 1;
  }
}

// Выполнение автомасштабирования
void mx_ext_chart_item_t::DoAutoScale()
{
  bool ChangeArea = false;
  float_type t = 0;
  float_type Max = 0;
  float_type Min = 0;
  for(size_type i = 0; i < 2; i++)
  if (FAutoScale[i])
  {
    FError = false;
    Min = Max = FFunc[i](FBounds.Begin);
    if (FError) goto Abnormal_Exit;
    for(t = FBounds.Begin + FStep; t < FBounds.End; t += FStep)
    {
      FError = false;
      if (Max<FFunc[i](t)) Max=FFunc[i](t);
      if (Min>FFunc[i](t)) Min=FFunc[i](t);
      if (FError) goto Abnormal_Exit;
    }
    t = FBounds.End;
    if (Max<FFunc[i](t)) Max=FFunc[i](t);
    if (Min>FFunc[i](t)) Min=FFunc[i](t);
    Abnormal_Exit:
    if (i == 0) { FArea.Left = Min; FArea.Right = Max; }
    else { FArea.Bottom = Min; FArea.Top = Max; }
    ChangeArea = true;
  }
  if (ChangeArea) if (FOnChange) FOnChange(this, cctArea);
}

TCompConv mx_ext_chart_item_t::GetCompConv(idx_t Index) const
{
  return FData[Index].CompConv;
}

// Установка типа конверсии из cmp в float_type
void mx_ext_chart_item_t::SetCompConv(idx_t Index, TCompConv Value)
{
  if ((Value >= ccReal) && (Value <= ccArg))
  if (FData[Index].CompConv != Value)
  {
    FData[Index].CompConv = Value;
    NeedCalculate = NeedAutoScale = true;
    if (FOnChange) FOnChange(this, cctCompConv);
    //** Для mx_ext_chart_select_t **
    mx_ext_chart_t *Chart = (mx_ext_chart_t *)Owner;
    for (size_type i = 0; i < Chart->FCount && Chart->FItems[i] != this; i++) {
      if (Chart->Select) {
        item_change_t item_change = ic_comp_conv_x;
        switch (Index) {
          case x_idx: {
            item_change = ic_comp_conv_x;
          } break;
          case y_idx: {
            item_change = ic_comp_conv_y;
          } break;
        }
        Chart->Select->ItemChange(item_change, i);
      }
    }
    //************************
  }
}

void mx_ext_chart_item_t::group(idx_t Index, size_type a_group_x)
{
  FGroup[Index] = a_group_x;
  if (FOnChange) FOnChange(this, cctGroup);
}
size_type mx_ext_chart_item_t::group_x() const
{
  return FGroup[x_idx];
}
void mx_ext_chart_item_t::group_x(size_type a_group_x)
{
  group(x_idx, a_group_x);
}
size_type mx_ext_chart_item_t::group_y() const
{
  return FGroup[y_idx];
}
void mx_ext_chart_item_t::group_y(size_type a_group_y)
{
  group(y_idx, a_group_y);
}
float_type mx_ext_chart_item_t::shift_x() const
{
  return FShiftX;
}
void mx_ext_chart_item_t::shift_x(const float_type& a_shift_x)
{
  FShiftX = a_shift_x;
}
float_type mx_ext_chart_item_t::shift_y() const
{
  return FShiftY;
}
void mx_ext_chart_item_t::shift_y(const float_type& a_shift_y)
{
  FShiftY = a_shift_y;
}
float_type mx_ext_chart_item_t::scale_x() const
{
  return FScaleX;
}
void mx_ext_chart_item_t::scale_x(const float_type& a_scale_x)
{
  FScaleX = a_scale_x;
}
float_type mx_ext_chart_item_t::scale_y() const
{
  return FScaleY;
}
void mx_ext_chart_item_t::scale_y(const float_type& a_scale_y)
{
  FScaleY = a_scale_y;
}
bool mx_ext_chart_item_t::hide() const
{
  return FHide;
}
void mx_ext_chart_item_t::hide(bool a_hide)
{
  if (FData[x_idx] || FData[y_idx]) {
    FHide = a_hide;
  } else {
    FHide = true;
  }
  if (FOnChange) {
    FOnChange(this, cctHide);
  }
}
void mx_ext_chart_item_t::data(idx_t Index, TPointer Value)
{
  FData[Index] = Value;
  if (static_cast<float_type*>(Value)) {
    FHide = false;
    if ((Value.Type == dtFunction) || (Value.Type == dtMethod))
      FFunc[Index] = Value.operator TClassDblFunc();
    else
      if (Index == 0) FFunc[0] = FuncX; else FFunc[1] = FuncY;
  } else {
    FFunc[Index] = DefFunc;
  }
  NeedCalculate = NeedAutoScale = true;
  FloorAxis();
  if (FOnChange) FOnChange(this, cctData);
}
TPointer mx_ext_chart_item_t::data_x() const
{
  return FData[x_idx];
}
void mx_ext_chart_item_t::data_x(TPointer ap_data_x)
{
  data(x_idx, ap_data_x);
}
TPointer mx_ext_chart_item_t::data_y() const
{
  return FData[y_idx];
}
void mx_ext_chart_item_t::data_y(TPointer ap_data_y)
{
  data(y_idx, ap_data_y);
}
TCompConv mx_ext_chart_item_t::comp_conv_x() const
{
  return GetCompConv(x_idx);
}
void mx_ext_chart_item_t::comp_conv_x(TCompConv a_comp_conv)
{
  SetCompConv(x_idx, a_comp_conv);
}
TCompConv mx_ext_chart_item_t::comp_conv_y() const
{
  return GetCompConv(y_idx);
}
void mx_ext_chart_item_t::comp_conv_y(TCompConv a_comp_conv)
{
  SetCompConv(y_idx, a_comp_conv);
}
bool mx_ext_chart_item_t::auto_scale_x() const
{
  return FAutoScale[x_idx];
}
void mx_ext_chart_item_t::auto_scale_x(bool a_auto_scale_x)
{
  SetAutoScale(x_idx, a_auto_scale_x);
}
bool mx_ext_chart_item_t::auto_scale_y() const
{
  return FAutoScale[y_idx];
}
void mx_ext_chart_item_t::auto_scale_y(bool a_auto_scale_y)
{
  SetAutoScale(y_idx, a_auto_scale_y);
}
float_type mx_ext_chart_item_t::step() const
{
  return FStep;
}
void mx_ext_chart_item_t::step(const float_type& a_step)
{
  if ((a_step > 0) && (FStep != a_step))
  {
    FStep = a_step; FloorAxis();
    NeedCalculate = NeedAutoScale = true;
    if (FOnChange) FOnChange(this, cctStep);
  }
}
bounds_float_type mx_ext_chart_item_t::bounds() const
{
  return FBounds;
}
// Установка пределов построения
void mx_ext_chart_item_t::bounds(const bounds_float_type& a_bounds)
{
  if ((a_bounds.begin < a_bounds.end) && (FBounds != a_bounds))
  {
    FBounds = a_bounds; FloorAxis();
    NeedCalculate = NeedAutoScale = true;
    if (FOnChange) FOnChange(this, cctBounds);
  }
}
rect_float_type mx_ext_chart_item_t::area() const
{
  if (NeedAutoScale) { NeedAutoScale = false; DoAutoScale(); }
  rect_float_type VArea = FArea;
  float_type ScX = FScaleX;
  float_type ScY = FScaleY;
  float_type ShX = FShiftX;
  float_type ShY = FShiftY;
  return DblRect(VArea.Left*ScX + ShX, VArea.Top*ScY + ShY,
    VArea.Right*ScX + ShX, VArea.Bottom*ScY + ShY);
}
// Установка области построения
void mx_ext_chart_item_t::area(const rect_float_type& AArea)
{
  bool AreaChange = false;
  float_type ScX = FScaleX;
  float_type ScY = FScaleY;
  float_type ShX = FShiftX;
  float_type ShY = FShiftY;
  if (AArea.Left > AArea.Right) swap(AArea.Left, AArea.Right);
  if (AArea.Bottom > AArea.Top) swap(AArea.Bottom, AArea.Top);
  AArea = DblRect((AArea.Left - ShX)/ScX, (AArea.Top - ShY)/ScY,
    (AArea.Right - ShX)/ScX, (AArea.Bottom - ShY)/ScY);
  if (!(FltEqual(FArea.Left, AArea.Left) && FltEqual(FArea.Right, AArea.Right)))
  {
    FArea.Left = AArea.Left; FArea.Right = AArea.Right;
    FAutoScale[0] = false;
    NeedCalculate = NeedAutoScale = true;
    AreaChange = true;
  }
  if (!(FltEqual(FArea.Bottom, AArea.Bottom) && FltEqual(FArea.Top, AArea.Top)))
  {
    FArea.Top = AArea.Top; FArea.Bottom = AArea.Bottom;
    FAutoScale[1] = false;
    NeedCalculate = NeedAutoScale = true;
    AreaChange = true;
  }
  if (AreaChange) if (FOnChange) FOnChange(this, cctArea);
}
size_type count_marker_x() const
{
  return IndexMarkerX;
}
size_type count_marker_y() const
{
  return IndexMarkerY;
}
float_type mx_ext_chart_item_t::marker_x(size_type Index) const
{
  if (Index == last_index_mark()) {
    if (IndexMarkerX) {
      return FMarkerX[IndexMarkerX - 1];
    }
  }
  if ((Index >= 0) && (Index < IndexMarkerX)) {
    return FMarkerX[Index];
  }
  return 0;
}
float_type mx_ext_chart_item_t::marker_y(size_type Index) const
{
  if (Index == last_index_mark()) {
    if (IndexMarkerY) return FMarkerY[IndexMarkerY - 1];
  }
  if ((Index >= 0) && (Index < IndexMarkerY)) {
    return FMarkerY[Index];
  }
  return 0;
}
void mx_ext_chart_item_t::marker_x(size_type Index, float_type Value)
{
  if (Index == last_index_mark()) {
    if (IndexMarkerX) {
      Index = IndexMarkerX - 1;
    }
  }
  if ((Index >= 0) && (Index < IndexMarkerX))
  {
    float_type X1 = (FMarkerX[Index] - FShiftX)/FScaleX;
    float_type X2 = (Value - FShiftX)/FScaleX;
    int_type OldPos = ConvCoor(DblPoint(X1, FArea.Top)).x;
    int_type NewPos = ConvCoor(DblPoint(X2, FArea.Top)).x;
    if (NewPos != OldPos)
    { PaintMarkerX(Index); FMarkerX[Index] = Value; PaintMarkerX(Index); }
    else
      FMarkerX[Index] = Value;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
void mx_ext_chart_item_t::marker_y(size_type Index, float_type Value)
{
  if (Index == last_index_mark()) {
    if (IndexMarkerY) {
      Index = IndexMarkerY - 1;
    }
  }
  if ((Index >= 0) && (Index < IndexMarkerY))
  {
    float_type Y1 = (FMarkerY[Index] - FShiftY)/FScaleY;
    float_type Y2 = (Value - FShiftY)/FScaleY;
    int_type OldPos = ConvCoor(DblPoint(FArea.Left, Y1)).y;
    int_type NewPos = ConvCoor(DblPoint(FArea.Left, Y2)).y;
    if (NewPos != OldPos)
    { PaintMarkerY(Index); FMarkerY[Index] = Value; PaintMarkerY(Index); }
    else
      FMarkerY[Index] = Value;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
float_type mx_ext_chart_item_t::first_marker_x()
{
  return marker_x(0);
}
float_type mx_ext_chart_item_t::first_marker_y()
{
  return marker_y(0);
}
void mx_ext_chart_item_t::first_marker_x(const float_type& a_position)
{
  marker_x(0, a_position);
}
void mx_ext_chart_item_t::first_marker_y(const float_type& a_position)
{
  marker_y(0, a_position);
}
float_type mx_ext_chart_item_t::last_marker_x()
{
  return marker_x(last_index_mark());
}
float_type mx_ext_chart_item_t::last_marker_y()
{
  return marker_y(last_index_mark());
}
void mx_ext_chart_item_t::last_marker_x(const float_type& a_position)
{
  marker_x(last_index_mark(), a_position);
}
void mx_ext_chart_item_t::last_marker_y(const float_type& a_position)
{
  marker_y(last_index_mark(), a_position);
}
pen_t mx_ext_chart_item_t::pen()
{
  return FPen;
}
void mx_ext_chart_item_t::pen(const pen_t& a_pen)
{
  FPen = a_pen;
}
pen_t mx_ext_chart_item_t::marker_pen()
{
  return FMarkerPen;
}
void mx_ext_chart_item_t::marker_pen(const pen_t& a_pen)
{
  FMarkerPen = a_pen;
}

// Функция по умолчанию.
float_type mx_ext_chart_item_t::DefFunc(float_type x) const
{
  return x;
}

void mx_ext_chart_item_t::SetShift(size_type Index, float_type Value)
{
  if (Index == 0) ShiftX = Value; else ShiftY = Value;
  if (FOnChange) FOnChange(this, cctShift);
}

void mx_ext_chart_item_t::SetScale(size_type Index, float_type Value)
{
  if (Index == 0) ScaleX = Value; else ScaleY = Value;
  if (FOnChange) FOnChange(this, cctScale);
}

// Установка автомасштабирования
void mx_ext_chart_item_t::SetAutoScale(idx_t Index, bool Value)
{
  if (FAutoScale[Index] != Value)
  {
    FAutoScale[Index] = Value;
    if (Value) NeedCalculate = true;
    NeedAutoScale = true;
    if (FOnChange) FOnChange(this, cctAutoScale);
  }
}

int_type mx_ext_chart_item_t::left() const
{
  return FBoundsRect.Left;
}

int_type mx_ext_chart_item_t::top() const
{
  return FBoundsRect.Top;
}

int_type mx_ext_chart_item_t::width() const
{
  return FBoundsRect.Right - FBoundsRect.Left;
}

int_type mx_ext_chart_item_t::height() const
{
  return FBoundsRect.Bottom - FBoundsRect.Top;
}

void mx_ext_chart_item_t::left(int_type Value)
{
  rect_int_type R = BoundsRect;
  R.Left = Value;
  BoundsRect = R;
}

void mx_ext_chart_item_t::top(int_type Value)
{
  rect_int_type R = BoundsRect;
  R.Top = Value;
  BoundsRect = R;
}

void mx_ext_chart_item_t::width(int_type Value)
{
  rect_int_type R = BoundsRect;
  R.Right = R.Left + Value;
  BoundsRect = R;
}

void mx_ext_chart_item_t::height(int_type Value)
{
  rect_int_type R = BoundsRect;
  R.Bottom = R.Top + Value;
  BoundsRect = R;
}

void mx_ext_chart_item_t::add_marker_x(float_type a_marker_pos)
{
  if (FMarkerX != NULL) {
    FMarkerX = reinterpret_cast<float_type*>(realloc(FMarkerX,
      (IndexMarkerX + 1)*sizeof(float_type)));
  } else {
    FMarkerX = reinterpret_cast<float_type*>(calloc(1, sizeof(float_type)));
  }
  FMarkerX[IndexMarkerX] = a_marker_pos;
  IndexMarkerX++;
  PaintMarkerX(IndexMarkerX - 1);
  if (FOnChange) FOnChange(this, cctMarker);
}

void mx_ext_chart_item_t::add_marker_y(float_type a_marker_pos)
{
  if (FMarkerY != NULL) {
    FMarkerY = reinterpret_cast<float_type*>(realloc(FMarkerY,
      (IndexMarkerY + 1)*sizeof(float_type)));
  } else {
    FMarkerY = reinterpret_cast<float_type*>(calloc(1, sizeof(float_type)));
  }
  FMarkerY[IndexMarkerY] = a_marker_pos;
  IndexMarkerY++;
  PaintMarkerY(IndexMarkerY - 1);
  if (FOnChange) FOnChange(this, cctMarker);
}

void mx_ext_chart_item_t::clear_marker_x()
{
  for (size_type i=0; i<IndexMarkerX; i++) PaintMarkerX(i);
  if (FMarkerX != NULL) {
    free(FMarkerX);
    FMarkerX = NULL;
  }
  IndexMarkerX = 0;
  if (FOnChange) FOnChange(this, cctMarker);
}

void mx_ext_chart_item_t::clear_marker_y()
{
  for (size_type i=0; i<IndexMarkerY; i++) PaintMarkerY(i);
  if (FMarkerY != NULL) {
    free(FMarkerY);
    FMarkerY = NULL;
  }
  IndexMarkerY = 0;
  if (FOnChange) FOnChange(this, cctMarker);
}

void mx_ext_chart_item_t::delete_marker_x(size_type Index)
{
  if ((Index >= 0) && (Index < IndexMarkerX))
  {
    PaintMarkerX(Index);
    for (size_type i=Index; i<IndexMarkerX-1; i++) {
      FMarkerX[i] = FMarkerX[i + 1];
    }
    IndexMarkerX--;
    if (IndexMarkerX != 0) {
      FMarkerX = reinterpret_cast<float_type*>(realloc(FMarkerX,
        IndexMarkerX*sizeof(float_type));
    } else {
      free(FMarkerX);
      FMarkerX = NULL;
    }
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_item_t::delete_marker_y(size_type Index)
{
  if ((Index >= 0) && (Index < IndexMarkerY))
  {
    PaintMarkerY(Index);
    for (size_type i=Index; i<IndexMarkerY-1; i++) {
      FMarkerY[i] = FMarkerY[i + 1];
    }
    IndexMarkerY--;
    if (IndexMarkerY != 0) {
      FMarkerY = reinterpret_cast<float_type*>(realloc(FMarkerY,
        IndexMarkerY*sizeof(float_type));
    } else {
      free(FMarkerY);
      FMarkerY = NULL;
    }
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_item_t::delete_marker_x()
{
  if (IndexMarkerX > 0)
  {
    PaintMarkerX(IndexMarkerX - 1);
    IndexMarkerX--;
    if (IndexMarkerX != 0) {
      FMarkerX = reinterpret_cast<float_type*>(realloc(FMarkerX,
        IndexMarkerX*sizeof(float_type));
    } else {
      free(FMarkerX);
      FMarkerX = 0;
    }
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_item_t::delete_marker_y()
{
  if (IndexMarkerY > 0)
  {
    PaintMarkerY(IndexMarkerY - 1);
    IndexMarkerY--;
    if (IndexMarkerY != 0) {
      FMarkerY = reinterpret_cast<float_type*>(realloc(FMarkerY,
        IndexMarkerY*sizeof(float_type)));
    } else {
      free(FMarkerY);
      FMarkerY = NULL;
    }
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_item_t::delete_marker_x(size_type Begin, size_type End)
{
  if ((Begin >= 0) && (Begin < IndexMarkerX) &&
        (End >= 0) && (End < IndexMarkerX))
  {
    for (size_type i=Begin; i<=End; i++) PaintMarkerX(i);
    for (size_type i=Begin; i<IndexMarkerX-(End-Begin+1); i++)
      FMarkerX[i] = FMarkerX[i + 1];
    IndexMarkerX-=(End - Begin + 1);
    if (IndexMarkerX != 0) {
      FMarkerX = reinterpret_cast<float_type*>(realloc(FMarkerX,
        IndexMarkerX*sizeof(float_type)));
    } else {
      free(FMarkerX);
      FMarkerX = NULL;
    }
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_item_t::delete_marker_y(size_type Begin, size_type End)
{
  if ((Begin >= 0) && (Begin < IndexMarkerY) &&
        (End >= 0) && (End < IndexMarkerY))
  {
    for (size_type i=Begin; i<=End; i++) PaintMarkerY(i);
    for (size_type i=Begin; i<IndexMarkerY-(End-Begin+1); i++)
      FMarkerY[i] = FMarkerY[i + 1];
    IndexMarkerY-=(End - Begin + 1);
    if (IndexMarkerY != 0) {
      FMarkerY = reinterpret_cast<float_type*>(realloc(FMarkerY,
        IndexMarkerY*sizeof(float_type)));
    } else {
      free(FMarkerY);
      FMarkerY = NULL;
    }
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

bool mx_ext_chart_item_t::ValidRect() const
{
  return FBoundsRect.Right - FBoundsRect.Left >= 1 &&
    FBoundsRect.Bottom - FBoundsRect.Top >= 1;
}


//***************************************************************************
// mx_ext_chart_t - Построение графиков


// Часть неизменная для всех констукторов
void mx_ext_chart_t::Constructor()
{
  NeedAutoScale = true;
  NeedCalculate = true;
  NeedRepaint = true;
  FOnPaint = NULL;
  FOnPrint = NULL;
  FOnChange = NULL;
  FOnError = NULL;
  FBitmap = new Graphics::TBitmap();
  FMarkerColor = clBlack;
  IndexMarkerX = 0;
  IndexMarkerY = 0;
  FMarkerX = NULL;
  FMarkerY = NULL;
  FShowGrid = true;
  FShowAuxGrid = true;
  FShowBounds = false;
  FCount = 0;
  FItems = NULL;
  FCanvas = new TCanvas();
  FPaintCanvas = NULL;
  FPrintCanvas = NULL;
  FCurCanvas = NULL;
  FMarkerPen = new TPen();
  FMarkerPen->OnChange = GraphObjChanged;
  FAuxGridPen = new TPen();
  FAuxGridPen->OnChange = GraphObjChanged;
  FAuxGridPen->Color = clSilver;
  FBoundsRect = Rect(0, 0, 0, 0);
  FGridRect = Rect(0, 0, 0, 0);
  FPrintRect = Rect(0, 0, 0, 0);
  FCurRect = Rect(0, 0, 0, 0);
  FAutoScaleX = true;
  FAutoScaleY = true;
  FArea = DblRect(-1., 1., -1., 1.);
  FMinWidthGrid = 1.;
  FMinHeightGrid = 1.;
  FMinWidthAuxGrid = 0.2;
  FMinHeightAuxGrid = 0.2;
  PaintMode = false;
  SW = 0.;
  SH = 0.;
  ApplyTextX = true;
  ApplyTextY = true;
  PenChanged = FCanvas->Pen->OnChange;
  BrushChanged = FCanvas->Brush->OnChange;
  FontChanged = FCanvas->Font->OnChange;
  FBaseItem = 0;
  CurGroupX = 0;
  CurGroupY = 0;
}

// Конструктор компонета
mx_ext_chart_t::mx_ext_chart_t(TComponent *AOwner)
  :TComponent(AOwner)
{
  Constructor();
}

// Конструктор компонета
mx_ext_chart_t::mx_ext_chart_t(TComponent *AOwner, TCanvas *ACanvas)
  :TComponent(AOwner)
{
  Constructor();
  FPaintCanvas = ACanvas;
  FCurCanvas = FPaintCanvas;
}

// Деструктор компонета
mx_ext_chart_t::~mx_ext_chart_t()
{
  if (FItems != NULL) {
    for (size_type i = 0; i < FCount; i++) delete FItems[i];
    free(FItems);
    FItems = NULL;
  }
  delete FBitmap;
  FBitmap = NULL;
  delete FCanvas;
  FCanvas = NULL;
  delete FMarkerPen;
  FMarkerPen = NULL;
  delete FAuxGridPen;
  FAuxGridPen = NULL;
}

void mx_ext_chart_t::Paint()
{
  if (!ValidRect()) return;
  PaintMode = true;
  CurCanvas = FBitmap->Canvas;
  FCurRect = Rect(0, 0, FBitmap->Width, FBitmap->Height);
  TCanvas *Dev = CurCanvas;
  Dev->Pen = FCanvas->Pen;
  Dev->Brush = FCanvas->Brush;
  Dev->Font = FCanvas->Font;
  if (NeedAutoScale) { NeedAutoScale = false; DoAutoScale(); }
  if (NeedCalculate) { NeedCalculate = false; DoCalculate(); }
  if (NeedRepaint) { NeedRepaint = false; DoRepaint(ps_display_paint); }
  FPaintCanvas->Draw(FBoundsRect.Left, FBoundsRect.Top, FBitmap);
  CurCanvas = FPaintCanvas;
  FCurRect = FBoundsRect;
  PaintMode = false;
  if (FOnPaint) FOnPaint(this);
}

void mx_ext_chart_t::SaveToMetafile(String FileName)
{
  if (!ValidRect()) return;
  PaintMode = true;
  TMetafile *emf = new TMetafile();
  int_type W = 32000, H = 16000;
  emf->Width = W; emf->Height = H;
  TMetafileCanvas *emfCanvas = new TMetafileCanvas(emf, 0);
  CurCanvas = emfCanvas;
  FCurRect = Rect(0, 0, W, H);
  TCanvas *Dev = CurCanvas;
  Dev->Pen = FCanvas->Pen;
  Dev->Brush = FCanvas->Brush;
  Dev->Font->Name = "Arial";
  Dev->Font->Height = 1000;
  Dev->Pen->Color = clBlack;
  Dev->Pen->Style = psSolid;
  Dev->Brush->Color = clWhite;
  Dev->Font->Color = clBlack;
  TPen *MPen = new TPen(), *AGPen = new TPen();
  MPen->Assign(FMarkerPen); AGPen->Assign(FAuxGridPen);
  FAuxGridPen->Style = psSolid;
  FMarkerPen->Color = clBlack;
  FAuxGridPen->Color = clBlack;
  Dev->FillRect(FCurRect);
  if (NeedAutoScale) { NeedAutoScale = false; DoAutoScale(); }
  DoCalculate(); NeedCalculate = true;
  DoRepaint(ps_black_print); NeedRepaint = true;
  //DoRepaint(ps_display_paint); NeedRepaint = true;
  CurCanvas = FPaintCanvas;
  Dev->Font = FCanvas->Font;
  FCurRect = FBoundsRect;
  PaintMode = false;
  FMarkerPen->Assign(MPen); FAuxGridPen->Assign(AGPen);
  delete MPen;
  MPen = NULL;
  delete AGPen;
  AGPen = NULL;
  delete emfCanvas;
  emfCanvas = NULL;
  emf->SaveToFile(FileName);
  delete emf;
  emf = NULL;
}

void mx_ext_chart_t::BlackPrint()
{
  if (!ValidRect()) return;
  PaintMode = true;
  CurCanvas = FPrintCanvas;
  FCurRect = FPrintRect;
  TCanvas *Dev = CurCanvas;
  Dev->Pen = FCanvas->Pen;
  Dev->Brush = FCanvas->Brush;
  Dev->Font = FCanvas->Font;
  Dev->Pen->Color = clBlack;
  Dev->Pen->Style = psSolid;
  Dev->Brush->Color = clWhite;
  Dev->Font->Color = clBlack;
  TPen *MPen = new TPen(), *AGPen = new TPen();
  MPen->Assign(FMarkerPen); AGPen->Assign(FAuxGridPen);
  FAuxGridPen->Style = psSolid;
  FMarkerPen->Color = clBlack;
  FAuxGridPen->Color = clBlack;
  Dev->FillRect(FCurRect);
  if (NeedAutoScale) { NeedAutoScale = false; DoAutoScale(); }
  DoCalculate();
  DoRepaint(ps_black_print);
  NeedRepaint = false;
  CurCanvas = FPaintCanvas;
  FCurRect = FBoundsRect;
  PaintMode = false;
  FMarkerPen->Assign(MPen); FAuxGridPen->Assign(AGPen);
  delete MPen;
  MPen = NULL;
  delete AGPen;
  AGPen = NULL;
  if (FOnPrint) FOnPrint(this);
}

void mx_ext_chart_t::ColorPrint()
{
  if (!ValidRect()) return;
  PaintMode = true;
  CurCanvas = FPrintCanvas;
  FCurRect = FPrintRect;
  TCanvas *Dev = CurCanvas;
  Dev->Pen = FCanvas->Pen;
  Dev->Brush = FCanvas->Brush;
  Dev->Font = FCanvas->Font;
  TPenStyle PS = Dev->Pen->Style;
  Dev->Pen->Style = psSolid;
  Dev->FillRect(FCurRect);
  if (NeedAutoScale) { NeedAutoScale = false; DoAutoScale(); }
  DoCalculate();
  DoRepaint(ps_color_print);
  CurCanvas = FPaintCanvas;
  FCurRect = FBoundsRect;
  PaintMode = false;
  Dev->Pen->Style = PS;
  if (FOnPrint) FOnPrint(this);
}

void mx_ext_chart_t::GraphObjChanged(TObject *Sender)
{
  NeedRepaint = true;
  if (FCanvas)
  {
    if (Sender == FCanvas->Pen) PenChanged(Sender);
    if (Sender == FCanvas->Brush) BrushChanged(Sender);
    if (Sender == FCanvas->Font) FontChanged(Sender);
  }
}

void mx_ext_chart_t::SetAutoScaleX(bool Value)
{
  if (FAutoScaleX != Value)
  {
    FAutoScaleX = Value;
    NeedAutoScale = NeedCalculate = NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctAutoScale);
  }
}

void mx_ext_chart_t::SetAutoScaleY(bool Value)
{
  if (FAutoScaleY != Value)
  {
    FAutoScaleY = Value;
    NeedAutoScale = NeedCalculate = NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctAutoScale);
  }
}

String mx_ext_chart_t::SFF(float_type x, float_type Step) const
{
  if (fabs(x) < mx_ext_chart_dbl_error) return "0";
  int_type ex = ( (x == 0)?0:static_cast<int_type>(
    floor(log10(fabs(x)))) );
  int_type es = ( (Step == 0)?0:static_cast<int_type>(
    floor(log10(fabs(Step)))) );
  String NumIntoStr;
  if (ex >= 0 && es >= 0 && es < 15)
  {
    NumIntoStr = Printf("%.*g", ex + 1, x);
    size_type Len = NumIntoStr.Length();
    for (size_type i = 0; i < es - 1; i++) NumIntoStr[Len - i] = '0';
  }
  else
    NumIntoStr = Printf("%.*g", ex - es + 1, x);
  return NumIntoStr;
}

float_type mx_ext_chart_t::StepCalc(float_type Begin, float_type End,
  int_type Size, int_type MinSize, bool IsWidth, bool AccountText)
{
  float_type AS = End - Begin;
  int_type e = (AS == 0)?0:static_cast<int_type>(floor(log10(fabs(AS))));
  int_type i0 = e - 1;
  int_type j0 = 0;
  int_type Steps[] = {5, 2, 1};
  int_type StepCount = static_cast<int_type>(sizeof(Steps)/sizeof(*Steps));
  for (int_type j = 0; j < StepCount; j++) {
    float_type ASE = Steps[j]*pow(10., e);
    if ((AS - ASE)/ASE > mx_ext_chart_dbl_error) { i0 = e; j0 = j; break; }
  }
  int_type iST = 0;
  int_type jST = 0;
  if (j0 > 0) {
    iST = i0;
    jST = j0 - 1;
  } else {
    iST = i0 + 1;
    jST = StepCount - 1;
  }
  float_type ST = Steps[jST]*pow(10., iST);
  int_type TH = FCurCanvas->TextHeight("0");
  int_type S = TH/4;
  for (int_type i = i0; i > mx_ext_chart_min_exp; i--)
  {
    bool Break = false;
    for (int_type j = ((i == i0)?j0:0); j < StepCount; j++)
    {
      float_type CurSS = Steps[j]*pow(10., i);
      float_type MinSW = 0;
      if (AccountText)
      {
        int_type STW = 0;
        if (IsWidth)
        {
          size_type_type TW1 = 0;
          int_type TW2 = 0;
          STW = 0;
          float_type X0 = CurSS*ceil(Begin/CurSS);
          if (X0 <= End)
          {
            TW1 = CurCanvas->TextWidth(SFF(X0, CurSS))/2;
            if (TW1 > (X0 - FArea.Left)/AS*(Size - 1))
              TW1 = static_cast<int_type>(2*TW1 - (X0 - FArea.Left)/
                AS*(Size - 1));
          }
          else return mx_ext_chart_inf;
          bool SingleLine = true;
          for (float_type i = X0 + CurSS; i <= End; i += CurSS)
          {
            SingleLine = false;
            TW2 = (CurCanvas->TextWidth(SFF(i, CurSS)) + 1)/2;
            if (i + CurSS > End) {
              if (TW2 > (End - i)/AS*(Size - 1)) {
                TW2 = static_cast<int_type>(2*TW2 - (End - i)/AS*(Size - 1));
              }
            }
            int_type CurW = TW1 + TW2 + S;
            STW = max(CurW, STW);
            TW1 = TW2;
          }
          if (SingleLine) {
            if (CurCanvas->TextWidth(SFF(X0, CurSS)) > Size) {
              return mx_ext_chart_inf;
            }
          }
        }
        else
        {
          int_type TH1 = TH/2, TH2 = (TH + 1)/2;
          float_type Y1 = CurSS*ceil(Begin/CurSS);
          if (Y1 <= End)
          {
            if (TH1 > (Y1 - Begin)/AS*(Size - 1)) {
              TH1 = static_cast<int_type>(TH - (Y1 - Begin)/AS*(Size - 1));
            }
            float_type Y2 = CurSS*floor(End/CurSS);
            if (fabs(Y2) < CurSS/2) Y2 = 0;
            if (Y2 > Y1) {
              if (TH2 > (End - Y2)/AS*(Size - 1)) {
                TH2 = static_cast<int_type>(TH - (End - Y2)/AS*(Size - 1));
              }
              STW = TH1 + TH2 + S;
            } else {
              if (TH < Size) {
                STW = TH;
              } else {
                return mx_ext_chart_inf;
              }
            }
          }
          else return mx_ext_chart_inf;
        }
        MinSW = max(STW, MinSize)*AS/(Size - 1);
      }
      else
      {
        if (AS/CurSS/floor(AS/CurSS) - 1 > mx_ext_chart_dbl_error) continue;
        MinSW = MinSize*AS/(Size - 1);
      }
      Break = false;
      if (CurSS < MinSW) {
        Break = true;
        break;
      } else {
        ST = CurSS;
      }
    }
    if (Break) {
      break;
    }
  }
  return ST;
}

void mx_ext_chart_t::CalcGrid()
{
  if (!FShowGrid) { FGridRect = FCurRect; return; }
  ApplyTextX = true; ApplyTextY = true;
  int PixelsX = GetDeviceCaps(FCurCanvas->Handle, LOGPIXELSX);
  int PixelsY = GetDeviceCaps(FCurCanvas->Handle, LOGPIXELSY);
  int_type PMinW = static_cast<int_type>(FMinWidthGrid*PixelsX/2.54);
  int_type PMinH = static_cast<int_type>(FMinHeightGrid*PixelsY/2.54);
  int_type W = FCurRect.Right - FCurRect.Left;
  int_type H = FCurRect.Bottom - FCurRect.Top;
  int_type TH = CurCanvas->TextHeight("0");
  int_type S = TH/4;
  int_type GH = H - TH - 2 - 2*S;
  if (GH < 1) { ApplyTextX = false; GH = H - 2; }

  // Вычисление SH - шага сетки по вертикали
  SH = StepCalc(FArea.Bottom, FArea.Top, GH, PMinH, false, true);
  if (SH == mx_ext_chart_inf) ApplyTextY = false;
  int_type TWM = 0;
  int_type GW = W - 2;
  if (ApplyTextY)
  {
    float_type Y0 = SH*ceil(FArea.Bottom/SH);
    if (Y0 < FArea.Top) TWM = CurCanvas->TextWidth(SFF(Y0, SH));
    for (float_type i = Y0 + SH; i < FArea.Top; i += SH)
    {
      int_type CurW = CurCanvas->TextWidth(SFF(i, SH));
      TWM = max(TWM, CurW);
    }
    GW = W - TWM - 2 - 2*S;
    if (GW < 1) { ApplyTextY = false; GW = W - 2; }
  }

  // Вычисление SW - шага сетки по горизонтали
  if (ApplyTextX)
  {
    SW = StepCalc(FArea.Left, FArea.Right, GW, PMinW, true, true);
    if (SW == mx_ext_chart_inf)
    {
      GH = H - 2; ApplyTextX = false; ApplyTextY = true;
      SH = StepCalc(FArea.Bottom, FArea.Top, GH, PMinH, false, true);
      if (SH == mx_ext_chart_inf) ApplyTextY = false;
      TWM = 0;
      GW = W - 2;
      if (ApplyTextY)
      {
        float_type Y0 = SH*ceil(FArea.Bottom/SH);
        if (Y0 < FArea.Top) TWM = CurCanvas->TextWidth(SFF(Y0, SH));
        for (float_type i = Y0 + SH; i < FArea.Top; i += SH)
        {
          int_type CurW = CurCanvas->TextWidth(SFF(i, SH));
          TWM = max(TWM, CurW);
        }
        GW = W - TWM - 2 - 2*S;
      }
    }
  }

  if (!ApplyTextX && !ApplyTextY) { FGridRect = FBoundsRect; return; }

  // Прямоугольник сетки
  FGridRect = Rect(FCurRect.Right - GW - 1, FCurRect.Top + 1,
    FCurRect.Right - 1, FCurRect.Top + GH + 1);
}

void mx_ext_chart_t::PaintGrid(bool BlackAuxGrid)
{
  if (!FShowGrid) return;
  if (!ApplyTextX && !ApplyTextY) return;
  int PixelsX = GetDeviceCaps(FCurCanvas->Handle, LOGPIXELSX);
  int PixelsY = GetDeviceCaps(FCurCanvas->Handle, LOGPIXELSY);
  int_type PMinWA = static_cast<int_type>(FMinWidthAuxGrid*PixelsX/2.54);
  int_type PMinHA = static_cast<int_type>(FMinHeightAuxGrid*PixelsY/2.54);
  int_type TH = CurCanvas->TextHeight("0");
  int_type S = TH/4;
  float_type AW = FArea.Right - FArea.Left;
  float_type AH = FArea.Top - FArea.Bottom;
  float_type SWA = 0., SHA = 0.;
  float_type GW = FGridRect.Right - FGridRect.Left;
  float_type GH = FGridRect.Bottom - FGridRect.Top;

  RectLine(CurCanvas, Rect(FGridRect.Left - 1, FGridRect.Top - 1,
    FGridRect.Right + 1, FGridRect.Bottom + 1));

  // Отображение подписей линий сетки по Y
  if (ApplyTextY)
  {
    float_type Y0 = SH*ceil(FArea.Bottom/SH);
    for (float_type i = Y0; i < FArea.Top; i += SH)
    {
      int_type TPY = static_cast<int_type>(FGridRect.Bottom -
        (i - FArea.Bottom)/AH*(GH - 1) - TH/2 - 1);
      if (TPY < FGridRect.Top) TPY = FGridRect.Top;
      if (TPY + TH > FGridRect.Bottom) TPY = FGridRect.Bottom - TH;
      int_type TPX = FGridRect.Left - CurCanvas->TextWidth(SFF(i, SH)) - 1 - S;
      CurCanvas->TextOut(TPX, TPY, SFF(i, SH));
    }
  }

  // Отображение подписей линий сетки по X
  if (ApplyTextX)
  {
    float_type X0 = SW*ceil(FArea.Left/SW);
    int_type TPY = FGridRect.Bottom + 1 + S;
    for (float_type i = X0; i < FArea.Right; i += SW)
    {
      int_type TW = CurCanvas->TextWidth(SFF(i, SW));
      int_type TPX = static_cast<int_type>((i - FArea.Left)/
        AW*(GW - 1) + FGridRect.Left - TW/2);
      if (TPX + TW > FGridRect.Right - 1) TPX = FGridRect.Right - TW - 1;
      if (TPX < FGridRect.Left) TPX = FGridRect.Left;
      CurCanvas->TextOut(TPX, TPY, SFF(i, SW));
    }
  }

  if (FShowAuxGrid)
  {
    // Вычисление SHA - шага вспомогательной сетки по вертикали
    if (ApplyTextY)
      SHA = StepCalc(0, SH, static_cast<int_type>(SH/AH*(GH - 1)),
        PMinHA, false, false);

    // Вычисление SWA - шага вспомогательной сетки по горизонтали
    if (ApplyTextX)
      SWA = StepCalc(0, SW, static_cast<int_type>(SW/AW*(GW - 1)),
        PMinWA, true, false);

    FCurCanvas->Pen = FAuxGridPen;
    FCurCanvas->Pen->Mode = pmCopy;

    // Отображение вспомогательных линий сетки по Y
    if (ApplyTextY)
    {
      float_type Y0 = SHA*ceil(FArea.Bottom/SHA);
      for (float_type i = Y0; i < FArea.Top; i += SHA)
      {
        if (i - SH*floor(i/SH) < SHA/2) continue;
        point_int_type P1 = ConvCoor(DblPoint(FArea.Left, i));
        point_int_type P2 = ConvCoor(DblPoint(FArea.Right, i));
        if ((P1.y == FGridRect.Top) || (P1.y == FGridRect.Bottom - 1)) {
          continue;
        }
        if (BlackAuxGrid) DashLine(CurCanvas, P1, P2, 1, 4);
        else Line(CurCanvas, P1, P2);
      }
    }

    // Отображение вспомогательных линий сетки по X
    if (ApplyTextX)
    {
      float_type X0 = SWA*ceil(FArea.Left/SWA);
      for (float_type i = X0; i < FArea.Right; i += SWA)
      {
        if (i - SW*floor(i/SW) < SWA/2) continue;
        point_int_type P1 = ConvCoor(DblPoint(i, FArea.Top));
        point_int_type P2 = ConvCoor(DblPoint(i, FArea.Bottom));
        if ((P1.x == FGridRect.Left) || (P1.x == FGridRect.Right - 1)) {
          continue;
        }
        if (BlackAuxGrid) DashLine(CurCanvas, P1, P2, 1, 4);
        else Line(CurCanvas, P1, P2);
      }
    }

    FCurCanvas->Pen = FCanvas->Pen;
  }

  // Отображение линий сетки по Y
  if (ApplyTextY)
  {
    float_type Y0 = SH*ceil(FArea.Bottom/SH);
    for (float_type i = Y0; i < FArea.Top; i += SH)
    {
      point_int_type P1 = ConvCoor(DblPoint(FArea.Left, i));
      point_int_type P2 = ConvCoor(DblPoint(FArea.Right, i));
      if ((P1.y == FGridRect.Top) || (P1.y == FGridRect.Bottom - 1)) {
        continue;
      }
      Line(CurCanvas, P1, P2);
    }
  }

  // Отображение линий сетки по X
  if (ApplyTextX)
  {
    float_type X0 = SW*ceil(FArea.Left/SW);
    for (float_type i = X0; i < FArea.Right; i += SW)
    {
      point_int_type P1 = ConvCoor(DblPoint(i, FArea.Top));
      point_int_type P2 = ConvCoor(DblPoint(i, FArea.Bottom));
      if ((P1.x == FGridRect.Left) || (P1.x == FGridRect.Right - 1)) {
        continue;
      }
      Line(CurCanvas, P1, P2);
    }
  }
}

void mx_ext_chart_t::PaintGrid()
{
  PaintGrid(false);
}

void mx_ext_chart_t::PaintBounds() const
{
  if (!FShowBounds) return;
  int_type TH = FCurCanvas->TextHeight("0");
  int_type S = TH/4;
  int_type PX, PY;
  PX = FGridRect.Left + S;
  PY = FGridRect.Bottom - TH - S;
  CurCanvas->TextOut(PX, PY, FArea.Left);
  PX = FGridRect.Right - CurCanvas->TextWidth(FArea.Right) - S;
  PY = FGridRect.Bottom - TH - S;
  CurCanvas->TextOut(PX, PY, FArea.Right);
  PX = FGridRect.Left + S;
  PY = FGridRect.Top + S;
  CurCanvas->TextOut(PX, PY, FArea.Top);
  PX = FGridRect.Left + S;
  PY = FGridRect.Bottom - 2*TH - 2*S;
  CurCanvas->TextOut(PX, PY, FArea.Bottom);
}

void mx_ext_chart_t::PreCalc()
{
  if (!ValidRect()) return;
  if (NeedAutoScale) { NeedAutoScale = false; DoAutoScale(); }
  if (NeedCalculate) { NeedCalculate = false; DoCalculate(); }
}

void mx_ext_chart_t::Invalidate()
{
  NeedAutoScale = NeedCalculate = NeedRepaint = true;
  for (size_type i = 0; i < FCount; i++) {
    FItems[i]->NeedAutoScale = true;
    FItems[i]->NeedCalculate = true;
  }
}

void mx_ext_chart_t::SetShowBounds(bool Value)
{
  if (FShowBounds != Value) {
    FShowBounds = Value;
    NeedCalculate = NeedRepaint = true;
  }
}

void mx_ext_chart_t::SetShowGrid(bool Value)
{
  if (FShowGrid != Value) {
    FShowGrid = Value;
    NeedCalculate = NeedRepaint = true;
  }
}

void mx_ext_chart_t::SetShowAuxGrid(bool Value)
{
  if (FShowAuxGrid != Value) {
    FShowAuxGrid = Value;
    NeedCalculate = NeedRepaint = true;
  }
}

void mx_ext_chart_t::DoAutoScale()
{
  for (size_type i = 0; i < FCount; i++)
    if (FItems[i]->NeedAutoScale && !FItems[i]->Hide)
    { FItems[i]->NeedAutoScale = false; FItems[i]->DoAutoScale(); }
  if (FAutoScaleX)
  {
    FArea.Left = FArea.Right = 0;
    if (FCount && FItems[FBaseItem]->Hide)
    for (size_type i = 0; i < FCount; i++)
      if (!FItems[i]->Hide) { FBaseItem = i; break; }
    if (FCount && !FItems[FBaseItem]->Hide)
    {
      FArea.Left = FItems[FBaseItem]->Area.Left;
      FArea.Right = FItems[FBaseItem]->Area.Right;
      for (size_type i = 0; i < FCount; i++)
        if (!FItems[i]->Hide && i != FBaseItem &&
          FItems[i]->FGroup[0] == FItems[FBaseItem]->FGroup[0])
        {
          if (FArea.Left > FItems[i]->Area.Left)
            FArea.Left = FItems[i]->Area.Left;
          if (FArea.Right < FItems[i]->Area.Right)
            FArea.Right = FItems[i]->Area.Right;
        }
    }
  }
  if (FAutoScaleY)
  {
    FArea.Bottom = FArea.Top = 0;
    if (FCount && FItems[FBaseItem]->Hide)
    for (size_type i = 0; i < FCount; i++)
      if (!FItems[i]->Hide) { FBaseItem = i; break; }
    if (FCount && !FItems[FBaseItem]->Hide)
    {
      FArea.Bottom = FItems[FBaseItem]->Area.Bottom;
      FArea.Top = FItems[FBaseItem]->Area.Top;
      for (size_type i = 0; i < FCount; i++)
        if (!FItems[i]->Hide && i != FBaseItem &&
          FItems[i]->FGroup[1] == FItems[FBaseItem]->FGroup[1])
        {
          if (FArea.Bottom > FItems[i]->Area.Bottom)
            FArea.Bottom = FItems[i]->Area.Bottom;
          if (FArea.Top < FItems[i]->Area.Top)
            FArea.Top = FItems[i]->Area.Top;
        }
    }
  }
  if (FArea.Left == FArea.Right) { FArea.Left--; FArea.Right++; }
  if (FArea.Bottom == FArea.Top) { FArea.Bottom--; FArea.Top++; }
  if (FOnChange) FOnChange(this, cctArea);
}

void mx_ext_chart_t::DoCalculate()
{
  CalcGrid();
  SetChildRect();
}

void mx_ext_chart_t::DoRepaint(paint_style_t a_paint_style)
{
  FCurCanvas->FillRect(FCurRect);
  if (a_paint_style == 1) PaintGrid(true); else PaintGrid();
  for (size_type i = 0; i < FCount; i++) {
    if (FItems[i]->NeedCalculate && !FItems[i]->Hide) {
      FItems[i]->NeedCalculate = false;
      FItems[i]->DoCalculate();
    }
  }
  SetClipRect(FCurCanvas, FGridRect);
  switch (a_paint_style)
  {
    case ps_display_paint: {
      for (size_type i = 0; i < FCount; i++) {
        if (!FItems[i]->Hide) {
          FItems[i]->DoPaint();
        }
      }
    } break;
    case ps_black_print: {
      for (size_type i = 0; i < FCount; i++) {
        if (!FItems[i]->Hide) {
          FItems[i]->DoBlackPrint();
        }
      }
    } break;
    case ps_color_print: {
      for (size_type i = 0; i < FCount; i++) {
        if (!FItems[i]->Hide) {
          FItems[i]->DoColorPrint();
        }
      }
    } break;
  }
  for (size_type i = 0; i < IndexMarkerX; i++) {
    PaintMarkerX(i);
  }
  for (size_type i = 0; i < IndexMarkerY; i++) {
    PaintMarkerY(i);
  }
  PaintBounds();
  DeleteClipRect(FCurCanvas);
}

point_int_type mx_ext_chart_t::ConvCoor(point_float_type P) const
{
  float_type k = 0.;
  int_type x = 0;
  int_type y = 0;
  int_type Width = FGridRect.Right - FGridRect.Left;
  int_type Height = FGridRect.Bottom - FGridRect.Top;
  float_type wx = FArea.Right - FArea.Left;
  if (0. == wx) wx = 1e-12;
  try {
    k = (P.x - FArea.Left)/wx;
  } catch (...) {
    k = 0.;
  }
  x = static_cast<int_type>(FGridRect.Left + (Width - 1)*k);
  float_type wy = FArea.Top - FArea.Bottom;
  if (0. == wy) wy = 1e-12;
  try {
    k = (P.y - FArea.Bottom)/wy;
  } catch (...) {
    k = 0.;
  }
  y = static_cast<int_type>(FGridRect.Bottom - (Height - 1)*k - 1);
  return Point(x, y);
}

int_type mx_ext_chart_t::ConvCoorX(float_type b, float_type e,
  float_type x) const
{
  int_type Width = FGridRect.Right - FGridRect.Left;
  float_type w = e - b;
  if (0. == w) w = 1e-12;
  float_type k = 0;
  try {
    k = (x - b)/w;
  } catch(...) {
    k = 0.;
  }
  return static_cast<int_type>(FGridRect.Left + (Width - 1)*k);
}

int_type mx_ext_chart_t::ConvCoorY(float_type b, float_type e,
  float_type y) const
{
  int_type Height = FGridRect.Bottom - FGridRect.Top;
  float_type w = e - b;
  if (0. == w) w = 1e-12;
  float_type k = 0;
  try {
    k = (y - b)/w;
  } catch(...) {
    k = 0.;
  }
  return static_cast<int_type>(FGridRect.Bottom - (Height - 1)*k - 1);
}

void mx_ext_chart_t::SetChildRect()
{
  if (!FCount || FItems[BaseItem]->Hide) return;
  typedef vector<size_type> veci;
  typedef map<size_type, veci> mapg;
  typedef mapg::iterator itr;
  mapg GroupsX, GroupsY;
  vector<size_type> vec;
  for (size_type i = 0; i < FCount; i++) if (!FItems[i]->Hide)
  {
    GroupsX[FItems[i]->GroupX].push_back(i);
    GroupsY[FItems[i]->GroupY].push_back(i);
  }

  PaintMode = true;

  // -------- Base X
  vec = GroupsX[FItems[BaseItem]->GroupX];
  for (size_t i = 0; i < vec.size(); i++)
  {
    rect_float_type A = FItems[vec[i]]->Area;
    A.Left = Area.Left; A.Right = Area.Right;
    FItems[vec[i]]->Area = A;
  }
  for (size_t i = 0; i < vec.size(); i++)
  {
    rect_int_type R = FItems[vec[i]]->BoundsRect;
    R.Left = ConvCoorX(Area.Left, Area.Right, FItems[vec[i]]->Area.Left);
    R.Right = ConvCoorX(Area.Left, Area.Right,
      FItems[vec[i]]->Area.Right) + 1;
    FItems[vec[i]]->BoundsRect = R;
  }

  // -------- Base Y
  vec = GroupsY[FItems[BaseItem]->GroupY];
  for (size_t i = 0; i < vec.size(); i++)
  {
    rect_float_type A = FItems[vec[i]]->Area;
    A.Bottom = Area.Bottom; A.Top = Area.Top;
    FItems[vec[i]]->Area = A;
  }
  for (size_t i = 0; i < vec.size(); i++)
  {
    rect_int_type R = FItems[vec[i]]->BoundsRect;
    R.Top = ConvCoorY(Area.Bottom, Area.Top, FItems[vec[i]]->Area.Top);
    R.Bottom = ConvCoorY(Area.Bottom, Area.Top,
      FItems[vec[i]]->Area.Bottom) + 1;
    FItems[vec[i]]->BoundsRect = R;
  }

  // -------- X
  for (itr i = GroupsX.begin(); i != GroupsX.end(); i++)
  {
    if (i->first == FItems[FBaseItem]->GroupX) continue;
    vec = i->second;
    float_type L = 1;
    float_type R = -1;
    for (size_t j = 0; j < vec.size(); j++)
    {
      if (L > R)
      {
        L = FItems[vec[j]]->Area.Left; R = FItems[vec[j]]->Area.Right;
      }
      else
      {
        if (L > FItems[vec[j]]->Area.Left) L = FItems[vec[j]]->Area.Left;
        if (R < FItems[vec[j]]->Area.Right) R = FItems[vec[j]]->Area.Right;
      }
    }
    if (L == R) { L--; R++; }
    for (size_t j = 0; j < vec.size(); j++)
    {
      rect_int_type Rt = FItems[vec[j]]->BoundsRect;
      Rt.Left = ConvCoorX(L, R, FItems[vec[j]]->Area.Left);
      Rt.Right = ConvCoorX(L, R, FItems[vec[j]]->Area.Right) + 1;
      FItems[vec[j]]->BoundsRect = Rt;
    }
  }

  // -------- Y
  for (itr i = GroupsY.begin(); i != GroupsY.end(); i++)
  {
    if (i->first == FItems[FBaseItem]->GroupY) continue;
    vec = i->second;
    float_type B = 1, T = -1;
    for (size_t j = 0; j < vec.size(); j++)
    {
      if (B > T)
      {
        B = FItems[vec[j]]->Area.Bottom; T = FItems[vec[j]]->Area.Top;
      }
      else
      {
        if (B > FItems[vec[j]]->Area.Bottom) B = FItems[vec[j]]->Area.Bottom;
        if (T < FItems[vec[j]]->Area.Top) T = FItems[vec[j]]->Area.Top;
      }
    }
    if (B == T) { B--; T++; }
    for (size_t j = 0; j < vec.size(); j++)
    {
      rect_int_type R = FItems[vec[j]]->BoundsRect;
      R.Top = ConvCoorY(B, T, FItems[vec[j]]->Area.Top);
      R.Bottom = ConvCoorY(B, T, FItems[vec[j]]->Area.Bottom) + 1;
      FItems[vec[j]]->BoundsRect = R;
    }
  }

  PaintMode = false;
}

void mx_ext_chart_t::SetBoundsRect(rect_int_type Value)
{
  if (Value.Left < Value.Right && Value.Top < Value.Bottom)
  {
    if ((Value.Right - Value.Left != FBoundsRect.Right - FBoundsRect.Left) ||
        (Value.Bottom - Value.Top != FBoundsRect.Bottom - FBoundsRect.Top))
    {
      NeedCalculate = NeedRepaint = true;
      FBitmap->Width = Value.Right - Value.Left;
      FBitmap->Height = Value.Bottom - Value.Top;
    }
    FCurRect = FBoundsRect = Value;
    if (FOnChange) FOnChange(this, cctBoundsRect);
  }
}

void mx_ext_chart_t::SetPrintRect(rect_int_type Value)
{
  if (Value.Left < Value.Right && Value.Top < Value.Bottom)
  {
    FPrintRect = Value;
    if (FOnChange) FOnChange(this, cctPrintRect);
  }
}

void mx_ext_chart_t::SetPaintCanvas(TCanvas *Value)
{
  if (FPaintCanvas != Value)
  {
    FPaintCanvas = Value;
    CurCanvas = FPaintCanvas;
    if (FOnChange) FOnChange(this, cctCanvas);
  }
}

void mx_ext_chart_t::SetArea(rect_float_type AArea)
{
  if (AArea.Left > AArea.Right) swap(AArea.Left, AArea.Right);
  if (AArea.Bottom > AArea.Top) swap(AArea.Bottom, AArea.Top);
  bool Change = false;
  if (!(FltEqual(FArea.Left, AArea.Left) && FltEqual(FArea.Right, AArea.Right)))
  {
    FAutoScaleX = false; Change = true;
  }
  if (!(FltEqual(FArea.Bottom, AArea.Bottom) && FltEqual(FArea.Top, AArea.Top)))
  {
    FAutoScaleY = false; Change = true;
  }
  if (Change)
  {
    FArea = AArea;
    NeedCalculate = NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctArea);
  }
}

void mx_ext_chart_t::SetCurCanvas(TCanvas *Value)
{
  FCurCanvas = Value;
  for (size_type i = 0; i < FCount; i++) FItems[i]->Canvas = Value;
}

void mx_ext_chart_t::SetMarkerPen(TPen *Value)
{
  FMarkerPen->Assign(Value);
}

void mx_ext_chart_t::SetAuxGridPen(TPen *Value)
{
  FAuxGridPen->Assign(Value);
}

void mx_ext_chart_t::SetMinWidthGrid(float_type Value)
{
  FMinWidthGrid = Value;
  NeedCalculate = NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctSizeGrid);
}

void mx_ext_chart_t::SetMinHeightGrid(float_type Value)
{
  FMinHeightGrid = Value;
  NeedCalculate = NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctSizeGrid);
}

void mx_ext_chart_t::SetMinWidthAuxGrid(float_type Value)
{
  FMinWidthAuxGrid = Value;
  NeedCalculate = NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctSizeAuxGrid);
}

void mx_ext_chart_t::SetMinHeightAuxGrid(float_type Value)
{
  FMinHeightAuxGrid = Value;
  NeedCalculate = NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctSizeAuxGrid);
}

void mx_ext_chart_t::SetBaseItem(size_type AChart)
{
  if (AChart == FBaseItem) return;
  //PaintMode = true;
  FBaseItem = AChart;
  //PaintMode = false;
  NeedAutoScale = NeedCalculate = NeedRepaint = true;
  if (Select) Select->ItemChange(ic_set_base_item, FBaseItem);
  if (FOnChange) FOnChange(this, cctBaseItem);
}

void mx_ext_chart_t::SetLeft(int_type Value)
{
  rect_int_type R = BoundsRect;
  R.Left = Value;
  BoundsRect = R;
}

void mx_ext_chart_t::SetTop(int_type Value)
{
  rect_int_type R = BoundsRect;
  R.Top = Value;
  BoundsRect = R;
}

void mx_ext_chart_t::SetWidth(int_type Value)
{
  rect_int_type R = BoundsRect;
  R.Right = R.Left + Value;
  BoundsRect = R;
}

void mx_ext_chart_t::SetHeight(int_type Value)
{
  rect_int_type_type R = BoundsRect;
  R.Bottom = R.Top + Value;
  BoundsRect = R;
}

rect_float_type mx_ext_chart_t::GetArea()
{
  PreCalc();
  return FArea;
}

rect_int_type mx_ext_chart_t::GetGridRect()
{
  PreCalc();
  return FGridRect;
}

int_type mx_ext_chart_t::GetLeft() const
{
  return FBoundsRect.Left;
}

int_type mx_ext_chart_t::GetTop() const
{
  return FBoundsRect.Top;
}

int_type mx_ext_chart_t::GetWidth() const
{
  return FBoundsRect.Right - FBoundsRect.Left;
}

int_type mx_ext_chart_t::GetHeight() const
{
  return FBoundsRect.Bottom - FBoundsRect.Top;
}

void mx_ext_chart_t::AddMarkerX(float_type Value)
{
  if (FMarkerX != NULL) {
    FMarkerX = reinterpret_cast<float_type*>realloc(FMarkerX,
      (IndexMarkerX + 1)*sizeof(float_type));
  } else {
    FMarkerX = reinterpret_cast<float_type*>(
      calloc(1, sizeof(float_type)));
  }
  FMarkerX[IndexMarkerX] = Value;
  PaintMarkerX(IndexMarkerX);
  IndexMarkerX++;
  if (FOnChange) FOnChange(this, cctMarker);
}

void mx_ext_chart_t::AddMarkerY(float_type Value)
{
  if (FMarkerY != NULL) {
    FMarkerY = reinterpret_cast<float_type*>(
      realloc(FMarkerY, (IndexMarkerY + 1)*sizeof(float_type)));
  } else {
    FMarkerY = reinterpret_cast<float_type*>(
      calloc(1, sizeof(float_type)));
  }
  FMarkerY[IndexMarkerY] = Value;
  PaintMarkerY(IndexMarkerY);
  IndexMarkerY++;
  if (FOnChange) FOnChange(this, cctMarker);
}

float_type mx_ext_chart_t::GetMarkerX(size_type Index) const
{
  if (Index == -1) if (IndexMarkerX) return FMarkerX[IndexMarkerX - 1];
  if ((Index >= 0) && (Index < IndexMarkerX)) return FMarkerX[Index];
  return 0;
}

float_type mx_ext_chart_t::GetMarkerY(size_type Index) const
{
  if (Index == -1) if (IndexMarkerY) return FMarkerY[IndexMarkerY - 1];
  if ((Index >= 0) && (Index < IndexMarkerY)) return FMarkerY[Index];
  return 0;
}

void mx_ext_chart_t::SetMarkerX(size_type Index, float_type Value)
{
  if (Index == -1) if (IndexMarkerX) Index = IndexMarkerX - 1;
  if ((Index >= 0) && (Index < IndexMarkerX))
  {
    int_type OldPos = ConvCoor(DblPoint(FMarkerX[Index], FArea.Top)).x;
    int_type NewPos = ConvCoor(DblPoint(Value, FArea.Top)).x;
    if (NewPos != OldPos)
    { PaintMarkerX(Index); FMarkerX[Index] = Value; PaintMarkerX(Index); }
    else
      FMarkerX[Index] = Value;
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_t::SetMarkerY(size_type Index, float_type Value)
{
  if (Index == -1) if (IndexMarkerY) Index = IndexMarkerY - 1;
  if ((Index >= 0) && (Index < IndexMarkerY))
  {
    int_type OldPos = ConvCoor(DblPoint(FArea.Left, FMarkerY[Index])).y;
    int_type NewPos = ConvCoor(DblPoint(FArea.Left, Value)).y;
    if (NewPos != OldPos)
    { PaintMarkerY(Index); FMarkerY[Index] = Value; PaintMarkerY(Index); }
    else
      FMarkerY[Index] = Value;
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_t::ClearMarkerX()
{
  for (size_type i = 0; i < IndexMarkerX; i++) PaintMarkerX(i);
  if (FMarkerX != NULL) {
    free(FMarkerX);
    FMarkerX = NULL;
  }
  IndexMarkerX = 0;
  NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctMarker);
}

void mx_ext_chart_t::ClearMarkerY()
{
  for (size_type i = 0; i < IndexMarkerY; i++) PaintMarkerY(i);
  if (FMarkerY != NULL) {
    free(FMarkerY);
    FMarkerY = NULL;
  }
  IndexMarkerY = 0;
  NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctMarker);
}

void mx_ext_chart_t::DeleteMarkerX(size_type Index)
{
  if ((Index >= 0) && (Index < IndexMarkerX))
  {
    PaintMarkerX(Index);
    for (size_type i = Index; i < IndexMarkerX - 1; i++) {
      FMarkerX[i] = FMarkerX[i + 1];
    }
    IndexMarkerX--;
    if (IndexMarkerX != 0) {
      FMarkerX = reinterpret_cast<float_type*>(
        realloc(FMarkerX, IndexMarkerX*sizeof(float_type)));
    } else {
      free(FMarkerX);
      FMarkerX = NULL;
    }
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_t::DeleteMarkerY(size_type Index)
{
  if ((Index >= 0) && (Index < IndexMarkerY))
  {
    PaintMarkerY(Index);
    for (size_type i = Index; i < IndexMarkerY - 1; i++) {
      FMarkerY[i] = FMarkerY[i + 1];
    }
    IndexMarkerY--;
    if (IndexMarkerY != 0) {
      FMarkerY = reinterpret_cast<float_type*>(
        realloc(FMarkerY, IndexMarkerY*sizeof(float_type)));
    } else {
      free(FMarkerY);
      FMarkerY = NULL;
    }
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_t::DeleteMarkerX()
{
  if (IndexMarkerX > 0)
  {
    IndexMarkerX--;
    PaintMarkerX(IndexMarkerX);
    if (IndexMarkerX != 0) {
      FMarkerX = reinterpret_cast<float_type*>(
        realloc(FMarkerX, IndexMarkerX*sizeof(float_type)));
    } else {
      free(FMarkerX);
      FMarkerX = NULL;
    }
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_t::DeleteMarkerY()
{
  if (IndexMarkerY > 0)
  {
    IndexMarkerY--;
    PaintMarkerY(IndexMarkerY);
    if (IndexMarkerY > 0) {
      FMarkerY = reinterpret_cast<float_type*>(
        realloc(FMarkerY, IndexMarkerY*sizeof(float_type)));
    } else {
      free(FMarkerY);
      FMarkerY = NULL;
    }
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_t::DeleteMarkerX(size_type Begin, size_type End)
{
  if ((Begin >= 0) && (Begin < IndexMarkerX) &&
        (End >= 0) && (End < IndexMarkerX))
  {
    for (size_type i=Begin; i<=End; i++) PaintMarkerX(i);
    for (size_type i=Begin; i<IndexMarkerX-(End-Begin+1); i++)
      FMarkerX[i] = FMarkerX[i + 1];
    IndexMarkerX-=(End - Begin + 1);
    if (IndexMarkerX > 0) {
      FMarkerX = reinterpret_cast<float_type*>(
        realloc(FMarkerX, IndexMarkerX*sizeof(float_type)));
    } else {
      free(FMarkerX);
      FMarkerX = NULL;
    }
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_t::DeleteMarkerY(size_type Begin, size_type End)
{
  if ((Begin >= 0) && (Begin < IndexMarkerY) &&
        (End >= 0) && (End < IndexMarkerY))
  {
    for (size_type i=Begin; i<=End; i++) PaintMarkerY(i);
    for (size_type i=Begin; i<IndexMarkerY-(End-Begin+1); i++)
      FMarkerY[i] = FMarkerY[i + 1];
    IndexMarkerY-=(End - Begin + 1);
    if (IndexMarkerY > 0) {
      FMarkerY = reinterpret_cast<float_type*>(
        realloc(FMarkerY, IndexMarkerY*sizeof(float_type)));
    } else {
      free(FMarkerY);
      FMarkerY = NULL;
    }
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}

void mx_ext_chart_t::PaintMarkerX(size_type i)
{
  if (MarkerX[i] < FArea.Left && MarkerX[i] > FArea.Right)
    return;
  PreCalc();
  TCanvas *Dev = CurCanvas;
  Dev->Pen = FMarkerPen;
  Dev->Pen->Style = psSolid;
  Dev->Pen->Mode = pmNotXor;
  point_int_type P1 = ConvCoor(DblPoint(FMarkerX[i], FArea.Top));
  point_int_type P2 = ConvCoor(DblPoint(FMarkerX[i], FArea.Bottom));
  DashLine(Dev, P1, P2, 5, 5);
}

void mx_ext_chart_t::PaintMarkerY(size_type i)
{
  if (MarkerY[i] < FArea.Bottom && MarkerY[i] > FArea.Top)
    return;
  PreCalc();
  TCanvas *Dev = CurCanvas;
  Dev->Pen = FMarkerPen;
  Dev->Pen->Style = psSolid;
  Dev->Pen->Mode = pmNotXor;
  point_int_type P1 = ConvCoor(DblPoint(FArea.Left, FMarkerY[i]));
  point_int_type P2 = ConvCoor(DblPoint(FArea.Right, FMarkerY[i]));
  DashLine(Dev, P1, P2, 5, 5);
}

void mx_ext_chart_t::PaintChildMarkerX(mx_ext_chart_item_t *Sender,
  int_type i) const
{
  TCanvas *Dev = CurCanvas;
  Dev->Pen = Sender->MarkerPen;
  Dev->Pen->Style = psSolid;
  Dev->Pen->Mode = pmNotXor;
  point_int_type P1 = Point(i, FGridRect.Top);
  point_int_type P2 = Point(i, FGridRect.Bottom - 1);
  DashLine(Dev, P1, P2, 5, 5);
}

void mx_ext_chart_t::PaintChildMarkerY(mx_ext_chart_item_t *Sender,
  int_type i) const
{
  TCanvas *Dev = CurCanvas;
  Dev->Pen = Sender->MarkerPen;
  Dev->Pen->Style = psSolid;
  Dev->Pen->Mode = pmNotXor;
  point_int_type P1 = Point(FGridRect.Left, i);
  point_int_type P2 = Point(FGridRect.Right - 1, i);
  DashLine(Dev, P1, P2, 5, 5);
}

void mx_ext_chart_t::Delete(size_type Index)
{
  if ((Index < 0) && (Index >= FCount)) return;
  //** Для mx_ext_chart_select_t **
  if (Select) Select->ItemChange(ic_delete, Index);
  //************************
  delete FItems[Index];
  FItems[Index] = NULL;
  for (size_type i = Index; i < FCount - 1; i++) FItems[i] = FItems[i + 1];
  FCount--;
  if (FCount != 0) {
    FItems = reinterpret_cast<mx_ext_chart_item_t **>(
      realloc(FItems, FCount*sizeof(mx_ext_chart_item_t*)));
  } else {
    free(FItems);
    FItems = NULL;
  }
  NeedAutoScale = NeedCalculate = NeedRepaint = true;
  if (Index >= BaseItem && Index != 1) BaseItem--;
  if (FOnChange) FOnChange(this, cctItems);
}

void mx_ext_chart_t::Clear()
{
  for (size_type i = 0; i < FCount; i++) delete FItems[i];
  FCount = 0;
  if (FItems != NULL) {
    free(FItems);
    FItems = NULL;
  }
  NeedAutoScale = NeedCalculate = NeedRepaint = true;
  FBaseItem = CurGroupX = CurGroupY = 0;
  if (FOnChange) FOnChange(this, cctItems);
  //** Для mx_ext_chart_select_t **
  if (Select) Select->ItemChange(ic_clear, 0);
  //************************
}

void mx_ext_chart_t::Add()
{
  //FItems = (mx_ext_chart_item_t **)realloc(FItems, (FCount + 1)*sizeof(mx_ext_chart_item_t));
  if (FItems != NULL) {
    FItems =
      (mx_ext_chart_item_t **)realloc(FItems, (FCount + 1)*sizeof(mx_ext_chart_item_t*));
  } else {
    FItems = (mx_ext_chart_item_t **)calloc(1, sizeof(mx_ext_chart_item_t*));
  }
  FItems[FCount] = new mx_ext_chart_item_t(this);
  FItems[FCount]->Canvas = CurCanvas;
  FItems[FCount]->OnChange = ChildChange;
  FItems[FCount]->OnError = ChildError;
  FItems[FCount]->Pen->OnChange = GraphObjChanged;
  FItems[FCount]->MarkerPen->OnChange = GraphObjChanged;
  FItems[FCount]->GroupX = CurGroupX;
  FItems[FCount]->GroupY = CurGroupY;
  FCount++;
  NeedAutoScale = NeedCalculate = NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctItems);
  //** Для mx_ext_chart_select_t **
  if (Select) Select->ItemChange(ic_insert, FCount - 1);
  //************************
}

void mx_ext_chart_t::Add(TPointer Data, size_type Count)
{
  Add();
  Items[FCount - 1]->DataY = Data;
  Items[FCount - 1]->Bounds.Begin = 0;
  Items[FCount - 1]->Bounds.End = Count - 1;
}

void mx_ext_chart_t::Add(TPointer Data, size_type Count, TColor AColor)
{
  Add(Data, Count);
  Items[FCount - 1]->Pen->Color = AColor;
}

void mx_ext_chart_t::Insert(size_type Index)
{
  if ((Index < 0) && (Index > FCount)) return;
  if (FItems != NULL) {
    FItems = reinterpret_cast<mx_ext_chart_item_t**>(
      realloc(FItems, (FCount + 1)*sizeof(mx_ext_chart_item_t*)));
  } else {
    FItems = reinterpret_cast<mx_ext_chart_item_t**>(
      calloc(1, sizeof(mx_ext_chart_item_t*)));
  }
  for (size_type i = FCount; i > Index; i--) FItems[i] = FItems[i - 1];
  FCount++;
  FItems[Index] = new mx_ext_chart_item_t(this);
  FItems[Index]->Canvas = CurCanvas;
  FItems[Index]->OnChange = ChildChange;
  FItems[Index]->OnError = ChildError;
  FItems[Index]->Pen->OnChange = GraphObjChanged;
  FItems[Index]->MarkerPen->OnChange = GraphObjChanged;
  FItems[Index]->GroupX = CurGroupX;
  FItems[Index]->GroupY = CurGroupY;
  NeedAutoScale = NeedCalculate = NeedRepaint = true;
  if (Index <= BaseItem) BaseItem++;
  if (FOnChange) FOnChange(this, cctItems);
  //** Для mx_ext_chart_select_t **
  if (Select) Select->ItemChange(ic_insert, Index);
  //************************
}

void mx_ext_chart_t::Insert(size_type Index, TPointer Data, size_type Count)
{
  Insert(Index);
  Items[Index]->DataY = Data;
  Items[Index]->Bounds.End = Count - 1;
}

void mx_ext_chart_t::Insert(size_type Index, TPointer Data, size_type Count,
  TColor AColor)
{
  Insert(Index, Data, Count);
  Items[FCount - 1]->Pen->Color = AColor;
}

void mx_ext_chart_t::NewGroupX()
{
  CurGroupX++;
}

void mx_ext_chart_t::NewGroupY()
{
  CurGroupY++;
}

mx_ext_chart_item_t *mx_ext_chart_t::GetItem(size_type Index) const
{
  if (Index == -1) return FItems[FCount - 1];
  if (Index < 0) Index = 0; if (Index >= FCount) Index = FCount - 1;
  return FItems[Index];
}

void mx_ext_chart_t::ChildChange(TObject *Sender,
  TChartChangeType ChartChangeType)
{
  if (!PaintMode)
  switch (ChartChangeType)
  {
    case cctCompConv:
    case cctArea:
    case cctAutoScale:
    case cctBounds:
    case cctHide:
    case cctGroup:
    case cctShift:
    case cctScale:
    case cctStep:
    case cctData:
      NeedAutoScale = NeedCalculate = NeedRepaint = true;
      break;
    case cctMarker:
      NeedRepaint = true;
  }
  if (FOnChange) FOnChange(Sender, ChartChangeType);
}

void mx_ext_chart_t::ChildError(TObject *Sender, Exception &e,
  point_float_type P, float_type t, TChartErrorType ChartErrorType) const
{
  if (FOnError) FOnError(Sender, e, P, t, ChartErrorType);
}

bool mx_ext_chart_t::ValidRect() const
{
  return (FCurRect.Right - FCurRect.Left > 1) &&
    (FCurRect.Bottom - FCurRect.Top > 1);
}


//***************************************************************************
// mx_ext_chart_select_t - Компонент на mx_ext_chart_t


mx_ext_chart_select_t::mx_ext_chart_select_t(mx_ext_chart_t *AChart)
  : TObject(),
  FChart(AChart),
  FControl(NULL),
  FOnError(NULL),
  AutoScales(),
  MAreas(),
  LockEvent(false),
  ValidAutoScales(false),
  ValidAreas(false),
  Areas(),
  BaseItems(),
  CompConvs(),
  //rect_float_type CurArea;
  Xn(0),
  Yn(0),
  PX(0),
  PY(0),
  SelRect(Rect(0, 0, 0, 0)),
  SelectMode(false),
  FirstSelect(false),
  FZoomFactor(0.5),
  FPositioningMode(false),
  FDragChartMode(false)
{
}

mx_ext_chart_select_t::mx_ext_chart_select_t(mx_ext_chart_t *AChart, TControl *AControl)
  : TObject(),
  FChart(AChart),
  FControl(NULL),
  FOnError(NULL),
  AutoScales(),
  MAreas(),
  LockEvent(false),
  ValidAutoScales(false),
  ValidAreas(false),
  Areas(),
  BaseItems(),
  CompConvs(),
  //rect_float_type CurArea;
  Xn(0),
  Yn(0),
  PX(0),
  PY(0),
  SelRect(Rect(0, 0, 0, 0)),
  SelectMode(false),
  FirstSelect(false),
  FZoomFactor(0.5),
  FPositioningMode(false),
  FDragChartMode(false)
{
  SetControl(AControl);
}

mx_ext_chart_select_t::~mx_ext_chart_select_t()
{
  Disconnect();
}

void mx_ext_chart_select_t::SetControl(TControl *AControl)
{
  FControl = AControl;
  Connect();
}

void mx_ext_chart_select_t::Connect()
{
  TPaintBox *PaintBox = (TPaintBox *)FControl;
  PaintBox->OnMouseDown = DoMouseDown;
  PaintBox->OnMouseMove = DoMouseMove;
  PaintBox->OnMouseUp = DoMouseUp;
  PaintBox->OnDblClick = DoDblClick;
  Chart->Select = this;
  SaveAutoScales();
  RestoreAreas();
}

void mx_ext_chart_select_t::Disconnect()
{
  TPaintBox *PaintBox = (TPaintBox *)FControl;
  PaintBox->OnMouseDown = NULL;
  PaintBox->OnMouseMove = NULL;
  PaintBox->OnMouseUp = NULL;
  PaintBox->OnDblClick = NULL;
  Chart->Select = NULL;
  SaveAreas();
  RestoreAutoScales();
}

void mx_ext_chart_select_t::SelectClip(rect_int_type &ARect) const
{
  int_type nLeft = max(min(ARect.Left, ARect.Right),
    Chart->GridRect.Left);
  int_type nRight = min(max(ARect.Left, ARect.Right),
    Chart->GridRect.Right);
  int_type nTop = max(min(ARect.Top, ARect.Bottom),
    Chart->GridRect.Top);
  int_type nBottom = min(max(ARect.Top, ARect.Bottom),
    Chart->GridRect.Bottom);
  ARect = Rect(nLeft, nTop, nRight, nBottom);
}

void mx_ext_chart_select_t::SelectRect(
  mx_ext_chart_select_t::rect_int_type Rect) const
{
  point_int_type Points[] = {
    Point(Rect.Left, Rect.Top),
    Point(Rect.Right - 1, Rect.Top),
    Point(Rect.Right - 1, Rect.Bottom - 1),
    Point(Rect.Left, Rect.Bottom - 1),
    Point(Rect.Left, Rect.Top)
  };
  Chart->PaintCanvas->Pen->Mode = pmXor;
  Chart->PaintCanvas->Pen->Style = psDot;
  Chart->PaintCanvas->Pen->Color = clBlack;
  Chart->PaintCanvas->Polyline(Points, 4);
}

void mx_ext_chart_select_t::DoMouseDown(TObject *Sender, TMouseButton Button,
  TShiftState Shift, int X, int Y)
{
  if (PositioningMode)
  {
    float_type AL = Chart->Area.Left, AB = Chart->Area.Bottom;
    float_type AW = Chart->Area.Right - AL, AH = Chart->Area.Top - AB;
    float_type GW = Chart->GridRect.Right - Chart->GridRect.Left - 1;
    float_type GH = Chart->GridRect.Bottom - Chart->GridRect.Top - 1;
    float_type DX = AL + (X - Chart->GridRect.Left)*AW/GW;
    float_type DY = AB + (Chart->GridRect.Bottom - Y)*AH/GH;
    Positioning(DX, DY);
    return;
  }
  if (DragChartMode)
  {
    float_type AL = Chart->Area.Left, AB = Chart->Area.Bottom;
    float_type AW = Chart->Area.Right - AL, AH = Chart->Area.Top - AB;
    float_type GW = Chart->GridRect.Right - Chart->GridRect.Left - 1;
    float_type GH = Chart->GridRect.Bottom - Chart->GridRect.Top - 1;
    PX = AL + (X - Chart->GridRect.Left)*AW/GW;
    PY = AB + (Chart->GridRect.Bottom - Y)*AH/GH;
    SelectMode = true;
    return;
  }
  if (X < Chart->GridRect.Left && X >= Chart->GridRect.Right &&
    Y < Chart->GridRect.Top && Y >= Chart->GridRect.Bottom) return;
  SelectMode = true;
  FirstSelect = true;
  Xn = X; Yn = Y;
}

void mx_ext_chart_select_t::DoMouseMove(TObject *Sender, TShiftState Shift,
  int X, int Y)
{
  if (PositioningMode) return;
  if (DragChartMode && SelectMode)
  {
    float_type AL = Chart->Area.Left, AB = Chart->Area.Bottom;
    float_type AW = Chart->Area.Right - AL, AH = Chart->Area.Top - AB;
    float_type GW = Chart->GridRect.Right - Chart->GridRect.Left;
    float_type GH = Chart->GridRect.Bottom - Chart->GridRect.Top;
    float_type CX = Chart->GridRect.Left + GW/2 - X;
    float_type CY = Y - (Chart->GridRect.Top + GH/2);
    Positioning(PX + CX*AW/GW, PY + CY*AH/GH);
    return;
  }
  if (FirstSelect && abs(Xn - X) < 5 && abs(Yn - Y) < 5) return;
  if (SelectMode)
  {
    rect_int_type VRect;
    if (Shift.Contains(ssShift))
      VRect = Rect(Xn, Chart->GridRect.Top, X, Chart->GridRect.Bottom);
    else if (Shift.Contains(ssCtrl))
      VRect = Rect(Chart->GridRect.Left, Yn, Chart->GridRect.Right, Y);
    else
      VRect = Rect(Xn, Yn, X, Y);
    SelectClip(VRect);
    if (VRect != SelRect)
    {
      if (!FirstSelect) SelectRect(SelRect);
      SelectRect(VRect);
      FirstSelect = false;
    }
    SelRect = VRect;
  }
}

void mx_ext_chart_select_t::DoMouseUp(TObject *Sender, TMouseButton Button,
  TShiftState Shift, int X, int Y)
{
  if (PositioningMode) return;
  if (DragChartMode) { SelectMode = false; return; }
  if (FirstSelect) { SelectMode = false; FirstSelect = false; return; }
  if (SelectMode)
  {
    SelectMode = false;
    SelectRect(SelRect);
    float_type AL = Chart->Area.Left, AB = Chart->Area.Bottom;
    float_type AW = Chart->Area.Right - AL, AH = Chart->Area.Top - AB;
    float_type GW = Chart->GridRect.Right - Chart->GridRect.Left;
    float_type GH = Chart->GridRect.Bottom - Chart->GridRect.Top;
    rect_float_type Area;
    Area.Left = AL + (SelRect.Left - Chart->GridRect.Left)*AW/GW;
    Area.Right = AL + (SelRect.Right - Chart->GridRect.Left)*AW/GW;
    Area.Bottom = AB + (Chart->GridRect.Bottom - SelRect.Bottom)*AH/GH;
    Area.Top = AB + (Chart->GridRect.Bottom - SelRect.Top)*AH/GH;
    Areas.push_back(Chart->Area);
    BaseItems.push_back(Chart->BaseItem);
    SaveCompConvs();
    SaveAutoScales();
    SaveAreas();
    Chart->Area = Area;
    DoPaint();
  }
}

void mx_ext_chart_select_t::DoDblClick(TObject *Sender)
{
  if (PositioningMode) return;
  if (DragChartMode) return;
  ZoomOut();
}

void mx_ext_chart_select_t::All()
{
  if (Areas.size())
  {
    LockEvent = true;
    Chart->BaseItem = BaseItems.front(); BaseItems.clear();
    RestoreCompConvs();
    LockEvent = false;
    Chart->Area = Areas.front(); Areas.clear();
    RestoreAutoScales();
    SaveAreas();
    DoPaint();
  }
}

void mx_ext_chart_select_t::SaveAreas()
{
  ValidAreas = true;
  for (size_type i = 0; i < Chart->Count; i++) {
    MAreas[static_cast<int*>(Chart->Items[i]->DataY)] =
      Chart->Items[i]->Area;
  }
}

void mx_ext_chart_select_t::RestoreAreas()
{
  if (!ValidAreas) return;
  ValidAreas = false;
  maparea::iterator it;
  for (size_type i = 0; i < Chart->Count; i++)
  {
    it = MAreas.find(Chart->Items[i]->DataY);
    if (it != MAreas.end()) Chart->Items[i]->Area = it->second;
  }
  MAreas.clear();
}

void mx_ext_chart_select_t::SaveAutoScales()
{
  ValidAutoScales = true;
  AutoScales.insert(make_pair(Chart,
    make_pair(Chart->AutoScaleX, Chart->AutoScaleY)));
  for (size_type i = 0; i < Chart->Count; i++)
    AutoScales.insert(make_pair(Chart->Items[i],
      make_pair(Chart->Items[i]->AutoScaleX, Chart->Items[i]->AutoScaleY)));
}

void mx_ext_chart_select_t::RestoreAutoScales()
{
  if (!ValidAutoScales) return;
  ValidAutoScales = false;
  mapscale::iterator it;
  for (size_type i = 0; i < Chart->Count; i++)
  {
    it = AutoScales.find(Chart->Items[i]);
    if (it != AutoScales.end())
    {
      Chart->Items[i]->AutoScaleX = it->second.first;
      Chart->Items[i]->AutoScaleY = it->second.second;
    }
  }
  it = AutoScales.find(Chart);
  if (it != AutoScales.end())
  {
    Chart->AutoScaleX = it->second.first;
    Chart->AutoScaleY = it->second.second;
  }
  AutoScales.clear();
}

void mx_ext_chart_select_t::DoPaint()
{
  try { Chart->Paint(); }
  catch (...) { if (FOnError) FOnError(this); }
}

void mx_ext_chart_select_t::Positioning(float_type X, float_type Y)
{
  if (!Areas.size()) return;
  rect_float_type VArea = Chart->Area;
  float_type CX = VArea.Left + (VArea.Right - VArea.Left)/2;
  float_type CY = VArea.Bottom + (VArea.Top - VArea.Bottom)/2;
  Shift(X - CX, Y - CY);
}

void mx_ext_chart_select_t::Shift(float_type X, float_type Y)
{
  if (!Areas.size()) return;
  if (X == 0 && Y == 0) return;
  rect_float_type VArea = Chart->Area;
  if (VArea.Right + X > Areas.front().Right)
  {
    VArea.Left = Areas.front().Right - (VArea.Right - VArea.Left);
    VArea.Right = Areas.front().Right;
  }
  else
  if (VArea.Left + X < Areas.front().Left)
  {
    VArea.Right = Areas.front().Left + (VArea.Right - VArea.Left);
    VArea.Left = Areas.front().Left;
  }
  else
  {
    VArea.Right += X; VArea.Left += X;
  }
  if (VArea.Top + Y > Areas.front().Top)
  {
    VArea.Bottom = Areas.front().Top - (VArea.Top - VArea.Bottom);
    VArea.Top = Areas.front().Top;
  }
  else
  if (VArea.Bottom + Y < Areas.front().Bottom)
  {
    VArea.Top = Areas.front().Bottom + (VArea.Top - VArea.Bottom);
    VArea.Bottom = Areas.front().Bottom;
  }
  else
  {
    VArea.Top += Y; VArea.Bottom += Y;
  }
  SaveAreas();
  Chart->Area = VArea;
  DoPaint();
}

void mx_ext_chart_select_t::RestoreCompConvs()
{
  mapconv Convs = CompConvs.back();
  for (size_type i = 0; i < Chart->Count; i++)
  {
    Chart->Items[i]->CompConvX = Convs[Chart->Items[i]].first;
    Chart->Items[i]->CompConvY = Convs[Chart->Items[i]].second;
  }
  CompConvs.pop_back();
}

void mx_ext_chart_select_t::SaveCompConvs()
{
  mapconv Convs;
  for (size_type i = 0; i < Chart->Count; i++)
    Convs[Chart->Items[i]] =
      make_pair(Chart->Items[i]->CompConvX, Chart->Items[i]->CompConvY);
  CompConvs.push_back(Convs);
}

void mx_ext_chart_select_t::ZoomIn(float_type FX, float_type FY)
{
  if (FX < 0) FX = FZoomFactor; FY = fabs(FY);
  rect_float_type VArea;
  VArea.Left = Chart->Area.Left + (1 - FX)*
    (Chart->Area.Right - Chart->Area.Left)/2;
  VArea.Right = Chart->Area.Left + (1 + FX)*
    (Chart->Area.Right - Chart->Area.Left)/2;
  VArea.Bottom = Chart->Area.Bottom + (1 - FY)*
    (Chart->Area.Top - Chart->Area.Bottom)/2;
  VArea.Top = Chart->Area.Bottom + (1 + FY)*
    (Chart->Area.Top - Chart->Area.Bottom)/2;
  Areas.push_back(Chart->Area);
  BaseItems.push_back(Chart->BaseItem);
  SaveCompConvs();
  SaveAutoScales();
  SaveAreas();
  Chart->Area = VArea;
  DoPaint();
}

void mx_ext_chart_select_t::ZoomOut()
{
  if (Areas.size())
  {
    LockEvent = true;
    Chart->BaseItem = BaseItems.back(); BaseItems.pop_back();
    RestoreCompConvs();
    LockEvent = false;
    Chart->Area = Areas.back(); Areas.pop_back();
    if (!Areas.size()) RestoreAutoScales();
    SaveAreas();
    DoPaint();
  }
}

void mx_ext_chart_select_t::SetZoomFactor(float_type Value)
{
  if (Value < 1) FZoomFactor = Value; else FZoomFactor = 1/Value;
}

void mx_ext_chart_select_t::ItemChange(item_change_t Oper, size_type Index)
{
  if (LockEvent) return;
  switch (Oper)
  {
    case ic_delete: {
      AutoScales.erase(Chart->Items[Index]);
      vector<size_type>::iterator it = BaseItems.begin();
      while (it != BaseItems.end())
      {
        it = find(it, BaseItems.end(), Index);
        if (it != BaseItems.end())
        {
          BaseItems.erase(it);
          Areas.erase(Areas.begin() + (it - BaseItems.begin()));
        }
      }
      if (AutoScales.size() == 1 && AutoScales.begin()->first == Chart)
      {
        Chart->AutoScaleY = AutoScales.begin()->second.first;
        Chart->AutoScaleX = AutoScales.begin()->second.second;
        AutoScales.clear();
        //CurArea = DblRect(0, 0, 0, 0);
      }
    } break;
    case ic_clear: {
      mapscale::iterator itm = AutoScales.find(Chart);
      if (itm != AutoScales.end())
      {
        Chart->AutoScaleY = itm->second.first;
        Chart->AutoScaleX = itm->second.second;
        //CurArea = DblRect(0, 0, 0, 0);
      }
      AutoScales.clear(); //MAreas.clear();
      Areas.clear(); BaseItems.clear();
    } break;
    case ic_insert: {
      AutoScales.insert(make_pair(Chart->Items[Index], make_pair(
        Chart->Items[Index]->AutoScaleX, Chart->Items[Index]->AutoScaleY)));
      for(vector<size_type>::iterator i = BaseItems.begin(); i != BaseItems.end();
        i++)
      {
        if (*i >= Index) {
          (*i)++;
        }
      }
    } break;
    case ic_comp_conv_x:
    case ic_comp_conv_y: {
      mx_ext_chart_item_t* BaseItem = Chart->Items[Chart->BaseItem];
      mapscale::iterator itm = AutoScales.find(BaseItem);
      if (itm != AutoScales.end())
      {
        if (Oper == ic_comp_conv_x) BaseItem->AutoScaleY = itm->second.first;
        if (Oper == ic_comp_conv_y) BaseItem->AutoScaleX = itm->second.second;
      }
      itm = AutoScales.find(Chart);
      if (itm != AutoScales.end())
      {
        if (Oper == ic_comp_conv_x) Chart->AutoScaleY = itm->second.first;
        if (Oper == ic_comp_conv_y) Chart->AutoScaleX = itm->second.second;
      }
    } break;
    case ic_set_base_item: {
      mapscale::iterator itm = AutoScales.find(Chart);
      if (itm != AutoScales.end())
      {
        Chart->AutoScaleY = itm->second.first;
        Chart->AutoScaleX = itm->second.second;
      }
    } break;
  }
}

void mx_ext_chart_select_t::SetPositioningMode(bool Value)
{
  FPositioningMode = Value;
  if (Value) FDragChartMode = false;
}

void mx_ext_chart_select_t::SetDragChartMode(bool Value)
{
  FDragChartMode = Value;
  if (Value) FPositioningMode = false;
}


} // namespace irs




#ifdef NOP
// Генератор цветов
irs::chart::color_gen_t::color_gen_t():
  m_colors(),
  m_names(),
  m_index(0)
{
  m_colors.push_back(clRed);
  m_colors.push_back(clBlue);
  m_colors.push_back(clGreen);
  m_colors.push_back(clPurple);
  m_colors.push_back(clMaroon);
  m_colors.push_back(clFuchsia);
  m_colors.push_back(clOlive);
  m_colors.push_back(clTeal);
  m_colors.push_back(clLime);
  m_colors.push_back(clNavy);

  m_names.push_back(irst("Красный"));
  m_names.push_back(irst("Синий"));
  m_names.push_back(irst("Зеленый"));
  m_names.push_back(irst("Сиреневый"));
  m_names.push_back(irst("Коричневый"));
  m_names.push_back(irst("Розовый"));
  m_names.push_back(irst("Оливковый"));
  m_names.push_back(irst("Голубой"));
  m_names.push_back(irst("Светло-зеленый"));
  m_names.push_back(irst("Темно-синий"));
}
void irs::chart::color_gen_t::start()
{
  m_index = 0;
}
TColor irs::chart::color_gen_t::next()
{
  TColor color = m_colors[m_index];
  ++m_index;
  int size = (int)m_colors.size();
  if (m_index >= size) m_index = 0;
  return color;
}
String irs::chart::color_gen_t::name_next()
{
  String name = m_names[m_index].c_str();
  ++m_index;
  int size = (int)m_colors.size();
  if (m_index >= size) m_index = 0;
  return name;
}
TColor irs::chart::color_gen_t::get(int a_index) const
{
  int size = (int)m_colors.size();
  return m_colors[a_index%size];
}
String irs::chart::color_gen_t::name(int a_index) const
{
  int size = (int)m_colors.size();
  return m_names[a_index%size].c_str();
}
int irs::chart::color_gen_t::size() const
{
  return (int)m_colors.size();
}

// Окно с графиком для C++ Builder
irs::chart::builder_chart_window_t::
builder_chart_window_t(irs_u32 a_size, irs_i32 a_refresh_time_ms,
  stay_on_top_t a_stay_on_top):
  //m_time(),
  m_data(),
  m_size(a_size),
  m_event(*this),
  mp_form(new TChartForm(m_data, m_event, a_refresh_time_ms, a_stay_on_top)),
  m_position(),
  m_stay_on_top(a_stay_on_top),
  m_chart_index(0)
{
}
void irs::chart::builder_chart_window_t::
show()
{
  mp_form->Show();
}
void irs::chart::builder_chart_window_t::
hide()
{
  mp_form->Hide();
}
irs::rect_t irs::chart::builder_chart_window_t::
position() const
{
  rect_t pos(mp_form->Left, mp_form->Top, mp_form->Width, mp_form->Height);
  return pos;
}
void irs::chart::builder_chart_window_t::
set_position(const rect_t &a_position)
{
  mp_form->Position = poDesigned;
  mp_form->Left = a_position.left;
  mp_form->Top = a_position.top;
  mp_form->Width = a_position.width();
  mp_form->Height = a_position.height();
}
void irs::chart::builder_chart_window_t::
add_param(const string_type &a_name)
{
  m_data[a_name] = chart_point_t();
  m_data[a_name].vec.push_back(0.);
  m_data[a_name].vectime.push_back(0.);
  m_data[a_name].index = m_chart_index;
  m_chart_index++;
  mp_form->invalidate();
  mp_form->chart_list_changed();
}
void irs::chart::builder_chart_window_t::
delete_param(const string_type &a_name)
{
  data_t::iterator data_it = m_data.find(a_name);
  if (data_it != m_data.end()) {
    m_data.erase(data_it);
  }
  mp_form->invalidate();
  mp_form->chart_list_changed();
}
void irs::chart::builder_chart_window_t::
clear_param()
{
  m_data.clear();
  m_chart_index = 0;
  mp_form->invalidate();
  mp_form->chart_list_changed();
}
void irs::chart::builder_chart_window_t::
set_value(const string_type &a_name, double a_value)
{
  data_t::iterator it = m_data.find(a_name);
  if (it != m_data.end()) {
    (*it).second.vec.back() = a_value;
  } else {
    // Ничего если параметра нет
  }
}
void irs::chart::builder_chart_window_t::
set_value(const string_type &a_name, double a_time, double a_value)
{
  data_t::iterator it = m_data.find(a_name);
  if (it != m_data.end()) {
    (*it).second.vectime.back() = a_time;
    (*it).second.vec.back() = a_value;
  } else {
    // Ничего если параметра нет
  }
}
void irs::chart::builder_chart_window_t::
set_time(double a_time)
{
  for (data_t::iterator it = m_data.begin(); it != m_data.end(); it++) {
    (*it).second.vectime.back() = a_time;
  }
}
void irs::chart::builder_chart_window_t::
add()
{
  for (data_t::iterator it = m_data.begin(); it != m_data.end(); it++) {
    (*it).second.vectime.push_back(0.);
    (*it).second.vec.push_back(0.);
  }
  if ((cur_size() > size()) || mp_form->fix()) {
    for (data_t::iterator it = m_data.begin(); it != m_data.end(); it++) {
      (*it).second.vectime.pop_front();
      (*it).second.vec.pop_front();
    }
  }
  mp_form->invalidate();
}
void irs::chart::builder_chart_window_t::
add(const string_type &a_name, double a_time, double a_value)
{
  data_t::iterator it = m_data.find(a_name);
  if (it != m_data.end()) {
    (*it).second.vectime.back() = a_time;
    (*it).second.vec.back() = a_value;
    (*it).second.vectime.push_back(0.);
    (*it).second.vec.push_back(0.);
    if ((*it).second.func->size() > size() || mp_form->fix()) {
      (*it).second.vectime.pop_front();
      (*it).second.vec.pop_front();
    }
    mp_form->invalidate();
  } else {
    // Ничего если параметра нет
  }
}
void irs::chart::builder_chart_window_t::
clear()
{
  for (data_t::iterator it = m_data.begin(); it != m_data.end(); it++) {
    chart_vec_t &vectime = (*it).second.vectime;
    vectime.clear();
    vectime.push_back(0.);
    chart_vec_t &vec = (*it).second.vec;
    vec.clear();
    vec.push_back(0.);
  }
  mp_form->invalidate();
}
void irs::chart::builder_chart_window_t::
set_refresh_time(irs_i32 a_refresh_time_ms)
{
  mp_form->set_refresh_time_ms(a_refresh_time_ms);
}
void irs::chart::builder_chart_window_t::
resize(irs_u32 a_size)
{
  m_size = a_size;
  if (cur_size() > m_size) {
    for (data_t::iterator it = m_data.begin(); it != m_data.end(); it++) {
      (*it).second.vectime.resize(m_size);
      (*it).second.vec.resize(m_size);
    }
  }
  mp_form->invalidate();
}
irs_u32 irs::chart::builder_chart_window_t::
size() const
{
  return m_size;
}
irs_u32 irs::chart::builder_chart_window_t::
cur_size() const
{
  if (m_data.size() > 0) return (*m_data.begin()).second.vec.size();
  return 0;
}
void irs::chart::builder_chart_window_t::
group_all()
{
  mp_form->group_all();
}
void irs::chart::builder_chart_window_t::
ungroup_all()
{
  mp_form->ungroup_all();
}

irs::chart::builder_chart_window_t::chart_func_t::
chart_func_t(const chart_vec_t &a_data):
  m_data(a_data)
{
}
double irs::chart::builder_chart_window_t::chart_func_t::
fn(double a_index)
{
  if (m_data.size() < 2) return 0.;
  if (a_index < 0.) return m_data.front();
  chart_vec_t::size_type index = static_cast<chart_vec_t::size_type>(a_index);
  chart_vec_t::size_type size = m_data.size() - 1;
  if (a_index > size + 0.9) return m_data.back();
  return m_data[index];
}
irs_u32 irs::chart::builder_chart_window_t::chart_func_t::
size() const
{
  chart_vec_t::size_type size = m_data.size();
  if (size < 1) return 0;
  return size - 1;
}

irs::chart::builder_chart_window_t::chart_point_t::
chart_point_t():
  vec(),
  vectime(),
  func(new chart_func_t(vec)),
  time(new chart_func_t(vectime)),
  index(0)
{}
irs::chart::builder_chart_window_t::chart_point_t::
chart_point_t(const chart_point_t& a_point):
  vec(a_point.vec),
  vectime(a_point.vectime),
  func(new chart_func_t(vec)),
  time(new chart_func_t(vectime)),
  index(a_point.index)
{}
irs::chart::builder_chart_window_t::chart_point_t&
irs::chart::builder_chart_window_t::chart_point_t::
operator=(const chart_point_t& a_point)
{
  vec = a_point.vec;
  vectime = a_point.vectime;
  func.reset(new chart_func_t(vec));
  time.reset(new chart_func_t(vectime));
  index = a_point.index;
  return *this;
}

// Пересортировка data_t в порядке добавления
irs::chart::builder_chart_window_t::unsort_data_t::
unsort_data_t():
  mp_data(IRS_NULL),
  m_unsort_data(),
  mp_unsort_data_it(),
  m_bad_point()
{
}
void irs::chart::builder_chart_window_t::unsort_data_t::
connect(const data_t &a_data)
{
  mp_data = &a_data;
  m_unsort_data.clear();
  for (data_t::const_iterator it = a_data.begin();
    it != a_data.end(); it++) {
    m_unsort_data[it->second.index] = it->first;
  }
  mp_unsort_data_it = m_unsort_data.begin();
}
bool irs::chart::builder_chart_window_t::unsort_data_t::
empty()
{
  if (mp_data == IRS_NULL) return false;
  return mp_data->size() == 0;
}
void irs::chart::builder_chart_window_t::unsort_data_t::
start()
{
  mp_unsort_data_it = m_unsort_data.begin();
}
void irs::chart::builder_chart_window_t::unsort_data_t::
next()
{
  if (mp_unsort_data_it == m_unsort_data.end()) {
    mp_unsort_data_it = m_unsort_data.begin();
  } else {
    mp_unsort_data_it++;
    if (mp_unsort_data_it == m_unsort_data.end()) {
      mp_unsort_data_it = m_unsort_data.begin();
    }
  }
}
irs::chart::builder_chart_window_t::string_type
irs::chart::builder_chart_window_t::unsort_data_t::
name()
{
  if (mp_unsort_data_it == m_unsort_data.end()) {
    return "";
  }
  return mp_unsort_data_it->second;
}
const irs::chart::builder_chart_window_t::chart_point_t&
irs::chart::builder_chart_window_t::unsort_data_t::
vec()
{
  if (mp_data == IRS_NULL) return m_bad_point;
  if (mp_unsort_data_it == m_unsort_data.end()) return m_bad_point;
  data_t::const_iterator data_it = mp_data->find(mp_unsort_data_it->second);
  return data_it->second;
}
irs::chart::builder_chart_window_t::unsort_data_t::size_type
irs::chart::builder_chart_window_t::unsort_data_t::
size()
{
  if (mp_data == IRS_NULL) return 0;
  return mp_data->size();
}
void irs::chart::builder_chart_window_t::unsort_data_t::
set(size_type a_index)
{
  mp_unsort_data_it = m_unsort_data.begin();
  advance(mp_unsort_data_it, a_index);
}

// Форма с графиком
irs::chart::builder_chart_window_t::TChartForm::
  TChartForm(const data_t &a_data, chart_event_t &a_event,
  irs_i32 a_refresh_time_ms, stay_on_top_t a_stay_on_top):
  TForm(IRS_NULL, 1),
  m_group_all(irs_false),
  //m_pause_time(),
  m_pause_data(),
  m_pause(irs_false),
  m_fix(irs_false),
  //m_time(a_time),
  m_data(a_data),
  m_event(a_event),
  m_IdleEvent(Application->OnIdle),
  mp_panel(new TPanel(this)),
  mp_chart_box(new TPaintBox(this)),
  mp_chart(new mx_ext_chart_t(this, mp_chart_box->Canvas)),
  mp_select(new mx_ext_chart_select_t(mp_chart, mp_chart_box)),
  mp_timer(new TTimer(this)),
  mp_pause_btn(new TButton(this)),
  mp_dbg_info(new TLabel(this)),
  mp_fix_btn(new TButton(this)),
  mp_clear_btn(new TButton(this)),
  mp_base_chart_combo(new TComboBox(this)),
  mp_pause_on_text("Пауза"),
  mp_pause_off_text("Продолжить"),
  mp_fix_on_text("Фиксация"),
  mp_fix_off_text("Разфиксация"),
  m_refresh_time_ms(a_refresh_time_ms),
  m_invalidate(irs_false),
  m_time_int(0.),
  mp_data(&m_data),
  m_base_item(0),
  m_colors(),
  m_is_lock(true),
  m_is_chart_list_changed(false),
  m_base_chart_name(irst("")),
  m_unsort_data()
{
  m_unsort_data.connect(m_data);

  Caption = irst("График");
  Width = 700;
  Height = 500;
  if (a_stay_on_top == stay_on_top_on) {
    FormStyle = fsStayOnTop;
  }
  Position = poDesktopCenter;
  OnResize = FormResize;
  OnShow = FormShow;
  OnHide = FormHide;
  //Parent = ap_parent_form;

  const irs_i32 btn_gap = 10;

  BorderIcons = BorderIcons >> biMinimize;
  //biSystemMenu >>
  mp_panel->Align = alTop;
  InsertControl(mp_panel);
  mp_chart_box->Align = alClient;
  mp_chart_box->OnPaint = ChartPaint;
  mp_chart_box->Parent = this;
  //InsertControl(mp_chart_box.get());
  mp_timer->Interval = m_refresh_time_ms;
  mp_timer->OnTimer = TimerEvent;
  mp_pause_btn->Caption = mp_pause_on_text;
  mp_pause_btn->OnClick = PauseBtnClick;
  mp_pause_btn->Left = 10;
  mp_pause_btn->Top = mp_panel->Height/2 - mp_pause_btn->Height/2;
  mp_pause_btn->Parent = this;
  //InsertControl(mp_pause_btn.get());
  mp_dbg_info->Left = mp_panel->Left + mp_panel->Width - 50;
  //mp_dbg_info->Top = mp_panel->Height/2 - mp_dbg_info->Height/2;
  mp_dbg_info->Top = 0;
  mp_dbg_info->Anchors = mp_dbg_info->Anchors >> akLeft;
  mp_dbg_info->Anchors = mp_dbg_info->Anchors << akRight;
  mp_dbg_info->Parent = this;
  //InsertControl(mp_dbg_info.get());
  mp_fix_btn->Left = mp_pause_btn->Left + mp_pause_btn->Width + btn_gap;
  mp_fix_btn->Top = mp_panel->Height/2 - mp_fix_btn->Height/2;
  mp_fix_btn->Caption = mp_fix_on_text;
  mp_fix_btn->OnClick = FixBtnClick;
  mp_fix_btn->Parent = this;
  //InsertControl(mp_fix_btn.get());
  mp_clear_btn->Left = mp_fix_btn->Left + mp_fix_btn->Width + btn_gap;
  mp_clear_btn->Top = mp_panel->Height/2 - mp_clear_btn->Height/2;
  mp_clear_btn->Caption = "Очистить";
  mp_clear_btn->OnClick = ClearBtnClick;
  mp_clear_btn->Parent = this;
  //InsertControl(mp_clear_btn.get());
  mp_base_chart_combo->Left = mp_clear_btn->Left + mp_clear_btn->Width +
    btn_gap;
  mp_base_chart_combo->Top = mp_panel->Height/2 -
    mp_base_chart_combo->Height/2;
  mp_base_chart_combo->Width = Width - mp_base_chart_combo->Left -
    btn_gap;
  mp_base_chart_combo->Anchors = mp_base_chart_combo->Anchors << akRight;
  mp_base_chart_combo->Style = Stdctrls::csDropDownList;
  mp_base_chart_combo->OnChange = BaseChartComboChange;
  mp_base_chart_combo->Parent = this;
}
irs::chart::builder_chart_window_t::TChartForm::
  ~TChartForm()
{
}
void irs::chart::builder_chart_window_t::TChartForm::
  FormResize(TObject *Sender)
{
  mp_chart->BoundsRect = mp_chart_box->ClientRect;
}
void irs::chart::builder_chart_window_t::TChartForm::
  TimerEvent(TObject *Sender)
{
  try {

    if (m_pause) {
      if (m_invalidate) {
        mp_chart->BaseItem = chart_from_combo_item(m_base_item);
        mp_chart->Paint();
      }
    } else {
      if (!m_is_lock) {
        connect_data(m_data);
      }
    }

  } catch (Exception &exception) {
    mp_timer->Enabled = false;
    Application->ShowException(&exception);
    exit(1);
  } catch (...) {
    mp_timer->Enabled = false;
    try {
      throw Exception("");
    } catch (Exception &exception) {
      Application->ShowException(&exception);
    }
    exit(1);
  }
}
void irs::chart::builder_chart_window_t::TChartForm::
  PauseBtnClick(TObject *Sender)
{
  m_is_chart_list_changed = true;
  m_invalidate = true;
  if (m_pause) {
    m_pause = irs_false;
    mp_pause_btn->Caption = mp_pause_on_text;
  } else {
    m_pause = irs_true;
    mp_pause_btn->Caption = mp_pause_off_text;
    m_pause_data = m_data;
    connect_data(m_pause_data);
  }
  TimerEvent(this);
}
void irs::chart::builder_chart_window_t::TChartForm::
  FixBtnClick(TObject *Sender)
{
  if (m_fix) {
    m_fix = irs_false;
    mp_fix_btn->Caption = mp_fix_on_text;
  } else {
    m_fix = irs_true;
    mp_fix_btn->Caption = mp_fix_off_text;
  }
}
void irs::chart::builder_chart_window_t::TChartForm::
  ClearBtnClick(TObject *Sender)
{
  m_event.clear();
}
void irs::chart::builder_chart_window_t::TChartForm::
  ChartPaint(TObject *Sender)
{
  mp_chart->Paint();
}
void irs::chart::builder_chart_window_t::TChartForm::
  FormShow(TObject *Sender)
{
  m_is_lock = false;
  m_invalidate = irs_true;
  TimerEvent(this);
}
void irs::chart::builder_chart_window_t::TChartForm::
  FormHide(TObject *Sender)
{
  m_is_lock = true;
}
void irs::chart::builder_chart_window_t::TChartForm::
  BaseChartComboChange(TObject *Sender)
{
  set_base_item(mp_base_chart_combo->ItemIndex);
  m_invalidate = irs_true;
  TimerEvent(this);
}
int irs::chart::builder_chart_window_t::TChartForm::
  chart_from_combo_item(int a_combo_item)
{
  const int bad_real_base_item = -1;
  int real_base_item = bad_real_base_item;
  int real_index = 0;
  for (unsort_data_t::size_type i = 0; i < m_unsort_data.size(); i++) {
    const chart_func_t *func = m_unsort_data.vec().func.get();
    if (func->size()) {
      if ((int)i == a_combo_item) {
        real_base_item = real_index;
        break;
      }
      real_index++;
    }
    m_unsort_data.next();
  }
  int chart_item = 0;
  if (real_base_item != bad_real_base_item) {
    chart_item = real_base_item;
  } else {
    //int chart_item = 0;
  }
  return chart_item;
}
void irs::chart::builder_chart_window_t::TChartForm::
  connect_data(const data_t &a_data)
{
  mp_data = &a_data;
  if (m_is_chart_list_changed) {
    m_is_chart_list_changed = false;
    m_unsort_data.connect(a_data);
    update_chart_combo();
  }
  if (m_invalidate) {
    m_invalidate = irs_false;
    mp_chart->Clear();
    m_colors.start();
    m_unsort_data.start();
    for (unsort_data_t::size_type i = 0; i < m_unsort_data.size(); i++) {
      const chart_func_t *time_func = m_unsort_data.vec().time.get();
      const chart_func_t *func = m_unsort_data.vec().func.get();
      TColor color = m_colors.next();
      if (func->size()) {
        mp_chart->Add(&func->fn, func->size(), color);
        mp_chart->Last->DataX = &time_func->fn;
        if (!m_group_all) mp_chart->NewGroupY();
      }
      m_unsort_data.next();
    }
    mp_chart->BaseItem = chart_from_combo_item(m_base_item);
    mp_chart->Paint();
  }
}
inline irs_bool irs::chart::builder_chart_window_t::TChartForm::
  fix()
{
  return m_fix;
}
inline void irs::chart::builder_chart_window_t::TChartForm::
  group_all()
{
  m_group_all = irs_true;
  m_invalidate = irs_true;
}
inline void irs::chart::builder_chart_window_t::TChartForm::
  ungroup_all()
{
  m_group_all = irs_false;
  m_invalidate = irs_true;
}
inline void irs::chart::builder_chart_window_t::TChartForm::
  invalidate()
{
  m_invalidate = irs_true;
}
void irs::chart::builder_chart_window_t::TChartForm::
  chart_list_changed()
{
  m_is_chart_list_changed = true;
}
void irs::chart::builder_chart_window_t::TChartForm::
  set_refresh_time_ms(irs_i32 a_refresh_time_ms)
{
  if (a_refresh_time_ms != 0) {
    mp_timer->Interval = a_refresh_time_ms;
  } else {
    mp_timer->Interval = 1;
  }
  mp_timer->Enabled = false;
  mp_timer->Enabled = true;
}
void irs::chart::builder_chart_window_t::TChartForm::
  update_chart_combo()
{
  mp_base_chart_combo->Items->Clear();
  m_colors.start();
  m_unsort_data.start();
  for (unsort_data_t::size_type i = 0; i < m_unsort_data.size(); i++) {
    String item_name = String(m_unsort_data.name().c_str()) + " - " +
      m_colors.name_next();
    mp_base_chart_combo->Items->Add(item_name);
    if (m_unsort_data.name() == m_base_chart_name) {
      set_base_item(mp_base_chart_combo->Items->Count - 1);
      mp_base_chart_combo->ItemIndex = m_base_item;
    }
    m_unsort_data.next();
  }

  if (mp_base_chart_combo->ItemIndex == -1)
  if (mp_base_chart_combo->Items->Count > 0) {
    set_base_item(0);
    mp_base_chart_combo->ItemIndex = m_base_item;
  }
}
void irs::chart::builder_chart_window_t::TChartForm::
  set_base_item(int a_base_item)
{
  if (a_base_item < 0) return;
  m_base_item = a_base_item;

  m_base_chart_name.clear();
  if (m_base_item < (int)m_unsort_data.size()) {
    m_unsort_data.set(m_base_item);
    m_base_chart_name = m_unsort_data.name();
  }
}
#endif //NOP

#endif //defined(QT_CORE_LIB) || defined(__BORLANDC__)
#endif //IRS_FULL_STDCPPLIB_SUPPORT
#endif //MXEXT_OFF
