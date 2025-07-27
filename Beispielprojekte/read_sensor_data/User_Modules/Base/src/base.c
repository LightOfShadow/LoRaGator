/**
* @file base.c
* @brief Source file for board configuration.
* @author Marcel Maas, Thomas Wiemken, ELV Elektronik AG
**/

/** @addtogroup BASE
* @{
**/
/*---------------------------------------------------------------------------*/

// Includes --------------------------------------------------------------------
#include "main.h"
#include "base.h"
#include "sys_app.h"
#include "hw_conf.h"
#include "hw_gpio.h"
#include "adc_if.h"
#include "stm32_mem.h"
#include "stm32_timer.h"
#include "stm32_systime.h"

// Definitions -----------------------------------------------------------------
// Typedefs --------------------------------------------------------------------
// Variables -------------------------------------------------------------------
static bool b_is_powermodule_present  = false;
static base_callbacks_t base_cb;
static uint16_t u16_bounce_state      = 0;
static UTIL_TIMER_Object_t debounce_timer;
static SysTime_t low_timestamp        = { .Seconds = 0, .SubSeconds = 0 };
static SysTime_t high_timestamp       = { .Seconds = 0, .SubSeconds = 0 };

// Prototypes ------------------------------------------------------------------

void base_print_bl_and_app_version( uint8_t *app, uint8_t *version )
{  
  uint8_t bl_version[3];
  memcpy( bl_version, ( uint32_t * )BASE_BL_ADDR, 3 );

#if defined( APP_LOG_ENABLED ) && ( APP_LOG_ENABLED == 0 )
  UTIL_ADV_TRACE_Init();
#endif
  APP_PPRINTF( "ELV-BM-TRX1_BL_VERSION: v%u.%u.%u\r\n", bl_version[0], bl_version[1], bl_version[2] );
  APP_PPRINTF( "%s_APP_VERSION: v%s\r\n", app, version );
#if defined( APP_LOG_ENABLED ) && ( APP_LOG_ENABLED == 0 )
  UTIL_ADV_TRACE_DeInit();
#endif
}

void base_init( base_callbacks_t *base_callbacks )
{
  UTIL_MEM_cpy_8( ( void * )&base_cb, ( const void * )base_callbacks, sizeof( base_callbacks_t ) );

  base_power_module_detection();
  base_init_input();
}

void base_init_input( void )
{
  GPIO_InitTypeDef initStruct = { 0 };

  initStruct.Mode   = GPIO_MODE_IT_RISING_FALLING;
  initStruct.Pull   = GPIO_PULLUP;
  initStruct.Speed  = GPIO_SPEED_FREQ_VERY_HIGH;

  UTIL_TIMER_Create( &debounce_timer, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, base_debounce_button, NULL );
  UTIL_TIMER_SetPeriod( &debounce_timer, BASE_BUTTON_DEBOUNCE_PERIOD );

  HW_GPIO_Init( STATE_BUTTON_GPIO_PORT, STATE_BUTTON_GPIO_PIN, &initStruct );
  while( !HW_GPIO_Read( STATE_BUTTON_GPIO_PORT, STATE_BUTTON_GPIO_PIN ) );
  HW_GPIO_SetIrq( STATE_BUTTON_GPIO_PORT, STATE_BUTTON_GPIO_PIN, 0, base_state_button_cb );
}

void base_init_en_adc_supply( void )
{
  GPIO_InitTypeDef initStruct = {0};

  initStruct.Mode   = GPIO_MODE_OUTPUT_PP;
  initStruct.Pull   = GPIO_NOPULL;
  initStruct.Speed  = GPIO_SPEED_FREQ_VERY_HIGH;
  
  HW_GPIO_Init( EN_BAT_VOLT_GPIO_PORT, EN_BAT_VOLT_GPIO_PIN, &initStruct );
  HW_GPIO_Write( EN_BAT_VOLT_GPIO_PORT, EN_BAT_VOLT_GPIO_PIN, GPIO_PIN_RESET );
}

void base_deinit_en_adc_supply( void )
{
  HW_GPIO_Deinit( EN_BAT_VOLT_GPIO_PORT, EN_BAT_VOLT_GPIO_PIN, GPIO_PIN_RESET );  
}

void base_en_adc_supply( bool enable )
{
  if( enable )
  {
    HW_GPIO_Write( EN_BAT_VOLT_GPIO_PORT, EN_BAT_VOLT_GPIO_PIN, GPIO_PIN_SET );
  }
  else
  {
    HW_GPIO_Write( EN_BAT_VOLT_GPIO_PORT, EN_BAT_VOLT_GPIO_PIN, GPIO_PIN_RESET );
  }
}

void base_power_module_detection( void )
{
  if( adc_get_channel_level( ADC_CHANNEL_BAT_VOLTAGE ) < POWER_MODULE_PRESENT_THRESHOLD_LOW )
  {
    base_init_en_adc_supply();
    base_en_adc_supply( true );
    HAL_Delay( 10 );

    if( adc_get_channel_level( ADC_CHANNEL_BAT_VOLTAGE ) > POWER_MODULE_PRESENT_THRESHOLD_HIGH )
    {
      base_set_is_pm_present( true );
    }

    base_deinit_en_adc_supply();
  }
}

uint16_t base_get_supply_level( void )
{
  uint16_t bat = 0;

  if( base_get_is_pm_present() )
  {
    base_init_en_adc_supply();
    base_en_adc_supply( true );
    HAL_Delay( 10 );
    bat = 2 * adc_get_channel_level( ADC_CHANNEL_BAT_VOLTAGE );
    base_deinit_en_adc_supply();
  }
  else
  {
    bat = adc_get_supply_level();
  }
  
  return bat;
}

void base_set_is_pm_present( bool b_value )
{
  b_is_powermodule_present = b_value;
}

bool base_get_is_pm_present( void )
{
  return b_is_powermodule_present;
}

void base_state_button_cb( void *context )
{
  base_disable_irqs();
  u16_bounce_state = 1;
  UTIL_TIMER_Start( &debounce_timer );
}

void base_debounce_button( void *context )
{
  u16_bounce_state = ( u16_bounce_state << 1 ) | HW_GPIO_Read( STATE_BUTTON_GPIO_PORT, STATE_BUTTON_GPIO_PIN ) | 0xe000;
  if( u16_bounce_state == 0xF000 )          // Button is LOW
  {
    // Set LOW Timestamp here
    low_timestamp = SysTimeGet();

    HW_GPIO_enable_irq( STATE_BUTTON_GPIO_PIN );
  }
  else if( u16_bounce_state == 0xFFFF )     // Button is HIGH
  {
    // Set HIGH Timestamp here, but only if LOW was set before
    if( low_timestamp.Seconds != 0 || low_timestamp.SubSeconds != 0 )
    {
//      high_timestamp = SysTimeGet();
//
//      SysTime_t div_timestamp = SysTimeSub( high_timestamp, low_timestamp );
//      uint32_t u32_time_diff = ( div_timestamp.Seconds * 1000 ) + div_timestamp.SubSeconds;

      base_cb.base_user_button_event();
    }
    else
    {
      base_enable_irqs();
    }

    base_reset_timestamps();
  }
  else
  {
    UTIL_TIMER_Start( &debounce_timer );
  }
}

void base_reset_timestamps( void )
{
  low_timestamp.Seconds     = 0;
  low_timestamp.SubSeconds  = 0;
  high_timestamp.Seconds    = 0;
  high_timestamp.SubSeconds = 0;
}

void base_enable_irqs( void )
{
  HW_GPIO_enable_irq( STATE_BUTTON_GPIO_PIN );
}

void base_disable_irqs( void )
{
  HW_GPIO_disable_irq( STATE_BUTTON_GPIO_PIN );
}
