/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#ifndef FMC_H
#define FMC_H 
#include "stm32f4xx.h" 
#include <stdint.h> 

/* Private defines -----------------------------------------------------------*/
#define SDRAM_BANK_ADDR ((uint32_t)0xD0000000)
#define SDRAM_TIMEOUT ((uint32_t)10000000)
#define SDRAM_LENGTH (64000000/8)
#define SDRAM_LENGTH_SHORT (64000/8)

void fmc_setup(void);
#endif /* FMC_H */
