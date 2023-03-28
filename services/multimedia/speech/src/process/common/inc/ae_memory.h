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
#ifndef AE_MEMORY_H
#define AE_MEMORY_H

#include <string.h>

#ifdef USE_MED_MALLOC
#include "med_memory.h"
#define AE_MALLOC(size) \
    med_malloc(size)

#define AE_FREE(size) \
    med_free(size)
#else
#include <stdlib.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif
#define AE_MALLOC(size) \
    malloc(size)

#define AE_FREE(size) \
    free(size)
#endif

#define AE_MEMCPY(dst, src, len) \
    memcpy(dst, src, len)

#define AE_MEMSET(p, v, len) \
    memset(p, v, len)

#endif
