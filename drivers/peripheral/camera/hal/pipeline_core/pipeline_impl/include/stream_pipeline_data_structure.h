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

#ifndef STREAM_PIPELINE_DATA_STRUCTURE_H
#define STREAM_PIPELINE_DATA_STRUCTURE_H

#include <vector>
#include <string>
#include <memory>
extern "C" {
#include "config.h"
#include "params.h"
#include "ibuffer.h"
#include "stream.h"
}
namespace OHOS::Camera {
struct PortInfo {
    std::string name_;
    std::string peerPortName_;
    std::string peerPortNodeName_;
};
using PortInfo = struct PortInfo;

struct PortFormat {
    int32_t w_;
    int32_t h_;
    uint32_t streamId_;
    int32_t format_;
    uint64_t usage_;
    uint8_t needAllocation_;
    uint32_t bufferCount_;
    int64_t bufferPoolId_;
};
using PortFormat = struct PortFormat;

struct PortSpec {
    uint8_t direction_;
    PortInfo info_;
    PortFormat format_;
};

struct NodeSpec {
    std::string name_;
    std::string status_;
    std::string type_;
    int streamId_;
    std::vector<PortSpec> portSpecSet_;
    bool operator==(const NodeSpec& n)
    {
        if (this->portSpecSet_.size() == n.portSpecSet_.size()) {
            return true;
        } else {
            return false;
        }
    }
    bool operator!=(const NodeSpec& n)
    {
        if (this->portSpecSet_.size() != n.portSpecSet_.size()) {
            return true;
        } else {
            return false;
        }
    }
};

struct PipelineSpec {
    std::vector<NodeSpec>   nodeSpecSet_;
};

#define G_STREAM_SCENE_TABLE HdfGetModuleConfigRoot()
#define G_STREAM_TABLE_PTR HdfGetModuleConfigRoot()->streamInfo
#define G_STREAM_TABLE_SIZE HdfGetModuleConfigRoot()->streamInfoSize
#define G_STREAM_INFO const struct HdfConfigStreamInfo*
#define G_SCENE_TABLE_PTR HdfGetModuleConfigRoot()->sceneInfo
#define G_SCENE_TABLE_SIZE HdfGetModuleConfigRoot()->sceneInfoSize
#define G_SCENE_INFO const struct HdfConfigSceneInfo*
#define G_PIPELINE_CONFIG_TABLE HdfGetPipelineSpecsModuleConfigRoot()
#define G_PIPELINE_SPECS_TABLE HdfGetPipelineSpecsModuleConfigRoot()->pipelineSpec
#define G_PIPELINE_SPECS_SIZE HdfGetPipelineSpecsModuleConfigRoot()->pipelineSpecSize
#define G_PIPELINE_SPEC_DATA_TYPE const struct HdfConfigPipelineSpecsPipelineSpec*
#define G_NODE_SPEC_DATA_TYPE const struct HdfConfigPipelineSpecsNodeSpec*
#define G_PORT_SPEC_DATA_TYPE const struct HdfConfigPipelineSpecsPORTSpec*

class INode;
struct Pipeline {
    std::vector<std::shared_ptr<INode>> nodes_;
};
using Pipeline = struct Pipeline;
}
#endif
