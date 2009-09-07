//---------------------------------------------------------------------------


#pragma hdrstop
#include "tablel.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//class string_grid_t

string_grid_t::string_grid_t(TStringGrid* ap_table
):
  mp_table(ap_table)
{
  IRS_LIB_ASSERT_EX(
    ap_table != IRS_NULL, "Нулевой указатель недопустим.");
}

string_grid_t::size_type string_grid_t::get_col() const
{
  return mp_table->Col;
}

string_grid_t::size_type string_grid_t::get_row() const
{
  return mp_table->Row;
}

void string_grid_t::set_col(const size_type a_col)
{
  mp_table->Col = a_col;
}

void string_grid_t::set_row(const size_type a_row)
{
  mp_table->Row = a_row;
}

void string_grid_t::set_col_count(const size_type a_col_count)
{
  mp_table->ColCount = a_col_count;
}

void string_grid_t::set_row_count(const size_type a_row_count)
{
  mp_table->RowCount = a_row_count;
}

string_grid_t::size_type string_grid_t::get_col_count() const
{
  return mp_table->ColCount;
}

string_grid_t::size_type string_grid_t::get_row_count() const
{ 
  return mp_table->RowCount;
}

void string_grid_t::read_cell(
  const size_type a_col, const size_type a_row, irs::string* ap_cell) const
{
  *ap_cell = mp_table->Cells[a_col][a_row].c_str();
}

void string_grid_t::write_cell(
  const size_type a_col, const size_type a_row, const irs::string& a_cell)
{
  mp_table->Cells[a_col][a_row] = a_cell.c_str();
}


#ifdef NOP
// class table_string_grid_t
string_grid_t::string_grid_t(TStringGrid* ap_table
):
  mp_table(ap_table),
  m_col_count(ap_table->ColCount),
  m_row_count(ap_table->RowCount),
  m_min_col_count(1),
  m_min_row_count(1),
  mp_object(new object_t)
{
}

void string_grid_t::add_ev_out_val_cell(mxfact_event_t *ap_event)
{
  mp_object->add_ev_out_val_cell(ap_event);
}

void string_grid_t::add_ev_out_short_val_cell(mxfact_event_t *ap_event)
{
  mp_object->add_ev_out_short_val_cell(ap_event);
}

void string_grid_t::add_ev_in_val_cell(mxfact_event_t *ap_event)
{
  mp_object->add_ev_in_val_cell(ap_event);
}

void string_grid_t::set_col_count(const int a_col_count)
{
  if (a_col_count >= m_min_col_count){
    mp_table->ColCount = a_col_count;
  } else {
    mp_table->ColCount = m_min_col_count;
  }
  m_col_count = a_col_count;
}

void string_grid_t::set_row_count(const int a_row_count)
{
  if (a_row_count >= m_min_row_count){
    mp_table->RowCount = a_row_count;
  } else {
    for (int y = 0; y < m_min_row_count; y++) {
      for (int x = 0, x_end = mp_table->ColCount; x < x_end; x++) {
        mp_table->Cells[x][y] = "";
      }
    }
    mp_table->RowCount = m_min_row_count;
  }
  m_row_count = a_row_count;
}

int string_grid_t::col_count()
{
  //int col_count = mp_table->ColCount;
  return m_col_count;
}

int string_grid_t::row_count()
{
  //int row_count = mp_table->RowCount;
  return m_row_count;
}

void string_grid_t::read_cell(
  irs::string& a_cell, const int a_col, const int a_row)
{
  AnsiString str = mp_table->Cells[a_col][a_row];
  a_cell = str.c_str();
}

void string_grid_t::write_cell(
  const irs::string& a_cell, const int a_col, const int a_row)
{
  mp_table->Cells[a_col][a_row] = a_cell.c_str();
}

void string_grid_t::add_event(
    const irs::string& a_event_name, mxfact_event_t *ap_event)
{
}

void string_grid_t::set_col(const int a_col)
{
  mp_table->Col = a_col;
}

void string_grid_t::set_row(const int a_row)
{
  mp_table->Row = a_row;
}

int string_grid_t::col()
{
  int col = mp_table->Col;
  return col;
}

int string_grid_t::row()
{
  int row = mp_table->Row;
  return row;
}

void string_grid_t::set_min_col_count(const int a_min_col_count)
{
  m_min_col_count = a_min_col_count;
}
void string_grid_t::set_min_row_count(const int a_min_row_count)
{
  m_min_row_count = a_min_row_count;
}

//class convert_type_t
cell_t convert_cell_t::tdisp_cell_to_tdata_cell(const irs::string& a_tdisp_cell)
{
  cell_t cell;
  if(a_tdisp_cell == ""){
    cell.init = false;
  }else{
    irs::string value_cell_str = a_tdisp_cell.c_str();
    bool success = value_cell_str.to_number(cell.value);
    if(success){

      cell.init = true;
    }else{
      cell.init = false;
    }
  }
  return cell;
}
irs::string convert_cell_t::tdata_cell_to_tdisp_cell(const cell_t& a_tdata_cell)
{
  irs::string cell;
  if(a_tdata_cell.init == true){
    cell = a_tdata_cell.value;
  }else{
    cell = "";
  }
  return cell;
}

//class short_val_cell_t
void short_val_cell_t::full_value_to_short_value(
  const cell_t& a_full_value, irs::string& a_short_value)
{
  AnsiString cell_tdisp;
  if(a_full_value.init == false){
    a_short_value = "";
  }else{
    a_short_value = a_full_value.value;
  }
}


#endif // NOP
