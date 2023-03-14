/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_SUB_CONTAINER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_SUB_CONTAINER_H

#include "ashmem.h"

#include "base/thread/task_executor.h"
#include "core/common/frontend.h"
#include "frameworks/bridge/card_frontend/card_frontend.h"
#include "frameworks/core/pipeline/pipeline_context.h"

namespace OHOS::Ace {
class ACE_EXPORT SubContainer : public virtual AceType {
    DECLARE_ACE_TYPE(SubContainer, AceType);

public:
    using OnFormAcquiredCallback = std::function<void(const size_t)>;

    explicit SubContainer(const WeakPtr<PipelineContext>& context) : outSidePipelineContext_(context) {}
    SubContainer(const WeakPtr<PipelineContext>& context, int32_t instanceId)
        : outSidePipelineContext_(context), instanceId_(instanceId)
    {}
    ~SubContainer() = default;

    void Initialize();
    void RunCard(const int64_t id, const std::string path, const std::string module, const std::string data,
        std::map<std::string, std::pair<int, int32_t>> imageDataMap, const std::string formSrc);
    void UpdateCard(const std::string content, std::map<std::string, std::pair<int, int32_t>> imageDataMap);
    void Destroy();
    void GetNamesOfSharedImage(std::vector<std::string>& picNameArray);
    void UpdateSharedImage(std::vector<std::string>& picNameArray, std::vector<int32_t>& byteLenArray,
        std::vector<int32_t>& fileDescriptorArray);
    void GetImageDataFromAshmem(
        const std::string& picName, Ashmem& ashmem, const RefPtr<PipelineContext>& pipelineContext, int len);
    void ProcessSharedImage(const std::map<std::string, std::pair<int, int32_t>> imageDataMap);

    void SetFormElement(const WeakPtr<Element>& element)
    {
        formElement_ = element;
    }

    const WeakPtr<Element> GetFormElement() const
    {
        return formElement_;
    }

    void SetFormComponet(const RefPtr<Component>& mountPoint)
    {
        formComponent_ = mountPoint;
    }

    RefPtr<TaskExecutor> GetTaskExecutor() const
    {
        return taskExecutor_;
    }

    RefPtr<PipelineContext> GetPipelineContext() const
    {
        return pipelineContext_;
    }

    void UpdateRootElmentSize();
    void UpdateSurfaceSize();

    void AddFormAcquireCallback(const OnFormAcquiredCallback& callback)
    {
        if (callback) {
            onFormAcquiredCallback_ = callback;
        }
    }

    void SetAllowUpdate(bool update)
    {
        allowUpdate_ = update;
    }

    int64_t GetRunningCardId() const
    {
        return runningCardId_;
    }
    bool Dump(const std::vector<std::string>& params);

    WindowConfig GetWindowConfig() const
    {
        return cardWindowConfig_;
    }

    void SetWindowConfig(const WindowConfig cardWindowConfig)
    {
        cardWindowConfig_ = cardWindowConfig;
    }

private:
    RefPtr<CardFrontend> frontend_;
    RefPtr<TaskExecutor> taskExecutor_;
    RefPtr<PipelineContext> pipelineContext_;
    WeakPtr<PipelineContext> outSidePipelineContext_;
    RefPtr<AssetManager> assetManager_;
    int32_t instanceId_;

    int64_t runningCardId_ = 0;
    bool allowUpdate_ = true;

    RefPtr<Component> formComponent_;
    WeakPtr<Element> formElement_;
    OnFormAcquiredCallback onFormAcquiredCallback_;
    WindowConfig cardWindowConfig_;

    double surfaceWidth_ = 1.0f;
    double surfaceHeight_ = 1.0f;
    Dimension rootWidht_ = 0.0_vp;
    Dimension rootHeight_ = 0.0_vp;
    double density_ = 1.0f;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_SUB_CONTAINER_H
