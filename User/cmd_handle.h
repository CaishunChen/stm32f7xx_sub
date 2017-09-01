#ifndef _cmd_handle_H_
#define _cmd_handle_H_

#include <stdint.h>
#include "stm32f7xx.h"	
#include "spi_handle.h"

typedef enum 
{
	NGVAM_STATUS_OK=0x11,
	NGVAM_STATUS_ERROR=0x00
}NGVAM_STATUS_TYPE;


typedef enum
{
	NGVAM_ACK_OK = 0xA0,
	NGVAM_ACK_ERROR_HANDLE =0XA1,
	NGVAM_ACK_ERROR_TYPE = 0xA2,
	NGVAM_ACK_ERROR_DATA =0XA3,
	NGVAM_ACK_CHECK = 0XA4,
	NGVAM_ACK_ERROR_OTHERS = 0XA5
}NGVAM_ACK_TYPE;

typedef enum
{
	NGVAM_MESSAGE_TYPE_PLAY = 0xA1,
	NGVAM_MESSAGE_TYPE_TAB,
	NGVAM_MESSAGE_TYPE_CTRL,
	NGVAM_MESSAGE_TYPE_READ
}NGVAM_MESSAGE_TYPE;

typedef enum
{
	TABLE_TYPE_AMPLITUDE = 1,
	TABLE_TYPE_PHASE = 0
}NGVAM_TABLE_TYPE;

/*
typedef enum
{
	TABLE_RX_PROCESS,
	TABLE_RX_COMPLETED
}NGVAM_TABLE_RX_STATE;
*/

typedef struct
{
	uint8_t moudle;
	uint8_t freq;
	uint8_t channel;
	uint8_t table_type;
	uint8_t packet_id;
}NGVAM_MSG_TAB;

extern NGVAM_MSG_TAB g_msg_tab;
extern NGVAM_MSG_TAB *gp_msg_tab;


void analyze_msg(void);

#endif//_cmd_handle_H_

