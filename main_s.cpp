#include <arm_cmse.h>
#include "mbed.h"

#if !defined(DEVICE_SERIAL) || (DEVICE_SERIAL == 0U)
#define printf(...)
#endif

/* typedef for non-secure callback functions */
#if defined(__ICCARM__)
typedef __cmse_nonsecure_call void (*non_secure_call)(void);
#else
typedef void (*non_secure_call) (void) __attribute__((cmse_nonsecure_call));
#endif

#if defined(MBED_CONF_APP_TZ_START_NS)
#undef TZ_START_NS
#define TZ_START_NS     MBED_CONF_APP_TZ_START_NS
#endif

/* Start address of non-secure application */
#ifndef TZ_START_NS
#error("TZ_START_NS not defined")
#endif

/* Secure main() */
int main(void)
{
    printf("\r\n");
    printf("+---------------------------------------------+\r\n");
    printf("|    Secure code is running ...               |\r\n");
    printf("+---------------------------------------------+\r\n");
    
    printf("NU_TZ_SECURE_FLASH_SIZE: 0x%08x\n", NU_TZ_SECURE_FLASH_SIZE);
    printf("NU_TZ_SECURE_SRAM_SIZE: 0x%08x\n", NU_TZ_SECURE_SRAM_SIZE);
    printf("NU_TZ_NSC_REGION_BASE: 0x%08x\n", NU_TZ_NSC_REGION_BASE);
    printf("NU_TZ_NSC_REGION_SIZE: 0x%08x\n", NU_TZ_NSC_REGION_SIZE);
    
    non_secure_call ns_reset_handler;
 
    /* Add user setup code for secure part here*/
    
    /* Set Non-secure main stack (MSP_NS) */
#if defined(__ICCARM__)
    asm volatile("MSR     SP_NS, %0" :: "r" (*((uint32_t *)(TZ_START_NS))));
#else
    __TZ_set_MSP_NS(*((uint32_t *)(TZ_START_NS)));
#endif

    printf("Secure main thread \r\n");
    printf("TZ_START_NS: 0x%08x\r\n", TZ_START_NS);
    
    /* Get non-secure Reset handler */
    ns_reset_handler = (non_secure_call) (*(((uint32_t *) TZ_START_NS) + 1));

    printf("ns_reset_handler (before cmse_nsfptr_create): 0x%x \r\n", ns_reset_handler); 
    ns_reset_handler = cmse_nsfptr_create(ns_reset_handler); 
    printf("ns_reset_handler (after cmse_nsfptr_create): 0x%x \r\n", ns_reset_handler); 


    /* Start non-secure state software application */
    ns_reset_handler();

    /* Non-secure software does not return, this code is not executed */
    uint32_t count = 0;
    while (1) {
        wait(0.5);
        printf(" Secure main thread: %d \r\n", count ++);
    }
}
