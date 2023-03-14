/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "app_sha256.h"
#include "securec.h"

void AppSha256Init(AppSha256Context *sha256)
{
    if (sha256 == NULL) {
        return;
    }

    mbedtls_sha256_init(&sha256->context);
    mbedtls_sha256_starts_ret(&sha256->context, APP_SHA256_FLAG);
    return;
}

void AppSha256Update(AppSha256Context *sha256, const uint8 *buffer, uint32 length)
{
    if ((buffer == NULL) || (sha256 == NULL)) {
        return;
    }

    mbedtls_sha256_update_ret(&sha256->context, buffer, length);
    return;
}

void AppSha256Finish(AppSha256Context *sha256, uint8 *hashOut)
{
    if ((hashOut == NULL) || (sha256 == NULL)) {
        return;
    }

    mbedtls_sha256_finish_ret(&sha256->context, hashOut);
    return;
}
