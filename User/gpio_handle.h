#ifndef _gpio_handle_H_
#define _gpio_handle_H_

#include <stdint.h>
#include "stm32f7xx.h"

typedef struct 
{
	uint32_t a;
	uint32_t p;
}type_gpio_ctrl;

extern uint16_t g_gpio_ctrl_table[8];
extern type_gpio_ctrl g_gpio_ctrl_table_raw[8];

void sync_ctrl_port1(void);
void reset_ctrl_port1(void);
void sync_ctrl_port2(void);
void reset_ctrl_port2(void);
void sync_ctrl_port3(void);
void reset_ctrl_port3(void);
void sync_ctrl_port4(void);
void reset_ctrl_port4(void);
void sync_ctrl_port5(void);
void reset_ctrl_port5(void);
void sync_ctrl_port6(void);
void reset_ctrl_port6(void);
void sync_ctrl_port7(void);
void reset_ctrl_port7(void);
void sync_ctrl_port8(void);
void reset_ctrl_port8(void);


void reset_ctrl_all(void);
void gpio_convert_all(void);
void gpio_convert_one(type_gpio_ctrl *sdat,uint16_t *ddat);
void sync_ctrl_all(void);
void all_gpio_self_test(void);



#endif

