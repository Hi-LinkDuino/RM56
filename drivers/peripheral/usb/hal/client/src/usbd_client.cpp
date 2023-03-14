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

#include "usbd_client.h"
#include "hdf_log.h"
#include "iservmgr_hdi.h"
#include "message_parcel.h"
#include "usb_errors.h"
#include "usbd_type.h"

#define HDF_LOG_TAG UsbdClient

namespace OHOS {
namespace USB {
using OHOS::HDI::ServiceManager::V1_0::IServiceManager;

namespace {
const std::string USBD_SERVICE = "usbd";

#define WRITE_PARCEL_WITH_RET(parcel, type, data, retval)            \
    do {                                                             \
        if (!(parcel).Write##type(data)) {                           \
            HDF_LOGE("%{public}s write " #data " failed", __func__); \
            return (retval);                                         \
        }                                                            \
    } while (0)

#define READ_PARCEL_WITH_RET(parcel, type, out, retval)            \
    do {                                                           \
        if (!(parcel).Read##type(out)) {                           \
            HDF_LOGE("%{public}s read " #out " failed", __func__); \
            return (retval);                                       \
        }                                                          \
    } while (0)
} // namespace

UsbdClient &UsbdClient::GetInstance()
{
    static UsbdClient instance;
    return instance;
}

sptr<IRemoteObject> UsbdClient::GetUsbdService()
{
    auto serviceManager = IServiceManager::Get();
    if (serviceManager == nullptr) {
        HDF_LOGE("service manager is nullptr");
        return nullptr;
    }
    auto usbdService = serviceManager->GetService(USBD_SERVICE.c_str());
    if (usbdService == nullptr) {
        HDF_LOGE("Usbd service is nullptr");
        return nullptr;
    }
    return usbdService;
}

int32_t UsbdClient::BindUsbdSubscriber(const sptr<UsbdSubscriber> &subscriber)
{
    if (subscriber == nullptr) {
        HDF_LOGE("subscriber is nullptr");
        return UEC_SERVICE_INVALID_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    data.WriteRemoteObject(subscriber);
    return DoDispatch(CMD_BIND_USB_SUBSCRIBER, data, reply);
}

int32_t UsbdClient::UnbindUsbdSubscriber(const sptr<UsbdSubscriber> &subscriber)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    data.WriteRemoteObject(subscriber);
    return DoDispatch(CMD_UNBIND_USB_SUBSCRIBER, data, reply);
}

int32_t UsbdClient::OpenDevice(const UsbDev &dev)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    int32_t ret = DoDispatch(CMD_FUN_OPEN_DEVICE, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("UsbdClient::%{public}s:%{public}d OpenDevice failed ret:%{public}d bus:%{public}d dev:%{public}d",
                 __func__, __LINE__, ret, dev.busNum, dev.devAddr);
    }
    return ret;
}

int32_t UsbdClient::GetCurrentFunctions(int32_t &funcs)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    int32_t ret = DoDispatch(CMD_FUN_GET_CURRENT_FUNCTIONS, data, reply);
    if (FAILED(ret)) {
        HDF_LOGE("CMD_FUN_GET_CURRENT_FUNCTIONS failed, return INVALID_USB_INT_VALUE");
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, funcs, UEC_SERVICE_READ_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbdClient::SetCurrentFunctions(int32_t funcs)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    WRITE_PARCEL_WITH_RET(data, Int32, funcs, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = DoDispatch(CMD_FUN_SET_CURRENT_FUNCTIONS, data, reply);
    if (FAILED(ret)) {
        HDF_LOGE("CMD_FUN_SET_CURRENT_FUNCTIONS failed");
        return ret;
    }
    return UEC_OK;
}

int32_t UsbdClient::SetPortRole(int32_t portId, int32_t powerRole, int32_t dataRole)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    WRITE_PARCEL_WITH_RET(data, Int32, portId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, powerRole, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, dataRole, UEC_SERVICE_WRITE_PARCEL_ERROR);

    int32_t ret = DoDispatch(CMD_SET_ROLE, data, reply);
    if (FAILED(ret)) {
        HDF_LOGE("CMD_SET_ROLE failed, return INVALID_STRING_VALUE");
        return ret;
    }
    return UEC_OK;
}

int32_t UsbdClient::QueryPort(int32_t &portId, int32_t &powerRole, int32_t &dataRole, int32_t &mode)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    int32_t ret = DoDispatch(CMD_QUERY_PORT, data, reply);
    READ_PARCEL_WITH_RET(reply, Int32, portId, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(reply, Int32, powerRole, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(reply, Int32, dataRole, UEC_SERVICE_READ_PARCEL_ERROR);
    READ_PARCEL_WITH_RET(reply, Int32, mode, UEC_SERVICE_READ_PARCEL_ERROR);
    if (FAILED(ret)) {
        HDF_LOGE("CMD_QUERY_PORT failed, return INVALID_STRING_VALUE");
        return ret;
    }
    return UEC_OK;
}

int32_t UsbdClient::DoDispatch(uint32_t cmd, MessageParcel &data, MessageParcel &reply)
{
    auto usbd = GetUsbdService();
    if (usbd == nullptr) {
        HDF_LOGE(" get usbd service failed.");
        return UEC_SERVICE_NO_INIT;
    }

    MessageOption option;
    auto ret = usbd->SendRequest(cmd, data, reply, option);
    if (ret != UEC_OK) {
        HDF_LOGE("failed to send request, cmd: %{public}u, ret: %{public}d", cmd, ret);
        return ret;
    }
    return UEC_OK;
}

int32_t UsbdClient::GetRawDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_GET_DESCRIPTOR, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d DoDispatch failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    ret = UsbdClient::GetBufferMessage(reply, decriptor);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d GetBufferMessage failed:%{public}d", __func__, __LINE__, ret);
    }
    return ret;
}

int32_t UsbdClient::GetFileDescriptor(const UsbDev &dev, int32_t &fd)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_GET_FILEDESCRIPTOR, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    READ_PARCEL_WITH_RET(reply, Int32, fd, UEC_SERVICE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbdClient::GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_GET_DEVICE_DESCRIPTOR, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s failed", __func__);
        return ret;
    }
    ret = UsbdClient::GetBufferMessage(reply, decriptor);
    return ret;
}

int32_t UsbdClient::GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, descId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_GET_STRING_DESCRIPTOR, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d strId:%{public}hhu failed:%{public}d", __func__, __LINE__, descId, ret);
        return ret;
    }
    ret = UsbdClient::GetBufferMessage(reply, decriptor);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d strId:%{public}hhu failed:%{public}d", __func__, __LINE__, descId, ret);
    }
    return ret;
}

int32_t UsbdClient::GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, descId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_GET_CONFIG_DESCRIPTOR, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d cfgId:%{public}d failed:%{public}d", __func__, __LINE__, descId, ret);
        return ret;
    }
    ret = UsbdClient::GetBufferMessage(reply, decriptor);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d strId:%{public}d failed:%{public}d", __func__, __LINE__, descId, ret);
    }
    return ret;
}

int32_t UsbdClient::SetConfig(const UsbDev &dev, uint8_t configIndex)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, configIndex, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_SET_CONFIG, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s failed", __func__);
    }
    return ret;
}

int32_t UsbdClient::GetConfig(const UsbDev &dev, uint8_t &configIndex)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_GET_CONFIG, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s failed", __func__);
        return ret;
    }
    READ_PARCEL_WITH_RET(data, Uint8, configIndex, UEC_SERVICE_READ_PARCEL_ERROR);
    return ret;
}

int32_t UsbdClient::ClaimInterface(const UsbDev &dev, uint8_t interfaceIndex, uint8_t force)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, interfaceIndex, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, force, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_CLAIM_INTERFACE, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s failed", __func__);
    }
    return ret;
}

int32_t UsbdClient::ReleaseInterface(const UsbDev &dev, uint8_t interfaceIndex)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, interfaceIndex, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_RELEASE_INTERFACE, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s failed", __func__);
    }
    return ret;
}

int32_t UsbdClient::SetInterface(const UsbDev &dev, uint8_t interfaceIndex, uint8_t altIndex)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, interfaceIndex, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, altIndex, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_SET_INTERFACE, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s failed", __func__);
    }
    return ret;
}

int32_t UsbdClient::BulkTransferRead(const UsbDev &devInfo, const UsbPipe &pipe, int32_t timeout,
    std::vector<uint8_t> &bufferData)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    SetDeviceMessage(data, devInfo);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, timeout, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_SEND_BULK_READ_SYNC, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed ret:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    ret = UsbdClient::GetBufferMessage(reply, bufferData);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
    }
    return ret;
}

int32_t UsbdClient::BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
    const std::vector<uint8_t> &bufferData)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, timeout, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = SetBufferMessage(data, bufferData);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    ret = UsbdClient::DoDispatch(CMD_FUN_SEND_BULK_WRITE_SYNC, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s failed", __func__);
    }
    return ret;
}

int32_t UsbdClient::ControlTransfer(const UsbDev &dev, const UsbCtrlTransfer &ctrl, std::vector<uint8_t> &bufferData)
{
    int32_t ret;
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrl.requestType, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrl.requestCmd, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrl.value, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrl.index, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, ctrl.timeout, UEC_SERVICE_WRITE_PARCEL_ERROR);
    bool isWrite = (ctrl.requestType & USB_ENDPOINT_DIR_MASK) == USB_ENDPOINT_DIR_OUT;
    if (isWrite) {
        ret = SetBufferMessage(data, bufferData);
        if (ret != UEC_OK) {
            HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
            return ret;
        }
    }
    ret = UsbdClient::DoDispatch(CMD_FUN_SEND_CTRL_REQUEST_SYNC, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    if (!isWrite) {
        ret = GetBufferMessage(reply, bufferData);
        if (ret != UEC_OK) {
            HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        }
    }
    return ret;
}

int32_t UsbdClient::InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
    std::vector<uint8_t> &bufferData)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, timeout, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_SEND_INTERRUPT_READ_SYNC, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed ret:%{public}d", __func__, __LINE__, ret);
        return ret;
    }

    ret = GetBufferMessage(reply, bufferData);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
    }
    return ret;
}

int32_t UsbdClient::InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
    std::vector<uint8_t> &bufferData)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, timeout, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = SetBufferMessage(data, bufferData);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    ret = UsbdClient::DoDispatch(CMD_FUN_SEND_INTERRUPT_WRITE_SYNC, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed ret:%{public}d", __func__, __LINE__, ret);
    }
    return ret;
}

int32_t UsbdClient::IsoTransferRead(const UsbDev &devInfo, const UsbPipe &pipe, int32_t timeout,
    std::vector<uint8_t> &bufferData)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    SetDeviceMessage(data, devInfo);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, timeout, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_SEND_ISO_READ_SYNC, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed ret:%{public}d", __func__, __LINE__, ret);
        return ret;
    }

    ret = GetBufferMessage(reply, bufferData);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
    }
    return ret;
}

int32_t UsbdClient::IsoTransferWrite(const UsbDev &devInfo, const UsbPipe &pipe, int32_t timeout,
    std::vector<uint8_t> &bufferData)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    SetDeviceMessage(data, devInfo);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Int32, timeout, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = SetBufferMessage(data, bufferData);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    ret = UsbdClient::DoDispatch(CMD_FUN_SEND_ISO_WRITE_SYNC, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed ret:%{public}d", __func__, __LINE__, ret);
    }
    return ret;
}

int32_t UsbdClient::CloseDevice(const UsbDev &dev)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_CLOSE_DEVICE, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d CloseDevice failed ret:%{public}d bus:%{public}d dev:%{public}d", __func__,
                 __LINE__, ret, dev.busNum, dev.devAddr);
    }
    return ret;
}

int32_t UsbdClient::RequestQueue(const UsbDev &dev, const UsbPipe &pipe, const std::vector<uint8_t> &clientData,
    const std::vector<uint8_t> &buffer)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);

    int32_t ret = UsbdClient::SetBufferMessage(data, clientData);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    ret = UsbdClient::SetBufferMessage(data, buffer);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }

    ret = UsbdClient::DoDispatch(CMD_FUN_REQUEST_QUEUE, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed ret:%{public}d", __func__, __LINE__, ret);
    }
    return ret;
}

int32_t UsbdClient::RequestWait(const UsbDev &dev, std::vector<uint8_t> &clientData, std::vector<uint8_t> &buffer,
    int32_t timeout)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Int32, timeout, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_REQUEST_WAIT, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed ret:%{public}d", __func__, __LINE__, ret);
        return ret;
    }

    ret = UsbdClient::GetBufferMessage(reply, clientData);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }

    ret = UsbdClient::GetBufferMessage(reply, buffer);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
    }

    return ret;
}

int32_t UsbdClient::RequestCancel(const UsbDev &dev, const UsbPipe &pipe)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_REQUEST_CANCEL, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
    }
    return ret;
}

int32_t UsbdClient::SetBufferMessage(MessageParcel &data, const std::vector<uint8_t> &bufferData)
{
    uint32_t length = bufferData.size();
    const uint8_t *ptr = bufferData.data();
    if (ptr == nullptr) {
        length = 0;
    }

    if (!data.WriteUint32(length)) {
        HDF_LOGE("%{public}s:%{public}d failed length:%{public}d", __func__, __LINE__, length);
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    if ((ptr != nullptr) && (length > 0) && !data.WriteBuffer(ptr, length)) {
        HDF_LOGE("%{public}s:%{public}d failed length:%{public}d", __func__, __LINE__, length);
        return UEC_SERVICE_WRITE_PARCEL_ERROR;
    }
    return UEC_OK;
}

int32_t UsbdClient::GetBufferMessage(MessageParcel &data, std::vector<uint8_t> &bufferData)
{
    uint32_t dataSize = 0;
    bufferData.clear();
    if (!data.ReadUint32(dataSize)) {
        HDF_LOGE("%{public}s:%{public}d failed", __func__, __LINE__);
        return UEC_SERVICE_READ_PARCEL_ERROR;
    }
    if (dataSize == 0) {
        HDF_LOGE("%{public}s:%{public}d size:%{public}d", __func__, __LINE__, dataSize);
        return UEC_OK;
    }

    const uint8_t *readData = data.ReadUnpadBuffer(dataSize);
    if (readData == nullptr) {
        HDF_LOGE("%{public}s:%{public}d failed size:%{public}d", __func__, __LINE__, dataSize);
        return UEC_SERVICE_READ_PARCEL_ERROR;
    }
    std::vector<uint8_t> tData(readData, readData + dataSize);
    bufferData.swap(tData);

    return UEC_OK;
}

int32_t UsbdClient::SetDeviceMessage(MessageParcel &data, const UsbDev &dev)
{
    WRITE_PARCEL_WITH_RET(data, Uint8, dev.busNum, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, dev.devAddr, UEC_SERVICE_WRITE_PARCEL_ERROR);
    return UEC_OK;
}

int32_t UsbdClient::RegBulkCallback(const UsbDev &dev, const UsbPipe &pipe, const sptr<IRemoteObject> &cb)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, RemoteObject, cb, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_REG_BULK_CALLBACK, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
    }
    return ret;
}

int32_t UsbdClient::UnRegBulkCallback(const UsbDev &dev, const UsbPipe &pipe)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_UNREG_BULK_CALLBACK, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    return ret;
}

int32_t UsbdClient::BulkRead(const UsbDev &dev, const UsbPipe &pipe, sptr<Ashmem> &ashmem)
{
    MessageParcel data;
    MessageParcel reply;
    if (ashmem == nullptr) {
        HDF_LOGE("%{public}s:%{public}d BulkRead error ashmem", __func__, __LINE__);
        return UEC_HDF_ERR_INVALID_PARAM;
    }

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Ashmem, ashmem, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_SEND_BULK_READ_ASYNC, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    return ret;
}

int32_t UsbdClient::BulkWrite(const UsbDev &dev, const UsbPipe &pipe, sptr<Ashmem> &ashmem)
{
    MessageParcel data;
    MessageParcel reply;
    if (ashmem == nullptr) {
        HDF_LOGE("%{public}s:%{public}d BulkWrite error ashmem", __func__, __LINE__);
        return UEC_HDF_ERR_INVALID_PARAM;
    }

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Ashmem, ashmem, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_SEND_BULK_WRITE_ASYNC, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    return ret;
}

int32_t UsbdClient::BulkCancel(const UsbDev &dev, const UsbPipe &pipe)
{
    MessageParcel data;
    MessageParcel reply;

    if (data.WriteInterfaceToken(GetDescriptor()) == false) {
        HDF_LOGE(" WriteInterfaceToken failed.");
        return UEC_HDF_FAILURE;
    }

    UsbdClient::SetDeviceMessage(data, dev);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.interfaceId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    WRITE_PARCEL_WITH_RET(data, Uint8, pipe.endpointId, UEC_SERVICE_WRITE_PARCEL_ERROR);
    int32_t ret = UsbdClient::DoDispatch(CMD_FUN_BULK_CANCEL, data, reply);
    if (ret != UEC_OK) {
        HDF_LOGE("%{public}s:%{public}d failed:%{public}d", __func__, __LINE__, ret);
        return ret;
    }
    return ret;
}
} // namespace USB
} // namespace OHOS
