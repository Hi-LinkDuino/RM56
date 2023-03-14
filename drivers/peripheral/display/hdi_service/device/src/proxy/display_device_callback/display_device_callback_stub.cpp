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

#include "display_device_callback_stub.h"

int32_t DisplayDeviceCallbackStub::OnRemoteRequest(
    uint32_t code, OHOS::MessageParcel &data, OHOS::MessageParcel &reply, OHOS::MessageOption &option)
{
    DISPLAY_START;
    DISPLAY_LOG("OnRemoteRequest receive code = %{public}u", code);
    switch (code) {
        case DSP_CMD_REGHOTPLUGCALLBACK: {
            if (data.ReadInterfaceToken() != DisplayDeviceCallbackStub::GetDescriptor()) {
                return HDF_ERR_INVALID_PARAM;
            }
            uint32_t outputId = data.ReadInt32();
            bool connected = data.ReadBool();
            DISPLAY_LOG("call OnHotplugIn id : %{public}d; connected : %{public}d", outputId, connected);
            OnHotplugIn(outputId, connected);
            break;
        }
        case DSP_CMD_REGDISPLAYVBLANKCALLBACK: {
            if (data.ReadInterfaceToken() != DisplayDeviceCallbackStub::GetDescriptor()) {
                return HDF_ERR_INVALID_PARAM;
            }
            unsigned int sequence = data.ReadUint32();
            uint64_t ns = data.ReadUint64();
            DISPLAY_LOG(" call VBlankCallback id : %{public}u connected", sequence);
            OnVBlankCallback(sequence, ns);
            break;
        }
        case DSP_CMD_REGDISPLAYREFRESHCALLBACK: {
            if (data.ReadInterfaceToken() != DisplayDeviceCallbackStub::GetDescriptor()) {
                return HDF_ERR_INVALID_PARAM;
            }
            uint32_t devId = data.ReadUint32();
            DISPLAY_LOG(" call VBlankCallback devId : %{public}d", devId);
            OnRefreshCallback(devId);
            break;
        }
        default:
            break;
    }
    DISPLAY_END;
    return 0;
}
