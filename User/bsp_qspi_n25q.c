
/*
********************************************************************************************************
MT25Q QSPI Flash Configuration File


STM32F7��quad-spi�ӿ���Ҫ�ص㣺
��1�����ֹ���ģʽ
��2��Dual-Flashģʽ������ͬʱ����ƬFlash������CLK��CSƬѡ�ߡ������������ͬʱ����8λ���ݣ�4+4��
��3��֧��SDR��DDR
��4�����ģʽ��DMAͨ��
��5����Ƕ���պͷ���FIFO
��6��֧��FIFO threshold, timeout, operation complete, access error�����ж�



---------------------------------------------------------------------------------------
     ��2 �������Զ�ʶ���������
=======================================================================================
����                   ����                               ����

READ             �����ݣ�CMD=0x03                  ����ͨ��D0����������ͨ��D0����

FAST_READ        ���ٶ����ݣ�CMD=0xB               ����ͨ��D0����������ͨ��D0����

DOR              ˫IO�����ݣ�CMD=0x3B              ����ͨ��D0����������ͨ��D[1:0]����

QOR              ��IO�߶����ݣ�CMD=0x6B            ����ͨ��D0����������ͨ��D[3:0]����

DIOR             ˫IO�����IO���ݣ�CMD=BB        ����ͨ��D[1:0]����������ͨ��D[3:0]����

QIOR             ��IO�����IO���ݣ�CMD=EB        ����ͨ��D[3:0]����������ͨ��D[3:0]����

PP               ҳ������CMD=02                ����ͨ��D0����������ͨ��D0����

QPP              ��IOҳ������CMD=32��38        ����ͨ��D0����������ͨ��D0[3:0]����




********************************************************************************************************
*/

#ifdef DEBUG
#define DBG_LOG(x) printf x
#else
#define DBG_LOG(x) 
#endif

#include "bsp_qspi_n25q.h"
#include "quadspi.h"

QSPI_Information  _QspiFlashInf;

static uint8_t QSPI_WorkMode = N25Q_SPI_MODE;		//QSPIģʽ��־:0,SPIģʽ;1,QPIģʽ.

static QSPI_StaticTypeDef QSPI_WriteEnable(QSPI_HandleTypeDef *handle);
//static QSPI_StaticTypeDef QSPI_WriteDisable(QSPI_HandleTypeDef *handle);
static QSPI_StaticTypeDef QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *handle);

static QSPI_StaticTypeDef QSPI_EnterFourBytesAddress(QSPI_HandleTypeDef *hqspi);
static QSPI_StaticTypeDef QSPI_Receive(uint8_t * _pBuf, uint32_t _NumByteToRead);
static QSPI_StaticTypeDef QSPI_Transmit(uint8_t * _pBuf, uint32_t _NumByteToRead);
static QSPI_StaticTypeDef QSPI_SendCmdData( uint8_t  __Instruction,       //  ����ָ��
                                             uint32_t __InstructionMode,   //  ָ��ģʽ
                                             uint32_t __AddressMode,       //  ��ַģʽ
                                             uint32_t __AddressSize,       //  ��ַ����  
                                             uint32_t __DataMode,          //  ����ģʽ
                                             uint32_t __NbData,            //  ���ݶ�д�ֽ���
                                             uint32_t __DummyCycles,       //  ���ÿ�ָ��������
                                             uint32_t __Address,           //  ���͵���Ŀ�ĵ�ַ
                                             uint8_t  *_pBuf,             //  �����͵�����
                                             __SEND_CMD_DATA_T  _SendCmdDat
                                           );

QSPI_StaticTypeDef QSPI_Read_SR(uint8_t ReadReg, uint8_t * RegValue, uint8_t ReadRegNum);


/*
**************************************************************************************
�������ƣ�QSPI_UserInit
�������ܣ�QSPI ��ʼ����IO�ڳ�ʼ��������QSPI оƬ�������Ϣ��������
������   ��
����ֵ�� QSPI_OK ��ʼ���ɹ�������ֵʧ��
**************************************************************************************
*/

QSPI_StaticTypeDef QSPI_UserInit(void)
{
  uint32_t i;
  uint8_t  _RegVal;
  uint8_t  QspiID[3];
  __IO QSPI_StaticTypeDef  __QspiStatus = QSPI_OUT_TIME;
  
	__QspiStatus=QSPI_WriteEnable(&hqspi);
	if ( __QspiStatus != QSPI_OK)   
	{
    return QSPI_ERROR;
	}
	
  #if 0
  	// QSPI memory reset  �ڵ���ʱ�������� ��ʽ��֧�����¿��ػ��������� 
  __QspiStatus = QSPI_ResetMemory(&hqspi);
	if ( __QspiStatus != QSPI_OK)   // ����������ܹ�ʹ�ã������Ῠ���
	{
    return QSPI_ERROR;
	}
	
	#endif
  
  // ���Ͻ��ܵ� �ȴ� tSHSL3 ʱ��
  i = 0x20000;
  while(i --)
  {
		;
  }
  
	
  __QspiStatus = QSPI_Quad_Enter();
  if(__QspiStatus != QSPI_OK)
  {
    return __QspiStatus;  
  }
  DBG_LOG(("quad enter...\r\n"));
  
  if(QSPI_WorkMode == N25Q_QUAD_MODE)
  {
    if(QSPI_Read_ID(QSPI_MULTIPLE_IO_READ_ID_CMD, (uint8_t *) & QspiID[0], 3) != QSPI_OK)
      return QSPI_ERROR;
  }
  else
  {
    if(QSPI_Read_ID(QSPI_READ_ID_CMD, (uint8_t *) & QspiID[0], 3) != QSPI_OK)
      return QSPI_ERROR;  
  }
    
  
  _QspiFlashInf.Id = (QspiID[0] << 16) | (QspiID[1] << 8) | QspiID[2];

  
  switch(_QspiFlashInf.Id)
  {
    case QSPI_N25Q512_JEDEC_ID:   // N25Q512A QSPI Chip  0x20BA20
    case QSPI_N25Q256_JEDEC_ID:   // N25Q256A QSPI Chip  0x20BA19
    case QSPI_MT25Q1GB_JEDEC_ID:  //MT25Q1GB  QSPI Chip  0x20BA21
    {
      if(QSPI_Read_SR(QSPI_READ_FLAG_STATUS_REG_CMD, &_RegVal , 1) == QSPI_OK)   // ��ȡ 0x70�Ĵ������鿴�Ƿ�Ϊ4�ֽڵ�ַ
      {
        if((_RegVal & 0x01) == 0x00)       //�������4�ֽڵ�ַģʽ,�����4�ֽڵ�ַģʽ
        {
          __QspiStatus = QSPI_EnterFourBytesAddress(&hqspi);   // ����4�ֽڵ�ַ
          if(__QspiStatus != QSPI_OK)
          {
            return QSPI_ERROR;  
          }
        }
        
        QSPI_GetInformation( &_QspiFlashInf );    
      }
      break ;
    }

    default : 
      {
        return QSPI_ERROR;  
      }
  }
  
  
  if(_QspiFlashInf.Id == QSPI_MT25Q1GB_JEDEC_ID)
  {
    _QspiFlashInf.FlashTotalSize /= 1024;
    DBG_LOG(("QSPI MT25Q1GB Capacity %d MByte ... \r\n", _QspiFlashInf.FlashTotalSize / 1024));
    DBG_LOG(("QSPI MT25Q1GB ID  0x%X ...\r\n", _QspiFlashInf.Id));
		DBG_LOG(("QSPI MT25Q1GB FlashTotalSize 0x%X ...\r\n", _QspiFlashInf.FlashTotalSize));
		DBG_LOG(("QSPI MT25Q1GB SectorSize 0x%X ...\r\n", _QspiFlashInf.SectorSize));
		DBG_LOG(("QSPI MT25Q1GB EraseSectorSize 0x%X ...\r\n", _QspiFlashInf.EraseSectorSize));
		DBG_LOG(("QSPI MT25Q1GB EraseSectorsNumber 0x%X ...\r\n", _QspiFlashInf.EraseSectorsNumber));
		DBG_LOG(("QSPI MT25Q1GB ProgPageSize 0x%X ...\r\n", _QspiFlashInf.ProgPageSize));
		DBG_LOG(("QSPI MT25Q1GB ProgPagesNumber 0x%X ... \r\n", _QspiFlashInf.ProgPagesNumber));
  } 
    DBG_LOG(("QSPI_UserInit OK ...\r\n\r\n"));
  return QSPI_OK;
}




/*
int QSPI_Read(uint8_t* data, uint32_t address, uint32_t size)	--Read data from QSPI flash
A function to read data from QSPI flash
Return an integer value (default QSPI_OK), a parameter for data buffer, 
a parameter for QSPI memory address, a parameter for data size.
*/

QSPI_StaticTypeDef QSPI_ReadBuff(uint8_t* data, uint32_t address, uint32_t size)
{
  uint32_t  __InstructionMode, __AddressMode, __DataMode;
  uint8_t _RegVal = 0;
  
  if(QSPI_WorkMode)   // Work In QUAD Model
  {
    __InstructionMode = QSPI_INSTRUCTION_4_LINES;
    __AddressMode     = QSPI_ADDRESS_4_LINES;
    __DataMode        = QSPI_DATA_4_LINES;
  }
  else
  {
    __InstructionMode = QSPI_INSTRUCTION_1_LINE;
    __AddressMode     = QSPI_ADDRESS_1_LINE;
    __DataMode        = QSPI_DATA_1_LINE;  
  } 
  
        
  //QUAD,���ٶ�����,��ַΪaddress,4�ߴ�������_32λ��ַ_4�ߴ����ַ_4�ߴ���ָ��,8������,NumByteToRead������
  if(QSPI_SendCmdData(  QSPI_QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD /*QSPI_FAST_READ_4_BYTE_ADDR_CMD*/ /*QSPI_FAST_READ_4_BYTE_ADDR_CMD*/ /*QSPI_QUAD_INOUT_FAST_READ_CMD*/  /*QSPI_QUAD_INOUT_FAST_READ_CMD*/ ,        // _Instruction,      ����ָ��
                        __InstructionMode,               // _InstructionMode,  ָ��ģʽ
                        __AddressMode,                   // _AddressMode,      ��ַģʽ
                        QSPI_ADDRESS_32_BITS,            // _AddressSize,      ��ַ����  
                        __DataMode,                      // _DataMode,         ����ģʽ
                        size,                            // _NbData,           ���ݶ�д�ֽ���
                        10 ,                             // _DummyCycles,      ���ÿ�ָ�������� �� QSPI_SET_READ_PARAM ���ָ�����õ�ֵһ��
                        address,                         // _Address,          ���͵���Ŀ�ĵ�ַ
                        &_RegVal,                        //  *_pBuf,           ��ȡ�����ݣ��˴�û��ʹ��
                        QSPI_SEND_CMD                   // __SEND_CMD_DATA_T  _SendCmdDat
                     ) != QSPI_OK )
  {
    return QSPI_ERROR;
  }    
  
  if(QSPI_Receive(  data,  size) != QSPI_OK)
    return  QSPI_ERROR;  
  return  QSPI_OK;

}



/*
**************************************************************************************
�������ƣ�QSPI_Read_SR
�������ܣ���ȡ�Ĵ���ֵ
������    ReadReg ��Ҫ��ȡ�ļĴ���
          @ QSPI_READ_ENHANCED_VOL_CFG_REG_CMD, QSPI_READ_EXT_ADDR_REG_CMD, QSPI_READ_NONVOL_CFG_REG_CMD
            QSPI_READ_FLAG_STATUS_REG_CMD, QSPI_READ_STATUS_REG_CMD  ... ...
          * RegValue  �Ĵ���ֵ
          ReadRegNum  ��ȡ�Ĵ����ֽ���
����ֵ��QSPI_OK��ȡ�ɹ�������ʧ��
**************************************************************************************
*/

QSPI_StaticTypeDef QSPI_Read_SR(uint8_t ReadReg, uint8_t * RegValue, uint8_t ReadRegNum)
{
  uint32_t  __InstructionMode, __DataMode;
  
  if(QSPI_WorkMode)   // Work In QUAD Model
  {
    __InstructionMode = QSPI_INSTRUCTION_4_LINES;
    __DataMode        = QSPI_DATA_4_LINES;
  }
  else
  {
    __InstructionMode = QSPI_INSTRUCTION_1_LINE;
    __DataMode        = QSPI_DATA_1_LINE;  
  }   
  
  
  if(QSPI_SendCmdData(  ReadReg,                // _Instruction,      ����ָ��
                        __InstructionMode,      // _InstructionMode,  ָ��ģʽ
                        QSPI_ADDRESS_NONE,      // _AddressMode,      ��ַģʽ
                        QSPI_ADDRESS_8_BITS,    // _AddressSize,      ��ַ����  
                        __DataMode,             // _DataMode,         ����ģʽ
                        ReadRegNum,             // _NbData,           ���ݶ�д�ֽ���
                        0,                      // _DummyCycles,      ���ÿ�ָ��������
                        0,                      // _Address,          ���͵���Ŀ�ĵ�ַ
                        RegValue,               //  *_pBuf,           �����͵�����
                        QSPI_SEND_CMD           // __SEND_CMD_DATA_T  _SendCmdDat
                     ) != QSPI_OK )
  {
    return QSPI_ERROR;
  }
  
  if(QSPI_Receive( RegValue , ReadRegNum) != QSPI_OK)
    return QSPI_ERROR;

	return QSPI_OK;
}


/*
**************************************************************************************
�������ƣ�QSPI_Write_SR
�������ܣ�д�Ĵ���ֵ
������    WriteReg ��Ҫд��ļĴ���
          @ QSPI_WRITE_ENHANCED_VOL_CFG_REG_CMD, QSPI_WRITE_EXT_ADDR_REG_CMD, QSPI_WRITE_NONVOL_CFG_REG_CMD
            QSPI_WRITE_FLAG_STATUS_REG_CMD, QSPI_WRITE_STATUS_REG_CMD ... ...
          RegValue    ��д��ļĴ���ֵ
          WriteRegNum  д��ļĴ������ֽ���
����ֵ��QSPI_OK��ȡ�ɹ�������ʧ��
**************************************************************************************
*/
QSPI_StaticTypeDef QSPI_Write_SR(uint8_t WriteReg, uint8_t  RegValue, uint8_t WriteRegNum)
{
  
  uint32_t  __InstructionMode, __DataMode;
  
  if(QSPI_WriteEnable(&hqspi) != QSPI_OK)
    return QSPI_ERROR;  
  
  if(QSPI_WorkMode)
  {
    __InstructionMode = QSPI_INSTRUCTION_4_LINES;
    __DataMode        = QSPI_DATA_4_LINES;
  }
  else
  {
    __InstructionMode = QSPI_INSTRUCTION_1_LINE;
    __DataMode        = QSPI_DATA_1_LINE;  
  } 

  if(QSPI_SendCmdData(  WriteReg,               // _Instruction,      ����ָ��
                        __InstructionMode,      // _InstructionMode,  ָ��ģʽ
                        QSPI_ADDRESS_NONE,      // _AddressMode,      ��ַģʽ
                        QSPI_ADDRESS_8_BITS,    // _AddressSize,      ��ַ����  
                        __DataMode,             // _DataMode,         ����ģʽ
                        WriteRegNum,            // _NbData,           ���ݶ�д�ֽ���
                        0,                      // _DummyCycles,      ���ÿ�ָ��������
                        0,                      // _Address,          ���͵���Ŀ�ĵ�ַ
                        &RegValue,              //  *_pBuf,           �����͵�����
                        QSPI_SEND_DAT           // __SEND_CMD_DATA_T  _SendCmdDat
                     ) != QSPI_OK )
  {
    return QSPI_ERROR;
  } 

  if(QSPI_AutoPollingMemReady(&hqspi) != QSPI_OK)
    return QSPI_ERROR;
  
  return QSPI_OK;
}



/*
**************************************************************************************
�������ƣ�QSPI_Read_ID
�������ܣ���ȡ ID ֵ
������    ReadID ��Ҫ��ȡ��ID�Ĵ���ָ��
          * _pIdBuf  ��ȡID��Ϣ�����ݻ�����
         ReadIdNum  ��Ҫ��ȡid �������ֽ�����
����ֵ��QSPI_OK��ȡ�ɹ�������ʧ��
**************************************************************************************
*/

QSPI_StaticTypeDef QSPI_Read_ID(uint8_t ReadID, uint8_t * _pIdBuf, uint8_t ReadIdNum)
{
  
  uint32_t  __InstructionMode, __DataMode;
  
  if(QSPI_WorkMode)   // Work In QUAD Model
  {
    __InstructionMode = QSPI_INSTRUCTION_4_LINES;
    __DataMode        = QSPI_DATA_4_LINES;
		DBG_LOG(("worked in 4 lines mode...\r\n"));
  }
  else
  {
    __InstructionMode = QSPI_INSTRUCTION_1_LINE;
    __DataMode        = QSPI_DATA_1_LINE;  
		DBG_LOG(("worked in 1 lines mode...\r\n"));
  }   
  

  if(QSPI_SendCmdData(  ReadID,                   // _Instruction,      ����ָ��
                        __InstructionMode,        // _InstructionMode,  ָ��ģʽ
                        QSPI_ADDRESS_NONE,        // _AddressMode,      ��ַģʽ
                        QSPI_ADDRESS_8_BITS,      // _AddressSize,      ��ַ����  
                        __DataMode,               // _DataMode,         ����ģʽ
                        ReadIdNum,                // _NbData,           ���ݶ�д�ֽ���
                        0,                        // _DummyCycles,      ���ÿ�ָ��������
                        0,                        // _Address,          ���͵���Ŀ�ĵ�ַ
                        &_pIdBuf[0],              //  *_pBuf,           �����͵�����
                        QSPI_SEND_CMD             // __SEND_CMD_DATA_T  _SendCmdDat
                     ) != QSPI_OK )
  {
    return QSPI_ERROR;
  }

  
  if(QSPI_Receive( (uint8_t * ) _pIdBuf , ReadIdNum) != QSPI_OK)
    return QSPI_ERROR;  
  
	return QSPI_OK;
}

/*
**************************************************************************************
�������ƣ�QSPI_Quad_Enter
�������ܣ����� QUAD ģʽ
����ֵ��QSPI_OK��ȡ�ɹ�������ֵʧ��
**************************************************************************************
*/
QSPI_StaticTypeDef QSPI_Quad_Enter(void)
{
  uint8_t _RegVal[2] = {0xaa, 0xaa};


  QSPI_WorkMode = N25Q_SPI_MODE;
  
  if(QSPI_Read_SR(QSPI_READ_ENHANCED_VOL_CFG_REG_CMD, &_RegVal[0], 1) != QSPI_OK )   // read reg 0x65
  {
    return QSPI_ERROR;
  }  
  
  if(_RegVal[0] == 0xff)   // оƬ��ʼֵ = 0xDF
  {
    _RegVal[0] = 0x2F;   // bit7=0,bit6=1 QUAD ģʽ, bit5 ϵͳ�����ұ�������Ϊ0///guangqiang �˴������
    if(QSPI_Write_SR(QSPI_WRITE_ENHANCED_VOL_CFG_REG_CMD, _RegVal[0], 1) == QSPI_OK)  // ����Ϊ QUAD ģʽ reg 0x61
    {
      QSPI_WorkMode = N25Q_QUAD_MODE;
      
      
      _RegVal [0]= 0xaa;
      QSPI_Read_SR(QSPI_READ_ENHANCED_VOL_CFG_REG_CMD, &_RegVal[0], 1);  // ���Զ�������ֵ��д���ֵ�Ƿ�һ�£������ 0x1F  ˵��ok
      DBG_LOG(("QSPI_READ_VOL_CFG_REG_CMD 2 =  0x%X\r\n", _RegVal[0]));    
      
      QSPI_Read_SR(QSPI_READ_VOL_CFG_REG_CMD, &_RegVal[0],1);  // 0xFB ˵��ok
      
    }
  }
  else
  {
    DBG_LOG(("QSPI N25Qxx Chip Error  ... ... \r\n"));
    return QSPI_ERROR;
  }

  return QSPI_OK;  
}


/*
**************************************************************************************
�������ƣ�QSPI_WritePageByte
���������� page д����дǰ�����Ȳ�����Ӧ����������֧�ֿ�ҳд
������_pBuf          ���ݻ�����
      _uiWriteAddr   д��ĵ�ַ
      _size          д�����ݴ�С,ÿ������ܹ�д��256�ֽ�
����ֵ��QSPI_OK ��ʾ�ɹ�������ʧ��
**************************************************************************************
*/
QSPI_StaticTypeDef QSPI_WritePageByte(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint32_t _size)
{
  uint32_t  __InstructionMode, __AddressMode, __DataMode;
  
	if (QSPI_WriteEnable(&hqspi) != QSPI_OK)
	{
		return QSPI_ERROR;
	}  
  
  if(QSPI_WorkMode)   // Work In QUAD Model
  {
    /*
    
       QSPI_EXT_QUAD_IN_FAST_PROG_CMD  д�� 8192*4 ��Ҫ 2018ms
       QSPI_PAGE_PROG_CMD              д�� 8192*4 ��Ҫ 2035ms
       QSPI_PAGE_PROG_4_BYTE_ADDR_CMD  д�� 8192*4 ��Ҫ 2029ms
       QSPI_QUAD_IN_FAST_PROG_CMD      д�� 8192*4 ��Ҫ 2014ms - 2023ms
       QSPI_EXT_QUAD_IN_FAST_PROG_CMD  д�� 8192*4 ��Ҫ 2025ms

    */
    __InstructionMode = QSPI_INSTRUCTION_4_LINES;
    __AddressMode     = QSPI_ADDRESS_4_LINES;
    __DataMode        = QSPI_DATA_4_LINES;
  }
  else
  {
    __InstructionMode = QSPI_INSTRUCTION_1_LINE;
    __AddressMode     = QSPI_ADDRESS_1_LINE;
    __DataMode        = QSPI_DATA_1_LINE;  
  }   
  
  if(QSPI_SendCmdData(  QSPI_PAGE_PROG_4_BYTE_ADDR_CMD,   //QSPI_QUAD_IN_FAST_PROG_CMD, //QSPI_PAGE_PROG_4_BYTE_ADDR_CMD, QSPI_PAGE_PROG_CMD,         // _Instruction,      ����ָ��
                        __InstructionMode,                // _InstructionMode,  ָ��ģʽ
                        __AddressMode,                    // _AddressMode,      ��ַģʽ
                        QSPI_ADDRESS_32_BITS,             // _AddressSize,      ��ַ����  
                        __DataMode,                       // _DataMode,         ����ģʽ
                        _size,                            // _NbData,           ���ݶ�д�ֽ���
                        0,                                // _DummyCycles,      ���ÿ�ָ��������
                        _uiWriteAddr,                     // _Address,          ���͵���Ŀ�ĵ�ַ
                        (uint8_t *) _pBuf,               //  *_pBuf,           �����͵�����
                        QSPI_SEND_DAT                    // __SEND_CMD_DATA_T  _SendCmdDat
                     ) != QSPI_OK )
  {
    return QSPI_ERROR;
  }   
    
  // Configure automatic polling mode to wait for end of program ----- 
  if(QSPI_AutoPollingMemReady(&hqspi) != QSPI_OK)
    return QSPI_ERROR;
  
  return QSPI_OK;
}



/*
**************************************************************************************
�������ƣ�QSPI_WriteBuff
����������֧�ֿ� page д����дǰ�����Ȳ�����Ӧ������
������_pBuf          ���ݻ�����
      _uiWriteAddr   д��ĵ�ַ
      _size          д�����ݴ�С,
����ֵ��QSPI_OK ��ʾ�ɹ�������ʧ��
**************************************************************************************
*/
QSPI_StaticTypeDef QSPI_WriteBuff(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint32_t _size)
{
  __IO uint32_t end_addr;
  __IO uint32_t current_size, current_addr=0;

	//Calculation of the size between the write address and the end of the page
	current_addr = 0;

	while (current_addr <= _uiWriteAddr)
	{
		current_addr += QSPI_PAGE_SIZE;
	}
	
	current_size = current_addr - _uiWriteAddr;

	//Check if the size of the data is less than the remaining place in the page
	if (current_size > _size)
	{
		current_size = _size;
	}
	
	//Initialize the adress variables
	current_addr = _uiWriteAddr;
	end_addr = _uiWriteAddr + _size;

	//Perform the write page by page
	do
	{

    if(QSPI_WritePageByte( (uint8_t *) _pBuf, current_addr, current_size) != QSPI_OK)
      return QSPI_ERROR;  
        
		//Update the address and size variables for next page programming
		current_addr += current_size;
		_pBuf += current_size;
		current_size = ((current_addr + QSPI_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : QSPI_PAGE_SIZE;
	} while (current_addr < end_addr);

	return QSPI_OK;
}


/*
**************************************************************************************
�������ƣ�QSPI_Write_NoCheck
���������� �޼���д QSPI FLASH, ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��
           д������ݽ�ʧ��! �����Զ���ҳ����, ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫ
           ȷ����ַ��Խ��!
������_pBuf          ���ݻ�����
      _uiWriteAddr   д��ĵ�ַ
      _NumByteToWrite Ҫд����ֽ���(���65535)
����ֵ��QSPI_OK ��ʾ�ɹ�������ʧ��
**************************************************************************************
*/
QSPI_StaticTypeDef QSPI_Write_NoCheck(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint32_t  _NumByteToWrite)   
{ 			 		 
	uint32_t pageremain;	   
	pageremain = 256 - (_uiWriteAddr % QSPI_PAGE_SIZE); //��ҳʣ����ֽ���		 	    
	if(_NumByteToWrite <= pageremain)
    pageremain = _NumByteToWrite;           //������256���ֽ�
	while(1)
	{   
		if(QSPI_WritePageByte(_pBuf, _uiWriteAddr, pageremain) != QSPI_OK)
      return QSPI_ERROR;
    
		if(_NumByteToWrite == pageremain)         //д�������
      break;
    
	 	else                                      //NumByteToWrite>pageremain
		{
			_pBuf        += pageremain;
			_uiWriteAddr += pageremain;	

			_NumByteToWrite -= pageremain;			    //��ȥ�Ѿ�д���˵��ֽ���
			if(_NumByteToWrite > QSPI_PAGE_SIZE)
        pageremain = QSPI_PAGE_SIZE;          //һ�ο���д��256���ֽ�
			else 
        pageremain = _NumByteToWrite; 	      //����256���ֽ���
		}
	} 
  return QSPI_OK;
}


/*
**************************************************************************************
�������ƣ�QSPI_WriteBuffAutoEraseSector
������������ָ����ַ��ʼд��ָ�����ȵ�����, �ú��������������������� !
������_pBuf           ���ݻ�����
      _uiWriteAddr    д��ĵ�ַ
      _NumByteToWrite д�����ݴ�С,
����ֵ��QSPI_OK ��ʾ�ɹ�������ʧ��
**************************************************************************************
*/
#if 1
#pragma pack(4)
static uint8_t __packed g_tQSpiBuf[4*1024];
#pragma pack()
#endif

QSPI_StaticTypeDef QSPI_WriteBuffAutoEraseSector(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint32_t _NumByteToWrite)
{
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * _pStr = g_tQSpiBuf; 
     
 	secpos = _uiWriteAddr / 4096;           //������ַ  
	secoff = _uiWriteAddr % 4096;           //�������ڵ�ƫ��
	secremain = 4096 - secoff;              //����ʣ��ռ��С   

 	if(_NumByteToWrite <= secremain)
    secremain = _NumByteToWrite;          //������4096���ֽ�
	while(1) 
	{	
		if(QSPI_ReadBuff(_pStr, (secpos * 4096), 4096) !=  QSPI_OK)  //������������������
      return QSPI_ERROR;
    
		for(i=0;i<secremain;i++)//У������
		{
			if(_pStr[secoff+i] != 0XFF)
        break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			if(QSPI_EraseSector_4K(secpos) !=  QSPI_OK)   //�����������
        return QSPI_ERROR;
      
			for(i=0;i<secremain;i++)	   //����
			{
				_pStr[i+secoff] = _pBuf[i];	  
			}
			if(QSPI_Write_NoCheck(_pStr, (secpos * 4096), 4096) !=  QSPI_OK)    //д���������� 
        return QSPI_ERROR;
		}
    else 
    {
      if(QSPI_Write_NoCheck(_pBuf, _uiWriteAddr, secremain) !=  QSPI_OK)     //д�Ѿ������˵�,ֱ��д������ʣ������. 	
        return QSPI_ERROR;
    }
    
		if(_NumByteToWrite == secremain)  break;  //д�������
    
		else                                    //д��δ����
		{
			secpos ++;                            //������ַ��1
			secoff = 0;                           //ƫ��λ��Ϊ0 	 

		  _pBuf           += secremain;         //ָ��ƫ��
			_uiWriteAddr    += secremain;         //д��ַƫ��	   
		  _NumByteToWrite -= secremain;				  //�ֽ����ݼ�
      
			if(_NumByteToWrite > 4096)          // 4096
        secremain = 4096;	                  //��һ����������д����
			else 
        secremain = _NumByteToWrite;			    //��һ����������д����
		}	 
	} ;
	return QSPI_OK;
}




/*
int QSPI_Erase_Block(uint32_t Sector_address)	--Earse a QSPI flash Sector
A function to earse a QSPI flash Sector
Return an integer value (default QSPI_OK), a parameter for block address.
*/
QSPI_StaticTypeDef QSPI_EraseSector_4K(uint32_t Sector_address)
{
  uint8_t _RegVal = 0;
  uint32_t  __InstructionMode, __AddressMode;
  
	if (QSPI_WriteEnable(&hqspi) != QSPI_OK)
	{
		return QSPI_ERROR;
	}    
  
  if(QSPI_WorkMode)   // Work In QUAD Model
  {
    __InstructionMode = QSPI_INSTRUCTION_4_LINES;
    __AddressMode     = QSPI_ADDRESS_4_LINES;
  }
  else
  {
    __InstructionMode = QSPI_INSTRUCTION_1_LINE;
    __AddressMode     = QSPI_ADDRESS_1_LINE;  
  }   

  Sector_address <<= 12;  

  if(QSPI_SendCmdData(  QSPI_SUBSECTOR_4K_ERASE_CMD,   // _Instruction,      ����ָ��
                        __InstructionMode,          // _InstructionMode,  ָ��ģʽ
                        __AddressMode,              // _AddressMode,      ��ַģʽ
                        QSPI_ADDRESS_32_BITS,       // _AddressSize,      ��ַ����  
                        QSPI_DATA_NONE,             // _DataMode,         ����ģʽ
                        0,                          // _NbData,           ���ݶ�д�ֽ���
                        0,                          // _DummyCycles,      ���ÿ�ָ��������
                        Sector_address,             // _Address,          ���͵���Ŀ�ĵ�ַ
                        &_RegVal,                   //  *_pBuf,           �����͵�����
                        QSPI_SEND_CMD               // __SEND_CMD_DATA_T  _SendCmdDat
                     ) != QSPI_OK )
  {
    return QSPI_ERROR;
  }   
 
  if(QSPI_AutoPollingMemReady(&hqspi) != QSPI_OK)
    return QSPI_ERROR;  
  
  return QSPI_OK;  
}




/*
int QSPI_Erase_Block(uint32_t Sector_address)	--Earse a QSPI flash Sector
A function to earse a QSPI flash Sector
Return an integer value (default QSPI_OK), a parameter for block address.

*/
QSPI_StaticTypeDef QSPI_EraseSector_32K(uint32_t Sector_address)
{
  uint8_t _RegVal = 0;
  uint32_t  __InstructionMode, __AddressMode;
  
	if (QSPI_WriteEnable(&hqspi) != QSPI_OK)
	{
		return QSPI_ERROR;
	}    
  
  if(QSPI_WorkMode)   // Work In QUAD Model
  {
    __InstructionMode = QSPI_INSTRUCTION_4_LINES;
    __AddressMode     = QSPI_ADDRESS_4_LINES;
  }
  else
  {
    __InstructionMode = QSPI_INSTRUCTION_1_LINE;
    __AddressMode     = QSPI_ADDRESS_1_LINE;  
  }   

  Sector_address <<= 15;  

  if(QSPI_SendCmdData( QSPI_SUBSECTOR_32K_ERASE_CMD,   // _Instruction,      ����ָ��
  
                        __InstructionMode,          // _InstructionMode,  ָ��ģʽ
                        __AddressMode,              // _AddressMode,      ��ַģʽ
                        QSPI_ADDRESS_32_BITS,       // _AddressSize,      ��ַ����  
                        QSPI_DATA_NONE,             // _DataMode,         ����ģʽ
                        0,                          // _NbData,           ���ݶ�д�ֽ���
                        0,                          // _DummyCycles,      ���ÿ�ָ��������
                        Sector_address,             // _Address,          ���͵���Ŀ�ĵ�ַ
                        &_RegVal,                   //  *_pBuf,           �����͵�����
                        QSPI_SEND_CMD               // __SEND_CMD_DATA_T  _SendCmdDat
                     ) != QSPI_OK )
  {
    return QSPI_ERROR;
  }   
  if(QSPI_AutoPollingMemReady(&hqspi) != QSPI_OK)
    return QSPI_ERROR;  
  
  return QSPI_OK;  
}



/*
int QSPI_Erase_Chip(void)	--Earse QSPI flash full chip
A function to earse QSPI flash full chip
Return an integer value (default QSPI_OK), no parameters.

���� N25Q256A13xx ����оƬʱ�� 233879ms ����, �ӽ�4����
*/
QSPI_StaticTypeDef QSPI_EraseChip(void)
{
  uint32_t  __InstructionMode ;
  uint8_t _RegVal = 0;
	//Enable write operations
	if (QSPI_WriteEnable(&hqspi) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

  if(QSPI_WorkMode)   // Work In QUAD Model
  {
    __InstructionMode = QSPI_INSTRUCTION_4_LINES;
  }
  else
  {
    __InstructionMode = QSPI_INSTRUCTION_1_LINE; 
  }   
  
  if(QSPI_SendCmdData(  QSPI_BULK_ERASE_CMD,        // _Instruction,      ����ָ��
                        __InstructionMode,          // _InstructionMode,  ָ��ģʽ
                        QSPI_ADDRESS_NONE,          // _AddressMode,      ��ַģʽ
                        QSPI_ADDRESS_8_BITS,        // _AddressSize,      ��ַ����  
                        QSPI_DATA_NONE,             // _DataMode,         ����ģʽ
                        0,                          // _NbData,           ���ݶ�д�ֽ���
                        0,                          // _DummyCycles,      ���ÿ�ָ��������
                        0,                          // _Address,          ���͵���Ŀ�ĵ�ַ
                        &_RegVal,                   //  *_pBuf,           �����͵�����
                        QSPI_SEND_CMD               // __SEND_CMD_DATA_T  _SendCmdDat
                     ) != QSPI_OK )
  {
    return QSPI_ERROR;
  }
  
  
  if(QSPI_AutoPollingMemReady(&hqspi) != QSPI_OK)
    return QSPI_ERROR;  
  
  return QSPI_OK;  
}



/*
int QSPI_GetStatus(void)	--Get QSPI flash's status
A function to get QSPI flash's status
Return an integer value (no default value), no parameters.
���� ok
*/
/*
QSPI_StaticTypeDef QSPI_GetStatus(void)
{
	QSPI_CommandTypeDef qspi_cmd;
	uint8_t reg;
  __IO uint32_t i=0;

	//Initialize the read flag status register command
	qspi_cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	qspi_cmd.Instruction       = QSPI_READ_FLAG_STATUS_REG_CMD;
	qspi_cmd.AddressMode       = QSPI_ADDRESS_NONE;
	qspi_cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	qspi_cmd.DataMode          = QSPI_DATA_1_LINE;
	qspi_cmd.DummyCycles       = 0;
	qspi_cmd.NbData            = 1;
	qspi_cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	qspi_cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	qspi_cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//Configure the command
	if (HAL_QSPI_Command(&hqspi, &qspi_cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

  do{
  
    i ++;
    
    //Reception of the data
    if (HAL_QSPI_Receive(&hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return QSPI_ERROR;
    }    
    
    if ((reg & QSPI_FSR_READY) != 0)  //Check the value of the register
    {
      return QSPI_OK;
    }    
  } while(i < QSPI_WAIT_MAX_TIME );
  
  
	//Check the value of the register
	if ((reg & (QSPI_FSR_PRERR | QSPI_FSR_VPPERR | QSPI_FSR_PGERR | QSPI_FSR_ERERR)) != 0)
	{
		return QSPI_ERROR;
	}
	else if ((reg & (QSPI_FSR_PGSUS | QSPI_FSR_ERSUS)) != 0)
	{
		return QSPI_SUSPENDED;
	}
	else
	{
		return QSPI_BUSY;
	}  
}
*/

/*
int QSPI_GetInformation(QSPI_Information* info)		--Get QSPI flash's informations
A function to get QSPI flash's informations
Return an integer value (no default value), a parameter for QSPI informataion.
*/
QSPI_StaticTypeDef QSPI_GetInformation(QSPI_Information* info)
{
	//Configure the structure with the memory configuration
	info->FlashTotalSize     = QSPI_MT25Q1GB_TOTAL_SIZE;      
  info->SectorSize         = QSPI_SUBSECTOR_SIZE;   // 
	info->EraseSectorSize    = QSPI_SUBSECTOR_SIZE;
	info->EraseSectorsNumber = ((QSPI_MT25Q1GB_TOTAL_SIZE) / QSPI_SUBSECTOR_SIZE);
	info->ProgPageSize       = QSPI_PAGE_SIZE;
	info->ProgPagesNumber    = ((QSPI_MT25Q1GB_TOTAL_SIZE) / QSPI_PAGE_SIZE);

	return QSPI_OK;
}



/*
QSPI_TurnOnMemoryMappedMode QSPI_TurnOnMemoryMappedMode(void)		--Turn on QSPI memory mapped mode
A function to turn on QSPI memory mapped mode
Return an integer value (no default value), No parameters.
*/
/*
QSPI_StaticTypeDef QSPI_TurnOnMemoryMappedMode(void)
{
	QSPI_CommandTypeDef   qspi_cmd;
	QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

	//Configure the command for the read instruction
	qspi_cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	qspi_cmd.Instruction = QSPI_QUAD_INOUT_FAST_READ_CMD;
	qspi_cmd.AddressMode = QSPI_ADDRESS_4_LINES;
	qspi_cmd.AddressSize = QSPI_ADDRESS_24_BITS;
	qspi_cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	qspi_cmd.DataMode = QSPI_DATA_4_LINES;
	qspi_cmd.DummyCycles = QSPI_DUMMY_CYCLES_READ;
	qspi_cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
	qspi_cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	qspi_cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	//Configure the memory mapped mode
	s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_ENABLE;
	s_mem_mapped_cfg.TimeOutPeriod = 1;

	if (HAL_QSPI_MemoryMapped(&hqspi, &qspi_cmd, &s_mem_mapped_cfg) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}
*/


/*
static QSPI_StaticTypeDef QSPI_ResetMemory(QSPI_HandleTypeDef *handle)	--Reset QSPI flash memory
A function to reset QSPI flash memory
Return a integer value(default QSPI_OK), a parameter for QSPI handle
*/
QSPI_StaticTypeDef QSPI_ResetMemory(QSPI_HandleTypeDef *handle)
{
	QSPI_CommandTypeDef qspi_cmd;

	//Initialize the reset enable command
	qspi_cmd.InstructionMode    = QSPI_INSTRUCTION_1_LINE;
	qspi_cmd.Instruction        = QSPI_RESET_ENABLE_CMD;
	qspi_cmd.AddressMode        = QSPI_ADDRESS_NONE;
	qspi_cmd.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
	qspi_cmd.DataMode           = QSPI_DATA_NONE;
	qspi_cmd.DummyCycles        = 0;
	qspi_cmd.DdrMode            = QSPI_DDR_MODE_DISABLE;
	qspi_cmd.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
	qspi_cmd.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

	//Send the command
	if (HAL_QSPI_Command(handle, &qspi_cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
  
	//Send the reset memory command
	qspi_cmd.Instruction = QSPI_RESET_MEMORY_CMD;
	if (HAL_QSPI_Command(handle, &qspi_cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	//Configure automatic polling mode to wait the memory is ready
	if (QSPI_AutoPollingMemReady(handle) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;	
}


/**
  * @brief  This function set the QSPI memory in 4-byte address mode
  * @param  hqspi: QSPI handle
  * @retval None
  */
static QSPI_StaticTypeDef QSPI_EnterFourBytesAddress(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef s_command;

/* Initialize the command */
  
  if(QSPI_WorkMode)   // QUAD Model 
    s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
  else
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  
  s_command.Instruction       = QSPI_ENTER_4_BYTE_ADDR_MODE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations */
  if (QSPI_WriteEnable(hqspi) != QSPI_OK)
  {
    return QSPI_WRITE_ENABLE_ERROR;
  } 

  /* Send the command */
  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }  

  
  /* Configure automatic polling mode to wait the memory is ready */
  if (QSPI_AutoPollingMemReady(hqspi) != QSPI_OK)
  {
    return QSPI_ERROR;
  }
  return QSPI_OK;
}




/*
static QSPI_StaticTypeDef QSPI_WriteEnable(QSPI_HandleTypeDef *handle)	--Enable QSPI memory write mode
A function to enable QSPI memory write mode
Return a integer value(default QSPI_OK), a parameter for QSPI handle
N25Q256/N25Q512 ���� Table 9: Status Register Bit Definitions�н���
*/
static QSPI_StaticTypeDef QSPI_WriteEnable(QSPI_HandleTypeDef *handle)
{
  uint32_t  __InstructionMode;
  uint8_t _RegVal = 0;
  
  if(QSPI_WorkMode)   // Work In QUAD Model
  {
    __InstructionMode = QSPI_INSTRUCTION_4_LINES;
  }
  else
  {
    __InstructionMode = QSPI_INSTRUCTION_1_LINE; 
  }   
  
  if(QSPI_SendCmdData(  QSPI_WRITE_ENABLE_CMD,        // _Instruction,      ����ָ��
                        __InstructionMode,            // _InstructionMode,  ָ��ģʽ
                        QSPI_ADDRESS_NONE,            // _AddressMode,      ��ַģʽ
                        QSPI_ADDRESS_8_BITS,          // _AddressSize,      ��ַ����  
                        QSPI_DATA_NONE,               // _DataMode,         ����ģʽ
                        0,                            // _NbData,           ���ݶ�д�ֽ���
                        0,                            // _DummyCycles,      ���ÿ�ָ��������
                        0,                            // _Address,          ���͵���Ŀ�ĵ�ַ
                        &_RegVal,                     //  *_pBuf,           �����͵�����
                        QSPI_SEND_CMD                 // __SEND_CMD_DATA_T  _SendCmdDat
                     ) != QSPI_OK )
  {
    return QSPI_ERROR;
  } 
  
	return QSPI_OK;
}



#if   0
/*
static QSPI_StaticTypeDef QSPI_WriteDisable(QSPI_HandleTypeDef *handle)	--Enable QSPI memory write mode
A function to enable QSPI memory write mode
Return a integer value(default QSPI_OK), a parameter for QSPI handle
*/
static QSPI_StaticTypeDef QSPI_WriteDisable(QSPI_HandleTypeDef *handle)
{
  QSPI_CommandTypeDef     sCommand;
  QSPI_AutoPollingTypeDef sConfig;

  /* Enable write operations ------------------------------------------ */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;     //ָ��ģʽ
  sCommand.Instruction       = QSPI_WRITE_DISABLE_CMD;       //ָ��
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;           //��ַģʽ
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;   //�޽����ֽ�
  sCommand.DataMode          = QSPI_DATA_NONE;              //����ģʽ
  sCommand.DummyCycles       = 0;                           //���ÿ�ָ��������
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;       //�ر�DDRģʽ
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode         = QSPI_SIOO_INST_EVERY_CMD;     //ÿ�ζ�����ָ��

  if (HAL_QSPI_Command( handle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }
  
  /* Configure automatic polling mode to wait for write enabling ---- */  
  sConfig.Match           = 0x02;
  sConfig.Mask            = 0x02;
  sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
  sConfig.StatusBytesSize = 1;
  sConfig.Interval        = 0x10;
  sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  sCommand.Instruction    = QSPI_READ_STATUS_REG_CMD;
  sCommand.DataMode       = QSPI_DATA_1_LINE;

  if (HAL_QSPI_AutoPolling( handle, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

	return QSPI_OK;
}

#endif




/*
static QSPI_StaticTypeDef QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *handle, uint32_t timeout)	--Ready auto polling memory
A function to ready auto polling memory
Return a integer value(default QSPI_OK), a parameter for QSPI handle, 
a parameter for timeout(uint32_t).
*/
static QSPI_StaticTypeDef QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *handle)
{
  uint32_t cnt = 0;
  uint8_t _RegVal;
	
	do
	{
		QSPI_Read_SR(QSPI_READ_STATUS_REG_CMD, &_RegVal , 1);   // ��״̬�Ĵ��� 1 

		if((_RegVal & 0x01) == 0x00)
			return QSPI_OK;
		
		cnt ++;
		
	}while( cnt < QSPI_WAIT_MAX_TIME );  

	return QSPI_OUT_TIME;
}


/**
  * @brief  This function configure the dummy cycles on memory side.
  * @param  hqspi: QSPI handle
  * @retval None
  */
QSPI_StaticTypeDef QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef sCommand;
  uint8_t reg;
  
  if(QSPI_WorkMode)   // qpi 
  {
    sCommand.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
    sCommand.DataMode          = QSPI_DATA_4_LINES;
  }
  else
  {
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.DataMode          = QSPI_DATA_1_LINE;  
  }


  /* Read Volatile Configuration register --------------------------- */
//  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = QSPI_READ_VOL_CFG_REG_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//  sCommand.DataMode          = QSPI_DATA_1_LINE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode         = QSPI_SIOO_INST_EVERY_CMD;
  sCommand.NbData            = 1;

  if (HAL_QSPI_Command( hqspi , &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  if (HAL_QSPI_Receive( hqspi , &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Enable write operations ---------------------------------------- */
  QSPI_WriteEnable( hqspi );

  /* Write Volatile Configuration register (with new dummy cycles) -- */  
  sCommand.Instruction = QSPI_WRITE_VOL_CFG_REG_CMD;
  MODIFY_REG(reg, 0xF0, (QSPI_DUMMY_CYCLES_READ_QUAD << POSITION_VAL(0xF0)));
      
  if (HAL_QSPI_Command(hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  if (HAL_QSPI_Transmit(hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}



/*
**************************************************************************************
�������ƣ�QSPI_Receive
�������ܣ�QSPI����ָ�����ȵ�����
������    _pBuf:���ݴ洢��
          _NumByteToRead:Ҫ��ȡ���ֽ���(���0xFFFFFFFF)
����ֵ��QSPI_OK��ȡ�ɹ�������ʧ��
**************************************************************************************
*/
static QSPI_StaticTypeDef QSPI_Receive(uint8_t * _pBuf, uint32_t _NumByteToRead)
{
    hqspi.Instance->DLR = _NumByteToRead-1;                           //�������ݳ���
    if(HAL_QSPI_Receive(&hqspi, _pBuf, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) 
      return QSPI_ERROR; 

      return QSPI_OK;
}

/*
**************************************************************************************
�������ƣ�QSPI_Transmit
�������ܣ�QSPI����ָ�����ȵ�����
������    _pBuf:�������ݻ������׵�ַ
          _NumByteToRead:Ҫ��������ݳ���
����ֵ��QSPI_OK��ȡ�ɹ�������ʧ��
**************************************************************************************
*/
static QSPI_StaticTypeDef QSPI_Transmit(uint8_t * _pBuf, uint32_t _NumByteToRead)
{
    hqspi.Instance->DLR = _NumByteToRead-1;                            //�������ݳ���
    if(HAL_QSPI_Transmit(&hqspi, (uint8_t *) _pBuf, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) 
      return QSPI_ERROR; 

      return QSPI_OK;
}



static QSPI_StaticTypeDef QSPI_SendCmdData( uint8_t  __Instruction,       //  ����ָ��
                                             uint32_t __InstructionMode,   //  ָ��ģʽ
                                             uint32_t __AddressMode,       //  ��ַģʽ
                                             uint32_t __AddressSize,       //  ��ַ����  
                                             uint32_t __DataMode,          //  ����ģʽ
                                             uint32_t __NbData,            //  ���ݶ�д�ֽ���
                                             uint32_t __DummyCycles,       //  ���ÿ�ָ��������
                                             uint32_t __Address,           //  ���͵���Ŀ�ĵ�ַ
                                             uint8_t  *_pBuf,              //  �����͵�����
                                             __SEND_CMD_DATA_T  _SendCmdDat
                                           )
{
  QSPI_CommandTypeDef     sCommand;
  
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;      //ÿ�ζ�����ָ��
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;     //�޽����ֽ�
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;         //�ر�DDRģʽ
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  
  sCommand.Instruction       = __Instruction;                  //ָ��
  sCommand.DummyCycles       = __DummyCycles;                  //���ÿ�ָ��������
  sCommand.Address           = __Address;                      //���͵���Ŀ�ĵ�ַ
//  sCommand.NbData            = __NbData;                     //����ط���ʹ��

  sCommand.InstructionMode   = __InstructionMode;              //ָ��ģʽ
  sCommand.AddressMode       = __AddressMode;                  //��ַģʽ
  sCommand.AddressSize       = __AddressSize;                  //��ַ����  
  sCommand.DataMode          = __DataMode;                     //����ģʽ  

  if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }
  if( _SendCmdDat == QSPI_SEND_DAT)
  {
    if(QSPI_Transmit( ( uint8_t * )_pBuf, __NbData) != QSPI_OK)
    {
      return QSPI_ERROR;
    }
  }
  
  return QSPI_OK; 
}




