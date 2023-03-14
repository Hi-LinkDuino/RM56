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

#include "stream_operator_proxy.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <message_parcel.h>
#include "istream_operator_callback.h"
#include "ioffline_stream_operator.h"
#include "camera_metadata_info.h"
#include "utils_data_stub.h"
#include "metadata_utils.h"

namespace OHOS::Camera {
CamRetCode StreamOperatorProxy::IsStreamsSupported(
    OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
    const std::vector<std::shared_ptr<StreamInfo>> &pInfo,
    StreamSupportType &pType)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    // stub operation mode
    if (!data.WriteInt32(mode)) {
        HDF_LOGE("%s: write operation mode failed", __func__);
        return INVALID_ARGUMENT;
    }

    bool nullFlag = (modeSetting != nullptr);
    if (!data.WriteBool(nullFlag)) {
        HDF_LOGE("%s: write mode nullflag failed", __func__);
        return INVALID_ARGUMENT;
    }

    // stub metadata
    if (nullFlag && !CameraStandard::MetadataUtils::EncodeCameraMetadata(modeSetting, data)) {
        HDF_LOGE("%s: write metadata failed", __func__);
        return INVALID_ARGUMENT;
    }

    size_t count = pInfo.size();
    if (!data.WriteInt32(static_cast<int32_t>(count))) {
        HDF_LOGE("%s: write pInfo count failed", __func__);
        return INVALID_ARGUMENT;
    }

    bool bRet = true;
    for (size_t i = 0; i < count; i++) {
        std::shared_ptr<StreamInfo> streamInfo = pInfo.at(i);
        bRet = UtilsDataStub::EncodeStreamInfo(streamInfo, data);
        if (!bRet) {
            HDF_LOGE("%s: write streamInfo failed. index = %zu", __func__, i);
            return INVALID_ARGUMENT;
        }
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_IS_STREAMS_SUPPORTED, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    pType = static_cast<StreamSupportType>(reply.ReadInt32());

    return retCode;
}

CamRetCode StreamOperatorProxy::CreateStreams(
    const std::vector<std::shared_ptr<StreamInfo>> &streamInfos)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    size_t count = streamInfos.size();
    if (!data.WriteInt32(static_cast<int32_t>(count))) {
        HDF_LOGE("%s: write streamInfos count failed", __func__);
        return INVALID_ARGUMENT;
    }

    bool bRet = true;
    for (size_t i = 0; i < count; i++) {
        std::shared_ptr<StreamInfo> streamInfo = streamInfos.at(i);
        bRet = UtilsDataStub::EncodeStreamInfo(streamInfo, data);
        if (!bRet) {
            HDF_LOGE("%s: write streamInfo failed. index = %zu", __func__, i);
            return INVALID_ARGUMENT;
        }
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_CREATE_STREAMS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode StreamOperatorProxy::ReleaseStreams(const std::vector<int> &streamIds)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    std::vector<int32_t> pxyStreamIds = streamIds;
    if (!data.WriteInt32Vector(pxyStreamIds)) {
        HDF_LOGE("%s: write streamIds failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_RELEASE_STREAMS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode StreamOperatorProxy::CommitStreams(OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(mode)) {
        HDF_LOGE("%s: write operation mode failed", __func__);
        return INVALID_ARGUMENT;
    }

    bool bRet = CameraStandard::MetadataUtils::EncodeCameraMetadata(modeSetting, data);
    if (!bRet) {
        HDF_LOGE("%s: write metadata failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_COMMIT_STREAMS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode StreamOperatorProxy::GetStreamAttributes(
    std::vector<std::shared_ptr<StreamAttribute>> &attributes)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_GET_STREAM_ATTRIBUTES, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    int32_t retCode = reply.ReadInt32();
    int32_t count = reply.ReadInt32();
    for (int i = 0; i < count; i++) {
        const uint8_t *buffer = data.ReadBuffer(sizeof(StreamAttribute));
        std::shared_ptr<StreamAttribute> attribute =
        std::shared_ptr<StreamAttribute>(
        reinterpret_cast<StreamAttribute*>(
        const_cast<uint8_t *>(buffer)));
        attributes.push_back(attribute);
    }

    return static_cast<CamRetCode>(retCode);
}

CamRetCode StreamOperatorProxy::AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::IBufferProducer> &producer)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    if (producer == nullptr) {
        HDF_LOGE("%s: producer is NULL", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(static_cast<int32_t>(streamId))) {
        HDF_LOGE("%s: write streamId failed", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteRemoteObject(producer->AsObject())) {
        HDF_LOGE("%s: write buffer producer failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_ATTACH_BUFFER_QUEUE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode StreamOperatorProxy::DetachBufferQueue(int streamId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(static_cast<int32_t>(streamId))) {
        HDF_LOGE("%s: write streamId failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_DETACH_BUFFER_QUEUE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode StreamOperatorProxy::Capture(int captureId,
    const std::shared_ptr<CaptureInfo> &pInfo, bool isStreaming)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    if (pInfo == nullptr) {
        return INVALID_ARGUMENT;
    }

    // stub captureId
    if (!data.WriteInt32(static_cast<int32_t>(captureId))) {
        HDF_LOGE("%s: write captureId failed", __func__);
        return INVALID_ARGUMENT;
    }

    // stub CaptureInfo
    std::vector<int32_t> pxyStreamIds = pInfo->streamIds_;
    if (!data.WriteInt32Vector(pxyStreamIds)) {
        HDF_LOGE("%s: write streamIds failed", __func__);
        return INVALID_ARGUMENT;
    }

    bool bRet = CameraStandard::MetadataUtils::EncodeCameraMetadata(pInfo->captureSetting_, data);
    if (!bRet) {
        HDF_LOGE("%s: write metadata failed", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteBool(pInfo->enableShutterCallback_)) {
        HDF_LOGE("%s: write enableShutterCallback_ failed", __func__);
        return INVALID_ARGUMENT;
    }

    // stub isStreaming
    if (!data.WriteBool(isStreaming)) {
        HDF_LOGE("%s: write isStreaming failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_CAPTURE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode StreamOperatorProxy::CancelCapture(int captureId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(static_cast<int32_t>(captureId))) {
        HDF_LOGE("%s: write captureId failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_CANCEL_CAPTURE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode StreamOperatorProxy::ChangeToOfflineStream(
    const std::vector<int> &streamIds,
    OHOS::sptr<IStreamOperatorCallback> &callback,
    OHOS::sptr<IOfflineStreamOperator> &offlineOperator)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(StreamOperatorProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    if (callback == nullptr) {
        return INVALID_ARGUMENT;
    }

    std::vector<int32_t> pxyStreamIds = streamIds;
    if (!data.WriteInt32Vector(pxyStreamIds)) {
        HDF_LOGE("%s: write streamIds failed", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HDF_LOGE("%s: write offline stream operator callback failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_CHANGE_TO_OFFLINE_STREAM, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    sptr<IRemoteObject> remoteObj = reply.ReadRemoteObject();
    offlineOperator = OHOS::iface_cast<IOfflineStreamOperator>(remoteObj);

    return retCode;
}
}