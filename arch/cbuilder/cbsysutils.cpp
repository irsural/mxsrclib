// ������� ��� ������ � ������������ C++Builder
// ����: 14.04.2010
// ���� ��������: 3.12.2009

#include <irsdefs.h>

#include <vcl.h>
#pragma hdrstop

#include <cbsysutils.h>

#include <irsfinal.h>

// ������ ������ �����
bool irs::cbuilder::get_file_version(
  const irs::string& a_file_name, irs::cbuilder::file_version_t& a_version)
{
  bool fsuccess = true;
  const int file_name_size = a_file_name.size();
  char* file_name = new char[file_name_size+1];
  memcpy(file_name, a_file_name.c_str(), file_name_size+1);
  LPVOID MS = IRS_NULL;
  LPVOID pBlock = IRS_NULL;
  UINT len;
  // ������ ���������� � ������ �����
  DWORD info_size =
    GetFileVersionInfoSize(file_name, NULL);
  if(info_size == 0) {
    fsuccess = false;
  }
  if (fsuccess) {
    // ����� ��� ���������� � ������ �����
    pBlock = new char[info_size];
    memcpy(file_name, a_file_name.c_str(), file_name_size);
    GetFileVersionInfo(file_name, IRS_NULL, info_size, pBlock);


    if (!VerQueryValue(pBlock, "\\",&MS,&len)) {
      fsuccess = false;
    }
  }
  if(fsuccess) {
    // ��������� � ����������� � ������ �����
    VS_FIXEDFILEINFO FixedFileInfo;
    // �������� ���������� � ���������
    memmove(&FixedFileInfo, MS, len);

    DWORD FileVersionMS = FixedFileInfo.dwFileVersionMS;
    DWORD FileVersionLS = FixedFileInfo.dwFileVersionLS;

    a_version.major = HIWORD(FileVersionMS);
    a_version.minor = LOWORD(FileVersionMS);
    a_version.release = HIWORD(FileVersionLS);
    a_version.build = LOWORD(FileVersionLS);
  }

  delete[] pBlock;
  delete[] file_name;
  return fsuccess;
}

// ������� ��������� � ������ ����� � ������   
irs::string irs::cbuilder::file_version_to_str(
  const irs::cbuilder::file_version_t& a_file_version)
{
  irs::string file_version_str;
  file_version_str =
    static_cast<irs::string>(a_file_version.major) + "." +
    static_cast<irs::string>(a_file_version.minor) + "." +
    static_cast<irs::string>(a_file_version.release) + "." +
    static_cast<irs::string>(a_file_version.build);
  return file_version_str;
}

void irs::cbuilder::file_xls_table_read(const string_t& a_book_name,
  const string_t& a_sheet,
  const string_t& a_cell_first,
  const string_t& a_cell_last,
  table_string_t* ap_table_string,
  bool* ap_read_success)
{
  typedef sizens_t size_type;
  typedef string_t str_type;
  *ap_read_success = true;
  Variant WorkBook;
  try {
    ap_table_string->clear();
    Variant Excel = CreateOleObject("Excel.Application");
    Variant WorkBooks = Excel.OlePropertyGet("WorkBooks");
    WorkBooks.OleProcedure("Open", a_book_name.c_str());
    WorkBook = Excel.OlePropertyGet("ActiveWorkBook"); 
    Variant WorkSheets = WorkBook.OlePropertyGet("WorkSheets");
    Variant Sheet = WorkSheets.OlePropertyGet("Item", a_sheet.c_str());
    const str_type range_size = a_cell_first + ":" + a_cell_last;
    Variant Range = Sheet.OlePropertyGet("Range", range_size.c_str());
    Variant ColCount = Range.OlePropertyGet("Columns").OlePropertyGet("Count");
    const size_type col_count = ColCount.AsType(varInteger);
    Variant RowCount = Range.OlePropertyGet("Rows").OlePropertyGet("Count");
    const size_type row_count = RowCount.AsType(varInteger);
    Variant Cells = Range.OlePropertyGet("Cells");
    ap_table_string->resize(col_count, row_count);
    for (size_type col_i = 0; col_i < col_count; col_i++) {
      for (size_type row_i = 0; row_i < row_count; row_i++) {
        Variant Cell = Cells.OlePropertyGet("Item", row_i+1, col_i+1);
        Variant CellValue = Cell.OlePropertyGet("Value");
        AnsiString CellValueStr = static_cast<wchar_t*>(CellValue);
        ap_table_string->write_cell(col_i, row_i, CellValueStr.c_str());
      }
    }
  } catch (Exception& e) {
    ap_table_string->clear();
    *ap_read_success = false;
    IRS_LIB_ASSERT_MSG(static_cast<AnsiString>(e.Message).c_str());
  } catch (exception& e) {
    ap_table_string->clear();
    *ap_read_success = false;
    IRS_LIB_ASSERT_MSG(e.what());
  } catch (...) {
    ap_table_string->clear();
    *ap_read_success = false;
    IRS_LIB_ASSERT_MSG("��������� ������");
  }
  if (!WorkBook.IsEmpty()) {
    WorkBook.OleFunction("Close");
  } else {
    // ������ �� ���������������
  }
}

void irs::cbuilder::file_xls_table_write(const string_t& a_book_name,
  const string_t& a_sheet,
  const string_t& a_cell_first,
  const table_string_t& a_table_string,
  bool* ap_read_success)
{
  typedef sizens_t size_type;
  typedef string_t str_type;
  *ap_read_success = true;
  Variant Excel;
  Variant WorkBook;
  bool file_create = false;
  try {
    Excel = CreateOleObject("Excel.Application");
    Variant WorkBooks = Excel.OlePropertyGet("WorkBooks");
    Variant WorkSheets;
    Variant Sheet;
    if (FileExists(a_book_name.c_str())) {
      WorkBooks.OleProcedure("Open", a_book_name.c_str());
      WorkBook = Excel.OlePropertyGet("ActiveWorkBook");
      WorkSheets = WorkBook.OlePropertyGet("WorkSheets");
      Sheet = WorkSheets.OlePropertyGet("Item", a_sheet.c_str());
    } else {
      const size_type sheet_count = 1;
      // ������� ����� �����
      Excel.OlePropertySet("SheetsInNewWorkbook", sheet_count);
      WorkBooks.OleProcedure("Add");
      file_create = true;
      WorkBook = Excel.OlePropertyGet("ActiveWorkBook");
      AnsiString ShortBookNameAndExt = ExtractFileName(a_book_name.c_str());
      str_type short_book_name_and_ext = ShortBookNameAndExt.c_str();
      size_type pos = short_book_name_and_ext.rfind(".");
      const str_type short_book_name = short_book_name_and_ext.substr(0, pos);
      WorkSheets = WorkBook.OlePropertyGet("WorkSheets");
      Sheet = WorkSheets.OlePropertyGet("Item", 1);
      Sheet.OlePropertySet("Name", a_sheet.c_str());
    } 
    // ������� ������ ������
    Variant RangeFirstCell =
      Sheet.OlePropertyGet("Range", a_cell_first.c_str());
    const size_type col_count = a_table_string.get_col_count();
    const size_type row_count = a_table_string.get_row_count();
    Variant Range = RangeFirstCell.OlePropertyGet("Resize", row_count,
      col_count);
    Variant Cells = Range.OlePropertyGet("Cells");
    for (size_type col_i = 0; col_i < col_count; col_i++) {
      for (size_type row_i = 0; row_i < row_count; row_i++) {
        str_type cell_str;
        cell_str = a_table_string.read_cell(col_i, row_i);
        Variant Cell = Cells.OlePropertyGet("Item", row_i+1, col_i+1);
        Cell.OlePropertySet("Value", cell_str.c_str());
      }
    }
  } catch (Exception& e) {
    *ap_read_success = false;
    IRS_LIB_ASSERT_MSG(static_cast<AnsiString>(e.Message).c_str());
  } catch (exception& e) {
    *ap_read_success = false;
    IRS_LIB_ASSERT_MSG(e.what());
  } catch (...) {
    *ap_read_success = false;
    IRS_LIB_ASSERT_MSG("��������� ������");
  }
  if (!WorkBook.IsEmpty()) {
    if (file_create) {
      WorkBook.OleFunction("SaveAs", a_book_name.c_str());
    } else {
      WorkBook.OleFunction("Close", true);
    }
    Excel = Unassigned;
  } else {
    // ������ �� ���������������
  }
}

void irs::cbuilder::string_grid_to_table_string(
  const TStringGrid& a_string_grid,
  irs::table_string_t* ap_table_string)
{
  IRS_LIB_ASSERT(ap_table_string != IRS_NULL);
  rect_t string_grid_rect(0, 0, a_string_grid.ColCount, a_string_grid.RowCount);
	point_t table_string_point(0, 0);
  string_grid_to_table_string(string_grid_rect, a_string_grid,
		table_string_point, ap_table_string);
}

void irs::cbuilder::string_grid_to_table_string(
	const rect_t& a_string_grid_rect,
	const TStringGrid& a_string_grid,
	const point_t& a_table_string_point,
	irs::table_string_t* ap_table_string)
{
	IRS_LIB_ASSERT(ap_table_string != IRS_NULL);
  IRS_LIB_ASSERT(a_string_grid_rect.left <= a_string_grid_rect.right);
	IRS_LIB_ASSERT(a_string_grid_rect.right <
    static_cast<sizens_t>(a_string_grid.ColCount));
	IRS_LIB_ASSERT(a_string_grid_rect.bottom <
	static_cast<sizens_t>(a_string_grid.RowCount));  	 
	const sizens_t string_grid_col_count = min(
		static_cast<sizens_t>(a_string_grid.ColCount), a_string_grid_rect.right);
	const sizens_t string_grid_row_count = min(
		static_cast<sizens_t>(a_string_grid.RowCount), a_string_grid_rect.bottom);
	const sizens_t table_string_last_col = a_table_string_point.left +
		a_string_grid_rect.width();
	const sizens_t table_string_last_row = a_table_string_point.top +
		a_string_grid_rect.height();
	if (table_string_last_col >= ap_table_string->get_col_count())
	{
		ap_table_string->set_col_count(table_string_last_col + 1);
	} else {
		// �������-�������� ����� ���������� ���������� ��������
	}
	if (table_string_last_row >= ap_table_string->get_row_count())
	{
		ap_table_string->set_row_count(table_string_last_row + 1);
	} else {
		// �������-�������� ����� ���������� ���������� �����
	}
	sizens_t table_string_col_i = a_table_string_point.left;
	string cell;
	for (sizens_t string_grid_col_i = a_string_grid_rect.left;
		string_grid_col_i < string_grid_col_count;
		string_grid_col_i++)
	{
		sizens_t table_string_row_i = a_table_string_point.top;
		for (sizens_t string_grid_row_i = a_string_grid_rect.left;
			string_grid_row_i < string_grid_row_count;
			string_grid_row_i++)
		{
			cell = const_cast<TStringGrid*>(&a_string_grid)->Cells
				[string_grid_col_i][string_grid_row_i].c_str();
			ap_table_string->write_cell(table_string_col_i, table_string_row_i, 
				cell);
			table_string_row_i++;
		}
		table_string_col_i++;
	} 
	
}

void irs::cbuilder::table_string_to_string_grid(
  const table_string_t& a_table_string,
  TStringGrid* ap_string_grid)
{
  IRS_LIB_ASSERT(ap_string_grid != IRS_NULL);
  rect_t table_string_rect(0, 0, a_table_string.get_col_count(),
    a_table_string.get_row_count());
  point_t a_string_grid_point(0, 0);
  table_string_to_string_grid(table_string_rect, a_table_string,
    a_string_grid_point, ap_string_grid);
}

void irs::cbuilder::table_string_to_string_grid(
  const rect_t& a_table_string_rect,
  const table_string_t& a_table_string,
  const point_t& a_string_grid_point,
  TStringGrid* ap_string_grid)
{ 
  IRS_LIB_ASSERT(ap_string_grid != IRS_NULL);
  IRS_LIB_ASSERT(a_table_string_rect.left <= a_table_string_rect.right);
  IRS_LIB_ASSERT(a_table_string_rect.top <= a_table_string_rect.bottom);
	IRS_LIB_ASSERT(a_table_string_rect.right <=
    static_cast<sizens_t>(a_table_string.get_col_count()));
  IRS_LIB_ASSERT(a_table_string_rect.bottom <=
		static_cast<sizens_t>(a_table_string.get_row_count()));   	
	const sizens_t table_string_col_count = min(
		a_table_string.get_col_count(), a_table_string_rect.right);
	const sizens_t table_string_row_count = min(
		a_table_string.get_row_count(), a_table_string_rect.bottom);
	const sizens_t string_grid_last_col = a_string_grid_point.left +
		a_table_string_rect.width();
	const sizens_t string_grid_last_row = a_string_grid_point.top +
    a_table_string_rect.height();
  if (string_grid_last_col >= static_cast<sizens_t>(ap_string_grid->ColCount))
  {
    ap_string_grid->ColCount = string_grid_last_col + 1;
  } else {
    // �������-�������� ����� ���������� ���������� ��������
  }
	if (string_grid_last_row >= static_cast<sizens_t>(ap_string_grid->RowCount))
  {
		ap_string_grid->RowCount = string_grid_last_row + 1;
  } else {
    // �������-�������� ����� ���������� ���������� �����
  }
  sizens_t string_grid_col_i = a_string_grid_point.left;
  AnsiString cell;
  for (sizens_t table_string_col_i = a_table_string_rect.left;
		table_string_col_i < table_string_col_count;
    table_string_col_i++)
  {
    sizens_t string_grid_row_i = a_string_grid_point.top;
    for (sizens_t table_string_row_i = a_table_string_rect.left;
			table_string_row_i < table_string_row_count;
      table_string_row_i++)
    {
      cell = a_table_string.read_cell(
        table_string_col_i, table_string_row_i).c_str();
      ap_string_grid->Cells[string_grid_col_i][string_grid_row_i] = cell;
      string_grid_row_i++;
    }
		string_grid_col_i++;
  }
}




