#ifndef _tab_H_
#define _tab_H_

#include <stdint.h>
#include "sdram_handle.h"

//typedef struct
//{
//	uint32_t stand_val;
//	uint32_t actual_val;
//	uint32_t calibration_val;
//}DataStoreTypeDef;

//typedef DataStoreTypeDef AmplitudeDataTypeDef;
//typedef DataStoreTypeDef PhaseDataTypeDef;

extern AmplitudeDataTypeDef g_amplitude_tab[8][128];
extern PhaseDataTypeDef g_phase_tab[8][64];

extern AmplitudeDataTypeDef *g_ptra;
extern PhaseDataTypeDef *g_ptrp;

void print_all(void);
void tab_init(void);
uint32_t get_a_index(uint32_t a);
uint32_t bin_search(PhaseDataTypeDef a_ptrp[],uint32_t array_size, uint32_t key,int32_t error);
uint32_t search_left_right(PhaseDataTypeDef a_ptrp[],uint32_t index, uint32_t key);
void search_1ch(uint8_t ch,uint32_t a_set,uint32_t p_set,uint32_t *a_out,uint32_t *p_out);
#endif //_tab_H_

