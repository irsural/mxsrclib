// Полезные функции
// C++ Builder
// Дата: 14.04.2010
// Дата создания: 9.09.2009
//---------------------------------------------------------------------------
#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <MxBase.h>

#include <irsfinal.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void __fastcall Point(TCanvas *Canvas, TPoint P)
{
  Canvas->Pixels[P.x][P.y] = Canvas->Pen->Color;
}
//---------------------------------------------------------------------------


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

namespace Mxbase
{

//***************************************************************************
// Глобальные переменные

String AppName = Application->Title;

//***************************************************************************
// Глобальные функции

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
  GetTextExtentPoint32(Canvas->Handle, static_cast<AnsiString>(Text).c_str(),
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
  hmmio = mmioOpen(static_cast<AnsiString>(FileName).c_str(), NULL,
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
  hmmio = mmioOpen(static_cast<AnsiString>(FileName).c_str(), NULL,
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
  if (AppName != "") return ShowMsg(AppName, Text, mtCustom, Buttons);
  else return ShowMsg(Application->Title, Text, mtCustom, Buttons);
}
//---------------------------------------------------------------------------
void __fastcall ShowMsg(String Text)
{
  if (AppName != "")
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
char *__fastcall FetchStr(char *&Str)
{
  char *P, *Result;
  Result = Str;
  if (Str == NULL) return(Result);
  P = Str;
  while (*P == ' ') P++;
  Result = P;
  while ((*P != 0) && (*P != ',')) P++;
  if (*P == ',') { *P = 0; P++; }
  Str = P;
  return Result;
}
//---------------------------------------------------------------------------
TPrinterDC::TPrinterDC()
{
  DWORD ByteCnt, StructCnt;
  char DefaultPrinter[79];
  char *Cur, *Device;
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
    GetProfileString("windows", "device", "", DefaultPrinter,
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

} // namespace Mxbase

