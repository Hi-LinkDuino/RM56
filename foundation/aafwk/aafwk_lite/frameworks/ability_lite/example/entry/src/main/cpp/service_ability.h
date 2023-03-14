/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_SERVICE_ABILITY_H
#define OHOS_SERVICE_ABILITY_H

#include "ability_loader.h"

namespace OHOS {
class ServiceAbility : public Ability {
protected:
    const SvcIdentity *OnConnect(const Want &want) override;
    void MsgHandle(uint32_t funcId, IpcIo *request, IpcIo *reply) override;
};
}

#endif // OHOS_SERVICE_ABILITY_H
