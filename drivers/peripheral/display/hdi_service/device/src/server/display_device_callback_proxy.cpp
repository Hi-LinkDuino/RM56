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

#include "display_device_callback_proxy.h"

int32_t DisplayDeviceCallbackProxy::OnHotplugIn(uint32_t outputId, bool connected)
{
    DISPLAY_START;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceCallbackProxy::GetDescriptor()) || !data.WriteInt32(outputId) ||
        !data.WriteBool(connected)) {
        return HDF_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_REGHOTPLUGCALLBACK, data, reply, option);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceCallbackProxy::OnVBlankCallback(unsigned int sequence, uint64_t ns)
{
    DISPLAY_START;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceCallbackProxy::GetDescriptor()) || !data.WriteInt32(sequence) ||
        !data.WriteUint64(ns)) {
        return HDF_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_REGDISPLAYVBLANKCALLBACK, data, reply, option);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceCallbackProxy::OnRefreshCallback(uint32_t devId)
{
    DISPLAY_START;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceCallbackProxy::GetDescriptor()) || !data.WriteInt32(devId)) {
        return HDF_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_REGDISPLAYREFRESHCALLBACK, data, reply, option);
    DISPLAY_END;
    return ret;
}
