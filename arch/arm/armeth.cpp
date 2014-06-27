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
#include <irssysutils.h>

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

// class eth_auto_negotation_t
irs::arm::st_ethernet_t::eth_auto_negotation_t::eth_auto_negotation_t(
  const ETH_InitTypeDef& ETH_InitStruct,
  const irs_u16 a_phy_address
):
  m_process(process_reset),
  m_phy_address(a_phy_address),
  m_auto_negotation(ETH_InitStruct.ETH_AutoNegotiation),
  m_speed(ETH_InitStruct.ETH_Speed),
  m_mode(ETH_InitStruct.ETH_Mode),
  m_speed_or_mode_phy_changed(true),
  m_delay_assure_reset_timer(make_cnt_s(0.1)),
  m_speed_and_mode_check_timer(make_cnt_s(1)),
  m_phy_register(0),
  m_complete(false)
{
  assert_param(IS_ETH_AUTONEGOTIATION(ETH_InitStruct->ETH_AutoNegotiation));
  assert_param(IS_ETH_SPEED(ETH_InitStruct->ETH_Speed));
  assert_param(IS_ETH_DUPLEX_MODE(ETH_InitStruct->ETH_Mode));

  set_speed_and_mode_mac();
}

bool irs::arm::st_ethernet_t::eth_auto_negotation_t::complete() const
{
  return m_complete;
}

void irs::arm::st_ethernet_t::eth_auto_negotation_t::tick()
{
  switch(m_process) {
    case process_reset: {
      if(ETH_WritePHYRegister(m_phy_address, PHY_BCR, PHY_Reset)) {
        m_delay_assure_reset_timer.start();
        m_process = process_delay_assure_reset;
      }
    } break;
    case process_delay_assure_reset: {
      if (m_delay_assure_reset_timer.check()) {
        if(m_auto_negotation == ETH_AutoNegotiation_Enable) {
          if(ETH_WritePHYRegister(m_phy_address, PHY_BCR,
            PHY_AutoNegotiation)) {
            m_process = process_auto_negotation;
          } else {
            m_process = process_reset;
          }
        } else {
          if (set_speed_and_mode_phy()) {
            m_complete = true;
            m_process = process_nothing;
          } else {
            m_process = process_reset;
          }
        }
      }
    } break;
    case process_auto_negotation: {
      if((m_auto_negotation == ETH_AutoNegotiation_Enable) &&
        m_speed_and_mode_check_timer.check()) {
        if (ETH_ReadPHYRegister(m_phy_address, PHY_BSR) &
          PHY_AutoNego_Complete) {
          read_speed_and_mode_phy();
          if (m_speed_or_mode_phy_changed) {
            set_speed_and_mode_mac();
            m_speed_or_mode_phy_changed = false;
          }
          m_complete = true;
          //m_process = process_auto_negotation;
        }
      }
    } break;
    case process_nothing: {
      // Ничего не делаем
    } break;
  }
}

void irs::arm::st_ethernet_t::eth_auto_negotation_t::read_speed_and_mode_phy()
{
  const irs_u32 prev_mode = m_mode;
  const irs_u32 prev_speed = m_speed;
  irs_u32 RegValue = ETH_ReadPHYRegister(m_phy_address, PHY_SR);
  if((RegValue & PHY_DUPLEX_STATUS) != (uint32_t)RESET) {
    m_mode = ETH_Mode_FullDuplex;
  } else {
    m_mode = ETH_Mode_HalfDuplex;
  }
  if(RegValue & PHY_SPEED_STATUS) {
    m_speed = ETH_Speed_10M;
  } else {
    m_speed = ETH_Speed_100M;
  }
  m_speed_or_mode_phy_changed =
    (prev_mode != m_mode) || (prev_speed != m_speed);
}

bool irs::arm::st_ethernet_t::eth_auto_negotation_t::set_speed_and_mode_phy()
{
  return ETH_WritePHYRegister(m_phy_address, PHY_BCR, (
    (uint16_t)(m_mode >> 3) | (uint16_t)(m_speed >> 1)));
}

void irs::arm::st_ethernet_t::eth_auto_negotation_t::set_speed_and_mode_mac()
{
  ETH_MACCR_bit.FES = (m_speed == ETH_Speed_100M) ? 1 : 0;
  ETH_MACCR_bit.DM = (m_mode == ETH_Mode_FullDuplex) ? 1 : 0;
}

// class st_ethernet_t
irs::arm::st_ethernet_t::st_ethernet_t(
  size_t a_recv_buf_size,
  mxmac_t& a_mac,
  const config_t a_config,
  tx_checksum_insertion_control_t a_tx_checksum_insertion_control
):
  m_receive_buf(a_recv_buf_size),
  m_transmit_buf(a_recv_buf_size - frame_check_sequence_size),
  m_config(a_config),
  mp_eth_auto_negotation(IRS_NULL)
{
  rcc_configuration();
  gpio_configuration();

  /* Reset ETHERNET on AHB Bus */
  ETH_DeInit();

  /* Software reset */
  ETH_SoftwareReset();
  /* Wait for software reset */
  while(ETH_GetSoftwareResetStatus() == SET);

  clock_range_configuration();

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
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;

  mac_configuration(&ETH_InitStructure);

  hardware_reset_phy();

  mp_eth_auto_negotation.reset(
    new eth_auto_negotation_t(ETH_InitStructure, phy_address));

  ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

  // Конфигурация расчета контрольной суммы
  for(int i = 0; i<ETH_TXBUFNB; i++) {
    ETH_DMATxDescChecksumInsertionConfig(
      &DMATxDscrTab[i], a_tx_checksum_insertion_control);
  }

  set_mac(a_mac);

  ETH_Start();
}

void irs::arm::st_ethernet_t::hardware_reset_phy()
{
  io_pin_t reset_eth_pin(m_config.phy_reset, io_t::dir_out, io_pin_off);
  pause(irs::make_cnt_us(100));
  reset_eth_pin.set();
  pause(irs::make_cnt_us(100));
}

void irs::arm::st_ethernet_t::rcc_configuration()
{
  RCC_APB2ENR_bit.SYSCFGEN = 1;
  if (m_config.mii_mode == normal_mii_mode) {
    SYSCFG_PMC_bit.MII_RMII_SEL = 0;
  } else {
    SYSCFG_PMC_bit.MII_RMII_SEL = 1;
    SYSCFG_CMPCR_bit.CMP_PD = 1;
  }

  RCC_AHB2ENR_bit.HASHEN = 1;
  RCC_AHB1ENR_bit.CRCEN = 1;
  RCC_AHB1ENR_bit.ETHMACRXEN = 1;
  RCC_AHB1ENR_bit.ETHMACTXEN = 1;
  RCC_AHB1ENR_bit.ETHMACEN = 1;
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
}

void irs::arm::st_ethernet_t::clock_range_configuration()
{
  RCC_ClocksTypeDef  rcc_clocks;
  uint32_t hclk = 60000000;
  /* Get the ETHERNET MACMIIAR value */
  uint32_t tmpreg = ETH->MACMIIAR;
  /* Clear CSR Clock Range CR[2:0] bits */
  tmpreg &= MACMIIAR_CR_MASK;
  /* Get hclk frequency value */
  RCC_GetClocksFreq(&rcc_clocks);
  hclk = rcc_clocks.HCLK_Frequency;

  /* Set CR bits depending on hclk value */
  if((hclk >= 20000000)&&(hclk < 35000000)) {
    /* CSR Clock Range between 20-35 MHz */
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div16;
  } else if((hclk >= 35000000)&&(hclk < 60000000)) {
    /* CSR Clock Range between 35-60 MHz */
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div26;
  } else if((hclk >= 60000000)&&(hclk < 100000000)) {
    /* CSR Clock Range between 60-100 MHz */
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div42;
  } else if((hclk >= 100000000)&&(hclk < 150000000)) {
    /* CSR Clock Range between 100-150 MHz */
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div62;
  } else /* ((hclk >= 150000000)&&(hclk <= 168000000)) */ {
    #ifdef IRS_STM32F2xx
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div62;
    #else // IRS_STM32F4xx
    /* CSR Clock Range between 150-168 MHz */
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div102;
    #endif // IRS_STM32F4xx
  }
  /* Write to ETHERNET MAC MIIAR: Configure the ETHERNET CSR Clock Range */
  ETH->MACMIIAR = (uint32_t)tmpreg;
}

void irs::arm::st_ethernet_t::mac_configuration(
  const ETH_InitTypeDef* ETH_InitStruct)
{
  assert_param(IS_ETH_AUTONEGOTIATION(ETH_InitStruct->ETH_AutoNegotiation));
  assert_param(IS_ETH_WATCHDOG(ETH_InitStruct->ETH_Watchdog));
  assert_param(IS_ETH_JABBER(ETH_InitStruct->ETH_Jabber));
  assert_param(IS_ETH_INTER_FRAME_GAP(ETH_InitStruct->ETH_InterFrameGap));
  assert_param(IS_ETH_CARRIER_SENSE(ETH_InitStruct->ETH_CarrierSense));
  assert_param(IS_ETH_SPEED(ETH_InitStruct->ETH_Speed));
  assert_param(IS_ETH_RECEIVE_OWN(ETH_InitStruct->ETH_ReceiveOwn));
  assert_param(IS_ETH_LOOPBACK_MODE(ETH_InitStruct->ETH_LoopbackMode));
  assert_param(IS_ETH_DUPLEX_MODE(ETH_InitStruct->ETH_Mode));
  assert_param(IS_ETH_CHECKSUM_OFFLOAD(ETH_InitStruct->ETH_ChecksumOffload));
  assert_param(IS_ETH_RETRY_TRANSMISSION(
    ETH_InitStruct->ETH_RetryTransmission));
  assert_param(IS_ETH_AUTOMATIC_PADCRC_STRIP(
    ETH_InitStruct->ETH_AutomaticPadCRCStrip));
  assert_param(IS_ETH_BACKOFF_LIMIT(ETH_InitStruct->ETH_BackOffLimit));
  assert_param(IS_ETH_DEFERRAL_CHECK(ETH_InitStruct->ETH_DeferralCheck));
  assert_param(IS_ETH_RECEIVE_ALL(ETH_InitStruct->ETH_ReceiveAll));
  assert_param(IS_ETH_SOURCE_ADDR_FILTER(ETH_InitStruct->ETH_SourceAddrFilter));
  assert_param(IS_ETH_CONTROL_FRAMES(ETH_InitStruct->ETH_PassControlFrames));
  assert_param(IS_ETH_BROADCAST_FRAMES_RECEPTION(
    ETH_InitStruct->ETH_BroadcastFramesReception));
  assert_param(IS_ETH_DESTINATION_ADDR_FILTER(
    ETH_InitStruct->ETH_DestinationAddrFilter));
  assert_param(IS_ETH_PROMISCIOUS_MODE(
    ETH_InitStruct->ETH_PromiscuousMode));
  assert_param(IS_ETH_MULTICAST_FRAMES_FILTER(
    ETH_InitStruct->ETH_MulticastFramesFilter));
  assert_param(IS_ETH_UNICAST_FRAMES_FILTER(
    ETH_InitStruct->ETH_UnicastFramesFilter));
  assert_param(IS_ETH_PAUSE_TIME(ETH_InitStruct->ETH_PauseTime));
  assert_param(IS_ETH_ZEROQUANTA_PAUSE(ETH_InitStruct->ETH_ZeroQuantaPause));
  assert_param(IS_ETH_PAUSE_LOW_THRESHOLD(
    ETH_InitStruct->ETH_PauseLowThreshold));
  assert_param(IS_ETH_UNICAST_PAUSE_FRAME_DETECT(
    ETH_InitStruct->ETH_UnicastPauseFrameDetect));
  assert_param(IS_ETH_RECEIVE_FLOWCONTROL(
    ETH_InitStruct->ETH_ReceiveFlowControl));
  assert_param(IS_ETH_TRANSMIT_FLOWCONTROL(
    ETH_InitStruct->ETH_TransmitFlowControl));
  assert_param(IS_ETH_VLAN_TAG_COMPARISON(
    ETH_InitStruct->ETH_VLANTagComparison));
  assert_param(IS_ETH_VLAN_TAG_IDENTIFIER(
    ETH_InitStruct->ETH_VLANTagIdentifier));
  /* DMA --------------------------*/
  assert_param(IS_ETH_DROP_TCPIP_CHECKSUM_FRAME(
    ETH_InitStruct->ETH_DropTCPIPChecksumErrorFrame));
  assert_param(IS_ETH_RECEIVE_STORE_FORWARD(
    ETH_InitStruct->ETH_ReceiveStoreForward));
  assert_param(IS_ETH_FLUSH_RECEIVE_FRAME(
    ETH_InitStruct->ETH_FlushReceivedFrame));
  assert_param(IS_ETH_TRANSMIT_STORE_FORWARD(
    ETH_InitStruct->ETH_TransmitStoreForward));
  assert_param(IS_ETH_TRANSMIT_THRESHOLD_CONTROL(
    ETH_InitStruct->ETH_TransmitThresholdControl));
  assert_param(IS_ETH_FORWARD_ERROR_FRAMES(
    ETH_InitStruct->ETH_ForwardErrorFrames));
  assert_param(IS_ETH_FORWARD_UNDERSIZED_GOOD_FRAMES(
    ETH_InitStruct->ETH_ForwardUndersizedGoodFrames));
  assert_param(IS_ETH_RECEIVE_THRESHOLD_CONTROL(
    ETH_InitStruct->ETH_ReceiveThresholdControl));
  assert_param(IS_ETH_SECOND_FRAME_OPERATE(
    ETH_InitStruct->ETH_SecondFrameOperate));
  assert_param(IS_ETH_ADDRESS_ALIGNED_BEATS(
    ETH_InitStruct->ETH_AddressAlignedBeats));
  assert_param(IS_ETH_FIXED_BURST(ETH_InitStruct->ETH_FixedBurst));
  assert_param(IS_ETH_RXDMA_BURST_LENGTH(ETH_InitStruct->ETH_RxDMABurstLength));
  assert_param(IS_ETH_TXDMA_BURST_LENGTH(ETH_InitStruct->ETH_TxDMABurstLength));
  assert_param(IS_ETH_DMA_DESC_SKIP_LENGTH(
    ETH_InitStruct->ETH_DescriptorSkipLength));
  assert_param(IS_ETH_DMA_ARBITRATION_ROUNDROBIN_RXTX(
    ETH_InitStruct->ETH_DMAArbitration));

  /* Get the ETHERNET MACCR value */
  uint32_t tmpreg = ETH->MACCR;
  /* Clear WD, PCE, PS, TE and RE bits */
  tmpreg &= MACCR_CLEAR_MASK;
  /* Set the WD bit according to ETH_Watchdog value */
  /* Set the JD: bit according to ETH_Jabber value */
  /* Set the IFG bit according to ETH_InterFrameGap value */
  /* Set the DCRS bit according to ETH_CarrierSense value */
  /* Set the FES bit according to ETH_Speed value */
  /* Set the DO bit according to ETH_ReceiveOwn value */
  /* Set the LM bit according to ETH_LoopbackMode value */
  /* Set the DM bit according to ETH_Mode value */
  /* Set the IPCO bit according to ETH_ChecksumOffload value */
  /* Set the DR bit according to ETH_RetryTransmission value */
  /* Set the ACS bit according to ETH_AutomaticPadCRCStrip value */
  /* Set the BL bit according to ETH_BackOffLimit value */
  /* Set the DC bit according to ETH_DeferralCheck value */
  tmpreg |= (uint32_t)(ETH_InitStruct->ETH_Watchdog |
    ETH_InitStruct->ETH_Jabber |
    ETH_InitStruct->ETH_InterFrameGap |
    ETH_InitStruct->ETH_CarrierSense |
    ETH_InitStruct->ETH_Speed |
    ETH_InitStruct->ETH_ReceiveOwn |
    ETH_InitStruct->ETH_LoopbackMode |
    ETH_InitStruct->ETH_Mode |
    ETH_InitStruct->ETH_ChecksumOffload |
    ETH_InitStruct->ETH_RetryTransmission |
    ETH_InitStruct->ETH_AutomaticPadCRCStrip |
    ETH_InitStruct->ETH_BackOffLimit |
    ETH_InitStruct->ETH_DeferralCheck);
  /* Write to ETHERNET MACCR */
  ETH->MACCR = (uint32_t)tmpreg;

  /*----------------------- ETHERNET MACFFR Configuration --------------------*/
  /* Set the RA bit according to ETH_ReceiveAll value */
  /* Set the SAF and SAIF bits according to ETH_SourceAddrFilter value */
  /* Set the PCF bit according to ETH_PassControlFrames value */
  /* Set the DBF bit according to ETH_BroadcastFramesReception value */
  /* Set the DAIF bit according to ETH_DestinationAddrFilter value */
  /* Set the PR bit according to ETH_PromiscuousMode value */
  /* Set the PM, HMC and HPF bits according to ETH_MulticastFramesFilter value*/
  /* Set the HUC and HPF bits according to ETH_UnicastFramesFilter value */
  /* Write to ETHERNET MACFFR */
  ETH->MACFFR = (uint32_t)(ETH_InitStruct->ETH_ReceiveAll |
    ETH_InitStruct->ETH_SourceAddrFilter |
    ETH_InitStruct->ETH_PassControlFrames |
    ETH_InitStruct->ETH_BroadcastFramesReception |
    ETH_InitStruct->ETH_DestinationAddrFilter |
    ETH_InitStruct->ETH_PromiscuousMode |
    ETH_InitStruct->ETH_MulticastFramesFilter |
    ETH_InitStruct->ETH_UnicastFramesFilter);
  /*--------------- ETHERNET MACHTHR and MACHTLR Configuration ---------------*/
  /* Write to ETHERNET MACHTHR */
  ETH->MACHTHR = (uint32_t)ETH_InitStruct->ETH_HashTableHigh;
  /* Write to ETHERNET MACHTLR */
  ETH->MACHTLR = (uint32_t)ETH_InitStruct->ETH_HashTableLow;
  /*----------------------- ETHERNET MACFCR Configuration --------------------*/
  /* Get the ETHERNET MACFCR value */
  tmpreg = ETH->MACFCR;
  /* Clear xx bits */
  tmpreg &= MACFCR_CLEAR_MASK;

  /* Set the PT bit according to ETH_PauseTime value */
  /* Set the DZPQ bit according to ETH_ZeroQuantaPause value */
  /* Set the PLT bit according to ETH_PauseLowThreshold value */
  /* Set the UP bit according to ETH_UnicastPauseFrameDetect value */
  /* Set the RFE bit according to ETH_ReceiveFlowControl value */
  /* Set the TFE bit according to ETH_TransmitFlowControl value */
  tmpreg |= (uint32_t)((ETH_InitStruct->ETH_PauseTime << 16) |
    ETH_InitStruct->ETH_ZeroQuantaPause |
    ETH_InitStruct->ETH_PauseLowThreshold |
    ETH_InitStruct->ETH_UnicastPauseFrameDetect |
    ETH_InitStruct->ETH_ReceiveFlowControl |
    ETH_InitStruct->ETH_TransmitFlowControl);
  /* Write to ETHERNET MACFCR */
  ETH->MACFCR = (uint32_t)tmpreg;
  /*----------------------- ETHERNET MACVLANTR Configuration -----------------*/
  /* Set the ETV bit according to ETH_VLANTagComparison value */
  /* Set the VL bit according to ETH_VLANTagIdentifier value */
  ETH->MACVLANTR = (uint32_t)(ETH_InitStruct->ETH_VLANTagComparison |
                             ETH_InitStruct->ETH_VLANTagIdentifier);

  /*-------------------------------- DMA Config ------------------------------*/
  /*----------------------- ETHERNET DMAOMR Configuration --------------------*/
  /* Get the ETHERNET DMAOMR value */
  tmpreg = ETH->DMAOMR;
  /* Clear xx bits */
  tmpreg &= DMAOMR_CLEAR_MASK;

  /* Set the DT bit according to ETH_DropTCPIPChecksumErrorFrame value */
  /* Set the RSF bit according to ETH_ReceiveStoreForward value */
  /* Set the DFF bit according to ETH_FlushReceivedFrame value */
  /* Set the TSF bit according to ETH_TransmitStoreForward value */
  /* Set the TTC bit according to ETH_TransmitThresholdControl value */
  /* Set the FEF bit according to ETH_ForwardErrorFrames value */
  /* Set the FUF bit according to ETH_ForwardUndersizedGoodFrames value */
  /* Set the RTC bit according to ETH_ReceiveThresholdControl value */
  /* Set the OSF bit according to ETH_SecondFrameOperate value */
  tmpreg |= (uint32_t)(ETH_InitStruct->ETH_DropTCPIPChecksumErrorFrame |
    ETH_InitStruct->ETH_ReceiveStoreForward |
    ETH_InitStruct->ETH_FlushReceivedFrame |
    ETH_InitStruct->ETH_TransmitStoreForward |
    ETH_InitStruct->ETH_TransmitThresholdControl |
    ETH_InitStruct->ETH_ForwardErrorFrames |
    ETH_InitStruct->ETH_ForwardUndersizedGoodFrames |
    ETH_InitStruct->ETH_ReceiveThresholdControl |
    ETH_InitStruct->ETH_SecondFrameOperate);
  /* Write to ETHERNET DMAOMR */
  ETH->DMAOMR = (uint32_t)tmpreg;

  /*----------------------- ETHERNET DMABMR Configuration --------------------*/
  /* Set the AAL bit according to ETH_AddressAlignedBeats value */
  /* Set the FB bit according to ETH_FixedBurst value */
  /* Set the RPBL and 4*PBL bits according to ETH_RxDMABurstLength value */
  /* Set the PBL and 4*PBL bits according to ETH_TxDMABurstLength value */
  /* Set the DSL bit according to ETH_DesciptorSkipLength value */
  /* Set the PR and DA bits according to ETH_DMAArbitration value */
  ETH->DMABMR = (uint32_t)(ETH_InitStruct->ETH_AddressAlignedBeats |
    ETH_InitStruct->ETH_FixedBurst |
    /* !! if 4xPBL is selected for Tx or Rx it is applied for the other */
    ETH_InitStruct->ETH_RxDMABurstLength |
    ETH_InitStruct->ETH_TxDMABurstLength |
    (ETH_InitStruct->ETH_DescriptorSkipLength << 2) |
    ETH_InitStruct->ETH_DMAArbitration |
    ETH_DMABMR_USP); /* Enable use of separate PBL for Rx and Tx */

  #ifdef USE_ENHANCED_DMA_DESCRIPTORS
  /* Enable the Enhanced DMA descriptors */
  ETH->DMABMR |= ETH_DMABMR_EDE;
  #endif /* USE_ENHANCED_DMA_DESCRIPTORS */
}

irs::arm::st_ethernet_t::~st_ethernet_t()
{
}

void irs::arm::st_ethernet_t::send_packet(irs_size_t a_size)
{
  m_transmit_buf.data_size = a_size;
  m_transmit_buf.status = buf_filled;
  transmit();
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

bool irs::arm::st_ethernet_t::get_ready_status() const
{
  return mp_eth_auto_negotation->complete() && get_linked_status();
}

bool irs::arm::st_ethernet_t::get_linked_status() const
{
  return ETH_ReadPHYRegister(phy_address, PHY_BSR) & PHY_Linked_Status;
}

void irs::arm::st_ethernet_t::tick()
{
  mp_eth_auto_negotation->tick();
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

mxmac_t irs::arm::st_generate_mac(device_code_t a_device_code)
{
  IRS_STATIC_ASSERT(sizeof(device_code_t) == 1);
  const irs_u16 device_id = irs::crc16(UNIQUE_DEVICE_ID_BEGIN,
    UNIQUE_DEVICE_ID_SIZE);
  return generate_mac(a_device_code, device_id);
}

#else
  #error Тип контроллера не определён
#endif // mcu type
