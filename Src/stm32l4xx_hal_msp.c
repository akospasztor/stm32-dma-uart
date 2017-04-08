/**
  ******************************************************************************
  * STM32L4 UART DMA implementation with Timeout Event
  ******************************************************************************
  * @author Akos Pasztor
  * @file   stm32l4xx_hal_msp.c
  * @brief  
  *			
  *			
  *
  ******************************************************************************
  * Copyright (c) 2017 Akos Pasztor. All rights reserved.
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "main.h"

void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
    HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
    HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
    HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    if(huart->Instance == USART2)
    {
        __HAL_RCC_USART2_CLK_ENABLE();

        /* UART2 GPIO Configuration    
        PD5     --> USART2_TX
        PD6     --> USART2_RX 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
    if(huart->Instance == USART2)
    {
        __HAL_RCC_USART2_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5 | GPIO_PIN_6);
        HAL_DMA_DeInit(huart->hdmarx);
    }
}

