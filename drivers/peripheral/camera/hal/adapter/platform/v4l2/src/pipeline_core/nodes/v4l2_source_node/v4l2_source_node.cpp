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

#include "v4l2_source_node.h"
#include <unistd.h>

namespace OHOS::Camera {
V4L2SourceNode::V4L2SourceNode(const std::string& name, const std::string& type)
    : SourceNode(name, type), NodeBase(name, type)
{
    CAMERA_LOGI("%s enter, type(%s)\n", name_.c_str(), type_.c_str());
}

RetCode V4L2SourceNode::GetDeviceController()
{
    CameraId cameraId = CAMERA_FIRST;
    sensorController_ = std::static_pointer_cast<SensorController>
        (deviceManager_->GetController(cameraId, DM_M_SENSOR, DM_C_SENSOR));
    if (sensorController_ == nullptr) {
        CAMERA_LOGE("get device controller failed");
        return RC_ERROR;
    }
    return RC_OK;
}

RetCode V4L2SourceNode::Init(const int32_t streamId)
{
    return RC_OK;
}

RetCode V4L2SourceNode::Start(const int32_t streamId)
{
    RetCode rc = RC_OK;
    deviceManager_ = IDeviceManager::GetInstance();
    if (deviceManager_ == nullptr) {
        CAMERA_LOGE("get device manager failed.");
        return RC_ERROR;
    }
    rc = GetDeviceController();
    if (rc == RC_ERROR) {
        CAMERA_LOGE("GetDeviceController failed.");
        return RC_ERROR;
    }
    std::vector<std::shared_ptr<IPort>> outPorts = GetOutPorts();
    for (auto& it : outPorts) {
        DeviceFormat format;
        format.fmtdesc.pixelformat = V4L2_PIX_FMT_YUV420;
        format.fmtdesc.width = it->format_.w_;
        format.fmtdesc.height = it->format_.h_;
        int bufCnt = it->format_.bufferCount_;
        rc = sensorController_->Start(bufCnt, format);
        if (rc == RC_ERROR) {
            CAMERA_LOGE("start failed.");
            return RC_ERROR;
        }
    }
    rc = SourceNode::Start(streamId);
    return RC_OK;
}

V4L2SourceNode::~V4L2SourceNode()
{
    CAMERA_LOGV("%{public}s, v4l2 source node dtor.", __FUNCTION__);
}

RetCode V4L2SourceNode::Flush(const int32_t streamId)
{
    RetCode rc = RC_OK;
    rc = SourceNode::Flush(streamId);
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(rc, RC_OK, RC_ERROR);

    rc = sensorController_->Flush(streamId);
    return rc;
}

RetCode V4L2SourceNode::Stop(const int32_t streamId)
{
    SourceNode::Stop(streamId);

    return sensorController_->Stop();
}

void V4L2SourceNode::SetBufferCallback()
{
    sensorController_->SetNodeCallBack([&](std::shared_ptr<FrameSpec> frameSpec) {
            OnPackBuffer(frameSpec);
    });
    return;
}

RetCode V4L2SourceNode::ProvideBuffers(std::shared_ptr<FrameSpec> frameSpec)
{
    CAMERA_LOGI("provide buffers enter.");
    if (sensorController_->SendFrameBuffer(frameSpec) == RC_OK) {
        CAMERA_LOGI("sendframebuffer success bufferpool id = %llu", frameSpec->bufferPoolId_);
        return RC_OK;
    }
    return RC_ERROR;
}
REGISTERNODE(V4L2SourceNode, {"v4l2_source"})
} // namespace OHOS::Camera
