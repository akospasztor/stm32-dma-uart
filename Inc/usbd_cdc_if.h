#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc.h"

/* Exported variables --------------------------------------------------------*/
extern USBD_CDC_ItfTypeDef  USBD_Interface_fops_FS;

/* Exported functions --------------------------------------------------------*/
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);


#ifdef __cplusplus
}
#endif
  
#endif /* __USBD_CDC_IF_H */
