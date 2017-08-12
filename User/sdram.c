#include "sdram.h"

#ifdef DEBUG
#define DBG_LOG(x) printf x
#else
#define DBG_LOG(x) 
#endif

static volatile void delay(uint32_t t);

static volatile void delay(uint32_t t)
{
	uint32_t i,j,k;
	while(t--)
	{
		for(i=0;i<10;i++)
		{
			for(j=0;j<100;j++)
				{
					for(k=0;k<1000;k++)
					{
						k=k;
					}
				}
		}
	}
}

void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram)
{
	uint32_t temp=0;
  //SDRAM��������ʼ������Ժ���Ҫ��������˳���ʼ��SDRAM
  SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_CLK_ENABLE,1,0); //ʱ������ʹ��
  //delay_us(500);                                  //������ʱ200us
	delay(100);
  SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_PALL,1,0);       //�����д洢��Ԥ���
  SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_AUTOREFRESH_MODE,8,0);//������ˢ�´��� 
  //����ģʽ�Ĵ���,SDRAM��bit0~bit2Ϊָ��ͻ�����ʵĳ��ȣ�
	//bit3Ϊָ��ͻ�����ʵ����ͣ�bit4~bit6ΪCASֵ��bit7��bit8Ϊ����ģʽ
	//bit9Ϊָ����дͻ��ģʽ��bit10��bit11λ����λ
	temp=(uint32_t)SDRAM_MODEREG_BURST_LENGTH_1       |	//����ͻ������:1(������1/2/4/8)
              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |	//����ͻ������:����(����������/����)
              SDRAM_MODEREG_CAS_LATENCY_3           |	//����CASֵ:3(������2/3)
              SDRAM_MODEREG_OPERATING_MODE_STANDARD |   //���ò���ģʽ:0,��׼ģʽ
              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;     //����ͻ��дģʽ:1,�������
  SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_LOAD_MODE,1,temp);   //����SDRAM��ģʽ�Ĵ���
    
  //ˢ��Ƶ�ʼ�����(��SDCLKƵ�ʼ���),���㷽��:
	//COUNT=SDRAMˢ������/����-20=SDRAMˢ������(us)*SDCLKƵ��(Mhz)/����
  //����ʹ�õ�SDRAMˢ������Ϊ64ms,SDCLK=216/2=108Mhz,����Ϊ8192(2^13).
	//����,COUNT=64*1000*108/8192-20=823
	HAL_SDRAM_ProgramRefreshRate(&hsdram1,823);	
}	

//��SDRAM��������
//bankx:0,��BANK5�����SDRAM����ָ��
//      1,��BANK6�����SDRAM����ָ��
//cmd:ָ��(0,����ģʽ/1,ʱ������ʹ��/2,Ԥ������д洢��/3,�Զ�ˢ��/4,����ģʽ�Ĵ���/5,��ˢ��/6,����)
//refresh:��ˢ�´���
//regval:ģʽ�Ĵ����Ķ���
//����ֵ:0,����;1,ʧ��.
uint8_t SDRAM_Send_Cmd(uint8_t bankx,uint8_t cmd,uint8_t refresh,uint16_t regval)
{
    uint32_t target_bank=0;
    FMC_SDRAM_CommandTypeDef Command;
    
    if(bankx==0) target_bank=FMC_SDRAM_CMD_TARGET_BANK1;       
    else if(bankx==1) target_bank=FMC_SDRAM_CMD_TARGET_BANK2;   
    Command.CommandMode=cmd;                //����
    Command.CommandTarget=target_bank;      //Ŀ��SDRAM�洢����
    Command.AutoRefreshNumber=refresh;      //��ˢ�´���
    Command.ModeRegisterDefinition=regval;  //Ҫд��ģʽ�Ĵ�����ֵ
    if(HAL_SDRAM_SendCommand(&hsdram1,&Command,0X1000)==HAL_OK) //��SDRAM��������
    {
        return 0;  
    }
    else return 1;    
}

//��ָ����ַ(WriteAddr+Bank5_SDRAM_ADDR)��ʼ,����д��n���ֽ�.
//pBuffer:�ֽ�ָ��
//WriteAddr:Ҫд��ĵ�ַ
//n:Ҫд����ֽ���
void FMC_SDRAM_WriteBuffer(uint8_t *pBuffer,uint32_t WriteAddr,uint32_t n)
{
	for(;n!=0;n--)
	{
		*(volatile uint8_t*)(Bank5_SDRAM_ADDR+WriteAddr)=*pBuffer;
		WriteAddr++;
		pBuffer++;
	}
}

//��ָ����ַ((WriteAddr+Bank5_SDRAM_ADDR))��ʼ,��������n���ֽ�.
//pBuffer:�ֽ�ָ��
//ReadAddr:Ҫ��������ʼ��ַ
//n:Ҫд����ֽ���
void FMC_SDRAM_ReadBuffer(uint8_t *pBuffer,uint32_t ReadAddr,uint32_t n)
{
	for(;n!=0;n--)
	{
		*pBuffer++=*(volatile uint8_t*)(Bank5_SDRAM_ADDR+ReadAddr);
		ReadAddr++;
	}
}

void fsmc_sdram_test(void)
{  
	uint32_t i=0;  	  
	uint32_t temp=0;	   
	uint32_t sval=0;	
	for(i=0;i<32*1024*1024;i+=16*1024)
	{
		*(volatile uint32_t*)(Bank5_SDRAM_ADDR+i)=temp; 
		temp++;
	}	  
 	for(i=0;i<32*1024*1024;i+=16*1024) 
	{	
  		temp=*(volatile uint32_t*)(Bank5_SDRAM_ADDR+i);
		if(i==0)sval=temp;
 		else if(temp<=sval)break;   		   
 	}		
	DBG_LOG(("SDRAM Capacity:%dKB...\r\n",(uint16_t)(temp-sval+1)*16));
}	

void read_write_test(uint32_t base)
{
	uint8_t tx[256]={"\0"};
	uint8_t rx[256]={"\0"};
	uint8_t *ptx=tx;
	uint8_t *prx=rx;
	uint16_t i=0;
	DBG_LOG(("-->BASE at 0x%X\r\n",base));
	//DBG_LOG(("Start Read/Write Test\r\n"));
	//DBG_LOG(("Set TX data:\r\n"));
	#if 1
	for(i=0;i<256;i++)
	{
		tx[i]=(uint8_t)i;
		//DBG_LOG(("%.2X ",tx[i]));
	}
	#endif
	FMC_SDRAM_WriteBuffer(ptx,base,256);
	FMC_SDRAM_ReadBuffer (prx,base,256);
	//DBG_LOG(("\r\nRX is :\r\n "));
	for(i=0;i<256;i++)
	{
		if(rx[i] != i)
		{
			DBG_LOG(("%.2X ",rx[i]));
		}
	}
	DBG_LOG(("\r\n"));
}

void sdram_rw_test(void)
{
	uint32_t base=0;
	while(base <= 0x20000)
	{
		read_write_test(base);
		base=base+256;
	}
}
