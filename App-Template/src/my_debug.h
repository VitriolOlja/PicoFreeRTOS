#ifndef MY_DEBUG_H
#define MY_DEBUG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void my_assert_func(const char *file, int line, const char *func, const char *pred)
    __attribute__((noreturn));
#ifdef NDEBUG           /* required by ANSI standard */
#  define myASSERT(__e) ((void)0)
#else
#  define myASSERT(__e) ((__e) ? (void)0 : my_assert_func(__FILE__, __LINE__, __func__, #__e))
#endif

#endif /* MY_DEBUG_H */