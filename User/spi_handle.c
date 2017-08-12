#include "spi_handle.h"
#include "spi.h"

#ifdef DEBUG
#define DBG_LOG(x) printf x
#else
#define DBG_LOG(x) 
#endif

#define SPI2_RX_BUFFER_size 1024
#define NULL 0

enum spi_rx_state{spi_idle=0,spi_rx_cmd,spi_rx_data,spi_reply};//global

/*global variable*/
uint8_t g_spi_state;
uint8_t g_spi_rx_buffer[SPI2_RX_BUFFER_size]={"0"};

static void __handle_spi_idle(SPI_HandleTypeDef *hspi);
static void __handle_spi_rx_data(SPI_HandleTypeDef *hspi);
static void __handle_spi_reply(SPI_HandleTypeDef *hspi);

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
		case spi_rx_data:
			__handle_spi_rx_data(hspi);
			break;
		case spi_reply:
			__handle_spi_reply(hspi);
			break;
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
	}
}

static void __handle_spi_reply(SPI_HandleTypeDef *hspi)
{
	DBG_LOG(("__handle_spi2_reply\r\n"));
}

void __to_spi_rx_data(SPI_HandleTypeDef *hspi)
{
	hspi->RxXferCount=0;
	hspi->RxXferSize=0;
	hspi->pRxBuffPtr=g_spi_rx_buffer;
	g_spi_state=spi_rx_data;
}

void spi_int_config(SPI_HandleTypeDef *hspi,void (*RxISR)(struct __SPI_HandleTypeDef *hspi))
{
	hspi->RxXferCount=0;
	hspi->RxXferSize=0;
	hspi->pRxBuffPtr=NULL;//g_spi_rx_buffer;
	hspi->RxISR=RxISR;
}


