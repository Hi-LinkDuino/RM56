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

#include "node_base.h"

namespace OHOS::Camera {
std::string PortBase::GetName() const
{
    return name_;
}

RetCode PortBase::SetFormat(const PortFormat& format)
{
    format_ = format;
    return RC_OK;
}

RetCode PortBase::GetFormat(PortFormat& format) const
{
    format = format_;
    return RC_OK;
}

int32_t PortBase::GetStreamId() const
{
    PortFormat format = {};
    GetFormat(format);
    return format.streamId_;
}

RetCode PortBase::Connect(const std::shared_ptr<IPort>& peer)
{
    peer_ = peer;
    return RC_OK;
}

RetCode PortBase::DisConnect()
{
    peer_.reset();
    return RC_OK;
}

int32_t PortBase::Direction() const
{
    if (name_.empty()) {
        return 0;
    }

    if (name_[0] == 'i') {
        return 0;
    } else {
        return 1;
    }
}

std::shared_ptr<INode> PortBase::GetNode() const
{
    return owner_.lock();
}

std::shared_ptr<IPort> PortBase::Peer() const
{
    return peer_;
}

void PortBase::DeliverBuffer(std::shared_ptr<IBuffer>& buffer)
{
    auto peerPort = Peer();
    CHECK_IF_PTR_NULL_RETURN_VOID(peerPort);
    auto peerNode = peerPort->GetNode();
    CHECK_IF_PTR_NULL_RETURN_VOID(peerNode);
    peerNode->DeliverBuffer(buffer);

    return;
}

void PortBase::DeliverBuffers(std::vector<std::shared_ptr<IBuffer>>& buffers)
{
    auto peerPort = Peer();
    CHECK_IF_PTR_NULL_RETURN_VOID(peerPort);
    auto peerNode = peerPort->GetNode();
    CHECK_IF_PTR_NULL_RETURN_VOID(peerNode);
    peerNode->DeliverBuffers(buffers);

    return;
}

void PortBase::DeliverBuffers(std::shared_ptr<FrameSpec> frameSpec)
{
    (void)frameSpec;
    return;
}

void PortBase::DeliverBuffers(std::vector<std::shared_ptr<FrameSpec>> mergeVec)
{
    (void)mergeVec;
    return;
}

std::string NodeBase::GetName() const
{
    return name_;
}

std::string NodeBase::GetType() const
{
    return type_;
}

std::shared_ptr<IPort> NodeBase::GetPort(const std::string& name)
{
    auto it = std::find_if(portVec_.begin(), portVec_.end(),
                           [name](std::shared_ptr<IPort> p) { return p->GetName() == name; });
    if (it != portVec_.end()) {
        return *it;
    }
    std::shared_ptr<IPort> port = std::make_shared<PortBase>(name, shared_from_this());
    portVec_.push_back(port);
    return port;
}

RetCode NodeBase::Init(const int32_t streamId)
{
    (void)streamId;
    return RC_OK;
}

RetCode NodeBase::Start(const int32_t streamId)
{
    (void)streamId;
    CAMERA_LOGI("name:%{public}s start enter\n", name_.c_str());
    return RC_OK;
}

RetCode NodeBase::Flush(const int32_t streamId)
{
    (void)streamId;
    return RC_OK;
}

RetCode NodeBase::Stop(const int32_t streamId)
{
    (void)streamId;
    return RC_OK;
}

RetCode NodeBase::Config(const int32_t streamId, const CaptureMeta& meta)
{
    (void)streamId;
    (void)meta;
    return RC_OK;
}

int32_t NodeBase::GetNumberOfInPorts() const
{
    int32_t re = 0;
    for (const auto& it : portVec_) {
        if (it->Direction() == 0) {
            re++;
        }
    }
    return re;
}

int32_t NodeBase::GetNumberOfOutPorts() const
{
    int32_t re = 0;
    for (const auto& it : portVec_) {
        if (it->Direction() == 1) {
            re++;
        }
    }
    return re;
}

std::vector<std::shared_ptr<IPort>> NodeBase::GetInPorts() const
{
    std::vector<std::shared_ptr<IPort>> re;
    for (const auto& it : portVec_) {
        if (it->Direction() == 0) {
            re.push_back(it);
        }
    }
    return re;
}

std::vector<std::shared_ptr<IPort>> NodeBase::GetOutPorts()
{
    std::vector<std::shared_ptr<IPort>> out = {};
    for (const auto& it : portVec_) {
        if (it->Direction() == 1) {
            out.push_back(it);
        }
    }
    return out;
}

std::shared_ptr<IPort> NodeBase::GetOutPortById(const int32_t id)
{
    auto ports = GetOutPorts();
    if (ports.size() <= id) {
        return nullptr;
    }
    return ports[id];
}

void NodeBase::SetCallBack(BufferCb c)
{
    (void)c;
    return;
}

RetCode NodeBase::Capture(const int32_t streamId, const int32_t captureId)
{
    (void)streamId;
    (void)captureId;
    return RC_OK;
}

RetCode NodeBase::CancelCapture(const int32_t streamId)
{
    (void)streamId;
    return RC_OK;
}

void NodeBase::DeliverBuffer(std::shared_ptr<IBuffer>& buffer)
{
    auto outPorts = GetOutPorts();
    for (auto it : outPorts) {
        if (it->format_.bufferPoolId_ == buffer->GetPoolId()) {
            it->DeliverBuffer(buffer);
            return;
        }
    }
    return;
}

void NodeBase::DeliverBuffers(std::vector<std::shared_ptr<IBuffer>>& buffers)
{
    if (buffers.empty()) {
        return;
    }
    auto outPorts = GetOutPorts();
    for (auto it : outPorts) {
        if (it->format_.bufferPoolId_ == buffers[0]->GetPoolId()) {
            it->DeliverBuffers(buffers);
            return;
        }
    }
    return;
}

RetCode NodeBase::ProvideBuffers(std::shared_ptr<FrameSpec> frameSpec)
{
    (void)frameSpec;
    return RC_OK;
}

void NodeBase::DeliverBuffers(std::shared_ptr<FrameSpec> frameSpec)
{
    (void)frameSpec;
    return;
}

void NodeBase::DeliverBuffers(std::vector<std::shared_ptr<FrameSpec>> mergeVec)
{
    (void)mergeVec;
    return;
}
} // namespace OHOS::Camera