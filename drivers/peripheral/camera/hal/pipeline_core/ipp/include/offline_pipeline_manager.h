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

#ifndef HOS_CAMERA_OFFLINE_PIPELINE_MANAGER_H
#define HOS_CAMERA_OFFLINE_PIPELINE_MANAGER_H

#include "camera.h"
#include "istream_pipeline_core.h"
#include "offline_pipeline.h"
#include <functional>
#include <list>
#include <memory>
#include <mutex>

namespace OHOS::Camera {
class OfflinePipelineManager {
public:
    static OfflinePipelineManager& GetInstance();
    RetCode SwitchToOfflinePipeline(int32_t streamId,
                                    int32_t streamType,
                                    const std::shared_ptr<IStreamPipelineCore>& pipeline,
                                    std::function<void(std::shared_ptr<IBuffer>&)> callback);
    RetCode CancelCapture(int32_t streamId, int32_t captureId);
    RetCode DestoryOfflinePipeline(int32_t id);
    RetCode DestoryOfflinePipelines();
    bool CheckCaptureIdExist(int32_t id, int32_t captureId);

private:
    OfflinePipelineManager() = default;
    OfflinePipelineManager(const OfflinePipelineManager&);
    OfflinePipelineManager& operator=(const OfflinePipelineManager&);
    OfflinePipelineManager(OfflinePipelineManager&&);
    OfflinePipelineManager& operator=(OfflinePipelineManager&&);
    ~OfflinePipelineManager();

    std::shared_ptr<OfflinePipeline> GetOfflinePipeline(int32_t streamId,
                                                        const std::shared_ptr<IStreamPipelineCore>& pipeline);
    std::shared_ptr<OfflinePipeline> FindOfflinePipeline(int32_t id);

private:
    std::mutex lock_;
    std::list<std::pair<int32_t, std::shared_ptr<OfflinePipeline>>> offlinePipelineList_ = {};
};
} // namespace OHOS::Camera
#endif

