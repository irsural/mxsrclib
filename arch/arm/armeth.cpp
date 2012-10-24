//! \file
//! \ingroup drivers_group
//! \brief Интерфейс Ethernet (MAC) для ARM
//!
//! Дата создания: 20.05.2010

#include <irsdefs.h>

#include <armeth.h>
#include <armioregs.h>
#include <irscpu.h>
#include <irsint.h>
#include <irsarchint.h>
#include <timer.h>
#include <irsalg.h>
#include <armcfg.h>
#include <irscpp.h>

#include <irsfinal.h>

#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__) || defined(__STM32F100RBT__)
template <class T>
T spec_div(T a_num, T a_denom)
{
  T div_result = a_num/a_denom;
  if (a_num%a_denom != 0) {
    div_result++;
  }
  return div_result;
}

irs::arm::arm_ethernet_t::arm_ethernet_t(
  buffer_num_t a_buf_num,
  size_t a_buf_size,
  mxmac_t a_mac,
  use_int_t a_use_interrupt
):
  m_buf_num(a_buf_num),
  #ifndef NEW_21092011
  m_buf_size(a_buf_size),
  #endif //NEW_21092011
  mp_rx_buf(IRS_NULL),
  mp_tx_buf(IRS_NULL),
  m_rx_buf_handled(true),
  m_rx_buf_filled(false),
  m_rx_size(0),
  m_rx_int_flag(false),
  m_rx_int_event(this, &arm_ethernet_t::rx_interrupt),
  m_use_interrupt(a_use_interrupt),
  m_send_buf_locked(false),
  m_send_packet_action(false),
  #ifdef NEW_21092011
  m_rx_buf_size(a_buf_size + FCS_size),
  m_tx_buf_size(a_buf_size)
  #else //NEW_21092011
  m_rx_buf_size(DA_size + SA_size + L_size + m_buf_size + FCS_size),
  m_tx_buf_size(DA_size + SA_size + L_size + m_buf_size)
  #endif //NEW_21092011
{
  #ifdef NEW_21092011
  mp_rx_buf = new irs_u8[m_rx_buf_size + align_size];
  #else //NEW_21092011
  mp_rx_buf = new irs_u8[m_rx_buf_size];
  #endif //NEW_21092011
  if (m_buf_num == single_buf) {
    mp_tx_buf = mp_rx_buf;
  } else {
    #ifdef NEW_21092011
    mp_tx_buf = new irs_u8[m_tx_buf_size + align_size];
    #else //NEW_21092011
    mp_tx_buf = new irs_u8[m_tx_buf_size];
    #endif //NEW_21092011
  }
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  RCGC2_bit.PORTF = 1;
  RCGC2_bit.EPHY0 = 1;  //  В iolm3sxxxx.h биты указаны неверно
  RCGC2_bit.EMAC0 = 1;
  for (irs_u8 i = 10; i; i--);
  //  Делитель частоты интерфейса MII
  const irs_u32 MII_freq = 2500000; //  max MII freq, Hz
  MACMDV_bit.DIV = irs_u8((irs::cpu_traits_t::frequency()/(2 * MII_freq)) - 1);
  //  MAC-адрес
  MACIA0 = *(irs_u32*)a_mac.val;
  MACIA1_bit.MACOCT5 = a_mac.val[4];
  MACIA1_bit.MACOCT6 = a_mac.val[5];
  //  Передатчик
  MACTCTL_bit.DUPLEX = 1;
  MACTCTL_bit.CRC = 1;
  MACTCTL_bit.PADEN = 1;
  MACTCTL_bit.TXEN = 1;
  //  Приёмник
  MACIM = 0;
  MACRCTL_bit.RXEN = 0;
  MACRCTL_bit.RSTFIFO = 1;
  MACRCTL_bit.BADCRC = 1;
  MACRCTL_bit.PRMS = 0;
  MACRCTL_bit.AMUL = 0;
  MACRCTL_bit.RXEN = 1;
  //  Светодиоды Ethernet
  GPIOFDEN_bit.no2 = 1;
  GPIOFDEN_bit.no3 = 1;
  GPIOFDIR_bit.no2 = 1;
  GPIOFDIR_bit.no3 = 1;
  #ifdef __LM3SxBxx__
  (*((volatile irs_u32*)(PORTF_BASE + GPIO_LOCK))) = GPIO_UNLOCK_VALUE;
  #endif // __LM3SxBxx__
  GPIOFAFSEL_bit.no2 = 1;
  GPIOFAFSEL_bit.no3 = 1;
  #ifdef __LM3SxBxx__
  (*((volatile irs_u32*)(PORTF_BASE + GPIO_PCTL))) |= PORTF2_LED1;
  (*((volatile irs_u32*)(PORTF_BASE + GPIO_PCTL))) |= PORTF3_LED0;
  #endif // __LM3SxBxx__
  //  Прерывание приёма пакета
  if (m_use_interrupt == USE_INT) {
    interrupt_array()->int_event_gen(ethernet_int)->add(&m_rx_int_event);
    MACIM_bit.RXINT = 1;
    SETENA1_bit.NVIC_ETH_INT = 1;  //  Разрешение прерывания в NVIC
  } else {
    MACIM_bit.RXINT = 0;
    SETENA1_bit.NVIC_ETH_INT = 0;
  }
  #elif defined(__STM32F100RBT__)
  volatile irs_u8 y = a_mac.val[0];
  #elif defined(IRS_STM32F_2_AND_4)
  volatile irs_u8 y = a_mac.val[0];
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

irs::arm::arm_ethernet_t::~arm_ethernet_t()
{
  delete []mp_rx_buf;
  if (m_buf_num == double_buf) {
    delete []mp_tx_buf;
  }
  #if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
  SETENA1_bit.NVIC_ETH_INT = 0;
  MACIM_bit.RXINT = 0;
  MACTCTL_bit.TXEN = 0;
  MACRCTL_bit.RXEN = 0;
  RCGC2_bit.EPHY0 = 0;
  RCGC2_bit.EMAC0 = 0;
  #elif defined(__STM32F100RBT__)
  #elif defined(IRS_STM32F_2_AND_4)
  #else
    #error Тип контроллера не определён
  #endif  //  mcu type
}

void irs::arm::arm_ethernet_t::send_packet(irs_size_t a_size)
{
  if (a_size < DA_size + SA_size + L_size) return;

  #ifdef NEW_21092011
  irs_u32 fifo_data = a_size - DA_size - SA_size;
  IRS_HIWORD(fifo_data) = reinterpret_cast<irs_u16&>(mp_tx_buf[0]);
  #else //NEW_21092011
  irs_u32 fifo_data = a_size - DA_size - SA_size - L_size;
  IRS_HIWORD(fifo_data) = reinterpret_cast<irs_u16&>(*mp_tx_buf);
  #endif //NEW_21092011
  //MACDATA = fifo_data;
  set_fifo(fifo_data);

  const irs_size_t shift = sizeof(irs_u16);
  irs_u32* p_shifted_buf = reinterpret_cast<irs_u32*>(mp_tx_buf + shift);
  #ifdef NEW_21092011
  const irs_size_t whole_dwords_cnt =
    spec_div(a_size - shift, sizeof(fifo_data));
  #else //NEW_21092011
  const irs_size_t whole_dwords_cnt = (a_size - shift)/sizeof(fifo_data);
  #endif //NEW_21092011
  #ifdef NEW_21092011
  for (irs_size_t current_dword = 0; current_dword < whole_dwords_cnt;
    current_dword++)
  {
    MACDATA = p_shifted_buf[current_dword];
  }
  #else //NEW_21092011
  irs_size_t current_dword = 0;
  for (; current_dword < whole_dwords_cnt; current_dword++) {
    //MACDATA = p_shifted_buf[current_dword];
    set_fifo(p_shifted_buf[current_dword]);
  }
  irs_size_t current_byte = current_dword * sizeof(fifo_data) + shift;
  fifo_data = 0;
  for (irs_u8 i = 0; current_byte < a_size; current_byte++, i++) {
    *((irs_u8*)&fifo_data + i) = mp_tx_buf[current_byte];
  }
  //MACDATA = fifo_data;
  set_fifo(fifo_data);
  #endif //NEW_21092011
  m_send_packet_action = true;
  //MACTR_bit.NEWTX = 1;
  send_packet();
}

void irs::arm::arm_ethernet_t::set_recv_handled()
{
  m_rx_buf_handled = true;
  m_rx_buf_filled = false;
}

void irs::arm::arm_ethernet_t::set_send_buf_locked()
{
  m_send_buf_locked = true;
}

bool irs::arm::arm_ethernet_t::is_recv_buf_filled() const
{
  return m_rx_buf_filled;
}

bool irs::arm::arm_ethernet_t::is_send_buf_empty() const
{
  if (m_send_buf_locked) return false;
  //if (MACTR_bit.NEWTX == 0) {
  if (tx_buf_empty()) {
    if (m_buf_num == double_buf) {
      return true;
    } else if (!m_rx_buf_filled && !m_send_buf_locked) {
      return true;
    }
  }
  return false;
}

irs_u8* irs::arm::arm_ethernet_t::get_recv_buf()
{
  return mp_rx_buf;
}

irs_u8* irs::arm::arm_ethernet_t::get_send_buf()
{
  return mp_tx_buf;
}

irs_size_t irs::arm::arm_ethernet_t::recv_buf_size() const
{
  return m_rx_size;
}

irs_size_t irs::arm::arm_ethernet_t::send_buf_max_size() const
{
  return m_tx_buf_size;
}

irs::simple_ethernet_t::buffer_num_t
irs::arm::arm_ethernet_t::get_buf_num() const
{
  return m_buf_num;
}

#if defined(__LM3SxBxx__) || defined(__LM3Sx9xx__)
mxmac_t irs::arm::arm_ethernet_t::get_local_mac() const
{
  mxmac_t mac;
  *(irs_u32*)mac.val = MACIA0;
  mac.val[4] = MACIA1_bit.MACOCT5;
  mac.val[5] = MACIA1_bit.MACOCT6;
  return mac;
}

void irs::arm::arm_ethernet_t::set_mac(mxmac_t& a_mac)
{
  MACIA0 = *(irs_u32*)a_mac.val;
  MACIA1_bit.MACOCT5 = a_mac.val[4];
  MACIA1_bit.MACOCT6 = a_mac.val[5];
}
#elif defined(__STM32F100RBT__)
mxmac_t irs::arm::arm_ethernet_t::get_local_mac() const
{
  mxmac_t mac;
  *(irs_u32*)mac.val = 0;
  mac.val[4] = 0;
  mac.val[5] = 0;
  return mac;
}

void irs::arm::arm_ethernet_t::set_mac(mxmac_t& /*a_mac*/)
{
}
#elif defined(IRS_STM32F_2_AND_4)
#else
  #error Тип контроллера не определён
#endif  //  mcu type

void irs::arm::arm_ethernet_t::tick()
{
  if (m_send_packet_action && tx_buf_empty()) {
    m_send_packet_action = false;
    m_send_buf_locked = false;
  }
  if (m_rx_buf_handled) {
    if (m_use_interrupt == NO_USE_INT) {
      //m_rx_int_flag = (MACIS_bit.RXINT);
      if (packets_in_mac()) {
        m_rx_int_flag = true;
      }
    }

    bool can_read_fifo = ((m_buf_num == single_buf) && (!m_send_buf_locked)) ||
      (m_buf_num == double_buf);
    if (m_rx_int_flag && can_read_fifo) {
      m_rx_int_flag = false;
      irs_u32 fifo_data = 0;
      fifo_data = get_fifo();

      enum reset_fifo_t { rf_single_packet, rf_all_packet };
      reset_fifo_t reset_fifo_action = rf_single_packet;
      bool is_packet_valid = false;
      #ifdef ARMETH_DBGMSG
      enum { bug_marker = 0xDEADBEAF };
      volatile irs_size_t bug_finder = bug_marker;
      #endif //ARMETH_DBGMSG
      irs_size_t fifo_packet_size = IRS_LOWORD(fifo_data);
      if (fifo_packet_size > fifo_min_size) {
        #ifdef NEW_21092011
        m_rx_size = IRS_LOWORD(fifo_data);
        if (m_rx_size <= m_rx_buf_size) {
        #else //NEW_21092011
        m_rx_size = IRS_LOWORD(fifo_data) - L_size;
        if (m_rx_size <= m_buf_size) {
        #endif //NEW_21092011
          is_packet_valid = true;
        } else {
          is_packet_valid = false;
          if (m_rx_size <= max_packet_size) {
            reset_fifo_action = rf_single_packet;
          } else {
            reset_fifo_action = rf_all_packet;
          }
        }
      } else {
        m_rx_size = 0;
        is_packet_valid = false;
        reset_fifo_action = rf_all_packet;

        #ifdef ARMETH_DBGMSG
        mlog() << "Время: " << CNT_TO_DBLTIME(counter_get()) << endl;
        mlog() << "!!!!! ARM Ethernet Маленький пакет" << endl;
        #endif //ARMETH_DBGMSG
      }

      if (is_packet_valid) {
        *(irs_u16*)mp_rx_buf = IRS_HIWORD(fifo_data);

        const irs_size_t shift = sizeof(irs_u16);
        irs_u32* p_shifted_buf = reinterpret_cast<irs_u32*>(mp_rx_buf + shift);

        const irs_size_t whole_dwords_cnt
          = (m_rx_size - shift) / sizeof(fifo_data);
        irs_size_t current_dword = 0;

        #ifndef NOP
        #ifdef ARMETH_DBGMSG
        irs_u8* p_rx_buf_end = mp_rx_buf + m_rx_buf_size;
        #endif //ARMETH_DBGMSG
        #endif //NOP

        for (; current_dword < whole_dwords_cnt; current_dword++) {

          #ifndef NOP
          #ifdef ARMETH_DBGMSG
          irs_u8* p_begin = reinterpret_cast<irs_u8*>(p_shifted_buf +
            current_dword);
          irs_u8* p_end = p_begin + sizeof(*p_shifted_buf);
          bool is_begin_bad =
            (p_begin < mp_rx_buf) || (p_rx_buf_end < p_begin);
          bool is_end_bad =
            (p_end < mp_rx_buf) || (p_rx_buf_end < p_end);
          if (is_begin_bad || is_end_bad) {
            mlog() << boolalpha;
            mlog() << "Время: " << CNT_TO_DBLTIME(counter_get()) << endl;
            mlog() << "Косяк p_shifted_buf[current_dword] = MACDATA;" << endl;
            mlog() << "is_begin_bad = " << is_begin_bad << endl;
            mlog() << "is_end_bad = " << is_begin_bad << endl;
            continue;
          }
          #endif //ARMETH_DBGMSG
          #endif //NOP

          p_shifted_buf[current_dword] = get_fifo();
        }

        fifo_data = get_fifo();
        irs_u8* p_fifo_data = reinterpret_cast<irs_u8*>(&fifo_data);
        const irs_size_t start_byte =
          current_dword * sizeof(fifo_data) + shift;
        for (
          irs_size_t current_byte = start_byte;
          current_byte < m_rx_size;
          current_byte++
        ) {
          mp_rx_buf[current_byte] = *p_fifo_data;
          p_fifo_data++;
        }
        m_rx_buf_filled = true;
        m_rx_buf_handled = false;
      } else {
        switch (reset_fifo_action) {
          case rf_single_packet: {
            const irs_size_t skip_dwords_cnt = spec_div(m_rx_size -
              (sizeof(fifo_data) - L_size), sizeof(fifo_data));
            for (irs_size_t i = 0; i < skip_dwords_cnt; i++) {
              fifo_data = get_fifo();
            }
          } break;
          case rf_all_packet: {
            reset_fifo();
          } break;
        }
      }

      #ifdef ARMETH_DBGMSG
      if (bug_finder != bug_marker) {
        mlog() << "Время: " << CNT_TO_DBLTIME(counter_get()) << endl;
        mlog() << "!!!!! ARM Ethernet Поймана запись мимо памяти" << endl;
      }
      #endif //ARMETH_DBGMSG

      if (m_use_interrupt == USE_INT) {
        clear_rx_interrupt();
      }
    }
  }
}


#elif defined IRS_STM32F_2_AND_4

extern ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB];
extern ETH_DMADESCTypeDef  DMATxDscrTab[ETH_TXBUFNB];
extern uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE];
extern uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE];
extern ETH_DMADESCTypeDef  *DMATxDescToSet;
extern ETH_DMADESCTypeDef  *DMARxDescToGet;
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;

// class st_ethernet_t
irs::arm::st_ethernet_t::st_ethernet_t(
  size_t a_buf_size,
  mxmac_t& a_mac,
  const config_t a_config
):
  m_receive_buf(a_buf_size),
  m_transmit_buf(a_buf_size),
  m_config(a_config)
{
  rcc_configuration();
  gpio_configuration();

  /* Reset ETHERNET on AHB Bus */
  ETH_DeInit();

  /* Software reset */
  ETH_SoftwareReset();

  /* Wait for software reset */
  while(ETH_GetSoftwareResetStatus()==SET);

  ETH_InitTypeDef ETH_InitStructure;
  /* ETHERNET Configuration ------------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);

  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable; 
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;  
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
    
  ETH_Init(&ETH_InitStructure, phy_address);  
  ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

  set_mac(a_mac);

  ETH_Start();
}

void irs::arm::st_ethernet_t::rcc_configuration()
{
  //RCC->APB2ENR |= (1 << 14);	
  RCC_APB2ENR_bit.SYSCFGEN = 1;
  if (m_config.mii_mode == normal_mii_mode) {
    SYSCFG_PMC_bit.MII_RMII_SEL = 0;
  } else {
    SYSCFG_PMC_bit.MII_RMII_SEL = 1;
    SYSCFG_CMPCR_bit.CMP_PD = 1;
  }
  /*#ifdef MII_MODE
  SYSCFG->PMC &= (~(1<<23));
  #else
  SYSCFG->PMC |= (1 << 23);
  #endif*/
  //SYSCFG->CMPCR |= (1 << 0);
  /*
  RCC AHB1 peripheral clock register (RCC_AHB1ENR)
  Bit 25 ETHMACEN: Ethernet MAC clock enable
  Bit 26 ETHMACTXEN: Ethernet Transmission clock enable
  Bit 27 ETHMACRXEN: Ethernet Reception clock enable
  */
  RCC_AHB2ENR_bit.HASHEN = 1;
  //RCC->AHB2ENR |= (1 << 5);
  RCC_AHB1ENR_bit.CRCEN = 1;
  RCC_AHB1ENR_bit.ETHMACRXEN = 1;
  RCC_AHB1ENR_bit.ETHMACTXEN = 1;
  RCC_AHB1ENR_bit.ETHMACEN = 1;
  //RCC->AHB1ENR |= (1 << 25) + (1 << 26) + (1 << 27) + (1 << 12);
}

void irs::arm::st_ethernet_t::gpio_configuration()
{
  if (m_config.mii_mode == normal_mii_mode) {
    clock_enable(m_config.txd[2]);
    gpio_moder_alternate_function_enable(m_config.txd[2]);
    gpio_alternate_function_select(m_config.txd[2], GPIO_AF_ETH);

    clock_enable(m_config.txd[3]);
    gpio_moder_alternate_function_enable(m_config.txd[3]);
    gpio_alternate_function_select(m_config.txd[3], GPIO_AF_ETH);

    clock_enable(m_config.rxd[2]);
    gpio_moder_alternate_function_enable(m_config.rxd[2]);
    gpio_alternate_function_select(m_config.rxd[2], GPIO_AF_ETH);

    clock_enable(m_config.rxd[3]);
    gpio_moder_alternate_function_enable(m_config.rxd[3]);
    gpio_alternate_function_select(m_config.rxd[3], GPIO_AF_ETH);

    clock_enable(m_config.tx_clk);
    gpio_moder_alternate_function_enable(m_config.rxd[3]);
    gpio_alternate_function_select(m_config.rxd[3], GPIO_AF_ETH);

    clock_enable(m_config.rx_er);
    gpio_moder_alternate_function_enable(m_config.rx_er);
    gpio_alternate_function_select(m_config.rx_er, GPIO_AF_ETH);

    clock_enable(m_config.rx_crs);
    gpio_moder_alternate_function_enable(m_config.rx_crs);
    gpio_alternate_function_select(m_config.rx_crs, GPIO_AF_ETH);

    clock_enable(m_config.col);
    gpio_moder_alternate_function_enable(m_config.col);
    gpio_alternate_function_select(m_config.col, GPIO_AF_ETH);
  }
  clock_enable(m_config.rx_clk_or_ref_clk);
  gpio_moder_alternate_function_enable(m_config.rx_clk_or_ref_clk);
  gpio_alternate_function_select(m_config.rx_clk_or_ref_clk, GPIO_AF_ETH);

  clock_enable(m_config.txd[0]);
  gpio_moder_alternate_function_enable(m_config.txd[0]);
  gpio_alternate_function_select(m_config.txd[0], GPIO_AF_ETH);

  clock_enable(m_config.txd[1]);
  gpio_moder_alternate_function_enable(m_config.txd[1]);
  gpio_alternate_function_select(m_config.txd[1], GPIO_AF_ETH);

  clock_enable(m_config.rxd[0]);
  gpio_moder_alternate_function_enable(m_config.rxd[0]);
  gpio_alternate_function_select(m_config.rxd[0], GPIO_AF_ETH);

  clock_enable(m_config.rxd[1]);
  gpio_moder_alternate_function_enable(m_config.rxd[1]);
  gpio_alternate_function_select(m_config.rxd[1], GPIO_AF_ETH);

  clock_enable(m_config.tx_en);
  gpio_moder_alternate_function_enable(m_config.tx_en);
  gpio_alternate_function_select(m_config.tx_en, GPIO_AF_ETH);

  clock_enable(m_config.rx_dv_or_crs_dv);
  gpio_moder_alternate_function_enable(m_config.rx_dv_or_crs_dv);
  gpio_alternate_function_select(m_config.rx_dv_or_crs_dv, GPIO_AF_ETH);

  clock_enable(m_config.mdc);
  gpio_moder_alternate_function_enable(m_config.mdc);
  gpio_alternate_function_select(m_config.mdc, GPIO_AF_ETH);

  clock_enable(m_config.mdio);
  gpio_otyper_output_open_drain_enable(m_config.mdio);
  gpio_moder_alternate_function_enable(m_config.mdio);
  gpio_alternate_function_select(m_config.mdio, GPIO_AF_ETH);

  if (m_config.mii_mode == normal_mii_mode) {
    gpio_ospeedr_select(m_config.txd[0], IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.txd[1], IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.txd[2], IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.txd[3], IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.rxd[0], IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.rxd[1], IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.rxd[2], IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.rxd[3], IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.tx_clk, IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.rx_er, IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.rx_crs, IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.col, IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.rx_clk_or_ref_clk, IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.tx_en, IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.rx_dv_or_crs_dv, IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.mdc, IRS_GPIO_SPEED_25MHZ);
    gpio_ospeedr_select(m_config.mdio, IRS_GPIO_SPEED_25MHZ);
  } else {
    gpio_ospeedr_select(m_config.rx_clk_or_ref_clk, IRS_GPIO_SPEED_50MHZ);
    gpio_ospeedr_select(m_config.txd[0], IRS_GPIO_SPEED_100MHZ);
    gpio_ospeedr_select(m_config.txd[1], IRS_GPIO_SPEED_100MHZ);
    gpio_ospeedr_select(m_config.rxd[0], IRS_GPIO_SPEED_100MHZ);
    gpio_ospeedr_select(m_config.rxd[1], IRS_GPIO_SPEED_100MHZ);
    gpio_ospeedr_select(m_config.tx_en, IRS_GPIO_SPEED_50MHZ);
    gpio_ospeedr_select(m_config.rx_dv_or_crs_dv, IRS_GPIO_SPEED_100MHZ);
    gpio_ospeedr_select(m_config.mdc, IRS_GPIO_SPEED_50MHZ);
    gpio_ospeedr_select(m_config.mdio, IRS_GPIO_SPEED_50MHZ);
  }
  #ifdef NOP
  /* Enable GPIOA, GPIOB, GPIOC, GPIOG clock */
  RCC->AHB1ENR  |= (1<<0) + (1<<1) + (1<<2) + (1<<6);	

  GPIO_InitTypeDef GPIO_InitStructure;

  // Конфигурация ног  PG13, PG14
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);

  /* Configure PB11 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);

  /* Configure PA7 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource7, GPIO_AF_ETH);

  /* Configure PC4 and PC5 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource4, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource5, GPIO_AF_ETH);

  /* Configure PC1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource1, GPIO_AF_ETH);

  /* Configure PA2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);

  /* Configure PA1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);

  /* Configure PA8 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
  #endif // NOP
}

irs::arm::st_ethernet_t::~st_ethernet_t()
{
}

void irs::arm::st_ethernet_t::send_packet(irs_size_t a_size)
{
  m_transmit_buf.data_size = a_size;
  m_transmit_buf.status = buf_filled;
}

void irs::arm::st_ethernet_t::set_recv_handled()
{
  m_receive_buf.status = buf_empty;
}

void irs::arm::st_ethernet_t::set_send_buf_locked()
{
  m_transmit_buf.status = buf_locked;
}

bool irs::arm::st_ethernet_t::is_recv_buf_filled() const
{
  return m_receive_buf.status == buf_filled;
}

bool irs::arm::st_ethernet_t::is_send_buf_empty() const
{
  return m_transmit_buf.status == buf_empty;
}

irs_u8* irs::arm::st_ethernet_t::get_recv_buf()
{
  return m_receive_buf.begin();
}

irs_u8* irs::arm::st_ethernet_t::get_send_buf()
{
  return m_transmit_buf.begin();
}

irs::arm::st_ethernet_t::size_type
irs::arm::st_ethernet_t::recv_buf_size() const
{
  return m_receive_buf.data_size + frame_check_sequence_size;
}

irs::arm::st_ethernet_t::size_type
irs::arm::st_ethernet_t::send_buf_max_size() const
{
  return m_transmit_buf.size();
}

irs::arm::st_ethernet_t::buffer_num_t
irs::arm::st_ethernet_t::get_buf_num() const
{
  return double_buf;
}

mxmac_t irs::arm::st_ethernet_t::get_local_mac() const
{
  mxmac_t mac;
  ETH_GetMACAddress(ETH_MAC_Address0, mac.val);
  return mac;
}

void irs::arm::st_ethernet_t::set_mac(mxmac_t& a_mac)
{
  ETH_MACAddressConfig(ETH_MAC_Address0, a_mac.val);
}

void irs::arm::st_ethernet_t::tick()
{
  if (m_receive_buf.status == buf_empty) {
    if (ETH_CheckFrameReceived()) {
      receive();
    }
  }
  if (m_transmit_buf.status == buf_filled) {
    transmit();
  }
}

void irs::arm::st_ethernet_t::transmit()
{
  if ((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) == 0) {
    irs_u8 *buffer =  (irs_u8 *)(DMATxDescToSet->Buffer1Addr);
    const size_t size = min(m_transmit_buf.data_size,
      static_cast<size_t>(ETH_MAX_PACKET_SIZE));
    irs::memcpyex(buffer, m_transmit_buf.begin(), size);
    ETH_Prepare_Transmit_Descriptors(m_transmit_buf.data_size);
    m_transmit_buf.status = buf_empty;
    m_transmit_buf.data_size = 0;
  }
}

void irs::arm::st_ethernet_t::receive()
{  
  __IO ETH_DMADESCTypeDef *DMARxNextDesc;
  FrameTypeDef frame = ETH_Get_Received_Frame();
  //irs_u16 length = frame.length;
  if (frame.length > 0) {
    irs_u8* buffer = reinterpret_cast<irs_u8*>(frame.buffer);
    const size_t length = min(frame.length, m_receive_buf.size());
    irs::memcpyex(m_receive_buf.begin(), buffer, length);
    m_receive_buf.data_size = length;
    m_receive_buf.status = buf_filled;
  }

  if (DMA_RX_FRAME_infos->Seg_Count > 1) {
    DMARxNextDesc = DMA_RX_FRAME_infos->FS_Rx_Desc;
  } else {
    DMARxNextDesc = frame.descriptor;
  }

  for (size_type i = 0; i<DMA_RX_FRAME_infos->Seg_Count; i++) {
    DMARxNextDesc->Status = ETH_DMARxDesc_OWN;
    DMARxNextDesc = (ETH_DMADESCTypeDef *)(DMARxNextDesc->Buffer2NextDescAddr);
  }

  DMA_RX_FRAME_infos->Seg_Count =0;
  if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET) {
    ETH->DMASR = ETH_DMASR_RBUS;
    ETH->DMARPDR = 0;
  }  
}
#else
  #error Тип контроллера не определён
#endif // mcu type
