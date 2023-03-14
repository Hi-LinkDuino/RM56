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

#include "video_layer_proxy.h"
#include <buffer_handle_parcel.h>
#include <hdf_base.h>
#include <hdf_log.h>
#include <message_parcel.h>

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
sptr<IDisplayLayer> IDisplayLayer::Get(const char *serviceName)
{
    do {
        using namespace OHOS::HDI::ServiceManager::V1_0;
        auto servMgr = IServiceManager::Get();
        if (servMgr == nullptr) {
            HDF_LOGE("%{public}s: IServiceManager failed", __func__);
            break;
        }

        auto remote = servMgr->GetService(serviceName);
        if (remote != nullptr) {
            sptr<VideoLayerProxy> layerSptr = iface_cast<VideoLayerProxy>(remote);
            return layerSptr;
        }
        HDF_LOGE("%{public}s: GetService failed! serviceName = %{public}s", __func__, serviceName);
    } while (false);

    return nullptr;
}

DispErrCode VideoLayerProxy::InitDisplay(unsigned int devId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        HDF_LOGE("%{public}s: write devId failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_INIT_DISPLAY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
    }
    return retCode;
}

DispErrCode VideoLayerProxy::DeinitDisplay(unsigned int devId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        HDF_LOGE("%{public}s: write devId failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_DEINIT_DISPLAY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
    }
    return retCode;
}

DispErrCode VideoLayerProxy::GetDisplayInfo(unsigned int devId, std::shared_ptr<DisplayInfo> &dispInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        HDF_LOGE("%{public}s: write devId failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_GET_DISPLAY_INFO, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
        return retCode;
    }

    dispInfo = std::make_shared<DisplayInfo>();
    if (dispInfo == nullptr) {
        HDF_LOGE("%{public}s: dispInfo is nullptr", __func__);
        return DISPLAY_FAILURE;
    }
    dispInfo->width = reply.ReadUint32();
    dispInfo->height = reply.ReadUint32();
    dispInfo->rotAngle = reply.ReadInt32();

    return DISPLAY_SUCCESS;
}

DispErrCode VideoLayerProxy::CreateLayer(unsigned int devId, LayerInfo &layerInfo, unsigned int &layerId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId)) {
        HDF_LOGE("%{public}s: write devId failed", __func__);
        return DISPLAY_FAILURE;
    }

    bool flag = data.WriteInt32(layerInfo.width);
    flag |= data.WriteInt32(layerInfo.height);
    flag |= data.WriteInt32(static_cast<int32_t>(layerInfo.type));
    flag |= data.WriteInt32(layerInfo.bpp);
    flag |= data.WriteInt32(static_cast<int32_t>(layerInfo.pixFormat));
    if (!flag) {
        HDF_LOGE("%s: write layer info failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_CREATE_LAYER, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
        return retCode;
    }

    layerId = reply.ReadUint32();
    return DISPLAY_SUCCESS;
}

DispErrCode VideoLayerProxy::CloseLayer(unsigned int devId, unsigned int layerId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId) ||
        !data.WriteUint32(layerId)) {
        HDF_LOGE("%{public}s: write devId or layerId failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_CLOSE_LAYER, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
    }
    return retCode;
}

DispErrCode VideoLayerProxy::SetLayerVisible(unsigned int devId, unsigned int layerId, bool visible)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId) ||
        !data.WriteUint32(layerId)) {
        HDF_LOGE("%{public}s: write devId or layerId failed", __func__);
        return DISPLAY_FAILURE;
    }

    if (!data.WriteBool(visible)) {
        HDF_LOGE("%{public}s: write visible failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_SET_LAYER_VISIBLE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
    }
    return retCode;
}

DispErrCode VideoLayerProxy::GetLayerVisibleState(unsigned int devId, unsigned int layerId, bool &visible)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId) ||
        !data.WriteUint32(layerId)) {
        HDF_LOGE("%{public}s: write devId or layerId failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_GET_LAYER_VISIBLE_STATE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
        return retCode;
    }

    visible = reply.ReadBool();
    return DISPLAY_SUCCESS;
}

DispErrCode VideoLayerProxy::SetLayerRect(unsigned int devId, unsigned int layerId, IRect &rect)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId) ||
        !data.WriteUint32(layerId)) {
        HDF_LOGE("%{public}s: write devId or layerId failed", __func__);
        return DISPLAY_FAILURE;
    }

    bool flag = data.WriteInt32(rect.x);
    flag |= data.WriteInt32(rect.y);
    flag |= data.WriteInt32(rect.w);
    flag |= data.WriteInt32(rect.h);
    if (!flag) {
        HDF_LOGE("%s: write rect failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_SET_LAYER_RECT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
    }
    return retCode;
}

DispErrCode VideoLayerProxy::GetLayerRect(unsigned int devId, unsigned int layerId, std::shared_ptr<IRect> &rect)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId) ||
        !data.WriteUint32(layerId)) {
        HDF_LOGE("%{public}s: write devId or layerId failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_GET_LAYER_RECT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
        return retCode;
    }

    rect = std::make_shared<IRect>();
    if (rect == nullptr) {
        HDF_LOGE("%{public}s: rect is nullptr", __func__);
        return DISPLAY_FAILURE;
    }
    rect->x = reply.ReadInt32();
    rect->y = reply.ReadInt32();
    rect->w = reply.ReadInt32();
    rect->h = reply.ReadInt32();
    return DISPLAY_SUCCESS;
}

DispErrCode VideoLayerProxy::SetLayerZorder(unsigned int devId, unsigned int layerId, unsigned int zorder)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId) ||
        !data.WriteUint32(layerId)) {
        HDF_LOGE("%{public}s: write devId or layerId failed", __func__);
        return DISPLAY_FAILURE;
    }

    if (!data.WriteUint32(zorder)) {
        HDF_LOGE("%{public}s: write zorder failed!", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_SET_LAYER_ZORDER, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
    }
    return retCode;
}

DispErrCode VideoLayerProxy::GetLayerZorder(unsigned int devId, unsigned int layerId, unsigned int &zorder)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId) ||
        !data.WriteUint32(layerId)) {
        HDF_LOGE("%{public}s: write devId or layerId failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_GET_LAYER_ZORDER, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
        return retCode;
    }
    zorder = reply.ReadUint32();
    return DISPLAY_SUCCESS;
}

DispErrCode VideoLayerProxy::SetTransformMode(unsigned int devId, unsigned int layerId, TransformType &type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId) ||
        !data.WriteUint32(layerId)) {
        HDF_LOGE("%{public}s: write devId or layerId failed", __func__);
        return DISPLAY_FAILURE;
    }

    if (!data.WriteUint32(static_cast<unsigned int>(type))) {
        HDF_LOGE("%{public}s: write transmode failed!", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_SET_TRANSFORM_MODE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
    }
    return retCode;
}

DispErrCode VideoLayerProxy::SetLayerBuffer(unsigned int devId, unsigned int layerId,
    const BufferHandle &buffer, int fence)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    (void)fence;

    if (!data.WriteInterfaceToken(VideoLayerProxy::GetDescriptor()) || !data.WriteUint32(devId) ||
        !data.WriteUint32(layerId)) {
        HDF_LOGE("%{public}s: write devId or layerId failed", __func__);
        return DISPLAY_FAILURE;
    }

    if (!OHOS::WriteBufferHandle(data, buffer)) {
        HDF_LOGE("%{public}s: write bufferHandle failed", __func__);
        return DISPLAY_FAILURE;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISPLAY_LAYER_REMOTE_SET_LAYER_BUFFER, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return DISPLAY_FAILURE;
    }

    DispErrCode retCode = static_cast<DispErrCode>(reply.ReadInt32());
    if (retCode != DISPLAY_SUCCESS) {
        HDF_LOGE("%{public}s: failed retCode is %{public}d", __func__, retCode);
    }
    return retCode;
}
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS