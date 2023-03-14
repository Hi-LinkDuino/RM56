/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#include "clock_manager.h"
namespace OHOS {
namespace Media {
namespace Pipeline {
void ClockManager::RegisterProvider(const std::shared_ptr<ClockProvider> &provider)
{
    providerProxy_.SetStub(const_cast<std::shared_ptr<ClockProvider> &>(provider));
}

void ClockManager::UnRegisterProvider(const std::shared_ptr<ClockProvider> &provider)
{
    std::shared_ptr<ClockProvider> ptrRest {nullptr};
    providerProxy_.SetStub(ptrRest);
}

void ClockManager::ClearProviders()
{
    std::shared_ptr<ClockProvider> ptrRest {nullptr};
    providerProxy_.SetStub(ptrRest);
}
} // Pipeline
} // Media
} // OHOS