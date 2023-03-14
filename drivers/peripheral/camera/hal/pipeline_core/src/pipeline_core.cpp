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

#include "pipeline_core.h"

namespace OHOS::Camera {
RetCode PipelineCore::Init()
{
    context_ = std::make_shared<NodeContext>();
    context_->streamMgr_ = HostStreamMgr::Create();
    spc_ = IStreamPipelineCore::Create(context_);
    return RC_OK;
}

void PipelineCore::UpdateMetadata(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    RetCode rc = RC_OK;
    std::shared_ptr<IDeviceManager> deviceManager = IDeviceManager::GetInstance();
    deviceManager->Configure(meta);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("update metadata failed.");
        return;
    }
    return;
}

std::shared_ptr<HostStreamMgr> PipelineCore::GetHostStreamMgr() const
{
    return context_->streamMgr_;
}

std::shared_ptr<IStreamPipelineCore> PipelineCore::GetStreamPipelineCore() const
{
    return spc_;
}

std::shared_ptr<IPipelineCore> IPipelineCore::Create()
{
    return std::make_shared<PipelineCore>();
}
}
