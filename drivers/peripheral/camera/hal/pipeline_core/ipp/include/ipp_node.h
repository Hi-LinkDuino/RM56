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

#ifndef HOS_CAMERA_IPP_NODE_H
#define HOS_CAMERA_IPP_NODE_H

#include "node_base.h"
#include "camera.h"
#include "offline_pipeline.h"
#include "algo_plugin_manager.h"
#include "algo_plugin.h"

namespace OHOS::Camera {
class IppNode : public NodeBase, public OfflinePipeline {
public:
    IppNode(const std::string& name, const std::string& type);
    ~IppNode();
    virtual RetCode Init(const int32_t streamId) override;
    virtual RetCode Start(const int32_t streamId) override;
    virtual RetCode Stop(const int32_t streamId) override;
    virtual RetCode Flush(const int32_t streamId) override;
    virtual RetCode Config(const int32_t streamId, const CaptureMeta& meta) override;
    virtual void DeliverBuffer(std::shared_ptr<IBuffer>& buffer) override;
    virtual void DeliverBuffers(std::vector<std::shared_ptr<IBuffer>>& buffers) override;
    virtual void ProcessCache(std::vector<std::shared_ptr<IBuffer>>& buffers) override;
    virtual void DeliverCache(std::vector<std::shared_ptr<IBuffer>>& buffers) override;
    virtual void DeliverCancelCache(std::vector<std::shared_ptr<IBuffer>>& buffers) override;

protected:
    RetCode GetOutputBuffer(std::vector<std::shared_ptr<IBuffer>>& buffers, std::shared_ptr<IBuffer>& outBuffer);
    void DeliverAlgoProductBuffer(std::shared_ptr<IBuffer>& result);
    void ClassifyOutputBuffer(std::shared_ptr<IBuffer>& outBuffer,
                              std::vector<std::shared_ptr<IBuffer>>& inBuffers,
                              std::shared_ptr<IBuffer>& product,
                              std::vector<std::shared_ptr<IBuffer>>& recycleBuffers);

protected:
    std::shared_ptr<AlgoPluginManager> algoPluginManager_ = nullptr;
    std::shared_ptr<AlgoPlugin> algoPlugin_ = nullptr;
};
} // namespace OHOS::Camera

#endif
