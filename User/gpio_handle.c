#include "gpio_handle.h"
#include "gpio.h"

#ifdef DEBUG
#define DBG_LOG(x) printf x
#else
#define DBG_LOG(x) 
#endif

#define CTRL_MASK (0x1FFF)

//此表储存8组IO口的默认值
const uint16_t gpio_ctrl_table_reset[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//const uint32_t gpio_ctrl_table_base[8]={GPIOA,GPIOB,GPIOC}
uint16_t g_gpio_ctrl_table[8]={0};
uint8_t g_gpio_ctrl_table_raw[8][3]={0};

static void __set_bit(uint16_t *val,uint8_t bit);
static void __clr_bit(uint16_t *val,uint8_t bit);


static void __set_bit(uint16_t *val,uint8_t bit)
{
	*val |= (1 << bit);
}

static void __clr_bit(uint16_t *val,uint8_t bit)
{
	*val &= ~(1 << bit);
}

// PORT 1
void sync_ctrl_port1(void)
{
	GPIOA->ODR=CTRL_MASK|g_gpio_ctrl_table[0]；
}
void reset_ctrl_port1(void)
{
	GPIOA->ODR=CTRL_MASK|gpio_ctrl_table_reset[0]；
}

// PORT 2
void sync_ctrl_port2(void)
{
	GPIOA->ODR=CTRL_MASK|g_gpio_ctrl_table[1]；
}
void reset_ctrl_port2(void)
{
	GPIOA->ODR=CTRL_MASK|gpio_ctrl_table_reset[1]；
}

// PORT 3
void sync_ctrl_port3(void)
{
	GPIOA->ODR=CTRL_MASK|g_gpio_ctrl_table[2]；
}
void reset_ctrl_port3(void)
{
	GPIOA->ODR=CTRL_MASK|gpio_ctrl_table_reset[2]；
}

// PORT 4
void sync_ctrl_port4(void)
{
	GPIOA->ODR=CTRL_MASK|g_gpio_ctrl_table[3]；
}
void reset_ctrl_port4(void)
{
	GPIOA->ODR=CTRL_MASK|gpio_ctrl_table_reset[3]；
}

// PORT 5
void sync_ctrl_port5(void)
{
	GPIOA->ODR=CTRL_MASK|g_gpio_ctrl_table[0]；
}
void reset_ctrl_port5(void)
{
	GPIOA->ODR=CTRL_MASK|gpio_ctrl_table_reset[0]；
}

// PORT 6
void sync_ctrl_port6(void)
{
	GPIOI->ODR=CTRL_MASK|g_gpio_ctrl_table[5]；
}
void reset_ctrl_port6(void)
{
	GPIOI->ODR=CTRL_MASK|gpio_ctrl_table_reset[5]；
}


// PORT 7
void sync_ctrl_port7(void)
{
	GPIOJ->ODR=CTRL_MASK|g_gpio_ctrl_table[6]；
}
void reset_ctrl_port7(void)
{
	GPIOJ->ODR=CTRL_MASK|gpio_ctrl_table_reset[6]；
}

// PORT 8
void sync_ctrl_port8(void)
{
	GPIOC->ODR=CTRL_MASK|g_gpio_ctrl_table[7]；
}
void reset_ctrl_port8(void)
{
	GPIOC->ODR=CTRL_MASK|gpio_ctrl_table_reset[7]；
}

void gpio_convert_all(void)
{
	uint8_t i=0;
	
	for(i=0;i<8;i++)
	{
		gpio_convert_one(g_gpio_ctrl_table_raw[i],&g_gpio_ctrl_table[i]);
	}
}

void gpio_convert_one(uint8_t *sdat,uint16_t *ddat)
{
	// *sdat *sdat+1 *sdat+2
}


