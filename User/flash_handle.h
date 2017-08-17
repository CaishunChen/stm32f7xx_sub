#ifndef _flash_handle_H_
#define _flash_handle_H_

#include "stdint.h"
#include "stm32f7xx.h"
#include "bsp_mt25q.h"


#ifdef DEBUG
void print_all_address(void);
#endif
uint32_t get_flash_address(uint8_t freq,uint8_t channel);
QSPI_StaticTypeDef flash_read(uint8_t freq,uint8_t channel,uint8_t *p_a_data,uint8_t *p_p_data);
QSPI_StaticTypeDef flash_write(uint8_t freq,uint8_t channel,uint8_t *p_a_data,uint8_t *p_p_data);
QSPI_StaticTypeDef flash_clean(uint8_t freq,uint8_t channel);


#endif//__flash_H_

