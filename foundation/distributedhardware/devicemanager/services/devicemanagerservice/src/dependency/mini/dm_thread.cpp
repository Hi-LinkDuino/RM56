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
#include "dm_thread.h"

namespace OHOS {
namespace DistributedHardware {
using funcstr = void (*)(std::map<std::string, std::shared_ptr<ISoftbusStateCallback>>, DmDeviceInfo);

funcstr DmThread::funcName_;
std::map<std::string, std::shared_ptr<ISoftbusStateCallback>> DmThread::parameter1_ = {};
DmDeviceInfo DmThread::parameter2_;

DmThread::DmThread(funcstr funcname,\
    std::map<std::string, std::shared_ptr<ISoftbusStateCallback>> parameter1,\
    DmDeviceInfo parameter2)
{
    funcName_ = funcname;
    parameter1_ = parameter1;
    parameter2_ = parameter2;
    parameterGroup_.member1 = parameter1;
    parameterGroup_.member2 = parameter2;
}

void* DmThread::PthreadDeviceStart(void* parameterInfo)
{
    PthreadCallbackParameter *parameterStruct = static_cast<PthreadCallbackParameter *>(parameterInfo);
    funcName_(parameterStruct->member1, parameterStruct->member2);
    return nullptr;
}

void DmThread::DmCreatThread()
{
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, PthreadDeviceStart, static_cast<void *>(&parameterGroup_));
}
} // namespace DistributedHardware
} // namespace OHOS
