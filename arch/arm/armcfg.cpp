//! \file
//! \ingroup system_utils_group
//! \brief Конфигурация процессора
//!
//! Дата: 30.04.2012\n
//! Дата создания: 17.05.2010

#include <irspch.h>

#include <armcfg.h>
#include <armioregs.h>
#include <irscpu.h>

#include <irsfinal.h>
#define F7_216Hz 1
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
#elif defined(IRS_STM32_F2_F4_F7)
  irs::param_pll_t param_pll;
  #if defined(IRS_STM32F2xx)
    // На входе кварц 25 МГц
    param_pll.freq_quartz = 25000000;
    param_pll.PLLM = 20; // Делитель на входе PLL
    param_pll.PLLN = 192; // Множитель внутри PLL
    param_pll.PLLP = 0; // 0 это деление на 2, делитель для ядра 120 МГц
    param_pll.PLLQ = 5; // Делитель для USB 48 МГц
    // APB Low speed prescaler (APB1). 101: AHB clock divided by 4
    param_pll.PPRE1 = 5;
    // APB high-speed prescaler (APB2). 101: 100: AHB clock divided by 2
    param_pll.PPRE2 = 4;
    param_pll.FLASH_STATE = 3;
    param_pll.HSEBYP = 0; //HSE clock bypass External crystal/ceramic resonator
    irs::pll_on(param_pll);
  #elif defined(IRS_STM32F4xx)
    // На входе кварц 25 МГц
    param_pll.freq_quartz = 25000000;
    param_pll.PLLM = 25; // Делитель на входе PLL
    param_pll.PLLN = 336; // Множитель внутри PLL
    param_pll.PLLP = 0; // 0 это деление на 2, делитель для ядра 168 МГц
    param_pll.PLLQ = 7; // Делитель для USB 48 МГц
    // APB Low speed prescaler (APB1). 101: AHB clock divided by 4
    param_pll.PPRE1 = 5;
    // APB high-speed prescaler (APB2). 100: AHB clock divided by 2
    param_pll.PPRE2 = 4;
    param_pll.FLASH_STATE = 5;
    param_pll.HSEBYP = 0; //HSE clock bypass External crystal/ceramic resonator
    irs::pll_on(param_pll);
  #elif defined(IRS_STM32F7xx)
    // Это непроверенный код, скопирован из секции для IRS_STM32F4xx !!!!!!
    // На входе кварц 25 МГц
#if !F7_216Hz
    param_pll.freq_quartz = 25000000;
    param_pll.PLLM = 25; // Делитель на входе PLL
    param_pll.PLLN = 336; // Множитель внутри PLL
    param_pll.PLLP = 0; // 0 это деление на 2, делитель для ядра 168 МГц
    param_pll.PLLQ = 7; // Делитель для USB 48 МГц
    // APB Low speed prescaler (APB1). 101: AHB clock divided by 4
    param_pll.PPRE1 = 5;
    // APB high-speed prescaler (APB2). 100: AHB clock divided by 2
    param_pll.PPRE2 = 4;
    param_pll.FLASH_STATE = 5;
    param_pll.HSEBYP = 0; //HSE clock bypass External crystal/ceramic resonator
    irs::pll_on(param_pll);
#else
    param_pll.freq_quartz = 25000000;
    param_pll.PLLM = 25; // Делитель на входе PLL
    param_pll.PLLN = 432; // Множитель внутри PLL
    param_pll.PLLP = 0; // 0 это деление на 2, делитель для ядра 216 МГц
    param_pll.PLLQ = 9; // Делитель для USB 48 МГц
    // APB Low speed prescaler (APB1). 110: AHB clock divided by 4
    param_pll.PPRE1 = 5;//6;
    // APB high-speed prescaler (APB2). 101: AHB clock divided by 2
    param_pll.PPRE2 = 4;//5;
    param_pll.FLASH_STATE = 7;
    param_pll.HSEBYP = 0; //HSE clock bypass External crystal/ceramic resonator

    // Не хватает включения overdrive режима 
    
    
    // PLLI2SN[8:0] = 192 (x192)По умолчанию
    // PLLI2SP[1:0] = 00: (/2) По умолчанию
    // PLLI2SQ[0:3] = 0100: (/4) По умолчанию
    // PLLI2SR[2:0]: = 010: (/2) По умолчанию

    irs::pll_on(param_pll);
#endif
  #else
    #error Тип контроллера не определён
  #endif //ARM_devices
#else
  #error Тип контроллера не определён
#endif // ARM_devices
}

void irs::pll_on()
{
  pll_on();
}

#ifdef IRS_STM32_F2_F4_F7
void irs::pll_on(param_pll_t a_param_pll)
{
  RCC_CR_bit.HSEON = 1;
  RCC_CR_bit.HSEBYP = a_param_pll.HSEBYP;
  while (!RCC_CR_bit.HSERDY);

  #if defined(IRS_STM32F4xx)
  RCC_CFGR_bit.MCO2 = 3; //PLL clock selected
  RCC_CFGR_bit.MCO2PRE = 5; //division by 3
  #endif //ARM_devices

  RCC_PLLCFGR_bit.PLLM = a_param_pll.PLLM; // Делитель на входе PLL
  RCC_PLLCFGR_bit.PLLN = a_param_pll.PLLN; // Множитель внутри PLL
  RCC_PLLCFGR_bit.PLLP = a_param_pll.PLLP; // 0 это деление на 2
  RCC_PLLCFGR_bit.PLLQ = a_param_pll.PLLQ; // Делитель для USB 48 МГц
  // Main PLL(PLL) and audio PLL (PLLI2S) entry clock source.
  // HSE oscillator clock selected as PLL and PLLI2S clock entry
  RCC_PLLCFGR_bit.PLLSRC = 1;
  // AHB prescaler.system clock not divided
  RCC_CFGR_bit.HPRE = 0;
  // APB Low speed prescaler (APB1).
  RCC_CFGR_bit.PPRE1 = a_param_pll.PPRE1;
  // APB high-speed prescaler (APB2).
  RCC_CFGR_bit.PPRE2 = a_param_pll.PPRE2;
  // Main PLL (PLL) enable. PLL ON
  RCC_CR_bit.PLLON = 1;
  while (!RCC_CR_bit.PLLRDY);
  //FLASH_STATE
  FLASH_ACR_bit.LATENCY = a_param_pll.FLASH_STATE;
  while (FLASH_ACR_bit.LATENCY != a_param_pll.FLASH_STATE);
  // 1: Data cache is enabled
  FLASH_ACR_bit.DCEN = 1;
  // 1: Instruction cache is enabled
  FLASH_ACR_bit.ICEN = 1;
  // 0: Prefetch is disabled
  FLASH_ACR_bit.PRFTEN = 0;
  RCC_CFGR_bit.SW = 2;
  RCC_CFGR_bit.HPRE = 0;
  while (RCC_CFGR_bit.SWS != 2);
  while (RCC_CFGR_bit.HPRE != 0);
  irs_u32 freq = static_cast<irs_u32>(a_param_pll.freq_quartz/a_param_pll.PLLM*
    a_param_pll.PLLN/pow(2.,(a_param_pll.PLLP+1)));
  irs::cpu_traits_t::frequency(static_cast<cpu_traits_t::frequency_type>(freq));

  irs_u32 divider = 2;
  switch (a_param_pll.PPRE1) {
    case 4: {divider = 2;} break;
    case 5: {divider = 4;} break;
    case 6: {divider = 8;} break;
    case 7: {divider = 16;} break;
    default: IRS_LIB_ASSERT_MSG("Задан недопустимый делитель APB1");
  }
  irs::cpu_traits_t::periphery_frequency_first(
    static_cast<cpu_traits_t::frequency_type>(freq/divider));

  switch (a_param_pll.PPRE2) {
    case 4: {divider = 2;} break;
    case 5: {divider = 4;} break;
    case 6: {divider = 8;} break;
    case 7: {divider = 16;} break;
    default: IRS_LIB_ASSERT_MSG("Задан недопустимый делитель APB2");
  }
  irs::cpu_traits_t::periphery_frequency_second(
    static_cast<cpu_traits_t::frequency_type>(freq/divider));
  irs::cpu_traits_t::flash_voltage(3.3);
}

size_t irs::get_port_address(gpio_channel_t a_gpio_channel)
{
  if (a_gpio_channel == PNONE) {
    IRS_LIB_ERROR(irs::ec_standard, "Недопустимый канал");
  }
  const size_t port_pin_count = 16;
  const size_t port_address_delta = IRS_PORTB_BASE - IRS_PORTA_BASE;
  return IRS_PORTA_BASE + port_address_delta*(a_gpio_channel/port_pin_count);
}

size_t irs::get_pin_index(gpio_channel_t a_gpio_channel)
{
  if (a_gpio_channel == PNONE) {
    IRS_LIB_ERROR(irs::ec_standard, "Недопустимый канал");
  }
  const size_t port_pin_count = 16;
  return a_gpio_channel%port_pin_count;
}

void irs::reset_peripheral(size_t a_address)
{
  switch (a_address) {
    case IRS_WWDG_BASE: {
      RCC_APB1RSTR_bit.WWDGRST = 1;
      RCC_APB1RSTR_bit.WWDGRST = 0;
    } break;
    case IRS_TIM1_PWM1_BASE: {
      RCC_APB2RSTR_bit.TIM1RST = 1;
      RCC_APB2RSTR_bit.TIM1RST = 0;
    } break;
    case IRS_TIM2_BASE: {
      RCC_APB1RSTR_bit.TIM2RST = 1;
      RCC_APB1RSTR_bit.TIM2RST = 0;
    } break;
    case IRS_TIM3_BASE: {
      RCC_APB1RSTR_bit.TIM3RST = 1;
      RCC_APB1RSTR_bit.TIM3RST = 0;
    } break;
    case IRS_TIM4_BASE: {
      RCC_APB1RSTR_bit.TIM4RST = 1;
      RCC_APB1RSTR_bit.TIM4RST = 0;
    } break;
    case IRS_TIM5_BASE: {
      RCC_APB1RSTR_bit.TIM5RST = 1;
      RCC_APB1RSTR_bit.TIM5RST = 0;
    } break;
    case IRS_TIM6_BASE: {
      RCC_APB1RSTR_bit.TIM6RST = 1;
      RCC_APB1RSTR_bit.TIM6RST = 0;
    } break;
    case IRS_TIM7_BASE: {
      RCC_APB1RSTR_bit.TIM7RST = 1;
      RCC_APB1RSTR_bit.TIM7RST = 0;
    } break;
    case IRS_TIM8_PWM2_BASE: {
      RCC_APB2RSTR_bit.TIM8RST = 1;
      RCC_APB2RSTR_bit.TIM8RST = 0;
    } break;
    case IRS_TIM9_BASE: {
      RCC_APB2RSTR_bit.TIM9RST = 1;
      RCC_APB2RSTR_bit.TIM9RST = 0;
    } break;
    case IRS_TIM10_BASE: {
      RCC_APB2RSTR_bit.TIM10RST = 1;
      RCC_APB2RSTR_bit.TIM10RST = 0;
    } break;
    case IRS_TIM11_BASE:{
      RCC_APB2RSTR_bit.TIM11RST = 1;
      RCC_APB2RSTR_bit.TIM11RST = 0;
    } break;
    case IRS_TIM12_BASE: {
      RCC_APB1RSTR_bit.TIM12RST = 1;
      RCC_APB1RSTR_bit.TIM12RST = 0;
    } break;
    case IRS_TIM13_BASE: {
      RCC_APB1RSTR_bit.TIM13RST = 1;
      RCC_APB1RSTR_bit.TIM13RST = 0;
    } break;
    case IRS_TIM14_BASE: {
      RCC_APB1RSTR_bit.TIM14RST = 1;
      RCC_APB1RSTR_bit.TIM14RST = 0;
    } break;
    case IRS_SPI1_BASE: {
      RCC_APB2RSTR_bit.SPI1RST = 1;
      RCC_APB2RSTR_bit.SPI1RST = 0;
    } break;
    case IRS_SPI2_I2S2_BASE: {
      RCC_APB1RSTR_bit.SPI2RST = 1;
      RCC_APB1RSTR_bit.SPI2RST = 0;
    } break;
    case IRS_SPI3_I2S3_BASE: {
      RCC_APB1RSTR_bit.SPI3RST = 1;
      RCC_APB1RSTR_bit.SPI3RST = 0;
    } break;
    /*case ADC1_ADC2_ADC3_BASE: {
      RCC_APB2RSTR.ADCRST = 1;
      RCC_APB2RSTR.ADCRST = 0;
    } break;*/
    case IRS_DAC1_DAC2_BASE: {
      RCC_APB1RSTR_bit.DACRST = 1;
      RCC_APB1RSTR_bit.DACRST = 0;
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("Сброс для указанного устройства не определен");
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
    case IRS_PORTA_BASE: {
      RCC_AHB1ENR_bit.GPIOAEN = value;
    } break;
    case IRS_PORTB_BASE: {
      RCC_AHB1ENR_bit.GPIOBEN = value;
    } break;
    case IRS_PORTC_BASE: {
      RCC_AHB1ENR_bit.GPIOCEN = value;
    } break;
    case IRS_PORTD_BASE: {
      RCC_AHB1ENR_bit.GPIODEN = value;
    } break;
    case IRS_PORTE_BASE: {
      RCC_AHB1ENR_bit.GPIOEEN = value;
    } break;
    case IRS_PORTF_BASE: {
      RCC_AHB1ENR_bit.GPIOFEN = value;
    } break;
    case IRS_PORTG_BASE: {
      RCC_AHB1ENR_bit.GPIOGEN = value;
    } break;
    case IRS_PORTH_BASE: {
      RCC_AHB1ENR_bit.GPIOHEN = value;
    } break;
    case IRS_PORTI_BASE: {
      RCC_AHB1ENR_bit.GPIOIEN = value;
    } break;
    case IRS_USART1_BASE: {
      RCC_APB2ENR_bit.USART1EN = value;
    } break;
    case IRS_USART2_BASE: {
      RCC_APB1ENR_bit.USART2EN = value;
    } break;
    case IRS_USART3_BASE: {
      RCC_APB1ENR_bit.USART3EN = value;
    } break;
    case IRS_UART4_BASE: {
      RCC_APB1ENR_bit.UART4EN = value;
    } break;
    case IRS_UART5_BASE: {
      RCC_APB1ENR_bit.UART5EN = value;
    } break;
    case IRS_USART6_BASE: {
      RCC_APB2ENR_bit.USART6EN = value;
    } break;
    case IRS_WWDG_BASE: {
      RCC_APB1ENR_bit.WWDGEN = 1;
    } break;
    case IRS_TIM1_PWM1_BASE: {
      RCC_APB2ENR_bit.TIM1EN = value;
    } break;
    case IRS_TIM2_BASE: {
      RCC_APB1ENR_bit.TIM2EN = value;
    } break;
    case IRS_TIM3_BASE: {
      RCC_APB1ENR_bit.TIM3EN = value;
    } break;
    case IRS_TIM4_BASE: {
      RCC_APB1ENR_bit.TIM4EN = value;
    } break;
    case IRS_TIM5_BASE: {
      RCC_APB1ENR_bit.TIM5EN = value;
    } break;
    case IRS_TIM6_BASE: {
      RCC_APB1ENR_bit.TIM6EN = value;
    } break;
    case IRS_TIM7_BASE: {
      RCC_APB1ENR_bit.TIM7EN = value;
    } break;
    case IRS_TIM8_PWM2_BASE: {
      RCC_APB2ENR_bit.TIM8EN = value;
    } break;
    case IRS_TIM9_BASE: {
      RCC_APB2ENR_bit.TIM9EN = value;
    } break;
    case IRS_TIM10_BASE: {
      RCC_APB2ENR_bit.TIM10EN = value;
    } break;
    case IRS_TIM11_BASE:{
      RCC_APB2ENR_bit.TIM11EN = value;
    } break;
    case IRS_TIM12_BASE: {
      RCC_APB1ENR_bit.TIM12EN = value;
    } break;
    case IRS_TIM13_BASE: {
      RCC_APB1ENR_bit.TIM13EN = value;
    } break;
    case IRS_TIM14_BASE: {
      RCC_APB1ENR_bit.TIM14EN = value;
    } break;
    case IRS_SPI1_BASE: {
      RCC_APB2ENR_bit.SPI1EN = value;
    } break;
    case IRS_SPI2_I2S2_BASE: {
      RCC_APB1ENR_bit.SPI2EN = value;
    } break;
    case IRS_SPI3_I2S3_BASE: {
      RCC_APB1ENR_bit.SPI3EN = value;
    } break;
    /*case IRS_SPI6_BASE: {
      RCC_APB2ENR_bit.SPI6EN = value;
    } break;*/
    case IRS_ADC1_BASE: {
      RCC_APB2ENR_bit.ADC1EN = value;
    } break;
    case IRS_ADC2_BASE: {
      RCC_APB2ENR_bit.ADC2EN = value;
    } break;
    case IRS_ADC3_BASE: {
      RCC_APB2ENR_bit.ADC3EN = value;
    } break;
    case IRS_DAC1_DAC2_BASE: {
      RCC_APB1ENR_bit.DACEN = value;
    } break;
    case IRS_DMA1_BASE: {
      RCC_AHB1ENR_bit.DMA1EN = value;
    } break;
    case IRS_DMA2_BASE: {
      RCC_AHB1ENR_bit.DMA2EN = value;
    } break;
    default : {
      IRS_LIB_ASSERT_MSG("Включение/отключение для указанного "
        "устройства не определено");
    }
  }
}

namespace {

void gpio_set_bits(gpio_channel_t a_channel, int a_bits_count,
  int a_gpio_bits_group, int a_gpio_bits_value)
{
  const size_t port_address = irs::get_port_address(a_channel);
  const int pin_index = irs::get_pin_index(a_channel);
  //const int bits_count = 2;
  IRS_SET_BITS(port_address + a_gpio_bits_group, a_bits_count*pin_index,
    a_bits_count, a_gpio_bits_value);
}

} // empty namespace

void irs::gpio_moder_alternate_function_enable(gpio_channel_t a_channel)
{
  /*const size_t port_address = get_port_address(a_channel);
  const int pin_index = get_pin_index(a_channel);
  const int bits_count = 2;
  IRS_SET_BITS(port_address + GPIO_MODER_S, bits_count*pin_index,
    bits_count, GPIO_MODER_ALTERNATE_FUNCTION);*/
  const int bits_count = 2;
  gpio_set_bits(a_channel, bits_count, GPIO_MODER_S,
    GPIO_MODER_ALTERNATE_FUNCTION);
}

void irs::gpio_moder_analog_enable(gpio_channel_t a_channel)
{
  const int bits_count = 2;
  gpio_set_bits(a_channel, bits_count, GPIO_MODER_S, GPIO_MODER_ANALOG);
}

void irs::gpio_moder_input_enable(gpio_channel_t a_channel)
{
  const int bits_count = 2;
  gpio_set_bits(a_channel, bits_count, GPIO_MODER_S, GPIO_MODER_INPUT);
}

void irs::gpio_otyper_output_open_drain_enable(gpio_channel_t a_channel)
{
  const int bits_count = 1;
  gpio_set_bits(a_channel, bits_count, GPIO_OTYPER_S,
    GPIO_OTYPER_OUTPUT_OPEN_DRAIN);
}

void irs::gpio_alternate_function_select(gpio_channel_t a_channel,
  size_t a_function_number)
{
  IRS_LIB_ASSERT(a_function_number <= 0xF);
  const size_t port_address = irs::get_port_address(a_channel);
  int pin_index = get_pin_index(a_channel);
  const int bits_count = 4;
  const int gpio_shift = (pin_index < 8) ? GPIO_AFRL_S : GPIO_AFRH_S;
  pin_index = (pin_index < 8) ? pin_index : pin_index%8;
  gpio_set_bits(a_channel, bits_count, gpio_shift, a_function_number);
  IRS_SET_BITS(port_address + gpio_shift, bits_count*pin_index,
    bits_count, a_function_number);
}

void irs::gpio_ospeedr_select(gpio_channel_t a_channel, size_t a_speed)
{
  IRS_LIB_ASSERT(a_speed <= IRS_GPIO_SPEED_100MHZ);
  const int bits_count = 2;
  gpio_set_bits(a_channel, bits_count, GPIO_OSPEEDR_S, a_speed);
}

void irs::gpio_usart_alternate_function_select(gpio_channel_t a_channel,
  int a_com_index)
{
  int alternate_function = 0x07;
  switch (a_com_index) {
    case 1: {
      alternate_function = 0x07;
    } break;
    case 2: {
      alternate_function = 0x07;
    } break;
    case 3: {
      alternate_function = 0x07;
    } break;
    case 4: {
      alternate_function = 0x08;
    } break;
    case 5: {
      alternate_function = 0x08;
    } break;
    case 6: {
      alternate_function = 0x08;
    } break;
    #ifdef IRS_STM32F4xx
    case 7: {
      alternate_function = 0x08;
    } break;
    case 8: {
      alternate_function = 0x08;
    } break;
    #endif // IRS_STM32F4xx
    default: {
      #ifdef IRS_STM32F2xx
      IRS_LIB_ASSERT_MSG("Индекс ком-порта должен быть от 1 до 6");
      #else // IRS_STM32F4xx IRS_STM32F7xx
      IRS_LIB_ASSERT_MSG("Индекс ком-порта должен быть от 1 до 8");
      #endif // IRS_STM32F4xx IRS_STM32F7xx
    }
  }
  gpio_alternate_function_select(a_channel, alternate_function);
}

usart_regs_t* irs::get_usart(int a_com_index)
{
  usart_regs_t* usart = NULL;
  switch (a_com_index) {
    case 1: {
      usart = reinterpret_cast<usart_regs_t*>(IRS_USART1_BASE);
    } break;
    case 2: {
      usart = reinterpret_cast<usart_regs_t*>(IRS_USART2_BASE);
    } break;
    case 3: {
      usart = reinterpret_cast<usart_regs_t*>(IRS_USART3_BASE);
    } break;
    case 4: {
      usart = reinterpret_cast<usart_regs_t*>(IRS_UART4_BASE);
    } break;
    case 5: {
      usart = reinterpret_cast<usart_regs_t*>(IRS_UART5_BASE);
    } break;
    case 6: {
      usart = reinterpret_cast<usart_regs_t*>(IRS_USART6_BASE);
    } break;
    default: {
      IRS_LIB_ASSERT_MSG("Индекс ком-порта должен быть от 1 до 6");
    }
  }
  return usart;
}

void irs::update_interrupt_enable(size_t a_address)
{
  update_interrupt_enabled(a_address, true);
}

void irs::update_interrupt_enabled(size_t a_address, bool a_enabled)
{
  const irs_u32 value = a_enabled ? 1 : 0;
  switch (a_address) {
    case IRS_TIM1_PWM1_BASE: {
      SETENA0_bit.SETENA_TIM1_UP_TIM10 = value;
    } break;
    /*case TIM2_BASE: {

    } break;*/
    case IRS_TIM3_BASE: {
      SETENA0_bit.SETENA_TIM3 = value;
    } break;
    case IRS_TIM4_BASE: {
      SETENA0_bit.SETENA_TIM4 = value;
    } break;
    /*case TIM5_BASE: {

    } break;
    case TIM6_BASE: {

    } break;
    case TIM7_BASE: {

    } break;*/
    case IRS_TIM8_PWM2_BASE: {
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
      IRS_LIB_ASSERT_MSG("Включение/отключение прерывания для указанного "
        "устройства не определено");
    }
  }
}

#endif // IRS_STM32_F2_F4_F7
