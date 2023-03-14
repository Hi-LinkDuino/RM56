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

#include "offline_stream_operator_stub.h"
#include <hdf_log.h>
#include <hdf_base.h>
#include <hdf_sbuf_ipc.h>
#include "cmd_common.h"

namespace OHOS::Camera {
int32_t OfflineStreamOperatorStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    HDF_LOGE("%s: CameraDeviceStub::OnRemoteRequest entry!", __func__);
    (void)option;
    int32_t ret = HDF_SUCCESS;
    switch (code) {
        case CMD_OFFLINE_STREAM_OPERATOR_CANCEL_CAPTURE: {
            if (data.ReadInterfaceToken() != OfflineStreamOperatorStub::GetDescriptor()) {
                ret = HDF_ERR_INVALID_PARAM;
                break;
            }
            ret = OfflineStreamOperatorStubCancelCapture(data, reply, option);
            break;
        }
        case CMD_OFFLINE_STREAM_OPERATOR_RELEASE_STREAMS: {
            if (data.ReadInterfaceToken() != OfflineStreamOperatorStub::GetDescriptor()) {
                ret = HDF_ERR_INVALID_PARAM;
                break;
            }
            ret = OfflineStreamOperatorStubReleaseStreams(data, reply, option);
            break;
        }
        case CMD_OFFLINE_STREAM_OPERATOR_RELEASE: {
            if (data.ReadInterfaceToken() != OfflineStreamOperatorStub::GetDescriptor()) {
                ret = HDF_ERR_INVALID_PARAM;
                break;
            }
            ret = OfflineStreamOperatorStubRelease(data, reply, option);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return ret;
}

int32_t OfflineStreamOperatorStub::OfflineStreamOperatorStubCancelCapture(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t captureId = data.ReadInt32();
    CamRetCode ret = CancelCapture(captureId);
    if (reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t OfflineStreamOperatorStub::OfflineStreamOperatorStubReleaseStreams(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::vector<int32_t> streamIds;
    if (!data.ReadInt32Vector(&streamIds)) {
        HDF_LOGE("%s: read streamIds failed", __func__);
        return HDF_FAILURE;
    }

    CamRetCode ret = ReleaseStreams(streamIds);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t OfflineStreamOperatorStub::OfflineStreamOperatorStubRelease(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    CamRetCode ret = Release();
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}
}
