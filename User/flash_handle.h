#ifndef _flash_handle_H_
#define _flash_handle_H_

#include "stdint.h"
#include "stm32f7xx.h"
#include "bsp_mt25q.h"

//data format
//amplitude 0.5,1.0,1.5......63;
//phase 5.625,11.25......


#define MAX_CHANNEL 						(8)
#define MAX_FREQ 								(7)

#define AMPLITUDE_STEP 					(500)
#define PHASE_STEP 							(5625)

#define AMPLITUDE_STORE_SIZE  	(0x80*3) //128*3
#define PHASE_STORE_SIZE			 	(0x40*3) //64*3
#define AMPLITUDE_STORE_OFFSET 	(0x0000)
#define PHASE_STORE_OFFSET			(0x1000)


//#define QSPI_MT25Q1GB_TOTAL_SIZE   ( (uint32_t) 0x8000000) 
#define FREQ0_ADDR_BASE					(0x0000000)
#define FREQ1_ADDR_BASE					(0x0040000) //32kB*8  
#define FREQ2_ADDR_BASE					(0x0080000)
#define FREQ3_ADDR_BASE					(0x00C0000)
#define FREQ4_ADDR_BASE					(0x0100000)
#define FREQ5_ADDR_BASE					(0x0140000)
#define FREQ6_ADDR_BASE					(0x0180000)
//#define FREQ0_ADDR_BASE

#define CHANNEL0_BASE						(0x0000000)
#define CHANNEL1_BASE						(0x0008000)
#define CHANNEL2_BASE						(0x0010000)
#define CHANNEL3_BASE						(0x0018000)
#define CHANNEL4_BASE						(0x0020000)
#define CHANNEL5_BASE						(0x0028000)
#define CHANNEL6_BASE						(0x0030000)
#define CHANNEL7_BASE						(0x0038000)
 



#ifdef DEBUG
void print_all_address(void);
#endif
uint32_t get_flash_address(uint8_t freq,uint8_t channel);
QSPI_StaticTypeDef flash_read(uint8_t freq,uint8_t channel,uint8_t *p_a_data,uint8_t *p_p_data);
QSPI_StaticTypeDef flash_write(uint8_t freq,uint8_t channel,uint8_t *p_a_data,uint8_t *p_p_data);
QSPI_StaticTypeDef flash_clean(uint8_t freq,uint8_t channel);


#endif//__flash_H_

