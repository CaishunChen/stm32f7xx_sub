#ifndef _cmd_handle_H_
#define _cmd_handle_H_

#include <stdint.h>
#include "stm32f7xx.h"	
#include "spi_handle.h"

typedef enum 
{
	NGVAM_STATUS_OK=0x11,
	NGVAM_STATUS_ERROR=0x00
}NGVAM_STATUS;


typedef enum
{
	NGVAM_ACK_OK = 0xA0,
	NGVAM_ACK_ERROR_HANDLE =0XA1,
	NGVAM_ACK_ERROR_TYPE = 0xA2,
	NGVAM_ACK_ERROR_DATA =0XA3,
	NGVAM_ACK_CHECK = 0XA4,
	NGVAM_ACK_ERROR_OTHERS = 0XA5
}NGVAM_ACK;

typedef enum
{
	NGVAM_MESSAGE_TYPE_PLAY = 0xA1,
	NGVAM_MESSAGE_TYPE_TAB,
	NGVAM_MESSAGE_TYPE_CTRL,
	NGVAM_MESSAGE_TYPE_READ
}NGVAM_MESSAGE_TYPE;


#endif//_cmd_handle_H_

