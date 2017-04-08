/**
  ******************************************************************************
  * @file           : USB_DEVICE  
  * @version        : v2.0_Cube
  * @brief          : This file implements the USB Device 
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

/* USB Device Core handle declaration */
USBD_HandleTypeDef hUsbDeviceFS;

/* Return USBD_OK if the Battery Charging Detection mode (BCD) is used, else USBD_FAIL */
extern USBD_StatusTypeDef USBD_LL_BatteryCharging(USBD_HandleTypeDef *pdev);

/* USB init function */				        
void USB_DEVICE_Init(void)
{
    /* Init Device Library,Add Supported Class and Start the library */
    USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);
    USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC);
    USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS);
    /* Verify if the Battery Charging Detection mode (BCD) is used : */
    /* If yes, the USB device is started in the HAL_PCDEx_BCD_Callback */
    /* upon reception of PCD_BCD_DISCOVERY_COMPLETED message. */
    /* If no, the USB device is started now. */
    if(USBD_LL_BatteryCharging(&hUsbDeviceFS) != USBD_OK)
    {
        USBD_Start(&hUsbDeviceFS);
    }
}

/**
  * @brief  HAL_PCDEx_BCD_Callback : Send BCD message to user layer
  * @param  hpcd: PCD handle
  * @param  msg: LPM message
  * @retval HAL status
  */
void HAL_PCDEx_BCD_Callback(PCD_HandleTypeDef *hpcd, PCD_BCD_MsgTypeDef msg)
{
    USBD_HandleTypeDef usbdHandle = hUsbDeviceFS;  
    if(hpcd->battery_charging_active == ENABLE)
    {
        switch(msg)
        {    
          case PCD_BCD_CONTACT_DETECTION:
          break;

          case PCD_BCD_STD_DOWNSTREAM_PORT:
          break;

          case PCD_BCD_CHARGING_DOWNSTREAM_PORT:
          break;

          case PCD_BCD_DEDICATED_CHARGING_PORT:
          break;

          case PCD_BCD_DISCOVERY_COMPLETED:
            USBD_Start(&usbdHandle);
          break;

          case PCD_BCD_ERROR:
          default:
          break;
        }
    }
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
