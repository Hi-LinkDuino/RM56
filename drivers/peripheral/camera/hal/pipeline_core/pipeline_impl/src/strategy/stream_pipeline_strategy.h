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

#ifndef STREAM_PIPELINE_STRATEGY_H
#define STREAM_PIPELINE_STRATEGY_H

#include "stream_pipeline_data_structure.h"
#include "host_stream_mgr.h"
#include "object_factory.h"
#include "config_parser.h"
#include "no_copyable.h"

namespace OHOS::Camera {
class StreamPipelineStrategy : public NoCopyable, private ConfigParser {
public:
    static std::unique_ptr<StreamPipelineStrategy> Create(const std::shared_ptr<HostStreamMgr>& streamMgr);
    virtual std::shared_ptr<PipelineSpec> GeneratePipelineSpec(const int32_t& mode);
    virtual RetCode Destroy(const int32_t& streamType = -1);
    StreamPipelineStrategy(const std::shared_ptr<HostStreamMgr>& streamMgr,
                const std::shared_ptr<PipelineSpec>& spec);
    virtual ~StreamPipelineStrategy() = default;
    virtual RetCode CheckPipelineSpecExist(const int32_t mode, const std::vector<int32_t>& types);
    PortFormat SetPortFormat(G_PIPELINE_SPEC_DATA_TYPE &pipeSpecPtr, std::optional<int32_t> typeId, int j, int k, HostStreamInfo hostStreamInfo);
    void InitPipeSpecPtr(G_PIPELINE_SPEC_DATA_TYPE &pipeSpecPtr, std::string keyStr);

protected:
    virtual std::string ConstructKeyStrIndex(const int32_t& mode);
    virtual RetCode SetUpBasicOutPortFormat(const PipelineSpec& pipe, const PortInfo& info, PortFormat& format);
    virtual RetCode SetUpBasicInPortFormat(const NodeSpec& nodeSpec, PortFormat& format);
    virtual RetCode SelectPipelineSpec(const int32_t& mode, PipelineSpec& pipe);
    void PrintConnection(const NodeSpec& n);
    RetCode CombineSpecs(PipelineSpec& pipe);

protected:
    std::shared_ptr<HostStreamMgr> hostStreamMgr_ = nullptr;
    std::shared_ptr<PipelineSpec> pipelineSpec_ = nullptr;
};
}
#endif
