#ifndef _spi_handle_H_
#define _spi_handle_H_

#include <stdint.h>
#include "stm32f7xx.h"

void spi_rx_isr(SPI_HandleTypeDef *hspi);
void __to_spi_rx_data(SPI_HandleTypeDef *hspi);
void spi_int_config(SPI_HandleTypeDef *hspi,void (*RxISR)(struct __SPI_HandleTypeDef *hspi));



#endif

