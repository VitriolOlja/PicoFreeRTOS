#include "crash_flag.h"

__attribute__((section(".noinit"), aligned(4))) volatile uint32_t crash_flag;
