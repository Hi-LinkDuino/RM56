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
#include "input_host_callback.h"
#include <iostream>
#include "hdf_log.h"

#define HDF_LOG_TAG InputReportEventCallback

namespace OHOS {
namespace Input {
void InputReportEventCallback::ReportEventPkgCallback(const EventPackage *pkgs, uint32_t count, uint32_t devIndex)
{
    HDF_LOGD("device action pkgs[%{public}p] = count:%{public}d", pkgs, count);
    if (pkgs == NULL) {
        HDF_LOGI("pkgs nulll,");
        return;
    }
    for (uint32_t i = 0; i < count; i++) {
        printf("device action pkgs[%u] = type:0x%x, code:0x%x, value:%d\n",
               i, (pkgs + i)->type, (pkgs + i)->code, (pkgs + i)->value);
    }
}

void InputHostReportEventCallback::ReportHotPlugEventCallback(const HotPlugEvent *msg)
{
    if (msg == NULL) {
        return;
    }
    HDF_LOGD("hogplug action status =%{public}d devId=%{public}d type =%{public}d",
             msg->status, msg->devIndex, msg->devType);
}
}  // namespace Input
}  // namespace OHOS
