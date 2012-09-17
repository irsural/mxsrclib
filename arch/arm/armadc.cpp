// ����� ��� ������ � ��� ARM
// ����: 28.06.2011
// ���� ��������: 13.01.2011

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

#elif defined(IRS_STM32F2xx)

// class st_adc_t
irs::arm::st_adc_t::st_adc_t(size_t a_adc_address,
  select_channel_type a_selected_channels,
  counter_t a_adc_interval
):
  mp_adc(reinterpret_cast<adc_regs_t*>(a_adc_address)),
  m_adc_timer(a_adc_interval),
  m_regular_channels_values(),
  m_active_channels(),
  m_current_channel(),
  m_temperature_channel_value()
{
  clock_enable(a_adc_address);
  mp_adc->ADC_SMPR1 = 0xFFFFFFFF;
  mp_adc->ADC_SMPR2 = 0xFFFFFFFF;
  // 3: PCLK2 divided by 8
  ADC_CCR_bit.ADCPRE = 3;

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

  for (size_t i = 0; i < adc_gpio_pairs.size(); i++) {
    adc_channel_t adc_channel = adc_gpio_pairs[i].first;
    const select_channel_type adc_mask = (ADC1 | ADC2 | ADC3) & adc_channel;
    const select_channel_type channel_mask = static_cast<irs_u16>(adc_channel);
    if ((a_selected_channels & channel_mask) &&
      (a_selected_channels & adc_mask)) {
      clock_enable(adc_gpio_pairs[i].second);
      gpio_moder_analog_enable(adc_gpio_pairs[i].second);
      m_active_channels.push_back(adc_channel_to_channel_index(adc_channel));
      m_regular_channels_values.push_back(0);
    }
  }

  if ((a_adc_address == ADC1_BASE) || !m_regular_channels_values.empty()) {
    // �������� ���
    mp_adc->ADC_CR2_bit.ADON = 1;
  }
  // 0: ���� �����
  mp_adc->ADC_SQR1_bit.L = 0;
  if (!m_active_channels.empty()) {
    m_current_channel = 0;
    mp_adc->ADC_SQR3_bit.SQ1 = m_active_channels[m_current_channel];
    mp_adc->ADC_CR2_bit.SWSTART = 1;
  }
  if (a_adc_address == ADC1_BASE) {
    ADC_CCR_bit.TSVREFE = 1;
    // 0: ���� ���������� �����
    mp_adc->ADC_JSQR_bit.JL = 0;
    // ������������� �� ���������� ��������� ����������� ������������
    mp_adc->ADC_JSQR_bit.JSQ4 = 16;
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
  }
  return static_cast<float>(m_regular_channels_values[a_channel])/adc_max_value;
}

float irs::arm::st_adc_t::get_temperature()
{
  if (reinterpret_cast<size_t>(mp_adc) == ADC1_BASE) {
    const float v25 = 0.76;
    const float avg_Slope = 2.5;
    const float v_ref = 3.3;
    const float koef = v_ref/adc_max_value;
    const float v_sence = m_temperature_channel_value*koef;
    // ������� �� ��������!!!
    return (v_sence - v25)/avg_Slope + 25;
  } else {
    return 0;
  }
}

void irs::arm::st_adc_t::tick()
{
  if (m_adc_timer.check()) {
    if (mp_adc->ADC_SR_bit.EOC == 1) {
      m_regular_channels_values[m_current_channel] =
        static_cast<irs_u16>(mp_adc->ADC_DR);
      //IRS_DBG_RAW_MSG(CNT_TO_DBLTIME(counter_get()) <<
        //" ADC_DR =  " << m_regular_channels_values[m_current_channel] << endl);
      m_current_channel++;
      if (m_current_channel >= m_active_channels.size()) {
        m_current_channel = 0;
      }
      mp_adc->ADC_SQR3_bit.SQ1 = m_active_channels[m_current_channel];
      mp_adc->ADC_CR2_bit.SWSTART = 1;
    }
    if (reinterpret_cast<size_t>(mp_adc) == ADC1_BASE) {
      if (mp_adc->ADC_SR_bit.JEOC == 1) {
        m_temperature_channel_value =
          static_cast<irs_i16>(mp_adc->ADC_JDR1_bit.JDATA);
        mp_adc->ADC_CR2_bit.JSWSTART = 1;
      }
    }
  }
}

// class st_dac_t
irs::arm::st_dac_t::st_dac_t(select_channel_type a_selected_channels)
{
  irs::clock_enable(DAC1_DAC2_BASE);
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
#endif // IRS_STM32F2xx
