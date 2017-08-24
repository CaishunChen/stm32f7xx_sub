#ifndef _spi_handle_H_
#define _spi_handle_H_

#include <stdint.h>
#include "stm32f7xx.h"

typedef enum 
{
	msg_invalid = 0,
	msg_valid = !msg_invalid
}NGVAM_MSG_VALID;

typedef struct 
{
	uint16_t msg_len;
	uint8_t msg_type;
	uint8_t *msg_ptr;
	uint8_t check_val;
	//uint8_t valid;
}NGVAM_MSG;

extern NGVAM_MSG g_msg;
extern NGVAM_MSG *gp_msg;


void spi_rx_isr(SPI_HandleTypeDef *hspi);

void __to_spi_rx_data(SPI_HandleTypeDef *hspi);
void __to_spi_rx_cmd(SPI_HandleTypeDef *hspi);
void __to_spi_idle(SPI_HandleTypeDef *hspi);
void spi_int_config(SPI_HandleTypeDef *hspi,void (*RxISR)(struct __SPI_HandleTypeDef *hspi));



#endif

