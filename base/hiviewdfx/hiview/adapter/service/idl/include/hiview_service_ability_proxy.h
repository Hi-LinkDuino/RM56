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
#ifndef HIVIEW_SERVICE_ABILITY_PROXY_H
#define HIVIEW_SERVICE_ABILITY_PROXY_H

#include <string>
#include "iremote_proxy.h"
#include "nocopyable.h"
#include "ihiview_service_ability.h"

namespace OHOS {
namespace HiviewDFX {
class HiviewServiceAbilityProxy : public IRemoteProxy<IHiviewServiceAbility> {
public:
    explicit HiviewServiceAbilityProxy(const sptr<IRemoteObject> &remote) : IRemoteProxy<IHiviewServiceAbility>(remote)
    {}
    virtual ~HiviewServiceAbilityProxy() = default;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_SERVICE_ABILITY_PROXY_H
