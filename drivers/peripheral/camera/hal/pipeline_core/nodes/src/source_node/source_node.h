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

#ifndef HOS_CAMERA_SOURCE_NODE_H
#define HOS_CAMERA_SOURCE_NODE_H

#include "camera.h"
#include "node_base.h"
#include "utils.h"
#include <vector>

namespace OHOS::Camera {
class SourceNode : virtual public NodeBase {
public:
    SourceNode(const std::string& name, const std::string& type);
    ~SourceNode() override;
    virtual RetCode Init(const int32_t streamId) override;
    virtual RetCode Start(const int32_t streamId) override;
    virtual RetCode Flush(const int32_t streamId) override;
    virtual RetCode Stop(const int32_t streamId) override;
    virtual RetCode Capture(const int32_t streamId, const int32_t captureId) override;
    virtual RetCode CancelCapture(const int32_t streamId) override;
    virtual RetCode Config(const int32_t streamId, const CaptureMeta& meta) override;
    virtual void DeliverBuffer(std::shared_ptr<IBuffer>& buffer) override;
    virtual RetCode ProvideBuffers(std::shared_ptr<FrameSpec> frameSpec) override;

    virtual void OnPackBuffer(std::shared_ptr<FrameSpec> frameSpec);
    virtual void SetBufferCallback();

protected:
    class PortHandler {
    public:
        PortHandler() = default;
        virtual ~PortHandler() = default;
        PortHandler(std::shared_ptr<IPort>& p);
        RetCode StartCollectBuffers();
        RetCode StopCollectBuffers();
        RetCode StartDistributeBuffers();
        RetCode StopDistributeBuffers();
        void OnBuffer(std::shared_ptr<IBuffer>& buffer);

    private:
        void CollectBuffers();
        void DistributeBuffers();
        void FlushBuffers();

    private:
        std::shared_ptr<IPort> port = nullptr;

        bool cltRun = false;
        std::unique_ptr<std::thread> collector = nullptr;

        bool dbtRun = false;
        std::unique_ptr<std::thread> distributor = nullptr;

        std::shared_ptr<IBufferPool> pool = nullptr;

        std::condition_variable rbcv;
        std::mutex rblock;
        std::list<std::shared_ptr<IBuffer>> respondBufferList = {};
    };

    std::mutex hndl_ = {};
    std::unordered_map<int32_t, std::shared_ptr<PortHandler>> handler_ = {};

    std::mutex requestLock_;
    std::unordered_map<int32_t, std::list<int32_t>> captureRequests_ = {};
};
} // namespace OHOS::Camera
#endif
