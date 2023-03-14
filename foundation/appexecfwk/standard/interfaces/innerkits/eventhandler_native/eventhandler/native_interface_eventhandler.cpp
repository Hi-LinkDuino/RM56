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

#include "native_interface_eventhandler.h"
#include "hilog/log.h"
#include "native_implement_eventhandler.h"

#include <cstdint>

using OHOS::ErrCode;
using OHOS::HiviewDFX::HiLog;
using OHOS::HiviewDFX::HiLogLabel;

namespace {
constexpr HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "EventRunnerNativeInterface"};
}

const EventRunnerNativeImplement *GetEventRunnerNativeObjForThread()
{
    return EventRunnerNativeImplement::GetEventRunnerNativeObj();
}

const EventRunnerNativeImplement *CreateEventRunnerNativeObj()
{
    return EventRunnerNativeImplement::CreateEventRunnerNativeObj();
}

int EventRunnerRun(const EventRunnerNativeImplement *nativeObj)
{
    if (nativeObj == nullptr) {
        HiLog::Error(LABEL, "Input nkd object is null");
        return OHOS::AppExecFwk::EVENT_HANDLER_ERR_INVALID_PARAM;
    }
    return nativeObj->RunEventRunnerNativeObj();
}

int EventRunnerStop(const EventRunnerNativeImplement *nativeObj)
{
    if (nativeObj == nullptr) {
        HiLog::Error(LABEL, "Input nkd object is null");
        return OHOS::AppExecFwk::EVENT_HANDLER_ERR_INVALID_PARAM;
    }
    return nativeObj->StopEventRunnerNativeObj();
}

int EventRunnerAddFileDescriptorListener(const EventRunnerNativeImplement *nativeObj, int32_t fileDescriptor,
    uint32_t events, const FileDescriptorCallbacks *fileDescriptorCallbacks)
{
    if (nativeObj == nullptr) {
        HiLog::Error(LABEL, "Input nkd object is null");
        return OHOS::AppExecFwk::EVENT_HANDLER_ERR_INVALID_PARAM;
    }
    return nativeObj->AddFileDescriptorListener(fileDescriptor, events, fileDescriptorCallbacks);
}

void EventRunnerRemoveFileDescriptorListener(const EventRunnerNativeImplement *nativeObj, int32_t fileDescriptor)
{
    if (nativeObj == nullptr) {
        HiLog::Error(LABEL, "Input nkd object is null");
        return;
    }
    nativeObj->RemoveFileDescriptorListener(fileDescriptor);
}
