/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hks_openssl_common.h"

#include <openssl/rand.h>

#include "hks_log.h"
#include "hks_mem.h"
#include "hks_openssl_engine.h"
#include "hks_type_inner.h"

int32_t HksOpensslGenerateRandomKey(const uint32_t keySize, struct HksBlob *key)
{
    uint32_t keySizeByte = keySize / BIT_NUM_OF_UINT8;
    int32_t ret = HKS_FAILURE;

    uint8_t *tmpKey = (uint8_t *)HksMalloc(keySizeByte);
    if (tmpKey == NULL) {
        HKS_LOG_E("malloc buffer failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    do {
        if (RAND_bytes(tmpKey, keySizeByte) <= 0) {
            HKS_LOG_E("generate key is failed:0x%x", ret);
            break;
        }

        key->data = tmpKey;
        key->size = keySizeByte;
        ret = HKS_SUCCESS;
    } while (0);

    if (ret != HKS_SUCCESS) {
        (void)memset_s(tmpKey, keySizeByte, 0, keySizeByte);
        HksFree(tmpKey);
    }
    return ret;
}

int32_t HksOpensslFillRandom(struct HksBlob *randomData)
{
    int ret = RAND_bytes(randomData->data, randomData->size);
    if (ret <= 0) {
        HKS_LOG_E("generate random failed, ret = 0x%x", ret);
        return HKS_FAILURE;
    }

    if (randomData->size == 1) {
        return HKS_SUCCESS;
    }

    uint32_t j = 0;

    for (uint32_t i = 0; i < randomData->size; i++) {
        if (randomData->data[i] == 0) {
            j++;
        }
    }
    if (j == randomData->size) {
        HKS_LOG_E("fill random failed, size %x", randomData->size);
        return HKS_ERROR_UNKNOWN_ERROR;
    }
    HKS_LOG_D("generate random success");
    return HKS_SUCCESS;
}