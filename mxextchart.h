//! \file
//! \ingroup graphical_user_interface_group
//! \brief ���������� �������� � C++ Builder
//!
//! ����: 30.04.2012\n
//! ���� ��������: 30.04.2012

#ifndef MXEXTCHARTH
#define MXEXTCHARTH

#include <irsdefs.h>

#include <mxextbase.h>
#include <limits> // ����������� � IAR

#include <irsstd.h>
#include <irscpp.h>
#include <irschartwin.h>

#include <irsfinal.h>

#ifdef NOP

#define CHARTRANGE(Chart) BeginChartRange(Chart, 0), EndChartRange(Chart, 0)
#define CHARTRANGEL(Chart) BeginChartRange(Chart, -1), EndChartRange(Chart, -1)
#define CHARTRANGEI(Chart, Index) BeginChartRange(Chart, Index),\
  EndChartRange(Chart, Index)

namespace irs {

//! \addtogroup graphical_user_interface_group
//! @{


class mx_ext_chart_t;
class mx_ext_chart_select_t;
class mx_ext_chart_item_t;

int BeginChartRange(mx_ext_chart_t *Chart, int Index);
int EndChartRange(mx_ext_chart_t *Chart, int Index);


enum TChartChangeType {cctArea, cctBounds, cctBoundsRect, cctCanvas, cctHide,
  cctAutoScale, cctGroup, cctShift, cctScale, cctStep, cctCompConv,
  cctSizeGrid, cctSizeAuxGrid, cctData, cctPrintRect, cctGraphObj, cctItems,
  cctMarker, cctBaseItem};
enum TChartErrorType {cetConvX, cetConvY, cetCalcX, cetCalcY};
#ifdef NOP
typedef void (__closure *TChartChange)(TObject *Sender,
  TChartChangeType ChartChangeType);
typedef void (__closure *TChartError)(TObject *Sender, Exception &e,
  TDblPoint P, double t, TChartErrorType ChartErrorType);
#endif //NOP

class mx_ext_chart_item_t: public TComponent
{
  friend class mx_ext_chart_t;
  // ����
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
  TChartChange FOnChange;
  TChartError FOnError;
  int FGroup[2];
  double FShiftX, FShiftY;
  double FScaleX, FScaleY;
  TPen *FPen;
  TPen *FMarkerPen;
  // ��������
  generator_events_t ;
  __property int Left = {read=GetLeft, write=SetLeft};
  __property int Top = {read=GetTop, write=SetTop};
  __property int Width = {read=GetWidth, write=SetWidth};
  __property int Height = {read=GetHeight, write=SetHeight};
  __property TCanvas *Canvas = {read=FCanvas, write=SetCanvas};
  __property TRect BoundsRect = {read=GetBoundsRect, write=SetBoundsRect};
  // �������
  __property TChartChange OnChange = {read=FOnChange, write=FOnChange};
  __property TChartError OnError = {read=FOnError, write=FOnError};
  // ������
  bool IntoArea(TDblPoint P) const;
  bool OutArea(TDblPoint P1, TDblPoint P2) const;
  bool ClipLine(TDblPoint &P1, TDblPoint &P2) const;
  double DefFunc(double x) const;
  double Func(int i, double x) const;
  double FuncX(double x);
  double FuncY(double x);
  TPoint ConvCoor(TDblPoint P);
  TDblPoint XYFunc(double t) const;
  void FloorAxis();
  TRect GetBoundsRect() const;
  TDblRect GetArea();
  int GetLeft() const;
  int GetTop() const;
  int GetWidth() const;
  int GetHeight() const;
  double GetMarkerX(int Index) const;
  double GetMarkerY(int Index) const;
  TCompConv GetCompConv(int Index) const;
  void PaintMarkerX(int i);
  void PaintMarkerY(int i);
  void SetArea(TDblRect Area);
  void SetAutoScale(int Index, bool Value);
  void SetBounds(TDblBounds Bounds);
  void SetBoundsRect(TRect Value);
  void SetCompConv(int Index, TCompConv Value);
  void SetData(int Index, TPointer Value);
  void SetStep(double Step);
  void SetLeft(int Value);
  void SetTop(int Value);
  void SetWidth(int Value);
  void SetHeight(int Value);
  void SetMarkerX(int Index, double Value);
  void SetMarkerY(int Index, double Value);
  void SetCanvas(TCanvas *Value);
  void SetHide(bool Value);
  void SetShift(int Index, double Value);
  void SetScale(int Index, double Value);
  void SetPen(TPen *Value);
  void SetMarkerPen(TPen *Value);
  bool ValidRect() const;
  void Paint(int Tag);
  void DoAutoScale();
  void DoCalculate();
  void DoPaint();
  void DoBlackPrint();
  void DoColorPrint();
  void SetGroup(int Index, int AGroup);
public:
  // ��������
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
  // ������������ � �����������
  mx_ext_chart_item_t(TComponent *AOwner);
  virtual ~mx_ext_chart_item_t();
  // ������
  void AddMarkerX(double Value);
  void AddMarkerY(double Value);
  void ClearMarkerX();
  void ClearMarkerY();
  void DeleteMarkerX(int Index);
  void DeleteMarkerY(int Index);
  void DeleteMarkerX();
  void DeleteMarkerY();
  void DeleteMarkerX(int Begin, int End);
  void DeleteMarkerY(int Begin, int End);
  void Invalidate();
};

class mx_ext_chart_t: public TComponent
{
  friend class mx_ext_chart_item_t;
  // ����
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
  mx_ext_chart_item_t **FItems;
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
  // ������
  void PaintGrid();
  void PaintGrid(bool BlackAuxGrid);
  void CalcGrid();
  void PaintBounds() const;
  TDblRect GetArea();
  TRect GetGridRect();
  int GetLeft() const;
  int GetTop() const;
  int GetWidth() const;
  int GetHeight() const;
  double GetMarkerX(int Index) const;
  double GetMarkerY(int Index) const;
  mx_ext_chart_item_t *GetItem(int Index) const;
  void SetChildRect();
  void SetArea(TDblRect Area);
  void SetPrintRect(TRect Value);
  void SetLeft(int Value);
  void SetTop(int Value);
  void SetWidth(int Value);
  void SetHeight(int Value);
  void SetBoundsRect(TRect Value);
  void SetShowBounds(bool Value);
  void SetShowGrid(bool Value);
  void SetShowAuxGrid(bool Value);
  void SetAutoScaleX(bool Value);
  void SetAutoScaleY(bool Value);
  void SetPaintCanvas(TCanvas *Value);
  void SetMarkerX(int Index, double Value);
  void SetMarkerY(int Index, double Value);
  void SetCurCanvas(TCanvas *Value);
  void SetMarkerPen(TPen *Value);
  void SetAuxGridPen(TPen *Value);
  void SetMinWidthGrid(double Value);
  void SetMinHeightGrid(double Value);
  void SetMinWidthAuxGrid(double Value);
  void SetMinHeightAuxGrid(double Value);
  void DoAutoScale();
  void DoCalculate();
  void DoRepaint(int Tag);
  TPoint ConvCoor(TDblPoint P) const;
  int ConvCoorX(double b, double e, double x) const;
  int ConvCoorY(double b, double e, double y) const;
  void PaintMarkerX(int i);
  void PaintMarkerY(int i);
  void ChildChange(TObject *Sender,
    TChartChangeType ChartChangeType);
  void ChildError(TObject *Sender, Exception &e, TDblPoint P,
    double t, TChartErrorType ChartErrorType) const;
  void Constructor();
  double StepCalc(double Begin, double End, int Size,
    int MinSize, bool IsWidth, bool AccountText);
  void GraphObjChanged(TObject *Sender);
  String SFF(double x, double Step) const;
  bool ValidRect() const;
  void PaintChildMarkerX(mx_ext_chart_item_t *Sender, int i) const;
  void PaintChildMarkerY(mx_ext_chart_item_t *Sender, int i) const;
  void SetBaseItem(int AChart);
  // ��������
  __property TCanvas *CurCanvas = {read=FCurCanvas, write=SetCurCanvas};
public:
  // ����
  mx_ext_chart_select_t *Select;
  // ��������
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
  __property mx_ext_chart_item_t *Items[int Index] = {read=GetItem};
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
  __property mx_ext_chart_item_t *First = {index=0, read=GetItem};
  __property mx_ext_chart_item_t *Last = {index=-1, read=GetItem};
  // �������
  __property TNotifyEvent OnPaint = {read=FOnPaint, write=FOnPaint};
  __property TNotifyEvent OnPrint = {read=FOnPrint, write=FOnPrint};
  __property TChartChange OnChange = {read=FOnChange, write=FOnChange};
  __property TChartError OnError = {read=FOnError, write=FOnError};
  // ������������ � �����������
  mx_ext_chart_t(TComponent *AOwner);
  mx_ext_chart_t(TComponent *AOwner, TCanvas *ACanvas);
  virtual ~mx_ext_chart_t();
  // ������
  void Add();
  void Add(TPointer Data, int Count);
  void Add(TPointer Data, int Count, TColor AColor);
  void Delete(int Index);
  void Clear();
  void Insert(int Index);
  void Insert(int Index, TPointer Data, int Count);
  void Insert(int Index, TPointer Data, int Count, TColor AColor);
  void NewGroupX();
  void NewGroupY();
  void Paint();
  void BlackPrint();
  void ColorPrint();
  void SaveToMetafile(String FileName);
  void PreCalc();
  void AddMarkerX(double Value);
  void AddMarkerY(double Value);
  void ClearMarkerX();
  void ClearMarkerY();
  void DeleteMarkerX(int Index);
  void DeleteMarkerY(int Index);
  void DeleteMarkerX();
  void DeleteMarkerY();
  void DeleteMarkerX(int Begin, int End);
  void DeleteMarkerY(int Begin, int End);
  void Invalidate();
};

class mx_ext_chart_select_t: public TObject
{
  typedef pair<bool, bool> pairbool;
  typedef map<TComponent *, pairbool> mapscale;
  typedef pair<TCompConv, TCompConv> pairconv;
  typedef map<TComponent *, pairconv> mapconv;
  typedef mapconv::iterator mapconvit;
  typedef map<int *, TDblRect> maparea;

  mx_ext_chart_t *FChart;
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
  void SelectClip(TRect &ARect) const;
  void SelectRect(TRect Rect) const;
  void SetControl(TControl *AControl);
  void SetZoomFactor(double Value);
  void SetPositioningMode(bool Value);
  void SetDragChartMode(bool Value);
  void SaveAreas();
  void RestoreAreas();
public:
  __property mx_ext_chart_t *Chart = {read=FChart, write=FChart};
  __property TControl *Control = {read=FControl, write=SetControl};
  __property TNotifyEvent OnError = {read=FOnError, write=FOnError};
  __property double ZoomFactor = {read=FZoomFactor, write=SetZoomFactor};
  __property bool PositioningMode = {read=FPositioningMode, write=SetPositioningMode};
  __property bool DragChartMode = {read=FDragChartMode, write=SetDragChartMode};
  void All();
  void Shift(double X, double Y = 0);
  void Positioning(double X, double Y = 0);
  void ZoomIn(double FX = -1, double FY = 1);
  void ZoomOut();
  void Connect();
  void Disconnect();
  void ItemChange(int Oper, int Index);
  mx_ext_chart_select_t(): TObject() {}
  mx_ext_chart_select_t(mx_ext_chart_t *AChart);
  mx_ext_chart_select_t(mx_ext_chart_t *AChart, TControl *AControl);
  ~mx_ext_chart_select_t();
};

//! @}



namespace chart {

//! \addtogroup graphical_user_interface_group
//! @{

// ��������� ������
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

// ���� � �������� ��� C++ Builder
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
  // �������������� data_t � ������� ����������
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
  // ����� � ��������
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

} //namespace irs

#endif //NOP

#endif //MXEXTCHARTH
