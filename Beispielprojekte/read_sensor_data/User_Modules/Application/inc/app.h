/**
* @file app.h
* @brief Header file for Application functions.
* @author Marcel Maas, Thomas Wiemken, ELV Elektronik AG
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
 extern "C" {
#endif

// Includes --------------------------------------------------------------------
#include <stdbool.h>

// Definitions -----------------------------------------------------------------
#define APP_APPLICATION_NAME_STR                "ELV-BM-TRX1 Template (basic)"
#define APP_APPLICATION_VERSION_STR             "1.0.0"

// Exported types --------------------------------------------------------------
// Exported macro --------------------------------------------------------------
// Exported functions ----------------------------------------------------------
void app_init( void );

void app_user_button_event( void );

void app_set_user_button_pressed( bool b_button_pressed );
bool app_get_user_button_pressed( void );

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

#endif /* __APP_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
