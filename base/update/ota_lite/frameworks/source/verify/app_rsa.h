/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef APP_RSA_H
#define APP_RSA_H

#include "ohos_types.h"
#include "mbedtls/pk.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define RSA_SUCCESS 0

typedef struct {
    mbedtls_pk_context context;
} AppRsaContext;

/*
 * RSA Initialization
 */
void AppRsaInit(AppRsaContext *rsa);

/*
 * RSA decode public key
 */
int32 AppRsaDecodePublicKey(AppRsaContext *rsa, const uint8 *publicKey, uint32 length);

/*
 * RSA verify data
 */
int32 AppVerifyData(AppRsaContext *rsa, const uint8 *plainBuf, uint32 plainBufLen, const uint8 *cipherBuf,
    uint32 cipherBufLen);

/*
 * RSA free context
 */
void AppRsaFree(AppRsaContext *rsa);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* End of #ifndef APP_RSA_H */
