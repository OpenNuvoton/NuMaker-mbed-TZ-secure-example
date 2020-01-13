#include <arm_cmse.h>
#include <stdint.h>

typedef void (*ns_reset_handler_call) (void) __attribute__((cmse_nonsecure_call));

void call_ns_reset_handler(uintptr_t ns_reset_handler_addr)
{
    ns_reset_handler_call ns_reset_handler = (ns_reset_handler_call) ns_reset_handler_addr;

    ns_reset_handler = cmse_nsfptr_create(ns_reset_handler); 

    ns_reset_handler();
}
