/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H 
#include <stdint.h> 
void system_init(void);
uint8_t get_APBPresc(int number);
uint8_t get_AHBPresc(void);
void SystemCoreClockUpdate(void);
uint32_t get_SystemCoreClock(void);
#endif /* SYSTEM_INIT_H */
