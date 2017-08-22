#include "tab.h"
#include <stdio.h>
//#include <math.h>
#include <stdlib.h>

#ifdef DEBUG
#define DBG_LOG(x) printf x
#else
#define DBG_LOG(x) 
#endif

AmplitudeDataTypeDef g_amplitude_tab[8][128]={0};
PhaseDataTypeDef g_phase_tab[8][64]={0};

AmplitudeDataTypeDef *g_ptra;
PhaseDataTypeDef *g_ptrp;

volatile  uint32_t g_acnt = 0;
volatile uint32_t g_pcnt = 0;

//extern FILE *fp;

void tab_init(void)
{
	uint32_t i=0;
	uint32_t j=0;
	for(i=0;i<8;i++)
	{
		for(j=0;j<128;j++)
		{
			g_amplitude_tab[i][j].stand_val = g_acnt;
			g_amplitude_tab[i][j].actual_val = g_acnt+3;
			g_amplitude_tab[i][j].calibration_val = 0;//g_acnt+1;
			g_acnt += 100;
		}
		for(j=0;j<64;j++)
		{
			g_phase_tab[i][j].stand_val = g_pcnt;
			g_phase_tab[i][j].actual_val = g_pcnt+3;
			g_phase_tab[i][j].calibration_val = 0;//g_pcnt+4;
			g_pcnt += 100;
		}
			
	}
}


void print_all(void)
{
	uint32_t i=0;
	uint32_t j=0;

	for(i=0;i<8;i++)
	{
		DBG_LOG(("\r\n\namp\r\n"));
		for(j=0;j<128;j++)
		{
			DBG_LOG(("ch%.2d%.3d %.8d  %.8d  %.8d\n",i,j,g_amplitude_tab[i][j].stand_val,g_amplitude_tab[i][j].actual_val,g_amplitude_tab[i][j].calibration_val));
		}
		DBG_LOG(("\r\n\nphase\r\n"));
		for(j=0;j<64;j++)
		{
			DBG_LOG(("ch%.2d%.2d %.8d  %.8d  %.8d\n",i,j,g_phase_tab[i][j].stand_val,g_phase_tab[i][j].actual_val,g_phase_tab[i][j].calibration_val));
		}
	}

}


void search_1ch(uint8_t ch,uint32_t a_set,uint32_t p_set,uint32_t *a_out,uint32_t *p_out)
{
	uint32_t a_index;
	uint32_t p_index;
	uint32_t a_cal_temp;
	uint32_t p_cal_temp;
	uint32_t a_raw;
	uint32_t p_raw;
	 
	DBG_LOG(("aset %d pset %d\n",a_set,p_set));
	g_ptra = (AmplitudeDataTypeDef*)g_amplitude_tab[ch];
	g_ptrp = (PhaseDataTypeDef *) (g_phase_tab[ch]);
	
	a_index=get_a_index(a_set);
	
	p_cal_temp = (g_ptra+a_index)->calibration_val;
	p_raw = p_set-p_cal_temp; //p_raw in mcu
	p_index =bin_search(g_ptrp,64,p_raw,80);
	DBG_LOG(("p index %d\n",p_index));
	p_index = search_left_right(g_ptrp,p_index,p_raw);//get p
	DBG_LOG(("p lr index %d\n",p_index));
	*p_out = (g_ptrp + p_index)->stand_val;//out 
	
	a_cal_temp=(g_ptrp+p_index)->calibration_val;
	a_raw = a_set - a_cal_temp;
	a_index =bin_search(g_ptra,128,a_raw,80);
	DBG_LOG(("a index %d\n",a_index));
	a_index = search_left_right(g_ptra,a_index,a_raw);//get a
	DBG_LOG(("a lr index %d\n",a_index));
	*a_out = (g_ptra + a_index)->stand_val;//out
	
}



uint32_t get_a_index(uint32_t a)
{
	return (uint32_t)(a/100);
}

uint32_t bin_search(PhaseDataTypeDef a_ptrp[],uint32_t array_size, uint32_t key,int32_t error)
{
	uint32_t start = 0;
	uint32_t end = array_size-1;
	uint32_t mid;
	int32_t tmp;
	
	while(start <= end)
	{
		mid = (start+end)/2;
		DBG_LOG(("start:%d end %d %d--%d\n",start,end,a_ptrp[mid].actual_val,key));
		if( a_ptrp[mid].actual_val >= key )
		{
			if( (a_ptrp[mid].actual_val - key) <= error)
			{
				DBG_LOG(("--found\n"));
				return mid;
			}
			end = mid;
			DBG_LOG(("*"));
		}
		else
		{
			if( (key -a_ptrp[mid].actual_val) <= error)
			{
				DBG_LOG(("++found\n"));
				return mid;
			}
			start = mid;
			DBG_LOG(("#"));
		}
	}
	return 0;
}

uint32_t search_left_right(PhaseDataTypeDef a_ptrp[],uint32_t index, uint32_t key)
{
	uint32_t a[3];
	if(index == 0) return 0;
	if(index >= 127) return 127;
	
	a[0]=abs(a_ptrp[index-1].actual_val-key);
	a[1]=abs(a_ptrp[index].actual_val-key);
	a[2]=abs(a_ptrp[index+1].actual_val-key);
	
	DBG_LOG(("%d-%d--%d\n",a[0],a[1],a[2]));
	#if 0
	if(a[0] > a[1]) 
	{
		if(a[2] > a[1])
		{
			return index;
		}
		else
		{
			if(a[0] <a[2])
			{
				return index;
			}
			else
			{
				return index+1;
			}
		}
	}
	else
	{
		if(a[0] < a[2])
		{
			return index;
		}
		else
		{
			if(a[2]< a[0])
			{
				return index+1;
			}
			else
			{
				return index-1;
			}
		}
	}
	#endif
	
	if(a[0]>a[1])//a[0]>a[1]
	{
		if(a[2]>a[1])
		{
			return index;//a[0]>a[1] a[2]>a[1]  min=a[1]
		}
		else//a[2]<a[1]
		{
			return index+1;//a[0]>a[1]>a[2] min =a[2]
		}
	}
	else//a[0]<a[1]
	{
		if(a[2]>a[1])
		{
			return index-1;//a[2]>a[1]>a[0] min=a[0]
		}
		else//a[2]<a[1]
		{
			if(a[0]>a[2])
			{
				return index+1;//a[2]<a[1];a[0]<1[1]-->a[2]<a[0]===>min = a[2]
			}
			else
			{
				return index-1;//a[2]<a[1];a[0]<1[1]-->a[2]<a[1]===>min = a[0]
			}
		}
	}
	
	
	
 }



