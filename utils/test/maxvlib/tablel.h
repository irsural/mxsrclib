//---------------------------------------------------------------------------

#ifndef tablelH
#define tablelH
#include <Grids.hpp>
#include "tableg.h"
#include <irserror.h>
//---------------------------------------------------------------------------
enum table_type_t {tt_string_grid};
class string_grid_t: public table_display_t
{
private:
  TStringGrid* mp_table;

public:
  string_grid_t(TStringGrid* ap_table);
  virtual size_type get_col() const;
  virtual size_type get_row() const;
  virtual void set_col(const size_type a_col);
  virtual void set_row(const size_type a_row);
  virtual void set_col_count(const size_type a_col_count);
  virtual void set_row_count(const size_type a_row_count);
  virtual size_type get_col_count() const;
  virtual size_type get_row_count() const;
  virtual void read_cell(
    const size_type a_col, const size_type a_row, irs::string* ap_cell) const;
  virtual void write_cell(
    const size_type a_col, const size_type a_row, const irs::string& a_cell);
};

#ifdef NOP
class string_grid_t:public table_display_t
{
private:
  class object_t: public TObject
  {
  private:
    mxfact_event_t *mp_event_out_value_cell;
    mxfact_event_t *mp_event_out_short_value_cell;
    mxfact_event_t *mp_event_in_value_cell;
    void __fastcall RawDataStringGridClick(TObject *Sender)
    {
      //mp_event->exec();
    }
  public:
    object_t(
    ):
      mp_event_out_value_cell(NULL),
      mp_event_out_short_value_cell(NULL),
      mp_event_in_value_cell(NULL)
    {
    }
    void add_ev_out_val_cell(mxfact_event_t *ap_event)
    {
      ap_event->connect(mp_event_out_value_cell);
    }
    void add_ev_out_short_val_cell(mxfact_event_t *ap_event)
    {
      ap_event->connect(mp_event_out_short_value_cell);
    }
    void add_ev_in_val_cell(mxfact_event_t *ap_event)
    {
      ap_event->connect(mp_event_in_value_cell);
    }
  };
  int m_col_count;
  int m_row_count;
  int m_min_col_count;
  int m_min_row_count;
  auto_ptr<object_t> mp_object;
  TStringGrid* mp_table;
  void add_ev_out_val_cell(mxfact_event_t *ap_event);
  void add_ev_out_short_val_cell(mxfact_event_t *ap_event);
  void add_ev_in_val_cell(mxfact_event_t *ap_event);
public:
  string_grid_t(TStringGrid* ap_table);
  virtual void set_col_count(const int a_col_count);
  virtual void set_row_count(const int a_row_count);
  virtual int col_count();
  virtual int row_count();
  virtual void read_cell(irs::string& a_cell, const int a_col, const int a_row);
  virtual void write_cell(
    const irs::string& a_cell, const int a_col, const int a_row);
  virtual void add_event(
    const irs::string& a_event_name, mxfact_event_t *ap_event);
  virtual void set_col(const int a_col);
  virtual void set_row(const int a_row);
  virtual int col();
  virtual int row();
  void set_min_col_count(const int a_min_col_count);
  void set_min_row_count(const int a_min_row_count);
};


class convert_cell_t:public converter_cell_t<cell_t, irs::string>
{
  virtual cell_t tdisp_cell_to_tdata_cell(const irs::string& a_disp_type);
  virtual irs::string tdata_cell_to_tdisp_cell(const cell_t& a_data_stype);
};

class short_val_cell_t:public short_value_cell_t<cell_t, irs::string>
{
public:
  virtual void full_value_to_short_value(
  const cell_t& a_full_value, irs::string& a_short_value);
};

#endif // NOP

#endif
