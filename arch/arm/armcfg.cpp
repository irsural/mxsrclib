//! \file
//! \ingroup system_utils_group
//! \brief ������������ ����������
//!
//! ����: 30.04.2012\n
//! ���� ��������: 17.05.2010

#include <irspch.h>

#include <armcfg.h>
#include <armioregs.h>
#include <irscpu.h>

#include <irsfinal.h>

void pll_on()
{
#if defined(__LM3Sx9xx__)
  LDOPCTL_bit.VADJ = 0x1B;  //  V = 2.75 V
  RCC_bit.BYPASS = 1;   //  Clocking from OSC source
  RCC_bit.USESYS = 0;   //  Disable System Clock Divider
  RCC_bit.MOSCDIS = 0;  //  Enable main osc
  RCC_bit.XTAL = 0xE;   //  Crystal 8 MHz
  RCC_bit.OSCSRC = 0x0; //  Main osc
  RCC_bit.PWRDN = 0;    //  Power on PLL
  RCC_bit.SYSDIV = 0x3; //  to 50 MHz
  RCC_bit.USESYS = 1;   //  Enable System Clock Divider
  while (!RIS_bit.PLLLRIS); //  Wait for PLL lock
  RCC_bit.BYPASS = 0;   //  Clocking from PLL source
  irs::cpu_traits_t::frequency(50000000);
#elif defined(__LM3SxBxx__)
  RCC2_bit.USERCC2 = 1; // Use RCC2 bit field
  RCC2_bit.DIV400 = 1;
  RCC2_bit.SYSDIV2LSB = 0;
  RCC_bit.BYPASS = 1;   //  Clocking from OSC source
  RCC_bit.USESYS = 0;   //  Disable System Clock Divider
  RCC_bit.XTAL = 0xE;   //  Crystal 8 MHz
  RCC2_bit.OSCSRC2 = 0x0; //  Main osc
  RCC2_bit.PWRDN2 = 0;    //  Power on PLL
  RCC2_bit.SYSDIV2 = 0x2; //  to 80 MHz
  RCC_bit.USESYS = 1;   //  Enable System Clock Divider
  while (!RIS_bit.PLLLRIS); //  Wait for PLL lock
  RCC2_bit.BYPASS2 = 0;   //  Clocking from PLL source
  irs::cpu_traits_t::frequency(80000000);
#elif defined(__STM32F100RBT__)
  RCC_CR_bit.HSEON = 1;
  while (!RCC_CR_bit.HSERDY);
  RCC_CFGR2_bit.PREDIV1 = 0;
  RCC_CFGR_bit.PLLMUL = 0x1;  //  = x3
  RCC_CFGR_bit.PLLSRC = 1;    //  HSE
  RCC_CR_bit.PLLON = 1;
  while (!RCC_CR_bit.PLLRDY);
  RCC_CFGR_bit.SW = 0x2;      //  SRC = PLL
  irs::cpu_traits_t::frequency(24000000);
#elif defined(IRS_STM32F2xx)
  RCC_CR_bit.HSEON = 1;
  RCC_CR_bit.HSEBYP = 0;
  while (!RCC_CR_bit.HSERDY);

  // �� ����� ����� 25 ���
  RCC_PLLCFGR_bit.PLLM = 20; // �������� �� ����� PLL
  RCC_PLLCFGR_bit.PLLN = 192; // ��������� ������ PLL
  RCC_PLLCFGR_bit.PLLP = 0; // 0 ��� ������� �� 2, �������� ��� ���� 120 ���
  RCC_PLLCFGR_bit.PLLQ = 5; // �������� ��� USB 48 ���
  // Main PLL(PLL) and audio PLL (PLLI2S) entry clock source.
  // HSE oscillator clock selected as PLL and PLLI2S clock entry
  RCC_PLLCFGR_bit.PLLSRC = 1;
  // AHB prescaler.system clock not divided
  RCC_CFGR_bit.HPRE = 0;
  // APB Low speed prescaler (APB1). 101: AHB clock divided by 4
  RCC_CFGR_bit.PPRE1 = 5;
  // APB high-speed prescaler (APB2). 101: 100: AHB clock divided by 2
  RCC_CFGR_bit.PPRE2 = 4;
  // Main PLL (PLL) enable. PLL ON
  RCC_CR_bit.PLLON = 1;
  while (!RCC_CR_bit.PLLRDY);

  FLASH_ACR_bit.LATENCY = 3;
  while (FLASH_ACR_bit.LATENCY != 3);
  RCC_CFGR_bit.SW = 2;
  RCC_CFGR_bit.HPRE = 0;
  while (RCC_CFGR_bit.SWS != 2);
  while (RCC_CFGR_bit.HPRE != 0);
  irs::cpu_traits_t::periphery_frequency_first(30000000);
  irs::cpu_traits_t::periphery_frequency_second(60000000);
  irs::cpu_traits_t::frequency(120000000);
#else
  #error ��� ����������� �� ��������
#endif // ARM_devices
}

#ifdef IRS_STM32F2xx
size_t irs::get_port_address(gpio_channel_t a_gpio_channel)
{
  if (a_gpio_channel == PNONE) {
    IRS_LIB_ERROR(irs::ec_standard, "������������ �����");
  }
  const size_t port_pin_count = 16;
  const size_t port_address_delta = PORTB_BASE - PORTA_BASE;
  return PORTA_BASE + port_address_delta*(a_gpio_channel/port_pin_count);
}

size_t irs::get_pin_index(gpio_channel_t a_gpio_channel)
{
  if (a_gpio_channel == PNONE) {
    IRS_LIB_ERROR(irs::ec_standard, "������������ �����");
  }
  const size_t port_pin_count = 16;
  return a_gpio_channel%port_pin_count;
}

void irs::reset_peripheral(size_t a_address)
{
  switch (a_address) {
    case TIM1_PWM1_BASE: {
      RCC_APB2RSTR_bit.TIM1RST = 1;
      RCC_APB2RSTR_bit.TIM1RST = 0;
    } break;
    case TIM2_BASE: {
      RCC_APB1RSTR_bit.TIM2RST = 1;
      RCC_APB1RSTR_bit.TIM2RST = 0;
    } break;
    case TIM3_BASE: {
      RCC_APB1RSTR_bit.TIM3RST = 1;
      RCC_APB1RSTR_bit.TIM3RST = 0;
    } break;
    case TIM4_BASE: {
      RCC_APB1RSTR_bit.TIM4RST = 1;
      RCC_APB1RSTR_bit.TIM4RST = 0;
    } break;
    case TIM5_BASE: {
      RCC_APB1RSTR_bit.TIM5RST = 1;
      RCC_APB1RSTR_bit.TIM5RST = 0;
    } break;
    case TIM6_BASE: {
      RCC_APB1RSTR_bit.TIM6RST = 1;
      RCC_APB1RSTR_bit.TIM6RST = 0;
    } break;
    case TIM7_BASE: {
      RCC_APB1RSTR_bit.TIM7RST = 1;
      RCC_APB1RSTR_bit.TIM7RST = 0;
    } break;
    case TIM8_PWM2_BASE: {
      RCC_APB2RSTR_bit.TIM8RST = 1;
      RCC_APB2RSTR_bit.TIM8RST = 0;
    } break;
    case TIM9_BASE: {
      RCC_APB2RSTR_bit.TIM9RST = 1;
      RCC_APB2RSTR_bit.TIM9RST = 0;
    } break;
    case TIM10_BASE: {
      RCC_APB2RSTR_bit.TIM10RST = 1;
      RCC_APB2RSTR_bit.TIM10RST = 0;
    } break;
    case TIM11_BASE:{
      RCC_APB2RSTR_bit.TIM11RST = 1;
      RCC_APB2RSTR_bit.TIM11RST = 0;
    } break;
    case TIM12_BASE: {
      RCC_APB1RSTR_bit.TIM12RST = 1;
      RCC_APB1RSTR_bit.TIM12RST = 0;
    } break;
    case TIM13_BASE: {
      RCC_APB1RSTR_bit.TIM13RST = 1;
      RCC_APB1RSTR_bit.TIM13RST = 0;
    } break;
    case TIM14_BASE: {
      RCC_APB1RSTR_bit.TIM14RST = 1;
      RCC_APB1RSTR_bit.TIM14RST = 0;
    } break;
    case SPI1_BASE: {
      RCC_APB2RSTR_bit.SPI1RST = 1;
      RCC_APB2RSTR_bit.SPI1RST = 0;
    } break;
    case SPI2_I2S2_BASE: {
      RCC_APB1RSTR_bit.SPI2RST = 1;
      RCC_APB1RSTR_bit.SPI2RST = 0;
    } break;
    case SPI3_I2S3_BASE: {
      RCC_APB1RSTR_bit.SPI3RST = 1;
      RCC_APB1RSTR_bit.SPI3RST = 0;
    } break;
    /*case ADC1_ADC2_ADC3_BASE: {
      RCC_APB2RSTR.ADCRST = 1;
      RCC_APB2RSTR.ADCRST = 0;
    } break;*/
    case DAC1_DAC2_BASE: {
      RCC_APB1RSTR_bit.DACRST = 1;
      RCC_APB1RSTR_bit.DACRST = 0;
    } break;
    default : {
      IRS_ASSERT_MSG("����� ��� ���������� ���������� �� ���������");
    }
  }
}

void irs::clock_enable(gpio_channel_t a_channel)
{
  clock_enable(get_port_address(a_channel));
}

void irs::clock_disable(gpio_channel_t a_channel)
{
  clock_disable(get_port_address(a_channel));
}

void irs::clock_enable(size_t a_address)
{
  clock_enabled(a_address, true);
}

void irs::clock_disable(size_t a_address)
{
  clock_enabled(a_address, false);
}

void irs::clock_enabled(size_t a_address, bool a_enabled)
{
  const irs_u32 value = a_enabled ? 1 : 0;
  switch (a_address) {
    case PORTA_BASE: {
      RCC_AHB1ENR_bit.GPIOAEN = value;
    } break;
    case PORTB_BASE: {
      RCC_AHB1ENR_bit.GPIOBEN = value;
    } break;
    case PORTC_BASE: {
      RCC_AHB1ENR_bit.GPIOCEN = value;
    } break;
    case PORTD_BASE: {
      RCC_AHB1ENR_bit.GPIODEN = value;
    } break;
    case PORTE_BASE: {
      RCC_AHB1ENR_bit.GPIOEEN = value;
    } break;
    case PORTF_BASE: {
      RCC_AHB1ENR_bit.GPIOFEN = value;
    } break;
    case PORTG_BASE: {
      RCC_AHB1ENR_bit.GPIOGEN = value;
    } break;
    case PORTH_BASE: {
      RCC_AHB1ENR_bit.GPIOHEN = value;
    } break;
    case PORTI_BASE: {
      RCC_AHB1ENR_bit.GPIOIEN = value;
    } break;
    case USART1_BASE: {
      RCC_APB2ENR_bit.USART1EN = value;
    } break;
    case USART2_BASE: {
      RCC_APB1ENR_bit.USART2EN = value;
    } break;
    case USART3_BASE: {
      RCC_APB1ENR_bit.USART3EN = value;
    } break;
    case UART4_BASE: {
      RCC_APB1ENR_bit.UART4EN = value;
    } break;
    case UART5_BASE: {
      RCC_APB1ENR_bit.UART5EN = value;
    } break;
    case USART6_BASE: {
      RCC_APB2ENR_bit.USART6EN = value;
    } break;
    case TIM1_PWM1_BASE: {
      RCC_APB2ENR_bit.TIM1EN = value;
    } break;
    case TIM2_BASE: {
      RCC_APB1ENR_bit.TIM2EN = value;
    } break;
    case TIM3_BASE: {
      RCC_APB1ENR_bit.TIM3EN = value;
    } break;
    case TIM4_BASE: {
      RCC_APB1ENR_bit.TIM4EN = value;
    } break;
    case TIM5_BASE: {
      RCC_APB1ENR_bit.TIM5EN = value;
    } break;
    case TIM6_BASE: {
      RCC_APB1ENR_bit.TIM6EN = value;
    } break;
    case TIM7_BASE: {
      RCC_APB1ENR_bit.TIM7EN = value;
    } break;
    case TIM8_PWM2_BASE: {
      RCC_APB2ENR_bit.TIM8EN = value;
    } break;
    case TIM9_BASE: {
      RCC_APB2ENR_bit.TIM9EN = value;
    } break;
    case TIM10_BASE: {
      RCC_APB2ENR_bit.TIM10EN = value;
    } break;
    case TIM11_BASE:{
      RCC_APB2ENR_bit.TIM11EN = value;
    } break;
    case TIM12_BASE: {
      RCC_APB1ENR_bit.TIM12EN = value;
    } break;
    case TIM13_BASE: {
      RCC_APB1ENR_bit.TIM13EN = value;
    } break;
    case TIM14_BASE: {
      RCC_APB1ENR_bit.TIM14EN = value;
    } break;
    case SPI1_BASE: {
      RCC_APB2ENR_bit.SPI1EN = value;
    } break;
    case SPI2_I2S2_BASE: {
      RCC_APB1ENR_bit.SPI2EN = value;
    } break;
    case SPI3_I2S3_BASE: {
      RCC_APB1ENR_bit.SPI3EN = value;
    } break;
    case ADC1_BASE: {
      RCC_APB2ENR_bit.ADC1EN = value;
    } break;
    case ADC2_BASE: {
      RCC_APB2ENR_bit.ADC2EN = value;
    } break;
    case ADC3_BASE: {
      RCC_APB2ENR_bit.ADC3EN = value;
    } break;
    case DAC1_DAC2_BASE: {
      RCC_APB1ENR_bit.DACEN = value;
    } break;
    default : {
      IRS_ASSERT_MSG("���������/���������� ��� ���������� "
        "���������� �� ����������");
    }
  }
}

namespace {

void gpio_set_bits(gpio_channel_t a_channel, int gpio_bits_group,
  int gpio_bits_value)
{
  const size_t port_address = irs::get_port_address(a_channel);
  const int pin_index = irs::get_pin_index(a_channel);
  const int bits_count = 2;
  IRS_SET_BITS(port_address + gpio_bits_group, bits_count*pin_index,
    bits_count, gpio_bits_value);
}

} // empty namespace

void irs::gpio_moder_alternate_function_enable(gpio_channel_t a_channel)
{
  /*const size_t port_address = get_port_address(a_channel);
  const int pin_index = get_pin_index(a_channel);
  const int bits_count = 2;
  IRS_SET_BITS(port_address + GPIO_MODER_S, bits_count*pin_index,
    bits_count, GPIO_MODER_ALTERNATE_FUNCTION);*/
  gpio_set_bits(a_channel, GPIO_MODER_S, GPIO_MODER_ALTERNATE_FUNCTION);
}

void irs::gpio_moder_analog_enable(gpio_channel_t a_channel)
{
  gpio_set_bits(a_channel, GPIO_MODER_S, GPIO_MODER_ANALOG);
}

void irs::gpio_moder_input_enable(gpio_channel_t a_channel)
{
  gpio_set_bits(a_channel, GPIO_MODER_S, GPIO_MODER_INPUT);
}

void irs::update_interrupt_enable(size_t a_address)
{
  update_interrupt_enabled(a_address, true);
}

void irs::update_interrupt_enabled(size_t a_address, bool a_enabled)
{
  const irs_u32 value = a_enabled ? 1 : 0;
  switch (a_address) {
    case TIM1_PWM1_BASE: {
      SETENA0_bit.SETENA_TIM1_UP_TIM10 = value;
    } break;
    /*case TIM2_BASE: {

    } break;*/
    case TIM3_BASE: {
      SETENA0_bit.SETENA_TIM3 = value;
    } break;
    case TIM4_BASE: {
      SETENA0_bit.SETENA_TIM4 = value;
    } break;
    /*case TIM5_BASE: {

    } break;
    case TIM6_BASE: {

    } break;
    case TIM7_BASE: {

    } break;*/
    case TIM8_PWM2_BASE: {
      SETENA1_bit.SETENA_TIM8_UP_TIM13 = value;
    } break;
    /*case TIM9_BASE: {

    } break;
    case TIM10_BASE: {

    } break;
    case TIM11_BASE:{

    } break;
    case TIM12_BASE: {

    } break;
    case TIM13_BASE: {
      SETENA1_bit.SETENA_TIM8_UP_TIM13 = value;
    } break;
    case TIM14_BASE: {

    } break;*/
    default : {
      IRS_ASSERT_MSG("���������/���������� ���������� ��� ���������� "
        "���������� �� ����������");
    }
  }
}

#endif // IRS_STM32F2xx
