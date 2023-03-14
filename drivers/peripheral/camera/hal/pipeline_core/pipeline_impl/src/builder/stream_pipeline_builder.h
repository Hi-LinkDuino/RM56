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

#ifndef STREAM_PIPELINE_BUILDER_H
#define STREAM_PIPELINE_BUILDER_H
#include "stream_pipeline_data_structure.h"
#include "host_stream_mgr.h"
#include "config_parser.h"
#include "no_copyable.h"

namespace OHOS::Camera {
class StreamPipelineBuilder : public NoCopyable, ConfigParser {
public:
    std::shared_ptr<Pipeline> BuildOperation(const std::shared_ptr<PipelineSpec>& pipelineSpec);
    static std::unique_ptr<StreamPipelineBuilder> Create(const std::shared_ptr<HostStreamMgr>& streamMgr);
    virtual std::shared_ptr<Pipeline> Build(const std::shared_ptr<PipelineSpec>& pipelineSpec);
    virtual RetCode Destroy(int32_t streamType = -1);
    StreamPipelineBuilder(const std::shared_ptr<HostStreamMgr>& streamMgr, const std::shared_ptr<Pipeline>& p);
    virtual ~StreamPipelineBuilder() = default;
protected:
    std::shared_ptr<HostStreamMgr> hostStreamMgr_ = nullptr;
    std::shared_ptr<Pipeline> pipeline_ = nullptr;
};
}
#endif
