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

#include "service_ability.h"

namespace OHOS {
REGISTER_AA(ServiceAbility)

const SvcIdentity *ServiceAbility::OnConnect(const Want &want)
{
    printf("ServiceAbility::OnConnect\n");
    return Ability::OnConnect(want);
}

void ServiceAbility::MsgHandle(uint32_t funcId, IpcIo *request, IpcIo *reply)
{
    printf("ServiceAbility::MsgHandle, funcId is %u\n", funcId);
    int result = 0;
    if (funcId == 0) {
        result = IpcIoPopInt32(request) + IpcIoPopInt32(request);
    }
    // push data
    IpcIoPushInt32(reply, result);
}
}
