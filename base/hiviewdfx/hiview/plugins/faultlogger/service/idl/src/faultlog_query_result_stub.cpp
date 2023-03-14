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
#include "faultlog_query_result_stub.h"

#include "ipc_types.h"
#include "message_parcel.h"

#include "faultlog_info_ohos.h"
#include "logger.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("FaultLogQueryResultStub");
int FaultLogQueryResultStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    std::u16string descripter = FaultLogQueryResultStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        HIVIEW_LOGE("read descriptor failed.");
        return -1;
    }

    switch (code) {
        case IFaultLogQueryResult_HASNEXT: {
            if (!reply.WriteBool(HasNext())) {
                HIVIEW_LOGE("failed to query HasNext.");
                return -1;
            }
            return 0;
        }
        case IFaultLogQueryResult_GETNEXT: {
            auto result = GetNext();
            if (result == nullptr) {
                HIVIEW_LOGE("failed to GetNext.");
                return -1;
            }

            if (!result->Marshalling(reply)) {
                HIVIEW_LOGE("failed to write query result.");
                return ERR_FLATTEN_OBJECT;
            }

            if (!reply.WriteFileDescriptor(result->fd)) {
                HIVIEW_LOGE("failed to write file descriptor.");
                return ERR_FLATTEN_OBJECT;
            }
            return 0;
        }

        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
} // namespace HiviewDFX
} // namespace OHOS
