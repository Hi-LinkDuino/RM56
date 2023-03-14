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
#include "faultlog_query_result_proxy.h"

#include "ipc_types.h"
#include "message_parcel.h"

#include "faultlog_info_ohos.h"

namespace OHOS {
namespace HiviewDFX {
bool FaultLogQueryResultProxy::HasNext()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(FaultLogQueryResultProxy::GetDescriptor())) {
        return false;
    }

    if (remote->SendRequest(static_cast<int>(IFaultLogQueryResult::IFaultLogQueryResult_HASNEXT),
        data, reply, option) != ERR_OK) {
        return false;
    }

    bool hasNext = false;
    if (!reply.ReadBool(hasNext)) {
        return false;
    }
    return hasNext;
}

sptr<FaultLogInfoOhos> FaultLogQueryResultProxy::GetNext()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(FaultLogQueryResultProxy::GetDescriptor())) {
        return nullptr;
    }

    if (remote->SendRequest(static_cast<int>(IFaultLogQueryResult::IFaultLogQueryResult_GETNEXT),
        data, reply, option) != ERR_OK) {
        return nullptr;
    }

    sptr<FaultLogInfoOhos> ret = FaultLogInfoOhos::Unmarshalling(reply);
    if (ret != nullptr && reply.ContainFileDescriptors()) {
        ret->fd = reply.ReadFileDescriptor();
    }
    return ret;
}
} // namespace HiviewDFX
} // namespace OHOS
