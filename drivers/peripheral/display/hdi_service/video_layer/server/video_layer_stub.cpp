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

#include <buffer_handle_parcel.h>
#include <buffer_handle_utils.h>
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdf_sbuf_ipc.h>

#include "video_layer_stub.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
VideoLayerStub::VideoLayerStub() {}

DispErrCode VideoLayerStub::Init()
{
    layerService_ = std::make_shared<VideoLayerService>();
    if (layerService_ == nullptr) {
        HDF_LOGE("%{public}s: layer service start failed", __func__);
        return DISPLAY_FAILURE;
    }
    return DISPLAY_SUCCESS;
}

int32_t VideoLayerStub::LayerStubInitDisplay(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();

    DispErrCode retCode = layerService_->InitDisplay(devId);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubDeinitDisplay(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();

    DispErrCode retCode = layerService_->DeinitDisplay(devId);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubGetDisplayInfo(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();
    std::shared_ptr<DisplayInfo> dispInfo = std::make_shared<DisplayInfo>();

    DispErrCode retCode = layerService_->GetDisplayInfo(devId, dispInfo);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    bool flag = reply.WriteUint32(dispInfo->width);
    flag |= reply.WriteUint32(dispInfo->height);
    flag |= reply.WriteInt32(dispInfo->rotAngle);
    if (!flag) {
        HDF_LOGE("%{public}s: write display info failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubCreateLayer(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();
    uint32_t layerId = 0;
    LayerInfo pLayerInfo;

    pLayerInfo.width = data.ReadInt32();
    pLayerInfo.height = data.ReadInt32();
    pLayerInfo.type = static_cast<LayerType>(data.ReadInt32());
    pLayerInfo.bpp = data.ReadInt32();
    pLayerInfo.pixFormat = static_cast<PixelFormat>(data.ReadInt32());

    DispErrCode retCode = layerService_->CreateLayer(devId, pLayerInfo, layerId);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    if (!reply.WriteUint32(layerId)) {
        HDF_LOGE("%{public}s: write layerId failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubCloseLayer(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();
    uint32_t layerId = data.ReadUint32();

    DispErrCode retCode = layerService_->CloseLayer(devId, layerId);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubSetLayerVisible(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();
    uint32_t layerId = data.ReadUint32();
    bool visible = data.ReadBool();

    DispErrCode retCode = layerService_->SetLayerVisible(devId, layerId, visible);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubGetLayerVisibleState(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();
    uint32_t layerId = data.ReadUint32();
    bool visible = false;

    DispErrCode retCode = layerService_->GetLayerVisibleState(devId, layerId, visible);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    if (!reply.WriteBool(visible)) {
        HDF_LOGE("%{public}s: write visible state failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubSetLayerRect(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();
    uint32_t layerId = data.ReadUint32();
    IRect rect;

    rect.x = data.ReadInt32();
    rect.y = data.ReadInt32();
    rect.w = data.ReadInt32();
    rect.h = data.ReadInt32();

    DispErrCode retCode = layerService_->SetLayerRect(devId, layerId, rect);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubGetLayerRect(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();
    uint32_t layerId = data.ReadUint32();
    std::shared_ptr<IRect> rect = std::make_shared<IRect>();

    DispErrCode retCode = layerService_->GetLayerRect(devId, layerId, rect);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    bool flag = reply.WriteInt32(rect->x);
    flag |= reply.WriteInt32(rect->y);
    flag |= reply.WriteInt32(rect->w);
    flag |= reply.WriteInt32(rect->h);
    if (!flag) {
        HDF_LOGE("%{public}s: write rect failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubSetLayerZorder(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();
    uint32_t layerId = data.ReadUint32();
    uint32_t zorder = data.ReadUint32();

    DispErrCode retCode = layerService_->SetLayerZorder(devId, layerId, zorder);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubGetLayerZorder(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();
    uint32_t layerId = data.ReadUint32();
    uint32_t zorder = 0;

    DispErrCode retCode = layerService_->GetLayerZorder(devId, layerId, zorder);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    if (!reply.WriteUint32(zorder)) {
        HDF_LOGE("%{public}s: write zorder failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubSetLayerTransformMode(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();
    uint32_t layerId = data.ReadUint32();
    TransformType transForm = static_cast<TransformType>(data.ReadUint32());

    DispErrCode retCode = layerService_->SetTransformMode(devId, layerId, transForm);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubSetLayerBuffer(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != VideoLayerStub::GetDescriptor()) {
        return HDF_ERR_INVALID_PARAM;
    }
    uint32_t devId = data.ReadUint32();
    uint32_t layerId = data.ReadUint32();
    BufferHandle *buffer = nullptr;

    buffer = OHOS::ReadBufferHandle(data);
    if (buffer == nullptr) {
        HDF_LOGE("%{public}s: read bufferHandle failed", __func__);
        return DISPLAY_FAILURE;
    }
    buffer->virAddr = nullptr;
    uint32_t fence = data.ReadInt32();

    DispErrCode retCode = layerService_->SetLayerBuffer(devId, layerId, *buffer, fence);
    (void)FreeBufferHandle(buffer);
    if (!reply.WriteInt32(static_cast<int32_t>(retCode))) {
        HDF_LOGE("%{public}s: write retcode failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t VideoLayerStub::LayerStubOnRemoteRequest(
    int cmdId, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    switch (cmdId) {
        case CMD_DISPLAY_LAYER_INIT_DISPLAY: {
            return LayerStubInitDisplay(data, reply, option);
        }
        case CMD_DISPLAY_LAYER_DEINIT_DISPLAY: {
            return LayerStubDeinitDisplay(data, reply, option);
        }
        case CMD_DISPLAY_LAYER_GET_DISPLAY_INFO: {
            return LayerStubGetDisplayInfo(data, reply, option);
        }
        case CMD_DISPLAY_LAYER_CREATE_LAYER: {
            return LayerStubCreateLayer(data, reply, option);
        }
        case CMD_DISPLAY_LAYER_CLOSE_LAYER: {
            return LayerStubCloseLayer(data, reply, option);
        }
        case CMD_DISPLAY_LAYER_SET_LAYER_BUFFER: {
            return LayerStubSetLayerBuffer(data, reply, option);
        }
        case CMD_DISPLAY_LAYER_SET_LAYER_RECT: {
            return LayerStubSetLayerRect(data, reply, option);
        }
        case CMD_DISPLAY_LAYER_GET_LAYER_RECT: {
            return LayerStubGetLayerRect(data, reply, option);
        }
        case CMD_DISPLAY_LAYER_SET_LAYER_ZORDER: {
            return LayerStubSetLayerZorder(data, reply, option);
        }
        case CMD_DISPLAY_LAYER_GET_LAYER_ZORDER: {
            return LayerStubGetLayerZorder(data, reply, option);
        }
        case CMD_DISPLAY_LAYER_SET_LAYER_VISIBLE: {
            return LayerStubSetLayerVisible(data, reply, option);
        }
        case CMD_DISPLAY_LAYER_GET_LAYER_VISIBLE_STATE: {
            return LayerStubGetLayerVisibleState(data, reply, option);
        }
        case CMD_DISPLAY_LAYER_SET_TRANSFORM_MODE: {
            return LayerStubSetLayerTransformMode(data, reply, option);
        }
        default: {
            HDF_LOGE("%{public}s: not support cmd %{public}d", __func__, cmdId);
            return HDF_ERR_INVALID_PARAM;
        }
    }
    return HDF_SUCCESS;
}
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

using namespace OHOS::HDI::Display::V1_0;

void *LayerStubInstance()
{
    VideoLayerStub *stub = new (std::nothrow) VideoLayerStub();
    if (stub == nullptr) {
        HDF_LOGE("%{public}s: display layer stub create failed", __func__);
        return nullptr;
    }

    DispErrCode ret = stub->Init();
    if (ret != DISPLAY_SUCCESS) {
        delete stub;
        stub = nullptr;
        return nullptr;
    }
    return reinterpret_cast<void *>(stub);
}

void DestroyLayerStub(void *obj)
{
    delete reinterpret_cast<VideoLayerStub *>(obj);
    obj = nullptr;
}

int32_t LayerServiceOnRemoteRequest(void *stub, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (stub == nullptr) {
        HDF_LOGE("%{public}s:stub is nullptr", __func__);
        return HDF_FAILURE;
    }

    VideoLayerStub *layerStub = reinterpret_cast<VideoLayerStub *>(stub);
    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;

    if (SbufToParcel(reply, &replyParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:invalid reply sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::MessageOption option;
    return layerStub->LayerStubOnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}
