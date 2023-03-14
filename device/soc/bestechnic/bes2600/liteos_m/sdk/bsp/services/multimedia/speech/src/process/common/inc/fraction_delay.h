#ifndef FRACTION_DELAY_H
#define FRACTION_DELAY_H

#include <stdint.h>

typedef struct FracDelayState_ FracDelayState;

FracDelayState *fraction_delay_init(int frame_size, float delay);

void fraction_delay_destroy(FracDelayState *st);

void fraction_delay_process(FracDelayState *st, int16_t *buf);

void fraction_delay_process2(FracDelayState * st, int16_t * buf, int stride);

int fraction_delay_get_filter_delay(FracDelayState *st);

#endif