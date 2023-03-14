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

#include "mpi_node.h"
namespace OHOS::Camera {
MpiNode::MpiNode(const std::string& name, const std::string& type)
    : NodeBase(name, type)
{
    CAMERA_LOGV("%{public}s enter, type(%{public}s)\n", name_.c_str(), type_.c_str());
}

RetCode MpiNode::GetMpiDeviceManager()
{
    deviceManager_ = IDeviceManager::GetInstance();
    if (deviceManager_ == nullptr) {
        CAMERA_LOGE("get device manager failed.");
        return RC_ERROR;
    }
    return RC_OK;
}


RetCode MpiNode::ConnectMpi(const int32_t streamId)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(deviceManager_, RC_ERROR);
    std::vector<std::shared_ptr<IPort>> ports = GetOutPorts();
    if (ports.empty()) {
        return RC_OK;
    }

    std::shared_ptr<IPort> port = nullptr;
    for (auto& it : ports) {
        PortFormat f;
        it->GetFormat(f);
        if (f.streamId_ == streamId) {
            port = it;
            break;
        }
    }
    if (port == nullptr) {
        CAMERA_LOGV("can't find port for stream id:%{public}d, no need to connect", streamId);
        return RC_OK;
    }

    auto peerPort = port->Peer();
    CHECK_IF_PTR_NULL_RETURN_VALUE(peerPort, RC_ERROR);
    auto peerNode = peerPort->GetNode();
    CHECK_IF_PTR_NULL_RETURN_VALUE(peerNode, RC_ERROR);

    CAMERA_LOGI("mpp try to connect %{public}s to %{public}s.",
        port->GetNode()->GetName().c_str(), peerNode->GetName().c_str());
    RetCode rc = deviceManager_->Connect(name_, port->GetName(), peerNode->GetName(), peerPort->GetName());
    if (rc == RC_ERROR) {
        CAMERA_LOGE("failed to connect.");
        return rc;
    }

    return rc;
}

RetCode MpiNode::DisConnectMpi(const int32_t streamId)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(deviceManager_, RC_ERROR);
    std::vector<std::shared_ptr<IPort>> ports = GetOutPorts();
    if (ports.empty()) {
        return RC_OK;
    }

    std::shared_ptr<IPort> port = nullptr;
    for (auto& it : ports) {
        PortFormat f;
        it->GetFormat(f);
        if (f.streamId_ == streamId) {
            port = it;
            break;
        }
    }
    if (port == nullptr) {
        CAMERA_LOGE("can't find port for stream id:%{public}d, no need to disconnect", streamId);
        return RC_OK;
    }

    auto peerPort = port->Peer();
    CHECK_IF_PTR_NULL_RETURN_VALUE(peerPort, RC_ERROR);
    auto peerNode = peerPort->GetNode();
    CHECK_IF_PTR_NULL_RETURN_VALUE(peerNode, RC_ERROR);

    CAMERA_LOGI("mpp try to disconnect %{public}s and %{public}s.",
        port->GetNode()->GetName().c_str(), peerNode->GetName().c_str());
    RetCode rc = deviceManager_->UnConnect(name_, port->GetName(), peerNode->GetName(), peerPort->GetName());
    if (rc == RC_ERROR) {
        CAMERA_LOGE("failed to disconnect.");
        return rc;
    }

    return rc;
}

MpiNode::~MpiNode()
{
    CAMERA_LOGV("%{public}s, mpi node dtor.", __FUNCTION__);
}
} // namespace OHOS::Camera
