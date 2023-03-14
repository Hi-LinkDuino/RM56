/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "connected_tag_server_stub.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdf_sbuf_ipc.h>
#include "cmd_def.h"

namespace OHOS {
namespace HDI {
namespace NFC {
namespace V1_0 {
int32_t ConnectedTagServerStub::StubOnRemoteRequest(int cmdId,
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    HDF_LOGE("StubOnRemoteRequest cmdId = %{public}d", cmdId);
    switch (cmdId) {
        case CMD_INIT:
            return StubInit(data, reply, option);
        case CMD_UNINIT:
            return StubUninit(data, reply, option);
        case CMD_READ_NDEF:
            return StubReadNdef(data, reply, option);
        case CMD_WRITE_NDEF:
            return StubWriteNdef(data, reply, option);
        default: {
            HDF_LOGE("%{public}s: not support cmd %{public}d", __func__, cmdId);
            return HDF_ERR_INVALID_PARAM;
        }
    }
}

int32_t ConnectedTagServerStub::StubInit(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return Init();
}

int32_t ConnectedTagServerStub::StubUninit(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return Uninit();
}
int32_t ConnectedTagServerStub::StubReadNdef(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    std::string respNdef = ReadNdefTag();
    reply.WriteInt32(HDF_SUCCESS);
    reply.WriteString(respNdef);
    return HDF_SUCCESS;
}

int32_t ConnectedTagServerStub::StubWriteNdef(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        HDF_LOGE("%{public}s: interface token check failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t status = data.ReadInt32();
    if (status != HDF_SUCCESS) {
        return status;
    }
    std::string ndefData = data.ReadString();
    HDF_LOGE("StubWriteNdef status = %{public}d, len = %{public}d, data = %{public}s",
        status, ndefData.length(), ndefData.c_str());
    int32_t ret = WriteNdefTag(ndefData);
    reply.WriteInt32(HDF_SUCCESS);
    reply.WriteInt32(ret);
    return ret;
}
int32_t ConnectedTagServerStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    return HDF_SUCCESS;
}
int32_t ConnectedTagServerStub::Init()
{
    return service_.Init();
}

int32_t ConnectedTagServerStub::Uninit()
{
    return service_.Uninit();
}
std::string ConnectedTagServerStub::ReadNdefTag()
{
    return service_.ReadNdefTag();
}
int32_t ConnectedTagServerStub::WriteNdefTag(std::string ndefData)
{
    return service_.WriteNdefTag(ndefData);
}
}  // namespace V1_0
}  // namespace NFC
}  // namespace HDI
}  // namespace OHOS

void *ConnectedTagServerStubInstance()
{
    using namespace OHOS::HDI::NFC::V1_0;
    return reinterpret_cast<void *>(new ConnectedTagServerStub());
}

void ConnectedTagServerStubRelease(void *obj)
{
    using namespace OHOS::HDI::NFC::V1_0;
    delete reinterpret_cast<ConnectedTagServerStub *>(obj);
}

int32_t DispatchStubOnRemoteRequest(void *stub, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    using namespace OHOS::HDI::NFC::V1_0;
    ConnectedTagServerStub *nfcHdiStub = reinterpret_cast<ConnectedTagServerStub *>(stub);
    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;

    (void)SbufToParcel(reply, &replyParcel);
    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%s:invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (dataParcel == nullptr || replyParcel == nullptr) {
        HDF_LOGE("%s:HDF_ERR_INVALID_PARAM to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::MessageOption option;
    return nfcHdiStub->StubOnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}
