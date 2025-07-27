/**
* @file app_settings.c
* @brief Source file for Application Settings functions.
* @author Marcel Maas, Thomas Wiemken, ELV Elektronik AG
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_settings.h"
#include "eeprom_emul_types.h"
#include "eeprom.h"
#include "flash_user_func.h"
#include "base.h"
#include "sys_app.h"

/* Definitions ---------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Prototypes ----------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
EE_Status app_eeprom_set_defaults( void )
{
  EE_Status ee_status = EE_OK;

  ee_status = EE_WriteVariable8bits( EEPROM_EMU_APP_MODE_ADDRESS, APP_MODE_DEFAULT );
  ee_status = EEPROM_start_cleanup_polling_mode_if_needed( ee_status );

  ee_status = EE_WriteVariable32bits( EEPROM_EMU_DUTYCYCLE_ADDRESS, APP_DUTYCYCLE_DEFAULT );
  ee_status = EEPROM_start_cleanup_polling_mode_if_needed( ee_status );
  
  return ee_status;
}

EE_Status app_eeprom_set_settings( application_settings_t settings )
{
  EE_Status ee_status = EE_OK;
  
  application_settings_t temp_settings;
  app_eeprom_get_settings( &temp_settings );
  
  ee_status = app_eeprom_set_variable( APP_DATATYPE_U8, EEPROM_EMU_APP_MODE_ADDRESS, temp_settings.u8_app_mode, settings.u8_app_mode );
  ee_status = app_eeprom_set_variable( APP_DATATYPE_U32, EEPROM_EMU_DUTYCYCLE_ADDRESS, temp_settings.u32_app_dutycycle, settings.u32_app_dutycycle );
  
  return ee_status;
}

EE_Status app_eeprom_get_settings( application_settings_t *settings )
{
  EE_Status ee_status = EE_OK;

  ee_status = EEPROM_read_ee_variable_8bits( EEPROM_EMU_APP_MODE_ADDRESS, &settings->u8_app_mode );
  ee_status = EEPROM_read_ee_variable_32bits( EEPROM_EMU_DUTYCYCLE_ADDRESS, &settings->u32_app_dutycycle );
  
  return ee_status;
}

EE_Status app_eeprom_set_variable( uint32_t datatype, EEPROM_EMU_VirtTable addr, uint32_t var_old, uint32_t var_new )
{
  EE_Status ee_status = EE_OK;

  if( var_old != var_new )
  {
    switch( datatype )
    {
      case APP_DATATYPE_U8:
        {
          ee_status = EEPROM_write_ee_variable_8bits( addr, var_new );
        }
        break;
      case APP_DATATYPE_U16:
        {
          ee_status = EEPROM_write_ee_variable_16bits( addr, var_new );
        }
        break;
      case APP_DATATYPE_U32:
        {
          ee_status = EEPROM_write_ee_variable_32bits( addr, var_new );
        }
        break;
      default:
        {
          ;
        }
        break;
    }
  }
  
  return ee_status;
}

void app_settings_print( application_settings_t settings )
{
  APP_LOG( TS_OFF, VLEVEL_L, "############ ELV-BM-TRX1 Template - settings ############\r\n" );
//  APP_LOG( TS_OFF, VLEVEL_L, "Mode:                       %u\r\n", settings.u8_app_mode );
  APP_LOG( TS_OFF, VLEVEL_L, "Interval:                          0x%x\r\n", settings.u32_app_dutycycle / 30000 );
  APP_LOG( TS_OFF, VLEVEL_L, "#########################################################\r\n" );
}

void app_settings_process_dl( uint8_t *buffer, uint8_t buffer_size )
{
  application_settings_t temp_settings;

  if( buffer_size == APP_DL_SIZE )
  {
    if( buffer[0] == APP_DEVICE_ID )
    {
      app_eeprom_get_settings( &temp_settings );
      
      // Application dutycycle
      if( buffer[1] != 0 )
      {
        temp_settings.u32_app_dutycycle = ( buffer[1] * 30000 );
      }
    }
    app_eeprom_set_settings( temp_settings );
  }
}
