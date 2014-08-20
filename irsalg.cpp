//! \file
//! \ingroup signal_processing_group
//! \brief Алгоритмы
//!
//! Дата: 29.09.2010\n
//! Ранняя дата: 2.09.2009

// Номер файла
#define IRSALGCPP_IDX 9

#include <irspch.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif // __BORLANDC__

#include <string.h>
#include <math.h>

#include <irsalg.h>

#include <irsfinal.h>

// Реализация алгоритма кольцевого буфера

// Запрет конструктора по умолчанию
alg_ring::alg_ring()
{
}
// Конструктор
alg_ring::alg_ring(int size_max)
{
  // Текущее начало буфера
  f_begin = 0;
  // Текущая длина буфера
  f_size = 0;
  // Максимальная длина буфера
  f_size_max = size_max;
  // Кольцевой буфер
  f_buf_ring = IRS_NULL;
  // Ошибка в конструкторе
  f_create_error = irs_false;
  // Кольцевой буфер
  f_buf_ring = IRS_LIB_NEW_ASSERT(void*[f_size_max], IRSALGCPP_IDX);
  memset(f_buf_ring, 0, f_size_max*sizeof(void*));
  if (!f_buf_ring) f_create_error = irs_true;
}
//Деструктор
alg_ring::~alg_ring()
{
  if (f_buf_ring) IRS_LIB_ARRAY_DELETE_ASSERT(f_buf_ring);
}
// Чтение и удаление первого элемента буфера
void *alg_ring::read_and_remove_first()
{
  void *readed_elem = IRS_NULL;
  if (f_size > 0) {
    readed_elem = f_buf_ring[f_begin];
    begin_inc();
    f_size--;
  }
  return readed_elem;
}
// Добавление нового элемента в конец буфера
void *alg_ring::add_last_and_get_removed(void *buf_elem)
{
  void *removed_elem = IRS_NULL;
  if (f_size >= f_size_max) {
    removed_elem = f_buf_ring[f_begin];
    f_buf_ring[f_begin] = buf_elem;
    begin_inc();
  } else {
    irs_u32 end = get_end();
    f_buf_ring[end] = buf_elem;
    f_size++;
  }
  return removed_elem;
}
// Ошибка в конструкторе
irs_bool alg_ring::create_error()
{
  return f_create_error;
}
// Увеличение начала буфера на 1 с учетом границы
void alg_ring::begin_inc()
{
  f_begin++;
  if (f_begin >= f_size_max) f_begin = 0;
}
// Вычисление конца буфера
irs_u32 alg_ring::get_end()
{
  irs_u32 end = f_begin + f_size;
  if (end >= f_size_max) end -= f_size_max;
  return end;
}
// Чтение элемента буфера
void *alg_ring::operator[](irs_u32 index) const
{
  if (index < f_size) {
    irs_u32 buf_index = f_begin + index;
    if (buf_index >= f_size_max) buf_index -= f_size_max;
    return f_buf_ring[buf_index];
  }
  return IRS_NULL;
}

double irs::student_t_inverse_distribution_2x(double a_probability,
  size_t a_degrees_of_freedom)
{
  IRS_LIB_ASSERT((a_probability >= 0) && (a_probability <= 1));
  IRS_LIB_ASSERT(a_degrees_of_freedom > 0);
  size_t n = a_degrees_of_freedom;
  size_t index = 0;
  if ((n > 0) && (n <= 50)) {
    index = n - 1 ;
  } else if (n < 60) {
    index = 49;
  } else if (n < 70) {
    index = 50;
  } else if (n < 80) {
    index = 51;
  } else if (n < 90) {
    index = 52;
  } else if (n < 100) {
    index = 53;
  } else if (n < 200) {
    index = 54;
  } else if (n < 300) {
    index = 55;
  } else if (n < 1000) {
    index = 56;
  } else if (n < 10000) {
    index = 57;
  } else if (n >= 10000) {
    index = 58;
  }
  double result = 0;
  double** table = NULL;
  if (a_probability == 0.95) {
    double table_0_95[] = {
      // Для n от 1 до 50
      12.70620474,  4.30265273,   3.182446305,  2.776445105,  2.570581836,
      2.446911851,  2.364624252,  2.306004135,  2.262157163,  2.228138852,
      2.20098516,   2.17881283,   2.160368656,  2.144786688,  2.131449546,
      2.119905299,  2.109815578,  2.10092204,   2.093024054,  2.085963447,
      2.079613845,  2.073873068,  2.06865761,   2.063898562,  2.059538553,
      2.055529439,  2.051830516,  2.048407142,  2.045229642,  2.042272456,
      2.039513446,  2.036933343,  2.034515297,  2.032244509,  2.030107928,
      2.028094001,  2.026192463,  2.024394164,  2.02269092,   2.02107539,
      2.01954097,   2.018081703,  2.016692199,  2.015367574,  2.014103389,
      2.012895599,  2.011740514,  2.010634758,  2.009575237,  2.008559112,
      // Для n = 60, 70, 80, 90
      2.000297822,  1.994437112,  1.990063421,  1.986674541,
      // Для n = 100, 200, 300, 1000, 10000
      1.983971519,  1.971896224,  1.967903011,  1.962339081,  1.96020124
    };
    IRS_LIB_ASSERT(IRS_ARRAYSIZE(table_0_95) == 59);
    //*table = table_0_95;
    result = table_0_95[index];
  } else if (a_probability == 0.99) {
    double table_0_99[] = {
      // Для n от 1 до 50
      63.65674116,  9.924843201,  5.84090931,   4.604094871,  4.032142984,
      3.707428021,  3.499483297,  3.355387331,  3.249835542,  3.169272673,
      3.105806516,  3.054539589,  3.012275839,  2.976842734,  2.946712883,
      2.920781622,  2.89823052,   2.878440473,  2.860934606,  2.84533971,
      2.831359558,  2.818756061,  2.807335684,  2.796939505,  2.787435814,
      2.778714533,  2.770682957,  2.763262455,  2.756385904,  2.749995654,
      2.744041919,  2.738481482,  2.733276642,  2.728394367,  2.723805589,
      2.71948463,   2.715408722,  2.711557602,  2.707913184,  2.704459267,
      2.701181304,  2.698066186,  2.695102079,  2.692278266,  2.689585019,
      2.687013492,  2.684555618,  2.682204027,  2.679951974,  2.677793271,
      // Для n = 60, 70, 80, 90
      2.660283029,  2.647904624,  2.638690596,  2.631565166,
      // Для n = 100, 200, 300, 1000, 10000
      2.625890521,  2.600634436,  2.592316411,  2.580754698,  2.576321047
    };
    IRS_LIB_ASSERT(IRS_ARRAYSIZE(table_0_99) == 59);
    //*table = table_0_99;
    result = table_0_99[index];
  } else if (a_probability == 0.999) {
    double table_0_999[] = {
      // Для n от 1 до 50
      636.6192488,  31.59905458,  12.92397864,  8.610301581,  6.868826626,
      5.958816179,  5.407882521,  5.041305433,  4.780912586,  4.586893859,
      4.436979338,  4.317791284,  4.220831728,  4.140454113,  4.072765196,
      4.014996327,  3.965126272,  3.921645825,  3.883405853,  3.849516275,
      3.819277164,  3.792130672,  3.767626804,  3.745398619,  3.72514395,
      3.706611743,  3.689591713,  3.673906401,  3.659405019,  3.645958635,
      3.63345635,   3.62180226,   3.610913008,  3.600715797,  3.591146776,
      3.582149701,  3.573674844,  3.565678072,  3.558120081,  3.550965761,
      3.544183643,  3.537745445,  3.531625678,  3.525801306,  3.520251465,
      3.514957205,  3.509901283,  3.50506797,   3.500442891,  3.496012882,
      // Для n = 60, 70, 80, 90
      3.460200469,  3.435014521,  3.416337458,  3.401935307,
      // Для n = 100, 200, 300, 1000, 10000
      3.390491311,  3.339835406,  3.323251513,  3.300282648,  3.291499966
    };
    IRS_LIB_ASSERT(IRS_ARRAYSIZE(table_0_999) == 59);
    //*table = table_0_999;
    result = table_0_999[index];
  } else {
    IRS_LIB_ASSERT_MSG("Для указанного значения a_probability функция не "
      "реализована");
  }

  return result;
}

#ifdef __ICCAVR__
MEMORY_MODIFIER irs_u16 irs::crc16_data_t::table[size] =
#else //__ICCAVR__
irs::crc16_data_t::crc16_data_t()
{
  const irs_u16 table_src[size] =
#endif //__ICCAVR__
{
  0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
  0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
  0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
  0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
  0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
  0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
  0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
  0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
  0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
  0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
  0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
  0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
  0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
  0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
  0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
  0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
  0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
  0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
  0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
  0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
  0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
  0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
  0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
  0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
  0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
  0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
  0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
  0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
  0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
  0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
  0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
  0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};
#ifndef __ICCAVR__
  ::memcpy(table, table_src, size*sizeof(irs_u16));
}
#endif //__ICCAVR__

#ifdef __ICCAVR__
MEMORY_MODIFIER irs_u32 irs::crc32_data_t::table[size] =
#else //__ICCAVR__
irs::crc32_data_t::crc32_data_t()
{
  const irs_u32 table_src[size] =
#endif //__ICCAVR__
{
  0x00000000L, 0x77073096L, 0xEE0E612CL, 0x990951BAL,
  0x076DC419L, 0x706AF48FL, 0xE963A535L, 0x9E6495A3L,
  0x0EDB8832L, 0x79DCB8A4L, 0xE0D5E91EL, 0x97D2D988L,
  0x09B64C2BL, 0x7EB17CBDL, 0xE7B82D07L, 0x90BF1D91L,
  0x1DB71064L, 0x6AB020F2L, 0xF3B97148L, 0x84BE41DEL,
  0x1ADAD47DL, 0x6DDDE4EBL, 0xF4D4B551L, 0x83D385C7L,
  0x136C9856L, 0x646BA8C0L, 0xFD62F97AL, 0x8A65C9ECL,
  0x14015C4FL, 0x63066CD9L, 0xFA0F3D63L, 0x8D080DF5L,
  0x3B6E20C8L, 0x4C69105EL, 0xD56041E4L, 0xA2677172L,
  0x3C03E4D1L, 0x4B04D447L, 0xD20D85FDL, 0xA50AB56BL,
  0x35B5A8FAL, 0x42B2986CL, 0xDBBBC9D6L, 0xACBCF940L,
  0x32D86CE3L, 0x45DF5C75L, 0xDCD60DCFL, 0xABD13D59L,
  0x26D930ACL, 0x51DE003AL, 0xC8D75180L, 0xBFD06116L,
  0x21B4F4B5L, 0x56B3C423L, 0xCFBA9599L, 0xB8BDA50FL,
  0x2802B89EL, 0x5F058808L, 0xC60CD9B2L, 0xB10BE924L,
  0x2F6F7C87L, 0x58684C11L, 0xC1611DABL, 0xB6662D3DL,
  0x76DC4190L, 0x01DB7106L, 0x98D220BCL, 0xEFD5102AL,
  0x71B18589L, 0x06B6B51FL, 0x9FBFE4A5L, 0xE8B8D433L,
  0x7807C9A2L, 0x0F00F934L, 0x9609A88EL, 0xE10E9818L,
  0x7F6A0DBBL, 0x086D3D2DL, 0x91646C97L, 0xE6635C01L,
  0x6B6B51F4L, 0x1C6C6162L, 0x856530D8L, 0xF262004EL,
  0x6C0695EDL, 0x1B01A57BL, 0x8208F4C1L, 0xF50FC457L,
  0x65B0D9C6L, 0x12B7E950L, 0x8BBEB8EAL, 0xFCB9887CL,
  0x62DD1DDFL, 0x15DA2D49L, 0x8CD37CF3L, 0xFBD44C65L,
  0x4DB26158L, 0x3AB551CEL, 0xA3BC0074L, 0xD4BB30E2L,
  0x4ADFA541L, 0x3DD895D7L, 0xA4D1C46DL, 0xD3D6F4FBL,
  0x4369E96AL, 0x346ED9FCL, 0xAD678846L, 0xDA60B8D0L,
  0x44042D73L, 0x33031DE5L, 0xAA0A4C5FL, 0xDD0D7CC9L,
  0x5005713CL, 0x270241AAL, 0xBE0B1010L, 0xC90C2086L,
  0x5768B525L, 0x206F85B3L, 0xB966D409L, 0xCE61E49FL,
  0x5EDEF90EL, 0x29D9C998L, 0xB0D09822L, 0xC7D7A8B4L,
  0x59B33D17L, 0x2EB40D81L, 0xB7BD5C3BL, 0xC0BA6CADL,
  0xEDB88320L, 0x9ABFB3B6L, 0x03B6E20CL, 0x74B1D29AL,
  0xEAD54739L, 0x9DD277AFL, 0x04DB2615L, 0x73DC1683L,
  0xE3630B12L, 0x94643B84L, 0x0D6D6A3EL, 0x7A6A5AA8L,
  0xE40ECF0BL, 0x9309FF9DL, 0x0A00AE27L, 0x7D079EB1L,
  0xF00F9344L, 0x8708A3D2L, 0x1E01F268L, 0x6906C2FEL,
  0xF762575DL, 0x806567CBL, 0x196C3671L, 0x6E6B06E7L,
  0xFED41B76L, 0x89D32BE0L, 0x10DA7A5AL, 0x67DD4ACCL,
  0xF9B9DF6FL, 0x8EBEEFF9L, 0x17B7BE43L, 0x60B08ED5L,
  0xD6D6A3E8L, 0xA1D1937EL, 0x38D8C2C4L, 0x4FDFF252L,
  0xD1BB67F1L, 0xA6BC5767L, 0x3FB506DDL, 0x48B2364BL,
  0xD80D2BDAL, 0xAF0A1B4CL, 0x36034AF6L, 0x41047A60L,
  0xDF60EFC3L, 0xA867DF55L, 0x316E8EEFL, 0x4669BE79L,
  0xCB61B38CL, 0xBC66831AL, 0x256FD2A0L, 0x5268E236L,
  0xCC0C7795L, 0xBB0B4703L, 0x220216B9L, 0x5505262FL,
  0xC5BA3BBEL, 0xB2BD0B28L, 0x2BB45A92L, 0x5CB36A04L,
  0xC2D7FFA7L, 0xB5D0CF31L, 0x2CD99E8BL, 0x5BDEAE1DL,
  0x9B64C2B0L, 0xEC63F226L, 0x756AA39CL, 0x026D930AL,
  0x9C0906A9L, 0xEB0E363FL, 0x72076785L, 0x05005713L,
  0x95BF4A82L, 0xE2B87A14L, 0x7BB12BAEL, 0x0CB61B38L,
  0x92D28E9BL, 0xE5D5BE0DL, 0x7CDCEFB7L, 0x0BDBDF21L,
  0x86D3D2D4L, 0xF1D4E242L, 0x68DDB3F8L, 0x1FDA836EL,
  0x81BE16CDL, 0xF6B9265BL, 0x6FB077E1L, 0x18B74777L,
  0x88085AE6L, 0xFF0F6A70L, 0x66063BCAL, 0x11010B5CL,
  0x8F659EFFL, 0xF862AE69L, 0x616BFFD3L, 0x166CCF45L,
  0xA00AE278L, 0xD70DD2EEL, 0x4E048354L, 0x3903B3C2L,
  0xA7672661L, 0xD06016F7L, 0x4969474DL, 0x3E6E77DBL,
  0xAED16A4AL, 0xD9D65ADCL, 0x40DF0B66L, 0x37D83BF0L,
  0xA9BCAE53L, 0xDEBB9EC5L, 0x47B2CF7FL, 0x30B5FFE9L,
  0xBDBDF21CL, 0xCABAC28AL, 0x53B39330L, 0x24B4A3A6L,
  0xBAD03605L, 0xCDD70693L, 0x54DE5729L, 0x23D967BFL,
  0xB3667A2EL, 0xC4614AB8L, 0x5D681B02L, 0x2A6F2B94L,
  0xB40BBE37L, 0xC30C8EA1L, 0x5A05DF1BL, 0x2D02EF8DL
};
#ifndef __ICCAVR__
  ::memcpy(table, table_src, size*sizeof(irs_u32));
}
#endif //__ICCAVR__

// class crc32_table_stream_t
irs::handle_t<irs::crc32_data_t> irs::crc32_table_stream_t::mp_crc32_data =
  new irs::crc32_data_t();

irs::crc32_table_stream_t::crc32_table_stream_t():
  m_crc(0xFFFFFFFF)
{
}

irs_u32 irs::crc32_table_stream_t::get_crc()
{
  return ~m_crc;
}

void irs::crc32_table_stream_t::reset()
{
  m_crc = 0xFFFFFFFF;
}

irs_u8 irs::crc8(irs_u8 *a_buf, irs_u8 a_start, irs_u8 a_cnt)
{ //  Взято с википедии
  irs_u8 i, j;
  irs_u8 top = irs_u8(a_start + a_cnt);
  irs_u8 crc = 0xFF; /* Начальное состояние сдвигового регистра */
  irs_u8 flag = 0;
  for (i = a_start; i < top; i++)
  {
    crc = crc^a_buf[i];
    for (j = 1; j <= 8; j++)
    {
      flag = irs_u8(crc & 0x01);
      crc = irs_u8(crc >> 1); /* Сдвиг регистра на 1 позицию */
      if (flag) crc ^= 0x18;    //  0x18 - примитивный полином,
                                //  взят у Dallas, CRC8 for 1-Wire
    }
  }
  return(crc);
}

// Приведение фазы к диапазону a_phase_begin - a_phase_end
double irs::phase_normalize(double a_phase_in, double a_phase_begin,
  double a_phase_end)
{
  return phase_norm(a_phase_in, a_phase_begin, a_phase_end);
}

// Приведение фазы к диапазону -180 - +180
double irs::phase_normalize_180(double a_phase_in)
{
  return phase_normalize(a_phase_in, -180., +180.);
}

// Функция возвращает среднее арифметическое значение элементов массива
// Аналог функции Mean из библиотеки Math.hpp C++ Builder6
double mean(const double* ap_data, const int a_data_size)
{
  double result = 0.;
  for(int i = 0; i < a_data_size; i++){
    result += ap_data[i];
  }
  result /= a_data_size;
  return result;
}

// Функция возвращает дисперсию элементов массива
// Аналог функции Variance из библиотеки Math.hpp C++ Builder6
double variance(const double* ap_data, const int a_data_size)
{
  double data_mean = mean(ap_data, a_data_size);
  double result = 0.0;
  for(int i = 0; i <a_data_size; i++){
    double difference = ap_data[i] - data_mean;
    result += difference*difference;
  }
  result /= (a_data_size - 1);
  return result;
}

// Функция возвращает среднее квадратическое отклонение элементов массива
// Аналог функции StdDev из библиотеки Math.hpp C++ Builder6
double std_dev(const double* ap_data, const int a_data_size)
{
  double result = variance(ap_data, a_data_size);
  result = sqrt(result);
  return result;
}

