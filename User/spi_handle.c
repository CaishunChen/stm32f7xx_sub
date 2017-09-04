#include "spi_handle.h"
#include "spi.h"
#include "cmd_handle.h"

#ifdef DEBUG
#define DBG_LOG(x) printf x
#else
#define DBG_LOG(x) 
#endif

#define SPI2_RX_CMD_SIZE 		5
#define SPI2_RX_BUFFER_SIZE 256
#define NULL 0

enum spi_rx_state{spi_idle=0,spi_rx_cmd,spi_rx_data,spi_reply};//global
enum msg_flag{msg_to_process,process_completed};

/*global variable*/
uint8_t g_spi_state;
uint8_t g_spi_rx_buffer[SPI2_RX_BUFFER_SIZE]={0};
uint8_t g_spi_rx_cmd[5]={0};
uint8_t  *gp_rx_u8=g_spi_rx_cmd;
uint16_t *gp_rx_u16=(uint16_t *)g_spi_rx_cmd;
uint8_t g_msg_to_process;

NGVAM_MSG_HEAD g_msg;//传表消息头 
NGVAM_MSG_HEAD *gp_msg=&g_msg;

static void __handle_spi_idle(SPI_HandleTypeDef *hspi);
static void __handle_spi_rx_data(SPI_HandleTypeDef *hspi);
static void __handle_spi_reply(SPI_HandleTypeDef *hspi);
static void __handle_spi_rx_cmd(SPI_HandleTypeDef *hspi);
static void __spi_busy(void);
static void __spi_idle(void);
static void read_delay(void);
static void ready_to_read(void);


void spi_rx_isr(SPI_HandleTypeDef *hspi)
{
	//to add rx data from spi2
	//DBG_LOG(("enter spi2 int\r\n"));
	switch (g_spi_state)
		{
		case spi_idle:
			__handle_spi_idle(hspi);
			break;
		case spi_rx_cmd:
		__handle_spi_rx_cmd(hspi);
			break;
		case spi_rx_data:
			__handle_spi_rx_data(hspi);
			break;
		/*case spi_reply:
			__handle_spi_reply(hspi);
			break;*/
		default:break;
	}
	;
}


static void __handle_spi_idle(SPI_HandleTypeDef *hspi)
{
	uint8_t i=0;
	//DBG_LOG(("__handle_spi2_idle\r\n"));
	for(i=0;i<hspi->RxXferSize;i++)
	{
		DBG_LOG((" %.2X",g_spi_rx_buffer[i]));
	}
	analyze_msg();
	__spi_idle();
	__to_spi_rx_cmd(hspi);
	__HAL_SPI_ENABLE_IT(&hspi2, (SPI_IT_RXNE | SPI_IT_ERR));
	__HAL_SPI_ENABLE(&hspi2);
	//while(1);
}

static void __handle_spi_rx_data(SPI_HandleTypeDef *hspi)
{
	//DBG_LOG(("__handle_spi2_rx_data\r\n"));
//	DBG_LOG(("%d->%d\r",hspi->RxXferCount,hspi->RxXferSize));
	if(hspi->RxXferCount < hspi->RxXferSize)
	{
		*(hspi->pRxBuffPtr++)=*((__IO uint8_t *)&SPI2->DR);//get data
		hspi->RxXferCount++;
		//DBG_LOG(("*"));
	}
	if(hspi->RxXferCount == hspi->RxXferSize)
	{
		__spi_busy();
		__to_spi_idle(hspi);
		__handle_spi_idle(hspi);
		// to cmd.c 
	}
}

static void __handle_spi_rx_cmd(SPI_HandleTypeDef *hspi)
{
	//DBG_LOG(("__handle_spi2_rx_cmd\r\n"));
	//DBG_LOG(("%d %d\r\n",hspi->RxXferCount,hspi->RxXferSize));
	if(hspi->RxXferCount < hspi->RxXferSize)
	{
		*(hspi->pRxBuffPtr++)=*((__IO uint8_t *)&SPI2->DR);
		hspi->RxXferCount++;
		//DBG_LOG(("+"));
	}
	else
	{
		//DBG_LOG(("%x %x %x %x %x",g_spi_rx_cmd[0],g_spi_rx_cmd[1],g_spi_rx_cmd[2],g_spi_rx_cmd[3],g_spi_rx_cmd[4]));
	}
	#if 1
	
	if(hspi->RxXferCount == 5)//
	{
		//check
		//DBG_LOG(("check head\r\n"));
		if(*gp_rx_u16 == 0x5A5A)
		{
			gp_msg->msg_len = *(gp_rx_u16 + 1);//len
			if(gp_msg->msg_len >255) gp_msg->msg_len = 255;//limit len 
			gp_msg->msg_type = *(gp_rx_u8 + 4);//type
			gp_msg->msg_ptr = g_spi_rx_buffer;
			//DBG_LOG(("%d-%X\r\n",gp_msg->msg_len,gp_msg->msg_type));
//			DBG_LOG(("__handle_spi2_rx_cmd_completed,to rec data\r\n"));
			__to_spi_rx_data(hspi);
		}
		else
		{
			__to_spi_rx_cmd(hspi);
		}
	}
	#endif
}


static void __handle_spi_reply(SPI_HandleTypeDef *hspi)
{
	DBG_LOG(("__handle_spi2_reply\r\n"));
	ready_to_read();
}


//准备接收数据
void __to_spi_rx_data(SPI_HandleTypeDef *hspi)
{
	//DBG_LOG(("__to_spi2_rx_data\r\n"));
	hspi->RxXferCount=0;
	hspi->RxXferSize=(gp_msg->msg_len)+5;
	hspi->pRxBuffPtr=g_spi_rx_buffer;
	g_spi_state=spi_rx_data;
}

//准备接收命令
void __to_spi_rx_cmd(SPI_HandleTypeDef *hspi)
{
	hspi->RxXferCount=0;
	hspi->RxXferSize=5;
	hspi->pRxBuffPtr=g_spi_rx_cmd;
	g_spi_state=spi_rx_cmd;
}

//准备进入空闲
void __to_spi_idle(SPI_HandleTypeDef *hspi)
{
	
	__HAL_SPI_DISABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));
	__HAL_SPI_DISABLE(hspi);
	DBG_LOG(("__to_spi2_idle\r\n"));
	g_spi_state = spi_idle;
}

void spi_int_config(SPI_HandleTypeDef *hspi,void (*RxISR)(struct __SPI_HandleTypeDef *hspi))
{
	DBG_LOG(("config spi2 int\r\n"));
	hspi->RxISR=RxISR;
	__to_spi_rx_cmd(hspi);
	__HAL_SPI_ENABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));
	__HAL_SPI_ENABLE(hspi);
	__spi_idle();
}

static void __spi_busy(void)
{
	GPIOH->BSRR = (uint32_t)GPIO_PIN_15;
}

static void __spi_idle(void)
{
	GPIOH->BSRR = (uint32_t)GPIO_PIN_15 << 16;
}

static void ready_to_read(void)
{
	SPI2->DR =0xAA;//
	GPIOH->BSRR = (uint32_t)GPIO_PIN_14;
	read_delay();
	GPIOH->BSRR = (uint32_t)GPIO_PIN_14 << 16;
}

static void read_delay(void)
{
	__IO uint16_t i=0x10;
	while(i--);
}

