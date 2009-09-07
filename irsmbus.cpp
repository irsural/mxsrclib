// Клиент и сервер modbus написаный Олегом Коноваловым
// Дата: 17.02.2009

#include <irsmbus.h>
#include <string.h>
#include <timer.h>

counter_t rtimeout;
counter_t wait;
#define cl_rtime TIME_TO_CNT(2,1)
//#define WAIT_LIENT_TIME 0.2
//#define bit_msize   65535
//#define reg_msize   65535
//#define word_msize  131070
//#define byte_msize  8191
//Error cods
#define ILLEGAL_FUNCTION        1
#define ILLEGAL_DATA_ADDRESS    2
#define ILLEGAL_DATA_VALUE      3
#define SLAVE_DEVICE_FAILURE    4
//enum commands {Rcoil = 1,R_DI,R_HR,R_IR,Wcoil,W_HR,Status,W_SevHR = 10};

//#define OUTDBG(out) out
#define OUTDBG(out)


ostream &hex_u8(ostream &s)
{
  s.setf(ios::hex, ios::basefield);
  s<<setw(2)<<setfill('0');
  return s;
}
ostream &hex_u16(ostream &s)
{
  s.setf(ios::showbase);
  s.setf(ios::hex , ios::basefield);
  s<<setw(4)<<setfill('0');
  return s;
}
ostream &dec_u8(ostream &s)
{
  s.setf(ios::dec, ios::basefield);
  s<<setw(3)<<setfill('0');
  return s;
}
template <class T>
irs_bool to_irs_bool(T a_value)
{
  if (a_value) {
    return irs_true;
  } else {
    return irs_false;
  }
}

//#define OUTDBG(out)

irs::irsmb_se::irsmb_se(
  mxifa_ch_t channel,
  irs_u16 local_port,
  irs_u16 a_discr_inputs_size,
  irs_u16 a_coils_size,
  irs_u16 a_hold_regs,
  irs_u16 a_input_regs
):
  m_discr_inputs(int(a_discr_inputs_size*8)),
  m_coils(int(a_coils_size*8)),
  m_input_regs(int(a_input_regs)),
  m_hold_regs(int(a_hold_regs))
{
  coils_inbits  = irs_u16(a_coils_size*8);
  di_inbits     = irs_u16(a_discr_inputs_size*8);
  hr_inbytes    = a_hold_regs*2;
  ir_inbytes    = a_input_regs*2;

  di_end        = a_discr_inputs_size;
  coils_end     = a_discr_inputs_size + a_coils_size;
  hr_end        = a_discr_inputs_size + a_coils_size + a_hold_regs*2;
  ir_end        = a_discr_inputs_size + a_coils_size + a_hold_regs*2
                  + a_input_regs*2;

  memset(mess,0,sizeof(mess));
  memset(in_str,0,sizeof(in_str));
  m_size = a_coils_size + a_discr_inputs_size + hr_inbytes + ir_inbytes;
  mp_buf        = IRS_NULL;
  m_staddr      = 0;
  m_nregs       = 0;
  m_rcoil       = irs_false;
  mode          = read_mode;
  const mxip_t zero_ip = {{0,0,0,0}};
  m_mbchdata.mxifa_linux_dest.ip      = zero_ip;
  m_mbchdata.mxifa_linux_dest.port    = 0;
  mp_handle    = IRS_NULL;
  di_size       = 0;
  di_start      = 0;
  coils_size    = 0;
  coils_start   = 0;
  hr_size       = 0;
  hr_start      = 0;
  ir_size       = 0;
  ir_start      = 0;
  m_size_end    = 0;
  mxifa_init();
  mp_handle = mxifa_open(channel,irs_false);
  mxifa_linux_tcp_ip_cl_cfg cfg;
  mxifa_get_config(mp_handle, (void *)&cfg);
  cfg.local_port = local_port;
  mxifa_set_config(mp_handle, (void *)&cfg);
}

irs_uarc irs::irsmb_se::size()
{
  return m_size;
}

irs_bool irs::irsmb_se::connected()
{
  return irs_true;
}
void irs::irsmb_se::write_bytes(vector<irs_u16> &vec,irs_u32 index,irs_u32
size,const irs_u8 *buf,irs_u32 start)
{
  if(((index%2) == 0)&&((size%2)==0))
  {
    OUTDBG(cout << " OK\n");
    pack_tb_t &head = *(pack_tb_t *)(buf+start);
    copy(head.value,head.value +size/2,vec.begin()+index/2);
  }
  if(((index%2) == 0)&&((size%2)!=0))
  {
    pack_tb_t &head = *(pack_tb_t *)(buf+start);
    copy(head.value,head.value +size/2,vec.begin()+index/2);
    IRS_LOBYTE(vec[index/2+size/2]) = buf[start + size -1];
  }
  if(((index%2) != 0)&&((size%2)==0))
  {
    pack_tb_t &head = *(pack_tb_t *)(buf + start + 1);
    IRS_HIBYTE(vec[index/2]) = buf[start];
    copy(head.value,head.value + size/2-1,
      vec.begin()+index/2+1);
    IRS_LOBYTE(vec[index/2+size/2]) = buf[start + size-1];
  }
  if(((index%2) != 0)&&((size%2)!=0))
  {
    pack_tb_t &head = *(pack_tb_t *)(buf + start + 1);
    IRS_HIBYTE(vec[index/2]) = buf[start];
    copy(head.value,head.value + size/2,
      vec.begin()+index/2+1);
  }
  #ifdef NOP6
  for(irs_u16 i = (index/2); i<(size/2+index/2);i++)
  {
    OUTDBG(cout <<dec<<"Vector["<<(int)i<<"] = "<< hex<<showbase<<
      (int)vec[i]<<'\n');
  }
  OUTDBG(cout <<"\n");
  for(irs_u16 i = (start); i<(size+start);i++)
  {
    OUTDBG(cout <<dec<<"Buf["<<(int)i<<"] = "<<hex<<showbase <<
      (int)buf[i]<<'\n');
  }
  OUTDBG(cout <<"\n");
  #endif
}

void irs::irsmb_se::get_bytes(vector<irs_u16> &vec,irs_u32 index,
  irs_u32 size,irs_u8 *buf,irs_u32 start)
{
  //irs_u16 mass[vec.size()];

  if(((index%2) == 0)&&((size%2)==0))
  {
    OUTDBG(cout << " OK\n");
    pack_tb_t &head = *(pack_tb_t *)(buf+start);
    //copy(vec.begin()+index/2,vec.begin()+index/2+size/2,mass);
    copy(vec.begin()+index/2,vec.begin()+index/2+size/2,head.value);
    #ifdef NOPP
    for(irs_u16 i = 0; i<size/2;i++)
    {
      head.value[i] = mass[i];
      OUTDBG(cout <<hex_u16<< (int)mass[i]<<'=');
      OUTDBG(cout <<hex_u16<< (int)head.value[i]<<' ');
      OUTDBG(cout <<"|");
    }
    #endif
    //OUTDBG(cout <<dec<<noshowbase<< "\n");
  }
  if(((index%2) == 0)&&((size%2)!=0))
  {
    pack_tb_t &head = *(pack_tb_t *)(buf+start);
    //copy(vec.begin()+index/2,vec.begin()+index/2+size/2,mass);
    copy(vec.begin()+index/2,vec.begin()+index/2+size/2,head.value);
    //for(irs_u16 i = 0; i<(size/2);i++)
      //head.value[i] = mass[i];
    buf[size-1+start] = IRS_LOBYTE(vec[index/2+size/2]);
  }
  if(((index%2) != 0)&&((size%2)==0))
  {
    pack_tb_t &head = *(pack_tb_t *)(buf + 1 + start);
    buf[start] = IRS_HIBYTE(vec[index/2]);
    //copy(vec.begin()+index/2+1,vec.begin()+index/2+size/2,mass);
    copy(vec.begin()+index/2+1,vec.begin()+index/2+size/2,head.value);
    //for(irs_u16 i = 0; i<(size/2-1);i++)
      //head.value[i] = mass[i];
    buf[size-1+start] = IRS_LOBYTE(vec[index/2+size/2]);
  }
  if(((index%2) != 0)&&((size%2)!=0))
  {
    pack_tb_t &head = *(pack_tb_t *)(buf+1+start);
    buf[start] = IRS_HIBYTE(vec[index/2]);
    //copy(vec.begin()+index/2+1,vec.begin()+index/2+size/2+1,mass);
    copy(vec.begin()+index/2+1,vec.begin()+index/2+size/2+1,head.value);
    //for(irs_u16 i = 0; i<(size/2);i++)
      //head.value[i] = mass[i];
  }
  OUTDBG(cout << " OK\n");
}
void irs::irsmb_se::range(irs_u32 index, irs_u32 size, irs_u32 M1, irs_u32 M2,
irs_u32 *num,irs_u32 *start)
{
  irs_u32 var = index + size;
  if((index>=M1)&&(index<M2)&&(var>M1)&&(var <= M2))
  {
    *num = size;
    *start = index - M1;
  }
  else if((index>M1)&&(index<M2)&&(var>M2))
  {
    *num = M2 - index;
    *start = index - M1;
  }
  else if((index<M1)&&(var>M1)&&(var < M2))
  {
    *num = var - M1;
    *start = 0;
  }
  else if((index<=M1)&&(var >= M2))
  {
    *num = M2 - M1;
    *start = 0;
  }
}
//Чтение из массива данных в буфер размером size с позиции index
void irs::irsmb_se::read(irs_u8 *ap_buf, irs_uarc index, irs_uarc a_size)
{
  irs_u32 di_size = 0, di_start = 0;
  irs_u32 coils_size = 0, coils_start = 0;
  irs_u32 hr_size = 0, hr_start = 0;
  irs_u32 ir_size = 0, ir_start = 0;
  range(index,a_size,0,di_end,&di_size,&di_start);
  range(index,a_size,di_end,coils_end,&coils_size,&coils_start);
  range(index,a_size,coils_end,hr_end,&hr_size,&hr_start);
  range(index,a_size,hr_end,ir_end,&ir_size,&ir_start);
  irs_u32 i = 0;
  OUTDBG(cout << "Index: " << index<<'\n');
  if((di_size != 0)||(di_start != 0))
  {
    OUTDBG(cout << "DI size: "<<di_size<<" DI_start: "<< di_start<< "\n");
    packet_data_8_t &m_packet_di = *(packet_data_8_t *)in_str;
    m_packet_di.byte_count = (irs_u8)di_size;
    BintoDec(m_discr_inputs,di_start*8,m_packet_di);
    for(;i<di_size;i++)
      ap_buf[i] = m_packet_di.value[i];
  }
  if((coils_size != 0)||(coils_start != 0))
  {
    OUTDBG(cout << "coils_size: "<<coils_size<<" coils_start: "<<
      coils_start<< "\n");
    packet_data_8_t &m_packet_coils = *(packet_data_8_t *)in_str;
    m_packet_coils.byte_count = (irs_u8)coils_size;
    BintoDec(m_coils,coils_start*8,m_packet_coils);
    for(;i<(coils_size+di_size);i++)
      ap_buf[i] = m_packet_coils.value[i-di_size];
  }
  if((hr_size != 0)||(hr_start != 0))
  {
    OUTDBG(cout << "HR size: "<<hr_size<<" HR_start: "<<
      hr_start<< "\n");
    get_bytes(m_hold_regs,hr_start,hr_size,ap_buf,i);
    //for(irs_u16 i = 0; i < hr_size;i++)
      //OUTDBG(cout <<hex<<showbase<< (int)ap_buf[i] << ' ');
    //OUTDBG(cout <<dec<<noshowbase<< "\n");
  }
  if((ir_size != 0)||(ir_start != 0))
  {
    OUTDBG(cout << "IR_size: "<<ir_size<<" IR_start: "<< ir_start<< "\n");
    get_bytes(m_input_regs,ir_start,ir_size,ap_buf,i+hr_size);
    OUTDBG(cout << "Regs\n");
    //for(irs_u16 i = ir_start/2; i< (ir_size/2+ir_start/2);i++)
      //OUTDBG(cout <<hex<<showbase<< (int)m_input_regs_r[i]<<' ');
    //OUTDBG(cout <<dec<<noshowbase<< "\n");
  }
}
void irs::irsmb_se::write(const irs_u8 *buf, irs_uarc index, irs_uarc size)
{
  irs_u32 di_size = 0, di_start = 0;
  irs_u32 coils_size = 0, coils_start = 0;
  irs_u32 hr_size = 0, hr_start = 0;
  irs_u32 ir_size = 0, ir_start = 0;
  range(index,size,0,di_end,&di_size,&di_start);
  range(index,size,di_end,coils_end,&coils_size,&coils_start);
  range(index,size,coils_end,hr_end,&hr_size,&hr_start);
  range(index,size,hr_end,ir_end,&ir_size,&ir_start);
  //irs_u16 i = 0;
  if((di_size != 0)||(di_start != 0))
  {
    OUTDBG(cout << "DI_size: "<<di_size<<" DI_start: "<< di_start<< "\n");
    pack_d8_t &m_pack_di = *(pack_d8_t *)(buf);
    DectoBin(m_discr_inputs,di_start*8,m_pack_di,(irs_u16)di_size);
    //for(irs_u16 i = di_start; i< (di_size+di_start);i++)
      //OUTDBG(cout << (int)m_discr_inputs[i] << ' ');
    //OUTDBG(cout <<dec<<noshowbase<< "\n");
  }
  if((coils_size != 0)||(coils_start != 0))
  {
    OUTDBG(cout << "coils_size: "<<coils_size<<" coils_start: "<< coils_start<< "\n");
    pack_d8_t &m_pack_coils = *(pack_d8_t *)(buf+di_size);
    DectoBin(m_coils,coils_start*8,m_pack_coils,(irs_u16)coils_size);
    //for(irs_u16 i = hr_start; i< (hr_size+hr_start);i++)
      //OUTDBG(cout << (int)m_coils[i] << ' ');
    //OUTDBG(cout <<dec<<noshowbase<< "\n");
  }
  if((hr_size != 0)||(hr_start != 0))
  {
    OUTDBG(cout << "HR size: "<<hr_size<<" HR_start: "<< hr_start<< "\n");
    write_bytes(m_hold_regs,hr_start,hr_size,buf,coils_size+di_size);
    //for(irs_u16 i = hr_start/2; i< (hr_size/2+hr_start/2);i++)
      //OUTDBG(cout <<hex<<showbase<< (int)m_hold_regs[i] << ' ');
    //OUTDBG(cout <<dec<<noshowbase<< "\n");
  }
  if((ir_size != 0)||(ir_start != 0))
  {
    OUTDBG(cout << "IR size: "<<ir_size<<" IR_start: "<< ir_start<< "\n");
    write_bytes(m_input_regs,ir_start,ir_size,buf,coils_size+hr_size+di_size);
    //for(irs_u16 i = 0; i< (ir_inbytes/2);i++)
      //OUTDBG(cout <<hex_u16<<(int)m_input_regs[i]<<' ');
    //OUTDBG(cout <<dec<<noshowbase<< "\n");
  }
  OUTDBG(
    if((coils_size == 0)&&(di_size == 0)&&(hr_size == 0)&&(ir_size == 0))
      cout << "\n***************Illegal index or bufer size"
        "******************\n\n"
  );

}
irs_bool irs::irsmb_se::bit(irs_uarc index, irs_uarc bit_index)
{
  irs_u32 di_size = 0, di_start = 0;
  irs_u32 coils_size = 0, coils_start = 0;
  irs_u32 hr_size = 0, hr_start = 0;
  irs_u32 ir_size = 0, ir_start = 0;
  // irs_uarc byte = 0;
  range(index,1,0,di_end,&di_size,&di_start);
  range(index,1,di_end,coils_end,&coils_size,&coils_start);
  range(index,1,coils_end,hr_end,&hr_size,&hr_start);
  range(index,1,hr_end,ir_end,&ir_size,&ir_start);
  if (bit_index < 8) {
    if ((di_size != 0) || (di_start != 0))
    {
      //byte = m_discr_inputs[di_start*8];
      return to_irs_bool(m_discr_inputs[di_start*8 + bit_index]);
    }
    if ((coils_size != 0) || (coils_start != 0))
    {
      //byte = m_coils[coils_start*8];
      return to_irs_bool(m_coils[coils_start*8 + bit_index]);
    }
    if ((hr_size != 0) || (hr_start != 0))
    {
      return to_irs_bool(m_hold_regs[ir_start/2]&(1 << bit_index));
    }
    if ((ir_size != 0) || (ir_start != 0))
    {
      return to_irs_bool(m_input_regs[hr_start/2]&(1 << bit_index));
    }
  }
  OUTDBG(
    if((coils_size == 0)&&(di_size == 0)&&(hr_size == 0)&&(ir_size == 0))
      cout << "\n***************Illegal index or bufer size******************"
        "\n\n"
  );
  return irs_false;
}

void irs::irsmb_se::set_bit(irs_uarc index, irs_uarc bit_index)
{
  irs_uarc byte = 0;
  irs_u32 di_size = 0, di_start = 0;
  irs_u32 coils_size = 0, coils_start = 0;
  irs_u32 hr_size = 0, hr_start = 0;
  irs_u32 ir_size = 0, ir_start = 0;
  range(index,1,0,di_end,&di_size,&di_start);
  range(index,1,di_end,coils_end,&coils_size,&coils_start);
  range(index,1,coils_end,hr_end,&hr_size,&hr_start);
  range(index,1,hr_end,ir_end,&ir_size,&ir_start);
  OUTDBG(
    if((coils_size == 0)&&(di_size == 0)&&(hr_size == 0)&&(ir_size == 0))
      cout << "\n************Illegal index or bufer size***************\n\n"
  );

  if (bit_index < 8) {
    if((di_size != 0)||(di_start != 0))
    {
      m_discr_inputs[di_start*8+bit_index] = irs_true;
    }
    if((coils_size != 0)||(coils_start != 0))
    {
      m_coils[coils_start*8+bit_index] = irs_true;
    }
    if((hr_size != 0)||(hr_start != 0))
    {
      byte = m_hold_regs[hr_start/2u];
      byte |= (1<<bit_index);
      m_hold_regs[hr_start/2u] = IRS_LOBYTE(byte);
    }
    if((ir_size != 0)||(ir_start != 0))
    {
      byte = m_input_regs[ir_start/2u];
      byte |= (1<<bit_index);
      m_input_regs[ir_start/2u] = IRS_LOBYTE(byte);
    }
  }
}
void irs::irsmb_se::clear_bit(irs_uarc index, irs_uarc bit_index)
{
  irs_u16 byte = 0;
  irs_u32 di_size = 0, di_start = 0;
  irs_u32 coils_size = 0, coils_start = 0;
  irs_u32 hr_size = 0, hr_start = 0;
  irs_u32 ir_size = 0, ir_start = 0;
  range(index,1,0,di_end,&di_size,&di_start);
  range(index,1,di_end,coils_end,&coils_size,&coils_start);
  range(index,1,coils_end,hr_end,&hr_size,&hr_start);
  range(index,1,hr_end,ir_end,&ir_size,&ir_start);
  OUTDBG(
    if((coils_size == 0)&&(di_size == 0)&&(hr_size == 0)&&(ir_size == 0))
      cout << "\n****************Illegal index or bufer size"
        "***************\n\n"
  );
  if (bit_index < 8) {
    if((di_size != 0)||(di_start != 0))
    {
      m_discr_inputs[di_start*8+bit_index] = irs_false;
    }
    if((coils_size != 0)||(coils_start != 0))
    {
      m_coils[coils_start*8+bit_index] = irs_false;
    }
    if((hr_size != 0)||(hr_start != 0))
    {
      byte = m_hold_regs[hr_start/2];
      byte&=irs_u16(~(1<<bit_index));
      m_hold_regs[hr_start/2] = byte;
    }
    if((ir_size != 0)||(ir_start != 0))
    {
      byte = m_input_regs[ir_start/2];
      byte&=irs_u16(~(1<<bit_index));
      m_input_regs[ir_start/2] = byte;
    }
  }
}
void irs::irsmb_se::DectoBin(vector<bool> &bits,irs_u32 index,const
packet_data_8_t &data)
{
  irs_u8 copy = 0;

  request_read_t &m_dop_head =
    *(request_read_t *)(mess+size_of_header);
  //OUTDBG(cout << "---------------Function Dec to Bin---------------------\n");

  OUTDBG(cout <<"Start position:"<< index<<"\n");
  OUTDBG(cout <<"Number of registers:" <<(int)data.byte_count<<"\n");
  for(irs_u16 y = 0; y < (m_dop_head.num_of_regs); y++)
  {
    if((y%8) == 0)
    {
      copy = data.value[y/8];
      OUTDBG(cout << hex_u8<<(int)data.value[y]<<' ');
    }
    bits[index + y] = (1&copy);
    copy>>=1;
    //OUTDBG(cout <<" bits["<<(index + y)<<"] = "<< (int)bits[index + y]<<'\n');
  }
  //OUTDBG(cout << "\n---------------------END------------------------------\n");
}
void irs::irsmb_se::DectoBin(vector<bool> &bits,irs_u32 index,
  const pack_d8_t &data,irs_u16 bcount)
{
  irs_u8 copy = 0;
  //OUTDBG(cout << "---------------Function Dec to Bin---------------------\n");
  //OUTDBG(cout <<"Start position:"<< index<<"\n");
  //OUTDBG(cout <<"Number of registers:" <<(int)data.byte_count<<"\n");
  for(irs_u8 y = 0; y < bcount; y++)
  {
    copy = data.value[y];
    //OUTDBG(cout << (int)data.value[y]<<' ');
    for(irs_u8 x = 0; x<8;x++)
    {
      bits[index + y*8 + x] = (1&copy);
      copy>>=1;
    }
  }
  //OUTDBG(cout << "\n---------------------END------------------------------\n");
}

void irs::irsmb_se::BintoDec(vector<bool> &bits,irs_u32 index,
  packet_data_8_t &data)
{
  irs_u8 mass = 0;
  //unsigned e = 1;
  irs_u8 tmp = 0;
  unsigned int i = 0;
  OUTDBG(cout << "\n"<<(int)index<<"\n");
  OUTDBG(cout << (int)data.byte_count<<"\n");
  OUTDBG(cout<<"\n--------------Function--Bin to Dec-------------------- \n");
  for(irs_u32 y = index; y< (index+data.byte_count*8);y++)
  {
    OUTDBG(cout << dec<<noshowbase<<(int)i);
    OUTDBG(cout <<" - mass "<<(int)mass <<" + = "<< (int)bits[y]<<"*"<<e<<'\n');
    if(bits[y])
      mass |= irs_u8(1<<i);
    i++;
    if(i == 8) {
      i = 0;
      data.value[tmp] = mass;
      mass = 0;
      tmp++;
    }
  }
  OUTDBG(cout << "\nReturn value - "<<(int)data.value[tmp-1]<< '\n');
  OUTDBG(cout<<"\n--------------------------END-------------------------\n");
}
void irs::irsmb_se::Error_response(irs_u8 error_code)
{
  complex_modbus_header_t &m_cmb_head = *(complex_modbus_header_t *)mess;
  pack_d8_t &m_ercode = *(pack_d8_t *)(mess+size_of_header);
  m_cmb_head.function_code += irs_u8(0x80);
  m_ercode.value[0] = error_code;

}
void irs::irsmb_se::convert(irs_u8 *ap_mess,irs_u8 a_start,irs_u8 a_length)
{
  irs_u8 var = 0;
  for(irs_u8 i = a_start; i < irs_u8(a_length+a_start);i+=irs_u8(2))
  {
    var = ap_mess[i+1];
    ap_mess[i+1] = ap_mess[i];
    ap_mess[i] = var;
  }
}
void irs::irsmb_se::status()
{
  OUTDBG(cout << "*******************Discret Inputs*********************\n");
  for(irs_u16 i = 0; i< di_inbits;i++)
  {
      OUTDBG(cout << m_discr_inputs[i]);
  }
  OUTDBG(cout <<"\n*****************************************************\n");
  OUTDBG(cout <<"******************COILS********************************\n");
  for(irs_u16 i = 0; i< coils_inbits;i++)
  {
      OUTDBG(cout << m_coils[i]);
  }
  OUTDBG(cout <<"\n*****************************************************\n");
  OUTDBG(cout << "**************Holding registers***********************\n");
  for(irs_u16 i = 0; i< irs_u16(hr_inbytes/2);i++)
  {
      OUTDBG(cout <<(int)m_hold_regs[i]<<' ');
  }
  OUTDBG(cout <<"\n*****************************************************\n");
  OUTDBG(cout << "**************Input registers*************************\n");
  for(irs_u16 i = 0; i< irs_u16(ir_inbytes/2);i++)
  {
      OUTDBG(cout <<(int)m_input_regs[i]<<' ');
  }
  OUTDBG(cout<<"\n*************************************\n");
}
void irs::irsmb_se::tick()
{
  switch(mode)
  {
    case read_mode:
    {
      if(mxifa_read_end(mp_handle,irs_false))
      {
        if((mess[5] != 0)||(mess[4] != 0))
          mode = read_pack;
        else
        {
          mxifa_read_begin(mp_handle,&m_mbchdata,mess,size_of_MBAP);
          mode = read_pack;
        }

        //for(irs_u16 t=0; t<size_of_packet;t++);
          //OUTDBG(cout << (int)mess[t]<< ' ');
        //OUTDBG(cout << "Message: "<< (int)mess<< "\n");
      }
    }
    break;
    case read_pack:
    {
      if(mxifa_read_end(mp_handle,irs_false))
      {
        convert(mess,0,size_of_MBAP);
        complex_modbus_header_t &m_cmb_head = *(complex_modbus_header_t *)mess;
        if((mess[5] == 0)&&(mess[4] == 0))
          mode = read_mode;
        else
        {

          OUTDBG(cout<<"MBAP header\n");
          for(irs_u16 t = 0; t<size_of_MBAP;t++)
            OUTDBG(cout << hex_u8<< ((int)(mess[t])) << ' ');
          OUTDBG(cout<<"\n");
          pack_d8_t &raw_bytes = *(pack_d8_t *)(mess+size_of_MBAP);
          mxifa_read_begin(mp_handle,&m_mbchdata,raw_bytes.value,
            m_cmb_head.length);
          mode = read_end;
        }
        //for(irs_u16 t=0; t<size_of_packet;t++);
         // OUTDBG(cout << (int)mess[t]<< ' ');
        //OUTDBG(cout << "Message: "<< (int)mess<< "\n");
      }
    }
    break;
    case write_mode:
    {
      if(mxifa_write_end(mp_handle,irs_false))
      {
        OUTDBG(cout << "Header of message: \n");
        for(irs_u16 t = 0; t<(size_of_MBAP + size_of_res);t++)
          OUTDBG(cout << hex_u8<< ((int)(mess[t])) << ' ');
        OUTDBG(cout << "\nTransmit message: \n");
        for(irs_u16 t=0; t<irs_u16(size_of_MBAP + m_size_end);t++)
          OUTDBG(cout << hex_u8<<(int)mess[t]<< ' ');
        OUTDBG(cout << "\nSize of transmit message "<<
          (size_of_MBAP + m_size_end)<< '\n');
        mxifa_write_begin(mp_handle,&m_mbchdata,mess,size_of_MBAP + m_size_end);
        mode = write_end;
      }
    }
    break;
    case read_end:
    {
      if(mxifa_read_end(mp_handle,irs_false))
      {
        //copy(m_packet.begin(),m_packet.begin()+size_of_header,&m_cmb_head);
        //memcpy(&m_cmb_head,mess,size_of_header);
        //OUTDBG(cout <<"Recieved message: \n");
        for(irs_u16 t = 0; t<12;t++)
          OUTDBG(cout << hex_u8<< ((int)(mess[t])) << ' ');
        OUTDBG(cout <<"\n");
        //convert(mess,0,size_of_MBAP);
        convert(mess,8,size_of_read_header);
        OUTDBG(cout <<"\nRecieved Message after convert: \n");
        for(irs_u16 t = 0; t<12;t++)
          OUTDBG(cout << hex_u8<< ((int)(mess[t])) << ' ');
        OUTDBG(cout <<"\n");
        complex_modbus_header_t &m_cmb_head = *(complex_modbus_header_t *)mess;
        request_read_t &m_sec_head = *(request_read_t *)(mess+size_of_header);
        pack_tb_t &m_ssec_head = *(pack_tb_t *)(mess+size_of_header);
        m_staddr = m_ssec_head.value[0];
        pack_tb_t &m_trec_head = *(pack_tb_t *)(mess+size_of_header+1);
        m_nregs = m_trec_head.value[0];
        for(irs_u16 t=0; t<(m_cmb_head.length+size_of_MBAP);t++)
          OUTDBG(cout << hex_u8<<(int)mess[t]<< ' ');
        OUTDBG(cout <<"\n");
        OUTDBG(cout << "Recieved header - transaction_id:    " <<
          hex_u16<<m_cmb_head.transaction_id << "\n");
        OUTDBG(cout << "Recieved header - proto_id:          " <<
          hex_u16<<m_cmb_head.proto_id<< "\n");
        OUTDBG(cout << "Recieved header - length:            " <<
          hex_u16<<m_cmb_head.length<< "\n");
        OUTDBG(cout << "Recieved header - unit_identifier:   " <<
          hex_u8<<(int)m_cmb_head.unit_identifier  << "\n");
        OUTDBG(cout << "Recieved header - function_code:     " <<
          hex_u8<<(int)m_cmb_head.function_code << "\n");
        //OUTDBG(cout << "Recieved header - starting_address:  " <<
          //m_sec_head.starting_address << "\n");
        //OUTDBG(cout << "Recieved header - num_of_regs:       " <<
          //m_sec_head.num_of_regs << "\n");
        //OUTDBG(cout << "M_hold regs "<< (int)m_hold_regs[0]<<"\n");
        m_nregs = m_sec_head.num_of_regs;
        m_staddr = m_sec_head.starting_address;
        OUTDBG(cout << "Recieved header - starting_address:  " << hex_u16<<m_staddr<<"\n");
        OUTDBG(cout << "Recieved header - num_of_regs:       " << hex_u16<<m_nregs<<"\n");

        switch(m_cmb_head.function_code)
        {
          case Rcoil:
          {
            range(m_staddr,m_nregs,0,coils_inbits,&coils_size,&coils_start);
            OUTDBG(cout << "Coils_start " << coils_start<<"\n");
            OUTDBG(cout << "Coils_size " << coils_size<<"\n");
            if((coils_size != 0)||(coils_start != 0))
            {
              //m_nregs = m_sec_head.num_of_regs;
              m_rcoil = irs_true;
              packet_data_8_t &m_packet_coils =
                *(packet_data_8_t*)(mess+size_of_header);
              if((m_sec_head.num_of_regs%8)==0)
                m_packet_coils.byte_count = irs_u8(m_sec_head.num_of_regs/8);
              else
                m_packet_coils.byte_count =
                  irs_u8(m_sec_head.num_of_regs/8 + 1);
              //OUTDBG(cout << "Recieved header - starting_address:  " <<
                //m_staddr <<"\n");
              BintoDec(m_coils,m_staddr,m_packet_coils);
              m_cmb_head.length =
                irs_u16(size_of_res + m_packet_coils.byte_count);
              m_size_end = m_cmb_head.length;
              convert(mess,0,size_of_MBAP);
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case R_DI:
          {
            range(m_staddr,m_nregs,0,di_inbits,&di_size,&di_start);
            OUTDBG(cout << "DI_start " << di_start<<"\n");
            OUTDBG(cout << "DI_size " << di_size<<"\n");
            if((di_size != 0)||(di_start != 0))
            {
              m_rcoil = irs_false;
              packet_data_8_t &m_packet_di =
                *(packet_data_8_t*)(mess+size_of_header);
              if((m_sec_head.num_of_regs%8)==0)
                m_packet_di.byte_count = irs_u8(m_sec_head.num_of_regs/8);
              else
                m_packet_di.byte_count = irs_u8(m_sec_head.num_of_regs/8 + 1);
              //OUTDBG(cout << "Recieved header - starting_address:  " <<
                //m_staddr <<"\n");
              BintoDec(m_discr_inputs,m_staddr,m_packet_di);
              m_cmb_head.length = irs_u16(size_of_res + m_packet_di.byte_count);
              m_size_end = m_cmb_head.length;
              convert(mess,0,size_of_MBAP);
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case R_HR:
          {
            range(m_staddr,m_nregs,0,hr_inbytes/2,&hr_size,&hr_start);
            OUTDBG(cout << "HR_start " << hr_start<<"\n");
            OUTDBG(cout << "HR_size " << hr_size<<"\n");
            if((hr_size != 0)||(hr_start != 0))
            {
              request_wr_coils_t &m_dop_head =
                *(request_wr_coils_t*)(mess+size_of_header);
              m_dop_head.byte_count = irs_u8(hr_size*2);
              get_bytes(m_hold_regs,hr_start,hr_size*2,m_dop_head.value,0);
              //m_data_16.clear();
              //copy(m_hold_regs.begin()+ hr_start,
                //m_hold_regs.begin()+ hr_start + hr_size,m_data16);
              //packet_data_16_t &m_packet_hr =
                //*(packet_data_16_t*)(mess+size_of_header);
              //m_packet_hr.byte_count = hr_size*2;
              OUTDBG(cout <<"Memory contein: \n");
              for(irs_u8 i = 0; i< irs_u8(hr_size*2);i++)
              {
                //m_packet_hr.value[i] = m_data16[i];
                OUTDBG(cout <<hex_u8<< (int)m_dop_head.value[i]<<'|');
              }
              OUTDBG(cout <<"\n");
              //OUTDBG(cout <<dec<<noshowbase<< "\n");
              OUTDBG(cout <<"Byte count: "<< hex_u8 <<
                (int)m_dop_head.byte_count<<"\n");
              m_cmb_head.length = irs_u16(size_of_res + m_dop_head.byte_count);
              m_size_end = m_cmb_head.length;
              convert(mess,size_of_header+1,m_dop_head.byte_count);
              convert(mess,0,size_of_MBAP);
              //convert(mess,8,size_of_read_header);
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case R_IR:
          {
            range(m_staddr,m_nregs,0,ir_inbytes/2,&ir_size,&ir_start);
            //OUTDBG(cout << "IR_start " << ir_start<<"\n");
            //OUTDBG(cout << "IR_size " << ir_size<<"\n");
            if((ir_size != 0)||(ir_start != 0))
            {
              request_wr_coils_t &m_dop_head =
                *(request_wr_coils_t*)(mess+size_of_header);
              m_dop_head.byte_count = irs_u8(ir_size*2);
              OUTDBG(cout <<"INPUT REGS\n" );
              get_bytes(m_input_regs,ir_start,ir_size*2,m_dop_head.value,0);
              for(irs_u16 i = 0; i< irs_u16(ir_inbytes/2);i++)
              {
                  OUTDBG(cout <<hex_u16<<(int)m_dop_head.value[i]<<' ');
              }
              //OUTDBG(cout <<dec<<noshowbase<< "\n");
              m_cmb_head.length = irs_u16(size_of_res + m_dop_head.byte_count);
              m_size_end = m_cmb_head.length;
              OUTDBG(cout <<"before convert\n" );
              for(irs_u16 t=0; t<(size_of_header+1+m_dop_head.byte_count);t++)
              OUTDBG(cout << hex_u8<<(int)mess[t]<< ' ');
              OUTDBG(cout <<"\n" );
              convert(mess,size_of_header+1,m_dop_head.byte_count);
              OUTDBG(cout <<"after convert\n" );
              for(irs_u16 t=0; t<(size_of_header+1+m_dop_head.byte_count);t++)
              OUTDBG(cout << hex_u8<<(int)mess[t]<< ' ');
              OUTDBG(cout <<"\n" );
              convert(mess,0,size_of_MBAP);
              //convert(mess,8,size_of_read_header);
            }
            else
              Error_response(ILLEGAL_DATA_ADDRESS);
          }
          break;
          case Wcoil:
          {
            //packet_data_8_t &m_packet_coil =
              // *(packet_data_8_t*)(mess+size_of_header+size_of_read_header);
            //DectoBin(m_coils,m_sec_head.starting_address,m_packet_coil);
            if(IRS_HIBYTE(m_sec_head.num_of_regs) == 0xFF)
              m_coils[m_sec_head.starting_address] = irs_true;
            else
              m_coils[m_sec_head.starting_address] = irs_false;
            m_size_end = size_of_MBAP;
            convert(mess,0,size_of_MBAP);
            convert(mess,8,size_of_read_header);
          }
          break;
          case WMcoils:
          {
            packet_data_8_t &m_dop_head =
              *(packet_data_8_t*)(mess+size_of_header+size_of_read_header);
            for(irs_u16 t=0; t<(size_of_MBAP + m_cmb_head.length);t++)
              OUTDBG(cout << (int)mess[t]<< ' ');
            OUTDBG(cout <<"\n");
            for(irs_u8 i = 0;i<m_dop_head.byte_count;i++)
              OUTDBG(cout <<(int) m_dop_head.value[i]<< ' ');
            OUTDBG(cout << "\n");
            OUTDBG(cout<<"Byte count: "<<(int)m_dop_head.byte_count<<"\n");
            OUTDBG(cout<<"Starting address: "<< m_sec_head.starting_address<<"****\n");
            DectoBin(m_coils,m_sec_head.starting_address,m_dop_head);
            for(irs_u16 i = m_sec_head.starting_address;
              i<(m_sec_head.num_of_regs+m_sec_head.starting_address);i++)
              OUTDBG(cout <<m_coils[i]);
            OUTDBG(cout << "\nEND\n");
            m_cmb_head.length = size_of_res*2;
            m_size_end = m_cmb_head.length;
            convert(mess,0,size_of_MBAP);
            convert(mess,8,size_of_read_header);
          }
          break;
          case WM_regs:
          {
            //request_wr_regs_t &m_dop_head =
              //*(request_wr_regs_t*)(mess+size_of_header+size_of_read_header);
            for(irs_u16 t=0; t<size_of_packet;t++)
              OUTDBG(cout << (int)mess[t]<< ' ');
            OUTDBG(cout << "\n");
            request_wr_coils_t &m_dop_head =
              *(request_wr_coils_t*)(mess+size_of_header+size_of_read_header);
            convert(
              mess,
              irs_u8(size_of_header+size_of_read_header+1),
              irs_u8(m_sec_head.num_of_regs*2)
            );
            for(irs_u8 i = 0;i<m_sec_head.num_of_regs*2;i++)
              OUTDBG(cout << (int)m_dop_head.value[i]<< ' ');
            OUTDBG(cout << "\n");
            range(m_staddr*2,m_sec_head.num_of_regs*2,0,hr_inbytes,&hr_size,&hr_start);
            //copy(m_dop_head.value,m_dop_head.value + m_sec_head.num_of_regs,
              //m_hold_regs.begin()+m_sec_head.starting_address);
            write_bytes(m_hold_regs,hr_start,hr_size,m_dop_head.value,0);
            OUTDBG(cout<<"---Starting address: "<< m_sec_head.starting_address<<"\n");
            for(irs_u16 i = m_sec_head.starting_address;
              i<(m_sec_head.num_of_regs+m_sec_head.starting_address);i++)
              OUTDBG(cout << m_hold_regs[i]<< ' ');
            OUTDBG(cout <<"\n");
            m_cmb_head.length = size_of_res*2;
            m_size_end = m_cmb_head.length;
            convert(mess,0,size_of_MBAP);
            convert(mess,8,size_of_read_header);
          }
          break;
          case W_HR:
          {
            wr_sreg_t &m_dop_head =
              *(wr_sreg_t*)(mess+size_of_header);
            hr_size = 2;
            //convert(mess,size_of_header+size_of_read_header+1,
              //m_sec_head.num_of_regs*2);
            range(m_staddr*2,2,0,hr_inbytes,&hr_size,&hr_start);
            write_bytes(m_hold_regs,hr_start,hr_size,m_dop_head.value,0);
            m_size_end = 6;
            convert(mess,0,size_of_MBAP);
            convert(mess,8,size_of_read_header);
          }
          break;
          case Status:
          {

            OUTDBG(cout << "*******************Discret Inputs*******"<<
              di_inbits<<"**************\n");
            for(irs_u16 i = 0; i< di_inbits;i++)
            {
                OUTDBG(cout << m_discr_inputs[i]);
            }
            OUTDBG(cout <<"\n********************************************"
              "*********\n");
            OUTDBG(cout <<"******************COILS**************"<<
              coils_inbits<<"******************\n");
            for(irs_u16 i = 0; i< coils_inbits;i++)
            {
                OUTDBG(cout << m_coils[i]);
            }
            OUTDBG(cout <<"\n*****************************************"
              "************\n");
            OUTDBG(cout << "**************Holding registers************"
              "***********\n");
            for(irs_u16 i = 0; i< irs_u16(hr_inbytes/2);i++)
            {
                OUTDBG(cout <<hex_u16<<(int)m_hold_regs[i]<<' ');
            }
            OUTDBG(cout <<"\n******************************************"
              "***********\n");
            OUTDBG(cout << "**************Input registers***************"
              "**********\n");
            for(irs_u16 i = 0; i< irs_u16(ir_inbytes/2);i++)
            {
                OUTDBG(cout <<hex_u16<<(int)m_input_regs[i]<<' ');
            }
            OUTDBG(cout<<"\n*************************************\n");
            convert(mess,0,size_of_MBAP);
            convert(mess,8,size_of_read_header);
            m_size_end = size_of_MBAP;
            for(irs_u16 t = 0; t<12;t++)
              OUTDBG(cout << hex_u8<< ((int)(mess[t])) << ' ');
            OUTDBG(cout <<"\n");

            //cin.get();
          }
          break;
          case Rsize:
          {
          }
          break;
          case Rtab:
          {
          }
          break;
          default:
          {
            Error_response(ILLEGAL_FUNCTION);
          }
          break;
        }
        mode = write_mode;
      }
      //else
        //mode = read_mode;
    }
    break;
    case write_end:
    {
      if(mxifa_write_end(mp_handle,irs_false))
      {
        mode = read_mode;
        memset(mess,0,sizeof(mess));
        m_mbchdata.mxifa_linux_dest.ip      = zero_ip;
        m_mbchdata.mxifa_linux_dest.port    = 0;
      }
    }
    break;
    case wait_mode:
    {
    }
  }
  mxifa_tick();
}

irs::irsmb_cl::irsmb_cl(
  mxifa_ch_t channel,
  mxip_t dest_ip,
  irs_u16 dest_port,
  irs_u16 a_discr_inputs_size,
  irs_u16 a_coils_size,
  irs_u16 a_hold_regs,
  irs_u16 a_input_regs
):
  m_rforwr(a_coils_size*8 + a_hold_regs*2),
  m_rforwr_wait(a_coils_size*8 + a_hold_regs*2),
  m_discr_inputs_r(a_discr_inputs_size*8),
  m_coils_r(a_coils_size*8),
  m_coils_w(a_coils_size*8),
  m_input_regs_r(a_input_regs),
  m_hold_regs_r(a_hold_regs),
  m_hold_regs_w(a_hold_regs)
{
  di_size         = 0;
  di_start        = 0;
  coils_size      = 0;
  coils_start     = 0;
  hr_size         = 0;
  hr_start        = 0;
  ir_size         = 0;
  ir_start        = 0;
  m_ibank         = 0;
  m_global_index  = 0;
  m_size_end      = 0;
  m_read_table    = 0;
  m_nregs         = 0;
  m_start_block   = 0;
  search_index    = 0;
  m_bytes         = 0;
  m_stpos         = 0;
  init_to_cnt();
  del_time = TIME_TO_CNT(1, 5);
  set_to_cnt(wait,del_time);
  m_command       = R_DI;
  m_mbchdata.mxifa_linux_dest.ip      = dest_ip;
  m_mbchdata.mxifa_linux_dest.port    = dest_port;
  coils_inbits    = irs_u16(a_coils_size*8);
  di_inbits       = irs_u16(a_discr_inputs_size*8);
  hr_inbytes      = a_hold_regs*2;
  ir_inbytes      = a_input_regs*2;
  di_end          = a_discr_inputs_size;
  coils_end       = a_discr_inputs_size + a_coils_size;
  hr_end          = a_discr_inputs_size + a_coils_size + a_hold_regs*2;
  ir_end          = a_discr_inputs_size + a_coils_size + a_hold_regs*2
                  + a_input_regs*2;
  m_channel       = channel;
  m_dport         = dest_port;
  m_dip           = dest_ip;
  mp_buf          = IRS_NULL;
  m_part_send     = irs_false;
  m_mode          = get_table;
  m_nothing       = irs_true;
  RT              = R_DI;
  m_first_read    = irs_true;
  memset(m_spacket,0,sizeof(m_spacket));
  memset(m_rpacket,0,sizeof(m_rpacket));
  memset(in_str,0,sizeof(in_str));
  mxifa_init();
  mp_handle = mxifa_open(channel,irs_false);
  mxifa_linux_tcp_ip_cl_cfg cfg;
  mxifa_get_config(mp_handle, (void *)&cfg);
  cfg.dest_port = dest_port;
  cfg.dest_ip = dest_ip;
  mxifa_set_config(mp_handle, (void *)&cfg);
}
irs::irsmb_cl::~irsmb_cl()
{
  deinit_to_cnt();
}
irs_uarc irs::irsmb_cl::size()
{
  return ir_end;
}
void irs::irsmb_cl::status()
{
}
irs_bool irs::irsmb_cl::connected()
{
  if(m_first_read)
    return irs_false;
  else
    return irs_true;
}
irs_bool irs::irsmb_cl::connected_1()
{
  return irs_true;
}
void irs::irsmb_cl::write_bytes(vector<irs_u16> &vec,irs_u32 index,
  irs_u32 size,const irs_u8 *buf,irs_u32 start)
{
  //OUTDBG(cout <<"Start " <<start<<'\n');
  //OUTDBG(cout << "**************************Write bytes************************\n");
  if(((index%2) == 0)&&((size%2)==0))
  {
    //OUTDBG(cout << " OK\n");
    pack_tb_t &head = *(pack_tb_t *)(buf+start);
    copy(head.value,head.value + size/2,vec.begin()+index/2);
  }
  if(((index%2) == 0)&&((size%2)!=0))
  {
    pack_tb_t &head = *(pack_tb_t *)(buf+start);
    copy(head.value,head.value + size/2,vec.begin()+index/2);
    IRS_LOBYTE(vec[index/2+size/2]) = buf[start + size -1];
  }
  if(((index%2) != 0)&&((size%2)==0))
  {
    pack_tb_t &head = *(pack_tb_t *)(buf + start + 1);
    IRS_HIBYTE(vec[index/2]) = buf[start];
    copy(head.value,head.value + size/2 - 1,
      vec.begin()+index/2+1);
    IRS_LOBYTE(vec[index/2+size/2]) = buf[start + size-1];
  }
  if(((index%2) != 0)&&((size%2)!=0))
  {
    pack_tb_t &head = *(pack_tb_t *)(buf + start + 1);
    IRS_HIBYTE(vec[index/2]) = buf[start];
    copy(head.value,head.value + size/2,
      vec.begin()+index/2+1);
  }
  #ifdef nop2
  for(irs_u16 i = (index/2); i<(size/2+index/2);i++)
  {
    OUTDBG(cout <<dec<<"Vector["<<(int)i<<"] = "<< hex<<showbase <<(int)vec[i]<<'\n');
  }
  OUTDBG(cout <<"\n");
  for(irs_u16 i = (start); i<(size+start);i++)
  {
    OUTDBG(cout <<dec<<"Buf["<<(int)i<<"] = "<<hex<<showbase <<(int)buf[i]<<'\n');
  }
  OUTDBG(cout <<"\n");
  #endif
}

void irs::irsmb_cl::get_bytes(vector<irs_u16> &vec,irs_u32 index,irs_u32 size,
  irs_u8 *buf,irs_u32 start)
{
  if(((index%2) == 0)&&((size%2)==0))
  {
    //OUTDBG(cout << " OK\n");
    pack_tb_t &head = *(pack_tb_t *)(buf+start);
    copy(vec.begin()+index/2,vec.begin()+index/2+size/2,head.value);
  }
  if(((index%2) == 0)&&((size%2)!=0))
  {
    pack_tb_t &head = *(pack_tb_t *)(buf+start);
    copy(vec.begin()+index/2,vec.begin()+index/2+size/2,head.value);
    buf[size-1+start] = IRS_LOBYTE(vec[index/2+size/2]);
  }
  if(((index%2) != 0)&&((size%2)==0))
  {
    pack_tb_t &head = *(pack_tb_t *)(buf + 1 + start);
    buf[start] = IRS_HIBYTE(vec[index/2]);
    copy(vec.begin()+index/2+1,vec.begin()+index/2+size/2,head.value);
    buf[size-1+start] = IRS_LOBYTE(vec[index/2+size/2]);
  }
  if(((index%2) != 0)&&((size%2)!=0))
  {
    pack_tb_t &head = *(pack_tb_t *)(buf+1+start);
    buf[start] = IRS_HIBYTE(vec[index/2]);
    copy(vec.begin()+index/2+1,vec.begin()+index/2+size/2+1,head.value);
  }
  #ifdef Nop4
  OUTDBG(cout << "**************************Get bytes************************\n");
  for(irs_u16 i = (index/2); i<(size/2+index/2);i++)
  {
    OUTDBG(cout <<dec<<"Vector["<<(int)i<<"] = "<< hex<<showbase <<(int)vec[i]<<'\n');
  }
  OUTDBG(cout <<"\n");
  for(irs_u16 i = (start); i<(size+start);i++)
  {
    OUTDBG(cout <<dec<<"Buf["<<(int)i<<"] = "<<hex<<showbase <<(int)buf[i]<<'\n');
  }
  OUTDBG(cout <<"\n");

  //OUTDBG(cout << " OK\n");
  if(buf[0] == 0)
  {
    OUTDBG(cout <<"Index: "<<dec<<(int)index<<"\n");
    OUTDBG(cout <<"Size: "<<dec <<(int)size<<"\n");
    OUTDBG(cout <<"Vec[" <<dec<<(int)(index/2)<<"] = "<< vec[index/2]<<"\n");
  }
  #endif
  //for(irs_u16 i = start; i<(size+start);i++)
  //{
    //OUTDBG(cout <<hex<<"Buf["<<(int)i<<"] = 0x"<< (int)buf[i]<<' ');
  //}
  //OUTDBG(cout <<"\n");
}
void irs::irsmb_cl::range(irs_u32 index, irs_u32 size, irs_u32 M1, irs_u32 M2,
  irs_u32 *num,irs_u32 *start)
{
  irs_u32 var = index + size;
  if((index>=M1)&&(index<M2)&&(var>M1)&&(var <= M2))
  {
    *num = size;
    *start = index - M1;
  }
  else if((index>M1)&&(index<M2)&&(var>M2))
  {
    *num = M2 - index;
    *start = index - M1;
  }
  else if((index<M1)&&(var>M1)&&(var < M2))
  {
    *num = var - M1;
    *start = 0;
  }
  else if((index<=M1)&&(var >= M2))
  {
    *num = M2 - M1;
    *start = 0;
  }
}
void irs::irsmb_cl::read(irs_u8 *ap_buf, irs_uarc index, irs_uarc a_size)
{
  di_size = 0; di_start = 0;
  coils_size = 0; coils_start = 0;
  hr_size = 0; hr_start = 0;
  ir_size = 0; ir_start = 0;
  range(index,a_size,0,di_end,&di_size,&di_start);
  range(index,a_size,di_end,coils_end,&coils_size,&coils_start);
  range(index,a_size,coils_end,hr_end,&hr_size,&hr_start);
  range(index,a_size,hr_end,ir_end,&ir_size,&ir_start);
  irs_u16 i = 0;
  //OUTDBG(cout << "Index: " << index<<'\n');
  if((di_size != 0)||(di_start != 0))
  {
    //OUTDBG(cout << "DI size: "<<di_size<<" DI_start: "<< di_start<< "\n");
    packet_data_8_t &m_packet_di = *(packet_data_8_t *)in_str;
    m_packet_di.byte_count = irs_u8(di_size);
    BintoDec(m_discr_inputs_r,di_start*8,m_packet_di);
    for(;i<(irs_u16)di_size;i++)
      ap_buf[i] = m_packet_di.value[i];
  }
  if((coils_size != 0)||(coils_start != 0))
  {
    //OUTDBG(cout << "coils_size: "<<coils_size<<" coils_start: "<< coils_start<< "\n");
    packet_data_8_t &m_packet_coils = *(packet_data_8_t *)in_str;
    m_packet_coils.byte_count = irs_u8(coils_size);
    BintoDec(m_coils_r,coils_start*8,m_packet_coils);
    for(;i<irs_u16(coils_size + di_size);i++)
    {
      ap_buf[i] = m_packet_coils.value[i-di_size];
      //OUTDBG(cout << (int)m_packet_coils.value[i-di_size]<<' ');
    }
    //OUTDBG(cout << "\n");
  }
  if((hr_size != 0)||(hr_start != 0))
  {
    //OUTDBG(cout << "HR size: "<<hr_size<<" HR_start: "<< hr_start<< "\n");
    //irs_u8 *bufer;
    get_bytes(m_hold_regs_r,hr_start,hr_size,ap_buf,i);
    //for(irs_u16 i = 0; i < hr_size;i++)
      //OUTDBG(cout <<hex<<showbase<< (int)ap_buf[i] << ' ');
    //OUTDBG(cout <<dec<<noshowbase<< "\n");
  }
  if((ir_size != 0)||(ir_start != 0))
  {
    //OUTDBG(cout << "IR_size: "<<ir_size<<" IR_start: "<< ir_start<< "\n");
    get_bytes(m_input_regs_r,ir_start,ir_size,ap_buf,i+hr_size);
    //OUTDBG(cout << "Regs\n");
    //for(irs_u16 i = ir_start/2; i< (ir_size/2+ir_start/2);i++)
        //OUTDBG(cout <<hex<<showbase<< (int)m_input_regs_r[i]<<' ');
    //OUTDBG(cout <<dec<<noshowbase<< "\n");
  }
}
void irs::irsmb_cl::write(const irs_u8 *buf, irs_uarc index, irs_uarc size)
{
  irs_u32 coils_size = 0, coils_start = 0;
  irs_u32 hr_size = 0, hr_start = 0;
  range(index,size,di_end,coils_end,&coils_size,&coils_start);
  range(index,size,coils_end,hr_end,&hr_size,&hr_start);
  //irs_u16 i = 0;
  if((coils_size != 0)||(coils_start != 0))
  {
    //OUTDBG(cout << "coils_size: "<<coils_size<<" coils_start: "<< coils_start<< "\n");
    packet_data_8_t &m_pack_coils = *(packet_data_8_t *)(buf-1);
    m_pack_coils.byte_count = irs_u8(coils_size);
    //if(m_nothing)
    //{
      DectoBin(m_coils_w,coils_start*8,m_pack_coils);
      for(irs_u16 i = irs_u16(coils_start*8);
        i < irs_u16(coils_start*8+coils_size*8);i++)
      {
        m_rforwr[i] = 1;
        m_rforwr_wait[i] = 1;

        //OUTDBG(cout <<dec<<noshowbase<<"Vector["<<(int)i<<"] = "<< m_rforwr[i]<<'\n');
      }
    //}
    //for(irs_u16 i = hr_start; i< (hr_size+hr_start);i++)
    //{
      //OUTDBG(cout << (int)m_coils_w[i] << ' ');
    //}
    //OUTDBG(cout <<dec<<noshowbase<< "\n");
  }
  if((hr_size != 0)||(hr_start != 0))
  {
    //OUTDBG(cout << "HR size: "<<hr_size<<" HR_start: "<< hr_start<< "\n");
    //if(m_nothing)
    //{
      write_bytes(m_hold_regs_w,hr_start,hr_size,buf,coils_size);
      for(irs_u32 i = (hr_start+coils_inbits); i < (hr_size+coils_inbits+hr_start);i++)
      {
        m_rforwr[i] = 1;
        m_rforwr_wait[i] = 1;
        //OUTDBG(cout <<dec<<noshowbase<<"Vector["<<(int)i<<"] = "<< m_rforwr[i]<<'\n');
      }
    //}
    //for(irs_u16 i = hr_start/2; i< (hr_size/2+hr_start/2);i++)
    //{
      //OUTDBG(cout <<hex<<showbase<< (int)m_hold_regs_w[i] << ' ');
    //}
    //OUTDBG(cout <<dec<<noshowbase<< "\n");
  }
}

irs_bool irs::irsmb_cl::bit(irs_uarc index, irs_uarc a_bit_index)
{
  hr_size = 0;hr_start = 0;
  coils_size = 0;coils_start = 0;
  di_start = 0; di_size = 0;
  ir_start = 0; ir_size = 0;
  range(index,1,0,di_end,&di_size,&di_start);
  range(index,1,di_end,coils_end,&coils_size,&coils_start);
  range(index,1,coils_end,hr_end,&hr_size,&hr_start);
  range(index,1,hr_end,ir_end,&ir_size,&ir_start);
  irs_uarc byte = 0;
  //irs_u16 i = 0;
  if((di_size != 0)||(di_start != 0))
    return m_discr_inputs_r[di_start*8+a_bit_index];
  if((coils_size != 0)||(coils_start != 0))
    return m_coils_r[coils_start*8+a_bit_index];
  if (a_bit_index < 8) {
    if((hr_size != 0)||(hr_start != 0))
    {
      byte = m_hold_regs_r[ir_start/2];
      if(byte&=(1<<a_bit_index))
        return irs_true;
      else
        return irs_false;
    }
    if((ir_size != 0)||(ir_start != 0))
    {
      byte = m_input_regs_r[hr_start/2];
      if(byte &= (1<<a_bit_index))
        return irs_true;
      else
        return irs_false;
    }
  }
  OUTDBG(
    if((coils_size == 0)&&(di_size == 0)&&(hr_size == 0)&&(ir_size == 0))
      cout << "\n***************Illegal index or bufer size******************"
        "\n\n"
  );
  return irs_false;
}
void irs::irsmb_cl::set_bit(irs_uarc index, irs_uarc a_bit_index)
{
  hr_size = 0;
  hr_start = 0;
  coils_size = 0;
  coils_start = 0;
  range(index,1,di_end,coils_end,&coils_size,&coils_start);
  range(index,1,hr_end,ir_end,&hr_size,&hr_start);
  irs_uarc byte = 0;
  //irs_u16 i = 0;
  if (a_bit_index < 8) {
    if((coils_size != 0) || (coils_start != 0))
    {
      m_coils_w[coils_start*8+a_bit_index] = 1;
      m_rforwr[coils_start*8+a_bit_index] = 1;
      m_rforwr_wait[coils_start*8+a_bit_index] = 1;
    }
    if((hr_size != 0) || (hr_start != 0))
    {
      byte = m_hold_regs_r[hr_start/2];
      byte |= (1<<a_bit_index);
      m_hold_regs_w[hr_start/2] = irs_u16(byte);
      m_rforwr[hr_start + coils_inbits] = 1;
      m_rforwr_wait[hr_start + coils_inbits] = 1;
    }
  }

  OUTDBG(
    if((coils_size == 0)&&(hr_size == 0))
      cout<<"ERROR\n"
  );
}
void irs::irsmb_cl::clear_bit(irs_uarc index, irs_uarc a_bit_index)
{
  hr_size = 0;
  hr_start = 0;
  coils_size = 0;
  coils_start = 0;
  range(index,1,di_end,coils_end,&coils_size,&coils_start);
  range(index,1,hr_end,ir_end,&hr_size,&hr_start);
  irs_uarc byte = 0;
  //irs_u16 i = 0;
  if (a_bit_index < 8) {
    if((coils_size != 0)||(coils_start != 0))
    {
      m_coils_w[coils_start*8+a_bit_index] = 0;
      m_rforwr[coils_start*8+a_bit_index] = 1;
      m_rforwr_wait[coils_start*8+a_bit_index] = 1;
    }
    if((hr_size != 0)||(hr_start != 0))
    {
      byte = m_hold_regs_r[hr_start/2];
      byte&=~(1<<a_bit_index);
      m_hold_regs_w[hr_start/2] = irs_u16(byte);
      m_rforwr[hr_start + coils_inbits] = 1;
      m_rforwr_wait[hr_start + coils_inbits] = 1;
    }
  }
  OUTDBG(
    if((coils_size == 0)&&(hr_size == 0))
      cout<<"ERROR\n"
  );
}

void irs::irsmb_cl::BintoDec(vector<bool> &bits,irs_u32 index,
  packet_data_8_t &data)
{
  irs_u8 mass = 0;
  //unsigned e = 1;
  irs_u8 tmp = 0;
  unsigned int i = 0;
  //OUTDBG(cout << "\n"<<(int)index<<"\n");
  //OUTDBG(cout << (int)data.byte_count<<"\n");
  //OUTDBG(cout<<"\n--------------Function--Bin to Dec-------------------- \n");
  for(irs_u32 y = index; y< (index+data.byte_count*8);y++)
  {
    if(bits[y])
      mass |=  irs_u8(1<<i);
    i++;
    if(i == 8) {
      i = 0;
      data.value[tmp] = mass;
      mass = 0;
      tmp++;
    }
  }
  //OUTDBG(cout<<"--------------------------END-------------------------\n");
}

void irs::irsmb_cl::DectoBin(vector<bool> &bits,irs_u32 index,
  const packet_data_8_t &data)
{
  irs_u8 copy = 0;
  //OUTDBG(cout << "---------------Function Dec to Bin---------------------\n");
  //OUTDBG(cout <<"Start position:"<< index<<"\n");
  //OUTDBG(cout <<"Number of registers:" <<(int)data.byte_count<<"\n");
  for(irs_u8 y = 0; y < data.byte_count; y++)
  {
    copy = data.value[y];
    //OUTDBG(cout << (int)data.value[y]<<' ');
    for(irs_u8 x = 0; x<8;x++)
    {
      bits[index + y*8 + x] = (1&copy);
      copy>>=1;
    }
  }
  //OUTDBG(cout << "\n---------------------END------------------------------\n");
}
void irs::irsmb_cl::make_packet(irs_uarc a_index, irs_u16 a_size)
{
  complex_modbus_header_t &m_cmb_head = *(complex_modbus_header_t *)m_spacket;
  request_read_t &m_sec_head = *(request_read_t *)(m_spacket+size_of_header);
  m_bytes = 0;
  m_size_end = 0;
  switch(m_command)
  {
    case WMcoils:
    {
      if(a_size%8 == 0)
         m_bytes = irs_u16(a_size/8 + 1);
      else
         m_bytes = irs_u16(a_size/8 + 2);
    }
    break;
    case WM_regs:
    {
      m_bytes = irs_u16(a_size*2 + 1);
      convert(m_spacket,size_of_header + size_of_read_header+1,(irs_u8)m_bytes);
    }
    break;
    default : {
    }
  }
  m_cmb_head.transaction_id   = IRS_NULL;
  m_cmb_head.proto_id         = IRS_NULL;
  m_cmb_head.length           =
    irs_u16(size_of_header+size_of_read_header+m_bytes-size_of_MBAP);
  m_cmb_head.unit_identifier  = 1;
  m_cmb_head.function_code    = m_command;
  m_sec_head.starting_address = (irs_u16)a_index;
  m_sec_head.num_of_regs      = a_size;
  m_stpos = m_sec_head.starting_address;
  m_size_end = m_cmb_head.length;
  //for(irs_u16 t = 0; t<(m_cmb_head.length+size_of_MBAP);t++)
    //OUTDBG(cout << hex_u8<< ((int)(m_spacket[t])) << ' ');
  //OUTDBG(cout << "\n");

  convert(m_spacket,0,size_of_MBAP);
  convert(m_spacket,size_of_header,size_of_read_header);
  //for(irs_u16 t = 0; t<13;t++)
    //OUTDBG(cout << hex_u8<< ((int)(m_spacket[t])) << ' ');
  //OUTDBG(cout << "\n");
  //OUTDBG(cout <<"Packet ready\n");
}
void irs::irsmb_cl::convert(irs_u8 *ap_mess,irs_u8 a_start,irs_u8 a_length)
{
  irs_u8 var = 0;
  for(irs_u8 i = a_start; i < (a_length+a_start);i+=irs_u8(2))
  {
    var = ap_mess[i+1];
    ap_mess[i+1] = ap_mess[i];
    ap_mess[i] = var;
  }
}
void irs::irsmb_cl::set_delay_time(double time)
{
   del_time = DBLTIME_TO_CNT(time);
}
void irs::irsmb_cl::tick()
{
  if(connected_1())
  {
    switch(m_mode)
    {
      case get_table:
      {
        switch(RT)
        {
          case R_DI:
          {
            m_command = RT;

            if((m_global_index + size_of_data) < di_inbits)
            {
              make_packet(m_global_index,size_of_data*8);
              m_global_index += irs_u16(size_of_data*8);
              m_ibank += size_of_data;
              m_mode = wait_mode;
            }
            else
            {
              make_packet(m_global_index,irs_u16(di_inbits-m_global_index));
              m_ibank += (di_inbits-m_global_index)/8;
              m_global_index = 0;
              RT = Rcoil;
              m_mode = wait_mode;
            }
          }
          break;
          case Rcoil:
          {
            m_command = RT;
            if((m_global_index + size_of_data*8) < coils_inbits)
            {
              make_packet(m_global_index,(size_of_data)*8);
              m_global_index += irs_u16(size_of_data*8);
              m_ibank += size_of_data;
              m_mode = wait_mode;
            }
            else
            {
              make_packet(m_global_index,irs_u16(coils_inbits-m_global_index));
              m_ibank += (coils_inbits-m_global_index)/8;
              m_global_index = 0;
              RT = R_HR;
              m_mode = wait_mode;
            }
          }
          break;
          case R_HR:
          {
            m_command = RT;
            if(irs_u32(m_global_index + s_regbuf) < irs_u32(hr_inbytes/2))
            {
              make_packet(m_global_index,s_regbuf);
              m_global_index += s_regbuf;
              m_ibank += s_regbuf;
              m_mode = wait_mode;
            }
            else
            {
              make_packet(m_global_index,
                irs_u16(hr_inbytes/2u-(irs_u32)m_global_index));
              m_ibank += ((hr_inbytes/2u)-m_global_index);
              m_global_index = 0;
              RT = R_IR;
              m_mode = wait_mode;
            }
          }
          break;
          case R_IR:
          {
            m_command = RT;
            if(irs_u32(m_global_index + s_regbuf) < irs_u32(ir_inbytes/2))
            {
              make_packet(m_global_index,s_regbuf);
              m_global_index += s_regbuf;
              m_ibank += s_regbuf;
              m_mode = wait_mode;
            }
            else
            {
              make_packet(m_global_index,
                irs_u16((ir_inbytes/2) - m_global_index));
              m_ibank += ((ir_inbytes/2) - m_global_index);
              m_global_index = 0;
              RT = ERT;
              m_mode = wait_mode;
            }
          }
          break;
          case ERT:
          {
            m_ibank = 0;
            m_first_read = irs_false;
            //OUTDBG(cout << "We read entire table\n");
            m_global_index = 0;
            m_command = Status;
            //#ifdef NOP

            OUTDBG(cout << "**********************Discret Inputs*******************\n");
            for(irs_u16 i = 0; i<  di_inbits;i++)
            {
                OUTDBG(cout << m_discr_inputs_r[i]);
            }
            OUTDBG(cout << "\n*************************************************\n");
            OUTDBG(cout << "**************COILS********************************\n");
            for(irs_u16 i = 0; i< coils_inbits;i++)
            {
                OUTDBG(cout << m_coils_r[i]);
            }
            OUTDBG(cout << "\n**************************************************\n");
            OUTDBG(cout << "************Holding registers***********************\n");
            for(irs_u16 i = 0; i< irs_u16(hr_inbytes/2);i++)
            {
                OUTDBG(cout <<hex_u16<< (int)m_hold_regs_r[i]<<' ');
            }
            OUTDBG(cout << "\n****************************************************\n");
            OUTDBG(cout << "**************Input registers*************************\n");
            for(irs_u16 i = 0; i<irs_u16(ir_inbytes/2) ;i++)
            {
                OUTDBG(cout <<hex_u16<< (int)m_input_regs_r[i]<<' ');
            }
            OUTDBG(cout <<hex_u16<< "\n***********************************\n");
            //#endif
            //cin.get();
            make_packet(m_global_index,0);
            RT = R_DI;

            m_mode = wait_mode;
          }
          break;
          default : {
          }
        }
        m_read_table = irs_false;
      }
      break;
      case w_mode:
      {
        OUTDBG(cout << "Write mode\n");
        for(irs_u16 t = 0; t<irs_u16(size_of_MBAP + m_size_end);t++)
          OUTDBG(cout << "(" << ((int)(m_spacket[t])) << ") ");
        OUTDBG(cout << "\nPacket: "<< ((int)m_spacket) <<"\n");
        mxifa_write_begin(mp_handle,&m_mbchdata,m_spacket,size_of_MBAP + m_size_end);
        m_mode = wend_w;
        //set_delay_time(WAIT_LIENT_TIME);
        set_to_cnt(wait, del_time);
      }
      break;
      case wend_w:
      {
        if(mxifa_write_end(mp_handle,irs_false))
        {
          //OUTDBG(cout << "Write end\n");
            m_mode = r_head_mode;
        }
      }
      break;
      case r_head_mode:
      {
          mxifa_read_begin(mp_handle,&m_mbchdata,m_rpacket,size_of_MBAP);
          m_mode = r_pack_mode;
      }
      break;
      case r_pack_mode:
      {
        if(mxifa_read_end(mp_handle,irs_false))
        {
          convert(m_rpacket,0,size_of_MBAP);
          for(irs_u16 t = 0; t<size_of_MBAP;t++)
            OUTDBG(cout << hex_u8<< ((int)(m_rpacket[t])) << ' ');
          OUTDBG(cout<<"\n");
          complex_modbus_header_t &m_cmb_head =
            *(complex_modbus_header_t *)m_rpacket;
          pack_d8_t &raw_bytes = *(pack_d8_t *)(m_rpacket+size_of_MBAP);
          mxifa_read_begin(mp_handle,&m_mbchdata,raw_bytes.value,m_cmb_head.length);
          set_to_cnt(rtimeout,cl_rtime);
          m_mode = wend_r;
        }
      }
      break;
      case wend_r:
      {
        OUTDBG(cout<<"End of operation read\n");
        if(mxifa_read_end(mp_handle,irs_false))
        {
          convert(m_spacket,8,size_of_read_header);
          for(irs_u16 t = 0; t<(size_of_MBAP*2);t++)
            OUTDBG(cout << hex_u8<< ((int)(m_rpacket[t])) << ' ');
          OUTDBG(cout<<"\n");

          //mp_buf = m_rpacket;
          //OUTDBG(cout << "Packet: " << (int)mp_buf << "\n");
          //memcpy((void *)&mb_rpacket,(void *)&m_rpacket,12);
          m_mode = pr_mode;
        }
        if(test_to_cnt(rtimeout))
        {
          if(m_read_table)
            m_mode = get_table;
          else
            m_mode = search_block;
          if(m_part_send)
            m_mode = send_data;
          if(m_first_read)
            m_mode = get_table;
        }
      }
      break;
      case pr_mode:
      {
        //copy(m_packet.begin(),m_packet.begin()+size_of_header,&m_cmb_head);
        //memcpy(&m_cmb_head,m_rpacket,size_of_header);
        complex_modbus_header_t &m_cmb_head =
          *(complex_modbus_header_t *)m_rpacket;
        //request_read_t &m_head_s = *(request_read_t *)m_spacket;
        //OUTDBG(cout << "Recieved header - transaction_id:    " <<
          //m_cmb_head.transaction_id << "\n");
        //OUTDBG(cout << "Recieved header - proto_id:          " <<
          //m_cmb_head.proto_id << "\n");
        OUTDBG(cout << "Send packet - length:            " <<
          hex_u16 << m_cmb_head.length << "\n");
        OUTDBG(cout << "Send packet - unit_identifier:   " <<
          hex_u16 << (int)m_cmb_head.unit_identifier  << "\n");
        OUTDBG(cout << "Send packet - function_code:     " <<
          hex_u16 << (int)m_cmb_head.function_code << "\n");
        //OUTDBG(cout << "Recieved header - starting_address:  " <<
          //m_cmb_head.starting_address << "\n");

        for(irs_u16 i = 0;i<(m_cmb_head.length + size_of_MBAP);i++)
          OUTDBG(cout<<hex_u8<<(int)m_rpacket[i]<<' ');
        OUTDBG(cout << "\n");
        OUTDBG(cout << "Length of whole send paket:"<<(m_cmb_head.length + size_of_MBAP)<<"\n");
        //OUTDBG(cout << "M_ibank:"<<m_ibank<<'\n');
        //OUTDBG(cout << "Read table status : "<<
          //(float)((float)m_ibank/(float)(coils_inbits/8+di_inbits/8+
          //hr_inbytes/2+ir_inbytes/2))*100 << "%"<<'\n');
        switch(m_cmb_head.function_code)
        {
          case Rcoil:
          {
            packet_data_8_t &m_pack_coils =
              *(packet_data_8_t *)(m_rpacket + size_of_header);
            DectoBin(m_coils_r,m_stpos,m_pack_coils);
            //for(irs_u16 i = m_stpos;i<(m_stpos+m_pack_coils.byte_count*8);i++ )
              //OUTDBG(cout<<m_coils_r[i]);
            //OUTDBG(cout<<'\n');
          }
          break;
          case R_DI:
          {
            packet_data_8_t &m_pack_di =
              *(packet_data_8_t *)(m_rpacket + size_of_header);
            DectoBin(m_discr_inputs_r,m_stpos,m_pack_di);
          }
          break;
          case R_HR:
          {
            request_wr_coils_t &m_packet_hr =
              *(request_wr_coils_t*)(m_rpacket+size_of_header);
            convert(m_rpacket,size_of_header + 1,m_packet_hr.byte_count);
            range(m_stpos*2,m_packet_hr.byte_count,0,hr_inbytes,&hr_size,&hr_start);
            write_bytes(m_hold_regs_r,hr_start,hr_size,m_packet_hr.value,0);

            //packet_data_16_t &m_packet_hr =
              //*(packet_data_16_t *)(m_rpacket+size_of_header);
            //OUTDBG(cout<<"\n");
            //for(irs_u16 i = 0;i<(m_packet_hr.byte_count);i++)
              //OUTDBG(cout<<hex_u8<<(int)m_packet_hr.value[i]<<' ');
            OUTDBG(cout <<"\n");
            OUTDBG(cout<<"Start position: "<<m_stpos<<"\n");
            OUTDBG(cout<<"Count of regs: "<<(int)m_packet_hr.byte_count<<"\n");
            //copy(m_packet_hr.value,m_packet_hr.value + m_packet_hr.byte_count/2,
              //m_hold_regs_r.begin()+m_stpos);
            for(irs_u16 i = m_stpos;i<(m_stpos + m_packet_hr.byte_count/2);i++)
              OUTDBG(cout<<hex_u16<<(int)m_hold_regs_r[i]<<' ');
            OUTDBG(cout <<"\n");
          }
          break;
          case R_IR:
          {
            request_wr_coils_t &m_packet_ir =
              *(request_wr_coils_t*)(m_rpacket+size_of_header);
            convert(m_rpacket,size_of_header + 1,m_packet_ir.byte_count);
            range(m_stpos*2,m_packet_ir.byte_count,0,ir_inbytes,&ir_size,&ir_start);
            write_bytes(m_input_regs_r,ir_start,ir_size,m_packet_ir.value,0);
          }
          break;
          case WMcoils:
          {
            request_read_t &m_sec_head =
              *(request_read_t*)(m_spacket+size_of_header);
            //OUTDBG(cout << "Recieved header - num_of_regs:       " <<
              //m_sec_head.num_of_regs << "\n");
            for(irs_u16 l = 0; l < m_sec_head.num_of_regs;l++)
            {
              m_rforwr_wait[m_stpos + l] = 0;
              m_rforwr[m_stpos + l] = 0;
              m_coils_w[m_stpos + l] = 0;
              //OUTDBG(cout <<"m_coils_w["<<(m_stpos + l)<<"] = "<<
                //m_coils_w[m_stpos +l]<< '\n');
            }
          }
          break;
          case WM_regs:
          {
            request_read_t &m_sec_head = *
              (request_read_t*)(m_spacket+size_of_header);
            //OUTDBG(cout << "Recieved header - num_of_regs:       " <<
              //m_sec_head.num_of_regs << "\n");
            //OUTDBG(cout<<"Starting address: "<<m_stpos<<"\n");
            for(irs_u16 l = 0; l < (m_sec_head.num_of_regs*2);l++)
            {
              m_rforwr_wait[coils_inbits + m_stpos + l] = 0;
              m_rforwr[coils_inbits + m_stpos + l] = 0;
              m_hold_regs_w[m_stpos + l] = 0;
              //OUTDBG(cout <<hex<<showbase<<"m_hold_regs_w["<<dec<<noshowbase<<
                //(m_stpos + l)<<"] = "
                //<< m_hold_regs_w[m_stpos + l]<< '\n');
            }
            //OUTDBG(cout <<dec<<noshowbase<< "\n");
          }
          break;
          case Status:
          {
          }
          break;
        }
          for(irs_u16 t = 0; t<size_of_packet;t++)
            m_spacket[t] = 0;
        //for(irs_u16 i = 0;i<12;i++)
          //OUTDBG(cout<<hex<<showbase<<(int)m_hold_regs_r[i]<<' ');
        //OUTDBG(cout <<dec<<noshowbase <<"\n");

        if(m_read_table)
          m_mode = get_table;
        else
          m_mode = search_block;
        if(m_part_send)
          m_mode = send_data;
        if(m_first_read)
          m_mode = get_table;
      }
      break;
      case search_block:
      {
        OUTDBG(cout << "!!!!!!!!!!!!!!!!Search Block!!!!!!!!!!!!!!!!!!\n");
        for(irs_u8 i = 0; i < (irs_u8)m_rforwr.size();i++)
          OUTDBG(cout << m_rforwr[i]<<' ');
        OUTDBG(cout << '\n');
        irs_bool catch_block = irs_false;
        //OUTDBG(cout << "ok";
        for(;search_index <(coils_inbits+hr_inbytes);)
        {
          if((m_rforwr[search_index] == irs_true)&&(catch_block == irs_false))
          {
            m_start_block = search_index;
            catch_block = irs_true;
            m_nothing = irs_false;
          }
          if((catch_block == irs_true)&&(m_rforwr[search_index] == irs_false))
          {
            m_nregs = search_index - m_start_block;
            m_mode = send_data;
            if((irs_u32(m_nregs + m_start_block) >= irs_u32(coils_inbits))&&
              (m_start_block < (irs_u32)coils_inbits))
            {
              search_index -= m_nregs;
              m_nregs = coils_inbits - m_start_block;
              search_index += m_nregs;
            }
            //OUTDBG(cout<<"Find block\n");
            //OUTDBG(cout<< "Search index"<<dec<<search_index<<"\n");
            //OUTDBG(cout << "Mstart block "<<m_start_block<<"\n");
            //OUTDBG(cout << "MNregs "<<m_nregs<<"\n");
            break;
         }
         search_index++;
         if((catch_block == irs_true)&&(search_index == (coils_inbits+hr_inbytes)))
         {
            m_nregs = search_index - m_start_block;
            m_mode = send_data;
            search_index = 0;
            break;
         }
          if(search_index >= (coils_inbits+hr_inbytes))
          {
            search_index = 0;
            m_nothing = irs_true;
            m_mode = get_table;
            break;
          }
        }
      }
      break;
      case send_data:
      {
        //OUTDBG(cout << "Send data\n");
        m_read_table = irs_true;
        //range(m_start_block,m_nregs,coils_inbits,hr_inbytes+coils_inbits,
           //&hr_size,&hr_start);
        //irs_u32 coils_size = 0, coils_start = 0;
        //irs_u32 hr_size = 0, hr_start = 0;
        //if((coils_size !=0)&&(hr_size!=0))
          //send_part = irs_true;
        if((m_start_block>=(irs_u32)coils_inbits) &&
          (m_start_block<(coils_inbits + hr_inbytes)))
        {
          //range(m_start_block,m_nregs,bit_msize,reg_msize+bit_msize,
            //&hr_size,&hr_start);
          m_command = WM_regs;
          hr_start = m_start_block - coils_inbits;
          hr_size = m_nregs;
          //OUTDBG(cout << "Size " << hr_size <<"\n");
          //OUTDBG(cout << "Start "<< hr_start<<"\n");
          //make_packet(hr_start,s_regbuf);
          //request_wr_regs_t &m_packet_hr =
            //*(request_wr_regs_t*)(m_spacket+size_of_header+size_of_read_header);
          auto_arr<irs_u16> mass(new irs_u16[hr_inbytes/2]);
          if(hr_size > ((s_regbuf-1)*2))
          {
            request_wr_regs_t &m_packet_hr =
              *(request_wr_regs_t*)(m_spacket+size_of_header+size_of_read_header);
            m_stpos = irs_u16(hr_start/2);
            m_packet_hr.byte_count = (s_regbuf-1)*2;
            irs_u8 rmass[size_of_data];
            irs_u16 nul = 0;
            get_bytes(m_hold_regs_w,hr_start,(s_regbuf-1)*2,rmass,nul);
            pack_tb_t &m_pack_regs = *(pack_tb_t *)rmass;
            //copy(m_hold_regs_w.begin() + hr_start,
              //m_hold_regs_w.begin() + hr_start + hr_size,mass);
            for(irs_u8 i = 0;i<(s_regbuf-1);i++)
            {
              m_packet_hr.value[i] = m_pack_regs.value[i];
              //OUTDBG(cout<<hex<<showbase<<m_packet_hr.value[i]<<"="<<
                //m_pack_regs.value[i]<<' ');
            }
            //OUTDBG(cout <<dec<<noshowbase<< '\n');
            make_packet(hr_start/2,(s_regbuf-1));
            for(irs_u32 i = m_start_block;
              i < (m_start_block +(s_regbuf-1)*2);i++)
              m_rforwr[i] = 0;
            m_start_block += ((s_regbuf-1)*2);
            m_nregs -= ((s_regbuf-1)*2);
            m_part_send = irs_true;
          }
          else
          {
            //copy(m_hold_regs_w.begin() + hr_start/2,
              //m_hold_regs_w.begin() + hr_start/2 + hr_size/2,mass);
            for(irs_u16 t = 0; t<size_of_packet;t++)
              m_spacket[t] = 0;
            if(hr_size%2 == 0)
            {
              packet_data_8_t &m_packet_hr =
                *(packet_data_8_t * )(m_spacket+size_of_header+size_of_read_header);
              if((hr_start%2) !=0)
              {
                irs_u16 nul = 0;
                get_bytes(m_hold_regs_r,hr_start-1,1,m_packet_hr.value,nul);
                m_stpos = irs_u16(hr_start/2);
                //irs_u8 rmass[size_of_data]);
                //pack_tb_t &m_pack_regs = *(pack_tb_t *)rmass;
                m_packet_hr.byte_count = (irs_u8)hr_size;
                get_bytes(m_hold_regs_w,hr_start,hr_size,m_packet_hr.value,1);
                make_packet(hr_start/2,irs_u16((hr_size/2)+1));
              }
              else
              {
                m_stpos = irs_u16(hr_start/2);
                //irs_u8 rmass[size_of_data]);
                irs_u16 nul = 0;
                //pack_tb_t &m_pack_regs = *(pack_tb_t *)rmass;
                m_packet_hr.byte_count = irs_u8(hr_size);
                get_bytes(m_hold_regs_w,hr_start,hr_size,m_packet_hr.value,nul);
                make_packet(hr_start/2,irs_u16(hr_size/2));
              }
              //for(irs_u8 i = 0;i<(hr_size/2);i++)
              //{
                //m_packet_hr.value[i] = m_pack_regs.value[i]);
                //OUTDBG(cout<<hex<<showbase<<(int)m_packet_hr.value[i]);
                  //<<"="<<  (int)m_pack_regs.value[i]<<' ');
                //OUTDBG(cout<<hex<<showbase<<m_packet_hr.value[i]<<"="<<mass[i]<<' ');
              //}
              //for(irs_u16 t = 0; t<size_of_packet;t++)
                //OUTDBG(cout << ((int)(m_spacket[t])) << " ";
              //OUTDBG(cout << "\n");
            }
            else
            {
              packet_data_8_t &m_packet_hr =
                *(packet_data_8_t * )(m_spacket+size_of_header+size_of_read_header);
              if((hr_start%2) !=0)
              {
                irs_u16 nul = 0;
                get_bytes(m_hold_regs_r,hr_start-1,1,m_packet_hr.value,nul);
                m_stpos = irs_u16(hr_start/2);
                m_packet_hr.byte_count = irs_u8(hr_size);
                get_bytes(m_hold_regs_w,hr_start,hr_size,m_packet_hr.value,1);
                make_packet(hr_start/2,irs_u16(hr_size/2+1));
              }
              else
              {

                m_stpos = irs_u16(hr_start/2);
                m_packet_hr.byte_count = irs_u8(hr_size);
                //irs_u8 rmass[size_of_data]);
                irs_u16 nul = 0;
                get_bytes(m_hold_regs_w,hr_start,hr_size,m_packet_hr.value,nul);
                make_packet(hr_start/2,irs_u16(hr_size/2+1));
                //pack_tb_t &m_pack_regs = *(pack_tb_t *)rmass;
              }
              //for(irs_u8 i = 0;i<((hr_size/2)+1);i++)
              //{
                //m_packet_hr.value[i] = m_pack_regs.value[i]);
                //m_packet_hr.value[i] = mass[i]);
                //OUTDBG(cout<<hex<<showbase<<(int)m_packet_hr.value[i]<<"="<<
                  //(int)m_pack_regs.value[i]<<' ');
              //}
            }
            //OUTDBG(cout <<dec<<noshowbase<< '\n');
            //for(irs_u32 i = m_start_block; i < (m_start_block + hr_size);i++)
              //m_rforwr[i] = 0;
            //m_part_send = irs_false;
          }
        }
        else if (m_start_block<(irs_u32)coils_inbits)
        {
          irs_u32 nul = 0;
          range(m_start_block,m_nregs,nul,coils_inbits,&coils_size,&coils_start);
          //coils_start = m_start_block;
          //coils_size = m_nregs;
          //OUTDBG(cout << "Coils in bits "<<dec<<(int)coils_inbits<<"\n");
          //OUTDBG(cout << "Size " << dec<<(int)coils_size <<"\n");
          //OUTDBG(cout << "Start "<< dec<<(int)coils_start<<"\n");
          m_command = WMcoils;
          packet_data_8_t &m_packet_coils =
            *(packet_data_8_t*)(m_spacket+size_of_header+size_of_read_header);
          if(coils_size > size_of_data*8)
          {
            make_packet(coils_start,size_of_data*8);
            m_stpos = irs_u16(coils_start);
            m_packet_coils.byte_count   = size_of_data;
            BintoDec(m_coils_w,coils_start,m_packet_coils);
            for(irs_u32 i = m_start_block;
              i < (m_start_block + size_of_data*8);i++)
              m_rforwr[i] = 0;
          }
          else
          {
            make_packet(coils_start, irs_u16(coils_size));
            m_stpos = irs_u16(coils_start);
            if(coils_size%8 == 0)
              m_packet_coils.byte_count = irs_u8(coils_size/8);
            else
              m_packet_coils.byte_count = irs_u8(coils_size/8+1);
            BintoDec(m_coils_w,coils_start,m_packet_coils);
            for(irs_u32 i = m_start_block;
              i < (m_start_block + coils_size);i++)
              m_rforwr[i] = 0;
          }
        }
        m_mode = wait_mode;
      }
      break;
      case wait_mode:
      {
        if(test_to_cnt(wait))
          m_mode = w_mode;
      }
      break;
    }
  }
  else
  {
    mp_handle = mxifa_open(m_channel,irs_false);
    //OUTDBG(cout << "Channel open\n");
    mxifa_linux_tcp_ip_cl_cfg cfg;
    mxifa_get_config(mp_handle, (void *)&cfg);
    cfg.dest_port = m_dport;
    cfg.dest_ip = m_dip;
    mxifa_set_config(mp_handle, (void *)&cfg);
  }
  mxifa_tick();
}

