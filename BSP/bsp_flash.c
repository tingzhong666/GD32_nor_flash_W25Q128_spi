/**
 * bsp_flash.c
 * @tingzhong666
 * 2024-03-30 14:30:49
 */
#include <bsp_flash.h>

void bsp_flash_init()
{
    spi_parameter_struct spi_struct;
    // io复用
    rcu_periph_clock_enable(BSP_FLASH_SPI_CS_GPIO_RCU);
    rcu_periph_clock_enable(BSP_FLASH_SPI_CLK_GPIO_RCU);
    rcu_periph_clock_enable(BSP_FLASH_SPI_MISO_GPIO_RCU);
    rcu_periph_clock_enable(BSP_FLASH_SPI_MOSI_GPIO_RCU);

    gpio_mode_set(BSP_FLASH_SPI_CS_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BSP_FLASH_SPI_CS_GPIO_PIN);
    gpio_output_options_set(BSP_FLASH_SPI_CS_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, BSP_FLASH_SPI_CS_GPIO_PIN);
    gpio_bit_set(BSP_FLASH_SPI_CS_GPIO, BSP_FLASH_SPI_CS_GPIO_PIN);

    gpio_mode_set(BSP_FLASH_SPI_CS_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, BSP_FLASH_SPI_MISO_GPIO_PIN);
    gpio_output_options_set(BSP_FLASH_SPI_CS_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, BSP_FLASH_SPI_MISO_GPIO_PIN);
    gpio_af_set(BSP_FLASH_SPI_CS_GPIO, GPIO_AF_5, BSP_FLASH_SPI_MISO_GPIO_PIN); // 函数说明AF5 AF6都有SPI4 但是af6没效果 只能AF5有效果
    gpio_mode_set(BSP_FLASH_SPI_CLK_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, BSP_FLASH_SPI_CLK_GPIO_PIN);
    gpio_output_options_set(BSP_FLASH_SPI_CLK_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, BSP_FLASH_SPI_CLK_GPIO_PIN);
    gpio_af_set(BSP_FLASH_SPI_CLK_GPIO, GPIO_AF_5, BSP_FLASH_SPI_CLK_GPIO_PIN);
    gpio_mode_set(BSP_FLASH_SPI_MOSI_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, BSP_FLASH_SPI_MOSI_GPIO_PIN);
    gpio_output_options_set(BSP_FLASH_SPI_MOSI_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, BSP_FLASH_SPI_MOSI_GPIO_PIN);
    gpio_af_set(BSP_FLASH_SPI_MOSI_GPIO, GPIO_AF_5, BSP_FLASH_SPI_MOSI_GPIO_PIN);
    // spi
    rcu_periph_clock_enable(BSP_FLASH_SPI_RCU);

    spi_struct_para_init(&spi_struct);
    spi_struct.device_mode = SPI_MASTER;                       // 主机模式
    spi_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;          // 全双工
    spi_struct.frame_size = SPI_FRAMESIZE_8BIT;                // 1帧数据大小 8位
    spi_struct.nss = SPI_NSS_SOFT;                             // 软件片选
    spi_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE; // SPI模式3 (1,1) 高极性 下降沿
    spi_struct.prescale = SPI_PSC_2;                           // 预分频
    spi_struct.endian = SPI_ENDIAN_MSB;                        // 高位在前 大端模式
    spi_init(BSP_FLASH_SPI, &spi_struct);
    spi_enable(BSP_FLASH_SPI);
}

// SPI发送数据
void bsp_flash_spi_send_8bit(uint8_t data)
{
    spi_i2s_data_transmit(BSP_FLASH_SPI, (uint16_t)data);
    while (spi_i2s_flag_get(BSP_FLASH_SPI, SPI_FLAG_TBE) == RESET)
        ;
    while (spi_i2s_flag_get(BSP_FLASH_SPI, SPI_FLAG_RBNE) == RESET)
        ;
    spi_i2s_data_receive(BSP_FLASH_SPI);
}

// SPI接收数据
uint8_t bsp_flash_spi_rec_8bit()
{
    spi_i2s_data_transmit(BSP_FLASH_SPI, 0xff);
    while (spi_i2s_flag_get(BSP_FLASH_SPI, SPI_FLAG_TBE) == RESET)
        ;
    while (spi_i2s_flag_get(BSP_FLASH_SPI, SPI_FLAG_RBNE) == RESET)
        ;
    return (uint8_t)spi_i2s_data_receive(BSP_FLASH_SPI);
}

// 开启flash
void bsp_flash_open()
{
    gpio_bit_reset(BSP_FLASH_SPI_CS_GPIO, BSP_FLASH_SPI_CS_GPIO_PIN);
}

// 关闭flash
void bsp_flash_close()
{
    gpio_bit_set(BSP_FLASH_SPI_CS_GPIO, BSP_FLASH_SPI_CS_GPIO_PIN);
}

// 读取设备信息
uint16_t bsp_flash_deviceInfo()
{
    uint16_t tmp = 0;
    bsp_flash_open();
    // 指令
    bsp_flash_spi_send_8bit(0x90);
    // 地址
    bsp_flash_spi_send_8bit(0x00);
    bsp_flash_spi_send_8bit(0x00);
    bsp_flash_spi_send_8bit(0x00);
    // 接收
    tmp = bsp_flash_spi_rec_8bit() << 8;
    tmp |= bsp_flash_spi_rec_8bit();
    bsp_flash_close();
    return tmp;
}

// 写使能
void bsp_flash_writeEnable()
{
    bsp_flash_open();
    bsp_flash_spi_send_8bit(0x06);
    bsp_flash_close();
}

// 忙检测 1忙 0闲
FlagStatus bsp_flash_isBusy()
{
    uint8_t status = 0;
    bsp_flash_open();
    bsp_flash_spi_send_8bit(0x05);
    status = bsp_flash_spi_rec_8bit();
    status &= 0x01; // S0为忙检测位
    bsp_flash_close();
    if (status)
        return SET;
    else
        return RESET;
}

// 扇区擦除
void bsp_flash_eraseSector(uint32_t addr)
{
    bsp_flash_writeEnable();
    while (bsp_flash_isBusy() == SET)
        ;
    bsp_flash_open();
    bsp_flash_spi_send_8bit(0x20);
    bsp_flash_spi_send_8bit(addr >> 16);
    bsp_flash_spi_send_8bit(addr >> 8);
    bsp_flash_spi_send_8bit(addr);
    bsp_flash_close();
    while (bsp_flash_isBusy() == SET)
        ;
}

// 页写入数据
void bsp_flash_writeDatePage(uint32_t addr, uint8_t *data, uint16_t length)
{
    bsp_flash_eraseSector(addr);
    bsp_flash_writeEnable();
    while (bsp_flash_isBusy() == SET)
        ;
    bsp_flash_open();
    bsp_flash_spi_send_8bit(0x02);
    bsp_flash_spi_send_8bit(addr >> 16);
    bsp_flash_spi_send_8bit(addr >> 8);
    bsp_flash_spi_send_8bit(addr);
    while (length--)
        bsp_flash_spi_send_8bit(*data++);
    bsp_flash_close();
    while (bsp_flash_isBusy() == SET)
        ;
}

// 读取数据
void bsp_flash_readDataByte(uint32_t addr, uint8_t *data, uint16_t length)
{
    bsp_flash_open();
    bsp_flash_spi_send_8bit(0x03);
    bsp_flash_spi_send_8bit(addr >> 16);
    bsp_flash_spi_send_8bit(addr >> 8);
    bsp_flash_spi_send_8bit(addr);
    while (length--)
        *data++ = bsp_flash_spi_rec_8bit();
    bsp_flash_close();
}
