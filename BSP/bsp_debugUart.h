/**
 * bsp_debugUart.h
 * @tingzhong666
 * 2024-03-29 21:35:03
 */

#ifndef BSP_DEBUGUART_H
#define BSP_DEBUGUART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "gd32f4xx.h"

#define BSP_DEBUGUART_UART_RCU RCU_USART0
#define BSP_DEBUGUART_UART USART0

#define BSP_DEBUGUART_UART_RX_DMA_RCU RCU_DMA1
#define BSP_DEBUGUART_UART_RX_DMA DMA1
#define BSP_DEBUGUART_UART_RX_DMA_CHANNEL DMA_CH2
#define BSP_DEBUGUART_UART_RX_DMA_MEM_SIZE 256

#define BSP_DEBUGUART_UART_RX_IO_RCU RCU_GPIOA
#define BSP_DEBUGUART_UART_RX_IO GPIOA
#define BSP_DEBUGUART_UART_RX_IO_PIN GPIO_PIN_10

#define BSP_DEBUGUART_UART_TX_IO_RCU RCU_GPIOA
#define BSP_DEBUGUART_UART_TX_IO GPIOA
#define BSP_DEBUGUART_UART_TX_IO_PIN GPIO_PIN_9

    void bsp_debugUart_init(void);
    void bsp_debugUart_RX_ISR(void);

#ifdef __cplusplus
}
#endif

#endif
