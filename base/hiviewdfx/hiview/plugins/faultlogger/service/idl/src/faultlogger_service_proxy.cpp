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
#include "faultlogger_service_proxy.h"


#include "ipc_types.h"
#include "message_parcel.h"

#include "faultlog_info_ohos.h"
#include "faultlog_query_result_proxy.h"
#include "hilog/log_cpp.h"

namespace OHOS {
namespace HiviewDFX {
static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, 0xD002D10, "FaultLoggerProxy"};
void FaultLoggerServiceProxy::AddFaultLog(const FaultLogInfoOhos& info)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(FaultLoggerServiceProxy::GetDescriptor())) {
        return;
    }

    if (!info.Marshalling(data)) {
        return;
    }

    auto flags = option.GetFlags();
    option.SetFlags(flags | 0x01); // 0X01 return immediately
    if (remote->SendRequest(static_cast<int>(IFaultLoggerService::IFaultLoggerService_ADD_FAULTLOG),
        data, reply, option) != ERR_OK) {
        return;
    }
}

sptr<IRemoteObject> FaultLoggerServiceProxy::QuerySelfFaultLog(int32_t faultType, int32_t maxNum)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(FaultLoggerServiceProxy::GetDescriptor())) {
        return nullptr;
    }

    if (!data.WriteInt32(faultType)) {
        return nullptr;
    }

    if (!data.WriteInt32(maxNum)) {
        return nullptr;
    }

    if (remote->SendRequest(static_cast<int>(IFaultLoggerService::IFaultLoggerService_QUERY_SELF_FAULTLOG),
        data, reply, option) != ERR_OK) {
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = reply.ReadRemoteObject();
    if (remoteObject == nullptr) {
        OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, "Failed to transfer Result.");
    }
    return remoteObject;
}

void FaultLoggerServiceProxy::Destroy()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(FaultLoggerServiceProxy::GetDescriptor())) {
        return;
    }

    if (remote->SendRequest(static_cast<int>(IFaultLoggerService::IFaultLoggerService_DESTROY),
        data, reply, option) != ERR_OK) {
        return;
    }
}
} // namespace HiviewDFX
} // namespace OHOS
