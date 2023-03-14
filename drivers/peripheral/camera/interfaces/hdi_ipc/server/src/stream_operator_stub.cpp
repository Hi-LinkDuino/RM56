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

#include "stream_operator_stub.h"
#include <hdf_log.h>
#include <hdf_base.h>
#include <hdf_sbuf_ipc.h>
#include "utils_data_stub.h"
#include "metadata_utils.h"
#include "istream_operator_callback.h"
#include "ioffline_stream_operator.h"
#include "cmd_common.h"

namespace OHOS::Camera {
int32_t StreamOperatorStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    HDF_LOGE("%s: CameraDeviceStub::OnRemoteRequest entry!", __func__);
    int32_t ret = HDF_SUCCESS;
    switch (code) {
        case CMD_STREAM_OPERATOR_IS_STREAMS_SUPPORTED: {
            ret = StreamOperatorStubIsStreamsSupported(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_CREATE_STREAMS: {
            ret = StreamOperatorStubCreateStreams(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_RELEASE_STREAMS: {
            ret = StreamOperatorStubReleaseStreams(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_COMMIT_STREAMS: {
            ret = StreamOperatorStubCommitStreams(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_GET_STREAM_ATTRIBUTES: {
            ret = StreamOperatorStubGetStreamAttributes(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_ATTACH_BUFFER_QUEUE: {
            ret = StreamOperatorStubAttachBufferQueue(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_DETACH_BUFFER_QUEUE: {
            ret = StreamOperatorStubDetachBufferQueue(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_CAPTURE: {
            ret = StreamOperatorStubCapture(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_CANCEL_CAPTURE: {
            ret = StreamOperatorStubCancelCapture(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_CHANGE_TO_OFFLINE_STREAM: {
            ret = StreamOperatorStubChangeToOfflineStream(data, reply, option);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return ret;
}

int32_t StreamOperatorStub::StreamOperatorStubIsStreamsSupported(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    OperationMode operationMode = static_cast<OperationMode>(data.ReadInt32());

    std::shared_ptr<CameraStandard::CameraMetadata> metadata = nullptr;
    bool nullFlag = data.ReadBool();
    if (nullFlag) {
        CameraStandard::MetadataUtils::DecodeCameraMetadata(data, metadata);
    }

    int32_t count = data.ReadInt32();
    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    for (int i = 0; i < count; i++) {
        std::shared_ptr<StreamInfo> streamInfo = std::make_shared<StreamInfo>();
        UtilsDataStub::DecodeStreamInfo(data, streamInfo);
        streamInfos.push_back(streamInfo);
    }

    StreamSupportType streamSupportType;
    CamRetCode ret = IsStreamsSupported(operationMode, metadata, streamInfos, streamSupportType);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    if (!reply.WriteInt32(static_cast<int32_t>(streamSupportType))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t StreamOperatorStub::StreamOperatorStubCreateStreams(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t count = data.ReadInt32();
    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    for (int i = 0; i < count; i++) {
        std::shared_ptr<StreamInfo> streamInfo = std::make_shared<StreamInfo>();
        UtilsDataStub::DecodeStreamInfo(data, streamInfo);
        streamInfos.push_back(streamInfo);
    }

    CamRetCode ret = CreateStreams(streamInfos);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t StreamOperatorStub::StreamOperatorStubReleaseStreams(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    std::vector<int32_t> streamIds;
    if (!data.ReadInt32Vector(&streamIds)) {
        HDF_LOGE("%s: read streamIds failed", __func__);
        return HDF_FAILURE;
    }

    CamRetCode ret = ReleaseStreams(streamIds);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t StreamOperatorStub::StreamOperatorStubCommitStreams(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    OperationMode mode = static_cast<OperationMode>(data.ReadInt32());

    std::shared_ptr<CameraStandard::CameraMetadata> metadata = nullptr;
    CameraStandard::MetadataUtils::DecodeCameraMetadata(data, metadata);

    CamRetCode ret = CommitStreams(mode, metadata);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t StreamOperatorStub::StreamOperatorStubGetStreamAttributes(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    std::vector<std::shared_ptr<StreamAttribute>> attributes;
    CamRetCode ret = GetStreamAttributes(attributes);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    size_t count = attributes.size();
    if (!reply.WriteInt32(static_cast<int32_t>(count))) {
        HDF_LOGE("%s: write attributes count failed", __func__);
        return HDF_FAILURE;
    }

    for (size_t i = 0; i < count; i++) {
        if (!reply.WriteBuffer((void*)(attributes[i].get()), sizeof(StreamAttribute))) {
            HDF_LOGE("%s: write attribute failed. index = %d", __func__, i);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

int32_t StreamOperatorStub::StreamOperatorStubAttachBufferQueue(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t streamId = data.ReadInt32();
    sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
    const sptr<OHOS::IBufferProducer> bufferProducer =
        OHOS::iface_cast<OHOS::IBufferProducer>(remoteObj);

    CamRetCode ret = AttachBufferQueue(streamId, bufferProducer);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t StreamOperatorStub::StreamOperatorStubDetachBufferQueue(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t streamId = data.ReadInt32();
    CamRetCode ret = DetachBufferQueue(streamId);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t StreamOperatorStub::StreamOperatorStubCapture(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    int captureId = static_cast<int>(data.ReadInt32());

    std::vector<int32_t> streamIds;
    if (!data.ReadInt32Vector(&streamIds)) {
        HDF_LOGE("%s: write streamIds failed", __func__);
        return HDF_FAILURE;
    }

    std::shared_ptr<CameraStandard::CameraMetadata> metadata = nullptr;
    CameraStandard::MetadataUtils::DecodeCameraMetadata(data, metadata);

    bool enableShutterCallback = data.ReadBool();
    std::shared_ptr<CaptureInfo> pInfo = std::make_shared<CaptureInfo>();
    pInfo->streamIds_ = streamIds;
    pInfo->captureSetting_ = metadata;
    pInfo->enableShutterCallback_ = enableShutterCallback;

    bool isStreaming = data.ReadBool();

    CamRetCode ret = Capture(captureId, pInfo, isStreaming);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t StreamOperatorStub::StreamOperatorStubCancelCapture(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t captureId = data.ReadInt32();
    CamRetCode ret = CancelCapture(captureId);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t StreamOperatorStub::StreamOperatorStubChangeToOfflineStream(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != StreamOperatorStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    std::vector<int32_t> streamIds;
    if (!data.ReadInt32Vector(&streamIds)) {
        HDF_LOGE("%s: read streamIds failed", __func__);
        return HDF_FAILURE;
    }

    sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
    sptr<IStreamOperatorCallback> spStreamOperatorCallback =
        OHOS::iface_cast<IStreamOperatorCallback>(remoteObj);
    if (spStreamOperatorCallback == nullptr) {
        HDF_LOGE("%s: read operator callback failed", __func__);
        return HDF_FAILURE;
    }

    OHOS::sptr<IOfflineStreamOperator> offlineOperator = nullptr;
    CamRetCode ret = ChangeToOfflineStream(streamIds, spStreamOperatorCallback, offlineOperator);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    if (offlineOperator == nullptr) {
        HDF_LOGE("%s, change to offline stream failed, offlineOperator is null.", __func__);
        return HDF_FAILURE;
    }

    if (!reply.WriteRemoteObject(offlineOperator->AsObject())) {
        HDF_LOGE("%s: write offline stream operator failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}
}
