/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#ifdef HKS_SUPPORT_BN_C

#include "hks_mbedtls_bn.h"

#include <mbedtls/bignum.h>

#include "hks_log.h"
#include "hks_mbedtls_common.h"

static int32_t CheckBnExpModNx(const struct HksBlob *n, const struct HksBlob *x)
{
    /* zero is even number, so doesn't have to be checked */
    if ((n->data[n->size - 1] & 0x1) == 0x0) {
        HKS_LOG_E("The param n(modular) must be odd!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (x->size < n->size) {
        HKS_LOG_E("The param x's size is too samll! x size = 0x%X", x->size);
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    return HKS_SUCCESS;
}

int32_t HksMbedtlsBnExpMod(struct HksBlob *x, const struct HksBlob *a,
    const struct HksBlob *e, const struct HksBlob *n)
{
    int32_t ret = CheckBnExpModNx(n, x);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    mbedtls_mpi bnX;
    mbedtls_mpi bnA;
    mbedtls_mpi bnE;
    mbedtls_mpi bnN;

    mbedtls_mpi_init(&bnX);
    mbedtls_mpi_init(&bnA);
    mbedtls_mpi_init(&bnE);
    mbedtls_mpi_init(&bnN);

    do {
        ret = mbedtls_mpi_read_binary(&bnA, a->data, a->size);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls mpi read a failed! mbedtls ret = 0x%X", ret);
            break;
        }
        ret = mbedtls_mpi_read_binary(&bnE, e->data, e->size);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls mpi read e failed! mbedtls ret = 0x%X", ret);
            break;
        }
        ret = mbedtls_mpi_read_binary(&bnN, n->data, n->size);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls mpi read n failed! mbedtls ret = 0x%X", ret);
            break;
        }

        ret = mbedtls_mpi_exp_mod(&bnX, &bnA, &bnE, &bnN, NULL);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls exp mod failed! mbedtls ret = 0x%X", ret);
            break;
        }

        ret = mbedtls_mpi_write_binary(&bnX, x->data, x->size);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls mpi write x failed! mbedtls ret = 0x%X", ret);
        }
    } while (0);

    mbedtls_mpi_free(&bnX);
    mbedtls_mpi_free(&bnA);
    mbedtls_mpi_free(&bnE);
    mbedtls_mpi_free(&bnN);
    return ret;
}
#endif /* HKS_SUPPORT_BN_C */
