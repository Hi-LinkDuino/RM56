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

#include "bundle_manager.h"
#include "bundle_info.h"
#include "securec.h"

extern "C" {
constexpr static uint8_t MAX_BUNDLE_NAME_LENGTH = 128;

uint8_t GetBundleInfo(const char *bundleName, int32_t flags, BundleInfo *bundleInfo)
{
    if ((bundleName == nullptr) || (bundleInfo == nullptr)) {
        return UINT8_MAX;
    }

    if ((flags < 0) || (flags > 1) || (strlen(bundleName) >= MAX_BUNDLE_NAME_LENGTH)) {
        return UINT8_MAX;
    }

    const uint8_t mockCompatibleApi = 6;
    bundleInfo->compatibleApi = mockCompatibleApi;
    const uint8_t mockTargetApi = 6;
    bundleInfo->targetApi = mockTargetApi;

    return 0;
}
}
