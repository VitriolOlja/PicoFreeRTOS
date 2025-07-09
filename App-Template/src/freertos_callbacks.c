#include <stdio.h>
#include <stdlib.h>
//
#if PICO_RP2040
#  include "RP2040.h"
#endif
#if PICO_RP2350
#  include "RP2350.h"
#endif
//
#include "FreeRTOS.h"
#include "task.h"

//#include "watchdog.h"
#include "hardware/watchdog.h"
#include "hardware/gpio.h"
#include "pico/time.h"

//#include "crash_flag.h"

/**
 * see https://github.com/carlk3/FreeRTOS-FAT-CLI-for-RPi-Pico/blob/master/src/FreeRTOS%2BFAT%2BCLI/src/freertos_callbacks.c
 * for this file.
 */



/**
 * The stack overflow hook (or stack overflow callback) is a function that is called by the kernel when it detects a
    stack overflow. To use a stack overflow hook function:
 */
void __attribute__((weak))
vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    //cannot printf here
    //although it seems like a nice way. printf seems to use stack which in turn exaggerates the crash
    //https://forums.freertos.org/t/question-vapplicationstackoverflowhook/980/6
    (void) xTask;
    //( void ) pcTaskName;
    /* The stack space has been exceeded for a task, considering allocating more. */

    gpio_init(15);             
    gpio_set_dir(15, GPIO_OUT);

    while (1) {
        gpio_put(15, 1);
        sleep_ms(100);
        gpio_put(15, 0);
        sleep_ms(100);
    }
}

// #ifndef NDEBUG
//  Note: All pvPortMallocs should be checked individually,
//  but we don't expect any to fail,
//  so this can help flag problems in Debug builds.
void __attribute__((weak))
vApplicationMallocFailedHook(void) {
    printf("\nMalloc failed!\n");
    printf(pcTaskGetName(NULL));
    printf("\n");
    taskDISABLE_INTERRUPTS();
    exit(2);
}
// #endif

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide
an implementation of vApplicationGetIdleTaskMemory() to provide the memory that
is used by the Idle task. 
´
From documentation:
    The idle task is run when a core runs out of scheduled work. The idle task performs some housekeeping and
    can also trigger the user's vTaskIdleHook() if it is enabled. In a symmetric multiprocessing system (SMP) there
    are also non-housekeeping idle tasks for each of the remaining cores, but these are statically allocated
    internally to configMINIMUM_STACK_SIZE bytes.

    The vApplicationGetIdleTaskMemory function is called to allow the application to create the needed
    buffers for the "main" idle task. Listing 3.14 shows a typical implementation of the
    vApplicationIdleTaskMemory() function using static local variables to create the needed buffers.

    This is identical to the documentation.
*/
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static – otherwise they will be
    allocated on the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE]
        __attribute__((aligned));

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task’s
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task’s stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetPassiveIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                          StackType_t **ppxIdleTaskStackBuffer,
                                          uint32_t *pulIdleTaskStackSize,
                                          BaseType_t xCoreID) {
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static – otherwise they will be
    allocated on the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB[configNUMBER_OF_CORES];
    static StackType_t uxIdleTaskStack[configNUMBER_OF_CORES][configMINIMAL_STACK_SIZE]
        __attribute__((aligned));

    configASSERT(xCoreID < configNUMBER_OF_CORES);

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task’s
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB[xCoreID];

    /* Pass out the array that will be used as the Idle task’s stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack[xCoreID];

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so
the application must provide an implementation of
vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. 

From the FreeRTOS documentation 3.4.2.1 

*/
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize) {
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static – otherwise they will be
    allocated on the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;

    //in the documentation the use:
    // static StackType_t uxTimerTaskStack[ configMINIMAL_STACK_SIZE ];
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH]
        __attribute__((aligned));

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task’s state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task’s stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */

    //in the documentation they do:
    //*pulTimerTaskStackSize = sizeof(uxTimerTaskStack) / sizeof(*uxTimerTaskStack);
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
