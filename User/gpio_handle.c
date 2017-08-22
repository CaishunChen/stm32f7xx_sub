#include "gpio_handle.h"
#include "gpio.h"

#ifdef DEBUG
#define DBG_LOG(x) printf x
#else
#define DBG_LOG(x) 
#endif

#define CTRL_MASK (0x1FFF) //控制输出掩码

//此表储存8组IO口的默认值
const uint16_t gpio_ctrl_table_reset[8]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
//const uint32_t gpio_ctrl_table_base[8]={GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOI,GPIOJ,GPIOC};
uint16_t g_gpio_ctrl_table[8]={0};
type_gpio_ctrl g_gpio_ctrl_table_raw[8]={0};

static void __set_bit(uint16_t *val,uint16_t bit);
static void __clr_bit(uint16_t *val,uint16_t bit);
static __IO void __delay(void);


static __IO void __delay(void)
{
	__IO uint32_t i=0x100;//0x100 1us 
	while(i--);
}

static void __set_bit(uint16_t *val,uint16_t bit)
{
	*val |= (1 << bit);
}

static void __clr_bit(uint16_t *val,uint16_t bit)
{
	*val &= ~(1 << bit);
}

// PORT 1  PA  PB5
void sync_ctrl_port1(void)
{
	GPIOA->ODR=CTRL_MASK&g_gpio_ctrl_table[0];
	GPIOB->BSRR = GPIO_PIN_5;//set
	__delay();
	GPIOB->BSRR = (uint32_t)GPIO_PIN_5 << 16;//reset
	
}
void reset_ctrl_port1(void)
{	
	GPIOA->ODR=CTRL_MASK&gpio_ctrl_table_reset[0];
	GPIOB->BSRR = GPIO_PIN_5;//set
	__delay();
	GPIOB->BSRR = (uint32_t)GPIO_PIN_5 << 16;//reset
}

// PORT 2 PA  PB6
void sync_ctrl_port2(void)
{
	GPIOA->ODR=CTRL_MASK&g_gpio_ctrl_table[1];
	GPIOB->BSRR = GPIO_PIN_6;//set
	__delay();
	GPIOB->BSRR = (uint32_t)GPIO_PIN_6 << 16;//reset
}
void reset_ctrl_port2(void)
{
	GPIOA->ODR=CTRL_MASK&gpio_ctrl_table_reset[1];
	GPIOB->BSRR = GPIO_PIN_6;//set
	__delay();
	GPIOB->BSRR = (uint32_t)GPIO_PIN_6 << 16;//reset
}

// PORT 3 PA  PB7
void sync_ctrl_port3(void)
{
	GPIOA->ODR=CTRL_MASK&g_gpio_ctrl_table[2];
	GPIOB->BSRR = GPIO_PIN_7;//set
	__delay();
	GPIOB->BSRR = (uint32_t)GPIO_PIN_7 << 16;//reset
}
void reset_ctrl_port3(void)
{
	GPIOA->ODR=CTRL_MASK&gpio_ctrl_table_reset[2];
	GPIOB->BSRR = GPIO_PIN_7;//set
	__delay();
	GPIOB->BSRR = (uint32_t)GPIO_PIN_7 << 16;//reset
}

// PORT 4 PA  PB8
void sync_ctrl_port4(void)
{
	GPIOA->ODR=CTRL_MASK&g_gpio_ctrl_table[3];
	GPIOB->BSRR = GPIO_PIN_8;//set
	__delay();
	GPIOB->BSRR = (uint32_t)GPIO_PIN_8 << 16;//reset
}
void reset_ctrl_port4(void)
{
	GPIOA->ODR=CTRL_MASK&gpio_ctrl_table_reset[3];
	GPIOB->BSRR = GPIO_PIN_8;//set
	__delay();
	GPIOB->BSRR = (uint32_t)GPIO_PIN_8 << 16;//reset
}

// PORT 5 PA  PB9
void sync_ctrl_port5(void)
{
	GPIOA->ODR=CTRL_MASK&g_gpio_ctrl_table[4];
	GPIOB->BSRR = GPIO_PIN_9;//set
	__delay();
	GPIOB->BSRR = (uint32_t)GPIO_PIN_9 << 16;//reset
}
void reset_ctrl_port5(void)
{
	GPIOA->ODR=CTRL_MASK&gpio_ctrl_table_reset[4]; 
	GPIOB->BSRR = GPIO_PIN_9;//set
	__delay();
	GPIOB->BSRR = (uint32_t)GPIO_PIN_9 << 16;//reset
}

// PORT 6
void sync_ctrl_port6(void)
{
	GPIOI->ODR=CTRL_MASK&g_gpio_ctrl_table[5];
	if(g_gpio_ctrl_table[5]&0x04) GPIOE->BSRR= GPIO_PIN_2;else GPIOE->BSRR= (uint32_t)GPIO_PIN_2<< 16;
	if(g_gpio_ctrl_table[5]&0x10) GPIOE->BSRR= GPIO_PIN_4;else GPIOE->BSRR= (uint32_t)GPIO_PIN_4<< 16;
	DBG_LOG(("GPIOI output %.4X\r\n",CTRL_MASK&g_gpio_ctrl_table[5]));
}
void reset_ctrl_port6(void)
{
	GPIOI->ODR=CTRL_MASK&gpio_ctrl_table_reset[5];
	if(g_gpio_ctrl_table[5]&0x04) GPIOE->BSRR= GPIO_PIN_2;else GPIOE->BSRR= (uint32_t)GPIO_PIN_2<< 16;
	if(g_gpio_ctrl_table[5]&0x10) GPIOE->BSRR= GPIO_PIN_4;else GPIOE->BSRR= (uint32_t)GPIO_PIN_4<< 16;
	DBG_LOG(("GPIOI reset %.4X\r\n",CTRL_MASK&gpio_ctrl_table_reset[5]));
}


// PORT 7
void sync_ctrl_port7(void)
{
	GPIOJ->ODR=CTRL_MASK&g_gpio_ctrl_table[6];
}
void reset_ctrl_port7(void)
{
	GPIOJ->ODR=CTRL_MASK&gpio_ctrl_table_reset[6];
}

// PORT 8
void sync_ctrl_port8(void)
{
	GPIOC->ODR=CTRL_MASK&g_gpio_ctrl_table[7];
}
void reset_ctrl_port8(void)
{
	GPIOC->ODR=CTRL_MASK&gpio_ctrl_table_reset[7]; 
}

void gpio_convert_all(void)
{
	uint8_t i=0;
	
	for(i=0;i<8;i++)
	{
		gpio_convert_one(&g_gpio_ctrl_table_raw[i],&g_gpio_ctrl_table[i]);
		DBG_LOG(("now g_gpio_ctrl_table[%d] is %.4X \r\n",i,g_gpio_ctrl_table[i]));
	}
}

void gpio_convert_one(type_gpio_ctrl *sdat,uint16_t *ddat)
{
	//sdat.a  sdat.p 
	// 	00		V1  	V2  	V3  	V4 		V5 		V6 		V1 		V2 		V3  		V4 		V5  	V6 
	//bit 		1			2			3			4			5			6			7			8			9				10		11		12	 
	//raw		16		0.5			2			-------------------31.5------------		4			1			8	
	//cal		16000	500			2000	------------------31500------------		4000	1000	8000
	//ctr_bit	11		10		9			-------------------12--------------		2			1			0	(1 1100 0000 1111B)													
	uint32_t temp_a=sdat->a;
	uint32_t temp_p=sdat->p;
	uint16_t res=0x01F8;


	if(temp_a >= 31500)	{temp_a -=31500;__set_bit(&res,12);}//DBG_LOG(("\t\t\tset bit12\r\n"));}
	if(temp_a >= 16000)	{temp_a -=16000;__set_bit(&res,0);}
	if(temp_a >= 8000)	{temp_a -=8000;	__set_bit(&res,11);}
	if(temp_a >= 4000)	{temp_a -=4000;	__set_bit(&res,9);}
	if(temp_a >= 2000)	{temp_a -=2000;	__set_bit(&res,2);}
	if(temp_a >= 1000)	{temp_a -=1000;	__set_bit(&res,10);}
	if(temp_a >= 500)	  {temp_a -=500;	__set_bit(&res,1);}


	//bit 			1A			2A		3A		4A		5A		6A		//B = !A
	//RAW				180			45		5.625	11.25	22.5	90
	//cal				180000	45000	5625	11250	22500	90000
	//ctrl_bit	8				7			6			5			4			3	
	if(temp_p >= 180000){temp_p -= 180000;__clr_bit(&res,8);}
	if(temp_p >= 90000)	{temp_p -= 90000;__clr_bit(&res,3);}
	if(temp_p >= 45000)	{temp_p -= 45000;__clr_bit(&res,7);}
	if(temp_p >= 22500)	{temp_p -= 22500;__clr_bit(&res,4);}
	if(temp_p >= 11250)	{temp_p -= 11250;__clr_bit(&res,5);}
	if(temp_p >= 5625)	{temp_p -=  5625;__clr_bit(&res,6);}
	
	*ddat=res;
	
}

void reset_ctrl_all(void)
{
	reset_ctrl_port1();
	reset_ctrl_port2();
	reset_ctrl_port3();
	reset_ctrl_port4();

	reset_ctrl_port5();
	reset_ctrl_port6();
	reset_ctrl_port7();
	reset_ctrl_port8();
	DBG_LOG(("all ctrl are reset\r\n"));
}


void sync_ctrl_all(void)
{
	sync_ctrl_port1();
	sync_ctrl_port2();
	sync_ctrl_port3();
	sync_ctrl_port4();
	
	sync_ctrl_port5();
	#if 1
	sync_ctrl_port6();
	sync_ctrl_port7();
	sync_ctrl_port8();
	#endif
	DBG_LOG(("all ctrl are update\r\n"));
}



