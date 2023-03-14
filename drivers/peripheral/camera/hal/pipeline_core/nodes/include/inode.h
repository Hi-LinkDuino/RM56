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

#ifndef I_NODE_H
#define I_NODE_H
#include <string>
#include <thread>
#include <list>
#include "stream_pipeline_data_structure.h"
#include "object_factory.h"
#include "no_copyable.h"
#include "ibuffer.h"
#include "ibuffer_pool.h"
#include "camera_metadata_info.h"
#include <functional>

namespace OHOS::Camera {
class INode;
class IPort : public NoCopyable {
public:
    virtual ~IPort() = default;
    virtual std::string GetName() const = 0;
    virtual RetCode SetFormat(const PortFormat& format) = 0;
    virtual RetCode GetFormat(PortFormat& format) const = 0;
    virtual int32_t GetStreamId() const = 0;
    virtual RetCode Connect(const std::shared_ptr<IPort>& peer) = 0;
    virtual RetCode DisConnect() = 0;
    virtual int32_t Direction() const = 0;
    virtual std::shared_ptr<INode> GetNode() const = 0;
    virtual std::shared_ptr<IPort> Peer() const = 0;
    virtual void DeliverBuffer(std::shared_ptr<IBuffer>& buffer) = 0;
    virtual void DeliverBuffers(std::vector<std::shared_ptr<IBuffer>>& buffers) = 0;
    virtual void DeliverBuffers(std::shared_ptr<FrameSpec> frameSpec) = 0;
    virtual void DeliverBuffers(std::vector<std::shared_ptr<FrameSpec>> mergeVec) = 0;
    PortFormat format_ {};
};

class INode : public NoCopyable {
public:
    using BufferCb = std::function<void(std::shared_ptr<IBuffer>)>;
    virtual ~INode() = default;
    virtual std::string GetName() const = 0;
    virtual std::string GetType() const = 0;
    virtual std::shared_ptr<IPort> GetPort(const std::string& name) = 0;
    virtual RetCode Init(const int32_t streamId) = 0;
    virtual RetCode Start(const int32_t streamId) = 0;
    virtual RetCode Flush(const int32_t streamId) = 0;
    virtual RetCode Stop(const int32_t streamId) = 0;
    virtual RetCode Config(const int32_t streamId, const CaptureMeta& meta) = 0;
    virtual RetCode Capture(const int32_t streamId, const int32_t captureId) = 0;
    virtual RetCode CancelCapture(const int32_t streamId) = 0;
    virtual int32_t GetNumberOfInPorts() const = 0;
    virtual int32_t GetNumberOfOutPorts() const = 0;
    virtual std::vector<std::shared_ptr<IPort>> GetInPorts() const = 0;
    virtual std::vector<std::shared_ptr<IPort>> GetOutPorts() = 0;
    virtual std::shared_ptr<IPort> GetOutPortById(const int32_t id) = 0;
    virtual void DeliverBuffer(std::shared_ptr<IBuffer>& buffer) = 0;
    virtual void DeliverBuffers(std::vector<std::shared_ptr<IBuffer>>& buffers) = 0;
    virtual void SetCallBack(BufferCb c) = 0;

    virtual RetCode ProvideBuffers(std::shared_ptr<FrameSpec> frameSpec) = 0;
    virtual void DeliverBuffers(std::shared_ptr<FrameSpec> frameSpec) = 0;
    virtual void DeliverBuffers(std::vector<std::shared_ptr<FrameSpec>> mergeVec) = 0;
};


using NodeFactory = RegisterFactoty<INode, const std::string&, const std::string&>;

#define REGISTERNODE(cls, ...) \
namespace { \
    static std::string g_##cls = NodeFactory::Instance().DoRegister<cls>(__VA_ARGS__, \
                [](const std::string& name, const std::string& type) {return std::make_shared<cls>(name, type);}); \
}
}
#endif
