#include <stdint.h>

#ifndef HAL_H
#define HAL_H

enum clock_mode {
    CLOCK_FAST,
    CLOCK_BENCHMARK
};

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void hal_setup(const enum clock_mode clock);
EXTERNC void hal_send_str(const char* in);
EXTERNC uint64_t hal_get_time(void);

#undef EXTERNC

#endif
