/* my_debug.c */



#if PICO_RP2040
#  include "RP2040.h"
#endif
#if PICO_RP2350
#  include "RP2350.h"
#endif
#include "pico/stdlib.h"
//
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
//
#include "my_debug.h"


void __attribute__((weak)) my_assert_func(const char *file, int line, const char *func, const char *pred) {
    printf("ASSERT FAILED: %s:%d (%s): %s\n", file, line, func, pred);
    stdio_flush();
    vTaskSuspendAll();
    __disable_irq(); /* Disable global interrupts. */

    while (1) {
        __wfi(); // wait for interrupt to reduce power usage
    }
}
