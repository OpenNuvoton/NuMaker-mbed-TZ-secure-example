#include <arm_cmse.h>
#include "mbed.h"
#include <stdint.h>

#if !defined(DEVICE_SERIAL) || (DEVICE_SERIAL == 0U)
#define printf(...)
#endif

#if defined(MBED_CONF_APP_TZ_START_NS)
#undef TZ_START_NS
#define TZ_START_NS     MBED_CONF_APP_TZ_START_NS
#endif

/* Start address of non-secure application */
#ifndef TZ_START_NS
#error("TZ_START_NS not defined")
#endif

extern "C" void call_ns_reset_handler(uintptr_t ns_reset_handler_addr);

/* Secure main() */
int main(void)
{
    printf("\r\n");
    printf("+---------------------------------------------+\r\n");
    printf("|    Secure code is running ...               |\r\n");
    printf("+---------------------------------------------+\r\n");
    
    printf("NU_TZ_SECURE_FLASH_SIZE: 0x%08x\n", NU_TZ_SECURE_FLASH_SIZE);
    printf("NU_TZ_SECURE_SRAM_SIZE: 0x%08x\n", NU_TZ_SECURE_SRAM_SIZE);
    printf("NU_TZ_NSC_REGION_START: 0x%08x\n", NU_TZ_NSC_REGION_START);
    printf("NU_TZ_NSC_REGION_SIZE: 0x%08x\n", NU_TZ_NSC_REGION_SIZE);
    
    uintptr_t ns_reset_handler_addr;
 
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
    ns_reset_handler_addr = (uintptr_t) (*(((uint32_t *) TZ_START_NS) + 1));

    /* Start non-secure state software application */
    call_ns_reset_handler(ns_reset_handler_addr);

    /* Non-secure software does not return, this code is not executed */
    uint32_t count = 0;
    while (1) {
        wait(0.5);
        printf(" Secure main thread: %d \r\n", count ++);
    }
}
