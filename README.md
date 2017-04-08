# STM32 Circular DMA with Timeout Event
This example presents how to implement performance-effective DMA timeout mechanism for peripheral DMA configured in circular mode.

## Table of contents

## Description
Implementing DMA for peripherals (e.g. UART) can significantly boost performance while reducing workload on the microcontroller core [[1]](#references), therefore configuring the DMA controller in circular mode can be straightforward for peripherals. However, in time-critical systems or hard real-time systems it is crucial to perform the required actions within specified deadlines. A DMA controller is only able to issue interrupts when its buffer is either full or halfway full, however in most cases the received amount of data is unpredictable. Therefore, when a transmission from a peripheral ends and the DMA buffer is not entirely filled, the remaining data in the buffer has to be processed within a time limit.

ST suggests two methods for implementing DMA timeout in Section 2 of AN3019 [[2]](#references). The first method utilizes a timer 

Method 1 and Method 2 description and drawbacks. Then introduce my idea and explain why it is better than the suggested methods. Put link to AN in prev. paragraph.


## Device & Environment

## Source code organization

## How to use

## Refs

- AN3109 (fifo emnulation with DMA and DMA timeout..) (www.st.com/resource/en/application_note/cd00256689.pdf)
- UM1734 (stm32cube usb device library)
- measurements (http://letanphuc.net/2014/06/how-to-use-stm32-dma/)
- STM32L4 DMA introduction presentation (www.st.com/resource/en/product_training/stm32l4_system_dma.pdf)
- stm32l476vg datasheet
- stm32l476vg reference manual

## References
[1] Wikipedia, “Direct Memory Access”, https://en.wikipedia.org/wiki/Direct_memory_access

[2] AN3019, “Communication peripheral FIFO emulation with DMA and DMA timeout in STM32F10x microcontrollers”, http://www.st.com/resource/en/application_note/cd00256689.pdf

