// Построение графиков
// C++ Builder
// Дата: 03.09.2010
// Дата создания: 8.09.2008
//---------------------------------------------------------------------------
#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <timer.h>

#include <MxChart.h>

#include <irsfinal.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
namespace Mxchart
{

const double Inf = numeric_limits<double>::infinity();
const int MinExp = numeric_limits<double>::min_exponent10;
const double DblError = 1e-9;

//***************************************************************************
// Глобальные функции

#if __BORLANDC__ < IRS_CPP_BUILDER2007
//---------------------------------------------------------------------------
bool __fastcall operator == (TPoint P1, TPoint P2)
{
  return P1.x == P2.x && P1.y == P2.y;
}
//---------------------------------------------------------------------------
bool __fastcall operator != (TRect R1, TRect R2)
{
  return R1.Left != R2.Left || R1.Top != R2.Top || R1.Right != R2.Right ||
    R1.Bottom != R2.Bottom;
}
#endif //__BORLANDC__ < IRS_CPP_BUILDER2010
//---------------------------------------------------------------------------
int __fastcall BeginChartRange(TMxChart *Chart, int Index)
{
  TDblRect A = Chart->Area;
  TMxChartItem *Item;
  if (Index == -1) Item = Chart->Last; else Item = Chart->Items[Index];
  TDblBounds B = Item->Bounds;
  double a = Item->ScaleX, b = Item->ShiftX;
  double L;
  if ((int *)Item->DataX)
  {
    for (int i = static_cast<int>(B.Begin); i <= static_cast<int>(B.End); i++)
      if ((Item->DataX[i] - b)/a > A.Left) { L = i; break; }
      else if (FltEqual((Item->DataX[i] - b)/a, A.Left)) { L = i; break; }
  }
  else
    L = (A.Left - b)/a;
  if (L < B.Begin) L = B.Begin;
  return static_cast<int>(ceil(L));
}
//---------------------------------------------------------------------------
int __fastcall EndChartRange(TMxChart *Chart, int Index)
{
  TDblRect A = Chart->Area;
  TMxChartItem *Item;
  if (Index == -1) Item = Chart->Last; else Item = Chart->Items[Index];
  TDblBounds B = Item->Bounds;
  double a = Item->ScaleX, b = Item->ShiftX;
  double L;
  if ((int *)Item->DataX)
  {
    for (int i = static_cast<int>(B.End); i >= static_cast<int>(B.Begin); i--)
      if ((Item->DataX[i] - b)/a < A.Right) { L = i; break; }
      else if (FltEqual((Item->DataX[i] - b)/a, A.Right)) { L = i; break; }
  }
  else
    L = (A.Right - b)/a;
  if (L > B.End) L = B.End;
  return static_cast<int>(floor(L));
}
//---------------------------------------------------------------------------
double __fastcall RoundTo2(double x, int Dig2 = 5)
{
  union { double y; __int64 i64; };
  y = x; i64 >> Dig2; i64 << Dig2;
  return y;
}
//---------------------------------------------------------------------------
double __fastcall AddMant(double x, double am = DblError)
{
  int e; double m = frexp(x, &e) + am;
  return ldexp(m, e);
}
//---------------------------------------------------------------------------
double __fastcall SubMant(double x, double am = DblError)
{
  return AddMant(x, -am);
}
//---------------------------------------------------------------------------
bool __fastcall KeyPress(int VirtKey)
{
  return GetAsyncKeyState(VirtKey) & 0x8000;
}
//---------------------------------------------------------------------------
bool __fastcall AnyKeyPress()
{
  for(register int i = 0; i < 256; i++) if (KeyPress(i)) return true;
  return false;
}
//---------------------------------------------------------------------------

//***************************************************************************
// TMxChartItem - Компонент на TMxChart

//---------------------------------------------------------------------------
// Конструктор компонета
__fastcall TMxChartItem::TMxChartItem(TComponent *AOwner)
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
  //int FGroup[2];
  FShiftX(0.),
  FShiftY(0.),
  FScaleX(1.),
  FScaleY(1.),
  FPen(new TPen()),
  FMarkerPen(new TPen())
{
  for (int i = 0; i < 2; i++) {
    FFunc[i] = DefFunc;
    FAutoScale[i] = true;
    FData[i] = (int *)IRS_NULL;
    FGroup[i] = 0;
  }
}
//---------------------------------------------------------------------------
// Деструктор компонета
__fastcall TMxChartItem::~TMxChartItem()
{
  delete FPen;
  FPen = NULL;
  delete FMarkerPen;
  FMarkerPen = NULL;
}
//---------------------------------------------------------------------------
TPoint __fastcall TMxChartItem::ConvCoor(TDblPoint P)
{
  double k = 0; int x = 0, y = 0;
  try
  {
    if (FArea.Right != FArea.Left)
      k = (P.x - FArea.Left)/(FArea.Right - FArea.Left);
    x = static_cast<int>(BoundsRect.Left + (Width - 1)*k);
  }
  catch (Exception &e)
  { FError = true; if (FOnError) FOnError(this, e, P, 0, cetConvX); }
  try
  {
    if (FArea.Top != FArea.Bottom)
      k = (P.y - FArea.Bottom)/(FArea.Top - FArea.Bottom);
    y = static_cast<int>(BoundsRect.Bottom - (Height - 1)*k - 1);
  }
  catch (Exception &e)
  { FError = true; if (FOnError) FOnError(this, e, P, 0, cetConvY); }
  return Point(x, y);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::Invalidate()
{
  NeedCalculate = NeedAutoScale = true;
}
//---------------------------------------------------------------------------
TDblPoint __fastcall TMxChartItem::XYFunc(double t) const
{
  return DblPoint(FFunc[0](t), FFunc[1](t));
}
//---------------------------------------------------------------------------
bool __fastcall TMxChartItem::IntoArea(TDblPoint P) const
{
  return (P.x >= ClipArea.Left) && (P.x <= ClipArea.Right) &&
    (P.y >= ClipArea.Bottom) && (P.y <= ClipArea.Top);
}
//---------------------------------------------------------------------------
bool __fastcall TMxChartItem::OutArea(TDblPoint P1, TDblPoint P2) const
{
  return (P1.x < ClipArea.Left) && (P2.x < ClipArea.Left) ||
         (P1.x > ClipArea.Right) && (P2.x > ClipArea.Right) ||
         (P1.y < ClipArea.Bottom) && (P2.y < ClipArea.Bottom) ||
         (P1.y > ClipArea.Top && P2.y > ClipArea.Top);
}
//---------------------------------------------------------------------------
bool __fastcall TMxChartItem::ClipLine(TDblPoint &P1, TDblPoint &P2) const
{
  if (OutArea(P1, P2)) return false;
  bool P1Into = IntoArea(P1), P2Into = IntoArea(P2);
  if (P1Into && P2Into) return true;
  vector<TDblPoint> P;
  if (P1Into) P.push_back(P1); if (P2Into) P.push_back(P2);
  double x1 = ClipArea.Left, x2 = ClipArea.Right;
  double y1 = ClipArea.Bottom, y2 = ClipArea.Top;
  double xs = min(P1.x, P2.x), xe = max(P1.x, P2.x);
  double ys = min(P1.y, P2.y), ye = max(P1.y, P2.y);
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
    double a = (P2.y - P1.y)/(P2.x - P1.x), b = P1.y - a*P1.x;
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
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::DoCalculate()
{
  TPoint P = TPoint(), OldP = TPoint();
  TDblPoint DP = TDblPoint(), OldDP = TDblPoint();
  double Begin = 0., End = 0.;
  double AW = FArea.Right - FArea.Left; double AH = FArea.Top - FArea.Bottom;
  int B = 0, E = 0, L = 0;
  ClipArea = DblRect(FArea.Left - 0.1*AW, FArea.Top + 0.1*AH,
    FArea.Right + 0.1*AW, FArea.Bottom - 0.1*AH);
  Lines.clear();
  if (!(int *)DataX)
  {
    Begin = max(FStep*floor(FArea.Left/FStep), Bounds.Begin);
    End = min(FStep*ceil(FArea.Right/FStep), Bounds.End);
    B = ConvCoor(DblPoint(Begin, 0)).x;
    E = ConvCoor(DblPoint(End, 0)).x;
    L = E - B + 1;
  }
  else if (!(int *)DataY)
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
  if (!(int *)DataX && (End - Begin)/Step > 2*L)
  {
    double dA = (End - Begin)/L;
    double t = Begin, y = 0.;
    for(int i = 0; i < L; i++)
    {
      TPoint P1, P2;
      double t2 = Begin + (i + 1)*dA - Step/4;
      FError = false;
      double Max = FFunc[1](t), Min = Max; t += Step;
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
      double s = y, e = FFunc[1](t);
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
  else if (!(int *)DataY && (End - Begin)/Step > 2*L)
  {
    double dA = (End - Begin)/L;
    double t = Begin, x = 0.;
    for(int i = 0; i < L; i++)
    {
      TPoint P1, P2;
      double t2 = Begin + (i + 1)*dA - Step/4;
      FError = false;
      double Max = FFunc[0](t), Min = Max;
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
      double s = x, e = FFunc[0](t);
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
    for(double t = Begin + FStep; t < End; t += FStep)
    {
      FError = false;
      DP = XYFunc(t);
      TDblPoint OldDP1 = OldDP, DP1 = DP;
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
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::Paint(int Tag)
{
  if (!ValidRect()) return;
  TCanvas *Dev = FCanvas;
  TPen *OldPen = new TPen(); OldPen->Assign(Dev->Pen);
  // Настройка инструментов для рисования
  Dev->Pen = FPen;
  switch (Tag)
  {
    case 1:
      Dev->Pen->Color = clBlack;
    case 2:
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
  else
    for (unsigned i = 0; i < Lines.size(); i++) LineP(Dev, Lines[i]);
  // Отображение маркеров
  for(int i=0; i<IndexMarkerX; i++) PaintMarkerX(i);
  for(int i=0; i<IndexMarkerY; i++) PaintMarkerY(i);
  Dev->Pen = OldPen;
  delete OldPen;
  OldPen = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::DoPaint()
{
  Paint(0);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::DoBlackPrint()
{
  Paint(1);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::DoColorPrint()
{
  Paint(2);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetCanvas(TCanvas *Value)
{
  if (FCanvas != Value)
  {
    FCanvas = Value;
    if (FOnChange) FOnChange(this, cctCanvas);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::PaintMarkerX(int i)
{
  if (!ValidRect()) return;
  TMxChart *OwnerChart = (TMxChart *)Owner;
  OwnerChart->PreCalc();
  TCanvas *Dev = FCanvas;
  TPen *OldPen = new TPen(); OldPen->Assign(Dev->Pen);
  Dev->Pen = FMarkerPen;
  Dev->Pen->Style = psSolid;
  Dev->Pen->Mode = pmNotXor;
  double XCoor = (FMarkerX[i] - FShiftX)/FScaleX;
  if (XCoor < FArea.Left && XCoor > FArea.Right)
    return;
  TPoint P1 = ConvCoor(DblPoint(XCoor, FArea.Top));
  if (GroupX == OwnerChart->Items[OwnerChart->BaseItem]->GroupX)
  { OwnerChart->PaintChildMarkerX(this, P1.x); return; }
  TPoint P2 = ConvCoor(DblPoint(XCoor, FArea.Bottom));
  DashLine(Dev, P1, P2, 5, 5);
  Dev->Pen = OldPen;
  delete OldPen;
  OldPen = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::PaintMarkerY(int i)
{
  if (!ValidRect()) return;
  TMxChart *OwnerChart = (TMxChart *)Owner;
  OwnerChart->PreCalc();
  TCanvas *Dev = FCanvas;
  TPen *OldPen = new TPen(); OldPen->Assign(Dev->Pen);
  Dev->Pen = FMarkerPen;
  Dev->Pen->Style = psSolid;
  Dev->Pen->Mode = pmNotXor;
  double YCoor = (FMarkerY[i] - FShiftY)/FScaleY;
  if (YCoor < FArea.Bottom && YCoor > FArea.Top)
    return;
  TPoint P1 = ConvCoor(DblPoint(FArea.Left, YCoor));
  if (GroupY == OwnerChart->Items[OwnerChart->BaseItem]->GroupY)
  { OwnerChart->PaintChildMarkerY(this, P1.y); return; }
  TPoint P2 = ConvCoor(DblPoint(FArea.Right, YCoor));
  DashLine(Dev, P1, P2, 5, 5);
  Dev->Pen = OldPen;
  delete OldPen;
  OldPen = NULL;
}
//---------------------------------------------------------------------------
// Представление массивов в виде функций
double TMxChartItem::Func(int i, double x) const
{
  return static_cast<double>(FData[i][static_cast<int>(x + 0.5)]);
}
//---------------------------------------------------------------------------
// Вспомогательная функция по X
double TMxChartItem::FuncX(double x)
{
  double f = 0;
  try { f = Func(0,x); }
  catch (Exception &e)
  {
    FError = true;
    if (FOnError) FOnError(this, e, DblPoint(0, 0), x, cetCalcX);
  }
  return f;
}
//---------------------------------------------------------------------------
// Вспомогательная функция по Y
double TMxChartItem::FuncY(double x)
{
  double f = 0;
  try { f = Func(1,x); }
  catch (Exception &e)
  {
    FError = true;
    if (FOnError) FOnError(this, e, DblPoint(0, 0), x, cetCalcY);
  }
  return f;
}
//---------------------------------------------------------------------------
// Установка данных (массивов или функций) для построения
void __fastcall TMxChartItem::SetData(int Index, TPointer Value)
{
  FData[Index] = Value;
  if ((double*)Value)
  {
    FHide = false;
    if ((Value.Type == dtFunction) || (Value.Type == dtMethod))
      FFunc[Index] = Value.operator TClassDblFunc();
    else
      if (Index == 0) FFunc[0] = FuncX; else FFunc[1] = FuncY;
  }
  else
    FFunc[Index] = DefFunc;
  NeedCalculate = NeedAutoScale = true;
  FloorAxis();
  if (FOnChange) FOnChange(this, cctData);
}
//---------------------------------------------------------------------------
// Выравнивание под массив
void __fastcall TMxChartItem::FloorAxis()
{
  if ((FData[0].Type != dtFunction) || (FData[1].Type != dtFunction) ||
        (FData[0].Type != dtMethod) || (FData[1].Type != dtMethod))
  {
    FBounds.Begin = floor(FBounds.Begin); FBounds.End = floor(FBounds.End);
    FStep = 1;
  }
}
//---------------------------------------------------------------------------
// Выполнение автомасштабирования
void __fastcall TMxChartItem::DoAutoScale()
{
  bool ChangeArea = false;
  double t, Max = 0, Min = 0;
  for(int i = 0; i < 2; i++)
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
//---------------------------------------------------------------------------
TCompConv __fastcall TMxChartItem::GetCompConv(int Index) const
{
  return FData[Index].CompConv;
}
//---------------------------------------------------------------------------
// Установка типа конверсии из cmp в double
void __fastcall TMxChartItem::SetCompConv(int Index, TCompConv Value)
{
	if ((Value >= ccReal) && (Value <= ccArg))
  if (FData[Index].CompConv != Value)
  {
    FData[Index].CompConv = Value;
    NeedCalculate = NeedAutoScale = true;
    if (FOnChange) FOnChange(this, cctCompConv);
    //** Для TMxChartSelect **
    TMxChart *Chart = (TMxChart *)Owner;
    int i = 0; for (; i < Chart->FCount && Chart->FItems[i] != this; i++)
    if (Chart->Select) Chart->Select->ItemChange(3 + Index, i);
    //************************
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetGroup(int Index, int AGroup)
{
  FGroup[Index] = AGroup;
  if (FOnChange) FOnChange(this, cctGroup);
}
//---------------------------------------------------------------------------
// Функция по умолчанию.
double TMxChartItem::DefFunc(double x) const
{
  return x;
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetPen(TPen *Value)
{
  FPen->Assign(Value);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetMarkerPen(TPen *Value)
{
  FMarkerPen->Assign(Value);
}
//---------------------------------------------------------------------------
// Установка области построения
void __fastcall TMxChartItem::SetArea(TDblRect AArea)
{
  bool AreaChange = false;
  double ScX = FScaleX, ScY = FScaleY;
  double ShX = FShiftX, ShY = FShiftY;
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
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetHide(bool Value)
{
  if ((int*)FData[0] || (int*)FData[1]) FHide = Value; else FHide = true;
  if (FOnChange) FOnChange(this, cctHide);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetShift(int Index, double Value)
{
  if (Index == 0) ShiftX = Value; else ShiftY = Value;
  if (FOnChange) FOnChange(this, cctShift);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetScale(int Index, double Value)
{
  if (Index == 0) ScaleX = Value; else ScaleY = Value;
  if (FOnChange) FOnChange(this, cctScale);
}
//---------------------------------------------------------------------------
// Установка пределов построения
void __fastcall TMxChartItem::SetBounds(TDblBounds Value)
{
	if ((Value.Begin < Value.End) && (FBounds != Value))
  {
    FBounds = Value; FloorAxis();
    NeedCalculate = NeedAutoScale = true;
    if (FOnChange) FOnChange(this, cctBounds);
  }
}
//---------------------------------------------------------------------------
// Установка шага
void __fastcall TMxChartItem::SetStep(double Step)
{
	if ((Step > 0) && (FStep != Step))
  {
    FStep = Step; FloorAxis();
    NeedCalculate = NeedAutoScale = true;
    if (FOnChange) FOnChange(this, cctStep);
  }
}
//---------------------------------------------------------------------------
// Установка автомасштабирования
void __fastcall TMxChartItem::SetAutoScale(int Index, bool Value)
{
	if (FAutoScale[Index] != Value)
  {
    FAutoScale[Index] = Value;
    if (Value) NeedCalculate = true;
    NeedAutoScale = true;
    if (FOnChange) FOnChange(this, cctAutoScale);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetBoundsRect(TRect Value)
{
  if ((Value.Right - Value.Left != FBoundsRect.Right - FBoundsRect.Left) ||
      (Value.Bottom - Value.Top != FBoundsRect.Bottom - FBoundsRect.Top))
    NeedCalculate = true;
  FPrevBoundsRect = FBoundsRect;
  FBoundsRect = Value;
  if (FOnChange) FOnChange(this, cctBoundsRect);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetLeft(int Value)
{
  TRect R = BoundsRect;
  R.Left = Value;
  BoundsRect = R;
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetTop(int Value)
{
  TRect R = BoundsRect;
  R.Top = Value;
  BoundsRect = R;
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetWidth(int Value)
{
  TRect R = BoundsRect;
  R.Right = R.Left + Value;
  BoundsRect = R;
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetHeight(int Value)
{
  TRect R = BoundsRect;
  R.Bottom = R.Top + Value;
  BoundsRect = R;
}
//---------------------------------------------------------------------------
TRect __fastcall TMxChartItem::GetBoundsRect() const
{
  ((TMxChart *)Owner)->PreCalc();
  return FBoundsRect;
}
//---------------------------------------------------------------------------
TDblRect __fastcall TMxChartItem::GetArea()
{
  if (NeedAutoScale) { NeedAutoScale = false; DoAutoScale(); }
  TDblRect VArea = FArea;
  double ScX = FScaleX, ScY = FScaleY;
  double ShX = FShiftX, ShY = FShiftY;
  return DblRect(VArea.Left*ScX + ShX, VArea.Top*ScY + ShY,
    VArea.Right*ScX + ShX, VArea.Bottom*ScY + ShY);
}
//---------------------------------------------------------------------------
int __fastcall TMxChartItem::GetLeft() const
{
  return FBoundsRect.Left;
}
//---------------------------------------------------------------------------
int __fastcall TMxChartItem::GetTop() const
{
  return FBoundsRect.Top;
}
//---------------------------------------------------------------------------
int __fastcall TMxChartItem::GetWidth() const
{
  return FBoundsRect.Right - FBoundsRect.Left;
}
//---------------------------------------------------------------------------
int __fastcall TMxChartItem::GetHeight() const
{
  return FBoundsRect.Bottom - FBoundsRect.Top;
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::AddMarkerX(double Value)
{
  if (FMarkerX != NULL) {
    FMarkerX = (double *)realloc(FMarkerX, (IndexMarkerX + 1)*sizeof(double));
  } else {
    FMarkerX = (double *)calloc(1, sizeof(double));
  }
  FMarkerX[IndexMarkerX] = Value;
  IndexMarkerX++;
  PaintMarkerX(IndexMarkerX - 1);
  if (FOnChange) FOnChange(this, cctMarker);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::AddMarkerY(double Value)
{
  if (FMarkerY != NULL) {
    FMarkerY = (double *)realloc(FMarkerY, (IndexMarkerY + 1)*sizeof(double));
  } else {
    FMarkerY = (double *)calloc(1, sizeof(double));
  }
  FMarkerY[IndexMarkerY] = Value;
  IndexMarkerY++;
  PaintMarkerY(IndexMarkerY - 1);
  if (FOnChange) FOnChange(this, cctMarker);
}
//---------------------------------------------------------------------------
double __fastcall TMxChartItem::GetMarkerX(int Index) const
{
  if (Index == -1) if (IndexMarkerX) return FMarkerX[IndexMarkerX - 1];
  if ((Index >= 0) && (Index < IndexMarkerX)) return FMarkerX[Index];
  return 0;
}
//---------------------------------------------------------------------------
double __fastcall TMxChartItem::GetMarkerY(int Index) const
{
  if (Index == -1) if (IndexMarkerY) return FMarkerY[IndexMarkerY - 1];
  if ((Index >= 0) && (Index < IndexMarkerY)) return FMarkerY[Index];
  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetMarkerX(int Index, double Value)
{
  if (Index == -1) if (IndexMarkerX) Index = IndexMarkerX - 1;
  if ((Index >= 0) && (Index < IndexMarkerX))
  {
    double X1 = (FMarkerX[Index] - FShiftX)/FScaleX;
    double X2 = (Value - FShiftX)/FScaleX;
    int OldPos = ConvCoor(DblPoint(X1, FArea.Top)).x;
    int NewPos = ConvCoor(DblPoint(X2, FArea.Top)).x;
    if (NewPos != OldPos)
    { PaintMarkerX(Index); FMarkerX[Index] = Value; PaintMarkerX(Index); }
    else
      FMarkerX[Index] = Value;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::SetMarkerY(int Index, double Value)
{
  if (Index == -1) if (IndexMarkerY) Index = IndexMarkerY - 1;
  if ((Index >= 0) && (Index < IndexMarkerY))
  {
    double Y1 = (FMarkerY[Index] - FShiftY)/FScaleY;
    double Y2 = (Value - FShiftY)/FScaleY;
    int OldPos = ConvCoor(DblPoint(FArea.Left, Y1)).y;
    int NewPos = ConvCoor(DblPoint(FArea.Left, Y2)).y;
    if (NewPos != OldPos)
    { PaintMarkerY(Index); FMarkerY[Index] = Value; PaintMarkerY(Index); }
    else
      FMarkerY[Index] = Value;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::ClearMarkerX()
{
  for (int i=0; i<IndexMarkerX; i++) PaintMarkerX(i);
  //FMarkerX = (double *)realloc(FMarkerX, 0);
  if (FMarkerX != NULL) {
    free(FMarkerX);
    FMarkerX = NULL;
  }
  IndexMarkerX = 0;
  if (FOnChange) FOnChange(this, cctMarker);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::ClearMarkerY()
{
  for (int i=0; i<IndexMarkerY; i++) PaintMarkerY(i);
  //FMarkerY = (double *)realloc(FMarkerY, 0);
  if (FMarkerY != NULL) {
    free(FMarkerY);
    FMarkerY = NULL;
  }
  IndexMarkerY = 0;
  if (FOnChange) FOnChange(this, cctMarker);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::DeleteMarkerX(int Index)
{
  if ((Index >= 0) && (Index < IndexMarkerX))
  {
    PaintMarkerX(Index);
    for (int i=Index; i<IndexMarkerX-1; i++) FMarkerX[i] = FMarkerX[i + 1];
    IndexMarkerX--;
    if (IndexMarkerX != 0) {
      FMarkerX = (double *)realloc(FMarkerX, IndexMarkerX*sizeof(double));
    } else {
      free(FMarkerX);
      FMarkerX = NULL;
    }
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::DeleteMarkerY(int Index)
{
  if ((Index >= 0) && (Index < IndexMarkerY))
  {
    PaintMarkerY(Index);
    for (int i=Index; i<IndexMarkerY-1; i++) FMarkerY[i] = FMarkerY[i + 1];
    IndexMarkerY--;
    if (IndexMarkerY != 0) {
      FMarkerY = (double *)realloc(FMarkerY, IndexMarkerY*sizeof(double));
    } else {
      free(FMarkerY);
      FMarkerY = NULL;
    }
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::DeleteMarkerX()
{
  if (IndexMarkerX > 0)
  {
    PaintMarkerX(IndexMarkerX - 1);
    IndexMarkerX--;
    //FMarkerX = (double *)realloc(FMarkerX, IndexMarkerX*sizeof(double));
    if (IndexMarkerX != 0) {
      FMarkerX = (double *)realloc(FMarkerX, IndexMarkerX*sizeof(double));
    } else {
      free(FMarkerX);
      FMarkerX = 0;
    }
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::DeleteMarkerY()
{
  if (IndexMarkerY > 0)
  {
    PaintMarkerY(IndexMarkerY - 1);
    IndexMarkerY--;
    //FMarkerY = (double *)realloc(FMarkerY, IndexMarkerY*sizeof(double));
    if (IndexMarkerY != 0) {
      FMarkerY = (double *)realloc(FMarkerY, IndexMarkerY*sizeof(double));
    } else {
      free(FMarkerY);
      FMarkerY = NULL;
    }
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::DeleteMarkerX(int Begin, int End)
{
  if ((Begin >= 0) && (Begin < IndexMarkerX) &&
        (End >= 0) && (End < IndexMarkerX))
  {
    for (int i=Begin; i<=End; i++) PaintMarkerX(i);
    for (int i=Begin; i<IndexMarkerX-(End-Begin+1); i++)
      FMarkerX[i] = FMarkerX[i + 1];
    IndexMarkerX-=(End - Begin + 1);
    //FMarkerX = (double *)realloc(FMarkerX, IndexMarkerX*sizeof(double));
    if (IndexMarkerX != 0) {
      FMarkerX = (double *)realloc(FMarkerX, IndexMarkerX*sizeof(double));
    } else {
      free(FMarkerX);
      FMarkerX = NULL;
    }
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChartItem::DeleteMarkerY(int Begin, int End)
{
  if ((Begin >= 0) && (Begin < IndexMarkerY) &&
        (End >= 0) && (End < IndexMarkerY))
  {
    for (int i=Begin; i<=End; i++) PaintMarkerY(i);
    for (int i=Begin; i<IndexMarkerY-(End-Begin+1); i++)
      FMarkerY[i] = FMarkerY[i + 1];
    IndexMarkerY-=(End - Begin + 1);
    //FMarkerY = (double *)realloc(FMarkerY, IndexMarkerY*sizeof(double));
    if (IndexMarkerY != 0) {
      FMarkerY = (double *)realloc(FMarkerY, IndexMarkerY*sizeof(double));
    } else {
      free(FMarkerY);
      FMarkerY = NULL;
    }
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
bool __fastcall TMxChartItem::ValidRect() const
{
  return FBoundsRect.Right - FBoundsRect.Left >= 1 &&
    FBoundsRect.Bottom - FBoundsRect.Top >= 1;
}
//---------------------------------------------------------------------------

//***************************************************************************
// TMxChart - Построение графиков

//---------------------------------------------------------------------------
// Часть неизменная для всех констукторов
void __fastcall TMxChart::Constructor()
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
//---------------------------------------------------------------------------
// Конструктор компонета
__fastcall TMxChart::TMxChart(TComponent *AOwner)
	:TComponent(AOwner)
{
  Constructor();
}
//---------------------------------------------------------------------------
// Конструктор компонета
__fastcall TMxChart::TMxChart(TComponent *AOwner, TCanvas *ACanvas)
	:TComponent(AOwner)
{
  Constructor();
  FPaintCanvas = ACanvas;
  FCurCanvas = FPaintCanvas;
}
//---------------------------------------------------------------------------
// Деструктор компонета
__fastcall TMxChart::~TMxChart()
{
  if (FItems != NULL) {
    for (int i = 0; i < FCount; i++) delete FItems[i];
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
//---------------------------------------------------------------------------
void __fastcall TMxChart::Paint()
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
  if (NeedRepaint) { NeedRepaint = false; DoRepaint(0); }
  FPaintCanvas->Draw(FBoundsRect.Left, FBoundsRect.Top, FBitmap);
  CurCanvas = FPaintCanvas;
  FCurRect = FBoundsRect;
  PaintMode = false;
  if (FOnPaint) FOnPaint(this);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SaveToMetafile(String FileName)
{
  if (!ValidRect()) return;
  PaintMode = true;
  TMetafile *emf = new TMetafile();
  int W = 32000, H = 16000;
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
  DoRepaint(1); NeedRepaint = true;
  //DoRepaint(0); NeedRepaint = true;
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
//---------------------------------------------------------------------------
void __fastcall TMxChart::BlackPrint()
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
  DoRepaint(1);
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
//---------------------------------------------------------------------------
void __fastcall TMxChart::ColorPrint()
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
  DoRepaint(2);
  CurCanvas = FPaintCanvas;
  FCurRect = FBoundsRect;
  PaintMode = false;
  Dev->Pen->Style = PS;
  if (FOnPrint) FOnPrint(this);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::GraphObjChanged(TObject *Sender)
{
  NeedRepaint = true;
  if (FCanvas)
  {
    if (Sender == FCanvas->Pen) PenChanged(Sender);
    if (Sender == FCanvas->Brush) BrushChanged(Sender);
    if (Sender == FCanvas->Font) FontChanged(Sender);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetAutoScaleX(bool Value)
{
  if (FAutoScaleX != Value)
  {
    FAutoScaleX = Value;
    NeedAutoScale = NeedCalculate = NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctAutoScale);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetAutoScaleY(bool Value)
{
  if (FAutoScaleY != Value)
  {
    FAutoScaleY = Value;
    NeedAutoScale = NeedCalculate = NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctAutoScale);
  }
}
//---------------------------------------------------------------------------
String __fastcall TMxChart::SFF(double x, double Step) const
{
  if (fabs(x) < DblError) return "0";
  int ex = ( (x == 0)?0:static_cast<int>(floor(log10(fabs(x)))) );
  int es = ( (Step == 0)?0:static_cast<int>(floor(log10(fabs(Step)))) );
  String NumIntoStr;
  if (ex >= 0 && es >= 0 && es < 15)
  {
    NumIntoStr = Printf("%.*g", ex + 1, x);
    int Len = NumIntoStr.Length();
    for (int i = 0; i < es - 1; i++) NumIntoStr[Len - i] = '0';
  }
  else
    NumIntoStr = Printf("%.*g", ex - es + 1, x);
  return NumIntoStr;
}
//---------------------------------------------------------------------------
double __fastcall TMxChart::StepCalc(double Begin, double End, int Size,
  int MinSize, bool IsWidth, bool AccountText)
{
  int Steps[] = {5, 2, 1};
  int StepCount = sizeof(Steps)/sizeof(*Steps);
  bool Break = false;
  int STW, i0, j0, iST, jST;
  int TH = FCurCanvas->TextHeight("0");
  int S = TH/4;
  double MinSW;
  double AS = End - Begin;
  int e = (AS == 0)?0:static_cast<int>(floor(log10(fabs(AS))));
  i0 = e - 1; j0 = 0;
  for (int j = 0; j < StepCount; j++)
  {
    double ASE = Steps[j]*pow(10., e);
    if ((AS - ASE)/ASE > DblError) { i0 = e; j0 = j; break; }
  }
  jST = (j0 > 0)?((iST = i0), (j0 - 1)):((iST = i0 + 1), (StepCount - 1));
  double ST = Steps[jST]*pow(10., iST);
  for (int i = i0; i > MinExp; i--)
  {
    for (int j = (i == i0)?j0:0; j < StepCount; j++)
    {
      double CurSS = Steps[j]*pow(10., i);
      if (AccountText)
      {
        if (IsWidth)
        {
          int TW1 = 0, TW2 = 0;
          STW = 0;
          double X0 = CurSS*ceil(Begin/CurSS);
          if (X0 <= End)
          {
            TW1 = CurCanvas->TextWidth(SFF(X0, CurSS))/2;
            if (TW1 > (X0 - FArea.Left)/AS*double(Size - 1))
              TW1 = static_cast<int>(2*TW1 - (X0 - FArea.Left)/
                AS*double(Size - 1));
          }
          else return Inf;
          bool SingleLine = true;
          for (double i = X0 + CurSS; i <= End; i += CurSS)
          {
            SingleLine = false;
            TW2 = (CurCanvas->TextWidth(SFF(i, CurSS)) + 1)/2;
            if (i + CurSS > End)
              if (TW2 > (End - i)/AS*double(Size - 1))
                TW2 = static_cast<int>(2*TW2 - (End - i)/AS*double(Size - 1));
            int CurW = TW1 + TW2 + S;
            STW = max(CurW, STW);
            TW1 = TW2;
          }
          if (SingleLine) if (CurCanvas->TextWidth(SFF(X0, CurSS)) > Size)
            return Inf;
        }
        else
        {
          int TH1 = TH/2, TH2 = (TH + 1)/2;
          double Y1 = CurSS*ceil(Begin/CurSS);
          if (Y1 <= End)
          {
            if (TH1 > (Y1 - Begin)/AS*double(Size - 1))
              TH1 = static_cast<int>(TH - (Y1 - Begin)/AS*double(Size - 1));
            double Y2 = CurSS*floor(End/CurSS);
            if (fabs(Y2) < CurSS/2) Y2 = 0;
            if (Y2 > Y1)
            {
              if (TH2 > (End - Y2)/AS*double(Size - 1))
                TH2 = static_cast<int>(TH - (End - Y2)/AS*double(Size - 1));
              STW = TH1 + TH2 + S;
            }
            else if (TH < Size) STW = TH; else return Inf;
          }
          else return Inf;
        }
        MinSW = max(STW, MinSize)*AS/double(Size - 1);
      }
      else
      {
        if (AS/CurSS/floor(AS/CurSS) - 1 > DblError) continue;
        MinSW = MinSize*AS/double(Size - 1);
      }
      Break = false;
      if (CurSS < MinSW) { Break = true; break; } else ST = CurSS;
    }
    if (Break) break;
  }
  return ST;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::CalcGrid()
{
  if (!FShowGrid) { FGridRect = FCurRect; return; }
  ApplyTextX = true; ApplyTextY = true;
  int PixelsX = GetDeviceCaps(FCurCanvas->Handle, LOGPIXELSX);
  int PixelsY = GetDeviceCaps(FCurCanvas->Handle, LOGPIXELSY);
  double PMinW = FMinWidthGrid*PixelsX/2.54;
  double PMinH = FMinHeightGrid*PixelsY/2.54;
  int W = FCurRect.Right - FCurRect.Left;
  int H = FCurRect.Bottom - FCurRect.Top;
  int TH = CurCanvas->TextHeight("0");
  int S = TH/4;
  int GH = H - TH - 2 - 2*S;
  if (GH < 1) { ApplyTextX = false; GH = H - 2; }

  // Вычисление SH - шага сетки по вертикали
  SH = StepCalc(FArea.Bottom, FArea.Top, GH, (int)PMinH, false, true);
  if (SH == Inf) ApplyTextY = false;
  int TWM = 0;
  int GW = W - 2;
  if (ApplyTextY)
  {
    double Y0 = SH*ceil(FArea.Bottom/SH);
    if (Y0 < FArea.Top) TWM = CurCanvas->TextWidth(SFF(Y0, SH));
    for (double i = Y0 + SH; i < FArea.Top; i += SH)
    {
      int CurW = CurCanvas->TextWidth(SFF(i, SH));
      TWM = max(TWM, CurW);
    }
    GW = W - TWM - 2 - 2*S;
    if (GW < 1) { ApplyTextY = false; GW = W - 2; }
  }

  // Вычисление SW - шага сетки по горизонтали
  if (ApplyTextX)
  {
    SW = StepCalc(FArea.Left, FArea.Right, GW, (int)PMinW, true, true);
    if (SW == Inf)
    {
      GH = H - 2; ApplyTextX = false; ApplyTextY = true;
      SH = StepCalc(FArea.Bottom, FArea.Top, GH, (int)PMinH, false, true);
      if (SH == Inf) ApplyTextY = false;
      TWM = 0;
      GW = W - 2;
      if (ApplyTextY)
      {
        double Y0 = SH*ceil(FArea.Bottom/SH);
        if (Y0 < FArea.Top) TWM = CurCanvas->TextWidth(SFF(Y0, SH));
        for (double i = Y0 + SH; i < FArea.Top; i += SH)
        {
          int CurW = CurCanvas->TextWidth(SFF(i, SH));
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
//---------------------------------------------------------------------------
void __fastcall TMxChart::PaintGrid(bool BlackAuxGrid)
{
  if (!FShowGrid) return;
  if (!ApplyTextX && !ApplyTextY) return;
  int PixelsX = GetDeviceCaps(FCurCanvas->Handle, LOGPIXELSX);
  int PixelsY = GetDeviceCaps(FCurCanvas->Handle, LOGPIXELSY);
  double PMinWA = FMinWidthAuxGrid*PixelsX/2.54;
  double PMinHA = FMinHeightAuxGrid*PixelsY/2.54;
  int TH = CurCanvas->TextHeight("0");
  int S = TH/4;
  double AW = FArea.Right - FArea.Left;
  double AH = FArea.Top - FArea.Bottom;
  double SWA = 0., SHA = 0.;
  double GW = FGridRect.Right - FGridRect.Left;
  double GH = FGridRect.Bottom - FGridRect.Top;

  RectLine(CurCanvas, Rect(FGridRect.Left - 1, FGridRect.Top - 1,
    FGridRect.Right + 1, FGridRect.Bottom + 1));

  // Отображение подписей линий сетки по Y
  if (ApplyTextY)
  {
    double Y0 = SH*ceil(FArea.Bottom/SH);
    for (double i = Y0; i < FArea.Top; i += SH)
    {
      int TPY = static_cast<int>(FGridRect.Bottom -
        (i - FArea.Bottom)/AH*double(GH - 1) - TH/2 - 1);
      if (TPY < FGridRect.Top) TPY = FGridRect.Top;
      if (TPY + TH > FGridRect.Bottom) TPY = FGridRect.Bottom - TH;
      int TPX = FGridRect.Left - CurCanvas->TextWidth(SFF(i, SH)) - 1 - S;
      CurCanvas->TextOut(TPX, TPY, SFF(i, SH));
    }
  }

  // Отображение подписей линий сетки по X
  if (ApplyTextX)
  {
    double X0 = SW*ceil(FArea.Left/SW);
    int TPY = FGridRect.Bottom + 1 + S;
    for (double i = X0; i < FArea.Right; i += SW)
    {
      int TW = CurCanvas->TextWidth(SFF(i, SW));
      int TPX = static_cast<int>((i - FArea.Left)/AW*double(GW - 1) +
        FGridRect.Left - TW/2);
      if (TPX + TW > FGridRect.Right - 1) TPX = FGridRect.Right - TW - 1;
      if (TPX < FGridRect.Left) TPX = FGridRect.Left;
      CurCanvas->TextOut(TPX, TPY, SFF(i, SW));
    }
  }

  if (FShowAuxGrid)
  {
    // Вычисление SHA - шага вспомогательной сетки по вертикали
    if (ApplyTextY)
      SHA = StepCalc(0, SH, static_cast<int>(SH/AH*double(GH - 1)),
        static_cast<int>(PMinHA), false, false);

    // Вычисление SWA - шага вспомогательной сетки по горизонтали
    if (ApplyTextX)
      SWA = StepCalc(0, SW, static_cast<int>(SW/AW*double(GW - 1)),
        static_cast<int>(PMinWA), true, false);

    FCurCanvas->Pen = FAuxGridPen;
    FCurCanvas->Pen->Mode = pmCopy;

    // Отображение вспомогательных линий сетки по Y
    if (ApplyTextY)
    {
      double Y0 = SHA*ceil(FArea.Bottom/SHA);
      for (double i = Y0; i < FArea.Top; i += SHA)
      {
        if (i - SH*floor(i/SH) < SHA/2) continue;
        TPoint P1 = ConvCoor(DblPoint(FArea.Left, i));
        TPoint P2 = ConvCoor(DblPoint(FArea.Right, i));
        if ((P1.y == FGridRect.Top) || (P1.y == FGridRect.Bottom - 1)) continue;
        if (BlackAuxGrid) DashLine(CurCanvas, P1, P2, 1, 4);
        else Line(CurCanvas, P1, P2);
      }
    }

    // Отображение вспомогательных линий сетки по X
    if (ApplyTextX)
    {
      double X0 = SWA*ceil(FArea.Left/SWA);
      for (double i = X0; i < FArea.Right; i += SWA)
      {
        if (i - SW*floor(i/SW) < SWA/2) continue;
        TPoint P1 = ConvCoor(DblPoint(i, FArea.Top));
        TPoint P2 = ConvCoor(DblPoint(i, FArea.Bottom));
        if ((P1.x == FGridRect.Left) || (P1.x == FGridRect.Right - 1)) continue;
        if (BlackAuxGrid) DashLine(CurCanvas, P1, P2, 1, 4);
        else Line(CurCanvas, P1, P2);
      }
    }

    FCurCanvas->Pen = FCanvas->Pen;
  }

  // Отображение линий сетки по Y
  if (ApplyTextY)
  {
    double Y0 = SH*ceil(FArea.Bottom/SH);
    for (double i = Y0; i < FArea.Top; i += SH)
    {
      TPoint P1 = ConvCoor(DblPoint(FArea.Left, i));
      TPoint P2 = ConvCoor(DblPoint(FArea.Right, i));
      if ((P1.y == FGridRect.Top) || (P1.y == FGridRect.Bottom - 1)) continue;
      Line(CurCanvas, P1, P2);
    }
  }

  // Отображение линий сетки по X
  if (ApplyTextX)
  {
    double X0 = SW*ceil(FArea.Left/SW);
    for (double i = X0; i < FArea.Right; i += SW)
    {
      TPoint P1 = ConvCoor(DblPoint(i, FArea.Top));
      TPoint P2 = ConvCoor(DblPoint(i, FArea.Bottom));
      if ((P1.x == FGridRect.Left) || (P1.x == FGridRect.Right - 1)) continue;
      Line(CurCanvas, P1, P2);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::PaintGrid()
{
  PaintGrid(false);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::PaintBounds() const
{
  if (!FShowBounds) return;
  int TH = FCurCanvas->TextHeight("0");
  int S = TH/4;
  int PX, PY;
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
//---------------------------------------------------------------------------
void __fastcall TMxChart::PreCalc()
{
  if (!ValidRect()) return;
  if (NeedAutoScale) { NeedAutoScale = false; DoAutoScale(); }
  if (NeedCalculate) { NeedCalculate = false; DoCalculate(); }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::Invalidate()
{
  NeedAutoScale = NeedCalculate = NeedRepaint = true;
  for (int i = 0; i < FCount; i++) {
    FItems[i]->NeedAutoScale = true;
    FItems[i]->NeedCalculate = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetShowBounds(bool Value)
{
  if (FShowBounds != Value) {
    FShowBounds = Value;
    NeedCalculate = NeedRepaint = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetShowGrid(bool Value)
{
  if (FShowGrid != Value) {
    FShowGrid = Value;
    NeedCalculate = NeedRepaint = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetShowAuxGrid(bool Value)
{
  if (FShowAuxGrid != Value) {
    FShowAuxGrid = Value;
    NeedCalculate = NeedRepaint = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::DoAutoScale()
{
  for (int i = 0; i < FCount; i++)
    if (FItems[i]->NeedAutoScale && !FItems[i]->Hide)
    { FItems[i]->NeedAutoScale = false; FItems[i]->DoAutoScale(); }
  if (FAutoScaleX)
  {
    FArea.Left = FArea.Right = 0;
    if (FCount && FItems[FBaseItem]->Hide)
    for (int i = 0; i < FCount; i++)
      if (!FItems[i]->Hide) { FBaseItem = i; break; }
    if (FCount && !FItems[FBaseItem]->Hide)
    {
      FArea.Left = FItems[FBaseItem]->Area.Left;
      FArea.Right = FItems[FBaseItem]->Area.Right;
      for (int i = 0; i < FCount; i++)
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
    for (int i = 0; i < FCount; i++)
      if (!FItems[i]->Hide) { FBaseItem = i; break; }
    if (FCount && !FItems[FBaseItem]->Hide)
    {
      FArea.Bottom = FItems[FBaseItem]->Area.Bottom;
      FArea.Top = FItems[FBaseItem]->Area.Top;
      for (int i = 0; i < FCount; i++)
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
//---------------------------------------------------------------------------
void __fastcall TMxChart::DoCalculate()
{
  CalcGrid();
  SetChildRect();
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::DoRepaint(int Tag)
{
  FCurCanvas->FillRect(FCurRect);
  if (Tag == 1) PaintGrid(true); else PaintGrid();
  for (int i = 0; i < FCount; i++)
    if (FItems[i]->NeedCalculate && !FItems[i]->Hide)
    { FItems[i]->NeedCalculate = false; FItems[i]->DoCalculate(); }
  SetClipRect(FCurCanvas, FGridRect);
  switch (Tag)
  {
    case 0:
      for (int i = 0; i < FCount; i++) if (!FItems[i]->Hide) FItems[i]->DoPaint();
      break;
    case 1:
      for (int i = 0; i < FCount; i++) if (!FItems[i]->Hide) FItems[i]->DoBlackPrint();
      break;
    case 2:
      for (int i = 0; i < FCount; i++) if (!FItems[i]->Hide) FItems[i]->DoColorPrint();
      break;
  }
  for (int i = 0; i < IndexMarkerX; i++) PaintMarkerX(i);
  for (int i = 0; i < IndexMarkerY; i++) PaintMarkerY(i);
  PaintBounds();
  DeleteClipRect(FCurCanvas);
}
//---------------------------------------------------------------------------
TPoint __fastcall TMxChart::ConvCoor(TDblPoint P) const
{
  double k = 0.; int x = 0, y = 0;
  int Width = FGridRect.Right - FGridRect.Left;
  int Height = FGridRect.Bottom - FGridRect.Top;
  double wx = FArea.Right - FArea.Left;
  if (0. == wx) wx = 1e-12;
  try {
    k = (P.x - FArea.Left)/wx;
  } catch (...) {
    k = 0.;
  }
  x = static_cast<int>(FGridRect.Left + (Width - 1)*k);
  double wy = FArea.Top - FArea.Bottom;
  if (0. == wy) wy = 1e-12;
  try {
    k = (P.y - FArea.Bottom)/wy;
  } catch (...) {
    k = 0.;
  }
  y = static_cast<int>(FGridRect.Bottom - (Height - 1)*k - 1);
  return Point(x, y);
}
//---------------------------------------------------------------------------
int __fastcall TMxChart::ConvCoorX(double b, double e, double x) const
{
  int Width = FGridRect.Right - FGridRect.Left;
  double w = e - b;
  if (0. == w) w = 1e-12;
  double k;
  try {
    k = (x - b)/w;
  } catch(...) {
    k = 0.;
  }
  return static_cast<int>(FGridRect.Left + (Width - 1)*k);
}
//---------------------------------------------------------------------------
int __fastcall TMxChart::ConvCoorY(double b, double e, double y) const
{
  int Height = FGridRect.Bottom - FGridRect.Top;
  double w = e - b;
  if (0. == w) w = 1e-12;
  double k;
  try {
    k = (y - b)/w;
  } catch(...) {
    k = 0.;
  }
  return static_cast<int>(FGridRect.Bottom - (Height - 1)*k - 1);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetChildRect()
{
  if (!FCount || FItems[BaseItem]->Hide) return;
  typedef vector<int> veci;
  typedef map<int, veci> mapg;
  typedef mapg::iterator itr;
  mapg GroupsX, GroupsY;
  vector<int> vec;
  for (int i = 0; i < FCount; i++) if (!FItems[i]->Hide)
  {
    GroupsX[FItems[i]->GroupX].push_back(i);
    GroupsY[FItems[i]->GroupY].push_back(i);
  }

  PaintMode = true;

  // -------- Base X
  vec = GroupsX[FItems[BaseItem]->GroupX];
  for (size_t i = 0; i < vec.size(); i++)
  {
    TDblRect A = FItems[vec[i]]->Area;
    A.Left = Area.Left; A.Right = Area.Right;
    FItems[vec[i]]->Area = A;
  }
  for (size_t i = 0; i < vec.size(); i++)
  {
    TRect R = FItems[vec[i]]->BoundsRect;
    R.Left = ConvCoorX(Area.Left, Area.Right, FItems[vec[i]]->Area.Left);
    R.Right = ConvCoorX(Area.Left, Area.Right, FItems[vec[i]]->Area.Right) + 1;
    FItems[vec[i]]->BoundsRect = R;
  }

  // -------- Base Y
  vec = GroupsY[FItems[BaseItem]->GroupY];
  for (size_t i = 0; i < vec.size(); i++)
  {
    TDblRect A = FItems[vec[i]]->Area;
    A.Bottom = Area.Bottom; A.Top = Area.Top;
    FItems[vec[i]]->Area = A;
  }
  for (size_t i = 0; i < vec.size(); i++)
  {
    TRect R = FItems[vec[i]]->BoundsRect;
    R.Top = ConvCoorY(Area.Bottom, Area.Top, FItems[vec[i]]->Area.Top);
    R.Bottom = ConvCoorY(Area.Bottom, Area.Top, FItems[vec[i]]->Area.Bottom) + 1;
    FItems[vec[i]]->BoundsRect = R;
  }

  // -------- X
  for (itr i = GroupsX.begin(); i != GroupsX.end(); i++)
  {
    if (i->first == FItems[FBaseItem]->GroupX) continue;
    vec = i->second;
    double L = 1, R = -1;
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
      TRect Rt = FItems[vec[j]]->BoundsRect;
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
    double B = 1, T = -1;
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
      TRect R = FItems[vec[j]]->BoundsRect;
      R.Top = ConvCoorY(B, T, FItems[vec[j]]->Area.Top);
      R.Bottom = ConvCoorY(B, T, FItems[vec[j]]->Area.Bottom) + 1;
      FItems[vec[j]]->BoundsRect = R;
    }
  }

  PaintMode = false;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetBoundsRect(TRect Value)
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
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetPrintRect(TRect Value)
{
  if (Value.Left < Value.Right && Value.Top < Value.Bottom)
  {
    FPrintRect = Value;
    if (FOnChange) FOnChange(this, cctPrintRect);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetPaintCanvas(TCanvas *Value)
{
  if (FPaintCanvas != Value)
  {
    FPaintCanvas = Value;
    CurCanvas = FPaintCanvas;
    if (FOnChange) FOnChange(this, cctCanvas);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetArea(TDblRect AArea)
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
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetCurCanvas(TCanvas *Value)
{
  FCurCanvas = Value;
  for (int i = 0; i < FCount; i++) FItems[i]->Canvas = Value;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetMarkerPen(TPen *Value)
{
  FMarkerPen->Assign(Value);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetAuxGridPen(TPen *Value)
{
  FAuxGridPen->Assign(Value);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetMinWidthGrid(double Value)
{
  FMinWidthGrid = Value;
  NeedCalculate = NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctSizeGrid);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetMinHeightGrid(double Value)
{
  FMinHeightGrid = Value;
  NeedCalculate = NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctSizeGrid);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetMinWidthAuxGrid(double Value)
{
  FMinWidthAuxGrid = Value;
  NeedCalculate = NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctSizeAuxGrid);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetMinHeightAuxGrid(double Value)
{
  FMinHeightAuxGrid = Value;
  NeedCalculate = NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctSizeAuxGrid);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetBaseItem(int AChart)
{
  if (AChart == FBaseItem) return;
  //PaintMode = true;
  FBaseItem = AChart;
  //PaintMode = false;
  NeedAutoScale = NeedCalculate = NeedRepaint = true;
  if (Select) Select->ItemChange(5, FBaseItem);
  if (FOnChange) FOnChange(this, cctBaseItem);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetLeft(int Value)
{
  TRect R = BoundsRect;
  R.Left = Value;
  BoundsRect = R;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetTop(int Value)
{
  TRect R = BoundsRect;
  R.Top = Value;
  BoundsRect = R;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetWidth(int Value)
{
  TRect R = BoundsRect;
  R.Right = R.Left + Value;
  BoundsRect = R;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetHeight(int Value)
{
  TRect R = BoundsRect;
  R.Bottom = R.Top + Value;
  BoundsRect = R;
}
//---------------------------------------------------------------------------
TDblRect __fastcall TMxChart::GetArea()
{
  PreCalc();
  return FArea;
}
//---------------------------------------------------------------------------
TRect __fastcall TMxChart::GetGridRect()
{
  PreCalc();
  return FGridRect;
}
//---------------------------------------------------------------------------
int __fastcall TMxChart::GetLeft() const
{
  return FBoundsRect.Left;
}
//---------------------------------------------------------------------------
int __fastcall TMxChart::GetTop() const
{
  return FBoundsRect.Top;
}
//---------------------------------------------------------------------------
int __fastcall TMxChart::GetWidth() const
{
  return FBoundsRect.Right - FBoundsRect.Left;
}
//---------------------------------------------------------------------------
int __fastcall TMxChart::GetHeight() const
{
  return FBoundsRect.Bottom - FBoundsRect.Top;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::AddMarkerX(double Value)
{
  if (FMarkerX != NULL) {
    FMarkerX = (double *)realloc(FMarkerX, (IndexMarkerX + 1)*sizeof(double));
  } else {
    FMarkerX = (double *)calloc(1, sizeof(double));
  }
  FMarkerX[IndexMarkerX] = Value;
  PaintMarkerX(IndexMarkerX);
  IndexMarkerX++;
  if (FOnChange) FOnChange(this, cctMarker);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::AddMarkerY(double Value)
{
  if (FMarkerY != NULL) {
    FMarkerY = (double *)realloc(FMarkerY, (IndexMarkerY + 1)*sizeof(double));
  } else {
    FMarkerY = (double *)calloc(1, sizeof(double));
  }
  //FMarkerY = (double *)realloc(FMarkerY, (IndexMarkerY + 1)*sizeof(double));
  FMarkerY[IndexMarkerY] = Value;
  PaintMarkerY(IndexMarkerY);
  IndexMarkerY++;
  if (FOnChange) FOnChange(this, cctMarker);
}
//---------------------------------------------------------------------------
double __fastcall TMxChart::GetMarkerX(int Index) const
{
  if (Index == -1) if (IndexMarkerX) return FMarkerX[IndexMarkerX - 1];
  if ((Index >= 0) && (Index < IndexMarkerX)) return FMarkerX[Index];
  return 0;
}
//---------------------------------------------------------------------------
double __fastcall TMxChart::GetMarkerY(int Index) const
{
  if (Index == -1) if (IndexMarkerY) return FMarkerY[IndexMarkerY - 1];
  if ((Index >= 0) && (Index < IndexMarkerY)) return FMarkerY[Index];
  return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetMarkerX(int Index, double Value)
{
  if (Index == -1) if (IndexMarkerX) Index = IndexMarkerX - 1;
  if ((Index >= 0) && (Index < IndexMarkerX))
  {
    int OldPos = ConvCoor(DblPoint(FMarkerX[Index], FArea.Top)).x;
    int NewPos = ConvCoor(DblPoint(Value, FArea.Top)).x;
    if (NewPos != OldPos)
    { PaintMarkerX(Index); FMarkerX[Index] = Value; PaintMarkerX(Index); }
    else
      FMarkerX[Index] = Value;
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::SetMarkerY(int Index, double Value)
{
  if (Index == -1) if (IndexMarkerY) Index = IndexMarkerY - 1;
  if ((Index >= 0) && (Index < IndexMarkerY))
  {
    int OldPos = ConvCoor(DblPoint(FArea.Left, FMarkerY[Index])).y;
    int NewPos = ConvCoor(DblPoint(FArea.Left, Value)).y;
    if (NewPos != OldPos)
    { PaintMarkerY(Index); FMarkerY[Index] = Value; PaintMarkerY(Index); }
    else
      FMarkerY[Index] = Value;
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::ClearMarkerX()
{
  for (int i=0; i<IndexMarkerX; i++) PaintMarkerX(i);
  //FMarkerX = (double *)realloc(FMarkerX, 0);
  if (FMarkerX != NULL) {
    free(FMarkerX);
    FMarkerX = NULL;
  }
  IndexMarkerX = 0;
  NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctMarker);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::ClearMarkerY()
{
  for (int i=0; i<IndexMarkerY; i++) PaintMarkerY(i);
  //FMarkerY = (double *)realloc(FMarkerY, 0);
  if (FMarkerY != NULL) {
    free(FMarkerY);
    FMarkerY = NULL;
  }
  IndexMarkerY = 0;
  NeedRepaint = true;
  if (FOnChange) FOnChange(this, cctMarker);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::DeleteMarkerX(int Index)
{
  if ((Index >= 0) && (Index < IndexMarkerX))
  {
    PaintMarkerX(Index);
    for (int i=Index; i<IndexMarkerX-1; i++) FMarkerX[i] = FMarkerX[i + 1];
    IndexMarkerX--;
    if (IndexMarkerX != 0) {
      FMarkerX = (double *)realloc(FMarkerX, IndexMarkerX*sizeof(double));
    } else {
      free(FMarkerX);
      FMarkerX = NULL;
    }
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::DeleteMarkerY(int Index)
{
  if ((Index >= 0) && (Index < IndexMarkerY))
  {
    PaintMarkerY(Index);
    for (int i=Index; i<IndexMarkerY-1; i++) FMarkerY[i] = FMarkerY[i + 1];
    IndexMarkerY--;
    //FMarkerY = (double *)realloc(FMarkerY, IndexMarkerY*sizeof(double));
    if (IndexMarkerY != 0) {
      FMarkerY = (double *)realloc(FMarkerY, IndexMarkerY*sizeof(double));
    } else {
      free(FMarkerY);
      FMarkerY = NULL;
    }
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::DeleteMarkerX()
{
  if (IndexMarkerX > 0)
  {
    IndexMarkerX--;
    PaintMarkerX(IndexMarkerX);
    //FMarkerX = (double *)realloc(FMarkerX, IndexMarkerX*sizeof(double));
    if (IndexMarkerX != 0) {
      FMarkerX = (double *)realloc(FMarkerX, IndexMarkerX*sizeof(double));
    } else {
      free(FMarkerX);
      FMarkerX = NULL;
    }
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::DeleteMarkerY()
{
  if (IndexMarkerY > 0)
  {
    IndexMarkerY--;
    PaintMarkerY(IndexMarkerY);
    //FMarkerY = (double *)realloc(FMarkerY, IndexMarkerY*sizeof(double));
    if (IndexMarkerY != 0) {
      FMarkerY = (double *)realloc(FMarkerY, IndexMarkerY*sizeof(double));
    } else {
      free(FMarkerY);
      FMarkerY = NULL;
    }
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::DeleteMarkerX(int Begin, int End)
{
  if ((Begin >= 0) && (Begin < IndexMarkerX) &&
        (End >= 0) && (End < IndexMarkerX))
  {
    for (int i=Begin; i<=End; i++) PaintMarkerX(i);
    for (int i=Begin; i<IndexMarkerX-(End-Begin+1); i++)
      FMarkerX[i] = FMarkerX[i + 1];
    IndexMarkerX-=(End - Begin + 1);
    //FMarkerX = (double *)realloc(FMarkerX, IndexMarkerX*sizeof(double));
    if (IndexMarkerX != 0) {
      FMarkerX = (double *)realloc(FMarkerX, IndexMarkerX*sizeof(double));
    } else {
      free(FMarkerX);
      FMarkerX = NULL;
    }
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::DeleteMarkerY(int Begin, int End)
{
  if ((Begin >= 0) && (Begin < IndexMarkerY) &&
        (End >= 0) && (End < IndexMarkerY))
  {
    for (int i=Begin; i<=End; i++) PaintMarkerY(i);
    for (int i=Begin; i<IndexMarkerY-(End-Begin+1); i++)
      FMarkerY[i] = FMarkerY[i + 1];
    IndexMarkerY-=(End - Begin + 1);
    //FMarkerY = (double *)realloc(FMarkerY, IndexMarkerY*sizeof(double));
    if (IndexMarkerY != 0) {
      FMarkerY = (double *)realloc(FMarkerY, IndexMarkerY*sizeof(double));
    } else {
      free(FMarkerY);
      FMarkerY = NULL;
    }
    NeedRepaint = true;
    if (FOnChange) FOnChange(this, cctMarker);
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::PaintMarkerX(int i)
{
  if (MarkerX[i] < FArea.Left && MarkerX[i] > FArea.Right)
    return;
  PreCalc();
  TCanvas *Dev = CurCanvas;
  Dev->Pen = FMarkerPen;
  Dev->Pen->Style = psSolid;
  Dev->Pen->Mode = pmNotXor;
  TPoint P1 = ConvCoor(DblPoint(FMarkerX[i], FArea.Top));
  TPoint P2 = ConvCoor(DblPoint(FMarkerX[i], FArea.Bottom));
  DashLine(Dev, P1, P2, 5, 5);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::PaintMarkerY(int i)
{
  if (MarkerY[i] < FArea.Bottom && MarkerY[i] > FArea.Top)
    return;
  PreCalc();
  TCanvas *Dev = CurCanvas;
  Dev->Pen = FMarkerPen;
  Dev->Pen->Style = psSolid;
  Dev->Pen->Mode = pmNotXor;
  TPoint P1 = ConvCoor(DblPoint(FArea.Left, FMarkerY[i]));
  TPoint P2 = ConvCoor(DblPoint(FArea.Right, FMarkerY[i]));
  DashLine(Dev, P1, P2, 5, 5);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::PaintChildMarkerX(TMxChartItem *Sender, int i) const
{
  TCanvas *Dev = CurCanvas;
  Dev->Pen = Sender->MarkerPen;
  Dev->Pen->Style = psSolid;
  Dev->Pen->Mode = pmNotXor;
  TPoint P1 = Point(i, FGridRect.Top);
  TPoint P2 = Point(i, FGridRect.Bottom - 1);
  DashLine(Dev, P1, P2, 5, 5);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::PaintChildMarkerY(TMxChartItem *Sender, int i) const
{
  TCanvas *Dev = CurCanvas;
  Dev->Pen = Sender->MarkerPen;
  Dev->Pen->Style = psSolid;
  Dev->Pen->Mode = pmNotXor;
  TPoint P1 = Point(FGridRect.Left, i);
  TPoint P2 = Point(FGridRect.Right - 1, i);
  DashLine(Dev, P1, P2, 5, 5);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::Delete(int Index)
{
  if ((Index < 0) && (Index >= FCount)) return;
  //** Для TMxChartSelect **
  if (Select) Select->ItemChange(0, Index);
  //************************
  delete FItems[Index];
  FItems[Index] = NULL;
  for (int i = Index; i < FCount - 1; i++) FItems[i] = FItems[i + 1];
  FCount--;
  //FItems = (TMxChartItem **)realloc(FItems, FCount*sizeof(TMxChartItem*));
  if (FCount != 0) {
    FItems = (TMxChartItem **)realloc(FItems, FCount*sizeof(TMxChartItem*));
  } else {
    free(FItems);
    FItems = NULL;
  }
  NeedAutoScale = NeedCalculate = NeedRepaint = true;
  if (Index >= BaseItem && Index != 1) BaseItem--;
  if (FOnChange) FOnChange(this, cctItems);
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::Clear()
{
  for (int i = 0; i < FCount; i++) delete FItems[i];
  FCount = 0;
  //FItems = (TMxChartItem **)realloc(FItems, 0);
  if (FItems != NULL) {
    free(FItems);
    FItems = NULL;
  }
  NeedAutoScale = NeedCalculate = NeedRepaint = true;
  FBaseItem = CurGroupX = CurGroupY = 0;
  if (FOnChange) FOnChange(this, cctItems);
  //** Для TMxChartSelect **
  if (Select) Select->ItemChange(1, 0);
  //************************
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::Add()
{
  //FItems = (TMxChartItem **)realloc(FItems, (FCount + 1)*sizeof(TMxChartItem));
  if (FItems != NULL) {
    FItems =
      (TMxChartItem **)realloc(FItems, (FCount + 1)*sizeof(TMxChartItem*));
  } else {
    FItems = (TMxChartItem **)calloc(1, sizeof(TMxChartItem*));
  }
  FItems[FCount] = new TMxChartItem(this);
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
  //** Для TMxChartSelect **
  if (Select) Select->ItemChange(2, FCount - 1);
  //************************
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::Add(TPointer Data, int Count)
{
  Add();
  Items[FCount - 1]->DataY = Data;
  Items[FCount - 1]->Bounds.Begin = 0;
  Items[FCount - 1]->Bounds.End = Count - 1;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::Add(TPointer Data, int Count, TColor AColor)
{
  Add(Data, Count);
  Items[FCount - 1]->Pen->Color = AColor;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::Insert(int Index)
{
  if ((Index < 0) && (Index > FCount)) return;
  //FItems = (TMxChartItem **)realloc(FItems, (FCount + 1)*sizeof(TMxChartItem));
  if (FItems != NULL) {
    FItems =
      (TMxChartItem **)realloc(FItems, (FCount + 1)*sizeof(TMxChartItem*));
  } else {
    FItems = (TMxChartItem **)calloc(1, sizeof(TMxChartItem*));
  }
  for (int i = FCount; i > Index; i--) FItems[i] = FItems[i - 1];
  FCount++;
  FItems[Index] = new TMxChartItem(this);
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
  //** Для TMxChartSelect **
  if (Select) Select->ItemChange(2, Index);
  //************************
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::Insert(int Index, TPointer Data, int Count)
{
  Insert(Index);
  Items[Index]->DataY = Data;
  Items[Index]->Bounds.End = Count - 1;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::Insert(int Index, TPointer Data, int Count,
  TColor AColor)
{
  Insert(Index, Data, Count);
  Items[FCount - 1]->Pen->Color = AColor;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::NewGroupX()
{
  CurGroupX++;
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::NewGroupY()
{
  CurGroupY++;
}
//---------------------------------------------------------------------------
TMxChartItem *__fastcall TMxChart::GetItem(int Index) const
{
  if (Index == -1) return FItems[FCount - 1];
  if (Index < 0) Index = 0; if (Index >= FCount) Index = FCount - 1;
  return FItems[Index];
}
//---------------------------------------------------------------------------
void __fastcall TMxChart::ChildChange(TObject *Sender,
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
//---------------------------------------------------------------------------
void __fastcall TMxChart::ChildError(TObject *Sender, Exception &e,
  TDblPoint P, double t, TChartErrorType ChartErrorType) const
{
  if (FOnError) FOnError(Sender, e, P, t, ChartErrorType);
}
//---------------------------------------------------------------------------
bool __fastcall TMxChart::ValidRect() const
{
  return (FCurRect.Right - FCurRect.Left > 1) &&
    (FCurRect.Bottom - FCurRect.Top > 1);
}
//---------------------------------------------------------------------------

//***************************************************************************
// TMxChartSelect - Компонент на TMxChart

//---------------------------------------------------------------------------
__fastcall TMxChartSelect::TMxChartSelect(TMxChart *AChart)
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
  //TDblRect CurArea;
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
//---------------------------------------------------------------------------
__fastcall TMxChartSelect::TMxChartSelect(TMxChart *AChart, TControl *AControl)
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
  //TDblRect CurArea;
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
//---------------------------------------------------------------------------
__fastcall TMxChartSelect::~TMxChartSelect()
{
  Disconnect();
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::SetControl(TControl *AControl)
{
  FControl = AControl;
  Connect();
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::Connect()
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
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::Disconnect()
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
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::SelectClip(TRect &ARect) const
{
  int nLeft = max(min(ARect.Left, ARect.Right), Chart->GridRect.Left);
  int nRight = min(max(ARect.Left, ARect.Right), Chart->GridRect.Right);
  int nTop = max(min(ARect.Top, ARect.Bottom), Chart->GridRect.Top);
  int nBottom = min(max(ARect.Top, ARect.Bottom), Chart->GridRect.Bottom);
  ARect = Rect(nLeft, nTop, nRight, nBottom);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::SelectRect(TRect Rect) const
{
  TPoint Points[] = {Point(Rect.Left, Rect.Top), Point(Rect.Right - 1, Rect.Top),
    Point(Rect.Right - 1, Rect.Bottom - 1), Point(Rect.Left, Rect.Bottom - 1),
    Point(Rect.Left, Rect.Top)};
  Chart->PaintCanvas->Pen->Mode = pmXor;
  Chart->PaintCanvas->Pen->Style = psDot;
  Chart->PaintCanvas->Pen->Color = clBlack;
  Chart->PaintCanvas->Polyline(Points, 4);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::DoMouseDown(TObject *Sender, TMouseButton Button,
  TShiftState Shift, int X, int Y)
{
  if (PositioningMode)
  {
    double AL = Chart->Area.Left, AB = Chart->Area.Bottom;
    double AW = Chart->Area.Right - AL, AH = Chart->Area.Top - AB;
    double GW = Chart->GridRect.Right - Chart->GridRect.Left - 1;
    double GH = Chart->GridRect.Bottom - Chart->GridRect.Top - 1;
    double DX = AL + (X - Chart->GridRect.Left)*AW/GW;
    double DY = AB + (Chart->GridRect.Bottom - Y)*AH/GH;
    Positioning(DX, DY);
    return;
  }
  if (DragChartMode)
  {
    double AL = Chart->Area.Left, AB = Chart->Area.Bottom;
    double AW = Chart->Area.Right - AL, AH = Chart->Area.Top - AB;
    double GW = Chart->GridRect.Right - Chart->GridRect.Left - 1;
    double GH = Chart->GridRect.Bottom - Chart->GridRect.Top - 1;
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
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::DoMouseMove(TObject *Sender, TShiftState Shift,
  int X, int Y)
{
  if (PositioningMode) return;
  if (DragChartMode && SelectMode)
  {
    double AL = Chart->Area.Left, AB = Chart->Area.Bottom;
    double AW = Chart->Area.Right - AL, AH = Chart->Area.Top - AB;
    double GW = Chart->GridRect.Right - Chart->GridRect.Left;
    double GH = Chart->GridRect.Bottom - Chart->GridRect.Top;
    double CX = Chart->GridRect.Left + GW/2 - X;
    double CY = Y - (Chart->GridRect.Top + GH/2);
    Positioning(PX + CX*AW/GW, PY + CY*AH/GH);
    return;
  }
  if (FirstSelect && abs(Xn - X) < 5 && abs(Yn - Y) < 5) return;
  if (SelectMode)
  {
    TRect VRect;
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
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::DoMouseUp(TObject *Sender, TMouseButton Button,
  TShiftState Shift, int X, int Y)
{
  if (PositioningMode) return;
  if (DragChartMode) { SelectMode = false; return; }
  if (FirstSelect) { SelectMode = false; FirstSelect = false; return; }
  if (SelectMode)
  {
    SelectMode = false;
    SelectRect(SelRect);
    double AL = Chart->Area.Left, AB = Chart->Area.Bottom;
    double AW = Chart->Area.Right - AL, AH = Chart->Area.Top - AB;
    double GW = Chart->GridRect.Right - Chart->GridRect.Left;
    double GH = Chart->GridRect.Bottom - Chart->GridRect.Top;
    TDblRect Area;
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
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::DoDblClick(TObject *Sender)
{
  if (PositioningMode) return;
  if (DragChartMode) return;
  ZoomOut();
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::All()
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
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::SaveAreas()
{
  ValidAreas = true;
  for (int i = 0; i < Chart->Count; i++)
    MAreas[(int *)Chart->Items[i]->DataY] = Chart->Items[i]->Area;
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::RestoreAreas()
{
  if (!ValidAreas) return;
  ValidAreas = false;
  maparea::iterator it;
  for (int i = 0; i < Chart->Count; i++)
  {
    it = MAreas.find(Chart->Items[i]->DataY);
    if (it != MAreas.end()) Chart->Items[i]->Area = it->second;
  }
  MAreas.clear();
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::SaveAutoScales()
{
  ValidAutoScales = true;
  AutoScales.insert(make_pair(Chart,
    make_pair(Chart->AutoScaleX, Chart->AutoScaleY)));
  for (int i = 0; i < Chart->Count; i++)
    AutoScales.insert(make_pair(Chart->Items[i],
      make_pair(Chart->Items[i]->AutoScaleX, Chart->Items[i]->AutoScaleY)));
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::RestoreAutoScales()
{
  if (!ValidAutoScales) return;
  ValidAutoScales = false;
  mapscale::iterator it;
  for (int i = 0; i < Chart->Count; i++)
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
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::DoPaint()
{
  try { Chart->Paint(); }
  catch (...) { if (FOnError) FOnError(this); }
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::Positioning(double X, double Y)
{
  if (!Areas.size()) return;
  TDblRect VArea = Chart->Area;
  double CX = VArea.Left + (VArea.Right - VArea.Left)/2;
  double CY = VArea.Bottom + (VArea.Top - VArea.Bottom)/2;
  Shift(X - CX, Y - CY);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::Shift(double X, double Y)
{
  if (!Areas.size()) return;
  if (X == 0 && Y == 0) return;
  TDblRect VArea = Chart->Area;
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
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::RestoreCompConvs()
{
  mapconv Convs = CompConvs.back();
  for (int i = 0; i < Chart->Count; i++)
  {
    Chart->Items[i]->CompConvX = Convs[Chart->Items[i]].first;
    Chart->Items[i]->CompConvY = Convs[Chart->Items[i]].second;
  }
  CompConvs.pop_back();
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::SaveCompConvs()
{
  mapconv Convs;
  for (int i = 0; i < Chart->Count; i++)
    Convs[Chart->Items[i]] =
      make_pair(Chart->Items[i]->CompConvX, Chart->Items[i]->CompConvY);
  CompConvs.push_back(Convs);
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::ZoomIn(double FX, double FY)
{
  if (FX < 0) FX = FZoomFactor; FY = fabs(FY);
  TDblRect VArea;
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
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::ZoomOut()
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
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::SetZoomFactor(double Value)
{
  if (Value < 1) FZoomFactor = Value; else FZoomFactor = 1/Value;
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::ItemChange(int Oper, int Index)
{
  if (LockEvent) return;
  vector<int>::iterator it;
  mapscale::iterator itm;
  TMxChartItem *BaseItem;
  switch (Oper)
  {
    case 0:
      AutoScales.erase(Chart->Items[Index]);
      it = BaseItems.begin();
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
      break;
    case 1:
      itm = AutoScales.find(Chart);
      if (itm != AutoScales.end())
      {
        Chart->AutoScaleY = itm->second.first;
        Chart->AutoScaleX = itm->second.second;
        //CurArea = DblRect(0, 0, 0, 0);
      }
      AutoScales.clear(); //MAreas.clear();
      Areas.clear(); BaseItems.clear();
      break;
    case 2:
      AutoScales.insert(make_pair(Chart->Items[Index], make_pair(
        Chart->Items[Index]->AutoScaleX, Chart->Items[Index]->AutoScaleY)));
      for(vector<int>::iterator i = BaseItems.begin(); i != BaseItems.end(); i++)
        if (*i >= Index) (*i)++;
      break;
    case 3:
    case 4:
      BaseItem = Chart->Items[Chart->BaseItem];
      itm = AutoScales.find(BaseItem);
      if (itm != AutoScales.end())
      {
        if (Oper == 3) BaseItem->AutoScaleY = itm->second.first;
        if (Oper == 4) BaseItem->AutoScaleX = itm->second.second;
      }
      itm = AutoScales.find(Chart);
      if (itm != AutoScales.end())
      {
        if (Oper == 3) Chart->AutoScaleY = itm->second.first;
        if (Oper == 4) Chart->AutoScaleX = itm->second.second;
      }
      break;
    case 5:
      itm = AutoScales.find(Chart);
      if (itm != AutoScales.end())
      {
        Chart->AutoScaleY = itm->second.first;
        Chart->AutoScaleX = itm->second.second;
      }
      break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::SetPositioningMode(bool Value)
{
  FPositioningMode = Value;
  if (Value) FDragChartMode = false;
}
//---------------------------------------------------------------------------
void __fastcall TMxChartSelect::SetDragChartMode(bool Value)
{
  FDragChartMode = Value;
  if (Value) FPositioningMode = false;
}
//---------------------------------------------------------------------------

} // namespace Mxchart







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

  m_names.push_back("Красный");
  m_names.push_back("Синий");
  m_names.push_back("Зеленый");
  m_names.push_back("Сиреневый");
  m_names.push_back("Коричневый");
  m_names.push_back("Розовый");
  m_names.push_back("Оливковый");
  m_names.push_back("Голубой");
  m_names.push_back("Светло-зеленый");
  m_names.push_back("Темно-синий");
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
add_param(const irs::string &a_name)
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
delete_param(const irs::string &a_name)
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
set_value(const irs::string &a_name, double a_value)
{
  data_t::iterator it = m_data.find(a_name);
  if (it != m_data.end()) {
    (*it).second.vec.back() = a_value;
  } else {
    // Ничего если параметра нет
  }
}
void irs::chart::builder_chart_window_t::
set_value(const irs::string &a_name, double a_time, double a_value)
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
add(const irs::string &a_name, double a_time, double a_value)
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
  unsort_data_it(),
  bad_point()
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
  unsort_data_it = m_unsort_data.begin();
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
  unsort_data_it = m_unsort_data.begin();
}
void irs::chart::builder_chart_window_t::unsort_data_t::
next()
{
  if (unsort_data_it == m_unsort_data.end()) {
    unsort_data_it = m_unsort_data.begin();
  } else {
    unsort_data_it++;
    if (unsort_data_it == m_unsort_data.end()) {
      unsort_data_it = m_unsort_data.begin();
    }
  }
}
irs::string irs::chart::builder_chart_window_t::unsort_data_t::
name()
{
  if (unsort_data_it == m_unsort_data.end()) {
    return "";
  }
  return unsort_data_it->second;
}
const irs::chart::builder_chart_window_t::chart_point_t&
irs::chart::builder_chart_window_t::unsort_data_t::
vec()
{
  if (mp_data == IRS_NULL) return bad_point;
  if (unsort_data_it == m_unsort_data.end()) return bad_point;
  data_t::const_iterator data_it = mp_data->find(unsort_data_it->second);
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
  unsort_data_it = m_unsort_data.begin();
  advance(unsort_data_it, a_index);
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
  mp_chart(new TMxChart(this, mp_chart_box->Canvas)),
  mp_select(new TMxChartSelect(mp_chart, mp_chart_box)),
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
  m_base_chart_name(""),
  m_unsort_data()
{
  m_unsort_data.connect(m_data);

  Caption = "График";
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
  mp_dbg_info->Top = mp_panel->Height/2 - mp_dbg_info->Height/2;
  mp_dbg_info->Anchors >> akLeft;
  mp_dbg_info->Anchors << akRight;
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
  mp_base_chart_combo->Left = mp_clear_btn->Left + mp_clear_btn->Width + btn_gap;
  mp_base_chart_combo->Top = mp_panel->Height/2 - mp_base_chart_combo->Height/2;
  mp_base_chart_combo->Style = Stdctrls::csDropDownList;
  mp_base_chart_combo->OnChange = BaseChartComboChange;
  mp_base_chart_combo->Parent = this;
}
__fastcall irs::chart::builder_chart_window_t::TChartForm::
~TChartForm()
{
}
void __fastcall irs::chart::builder_chart_window_t::TChartForm::
FormResize(TObject *Sender)
{
  mp_chart->BoundsRect = mp_chart_box->ClientRect;
}
void __fastcall irs::chart::builder_chart_window_t::TChartForm::
TimerEvent(TObject *Sender)
{
  try {

    if (!m_pause) {
      if (!m_is_lock) {
        connect_data(m_data);
      }
    } else {
      if (m_invalidate) {
        m_invalidate = irs_false;
        mp_chart->BaseItem = m_base_item;
        mp_chart->Paint();
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
void __fastcall irs::chart::builder_chart_window_t::TChartForm::
PauseBtnClick(TObject *Sender)
{
  if (m_pause) {
    m_pause = irs_false;
    mp_pause_btn->Caption = mp_pause_on_text;
  } else {
    m_pause = irs_true;
    mp_pause_btn->Caption = mp_pause_off_text;
    m_pause_data = m_data;
    connect_data(m_pause_data);
  }
}
void __fastcall irs::chart::builder_chart_window_t::TChartForm::
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
void __fastcall irs::chart::builder_chart_window_t::TChartForm::
ClearBtnClick(TObject *Sender)
{
  m_event.clear();
}
void __fastcall irs::chart::builder_chart_window_t::TChartForm::
ChartPaint(TObject *Sender)
{
  mp_chart->Paint();
}
void __fastcall irs::chart::builder_chart_window_t::TChartForm::
FormShow(TObject *Sender)
{
  m_is_lock = false;
  m_invalidate = irs_true;
  TimerEvent(this);
}
void __fastcall irs::chart::builder_chart_window_t::TChartForm::
FormHide(TObject *Sender)
{
  m_is_lock = true;
}
void __fastcall irs::chart::builder_chart_window_t::TChartForm::
BaseChartComboChange(TObject *Sender)
{
  set_base_item(mp_base_chart_combo->ItemIndex);
  m_invalidate = irs_true;
  TimerEvent(this);
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
    int base_item_save = mp_chart->BaseItem;
    mp_chart->Clear();
    m_colors.start();
    m_unsort_data.start();
    const int bad_real_base_item = -1;
    int real_base_item = bad_real_base_item;
    int real_index = 0;
    for (unsort_data_t::size_type i = 0; i < m_unsort_data.size(); i++) {
      const chart_func_t *time_func = m_unsort_data.vec().time.get();
      const chart_func_t *func = m_unsort_data.vec().func.get();
      TColor color = m_colors.next();
      if (func->size()) {
        mp_chart->Add(&func->fn, func->size(), color);
        mp_chart->Last->DataX = &time_func->fn;
        if (!m_group_all) mp_chart->NewGroupY();
        if ((int)i == m_base_item) {
          real_base_item = real_index;
        }
        real_index++;
      }
      m_unsort_data.next();
    }
    if (real_base_item != bad_real_base_item) {
      mp_chart->BaseItem = real_base_item;
    } else {
      mp_chart->BaseItem = base_item_save;
    }
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
  
  m_base_chart_name = "";
  if (m_base_item < (int)m_unsort_data.size()) {
    m_unsort_data.set(m_base_item);
    m_base_chart_name = m_unsort_data.name();
  }
}


