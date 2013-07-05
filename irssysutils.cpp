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
