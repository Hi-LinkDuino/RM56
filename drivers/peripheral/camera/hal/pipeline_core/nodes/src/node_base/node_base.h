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

#ifndef NODE_BASE_H
#define NODE_BASE_H

#include <atomic>
#include "inode.h"
#include "buffer_manager.h"
#include "idevice_manager.h"

namespace OHOS::Camera {
class PortBase : public IPort {
public:
    PortBase(const std::string& name, const std::weak_ptr<INode>& n):
        name_(name),
        owner_(n)
    {
    }
    ~PortBase() override = default;
    std::string GetName() const override;
    RetCode SetFormat(const PortFormat& format) override;
    RetCode GetFormat(PortFormat& format) const override;
    int32_t GetStreamId() const override;
    RetCode Connect(const std::shared_ptr<IPort>& peer) override;
    RetCode DisConnect() override;
    int32_t Direction() const override;
    std::shared_ptr<INode> GetNode() const override;
    std::shared_ptr<IPort> Peer() const override;
    virtual void DeliverBuffer(std::shared_ptr<IBuffer>& buffer) override;
    virtual void DeliverBuffers(std::vector<std::shared_ptr<IBuffer>>& buffers) override;
    void DeliverBuffers(std::shared_ptr<FrameSpec> frameSpec) override;
    void DeliverBuffers(std::vector<std::shared_ptr<FrameSpec>> mergeVec) override;
protected:
    std::string name_;
    std::shared_ptr<IPort> peer_ = nullptr;
    std::weak_ptr<INode> owner_;
};

class NodeBase : public INode, public std::enable_shared_from_this<NodeBase> {
public:
    NodeBase(const std::string& name, const std::string& type)
        : name_(name)
        , type_(type)
    {
    }
    ~NodeBase() override
    {
        for (const auto& it : portVec_) {
            it->DisConnect();
        }
        portVec_.clear();
    }

    std::string GetName() const override;
    std::string GetType() const override;
    std::shared_ptr<IPort> GetPort(const std::string& name) override;
    RetCode Init(const int32_t streamId) override;
    RetCode Start(const int32_t streamId) override;
    RetCode Flush(const int32_t streamId) override;
    RetCode Stop(const int32_t streamId) override;
    RetCode Config(const int32_t streamId, const CaptureMeta& meta) override;
    RetCode Capture(const int32_t streamId, const int32_t captureId) override;
    RetCode CancelCapture(const int32_t streamId) override;
    int32_t GetNumberOfInPorts() const override;
    int32_t GetNumberOfOutPorts() const override;
    std::vector<std::shared_ptr<IPort>> GetInPorts() const override;
    std::vector<std::shared_ptr<IPort>> GetOutPorts() override;
    std::shared_ptr<IPort> GetOutPortById(const int32_t id) override;
    void SetCallBack(BufferCb c) override;
    void DeliverBuffer(std::shared_ptr<IBuffer>& buffer) override;
    void DeliverBuffers(std::vector<std::shared_ptr<IBuffer>>& buffers) override;

    virtual RetCode ProvideBuffers(std::shared_ptr<FrameSpec> frameSpec);
    void DeliverBuffers(std::shared_ptr<FrameSpec> frameSpec) override;
    void DeliverBuffers(std::vector<std::shared_ptr<FrameSpec>> mergeVec) override;

protected:
    std::string name_;
    std::string type_;
    std::vector<std::shared_ptr<IPort>> portVec_;
};
}
#endif

