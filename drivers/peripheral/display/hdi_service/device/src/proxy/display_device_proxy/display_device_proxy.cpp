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

#include "display_device_proxy.h"
#include "display_device_common.h"

#undef HDF_LOG_TAG
#define HDF_LOG_TAG DisplayHostProxy

using OHOS::Display::Device::IDisplayDevice;
using OHOS::Display::Device::Client::DisplayDeviceProxy;
using std::map;
using std::move;
using std::pair;
using std::unique_ptr;

static constexpr uint32_t MAX_DEVID = 4;
static const std::string serverName = "display_device_service";
static OHOS::sptr<IDisplayDevice> g_instance;

void IDisplayDevice::Initialize(void)
{
    DISPLAY_START;
    if (g_instance != nullptr) {
        DISPLAY_LOG("initialize was already");
        DISPLAY_END;
        return;
    }
    do {
        using OHOS::sptr;
        using OHOS::HDI::ServiceManager::V1_0::IServiceManager;
        auto servMgr = IServiceManager::Get();
        if (servMgr == nullptr) {
            DISPLAY_LOG("IServiceManager failed!");
            break;
        }
        sptr<IRemoteObject> remote = servMgr->GetService(serverName.c_str());
        if (remote == nullptr) {
            DISPLAY_LOG("IServiceManager IDisplayDevice(%{public}s) failed!", serverName.c_str());
            break;
        }
        g_instance = iface_cast<IDisplayDevice>(remote);
        if (g_instance == nullptr) {
            DISPLAY_LOG("failed to iface cast IDisplayDevice");
            break;
        }
        g_instance->m_hotPlugObject_ = new DisplayRegisterCallbackFramework();
        if (g_instance->m_hotPlugObject_ == nullptr) {
            DISPLAY_LOG("create DisplayRegisterCallbackFramework failed!");
            break;
        }
        DISPLAY_END;
        return;
    } while (0);
    return;
}

OHOS::sptr<IDisplayDevice> DisplayDeviceProxy::GetInstance(void)
{
    DISPLAY_START;
    DISPLAY_END;
    if (g_instance == nullptr) {
        Initialize();
    }
    return g_instance;
}

void DisplayDeviceProxy::ReleaseInstance(void)
{
    DISPLAY_START;
    DISPLAY_END;
    g_instance = nullptr;
}

int32_t DisplayDeviceProxy::RegHotPlugCallback(HotPlugCallback callback, void *callBackdata)
{
    DISPLAY_LOG("interface start");
    if (callback == nullptr || callBackdata == nullptr) {
        DISPLAY_LOG("callback %{public}s nullptr, data %{public}s nullptr", callback == nullptr ? "is" : "is not",
            callBackdata == nullptr ? "is" : "is not");
        return DISPLAY_PARAM_ERR;
    }

    m_hotPlugObject_->SetHotplugInData(callback, callBackdata);
    SetProxyRemoteCallback(m_hotPlugObject_);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor())) {
        return HDF_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_REGHOTPLUGCALLBACK, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::RegDisplayVBlankCallback(uint32_t devId, VBlankCallback callback, void *callbackData)
{
    DISPLAY_LOG("interface start");
    if (callback == nullptr || (devId > MAX_DEVID)) {
        DISPLAY_LOG("callback %{public}s nullptr, data %{public}s nullptr", callback == nullptr ? "is" : "is not",
            callbackData == nullptr ? "is" : "is not");
        return DISPLAY_PARAM_ERR;
    }

    m_hotPlugObject_->SetVBlankData(callback, callbackData);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into parcel failed");
        return false;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_REGDISPLAYVBLANKCALLBACK, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::RegDisplayRefreshCallback(uint32_t devId, RefreshCallback callback, void *callBackdata)
{
    DISPLAY_LOG("interface start");
    if (callback == nullptr || (devId > MAX_DEVID)) {
        DISPLAY_LOG("callback %{public}s nullptr, data %{public}s nullptr", callback == nullptr ? "is" : "is not",
            callBackdata == nullptr ? "is" : "is not");
        return DISPLAY_PARAM_ERR;
    }

    m_hotPlugObject_->SetRefreshData(callback, callBackdata);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_REGDISPLAYREFRESHCALLBACK, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetDisplayCapability(uint32_t devId, DisplayCapability &info)
{
    DISPLAY_LOG("interface start");
    printf("DisplayDeviceProxy:%s %d", __func__, __LINE__);
    if (devId > MAX_DEVID) {
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETDISPLAYCAPABILITY, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    if (!DisplayDeviceReadData(&info, &reply)) {
        DISPLAY_LOG("error: %{public}s failed %{public}d", __func__, ret);
        ret = DISPLAY_FAILURE;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetDisplaySupportedModes(uint32_t devId, uint32_t &num, DisplayModeInfo *modes)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteInt32(num)) {
        DISPLAY_LOG("error: %{public}s write num into data failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_GETDISPLAYSUPPORTEDMODES, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    num = reply.ReadInt32();
    if (num > 0 && modes != nullptr && !DisplayDeviceReadData(modes, &reply, num)) {
        DISPLAY_LOG("error: %{public}s read reply failed", __func__);
        ret = DISPLAY_FAILURE;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetDisplayMode(uint32_t devId, uint32_t &modeId)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETDISPLAYMODE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    modeId = reply.ReadUint32();
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetDisplayMode(uint32_t devId, uint32_t modeId)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("modeId is nullptr");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(modeId)) {
        DISPLAY_LOG("error: %{public}s write modeId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETDISPLAYMODE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetDisplayPowerStatus(uint32_t devId, DispPowerStatus &status)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("status is nullptr");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETDISPLAYPOWERSTATUS, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    status = static_cast<DispPowerStatus>(reply.ReadInt32());
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetDisplayPowerStatus(uint32_t devId, DispPowerStatus status)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("status is nullptr");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(status)) {
        DISPLAY_LOG("error: %{public}s write status into data failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_SETDISPLAYPOWERSTATUS, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetDisplayBacklight(uint32_t devId, uint32_t &level)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETDISPLAYBACKLIGHT, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    level = reply.ReadUint32();
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetDisplayBacklight(uint32_t devId, uint32_t level)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(level)) {
        DISPLAY_LOG("error: %{public}s write status into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETDISPLAYBACKLIGHT, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetDisplayProperty(uint32_t devId, uint32_t propertyId, uint64_t &value)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(propertyId)) {
        DISPLAY_LOG("error: %{public}s write status into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETDISPLAYPROPERTY, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    value = reply.ReadUint64();
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetDisplayProperty(uint32_t devId, uint32_t propertyId, uint64_t value)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(propertyId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(value)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETDISPLAYPROPERTY, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::PrepareDisplayLayers(uint32_t devId, bool &needFlushFb)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(needFlushFb)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_PREPAREDISPLAYLAYERS, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    needFlushFb = reply.ReadBool();
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetDisplayCompChange(uint32_t devId, uint32_t &num, uint32_t *layers, int32_t *type)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETDISPLAYCOMPCHANGE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    num = reply.ReadUint32();
    if (num == 0) {
        return ret;
    }

    if (layers != nullptr && !DisplayDeviceReadData(layers, &reply, num)) {
        DISPLAY_LOG("error: %{public}s read reply layers failed", __func__);
        ret = DISPLAY_FAILURE;
    }
    if (type != nullptr && !DisplayDeviceReadData(type, &reply, num)) {
        DISPLAY_LOG("error: %{public}s read reply layers failed", __func__);
        ret = DISPLAY_FAILURE;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetDisplayClientCrop(uint32_t devId, const IRect *rect)
{
    DISPLAY_LOG("interface start");
    if (rect == nullptr) {
        DISPLAY_LOG("rect is nullptr");
        return DISPLAY_PARAM_ERR;
    }
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(&data, rect)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETDISPLAYCLIENTCROP, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetDisplayClientDestRect(uint32_t devId, const IRect &rect)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(&data, &rect)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_SETDISPLAYCLIENTDESTRECT, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetDisplayClientBuffer(uint32_t devId, const BufferHandle &bufhandle, int32_t fence)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteBufHdl(&data, &bufhandle)) {
        DISPLAY_LOG("error: write bufferhandle into data failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteFileDescriptor(&data, fence)) {
        DISPLAY_LOG("error: write size into data failed");
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_SETDISPLAYCLIENTBUFFER, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetDisplayClientDamage(uint32_t devId, uint32_t num, const IRect &rect)
{
    DISPLAY_LOG("interface start");
    if (num == 0) {
        return DISPLAY_PARAM_ERR;
    }

    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(num)) {
        DISPLAY_LOG("error: %{public}s write num into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(&data, &rect, num)) {
        DISPLAY_LOG("error: write rect into data failed");
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_SETDISPLAYCLIENTDAMAGE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetDisplayVsyncEnabled(uint32_t devId, bool enabled)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteBool(enabled)) {
        DISPLAY_LOG("error: write enabled into data failed");
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_SETDISPLAYVSYNCENABLED, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetDisplayReleaseFence(uint32_t devId, uint32_t *num, uint32_t *layers, int32_t *fences)
{
    DISPLAY_LOG("interface start");
    if (num == nullptr) {
        DISPLAY_LOG("num %{public}s nullptr, layers %{public}s nullptr, fences %{public}s nullptr",
            (num == nullptr) ? "is" : "is not", (layers == nullptr) ? "is" : "is not",
            (fences == nullptr) ? "is" : "is not");
        return DISPLAY_PARAM_ERR;
    }
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETDISPLAYRELEASEFENCE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    uint32_t numRet = reply.ReadUint32();
    DISPLAY_LOG("numRet = %{public}d", numRet);
    if (numRet > *num && (layers != nullptr || fences != nullptr)) {
        DISPLAY_LOG("error: failed out buffer to small %{public}u vs %{public}u", numRet, *num);
        return DISPLAY_PARAM_ERR;
    }
    *num = numRet;
    if (layers != nullptr && !DisplayDeviceReadData(layers, &reply, numRet)) {
        DISPLAY_LOG("error: %{public}s read reply layers failed", __func__);
        ret = DISPLAY_FAILURE;
    }
    if (fences != nullptr && !DisplayDeviceReadFileDescriptorArray(fences, &reply, numRet)) {
        DISPLAY_LOG("error: %{public}s read reply fences failed", __func__);
        ret = DISPLAY_FAILURE;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::Commit(uint32_t devId, int32_t &fence)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_COMMIT, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    if (!DisplayDeviceReadFileDescriptor(&fence, &reply)) {
        DISPLAY_LOG("error: read reply failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::InvokeDisplayCmd(uint32_t devId, ...)
{
    (void)devId;
    return DISPLAY_NOT_SUPPORT;
}

int32_t DisplayDeviceProxy::CreateVirtualDisplay(uint32_t width, uint32_t height, int32_t &format, uint32_t &devId)
{
    DISPLAY_LOG("interface start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(width)) {
        DISPLAY_LOG("error: %{public}s write width into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(height)) {
        DISPLAY_LOG("error: %{public}s write height into data failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_CREATEVIRTUALDISPLAY, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    format = reply.ReadInt32();
    devId = reply.ReadUint32();
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::DestroyVirtualDisplay(uint32_t devId)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_DESTROYVIRTUALDISPLAY, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetVirtualDisplayBuffer(uint32_t devId, const BufferHandle *bufhandle, int32_t fence)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    if (bufhandle == nullptr) {
        DISPLAY_LOG("bufhandle is nullptr");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteBufHdl(&data, bufhandle)) {
        DISPLAY_LOG("error: write bufferhandle into data failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteFileDescriptor(&data, fence)) {
        DISPLAY_LOG("error: write size into data failed");
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_SETVIRTUALDISPLAYBUFFER, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetWriteBackFrame(uint32_t devId, BufferHandle &buffer, int32_t &fence)
{
    DISPLAY_LOG("interface not support");
    (void)devId;
    (void)buffer;
    (void)fence;
    return DISPLAY_NOT_SUPPORT;
}

int32_t DisplayDeviceProxy::CreateWriteBack(uint32_t &devId, uint32_t width, uint32_t height, int32_t &format)
{
    DISPLAY_LOG("interface start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(width)) {
        DISPLAY_LOG("error: %{public}s write width into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(height)) {
        DISPLAY_LOG("error: %{public}s write height into data failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_CREATEWRITEBACK, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    devId = reply.ReadUint32();
    format = reply.ReadInt32();
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::DestroyWriteBack(uint32_t devId)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_DESTROYWRITEBACK, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetProxyRemoteCallback(const OHOS::sptr<DisplayRegisterCallbackBase> &callback)
{
    DISPLAY_LOG("interface start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) ||
        !data.WriteRemoteObject(callback->AsObject())) {
        DISPLAY_LOG("error: write callback into data failed");
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_SET_PROXY_REMOTE_CALLBACK, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::InitDisplay(uint32_t devId)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_INITDISPLAY, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::DeinitDisplay(uint32_t devId)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_DEINITDISPLAY, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetDisplayInfo(uint32_t devId, DisplayInfo &dispInfo)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETDISPLAYINFO, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    if (!DisplayDeviceReadData(&dispInfo, &reply)) {
        DISPLAY_LOG("error: read reply failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::CreateLayer(uint32_t devId, const LayerInfo &layerInfo, uint32_t &layerId)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(&data, &layerInfo)) {
        DISPLAY_LOG("error: write rect into data failed");
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_CREATELAYER, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    layerId = reply.ReadUint32();
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::CloseLayer(uint32_t devId, uint32_t layerId)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_CLOSELAYER, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerVisible(uint32_t devId, uint32_t layerId, bool visible)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteBool(visible)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERVISIBLE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetLayerVisibleState(uint32_t devId, uint32_t layerId, bool &visible)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETLAYERVISIBLESTATE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    visible = reply.ReadBool();
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerSize(uint32_t devId, uint32_t layerId, const IRect *rect)
{
    DISPLAY_LOG("interface start");
    if (rect == nullptr) {
        DISPLAY_LOG("rect is nullptr");
        return DISPLAY_PARAM_ERR;
    }
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(&data, rect)) {
        DISPLAY_LOG("error: write rect into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERSIZE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetLayerSize(uint32_t devId, uint32_t layerId, IRect &rect)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETLAYERSIZE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    if (!DisplayDeviceReadData(&rect, &data)) {
        DISPLAY_LOG("error: read rect into data failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerCrop(uint32_t devId, uint32_t layerId, const IRect *rect)
{
    DISPLAY_LOG("interface start");
    if (rect == nullptr) {
        DISPLAY_LOG("rect is nullptr");
        return DISPLAY_PARAM_ERR;
    }

    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(&data, rect)) {
        DISPLAY_LOG("error: write rect into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERCROP, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerZorder(uint32_t devId, uint32_t layerId, uint32_t zorder)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(zorder)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERZORDER, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetLayerZorder(uint32_t devId, uint32_t layerId, uint32_t &zorder)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_GETLAYERZORDER, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    zorder = reply.ReadUint32();
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerPreMulti(uint32_t devId, uint32_t layerId, bool preMul)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteBool(preMul)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERPREMULTI, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetLayerPreMulti(uint32_t devId, uint32_t layerId, bool &preMul)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETLAYERPREMULTI, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    preMul = reply.ReadBool();
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerAlpha(uint32_t devId, uint32_t layerId, const LayerAlpha &alpha)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(&data, &alpha)) {
        DISPLAY_LOG("error: write alpha into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERALPHA, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetLayerAlpha(uint32_t devId, uint32_t layerId, LayerAlpha &alpha)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_GETLAYERALPHA, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    if (!DisplayDeviceReadData(&alpha, &reply)) {
        DISPLAY_LOG("error: read alpha failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerColorKey(uint32_t devId, uint32_t layerId, bool enable, uint32_t key)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteBool(enable)) {
        DISPLAY_LOG("error: %{public}s write enable into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(key)) {
        DISPLAY_LOG("error: %{public}s write key into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERCOLORKEY, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetLayerColorKey(uint32_t devId, uint32_t layerId, bool *enable, uint32_t *key)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETLAYERCOLORKEY, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    *enable = reply.ReadBool();
    *key = reply.ReadUint32();
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerPalette(uint32_t devId, uint32_t layerId, const uint32_t *palette, uint32_t len)
{
    DISPLAY_LOG("interface start");
    if (palette == nullptr) {
        DISPLAY_LOG("palette is nullptr");
        return DISPLAY_PARAM_ERR;
    }

    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(len)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(&data, &palette[0], len)) {
        DISPLAY_LOG("error: write enable into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERPALETTE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetLayerPalette(uint32_t devId, uint32_t layerId, uint32_t &palette, uint32_t len)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(DSP_CMD_GETLAYERPALETTE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    len = reply.ReadInt32();
    if (!DisplayDeviceReadData(&palette, &reply, len)) {
        DISPLAY_LOG("error: read reply failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetTransformMode(uint32_t devId, uint32_t layerId, TransformType type)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: %{public}s write devId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: %{public}s write layerId into data failed", __func__);
        return DISPLAY_FAILURE;
    }
    if (!data.WriteInt32(type)) {
        DISPLAY_LOG("error: write type into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETTRANSFORMMODE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerCompression(uint32_t devId, uint32_t layerId, int32_t compType)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteInt32(compType)) {
        DISPLAY_LOG("error: write compType into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERCOMPRESSION, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetLayerCompression(uint32_t devId, uint32_t layerId, int32_t &compType)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETLAYERCOMPRESSION, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    compType = reply.ReadInt32();
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerDirtyRegion(uint32_t devId, uint32_t layerId, uint32_t num, const IRect &region)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(num)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(&data, &region, num)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERDIRTYREGION, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::GetLayerBuffer(uint32_t devId, uint32_t layerId, LayerBuffer *buffer)
{
    DISPLAY_LOG("interface start");
    if (buffer == nullptr) {
        DISPLAY_LOG("buffer is nullptr");
        return DISPLAY_PARAM_ERR;
    }

    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_GETLAYERBUFFER, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    if (!DisplayDeviceReadData(buffer, &reply)) {
        DISPLAY_LOG("error: read reply failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceReadBufHdl(buffer->hdl, &reply)) {
        DISPLAY_LOG("error: read BufferHandle reply failed");
        return DISPLAY_FAILURE;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::Flush(uint32_t devId, uint32_t layerId, LayerBuffer &buffer)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(&data, &buffer)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteBufHdl(&data, buffer.hdl)) {
        DISPLAY_LOG("error: server write buffer handle into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_FLUSH, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::WaitForVBlank(uint32_t devId, uint32_t layerId, int32_t timeOut)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteInt32(timeOut)) {
        DISPLAY_LOG("error: write timeOut into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_WAITFORVBLANK, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SnapShot(uint32_t devId, LayerBuffer &buffer)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SNAPSHOT, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    // LayerBuffer's member 'BufferHandle* hdl' may not fully transmitted
    if (!DisplayDeviceReadData(&buffer, &reply)) {
        DISPLAY_LOG("error: read reply failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceReadBufHdl(buffer.hdl, &reply)) {
        DISPLAY_LOG("error: server write buffer handle into data failed");
        return DISPLAY_FAILURE;
    }

    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerVisibleRegion(uint32_t devId, uint32_t layerId, uint32_t num, const IRect &rect)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(num)) {
        DISPLAY_LOG("error: write num into data failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteData(&data, &rect, num)) {
        DISPLAY_LOG("error: write rect into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERVISIBLEREGION, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerBuffer(uint32_t devId, uint32_t layerId, const BufferHandle &buffer, int32_t fence)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteBufHdl(&data, &buffer)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!DisplayDeviceWriteFileDescriptor(&data, fence)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERBUFFER, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::InvokeLayerCmd(uint32_t devId, uint32_t layerId, uint32_t cmd, ...)
{
    DISPLAY_LOG("interface not support");
    (void)devId;
    (void)layerId;
    (void)cmd;
    return DISPLAY_NOT_SUPPORT;
}

int32_t DisplayDeviceProxy::SetLayerCompositionType(uint32_t devId, uint32_t layerId, CompositionType type)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteInt32(type)) {
        DISPLAY_LOG("error: write type into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERCOMPOSITIONTYPE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}

int32_t DisplayDeviceProxy::SetLayerBlendType(uint32_t devId, uint32_t layerId, BlendType type)
{
    DISPLAY_LOG("interface start");
    if (devId > MAX_DEVID) {
        DISPLAY_LOG("param error");
        return DISPLAY_PARAM_ERR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DisplayDeviceProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        DISPLAY_LOG("error: write devId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteUint32(layerId)) {
        DISPLAY_LOG("error: write layerId into data failed");
        return DISPLAY_FAILURE;
    }
    if (!data.WriteInt32(type)) {
        DISPLAY_LOG("error: write type into data failed");
        return DISPLAY_FAILURE;
    }
    int32_t ret = Remote()->SendRequest(DSP_CMD_SETLAYERBLENDTYPE, data, reply, option);
    if (ret != 0) {
        DISPLAY_LOG("error: failed %{public}d", ret);
        return ret;
    }
    DISPLAY_LOG("interface end");
    return ret;
}
