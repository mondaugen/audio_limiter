/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include "stm32f4xx.h"
#include "fmc.h" 

/*!< Uncomment the following line if you need to use external SRAM mounted
     on STM324xG_EVAL board as data memory  */
/* #define DATA_IN_ExtSRAM */

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x00 /*!< Vector Table base offset field. 
                                   This value must be a multiple of 0x200. */

/* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
#define PLL_M      8
#define PLL_N      360

/* SYSCLK = PLL_VCO / PLL_P */
#define PLL_P      2

/* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ
 * USB OTG not setup properly unless PLL_VCO / PLLQ = 48MHz */
#define PLL_Q      8

static uint32_t SystemCoreClock = 180000000;

__I uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};

/* Get the prescalar of the APB bus clock where int is the number. Returns 0 if
 * number isn't good (0 cannot be the prescalar). */
uint8_t get_APBPresc(int number)
{
    switch (number) {
        case 1:
            return 1 << APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> 10];
        case 2:
            return 1 << APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE2) >> 13];
        default:
            return 0;
    }
}

uint8_t get_AHBPresc(void)
{
    return 1 << AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE) >> 4];
}

/* Configures the System clock source, PLL Multiplier and Divider factors, 
 * AHB/APBx prescalers and Flash settings
 * This function should be called only once the RCC clock configuration  
 * is reset to the default reset state (done in SystemInit() function).   
 */
static void set_sys_clock(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);
    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));
    if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
        HSEStatus = (uint32_t)0x01;
    }
    else {
        HSEStatus = (uint32_t)0x00;
    }
    if (HSEStatus == (uint32_t)0x01) {
        /* Enable high performance mode, System frequency up to 168 MHz */
        RCC->APB1ENR |= RCC_APB1ENR_PWREN;
        PWR->CR |= PWR_CR_PMODE;  
        /* Configure the main PLL */
        RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
            (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);
        /* Enable the main PLL */
        RCC->CR |= RCC_CR_PLLON;
        /* Enable over drive mode */
        PWR->CR |= PWR_CR_ODEN;
        /* Wait for overdrive to be ready */
        while (!(PWR->CSR & PWR_CSR_ODRDY));
        /* Switch voltage regulator to overdrive mode */
        PWR->CR |= PWR_CR_ODSWEN;
        /* Wait for voltage regulator to switch */
        while (!(PWR->CSR & PWR_CSR_ODSWRDY));
        /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
        FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
        /* HCLK = SYSCLK / 1*/
        RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
        /* PCLK2 = HCLK / 2*/
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
        /* PCLK1 = HCLK / 4*/
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
        /* Wait till the main PLL is ready */
        while((RCC->CR & RCC_CR_PLLRDY) == 0);
        /* Select the main PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= RCC_CFGR_SW_PLL;
        /* Wait till the main PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
    }
    else { /* If HSE fails to start-up, the application will have wrong clock
              configuration. User can add here some code to deal with this error */
    }
}

static void system_preinit(void)
{
    /* Reset the RCC clock configuration to the default reset state ------------*/
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;
    /* Reset CFGR register */
    RCC->CFGR = 0x00000000;
    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;
    /* Reset PLLCFGR register */
    RCC->PLLCFGR = 0x24003010;
    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;
    /* Disable all interrupts */
    RCC->CIR = 0x00000000;
}

static void enable_fpu(void)
{
    SCB->CPACR |= 0xf << 20;
    __DSB();
    __ISB();
}

/* Setup the microcontroller system
 * Initialize the Embedded Flash Interface, the PLL and update the 
 * SystemFrequency variable.
 */
void system_init(void)
{
    system_preinit();
    /* Insert peripheral startup functions here as needed */
    set_sys_clock();
    /* Enable fpu */
    enable_fpu();
    /* Enable SDRAM */
    fmc_setup();
}

/* Update SystemCoreClock variable according to Clock Register Values.
 * The SystemCoreClock variable contains the core clock (HCLK), it can
 * be used by the user application to setup the SysTick timer or configure
 * other parameters.
 *   
 * Each time the core clock (HCLK) changes, this function must be called
 * to update SystemCoreClock variable value. Otherwise, any configuration
 * based on this variable will be incorrect.         
 *     
 * - The system frequency computed by this function is not the real 
 * frequency in the chip. It is calculated based on the predefined 
 * constant and the selected clock source:
 *   
 * - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
 *                                    
 * - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
 *                
 * - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**) 
 *   or HSI_VALUE(*) multiplied/divided by the PLL factors.
 *         
 * (*) HSI_VALUE is a constant defined in stm32f4xx.h file (default value
 *     16 MHz) but the real value may vary depending on the variations
 *     in voltage and temperature.   
 * 
 * (**) HSE_VALUE is a constant defined in stm32f4xx.h file (default value
 *      25 MHz), user has to ensure that HSE_VALUE is same as the real
 *      frequency of the crystal used. Otherwise, this function may
 *      have wrong result.
 *        
 * - The result of this function could be not correct when using fractional
 *   value for HSE crystal.
 */
void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;
    /* Get SYSCLK source */
    tmp = RCC->CFGR & RCC_CFGR_SWS;
    switch (tmp) {
        case 0x00:  /* HSI used as system clock source */
            SystemCoreClock = HSI_VALUE;
            break;
        case 0x04:  /* HSE used as system clock source */
            SystemCoreClock = HSE_VALUE;
            break;
        case 0x08:  /* PLL used as system clock source */
            /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
               SYSCLK = PLL_VCO / PLL_P
               */    
            pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
            pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
            if (pllsource != 0) {
                /* HSE used as PLL clock source */
                pllvco = (HSE_VALUE / pllm)
                    * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
            }
            else {
                /* HSI used as PLL clock source */
                pllvco = (HSI_VALUE / pllm) 
                    * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);      
            }
            pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
            SystemCoreClock = pllvco/pllp;
            break;
        default:
            SystemCoreClock = HSI_VALUE;
            break;
    }
    /* Compute HCLK frequency --------------------------------------------------*/
    /* Get HCLK prescaler */
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
    /* HCLK frequency */
    SystemCoreClock >>= tmp;
}

uint32_t get_SystemCoreClock(void)
{
    return SystemCoreClock;
}
