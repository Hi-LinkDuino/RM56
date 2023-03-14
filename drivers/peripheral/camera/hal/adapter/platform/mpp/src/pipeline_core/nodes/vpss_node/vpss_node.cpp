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

#include "vpss_node.h"
#include <unistd.h>

namespace OHOS::Camera {
VpssNode::VpssNode(const std::string& name, const std::string& type)
    : NodeBase(name, type), MpiNode(name, type), SourceNode(name, type)
{
    CAMERA_LOGV("%{public}s enter, type(%{public}s)\n", name_.c_str(), type_.c_str());
}

VpssNode::~VpssNode()
{
    CAMERA_LOGV("%{public}s, vpss node dtor.", __FUNCTION__);
}

RetCode VpssNode::GetDeviceController()
{
    GetMpiDeviceManager();
    vpssController_ = std::static_pointer_cast<VpssController>
        ((std::static_pointer_cast<VpssManager>(deviceManager_->GetManager(DM_M_VPSS)))->GetController(DM_C_VPSS));
    if (vpssController_ == nullptr) {
        CAMERA_LOGE("get device controller failed");
        return RC_ERROR;
    }
    return RC_OK;
}

RetCode VpssNode::Init(const int32_t streamId)
{
    (void)streamId;
    return RC_OK;
}

RetCode VpssNode::Start(const int32_t streamId)
{
    RetCode rc = RC_OK;
    rc = GetDeviceController();
    if (rc == RC_ERROR) {
        CAMERA_LOGE("GetDeviceController failed.");
        return RC_ERROR;
    }
    rc = vpssController_->ConfigVpss();
    if (rc == RC_ERROR) {
        CAMERA_LOGE("configvpss failed.");
        return RC_ERROR;
    }
    rc = vpssController_->StartVpss();
    if (rc == RC_ERROR) {
        CAMERA_LOGE("startvpss failed.");
        return RC_ERROR;
    }
    CAMERA_LOGI("%{public}s, begin to connect", __FUNCTION__);
    rc = ConnectMpi(streamId);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("startvpss failed.");
        return RC_ERROR;
    }
    rc = SourceNode::Start(streamId);
    ids_[streamId] = -1;
    return rc;
}

RetCode VpssNode::Flush(const int32_t streamId)
{
    RetCode rc = RC_OK;
    rc = SourceNode::Flush(streamId);
    CHECK_IF_NOT_EQUAL_RETURN_VALUE(rc, RC_OK, RC_ERROR);

    rc = DisConnectMpi(streamId);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("DisConnectMpi failed!");
        return RC_ERROR;
    }
    rc = vpssController_->StopVpss();
    if (rc == RC_ERROR) {
        CAMERA_LOGE("stopvpss failed!");
        return RC_ERROR;
    }
    rc = IDeviceManager::GetInstance()->Flush(streamId);

    return rc;
}

RetCode VpssNode::Stop(const int32_t streamId)
{
    return SourceNode::Stop(streamId);
}

void VpssNode::SetBufferCallback()
{
    deviceManager_->SetNodeCallBack([&](std::shared_ptr<FrameSpec> frameSpec) {
            OnPackBuffer(frameSpec);
    });
    return;
}

RetCode VpssNode::ProvideBuffers(std::shared_ptr<FrameSpec> frameSpec)
{
    if (deviceManager_->SendFrameBuffer(frameSpec) == RC_OK) {
        return RC_OK;
    }
    CAMERA_LOGE("provide buffer failed.");
    return RC_ERROR;
}

RetCode VpssNode::Capture(const int32_t streamId, const int32_t captureId)
{
    RetCode ret = SourceNode::Capture(streamId, captureId);
    if (ret != RC_OK) {
        return ret;
    }

    if (captureId == ids_[streamId]) {
        return RC_OK;
    }

    ret = IDeviceManager::GetInstance()->StartRecvFrame(streamId);
    ids_[streamId] = captureId;
    return ret;
}

RetCode VpssNode::CancelCapture(const int32_t streamId)
{
    return IDeviceManager::GetInstance()->StopRecvFrame(streamId);
}


REGISTERNODE(VpssNode, {"vpss"})
} // namespace OHOS::Camera
