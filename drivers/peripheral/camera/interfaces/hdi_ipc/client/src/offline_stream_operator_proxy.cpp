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

#include "offline_stream_operator_proxy.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <message_parcel.h>

namespace OHOS::Camera {
CamRetCode OfflineStreamOperatorProxy::CancelCapture(int captureId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(OfflineStreamOperatorProxy::GetDescriptor()) ||
        !data.WriteInt32(static_cast<int32_t>(captureId))) {
        HDF_LOGE("%s: write captureId obj failed!", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_OFFLINE_STREAM_OPERATOR_CANCEL_CAPTURE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    int32_t result = reply.ReadInt32();
    HDF_LOGE("%s: CameraHostProxy::return  entry! result = %d", __func__, result);

    return static_cast<CamRetCode>(result);
}

CamRetCode OfflineStreamOperatorProxy::ReleaseStreams(const std::vector<int> &streamIds)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    std::vector<int32_t> pxyStreamIds = streamIds;
    if (!data.WriteInterfaceToken(OfflineStreamOperatorProxy::GetDescriptor()) ||
        !data.WriteInt32Vector(pxyStreamIds)) {
        HDF_LOGE("%s: write streamIds obj failed!", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_OFFLINE_STREAM_OPERATOR_RELEASE_STREAMS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    int32_t result = reply.ReadInt32();
    HDF_LOGE("%s: CameraHostProxy::return  entry! result = %d", __func__, result);

    return static_cast<CamRetCode>(result);
}

CamRetCode OfflineStreamOperatorProxy::Release()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(OfflineStreamOperatorProxy::GetDescriptor())) {
        return INVALID_ARGUMENT;
    }
    int32_t ret = Remote()->SendRequest(CMD_OFFLINE_STREAM_OPERATOR_RELEASE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    int32_t result = reply.ReadInt32();
    HDF_LOGE("%s: CameraHostProxy::return  entry! result = %d", __func__, result);

    return static_cast<CamRetCode>(result);
}
}