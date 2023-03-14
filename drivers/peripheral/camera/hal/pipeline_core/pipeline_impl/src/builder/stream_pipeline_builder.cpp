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

#include "inode.h"
#include "stream_pipeline_builder.h"

namespace OHOS::Camera {
StreamPipelineBuilder::StreamPipelineBuilder(const std::shared_ptr<HostStreamMgr>& streamMgr,
    const std::shared_ptr<Pipeline>& p) :
    hostStreamMgr_(streamMgr),
    pipeline_(p)
{
}
std::shared_ptr<Pipeline> StreamPipelineBuilder::Build(const std::shared_ptr<PipelineSpec>& pipelineSpec)
{
    if (pipelineSpec == nullptr) {
        CAMERA_LOGI("pipelineSpec nullptr~ \n");
        return nullptr;
    }
    CAMERA_LOGI("------------------------Node Instantiation Begin-------------\n");
    RetCode re = RC_OK;
    for (auto& it : pipelineSpec->nodeSpecSet_) {
        if (it.status_ == "new") {
            std::string nodeName;
            size_t pos = it.name_.find_first_of('#');
            nodeName = it.name_.substr(0, pos);
            std::shared_ptr<INode> newNode = NodeFactory::Instance().CreateShared(nodeName, it.name_, it.type_);
            if (newNode == nullptr) {
                CAMERA_LOGI("create node failed! \n");
                return nullptr;
            }
            std::optional<int32_t> typeId = GetTypeId(it.type_, G_STREAM_TABLE_PTR, G_STREAM_TABLE_SIZE);
            if (typeId) {
                newNode->SetCallBack(hostStreamMgr_->GetBufferCb(it.streamId_));
            }
            pipeline_->nodes_.push_back(newNode);
            it.status_ = "remain";
            for (const auto& portSpec : it.portSpecSet_) {
                auto peerNode = std::find_if(pipeline_->nodes_.begin(), pipeline_->nodes_.end(),
                    [portSpec](const std::shared_ptr<INode>& n) {
                        return n->GetName() == portSpec.info_.peerPortNodeName_;
                    });
                if (peerNode != pipeline_->nodes_.end()) {
                    std::shared_ptr<IPort> peerPort = (*peerNode)->GetPort(portSpec.info_.peerPortName_);
                    re = peerPort->SetFormat(portSpec.format_);
                    if (re != RC_OK) {
                        return nullptr;
                    }
                    std::shared_ptr<IPort> port = newNode->GetPort(portSpec.info_.name_);
                    re = port->SetFormat(portSpec.format_);
                    if (re != RC_OK) {
                        return nullptr;
                    }
                    re = port->Connect(peerPort);
                    if (re != RC_OK) {
                        return nullptr;
                    }
                    re = peerPort->Connect(port);
                    if (re != RC_OK) {
                        return nullptr;
                    }
                }
            }
        }
    }
    CAMERA_LOGI("------------------------Node Instantiation End-------------\n");
    return pipeline_;
}

RetCode StreamPipelineBuilder::Destroy(int streamId)
{
    CHECK_IF_PTR_NULL_RETURN_VALUE(pipeline_, RC_ERROR);
    (void)streamId;
    pipeline_->nodes_.clear();
    return RC_OK;
}

std::unique_ptr<StreamPipelineBuilder> StreamPipelineBuilder::Create(const std::shared_ptr<HostStreamMgr>& streamMgr)
{
    std::shared_ptr<Pipeline> p = std::make_shared<Pipeline>();
    return std::make_unique<StreamPipelineBuilder>(streamMgr, p);
}
}
