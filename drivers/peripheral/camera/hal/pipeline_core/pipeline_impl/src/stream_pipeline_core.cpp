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

#include "stream_pipeline_core.h"
#include "idevice_manager.h"
#include "ipp_node.h"

namespace OHOS::Camera {
RetCode StreamPipelineCore::Init()
{
    strategy_ = StreamPipelineStrategy::Create(context_->streamMgr_);
    builder_ = StreamPipelineBuilder::Create(context_->streamMgr_);
    dispatcher_ = StreamPipelineDispatcher::Create();
    return RC_OK;
}

RetCode StreamPipelineCore::PreConfig(const ModeMeta& meta)
{
    auto deviceManager = IDeviceManager::GetInstance();
    CHECK_IF_PTR_NULL_RETURN_VALUE(deviceManager, RC_ERROR);

    std::vector<DeviceStreamSetting> settings = {};
    std::vector<int32_t> ids = {};
    context_->streamMgr_->GetStreamIds(ids);
    for (auto i : ids) {
        auto info = context_->streamMgr_->GetStreamInfo(i);
        DeviceStreamSetting setting = {info.streamId_, info.bufferCount_, info.width_, info.height_,
            info.format_, info.usage_, static_cast<CameraEncodeType>(info.encodeType_)};
        settings.emplace_back(setting);
    }
    return deviceManager->PreConfig(meta, settings);
}

RetCode StreamPipelineCore::CreatePipeline(const int32_t& mode)
{
    std::lock_guard<std::mutex> l(mutex_);
    std::shared_ptr<PipelineSpec> spec = strategy_->GeneratePipelineSpec(mode);
    if (spec == nullptr) {
        return RC_ERROR;
    }
    std::shared_ptr<Pipeline> pipeline = builder_->Build(spec);
    if (pipeline == nullptr) {
        return RC_ERROR;
    }
    return dispatcher_->Update(pipeline);
}

RetCode StreamPipelineCore::DestroyPipeline(const std::vector<int>& streamIds)
{
    std::lock_guard<std::mutex> l(mutex_);
    RetCode re = RC_OK;
    for (const auto& it : streamIds) {
        re = dispatcher_->Destroy(it) | re;
        re = builder_->Destroy(it) | re;
        re = strategy_->Destroy(it) | re;
    }
    return re;
}

RetCode StreamPipelineCore::Prepare(const std::vector<int>& streamIds)
{
    std::lock_guard<std::mutex> l(mutex_);
    RetCode re = RC_OK;
    for (const auto& it : streamIds) {
        re = dispatcher_->Prepare(it) | re;
    }
    return re;
}

RetCode StreamPipelineCore::Start(const std::vector<int>& streamIds)
{
    std::lock_guard<std::mutex> l(mutex_);
    RetCode re = RC_OK;
    for (const auto& it : streamIds) {
        re = dispatcher_->Start(it) | re;
    }
    return re;
}

RetCode StreamPipelineCore::Stop(const std::vector<int>& streamIds)
{
    std::lock_guard<std::mutex> l(mutex_);
    RetCode re = RC_OK;
    for (const auto& it : streamIds) {
        CAMERA_LOGV("stop stream %{public}d begin", it);
        re = dispatcher_->Stop(it) | re;
        CAMERA_LOGV("stop stream %{public}d end", it);
    }
    return re;
}

RetCode StreamPipelineCore::Config(const std::vector<int>& streamIds, const CaptureMeta& meta)
{
    std::lock_guard<std::mutex> l(mutex_);
    RetCode re = RC_OK;
    for (const auto& it : streamIds) {
        re = dispatcher_->Config(it, meta) | re;
    }
    return re;
}

RetCode StreamPipelineCore::Capture(const std::vector<int>& streamIds, const int32_t captureId)
{
    std::lock_guard<std::mutex> l(mutex_);
    RetCode re = RC_OK;
    for (const auto& it : streamIds) {
        re = dispatcher_->Capture(it, captureId) | re;
    }
    return re;
}

RetCode StreamPipelineCore::CancelCapture(const std::vector<int>& streamIds)
{
    std::lock_guard<std::mutex> l(mutex_);
    RetCode re = RC_OK;
    for (const auto& it : streamIds) {
        re = dispatcher_->CancelCapture(it) | re;
    }
    return re;
}

RetCode StreamPipelineCore::Flush(const std::vector<int>& streamIds)
{
    std::lock_guard<std::mutex> l(mutex_);
    RetCode re = RC_OK;
    for (const auto& it : streamIds) {
        CAMERA_LOGV("flush stream %{public}d begin", it);
        re = dispatcher_->Flush(it) | re;
        CAMERA_LOGV("flush stream %{public}d end", it);
    }
    return re;
}

std::shared_ptr<OfflinePipeline> StreamPipelineCore::GetOfflinePipeline(const int32_t id)
{
    std::lock_guard<std::mutex> l(mutex_);
    std::shared_ptr<INode> node = dispatcher_->GetNode(id, "ipp");
    return std::static_pointer_cast<IppNode>(node);
}

OperationMode StreamPipelineCore::GetCurrentMode() const
{
    return mode_;
}

DynamicStreamSwitchMode StreamPipelineCore::CheckStreamsSupported(OperationMode mode,
                                                                  const ModeMeta& meta,
                                                                  const std::vector<StreamConfiguration>& configs)
{
    // check metadata
    CHECK_IF_PTR_NULL_RETURN_VALUE(meta, DYNAMIC_STREAM_SWITCH_NOT_SUPPORT);
    CHECK_IF_EQUAL_RETURN_VALUE(configs.empty(), true, DYNAMIC_STREAM_SWITCH_NOT_SUPPORT);

    std::vector<DeviceStreamSetting> settings = {};
    std::vector<int32_t> ids = {};
    context_->streamMgr_->GetStreamIds(ids);

    std::vector<int32_t> types = {};
    for (const auto it : configs) {
        types.emplace_back(static_cast<std::underlying_type<StreamIntent>::type>(it.type));
    }
    std::sort(types.begin(), types.end(), [](const int32_t& f, const int32_t& n) { return f < n; });

    bool isSupport = strategy_->CheckPipelineSpecExist(mode, types) == RC_OK ? true : false;
    if (isSupport && !ids.empty()) {
        return DYNAMIC_STREAM_SWITCH_NEED_INNER_RESTART;
    } else if (isSupport && ids.empty()) {
        return DYNAMIC_STREAM_SWITCH_SUPPORT;
    } else {
        return DYNAMIC_STREAM_SWITCH_NOT_SUPPORT;
    }

    return DYNAMIC_STREAM_SWITCH_NOT_SUPPORT;
}

std::shared_ptr<IStreamPipelineCore> IStreamPipelineCore::Create(const std::shared_ptr<NodeContext>& c)
{
    return std::make_shared<StreamPipelineCore>(c);
}
} // namespace OHOS::Camera
