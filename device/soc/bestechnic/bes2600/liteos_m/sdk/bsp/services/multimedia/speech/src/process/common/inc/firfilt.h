#ifndef FIRFILT_H
#define FIRFILT_H

#include <stdint.h>

struct FirFiltState_;

typedef struct FirFiltState_ FirFiltState;

#define CONSTRUCT_FUNC_NAME_A(p, c, m)          p ## _ ## c ## _ ## m
#define CONSTRUCT_FUNC_NAME(p, c, m)            CONSTRUCT_FUNC_NAME_A(p, c, m)

#ifndef FIR_IMPL
#if defined(__arm__)
#define FIR_IMPL arm
#else
#define FIR_IMPL raw
#endif
#endif

#define firfilt_init CONSTRUCT_FUNC_NAME(firfilt, FIR_IMPL, init)
#define firfilt_destroy CONSTRUCT_FUNC_NAME(firfilt, FIR_IMPL, destroy)
#define firfilt_process CONSTRUCT_FUNC_NAME(firfilt, FIR_IMPL, process)
#define firfilt_process2 CONSTRUCT_FUNC_NAME(firfilt, FIR_IMPL, process2)

FirFiltState *firfilt_init(int frame_size, float *filter, int filter_length);

void firfilt_destroy(FirFiltState *st);

void firfilt_process(FirFiltState *st, int16_t *buf);

void firfilt_process2(FirFiltState *st, int16_t *buf, int stride);

#endif