/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef FASTMATH_H
#define FASTMATH_H

// TODO: we need to rename this file as it is conflict with standard lib

#define FASTMATH_IMPL_LUT 1
#define FASTMATH_IMPL_APPROX 2

#define FASTMATH_IMPL_METHOD FASTMATH_IMPL_APPROX

#if (FASTMATH_IMPL_METHOD == FASTMATH_IMPL_LUT)
#include "fastmath_impl_lut.h"
#elif (FASTMATH_IMPL_METHOD == FASTMATH_IMPL_APPROX)
#include "fastmath_impl_approx.h"
#else
#error "must choose a fastmath implementation"
#endif

// 10^x = 2^(log2(10)*x)
static inline float fastpow10(float x)
{
    return fastpow2(3.3219281f * x);
}

// e^x = 2^(log2(e)*x)
static inline float fastexp(float x)
{
	return fastpow2(1.4426950f * x);
}

// NOTE: x > 0
// x^y = 2^(y*log2(x))
static inline float fastpow(float x, float y)
{
    return fastpow2(y * fastlog2(x));
}

// log10(x) = log10(2) * log2(x)
static inline float fastlog10(float x)
{
    return 0.3010300f * fastlog2(x);
}

// log(x) = log(2) * log2(x)
static inline float fastlog(float x)
{
	return 0.6931472f * fastlog2(x);
}

#endif
