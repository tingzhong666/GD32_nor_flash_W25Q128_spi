/**
 * bsp_flash.h
 * @tingzhong666
 * 2024-03-30 14:31:06
 */

#ifndef BSP_FLASH_H
#define BSP_FLASH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "gd32f4xx.h"

#define BSP_FLASH_SPI_RCU RCU_SPI4
#define BSP_FLASH_SPI SPI4
#define BSP_FLASH_SPI_CLOCK_POLARITY_PHASE SPI_CK_PL_HIGH_PH_2EDGE

#define BSP_FLASH_SPI_CS_GPIO_RCU RCU_GPIOF
#define BSP_FLASH_SPI_CS_GPIO GPIOF
#define BSP_FLASH_SPI_CS_GPIO_PIN GPIO_PIN_6

#define BSP_FLASH_SPI_CLK_GPIO_RCU RCU_GPIOF
#define BSP_FLASH_SPI_CLK_GPIO GPIOF
#define BSP_FLASH_SPI_CLK_GPIO_PIN GPIO_PIN_7

#define BSP_FLASH_SPI_MISO_GPIO_RCU RCU_GPIOF
#define BSP_FLASH_SPI_MISO_GPIO GPIOF
#define BSP_FLASH_SPI_MISO_GPIO_PIN GPIO_PIN_8

#define BSP_FLASH_SPI_MOSI_GPIO_RCU RCU_GPIOF
#define BSP_FLASH_SPI_MOSI_GPIO GPIOF
#define BSP_FLASH_SPI_MOSI_GPIO_PIN GPIO_PIN_9

    void bsp_flash_init(void);
    uint16_t bsp_flash_deviceInfo(void);
    void bsp_flash_writeDatePage(uint32_t addr, uint8_t *data, uint16_t length);
    void bsp_flash_readDataByte(uint32_t addr, uint8_t *data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif
