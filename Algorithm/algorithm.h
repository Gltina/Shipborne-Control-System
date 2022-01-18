#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include "stm32f10x.h"

// Median value average filtering
// ��λֵƽ���˲�
float MVA_Filtering(uint16_t *array, int8_t arr_size);

void swap(uint16_t *v1, uint16_t *v2);

float Average_Filtering(uint16_t *array, int8_t arr_size);
#endif