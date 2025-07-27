/**
* @file base.h
* @brief Header file for board configuration.
* @author Marcel Maas, Thomas Wiemken, ELV Elektronik AG
**/

/** @addtogroup BASE
* @{
**/
/*---------------------------------------------------------------------------*/

#ifndef __BASE_H__
#define __BASE_H__

// Includes --------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

// Definitions -----------------------------------------------------------------
#define BASE_BL_ADDR                                0x080007D0
#define BASE_DEVEUI_ADDR                            0x080007E0
#define BASE_APPEUI_ADDR                            0x080007E8
#define BASE_APPKEY_ADDR                            0x080007F0

#define POWER_MODULE_PRESENT_THRESHOLD_LOW          50
#define POWER_MODULE_PRESENT_THRESHOLD_HIGH         500

#define BASE_BUTTON_DEBOUNCE_PERIOD                 5

// Typedefs --------------------------------------------------------------------
typedef struct base_callbacks_s
{
  void ( *base_user_button_event )( void );
} base_callbacks_t;

// Variables -------------------------------------------------------------------
// Prototypes ------------------------------------------------------------------
void base_print_bl_and_app_version( uint8_t *app, uint8_t *version );
void base_init( base_callbacks_t *base_callbacks );
void base_init_input( void );

void base_init_en_adc_supply( void );
void base_deinit_en_adc_supply( void );
void base_en_adc_supply( bool enable );
void base_power_module_detection( void );
uint16_t base_get_supply_level( void );
void base_set_is_pm_present( bool b_value );
bool base_get_is_pm_present( void );

void base_state_button_cb( void *context );
void base_debounce_button( void *context );
void base_reset_timestamps( void );

void base_enable_irqs( void );
void base_disable_irqs( void );

#endif /* __BASE__ */
