#include "flash_handle.h"

/*QSPI Status Bits
typedef enum
{
  QSPI_OK				= ((uint8_t)0x00),
  QSPI_ERROR			= ((uint8_t)0x01),
  QSPI_BUSY				= ((uint8_t)0x02),
  QSPI_NOT_SUPPORTED	= ((uint8_t)0x03),
  QSPI_SUSPENDED		= ((uint8_t)0x04),
  QSPI_OUT_TIME			= ((uint8_t)0x05),
  QSPI_WRITE_ENABLE_ERROR	  = ((uint8_t)0x06),
  
} QSPI_StaticTypeDef;*/


#ifdef DEBUG
#define DBG_LOG(x) printf x
#else
#define DBG_LOG(x) 
#endif



const uint32_t c_flash_store_tab[7][8]=
{
	{(FREQ0_ADDR_BASE|CHANNEL0_BASE),(FREQ0_ADDR_BASE|CHANNEL1_BASE),(FREQ0_ADDR_BASE|CHANNEL2_BASE),(FREQ0_ADDR_BASE|CHANNEL3_BASE),	//freq0
	(FREQ0_ADDR_BASE|CHANNEL4_BASE),(FREQ0_ADDR_BASE|CHANNEL5_BASE),(FREQ0_ADDR_BASE|CHANNEL6_BASE),(FREQ0_ADDR_BASE|CHANNEL7_BASE)},

	{(FREQ1_ADDR_BASE|CHANNEL0_BASE),(FREQ1_ADDR_BASE|CHANNEL1_BASE),(FREQ1_ADDR_BASE|CHANNEL2_BASE),(FREQ1_ADDR_BASE|CHANNEL3_BASE), //freq1
	(FREQ1_ADDR_BASE|CHANNEL4_BASE),(FREQ1_ADDR_BASE|CHANNEL5_BASE),(FREQ1_ADDR_BASE|CHANNEL6_BASE),(FREQ1_ADDR_BASE|CHANNEL7_BASE)},

	{(FREQ2_ADDR_BASE|CHANNEL0_BASE),(FREQ2_ADDR_BASE|CHANNEL1_BASE),(FREQ2_ADDR_BASE|CHANNEL2_BASE),(FREQ2_ADDR_BASE|CHANNEL3_BASE),	//freq2
	(FREQ2_ADDR_BASE|CHANNEL4_BASE),(FREQ2_ADDR_BASE|CHANNEL5_BASE),(FREQ2_ADDR_BASE|CHANNEL6_BASE),(FREQ2_ADDR_BASE|CHANNEL7_BASE)},

	{(FREQ3_ADDR_BASE|CHANNEL0_BASE),(FREQ3_ADDR_BASE|CHANNEL1_BASE),(FREQ3_ADDR_BASE|CHANNEL2_BASE),(FREQ3_ADDR_BASE|CHANNEL3_BASE),	//frqe3
	(FREQ3_ADDR_BASE|CHANNEL4_BASE),(FREQ3_ADDR_BASE|CHANNEL5_BASE),(FREQ3_ADDR_BASE|CHANNEL6_BASE),(FREQ3_ADDR_BASE|CHANNEL7_BASE)},

	{(FREQ4_ADDR_BASE|CHANNEL0_BASE),(FREQ4_ADDR_BASE|CHANNEL1_BASE),(FREQ4_ADDR_BASE|CHANNEL2_BASE),(FREQ4_ADDR_BASE|CHANNEL3_BASE),	//freq4
	(FREQ4_ADDR_BASE|CHANNEL4_BASE),(FREQ4_ADDR_BASE|CHANNEL5_BASE),(FREQ4_ADDR_BASE|CHANNEL6_BASE),(FREQ4_ADDR_BASE|CHANNEL7_BASE)},

	{(FREQ5_ADDR_BASE|CHANNEL0_BASE),(FREQ5_ADDR_BASE|CHANNEL1_BASE),(FREQ5_ADDR_BASE|CHANNEL2_BASE),(FREQ5_ADDR_BASE|CHANNEL3_BASE),	//freq5
	(FREQ5_ADDR_BASE|CHANNEL4_BASE),(FREQ5_ADDR_BASE|CHANNEL5_BASE),(FREQ5_ADDR_BASE|CHANNEL6_BASE),(FREQ5_ADDR_BASE|CHANNEL7_BASE)},

	{(FREQ6_ADDR_BASE|CHANNEL0_BASE),(FREQ6_ADDR_BASE|CHANNEL1_BASE),(FREQ6_ADDR_BASE|CHANNEL2_BASE),(FREQ6_ADDR_BASE|CHANNEL3_BASE),	//freq6
	(FREQ6_ADDR_BASE|CHANNEL4_BASE),(FREQ6_ADDR_BASE|CHANNEL5_BASE),(FREQ6_ADDR_BASE|CHANNEL6_BASE),(FREQ6_ADDR_BASE|CHANNEL7_BASE)}
};


#ifdef DEBUG
void print_all_address(void)
{
	uint8_t i,j;
	DBG_LOG(("all address are:\r\n"));
	for(i=0;i<MAX_CHANNEL;i++)
	{
		for(j=0;j<MAX_FREQ;j++)
		{
			DBG_LOG(("%.7X ",get_flash_address(i,j)));
		}
		DBG_LOG(("\r\n"));
	}
}
#endif


uint32_t get_flash_address(uint8_t freq,uint8_t channel)
{
	if( freq <=6 && channel <=7)
	{
//		DBG_LOG(("get flash addr:%.7X\r\n",c_flash_store_tab[freq][channel]));
		return c_flash_store_tab[freq][channel];
	}
	else
	{
		DBG_LOG(("get flash error\r\n"));
		return 0xFFFFFFFF;
	}
}

 
QSPI_StaticTypeDef flash_read(uint8_t freq,uint8_t channel,uint8_t *p_a_data,uint8_t *p_p_data)
{
	uint32_t addr=0;
	QSPI_StaticTypeDef sta=(QSPI_StaticTypeDef)0xFF;
	addr=get_flash_address(freq,channel);
	sta=QSPI_ReadBuff(p_a_data,addr|AMPLITUDE_STORE_OFFSET,AMPLITUDE_STORE_SIZE);
	if(sta != QSPI_OK)
	{
		DBG_LOG(("read amplitude %.7X error\r\n",addr));
		return sta;
	}
	sta=QSPI_ReadBuff(p_p_data,addr|PHASE_STORE_OFFSET,PHASE_STORE_SIZE);
	if(sta != QSPI_OK)
	{
		DBG_LOG(("read phase %.7X error\r\n",addr));
		return sta;
	}
	return QSPI_OK;
}

QSPI_StaticTypeDef flash_write(uint8_t freq,uint8_t channel,uint8_t *p_a_data,uint8_t *p_p_data)
{
	uint32_t addr=0;
	QSPI_StaticTypeDef sta=(QSPI_StaticTypeDef)0xFF;
	addr=get_flash_address(freq,channel);
	DBG_LOG(("write addr is %.7X\r\n",addr));
	sta=QSPI_EraseSector_32K(addr >> 15 );
	if(sta != QSPI_OK)
	{
		DBG_LOG(("sector %d erase error\r\n",addr));
		return sta;
	}
	sta=QSPI_WriteBuff(p_a_data,addr|AMPLITUDE_STORE_OFFSET,AMPLITUDE_STORE_SIZE);
	if(sta != QSPI_OK)
	{
		DBG_LOG(("write amplitude %.7X error\r\n",addr));
		return sta;
	}
	sta=QSPI_WriteBuff(p_p_data,addr|PHASE_STORE_OFFSET,PHASE_STORE_SIZE);
	if(sta != QSPI_OK)
	{
		DBG_LOG(("write phase %.7X error\r\n",addr));
		return sta;
	}
	 return QSPI_OK;
}


QSPI_StaticTypeDef flash_clean(uint8_t freq,uint8_t channel)
{
	uint32_t addr=0;
	QSPI_StaticTypeDef sta=(QSPI_StaticTypeDef)0xFF;
	addr=get_flash_address(freq,channel);
	DBG_LOG(("write addr is %.7X\r\n",addr));
	sta=QSPI_EraseSector_32K(addr >> 15 );
	if(sta != QSPI_OK)
	{
		DBG_LOG(("sector %d erase error\r\n",addr));
		return sta;
	}
	return QSPI_OK;
}




