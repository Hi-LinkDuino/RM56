/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cstddef>
#include <cstdint>

#include "shortcut_info.h"
#include "parcel.h"

#include "shortcutinfo_fuzzer.h"

using namespace OHOS::AppExecFwk;
namespace OHOS {
    bool fuzzelShortcutInfo(const uint8_t* data, size_t size)
    {
        if (size <= 4) {
            return false;
        }
        Parcel dataMessageParcel;
        ShortcutInfo shortcutInfo;
        shortcutInfo.bundleName = reinterpret_cast<const char*>(data);
        if (!shortcutInfo.Marshalling(dataMessageParcel)) {
            return false;
        };
        auto shortCut = ShortcutInfo::Unmarshalling(dataMessageParcel);
        return shortCut != nullptr;
    }
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    OHOS::fuzzelShortcutInfo(data, size);
    return 0;
}