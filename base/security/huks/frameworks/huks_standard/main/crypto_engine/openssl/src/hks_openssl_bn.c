/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "hks_openssl_bn.h"

#include "hks_log.h"
#include "hks_mem.h"
#include "hks_openssl_engine.h"

static void BnFreeParams(struct HksBnExpModParams *bnParams)
{
    BN_free(bnParams->bnX);
    BN_free(bnParams->bnA);
    BN_free(bnParams->bnE);
    BN_free(bnParams->bnN);
    BN_CTX_free(bnParams->ctx);

    bnParams->bnX = NULL;
    bnParams->bnA = NULL;
    bnParams->bnE = NULL;
    bnParams->bnN = NULL;
    bnParams->ctx = NULL;
}

static int32_t BnBuildParams(
    struct HksBnExpModParams *bnParams, const struct HksBlob *a, const struct HksBlob *e, const struct HksBlob *n)
{
    bnParams->ctx = BN_CTX_new();
    bnParams->bnX = BN_new();
    bnParams->bnA = BN_bin2bn(a->data, a->size, NULL);
    bnParams->bnE = BN_bin2bn(e->data, e->size, NULL);
    bnParams->bnN = BN_bin2bn(n->data, n->size, NULL);
    if ((bnParams->ctx == NULL) || (bnParams->bnX == NULL) || (bnParams->bnA == NULL) || (bnParams->bnE == NULL) ||
        (bnParams->bnN == NULL)) {
        BnFreeParams(bnParams);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }
    return HKS_SUCCESS;
}

static int32_t BnExpModExport(BIGNUM *bnX, struct HksBlob *x)
{
    int32_t outLen = BN_num_bytes(bnX);
    if ((outLen < 0) || (x->size < (uint32_t)outLen)) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    (void)memset_s(x->data, x->size, 0, x->size);
    if (outLen == 0) {
        return HKS_SUCCESS;
    }

    uint8_t *bnOutput = (uint8_t *)HksMalloc(outLen);
    if (bnOutput == NULL) {
        HKS_LOG_E("malloc fail");
        return HKS_ERROR_MALLOC_FAIL;
    }

    int32_t ret = HKS_SUCCESS;
    do {
        int32_t realOutLen = BN_bn2bin(bnX, bnOutput);
        if (realOutLen != outLen) {
            HKS_LOG_E("BN_bn2bin fail");
            ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
            break;
        }

        uint32_t i = x->size - 1;
        int32_t j = realOutLen - 1; /* realOutLen is greater than 0; x->size is no less than realOutLen */
        for (; j >= 0; --i, --j) {  /* i is no less than j */
            x->data[i] = bnOutput[j];
        }
    } while (0);

    HksFree(bnOutput);
    return ret;
}

int32_t HksOpensslBnExpMod(struct HksBlob *x, const struct HksBlob *a,
    const struct HksBlob *e, const struct HksBlob *n)
{
    struct HksBnExpModParams bnParams;
    (void)memset_s(&bnParams, sizeof(bnParams), 0, sizeof(bnParams));
    int32_t ret = BnBuildParams(&bnParams, a, e, n);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("BnInitParams fail");
        return ret;
    }

    do {
        /* mod 0 is not supported */
        if (BN_is_zero(bnParams.bnN)) {
            HKS_LOG_E("not support mod 0 operation.");
            ret = HKS_ERROR_INVALID_ARGUMENT;
            break;
        }

        ret = BN_mod_exp(bnParams.bnX, bnParams.bnA, bnParams.bnE, bnParams.bnN, bnParams.ctx);
        if (ret != HKS_OPENSSL_SUCCESS) {
            HKS_LOG_E("BN_mod_exp fail, ret = %d", ret);
            ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
            break;
        }

        ret = BnExpModExport(bnParams.bnX, x);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("BnExpModExport fail");
            break;
        }
    } while (0);

    BnFreeParams(&bnParams);
    return ret;
}
#endif /* HKS_SUPPORT_BN_C */
