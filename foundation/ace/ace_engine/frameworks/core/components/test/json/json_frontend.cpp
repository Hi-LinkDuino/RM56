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

#include "core/components/test/json/json_frontend.h"

#include "base/log/dump_log.h"
#include "core/components/test/json/json_ace_page.h"

namespace OHOS::Ace {

RefPtr<Frontend> Frontend::CreateDefault()
{
    return AceType::MakeRefPtr<JsonFrontend>();
}

bool JsonFrontend::Initialize(FrontendType type, const RefPtr<TaskExecutor>&)
{
    // This interface is inherited from base class 'Frontend', but we never use 'TaskExecutor'.
    type_ = type;
    return true;
}

void JsonFrontend::AttachPipelineContext(const RefPtr<PipelineContext>& context)
{
    pipelineContext_ = context;
}

void JsonFrontend::SetAssetManager(const RefPtr<AssetManager>&)
{
    // This interface is inherited from base class 'Frontend', but we never use 'AssetManager'.
}

void JsonFrontend::AddPage(const RefPtr<AcePage>& page)
{
    if (!page) {
        LOGE("the page is nullptr");
        return;
    }
    const auto pageId = page->GetPageId();
    const auto result = pageMap_.try_emplace(pageId, page);
    if (!result.second) {
        LOGW("the page has already in the map");
    }
}

RefPtr<AcePage> JsonFrontend::GetPage(int32_t pageId) const
{
    const auto iter = pageMap_.find(pageId);
    if (iter == pageMap_.end()) {
        LOGE("the page is not in the page map, id is %{public}d", pageId);
        return nullptr;
    }
    return iter->second;
}

void JsonFrontend::RunPage(int32_t pageId, const std::string& content, const std::string& params)
{
    if (!pipelineContext_) {
        LOGE("Not attached to pipeline context yet");
        return;
    }

    auto page = GetPage(pageId);
    if (!page) {
        LOGE("the page is nullptr");
        return;
    }
    auto taskRunner = pipelineContext_->GetTaskExecutor();
    if (!taskRunner) {
        LOGE("the taskRunner is nullptr");
        return;
    }
    taskRunner->PostTask(
        [page, content, context = pipelineContext_] {
            auto pageComponent = page->BuildPage(content);
            context->PushPage(pageComponent);
        },
        TaskExecutor::TaskType::UI);
}

void JsonFrontend::PushPage(const std::string& content, const std::string& params) {}

void JsonFrontend::UpdatePage(int32_t pageId, const std::string& content)
{
    if (!pipelineContext_) {
        LOGE("Not attached to pipeline context yet");
        return;
    }

    auto page = GetPage(pageId);
    if (!page) {
        LOGE("the page is nullptr");
        return;
    }
    RefPtr<JsonAcePage> jsonPage = AceType::DynamicCast<JsonAcePage>(page);
    auto taskRunner = pipelineContext_->GetTaskExecutor();
    if (!taskRunner) {
        LOGE("the taskRunner is nullptr");
        return;
    }
    taskRunner->PostTask(
        [jsonPage, content, context = pipelineContext_] {
            auto patchComponent = jsonPage->BuildPagePatch(content);
            context->ScheduleUpdate(patchComponent);
        },
        TaskExecutor::TaskType::UI);
}

RefPtr<AceEventHandler> JsonFrontend::GetEventHandler()
{
    return handler_;
}

void JsonFrontend::UpdateState(Frontend::State state)
{
    switch (state) {
        case Frontend::State::ON_CREATE:
            LOGD("update state in JsonFrontend with state ON_CREATE");
            break;
        case Frontend::State::ON_DESTROY:
            LOGD("update state in JsonFrontend with state ON_DESTROY");
            break;
        case Frontend::State::ON_SHOW:
            LOGD("update state in JsonFrontend with state ON_SHOW");
            break;
        case Frontend::State::ON_HIDE:
            LOGD("update state in JsonFrontend with state ON_HIDE");
            break;
        default:
            LOGE("error State: %{public}d", state);
            break;
    }
}

bool JsonFrontend::IsForeground()
{
    return false;
}

RefPtr<AccessibilityManager> JsonFrontend::GetAccessibilityManager() const
{
    return nullptr;
}

bool JsonFrontend::OnBackPressed()
{
    return false;
}

void JsonFrontend::OnShow()
{
    LOGD("JsonFrontend OnShow");
}

void JsonFrontend::OnHide()
{
    LOGD("JsonFrontend OnHide");
}

void JsonFrontend::OnActive()
{
    LOGD("JsonFrontend OnActive");
}

void JsonFrontend::OnInactive()
{
    LOGD("JsonFrontend OnInactive");
}

bool JsonFrontend::OnStartContinuation()
{
    LOGD("JsonFrontend OnStartContinuation");
    return false;
}

void JsonFrontend::OnCompleteContinuation(int32_t code)
{
    LOGD("JsonFrontend OnCompleteContinuation");
}

void JsonFrontend::OnSaveData(std::string& data)
{
    data = "";
    LOGD("JsonFrontend OnSaveData %{private}s", data.c_str());
}

bool JsonFrontend::OnRestoreData(const std::string& data)
{
    LOGD("JsonFrontend OnRestoreData");
    return false;
}

void JsonFrontend::OnNewRequest(const std::string& data)
{
    LOGD("JsonFrontend OnNewRequest");
}

void JsonFrontend::CallRouterBack() {}

} // namespace OHOS::Ace
