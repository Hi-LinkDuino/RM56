/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_DM_THREAD_H
#define OHOS_DM_THREAD_H

#include <pthread.h>
#include <map>
#include <string>
#include<memory>
#include "dm_device_info.h"
#include "softbus_state_callback.h"

namespace OHOS {
namespace DistributedHardware {
class DmThread {
using funcstr = void (*)(std::map<std::string, std::shared_ptr<ISoftbusStateCallback>>, DmDeviceInfo);
public:
    DmThread(funcstr funcname,\
    std::map<std::string, std::shared_ptr<ISoftbusStateCallback>> parameter1,\
    DmDeviceInfo parameter2);
    static void* PthreadDeviceStart(void* parameterInfo);
    void DmCreatThread();

private:
    struct PthreadCallbackParameter {
        std::map<std::string, std::shared_ptr<ISoftbusStateCallback>> member1;
        DmDeviceInfo member2;
    };
    static funcstr funcName_;
    static std::map<std::string, std::shared_ptr<ISoftbusStateCallback>> parameter1_;
    static DmDeviceInfo parameter2_;
    PthreadCallbackParameter parameterGroup_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DM_THREAD_H
