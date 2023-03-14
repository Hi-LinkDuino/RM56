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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RICHTEXT_RESOURCE_RICHTEXT_DELEGATE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RICHTEXT_RESOURCE_RICHTEXT_DELEGATE_H

#include <list>

#include "core/components/common/layout/constants.h"
#include "core/components/rich_text/resource/rich_text_resource.h"
#include "core/components/rich_text/rich_text_component.h"

namespace OHOS::Ace {

class RichTextDelegate : public RichTextResource {
    DECLARE_ACE_TYPE(RichTextDelegate, RichTextResource);

public:
    using CreatedCallback = std::function<void()>;
    using ReleasedCallback = std::function<void(bool)>;
    using EventCallback = std::function<void(const std::string&)>;
    using UpdateWebViewLayoutCallback = std::function<void(int32_t, int32_t, int32_t)>;
    enum class State: char {
        WAITINGFORSIZE,
        CREATING,
        CREATED,
        CREATEFAILED,
        RELEASED,
    };

    RichTextDelegate() = delete;
    ~RichTextDelegate() override;
    RichTextDelegate(const WeakPtr<RichTextComponent>& webComponent,
        const WeakPtr<PipelineContext>& context, ErrorCallback&& onError, const std::string& type)
        : RichTextResource(type, context, std::move(onError)),
          webComponent_(webComponent),
          state_(State::WAITINGFORSIZE) {
            ACE_DCHECK(!type.empty());
    }

    void CreatePlatformResource(const WeakPtr<PipelineContext>& context,
        const int32_t top, const int32_t left, const bool visible);
    void ReleasePlatformResource();
    void AddCreatedCallback(const CreatedCallback& createdCallback);
    void RemoveCreatedCallback();
    void AddReleasedCallback(const ReleasedCallback& releasedCallback);
    void RemoveReleasedCallback();
    void AddWebLayoutChangeCallback(const UpdateWebViewLayoutCallback& layoutChangeCallback);
    void UpdateRichTextData(const std::string& data);
    void UpdateWebPostion(const int32_t top, const int32_t left);
    void UpdateContentScroll(const int32_t x, const int32_t y);
    void ChangeRichTextVisibility(const bool visible);
    void HideRichText();

private:
    void CreatePluginResource(const WeakPtr<PipelineContext>& context,
        const int32_t top, const int32_t left, const bool visible);
    void Stop();
    void InitWebEvent();
    void RegisterWebEvent();
    void UnregisterEvent();

    void OnPageStarted(const std::string& param);
    void OnPageFinished(const std::string& param);
    void OnPageError(const std::string& param);
    void OnGotLayoutParam(const std::string& param);

    std::string GetUrlStringParam(const std::string& param, const std::string& name) const;
    void CallPopPageSuccessPageUrl(const std::string& url, const int32_t pageId);
    void CallIsPagePathInvalid(const bool& isPageInvalid);

    void BindPopPageSuccessMethod();
    void BindIsPagePathInvalidMethod();

    WeakPtr<RichTextComponent> webComponent_;
    std::list<CreatedCallback> createdCallbacks_;
    std::list<ReleasedCallback> releasedCallbacks_;
    EventCallback onPageStarted_;
    EventCallback onPageFinished_;
    EventCallback onPageError_;
    UpdateWebViewLayoutCallback webviewLayoutCallback_;
    State state_ { State::WAITINGFORSIZE };
    std::string richTextData_;
    std::string pageUrl_;
    int32_t pageId_ = -1;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RICHTEXT_RESOURCE_RICHTEXT_DELEGATE_H
