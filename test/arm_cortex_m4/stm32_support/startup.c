/* Copyright (c) 2016 Nicholas Esterer. All rights reserved. */

#include "system_init.h" 

#include <stdint.h> 
#include <string.h> 
#include "version.h" 


#define WEAK_ISR __attribute__((weak,isr))

extern void _estack (void);

void Reset_Handler(void);

void WEAK_ISR NMI_Handler(void)
{
}

void WEAK_ISR HardFault_Handler(void)
{
    while(1);
}

void WEAK_ISR MemManage_Handler(void)
{
}

void WEAK_ISR BusFault_Handler(void)
{
}

void WEAK_ISR UsageFault_Handler(void)
{
}

void WEAK_ISR SVC_Handler(void)
{
}

void WEAK_ISR DebugMon_Handler(void)
{
}

void WEAK_ISR PendSV_Handler(void)
{
}

void WEAK_ISR SysTick_Handler(void)
{
}

void WEAK_ISR WWDG_IRQHandler(void)                   /* Window WatchDog              */                                        
{
}

void WEAK_ISR PVD_IRQHandler(void)                    /* PVD through EXTI Line detection */                        
{
}

void WEAK_ISR TAMP_STAMP_IRQHandler(void)             /* Tamper and TimeStamps through the EXTI line */            
{
}

void WEAK_ISR RTC_WKUP_IRQHandler(void)               /* RTC Wakeup through the EXTI line */                      
{
}

void WEAK_ISR FLASH_IRQHandler(void)                  /* FLASH                        */                                          
{
}

void WEAK_ISR RCC_IRQHandler(void)                    /* RCC                          */                                            
{
}

void WEAK_ISR EXTI0_IRQHandler(void)                  /* EXTI Line0                   */                        
{
}

void WEAK_ISR EXTI1_IRQHandler(void)                  /* EXTI Line1                   */                          
{
}

void WEAK_ISR EXTI2_IRQHandler(void)                  /* EXTI Line2                   */                          
{
}

void WEAK_ISR EXTI3_IRQHandler(void)                  /* EXTI Line3                   */                          
{
}

void WEAK_ISR EXTI4_IRQHandler(void)                  /* EXTI Line4                   */                          
{
}

void WEAK_ISR DMA1_Stream0_IRQHandler(void)           /* DMA1 Stream 0                */                  
{
}

void WEAK_ISR DMA1_Stream1_IRQHandler(void)           /* DMA1 Stream 1                */                   
{
}

void WEAK_ISR DMA1_Stream2_IRQHandler(void)           /* DMA1 Stream 2                */                   
{
}

void WEAK_ISR DMA1_Stream3_IRQHandler(void)           /* DMA1 Stream 3                */                   
{
}

void WEAK_ISR DMA1_Stream4_IRQHandler(void)           /* DMA1 Stream 4                */                   
{
}

void WEAK_ISR DMA1_Stream5_IRQHandler(void)           /* DMA1 Stream 5                */                   
{
}

void WEAK_ISR DMA1_Stream6_IRQHandler(void)           /* DMA1 Stream 6                */                   
{
}

void WEAK_ISR ADC_IRQHandler(void)                    /* ADC1, ADC2 and ADC3s         */                   
{
}

void WEAK_ISR CAN1_TX_IRQHandler(void)                /* CAN1 TX                      */                         
{
}

void WEAK_ISR CAN1_RX0_IRQHandler(void)               /* CAN1 RX0                     */                          
{
}

void WEAK_ISR CAN1_RX1_IRQHandler(void)               /* CAN1 RX1                     */                          
{
}

void WEAK_ISR CAN1_SCE_IRQHandler(void)               /* CAN1 SCE                     */                          
{
}

void WEAK_ISR EXTI9_5_IRQHandler(void)                /* External Line[9:5]s          */                          
{
}

void WEAK_ISR TIM1_BRK_TIM9_IRQHandler(void)          /* TIM1 Break and TIM9          */         
{
}

void WEAK_ISR TIM1_UP_TIM10_IRQHandler(void)          /* TIM1 Update and TIM10        */         
{
}

void WEAK_ISR TIM1_TRG_COM_TIM11_IRQHandler(void)     /* TIM1 Trigger and Commutation and TIM11 */
{
}

void WEAK_ISR TIM1_CC_IRQHandler(void)                /* TIM1 Capture Compare         */                          
{
}

void WEAK_ISR TIM2_IRQHandler(void)                   /* TIM2                         */                   
{
}

void WEAK_ISR TIM3_IRQHandler(void)                   /* TIM3                         */                   
{
}

void WEAK_ISR TIM4_IRQHandler(void)                   /* TIM4                         */                   
{
}

void WEAK_ISR I2C1_EV_IRQHandler(void)                /* I2C1 Event                   */                          
{
}

void WEAK_ISR I2C1_ER_IRQHandler(void)                /* I2C1 Error                   */                          
{
}

void WEAK_ISR I2C2_EV_IRQHandler(void)                /* I2C2 Event                   */                          
{
}

void WEAK_ISR I2C2_ER_IRQHandler(void)                /* I2C2 Error                   */                            
{
}

void WEAK_ISR SPI1_IRQHandler(void)                   /* SPI1                         */                   
{
}

void WEAK_ISR SPI2_IRQHandler(void)                   /* SPI2                         */                   
{
}

void WEAK_ISR USART1_IRQHandler(void)                 /* USART1                       */                   
{
}

void WEAK_ISR USART2_IRQHandler(void)                 /* USART2                       */                   
{
}

void WEAK_ISR USART3_IRQHandler(void)                 /* USART3                       */                   
{
}

void WEAK_ISR EXTI15_10_IRQHandler(void)              /* External Line[15:10]s        */                          
{
}

void WEAK_ISR RTC_Alarm_IRQHandler(void)              /* RTC Alarm (A and B) through EXTI Line */                 
{
}

void WEAK_ISR OTG_FS_WKUP_IRQHandler(void)            /* USB OTG FS Wakeup through EXTI line */                       
{
}

void WEAK_ISR TIM8_BRK_TIM12_IRQHandler(void)         /* TIM8 Break and TIM12         */         
{
}

void WEAK_ISR TIM8_UP_TIM13_IRQHandler(void)          /* TIM8 Update and TIM13        */         
{
}

void WEAK_ISR TIM8_TRG_COM_TIM14_IRQHandler(void)     /* TIM8 Trigger and Commutation and TIM14 */
{
}

void WEAK_ISR TIM8_CC_IRQHandler(void)                /* TIM8 Capture Compare         */                          
{
}

void WEAK_ISR DMA1_Stream7_IRQHandler(void)           /* DMA1 Stream7                 */                          
{
}

void WEAK_ISR FSMC_IRQHandler(void)                   /* FSMC                         */                   
{
}

void WEAK_ISR SDIO_IRQHandler(void)                   /* SDIO                         */                   
{
}

void WEAK_ISR TIM5_IRQHandler(void)                   /* TIM5                         */                   
{
}

void WEAK_ISR SPI3_IRQHandler(void)                   /* SPI3                         */                   
{
}

void WEAK_ISR UART4_IRQHandler(void)                  /* UART4                        */                   
{
}

void WEAK_ISR UART5_IRQHandler(void)                  /* UART5                        */                   
{
}

void WEAK_ISR TIM6_DAC_IRQHandler(void)               /* TIM6 and DAC1&2 underrun errors */                   
{
}

void WEAK_ISR TIM7_IRQHandler(void)                   /* TIM7                         */
{
}

void WEAK_ISR DMA2_Stream0_IRQHandler(void)           /* DMA2 Stream 0                */                   
{
}

void WEAK_ISR DMA2_Stream1_IRQHandler(void)           /* DMA2 Stream 1                */                   
{
}

void WEAK_ISR DMA2_Stream2_IRQHandler(void)           /* DMA2 Stream 2                */                   
{
}

void WEAK_ISR DMA2_Stream3_IRQHandler(void)           /* DMA2 Stream 3                */                   
{
}

void WEAK_ISR DMA2_Stream4_IRQHandler(void)           /* DMA2 Stream 4                */                   
{
}

void WEAK_ISR ETH_IRQHandler(void)                    /* Ethernet                     */                   
{
}

void WEAK_ISR ETH_WKUP_IRQHandler(void)               /* Ethernet Wakeup through EXTI line */                     
{
}

void WEAK_ISR CAN2_TX_IRQHandler(void)                /* CAN2 TX                      */                          
{
}

void WEAK_ISR CAN2_RX0_IRQHandler(void)               /* CAN2 RX0                     */                          
{
}

void WEAK_ISR CAN2_RX1_IRQHandler(void)               /* CAN2 RX1                     */                          
{
}

void WEAK_ISR CAN2_SCE_IRQHandler(void)               /* CAN2 SCE                     */                          
{
}

void WEAK_ISR OTG_FS_IRQHandler(void)                 /* USB OTG FS                   */                   
{
}

void WEAK_ISR DMA2_Stream5_IRQHandler(void)           /* DMA2 Stream 5                */                   
{
}

void WEAK_ISR DMA2_Stream6_IRQHandler(void)           /* DMA2 Stream 6                */                   
{
}

void WEAK_ISR DMA2_Stream7_IRQHandler(void)           /* DMA2 Stream 7                */                   
{
}

void WEAK_ISR USART6_IRQHandler(void)                 /* USART6                       */                    
{
}

void WEAK_ISR I2C3_EV_IRQHandler(void)                /* I2C3 event                   */                          
{
}

void WEAK_ISR I2C3_ER_IRQHandler(void)                /* I2C3 error                   */                          
{
}

void WEAK_ISR OTG_HS_EP1_OUT_IRQHandler(void)         /* USB OTG HS End Point 1 Out   */                   
{
}

void WEAK_ISR OTG_HS_EP1_IN_IRQHandler(void)          /* USB OTG HS End Point 1 In    */                   
{
}

void WEAK_ISR OTG_HS_WKUP_IRQHandler(void)            /* USB OTG HS Wakeup through EXTI */                         
{
}

void WEAK_ISR OTG_HS_IRQHandler(void)                 /* USB OTG HS                   */                   
{
}

void WEAK_ISR DCMI_IRQHandler(void)                   /* DCMI                         */                   
{
}

void WEAK_ISR CRYP_IRQHandler(void)                   /* CRYP crypto                  */                   
{
}

void WEAK_ISR HASH_RNG_IRQHandler(void)               /* Hash and Rng                 */
{
}

void WEAK_ISR FPU_IRQHandler(void)                    /* FPU                          */
{
}

void WEAK_ISR UART7_IRQHandler(void) /* UART7 global interrupt */
{
}

void WEAK_ISR UART8_IRQHandler(void) /* UART8 global interrupt */
{
}

void WEAK_ISR SPI4_IRQHandler(void) /* SPI4 global Interrupt */
{
}

void WEAK_ISR SPI5_IRQHandler(void) /* SPI5 global Interrupt */
{
}

void WEAK_ISR SPI6_IRQHandler(void) /* SPI6 global Interrupt */
{
}

void WEAK_ISR SAI1_IRQHandler(void) /* SAI1 global Interrupt */
{
}

void WEAK_ISR LTDC_IRQHandler(void) /* LTDC global Interrupt */
{
}

void WEAK_ISR LTDC_ER_IRQHandler(void) /* LTDC Error global */
{
}

void WEAK_ISR DMA2D_IRQHandler(void) /* DMA2D global Interrupt */   
{
}

__attribute__((section(".isr_vector")))
void (*vector_table[]) (void) = {
    _estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
    WWDG_IRQHandler,
    PVD_IRQHandler,
    TAMP_STAMP_IRQHandler,
    RTC_WKUP_IRQHandler,
    FLASH_IRQHandler,
    RCC_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    DMA1_Stream0_IRQHandler,
    DMA1_Stream1_IRQHandler,
    DMA1_Stream2_IRQHandler,
    DMA1_Stream3_IRQHandler,
    DMA1_Stream4_IRQHandler,
    DMA1_Stream5_IRQHandler,
    DMA1_Stream6_IRQHandler,
    ADC_IRQHandler,
    CAN1_TX_IRQHandler,
    CAN1_RX0_IRQHandler,
    CAN1_RX1_IRQHandler,
    CAN1_SCE_IRQHandler,
    EXTI9_5_IRQHandler,
    TIM1_BRK_TIM9_IRQHandler,
    TIM1_UP_TIM10_IRQHandler,
    TIM1_TRG_COM_TIM11_IRQHandler,
    TIM1_CC_IRQHandler,
    TIM2_IRQHandler,
    TIM3_IRQHandler,
    TIM4_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,
    SPI1_IRQHandler,
    SPI2_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    USART3_IRQHandler,
    EXTI15_10_IRQHandler,
    RTC_Alarm_IRQHandler,
    OTG_FS_WKUP_IRQHandler,
    TIM8_BRK_TIM12_IRQHandler,
    TIM8_UP_TIM13_IRQHandler,
    TIM8_TRG_COM_TIM14_IRQHandler,
    TIM8_CC_IRQHandler,
    DMA1_Stream7_IRQHandler,
    FSMC_IRQHandler,
    SDIO_IRQHandler,
    TIM5_IRQHandler,
    SPI3_IRQHandler,
    UART4_IRQHandler,
    UART5_IRQHandler,
    TIM6_DAC_IRQHandler,
    TIM7_IRQHandler,
    DMA2_Stream0_IRQHandler,
    DMA2_Stream1_IRQHandler,
    DMA2_Stream2_IRQHandler,
    DMA2_Stream3_IRQHandler,
    DMA2_Stream4_IRQHandler,
    ETH_IRQHandler,
    ETH_WKUP_IRQHandler,
    CAN2_TX_IRQHandler,
    CAN2_RX0_IRQHandler,
    CAN2_RX1_IRQHandler,
    CAN2_SCE_IRQHandler,
    OTG_FS_IRQHandler,
    DMA2_Stream5_IRQHandler,
    DMA2_Stream6_IRQHandler,
    DMA2_Stream7_IRQHandler,
    USART6_IRQHandler,
    I2C3_EV_IRQHandler,
    I2C3_ER_IRQHandler,
    OTG_HS_EP1_OUT_IRQHandler,
    OTG_HS_EP1_IN_IRQHandler,
    OTG_HS_WKUP_IRQHandler,
    OTG_HS_IRQHandler,
    DCMI_IRQHandler,
    CRYP_IRQHandler,
    HASH_RNG_IRQHandler,
    FPU_IRQHandler,
    UART7_IRQHandler,
    UART8_IRQHandler,
    SPI4_IRQHandler,
    SPI5_IRQHandler,
    SPI6_IRQHandler,
    SAI1_IRQHandler,
    LTDC_IRQHandler,
    LTDC_ER_IRQHandler,
    DMA2D_IRQHandler
}; 

extern int main (void);

void Reset_Handler(void)
{
    extern char _sdata, _sidata, _edata,
           _ssmall_data, _sismall_data, _esmall_data,
           _sbig_data, _sibig_data, _ebig_data,
           _sbss, _ebss;
    /* Copy data section from flash to RAM */
    memcpy(&_sdata,&_sidata,(&_edata)-(&_sdata));
    /* Copy small_data section from flash to RAM */
    memcpy(&_ssmall_data,&_sismall_data,(&_esmall_data)-(&_ssmall_data));
    /* Copy big_data section from flash to RAM */
    memcpy(&_sbig_data,&_sibig_data,(&_ebig_data)-(&_sbig_data));
    /* Zero-fill the .bss section */
    memset(&_sbss,0,(&_ebss)-(&_sbss));
    /* Initialize system */
    system_init();
    /* Call main() */
    main();
    /* Should main ever return, we just loop forever */
    while(1);
}
    
__attribute__((section(".flash_comment_text")))
char flash_comment_text[] = 
                            "856 for ZELLERSASN version "
                            ZELLERSASN_VERSION
                            " Copyright (c) 2016-2017 Nicholas Esterer. "
                            "All rights reserved. "
                            "Special thanks to Scott Monk "
                            "and all of Montreal Assembly. "
                            "\"A ce petit bistrot-la, ils font "
                            "du petit salé aux lentilles tous les jeudis.\"";

