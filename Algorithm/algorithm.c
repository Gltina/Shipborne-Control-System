#include "algorithm.h"

uint16_t filter(uint16_t *array, int8_t arr_size)
{
    if(arr_size == 1)
    {
        return array[0];
    }
    else if(arr_size == 2)
    {
        return (float)(array[0] + array[1])/2.0;
    }
    
//    for(int8_t i = 0; i < arr_size; i++)
//    {
//        printf("%d ", array[i]);
//    }
//    printf("\r\n");
    
    // sort
    for (int8_t i = 0; i < arr_size - 1; i++)
    {
        for (int8_t j = i+1; j < arr_size; j++)
        {
            if (array[i] > array[j])
            {
                swap(&array[i], &array[j]);
            }
        }
    }
    
//    for(int8_t i = 0; i < arr_size; i++)
//    {
//        printf("%d ", array[i]);
//    }
//    printf("\r\n");
    
    uint16_t sum = 0;
    for (int8_t i = 1; i < arr_size - 1; i++)
    {
        sum += array[i];
    }
    
    return sum / (arr_size - 2);
}

void swap(uint16_t * v1,uint16_t * v2)
{
    uint16_t temp;
    temp = *v1;
    *v1 = *v2;
    *v2 = temp;
}