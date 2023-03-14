/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <string.h>

#if defined(MBEDTLS_ENTROPY_C)

#include "mbedtls/entropy.h"
#include "mbedtls/entropy_poll.h"

#if defined(MBEDTLS_TEST_NULL_ENTROPY)
#include "stdlib.h"
#include "hal_trng.h"
int mbedtls_null_entropy_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen )
{
    ((void) data);
    ((void) output);
    int ret;
    uint32_t i;

    len = len > HAL_TRNG_DATA_LEN ? HAL_TRNG_DATA_LEN : len;
    ret = hal_get_trngdata(output, len);
    if (ret) {
        for (i = 0; i < len; i++) {
            output[i] = rand() & 0xFF;
        }
    }

    *olen = len;
    return( 0 );
}
#endif
#endif /* MBEDTLS_ENTROPY_C */