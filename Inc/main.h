#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"


/* Configuration **************************************************************/
#define DMA_BUF_SIZE        64      /* DMA circular buffer size in bytes */
#define DMA_TIMEOUT_MS      10      /* DMA Timeout duration in msec */
/******************************************************************************/


/* Defines -------------------------------------------------------------------*/
#define LED_G_Port          GPIOE
#define LED_G_Pin           GPIO_PIN_8
#define LED_R_Port          GPIOB
#define LED_R_Pin           GPIO_PIN_2

#define LED_G_ON()          HAL_GPIO_WritePin(LED_G_Port, LED_G_Pin, GPIO_PIN_SET);
#define LED_G_OFF()         HAL_GPIO_WritePin(LED_G_Port, LED_G_Pin, GPIO_PIN_RESET);
#define LED_G_TG()          HAL_GPIO_TogglePin(LED_G_Port, LED_G_Pin);
#define LED_R_ON()          HAL_GPIO_WritePin(LED_R_Port, LED_R_Pin, GPIO_PIN_SET);
#define LED_R_OFF()         HAL_GPIO_WritePin(LED_R_Port, LED_R_Pin, GPIO_PIN_RESET);
#define LED_R_TG()          HAL_GPIO_TogglePin(LED_R_Port, LED_R_Pin);

/* Type definitions ----------------------------------------------------------*/
typedef struct
{
    volatile uint8_t  flag;     /* Timeout event flag */
    uint16_t timer;             /* Timeout duration in msec */
    uint16_t prevCNDTR;         /* Holds previous value of DMA_CNDTR */
} DMA_Event_t;

/* Functions -----------------------------------------------------------------*/
void UART_Init(void);
void DMA_Init(void);
void GPIO_Init(void);
void SystemClock_Config(void);
void Error_Handler(void);

#endif /* __MAIN_H */
