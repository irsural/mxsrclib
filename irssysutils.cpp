//! \file
//! \ingroup system_utils_group
//! \brief Утилиты ИРС
//!
//! Дата создания: 23.01.2013

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#ifdef __BORLANDC__
# include <cbsysutils.h>
#endif // __BORLANDC__

#if IRS_USE_BOOST

#if defined(__BORLANDC__)
#pragma warn -8092
#endif // defined(__BORLANDC__)

# include <boost/filesystem/path.hpp>
# include <boost/filesystem/operations.hpp>

#if defined(__BORLANDC__)
#pragma warn .8092
#endif // defined(__BORLANDC__)

#endif // IRS_USE_BOOST

#include <irssysutils.h>

#include <irsfinal.h>

#if defined(IRS_FULL_STDCPPLIB_SUPPORT) && defined(IRS_WIN32)
#if IRS_USE_SETUPAPI_WIN
bool irs::get_wave_format(irs::string_t a_file_name,
  wave_format_t* ap_wave_format)
{
  // дескриптор файла для открытия
  HMMIO hmmio = mmioOpen(const_cast<irs::char_t*>(a_file_name.c_str()), NULL,
    MMIO_READ | MMIO_ALLOCBUF);
  if (!hmmio) {
    return false;
  }
  // информация о родительском куске
  MMCKINFO mmckinfoParent;
  mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
  if (mmioDescend(hmmio, (LPMMCKINFO) &mmckinfoParent, NULL, MMIO_FINDRIFF)) {
    mmioClose(hmmio, 0);
    return false;
  }
  // структура с информацией о подкуске
  MMCKINFO mmckinfoSubchunk;
  mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
  if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, MMIO_FINDCHUNK)) {
    mmioClose(hmmio, 0);
    return false;
  }
  const int ftm_size = mmckinfoSubchunk.cksize;
  // стуктура содержащая формат
  vector<irs_u8> format_data(ftm_size, 0);
  //WAVEFORMATEX* Format = (WAVEFORMATEX *)malloc(ftm_size);
  WAVEFORMATEX* Format = reinterpret_cast<WAVEFORMATEX*>(&format_data.front());
  if (mmioRead(hmmio, (HPSTR)Format, ftm_size) != ftm_size) {
    mmioClose(hmmio, 0);
    return false;
  }
  ap_wave_format->format_tag = Format->wFormatTag;
  ap_wave_format->channels = Format->nChannels;
  ap_wave_format->bits = Format->wBitsPerSample;
  ap_wave_format->frecuency = Format->nSamplesPerSec;
  ap_wave_format->speed = Format->nAvgBytesPerSec;
  ap_wave_format->sample_size = ap_wave_format->channels*ap_wave_format->bits/8;
  mmioAscend(hmmio, &mmckinfoSubchunk, 0);
  mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
  if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, MMIO_FINDCHUNK)) {
    mmioClose(hmmio, 0);
    return false;
  }
  ap_wave_format->size = mmckinfoSubchunk.cksize;;
  ap_wave_format->pos = mmckinfoSubchunk.dwDataOffset;
  return true;
}

bool irs::get_pcm_wave_data(irs::string_t a_file_name,
  const wave_format_t& a_wave_format, std::size_t a_channel,
  std::vector<irs_i16>* ap_samples)
{
  if ((a_wave_format.format_tag != 1) || ((a_wave_format.bits != 8) &&
    (a_wave_format.bits != 16))) {
    return false;
  }
  const DWORD wave_size = a_wave_format.size/a_wave_format.sample_size;
  const DWORD data_size = wave_size*a_wave_format.sample_size;
  HMMIO hmmio = mmioOpen(const_cast<irs::char_t*>(a_file_name.c_str()), NULL,
    MMIO_READ | MMIO_ALLOCBUF);
  if (!hmmio) {
    return false;
  }
  ap_samples->resize(wave_size);
  const std::size_t buf_size = 256*1024;
  unsigned char buf[buf_size];
  const std::size_t frac = data_size%buf_size;
  const std::size_t count = data_size/buf_size;
  mmioSeek(hmmio, a_wave_format.pos, SEEK_SET);
  for (std::size_t i = 0; i <= count; i++) {
    const std::size_t part_size = (i != count) ? buf_size : frac;
    mmioRead(hmmio, (HPSTR)buf, part_size);
    const std::size_t sample_size = a_wave_format.sample_size;
    int j0 = a_channel*a_wave_format.bits/8;
    if (a_wave_format.bits == 8) {
      for (std::size_t j = j0; j < part_size; j += sample_size) {
        (*ap_samples)[(j + i*buf_size)/sample_size] =
          static_cast<irs_i16>(buf[j] - 128);
      }
    } else {
      for (std::size_t j = j0; j < part_size; j += sample_size) {
        (*ap_samples)[(j + i*buf_size)/sample_size] =
          static_cast<irs_i16>(buf[j] | (buf[j + 1] << 8));
      }
    }
  }

  mmioClose(hmmio, 0);
  return true;
}

namespace {

class hmmio_ptr
{
public:
  typedef irs::string_t string_type;
  hmmio_ptr(const string_type& a_file_name, LPMMIOINFO lpmmioinfo,
    DWORD dwOpenFlags
  ):
    hmmio(mmioOpen(const_cast<irs::char_t*>(a_file_name.c_str()), lpmmioinfo,
      dwOpenFlags))
  {
    if (!hmmio) {
      irs::stringstream_t msg;
      msg << irst("Ошибка открытия файла \"") << a_file_name << irst("\"");
      throw std::runtime_error(irs::str_conv<irs::irs_string_t>(msg.str()));
    }
  }
  ~hmmio_ptr()
  {
    if (hmmio) {   // Чтобы можно было копировать
      mmioClose(hmmio, 0);
    }
  }
  operator HMMIO()
  {
    return hmmio;
  }
private:
  HMMIO hmmio;
};

} // unnamed namespace

void irs::create_wave_pcm_16_mono_file(irs::string_t a_file_name,
  const std::size_t a_sampling_rate,
  const std::vector<irs_i16>& a_samples)
{
  const std::size_t bits_per_sample = 16;
  hmmio_ptr hmmio(a_file_name, NULL, MMIO_CREATE|MMIO_WRITE|MMIO_ALLOCBUF);
  WAVEFORMATEX format;
  format.wFormatTag = WAVE_FORMAT_PCM;
  format.nChannels = 1;
  format.nSamplesPerSec = static_cast<DWORD>(a_sampling_rate);
  format.nBlockAlign = static_cast<WORD>(bits_per_sample/8);
  format.nAvgBytesPerSec =
    static_cast<DWORD>(a_sampling_rate*format.nBlockAlign);
  format.wBitsPerSample = static_cast<WORD>(bits_per_sample);
  format.cbSize = 0;

  MMCKINFO ckOutRIFF;

  ckOutRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
  if (mmioCreateChunk(hmmio, &ckOutRIFF, MMIO_CREATERIFF) !=
    MMSYSERR_NOERROR) {
    throw std::runtime_error("Ошибка при создании файла.");
  }
  MMCKINFO ckOut;
  ckOut.ckid = mmioFOURCC('f', 'm', 't', ' ');
  if (mmioCreateChunk(hmmio, &ckOut, 0) != MMSYSERR_NOERROR ) {
    throw std::runtime_error("Ошибка создания fmt.");
  }
  const LONG format_size = static_cast<LONG>(sizeof(format));
  if (mmioWrite(hmmio, (HPSTR) &format, format_size) != format_size) {
    throw std::runtime_error("Ошибка при записи fmt.");
  }
  if (mmioAscend(hmmio, &ckOut, 0) != MMSYSERR_NOERROR) {
    throw std::runtime_error("Ошибка при закрытии сегмента fmt.");
  }
  ckOut.ckid = mmioFOURCC('d', 'a', 't', 'a');
  if (mmioCreateChunk(hmmio, &ckOut, 0) != MMSYSERR_NOERROR ) {
    throw std::runtime_error("Ошибка при создании сегмента данных.");
  }
  const char* buf = reinterpret_cast<const char*>(&a_samples.front());
  const LONG buf_size = a_samples.size()*bits_per_sample/8;
  if (mmioWrite(hmmio, buf, buf_size) != buf_size) {
    throw std::runtime_error("Ошибка при записи данных");
  }
  if (mmioAscend(hmmio, &ckOut, 0) !=  MMSYSERR_NOERROR) {
    throw std::runtime_error("Ошибка при закрытии блока данных data");
  }
  if (mmioAscend(hmmio, &ckOutRIFF, 0) !=  MMSYSERR_NOERROR) {
    throw std::runtime_error("Ошибка при закрытии блока RIFF");
  }
}
#endif // IRS_USE_SETUPAPI_WIN

#if IRS_USE_HID_WIN_API
// class usb_hid_info_t
irs::usb_hid_info_t::usb_hid_info_t():
  m_hid_dll(NULL),
  HidD_GetManufacturerString(NULL),
  HidD_GetProductString(NULL),
  HidD_GetSerialNumberString(NULL),
  HidD_GetPreparsedData(NULL),
  HidD_FreePreparsedData(NULL),
  HidD_GetHidGuid(NULL),
  HidD_GetAttributes(NULL),
  HidD_GetNumInputBuffers(NULL),
  HidD_SetFeature(NULL),
  HidD_GetFeature(NULL),
  HidD_GetInputReport(NULL),
  HidD_SetOutputReport(NULL),
  HidP_GetCaps(NULL)
{
  m_hid_dll = LoadLibrary(irst("HID.DLL"));
  if (!m_hid_dll) {
    throw std::runtime_error("Библиотека HID.DLL не найдена");
  }
  HidD_GetProductString = (PHidD_GetProductString)
    GetProcAddress(m_hid_dll, "HidD_GetProductString");
  HidD_GetHidGuid = (PHidD_GetHidGuid)
    GetProcAddress(m_hid_dll, "HidD_GetHidGuid");
  HidD_GetAttributes = (PHidD_GetAttributes)
    GetProcAddress(m_hid_dll, "HidD_GetAttributes");
  HidD_GetNumInputBuffers = (PHidD_GetNumInputBuffers)
    GetProcAddress(m_hid_dll, "HidD_GetNumInputBuffers");
  HidD_SetFeature = (PHidD_SetFeature)
    GetProcAddress(m_hid_dll, "HidD_SetFeature");
  HidD_GetFeature = (PHidD_GetFeature)
    GetProcAddress(m_hid_dll, "HidD_GetFeature");
  HidD_GetInputReport = (PHidD_GetInputReport)
    GetProcAddress(m_hid_dll, "HidD_GetInputReport");
  HidD_SetOutputReport = (PHidD_SetOutputReport)
    GetProcAddress(m_hid_dll, "HidD_SetOutputReport");
  HidD_GetManufacturerString = (PHidD_GetManufacturerString)
    GetProcAddress(m_hid_dll, "HidD_GetManufacturerString");
  HidD_GetProductString = (PHidD_GetProductString)
    GetProcAddress(m_hid_dll, "HidD_GetProductString");
  HidD_GetSerialNumberString = (PHidD_GetSerialNumberString)
    GetProcAddress(m_hid_dll, "HidD_GetSerialNumberString");
  HidD_GetPreparsedData = (PHidD_GetPreparsedData)
    GetProcAddress(m_hid_dll, "HidD_GetPreparsedData");
  HidD_FreePreparsedData = (PHidD_FreePreparsedData)
    GetProcAddress(m_hid_dll, "HidD_FreePreparsedData");
  HidP_GetCaps = (PHidP_GetCaps)
    GetProcAddress(m_hid_dll, "HidP_GetCaps");
}

irs::usb_hid_info_t::~usb_hid_info_t()
{
  FreeLibrary(m_hid_dll);
}

irs::usb_hid_info_t* irs::usb_hid_info_t::get_instance()
{
  static usb_hid_info_t usb_hid_info;
  return &usb_hid_info;
}

std::vector<irs::usb_hid_info_t::string_type>
irs::usb_hid_info_t::get_devices() const
{
  std::vector<string_type> devices;
  GUID HidGuid;
  (HidD_GetHidGuid)(&HidGuid);

  // Получаем информацию о hid-устройствах
  HDEVINFO hDevInfo = SetupDiGetClassDevs
    (&HidGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_DEVICEINTERFACE);
  if (hDevInfo == INVALID_HANDLE_VALUE) {
    throw std::runtime_error("Ошибка получения информации о hid-устройствах: "
      + last_error_str());
  }

  SP_DEVICE_INTERFACE_DATA devInfoData;
  devInfoData.cbSize = sizeof(devInfoData);
  DWORD device_index = 0;
  while (true)
  {
    BOOL Result = SetupDiEnumDeviceInterfaces(hDevInfo,
      0,
      &HidGuid,
      device_index,
      &devInfoData);
    if (!Result) {
      break;
    }
    device_index++;
    DWORD cbRequired = 0;

    SetupDiGetDeviceInterfaceDetail(hDevInfo,
      &devInfoData,
      0,
      0,
      &cbRequired,
      0);
    PSP_DEVICE_INTERFACE_DETAIL_DATA pdetailData =
      (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, cbRequired);
    pdetailData->cbSize = sizeof(*pdetailData);
    Result = SetupDiGetDeviceInterfaceDetail
      (hDevInfo, &devInfoData, pdetailData, cbRequired, &cbRequired, NULL);
    if (Result) {
      string_type path = pdetailData->DevicePath;
      devices.push_back(path);
    }
    LocalFree(pdetailData);
  }
  SetupDiDestroyDeviceInfoList(hDevInfo);
  return devices;
}

irs::usb_hid_device_attributes_t
irs::usb_hid_info_t::get_attributes(const string_type& a_path) const
{
  usb_hid_device_attributes_t attributes;
  HANDLE hid = NULL;
  try {
    hid = CreateFile (a_path.c_str(),
      0,
      FILE_SHARE_READ|FILE_SHARE_WRITE,
      (LPSECURITY_ATTRIBUTES)NULL,
      OPEN_EXISTING,
      0,
      NULL);
    if (hid == INVALID_HANDLE_VALUE) {
      throw std::runtime_error("Ошибка получения информации о hid-устройстве: "
        + last_error_str());
    }

    HIDD_ATTRIBUTES Attributes;
    Attributes.Size = sizeof(Attributes);
    if (HidD_GetAttributes (hid, &Attributes)) {
      attributes.vendor_id = Attributes.VendorID;
      attributes.product_id = Attributes.ProductID;
      attributes.version_number = Attributes.VersionNumber;
    }

    const int manufacturer_buf_size = 127;
    wchar_t manufacturer[manufacturer_buf_size];
    memset(manufacturer, 0, manufacturer_buf_size + sizeof(wchar_t));
    if (HidD_GetManufacturerString(hid, manufacturer, manufacturer_buf_size)) {
      std::wstring manufacturer_wstr = manufacturer;
      attributes.manufacturer = irs::str_conv<string_type>(manufacturer_wstr);
    }
    const int product_buf_size = 127;
    wchar_t product[product_buf_size];
    memset(product, 0, product_buf_size + sizeof(wchar_t));
    if (HidD_GetProductString(hid, product, product_buf_size)) {
      std::wstring product_wstr = product;
      attributes.product = irs::str_conv<string_type>(product_wstr);
    }
    const int serial_number_buf_size = 127;
    wchar_t serial_number[serial_number_buf_size];
    memset(serial_number, 0, serial_number_buf_size + sizeof(wchar_t));
    if (HidD_GetSerialNumberString(hid, serial_number,
        serial_number_buf_size)) {
      std::wstring serial_number_wstr = serial_number;
      attributes.serial_number = irs::str_conv<string_type>(serial_number_wstr);
    }
  } catch (...) {
    CloseHandle(hid);
    throw;
  }
  CloseHandle(hid);
  return attributes;
}

std::vector<irs::usb_hid_device_info_t>
irs::usb_hid_info_t::get_devices_info() const
{
  std::vector<usb_hid_device_info_t> devices;
  std::vector<string_type> paths = get_devices();
  for (size_type i = 0; i < paths.size(); i++) {
    usb_hid_device_info_t info;
    info.path = paths[i];
    try {
      info.attributes = get_attributes(info.path);
    } catch (...) {
      continue;
    }
    devices.push_back(info);
  }
  return devices;
}

#endif // IRS_USE_HID_WIN_API
#endif // defined(IRS_FULL_STDCPPLIB_SUPPORT) && defined(IRS_WIN32)

#if (defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__))
void irs::binary_data_to_hex_str(const irs_u8* ap_buf, std::size_t a_buf_size,
  irs::string_t* ap_str)
{
  typedef irs_size_t size_type;
  typedef irs::string_t string_type;
  const irs::char_t data_hex_map[] = {irst('0'),  irst('1'), irst('2'),
    irst('3'), irst('4'), irst('5'), irst('6'), irst('7'), irst('8'),
    irst('9'), irst('A'), irst('B'), irst('C'), irst('D'), irst('E'),
    irst('F')
  };
  ap_str->resize(a_buf_size*2);
  for (size_type elem_i = 0, str_i = 0; elem_i < a_buf_size;
    elem_i++) {
    const irs_u8 value = ap_buf[elem_i];
    (*ap_str)[str_i] = data_hex_map[value >> 4];
    str_i++;
    (*ap_str)[str_i] = data_hex_map[value & 0x0F];
    str_i++;
  }
}
#endif // defined(IRS_FULL_STDCPPLIB_SUPPORT) || defined(__ICCARM__)

#if IRS_USE_BOOST

#if (BOOST_VERSION >= 106000)

irs::string_t irs::absolute_path_to_relative(const irs::string_t& a_path,
  const irs::string_t& a_base)
{

  typedef boost::filesystem::path path_type;
  typedef boost::filesystem::path::string_type string_type;

  string_type path = irs::str_conv<string_type>(a_path);
  string_type base = irs::str_conv<string_type>(a_base);

  path_type absolute_path(path);
  path_type base_path(base);

  path_type relative_path = relative(absolute_path, base_path);

  //path_type relative_path =
    //irs::absolute_path_to_relative(absolute_path, base_path);

  string_type relative_str = relative_path.native();

  return irs::str_conv<irs::string_t>(relative_str);
}


irs::string_t irs::relative_path_to_absolute(const irs::string_t& a_path,
  const irs::string_t& a_base)
{
  typedef boost::filesystem::path path_type;
  typedef boost::filesystem::path::string_type string_type;

  string_type path = irs::str_conv<string_type>(a_path);
  string_type base = irs::str_conv<string_type>(a_base);

  path_type relative_path(path);
  path_type base_path(base);

  path_type absolute = boost::filesystem::absolute(relative_path, base_path);
  absolute = absolute.lexically_normal();
  string_type absolute_str = absolute.native();

  return irs::str_conv<irs::string_t>(absolute_str);
}

#elif (BOOST_VERSION < 104600) // (Для C++BuilderXE3)

namespace {

boost::filesystem::wpath absolute_path_to_relative(
  const boost::filesystem::wpath& a_path,
  const boost::filesystem::wpath& a_base)
{
  namespace fs = boost::filesystem;

  typedef fs::wpath path_type;
  typedef std::wstring string_type;

  if (!a_base.has_root_path()) {
    return a_path;
  }

  // Здесь стоит проверить наличие слеша после буквы диска и добавить его, если
  // он отсутвует

  if (!a_path.has_root_path()) {
    return a_path;
  }

  if (a_base.root_path() != a_path.root_path()) {
    return a_path;
  }

  const string_type _dot  = string_type(1, fs::dot<fs::wpath>::value);
  const string_type _dots = string_type(2, fs::dot<fs::wpath>::value);
  const string_type _sep = string_type(1, fs::slash<fs::wpath>::value);
  const string_type _dots_sep = _dots + _sep;

  path_type::iterator base_it = a_base.begin();
  path_type::iterator file_name_it = a_path.begin();
  while ((base_it != a_base.end()) && (file_name_it != a_path.end())) {
    if (*base_it != *file_name_it) {
      break;
    }
    ++base_it;
    ++file_name_it;
  }

  path_type relative_path;
  for (; base_it != a_base.end(); ++base_it) {

    if (*base_it == _dot) {
      continue;
    } else if (*base_it == _sep) {
      continue;
    }

    relative_path /= _dots_sep;
  }

  /*fs::wpath::iterator path_it_start = file_name_it;
  for (; file_name_it != file_name.end(); ++file_name_it) {
    if (file_name_it != path_it_start)
      relative_path /= irst("/");
    if (*file_name_it == _dot)
      continue;
    if (*file_name_it == _sep)
      continue;
    relative_path /= *file_name_it;
  }*/


  for (; file_name_it != a_path.end(); ++file_name_it) {
    relative_path /= *file_name_it;
  }

  return relative_path;
}

} // namespace empty

irs::string_t irs::absolute_path_to_relative(const irs::string_t& a_path,
  const irs::string_t& a_base)
{
  namespace fs = boost::filesystem;
  typedef fs::wpath path_type;
  typedef std::wstring string_type;
  string_type path = irs::str_conv<string_type>(a_path);
  string_type base = irs::str_conv<string_type>(a_base);

  path_type absolute_path(path);
  path_type base_path(base);

  path_type relative_path =
    absolute_path_to_relative(absolute_path, base_path);

  string_type relative_str = relative_path.native_file_string();
  return relative_str;
}

/*namespace {

boost::filesystem::wpath normalize_lexically(
  const boost::filesystem::wpath& a_path)
{
  namespace fs = boost::filesystem;
  typedef wchar_t char_type;
  typedef std::wstring string_type;
  typedef boost::filesystem::wpath path_type;



  const char_type dot = fs::dot<fs::wpath>::value;
  const char_type separator = fs::slash<fs::wpath>::value;
  const char_type colon = fs::colon<fs::wpath>::value;

  path_type dot_path(string_type(dot, 1));

  if (a_path.empty()) {
    return a_path;
  }

  path_type temp;

  path_type::iterator start(a_path.begin());
  path_type::iterator last(a_path.end());
  path_type::iterator stop(last--);
  for (path_type::iterator itr(start); itr != stop; ++itr)
  {
    path_type elem = *itr;
    string_type elem_str = elem.native_file_string();
    // ignore "." except at start and last
    if (elem_str.size() == 1
      && (elem_str[0] == dot)
      && itr != start
      && itr != last) continue;

    // ignore a name and following ".."
    if (!temp.empty()
      && elem_str.size() == 2
      && elem_str[0] == dot
      && elem_str[1] == dot) // dot dot
    {
      path_type filename = temp.leaf();
      string_type lf(filename.native_file_string());
      if (lf.size() > 0 &&
        (lf.size() != 1 || (lf[0] != dot && lf[0] != separator))
        && (lf.size() != 2 || (lf[0] != dot
            && lf[1] != dot
#             ifdef BOOST_WINDOWS_API
            && lf[1] != colon
#             endif
             )
           )
        )
      {
        temp.remove_filename();
        path_type filename123 = temp.leaf();
        //// if not root directory, must also remove "/" if any
        //if (temp.native().size() > 0
        //  && temp.native()[temp.native().size()-1]
        //    == separator)
        //{
        //  string_type::size_type rds(
        //    root_directory_start(temp.native(), temp.native().size()));
        //  if (rds == string_type::npos
        //    || rds != temp.native().size()-1)
        //  {
        //    temp.m_pathname.erase(temp.native().size()-1);
        //  }
        //}




        path_type::iterator next(itr);

        if (temp.empty() && (++next != stop) &&
          (next == last) && (*last == dot_path))
        {
          temp /= dot_path;
        }
        continue;
      }
    }

    temp /= *itr;
  };

  if (temp.empty())
    temp /= dot_path;
  return temp;

}

} // empty namespace */

irs::string_t irs::relative_path_to_absolute(const irs::string_t& a_path,
  const irs::string_t& a_base)
{
  typedef boost::filesystem::wpath path_type;
  typedef std::wstring string_type;

  string_type path = irs::str_conv<string_type>(a_path);
  string_type base = irs::str_conv<string_type>(a_base);

  path_type relative_path(path);
  path_type base_path(base);

  path_type absolute = boost::filesystem::complete(relative_path, base_path);

  absolute = absolute.normalize();

  string_type absolute_str = absolute.native_file_string();

  return irs::str_conv<irs::string_t>(absolute_str);
}
#endif // BOOST_VERSION < 104600 (Для C++BuilderXE3)

#endif // IRS_USE_BOOST

irs::string_t irs::default_ini_name()
{
  #ifdef __BORLANDC__
  return irs::cbuilder::default_ini_name();
  #else // __BORLANDC__
  return irst(".\\config.ini");
  #endif // __BORLANDC__
}
