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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_MOCK_MOCK_RESOURCE_REGISTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_MOCK_MOCK_RESOURCE_REGISTER_H

#include "core/common/platform_res_register.h"

namespace OHOS::Ace {

class MockResourceRegister final : public PlatformResRegister {
public:
    MockResourceRegister() = default;
    ~MockResourceRegister() override = default;

    int64_t CreateResource(const std::string& resourceType, const std::string& param) override
    {
        return -1;
    }

    bool ReleaseResource(const std::string& resourceHash) override
    {
        return false;
    }

    bool OnMethodCall(const std::string& method, const std::string& param, std::string& result) override
    {
        return false;
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_MOCK_MOCK_RESOURCE_REGISTER_H
