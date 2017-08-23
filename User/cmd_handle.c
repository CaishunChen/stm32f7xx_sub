#include "cmd_handle.h"
#include "spi.h"


#ifdef DEBUG
#define DBG_LOG(x) printf x
#else
#define DBG_LOG(x) 
#endif


static void __handle_play(void);
static void __handle_tab(void);
static void __handle_ctrl(void);
static void __handle_read(void);




void analyze_msg_type(void)
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
			DBG_LOG(("msg tyoe err\r\n"));
			break;
		}
		
	}
}

static void __handle_play(void)
{
	DBG_LOG(("__handle_play\r\n"));
	__HAL_SPI_ENABLE_IT(&hspi2, (SPI_IT_RXNE | SPI_IT_ERR));
	__HAL_SPI_ENABLE(&hspi2);

}

static void __handle_tab(void)
{
	DBG_LOG(("__handle_tab\r\n"));
}

static void __handle_ctrl(void)
{
	DBG_LOG(("__handle_ctrl\r\n"));

}

 static void __handle_read(void)
 {
	 DBG_LOG(("__handle_read\r\n"));

 }



