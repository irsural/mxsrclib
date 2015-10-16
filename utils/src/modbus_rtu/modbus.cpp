#include "modbus.h"

//-------------------------------------------------------------------------
// Defines
#define frame   1		//0 ��� ����� ASCII
										//1 ��� ����� RTU

#define MAXLENPACKET 256 	//������������ ���������� ������ � ������

#define erNoError	      			0	 //��� ������
#define erIllegalFunction     1	 //�������� ��� ������� ������� ���� ���������
#define erIllegalDataAddress  2	 //����� ������ ��������� � ������� �� ��������
#define erIllegalDataValue    3	 //�������� ������������ � ���� ������ �������� ������������
#define erSlaveDeviceFailure  4	 //��������������� ������ ����� ����� ���� ���� ������� ��������� ����. ��������
#define erACKNOWLEDGE  	      5	 //���������� ������ ������ � ������������ ���
#define erSlaveDeviceBusy     6	 //���������� ����� ���������� �������
#define erNegativeACKNOWLEDGE 7	 //���������� �� ����� ��������� ������� �������� � �������

#define tpReadCoilState      	1	 //������ ������� �������
#define tpReadInpState 	     	2	 //������ ������� ������
#define tpReadHoldReg 	     	3	 //������ ���������� ���������
#define tpReadInpReg 	     		4	 //������ ���������� ������� ���������
#define tpForceSingleCoil    	5	 //��������� ���������� ������
#define tpPresSingleReg      	6	 //������ �������� � ��������� �������
#define tpForceMultCoils     15  //������������� ������ ����� ������������������ �������
#define tpForceMultRegs      16	 //������ ������ � ������������������ ���������

//-------------------------------------------------------------------------
// ���������� �-���

#if frame
void WaitType(unsigned char);
void WaitDest(unsigned char);
void WaitHiAdres(unsigned char);
void WaitLoAdres(unsigned char);
void WaitHiCount(unsigned char);
void WaitLoCount(unsigned char);
void WaitCounter(unsigned char);

void WaitSrc(unsigned char);
void WaitLen1(unsigned char);
void WaitLen2(unsigned char);
void WaitTmpChk(unsigned char);
void WaitData(unsigned char);
void WaitChk1(unsigned char);
void WaitChk2(unsigned char);
void SendFF(unsigned char);
void Send99(unsigned char);
void SendType(unsigned char);
void SendDest(unsigned char);
void SendCountByte(unsigned char);
void SendHiAdres(unsigned char);
void SendLoAdres(unsigned char);
void SendHiCount(unsigned char);
void SendLoCount(unsigned char);
//void NullSend (unsigned char byt);
//void ANullSend (unsigned char byt);
//void NullWait (unsigned char byt);


void SendSource(unsigned char);
void SendLen1(unsigned char);
void SendLen2(unsigned char);
void SendTmpChk(unsigned char);
void SendData(unsigned char);
void SendChk1(unsigned char);
void SendChk2(unsigned char);

void EventDel();
unsigned short AccountCRC(unsigned char byt, unsigned short CRC);

#else //frame

void AWaitBegin(unsigned char);
void AWaitType1(unsigned char);
void AWaitType2(unsigned char);
void AWaitDest1(unsigned char);
void AWaitDest2(unsigned char);
void AWaitHiAdres1(unsigned char);
void AWaitHiAdres2(unsigned char);
void AWaitLoAdres1(unsigned char);
void AWaitLoAdres2(unsigned char);
void AWaitHiCount1(unsigned char);
void AWaitHiCount2(unsigned char);
void AWaitLoCount1(unsigned char);
void AWaitLoCount2(unsigned char);
void AWaitCounter1(unsigned char);
void AWaitCounter2(unsigned char);


void AWaitData1(unsigned char);
void AWaitData2(unsigned char);
void AWaitChk1(unsigned char);
void AWaitChk2(unsigned char);
void ASendBegin(unsigned char);
void ASendType1(unsigned char);
void ASendType2(unsigned char);
void ASendDest1(unsigned char);
void ASendDest2(unsigned char);
void ASendCountByte1(unsigned char);
void ASendCountByte2(unsigned char);
void ASendHiAdres1(unsigned char);
void ASendHiAdres2(unsigned char);
void ASendLoAdres1(unsigned char);
void ASendLoAdres2(unsigned char);
void ASendHiCount1(unsigned char);
void ASendHiCount2(unsigned char);
void ASendLoCount1(unsigned char);
void ASendLoCount2(unsigned char);


void ASendSource1(unsigned char);
void ASendSource2(unsigned char);
void ASendData1(unsigned char);
void ASendData2(unsigned char);
void ASendChk1(unsigned char);
void ASendChk2(unsigned char);

void AWaitLastASend1(unsigned char);
void AWaitLastASend2(unsigned char);
void AWaitLastASend3(unsigned char);

#endif //frame

void WaitLastSend(unsigned char);
void SwitchToReceive(unsigned char);
void SendByte(unsigned char);

void SendPacket(unsigned char);
void SetReceiveRetCode(unsigned char);
//void SendRetCode(unsigned char);

void Analiz(void);

void ReadingCoil(unsigned char *offset);		//������ ������
void ReadingReg(unsigned char *offset);		//������
void ForcesingData(unsigned char *offset);    	//���������
void ForcesingCoil(unsigned char *offset);    	//���������

//��������� �� �������
#if frame
void (*CurProc) (unsigned char)=WaitDest;
#else
void (*CurProc) (unsigned char)=AWaitBegin;
#endif

//-------------------------------------------------------------------------
// �������� �����
typedef unsigned char TBuffData[MAXLENPACKET];

typedef struct
{
	unsigned char Dest;		//*���� �����
	unsigned char Typ;		//*��� ������
  unsigned char RCode;
  unsigned short Adress;		//�����
  unsigned short Count;		//����������
  unsigned char Counter;	//������� ����
  TBuffData Buff;		//*������������ ������
	unsigned short Chk;		//����������� �����
} TNetBuff;

typedef struct
{
  unsigned short  Adress;		//*���� �����
  unsigned short  Typ;		//*�������
	TBuffData     Buff;			//*������������ ������
  unsigned short  Chk;		//����������� �����
  unsigned char Reserv;
} TASCIIBuff;

// ����
//typedef unsigned char BYTE;
//typedef enum { false=0, true=1 } bool;
//typedef enum {stListen,stCheck,stQueue,stMarker} TState;


//-------------------------------------------------------------------------
// ���������� ����������

TNetBuff RecBuf,TranBuf;		//������� ������ � ��������
unsigned short CurUk;
unsigned short CCC;
unsigned char LRC;

unsigned char *OutCoil, *InCoil;
unsigned short *OReg, *InReg;

// MODBUS-�����
unsigned char node = 1;
bool stTrans=false;
bool var_change;  //������� ��������� ����������
modbus_service_base_t* p_service = NULL;

//-------------------------------------------------------------------------
// �-���

//------------------------------------------------------
// modbus
modbusc::modbusc(unsigned short* ap_vars, unsigned short a_vars_count,
  modbus_service_base_t* ap_service
):
  out_regs(ap_vars),
  m_vars_count(a_vars_count),
  mp_service(ap_service),
  m_silence_time(mp_service->silence_time()),
  m_timer_silence_checked(false),
  m_silence_conter_target(0)
{
  p_service = mp_service;
	OutCoil = oc;
	InCoil = ic;
	InReg = ir;
	OReg = out_regs;
	*oc = 0x55;
	*ic = 0x55;
	{ for (int i = 0; i < 5; i++) ir[i] = i; }
}

modbusc::~modbusc()
{
}

void modbusc::timer_silence_start()
{
  m_timer_silence_checked = false;
  m_silence_conter_target = mp_service->counter() + m_silence_time;
}

bool modbusc::timer_silence_check()
{
  if (!m_timer_silence_checked)
  if (m_silence_conter_target - mp_service->counter() < 0) {
    m_timer_silence_checked = true;
  }
  return m_timer_silence_checked;
}
void modbusc::interrupt_handler()
{
	if (stTrans) {
    // ����� ����������
    if (mp_service->transmit_ready()) {
      CurProc(0);
    }
	} else {
    // ����� ���������
		if (mp_service->receive_ready()) {
			if (mp_service->receive_error() == false) {
				unsigned char byt = mp_service->receive();
				#if frame
        if (timer_silence_check()) {
          CurProc=WaitDest;
          mp_service->switch_on_receive();
        }
        timer_silence_start();
        CurProc(byt);
				#else
        CurProc(byt);
        if (byt==':') {
          CurProc=AWaitDest1;
        }
				#endif
			}
		}
	}
}
//------------------------------------------------------

//_______________________������ � ������_______________________________
#if frame
unsigned short AccountCRC(unsigned char byt, unsigned short CRC)
{
	char i;
	CRC^=byt;
	for (i=0;i<8;i++) {
		if (CRC&1) {CRC>>=1;CRC^=0xA001;}
		else {CRC>>=1;}
	}
	return(CRC);
}

void WaitDest(unsigned char byt) 		//���� �����
{
  RecBuf.Dest=byt;
  CCC=0xFFFF;
  CurUk=0;
  RecBuf.Chk=AccountCRC(byt,CCC);
  CurProc=WaitType;
}

void WaitType(unsigned char byt) 		//��� ������
{
  RecBuf.Typ=byt;
  CCC=RecBuf.Chk;
  RecBuf.Chk=AccountCRC(byt,CCC);
  CurProc=WaitHiAdres;
}

void WaitHiAdres(unsigned char byt) 		//��� ������
{
  RecBuf.Adress=byt<<8;
  CCC=RecBuf.Chk;
  RecBuf.Chk=AccountCRC(byt,CCC);
  CurProc=WaitLoAdres;
}


void WaitLoAdres(unsigned char byt) 		//��� ������
{
  RecBuf.Adress|=byt;
  CCC=RecBuf.Chk;
  RecBuf.Chk=AccountCRC(byt,CCC);
  if ((RecBuf.Typ==0x05)|(RecBuf.Typ==0x06)) {
    CurProc=WaitData;
    RecBuf.Counter=2;
  } else CurProc=WaitHiCount;
}

void WaitHiCount(unsigned char byt) 		//��� ������
{
  RecBuf.Count=byt<<8;
  CCC=RecBuf.Chk;
  RecBuf.Chk=AccountCRC(byt,CCC);
  CurProc=WaitLoCount;
}

void WaitLoCount(unsigned char byt) 		//��� ������
{
  RecBuf.Count+=byt;
  CCC=RecBuf.Chk;
  RecBuf.Chk=AccountCRC(byt,CCC);
  switch (RecBuf.Typ) {
    case 1: ;
    case 2: ;
    case 3: ;
    case 4: CurProc=WaitChk1; break;
    case 15: ;
    case 16: CurProc=WaitCounter; break;
    default: {
      SetReceiveRetCode(erIllegalFunction);
    }
  }
}

void WaitCounter(unsigned char byt) 		//��� ������
{
  RecBuf.Counter=byt;
  CCC=RecBuf.Chk;
  RecBuf.Chk=AccountCRC(byt,CCC);
  CurProc=WaitData;
}


void WaitData(unsigned char byt)		//����� ������
{
  CCC=RecBuf.Chk;
  RecBuf.Chk=AccountCRC(byt,CCC);
  RecBuf.Buff[CurUk++]=byt;
  if (CurUk==RecBuf.Counter) CurProc=WaitChk1;
}

void WaitChk1(unsigned char byt)			//����� ����������� �����
{
  CCC=RecBuf.Chk;
  RecBuf.Chk=AccountCRC(byt,CCC);
  CurProc=WaitChk2;
}

void WaitChk2(unsigned char byt)			//����� ����������� �����
{
  CCC=RecBuf.Chk;
  RecBuf.Chk=AccountCRC(byt,CCC);

  if (RecBuf.Chk==0) SetReceiveRetCode(erNoError);
  else SetReceiveRetCode(erIllegalFunction);
}


void SendSource(unsigned char byt)		//������� ���� �����
{
  CurProc=SendType;
  byt=node;
  CurUk=0;
  CCC=0xFFFF;
  TranBuf.Chk=AccountCRC(byt,CCC);
  SendByte(byt);
}


void SendType(unsigned char byt)			//������� ��� ������
{
  byt=TranBuf.Typ;
  switch (byt) {
    case 1:;
    case 2:;
    case 3:;
    case 4: CurProc=SendCountByte; break;
    case 5:;
    case 6:;
    case 15:;
    case 16: CurProc=SendHiAdres; break;
  }
  CCC=TranBuf.Chk;
  TranBuf.Chk=AccountCRC(byt,CCC);
  SendByte(byt);
}

void SendCountByte(unsigned char byt)		//������� ����� ����������
{
  CurProc=SendData;
  byt=TranBuf.Counter;
  CurUk=0;
  CCC=TranBuf.Chk;
  TranBuf.Chk=AccountCRC(byt,CCC);
  SendByte(byt);
}

void SendHiAdres(unsigned char byt)		//�������� �������� ����� ������
{
  CurProc=SendLoAdres;
  TranBuf.Counter=2;
  byt=TranBuf.Adress>>8;
  CCC=TranBuf.Chk;
  TranBuf.Chk=AccountCRC(byt,CCC);
  SendByte(byt);
}


void SendLoAdres(unsigned char byt)		//�������� �������� ����� ������
{
  if ((TranBuf.Typ==15)|(TranBuf.Typ==16)) CurProc=SendHiCount;
  else CurProc=SendData;
  byt=TranBuf.Adress&0xFF;
  CCC=TranBuf.Chk;
  TranBuf.Chk=AccountCRC(byt,CCC);
  SendByte(byt);
}

void SendHiCount(unsigned char byt)		//�������� �������� ����� ������
{
  CurProc=SendLoCount;
  byt=TranBuf.Count>>8;
  CCC=TranBuf.Chk;
  TranBuf.Chk=AccountCRC(byt,CCC);
  SendByte(byt);
}

void SendLoCount(unsigned char byt)		//�������� �������� ����� ������
{
  CurProc=SendChk1;
  byt=TranBuf.Count&0xFF;
  CCC=TranBuf.Chk;
  TranBuf.Chk=AccountCRC(byt,CCC);
  SendByte(byt);
}


void SendData(unsigned char byt)		//�������� ������
{
  byt=TranBuf.Buff[CurUk++];
  if (CurUk==TranBuf.Counter) CurProc=SendChk1;
  CCC=TranBuf.Chk;
  TranBuf.Chk=AccountCRC(byt,CCC);
  SendByte(byt);
}

void SendChk1(unsigned char byt)			//�������� ����������� �����
{
  CurProc=SendChk2;
  byt=TranBuf.Chk&0x00FF;
  SendByte(byt);
}

void SendChk2(unsigned char byt)			//�������� ����������� �����
{
  CurProc=WaitLastSend;
  byt=TranBuf.Chk>>8;
  SendByte(byt);
}

void WaitLastSend(unsigned char byt)		//���������� �������� �����
{
  CurProc=SwitchToReceive;
  byt=0;
  SendByte(byt);
}
#else
//ASCII
ascii_to_hex(char ascii)
{
if (ascii<'0') return(0);
else if (ascii<='9') return(ascii-'0');
else if (ascii<'A') return(0);
else if (ascii<='F') return(ascii-'A'+10);
else if (ascii<'a') return(0);
else if (ascii<='f') return(ascii-'a'+10);
return 0;
};


hex_to_ascii(char hex)
{
  if (hex<10) return('0'+hex);
  else
   if (hex<16) return('A'+hex-10);
  return 0;
};



void AWaitBegin(unsigned char byt)           	//����� ������ ������
{
  if (byt==':')  CurProc=AWaitDest1;
}

void AWaitDest1(unsigned char byt) 		//���� �����
{
  byt=ascii_to_hex(byt)<<4;
  RecBuf.Dest=byt;
  LRC=byt;
  CurProc=AWaitDest2;
  CurUk=0;
}

void AWaitDest2(unsigned char byt) 		//���� �����
{
  byt=ascii_to_hex(byt);
  LRC+=byt;
  RecBuf.Dest+=byt;
  CurProc=AWaitType1;
}


void AWaitType1(unsigned char byt) 		//��� ������
{
  byt=ascii_to_hex(byt)<<4;
  LRC+=byt;
  RecBuf.Typ=byt;
  CurProc=AWaitType2;
}

void AWaitType2(unsigned char byt) 		//��� ������
{
  byt=ascii_to_hex(byt);
  LRC+=byt;
  RecBuf.Typ+=byt;
  CurProc=AWaitHiAdres1;
}

void AWaitHiAdres1(unsigned char byt) 		//��� ������
{
  unsigned short tbt;
  tbt=byt=ascii_to_hex(byt);
  LRC+=byt<<4;
  RecBuf.Adress=tbt<<12;
  CurProc=AWaitHiAdres2;
}


void AWaitHiAdres2(unsigned char byt) 		//��� ������
{
  unsigned short tbt;
  tbt=byt=ascii_to_hex(byt);
  LRC+=byt;
  RecBuf.Adress+=tbt<<8;
  CurProc=AWaitLoAdres1;
}

void AWaitLoAdres1(unsigned char byt) 		//��� ������
{
  byt=ascii_to_hex(byt)<<4;
  LRC+=byt;
  RecBuf.Adress+=byt;
  CurProc=AWaitLoAdres2;
}


void AWaitLoAdres2(unsigned char byt) 		//��� ������
{
  byt=ascii_to_hex(byt);
  LRC+=byt;
  RecBuf.Adress+=byt;
  if ((RecBuf.Typ==0x05)|(RecBuf.Typ==0x06)) {
    CurProc=AWaitData1;
    RecBuf.Counter=2;
  } else CurProc=AWaitHiCount1;
}

void AWaitHiCount1(unsigned char byt) 		//��� ������
{
  unsigned short tbt;
  tbt=byt=ascii_to_hex(byt);
  RecBuf.Count=tbt<<12;
  LRC+=byt<<4;
  CurProc=AWaitHiCount2;
}

void AWaitHiCount2(unsigned char byt) 		//��� ������
{
  unsigned short tbt;
  tbt=byt=ascii_to_hex(byt);
  LRC+=byt;
  RecBuf.Count+=tbt<<8;
  CurProc=AWaitLoCount1;
}

void AWaitLoCount1(unsigned char byt) 		//��� ������
{
  byt=ascii_to_hex(byt)<<4;
  LRC+=byt;
  RecBuf.Count+=byt;
  CurProc=AWaitLoCount2;
}

void AWaitLoCount2(unsigned char byt) 		//��� ������
{
  byt=ascii_to_hex(byt);
  LRC+=byt;
  RecBuf.Count+=byt;
  switch (RecBuf.Typ) {
    case 1:;
    case 2:;
    case 3:;
    case 4: {CurProc=AWaitChk1; break;}
    case 15:;
    case 16: {CurProc=AWaitCounter1; break;}
    default: {
      SetReceiveRetCode(erIllegalFunction);
    }
  }
}

void AWaitCounter1(unsigned char byt) 		//��� ������
{
  byt=ascii_to_hex(byt)<<4;
  LRC+=byt;
  RecBuf.Counter=byt;
  CurProc=AWaitCounter2;
}

void AWaitCounter2(unsigned char byt) 		//��� ������
{
  byt=ascii_to_hex(byt);
  LRC+=byt;
  RecBuf.Counter+=byt;
  CurProc=AWaitData1;
  CurUk=0;
}

void AWaitData1(unsigned char byt)		//����� ������
{
  byt=ascii_to_hex(byt)<<4;
  LRC+=byt;
  RecBuf.Buff[CurUk]=byt;
  CurProc=AWaitData2;
}


void AWaitData2(unsigned char byt)		//����� ������
{
  byt=ascii_to_hex(byt);
  LRC+=byt;
  RecBuf.Buff[CurUk++]+=byt;
  if (CurUk==RecBuf.Counter+1) CurProc=AWaitChk1;
  else CurProc=AWaitData1;
}

void AWaitChk1(unsigned char byt)			//����� ����������� �����
{
  byt=ascii_to_hex(byt)<<4;
  LRC=0xFF-LRC+1;
  CCC=byt;
  CurProc=AWaitChk2;
}

void AWaitChk2(unsigned char byt)			//����� ����������� �����
{
  byt=ascii_to_hex(byt);
  CCC+=byt;
  if (LRC==CCC)  SetReceiveRetCode(erNoError);
  else SetReceiveRetCode(erIllegalFunction);
}

void ASendBegin(unsigned char byt)			//������� ��� ������ ������
{
  CurProc=ASendSource1;
  byt=':';
  CurUk=0;
  SendByte(byt);
}

void ASendSource1(unsigned char byt)		//������� ���� �����
{
  CurProc=ASendSource2;
  byt=hex_to_ascii(node>>4);
  TranBuf.Chk=byt;
  SendByte(byt);
}

void ASendSource2(unsigned char byt)		//������� ���� �����
{
  CurProc=ASendType1;
  byt=hex_to_ascii(node&0xF);
  TranBuf.Chk+=byt;
  SendByte(byt);
}

void ASendType1(unsigned char byt)			//������� ��� ������
{
  CurProc=ASendType2;
  byt=hex_to_ascii(TranBuf.Typ>>4);
  TranBuf.Chk+=byt;
  SendByte(byt);
}


void ASendType2(unsigned char byt)			//������� ��� ������
{
  byt=hex_to_ascii(TranBuf.Typ&0xF);
  switch (TranBuf.Typ) {
    case 1:;
    case 2:;
    case 3:;
    case 4: {CurProc=ASendCountByte1; break;}
    case 5:;
    case 6:;
    case 15: ;
    case 16: {CurProc=ASendHiAdres1; break;}
  }
  TranBuf.Chk+=byt;
  SendByte(byt);
}

void ASendCountByte1(unsigned char byt)		//������� ����� ����������
{
  CurProc=ASendCountByte2;
  byt=hex_to_ascii(TranBuf.Counter>>4);
  TranBuf.Chk+=byt;
  SendByte(byt);
}

void ASendCountByte2(unsigned char byt)		//������� ����� ����������
{
  CurProc=ASendData1;
  byt=hex_to_ascii(TranBuf.Counter&0xF);
  TranBuf.Chk+=byt;
  SendByte(byt);
}

void ASendHiAdres1(unsigned char byt)		//�������� �������� ����� ������
{
  CurProc=ASendHiAdres2;
  byt=hex_to_ascii(TranBuf.Adress>>12);
  TranBuf.Chk+=byt;
  SendByte(byt);
}

void ASendHiAdres2(unsigned char byt)		//�������� �������� ����� ������
{
  CurProc=ASendLoAdres1;
  TranBuf.Counter=4;
  byt=hex_to_ascii((TranBuf.Adress&0xFFF)>>8);
  TranBuf.Chk+=byt;
  SendByte(byt);
}

void ASendLoAdres1(unsigned char byt)		//�������� �������� ����� ������
{
  CurProc=ASendLoAdres2;
  byt=hex_to_ascii((TranBuf.Adress&0xFF)>>4);
  TranBuf.Chk+=byt;
  SendByte(byt);
}


void ASendLoAdres2(unsigned char byt)		//�������� �������� ����� ������
{
  if ((TranBuf.Typ==15)|(TranBuf.Typ==16)) CurProc=ASendHiCount1;
  else CurProc=ASendData1;
  byt=hex_to_ascii(TranBuf.Adress&0xF);
  TranBuf.Chk+=byt;
  SendByte(byt);
}

void ASendHiCount1(unsigned char byt)		//�������� �������� ����� ������
{
  CurProc=ASendHiCount2;
  byt=hex_to_ascii(TranBuf.Count>>12);
  TranBuf.Chk+=byt;
  SendByte(byt);
}

void ASendHiCount2(unsigned char byt)		//�������� �������� ����� ������
{
  CurProc=ASendLoCount1;
  byt=hex_to_ascii((TranBuf.Count&0xFFF)>>8);
  TranBuf.Chk+=byt;
  SendByte(byt);
}


void ASendLoCount1(unsigned char byt)		//�������� �������� ����� ������
{
  CurProc=ASendLoCount2;
  byt=hex_to_ascii((TranBuf.Count&0xFF)>>4);
  TranBuf.Chk+=byt;
  SendByte(byt);
}

void ASendLoCount2(unsigned char byt)		//�������� �������� ����� ������
{
  CurProc=ASendChk1;
  byt=hex_to_ascii(TranBuf.Count&0xF);
  TranBuf.Chk+=byt;
  SendByte(byt);
}

void ASendData1(unsigned char byt)		//�������� ������
{
  byt=hex_to_ascii(TranBuf.Buff[CurUk]>>4);
  TranBuf.Chk+=byt;
  CurProc=ASendData2;
  SendByte(byt);
}


void ASendData2(unsigned char byt)		//�������� ������
{
  byt=hex_to_ascii(TranBuf.Buff[CurUk++]&0xF);
  TranBuf.Chk+=byt;
  if (CurUk==TranBuf.Counter) CurProc=ASendChk1;
  else CurProc=ASendData1;
  SendByte(byt);
}

void ASendChk1(unsigned char byt)			//�������� ����������� �����
{
  CurProc=ASendChk2;
  TranBuf.Chk=0xFF-TranBuf.Chk+1;
  byt=hex_to_ascii(TranBuf.Chk>>4);
  SendByte(byt);
}

void ASendChk2(unsigned char byt)			//�������� ����������� �����
{
  CurProc=AWaitLastASend1;
  byt=hex_to_ascii(TranBuf.Chk&0xF);
  SendByte(byt);
}

void AWaitLastASend1(unsigned char byt)		//���������� �������� �����
{
  CurProc=AWaitLastASend2;
  byt=0x0D;
  SendByte(byt);
}

void AWaitLastASend2(unsigned char byt)		//���������� �������� �����
{
  CurProc=AWaitLastASend3;
  byt=0x0A;
  SendByte(byt);
}

void AWaitLastASend3(unsigned char byt)		//���������� �������� �����
{
  CurProc=SwitchToReceive;
  byt=0;
  SendByte(byt);
}

//end ASCII
#endif

void SwitchToReceive(unsigned char)			//������������ �� �����
{
  p_service->switch_on_receive();
  #if frame
    CurProc=WaitDest;
  #else
    CurProc=AWaitBegin;
  #endif
  CurUk=0;
}

void SendByte(unsigned char byt)               	//�������� �����
{
  p_service->transmit(byt);
  //outportb(uBaseUart+4,byt);
  //  printf("%x,",byt);
}

void SendPacket(unsigned char) 		//���������� � �������� ������
{
  p_service->switch_on_transmit();
  #if frame
    CurProc=SendSource;
  #else
    CurProc=ASendBegin;
  #endif
  CurUk=0;
  RecBuf.Dest=TranBuf.Dest;
}

void SetReceiveRetCode(unsigned char byt)	//byt-��� ������
{
  #if frame
    CurProc=WaitDest;
  #else
    CurProc=AWaitBegin;
  #endif
  CurUk=0;
  RecBuf.RCode=byt;
  Analiz();
}


void Analiz(void)			//������ ����������� ���������
{
  if ((RecBuf.RCode==erNoError)&(RecBuf.Dest==node))
  {
    switch (RecBuf.Typ)
    {
      case tpReadCoilState:	//������ ������� �������
      {
				ReadingCoil((unsigned char *)OutCoil);
				break;
			}
			case tpReadInpState:	//������ ������� ������
			{
				ReadingCoil((unsigned char *)InCoil);
				break;
			}
			case tpReadHoldReg:	//������ ���������� ���������
			{
				ReadingReg((unsigned char *)OReg);
				break;
			}
			case tpReadInpReg:	//������ ���������� ������� ���������
			{
				ReadingReg((unsigned char *)InReg);
				break;
			}
			case tpForceSingleCoil:	//��������� ���������� ������
			{
				ForcesingCoil((unsigned char *)OutCoil);
				break;
			}
			case tpPresSingleReg:	//��������� ���������� ��������
			{
				ForcesingData((unsigned char *)OReg);
				break;
			}
			case tpForceMultCoils:	//15��������� ������������������ �������
			{
				ForcesingCoil((unsigned char *)OutCoil);
				break;
			}
			case tpForceMultRegs:	//16��������� ������������������ ���������
			{
				ForcesingData((unsigned char *)OReg);
				break;
			}
		}
	}
}

void ReadingReg(unsigned char *offset)		//������
{
	unsigned char *labl;
	unsigned short count,j,i;

	SendPacket(0);

  TranBuf.Dest=RecBuf.Dest;
  TranBuf.Typ=RecBuf.Typ;
  labl=RecBuf.Adress*2+offset;
  count=RecBuf.Count;
  for (j=0,i=0; i<count; j+=2,i++) {
    TranBuf.Buff[j+1]=(unsigned char)*labl;
    labl++;			//��������� �����������
    TranBuf.Buff[j]=(unsigned char)*labl;
    labl++;
  }
	TranBuf.Counter=count*2;
};

void ReadingCoil(unsigned char *offset)		//������
{
  unsigned char *labl;
  unsigned char mskQ, mskA;
  unsigned short count,j;
  div_t x; 			//x- ����� ���������� ����

  SendPacket(0);
  TranBuf.Dest=RecBuf.Dest;
  TranBuf.Typ=RecBuf.Typ;
  x=div(RecBuf.Adress,8);	//x.quot-�������� � ������ x.rem-����� ������
  labl=x.quot+offset;
  int i = 0;
  for (i=0,mskQ=1;i<x.rem;i++,mskQ<<=1);
  count=RecBuf.Count;

  unsigned short ii = 0;
  for (ii=0,j=0,mskA=1; ii<count; ii++) {
    if (*labl&mskQ) TranBuf.Buff[j]|=mskA;
    else TranBuf.Buff[j]&=~mskA;
    if (mskA==128) {mskA=1; j++;}    else mskA<<=1;
    if (mskQ==128) {mskQ=1; labl++;} else mskQ<<=1;
  }
  TranBuf.Counter=j+1;
};

void ForcesingData(unsigned char *offset)    	//��������� ���������� Registra
{
  unsigned char *labl;
  unsigned short j, count,i;

  SendPacket(0);
	var_change=true;
  TranBuf.Dest=RecBuf.Dest;
	TranBuf.Typ=RecBuf.Typ;
  TranBuf.Adress=RecBuf.Adress;
  TranBuf.Count=RecBuf.Count;
  count=RecBuf.Count;
  labl=(RecBuf.Adress*2)+offset;
  for (i=0,j=0; i<count; i++,j+=2) {
    *labl=RecBuf.Buff[j+1];
    labl++;
    *labl=RecBuf.Buff[j];
    labl++;
  }
};

void ForcesingCoil(unsigned char *offset)    	//��������� ���������� ������
{
  unsigned char *labl;
  unsigned char msk;
  int i;
  div_t x;

  SendPacket(0);
  var_change=true;
  TranBuf.Dest=RecBuf.Dest;
  TranBuf.Typ=RecBuf.Typ;
  TranBuf.Adress=RecBuf.Adress;
  x=div(RecBuf.Adress,8);
  labl=x.quot+offset;
  for (i=0,msk=1;i<x.rem;msk<<=1,i++);
  if ((RecBuf.Buff[0]+RecBuf.Buff[1])>0)  *labl|=msk;
  else *labl&=~msk;
  TranBuf.Buff[0]=RecBuf.Buff[0];
  TranBuf.Buff[1]=RecBuf.Buff[1];
  TranBuf.Counter=2;
};


#ifdef NOP
// ������ ���������� modbus_service_t ��� ATmega324P
modbus_service_t::modbus_service_t():
  m_baud_rate(115200)
{
  counter_32bit_init();
  UBRR0 = F_CPU/(16*m_baud_rate) - 1;
  UCSR0B_RXEN0 = 0;
  UCSR0B_TXEN0 = 0;
  UCSR0C_UCSZ00 = 1;
  UCSR0C_UCSZ01 = 1;
  UCSR0B_UCSZ02 = 0;
}
long modbus_service_t::counter()
{
  return counter_32bit_get_counter();
}
long modbus_service_t::silence_time()
{
  return mcs_to_cnt(1000000/m_baud_rate);
}
void modbus_service_t::switch_on_receive()
{
}
bool modbus_service_t::receive_ready()
{
  return UCSR0A_UDRE0 == 1;
}
bool modbus_service_t::receive_error()
{
  return false;
}
unsigned char modbus_service_t::receive()
{
  return UDR0;
}
void modbus_service_t::switch_on_transmit()
{
}
bool modbus_service_t::transmit_ready()
{
  return UCSR0A_UDRE0 == 1;
}
void modbus_service_t::transmit(unsigned char a_byte)
{
  UDR0 = a_byte;
}

modbus_service_t modbus_service;
unsigned short modbus_vars[5];
modbusc modbus(modbus_vars, IRS_ARRAYSIZE(modbus_vars), &modbus_service);
#endif //NOP
