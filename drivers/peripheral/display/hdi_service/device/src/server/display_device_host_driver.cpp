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

#include <hdf_remote_service.h>
#include <hdf_sbuf_ipc.h>

#include "display_device_common.h"
#include "display_device_service.h"
#include "display_device_stub.h"

#undef HDF_LOG_TAG
#define HDF_LOG_TAG DisplayHostDriver

#define DDSS &DisplayDeviceServerStub

using OHOS::MessageParcel;
using OHOS::Display::Device::Server::DisplayDeviceServerStub;
using OHOS::Display::Device::Server::DisplayDeviceServerStubFunc;
using OHOS::Display::Device::Server::DisplayDeviceService;

struct HDF_CPS_SRV {
    struct IDeviceIoService ioservice;
    std::unique_ptr<DisplayDeviceServerStub> serviceStub;
};

static const DisplayDeviceServerStubFunc g_displayDeviceServerFuncTbl[HDF_DISPLAY_DRIVER_FUNC_TYPE_MAX]
                                                                     [HDF_DISPLAY_DRIVER_FUNC_NUM_MAX]
    = {
          /* reserved */
          {},
          /* DEVICE */
          { nullptr, nullptr, nullptr, DDSS::RegHotPlugCallback, DDSS::GetDisplayCapability,
              DDSS::GetDisplaySupportedModes, DDSS::GetDisplayMode, DDSS::SetDisplayMode, DDSS::GetDisplayPowerStatus,
              DDSS::SetDisplayPowerStatus, DDSS::GetDisplayBackLight, DDSS::SetDisplayBackLight,
              DDSS::GetDisplayProperty, DDSS::SetDisplayProperty, DDSS::PrepareDisplayLayers, nullptr,
              DDSS::GetDisplayCompChange, nullptr, DDSS::SetDisplayClientCrop, DDSS::SetDisplayClientDestRect,
              DDSS::SetDisplayClientBuffer, DDSS::SetDisplayClientDamage, DDSS::SetDisplayVsyncEnabled, nullptr,
              nullptr, DDSS::RegDisplayVBlankCallback, nullptr, DDSS::GetDisplayReleaseFence, DDSS::Commit,
              DDSS::InvokeDisplayCmd, DDSS::CreateVirtualDisplay, DDSS::DestroyVirtualDisplay,
              DDSS::SetVirtualDisplayBuffer, DDSS::RegDisplayRefreshCallback, DDSS::GetWriteBackFrame,
              DDSS::CreateWriteBack, DDSS::DestroyWriteBack, DDSS::SetProxyRemoteCallback, DDSS::FileTest },
          /* LAYER */
          { nullptr, nullptr, nullptr, DDSS::CreateLayer, nullptr, DDSS::SetLayerVisible, DDSS::GetLayerVisibleState,
              nullptr, nullptr, DDSS::SetLayerCrop, DDSS::SetLayerZorder, DDSS::GetLayerZorder, DDSS::SetLayerPreMulti,
              DDSS::GetLayerPreMulti, DDSS::SetLayerAlpha, DDSS::GetLayerAlpha, DDSS::SetLayerColorKey,
              DDSS::GetLayerColorKey, DDSS::SetLayerPalette, DDSS::GetLayerPalette, nullptr, DDSS::SetLayerCompression,
              DDSS::GetLayerCompression, nullptr, DDSS::Flush, DDSS::SetLayerVisibleRegion, DDSS::SetLayerDirtyRegion,
              DDSS::GetLayerBuffer, DDSS::SetLayerBuffer, DDSS::InvokeLayerCmd, DDSS::SetLayerCompositionType, nullptr,
              DDSS::InitDisplay, DDSS::DeinitDisplay, DDSS::GetDisplayInfo, DDSS::CloseLayer, DDSS::SetLayerSize,
              DDSS::GetLayerSize, DDSS::SetTransformMode, DDSS::WaitForVBlank, DDSS::SnapShot, DDSS::SetLayerBlendType,
              DDSS::FileTest },
      };

static int32_t DisplayDeviceServiceDispatch(
    struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    DISPLAY_LOG("receive cmdId = 0x%{public}0x", cmdId);
    do {
        if (client == nullptr || data == nullptr || reply == nullptr) {
            break;
        }

        HDF_CPS_SRV *displayDeviceSrv = CONTAINER_OF(client->device->service, HDF_CPS_SRV, ioservice);
        if (displayDeviceSrv == nullptr) {
            DISPLAY_LOG("error: invalid data sbuf object to dispatch");
            break;
        }

        if (displayDeviceSrv->serviceStub == nullptr) {
            displayDeviceSrv->serviceStub = std::make_unique<DisplayDeviceServerStub>();
        }

        MessageParcel *dataParcel = nullptr;
        MessageParcel *replyParcel = nullptr;
        if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS || SbufToParcel(reply, &replyParcel) != HDF_SUCCESS) {
            DISPLAY_LOG("error: invalid data or reply sbuf object to dispatch");
            break;
        }

        return displayDeviceSrv->serviceStub->OnRemoteRequest(cmdId, dataParcel, replyParcel);
    } while (0);

    DISPLAY_LOG("error: dispatch no matched");
    return HDF_ERR_INVALID_PARAM;
}

void DisplayDeviceDriverRelease(struct HdfDeviceObject *deviceObject)
{
    DISPLAY_START;
    (void)deviceObject;
    DISPLAY_END;
}

int DisplayDeviceDriverBind(struct HdfDeviceObject *deviceObject)
{
    DISPLAY_START;
    static HDF_CPS_SRV displayDeviceSrv = {
        .ioservice = {
            .Open = nullptr,
            .Dispatch = DisplayDeviceServiceDispatch,
            .Release = nullptr,
        },
        .serviceStub = nullptr,
    };

    deviceObject->service = &displayDeviceSrv.ioservice;
    DISPLAY_END;
    return HDF_SUCCESS;
}

int DisplayDeviceDriverInit(struct HdfDeviceObject *deviceObject)
{
    DISPLAY_START;
    DISPLAY_END;
    return HDF_SUCCESS;
}

static struct HdfDriverEntry g_DisplayDeviceDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "display_layer_driver",
    .Bind = DisplayDeviceDriverBind,
    .Init = DisplayDeviceDriverInit,
    .Release = DisplayDeviceDriverRelease,
};

extern "C" {
HDF_INIT(g_DisplayDeviceDriverEntry);
}

DisplayDeviceServerStub::DisplayDeviceServerStub()
{
    DISPLAY_START;
    device_ = std::make_unique<DisplayDeviceService>();
    DISPLAY_END;
}

int32_t DisplayDeviceServerStub::OnRemoteRequest(int cmdId, MessageParcel *data, MessageParcel *reply)
{
    if (device_ == nullptr || !device_->IsValid()) {
        DISPLAY_LOG("display device service is not valid");
        return HDF_DEV_ERR_NO_DEVICE_SERVICE;
    }
    DISPLAY_LOG("received cmdId = %{public}0x", cmdId);
    uint32_t functionType = (cmdId >> 16) & 0xF;
    uint32_t functionNum = cmdId & 0xFF;
    if (functionType >= HDF_DISPLAY_DRIVER_FUNC_TYPE_MAX || functionNum >= HDF_DISPLAY_DRIVER_FUNC_NUM_MAX) {
        DISPLAY_LOG("error : unknown cmdId 0x%{public}0x", cmdId);
        return DISPLAY_NOT_SUPPORT;
    }
    DisplayDeviceServerStubFunc functionPtr = g_displayDeviceServerFuncTbl[functionType][functionNum];
    if (functionPtr == nullptr) {
        DISPLAY_LOG("error : cmdId is undefined or deleted");
        return DISPLAY_NOT_SUPPORT;
    }
    int32_t ret = (this->*functionPtr)(data, reply);

    DISPLAY_LOG("server receive cmd number: %{public}d, ret = %{public}d!", cmdId, ret);
    return ret;
}

static void HotPlugCallbackFunc(uint32_t devId, bool connected, void *data)
{
    HDF_LOGI("hotplug callback %{public}d %{public}d", devId, connected);
    auto callbackRemote = reinterpret_cast<DisplayDeviceCallbackProxy *>(data);
    auto ret = callbackRemote->OnHotplugIn(devId, connected);
    if (ret != 0) {
        HDF_LOGE("failed to hotplug callback %{public}d %{public}d", devId, connected);
    } else {
        HDF_LOGD("succ to hotplug callback %{public}d %{public}d", devId, connected);
    }
}

int32_t DisplayDeviceServerStub::RegHotPlugCallback(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (callbackRemote_ == nullptr) {
        DISPLAY_LOG("callback remote object is invalid");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = device_->RegHotPlugCallback(HotPlugCallbackFunc, callbackRemote_.GetRefPtr());
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return DISPLAY_SUCCESS;
}

int32_t DisplayDeviceServerStub::GetDisplayCapability(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DisplayCapability capability = {};
    int32_t ret = device_->GetDisplayCapability(devId, capability);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!DisplayDeviceWriteData(reply, &capability)) {
        DISPLAY_LOG("error: server write ret into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return DISPLAY_SUCCESS;
}

int32_t DisplayDeviceServerStub::GetDisplaySupportedModes(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    uint32_t modesNum = 1;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    if (!data->ReadUint32(modesNum) || modesNum > COMPOSER_SERVER_ARRAY_NUMBER_MAX) {
        DISPLAY_LOG("modesNum invlid");
        return DISPLAY_FAILURE;
    }
    DisplayModeInfo modesTmp[COMPOSER_SERVER_ARRAY_NUMBER_MAX];
    (void)memset_s(&modesTmp, sizeof(modesTmp), 0, sizeof(modesTmp));

    int32_t ret = device_->GetDisplaySupportedModes(devId, modesNum, modesTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!reply->WriteInt32(modesNum)) {
        HDF_LOGD("write modesNum into reply failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(reply, modesTmp, modesNum)) {
        DISPLAY_LOG("error: server write DisplayModeInfo array into reply failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return DISPLAY_SUCCESS;
}

int32_t DisplayDeviceServerStub::GetDisplayMode(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t mode = 0;
    int32_t ret = device_->GetDisplayMode(devId, mode);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (!reply->WriteUint32(mode)) {
        HDF_LOGD("write data into reply failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetDisplayMode(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t mode = 0;
    if (!data->ReadUint32(mode)) {
        DISPLAY_LOG("read mode from data failed!");
        return DISPLAY_FAILURE;
    }

    int32_t ret = device_->SetDisplayMode(devId, mode);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetDisplayPowerStatus(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DispPowerStatus statusTmp = POWER_STATUS_BUTT;

    int32_t ret = device_->GetDisplayPowerStatus(devId, statusTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!reply->WriteInt32(statusTmp)) {
        DISPLAY_LOG("error: server write status into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetDisplayPowerStatus(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    uint32_t devId = 0;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t status = 0;
    if (!data->ReadInt32(status)) {
        DISPLAY_LOG("read enum from data failed!");
        return DISPLAY_FAILURE;
    }
    DispPowerStatus statusTmp = Convert2PowerStatus(status);

    int32_t ret = device_->SetDisplayPowerStatus(devId, statusTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetDisplayBackLight(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }

    uint32_t backlightLevel = 0;
    int32_t ret = device_->GetDisplayBacklight(devId, backlightLevel);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!reply->WriteUint32(backlightLevel)) {
        DISPLAY_LOG("error: server write value into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetDisplayBackLight(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t valueTmp = 0;
    if (!data->ReadUint32(valueTmp)) {
        DISPLAY_LOG("read value from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetDisplayBacklight(devId, valueTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetDisplayProperty(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t propertyId = 0;
    if (!data->ReadUint32(propertyId)) {
        DISPLAY_LOG("read propertyId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint64_t valueTmp = 0;
    int32_t ret = device_->GetDisplayProperty(devId, propertyId, valueTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!reply->WriteUint64(valueTmp)) {
        DISPLAY_LOG("error: server write value into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}
int32_t DisplayDeviceServerStub::SetDisplayProperty(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t propertyIdTmp = 0;
    if (!data->ReadUint32(propertyIdTmp)) {
        DISPLAY_LOG("read propertyId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint64_t valueTmp = 0;
    if (!data->ReadUint64(valueTmp)) {
        DISPLAY_LOG("read value from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetDisplayProperty(devId, propertyIdTmp, valueTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::PrepareDisplayLayers(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    bool needFlushFb = false;

    int32_t ret = device_->PrepareDisplayLayers(devId, needFlushFb);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!reply->WriteBool(needFlushFb)) {
        DISPLAY_LOG("error: server write needFlushFb into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetDisplayCompChange(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t numTmp = 0;
    uint32_t layersTmp[COMPOSER_SERVER_ARRAY_NUMBER_MAX];
    int32_t typeTmp[COMPOSER_SERVER_ARRAY_NUMBER_MAX];
    (void)memset_s(layersTmp, sizeof(layersTmp), 0, sizeof(layersTmp));
    (void)memset_s(typeTmp, sizeof(typeTmp), 0, sizeof(typeTmp));

    int32_t ret = device_->GetDisplayCompChange(devId, numTmp, layersTmp, typeTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!reply->WriteUint32(numTmp)) {
        DISPLAY_LOG("error: server write num into data failed");
        return DISPLAY_FAILURE;
    }
    if (numTmp != 0) {
        if (!DisplayDeviceWriteData(reply, &layersTmp[0], numTmp)) {
            DISPLAY_LOG("error: server write layers array into data failed");
            return DISPLAY_FAILURE;
        }
        if (!DisplayDeviceWriteData(reply, &typeTmp[0], numTmp)) {
            DISPLAY_LOG("error: server write type array into data failed");
            return DISPLAY_FAILURE;
        }
    }
    DISPLAY_END;
    return DISPLAY_SUCCESS;
}

int32_t DisplayDeviceServerStub::SetDisplayClientCrop(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    IRect rectTmp;
    memset_s(&rectTmp, sizeof(rectTmp), 0, sizeof(rectTmp));
    if (!DisplayDeviceReadData(&rectTmp, data)) {
        DISPLAY_LOG("read rect from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("receive devId = %{public}u, x = %{public}d, y = %{public}d, w = %{public}d, h = %{public}d", devId,
        rectTmp.x, rectTmp.y, rectTmp.w, rectTmp.h);
    int32_t ret = device_->SetDisplayClientCrop(devId, &rectTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return DISPLAY_SUCCESS;
}
int32_t DisplayDeviceServerStub::SetDisplayClientDestRect(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    IRect rectTmp = {};
    if (!DisplayDeviceReadData(&rectTmp, data)) {
        DISPLAY_LOG("read rect from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("receive devId = %{public}u, x = %{public}d, y = %{public}d, w = %{public}d, h = %{public}d", devId,
        rectTmp.x, rectTmp.y, rectTmp.w, rectTmp.h);

    int32_t ret = device_->SetDisplayClientDestRect(devId, rectTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetDisplayClientBuffer(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);

    BufferHandle *bufhandle = nullptr;
    if (!DisplayDeviceReadBufHdl(bufhandle, data)) {
        DISPLAY_LOG("read bufferhandle from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive bufferhandle fd = %{public}d, format = %{public}d", bufhandle->fd, bufhandle->format);

    int32_t fenceTmp = -1;
    if (!DisplayDeviceReadFileDescriptor(&fenceTmp, data)) {
        DISPLAY_LOG("read fence from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive fence = %{public}d", fenceTmp);
    int32_t ret = device_->SetDisplayClientBuffer(devId, *bufhandle, fenceTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return DISPLAY_SUCCESS;
}

int32_t DisplayDeviceServerStub::SetDisplayClientDamage(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t numTmp = 0;
    if (data->ReadUint32(numTmp)) {
        DISPLAY_LOG("read num from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive num = %{public}u", numTmp);
    const uint32_t arrayNum = numTmp;
    IRect rectTmp[arrayNum];
    (void)memset_s(&rectTmp, sizeof(rectTmp), 0, sizeof(rectTmp));
    if (!DisplayDeviceReadData(rectTmp, data, numTmp)) {
        DISPLAY_LOG("read rect from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetDisplayClientDamage(devId, numTmp, rectTmp[0]);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetDisplayVsyncEnabled(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    bool enableTmp = false;
    if (!data->ReadBool(enableTmp)) {
        DISPLAY_LOG("read enable from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive enable = %{public}s", enableTmp ? "true" : "false");
    int32_t ret = device_->SetDisplayVsyncEnabled(devId, enableTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

static void VBlankCallbackFunc(unsigned int sequence, uint64_t ns, void *data)
{
    HDF_LOGI("hotplug callback %{public}d", sequence);
    if (data == nullptr) {
        HDF_LOGI("vblank callback data nullptr");
        return;
    }
    auto callbackRemote = reinterpret_cast<DisplayDeviceCallbackProxy *>(data);
    auto ret = callbackRemote->OnVBlankCallback(sequence, ns);
    if (ret != 0) {
        HDF_LOGE("failed to vblank callback %{public}d", ret);
    } else {
        HDF_LOGE("succ to vblank callback");
    }
}

int32_t DisplayDeviceServerStub::RegDisplayVBlankCallback(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    if (callbackRemote_ == nullptr) {
        DISPLAY_LOG("callback remote object is invalid");
        return HDF_ERR_INVALID_OBJECT;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->RegDisplayVBlankCallback(devId, VBlankCallbackFunc, callbackRemote_.GetRefPtr());
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetDisplayReleaseFence(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t numTmp = 0;
    uint32_t layersTmp[COMPOSER_SERVER_ARRAY_NUMBER_MAX];
    int32_t fenceTmp[COMPOSER_SERVER_ARRAY_NUMBER_MAX];
    (void)memset_s(layersTmp, sizeof(layersTmp), 0, sizeof(layersTmp));
    (void)memset_s(fenceTmp, sizeof(fenceTmp), 0, sizeof(fenceTmp));
    int32_t ret = device_->GetDisplayReleaseFence(devId, &numTmp, layersTmp, fenceTmp);
    DISPLAY_LOG("call impl ret = %{public}d, fence num=%{public}d", ret, numTmp);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!reply->WriteUint32(numTmp)) {
        DISPLAY_LOG("error: server write num into data failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(reply, layersTmp, numTmp)) {
        DISPLAY_LOG("error: server write layers array into data failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteFileDescriptorArray(reply, fenceTmp, numTmp)) {
        DISPLAY_LOG("error: server write fence array into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return DISPLAY_SUCCESS;
}

int32_t DisplayDeviceServerStub::Commit(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }

    int32_t fenceTmp = -1;
    int32_t ret = device_->Commit(devId, fenceTmp);
    DISPLAY_LOG("call Commit impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!DisplayDeviceWriteFileDescriptor(reply, fenceTmp)) {
        DISPLAY_LOG("error: write value into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::InvokeDisplayCmd(MessageParcel *data, MessageParcel *reply)
{
    (void)data;
    (void)reply;
    return DISPLAY_NOT_SUPPORT;
}

int32_t DisplayDeviceServerStub::CreateVirtualDisplay(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t widthTmp = 0;
    if (!data->ReadUint32(widthTmp)) {
        DISPLAY_LOG("read width from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t heightTmp = 0;
    if (!data->ReadUint32(heightTmp)) {
        DISPLAY_LOG("read height from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t formatTmp = 0;
    uint32_t devId = 0;
    int32_t ret = device_->CreateVirtualDisplay(widthTmp, heightTmp, formatTmp, devId);
    DISPLAY_LOG("call impl ret=%{public}d,format=%{public}d,devid= %{public}d", ret, formatTmp, devId);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!reply->WriteInt32(formatTmp)) {
        DISPLAY_LOG("error: write format into data failed");
        return DISPLAY_FAILURE;
    }
    if (!reply->WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return DISPLAY_SUCCESS;
}
int32_t DisplayDeviceServerStub::DestroyVirtualDisplay(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->DestroyVirtualDisplay(devId);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetVirtualDisplayBuffer(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    BufferHandle *bufhandleTmp = nullptr;
    if (!DisplayDeviceReadBufHdl(bufhandleTmp, data)) {
        DISPLAY_LOG("read bufferhandle from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t fenceTmp = -1;
    if (!DisplayDeviceReadFileDescriptor(&fenceTmp, data)) {
        DISPLAY_LOG("read fence from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetVirtualDisplayBuffer(devId, bufhandleTmp, fenceTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

static void RefreshCallbackFunc(uint32_t devId, void *data)
{
    HDF_LOGI("hotplug callback %{public}d", devId);
    if (data == nullptr) {
        HDF_LOGE("refresh callback data nullptr");
        return;
    }
    auto callbackRemote = reinterpret_cast<DisplayDeviceCallbackProxy *>(data);
    auto ret = callbackRemote->OnRefreshCallback(devId);
    if (ret != 0) {
        HDF_LOGE("failed to hotplug callback %{public}d %{public}d", devId, ret);
    } else {
        HDF_LOGE("succ to hotplug callback");
    }
}

int32_t DisplayDeviceServerStub::RegDisplayRefreshCallback(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    if (callbackRemote_ == nullptr) {
        DISPLAY_LOG("callback remote object is invalid");
        return HDF_ERR_INVALID_OBJECT;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->RegDisplayRefreshCallback(devId, RefreshCallbackFunc, callbackRemote_.GetRefPtr());
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetWriteBackFrame(MessageParcel *data, MessageParcel *reply)
{
    (void)data;
    (void)reply;
    return DISPLAY_NOT_SUPPORT;
}

int32_t DisplayDeviceServerStub::CreateWriteBack(MessageParcel *data, MessageParcel *reply)
{
    (void)data;
    (void)reply;
    return DISPLAY_NOT_SUPPORT;
}

int32_t DisplayDeviceServerStub::DestroyWriteBack(MessageParcel *data, MessageParcel *reply)
{
    (void)data;
    (void)reply;
    return DISPLAY_NOT_SUPPORT;
}

int32_t DisplayDeviceServerStub::FileTest(MessageParcel *data, MessageParcel *reply)
{
    HDF_LOGD("testIF 04");
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    bool receiveData = data->ReadBool();
    if (receiveData) {
        HDF_LOGD("stub receive true");
    } else {
        HDF_LOGD("stub receive false");
    }
    if (!reply->WriteBool(receiveData)) {
        HDF_LOGD("testIF error 04 write failed!");
    }
    return DISPLAY_SUCCESS;
}

int32_t DisplayDeviceServerStub::SetCallBackObject(sptr<IRemoteObject> callbackRemote)
{
    callbackRemote_ = iface_cast<DisplayDeviceCallbackProxy>(callbackRemote);
    if (callbackRemote_ == nullptr) {
        HDF_LOGE("failed to iface_cast DisplayDeviceCallbackProxy");
        return HDF_ERR_INVALID_OBJECT;
    }

    return HDF_SUCCESS;
}

int32_t DisplayDeviceServerStub::SetProxyRemoteCallback(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    sptr<IRemoteObject> remoteObj = data->ReadRemoteObject();
    if (remoteObj == nullptr) {
        DISPLAY_LOG("read Remote Object from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = SetCallBackObject(remoteObj);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::CreateLayer(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    LayerInfo layerinfo;
    memset_s(&layerinfo, sizeof(layerinfo), 0, sizeof(layerinfo));
    if (!DisplayDeviceReadData(&layerinfo, data)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }

    uint32_t layerId = 0;
    int32_t ret = device_->CreateLayer(devId, layerinfo, layerId);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!reply->WriteUint32(layerId)) {
        DISPLAY_LOG("error: server write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerVisible(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }

    bool visibleTmp = 0;
    if (!data->ReadBool(visibleTmp)) {
        DISPLAY_LOG("read visible from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetLayerVisible(devId, layerId, visibleTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetLayerVisibleState(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    bool visible = false;
    int32_t ret = device_->GetLayerVisibleState(devId, layerId, visible);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (!reply->WriteBool(visible)) {
        DISPLAY_LOG("error: server write visible into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerCrop(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    IRect rectTmp;
    memset_s(&rectTmp, sizeof(rectTmp), 0, sizeof(rectTmp));
    if (!DisplayDeviceReadData(&rectTmp, data)) {
        DISPLAY_LOG("read rect from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetLayerCrop(devId, layerId, &rectTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerZorder(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t zorderTmp = 0;
    if (!data->ReadUint32(zorderTmp)) {
        DISPLAY_LOG("read zorder from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetLayerZorder(devId, layerId, zorderTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetLayerZorder(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t zorderTmp = 0;
    int32_t ret = device_->GetLayerZorder(devId, layerId, zorderTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!DisplayDeviceWriteInt32(reply, zorderTmp)) {
        DISPLAY_LOG("error: server write zorder into data failed");
        return DISPLAY_FAILURE;
    }

    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerPreMulti(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    bool preMulTmp = 0;
    if (!data->ReadBool(preMulTmp)) {
        DISPLAY_LOG("read preMul from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetLayerPreMulti(devId, layerId, preMulTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetLayerPreMulti(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    bool preMulTmp = 0;
    int32_t ret = device_->GetLayerPreMulti(devId, layerId, preMulTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!reply->WriteBool(preMulTmp)) {
        DISPLAY_LOG("error: server write preMul into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerAlpha(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }

    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    LayerAlpha alpha;
    memset_s(&alpha, sizeof(alpha), 0, sizeof(alpha));
    if (!DisplayDeviceReadData(&alpha, data)) {
        DISPLAY_LOG("read alpha from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetLayerAlpha(devId, layerId, alpha);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetLayerAlpha(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    LayerAlpha alphaTmp = {};
    int32_t ret = device_->GetLayerAlpha(devId, layerId, alphaTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!DisplayDeviceWriteData(reply, &alphaTmp)) {
        DISPLAY_LOG("error: server write zorder into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerColorKey(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    bool enableTmp = false;
    if (!data->ReadBool(enableTmp)) {
        DISPLAY_LOG("read enable from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t keyTmp = 0;
    if (!DisplayDeviceReadData(&keyTmp, data)) {
        DISPLAY_LOG("read key from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetLayerColorKey(devId, layerId, enableTmp, keyTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetLayerColorKey(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    bool enableTmp = false;
    uint32_t keyTmp = 0;
    int32_t ret = device_->GetLayerColorKey(devId, layerId, &enableTmp, &keyTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!reply->WriteBool(enableTmp)) {
        DISPLAY_LOG("error: server write enable into data failed");
        return DISPLAY_FAILURE;
    }
    if (!reply->WriteUint32(keyTmp)) {
        DISPLAY_LOG("error: server write key into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerPalette(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t lenTmp = 0;
    if (!data->ReadUint32(lenTmp)) {
        DISPLAY_LOG("read len from data failed!");
        return DISPLAY_FAILURE;
    }
    const uint32_t arrayNum = lenTmp;
    uint32_t paletteTmp[arrayNum];
    memset_s(&paletteTmp, sizeof(paletteTmp), 0, sizeof(paletteTmp));
    if (!DisplayDeviceReadData(&paletteTmp[0], data, lenTmp)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetLayerPalette(devId, layerId, paletteTmp, lenTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetLayerPalette(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t lenTmp = 0;
    if (!data->ReadUint32(lenTmp)) {
        DISPLAY_LOG("read len from data failed!");
        return DISPLAY_FAILURE;
    }
    const uint32_t arrayNum = lenTmp;
    uint32_t palette[arrayNum];
    (void)memset_s(palette, sizeof(palette), 0, sizeof(palette));
    int32_t ret = device_->GetLayerPalette(devId, layerId, palette[0], lenTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!DisplayDeviceWriteInt32(reply, lenTmp)) {
        DISPLAY_LOG("error: server write len into data failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(reply, &palette[0], lenTmp)) {
        DISPLAY_LOG("error: server write palette array into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerCompression(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t compType = 0;
    if (!data->ReadInt32(compType)) {
        DISPLAY_LOG("read compType from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetLayerCompression(devId, layerId, compType);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetLayerCompression(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t compType = 0;
    int32_t ret = device_->GetLayerCompression(devId, layerId, compType);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!reply->WriteInt32(compType)) {
        DISPLAY_LOG("error: server compType ret into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::Flush(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    LayerBuffer bufferTmp;
    memset_s(&bufferTmp, sizeof(bufferTmp), 0, sizeof(bufferTmp));
    if (!DisplayDeviceReadData(&bufferTmp, data)) {
        DISPLAY_LOG("read bufferTmp from data failed!");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceReadBufHdl(bufferTmp.hdl, reply)) {
        DISPLAY_LOG("error: server write buffer handle into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->Flush(devId, layerId, bufferTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerVisibleRegion(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t numTmp = 0;
    if (!DisplayDeviceReadData(&numTmp, data)) {
        DISPLAY_LOG("read num from data failed!");
        return DISPLAY_FAILURE;
    }
    const uint32_t arrayNum = numTmp;
    IRect rectTmp[arrayNum];
    memset_s(rectTmp, sizeof(rectTmp), 0, sizeof(rectTmp));
    if (!DisplayDeviceReadData(&rectTmp[0], data, numTmp)) {
        DISPLAY_LOG("read rect from data failed!");
        return DISPLAY_FAILURE;
    }

    int32_t ret = device_->SetLayerDirtyRegion(devId, layerId, numTmp, rectTmp[0]);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerDirtyRegion(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t numTmp = 0;
    if (!DisplayDeviceReadData(&numTmp, data)) {
        DISPLAY_LOG("read num from data failed!");
        return DISPLAY_FAILURE;
    }
    const uint32_t arrayNum = numTmp;
    IRect regionTmp[arrayNum];
    memset_s(&regionTmp, sizeof(regionTmp), 0, sizeof(regionTmp));
    if (!DisplayDeviceReadData(&regionTmp[0], data, numTmp)) {
        DISPLAY_LOG("read region from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->SetLayerDirtyRegion(devId, layerId, numTmp, regionTmp[0]);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetLayerBuffer(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    LayerBuffer bufferTmp = {};
    int32_t ret = device_->GetLayerBuffer(devId, layerId, &bufferTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!DisplayDeviceWriteData(reply, &bufferTmp)) {
        DISPLAY_LOG("error: server write buffer into data failed");
        bufferTmp.data.virAddr = nullptr;
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteBufHdl(reply, bufferTmp.hdl)) {
        DISPLAY_LOG("error: server write buffer handle into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerBuffer(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    BufferHandle *bufhandleTmp = nullptr;
    if (!DisplayDeviceReadBufHdl(bufhandleTmp, data)) {
        DISPLAY_LOG("read bufhandle from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t fenceTmp = -1;
    if (!DisplayDeviceReadFileDescriptor(&fenceTmp, data)) {
        DISPLAY_LOG("read fence from data failed!");
        return DISPLAY_FAILURE;
    }

    int32_t ret = device_->SetLayerBuffer(devId, layerId, *bufhandleTmp, fenceTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::InvokeLayerCmd(MessageParcel *data, MessageParcel *reply)
{
    (void)data;
    (void)reply;
    return DISPLAY_NOT_SUPPORT;
}

int32_t DisplayDeviceServerStub::SetLayerCompositionType(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t enumTmp = 0;
    if (!data->ReadUint32(enumTmp)) {
        DISPLAY_LOG("read CompositionType from data failed!");
        return DISPLAY_FAILURE;
    }
    CompositionType compositiontypeTmp = Convert2CompositionType(enumTmp);
    int32_t ret = device_->SetLayerCompositionType(devId, layerId, compositiontypeTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::InitDisplay(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);
    int32_t ret = device_->InitDisplay(devId);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::DeinitDisplay(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);

    int32_t ret = device_->DeinitDisplay(devId);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetDisplayInfo(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);

    DisplayInfo dispInfoTmp = {};
    int32_t ret = device_->GetDisplayInfo(devId, dispInfoTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!DisplayDeviceWriteData(reply, &dispInfoTmp)) {
        DISPLAY_LOG("error: server write dispInfo into data failed");
        return DISPLAY_FAILURE;
    }

    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::CloseLayer(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);

    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->CloseLayer(devId, layerId);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerSize(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }

    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }

    IRect rectTmp = {};
    if (!DisplayDeviceReadData(&rectTmp, data)) {
        DISPLAY_LOG("read rect from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("receive devId = %{public}d, x = %{public}d, y = %{public}d, w = %{public}d, h = %{public}d", devId,
        rectTmp.x, rectTmp.y, rectTmp.w, rectTmp.h);

    int32_t ret = device_->SetLayerSize(devId, layerId, &rectTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::GetLayerSize(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);

    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive layer = %{public}u", layerId);

    IRect rect = {};
    int32_t ret = device_->GetLayerSize(devId, layerId, rect);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!DisplayDeviceWriteData(reply, &rect)) {
        DISPLAY_LOG("write data into reply failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetTransformMode(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }

    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }

    int32_t enumTmp = 0;
    if (!data->ReadInt32(enumTmp)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    TransformType typeTmp = Convert2TransformType(enumTmp);

    int32_t ret = device_->SetTransformMode(devId, layerId, typeTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::WaitForVBlank(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);

    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive layer = %{public}u", layerId);

    int32_t timeOut = 0;
    if (!data->ReadInt32(timeOut)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    int32_t ret = device_->WaitForVBlank(devId, layerId, timeOut);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SnapShot(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive devId = %{public}u", devId);

    LayerBuffer bufferTmp = {};
    if (!DisplayDeviceReadData(&bufferTmp, data)) {
        DISPLAY_LOG("read LayerBuffer from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("receive displayInfo fenceId=%{public}d, pitch=%{public}d", bufferTmp.fenceId, bufferTmp.pitch);

    int32_t ret = device_->SnapShot(devId, bufferTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (!DisplayDeviceWriteData(reply, &bufferTmp)) {
        DISPLAY_LOG("write data into reply failed!");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteBufHdl(reply, bufferTmp.hdl)) {
        DISPLAY_LOG("error: server write buffer handle into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_END;
    return ret;
}

int32_t DisplayDeviceServerStub::SetLayerBlendType(MessageParcel *data, MessageParcel *reply)
{
    DISPLAY_START;
    if (data->ReadInterfaceToken() != DisplayDeviceServerStub::GetDescriptor()) {
        HDF_LOGE("failed to check interface token");
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = 0;
    if (!data->ReadUint32(devId)) {
        DISPLAY_LOG("read devId from data failed!");
        return DISPLAY_FAILURE;
    }
    uint32_t layerId = 0;
    if (!data->ReadUint32(layerId)) {
        DISPLAY_LOG("read layer from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive layer = %{public}u", layerId);

    int32_t enumTmp = 0;
    if (!data->ReadInt32(enumTmp)) {
        DISPLAY_LOG("read BlendType from data failed!");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("server receive type = %{public}d", enumTmp);
    BlendType typeTmp = Convert2BlendTypeType(enumTmp);

    int32_t ret = device_->SetLayerBlendType(devId, layerId, typeTmp);
    DISPLAY_LOG("call impl ret = %{public}d", ret);
    DISPLAY_END;
    return ret;
}
