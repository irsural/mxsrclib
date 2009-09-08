// Функции для работы с компонентами C++Builder
// Дата: 8.09.2009

#include <vcl.h>
#pragma hdrstop

#include <cbsysutils.h>


// Запрос версии файла
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
  // размер информации о версии файла
  DWORD info_size =
    GetFileVersionInfoSize(file_name, NULL);
  if(info_size == 0) {
    fsuccess = false;
  }
  if (fsuccess) {
    // буфер для информации о версии файла
    pBlock = new char[info_size];
    memcpy(file_name, a_file_name.c_str(), file_name_size);
    GetFileVersionInfo(file_name, IRS_NULL, info_size, pBlock);


    if (!VerQueryValue(pBlock, "\\",&MS,&len)) {
      fsuccess = false;
    }
  }
  if(fsuccess) {
    // структура с информацией о версии файла
    VS_FIXEDFILEINFO FixedFileInfo;
    // приводим информацию к структуре
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

// Перевод структуры о версии файла в строку   
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
        AnsiString CellValueStr = CellValue;
          //static_cast<AnsiString>(CellValue.AsType(varString));
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
    IRS_LIB_ASSERT_MSG("Системная ошибка");
  }
  if (!WorkBook.IsEmpty()) {
    WorkBook.OleFunction("Close");
  } else {
    // Объект не инициализирован
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
      // Создаем новую книгу
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
    // Область первой ячейки
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
    IRS_LIB_ASSERT_MSG("Системная ошибка");
  }
  if (!WorkBook.IsEmpty()) {
    if (file_create) {
      WorkBook.OleFunction("SaveAs", a_book_name.c_str());
    } else {
      WorkBook.OleFunction("Close", true);
    }
    Excel = Unassigned;
  } else {
    // Объект не инициализирован
  }
}

