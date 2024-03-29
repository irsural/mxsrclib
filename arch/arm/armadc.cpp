//! \file
//! \ingroup drivers_group
//! \brief ����� ��� ������ � ��� ��� ARM
//!
//! ���� ��������: 13.01.2011

#include <irsdefs.h>

#include <mxdata.h>

#include <armioregs.h>
#include <armcfg.h>

#include <armadc.h>

#include <irsfinal.h>

#ifdef __LM3SxBxx__

irs::arm::adc_stellaris_t::adc_stellaris_t(
  select_channel_type a_selected_channels,
  adc_ref_t a_adc_ref,
  adc_module_t a_adc_module,
  counter_t a_adc_interval):

  m_status(READ_SEQ_0),
  m_adc_module(a_adc_module),
  m_adc_timer(a_adc_interval),
  m_result_vector(),
  m_num_of_channels(1)  //  Termosensor - ���� ������
{
  switch (m_adc_module)
  {
    case ADC0:
    {
      RCGC0_bit.ADC0 = 1;
      RCGC0_bit.MAXADC0SPD = 0x3;  //  = 1 MSPS
      break;
    }
    case ADC1:
    {
      RCGC0_bit.ADC1 = 1;
      RCGC0_bit.MAXADC1SPD = 0x3;  //  = 1 MSPS
      break;
    }
  }
  if ((a_selected_channels & m_portb_mask) || a_adc_ref == EXT_REF)
  {
    RCGC2_bit.PORTB = 1;
  }
  if (a_selected_channels & m_portd_mask) RCGC2_bit.PORTD = 1;
  if (a_selected_channels & m_porte_mask) RCGC2_bit.PORTE = 1;
  for (irs_u8 i = 10; i; i--);

  //  GPIO
  if (a_selected_channels & (1 << PORT_PE7))
  {
    GPIOEDEN_bit.no7 = 0;
    GPIOEAMSEL_bit.no7 = 1;
  }
  if (a_selected_channels & (1 << PORT_PE6))
  {
    GPIOEDEN_bit.no6 = 0;
    GPIOEAMSEL_bit.no6 = 1;
  }
  if (a_selected_channels & (1 << PORT_PE5))
  {
    GPIOEDEN_bit.no5 = 0;
    GPIOEAMSEL_bit.no5 = 1;
  }
  if (a_selected_channels & (1 << PORT_PE4))
  {
    GPIOEDEN_bit.no4 = 0;
    GPIOEAMSEL_bit.no4 = 1;
  }
  if (a_selected_channels & (1 << PORT_PD7))
  {
    GPIODDEN_bit.no7 = 0;
    GPIODAMSEL_bit.no7 = 1;
  }
  if (a_selected_channels & (1 << PORT_PD6))
  {
    GPIODDEN_bit.no6 = 0;
    GPIODAMSEL_bit.no6 = 1;
  }
  if (a_selected_channels & (1 << PORT_PD5))
  {
    GPIODDEN_bit.no5 = 0;
    GPIODAMSEL_bit.no5 = 1;
  }
  if (a_selected_channels & (1 << PORT_PD4))
  {
    GPIODDEN_bit.no4 = 0;
    GPIODAMSEL_bit.no4 = 1;
  }
  if (a_selected_channels & (1 << PORT_PE3))
  {
    GPIOEDEN_bit.no3 = 0;
    GPIOEAMSEL_bit.no3 = 1;
  }
  if (a_selected_channels & (1 << PORT_PE2))
  {
    GPIOEDEN_bit.no2 = 0;
    GPIOEAMSEL_bit.no2 = 1;
  }
  if (a_selected_channels & (1 << PORT_PB4))
  {
    GPIOBDEN_bit.no4 = 0;
    GPIOBAMSEL_bit.no4 = 1;
  }
  if (a_selected_channels & (1 << PORT_PB5))
  {
    GPIOBDEN_bit.no5 = 0;
    GPIOBAMSEL_bit.no5 = 1;
  }
  if (a_selected_channels & (1 << PORT_PD3))
  {
    GPIODDEN_bit.no3 = 0;
    GPIODAMSEL_bit.no3 = 1;
  }
  if (a_selected_channels & (1 << PORT_PD2))
  {
    GPIODDEN_bit.no2 = 0;
    GPIODAMSEL_bit.no2 = 1;
  }
  if (a_selected_channels & (1 << PORT_PD1))
  {
    GPIODDEN_bit.no1 = 0;
    GPIODAMSEL_bit.no1 = 1;
  }
  if (a_selected_channels & (1 << PORT_PD0))
  {
    GPIODDEN_bit.no0 = 0;
    GPIODAMSEL_bit.no0 = 1;
  }

  if (a_adc_ref == EXT_REF)
  {
    GPIOBDEN_bit.no6 = 0;
    GPIOBAMSEL_bit.no6 = 1;
  }

  switch (m_adc_module)
  {
    case ADC0:
    {
      //  ADC Active Sample Sequencer
      ADC0ACTSS_bit.ASEN0 = 0;
      ADC0ACTSS_bit.ASEN1 = 0;
      ADC0ACTSS_bit.ASEN2 = 0;
      ADC0ACTSS_bit.ASEN3 = 0;
      //  ADC Interrupt Mask
      ADC0IM_bit.MASK0 = 0;
      ADC0IM_bit.MASK1 = 0;
      ADC0IM_bit.MASK2 = 0;
      ADC0IM_bit.MASK3 = 0;
      ADC0IM_bit.DCONSS0 = 0;
      ADC0IM_bit.DCONSS1 = 0;
      ADC0IM_bit.DCONSS2 = 0;
      ADC0IM_bit.DCONSS3 = 0;
      //  ADC Interrupt Status and Clear
      ADC0ISC_bit.IN0 = 1;
      ADC0ISC_bit.IN1 = 1;
      ADC0ISC_bit.IN2 = 1;
      ADC0ISC_bit.IN3 = 1;
      ADC0ISC_bit.DCINSS0 = 1;
      ADC0ISC_bit.DCINSS1 = 1;
      ADC0ISC_bit.DCINSS2 = 1;
      ADC0ISC_bit.DCINSS3 = 1;
      //  ADC Overflow Status
      ADC0OSTAT_bit.OV0 = 1;
      ADC0OSTAT_bit.OV1 = 1;
      ADC0OSTAT_bit.OV2 = 1;
      ADC0OSTAT_bit.OV3 = 1;
      //  ADC Event Multiplexer Select
      ADC0EMUX_bit.EM0 = 0;
      ADC0EMUX_bit.EM1 = 0;
      ADC0EMUX_bit.EM2 = 0;
      ADC0EMUX_bit.EM3 = 0;
      //  ADC Underflow Status
      ADC0USTAT_bit.UV0 = 1;
      ADC0USTAT_bit.UV1 = 1;
      ADC0USTAT_bit.UV2 = 1;
      ADC0USTAT_bit.UV3 = 1;
      //  ADC Sample Sequencer Priority
      ADC0SSPRI_bit.SS0 = 0;
      ADC0SSPRI_bit.SS1 = 0;
      ADC0SSPRI_bit.SS2 = 0;
      ADC0SSPRI_bit.SS3 = 0;
      //  ADC Sample Phase Control
      ADC0SPC = 0;
      //  ADC Sample Averaging Control
      ADC0SAC = 0x6;  //  Averaging 64x
      //  ADC Digital Comparator Interrupt Status and Clear
      ADC0DCISC_bit.DCINT0 = 1;
      ADC0DCISC_bit.DCINT1 = 1;
      ADC0DCISC_bit.DCINT2 = 1;
      ADC0DCISC_bit.DCINT3 = 1;
      ADC0DCISC_bit.DCINT4 = 1;
      ADC0DCISC_bit.DCINT5 = 1;
      ADC0DCISC_bit.DCINT6 = 1;
      ADC0DCISC_bit.DCINT7 = 1;
      //  ADC Control
      ADC0CTL_bit.VREF = a_adc_ref;
      //  ADC Sample Sequence 0 Operation
      ADC0SSOP0_bit.S0DCOP = 0;
      ADC0SSOP0_bit.S1DCOP = 0;
      ADC0SSOP0_bit.S2DCOP = 0;
      ADC0SSOP0_bit.S3DCOP = 0;
      ADC0SSOP0_bit.S4DCOP = 0;
      ADC0SSOP0_bit.S5DCOP = 0;
      ADC0SSOP0_bit.S6DCOP = 0;
      ADC0SSOP0_bit.S7DCOP = 0;
      //  ADC Sample Sequence 1 Operation
      ADC0SSOP1_bit.S0DCOP = 0;
      ADC0SSOP1_bit.S1DCOP = 0;
      ADC0SSOP1_bit.S2DCOP = 0;
      ADC0SSOP1_bit.S3DCOP = 0;
      //  ADC Sample Sequence 2 Operation
      ADC0SSOP2_bit.S0DCOP = 0;
      ADC0SSOP2_bit.S1DCOP = 0;
      ADC0SSOP2_bit.S2DCOP = 0;
      ADC0SSOP2_bit.S3DCOP = 0;
      //  ADC Sample Sequence 3 Operation
      ADC0SSOP3_bit.S0DCOP = 0;
      //  ADC Sample Sequence Input Multiplexer Select 0
      ADC0SSMUX0 = 0;
      ADC0SSMUX1 = 0;
      ADC0SSMUX2 = 0;
      ADC0SSMUX3 = 0;
      //  ADC Sample Sequence Control 0
      ADC0SSCTL0 = 0;
      ADC0SSCTL1 = 0;
      ADC0SSCTL2 = 0;
      ADC0SSCTL3 = 0;
      //  Termosensor
      ADC0SSCTL0_bit.TS0 = 1;  //  Sample Temp Sensor Select
      break;
    }
    case ADC1:
    {
      //  ADC Active Sample Sequencer
      ADC1ACTSS_bit.ASEN0 = 0;
      ADC1ACTSS_bit.ASEN1 = 0;
      ADC1ACTSS_bit.ASEN2 = 0;
      ADC1ACTSS_bit.ASEN3 = 0;
      //  ADC Interrupt Mask
      ADC1IM_bit.MASK0 = 0;
      ADC1IM_bit.MASK1 = 0;
      ADC1IM_bit.MASK2 = 0;
      ADC1IM_bit.MASK3 = 0;
      ADC1IM_bit.DCONSS0 = 0;
      ADC1IM_bit.DCONSS1 = 0;
      ADC1IM_bit.DCONSS2 = 0;
      ADC1IM_bit.DCONSS3 = 0;
      //  ADC Interrupt Status and Clear
      ADC1ISC_bit.IN0 = 1;
      ADC1ISC_bit.IN1 = 1;
      ADC1ISC_bit.IN2 = 1;
      ADC1ISC_bit.IN3 = 1;
      ADC1ISC_bit.DCINSS0 = 1;
      ADC1ISC_bit.DCINSS1 = 1;
      ADC1ISC_bit.DCINSS2 = 1;
      ADC1ISC_bit.DCINSS3 = 1;
      //  ADC Overflow Status
      ADC1OSTAT_bit.OV0 = 1;
      ADC1OSTAT_bit.OV1 = 1;
      ADC1OSTAT_bit.OV2 = 1;
      ADC1OSTAT_bit.OV3 = 1;
      //  ADC Event Multiplexer Select
      ADC1EMUX_bit.EM0 = 0;
      ADC1EMUX_bit.EM1 = 0;
      ADC1EMUX_bit.EM2 = 0;
      ADC1EMUX_bit.EM3 = 0;
      //  ADC Underflow Status
      ADC1USTAT_bit.UV0 = 1;
      ADC1USTAT_bit.UV1 = 1;
      ADC1USTAT_bit.UV2 = 1;
      ADC1USTAT_bit.UV3 = 1;
      //  ADC Sample Sequencer Priority
      ADC1SSPRI_bit.SS0 = 0;
      ADC1SSPRI_bit.SS1 = 0;
      ADC1SSPRI_bit.SS2 = 0;
      ADC1SSPRI_bit.SS3 = 0;
      //  ADC Sample Phase Control
      ADC1SPC = 0;
      //  ADC Sample Averaging Control
      ADC1SAC = 0x6;  //  Averaging 64x
      //  ADC Digital Comparator Interrupt Status and Clear
      ADC1DCISC_bit.DCINT0 = 1;
      ADC1DCISC_bit.DCINT1 = 1;
      ADC1DCISC_bit.DCINT2 = 1;
      ADC1DCISC_bit.DCINT3 = 1;
      ADC1DCISC_bit.DCINT4 = 1;
      ADC1DCISC_bit.DCINT5 = 1;
      ADC1DCISC_bit.DCINT6 = 1;
      ADC1DCISC_bit.DCINT7 = 1;
      //  ADC Control
      ADC1CTL_bit.VREF = a_adc_ref;
      //  ADC Sample Sequence 0 Operation
      ADC1SSOP0_bit.S0DCOP = 0;
      ADC1SSOP0_bit.S1DCOP = 0;
      ADC1SSOP0_bit.S2DCOP = 0;
      ADC1SSOP0_bit.S3DCOP = 0;
      ADC1SSOP0_bit.S4DCOP = 0;
      ADC1SSOP0_bit.S5DCOP = 0;
      ADC1SSOP0_bit.S6DCOP = 0;
      ADC1SSOP0_bit.S7DCOP = 0;
      //  ADC Sample Sequence 1 Operation
      ADC1SSOP1_bit.S0DCOP = 0;
      ADC1SSOP1_bit.S1DCOP = 0;
      ADC1SSOP1_bit.S2DCOP = 0;
      ADC1SSOP1_bit.S3DCOP = 0;
      //  ADC Sample Sequence 2 Operation
      ADC1SSOP2_bit.S0DCOP = 0;
      ADC1SSOP2_bit.S1DCOP = 0;
      ADC1SSOP2_bit.S2DCOP = 0;
      ADC1SSOP2_bit.S3DCOP = 0;
      //  ADC Sample Sequence 3 Operation
      ADC1SSOP3_bit.S0DCOP = 0;
      //  ADC Sample Sequence Input Multiplexer Select 0
      ADC1SSMUX0 = 0;
      ADC1SSMUX1 = 0;
      ADC1SSMUX2 = 0;
      ADC1SSMUX3 = 0;
      //  ADC Sample Sequence Control 0
      ADC1SSCTL0 = 0;
      ADC1SSCTL1 = 0;
      ADC1SSCTL2 = 0;
      ADC1SSCTL3 = 0;
      //  Termosensor
      ADC1SSCTL0_bit.TS0 = 1;  //  Sample Temp Sensor Select
      break;
    }
  }

  //  Channel 0 = termosensor
  for (irs_u8 i = 0; i < m_max_adc_channels_cnt; i++)
  {
    if (a_selected_channels & (1 << i))
    {
      irs_u32 mux_reg = 0;
      irs_u32 ctl_reg = 0;
      if (m_num_of_channels < m_sequence0_size)
      {
        //  m_num_of_channels = 1...7
        mux_reg |= (i << m_num_of_channels * 4);
        if ((m_num_of_channels >= m_sequence0_size - 1) ||
          (i >= m_max_adc_channels_cnt - 1))
        {
          //  0x03 = End of Sequence | Sample Interrupt Enable
          ctl_reg |= (0x03 << (m_num_of_channels * 4 + 1));
        }
        set_mux_and_ctl_regs(mux_reg, ctl_reg, SEQ0);
      }
      else if (m_num_of_channels < (m_sequence0_size + m_sequence1_size))
      {
        //  m_num_of_channels = 8...11
        mux_reg |= (i << (m_num_of_channels - m_sequence0_size) * 4);
        if ((m_num_of_channels >= m_sequence0_size + m_sequence1_size - 1) ||
          (i >= m_max_adc_channels_cnt - 1))
        {
          //  0x03 = End of Sequence | Sample Interrupt Enable
          irs_u8 shift = (m_num_of_channels - m_sequence0_size) * 4 + 1;
          ctl_reg |= (0x03 << shift);
        }
        set_mux_and_ctl_regs(mux_reg, ctl_reg, SEQ1);
      }
      else if (m_num_of_channels <
        (m_sequence0_size + m_sequence1_size + m_sequence2_size))
      {
        //  m_num_of_channels = 12...15
        mux_reg |=
          (i << (m_num_of_channels - m_sequence0_size - m_sequence1_size) * 4);
        if ((m_num_of_channels >=
          m_sequence0_size + m_sequence1_size + m_sequence2_size - 1) ||
          (i >= m_max_adc_channels_cnt - 1))
        {
          //  0x03 = End of Sequence | Sample Interrupt Enable
          irs_u8 shift = (m_num_of_channels - m_sequence0_size
            - m_sequence1_size) * 4 + 1;
          ctl_reg |= (0x03 << shift);
        }
        set_mux_and_ctl_regs(mux_reg, ctl_reg, SEQ2);
      }
      else
      {
        //  m_num_of_channels = 16
        mux_reg = i;
        //  0x06 = End of Sequence | Sample Interrupt Enable
        ctl_reg = 0x06;
        set_mux_and_ctl_regs(mux_reg, ctl_reg, SEQ3);
      }
      m_num_of_channels++;
    }
  }
  if (m_num_of_channels <= 1)
  {
    switch (m_adc_module)
    {
      case ADC0: ADC0SSCTL0_bit.END0 = 1; ADC0SSCTL0_bit.IE0 = 1; break;
      case ADC1: ADC1SSCTL0_bit.END0 = 1; ADC1SSCTL0_bit.IE0 = 1; break;
    }
  }
  m_result_vector.clear();
  m_result_vector.resize(m_num_of_channels, 0);
  switch (m_adc_module)
  {
    case ADC0:
    {
      ADC0ACTSS_bit.ASEN0 = 1;
      if (m_num_of_channels > m_sequence0_size)
      {
        ADC0ACTSS_bit.ASEN1 = 1;
      }
      if (m_num_of_channels > m_sequence0_size + m_sequence1_size)
      {
        ADC0ACTSS_bit.ASEN2 = 1;
      }
      if (m_num_of_channels > m_sequence0_size + m_sequence1_size
        + m_sequence2_size)
      {
        ADC0ACTSS_bit.ASEN3 = 1;
      }
      break;
    }
    case ADC1:
    {
      ADC1ACTSS_bit.ASEN0 = 1;
      if (m_num_of_channels > m_sequence0_size)
      {
        ADC0ACTSS_bit.ASEN1 = 1;
      }
      if (m_num_of_channels > m_sequence0_size + m_sequence1_size)
      {
        ADC0ACTSS_bit.ASEN2 = 1;
      }
      if (m_num_of_channels > m_sequence0_size + m_sequence1_size
        + m_sequence2_size)
      {
        ADC0ACTSS_bit.ASEN3 = 1;
      }
      break;
    }
  }
  clear_fifo(SEQ0);
  clear_fifo(SEQ1);
  clear_fifo(SEQ2);
  clear_fifo(SEQ3);
  //  Seq0 start
  switch (m_adc_module)
  {
    case ADC0: ADC0PSSI_bit.SS0 = 1; break;
    case ADC1: ADC1PSSI_bit.SS0 = 1; break;
  }
}

void irs::arm::adc_stellaris_t::set_mux_and_ctl_regs(
  irs_u32 a_mux, irs_u32 a_ctl, sequence_t a_seq)
{
  switch (m_adc_module)
  {
    case ADC0:
    {
      switch (a_seq)
      {
        case SEQ0:
        {
          ADC0SSMUX0 |= a_mux;
          ADC0SSCTL0 |= a_ctl;
          break;
        }
        case SEQ1:
        {
          ADC0SSMUX1 |= a_mux;
          ADC0SSCTL1 |= a_ctl;
          break;
        }
        case SEQ2:
        {
          ADC0SSMUX2 |= a_mux;
          ADC0SSCTL2 |= a_ctl;
          break;
        }
        case SEQ3:
        {
          ADC0SSMUX3 |= a_mux;
          ADC0SSCTL3 |= a_ctl;
          break;
        }
      }
      break;
    }
    case ADC1:
    {
      switch (a_seq)
      {
        case SEQ0:
        {
          ADC1SSMUX0 |= a_mux;
          ADC1SSCTL0 |= a_ctl;
          break;
        }
        case SEQ1:
        {
          ADC1SSMUX1 |= a_mux;
          ADC1SSCTL1 |= a_ctl;
          break;
        }
        case SEQ2:
        {
          ADC1SSMUX2 |= a_mux;
          ADC1SSCTL2 |= a_ctl;
          break;
        }
        case SEQ3:
        {
          ADC1SSMUX3 |= a_mux;
          ADC1SSCTL3 |= a_ctl;
          break;
        }
      }
      break;
    }
  }
}

bool irs::arm::adc_stellaris_t::check_sequence_ready(sequence_t a_seq)
{
  switch (m_adc_module)
  {
    case ADC0:
    {
      if (ADC0RIS & (1 << a_seq))
      {
        ADC0ISC |= (1 << a_seq);
        return true;
      }
      return false;
    }
    case ADC1:
    {
      if (ADC1RIS & (1 << a_seq))
      {
        ADC1ISC |= (1 << a_seq);
        return true;
      }
      return false;
    }
  }
  return false;
}

void irs::arm::adc_stellaris_t::clear_fifo(sequence_t a_seq)
{
  switch (m_adc_module)
  {
    case ADC0:
    {
      switch (a_seq)
      {
        case SEQ0:
        {
          if (!ADC0SSFSTAT0_bit.EMPTY)
          {
            volatile irs_u32 fifo_data = ADC0SSFIFO0;
            while(!ADC0SSFSTAT0_bit.EMPTY) fifo_data = ADC0SSFIFO0;
          }
          break;
        }
        case SEQ1:
        {
          if (!ADC0SSFSTAT1_bit.EMPTY)
          {
            volatile irs_u32 fifo_data = ADC0SSFIFO1;
            while(!ADC0SSFSTAT1_bit.EMPTY) fifo_data = ADC0SSFIFO1;
          }
          break;
        }
        case SEQ2:
        {
          if (!ADC0SSFSTAT2_bit.EMPTY)
          {
            volatile irs_u32 fifo_data = ADC0SSFIFO2;
            while(!ADC0SSFSTAT2_bit.EMPTY) fifo_data = ADC0SSFIFO2;
          }
          break;
        }
        case SEQ3:
        {
          if (!ADC0SSFSTAT3_bit.EMPTY)
          {
            volatile irs_u32 fifo_data = ADC0SSFIFO3;
            while(!ADC0SSFSTAT3_bit.EMPTY) fifo_data = ADC0SSFIFO3;
          }
          break;
        }
      }
      break;
    }
    case ADC1:
    {
      switch (a_seq)
      {
        case SEQ0:
        {
          if (!ADC1SSFSTAT0_bit.EMPTY)
          {
            volatile irs_u32 fifo_data = ADC1SSFIFO0;
            while(!ADC1SSFSTAT0_bit.EMPTY) fifo_data = ADC1SSFIFO0;
          }
          break;
        }
        case SEQ1:
        {
          if (!ADC1SSFSTAT1_bit.EMPTY)
          {
            volatile irs_u32 fifo_data = ADC1SSFIFO1;
            while(!ADC1SSFSTAT1_bit.EMPTY) fifo_data = ADC1SSFIFO1;
          }
          break;
        }
        case SEQ2:
        {
          if (!ADC1SSFSTAT2_bit.EMPTY)
          {
            volatile irs_u32 fifo_data = ADC1SSFIFO2;
            while(!ADC1SSFSTAT2_bit.EMPTY) fifo_data = ADC1SSFIFO2;
          }
          break;
        }
        case SEQ3:
        {
          if (!ADC1SSFSTAT3_bit.EMPTY)
          {
            volatile irs_u32 fifo_data = ADC1SSFIFO3;
            while(!ADC1SSFSTAT3_bit.EMPTY) fifo_data = ADC1SSFIFO3;
          }
          break;
        }
      }
      break;
    }
  }
}

irs::arm::adc_stellaris_t::~adc_stellaris_t()
{
  switch (m_adc_module)
  {
    case ADC0:
    {
      ADC0ACTSS_bit.ASEN0 = 0;
      ADC0ACTSS_bit.ASEN1 = 0;
      ADC0ACTSS_bit.ASEN2 = 0;
      ADC0ACTSS_bit.ASEN3 = 0;
      ADC0ISC_bit.IN0 = 1;
      ADC0ISC_bit.IN1 = 1;
      ADC0ISC_bit.IN2 = 1;
      ADC0ISC_bit.IN3 = 1;
      if (ADC0CTL_bit.VREF == EXT_REF)
      {
        GPIOBAMSEL_bit.no6 = 0;
      }
      RCGC0_bit.ADC0 = 0;
      break;
    }
    case ADC1:
    {
      ADC1ACTSS_bit.ASEN0 = 0;
      ADC1ACTSS_bit.ASEN1 = 0;
      ADC1ACTSS_bit.ASEN2 = 0;
      ADC1ACTSS_bit.ASEN3 = 0;
      ADC1ISC_bit.IN0 = 1;
      ADC1ISC_bit.IN1 = 1;
      ADC1ISC_bit.IN2 = 1;
      ADC1ISC_bit.IN3 = 1;
      if (ADC1CTL_bit.VREF == EXT_REF)
      {
        GPIOBAMSEL_bit.no6 = 0;
      }
      RCGC0_bit.ADC1 = 0;
      break;
    }
  }
}

irs_u16 irs::arm::adc_stellaris_t::get_u16_minimum()
{
  return 0;
}

irs_u16 irs::arm::adc_stellaris_t::get_u16_maximum()
{
  return 0xFFC0;
}

irs_u16 irs::arm::adc_stellaris_t::get_u16_data(irs_u8 a_channel)
{
  if (a_channel < m_num_of_channels - 1)
  {
    return (m_result_vector[a_channel + 1] << 6);
  }
  return 0;
}

irs_u32 irs::arm::adc_stellaris_t::get_u32_minimum()
{
  return 0;
}

irs_u32 irs::arm::adc_stellaris_t::get_u32_maximum()
{
  return 0xFFC00000;
}

irs_u32 irs::arm::adc_stellaris_t::get_u32_data(irs_u8 a_channel)
{
  if (a_channel < m_num_of_channels - 1)
  {
    return (m_result_vector[a_channel + 1] << 22);
  }
  return 0;
}

float irs::arm::adc_stellaris_t::get_float_minimum()
{
  return 0;
}

float irs::arm::adc_stellaris_t::get_float_maximum()
{
  return 1.f;
}

float irs::arm::adc_stellaris_t::get_float_data(irs_u8 a_channel)
{
  if (a_channel < m_num_of_channels - 1)
  {
    return (static_cast<float>(m_result_vector[a_channel + 1]) / 1023.f);
  }
  return 0;
}

float irs::arm::adc_stellaris_t::get_temperature()
{
  return (147.5f - ((225.f * static_cast<float>(m_result_vector[0])) / 1023.f));
}

void irs::arm::adc_stellaris_t::tick()
{
  switch (m_status)
  {
    case WAIT:
    {
      if (m_adc_timer.check())
      {
        //  Seq0 start
        switch (m_adc_module)
        {
          case ADC0: ADC0PSSI_bit.SS0 = 1; break;
          case ADC1: ADC1PSSI_bit.SS0 = 1; break;
        }
        m_status = READ_SEQ_0;
      }
      break;
    }
    case READ_SEQ_0:
    {
      if (check_sequence_ready(SEQ0))
      {
        for (irs_u8 i = 0; i < m_sequence0_size; i++)
        {
          if (i >= m_num_of_channels) break;
          switch (m_adc_module)
          {
            case ADC0: m_result_vector[i] = ADC0SSFIFO0; break;
            case ADC1: m_result_vector[i] = ADC1SSFIFO0; break;
          }
        }
        clear_fifo(SEQ0);
        if (m_num_of_channels > m_sequence0_size)
        {
          //  Seq1 start
          switch (m_adc_module)
          {
            case ADC0: ADC0PSSI_bit.SS1 = 1; break;
            case ADC1: ADC1PSSI_bit.SS1 = 1; break;
          }
          m_status = READ_SEQ_1;
        }
        else m_status = WAIT;
      }
      else
      {
        volatile irs_u32 x = ADC0CTL;
      }
      break;
    }
    case READ_SEQ_1:
    {
      if (check_sequence_ready(SEQ1))
      {
        for (irs_u8 i = 0; i < m_sequence1_size; i++)
        {
          if (i >= (m_num_of_channels - m_sequence0_size)) break;
          switch (m_adc_module)
          {
            case ADC0:
            {
              m_result_vector[i + m_sequence0_size] = ADC0SSFIFO1;
              break;
            }
            case ADC1:
            {
              m_result_vector[i + m_sequence0_size] = ADC1SSFIFO1;
              break;
            }
          }
        }
        clear_fifo(SEQ1);
        if (m_num_of_channels > m_sequence0_size + m_sequence1_size)
        {
          //  Seq2 start
          switch (m_adc_module)
          {
            case ADC0: ADC0PSSI_bit.SS2 = 1; break;
            case ADC1: ADC1PSSI_bit.SS2 = 1; break;
          }
          m_status = READ_SEQ_2;
        }
        else m_status = WAIT;
      }
      break;
    }
    case READ_SEQ_2:
    {
      if (check_sequence_ready(SEQ2))
      {
        for (irs_u8 i = 0; i < m_sequence2_size; i++)
        {
          if (i >= (m_num_of_channels - m_sequence0_size - m_sequence1_size))
          {
            break;
          }
          switch (m_adc_module)
          {
            case ADC0:
            {
              m_result_vector[i + m_sequence0_size + m_sequence1_size]
                = ADC0SSFIFO1;
              break;
            }
            case ADC1:
            {
              m_result_vector[i + m_sequence0_size + m_sequence1_size]
                = ADC1SSFIFO1;
              break;
            }
          }
        }
        clear_fifo(SEQ2);
        if (m_num_of_channels > m_sequence0_size + m_sequence1_size
           + m_sequence2_size)
        {
          //  Seq3 start
          switch (m_adc_module)
          {
            case ADC0: ADC0PSSI_bit.SS3 = 1; break;
            case ADC1: ADC1PSSI_bit.SS3 = 1; break;
          }
          m_status = READ_SEQ_3;
        }
        else m_status = WAIT;
      }
      break;
    }
    case READ_SEQ_3:
    {
      if (check_sequence_ready(SEQ3))
      {
        switch (m_adc_module)
        {
          case ADC0:
          {
            m_result_vector[m_max_adc_channels_cnt] = ADC0SSFIFO3;
            break;
          }
          case ADC1:
          {
            m_result_vector[m_max_adc_channels_cnt] = ADC1SSFIFO3;
            break;
          }
        }
        clear_fifo(SEQ3);
        m_status = WAIT;
      }
      break;
    }
  }
}

#elif defined(IRS_STM32_F2_F4_F7)

// class st_adc_t
irs::arm::st_adc_t::st_adc_t(size_t a_adc_address,
  select_channel_type a_selected_channels,
  counter_t a_adc_interval,
  counter_t a_adc_battery_interval,
  bool a_single_conversion,
  irs_u8 a_sampling_time,
  irs_u8 a_clock_div
):
  mp_adc(reinterpret_cast<adc_regs_t*>(a_adc_address)),
  m_adc_timer(a_adc_interval),
  m_adc_battery_timer(a_adc_battery_interval),
  m_regular_channels_values(),
  m_active_channels(),
  m_current_channel(),
  m_temperature_sensor_enabled(false),
  m_temperature_channel_value(0),
  m_v_battery_measurement_enabled(false),
  m_v_battery_channel_value(0),
  m_injected_channel_selected(ics_temperature_sensor),
  m_single_conversion(a_single_conversion)
{
  clock_enable(a_adc_address);
  //  000: 3 cycles
  //  001: 15 cycles
  //  010: 28 cycles
  //  011: 56 cycles
  //  100: 84 cycles
  //  101: 112 cycles
  //  110: 144 cycles
  //  111: 480 cycles
  enum {
    num_of_smpr1_channels = 9,
    num_of_smpr2_channels = 10,
    bit_len = 3,
    mask = 0x07
  };
  irs_u32 smpr = 0;
  for (irs_u8 i = 0; i < num_of_smpr1_channels; i++) {
    smpr |= (a_sampling_time & mask) << (i * bit_len);
  }
  mp_adc->ADC_SMPR1 = smpr;
  smpr = 0;
  for (irs_u8 i = 0; i < num_of_smpr2_channels; i++) {
    smpr |= (a_sampling_time & mask) << (i * bit_len);
  }
  mp_adc->ADC_SMPR2 = smpr;
  //  00: PCLK2 divided by 2
  //  01: PCLK2 divided by 4
  //  10: PCLK2 divided by 6
  //  11: PCLK2 divided by 8
  ADC_CCR_bit.ADCPRE = a_clock_div;

  vector<pair<adc_channel_t, gpio_channel_t> > adc_gpio_pairs;
  adc_gpio_pairs.push_back(make_pair(ADC123_PA0_CH0, PA0));
  adc_gpio_pairs.push_back(make_pair(ADC123_PA1_CH1, PA1));
  adc_gpio_pairs.push_back(make_pair(ADC123_PA2_CH2, PA2));
  adc_gpio_pairs.push_back(make_pair(ADC123_PA3_CH3, PA3));
  adc_gpio_pairs.push_back(make_pair(ADC12_PA4_CH4, PA4));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF6_CH4, PF6));
  adc_gpio_pairs.push_back(make_pair(ADC12_PA5_CH5, PA5));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF7_CH5, PF7));
  adc_gpio_pairs.push_back(make_pair(ADC12_PA6_CH6, PA6));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF8_CH6, PF8));
  adc_gpio_pairs.push_back(make_pair(ADC12_PA7_CH7, PA7));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF9_CH7, PF9));
  adc_gpio_pairs.push_back(make_pair(ADC12_PB0_CH8, PB0));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF10_CH8, PF10));
  adc_gpio_pairs.push_back(make_pair(ADC12_PB1_CH9, PB1));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF3_CH9, PF3));
  adc_gpio_pairs.push_back(make_pair(ADC123_PC0_CH10, PC0));
  adc_gpio_pairs.push_back(make_pair(ADC123_PC1_CH11, PC1));
  adc_gpio_pairs.push_back(make_pair(ADC123_PC2_CH12, PC2));
  adc_gpio_pairs.push_back(make_pair(ADC123_PC3_CH13, PC3));
  adc_gpio_pairs.push_back(make_pair(ADC12_PC4_CH14, PC4));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF4_CH14, PF4));
  adc_gpio_pairs.push_back(make_pair(ADC12_PC5_CH15, PC5));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF5_CH15, PF5));

  select_channel_type selected_adc_mask = ADC1_MASK;
  switch (a_adc_address) {
    case IRS_ADC1_BASE: {
      selected_adc_mask = ADC1_MASK;
    } break;
    case IRS_ADC2_BASE: {
      selected_adc_mask = ADC2_MASK;
    } break;
    case IRS_ADC3_BASE: {
      selected_adc_mask = ADC3_MASK;
    } break;
  }

  for (size_t i = 0; i < adc_gpio_pairs.size(); i++) {
    adc_channel_t adc_channel = adc_gpio_pairs[i].first;
    const select_channel_type adc_mask = selected_adc_mask & adc_channel;
    const select_channel_type channel_mask = static_cast<irs_u16>(adc_channel);
    if ((a_selected_channels & channel_mask) &&
      (a_selected_channels & adc_mask)) {
      clock_enable(adc_gpio_pairs[i].second);
      gpio_moder_analog_enable(adc_gpio_pairs[i].second);
      m_active_channels.push_back(adc_channel_to_channel_index(adc_channel));
      m_regular_channels_values.push_back(0);
    }
  }

  const irs_u32 channel_mask = a_selected_channels & 0xFFFFFF;
  if ((a_adc_address == IRS_ADC1_BASE) &&
    (a_selected_channels & ADC1_MASK) &&
    (channel_mask & ADC1_TEMPERATURE)) {
    m_temperature_sensor_enabled = true;
  }

  if ((a_adc_address == IRS_ADC1_BASE) &&
    (a_selected_channels & ADC1_MASK) &&
    (channel_mask & ADC1_V_BATTERY)) {
    m_v_battery_measurement_enabled = true;
  }

  if (m_temperature_sensor_enabled || m_v_battery_measurement_enabled ||
    !m_regular_channels_values.empty()) {
    // �������� ���
    mp_adc->ADC_CR2_bit.ADON = 1;
  }
  // 0: ���� �����
  mp_adc->ADC_SQR1_bit.L = 0;
  if (!m_active_channels.empty()) {
    m_current_channel = 0;
    mp_adc->ADC_SQR3_bit.SQ1 = m_active_channels[m_current_channel];
    if (!m_single_conversion) {
      mp_adc->ADC_CR2_bit.SWSTART = 1;
    }
  }
  if (m_temperature_sensor_enabled) {
    ADC_CCR_bit.TSVREFE = 1;
  }
  if (m_v_battery_measurement_enabled) {
    //ADC_CCR_bit.VBATE = 1;
  }
  if (m_temperature_sensor_enabled) {
    // ������������� �� ���������� ��������� ����������� ������������
    mp_adc->ADC_JSQR_bit.JSQ4 = 16;
    m_injected_channel_selected = ics_temperature_sensor;
  } else if (m_v_battery_measurement_enabled) {
    // ������������� �� ���������� ��������� ���������� ���������
    mp_adc->ADC_JSQR_bit.JSQ4 = 18;
    m_injected_channel_selected = ics_v_battery;
    ADC_CCR_bit.VBATE = 1;
  }
  if (m_temperature_sensor_enabled || m_v_battery_measurement_enabled) {
    // 0: ���� ���������� �����
    mp_adc->ADC_JSQR_bit.JL = 0;
    mp_adc->ADC_CR2_bit.JSWSTART = 1;
  }
}

irs_u32 irs::arm::st_adc_t::adc_channel_to_channel_index(
  adc_channel_t a_adc_channel)
{
  irs_u16 adc_channel = static_cast<irs_u16>(a_adc_channel);
  for (size_t i = 0; i < reqular_channel_count; i++) {
    if (adc_channel & 0x1) {
      return i;
    }
    adc_channel >>= 1;
  }
  IRS_LIB_ERROR(ec_standard, "����� �� ������");
  return 0;
}

irs::arm::st_adc_t::~st_adc_t()
{
  mp_adc->ADC_CR2_bit.ADON = 0;
}

irs::arm::st_adc_t::size_type irs::arm::st_adc_t::get_resolution() const
{
  return adc_resolution;
}

irs_u16 irs::arm::st_adc_t::get_u16_minimum()
{
  return 0;
}

irs_u16 irs::arm::st_adc_t::get_u16_maximum()
{
  return static_cast<irs_u16>(adc_max_value) << (16 - adc_resolution);
}

irs_u16 irs::arm::st_adc_t::get_u16_data(irs_u8 a_channel)
{
  if (a_channel >= m_regular_channels_values.size()) {
    IRS_LIB_ERROR(ec_standard, "��� ������ � ����� �������");
  } else if (m_single_conversion) {
    if (mp_adc->ADC_SR_bit.EOC == 1) {
      m_regular_channels_values[a_channel] =
        static_cast<irs_u16>(mp_adc->ADC_DR);
    }
    mp_adc->ADC_SQR3_bit.SQ1 = m_active_channels[a_channel];
    mp_adc->ADC_CR2_bit.SWSTART = 1;
  }
  return m_regular_channels_values[a_channel] << (16 - adc_resolution);
}

irs_u32 irs::arm::st_adc_t::get_u32_minimum()
{
  return 0;
}

irs_u32 irs::arm::st_adc_t::get_u32_maximum()
{
  return static_cast<irs_u32>(adc_max_value) << (32 - adc_resolution);
}

irs_u32 irs::arm::st_adc_t::get_u32_data(irs_u8 a_channel)
{
  if (a_channel >= m_regular_channels_values.size()) {
    IRS_LIB_ERROR(ec_standard, "��� ������ � ����� �������");
  } else if (m_single_conversion) {
    if (mp_adc->ADC_SR_bit.EOC == 1) {
      m_regular_channels_values[a_channel] =
        static_cast<irs_u16>(mp_adc->ADC_DR);
    }
    mp_adc->ADC_SQR3_bit.SQ1 = m_active_channels[a_channel];
    mp_adc->ADC_CR2_bit.SWSTART = 1;
  }
  return m_regular_channels_values[a_channel] << (32 - adc_resolution);
}

float irs::arm::st_adc_t::get_float_minimum()
{
  return 0.f;
}

float irs::arm::st_adc_t::get_float_maximum()
{
  return 1.f;
}

float irs::arm::st_adc_t::get_float_data(irs_u8 a_channel)
{
  if (a_channel >= m_regular_channels_values.size()) {
    IRS_LIB_ERROR(ec_standard, "��� ������ � ����� �������");
  } else if (m_single_conversion) {
    if (mp_adc->ADC_SR_bit.EOC == 1) {
      m_regular_channels_values[a_channel] =
        static_cast<irs_u16>(mp_adc->ADC_DR);
    }
    mp_adc->ADC_SQR3_bit.SQ1 = m_active_channels[a_channel];
    mp_adc->ADC_CR2_bit.SWSTART = 1;
  }
  return static_cast<float>(m_regular_channels_values[a_channel])/adc_max_value;
}

float irs::arm::st_adc_t::get_temperature()
{
  if (m_temperature_sensor_enabled) {
    return static_cast<float>(m_temperature_channel_value)/adc_max_value;
  }
  return 0;
}

float irs::arm::st_adc_t::get_v_battery()
{
  if (m_v_battery_measurement_enabled) {
    float k = 2;
    #ifdef IRS_STM32F7xx
    k = 4;
    #endif // IRS_STM32F7xx
    return static_cast<float>(m_v_battery_channel_value)*k/adc_max_value;
  }
  return 0;
}

float irs::arm::st_adc_t::get_temperature_degree_celsius(
  const float a_vref)
{
  if (m_temperature_sensor_enabled) {
    #if (defined (STM32F745xx) || defined (STM32F746xx))
    // ������ ������������� ������������ ����������������
    static const float TS_CAL1 =
      *reinterpret_cast<irs_u16*>((irs_u32)0x1FF0F44C); // 30 ��������
    static const float TS_CAL2 =
      *reinterpret_cast<irs_u16*>((irs_u32)0x1FF0F44E); // 110 ��������

    static const float ts_cal1_k = 3.3f*TS_CAL1/adc_max_value;
    static const float ts_cal2_k = 3.3f*TS_CAL2/adc_max_value;

    const float ts_data_k = a_vref*m_temperature_channel_value/adc_max_value;
    //float t = ((110.f-30.f)/(TS_CAL2-TS_CAL1)*(TS_DATA-TS_CAL1)+30.f);
    float t = ((110.f-30.f)/(ts_cal2_k-ts_cal1_k)*(ts_data_k-ts_cal1_k)+30.f);
    return t;
    #else // !((defined (STM32F745xx) || defined (STM32F746xx)))
    const float v25 = 0.76;
    const float avg_slope = 0.0025;
    const float koef = a_vref/adc_max_value;
    const float v_sence = m_temperature_channel_value*koef;
    return (v_sence - v25)/avg_slope + 25;
    #endif // !((defined (STM32F745xx) || defined (STM32F746xx)))
  }
  return 0;
}

void irs::arm::st_adc_t::tick()
{
  const bool timer_check = m_adc_timer.check();
  if (timer_check && !m_single_conversion) {
    if (mp_adc->ADC_SR_bit.EOC == 1) {
      m_regular_channels_values[m_current_channel] =
        static_cast<irs_u16>(mp_adc->ADC_DR);
      m_current_channel++;
      if (m_current_channel >= m_active_channels.size()) {
        m_current_channel = 0;
      }
      mp_adc->ADC_SQR3_bit.SQ1 = m_active_channels[m_current_channel];
      mp_adc->ADC_CR2_bit.SWSTART = 1;
    }
  }
  if (m_temperature_sensor_enabled || m_v_battery_measurement_enabled) {
    const bool adc_battery_timer_check = m_adc_battery_timer.check();
    if (mp_adc->ADC_SR_bit.JEOC == 0) {
      if (timer_check || adc_battery_timer_check) {
        if (m_injected_channel_selected == ics_temperature_sensor) {
          if (m_v_battery_measurement_enabled && adc_battery_timer_check) {
            mp_adc->ADC_JSQR_bit.JSQ4 = 18;
            ADC_CCR_bit.VBATE = 1;
            m_injected_channel_selected = ics_v_battery;
          }
        } else {
          if (m_temperature_sensor_enabled && timer_check) {
            mp_adc->ADC_JSQR_bit.JSQ4 = 16;
            ADC_CCR_bit.VBATE = 0;
            m_injected_channel_selected = ics_temperature_sensor;
          } else {
            ADC_CCR_bit.VBATE = 1;
          }
        }
        mp_adc->ADC_SR_bit.JEOC = 0;
        mp_adc->ADC_CR2_bit.JSWSTART = 1;
      }
    }
    if (mp_adc->ADC_SR_bit.JEOC == 1) {
      IRS_LIB_ASSERT((m_injected_channel_selected == ics_v_battery) ==
        (ADC_CCR_bit.VBATE == 1));
      read_injected_channel_value();
      // ��������� �������� ��� �������� ������ ���������
      ADC_CCR_bit.VBATE = 0;
      mp_adc->ADC_SR_bit.JEOC = 0;
    }
  }
}

void irs::arm::st_adc_t::read_injected_channel_value()
{
  if (m_injected_channel_selected == ics_temperature_sensor) {
    m_temperature_channel_value =
      static_cast<irs_i16>(mp_adc->ADC_JDR1_bit.JDATA);
  } else {
    m_v_battery_channel_value =
      static_cast<irs_u16>(mp_adc->ADC_JDR1_bit.JDATA);
  }
}

#ifdef USE_HAL_DRIVER

//template<float* const data, int i>
//float* const irs::arm::LOOP<float* const data, int i>::mp_data = 0;

//map<ADC_HandleTypeDef*, irs::arm::st_hal_adc_dma_t*>
  //irs::arm::st_hal_adc_dma_t::m_adc_handle_map;

//irs::arm::st_hal_adc_dma_t* irs::arm::st_hal_adc_dma_t::mp_temp;

// class st_hal_adc_dma_t
irs::arm::st_hal_adc_dma_t::st_hal_adc_dma_t(const settings_t& a_settings/*,
  counter_t a_adc_interval,
  counter_t a_adc_battery_interval,
  bool a_single_conversion*/
):
  m_settings(a_settings),
  mp_adc(reinterpret_cast<adc_regs_t*>(a_settings.adc_address)),
  /*m_adc_timer(a_adc_interval),
  m_adc_battery_timer(a_adc_battery_interval),*/
  m_regular_channels_values(),
  m_active_channels(),
  m_current_channel(),
  m_temperature_sensor_enabled(false),
  m_temperature_channel_value(0),
  m_v_battery_measurement_enabled(false),
  m_v_battery_channel_value(0),
  m_injected_channel_selected(ics_temperature_sensor),
  //m_single_conversion(a_single_conversion),

  m_adc_handle(),
  m_hdma_rx(),
  m_rx_status(false),
  m_hdma_init(false),

  m_rx_buffer(a_settings.rx_buffer),
  m_dma_event(&m_hdma_rx)
{
  IRS_LIB_ASSERT(a_settings.adc_address);
  IRS_LIB_ASSERT(a_settings.dma_address);


  IRS_LIB_ASSERT(IS_DMA_STREAM_ALL_INSTANCE(a_settings.rx_dma_y_stream_x));
  IRS_LIB_ASSERT(IS_DMA_CHANNEL(a_settings.rx_dma_channel));
  IRS_LIB_ASSERT(IS_DMA_PRIORITY(a_settings.rx_dma_priority));

  IRS_LIB_ASSERT((a_settings.data_item_byte_count == 1) ||
    (a_settings.data_item_byte_count == 2));

  IRS_LIB_ASSERT(a_settings.rx_buffer.data());
  IRS_LIB_ASSERT(a_settings.rx_buffer.size() >=
    a_settings.data_item_byte_count);
  IRS_LIB_ASSERT(
    (a_settings.rx_buffer.size()%
    (a_settings.rx_buffer.size()/a_settings.data_item_byte_count)) == 0);

  #if defined(IRS_STM32F2xx) || defined(IRS_STM32F2xx)
  // ������ �� ������
  #elif IRS_STM32F7xx
  IRS_LIB_ASSERT(reinterpret_cast<size_t>(a_settings.rx_buffer.data())%32 == 0);
  //IRS_LIB_ASSERT(a_settings.rx_buffer.size()%32 == 0);
  #else
  #error ��� ����������� �� ��������
  #endif // IRS_STM32F7xx

  map<adc_channel_t, gpio_channel_t> adc_gpio_pairs;
  adc_gpio_pairs.insert(make_pair(ADC123_PA0_CH0, PA0));
  adc_gpio_pairs.insert(make_pair(ADC123_PA1_CH1, PA1));
  adc_gpio_pairs.insert(make_pair(ADC123_PA2_CH2, PA2));
  adc_gpio_pairs.insert(make_pair(ADC123_PA3_CH3, PA3));
  adc_gpio_pairs.insert(make_pair(ADC12_PA4_CH4, PA4));
  adc_gpio_pairs.insert(make_pair(ADC3_PF6_CH4, PF6));
  adc_gpio_pairs.insert(make_pair(ADC12_PA5_CH5, PA5));
  adc_gpio_pairs.insert(make_pair(ADC3_PF7_CH5, PF7));
  adc_gpio_pairs.insert(make_pair(ADC12_PA6_CH6, PA6));
  adc_gpio_pairs.insert(make_pair(ADC3_PF8_CH6, PF8));
  adc_gpio_pairs.insert(make_pair(ADC12_PA7_CH7, PA7));
  adc_gpio_pairs.insert(make_pair(ADC3_PF9_CH7, PF9));
  adc_gpio_pairs.insert(make_pair(ADC12_PB0_CH8, PB0));
  adc_gpio_pairs.insert(make_pair(ADC3_PF10_CH8, PF10));
  adc_gpio_pairs.insert(make_pair(ADC12_PB1_CH9, PB1));
  adc_gpio_pairs.insert(make_pair(ADC3_PF3_CH9, PF3));
  adc_gpio_pairs.insert(make_pair(ADC123_PC0_CH10, PC0));
  adc_gpio_pairs.insert(make_pair(ADC123_PC1_CH11, PC1));
  adc_gpio_pairs.insert(make_pair(ADC123_PC2_CH12, PC2));
  adc_gpio_pairs.insert(make_pair(ADC123_PC3_CH13, PC3));
  adc_gpio_pairs.insert(make_pair(ADC12_PC4_CH14, PC4));
  adc_gpio_pairs.insert(make_pair(ADC3_PF4_CH14, PF4));
  adc_gpio_pairs.insert(make_pair(ADC12_PC5_CH15, PC5));
  adc_gpio_pairs.insert(make_pair(ADC3_PF5_CH15, PF5));

  map<adc_channel_t, irs_u32> adc_channel_pairs;
  adc_channel_pairs.insert(make_pair(ADC123_PA0_CH0, ADC_CHANNEL_0));
  adc_channel_pairs.insert(make_pair(ADC123_PA1_CH1, ADC_CHANNEL_1));
  adc_channel_pairs.insert(make_pair(ADC123_PA2_CH2, ADC_CHANNEL_2));
  adc_channel_pairs.insert(make_pair(ADC123_PA3_CH3, ADC_CHANNEL_3));
  adc_channel_pairs.insert(make_pair(ADC12_PA4_CH4, ADC_CHANNEL_4));
  adc_channel_pairs.insert(make_pair(ADC3_PF6_CH4, ADC_CHANNEL_4));
  adc_channel_pairs.insert(make_pair(ADC12_PA5_CH5, ADC_CHANNEL_5));
  adc_channel_pairs.insert(make_pair(ADC3_PF7_CH5, ADC_CHANNEL_5));
  adc_channel_pairs.insert(make_pair(ADC12_PA6_CH6, ADC_CHANNEL_6));
  adc_channel_pairs.insert(make_pair(ADC3_PF8_CH6, ADC_CHANNEL_6));
  adc_channel_pairs.insert(make_pair(ADC12_PA7_CH7, ADC_CHANNEL_7));
  adc_channel_pairs.insert(make_pair(ADC3_PF9_CH7, ADC_CHANNEL_7));
  adc_channel_pairs.insert(make_pair(ADC12_PB0_CH8, ADC_CHANNEL_8));
  adc_channel_pairs.insert(make_pair(ADC3_PF10_CH8, ADC_CHANNEL_8));
  adc_channel_pairs.insert(make_pair(ADC12_PB1_CH9, ADC_CHANNEL_9));
  adc_channel_pairs.insert(make_pair(ADC3_PF3_CH9, ADC_CHANNEL_9));
  adc_channel_pairs.insert(make_pair(ADC123_PC0_CH10, ADC_CHANNEL_10));
  adc_channel_pairs.insert(make_pair(ADC123_PC1_CH11, ADC_CHANNEL_11));
  adc_channel_pairs.insert(make_pair(ADC123_PC2_CH12, ADC_CHANNEL_12));
  adc_channel_pairs.insert(make_pair(ADC123_PC3_CH13, ADC_CHANNEL_13));
  adc_channel_pairs.insert(make_pair(ADC12_PC4_CH14, ADC_CHANNEL_14));
  adc_channel_pairs.insert(make_pair(ADC3_PF4_CH14, ADC_CHANNEL_14));
  adc_channel_pairs.insert(make_pair(ADC12_PC5_CH15, ADC_CHANNEL_15));
  adc_channel_pairs.insert(make_pair(ADC3_PF5_CH15, ADC_CHANNEL_15));
  adc_channel_pairs.insert(make_pair(ADC1_TEMPERATURE, ADC_CHANNEL_16));
  adc_channel_pairs.insert(make_pair(ADC1_V_BATTERY, ADC_CHANNEL_VBAT));

  /*select_channel_type selected_adc_mask = ADC1_MASK;
  switch (a_settings.adc_address) {
    case IRS_ADC1_BASE: {
      selected_adc_mask = ADC1_MASK;
    } break;
    case IRS_ADC2_BASE: {
      selected_adc_mask = ADC2_MASK;
    } break;
    case IRS_ADC3_BASE: {
      selected_adc_mask = ADC3_MASK;
    } break;
  }*/

  for (size_type i = 0; i < a_settings.channels.size(); i++) {
    map<adc_channel_t, gpio_channel_t>::const_iterator it =
      adc_gpio_pairs.find(a_settings.channels[i].channel_id);
    if (it != adc_gpio_pairs.end()) {
      //adc_channel_t adc_channel = adc_gpio_pairs[i].first;
      //const select_channel_type adc_mask = selected_adc_mask & adc_channel;
      //const select_channel_type channel_mask = static_cast<irs_u16>(adc_channel);

      clock_enable(it->second);
      gpio_moder_analog_enable(it->second);
      //m_active_channels.push_back(adc_channel_to_channel_index(adc_channel));
      m_regular_channels_values.push_back(0);

    } else {
      IRS_LIB_ASSERT_MSG("������ ������������ ����� ���");
    }
  }

  clock_enable(a_settings.adc_address);

  ADC_TypeDef* adc_ptr = reinterpret_cast<ADC_TypeDef*>(a_settings.adc_address);

  m_adc_handle.Instance = adc_ptr;
  m_adc_handle.Init.ClockPrescaler = m_settings.clock_prescaller;// ADC_CLOCKPRESCALER_PCLK_DIV8;
  m_adc_handle.Init.Resolution = ADC_RESOLUTION_12B;
  m_adc_handle.Init.ScanConvMode = ENABLE;
  m_adc_handle.Init.ContinuousConvMode = ENABLE;
  m_adc_handle.Init.DiscontinuousConvMode = DISABLE;
  m_adc_handle.Init.NbrOfDiscConversion = 0;
  m_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  m_adc_handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
  m_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  m_adc_handle.Init.NbrOfConversion = a_settings.channels.size();
  m_adc_handle.Init.DMAContinuousRequests = ENABLE;
  m_adc_handle.Init.EOCSelection = DISABLE;

  if (HAL_ADC_Init(&m_adc_handle) != HAL_OK) {
    IRS_LIB_ASSERT_MSG("�� ���������� ���������������� ���");
  }


  for (size_type i = 0; i < a_settings.channels.size(); i++) {
    const channel_config_t channel_cfg = a_settings.channels[i];
    ADC_ChannelConfTypeDef sConfig;
    map<adc_channel_t, irs_u32>::const_iterator it =
      adc_channel_pairs.find(channel_cfg.channel_id);
    if (it != adc_channel_pairs.end()) {
      sConfig.Channel = it->second;
      sConfig.Rank = i + 1;
      sConfig.SamplingTime = channel_cfg.sampling_time;
      sConfig.Offset = 0;

      if (HAL_ADC_ConfigChannel(&m_adc_handle, &sConfig) != HAL_OK) {
        IRS_LIB_ASSERT_MSG("�� ������� ���������������� ����� ���");
      }
    } else {
      IRS_LIB_ASSERT_MSG("������ ������������ ����� ���");
    }
  }

  irs::clock_enable(a_settings.dma_address);

  irs::interrupt_array()->int_gen_events(a_settings.interrupt_id)->
    push_back(&m_dma_event);
  /* Enable the DMA STREAM global Interrupt */
  HAL_NVIC_EnableIRQ(a_settings.interrupt);
  NVIC_SetPriority (a_settings.interrupt, a_settings.interrupt_priority);

  reset_dma();

  if (HAL_ADC_Start_DMA(&m_adc_handle,
      (uint32_t*)m_rx_buffer.begin(),
      m_rx_buffer.size()/a_settings.data_item_byte_count) != HAL_OK) {
    IRS_LIB_ASSERT_MSG("�� ������� ��������� ��� � ��� ������");
  }

  /*
  //  000: 3 cycles
  //  001: 15 cycles
  //  010: 28 cycles
  //  011: 56 cycles
  //  100: 84 cycles
  //  101: 112 cycles
  //  110: 144 cycles
  //  111: 480 cycles
  enum {
    num_of_smpr1_channels = 9,
    num_of_smpr2_channels = 10,
    bit_len = 3,
    mask = 0x07
  };
  irs_u32 smpr = 0;
  for (irs_u8 i = 0; i < num_of_smpr1_channels; i++) {
    smpr |= (a_settings.sampling_time & mask) << (i * bit_len);
  }
  mp_adc->ADC_SMPR1 = smpr;
  smpr = 0;
  for (irs_u8 i = 0; i < num_of_smpr2_channels; i++) {
    smpr |= (a_settings.sampling_time & mask) << (i * bit_len);
  }
  mp_adc->ADC_SMPR2 = smpr;
  //  00: PCLK2 divided by 2
  //  01: PCLK2 divided by 4
  //  10: PCLK2 divided by 6
  //  11: PCLK2 divided by 8
  ADC_CCR_bit.ADCPRE = a_settings.clock_div;

  const irs_u32 channel_mask = a_settings.selected_channels & 0xFFFFFF;
  if ((a_settings.adc_address == IRS_ADC1_BASE) &&
    (a_settings.selected_channels & ADC1_MASK) &&
    (channel_mask & ADC1_TEMPERATURE)) {
    m_temperature_sensor_enabled = true;
  }

  if ((a_settings.adc_address == IRS_ADC1_BASE) &&
    (a_settings.selected_channels & ADC1_MASK) &&
    (channel_mask & ADC1_V_BATTERY)) {
    m_v_battery_measurement_enabled = true;
  }

  if (m_temperature_sensor_enabled || m_v_battery_measurement_enabled ||
    !m_regular_channels_values.empty()) {
    // �������� ���
    mp_adc->ADC_CR2_bit.ADON = 1;
  }
  // 0: ���� �����
  mp_adc->ADC_SQR1_bit.L = 0;
  if (!m_active_channels.empty()) {
    m_current_channel = 0;
    mp_adc->ADC_SQR3_bit.SQ1 = m_active_channels[m_current_channel];
    if (!m_single_conversion) {
      mp_adc->ADC_CR2_bit.SWSTART = 1;
    }
  }
  if (m_temperature_sensor_enabled) {
    ADC_CCR_bit.TSVREFE = 1;
  }
  if (m_v_battery_measurement_enabled) {
    //ADC_CCR_bit.VBATE = 1;
  }
  if (m_temperature_sensor_enabled) {
    // ������������� �� ���������� ��������� ����������� ������������
    mp_adc->ADC_JSQR_bit.JSQ4 = 16;
    m_injected_channel_selected = ics_temperature_sensor;
  } else if (m_v_battery_measurement_enabled) {
    // ������������� �� ���������� ��������� ���������� ���������
    mp_adc->ADC_JSQR_bit.JSQ4 = 18;
    m_injected_channel_selected = ics_v_battery;
    ADC_CCR_bit.VBATE = 1;
  }
  if (m_temperature_sensor_enabled || m_v_battery_measurement_enabled) {
    // 0: ���� ���������� �����
    mp_adc->ADC_JSQR_bit.JL = 0;
    mp_adc->ADC_CR2_bit.JSWSTART = 1;
  }
*/
  //m_adc_handle_map[&m_adc_handle] = this;
  //mp_temp = this;
}

irs_u32 irs::arm::st_hal_adc_dma_t::adc_channel_to_channel_index(
  adc_channel_t a_adc_channel)
{
  irs_u16 adc_channel = static_cast<irs_u16>(a_adc_channel);
  for (size_t i = 0; i < reqular_channel_count; i++) {
    if (adc_channel & 0x1) {
      return i;
    }
    adc_channel >>= 1;
  }
  IRS_LIB_ERROR(ec_standard, "����� �� ������");
  return 0;
}

irs::arm::st_hal_adc_dma_t::~st_hal_adc_dma_t()
{
  //mp_adc->ADC_CR2_bit.ADON = 0;
  HAL_ADC_Stop_DMA(&m_adc_handle); //stop_dma();

  //m_adc_handle_map.erase(&m_adc_handle);

  HAL_DMA_DeInit(&m_hdma_rx);
  HAL_ADC_DeInit(&m_adc_handle);
}

irs::arm::st_hal_adc_dma_t::size_type irs::arm::st_hal_adc_dma_t::get_resolution() const
{
  return adc_resolution;
}

irs_u16 irs::arm::st_hal_adc_dma_t::get_u16_minimum()
{
  return 0;
}

irs_u16 irs::arm::st_hal_adc_dma_t::get_u16_maximum()
{
  return static_cast<irs_u16>(adc_max_value) << (16 - adc_resolution);
}

irs_u16 irs::arm::st_hal_adc_dma_t::get_u16_data(irs_u8 a_channel)
{
  if (a_channel >= m_regular_channels_values.size()) {
    IRS_LIB_ERROR(ec_standard, "��� ������ � ����� �������");
  }
  return m_regular_channels_values[a_channel] << (16 - adc_resolution);
}

irs_u32 irs::arm::st_hal_adc_dma_t::get_u32_minimum()
{
  return 0;
}

irs_u32 irs::arm::st_hal_adc_dma_t::get_u32_maximum()
{
  return static_cast<irs_u32>(adc_max_value) << (32 - adc_resolution);
}

irs_u32 irs::arm::st_hal_adc_dma_t::get_u32_data(irs_u8 a_channel)
{
  if (a_channel >= m_regular_channels_values.size()) {
    IRS_LIB_ERROR(ec_standard, "��� ������ � ����� �������");
  }
  return m_regular_channels_values[a_channel] << (32 - adc_resolution);
}

float irs::arm::st_hal_adc_dma_t::get_float_minimum()
{
  return 0.f;
}

float irs::arm::st_hal_adc_dma_t::get_float_maximum()
{
  return 1.f;
}

float irs::arm::st_hal_adc_dma_t::get_float_data(irs_u8 a_channel)
{
  if (a_channel >= m_regular_channels_values.size()) {
    IRS_LIB_ERROR(ec_standard, "��� ������ � ����� �������");
  }
  return static_cast<float>(m_regular_channels_values[a_channel])/adc_max_value;
}

float irs::arm::st_hal_adc_dma_t::get_temperature()
{
  if (m_temperature_sensor_enabled) {
    return static_cast<float>(m_temperature_channel_value)/adc_max_value;
  }
  return 0;
}

float irs::arm::st_hal_adc_dma_t::get_v_battery()
{
  if (m_v_battery_measurement_enabled) {
    float k = 2;
    #ifdef IRS_STM32F7xx
    k = 4;
    #endif // IRS_STM32F7xx
    return static_cast<float>(m_v_battery_channel_value)*k/adc_max_value;
  }
  return 0;
}

float irs::arm::st_hal_adc_dma_t::get_temperature_degree_celsius(
  const float a_vref)
{
  if (m_temperature_sensor_enabled) {
    const float v25 = 0.76;
    const float avg_slope = 0.0025;
    const float koef = a_vref/adc_max_value;
    const float v_sence = m_temperature_channel_value*koef;
    return (v_sence - v25)/avg_slope + 25;
  }
  return 0;
}

void irs::arm::st_hal_adc_dma_t::tick()
{
  /*const bool timer_check = m_adc_timer.check();
  if (timer_check && !m_single_conversion) {
    if (mp_adc->ADC_SR_bit.EOC == 1) {
      m_regular_channels_values[m_current_channel] =
        static_cast<irs_u16>(mp_adc->ADC_DR);
      m_current_channel++;
      if (m_current_channel >= m_active_channels.size()) {
        m_current_channel = 0;
      }
      mp_adc->ADC_SQR3_bit.SQ1 = m_active_channels[m_current_channel];
      mp_adc->ADC_CR2_bit.SWSTART = 1;
    }
  }
  if (m_temperature_sensor_enabled || m_v_battery_measurement_enabled) {
    const bool adc_battery_timer_check = m_adc_battery_timer.check();
    if (mp_adc->ADC_SR_bit.JEOC == 0) {
      if (timer_check || adc_battery_timer_check) {
        if (m_injected_channel_selected == ics_temperature_sensor) {
          if (m_v_battery_measurement_enabled && adc_battery_timer_check) {
            mp_adc->ADC_JSQR_bit.JSQ4 = 18;
            ADC_CCR_bit.VBATE = 1;
            m_injected_channel_selected = ics_v_battery;
          }
        } else {
          if (m_temperature_sensor_enabled && timer_check) {
            mp_adc->ADC_JSQR_bit.JSQ4 = 16;
            ADC_CCR_bit.VBATE = 0;
            m_injected_channel_selected = ics_temperature_sensor;
          } else {
            ADC_CCR_bit.VBATE = 1;
          }
        }
        mp_adc->ADC_SR_bit.JEOC = 0;
        mp_adc->ADC_CR2_bit.JSWSTART = 1;
      }
    }
    if (mp_adc->ADC_SR_bit.JEOC == 1) {
      IRS_LIB_ASSERT((m_injected_channel_selected == ics_v_battery) ==
        (ADC_CCR_bit.VBATE == 1));
      read_injected_channel_value();
      // ��������� �������� ��� �������� ������ ���������
      ADC_CCR_bit.VBATE = 0;
      mp_adc->ADC_SR_bit.JEOC = 0;
    }
  }*/
}

void irs::arm::st_hal_adc_dma_t::read_injected_channel_value()
{
  if (m_injected_channel_selected == ics_temperature_sensor) {
    m_temperature_channel_value =
      static_cast<irs_i16>(mp_adc->ADC_JDR1_bit.JDATA);
  } else {
    m_v_battery_channel_value =
      static_cast<irs_u16>(mp_adc->ADC_JDR1_bit.JDATA);
  }
}

void irs::arm::st_hal_adc_dma_t::reset_dma()
{
  if (m_hdma_init) {
    //stop_dma();
    //HAL_DMA_DeInit(&m_hdma_rx);
  }

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Reception process */
  m_hdma_rx.Instance                 = m_settings.rx_dma_y_stream_x;
  m_hdma_rx.Init.Channel             = m_settings.rx_dma_channel;
  m_hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  m_hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_1QUARTERFULL;

  m_hdma_rx.Init.MemBurst            = DMA_MBURST_SINGLE;
  m_hdma_rx.Init.PeriphBurst         = DMA_PBURST_SINGLE;

  m_hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  m_hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  m_hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;

  switch (m_settings.data_item_byte_count) {
    case 1: {
    m_hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    m_hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    } break;
    case 2: {
    m_hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    m_hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    } break;
    default: {
      IRS_ASSERT_MSG("���������� �������� ������� ��������");
    }
  }

  m_hdma_rx.Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  m_hdma_rx.Init.Priority            = m_settings.rx_dma_priority;

  /*m_hdma_rx.XferCpltCallback = sin_pwm_transfer_complete;//m_settings.XferCpltCallback;
  m_hdma_rx.XferHalfCpltCallback = m_settings.XferHalfCpltCallback;
  m_hdma_rx.XferErrorCallback = m_settings.XferErrorCallback;
*/
  HAL_DMA_Init(&m_hdma_rx);


  //mp_adc->ADC_CR2_bit.DMA = 1;

  __HAL_LINKDMA(&m_adc_handle, DMA_Handle, m_hdma_rx);

  m_hdma_init = true;
}

void irs::arm::st_hal_adc_dma_t::start_dma()
{
  HAL_DMA_Start_IT(&m_hdma_rx,
    reinterpret_cast<irs_u32>(&mp_adc->ADC_DR),
    (uint32_t)m_rx_buffer.data(),
    m_settings.rx_buffer.size()/m_settings.data_item_byte_count);
}

void irs::arm::st_hal_adc_dma_t::stop_dma()
{
  HAL_DMA_Abort(&m_hdma_rx);
}

void irs::arm::st_hal_adc_dma_t::invalidate_dcache_rx()
{
  #if defined(IRS_STM32F2xx) || defined(IRS_STM32F2xx)
  // ������ �� ������
  #elif IRS_STM32F7xx
  SCB_InvalidateDCache_by_Addr(reinterpret_cast<uint32_t*>(m_rx_buffer.data()),
    m_rx_buffer.size());
  #else
  #error ��� ����������� �� ��������
  #endif // IRS_STM32F7xx
}

ADC_HandleTypeDef* irs::arm::st_hal_adc_dma_t::get_adc_handle()
{
  return &m_adc_handle;
}

void irs::arm::st_hal_adc_dma_t::transfer_complete(ADC_HandleTypeDef *ap_adc_handle)
{
  process(sbp_right_part);

  /*map<ADC_HandleTypeDef*, st_hal_adc_dma_t*>::const_iterator it =
    m_adc_handle_map.find(ap_adc_handle);
  if (it != m_adc_handle_map.end()) {
    it->second->process(sbp_right_part);
  }*/
  /*if (mp_temp) {
    mp_temp->process(sbp_right_part);
  }*/
}

void irs::arm::st_hal_adc_dma_t::half_transfer_complete(ADC_HandleTypeDef *ap_adc_handle)
{
  process(sbp_left_part);

  /*map<ADC_HandleTypeDef*, st_hal_adc_dma_t*>::const_iterator it =
    m_adc_handle_map.find(ap_adc_handle);
  if (it != m_adc_handle_map.end()) {
    it->second->process(sbp_left_part);
  }*/

  /*if (mp_temp) {
    mp_temp->process(sbp_left_part);
  }*/
}

void irs::arm::st_hal_adc_dma_t::xfer_error_callback(ADC_HandleTypeDef *ap_adc_handle)
{
  irs_u32 error = HAL_DMA_GetError(ap_adc_handle->DMA_Handle);
  if (error != HAL_DMA_ERROR_NONE) {
    IRS_LIB_DBG_MSG("ADC DMA ERROR " << error);
  }
}

void irs::arm::st_hal_adc_dma_t::process(part_t a_part)
{
  invalidate_dcache_rx();

  //560
  irs_u16* buf = reinterpret_cast<irs_u16*>(m_rx_buffer.begin());
  const size_type n = m_rx_buffer.size()/sizeof(irs_u16); //data_item_byte_count
  size_type buf_size = n/2;


  switch (a_part) {
    case sbp_left_part: {
      // ��������� ��� ��������� �� ������ �������
    } break;
    case sbp_right_part: {
      buf += buf_size;
      buf_size = n - n/2;
    } break;
  }

  const size_type step = m_regular_channels_values.size();

  for (size_type i = 0; i < m_regular_channels_values.size(); i++) {
    m_regular_channels_values[i] = static_cast<irs_u16>(
      average<irs_u16, float>(buf+i, buf + buf_size, step));
  }
}

// class dma_event_t
irs::arm::st_hal_adc_dma_t::dma_event_t::dma_event_t(
  DMA_HandleTypeDef* ap_hdma_tx
):
  mp_hdma_rx(ap_hdma_tx),
  m_enabled(false)
{
}

void irs::arm::st_hal_adc_dma_t::dma_event_t::exec()
{
  irs::event_t::exec();
  /* Check the interrupt and clear flag */
  HAL_DMA_IRQHandler(mp_hdma_rx);
  //HAL_DMA_IRQHandler(m_adc_handle.DMA_Handle);
}

#endif // USE_HAL_DRIVER

irs::arm::st_adc_dma_t::settings_adc_dma_t::settings_adc_dma_t(
  size_t a_adc_address,
  select_channel_type a_adc_selected_channels,
  size_t a_dma_address,
  dma_stream_t a_dma_stream,
  dma_channel_t a_dma_channel,
  size_t a_timer_address,
  timer_channel_t a_timer_channel
):
  adc_address(a_adc_address),
  adc_selected_channels(a_adc_selected_channels),
  dma_address(a_dma_address),
  dma_stream(a_dma_stream),
  dma_channel(a_dma_channel),
  timer_address(a_timer_address),
  timer_channel(a_timer_channel)
{
}

irs::arm::st_adc_dma_t::st_adc_dma_t(settings_adc_dma_t* ap_settings,
  irs::c_array_view_t<irs_u16> a_buff,
  double a_frequency
):
  mp_adc(reinterpret_cast<adc_regs_t*>(ap_settings->adc_address)),
  m_regular_channels_values(),
  m_active_channels(),
  m_current_channel(),
  m_temperature_channel_value(),
  mp_dma(reinterpret_cast<dma_regs_t*>(ap_settings->dma_address)),
  mp_timer(reinterpret_cast<tim_regs_t*>(ap_settings->timer_address)),
  m_buff(a_buff),
  m_frequency(a_frequency),
  m_set_freq(0),
  m_psc(0),
  m_buff_size(),
  m_start(false),
  mp_settings(ap_settings)
{
  if (m_buff.data() != IRS_NULL) {
    m_buff_size = m_buff.size();
  } else {
    m_buff_size = 0;
  }
  irs::clock_enable(mp_settings->dma_address);

  mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.EN = 0;
  //����� �������� ���������
  mp_dma->stream[mp_settings->dma_stream].DMA_SPAR =
    reinterpret_cast<irs_u32>(&mp_adc->ADC_DR);
  //����� ������ � ������
  mp_dma->stream[mp_settings->dma_stream].DMA_SM0AR =
    reinterpret_cast<irs_u32>(m_buff.data());
  //���������� ������ ��� ������
  mp_dma->stream[mp_settings->dma_stream].DMA_SNDTR =
    static_cast<irs_u16>(m_buff_size);

  mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.CHSEL =
    mp_settings->dma_channel;

  //mp_dma->stream[0].DMA_SCR_bit.PFCTRL = 1;
  //��������� ����������� �����
  //�������� ��� �������� � PFCTRL
  mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.CIRC = 0;

  //�����������: ������ � ������
  mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.DIR = 0;
  //��������� ������ � ������������ �����������
  //����������� ������ 16 ���
  mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.PSIZE = 1;
  //�������������� ��������� ���������
  mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.PINC = 0;

  //��������� ������ � �������
  //����������� ������ 16 ���
  mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.MSIZE = 1;
  //������������ ��������� ���������
  mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.MINC = 1;

  mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.PL = 2; //���������

  clock_enable(mp_settings->adc_address);

  //0: PCLK2 divided by 4
  ADC_CCR_bit.ADCPRE = 1;
  set_sample_time(m_frequency);

  vector<pair<adc_channel_t, gpio_channel_t> > adc_gpio_pairs;
  adc_gpio_pairs.push_back(make_pair(ADC123_PA0_CH0, PA0));
  adc_gpio_pairs.push_back(make_pair(ADC123_PA1_CH1, PA1));
  adc_gpio_pairs.push_back(make_pair(ADC123_PA2_CH2, PA2));
  adc_gpio_pairs.push_back(make_pair(ADC123_PA3_CH3, PA3));
  adc_gpio_pairs.push_back(make_pair(ADC12_PA4_CH4, PA4));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF6_CH4, PF6));
  adc_gpio_pairs.push_back(make_pair(ADC12_PA5_CH5, PA5));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF7_CH5, PF7));
  adc_gpio_pairs.push_back(make_pair(ADC12_PA6_CH6, PA6));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF8_CH6, PF8));
  adc_gpio_pairs.push_back(make_pair(ADC12_PA7_CH7, PA7));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF9_CH7, PF9));
  adc_gpio_pairs.push_back(make_pair(ADC12_PB0_CH8, PB0));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF10_CH8, PF10));
  adc_gpio_pairs.push_back(make_pair(ADC12_PB1_CH9, PB1));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF3_CH9, PF3));
  adc_gpio_pairs.push_back(make_pair(ADC123_PC0_CH10, PC0));
  adc_gpio_pairs.push_back(make_pair(ADC123_PC1_CH11, PC1));
  adc_gpio_pairs.push_back(make_pair(ADC123_PC2_CH12, PC2));
  adc_gpio_pairs.push_back(make_pair(ADC123_PC3_CH13, PC3));
  adc_gpio_pairs.push_back(make_pair(ADC12_PC4_CH14, PC4));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF4_CH14, PF4));
  adc_gpio_pairs.push_back(make_pair(ADC12_PC5_CH15, PC5));
  adc_gpio_pairs.push_back(make_pair(ADC3_PF5_CH15, PF5));

  select_channel_type selected_adc_mask = ADC1_MASK;
  switch (ap_settings->adc_address) {
    case IRS_ADC1_BASE: {
      selected_adc_mask = ADC1_MASK;
    } break;
    case IRS_ADC2_BASE: {
      selected_adc_mask = ADC2_MASK;
    } break;
    case IRS_ADC3_BASE: {
      selected_adc_mask = ADC3_MASK;
    } break;
  }

  for (size_t i = 0; i < adc_gpio_pairs.size(); i++) {
    adc_channel_t adc_channel = adc_gpio_pairs[i].first;
    const select_channel_type adc_mask = selected_adc_mask & adc_channel;
    const select_channel_type channel_mask = static_cast<irs_u16>(adc_channel);
    if ((mp_settings->adc_selected_channels & channel_mask) &&
      (mp_settings->adc_selected_channels & adc_mask)) {
      clock_enable(adc_gpio_pairs[i].second);
      gpio_moder_analog_enable(adc_gpio_pairs[i].second);
      m_active_channels.push_back(adc_channel_to_channel_index(adc_channel));
      m_regular_channels_values.push_back(0);
    }
  }

  // 0: ���� �����
  mp_adc->ADC_SQR1_bit.L = 0;
  m_current_channel = 0;
  mp_adc->ADC_SQR3_bit.SQ1 = m_active_channels[m_current_channel];

  //mp_adc->ADC_CR1_bit.SCAN = 1; //�� �����
  //mp_adc->ADC_CR2_bit.CONT = 1; //���� ����� � �������� �� ��� �� ����

  //----��� �������
  //mp_adc->ADC_CR2_bit.EXTSEL = 2; //Timer 1 CC3 event
  set_adc_timer_channel(mp_settings->timer_address, mp_settings->timer_channel);
  mp_adc->ADC_CR2_bit.EXTEN = 1; //Trigger detection on the rising edge
  //--------------

  mp_adc->ADC_CR2_bit.DDS = 1; //����� ��� �� ������������ ��� ����� ��������� ������ �����
  mp_adc->ADC_CR2_bit.DMA = 1;
  //mp_adc->ADC_CR2_bit.ADON = 1;


  //-----Timer---------------------------
  clock_enable(mp_settings->timer_address);
  mp_timer->TIM_CR1_bit.CEN = 0;
  //TIM_TimeBaseInit
  mp_timer->TIM_CR1_bit.CMS = 0; //Center-aligned mode selection
  mp_timer->TIM_CR1_bit.DIR = 0; //Counter used as upcounter

  m_psc = 0;
  m_set_freq = timer_frequency()/m_frequency;
  mp_timer->TIM_ARR = 
    static_cast<irs_u16>((m_set_freq/static_cast<float>(m_psc+1))-1+0.5);
  mp_timer->TIM_PSC = m_psc;

  mp_timer->TIM_RCR = 0;
  //mp_timer->TIM_EGR_bit.UG = 1; //Update generation

  //TIM_SetCounter
  mp_timer->TIM_CNT = 0;
  //TIM_OC1Init ����� ������ 1
  timer_set_bit(mp_settings->timer_address,
    IRS_TIM_CCMR, mp_settings->timer_channel, OCM, OCM_SIZE, 7);

  //Capture/Compare 1 output enable
  timer_set_bit(mp_settings->timer_address,
    IRS_TIM_CCER, mp_settings->timer_channel, CCP, CCP_SIZE, 1);
  timer_set_bit(mp_settings->timer_address,
    IRS_TIM_CCER, mp_settings->timer_channel, CCE, CCE_SIZE, 1);

  timer_set_bit(mp_settings->timer_address, IRS_TIM_CCR,
    mp_settings->timer_channel, CCR_REG, CCR_REG_SIZE, mp_timer->TIM_ARR - 1);


  //mp_timer->TIM_CCER_bit.CC3NP = 0; //Capture/Compare 1 complementary output polarity
  //mp_timer->TIM_CCER_bit.CC3NE = 0; //Capture/Compare 1 complementary output enable

  //mp_timer->TIM_CR2_bit.OIS3 = 0; // Output Idle state 1 (OC1 output)
  //mp_timer->TIM_CR2_bit.OIS3N = 0; //Output Idle state 1 (OC1N output)
  mp_timer->TIM_CCR3 = mp_timer->TIM_ARR - 1; //Capture/Compare 1 value


  //mp_timer->TIM_CCMR2_bit.OC3PE = 0; //Output Compare 1 preload enable
  //mp_timer->TIM_CR1_bit.OPM = 0; //One pulse mode

  mp_timer->TIM_BDTR_bit.MOE = 1;
  mp_timer->TIM_BDTR_bit.AOE = 1;

  //TIM_Cmd
  mp_timer->TIM_CR1_bit.ARPE = 1;
  //mp_timer->TIM_CR1_bit.CEN = 1;
  //--------------------------------
}

void irs::arm::st_adc_dma_t::set_sample_time(double&
  a_frequency)
{
  int denominator = ADC_CCR_bit.ADCPRE*2 + 2;
  cpu_traits_t::frequency_type adc_frequency =
    irs::cpu_traits_t::periphery_frequency_second()/denominator;
  if (adc_frequency > 30000000) {
    adc_frequency = 30000000;
  }
  int cycles = static_cast<int>(adc_frequency/a_frequency);
  int res = 12 - mp_adc->ADC_CR1_bit.RES*2;
  int sample_time = cycles - res;
  int count_sample  = 0;
  if (3 < sample_time) {
    count_sample = 0;
  } if (15 < sample_time) {
    count_sample = 1;
  } if (28 < sample_time) {
    count_sample = 2;
  } if (56 < sample_time) {
    count_sample = 3;
  } if (84 < sample_time) {
    count_sample = 4;
  } if (112 < sample_time) {
    count_sample = 5;
  } if (144 < sample_time) {
    count_sample = 6;
  } if (480 < sample_time) {
    count_sample = 7;
  }
  switch (m_active_channels[0]) {
    case 0: {
      mp_adc->ADC_SMPR2_bit.SMP0 = count_sample;
    } break;
    case 1: {
      mp_adc->ADC_SMPR2_bit.SMP1 = count_sample;
    } break;
    case 2: {
      mp_adc->ADC_SMPR2_bit.SMP2 = count_sample;
    } break;
    case 3: {
      mp_adc->ADC_SMPR2_bit.SMP3 = count_sample;
    } break;
    case 4: {
      mp_adc->ADC_SMPR2_bit.SMP4 = count_sample;
    } break;
    case 5: {
      mp_adc->ADC_SMPR2_bit.SMP5 = count_sample;
    } break;
    case 6: {
      mp_adc->ADC_SMPR2_bit.SMP6 = count_sample;
    } break;
    case 7: {
      mp_adc->ADC_SMPR2_bit.SMP7 = count_sample;
    } break;
    case 8: {
      mp_adc->ADC_SMPR2_bit.SMP8 = count_sample;
    } break;
    case 9: {
      mp_adc->ADC_SMPR2_bit.SMP9 = count_sample;
    } break;
    case 10: {
      mp_adc->ADC_SMPR1_bit.SMP10 = count_sample;
    } break;
    case 11: {
      mp_adc->ADC_SMPR1_bit.SMP11 = count_sample;
    } break;
    case 12: {
      mp_adc->ADC_SMPR1_bit.SMP12 = count_sample;
    } break;
    case 13: {
      mp_adc->ADC_SMPR1_bit.SMP13 = count_sample;
    } break;
    case 14: {
      mp_adc->ADC_SMPR1_bit.SMP14 = count_sample;
    } break;
    case 15: {
      mp_adc->ADC_SMPR1_bit.SMP15 = count_sample;
    } break;
    case 16: {
      mp_adc->ADC_SMPR1_bit.SMP16 = count_sample;
    } break;
    case 17: {
      mp_adc->ADC_SMPR1_bit.SMP17 = count_sample;
    } break;
    case 18: {
      mp_adc->ADC_SMPR1_bit.SMP18 = count_sample;
    } break;
  };
}

void irs::arm::st_adc_dma_t::set_adc_timer_channel(size_t a_timer_address,
  timer_channel_t a_timer_channel)
{
  int adc_shift = 0;
  if (a_timer_address == IRS_TIM1_PWM1_BASE) {
    adc_shift = 0;
    switch (a_timer_channel) {
      case TIM_CH1:
      case TIM_CH2:
      case TIM_CH3: {
        mp_adc->ADC_CR2_bit.EXTSEL = adc_shift + a_timer_channel;
      } break;
      default: {
        IRS_LIB_ERROR(ec_standard, "������������ ����� ������ �������");
      };
    };
  } else if (a_timer_address == IRS_TIM2_BASE) {
    adc_shift = 3 - TIM_CH2;
    switch (a_timer_channel) {
      case TIM_CH2:
      case TIM_CH3:
      case TIM_CH4: {
        mp_adc->ADC_CR2_bit.EXTSEL = adc_shift + a_timer_channel;
      } break;
      default: {
        IRS_LIB_ERROR(ec_standard, "������������ ����� ������ �������");
      };
    };
  } else if (a_timer_address == IRS_TIM3_BASE) {
    adc_shift = 7;
    switch (a_timer_channel) {
      case TIM_CH1: {
        mp_adc->ADC_CR2_bit.EXTSEL = adc_shift + a_timer_channel;
      } break;
      default: {
        IRS_LIB_ERROR(ec_standard, "������������ ����� ������ �������");
      };
    };
  } else if (a_timer_address == IRS_TIM4_BASE) {
    adc_shift = 9 - TIM_CH4;
    switch (a_timer_channel) {
      case TIM_CH4: {
        mp_adc->ADC_CR2_bit.EXTSEL = adc_shift + a_timer_channel;
      } break;
      default: {
        IRS_LIB_ERROR(ec_standard, "������������ ����� ������ �������");
      };
    };
  } else if (a_timer_address == IRS_TIM5_BASE) {
    adc_shift = 10;
    switch (a_timer_channel) {
      case TIM_CH1:
      case TIM_CH2:
      case TIM_CH3: {
        mp_adc->ADC_CR2_bit.EXTSEL = adc_shift + a_timer_channel;
      } break;
      default: {
        IRS_LIB_ERROR(ec_standard, "������������ ����� ������ �������");
      };
    };
  } else if (a_timer_address == IRS_TIM8_PWM2_BASE) {
    adc_shift = 13;
    switch (a_timer_channel) {
      case TIM_CH1: {
        mp_adc->ADC_CR2_bit.EXTSEL = adc_shift + a_timer_channel;
      } break;
      default: {
        IRS_LIB_ERROR(ec_standard, "������������ ����� ������ �������");
      };
    };
  }
}

irs_u32 irs::arm::st_adc_dma_t::adc_channel_to_channel_index(
  adc_channel_t a_adc_channel)
{
  irs_u16 adc_channel = static_cast<irs_u16>(a_adc_channel);
  for (size_t i = 0; i < reqular_channel_count; i++) {
    if (adc_channel & 0x1) {
      return i;
    }
    adc_channel >>= 1;
  }
  IRS_LIB_ERROR(ec_standard, "����� �� ������");
  return 0;
}

irs::cpu_traits_t::frequency_type irs::arm::st_adc_dma_t::timer_frequency()
{
  const size_t timer_address = reinterpret_cast<size_t>(mp_timer);
  return cpu_traits_t::timer_frequency(timer_address);
}

irs::arm::st_adc_dma_t::~st_adc_dma_t()
{
  mp_timer->TIM_CR1_bit.CEN = 0;
  mp_adc->ADC_CR2_bit.ADON = 0;
  mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.EN = 0;
}

void irs::arm::st_adc_dma_t::start()
{
  if (m_buff.data() != IRS_NULL) {
    if (!m_start) {
      m_start = true;
      mp_timer->TIM_CR1_bit.CEN = 0;
      mp_adc->ADC_CR2_bit.ADON = 0;
      mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.EN = 0;
      mp_timer->TIM_PSC = m_psc;
      mp_timer->TIM_ARR = static_cast<irs_u16>(
        (m_set_freq/(m_psc+1))-1+0.5);
      timer_set_bit(mp_settings->timer_address, IRS_TIM_CCR,
        mp_settings->timer_channel, CCR_REG, CCR_REG_SIZE, mp_timer->TIM_ARR - 1);
      dma_set_bit(mp_settings->dma_address, IRS_DMA_IFCR,
        mp_settings->dma_stream, CTCIF, 1);
      dma_set_bit(mp_settings->dma_address, IRS_DMA_IFCR,
        mp_settings->dma_stream, CHTIF, 1);
      mp_dma->stream[mp_settings->dma_stream].DMA_SNDTR =
        static_cast<irs_u16>(m_buff_size);
      //����� ������ � ������
      mp_dma->stream[mp_settings->dma_stream].DMA_SM0AR =
        reinterpret_cast<irs_u32>(m_buff.data());
      mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.EN = 1;
      mp_adc->ADC_CR2_bit.ADON = 1;
      mp_timer->TIM_CR1_bit.CEN = 1;
    }
  } else {
    IRS_LIB_ERROR(ec_standard, "������� ������� dma ��� ������� "
      "��������� �� ������ ������");
  }
}

void irs::arm::st_adc_dma_t::stop()
{
  mp_timer->TIM_CR1_bit.CEN = 0;
  mp_adc->ADC_CR2_bit.ADON = 0;
  mp_dma->stream[mp_settings->dma_stream].DMA_SCR_bit.EN = 0;
  m_start = false;
}

bool irs::arm::st_adc_dma_t::status()
{
  bool result = false;
  int tcif = dma_get_bit(mp_settings->dma_address, IRS_DMA_ISR,
    mp_settings->dma_stream, TCIF);
  if(tcif && m_start) {
    result = true;
    m_start = false;
  } else if (!m_start) {
    result = true;
  }
  return result;
}

void irs::arm::st_adc_dma_t::set_frequency(
  double a_frequency)
{
  m_frequency = a_frequency;
  set_sample_time(m_frequency);
  m_set_freq = timer_frequency()/m_frequency;
}

void irs::arm::st_adc_dma_t::set_prescaler(irs_u16 a_psc)
{
  m_psc = a_psc;
}

void irs::arm::st_adc_dma_t::set_buff(irs::c_array_view_t<irs_u16> a_buff)
{
  m_buff = a_buff;
  m_buff_size = m_buff.size();
}

// class st_dac_t
irs::arm::st_dac_t::st_dac_t(select_channel_type a_selected_channels)
{
  irs::clock_enable(IRS_DAC1_DAC2_BASE);
  if (a_selected_channels & DAC_PA4_CH0) {
    irs::clock_enable(PA4);
    irs::gpio_moder_analog_enable(PA4);
    DAC_CR_bit.EN1 = 1;
    m_channels.push_back(reinterpret_cast<volatile data_reg_t*>(&DAC_DHR12R1));
  }
  if (a_selected_channels & DAC_PA5_CH1) {
    irs::clock_enable(PA5);
    irs::gpio_moder_analog_enable(PA5);
    DAC_CR_bit.EN2 = 1;
    m_channels.push_back(reinterpret_cast<volatile data_reg_t*>(&DAC_DHR12R2));
  }
}

irs::arm::st_dac_t::size_type irs::arm::st_dac_t::get_resolution() const
{
  return dac_resolution;
}

irs_u32 irs::arm::st_dac_t::get_u32_maximum() const
{
  return static_cast<irs_u32>(dac_max_value) << (32 - dac_resolution);
}

void irs::arm::st_dac_t::set_u32_data(size_type a_channel, const irs_u32 a_data)
{
  set_u16_normalized_data(a_channel,
    static_cast<irs_u16>(a_data >> (32 - dac_resolution)));
}

float irs::arm::st_dac_t::get_float_maximum() const
{
  return 1.f;
}

void irs::arm::st_dac_t::set_float_data(size_type a_channel, const float a_data)
{
  if (a_data > 1.f) {
    IRS_LIB_ERROR(ec_standard, "�������� ������ ���� �� 0 �� 1");
  }
  set_u16_normalized_data(a_channel,
    static_cast<irs_u16>(a_data*dac_max_value));
}

void irs::arm::st_dac_t::set_u16_normalized_data(size_t a_channel,
  const irs_u16 a_data)
{
  if (a_channel >= m_channels.size()) {
    IRS_LIB_ERROR(ec_standard, "������������ �����");
  }
  IRS_LIB_ASSERT(a_data <= dac_max_value);
  (*m_channels[a_channel]).data = a_data;
}
#endif // IRS_STM32_F2_F4_F7
