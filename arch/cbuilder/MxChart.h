//! \file
//! \ingroup graphical_user_interface_group
//! \brief Построение графиков в C++ Builder
//!
//! Дата: 02.03.2011\n
//! Дата создания: 8.09.2008
//---------------------------------------------------------------------------
#ifndef MxChartH
#define MxChartH
//---------------------------------------------------------------------------
//#include <Classes.hpp>
//#include <Graphics.hpp>

#include <irsdefs.h>

#include <MxBase.h>
#include <XMLDoc.hpp>
#include <CheckLst.hpp>
#include <Grids.hpp>
#include <limits> // Отсутствует в IAR

#include <irsstd.h>
#include <irscpp.h>
#include <irschartwin.h>
#include <irs_chart_data.h>
#include <cbsysutils.h>

#include <irsfinal.h>

#define CHARTRANGE(Chart) BeginChartRange(Chart, 0), EndChartRange(Chart, 0)
#define CHARTRANGEL(Chart) BeginChartRange(Chart, -1), EndChartRange(Chart, -1)
#define CHARTRANGEI(Chart, Index) BeginChartRange(Chart, Index),\
  EndChartRange(Chart, Index)
//---------------------------------------------------------------------------

namespace Mxchart
{

//! \addtogroup graphical_user_interface_group
//! @{

using namespace std;

//---------------------------------------------------------------------------
class TMxChart;
class TMxChartSelect;
class TMxChartItem;
//---------------------------------------------------------------------------
int __fastcall BeginChartRange(TMxChart *Chart, int Index);
int __fastcall EndChartRange(TMxChart *Chart, int Index);
#if __BORLANDC__ < IRS_CPP_BUILDER2007
bool __fastcall operator == (TPoint P1, TPoint P2);
bool __fastcall operator != (TRect R1, TRect R2);
#endif //__BORLANDC__ < IRS_CPP_BUILDER2010
//---------------------------------------------------------------------------
enum TChartChangeType {cctArea, cctBounds, cctBoundsRect, cctCanvas, cctHide,
  cctAutoScale, cctMonotone, cctGroup, cctShift, cctScale, cctStep, cctCompConv,
  cctSizeGrid, cctSizeAuxGrid, cctData, cctPrintRect, cctGraphObj, cctItems,
  cctMarker, cctBaseItem};
enum TChartErrorType {cetConvX, cetConvY, cetCalcX, cetCalcY};
typedef void __fastcall (__closure *TChartChange)(TObject *Sender,
  TChartChangeType ChartChangeType);
typedef void __fastcall (__closure *TChartError)(TObject *Sender, Exception &e,
  TDblPoint P, double t, TChartErrorType ChartErrorType);
//---------------------------------------------------------------------------
class TMxChartItem: public TComponent
{
  enum { CoorX = 0, CoorY = 1};

  friend class TMxChart;
  // Поля
  bool FHide;
  bool NeedCalculate;
  bool NeedAutoScale;
  bool FError;
  int IndexMarkerX, IndexMarkerY;
  double *FMarkerX, *FMarkerY;
  TDblRect ClipArea;
  vector<TLine> Lines;
  TCanvas *FCanvas;
  TClassDblFunc FFunc[2];
  TPointer FData[2];
  TDblRect FArea;
  TDblBounds FBounds;
  TRect FBoundsRect, FPrevBoundsRect;
  double FStep;
  bool FAutoScale[2];
  bool FMonotone[2];
  TChartChange FOnChange;
  TChartError FOnError;
  int FGroup[2];
  double FShiftX, FShiftY;
  double FScaleX, FScaleY;
  TPen *FPen;
  TPen *FMarkerPen;
  // Свойства
  __property int Left = {read=GetLeft, write=SetLeft};
  __property int Top = {read=GetTop, write=SetTop};
  __property int Width = {read=GetWidth, write=SetWidth};
  __property int Height = {read=GetHeight, write=SetHeight};
  __property TCanvas *Canvas = {read=FCanvas, write=SetCanvas};
  __property TRect BoundsRect = {read=GetBoundsRect, write=SetBoundsRect};
  // События
  __property TChartChange OnChange = {read=FOnChange, write=FOnChange};
  __property TChartError OnError = {read=FOnError, write=FOnError};
  // Методы
  bool __fastcall IntoArea(TDblPoint P) const;
  bool __fastcall OutArea(TDblPoint P1, TDblPoint P2) const;
  bool __fastcall ClipLine(TDblPoint &P1, TDblPoint &P2) const;
  double DefFunc(double x) const;
  double Func(int i, double x) const;
  double FuncX(double x);
  double FuncY(double x);
  TPoint __fastcall ConvCoor(TDblPoint P);
  TDblPoint __fastcall XYFunc(double t) const;
  void __fastcall FloorAxis();
  TRect __fastcall GetBoundsRect() const;
  TDblRect __fastcall GetArea();
  int __fastcall GetLeft() const;
  int __fastcall GetTop() const;
  int __fastcall GetWidth() const;
  int __fastcall GetHeight() const;
  double __fastcall GetMarkerX(int Index) const;
  double __fastcall GetMarkerY(int Index) const;
  TCompConv __fastcall GetCompConv(int Index) const;
  void __fastcall PaintMarkerX(int i);
  void __fastcall PaintMarkerY(int i);
  void __fastcall SetArea(TDblRect Area);
  void __fastcall SetAutoScale(int Index, bool Value);
  void __fastcall SetMonotone(int Index, bool Value);
  void __fastcall SetBounds(TDblBounds Bounds);
  void __fastcall SetBoundsRect(TRect Value);
  void __fastcall SetCompConv(int Index, TCompConv Value);
  void __fastcall SetData(int Index, TPointer Value);
  void __fastcall SetStep(double Step);
  void __fastcall SetLeft(int Value);
  void __fastcall SetTop(int Value);
  void __fastcall SetWidth(int Value);
  void __fastcall SetHeight(int Value);
  void __fastcall SetMarkerX(int Index, double Value);
  void __fastcall SetMarkerY(int Index, double Value);
  void __fastcall SetCanvas(TCanvas *Value);
  void __fastcall SetHide(bool Value);
  void __fastcall SetShift(int Index, double Value);
  void __fastcall SetScale(int Index, double Value);
  void __fastcall SetPen(TPen *Value);
  void __fastcall SetMarkerPen(TPen *Value);
  bool __fastcall ValidRect() const;
  void __fastcall Paint(int Tag);
  void __fastcall DoAutoScale();
  void MonotoneBounds(int ACoor, double AAreaBegin,
    double AAreaEnd, double* APBegin, double* APEnd, bool* is_finded);
  void __fastcall DoCalculate();
  void __fastcall DoPaint();
  void __fastcall DoBlackPrint();
  void __fastcall DoColorPrint();
  void __fastcall SetGroup(int Index, int AGroup);
public:
  // Свойства
  __property int GroupX = {index=0, read=FGroup[0], write=SetGroup};
  __property int GroupY = {index=1, read=FGroup[1], write=SetGroup};
  __property double ShiftX = {index=0, read=FShiftX, write=FShiftX};
  __property double ShiftY = {index=1, read=FShiftY, write=FShiftY};
  __property double ScaleX = {index=0, read=FScaleX, write=FScaleX};
  __property double ScaleY = {index=1, read=FScaleY, write=FScaleY};
  __property bool Hide = {read=FHide, write=SetHide};
  __property TPointer DataX = {index=0, read=FData[0], write=SetData};
  __property TPointer DataY = {index=1, read=FData[1], write=SetData};
  __property TCompConv CompConvX = {index=0, read=GetCompConv, write=SetCompConv};
  __property TCompConv CompConvY = {index=1, read=GetCompConv, write=SetCompConv};
  __property bool AutoScaleX = {index=0, read=FAutoScale[0], write=SetAutoScale};
  __property bool AutoScaleY = {index=1, read=FAutoScale[1], write=SetAutoScale};
  __property bool MonotoneX = {index=0, read=FMonotone[0], write=SetMonotone};
  __property bool MonotoneY = {index=1, read=FMonotone[1], write=SetMonotone};
  __property double Step = {read=FStep, write=SetStep};
  __property TDblBounds Bounds = {read=FBounds, write=SetBounds};
  __property TDblRect Area = {read=GetArea, write=SetArea};
  __property int CountMarkerX = {index=0, read=IndexMarkerX};
  __property int CountMarkerY = {index=1, read=IndexMarkerY};
  __property double MarkerX[int Index] = {read=GetMarkerX, write=SetMarkerX};
  __property double MarkerY[int Index] = {read=GetMarkerY, write=SetMarkerY};
  __property double FirstMarkerX = {index=0, read=GetMarkerX, write=SetMarkerX};
  __property double FirstMarkerY = {index=1, read=GetMarkerY, write=SetMarkerY};
  __property double LastMarkerX = {index=-1, read=GetMarkerX, write=SetMarkerX};
  __property double LastMarkerY = {index=-1, read=GetMarkerY, write=SetMarkerY};
  __property TPen *Pen = {read=FPen, write=SetPen};
  __property TPen *MarkerPen = {read=FMarkerPen, write=SetMarkerPen};
  // Конструкторы и деструкторы
  __fastcall TMxChartItem(TComponent *AOwner);
  __fastcall virtual ~TMxChartItem();
  // Методы
  void __fastcall AddMarkerX(double Value);
  void __fastcall AddMarkerY(double Value);
  void __fastcall ClearMarkerX();
  void __fastcall ClearMarkerY();
  void __fastcall DeleteMarkerX(int Index);
  void __fastcall DeleteMarkerY(int Index);
  void __fastcall DeleteMarkerX();
  void __fastcall DeleteMarkerY();
  void __fastcall DeleteMarkerX(int Begin, int End);
  void __fastcall DeleteMarkerY(int Begin, int End);
  void __fastcall Invalidate();
};
//---------------------------------------------------------------------------
class TMxChart: public TComponent
{
  friend class TMxChartItem;
  // Поля
  bool NeedAutoScale;
  bool NeedCalculate;
  bool NeedRepaint;
  TNotifyEvent FOnPaint;
  TNotifyEvent FOnPrint;
  TChartChange FOnChange;
  TChartError FOnError;
  Graphics::TBitmap *FBitmap;
  TColor FMarkerColor;
  int IndexMarkerX, IndexMarkerY;
  double *FMarkerX, *FMarkerY;
  bool FShowGrid;
  bool FShowAuxGrid;
  bool FShowBounds;
  int FCount;
  TMxChartItem **FItems;
  TCanvas *FCanvas;
  TCanvas *FPaintCanvas;
  TCanvas *FPrintCanvas;
  TCanvas *FCurCanvas;
  TPen *FMarkerPen;
  TPen *FAuxGridPen;
  TRect FBoundsRect;
  TRect FGridRect;
  TRect FPrintRect;
  TRect FCurRect;
  bool FAutoScaleX, FAutoScaleY;
  TDblRect FArea;
  double FMinWidthGrid, FMinHeightGrid;
  double FMinWidthAuxGrid, FMinHeightAuxGrid;
  bool PaintMode;
  double SW, SH;
  bool ApplyTextX, ApplyTextY;
  TNotifyEvent PenChanged;
  TNotifyEvent BrushChanged;
  TNotifyEvent FontChanged;
  int FBaseItem;
  int CurGroupX, CurGroupY;
  // Методы
  void __fastcall PaintGrid();
  void __fastcall PaintGrid(bool BlackAuxGrid);
  void __fastcall CalcGrid();
  void __fastcall PaintBounds() const;
  TDblRect __fastcall GetArea();
  TRect __fastcall GetGridRect();
  int __fastcall GetLeft() const;
  int __fastcall GetTop() const;
  int __fastcall GetWidth() const;
  int __fastcall GetHeight() const;
  double __fastcall GetMarkerX(int Index) const;
  double __fastcall GetMarkerY(int Index) const;
  TMxChartItem *__fastcall GetItem(int Index) const;
  void __fastcall SetChildRect();
  void __fastcall SetArea(TDblRect Area);
  void __fastcall SetPrintRect(TRect Value);
  void __fastcall SetLeft(int Value);
  void __fastcall SetTop(int Value);
  void __fastcall SetWidth(int Value);
  void __fastcall SetHeight(int Value);
  void __fastcall SetBoundsRect(TRect Value);
  void __fastcall SetShowBounds(bool Value);
  void __fastcall SetShowGrid(bool Value);
  void __fastcall SetShowAuxGrid(bool Value);
  void __fastcall SetAutoScaleX(bool Value);
  void __fastcall SetAutoScaleY(bool Value);
  void __fastcall SetPaintCanvas(TCanvas *Value);
  void __fastcall SetMarkerX(int Index, double Value);
  void __fastcall SetMarkerY(int Index, double Value);
  void __fastcall SetCurCanvas(TCanvas *Value);
  void __fastcall SetMarkerPen(TPen *Value);
  void __fastcall SetAuxGridPen(TPen *Value);
  void __fastcall SetMinWidthGrid(double Value);
  void __fastcall SetMinHeightGrid(double Value);
  void __fastcall SetMinWidthAuxGrid(double Value);
  void __fastcall SetMinHeightAuxGrid(double Value);
  void __fastcall DoAutoScale();
  void __fastcall DoCalculate();
  void __fastcall DoRepaint(int Tag);
  TPoint __fastcall ConvCoor(TDblPoint P) const;
  int __fastcall ConvCoorX(double b, double e, double x) const;
  int __fastcall ConvCoorY(double b, double e, double y) const;
  void __fastcall PaintMarkerX(int i);
  void __fastcall PaintMarkerY(int i);
  void __fastcall ChildChange(TObject *Sender,
    TChartChangeType ChartChangeType);
  void __fastcall ChildError(TObject *Sender, Exception &e, TDblPoint P,
    double t, TChartErrorType ChartErrorType) const;
  void __fastcall Constructor();
  double __fastcall StepCalc(double Begin, double End, int Size,
    int MinSize, bool IsWidth, bool AccountText);
  void __fastcall GraphObjChanged(TObject *Sender);
  String __fastcall SFF(double x, double Step) const;
  bool __fastcall ValidRect() const;
  void __fastcall PaintChildMarkerX(TMxChartItem *Sender, int i) const;
  void __fastcall PaintChildMarkerY(TMxChartItem *Sender, int i) const;
  void __fastcall SetBaseItem(int AChart);
  // Свойства
  __property TCanvas *CurCanvas = {read=FCurCanvas, write=SetCurCanvas};
public:
  // Поля
  TMxChartSelect *Select;
  // Свойства
  __property int BaseItem = {read=FBaseItem, write=SetBaseItem};
  __property bool ShowBounds = {read=FShowBounds, write=SetShowBounds};
  __property bool ShowGrid = {read=FShowGrid, write=SetShowGrid};
  __property bool ShowAuxGrid = {read=FShowAuxGrid, write=SetShowAuxGrid};
  __property TDblRect Area = {read=GetArea, write=SetArea};
  __property bool AutoScaleX = {read=FAutoScaleX, write=SetAutoScaleX};
  __property bool AutoScaleY = {read=FAutoScaleY, write=SetAutoScaleY};
  __property TCanvas *Canvas = {read=FCanvas};
  __property TCanvas *PaintCanvas = {read=FPaintCanvas, write=SetPaintCanvas};
  __property TCanvas *PrintCanvas = {read=FPrintCanvas, write=FPrintCanvas};
  __property int CountMarkerX = {index=0, read=IndexMarkerX};
  __property int CountMarkerY = {index=1, read=IndexMarkerY};
  __property double MarkerX[int Index] = {read=GetMarkerX, write=SetMarkerX};
  __property double MarkerY[int Index] = {read=GetMarkerY, write=SetMarkerY};
  __property double FirstMarkerX = {index=0, read=GetMarkerX, write=SetMarkerX};
  __property double FirstMarkerY = {index=1, read=GetMarkerY, write=SetMarkerY};
  __property double LastMarkerX = {index=-1, read=GetMarkerX, write=SetMarkerX};
  __property double LastMarkerY = {index=-1, read=GetMarkerY, write=SetMarkerY};
  __property int Count = {read=FCount};
  __property TMxChartItem *Items[int Index] = {read=GetItem};
  __property int Left = {read=GetLeft, write=SetLeft};
  __property int Top = {read=GetTop, write=SetTop};
  __property int Width = {read=GetWidth, write=SetWidth};
  __property int Height = {read=GetHeight, write=SetHeight};
  __property TRect BoundsRect = {read=FBoundsRect, write=SetBoundsRect};
  __property TRect PrintRect = {read=FPrintRect, write=SetPrintRect};
  __property TRect GridRect = {read=GetGridRect};
  __property TPen *MarkerPen = {read=FMarkerPen, write=SetMarkerPen};
  __property TPen *AuxGridPen = {read=FAuxGridPen, write=SetAuxGridPen};
  __property double MinWidthGrid = {read=FMinWidthGrid, write=SetMinWidthGrid};
  __property double MinHeightGrid = {read=FMinHeightGrid, write=SetMinHeightGrid};
  __property double MinWidthAuxGrid = {read=FMinWidthAuxGrid, write=SetMinWidthAuxGrid};
  __property double MinHeightAuxGrid = {read=FMinHeightAuxGrid, write=SetMinHeightAuxGrid};
  __property TMxChartItem *First = {index=0, read=GetItem};
  __property TMxChartItem *Last = {index=-1, read=GetItem};
  // События
  __property TNotifyEvent OnPaint = {read=FOnPaint, write=FOnPaint};
  __property TNotifyEvent OnPrint = {read=FOnPrint, write=FOnPrint};
  __property TChartChange OnChange = {read=FOnChange, write=FOnChange};
  __property TChartError OnError = {read=FOnError, write=FOnError};
  // Конструкторы и деструкторы
  __fastcall TMxChart(TComponent *AOwner);
  __fastcall TMxChart(TComponent *AOwner, TCanvas *ACanvas);
  __fastcall virtual ~TMxChart();
  // Методы
  void __fastcall Add();
  void __fastcall Add(TPointer Data, int Count);
  void __fastcall Add(TPointer Data, int Count, TColor AColor);
  void __fastcall Delete(int Index);
  void __fastcall Clear();
  void __fastcall Insert(int Index);
  void __fastcall Insert(int Index, TPointer Data, int Count);
  void __fastcall Insert(int Index, TPointer Data, int Count, TColor AColor);
  void __fastcall NewGroupX();
  void __fastcall NewGroupY();
  void __fastcall Paint();
  void __fastcall BlackPrint();
  void __fastcall ColorPrint();
  void __fastcall SaveToMetafile(String FileName);
  void __fastcall PreCalc();
  void __fastcall AddMarkerX(double Value);
  void __fastcall AddMarkerY(double Value);
  void __fastcall ClearMarkerX();
  void __fastcall ClearMarkerY();
  void __fastcall DeleteMarkerX(int Index);
  void __fastcall DeleteMarkerY(int Index);
  void __fastcall DeleteMarkerX();
  void __fastcall DeleteMarkerY();
  void __fastcall DeleteMarkerX(int Begin, int End);
  void __fastcall DeleteMarkerY(int Begin, int End);
  void __fastcall Invalidate();
};

class TMxChartSelect: public TObject
{
  typedef pair<bool, bool> pairbool;
  typedef map<TComponent *, pairbool> mapscale;
  typedef pair<TCompConv, TCompConv> pairconv;
  typedef map<TComponent *, pairconv> mapconv;
  typedef mapconv::iterator mapconvit;
  typedef map<int *, TDblRect> maparea;

  TMxChart *FChart;
  TControl *FControl;
  TNotifyEvent FOnError;
  mapscale AutoScales;
  maparea MAreas;
  bool LockEvent;
  bool ValidAutoScales;
  bool ValidAreas;
  vector<TDblRect> Areas;
  vector<int> BaseItems;
  vector<mapconv> CompConvs;
  //TDblRect CurArea;
  int Xn, Yn;
  double PX, PY;
  TRect SelRect;
  bool SelectMode, FirstSelect;
  double FZoomFactor;
  bool FPositioningMode;
  bool FDragChartMode;
  
  void __fastcall DoMouseDown(TObject *Sender, TMouseButton Button,
    TShiftState Shift, int X, int Y);
  void __fastcall DoMouseMove(TObject *Sender, TShiftState Shift, int X,
    int Y);
  void __fastcall DoMouseUp(TObject *Sender, TMouseButton Button,
    TShiftState Shift, int X, int Y);
  void __fastcall DoDblClick(TObject *Sender);
  void __fastcall DoPaint();
  TControl *__fastcall GetControl();
  void __fastcall RestoreAutoScales();
  void __fastcall SaveAutoScales();
  void __fastcall RestoreCompConvs();
  void __fastcall SaveCompConvs();
  void __fastcall SelectClip(TRect &ARect) const;
  void __fastcall SelectRect(TRect Rect) const;
  void __fastcall SetControl(TControl *AControl);
  void __fastcall SetZoomFactor(double Value);
  void __fastcall SetPositioningMode(bool Value);
  void __fastcall SetDragChartMode(bool Value);
  void __fastcall SaveAreas();
  void __fastcall RestoreAreas();
public:
  __property TMxChart *Chart = {read=FChart, write=FChart};
  __property TControl *Control = {read=FControl, write=SetControl};
  __property TNotifyEvent OnError = {read=FOnError, write=FOnError};
  __property double ZoomFactor = {read=FZoomFactor, write=SetZoomFactor};
  __property bool PositioningMode = {read=FPositioningMode, write=SetPositioningMode};
  __property bool DragChartMode = {read=FDragChartMode, write=SetDragChartMode};
  void __fastcall All();
  void __fastcall Shift(double X, double Y = 0);
  void __fastcall Positioning(double X, double Y = 0);
  void __fastcall ZoomIn(double FX = -1, double FY = 1);
  void __fastcall ZoomOut();
  void __fastcall Connect();
  void __fastcall Disconnect();
  void __fastcall ItemChange(int Oper, int Index);
  __fastcall TMxChartSelect(): TObject() {}
  __fastcall TMxChartSelect(TMxChart *AChart);
  __fastcall TMxChartSelect(TMxChart *AChart, TControl *AControl);
  __fastcall ~TMxChartSelect();
};

//! @}

} // namespace Mxchart

using namespace Mxchart;





namespace irs {

namespace chart {

//! \addtogroup graphical_user_interface_group
//! @{

// Генератор цветов
class color_gen_t
{
public:
  typedef irs::string_t string_type;
  color_gen_t();
  void start();
  TColor next();
  TColor get(int a_index) const;
  String name_next();
  String name(int a_index) const;
  int size() const;
private:
  vector<TColor> m_colors;
  vector<string_type> m_names;
  int m_index;
};

class charts_xml_file_t
{
public:
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;
  typedef irs::chart_data::charts_t charts_type;
  charts_xml_file_t();
  charts_type load(const string_type& a_file_name) const;
  void save(const charts_type& a_charts, const string_type& a_file_name) const;
private:
  _di_IXMLDocument mp_xmldoc;
  String m_root_element_name;
  String m_charts_node_name;
  String m_chart_node_name;
  String m_chart_name_attribute_name;
  String m_chart_index_attribute_name;
  String m_point_node_name;
  String m_x_attribute_name;
  String m_y_attribute_name;
};

// Окно с графиком для C++ Builder
class builder_chart_window_t: public irs::chart_window_t
{
public:
  typedef std::size_t size_type;
  typedef irs::string_t string_type;
  enum form_type_t { ft_internal, ft_external };
  enum stay_on_top_t { stay_on_top_on, stay_on_top_off };

  builder_chart_window_t(irs_u32 a_size = 1000,
    irs_i32 a_refresh_time_ms = 1000,
    stay_on_top_t a_stay_on_top = stay_on_top_on);
  builder_chart_window_t(TForm* ap_form,
    irs_u32 a_size = 1000,
    irs_i32 a_refresh_time_ms = 1000);
  virtual void show();
  virtual void hide();
  virtual rect_t position() const;
  virtual void set_position(const rect_t &a_position);
  virtual void add_param(const string_type &a_name);
  virtual void add_param(const string_type &a_name,
    const vector<double>& a_array_time,
    const vector<double>& a_array_value);
  virtual void delete_param(const string_type &a_name);
  virtual void clear_param();
  virtual void set_value(const string_type &a_name, double a_value);
  virtual void set_value(const string_type &a_name, double a_time,
    double a_value);
  virtual void set_time(double a_time);
  virtual void add();
  virtual void add(const string_type &a_name, double a_time, double a_value);
  virtual void clear();
  virtual void set_refresh_time(irs_i32 a_refresh_time_ms);
  virtual void resize(irs_u32 a_size);
  virtual irs_u32 size() const;
  virtual void group_all();
  virtual void ungroup_all();
  void set_visible(const string_type &a_name, bool a_enabled);
  void load_from_mxchart_file(const string_type& a_file_name);
  void save_to_mxchart_file(const string_type& a_file_name);
  void load_from_csv(const string_type& a_file_name);
  void save_to_csv(const string_type& a_file_name);
private:
   // Подключение к форме и создание элементов управления
  void init(TForm *ap_form);
  // Уничтожение всех компонентов формы
  void deinit();
  irs::chart_data::charts_t make_charts() const;
  class chart_func_t;
  struct chart_point_t;

  typedef deque<double> chart_vec_t;
  typedef map<string_type, chart_point_t> data_t;

  class chart_func_t: public TObject
  {
  public:
    explicit chart_func_t(const chart_vec_t &a_data);
    double fn(double a_index) const;
    irs_u32 size() const;
  private:
    const chart_vec_t &m_data;
  };
  struct chart_point_t
  {
    chart_vec_t vec;
    chart_vec_t vectime;
    auto_ptr<chart_func_t> func;
    auto_ptr<chart_func_t> time;
    irs_i32 index;
    bool visible;
    chart_point_t();
    chart_point_t(const chart_point_t& a_point);
    chart_point_t& operator=(const chart_point_t& a_point);
  }; //struct chart_point_t
  friend class chart_event_t;
  class chart_event_t
  {
    builder_chart_window_t &m_chart_window;
  public:
    chart_event_t(builder_chart_window_t &a_chart_window):
      m_chart_window(a_chart_window)
    {}
    void clear() { m_chart_window.clear(); }
  }; //class chart_event_t
  // Пересортировка data_t в порядке добавления
  class unsort_data_t
  {
  public:
    typedef data_t::size_type size_type;
    typedef irs::string_t string_type;

    unsort_data_t();
    void connect(const data_t &a_data);
    bool empty();
    void start();
    void next();
    void set(size_type a_index);
    string_type name();
    string_type name(size_type a_index);
    const chart_point_t& vec();
    const chart_point_t& vec(size_type a_index);
    size_type size();
  private:
    const data_t* mp_data;
    map<int, string_type> m_unsort_data;
    map<int, string_type>::iterator mp_unsort_data_it;
    chart_point_t m_bad_point;
  };
  // Форма с графиком
  class controls_t: public TObject
  {
  public:
    typedef irs::string_t string_type;
    controls_t(builder_chart_window_t* ap_builder_chart_window,
      form_type_t a_form_type,
      TForm* ap_form, const data_t &a_data, chart_event_t &a_event,
      irs_i32 a_refresh_time_ms, stay_on_top_t a_stay_on_top);
    virtual __fastcall ~controls_t();
    inline bool fix();
    inline void group_all();
    inline void ungroup_all();
    inline void invalidate();
    void chart_list_changed();
    void set_refresh_time_ms(irs_i32 a_refresh_time_ms);
  private:
    enum { list_box_scroll_width_additive = 20 };
    builder_chart_window_t* mp_builder_chart_window;
    form_type_t m_form_type;
    TForm* mp_form;
    irs::cbuilder::file_version_t m_version;
    irs_bool m_group_all;
    //chart_point_t m_pause_time;
    data_t m_pause_data;
    bool m_pause;
    bool m_fix;
    //const chart_point_t &m_time;
    const data_t &m_data;
    chart_event_t &m_event;
    TIdleEvent m_IdleEvent;
    TOpenDialog* mp_file_open_dialog;
    TSaveDialog* mp_file_save_dialog;
    String m_file_name;
    TMainMenu* mp_main_menu;
    TMenuItem* mp_file_menu_item;
    TMenuItem* mp_file_open_menu_item;
    TMenuItem* mp_file_save_menu_item;
    TMenuItem* mp_file_save_as_menu_item;
    TMenuItem* mp_tools_menu_item;
    TMenuItem* mp_chart_options_menu_item;
    TMenuItem* mp_help_menu_item;
    TMenuItem* mp_about_menu_item;
    TPanel* mp_panel;
    TPanel* mp_options_panel;
    TCheckListBox* mp_chart_list;
    TButton* mp_delete_chart_btn;
    TStringGrid* mp_points_grid;
    TSplitter* mp_options_splitter;
    TPanel* mp_paint_panel;
    TPaintBox* mp_chart_box;
    TMxChart* mp_chart;
    auto_ptr<TMxChartSelect> mp_select;
    TTimer* mp_timer;
    TButton* mp_pause_btn;
    TLabel* mp_dbg_info;
    TButton* mp_fix_btn;
    TButton* mp_clear_btn;
    TComboBox* mp_base_chart_combo;
    //! \brief Соответсвие индексов mp_base_chart_combo индексам в
    //!   m_unsort_data
    map<int, int> m_base_chart_combo_index_map;
    //! \brief Соответствие индексов m_unsort_data индексам отображаемых
    //!   графиков. Содержит индексы только отображаемых графиков
    map<int, int> m_unsort_data_chart_item_map;
    const char *mp_pause_on_text;
    const char *mp_pause_off_text;
    const char *mp_fix_on_text;
    const char *mp_fix_off_text;
    irs_i32 m_refresh_time_ms;
    irs_bool m_invalidate;
    mx_time_int_local_t m_time_int;
    const data_t* mp_data;
    //TMxChart *mp_chart;
    int m_base_item;
    color_gen_t m_colors;
    bool m_is_lock;
    bool m_is_chart_list_changed;
    string_type m_base_chart_name;
    unsort_data_t m_unsort_data;

    void __fastcall PaintPanelResize(TObject *Sender);

    void __fastcall FormResize(TObject *Sender);
    void __fastcall TimerEvent(TObject *Sender);

    void __fastcall FileOpenMenuItemClick(TObject *Sender);
    void __fastcall FileSaveMenuItemClick(TObject *Sender);
    void __fastcall FileSaveAsMenuItemClick(TObject *Sender);
    void __fastcall ChartOptionsMenuItemClick(TObject *Sender);
    void __fastcall HelpMenuItemClick(TObject *Sender);

    void __fastcall PauseBtnClick(TObject *Sender);
    void __fastcall FixBtnClick(TObject *Sender);
    void __fastcall ClearBtnClick(TObject *Sender);
    void __fastcall ChartPaint(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall BaseChartComboChange(TObject *Sender);

    void __fastcall ChartCheckListBoxClick(TObject *Sender);
    void __fastcall ChartCheckListBoxClickCheck(TObject *Sender);
    void __fastcall DeleteChartBtnClick(TObject *Sender);

    void connect_data(const data_t &a_data);
    void update_chart_combo();
    void update_chart_list();
    void update_points_grid();
    void update_chart();
    //! \brief Устанавливает текущий график
    //! \param[in] a_base_item - индекс графика в mp_base_chart_combo
    void set_base_item(int a_base_item);
    int chart_from_combo_item(int a_combo_item);
    //void connect_data(TMxChart *ap_chart, const data_t &a_data);
  }; //class controls_t

  //chart_point_t m_time;
  data_t m_data;
  irs_u32 m_size;
  chart_event_t m_event;
  irs_i32 m_refresh_time_ms;
  form_type_t m_form_type;
  irs::handle_t<TForm> mp_internal_form;
  TForm* mp_form;
  irs::handle_t<controls_t> mp_controls;
  rect_t m_position;
  stay_on_top_t m_stay_on_top;
  irs_i32 m_chart_index;

  irs_u32 cur_size() const;
}; //class builder_chart_window_t

//! @}

} //namespace chart

} //namespace irs
//---------------------------------------------------------------------------
#endif //MxChartH
