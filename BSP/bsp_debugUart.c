/**
 * bsp_debugUart.c
 * @tingzhong666
 * 2024-03-29 21:34:50
 * 调试串口
 */
#include <bsp_debugUart.h>

char bsp_debugUart_dma_rx_mem[BSP_DEBUGUART_UART_RX_DMA_MEM_SIZE] = {0};
void bsp_debugUart_init()
{
    dma_single_data_parameter_struct init_struct;
    // IO复用
    rcu_periph_clock_enable(BSP_DEBUGUART_UART_RX_IO_RCU);
    rcu_periph_clock_enable(BSP_DEBUGUART_UART_TX_IO_RCU);

    gpio_mode_set(BSP_DEBUGUART_UART_RX_IO, GPIO_MODE_AF, GPIO_PUPD_NONE, BSP_DEBUGUART_UART_RX_IO_PIN);
    gpio_mode_set(BSP_DEBUGUART_UART_TX_IO, GPIO_MODE_AF, GPIO_PUPD_NONE, BSP_DEBUGUART_UART_TX_IO_PIN);
    gpio_output_options_set(BSP_DEBUGUART_UART_RX_IO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, BSP_DEBUGUART_UART_RX_IO_PIN);
    gpio_output_options_set(BSP_DEBUGUART_UART_TX_IO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, BSP_DEBUGUART_UART_TX_IO_PIN);
    gpio_af_set(BSP_DEBUGUART_UART_RX_IO, GPIO_AF_7, BSP_DEBUGUART_UART_RX_IO_PIN);
    gpio_af_set(BSP_DEBUGUART_UART_TX_IO, GPIO_AF_7, BSP_DEBUGUART_UART_TX_IO_PIN);
    // 串口
    rcu_periph_clock_enable(BSP_DEBUGUART_UART_RCU);

    usart_baudrate_set(BSP_DEBUGUART_UART, 9600);
    usart_parity_config(BSP_DEBUGUART_UART, USART_PM_NONE);
    usart_word_length_set(BSP_DEBUGUART_UART, USART_WL_8BIT);
    usart_stop_bit_set(BSP_DEBUGUART_UART, USART_STB_1BIT);
    usart_enable(BSP_DEBUGUART_UART);
    usart_transmit_config(BSP_DEBUGUART_UART, USART_TRANSMIT_ENABLE);
    usart_receive_config(BSP_DEBUGUART_UART, USART_RECEIVE_ENABLE);
    // 串口 DMA中断接收
    usart_dma_receive_config(BSP_DEBUGUART_UART, USART_RECEIVE_DMA_ENABLE);
    usart_interrupt_enable(BSP_DEBUGUART_UART, USART_INT_IDLE);
    nvic_irq_enable(USART0_IRQn, 1, 0);

    rcu_periph_clock_enable(BSP_DEBUGUART_UART_RX_DMA_RCU);

    dma_deinit(BSP_DEBUGUART_UART_RX_DMA, BSP_DEBUGUART_UART_RX_DMA_CHANNEL);
    dma_single_data_para_struct_init(&init_struct);
    init_struct.periph_addr = (uint32_t)&USART_DATA(BSP_DEBUGUART_UART);
    init_struct.memory0_addr = (uint32_t)bsp_debugUart_dma_rx_mem;
    init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    init_struct.periph_memory_width = DMA_MEMORY_WIDTH_8BIT;
    init_struct.number = BSP_DEBUGUART_UART_RX_DMA_MEM_SIZE;
    dma_single_data_mode_init(BSP_DEBUGUART_UART_RX_DMA, BSP_DEBUGUART_UART_RX_DMA_CHANNEL, &init_struct);
    dma_channel_subperipheral_select(BSP_DEBUGUART_UART_RX_DMA, BSP_DEBUGUART_UART_RX_DMA_CHANNEL, DMA_SUBPERI4);
    dma_channel_enable(BSP_DEBUGUART_UART_RX_DMA, BSP_DEBUGUART_UART_RX_DMA_CHANNEL);
}

int fputc(int c, FILE *fp)
{
    usart_data_transmit(BSP_DEBUGUART_UART, c);
    while (usart_flag_get(BSP_DEBUGUART_UART, USART_FLAG_TBE) == RESET)
        ;
    usart_flag_clear(BSP_DEBUGUART_UART, USART_FLAG_TBE);
    return 1;
}

void bsp_debugUart_RX_ISR()
{
    usart_data_receive(BSP_DEBUGUART_UART);
    printf("rx: %s\n", bsp_debugUart_dma_rx_mem);
    memset(bsp_debugUart_dma_rx_mem, 0, BSP_DEBUGUART_UART_RX_DMA_MEM_SIZE);
    bsp_debugUart_init();
}
