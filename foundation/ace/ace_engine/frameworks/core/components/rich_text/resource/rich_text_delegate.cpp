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

#include "core/components/rich_text/resource/rich_text_delegate.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "base/log/log.h"
#include "core/event/ace_event_helper.h"
#include "core/event/back_end_event_manager.h"
#include "frameworks/bridge/js_frontend/frontend_delegate_impl.h"

namespace OHOS::Ace {

namespace {

constexpr char RICH_TEXT_METHOD_CHANGE_VISIBILITY[] = "changeRichTextVisibility";
constexpr char RICH_TEXT_METHOD_HIDE_RICHTEXT_WHEN_POP[] = "hideRichTextWhenPop";
constexpr char RICH_TEXT_METHOD_HIDE_RICHTEXT_WHEN_PUSH[] = "hideRichTextWhenPush";
constexpr char RICH_TEXT_METHOD_SHOW_RICHTEXT[] = "showRichText";
constexpr char RICH_TEXT_METHOD_UPDATE_CONTENT[] = "updateRichTextContent";
constexpr char RICH_TEXT_METHOD_UPDATE_TRANSLATE[] = "updateTranslate";
constexpr char RICH_TEXT_METHOD_UPDATE_CONTENT_TRANSLATE[] = "updateContentTranslate";

constexpr char RICH_TEXT_EVENT_LOAD_START[] = "onPageStarted";
constexpr char RICH_TEXT_EVENT_LOAD_FINISHED[] = "onPageFinished";
constexpr char RICH_TEXT_EVENT_GOT_LAYOUT_PARAM[] = "onGotLayoutParam";
constexpr char RICH_TEXT_EVENT_LOAD_ERROR[] = "onPageError";

constexpr char RICH_TEXT_RESOURCE_NAME[] = "richtext";
constexpr char NTC_PARAM_RICH_TEXT[] = "richtext";

constexpr char NTC_PARAM_LEFT[] = "left";
constexpr char NTC_PARAM_TOP[] = "top";
constexpr char NTC_PARAM_URL[] = "url";
constexpr char NTC_PARAM_X[] = "x";
constexpr char NTC_PARAM_Y[] = "y";
constexpr char NTC_PARAM_LAYOUT_HEIGHT[] = "layoutHeight";
constexpr char NTC_PARAM_LAYOUT_WIDTH[] = "layoutWidth";
constexpr char NTC_PARAM_CONTENT_HEIGHT[] = "contentHeight";
constexpr char NTC_PARAM_RICHTEXT_VISIBILITY[] = "visibility";
constexpr char NTC_PARAM_PAGE_PATH[] = "pageRoutePath";
constexpr char NTC_PARAM_DESCRIPTION[] = "description";
constexpr char NTC_PARAM_CONTENT_DATA[] = "data";
constexpr char NTC_PARAM_ERROR_CODE[] = "errorCode";

constexpr char NTC_ERROR[] = "create error";
constexpr char RICH_TEXT_ERROR_CODE_CREATEFAIL[] = "error-rich-text-delegate-000001";
constexpr char RICH_TEXT_ERROR_MSG_CREATEFAIL[] = "create rich text delegate failed.";

} // namespace

RichTextDelegate::~RichTextDelegate()
{
    ReleasePlatformResource();
}

void RichTextDelegate::ReleasePlatformResource()
{
    Stop();
    Release();
}

void RichTextDelegate::Stop()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context when stop");
        return;
    }
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(),
        TaskExecutor::TaskType::PLATFORM);
    if (platformTaskExecutor.IsRunOnCurrentThread()) {
        UnregisterEvent();
    } else {
        platformTaskExecutor.PostTask([weak = WeakClaim(this)] {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->UnregisterEvent();
            }
        });
    }
}

void RichTextDelegate::UnregisterEvent()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context when unregister event");
        return;
    }
    auto resRegister = context->GetPlatformResRegister();
    resRegister->UnregisterEvent(MakeEventHash(RICH_TEXT_EVENT_LOAD_START));
    resRegister->UnregisterEvent(MakeEventHash(RICH_TEXT_EVENT_LOAD_FINISHED));
    resRegister->UnregisterEvent(MakeEventHash(RICH_TEXT_EVENT_LOAD_ERROR));
    resRegister->UnregisterEvent(MakeEventHash(RICH_TEXT_EVENT_GOT_LAYOUT_PARAM));
}

void RichTextDelegate::CreatePlatformResource(
    const WeakPtr<PipelineContext>& context, const int32_t top, const int32_t left, const bool visible)
{
    context_ = context;
    CreatePluginResource(context, top, left, visible);
    InitWebEvent();
}

void RichTextDelegate::CreatePluginResource(
    const WeakPtr<PipelineContext>& context, const int32_t top, const int32_t left, const bool visible)
{
    state_ = State::CREATING;

    auto webCom = webComponent_.Upgrade();
    if (!webCom) {
        state_ = State::CREATEFAILED;
        OnError(NTC_ERROR, "fail to call WebDelegate::Create due to webComponent is null");
        return;
    }

    auto pipelineContext = context.Upgrade();
    if (!pipelineContext) {
        state_ = State::CREATEFAILED;
        OnError(NTC_ERROR, "fail to call WebDelegate::Create due to context is null");
        return;
    }
    context_ = context;
    auto platformTaskExecutor = SingleTaskExecutor::Make(pipelineContext->GetTaskExecutor(),
                                                         TaskExecutor::TaskType::PLATFORM);
    auto resRegister = pipelineContext->GetPlatformResRegister();
    auto weakRes = AceType::WeakClaim(AceType::RawPtr(resRegister));
    platformTaskExecutor.PostTask([weak = WeakClaim(this), weakRes, top, left, visible] {
        auto delegate = weak.Upgrade();
        if (!delegate) {
            return;
        }
        auto webCom = delegate->webComponent_.Upgrade();
        if (!webCom) {
            delegate->OnError(NTC_ERROR, "fail to call WebDelegate::SetSrc PostTask");
        }
        auto resRegister = weakRes.Upgrade();
        if (!resRegister) {
            delegate->OnError(RICH_TEXT_ERROR_CODE_CREATEFAIL, RICH_TEXT_ERROR_MSG_CREATEFAIL);
            return;
        }

        auto context = delegate->context_.Upgrade();
        if (!context) {
            LOGE("context is null");
            return;
        }

        delegate->id_ = CREATING_ID;

        std::string pageUrl;
        int32_t pageId;
        OHOS::Ace::Framework::DelegateClient::GetInstance().GetWebPageUrl(pageUrl, pageId);
        std::string richTextVisible = visible ? "true" : "false";
        delegate->pageUrl_ = pageUrl;
        delegate->pageId_ = pageId;

        std::stringstream paramStream;
        paramStream << NTC_PARAM_RICH_TEXT << RICHTEXT_PARAM_EQUALS << delegate->id_ << RICHTEXT_PARAM_AND
                    << NTC_PARAM_CONTENT_DATA << RICHTEXT_PARAM_EQUALS << webCom->GetData() << RICHTEXT_PARAM_AND
                    << NTC_PARAM_LEFT << RICHTEXT_PARAM_EQUALS << left << RICHTEXT_PARAM_AND
                    << NTC_PARAM_TOP << RICHTEXT_PARAM_EQUALS << top << RICHTEXT_PARAM_AND
                    << NTC_PARAM_RICHTEXT_VISIBILITY << RICHTEXT_PARAM_EQUALS << richTextVisible << RICHTEXT_PARAM_AND
                    << NTC_PARAM_PAGE_PATH << RICHTEXT_PARAM_EQUALS << pageUrl;

        std::string param = paramStream.str();
        delegate->id_ = resRegister->CreateResource(RICH_TEXT_RESOURCE_NAME, param);
        if (delegate->id_ == INVALID_ID) {
            delegate->OnError(RICH_TEXT_ERROR_CODE_CREATEFAIL, RICH_TEXT_ERROR_MSG_CREATEFAIL);
            return;
        }
        delegate->state_ = State::CREATED;
        delegate->hash_ = delegate->MakeResourceHash();
        delegate->RegisterWebEvent();
        delegate->BindPopPageSuccessMethod();
        delegate->BindIsPagePathInvalidMethod();
    });
}

void RichTextDelegate::InitWebEvent()
{
    auto webCom = webComponent_.Upgrade();
    if (!webCom) {
        state_ = State::CREATEFAILED;
        OnError(NTC_ERROR, "fail to call get event due to rich text component is null");
        return;
    }
    if (!webCom->GetPageStartedEventId().IsEmpty()) {
        onPageStarted_ =
            AceAsyncEvent<void(const std::string&)>::Create(webCom->GetPageStartedEventId(), context_);
    }
    if (!webCom->GetPageFinishedEventId().IsEmpty()) {
        onPageFinished_ =
            AceAsyncEvent<void(const std::string&)>::Create(webCom->GetPageFinishedEventId(), context_);
    }
    if (!webCom->GetPageErrorEventId().IsEmpty()) {
        onPageError_ =
            AceAsyncEvent<void(const std::string&)>::Create(webCom->GetPageErrorEventId(), context_);
    }
}

void RichTextDelegate::RegisterWebEvent()
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto resRegister = context->GetPlatformResRegister();
    resRegister->RegisterEvent(MakeEventHash(RICH_TEXT_EVENT_LOAD_START),
        [weak = WeakClaim(this)](const std::string& param) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->OnPageStarted(param);
            }
        });
    resRegister->RegisterEvent(MakeEventHash(RICH_TEXT_EVENT_LOAD_FINISHED),
        [weak = WeakClaim(this)](const std::string& param) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->OnPageFinished(param);
            }
        });
    resRegister->RegisterEvent(MakeEventHash(RICH_TEXT_EVENT_GOT_LAYOUT_PARAM),
        [weak = WeakClaim(this)](const std::string& param) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->OnGotLayoutParam(param);
            }
        });
    resRegister->RegisterEvent(MakeEventHash(RICH_TEXT_EVENT_LOAD_ERROR),
        [weak = WeakClaim(this)](const std::string& param) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->OnPageError(param);
            }
        });
}

// upper ui component which inherits from WebComponent
// could implement some curtain createdCallback to customized controller interface
// eg: web.loadurl.
void RichTextDelegate::AddCreatedCallback(const CreatedCallback& createdCallback)
{
    if (!createdCallback || state_ == State::RELEASED) {
        return;
    }
    createdCallbacks_.emplace_back(createdCallback);
}

void RichTextDelegate::AddWebLayoutChangeCallback(const UpdateWebViewLayoutCallback& layoutChangeCallback)
{
    if (!layoutChangeCallback || state_ == State::RELEASED) {
        return;
    }
    webviewLayoutCallback_ = layoutChangeCallback;
}

void RichTextDelegate::RemoveCreatedCallback()
{
    if (state_ == State::RELEASED) {
        return;
    }
    createdCallbacks_.pop_back();
}

void RichTextDelegate::AddReleasedCallback(const ReleasedCallback& releasedCallback)
{
    if (!releasedCallback || state_ == State::RELEASED) {
        return;
    }
    releasedCallbacks_.emplace_back(releasedCallback);
}

void RichTextDelegate::RemoveReleasedCallback()
{
    if (state_ == State::RELEASED) {
        return;
    }
    releasedCallbacks_.pop_back();
}

void RichTextDelegate::UpdateRichTextData(const std::string& data)
{
    if (id_ == INVALID_ID) {
        return;
    }

    hash_ = MakeResourceHash();
    Method updateContentMethod = MakeMethodHash(RICH_TEXT_METHOD_UPDATE_CONTENT);
    std::stringstream paramStream;
    paramStream << NTC_PARAM_CONTENT_DATA << RICHTEXT_PARAM_EQUALS << data;
    std::string param = paramStream.str();
    CallResRegisterMethod(updateContentMethod, param, nullptr);
}

void RichTextDelegate::UpdateWebPostion(const int32_t top, const int32_t left)
{
    hash_ = MakeResourceHash();

    Method updateLayoutPositionMethod = MakeMethodHash(RICH_TEXT_METHOD_UPDATE_TRANSLATE);
    std::stringstream paramStream;
    paramStream << NTC_PARAM_LEFT << RICHTEXT_PARAM_EQUALS << left << RICHTEXT_PARAM_AND
    << NTC_PARAM_TOP << RICHTEXT_PARAM_EQUALS << top;
    std::string param = paramStream.str();
    CallResRegisterMethod(updateLayoutPositionMethod, param, nullptr);
}

void RichTextDelegate::UpdateContentScroll(const int32_t x, const int32_t y)
{
    hash_ = MakeResourceHash();

    Method updateLayoutPositionMethod = MakeMethodHash(RICH_TEXT_METHOD_UPDATE_CONTENT_TRANSLATE);
    std::stringstream paramStream;
    paramStream << NTC_PARAM_X << RICHTEXT_PARAM_EQUALS << x << RICHTEXT_PARAM_AND
                << NTC_PARAM_Y << RICHTEXT_PARAM_EQUALS << y;
    std::string param = paramStream.str();
    CallResRegisterMethod(updateLayoutPositionMethod, param, nullptr);
}

void RichTextDelegate::CallPopPageSuccessPageUrl(const std::string& url, const int32_t pageId)
{
    if (url == pageUrl_ && pageId == pageId_) {
        hash_ = MakeResourceHash();
        Method reShowRichTextMethod = MakeMethodHash(RICH_TEXT_METHOD_SHOW_RICHTEXT);
        std::stringstream paramStream;
        paramStream << NTC_PARAM_PAGE_PATH << RICHTEXT_PARAM_EQUALS << url;
        std::string param = paramStream.str();
        CallResRegisterMethod(reShowRichTextMethod, param, nullptr);
    }
}

void RichTextDelegate::CallIsPagePathInvalid(const bool& isPageInvalid)
{
    hash_ = MakeResourceHash();
    Method hideRichTextMethod = MakeMethodHash(RICH_TEXT_METHOD_HIDE_RICHTEXT_WHEN_PUSH);
    std::stringstream paramStream;
    paramStream << RICHTEXT_PARAM_NONE;
    std::string param = paramStream.str();
    CallResRegisterMethod(hideRichTextMethod, param, nullptr);
}

void RichTextDelegate::HideRichText()
{
    std::string pageUrl;
    int32_t pageId;
    OHOS::Ace::Framework::DelegateClient::GetInstance().GetWebPageUrl(pageUrl, pageId);

    if (pageUrl != pageUrl_ || pageId != pageId_) {
        hash_ = MakeResourceHash();
        Method hideRichTextMethod = MakeMethodHash(RICH_TEXT_METHOD_HIDE_RICHTEXT_WHEN_POP);
        std::stringstream paramStream;
        paramStream << RICHTEXT_PARAM_NONE;
        std::string param = paramStream.str();
        CallResRegisterMethod(hideRichTextMethod, param, nullptr);
    }
}

void RichTextDelegate::ChangeRichTextVisibility(const bool visible)
{
    hash_ = MakeResourceHash();
    Method hideRichTextMethod = MakeMethodHash(RICH_TEXT_METHOD_CHANGE_VISIBILITY);
    std::string richTextVisible = visible ? "true" : "false";
    std::stringstream paramStream;
    paramStream << NTC_PARAM_RICHTEXT_VISIBILITY << RICHTEXT_PARAM_EQUALS << richTextVisible;
    std::string param = paramStream.str();
    CallResRegisterMethod(hideRichTextMethod, param, nullptr);
}
void RichTextDelegate::OnPageStarted(const std::string& param)
{
    if (onPageStarted_) {
        std::string param = std::string(R"("start", null, null)");
        onPageStarted_(param);
    }
}

void RichTextDelegate::OnPageFinished(const std::string& param)
{
    if (onPageFinished_) {
        std::string param = std::string(R"("complete", null, null)");
        onPageFinished_(param);
    }
}

void RichTextDelegate::OnGotLayoutParam(const std::string& param)
{
    int32_t layoutHeight = GetIntParam(param, NTC_PARAM_LAYOUT_HEIGHT);
    int32_t contentHeight = GetIntParam(param, NTC_PARAM_CONTENT_HEIGHT);
    int32_t layoutWidth = GetIntParam(param, NTC_PARAM_LAYOUT_WIDTH);
    if (webviewLayoutCallback_) {
        webviewLayoutCallback_(layoutWidth, layoutHeight, contentHeight);
    }
}

void RichTextDelegate::OnPageError(const std::string& param)
{
    if (onPageError_) {
        int32_t errorCode = GetIntParam(param, NTC_PARAM_ERROR_CODE);
        std::string url = GetUrlStringParam(param, NTC_PARAM_URL);
        std::string description = GetStringParam(param, NTC_PARAM_DESCRIPTION);

        std::string paramUrl = std::string(R"(")").append(url)
                                                  .append(std::string(R"(")"))
                                                  .append(",");

        std::string paramErrorCode = std::string(R"(")").append(NTC_PARAM_ERROR_CODE)
                                                        .append(std::string(R"(")"))
                                                        .append(":")
                                                        .append(std::to_string(errorCode))
                                                        .append(",");

        std::string paramDesc = std::string(R"(")").append(NTC_PARAM_DESCRIPTION)
                                                   .append(std::string(R"(")"))
                                                   .append(":")
                                                   .append(std::string(R"(")")
                                                   .append(description)
                                                   .append(std::string(R"(")")));
        std::string param = std::string(R"("error",{"url":)")
                                           .append((paramUrl + paramErrorCode + paramDesc)
                                           .append("},null"));
        onPageError_(param);
    }
}

std::string RichTextDelegate::GetUrlStringParam(const std::string& param, const std::string& name) const
{
    size_t len = name.length();
    size_t posErrorCode = param.find(NTC_PARAM_ERROR_CODE);
    size_t pos = param.find(name);
    std::string result;

    if (pos != std::string::npos && posErrorCode != std::string::npos) {
        std::stringstream ss;

        ss << param.substr(pos + 1 + len, posErrorCode - 5);
        ss >> result;
    }
    return result;
}

void RichTextDelegate::BindPopPageSuccessMethod()
{
    auto context = context_.Upgrade();
    if (context) {
        context->SetPopPageSuccessEventHandler(
            [weak = WeakClaim(this)](const std::string& pageUrl, const int32_t pageId) {
                std::string url = pageUrl.substr(0, pageUrl.length() - 3);
                auto delegate = weak.Upgrade();
                if (delegate) {
                    delegate->CallPopPageSuccessPageUrl(url, pageId);
                }
        });
    }
}

void RichTextDelegate::BindIsPagePathInvalidMethod()
{
    auto context = context_.Upgrade();
    if (context) {
        context->SetIsPagePathInvalidEventHandler([weak = WeakClaim(this)](bool& isPageInvalid) {
            auto delegate = weak.Upgrade();
            if (delegate) {
                delegate->CallIsPagePathInvalid(isPageInvalid);
            }
        });
    }
}

} // namespace OHOS::Ace
