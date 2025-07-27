/**
* @file app.c
* @brief Source file for Application functions.
* @author Marcel Maas, Thomas Wiemken, ELV Elektronik AG
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app.h"
#include "base.h"
#include "hw_gpio.h"
#include "led.h"
#include "sys_app.h"
#include "usart.h"
#include "i2c.h"
#include "ELV-AM-TH1.h"
#include "adc_if.h"

// Definitions -----------------------------------------------------------------
// Typedefs --------------------------------------------------------------------
// Variables -------------------------------------------------------------------
static base_callbacks_t base_app_cb =
{
  .base_user_button_event   = app_user_button_event,
};

static bool b_user_button_pressed = false;

// Prototypes ------------------------------------------------------------------
// Exported functions ----------------------------------------------------------

void app_init( void )
{
  base_print_bl_and_app_version( ( uint8_t* )APP_APPLICATION_NAME_STR, ( uint8_t* )APP_APPLICATION_VERSION_STR );

  base_init( &base_app_cb );

  leds_init();

  while( 1 )
  {
    APP_LOG( TS_OFF, VLEVEL_L, "Supply: %u mV\r\n", base_get_supply_level() );

    led_red_toggle();
    led_green_toggle();

    HAL_Delay( 500 );
  }
}

void app_user_button_event( void )
{
  app_set_user_button_pressed( true );
}

void app_set_user_button_pressed( bool b_button_pressed )
{
  b_user_button_pressed = b_button_pressed;
}

bool app_get_user_button_pressed( void )
{
  bool b_ret = b_user_button_pressed;

  if( b_ret )
  {
    app_set_user_button_pressed( false );
    base_enable_irqs();
  }

  return b_ret;
}
