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

#include "frameworks/bridge/js_frontend/engine/jsi/jsi_component_api_bridge.h"

#include "core/animation/curves.h"
#include "frameworks/bridge/common/dom/dom_list.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

shared_ptr<JsValue> JsiComponentApiBridge::JsGetScrollOffset(const shared_ptr<JsRuntime>& runtime, NodeId nodeId)
{
    if (!runtime) {
        LOGE("JsGetScrollOffset failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsGetScrollOffset failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("JsGetScrollOffset failed. page is null.");
        return runtime->NewUndefined();
    }
    Offset offset;
    auto task = [nodeId, page, &offset]() {
        auto domDoc = page->GetDomDocument();
        if (!domDoc) {
            return;
        }
        auto domNode = domDoc->GetDOMNodeById(nodeId);
        if (!domNode) {
            return;
        }
        auto domList = AceType::DynamicCast<DOMList>(domNode);
        if (domList) {
            offset = domList->GetCurrentOffset();
            return;
        }

        auto scrollComponent = domNode->GetScrollComponent();
        if (!scrollComponent) {
            return;
        }
        auto controller = scrollComponent->GetScrollPositionController();
        if (!controller) {
            return;
        }
        offset = controller->GetCurrentOffset();
    };
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsGetScrollOffset failed. delegate is null.");
        return runtime->NewUndefined();
    }
    delegate->PostSyncTaskToPage(task);
    shared_ptr<JsValue> offsetContext = runtime->NewObject();
    offsetContext->SetProperty(runtime, "x", runtime->NewNumber(offset.GetX()));
    offsetContext->SetProperty(runtime, "y", runtime->NewNumber(offset.GetY()));
    return offsetContext;
}

shared_ptr<JsValue> JsiComponentApiBridge::JsGetBoundingRect(const shared_ptr<JsRuntime>& runtime, NodeId nodeId)
{
    if (!runtime) {
        LOGE("JsGetBoundingRect failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsGetBoundingRect failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsGetBoundingRect failed. delegate is null.");
        return runtime->NewUndefined();
    }
    Rect boundingRect = delegate->GetBoundingRectData(nodeId);
    shared_ptr<JsValue> rectContext = runtime->NewObject();
    rectContext->SetProperty(runtime, "width", runtime->NewNumber(boundingRect.Width()));
    rectContext->SetProperty(runtime, "height", runtime->NewNumber(boundingRect.Height()));
    rectContext->SetProperty(runtime, "top", runtime->NewNumber(boundingRect.Top()));
    rectContext->SetProperty(runtime, "left", runtime->NewNumber(boundingRect.Left()));
    return rectContext;
}

shared_ptr<JsValue> JsiComponentApiBridge::JsGetInspector(const shared_ptr<JsRuntime>& runtime, NodeId nodeId)
{
    if (!runtime) {
        LOGE("JsGetInspector failed. runtime is null.");
        return nullptr;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsGetInspector failed. engine is null.");
        return runtime->NewUndefined();
    }
    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsGetInspector failed. delegate is null.");
        return runtime->NewUndefined();
    }
    auto attributes = delegate->GetInspector(nodeId);
    shared_ptr<JsValue> result = runtime->NewString(attributes);
    return result;
}

void JsiComponentApiBridge::JsScrollTo(
    const shared_ptr<JsRuntime>& runtime, const std::string& arguments, NodeId nodeId)
{
    if (!runtime) {
        LOGE("JsScrollTo failed. runtime is null.");
        return;
    }
    auto engine = static_cast<JsiEngineInstance*>(runtime->GetEmbedderData());
    if (!engine) {
        LOGE("JsScrollTo failed. engine is null.");
        return;
    }
    auto page = engine->GetRunningPage();
    if (!page) {
        LOGE("JsScrollTo failed. page is null.");
        return;
    }
    auto task = [nodeId, page, arguments]() {
        auto domDoc = page->GetDomDocument();
        if (!domDoc) {
            LOGE("JsScrollTo failed. dom document is null!");
            return;
        }
        auto domNode = domDoc->GetDOMNodeById(nodeId);
        if (!domNode) {
            LOGE("JsScrollTo failed. dom node is null!");
            return;
        }
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(arguments);
        if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() < 1) {
            LOGE("JsScrollTo failed. parse args error");
            return;
        }
        std::unique_ptr<JsonValue> scrollToPara = argsValue->GetArrayItem(0);
        int32_t index = scrollToPara->GetInt("index", 0);
        auto domList = AceType::DynamicCast<DOMList>(domNode);
        if (domList) {
            // list has specialized scrollTo method.
            domList->ScrollToMethod(index);
            return;
        }

        auto scrollComponent = domNode->GetScrollComponent();
        if (!scrollComponent) {
            return;
        }
        auto controller = scrollComponent->GetScrollPositionController();
        if (!controller) {
            return;
        }

        std::string id = scrollToPara->GetString("id", "");
        double position = scrollToPara->GetDouble("position", 0.0);
        double duration = scrollToPara->GetDouble("duration", 300.0); // Default duration is 300ms.
        std::string timingFunction = scrollToPara->GetString("timingFunction", "ease");
        std::string successId = scrollToPara->GetString("success", "");
        std::string failId = scrollToPara->GetString("fail", "");
        std::string completeId = scrollToPara->GetString("complete", "");
        auto context = domNode->GetPipelineContext();
        auto callback = [context, successId, completeId]() {
            auto refContext = context.Upgrade();
            if (refContext) {
                refContext->SendCallbackMessageToFrontend(successId, std::string("\"success\",null"));
                refContext->SendCallbackMessageToFrontend(completeId, std::string("\"complete\",null"));
            }
        };

        bool result = false;
        if (scrollToPara->Contains("position")) {
            result = controller->AnimateTo(position, duration, CreateCurve(timingFunction), false, callback);
        } else if (scrollToPara->Contains("id") && !id.empty()) {
            result = controller->AnimateToTarget(id, duration, CreateCurve(timingFunction), false, callback);
        } else {
            LOGW("JsScrollTo failed. param not valid.");
        }
        if (!result) {
            auto refContext = context.Upgrade();
            if (refContext) {
                refContext->SendCallbackMessageToFrontend(failId, std::string("\"fail\",null"));
                refContext->SendCallbackMessageToFrontend(completeId, std::string("\"complete\",null"));
            }
        }
    };

    auto delegate = engine->GetFrontendDelegate();
    if (!delegate) {
        LOGE("JsScrollTo failed. delegate is null.");
        return;
    }
    delegate->PostSyncTaskToPage(task);
}

} // namespace OHOS::Ace::Framework