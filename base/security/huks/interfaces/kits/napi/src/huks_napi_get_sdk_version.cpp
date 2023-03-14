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

#include "huks_napi_get_sdk_version.h"

#include "hks_api.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_type.h"

namespace HuksNapi {
namespace {
constexpr uint32_t MAX_SDK_VERSION_SIZE = 64;
}

napi_value HuksNapiGetSdkVersion(napi_env env, napi_callback_info info)
{
    HksBlob *sdkVersion = (HksBlob *)HksMalloc(sizeof(HksBlob));
    if (sdkVersion == nullptr) {
        return nullptr;
    }

    sdkVersion->data = (uint8_t *)HksMalloc(MAX_SDK_VERSION_SIZE);
    if (sdkVersion->data == nullptr) {
        HksFree(sdkVersion);
        return nullptr;
    }

    sdkVersion->size = MAX_SDK_VERSION_SIZE;

    int32_t result = HksGetSdkVersion(sdkVersion);
    if (result != HKS_SUCCESS) {
        HksFree(sdkVersion->data);
        HksFree(sdkVersion);
        return nullptr;
    }

    napi_value version = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, (const char *)sdkVersion->data, NAPI_AUTO_LENGTH, &version));
    HksFree(sdkVersion->data);
    HksFree(sdkVersion);
    return version;
}
}  // namespace HuksNapi