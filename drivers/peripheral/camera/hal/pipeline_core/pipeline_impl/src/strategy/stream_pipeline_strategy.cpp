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

#include "buffer_manager.h"
#include "stream_pipeline_strategy.h"

namespace OHOS::Camera {
StreamPipelineStrategy::StreamPipelineStrategy(const std::shared_ptr<HostStreamMgr>& streamMgr,
    const std::shared_ptr<PipelineSpec>& spec) :
    hostStreamMgr_(streamMgr),
    pipelineSpec_(spec)
{
}

std::shared_ptr<PipelineSpec> StreamPipelineStrategy::GeneratePipelineSpec(const int32_t& mode)
{
    PipelineSpec pipe {};
    if (SelectPipelineSpec(mode, pipe) != RC_OK) {
        return nullptr;
    }
    if (CombineSpecs(pipe) != RC_OK) {
        return nullptr;
    }
    return pipelineSpec_;
}

std::string StreamPipelineStrategy::ConstructKeyStrIndex(const int32_t& mode)
{
    std::string keyStr;
    std::string sceneStr= CheckIdExsit(mode, G_SCENE_TABLE_PTR, G_SCENE_TABLE_SIZE);
    if (sceneStr.empty()) {
        CAMERA_LOGE("scene:%{public}d not supported!\n", mode);
        return keyStr;
    }
    keyStr += sceneStr;
    std::vector<int32_t> streamTypeSet;
    hostStreamMgr_->GetStreamTypes(streamTypeSet);
    for (const auto& it : streamTypeSet) {
        std::string streamStr = CheckIdExsit(it, G_STREAM_TABLE_PTR, G_STREAM_TABLE_SIZE);
        if (streamStr.empty()) {
            CAMERA_LOGI("stream type:%{public}d not support!\n", it);
        }
        keyStr += "_" + streamStr;
    }
    return keyStr;
}

RetCode StreamPipelineStrategy::SetUpBasicOutPortFormat(const PipelineSpec& pipe,
    const PortInfo& info, PortFormat& format)
{
    auto peerNode = std::find_if(pipe.nodeSpecSet_.begin(), pipe.nodeSpecSet_.end(),
        [info](const NodeSpec& n) {
            return info.peerPortNodeName_ == n.name_;
        });
    if (peerNode == pipe.nodeSpecSet_.end()) {
        CAMERA_LOGI("config fail! node name:%{public}s not next node", info.peerPortNodeName_.c_str());
        return RC_ERROR;
    }
    auto peerPort = std::find_if(peerNode->portSpecSet_.begin(), peerNode->portSpecSet_.end(),
        [info] (const PortSpec& p) {
            return info.peerPortName_ == p.info_.name_;
        });
    if (peerPort == peerNode->portSpecSet_.end()) {
        CAMERA_LOGI("config fail! port name:%{public}s not exsit", info.name_.c_str());
        return RC_ERROR;
    }

    format = peerPort->format_;
    return RC_OK;
}

RetCode StreamPipelineStrategy::SetUpBasicInPortFormat(const NodeSpec& nodeSpec, PortFormat& format)
{
    auto outPort = std::find_if(nodeSpec.portSpecSet_.begin(), nodeSpec.portSpecSet_.end(),
        [] (const PortSpec& p) {
            return p.info_.name_ == "out0";
        });
    if (outPort == nodeSpec.portSpecSet_.end()) {
        CAMERA_LOGI("config fail! node name:%{public}s  out0 not exsit", nodeSpec.name_.c_str());
        return RC_ERROR;
    }
    format = (*outPort).format_;
    return RC_OK;
}
PortFormat StreamPipelineStrategy::SetPortFormat(G_PIPELINE_SPEC_DATA_TYPE &pipeSpecPtr,
                                                 std::optional<int32_t> typeId,
                                                 int j,
                                                 int k,
                                                 HostStreamInfo hostStreamInfo)
{
    struct PortFormat f {
        .w_ = hostStreamInfo.width_,
        .h_ = hostStreamInfo.height_,
        .streamId_ = hostStreamInfo.streamId_,
        .format_ = hostStreamInfo.format_,
        .usage_ = hostStreamInfo.usage_,
        .needAllocation_ = pipeSpecPtr->nodeSpec[j].portSpec[k].need_allocation,
        .bufferCount_ = hostStreamInfo.bufferCount_
    };
    (void)typeId;
    CAMERA_LOGI("buffercount = %{public}d", f.bufferCount_);
    return f;
}

void StreamPipelineStrategy::InitPipeSpecPtr(G_PIPELINE_SPEC_DATA_TYPE &pipeSpecPtr, std::string keyStr)
{
    for (int i = 0; i < G_PIPELINE_SPECS_SIZE; i++) {
        if (G_PIPELINE_SPECS_TABLE[i].name == keyStr) {
            pipeSpecPtr = &G_PIPELINE_SPECS_TABLE[i];
            break;
        }
    }
}

RetCode StreamPipelineStrategy::SelectPipelineSpec(const int32_t& mode, PipelineSpec& pipe)
{
    std::string keyStr = ConstructKeyStrIndex(mode);
    G_PIPELINE_SPEC_DATA_TYPE pipeSpecPtr = nullptr;
    InitPipeSpecPtr(pipeSpecPtr, keyStr);
    if (pipeSpecPtr == nullptr) {
        CAMERA_LOGE("target pipeline spec:%{public}s not exsit!i\n ", keyStr.c_str());
        return RC_ERROR;
    }

    for (int j = pipeSpecPtr->nodeSpecSize - 1; j >= 0; j--) {
        struct NodeSpec nodeSpec {
            .name_ = pipeSpecPtr->nodeSpec[j].name,
            .status_ = std::string(pipeSpecPtr->nodeSpec[j].status),
            .type_ = std::string(pipeSpecPtr->nodeSpec[j].stream_type)
        };
        for (int k = pipeSpecPtr->nodeSpec[j].portSpecSize - 1; k >= 0; k--) {
            struct PortInfo info {
                .name_ = std::string(pipeSpecPtr->nodeSpec[j].portSpec[k].name),
                .peerPortName_ = std::string(pipeSpecPtr->nodeSpec[j].portSpec[k].peer_port_name),
                .peerPortNodeName_ = std::string(pipeSpecPtr->nodeSpec[j].portSpec[k].peer_port_node_name)
            };
            CAMERA_LOGV("read node %{public}s", info.peerPortNodeName_.c_str());

            std::optional<int32_t> typeId = GetTypeId(std::string(pipeSpecPtr->nodeSpec[j].stream_type),
                G_STREAM_TABLE_PTR, G_STREAM_TABLE_SIZE);
            struct PortFormat format {};
            int32_t streamId = -1;
            if (typeId) {
                streamId = hostStreamMgr_->DesignateStreamIdForType(typeId.value());
                HostStreamInfo hostStreamInfo = hostStreamMgr_->GetStreamInfo(streamId);
                PortFormat f = SetPortFormat(pipeSpecPtr, typeId, j, k, hostStreamInfo);
                if (!f.needAllocation_) {
                    f.bufferPoolId_ =
                        static_cast<int64_t>(hostStreamInfo.bufferPoolId_);
                }
                format = f;
                nodeSpec.streamId_ = hostStreamInfo.streamId_;
            } else if (pipeSpecPtr->nodeSpec[j].portSpec[k].direction == 1) {
                if (SetUpBasicOutPortFormat(pipe, info, format) != RC_OK) {
                    return RC_ERROR;
                }
            } else {
                if (SetUpBasicInPortFormat(nodeSpec, format) != RC_OK) {
                    return RC_ERROR;
                }
            }
            struct PortSpec portSpec {
                .direction_ = pipeSpecPtr->nodeSpec[j].portSpec[k].direction,
                .info_ = info,
                .format_ = format,
            };
            nodeSpec.portSpecSet_.push_back(portSpec);
        }
        pipe.nodeSpecSet_.push_back(nodeSpec);
    }
    return RC_OK;
}

void StreamPipelineStrategy::PrintConnection(const NodeSpec& n)
{
    if (n.portSpecSet_.size() == 1 && n.portSpecSet_[0].direction_ == 0) {
        return;
    }

    for (const auto& it : n.portSpecSet_) {
        if (it.direction_ == 1) {
            CAMERA_LOGI("(%{public}s)(name:%{public}s w:%{public}d h:%{public}d format:%{public}d usage:%{public}llu \
                        bufferpoolid:%{public}llu) connect to (%{public}s)(name:%{public}s) \n",
                        n.name_.c_str(), it.info_.name_.c_str(), it.format_.w_, it.format_.h_, it.format_.format_,
                        it.format_.usage_, it.format_.bufferPoolId_,
                        it.info_.peerPortNodeName_.c_str(), it.info_.peerPortName_.c_str());
            auto nextNode = std::find_if(pipelineSpec_->nodeSpecSet_.begin(),
                pipelineSpec_->nodeSpecSet_.end(),
                [it](const NodeSpec& n) {
                    return n.name_ == it.info_.peerPortNodeName_;
                });
            if (nextNode != pipelineSpec_->nodeSpecSet_.end()) {
                PrintConnection(*nextNode);
            }
        }
    }
}

RetCode StreamPipelineStrategy::CombineSpecs(PipelineSpec& pipe)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(pipelineSpec_, RC_ERROR);
    CAMERA_LOGI("pipe.size = %{public}d,nodeSpecSet_.size = %{public}d", pipe.nodeSpecSet_.size(),
        pipelineSpec_->nodeSpecSet_.size());
    for (auto it = pipe.nodeSpecSet_.rbegin(); it != pipe.nodeSpecSet_.rend(); it++) {
        auto node = std::find_if(pipelineSpec_->nodeSpecSet_.begin(), pipelineSpec_->nodeSpecSet_.end(),
            [it](const NodeSpec& n) {
                return n.name_ == (*it).name_;
            });
        if (node == pipelineSpec_->nodeSpecSet_.end()) {
            CAMERA_LOGI("add node:%{public}s\n", (*it).name_.c_str());
            pipelineSpec_->nodeSpecSet_.push_back(*it);
        } else {
            if (*node != *it) {
                CAMERA_LOGI("change node:%{public}s\n", (*it).name_.c_str());
                std::swap((*node).portSpecSet_, (*it).portSpecSet_);
            }
        }
    }
    CAMERA_LOGI("------------------------Connection Dump Begin-------------\n");
    PrintConnection(pipelineSpec_->nodeSpecSet_[0]);
    CAMERA_LOGI("------------------------Connection Dump End-------------\n");
    return RC_OK;
}

RetCode StreamPipelineStrategy::Destroy(const int& streamId)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(pipelineSpec_, RC_ERROR);
    (void)streamId;
    pipelineSpec_->nodeSpecSet_.clear();
    return RC_OK;
}

std::unique_ptr<StreamPipelineStrategy> StreamPipelineStrategy::Create(const std::shared_ptr<HostStreamMgr>& streamMgr)
{
    std::shared_ptr<PipelineSpec> p = std::make_shared<PipelineSpec>();
    return std::make_unique<StreamPipelineStrategy>(streamMgr, p);
}

RetCode StreamPipelineStrategy::CheckPipelineSpecExist(const int32_t mode, const std::vector<int32_t>& types)
{
    std::string sceneStr= CheckIdExsit(mode, G_SCENE_TABLE_PTR, G_SCENE_TABLE_SIZE);
    if (sceneStr.empty()) {
        return RC_ERROR;
    }

    std::string keyStr = "";
    keyStr += sceneStr;
    for (const auto it : types) {
        std::string streamStr = CheckIdExsit(it, G_STREAM_TABLE_PTR, G_STREAM_TABLE_SIZE);
        if (streamStr.empty()) {
            return RC_ERROR;
        }
        keyStr += "_" + streamStr;
    }

    for (int i = 0; i < G_PIPELINE_SPECS_SIZE; i++) {
        if (G_PIPELINE_SPECS_TABLE[i].name == keyStr) {
            return RC_OK;
        }
    }

    return RC_ERROR;
}
}
