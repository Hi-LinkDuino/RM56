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

#ifndef FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_COMMON_UTILS_V8_V8_HELPER_H
#define FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_COMMON_UTILS_V8_V8_HELPER_H

#include "third_party/v8/include/libplatform/libplatform.h"
#include "third_party/v8/include/v8.h"

#include "base/utils/macros.h"

namespace OHOS::Ace::Framework {

// load snapshot, initialize platform, etc. V8Helper con will be called once during the whole process.
class ACE_EXPORT V8Helper {
public:
    static std::unique_ptr<v8::Platform>& GetPlatform();

    std::unique_ptr<v8::Platform> platform_;

private:
    V8Helper();
    ~V8Helper();
};

} // namespace OHOS::Ace::Framework
#endif // FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_COMMON_UTILS_V8_V8_HELPER_H
