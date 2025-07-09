#ifndef CRASH_FLAG_H
#define CRASH_FLAG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//
#if PICO_RP2040
#  include "RP2040.h"
#endif
#if PICO_RP2350
#  include "RP2350.h"
#endif


/*  this is a flash that we turn on in case we received stack overflowhook, 
    we then soft reboot the system 
    This should tell the linker to put this in a region of RAM that is not zeroed on boot. 

    on my application it compiles to:
    (check with arm-none-eabi-nm -S --size-sort my_app.elf | grep crash_flag)

    20025b48 00000004 B crash_flag

    this is not correct.

    so we add a linker script
    see ../linker/noinit.ld

    and add it to 
    cmake target_link_options(my_app PRIVATE "-T${CMAKE_SOURCE_DIR}/linker/noinit.ld")

    then we rerunning 

    arm-none-eabi-nm -S --size-sort my_app.elf | grep crash_flag
    20030000 00000004 B crash_flag

    Hmm. ill save it for now. 
    seems the watchdog reboot resets everything anyways
    ```
    // Reset everything apart from ROSC and XOSC
    hw_set_bits(&psm_hw->wdsel, PSM_WDSEL_BITS & ~(PSM_WDSEL_ROSC_BITS | PSM_WDSEL_XOSC_BITS));
    ```


*/
extern __attribute__((section(".noinit"), aligned(4))) volatile uint32_t crash_flag;


#endif /* CRASH_FLAG_H */