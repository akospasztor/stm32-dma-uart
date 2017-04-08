/**
  ******************************************************************************
  * STM32L4 UART DMA implementation with Timeout Event
  ******************************************************************************
  * @author Akos Pasztor
  * @file   stm32l4xx_it.c
  * @brief  Interrupt Service Routines
  *	        This file contains the exception and peripheral interrupt handlers.
  *	
  * 
  ******************************************************************************
  * Copyright (c) 2017 Akos Pasztor.                    https://akospasztor.com
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx_it.h"
#include "main.h"

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_Event_t dma_uart_rx;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
    
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
    while(1)
    {
    }
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
    while(1)
    {
    }
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
    while(1)
    {
    }
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
    while(1)
    {
    }
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{

}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{

}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{

}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
    
    /* DMA timer */
    if(dma_uart_rx.timer == 1)
    {
        /* DMA Timeout event: set Timeout Flag and call DMA Rx Complete Callback */
        dma_uart_rx.flag = 1;
        hdma_usart2_rx.XferCpltCallback(&hdma_usart2_rx);
    }
    if(dma_uart_rx.timer) { --dma_uart_rx.timer; }
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/******************************************************************************/
void USART2_IRQHandler(void)
{   
    /* UART IDLE Interrupt */
    if((USART2->ISR & USART_ISR_IDLE) != RESET)
    {
        USART2->ICR = UART_CLEAR_IDLEF;
        /* Start DMA timer */
        dma_uart_rx.timer = DMA_TIMEOUT_MS;
    }
}

/**
* @brief This function handles DMA1 channel6 global interrupt.
*/
void DMA1_Channel6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart2_rx);
}

/**
* @brief This function handles USB OTG FS global interrupt.
*/
void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

