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

#include "usbd_subscriber.h"
#include "hdf_log.h"
#include "usb_errors.h"

namespace OHOS {
namespace USB {
int32_t UsbdSubscriber::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t ret;
    UsbInfo info;
    PortInfo pinfo;
    HDF_LOGI("%{public}s:%{public}d subscriber event code:%{public}u", __func__, __LINE__, code);
    switch (code) {
        case CMD_NOTIFY_SUBSCRIBER_DEVICE_EVENT:
            if (data.ReadInterfaceToken() != GetDescriptor()) {
                HDF_LOGE("%{public}s:%{public}d token check failed\n", __func__, __LINE__);
                return UEC_HDF_ERR_INVALID_PARAM;
            }
            ret = ParserUsbInfo(data, reply, option, info);
            if (ret == UEC_OK) {
                ret = DeviceEvent(info);
            }
            break;
        case CMD_NOTIFY_PORT_CHANGED:
            if (data.ReadInterfaceToken() != GetDescriptor()) {
                HDF_LOGE("%{public}s:%{public}d token check failed\n", __func__, __LINE__);
                return UEC_HDF_ERR_INVALID_PARAM;
            }
            ret = ParserPortInfo(data, reply, option, pinfo);
            if (ret == UEC_OK) {
                ret = PortChangedEvent(pinfo.portId, pinfo.powerRole, pinfo.dataRole, pinfo.mode);
            }
            break;
        default:
            ret = ERR_INVALID_OPERATION;
            break;
    }
    return ret;
}

int32_t UsbdSubscriber::ParserUsbInfo(MessageParcel &data, MessageParcel &reply, MessageOption &option, UsbInfo &info)
{
    int32_t status;
    int32_t busNum;
    int32_t devNum;

    if (!data.ReadInt32(status)) {
        HDF_LOGE("Read devInfo status error");
        return UEC_HDF_ERR_INVALID_PARAM;
    }
    if (!data.ReadInt32(busNum)) {
        HDF_LOGE("Read devInfo busNum error");
        return UEC_HDF_ERR_INVALID_PARAM;
    }
    if (!data.ReadInt32(devNum)) {
        HDF_LOGE("Read devInfo devNum error");
        return UEC_HDF_ERR_INVALID_PARAM;
    }
    info.setDevInfoStatus(status);
    info.setDevInfoBusNum(busNum);
    info.setDevInfoDevNum(devNum);
    return UEC_OK;
}

int32_t UsbdSubscriber::ParserPortInfo(MessageParcel &data, MessageParcel &reply, MessageOption &option, PortInfo &info)
{
    int32_t portId;
    int32_t powerRole;
    int32_t dataRole;
    int32_t mode;

    if (!data.ReadInt32(portId)) {
        HDF_LOGE("Read PortInfo id error");
        return UEC_HDF_ERR_INVALID_PARAM;
    }
    if (!data.ReadInt32(powerRole)) {
        HDF_LOGE("Read PortInfo powerRole error");
        return UEC_HDF_ERR_INVALID_PARAM;
    }
    if (!data.ReadInt32(dataRole)) {
        HDF_LOGE("Read PortInfo dataRole error");
        return UEC_HDF_ERR_INVALID_PARAM;
    }
    if (!data.ReadInt32(mode)) {
        HDF_LOGE("Read PortInfo mode error");
        return UEC_HDF_ERR_INVALID_PARAM;
    }
    info.portId = portId;
    info.powerRole = powerRole;
    info.dataRole = dataRole;
    info.mode = mode;
    return UEC_OK;
}
} // namespace USB
} // namespace OHOS
