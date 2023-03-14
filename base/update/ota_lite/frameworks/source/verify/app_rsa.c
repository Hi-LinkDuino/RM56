/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "app_rsa.h"

#include <mbedtls/entropy.h>

#include "ohos_types.h"

void AppRsaInit(AppRsaContext *rsa)
{
    if (rsa == NULL) {
        return;
    }
    mbedtls_pk_init(&rsa->context);
    return;
}

int32 AppRsaDecodePublicKey(AppRsaContext *rsa, const uint8 *publicKey, uint32 length)
{
    if ((rsa == NULL) || (publicKey == NULL)) {
        return OHOS_FAILURE;
    }

    int32 parseRet = mbedtls_pk_parse_public_key(&rsa->context, publicKey, length);
    if (parseRet != RSA_SUCCESS) {
        return OHOS_FAILURE;
    }

    return OHOS_SUCCESS;
}

int32 AppVerifyData(AppRsaContext *rsa, const uint8 *plainBuf, uint32 plainBufLen, const uint8 *cipherBuf,
    uint32 cipherBufLen)
{
    int32 ret;

    if ((rsa == NULL) || (plainBuf == NULL) || (cipherBuf == NULL) || (plainBufLen == 0) || (cipherBufLen == 0)) {
        printf("input error.\r\n");
        return OHOS_FAILURE;
    }

    if (!mbedtls_pk_can_do(&(rsa->context), MBEDTLS_PK_RSA)) {
        printf(" failed\n  ! Key is not an RSA key\n");
        return OHOS_FAILURE;
    }
    mbedtls_entropy_context entropy;
    mbedtls_rsa_set_padding(mbedtls_pk_rsa(rsa->context), MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
    mbedtls_entropy_init(&entropy);
    if ((ret = mbedtls_rsa_pkcs1_verify(mbedtls_pk_rsa(rsa->context), NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256,
        plainBufLen, plainBuf, cipherBuf)) != 0) {
        printf("sign failed. %x\r\n", -ret);
        return OHOS_FAILURE;
    }

    return OHOS_SUCCESS;
}

void AppRsaFree(AppRsaContext *rsa)
{
    if (rsa == NULL) {
        return;
    }

    mbedtls_pk_free(&rsa->context);
    return;
}
