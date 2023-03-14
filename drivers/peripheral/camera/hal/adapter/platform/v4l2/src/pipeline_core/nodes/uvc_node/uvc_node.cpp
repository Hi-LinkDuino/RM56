/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "uvc_node.h"
#include <unistd.h>

namespace OHOS::Camera {
UvcNode::UvcNode(const std::string& name, const std::string& type) : SourceNode(name, type), NodeBase(name, type)
{
    CAMERA_LOGI("%s enter, type(%s)\n", name_.c_str(), type_.c_str());
}

UvcNode::~UvcNode()
{
    CAMERA_LOGI("~uvc Node exit.");
}


RetCode UvcNode::GetDeviceController()
{
    CameraId cameraId = CAMERA_THIRD;
    sensorController_ = std::static_pointer_cast<SensorController>
        (deviceManager_->GetController(cameraId, DM_M_SENSOR, DM_C_SENSOR));
    if (sensorController_ == nullptr) {
        CAMERA_LOGE("get device controller failed");
        return RC_ERROR;
    }
    return RC_OK;
}

RetCode UvcNode::Init(const int32_t streamId)
{
    return RC_OK;
}

RetCode UvcNode::Flush(const int32_t streamId)
{
    RetCode rc = RC_OK;
    rc = SourceNode::Flush(streamId);
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(rc, RC_OK, RC_ERROR);

    rc = sensorController_->Flush(streamId);
    return rc;
}

RetCode UvcNode::StartCheck(int64_t &bufferPoolId)
{
    deviceManager_ = IDeviceManager::GetInstance();
    if (deviceManager_ == nullptr) {
        CAMERA_LOGE("get device manager failed.");
        return RC_ERROR;
    }
    if (GetDeviceController() == RC_ERROR) {
        CAMERA_LOGE("GetDeviceController failed.");
        return RC_ERROR;
    }

    BufferManager* manager = Camera::BufferManager::GetInstance();
    if (manager == nullptr) {
        CAMERA_LOGE("buffer manager is null");
        return RC_ERROR;
    }
    bufferPoolId = manager->GenerateBufferPoolId();
    if (bufferPoolId == 0) {
        CAMERA_LOGE("bufferpool id is 0");
        return RC_ERROR;
    }
    bufferPool_ = manager->GetBufferPool(bufferPoolId);
    if (bufferPool_ == nullptr) {
        CAMERA_LOGE("bufferpool is null ");
        return RC_ERROR;
    }
    GetOutPorts();
    return RC_OK;
}

RetCode UvcNode::Start(const int32_t streamId)
{
    int64_t bufferPoolId;
    if (StartCheck(bufferPoolId) == RC_ERROR) {
        return RC_ERROR;
    }
    std::vector<std::shared_ptr<IPort>> outPorts = GetOutPorts();
    for (auto& iter : outPorts) {
        RetCode ret = bufferPool_->Init(iter->format_.w_,
            iter->format_.h_,
            iter->format_.usage_,
            iter->format_.format_,
            iter->format_.bufferCount_,
            CAMERA_BUFFER_SOURCE_TYPE_HEAP);
        if (ret == RC_ERROR) {
            CAMERA_LOGE("bufferpool init failed");
            break;
        }
        iter->format_.bufferPoolId_ = bufferPoolId;

        DeviceFormat format;
        format.fmtdesc.pixelformat = V4L2_PIX_FMT_YUYV;
        format.fmtdesc.width = iter->format_.w_;
        format.fmtdesc.height = iter->format_.h_;
        int bufCnt = iter->format_.bufferCount_;
        ret = sensorController_->Start(bufCnt, format);
        if (ret == RC_ERROR) {
            CAMERA_LOGE("start failed.");
            return RC_ERROR;
        }
    }
    return SourceNode::Start(streamId);
}

RetCode UvcNode::Stop(const int32_t streamId)
{
    return SourceNode::Stop(streamId);
}

void UvcNode::SetBufferCallback()
{
    sensorController_->SetNodeCallBack([&](std::shared_ptr<FrameSpec> frameSpec) {
            OnPackBuffer(frameSpec);
            });
    return;
}

RetCode UvcNode::ProvideBuffers(std::shared_ptr<FrameSpec> frameSpec)
{
    CAMERA_LOGI("provide buffers enter.");
    if (sensorController_->SendFrameBuffer(frameSpec) == RC_OK) {
        CAMERA_LOGD("sendframebuffer success bufferpool id = %llu", frameSpec->bufferPoolId_);
        return RC_OK;
    }
    return RC_ERROR;
}
REGISTERNODE(UvcNode, {"uvc"})
} // namespace OHOS::Camera
