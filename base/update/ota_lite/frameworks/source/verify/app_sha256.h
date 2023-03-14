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

#ifndef APP_SHA256_H
#define APP_SHA256_H

#include "ohos_types.h"
#include "mbedtls/sha256.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define APP_SHA256_FLAG 0
#define APP_SHA256_DIGET_LENGTH 32

typedef struct {
    mbedtls_sha256_context context;
} AppSha256Context;

/* sha256 Initialization */
void AppSha256Init(AppSha256Context* sha256);

/* sha256 calculate data */
void AppSha256Update(AppSha256Context* sha256, const uint8* buffer, uint32 length);

/* sha256 get calculation results */
void AppSha256Finish(AppSha256Context* sha256, uint8* hashOut);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* End of #ifndef APP_SHA256_H */