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

#include "offline_pipeline_manager.h"

namespace OHOS::Camera {
OfflinePipelineManager& OfflinePipelineManager::GetInstance()
{
    static OfflinePipelineManager manager;
    return manager;
}

OfflinePipelineManager::~OfflinePipelineManager()
{
    CAMERA_LOGV("dtor");
    offlinePipelineList_.clear();
}

RetCode OfflinePipelineManager::SwitchToOfflinePipeline(int32_t streamId,
                                                        int32_t streamType,
                                                        const std::shared_ptr<IStreamPipelineCore>& pipeline,
                                                        std::function<void(std::shared_ptr<IBuffer>&)> callback)
{
    (void)streamType;
    auto op = GetOfflinePipeline(streamId, pipeline);
    if (op == nullptr) {
        CAMERA_LOGW("can't get offlinepipeline, abort switch.");
        return RC_ERROR;
    }

    op->BindOfflineStreamCallback(callback);
    op->SwitchToOfflineMode();

    {
        std::lock_guard<std::mutex> l(lock_);
        offlinePipelineList_.emplace_back(std::make_pair(streamId, op));
    }

    return RC_OK;
}

RetCode OfflinePipelineManager::CancelCapture(int32_t streamId, int32_t captureId)
{
    auto op = FindOfflinePipeline(streamId);
    if (op == nullptr) {
        return RC_ERROR;
    }

    return op->CancelCapture(captureId);
}

RetCode OfflinePipelineManager::DestoryOfflinePipeline(int32_t id)
{
    std::shared_ptr<OfflinePipeline> op = FindOfflinePipeline(id);
    if (op == nullptr) {
        CAMERA_LOGE("cannot release stream %{public}d", id);
        return RC_ERROR;
    }

    op->FlushOfflineStream();

    {
        std::lock_guard<std::mutex> l(lock_);
        auto it = std::find_if(offlinePipelineList_.begin(), offlinePipelineList_.end(),
            [&id](const std::pair<int32_t, std::shared_ptr<OfflinePipeline>>& stream) {
                return id == stream.first;
            });
        offlinePipelineList_.erase(it);
    }

    return RC_OK;
}

RetCode OfflinePipelineManager::DestoryOfflinePipelines()
{
    std::lock_guard<std::mutex> l(lock_);
    if (offlinePipelineList_.empty()) {
        return RC_OK;
    }

    for (auto it : offlinePipelineList_) {
        it.second->FlushOfflineStream();
    }

    offlinePipelineList_.clear();

    return RC_OK;
}

std::shared_ptr<OfflinePipeline>
    OfflinePipelineManager::GetOfflinePipeline(int32_t streamId, const std::shared_ptr<IStreamPipelineCore>& pipeline)
{
    if (pipeline == nullptr) {
        CAMERA_LOGE("online pipeline is nullptr.");
        return nullptr;
    }
    return pipeline->GetOfflinePipeline(streamId);
}

std::shared_ptr<OfflinePipeline> OfflinePipelineManager::FindOfflinePipeline(int32_t id)
{
    std::lock_guard<std::mutex> l(lock_);
    std::shared_ptr<OfflinePipeline> op = nullptr;
    for (auto& it : offlinePipelineList_) {
        if (it.first == id) {
            op = it.second;
        }
    }

    if (op == nullptr) {
        return nullptr;
    }

    return op;
}

bool OfflinePipelineManager::CheckCaptureIdExist(int32_t id, int32_t captureId)
{
    auto op = FindOfflinePipeline(id);
    if (op == nullptr) {
        CAMERA_LOGW("can't get offlinepipeline");
        return false;
    }

    return op->CheckOwnerOfCaptureId(captureId);
}
} // namespace OHOS::Camera
