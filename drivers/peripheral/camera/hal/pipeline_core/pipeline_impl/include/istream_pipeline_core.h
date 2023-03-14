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

#ifndef ISTREAM_PIPELINE_CORE_H
#define ISTREAM_PIPELINE_CORE_H
#include <stdint.h>
#include <memory>
#include "host_stream_mgr.h"
#include "no_copyable.h"
#include "idevice_manager.h"
#include "offline_pipeline.h"
#include "camera_metadata_info.h"
#include "stream.h"

namespace OHOS::Camera {

struct NodeContext {
    std::shared_ptr<HostStreamMgr> streamMgr_ = nullptr;
};

class IStreamPipelineCore : public NoCopyable {
public:
    static std::shared_ptr<IStreamPipelineCore> Create(const std::shared_ptr<NodeContext>& c);
    virtual RetCode Init() = 0;
    virtual RetCode PreConfig(const ModeMeta& meta) = 0;
    virtual RetCode CreatePipeline(const int32_t& mode) = 0;
    virtual RetCode DestroyPipeline(const std::vector<int32_t>& ids) = 0;
    virtual RetCode Prepare(const std::vector<int32_t>& ids) = 0;
    virtual RetCode Start(const std::vector<int32_t>& ids) = 0;
    virtual RetCode Flush(const std::vector<int32_t>& ids) = 0;
    virtual RetCode Stop(const std::vector<int32_t>& ids) = 0;
    virtual RetCode Config(const std::vector<int32_t>& ids, const CaptureMeta& meta) = 0;
    virtual ~IStreamPipelineCore() = default;
    virtual std::shared_ptr<OfflinePipeline> GetOfflinePipeline(const int32_t id) = 0;
    virtual RetCode Capture(const std::vector<int32_t>& ids, const int32_t captureId) = 0;
    virtual RetCode CancelCapture(const std::vector<int32_t>& ids) = 0;
    virtual OperationMode GetCurrentMode() const = 0;
    virtual DynamicStreamSwitchMode CheckStreamsSupported(OperationMode mode,
        const ModeMeta& meta, const std::vector<StreamConfiguration>& configs) = 0;
};
}
#endif
