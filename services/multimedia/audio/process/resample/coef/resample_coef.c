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
#include "plat_types.h"
#include "hal_location.h"
#include "resample_coef.h"

#define COEF_DEF(n)                         FLASH_RODATA_DEF(n)

static const int16_t COEF_DEF(filter_50p7k_to_48k)[] = {
    #include "resample_50p7k_to_48k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_50p7k_to_48k) = {
    .upsample_factor = 69,
    .downsample_factor = 73,
    .phase_coef_num = 6,
    .total_coef_num = ARRAY_SIZE(filter_50p7k_to_48k),
    .coef_group = filter_50p7k_to_48k,
};

static const int16_t COEF_DEF(filter_50p7k_to_44p1k)[] = {
    #include "resample_50p7k_to_44p1k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_50p7k_to_44p1k) = {
    .upsample_factor = 33,
    .downsample_factor = 38,
    .phase_coef_num = 6,
    .total_coef_num = ARRAY_SIZE(filter_50p7k_to_44p1k),
    .coef_group = filter_50p7k_to_44p1k,
};

static const int16_t COEF_DEF(filter_44p1k_to_16k)[] = {
    #include "resample_44p1k_to_16k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_44p1k_to_16k) = {
    .upsample_factor = 160,
    .downsample_factor = 441,
    .phase_coef_num = 56,
    .total_coef_num = ARRAY_SIZE(filter_44p1k_to_16k),
    .coef_group = filter_44p1k_to_16k,
};

static const int16_t COEF_DEF(filter_44p1k_to_48k)[] = {
    #include "resample_44p1k_to_48k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_44p1k_to_48k) = {
    .upsample_factor = 160,
    .downsample_factor = 147,
    .phase_coef_num = 42,
    .total_coef_num = ARRAY_SIZE(filter_44p1k_to_48k),
    .coef_group = filter_44p1k_to_48k,
};

static const int16_t COEF_DEF(filter_44p1k_to_48k_2)[] = {
    #include "resample_44p1k_to_48k_filter2.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_44p1k_to_48k_2) = {
    .upsample_factor = 160,
    .downsample_factor = 147,
    .phase_coef_num = 24,
    .total_coef_num = ARRAY_SIZE(filter_44p1k_to_48k_2),
    .coef_group = filter_44p1k_to_48k_2,
};

static const int16_t COEF_DEF(filter_48k_to_44p1k)[] = {
    #include "resample_48k_to_44p1k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_48k_to_44p1k) = {
    .upsample_factor = 147,
    .downsample_factor = 160,
    .phase_coef_num = 8,
    .total_coef_num = ARRAY_SIZE(filter_48k_to_44p1k),
    .coef_group = filter_48k_to_44p1k,
};

static const int16_t COEF_DEF(filter_48k_to_44p1k_2)[] = {
    #include "resample_48k_to_44p1k_filter2.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_48k_to_44p1k_2) = {
    .upsample_factor = 147,
    .downsample_factor = 160,
    .phase_coef_num = 24,
    .total_coef_num = ARRAY_SIZE(filter_48k_to_44p1k_2),
    .coef_group = filter_48k_to_44p1k_2,
};

static const int16_t COEF_DEF(filter_32k_to_50p7k)[] = {
    #include "resample_32k_to_50p7k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_32k_to_50p7k) = {
    .upsample_factor = 73,
    .downsample_factor = 46,
    .phase_coef_num = 24,
    .total_coef_num = ARRAY_SIZE(filter_32k_to_50p7k),
    .coef_group = filter_32k_to_50p7k,
};

static const int16_t COEF_DEF(filter_44p1k_to_50p7k)[] = {
    #include "resample_44p1k_to_50p7k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_44p1k_to_50p7k) = {
    .upsample_factor = 38,
    .downsample_factor = 33,
    .phase_coef_num = 24,
    .total_coef_num = ARRAY_SIZE(filter_44p1k_to_50p7k),
    .coef_group = filter_44p1k_to_50p7k,
};

static const int16_t COEF_DEF(filter_48k_to_50p7k)[] = {
    #include "resample_48k_to_50p7k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_48k_to_50p7k) = {
    .upsample_factor = 73,
    .downsample_factor = 69,
    .phase_coef_num = 24,
    .total_coef_num = ARRAY_SIZE(filter_48k_to_50p7k),
    .coef_group = filter_48k_to_50p7k,
};

static const int16_t COEF_DEF(filter_8k_to_8p4k)[] = {
    #include "resample_8k_to_8p4k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_8k_to_8p4k) = {
    .upsample_factor = 73,
    .downsample_factor = 69,
    .phase_coef_num = 18,
    .total_coef_num = ARRAY_SIZE(filter_8k_to_8p4k),
    .coef_group = filter_8k_to_8p4k,
};

static const int16_t COEF_DEF(filter_8p4k_to_8k)[] = {
    #include "resample_8p4k_to_8k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_8p4k_to_8k) = {
    .upsample_factor = 69,
    .downsample_factor = 73,
    .phase_coef_num = 16,
    .total_coef_num = ARRAY_SIZE(filter_8p4k_to_8k),
    .coef_group = filter_8p4k_to_8k,
};

static const int16_t COEF_DEF(filter_16k_to_48k)[] = {
    #include "resample_16k_to_48k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_16k_to_48k) = {
    .upsample_factor = 3,
    .downsample_factor = 1,
    .phase_coef_num = 30,
    .total_coef_num = ARRAY_SIZE(filter_16k_to_48k),
    .coef_group = filter_16k_to_48k,
};

static const int16_t COEF_DEF(filter_48k_to_16k)[] = {
    #include "resample_48k_to_16k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_48k_to_16k) = {
    .upsample_factor = 1,
    .downsample_factor = 3,
    .phase_coef_num = 24,
    .total_coef_num = ARRAY_SIZE(filter_48k_to_16k),
    .coef_group = filter_48k_to_16k,
};

static const int16_t COEF_DEF(filter_48k_to_16k_2)[] = {
    #include "resample_48k_to_16k_filter2.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_48k_to_16k_2) = {
    .upsample_factor = 1,
    .downsample_factor = 3,
    .phase_coef_num = 72,
    .total_coef_num = ARRAY_SIZE(filter_48k_to_16k_2),
    .coef_group = filter_48k_to_16k_2,
};

static const int16_t COEF_DEF(filter_48k_to_32k)[] = {
    #include "resample_48k_to_32k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_48k_to_32k) = {
    .upsample_factor = 2,
    .downsample_factor = 3,
    .phase_coef_num = 32,
    .total_coef_num = ARRAY_SIZE(filter_48k_to_32k),
    .coef_group = filter_48k_to_32k,
};

static const int16_t COEF_DEF(filter_48k_to_32k_2)[] = {
    #include "resample_48k_to_32k_filter2.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_48k_to_32k_2) = {
    .upsample_factor = 2,
    .downsample_factor = 3,
    .phase_coef_num = 36,
    .total_coef_num = ARRAY_SIZE(filter_48k_to_32k_2),
    .coef_group = filter_48k_to_32k_2,
};

static const int16_t COEF_DEF(filter_24k_to_44p1k)[] = {
    #include "resample_24k_to_44p1k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_24k_to_44p1k) = {
    .upsample_factor = 147,
    .downsample_factor = 80,
    .phase_coef_num = 24,
    .total_coef_num = ARRAY_SIZE(filter_24k_to_44p1k),
    .coef_group = filter_24k_to_44p1k,
};

static const int16_t COEF_DEF(filter_44p1k_to_24k)[] = {
    #include "resample_44p1k_to_24k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_44p1k_to_24k) = {
    .upsample_factor = 80,
    .downsample_factor = 147,
    .phase_coef_num = 40,
    .total_coef_num = ARRAY_SIZE(filter_44p1k_to_24k),
    .coef_group = filter_44p1k_to_24k,
};

static const int16_t COEF_DEF(filter_32k_to_48k)[] = {
    #include "resample_32k_to_48k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_32k_to_48k) = {
    .upsample_factor = 3,
    .downsample_factor = 2,
    .phase_coef_num = 24,
    .total_coef_num = ARRAY_SIZE(filter_32k_to_48k),
    .coef_group = filter_32k_to_48k,
};

static const int16_t COEF_DEF(filter_96k_to_16k)[] = {
    #include "resample_96k_to_16k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_96k_to_16k) = {
    .upsample_factor = 1,
    .downsample_factor = 6,
    .phase_coef_num = 144,
    .total_coef_num = ARRAY_SIZE(filter_96k_to_16k),
    .coef_group = filter_96k_to_16k,
};

static const int16_t COEF_DEF(filter_48k_to_24k_or_96k_to_48k_or_16k_to_8k)[] = {
    #include "resample_48k_to_24k_or_96k_to_48k_or_16k_to_8k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_48k_to_24k_or_96k_to_48k_or_16k_to_8k) = {
    .upsample_factor = 1,
    .downsample_factor = 2,
    .phase_coef_num = 48,
    .total_coef_num = ARRAY_SIZE(filter_48k_to_24k_or_96k_to_48k_or_16k_to_8k),
    .coef_group = filter_48k_to_24k_or_96k_to_48k_or_16k_to_8k,
};

static const int16_t COEF_DEF(filter_24k_to_48k_or_48k_to_96k_or_8k_to_16k)[] = {
    #include "resample_24k_to_48k_or_48k_to_96k_or_8k_to_16k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_24k_to_48k_or_48k_to_96k_or_8k_to_16k) = {
    .upsample_factor = 2,
    .downsample_factor = 1,
    .phase_coef_num = 24,
    .total_coef_num = ARRAY_SIZE(filter_24k_to_48k_or_48k_to_96k_or_8k_to_16k),
    .coef_group = filter_24k_to_48k_or_48k_to_96k_or_8k_to_16k,
};

static const int16_t COEF_DEF(filter_16k_to_48k_or_32k_to_96k)[] = {
    #include "resample_16k_to_48k_or_32k_to_96k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_16k_to_48k_or_32k_to_96k) = {
    .upsample_factor = 3,
    .downsample_factor = 1,
    .phase_coef_num = 24,
    .total_coef_num = ARRAY_SIZE(filter_16k_to_48k_or_32k_to_96k),
    .coef_group = filter_16k_to_48k_or_32k_to_96k,
};

static const int16_t COEF_DEF(filter_16k_to_96k_or_8k_to_48k)[] = {
    #include "resample_16k_to_96k_or_8k_to_48k_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_16k_to_96k_or_8k_to_48k) = {
    .upsample_factor = 6,
    .downsample_factor = 1,
    .phase_coef_num = 24,
    .total_coef_num = ARRAY_SIZE(filter_16k_to_96k_or_8k_to_48k),
    .coef_group = filter_16k_to_96k_or_8k_to_48k,
};

#ifdef RESAMPLE_ANY_SAMPLE_RATE
static const int16_t COEF_DEF(filter_any_up64)[] = {
    #include "resample_any_up64_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_any_up64) = {
    .upsample_factor = 64,
    .downsample_factor = 0,
    .phase_coef_num = 32,
    .total_coef_num = ARRAY_SIZE(filter_any_up64),
    .coef_group = filter_any_up64,
};

static const int16_t COEF_DEF(filter_any_up256)[] = {
    #include "resample_any_up256_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_any_up256) = {
    .upsample_factor = 256,
    .downsample_factor = 0,
    .phase_coef_num = 24,
    .total_coef_num = ARRAY_SIZE(filter_any_up256),
    .coef_group = filter_any_up256,
};

#ifdef ALSA_RESAMPLE_IN_CP
static const int16_t filter_any_up256_len64[] = {
    #include "resample_any_up256_filter64.txt"
};
const struct RESAMPLE_COEF_T resample_coef_any_up256_len64 = {
    .upsample_factor = 256,
    .downsample_factor = 0,
    .phase_coef_num = 64,
    .total_coef_num = ARRAY_SIZE(filter_any_up256_len64),
    .coef_group = filter_any_up256_len64,
};
#else
static const int16_t COEF_DEF(filter_any_up256_len64)[] = {
    #include "resample_any_up256_filter64.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_any_up256_len64) = {
    .upsample_factor = 256,
    .downsample_factor = 0,
    .phase_coef_num = 64,
    .total_coef_num = ARRAY_SIZE(filter_any_up256_len64),
    .coef_group = filter_any_up256_len64,
};
#endif

// #define USE_FILTER_ANY_UP384_USE_1 /* should compensate resample gain loss*/
// #define USE_FILTER_ANY_UP384_USE_2 /* len = 64*/
#define USE_FILTER_ANY_UP384_USE_3    /* len = 96*/

#if 0 //#ifdef ALSA_RESAMPLE_IN_CP
#ifdef USE_FILTER_ANY_UP384_USE_1
#endif
#ifdef USE_FILTER_ANY_UP384_USE_2
#endif
#ifdef USE_FILTER_ANY_UP384_USE_3
#endif
static const int16_t filter_any_up384[] = {
#ifdef USE_FILTER_ANY_UP384_USE_1
    #include "resample_any_up384_filter1.txt" /* should compensate resample gain loss*/
#endif
#ifdef USE_FILTER_ANY_UP384_USE_2
    #include "resample_any_up384_filter2.txt"
#endif
#ifdef USE_FILTER_ANY_UP384_USE_3
    #include "resample_any_len96up384_filter3.txt"
#endif
};
const struct RESAMPLE_COEF_T resample_coef_any_up384 = {
    .upsample_factor = 384,
    .downsample_factor = 0,
#ifdef USE_FILTER_ANY_UP384_USE_1
    .phase_coef_num = 64,
#endif
#ifdef USE_FILTER_ANY_UP384_USE_2
    .phase_coef_num = 64,
#endif
#ifdef USE_FILTER_ANY_UP384_USE_3
    .phase_coef_num = 96,
#endif
    .total_coef_num = ARRAY_SIZE(filter_any_up384),
    .coef_group = filter_any_up384,
};
#else
static const int16_t COEF_DEF(filter_any_up384)[] = {
#ifdef USE_FILTER_ANY_UP384_USE_1
    #include "resample_any_up384_filter1.txt" /* should compensate resample gain loss*/
#endif
#ifdef USE_FILTER_ANY_UP384_USE_2
    #include "resample_any_up384_filter2.txt"
#endif
#ifdef USE_FILTER_ANY_UP384_USE_3
    #include "resample_any_len96up384_filter3.txt"
#endif
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_any_up384) = {
    .upsample_factor = 384,
    .downsample_factor = 0,
#ifdef USE_FILTER_ANY_UP384_USE_1
    .phase_coef_num = 64,
#endif
#ifdef USE_FILTER_ANY_UP384_USE_2
    .phase_coef_num = 64,
#endif
#ifdef USE_FILTER_ANY_UP384_USE_3
    .phase_coef_num = 96,
#endif
    .total_coef_num = ARRAY_SIZE(filter_any_up384),
    .coef_group = filter_any_up384,
};
#endif
static const int16_t COEF_DEF(filter_any_up512_32)[] = {
    #include "resample_any_up512_32_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_any_up512_32) = {
    .upsample_factor = 512,
    .downsample_factor = 0,
    .phase_coef_num = 32,
    .total_coef_num = ARRAY_SIZE(filter_any_up512_32),
    .coef_group = filter_any_up512_32,
};

static const int16_t COEF_DEF(filter_any_up512_36)[] = {
    #include "resample_any_up512_36_filter.txt"
};
const struct RESAMPLE_COEF_T COEF_DEF(resample_coef_any_up512_36) = {
    .upsample_factor = 512,
    .downsample_factor = 0,
    .phase_coef_num = 36,
    .total_coef_num = ARRAY_SIZE(filter_any_up512_36),
    .coef_group = filter_any_up512_36,
};
#endif

