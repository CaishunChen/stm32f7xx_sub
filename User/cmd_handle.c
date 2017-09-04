#include "cmd_handle.h"
#include "spi.h"
#include "string.h"
#include "gpio_handle.h"
#include "flash_handle.h"

#ifdef DEBUG
#define DBG_LOG(x) printf x
#else
#define DBG_LOG(x) 
#endif

NGVAM_MSG_TAB g_msg_tab;
NGVAM_MSG_TAB *gp_msg_tab=&g_msg_tab;

uint8_t g_rx_tab_tmp[10][120]={0};

/*
NGVAM_TABLE_RX_STATE g_rx_tab_a_sta=TABLE_RX_PROCESS;//flag of rx tab[a/p]
NGVAM_TABLE_RX_STATE g_rx_tab_p_sta=TABLE_RX_PROCESS;
*/

static void __handle_play(void);
static void __handle_tab(void);
static void __handle_ctrl(void);
static void __handle_read(void);


void analyze_msg(void)//analyze msg and return to spi_idle
{
	switch (gp_msg->msg_type)
	{
		case NGVAM_MESSAGE_TYPE_PLAY:
		{
			__handle_play();
			break;
		}
		
		case NGVAM_MESSAGE_TYPE_TAB:
		{
			__handle_tab();
			break;
		}
		
		case NGVAM_MESSAGE_TYPE_CTRL:
		{
			__handle_ctrl();
			break;
		}
		
		case NGVAM_MESSAGE_TYPE_READ:
		{
			__handle_read();
			break;
		}
		
		default :
		{
			DBG_LOG(("msg type err\r\n"));
			break;
		}
		
	}
}

static void __handle_play(void)
{
	uint16_t i=0;
	
	#ifdef DEBUG
	uint32_t *p=(uint32_t *)g_gpio_ctrl_table_raw;//-
	#endif 
	
	DBG_LOG(("__handle_play\r\n"));
	memcpy(g_gpio_ctrl_table_raw,gp_msg->msg_ptr+5,64);
	for(i=0;i<16;i++)
	{
		DBG_LOG(("%.2d:%.8X\r\n",i,*p++));
	}
	gpio_convert_all();
	sync_ctrl_all();
	//__to_spi_rx_cmd(&hspi2);

}

static void __handle_tab(void)
{
	/*static uint16_t g_msg_tab_cnt=0;*/
	uint8_t now_packet_cnt,all_packet_cnt;
	DBG_LOG(("__handle_tab\r\n"));
	memcpy(gp_msg_tab,gp_msg->msg_ptr,5);

	now_packet_cnt = gp_msg_tab->packet_id&0x0F;
	all_packet_cnt = (gp_msg_tab->packet_id&0xF0)>>4;
	DBG_LOG(("(%d/%d)\r\n",now_packet_cnt,all_packet_cnt));
	
	if(now_packet_cnt <all_packet_cnt)// start mid
	{
		memcpy(g_rx_tab_tmp[now_packet_cnt],gp_msg->msg_ptr+5,gp_msg->msg_len);
				
	}

	else if(now_packet_cnt == all_packet_cnt)//end 
	{
		memcpy(g_rx_tab_tmp[now_packet_cnt],gp_msg->msg_ptr+5,gp_msg->msg_len);
		//to flash
		//flash_clean(gp_msg_tab->freq,gp_msg_tab->channel);
		
		if(gp_msg_tab->table_type == TABLE_TYPE_AMPLITUDE)
		{
			DBG_LOG(("write amplitude to flash,f:%dch:%d,type%d\r\n",gp_msg_tab->freq,gp_msg_tab->channel,gp_msg_tab->table_type));
			flash_write_amplitude(gp_msg_tab->freq,gp_msg_tab->channel,(uint8_t *)g_rx_tab_tmp);
		}
		else if(gp_msg_tab->table_type == TABLE_TYPE_PHASE)
		{
			flash_write_phase(gp_msg_tab->freq,gp_msg_tab->channel,(uint8_t *)g_rx_tab_tmp);
		}
	}


	 
}

static void __handle_ctrl(void)
{
	DBG_LOG(("__handle_ctrl\r\n"));
	uint16_t i=0;
	
	#ifdef DEBUG
	uint32_t *p=(uint32_t *)g_gpio_ctrl_table_raw;//-
	#endif 
	
	DBG_LOG(("__handle_play\r\n"));
	memcpy(g_gpio_ctrl_table_raw,gp_msg->msg_ptr+5,64);
	for(i=0;i<16;i++)
	{
		DBG_LOG(("%.2d:%.8X\r\n",i,*p++));
	}
	gpio_convert_all();
	sync_ctrl_all();
	__HAL_SPI_ENABLE_IT(&hspi2, (SPI_IT_RXNE | SPI_IT_ERR));
	__HAL_SPI_ENABLE(&hspi2);
	//__to_spi_rx_cmd(&hspi2);

}

 static void __handle_read(void)
 {
	 DBG_LOG(("__handle_read\r\n"));

 }

 uint32_t check_msg(uint32_t *ptr,uint16_t len)
 {
	return 0;
 }



