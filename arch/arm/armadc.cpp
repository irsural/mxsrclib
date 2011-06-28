// Класс для работы с АЦП ARM
// Дата: 28.06.2011
// Дата создания: 13.01.2011

#include <irsdefs.h>

#include <armioregs.h>

#include <armadc.h>

#include <mxdata.h>

#include <irsfinal.h>

#ifdef NOP
// Необходимо сделать класс универсальным

irs::arm::adc_t::adc_t(
  counter_t a_adc_timer
):
  m_status(begin_ADC_sampling),
  mp_data(m_channel_max_number),
  m_adc_timer(a_adc_timer) // частота обработки АЦП
{
  // ADC0 Module Initialization
  RCGC0_bit.ADC0 = 1;
  RCGC0_bit.MAXADC0SPD = 0x3;  //  = 1 MSPS
  RCGC2_bit.PORTD = 1;
  RCGC2_bit.PORTE = 1;
  RCGC2_bit.PORTB = 1;
  for (irs_u8 i = 10; i; i--);

  GPIODDEN_bit.no1 = 0;
  GPIODAMSEL_bit.no1 = 1;
  //GPIODDEN_bit.no2 = 0;   //  Ahtung!!!
  //GPIODAMSEL_bit.no2 = 1;
  GPIODDEN_bit.no3 = 0;
  GPIODAMSEL_bit.no3 = 1;
  GPIODDEN_bit.no4 = 0;
  GPIODAMSEL_bit.no4 = 1;
  GPIODDEN_bit.no5 = 0;
  GPIODAMSEL_bit.no5 = 1;
  GPIODDEN_bit.no6 = 0;
  GPIODAMSEL_bit.no6 = 1;
  GPIODDEN_bit.no7 = 0;
  GPIODAMSEL_bit.no7 = 1;

  GPIOEDEN_bit.no4 = 0;
  GPIOEAMSEL_bit.no4 = 1;
  GPIOEDEN_bit.no5 = 0;
  GPIOEAMSEL_bit.no5 = 1;
  GPIOEDEN_bit.no6 = 0;
  GPIOEAMSEL_bit.no6 = 1;
  GPIOEDEN_bit.no7 = 0;
  GPIOEAMSEL_bit.no7 = 1;

  // Vref
  GPIOBDEN_bit.no6 = 0;
  GPIOBAMSEL_bit.no6 = 1;

  // Sample Sequencer Configuration
  // SS disable
  ADC0ACTSS_bit.ASEN0 = 0;

  // ADC Event Multiplexer Select
  ADC0EMUX_bit.EM0 = 0x0; // PWM0
  //  ADC Sample Averaging Control
  ADC0SAC_bit.AVG = 0;

  ADC0CTL |= EXTERNAL_REF;

  ADC0SSMUX0_bit.MUX0 = AIN15;
  ADC0SSMUX0_bit.MUX1 = AIN14;
  ADC0SSMUX0_bit.MUX2 = AIN7;
  ADC0SSMUX0_bit.MUX3 = AIN6;
  ADC0SSMUX0_bit.MUX4 = AIN5;
  ADC0SSMUX0_bit.MUX5 = AIN4;
  ADC0SSMUX0_bit.MUX6 = AIN3;
  ADC0SSMUX0_bit.MUX7 = AIN2;

  ADC0SSCTL0_bit.D0 = 0;   //  Single-ended input
  ADC0SSCTL0_bit.END0 = 0; //  Sample is End of Sequence
  ADC0SSCTL0_bit.IE0 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL0_bit.TS0 = 0;  //  Sample Temp Sensor Select

  ADC0SSCTL0_bit.D1 = 0;   //  Single-ended input
  ADC0SSCTL0_bit.END1 = 0; //  Sample is End of Sequence
  ADC0SSCTL0_bit.IE1 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL0_bit.TS1 = 0;  //  Sample Temp Sensor Select

  ADC0SSCTL0_bit.D2 = 0;   //  Single-ended input
  ADC0SSCTL0_bit.END2 = 0; //  Sample is End of Sequence
  ADC0SSCTL0_bit.IE2 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL0_bit.TS2 = 0;  //  Sample Temp Sensor Select

  ADC0SSCTL0_bit.D3 = 0;   //  Single-ended input
  ADC0SSCTL0_bit.END3 = 0; //  Sample is End of Sequence
  ADC0SSCTL0_bit.IE3 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL0_bit.TS3 = 0;  //  Sample Temp Sensor Select

  ADC0SSCTL0_bit.D4 = 0;   //  Single-ended input
  ADC0SSCTL0_bit.END4 = 0; //  Sample is End of Sequence
  ADC0SSCTL0_bit.IE4 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL0_bit.TS4 = 0;  //  Sample Temp Sensor Select

  ADC0SSCTL0_bit.D5 = 0;   //  Single-ended input
  ADC0SSCTL0_bit.END5 = 0; //  Sample is End of Sequence
  ADC0SSCTL0_bit.IE5 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL0_bit.TS5 = 0;  //  Sample Temp Sensor Select

  ADC0SSCTL0_bit.D6 = 0;   //  Single-ended input
  ADC0SSCTL0_bit.END6 = 0; //  Sample is End of Sequence
  ADC0SSCTL0_bit.IE6 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL0_bit.TS6 = 0;  //  Sample Temp Sensor Select

  ADC0SSCTL0_bit.D7 = 0;   //  Single-ended input
  ADC0SSCTL0_bit.END7 = 1; //  Sample is End of Sequence
  ADC0SSCTL0_bit.IE7 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL0_bit.TS7 = 0;  //  Sample Temp Sensor Select

  ADC0SSMUX1_bit.MUX0 = AIN1;
  ADC0SSMUX1_bit.MUX1 = AIN0;
  ADC0SSMUX1_bit.MUX2 = 0;

  ADC0SSCTL1_bit.D0 = 0;   //  Single-ended input
  ADC0SSCTL1_bit.END0 = 0; //  Sample is End of Sequence
  ADC0SSCTL1_bit.IE0 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL1_bit.TS0 = 0;  //  Sample Temp Sensor Select

  ADC0SSCTL1_bit.D1 = 0;   //  Single-ended input
  ADC0SSCTL1_bit.END1 = 0; //  Sample is End of Sequence
  ADC0SSCTL1_bit.IE1 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL1_bit.TS1 = 0;  //  Sample Temp Sensor Select

  ADC0SSCTL1_bit.D2 = 0;   //  Single-ended input
  ADC0SSCTL1_bit.END2 = 1; //  Sample is End of Sequence
  ADC0SSCTL1_bit.IE2 = 0;  //  Sample Interrupt Enable
  ADC0SSCTL1_bit.TS2 = 1;  //  Sample Temp Sensor Select

  // SS enable
  ADC0ACTSS_bit.ASEN0 = 1;
  ADC0ACTSS_bit.ASEN1 = 1;
  ADC0ISC_bit.IN0 = 1;

  m_adc_timer.start();
}

irs::arm::adc_t::~adc_t()
{
}

float irs::arm::adc_t::get_data(irs_u8 a_channel)
{
  if (a_channel >= m_channel_max_number) {
    return 0;
  } else if (a_channel == m_internal_temp_idx) {
    return get_temperature();
  } else {
    return static_cast<float>((mp_data[a_channel]*3.f)/1023.f);
  }
}

float irs::arm::adc_t::get_temperature()
{
  return static_cast<float>(
    147.5f - ((225.f*mp_data[m_internal_temp_idx])/1023.f)
  );
}

void irs::arm::adc_t::tick()
{
  switch(m_status)
  {
    case begin_ADC_sampling:
    {
      if (m_adc_timer.check()) {
        ADC0PSSI_bit.SS0 = 1;
        ADC0PSSI_bit.SS1 = 1;
        m_status = data_from_channel;
        m_adc_timer.start();
      }
    } break;
    case data_from_channel:
    {
      if (!ADC0SSFSTAT0_bit.EMPTY && !ADC0SSFSTAT1_bit.EMPTY) {
        for (irs_u8 cnt = 0; cnt < m_SSI0_channel_max_number; cnt++) {
          mp_data[cnt] = ADC0SSFIFO0;
        }
        if (!ADC0SSFSTAT0_bit.EMPTY) {
          volatile irs_u32 fifo_data = ADC0SSFIFO0;
          while(!ADC0SSFSTAT0_bit.EMPTY) {
            fifo_data = ADC0SSFIFO0;
          }
        }
        for (irs_u8 cnt = m_SSI0_channel_max_number;
          cnt < m_channel_max_number; cnt++)
        {
          mp_data[cnt] = ADC0SSFIFO1;
        }
        if (!ADC0SSFSTAT1_bit.EMPTY) {
          volatile irs_u32 fifo_data = ADC0SSFIFO1;
          while(!ADC0SSFSTAT1_bit.EMPTY) {
            fifo_data = ADC0SSFIFO1;
          }
        }
        ADC0ISC_bit.IN0 = 1;
        m_status = begin_ADC_sampling;
      }
    } break;
  }
}
#endif  //  NOP

irs::arm::adc_stellaris_t::adc_stellaris_t(
  select_channel_type a_selected_channels,
  adc_ref_t a_adc_ref,
  adc_module_t a_adc_module,
  counter_t a_adc_interval):

  m_status(READ_SEQ_0),
  m_adc_module(a_adc_module),
  m_adc_timer(a_adc_interval),
  m_result_vector(),
  m_num_of_channels(1)  //  Termosensor - есть всегда
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
      ADC0SAC = 0;
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
      ADC1SAC = 0;
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
          ctl_reg |= (1 << (m_num_of_channels * 4 + 1)); //  End of Sequence
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
          ctl_reg |= (1 << (m_num_of_channels * 4 + 1)); //  End of Sequence
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
          ctl_reg |= (1 << (m_num_of_channels * 4 + 1)); //  End of Sequence
        }
        set_mux_and_ctl_regs(mux_reg, ctl_reg, SEQ2);
      }
      else
      {
        //  m_num_of_channels = 16
        mux_reg = i;
        ctl_reg = 1;
        set_mux_and_ctl_regs(mux_reg, ctl_reg, SEQ3);
      }
      m_num_of_channels++;
    }
  }
  if (m_num_of_channels <= 1)
  {
    switch (m_adc_module)
    {
      case ADC0: ADC0SSCTL0_bit.END0 = 1; break;
      case ADC1: ADC1SSCTL0_bit.END0 = 1; break;
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
    case ADC0: return ADC0RIS & (1 << a_seq);
    case ADC1: return ADC1RIS & (1 << a_seq);
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
        GPIOBDEN_bit.no6 = 1;
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
        GPIOBDEN_bit.no6 = 1;
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
      break;
    }
    case READ_SEQ_1:
    {
      if (check_sequence_ready(SEQ1))
      {
        for (irs_u8 i = 0; i < m_sequence1_size; i++)
        {
          if (i >= m_num_of_channels) break;
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
          if (i >= m_num_of_channels) break;
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
