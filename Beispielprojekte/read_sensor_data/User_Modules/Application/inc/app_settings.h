/**
* @file app_settings.h
* @brief Header file for Application Settings functions.
* @author Marcel Maas, Thomas Wiemken, ELV Elektronik AG
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_SETTINGS_H
#define __APP_SETTINGS_H

#ifdef __cplusplus
 extern "C" {
#endif

// Includes --------------------------------------------------------------------
#include "eeprom_emul_types.h"
#include "flash_user_func.h"
#include <stdbool.h>
// Definitions -----------------------------------------------------------------
#define APP_DATATYPE_U8                                           8
#define APP_DATATYPE_U16                                          16
#define APP_DATATYPE_U32                                          32

#define APP_DL_PORT                                               10
#define APP_DL_SIZE                                               2
#define APP_DEVICE_ID                                             0x0E

#define APP_MODE_DEFAULT                                          APP_MODE_CYCLIC
#define APP_DUTYCYCLE_DEFAULT                                     30000

// SETTINGS - APP_DEFAULT - START
#define APP_SETTING_DEFAULT                                      {                                            \
                                                                    APP_MODE_DEFAULT,                         \
                                                                    APP_DUTYCYCLE_DEFAULT,                    \
                                                                 } 
// SETTINGS - APP_DEFAULT - END

// Exported types --------------------------------------------------------------
enum APPLICATION_MODE
{
  APP_MODE_CYCLIC = 1,

  APP_MODE_SIZE
};

typedef struct
{
  uint8_t u8_app_mode;
  uint32_t u32_app_dutycycle;
} application_settings_t;

// Exported macro --------------------------------------------------------------
// Exported functions ----------------------------------------------------------

EE_Status app_eeprom_set_defaults( void );
EE_Status app_eeprom_set_settings( application_settings_t settings );
EE_Status app_eeprom_get_settings( application_settings_t *settings );

EE_Status app_eeprom_set_variable( uint32_t datatype, EEPROM_EMU_VirtTable addr, uint32_t var_old, uint32_t var_new );

void app_settings_print( application_settings_t settings );

void app_settings_process_dl( uint8_t *buffer, uint8_t buffer_size );

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

#endif /* __APP_SETTINGS_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
