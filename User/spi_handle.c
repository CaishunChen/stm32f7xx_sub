#include "spi_handle.h"
#include "spi.h"

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

NGVAM_MSG g_msg;
NGVAM_MSG *gp_msg=&g_msg;

static void __handle_spi_idle(SPI_HandleTypeDef *hspi);
static void __handle_spi_rx_data(SPI_HandleTypeDef *hspi);
static void __handle_spi_reply(SPI_HandleTypeDef *hspi);
static void __handle_spi_rx_cmd(SPI_HandleTypeDef *hspi);
static void __spi_busy(void);
static void __spi_idle(void);

void spi_rx_isr(SPI_HandleTypeDef *hspi)
{
	hspi=hspi;//
	//to add rx data from spi2
	DBG_LOG(("enter spi2 int\r\n"));
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
	DBG_LOG(("__handle_spi2_idle\r\n"));

}

static void __handle_spi_rx_data(SPI_HandleTypeDef *hspi)
{
	DBG_LOG(("__handle_spi2_rx_data\r\n"));
	if(hspi->RxXferCount < hspi->RxXferSize)
	{
		*(hspi->pRxBuffPtr++)=*((__IO uint8_t *)(SPI2->DR));//get data
		hspi->RxXferCount++;
	}
	if(hspi->RxXferCount == (gp_msg->msg_len)+2)
	{
		__spi_busy();
		__to_spi_idle(hspi);
		//disable int 
	__HAL_SPI_DISABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));
	__HAL_SPI_DISABLE(hspi);
		// to cmd.c 
	}
}

static void __handle_spi_rx_cmd(SPI_HandleTypeDef *hspi)
{
	DBG_LOG(("__handle_spi2_rx_cmd\r\n"));
	if(hspi->RxXferCount < hspi->RxXferSize)
	{
		*(hspi->pRxBuffPtr++)=*((__IO uint8_t *)(SPI2->DR));//get data
		hspi->RxXferCount++;
	}
	
	if(hspi->RxXferCount == 5)//
	{
		//check
		if(*gp_rx_u16 == 0x5A5A)
		{
			gp_msg->msg_len = *(gp_rx_u16 + 1);//len
			if(gp_msg->msg_len >255) gp_msg->msg_len = 255;//limit len 
			gp_msg->msg_type = *(gp_rx_u8 + 4);//type
			gp_msg->msg_ptr = (gp_rx_u8 + 5);
			DBG_LOG(("__handle_spi2_rx_cmd_completed,to rec data\r\n"));
			gp_msg->valid = msg_valid;
			__to_spi_rx_data(hspi);
		}
		else
		{
			// frame error
			gp_msg->valid = msg_invalid;
		}
	}
}


static void __handle_spi_reply(SPI_HandleTypeDef *hspi)
{
	DBG_LOG(("__handle_spi2_reply\r\n"));
}

void __to_spi_rx_data(SPI_HandleTypeDef *hspi)
{
	hspi->RxXferCount=0;
	hspi->RxXferSize=(gp_msg->msg_len)+2;
	hspi->pRxBuffPtr=g_spi_rx_buffer;
	g_spi_state=spi_rx_data;
}

void __to_spi_rx_cmd(SPI_HandleTypeDef *hspi)
{
	hspi->RxXferCount=0;
	hspi->RxXferSize=5;
	hspi->pRxBuffPtr=g_spi_rx_cmd;
	g_spi_state=spi_rx_cmd;

}

void __to_spi_idle(SPI_HandleTypeDef *hspi)
{
	g_spi_state = spi_idle;
}

void spi_int_config(SPI_HandleTypeDef *hspi,void (*RxISR)(struct __SPI_HandleTypeDef *hspi))
{
	__to_spi_rx_cmd(hspi);
	hspi->RxISR=RxISR;
	gp_msg->valid = msg_invalid;
}

static void __spi_busy(void)
{
	;
}

static void __spi_idle(void)
{
	;
}

uint8_t cal_check_val(void)
{
	return 0;
}


