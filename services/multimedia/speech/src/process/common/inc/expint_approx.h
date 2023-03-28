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
#ifndef EXPINT_APPROX_H
#define EXPINT_APPROX_H

#include "fastmath.h"
#include "ae_math.h"
#include "ae_common.h"

/*
 * This function computes an approximation to the exponential integral
 * e ~integral from x to infinity of exp(-t) / t
 * References:
 * [1] Abramowitz, M. and I. A. Stegun. Handbook of Mathematical Functions.
 *     Chapter 5, New York: Dover Publications, 1965.
 */
static inline float expint_approx(float x)
{
    AE_ASSERT(4,x > 0, "[%s] input must be postive, but is %s%d.%d", __FUNCTION__,
        AE_SIGN(x), AE_INT(x), AE_FRAC(x));

    float e;
    if (x < 1.f) {
        // Uses the log() plus a polynomial for small values of x.
        e = -fastlog(x) - 0.57721566f + x * (0.99999193f
            - x * (0.24991055f - x * (0.05519968f
                - x * (0.00976004f - x * 0.00107857f))));
    } else if (x < 13) {
        // Uses a ratio of polynomials for larger values of x.
        e = fastexp(-x)* (0.250621f + x * (2.334733f + x)) /
            ((1.681534f + x * (3.330657f + x))*x);
    } else {
        // expint(13) = 1.6219e-07f, almost zeros
        e = 1.6219e-07f;
    }

    return e;
}

#endif
