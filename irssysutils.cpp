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

irs::string_t irs::default_ini_name()
{
  #ifdef __BORLANDC__
  return irs::cbuilder::default_ini_name();
  #else // __BORLANDC__
  return irst(".\\config.ini");
  #endif // __BORLANDC__
}
