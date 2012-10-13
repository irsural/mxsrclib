//! \file
//! \ingroup graphical_user_interface_group
//! \brief Построение графиков
//!
//! Дата: 30.04.2012\n
//! Дата создания: 30.04.2012

#ifndef MXEXTCHARTH
#define MXEXTCHARTH

#include <irsdefs.h>

#if defined(QT_CORE_LIB)
#include <QPen>
#elif defined(__BORLANDC__)
#include <Graphics.hpp>
#endif //GUI Libs

#include <mxextbase.h>
#include <irsstd.h>
#include <irscpp.h>
#include <irschartwin.h>
#include <mxdata.h>

#include <irsfinal.h>

#ifndef MXEXT_OFF
#ifdef IRS_FULL_STDCPPLIB_SUPPORT
#if defined(QT_CORE_LIB) || defined(__BORLANDC__)

#define MXCHARTRANGE(Chart) BeginChartRange(Chart, 0), EndChartRange(Chart, 0)
#define MXCHARTRANGEL(Chart) BeginChartRange(Chart, -1),\
  EndChartRange(Chart, -1)
#define MXCHARTRANGEI(Chart, Index) BeginChartRange(Chart, Index),\
  EndChartRange(Chart, Index)

namespace irs {

//! \addtogroup graphical_user_interface_group
//! @{

#ifdef NOP
class pen_t;
struct native_gui_lib_stuff_t {
  #if defined(QT_CORE_LIB)
  typedef QPen native_pen_type;
  typedef QBrush native_brush_type;
  typedef QColor native_color_type;
  #elif defined(__BORLANDC__)
  typedef TPen native_pen_type;
  typedef TBrush native_brush_type;
  typedef TColor native_color_type;
  #else //GUI Libs
  typedef int native_pen_type
  typedef int native_brush_type;
  typedef int native_color_type;
  #endif //GUI Libs

  static color_union_t color_from_native(
    const native_color_type& a_native_color);
  static void color_to_native(color_union_t a_color,
    native_color_type* ap_native_color);
  static void pen_general_to_native(const pen_t& a_general_pen,
    native_pen_type* ap_native_pen);
  static void pen_native_to_general(const native_pen_type& ap_native_pen,
    pen_t* a_general_pen);
};
#endif //NOP

class mx_ext_chart_t;
class mx_ext_chart_select_t;
class mx_ext_chart_item_t;

mx_ext_chart_types::size_type BeginChartRange(mx_ext_chart_t *Chart,
  mx_ext_chart_types::size_type Index);
mx_ext_chart_types::size_type EndChartRange(mx_ext_chart_t *Chart,
  mx_ext_chart_types::size_type Index);
mx_ext_chart_types::size_type mx_ext_chart_last();

enum TChartChangeType {cctArea, cctBounds, cctBoundsRect, cctCanvas, cctHide,
  cctAutoScale, cctGroup, cctShift, cctScale, cctStep, cctCompConv,
  cctSizeGrid, cctSizeAuxGrid, cctData, cctPrintRect, cctGraphObj, cctItems,
  cctMarker, cctBaseItem};
enum TChartErrorType {cetConvX, cetConvY, cetCalcX, cetCalcY};
#ifdef NOP
typedef void (__closure *TChartChange)(TObject *Sender,
  TChartChangeType ChartChangeType);
typedef void (__closure *TChartError)(TObject *Sender, Exception &e,
  point_float_type P, double t, TChartErrorType ChartErrorType);
#endif //NOP

class mx_ext_chart_item_t: public mx_ext_chart_types
{
public:
  enum idx_t { x_idx = 0, y_idx = 1};

  // Конструкторы и деструкторы
  mx_ext_chart_item_t();
  virtual ~mx_ext_chart_item_t();

  // Методы
  size_type group_x() const;
  void group_x(size_type a_group_x);
  size_type group_y() const;
  void group_y(size_type a_group_y);
  float_type shift_x() const;
  void shift_x(const float_type& a_shift_x);
  float_type shift_y() const;
  void shift_y(const float_type& a_shift_y);
  float_type scale_x() const;
  void scale_x(const float_type& a_scale_x);
  float_type scale_y() const;
  void scale_y(const float_type& a_scale_y);
  bool hide() const;
  void hide(bool a_hide);
  TPointer data_x() const;
  void data_x(TPointer ap_data_x);
  TPointer data_y() const;
  void data_y(TPointer ap_data_y);
  TCompConv comp_conv_x() const;
  void comp_conv_x(TCompConv a_comp_conv);
  TCompConv comp_conv_y() const;
  void comp_conv_y(TCompConv a_comp_conv);
  bool auto_scale_x() const;
  void auto_scale_x(bool a_auto_scale_x);
  bool auto_scale_y() const;
  void auto_scale_y(bool a_auto_scale_y);
  float_type step() const;
  void step(const float_type& a_step);
  bounds_float_type bounds() const;
  void bounds(const bounds_float_type& a_bounds);
  rect_float_type area() const;
  void area(const rect_float_type& AArea);
  size_type count_marker_x() const;
  size_type count_marker_y() const;
  float_type marker_x(size_type Index) const;
  float_type marker_y(size_type Index) const;
  void marker_x(size_type Index, float_type Value);
  void marker_y(size_type Index, float_type Value);
  float_type first_marker_x();
  float_type first_marker_y();
  void first_marker_x(const float_type& a_position);
  void first_marker_y(const float_type& a_position);
  float_type last_marker_x();
  float_type last_marker_y();
  void last_marker_x(const float_type& a_position);
  void last_marker_y(const float_type& a_position);
  pen_t pen();
  void pen(const pen_t& a_pen);
  pen_t marker_pen();
  void marker_pen(const pen_t& a_pen);
  void add_marker_x(float_type a_marker_pos);
  void add_marker_y(float_type a_marker_pos);
  void clear_marker_x();
  void clear_marker_y();
  void delete_marker_x(size_type Index);
  void delete_marker_y(size_type Index);
  void delete_marker_x();
  void delete_marker_y();
  void delete_marker_x(size_type Begin, size_type End);
  void delete_marker_y(size_type Begin, size_type End);
  void invalidate();
private:
  friend class mx_ext_chart_t;

  // Поля
  bool FHide;
  bool NeedCalculate;
  bool NeedAutoScale;
  bool FError;
  size_type IndexMarkerX, IndexMarkerY;
  float_type *FMarkerX, *FMarkerY;
  rect_float_type ClipArea;
  vector<line_type> Lines;
  TCanvas *FCanvas;
  TClassDblFunc FFunc[2];
  TPointer FData[2];
  rect_float_type FArea;
  bounds_float_type FBounds;
  rect_int_type FBoundsRect, FPrevBoundsRect;
  float_type FStep;
  bool FAutoScale[2];
  TChartChange FOnChange;
  TChartError FOnError;
  size_type FGroup[2];
  float_type FShiftX, FShiftY;
  float_type FScaleX, FScaleY;
  pen_t FPen;
  pen_t FMarkerPen;

  // Свойства
  #ifdef NOP
  //generator_events_t ;
  __property int_type Left = {read=GetLeft, write=SetLeft};
  __property int_type Top = {read=GetTop, write=SetTop};
  __property int_type Width = {read=GetWidth, write=SetWidth};
  __property int_type Height = {read=GetHeight, write=SetHeight};
  #endif //NOP
  __property TCanvas *Canvas = {read=FCanvas, write=SetCanvas};
  __property rect_int_type BoundsRect = {read=GetBoundsRect, write=SetBoundsRect};
  // События
  __property TChartChange OnChange = {read=FOnChange, write=FOnChange};
  __property TChartError OnError = {read=FOnError, write=FOnError};

  // Методы
  void group(idx_t Index, size_type a_group_x);
  void data(idx_t Index, TPointer Value);

  int_type left() const;
  int_type top() const;
  int_type width() const;
  int_type height() const;
  void left(int_type Value);
  void top(int_type Value);
  void width(int_type Value);
  void height(int_type Value);
  void canvas(TCanvas *Value);

  bool IntoArea(point_float_type P) const;
  bool OutArea(point_float_type P1, point_float_type P2) const;
  bool ClipLine(point_float_type &P1, point_float_type &P2) const;
  float_type DefFunc(float_type x) const;
  float_type Func(idx_t i, float_type x) const;
  float_type FuncX(float_type x);
  float_type FuncY(float_type x);
  point_int_type ConvCoor(point_float_type P);
  point_float_type XYFunc(float_type t) const;
  void FloorAxis();
  rect_int_type GetBoundsRect() const;
  //rect_float_type GetArea();
  TCompConv GetCompConv(idx_t Index) const;
  void PaintMarkerX(size_type i);
  void PaintMarkerY(size_type i);
  //void SetArea(rect_float_type Area);
  void SetAutoScale(idx_t Index, bool Value);
  //void SetBounds(bounds_float_type Bounds);
  void SetBoundsRect(rect_int_type Value);
  void SetCompConv(idx_t Index, TCompConv Value);
  void SetShift(size_type Index, float_type Value);
  void SetScale(size_type Index, float_type Value);
  bool ValidRect() const;
  void Paint(paint_style_t a_paint_style);
  void DoAutoScale();
  void DoCalculate();
  void DoPaint();
  void DoBlackPrint();
  void DoColorPrint();
};

class mx_ext_chart_t: public mx_ext_chart_types
{
public:

  // Поля
  mx_ext_chart_select_t *Select;
  // Свойства
  __property size_type BaseItem = {read=FBaseItem, write=SetBaseItem};
  __property bool ShowBounds = {read=FShowBounds, write=SetShowBounds};
  __property bool ShowGrid = {read=FShowGrid, write=SetShowGrid};
  __property bool ShowAuxGrid = {read=FShowAuxGrid, write=SetShowAuxGrid};
  __property rect_float_type Area = {read=GetArea, write=SetArea};
  __property bool AutoScaleX = {read=FAutoScaleX, write=SetAutoScaleX};
  __property bool AutoScaleY = {read=FAutoScaleY, write=SetAutoScaleY};
  __property TCanvas *Canvas = {read=FCanvas};
  __property TCanvas *PaintCanvas = {read=FPaintCanvas, write=SetPaintCanvas};
  __property TCanvas *PrintCanvas = {read=FPrintCanvas, write=FPrintCanvas};
  __property size_type CountMarkerX = {index=0, read=IndexMarkerX};
  __property size_type CountMarkerY = {index=1, read=IndexMarkerY};
  __property float_type MarkerX[size_type Index] = {read=GetMarkerX, write=SetMarkerX};
  __property float_type MarkerY[size_type Index] = {read=GetMarkerY, write=SetMarkerY};
  __property float_type FirstMarkerX = {index=0, read=GetMarkerX, write=SetMarkerX};
  __property float_type FirstMarkerY = {index=1, read=GetMarkerY, write=SetMarkerY};
  __property float_type LastMarkerX = {index=-1, read=GetMarkerX, write=SetMarkerX};
  __property float_type LastMarkerY = {index=-1, read=GetMarkerY, write=SetMarkerY};
  __property size_type Count = {read=FCount};
  __property mx_ext_chart_item_t *Items[size_type Index] = {read=GetItem};
  __property int_type Left = {read=GetLeft, write=SetLeft};
  __property int_type Top = {read=GetTop, write=SetTop};
  __property int_type Width = {read=GetWidth, write=SetWidth};
  __property int_type Height = {read=GetHeight, write=SetHeight};
  __property rect_int_type BoundsRect = {read=FBoundsRect, write=SetBoundsRect};
  __property rect_int_type PrintRect = {read=FPrintRect, write=SetPrintRect};
  __property rect_int_type GridRect = {read=GetGridRect};
  __property TPen *MarkerPen = {read=FMarkerPen, write=SetMarkerPen};
  __property TPen *AuxGridPen = {read=FAuxGridPen, write=SetAuxGridPen};
  __property float_type MinWidthGrid = {read=FMinWidthGrid, write=SetMinWidthGrid};
  __property float_type MinHeightGrid = {read=FMinHeightGrid, write=SetMinHeightGrid};
  __property float_type MinWidthAuxGrid = {read=FMinWidthAuxGrid, write=SetMinWidthAuxGrid};
  __property float_type MinHeightAuxGrid = {read=FMinHeightAuxGrid, write=SetMinHeightAuxGrid};
  __property mx_ext_chart_item_t *First = {index=0, read=GetItem};
  __property mx_ext_chart_item_t *Last = {index=-1, read=GetItem};
  // События
  __property TNotifyEvent OnPaint = {read=FOnPaint, write=FOnPaint};
  __property TNotifyEvent OnPrint = {read=FOnPrint, write=FOnPrint};
  __property TChartChange OnChange = {read=FOnChange, write=FOnChange};
  __property TChartError OnError = {read=FOnError, write=FOnError};
  // Конструкторы и деструкторы
  mx_ext_chart_t(TComponent *AOwner);
  mx_ext_chart_t(TComponent *AOwner, TCanvas *ACanvas);
  virtual ~mx_ext_chart_t();
  // Методы
  void Add();
  void Add(TPointer Data, size_type Count);
  void Add(TPointer Data, size_type Count, TColor AColor);
  void Delete(size_type Index);
  void Clear();
  void Insert(size_type Index);
  void Insert(size_type Index, TPointer Data, size_type Count);
  void Insert(size_type Index, TPointer Data, size_type Count, TColor AColor);
  void NewGroupX();
  void NewGroupY();
  void Paint();
  void BlackPrint();
  void ColorPrint();
  void SaveToMetafile(String FileName);
  void PreCalc();
  void AddMarkerX(float_type Value);
  void AddMarkerY(float_type Value);
  void ClearMarkerX();
  void ClearMarkerY();
  void DeleteMarkerX(size_type Index);
  void DeleteMarkerY(size_type Index);
  void DeleteMarkerX();
  void DeleteMarkerY();
  void DeleteMarkerX(size_type Begin, size_type End);
  void DeleteMarkerY(size_type Begin, size_type End);
  void Invalidate();

private:
  friend class mx_ext_chart_item_t;
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
  size_type IndexMarkerX, IndexMarkerY;
  float_type *FMarkerX, *FMarkerY;
  bool FShowGrid;
  bool FShowAuxGrid;
  bool FShowBounds;
  size_type FCount;
  mx_ext_chart_item_t **FItems;
  TCanvas *FCanvas;
  TCanvas *FPaintCanvas;
  TCanvas *FPrintCanvas;
  TCanvas *FCurCanvas;
  TPen *FMarkerPen;
  TPen *FAuxGridPen;
  rect_int_type FBoundsRect;
  rect_int_type FGridRect;
  rect_int_type FPrintRect;
  rect_int_type FCurRect;
  bool FAutoScaleX, FAutoScaleY;
  rect_float_type FArea;
  float_type FMinWidthGrid, FMinHeightGrid;
  float_type FMinWidthAuxGrid, FMinHeightAuxGrid;
  bool PaintMode;
  float_type SW, SH;
  bool ApplyTextX, ApplyTextY;
  TNotifyEvent PenChanged;
  TNotifyEvent BrushChanged;
  TNotifyEvent FontChanged;
  size_type FBaseItem;
  size_type CurGroupX, CurGroupY;
  // Методы
  void PaintGrid();
  void PaintGrid(bool BlackAuxGrid);
  void CalcGrid();
  void PaintBounds() const;
  rect_float_type GetArea();
  rect_int_type GetGridRect();
  int_type GetLeft() const;
  int_type GetTop() const;
  int_type GetWidth() const;
  int_type GetHeight() const;
  float_type GetMarkerX(size_type Index) const;
  float_type GetMarkerY(size_type Index) const;
  mx_ext_chart_item_t *GetItem(size_type Index) const;
  void SetChildRect();
  void SetArea(rect_float_type Area);
  void SetPrintRect(rect_int_type Value);
  void SetLeft(int_type Value);
  void SetTop(int_type Value);
  void SetWidth(int_type Value);
  void SetHeight(int_type Value);
  void SetBoundsRect(rect_int_type Value);
  void SetShowBounds(bool Value);
  void SetShowGrid(bool Value);
  void SetShowAuxGrid(bool Value);
  void SetAutoScaleX(bool Value);
  void SetAutoScaleY(bool Value);
  void SetPaintCanvas(TCanvas *Value);
  void SetMarkerX(size_type Index, float_type Value);
  void SetMarkerY(size_type Index, float_type Value);
  void SetCurCanvas(TCanvas *Value);
  void SetMarkerPen(TPen *Value);
  void SetAuxGridPen(TPen *Value);
  void SetMinWidthGrid(float_type Value);
  void SetMinHeightGrid(float_type Value);
  void SetMinWidthAuxGrid(float_type Value);
  void SetMinHeightAuxGrid(float_type Value);
  void DoAutoScale();
  void DoCalculate();
  void DoRepaint(paint_style_t a_paint_style);
  point_int_type ConvCoor(point_float_type P) const;
  int_type ConvCoorX(float_type b, float_type e, float_type x) const;
  int_type ConvCoorY(float_type b, float_type e, float_type y) const;
  void PaintMarkerX(size_type i);
  void PaintMarkerY(size_type i);
  void ChildChange(TObject *Sender,
    TChartChangeType ChartChangeType);
  void ChildError(TObject *Sender, Exception &e, point_float_type P,
    float_type t, TChartErrorType ChartErrorType) const;
  void Constructor();
  float_type StepCalc(float_type Begin, float_type End, size_type Size,
    size_type MinSize, bool IsWidth, bool AccountText);
  void GraphObjChanged(TObject *Sender);
  String SFF(float_type x, float_type Step) const;
  bool ValidRect() const;
  void PaintChildMarkerX(mx_ext_chart_item_t *Sender, size_type i) const;
  void PaintChildMarkerY(mx_ext_chart_item_t *Sender, size_type i) const;
  void SetBaseItem(size_type AChart);
  // Свойства
  __property TCanvas *CurCanvas = {read=FCurCanvas, write=SetCurCanvas};
};

class mx_ext_chart_select_t: public mx_ext_chart_types
{
public:
  __property mx_ext_chart_t *Chart = {read=FChart, write=FChart};
  __property TControl *Control = {read=FControl, write=SetControl};
  __property TNotifyEvent OnError = {read=FOnError, write=FOnError};
  __property float_type ZoomFactor = {read=FZoomFactor, write=SetZoomFactor};
  __property bool PositioningMode = {read=FPositioningMode, write=SetPositioningMode};
  __property bool DragChartMode = {read=FDragChartMode, write=SetDragChartMode};
  void All();
  void Shift(float_type X, float_type Y = 0);
  void Positioning(float_type X, float_type Y = 0);
  void ZoomIn(float_type FX = -1, float_type FY = 1);
  void ZoomOut();
  void Connect();
  void Disconnect();
  void ItemChange(item_change_t Oper, size_type Index);
  mx_ext_chart_select_t(): TObject() {}
  mx_ext_chart_select_t(mx_ext_chart_t *AChart);
  mx_ext_chart_select_t(mx_ext_chart_t *AChart, TControl *AControl);
  ~mx_ext_chart_select_t();

private:
  typedef pair<bool, bool> pairbool;
  typedef map<TComponent*, pairbool> mapscale;
  typedef pair<TCompConv, TCompConv> pairconv;
  typedef map<TComponent*, pairconv> mapconv;
  typedef mapconv::iterator mapconvit;
  typedef map<int*, rect_float_type> maparea;

  mx_ext_chart_t *FChart;
  TControl *FControl;
  TNotifyEvent FOnError;
  mapscale AutoScales;
  maparea MAreas;
  bool LockEvent;
  bool ValidAutoScales;
  bool ValidAreas;
  vector<rect_float_type> Areas;
  vector<size_type> BaseItems;
  vector<mapconv> CompConvs;
  //rect_float_type CurArea;
  int_type Xn, Yn;
  float_type PX, PY;
  rect_int_type SelRect;
  bool SelectMode, FirstSelect;
  float_type FZoomFactor;
  bool FPositioningMode;
  bool FDragChartMode;

  void DoMouseDown(TObject *Sender, TMouseButton Button,
    TShiftState Shift, int X, int Y);
  void DoMouseMove(TObject *Sender, TShiftState Shift, int X,
    int Y);
  void DoMouseUp(TObject *Sender, TMouseButton Button,
    TShiftState Shift, int X, int Y);
  void DoDblClick(TObject *Sender);
  void DoPaint();
  TControl *GetControl();
  void RestoreAutoScales();
  void SaveAutoScales();
  void RestoreCompConvs();
  void SaveCompConvs();
  void SelectClip(rect_int_type &ARect) const;
  void SelectRect(rect_int_type Rect) const;
  void SetControl(TControl *AControl);
  void SetZoomFactor(float_type Value);
  void SetPositioningMode(bool Value);
  void SetDragChartMode(bool Value);
  void SaveAreas();
  void RestoreAreas();
};

//! @}


#ifdef NOP
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

// Окно с графиком для C++ Builder
class builder_chart_window_t: public irs::chart_window_t
{
public:
  typedef irs::string_t string_type;
  enum stay_on_top_t { stay_on_top_on, stay_on_top_off };

  builder_chart_window_t(irs_u32 a_size = 1000,
    irs_i32 a_refresh_time_ms = 1000,
    stay_on_top_t a_stay_on_top = stay_on_top_on);
  virtual void show();
  virtual void hide();
  virtual rect_t position() const;
  virtual void set_position(const rect_t &a_position);
  virtual void add_param(const string_type &a_name);
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
private:
  class chart_func_t;
  struct chart_point_t;

  typedef deque<double> chart_vec_t;
  typedef map<string_type, chart_point_t> data_t;

  class chart_func_t: public TObject
  {
  public:
    explicit chart_func_t(const chart_vec_t &a_data);
    double fn(double a_index);
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
    const chart_point_t& vec();
    size_type size();
  private:
    const data_t* mp_data;
    map<int, string_type> m_unsort_data;
    map<int, string_type>::iterator mp_unsort_data_it;
    chart_point_t m_bad_point;
  };
  // Форма с графиком
  class TChartForm: public TForm
  {
  public:
    typedef irs::string_t string_type;
    TChartForm(const data_t &a_data, chart_event_t &a_event,
      irs_i32 a_refresh_time_ms, stay_on_top_t a_stay_on_top);
    virtual ~TChartForm();
    inline irs_bool fix();
    inline void group_all();
    inline void ungroup_all();
    inline void invalidate();
    void chart_list_changed();
    void set_refresh_time_ms(irs_i32 a_refresh_time_ms);
  private:
    irs_bool m_group_all;
    //chart_point_t m_pause_time;
    data_t m_pause_data;
    irs_bool m_pause;
    irs_bool m_fix;
    //const chart_point_t &m_time;
    const data_t &m_data;
    chart_event_t &m_event;
    TIdleEvent m_IdleEvent;
    TPanel* mp_panel;
    TPaintBox* mp_chart_box;
    mx_ext_chart_t* mp_chart;
    auto_ptr<mx_ext_chart_select_t> mp_select;
    TTimer* mp_timer;
    TButton* mp_pause_btn;
    TLabel* mp_dbg_info;
    TButton* mp_fix_btn;
    TButton* mp_clear_btn;
    TComboBox* mp_base_chart_combo;
    const char *mp_pause_on_text;
    const char *mp_pause_off_text;
    const char *mp_fix_on_text;
    const char *mp_fix_off_text;
    irs_i32 m_refresh_time_ms;
    irs_bool m_invalidate;
    mx_time_int_local_t m_time_int;
    const data_t* mp_data;
    //mx_ext_chart_t *mp_chart;
    int m_base_item;
    color_gen_t m_colors;
    bool m_is_lock;
    bool m_is_chart_list_changed;
    string_type m_base_chart_name;
    unsort_data_t m_unsort_data;

    void FormResize(TObject *Sender);
    void TimerEvent(TObject *Sender);
    void PauseBtnClick(TObject *Sender);
    void FixBtnClick(TObject *Sender);
    void ClearBtnClick(TObject *Sender);
    void ChartPaint(TObject *Sender);
    void FormShow(TObject *Sender);
    void FormHide(TObject *Sender);
    void BaseChartComboChange(TObject *Sender);
    void connect_data(const data_t &a_data);
    void update_chart_combo();
    void set_base_item(int a_base_item);
    int chart_from_combo_item(int a_combo_item);
    //void connect_data(mx_ext_chart_t *ap_chart, const data_t &a_data);
  }; //class TChartForm

  //chart_point_t m_time;
  data_t m_data;
  irs_u32 m_size;
  chart_event_t m_event;
  auto_ptr<TChartForm> mp_form;
  rect_t m_position;
  stay_on_top_t m_stay_on_top;
  irs_i32 m_chart_index;

  irs_u32 cur_size() const;
}; //class builder_chart_window_t

//! @}

} //namespace chart
#endif //NOP

} //namespace irs

#endif //defined(QT_CORE_LIB) || defined(__BORLANDC__)
#endif //IRS_FULL_STDCPPLIB_SUPPORT
#endif //MXEXT_OFF

#endif //MXEXTCHARTH
