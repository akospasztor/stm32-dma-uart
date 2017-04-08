/**
  ******************************************************************************
  * STM32L4 UART DMA implementation with Timeout Event
  ******************************************************************************
  * @author Akos Pasztor
  * @file   main.c
  * @brief  
  *			
  *			
  *
  ******************************************************************************
  * Copyright (c) 2017 Akos Pasztor. All rights reserved.
  ******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

/* HAL handle structures -----------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* DMA Timeout event structure
 * Note: prevCNDTR initial value must be set to maximum size of DMA buffer!
*/
DMA_Event_t dma_uart_rx = {0,0,DMA_BUF_SIZE};

uint8_t dma_rx_buf[DMA_BUF_SIZE];       /* Circular buffer for DMA */
uint8_t data[DMA_BUF_SIZE] = {'\0'};    /* Data buffer that contains newly received data */

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    GPIO_Init();
    USB_DEVICE_Init();
    HAL_Delay(1000);

    UART_Init();
    DMA_Init();
    
    /* Start DMA */
    if(HAL_UART_Receive_DMA(&huart2, (uint8_t*)dma_rx_buf, DMA_BUF_SIZE) != HAL_OK)
    {        
        Error_Handler();
    }
    
    /* Disable Half Transfer Interrupt */
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
    
    while(1)
    {
        LED_G_TG();
        HAL_Delay(500);
    }
}

/** DMA Rx Complete AND DMA Rx Timeout function
 * Timeout event: generated after UART IDLE IT + DMA Timeout value
 * Scenarios:
 *  - Timeout event when previous event was DMA Rx Complete --> new data is from buffer beginning till (MAX-currentCNDTR)
 *  - Timeout event when previous event was Timeout event   --> buffer contains old data, new data is in the "middle": from (MAX-previousCNDTR) till (MAX-currentCNDTR)
 *  - DMA Rx Complete event when previous event was DMA Rx Complete --> entire buffer holds new data
 *  - DMA Rx Complete event when previous event was Timeout event   --> buffer entirely filled but contains old data, new data is from (MAX-previousCNDTR) till MAX
 * Remarks:
 *  - If there is no following data after DMA Rx Complete, the generated IDLE Timeout has to be ignored!
 *  - When buffer overflow occurs, the following has to be performed in order not to lose data:
 *      (1): DMA Rx Complete event occurs, process first part of new data till buffer MAX.
 *      (2): In this case, the currentCNDTR is already decreased because of overflow.
 *              However, previousCNDTR has to be set to MAX in order to signal for upcoming Timeout event that new data has to be processed from buffer beginning.
 *      (3): When many overflows occur, simply process DMA Rx Complete events (process entire DMA buffer) until Timeout event occurs.
 *      (4): When there is no more overflow, Timeout event occurs, process last part of data from buffer beginning till currentCNDTR.
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint16_t i, pos, start, length;
    uint16_t currCNDTR = __HAL_DMA_GET_COUNTER(huart->hdmarx);
    
    /* Ignore IDLE Timeout when the received characters exactly filled up the DMA buffer and DMA Rx Complete IT is generated, but there is no new character during timeout */
    if(dma_uart_rx.flag && currCNDTR == DMA_BUF_SIZE)
    { 
        dma_uart_rx.flag = 0;
        return;
    }
    
    /* Determine start position in DMA buffer based on previous CNDTR value */
    start = (dma_uart_rx.prevCNDTR < DMA_BUF_SIZE) ? (DMA_BUF_SIZE - dma_uart_rx.prevCNDTR) : 0;
    
    if(dma_uart_rx.flag)    /* Timeout event */
    {
        /* Determine new data length based on previous DMA_CNDTR value:
         *  If previous CNDTR is less than DMA buffer size: there is old data in DMA buffer (from previous timeout) that has to be ignored.
         *  If CNDTR == DMA buffer size: entire buffer content is new and has to be processed.
        */
        length = (dma_uart_rx.prevCNDTR < DMA_BUF_SIZE) ? (dma_uart_rx.prevCNDTR - currCNDTR) : (DMA_BUF_SIZE - currCNDTR);
        dma_uart_rx.prevCNDTR = currCNDTR;
        dma_uart_rx.flag = 0;
    }
    else                /* DMA Rx Complete event */
    {
        length = DMA_BUF_SIZE - start;
        dma_uart_rx.prevCNDTR = DMA_BUF_SIZE;
    }
    
    /* Copy and Process new data */
    for(i=0,pos=start; i<length; ++i,++pos)
    {
        data[i] = dma_rx_buf[pos];
    }
    
    /* Send received data over USB */
    CDC_Transmit_FS(data, length);
}

/* Error callback */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    Error_Handler();
}

/* USART2 Configuration */
void UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if(HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* UART2 IDLE Interrupt Configuration */
    SET_BIT(USART2->CR1, USART_CR1_IDLEIE);
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
}

/* DMA Configuration */
void DMA_Init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    hdma_usart2_rx.Instance = DMA1_Channel6;
    hdma_usart2_rx.Init.Request = DMA_REQUEST_2;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    if(HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
        Error_Handler();
    }
            
    __HAL_LINKDMA(&huart2,hdmarx, hdma_usart2_rx);
    
    /* DMA Interrupt Configuration */
    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

/* GPIO Configuration */
void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = LED_G_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_G_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = LED_R_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_R_Port, &GPIO_InitStruct);
}

/* System Clock Configuration */
void SystemClock_Config(void)
{    
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /* Initializes the CPU, AHB and APB bus clocks */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 24;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType =   RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_USB;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
    PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
    PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
    PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure the main internal regulator output voltage */
    if(HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure the Systick */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
    while(1) 
    {
        LED_R_ON();
    }
}

#ifdef USE_FULL_ASSERT
/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
    
}
#endif
