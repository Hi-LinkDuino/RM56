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
#ifndef HIVIEW_SERVICE_ABILITY_STUB_H
#define HIVIEW_SERVICE_ABILITY_STUB_H

#include <string>
#include <mutex>
#include "logger.h"
#include "iremote_stub.h"
#include "message_parcel.h"
#include "nocopyable.h"
#include "ihiview_service_ability.h"

namespace OHOS {
namespace HiviewDFX {
class HiviewServiceAbilityStub : public IRemoteStub<IHiviewServiceAbility> {
public:
    HiviewServiceAbilityStub() {};
    virtual ~HiviewServiceAbilityStub() {};
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // IHIVIEW_SERVICE_H