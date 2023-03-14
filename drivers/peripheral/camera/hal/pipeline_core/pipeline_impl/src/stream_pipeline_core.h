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

#ifndef STREAM_PIPELINE_CORE_H
#define STREAM_PIPELINE_CORE_H

#include "camera_metadata_info.h"
#include "istream_pipeline_core.h"
#include "stream_pipeline_builder.h"
#include "stream_pipeline_dispatcher.h"
#include "stream_pipeline_strategy.h"
#include <mutex>

namespace OHOS::Camera {
class StreamPipelineCore : public IStreamPipelineCore {
public:
    StreamPipelineCore(const std::shared_ptr<NodeContext>& c) : context_(c) {}
    virtual ~StreamPipelineCore() override = default;
    virtual RetCode Init() override;
    virtual RetCode PreConfig(const ModeMeta& meta) override;
    virtual RetCode CreatePipeline(const int32_t& mode) override;
    virtual RetCode DestroyPipeline(const std::vector<int32_t>& ids) override;
    virtual RetCode Prepare(const std::vector<int32_t>& ids) override;
    virtual RetCode Start(const std::vector<int32_t>& ids) override;
    virtual RetCode Flush(const std::vector<int32_t>& ids) override;
    virtual RetCode Stop(const std::vector<int32_t>& ids) override;
    virtual RetCode Config(const std::vector<int32_t>& ids, const CaptureMeta& meta) override;
    virtual RetCode Capture(const std::vector<int32_t>& ids, const int32_t captureId) override;
    virtual RetCode CancelCapture(const std::vector<int>& streamIds) override;
    virtual std::shared_ptr<OfflinePipeline> GetOfflinePipeline(const int32_t id) override;
    virtual OperationMode GetCurrentMode() const override;
    virtual DynamicStreamSwitchMode CheckStreamsSupported(OperationMode mode,
        const ModeMeta& meta, const std::vector<StreamConfiguration>& configs) override;

protected:
    std::mutex mutex_;
    OperationMode mode_ = NORMAL;
    std::shared_ptr<NodeContext> context_ = nullptr;
    std::unique_ptr<StreamPipelineStrategy> strategy_ = nullptr;
    std::unique_ptr<StreamPipelineBuilder> builder_ = nullptr;
    std::unique_ptr<StreamPipelineDispatcher> dispatcher_ = nullptr;
};
} // namespace OHOS::Camera
#endif
