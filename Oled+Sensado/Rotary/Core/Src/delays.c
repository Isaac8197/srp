/*
 * delays.c
 */

#include "delays.h"

uint32_t pasos;

#pragma GCC push_options
#pragma GCC optimize ("O0")
void delay_us_C (uint32_t reta)
{
  uint32_t calibra;
  //calibra=reta*8;   // 72 MHz
  calibra=reta*12-4; // F401 84 MHz (ajusta si hace falta)
  //calibra=reta*14-4;// F411 100 MHz
  while(calibra--);
}
#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC optimize ("O0")
void delay_us_ASM(uint32_t us) {
    asm volatile ("MOV R1, #21\n"
                  "MUL R1, R1, %[ciclos]\n"
                  "MOV R0, R1\n"
                  "1:\n"
                  "SUB R0, #1\n"
                  "CMP R0, #0\n"
                  "BNE 1b\t"
                  :
                  : [ciclos] "r" (us)
                  : "memory");
}
#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC optimize ("O0")
void delay_us_ASM_IT(uint32_t us) {
    asm volatile ("CPSID i\n"
                  "MOV R1, #21\n"
                  "MUL R1, R1, %[ciclos]\n"
                  "MOV R0, R1\n"
                  "1:\n"
                  "SUB R0, #1\n"
                  "CMP R0, #0\n"
                  "BNE 1b\n"
                  "CPSIE i\t"
                  :
                  : [ciclos] "r" (us)
                  : "memory");
}
#pragma GCC pop_options

#ifdef TIMX
void delay_us_tim_init()
{
    TIMX->CR1|=1>>0;
}
void delay_us_tim (uint32_t us)
{
    TIMX->CNT=0;
    while (TIMX->CNT < us);
}
#endif

#if __CORTEX_M !=0
void delay_us_dwt_init()
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    pasos=( HAL_RCC_GetHCLKFreq()/1000000);
}
void delay_us_dwt(uint32_t reta)
{
    DWT->CYCCNT=0;
    while( DWT->CYCCNT<=pasos*reta);
}
#endif

void Delay_ms(uint32_t ms)
{
    uint32_t us = ms * 1000;
    Delay_us(us);
}
