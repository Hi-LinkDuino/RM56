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

#include "frameworks/bridge/js_frontend/engine/v8/v8_component_api_bridge.h"

#include "core/animation/curves.h"
#include "frameworks/bridge/common/dom/dom_list.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/engine/v8/v8_utils.h"

namespace OHOS::Ace::Framework {

v8::Local<v8::Object> V8ComponentApiBridge::JsGetScrollOffset(v8::Isolate* isolate, NodeId nodeId)
{
    if (!isolate) {
        return v8::Local<v8::Object>();
    }
    Offset offset;
    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (page == nullptr) {
        return v8::Local<v8::Object>();
    }
    auto task = [nodeId, page, &offset]() {
        auto domDoc = (*page)->GetDomDocument();
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

    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    if (delegate == nullptr) {
        return v8::Local<v8::Object>();
    }
    (*delegate)->PostSyncTaskToPage(task);

    auto ctx = isolate->GetCurrentContext();
    v8::Local<v8::Object> offsetContext = v8::Object::New(ctx->GetIsolate());
    offsetContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "x").ToLocalChecked(),
            v8::Number::New(ctx->GetIsolate(), offset.GetX()))
        .ToChecked();
    offsetContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "y").ToLocalChecked(),
            v8::Number::New(ctx->GetIsolate(), offset.GetY()))
        .ToChecked();
    return offsetContext;
}

v8::Local<v8::Object> V8ComponentApiBridge::JsGetBoundingRect(v8::Isolate* isolate, NodeId nodeId)
{
    if (!isolate) {
        return v8::Local<v8::Object>();
    }
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    if (delegate == nullptr) {
        return v8::Local<v8::Object>();
    }

    Rect boundingRect = (*delegate)->GetBoundingRectData(nodeId);
    auto ctx = isolate->GetCurrentContext();
    v8::Local<v8::Object> rectContext = v8::Object::New(ctx->GetIsolate());
    rectContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "width").ToLocalChecked(),
            v8::Number::New(ctx->GetIsolate(), boundingRect.Width()))
        .ToChecked();
    rectContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "height").ToLocalChecked(),
            v8::Number::New(ctx->GetIsolate(), boundingRect.Height()))
        .ToChecked();
    rectContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "top").ToLocalChecked(),
            v8::Number::New(ctx->GetIsolate(), boundingRect.Top()))
        .ToChecked();
    rectContext
        ->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "left").ToLocalChecked(),
            v8::Number::New(ctx->GetIsolate(), boundingRect.Left()))
        .ToChecked();
    return rectContext;
}

void V8ComponentApiBridge::JsScrollTo(
    const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& arguments, NodeId nodeId)
{
    v8::Isolate* isolate = args.GetIsolate();
    if (isolate == nullptr) {
        LOGE("V8ComponentApiBridge::JsScrollTo isolate is null!");
        return;
    }
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("V8ComponentApiBridge::JsScrollTo context is empty!");
        return;
    }
    v8::Local<v8::External> data = v8::Local<v8::External>::Cast(args.Data());
    V8EngineInstance* engineInstance = static_cast<V8EngineInstance*>(data->Value());
    if (engineInstance == nullptr) {
        LOGE("V8ComponentApiBridge::JsScrollTo engineInstance is null!");
        return;
    }

    auto page = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::RUNNING_PAGE));
    if (page == nullptr) {
        LOGE("V8ComponentApiBridge::JsScrollTo page is null");
        return;
    }

    auto task = [nodeId, page, arguments]() {
        auto domDoc = (*page)->GetDomDocument();
        if (!domDoc) {
            LOGE("V8ComponentApiBridge::JsScrollTo dom document is null!");
            return;
        }

        auto domNode = domDoc->GetDOMNodeById(nodeId);
        if (!domNode) {
            LOGE("V8ComponentApiBridge::JsScrollTo node not exist!");
            return;
        }

        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(arguments);
        if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() < 1) {
            LOGE("V8ComponentApiBridge::JsScrollTo parse args error");
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
            LOGW("V8ComponentApiBridge::JsScrollTo param not valid.");
        }
        if (!result) {
            auto refContext = context.Upgrade();
            if (refContext) {
                refContext->SendCallbackMessageToFrontend(failId, std::string("\"fail\",null"));
                refContext->SendCallbackMessageToFrontend(completeId, std::string("\"complete\",null"));
            }
        }
    };

    auto delegate = engineInstance->GetDelegate();
    if (!delegate) {
        LOGE("V8ComponentApiBridge::JsScrollTo delegate is null");
        return;
    }
    delegate->PostSyncTaskToPage(task);
}

v8::Local<v8::Object> V8ComponentApiBridge::JsAddVisibleListener(
    const v8::FunctionCallbackInfo<v8::Value>& args, const std::string& arguments, NodeId nodeId)
{
    v8::Isolate* isolate = args.GetIsolate();
    if (isolate == nullptr) {
        LOGE("V8ComponentApiBridge::JsAddVisibleListener isolate is null!");
        return v8::Local<v8::Object>();
    }
    v8::HandleScope handleScope(isolate);
    auto ctx = isolate->GetCurrentContext();
    if (ctx.IsEmpty()) {
        LOGE("V8ComponentApiBridge::JsAddVisibleListener context is empty!");
        return v8::Local<v8::Object>();
    }
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(arguments);
    if (!argsPtr) {
        LOGE("V8ComponentApiBridge::JsAddVisibleListener argsPtr is null!");
        return v8::Local<v8::Object>();
    }
    auto observerParam = argsPtr->GetArrayItem(0);
    if (!observerParam) {
        return v8::Local<v8::Object>();
    }
    std::string type;
    std::vector<double> ratios;
    if (observerParam->GetValue("type") != nullptr && observerParam->GetValue("type")->IsString()) {
        type = observerParam->GetValue("type")->GetString();
    }
    if (observerParam->GetValue("ratios") != nullptr && observerParam->GetValue("ratios")->IsArray()) {
        auto ratioLen = observerParam->GetValue("ratios")->GetArraySize();
        for (int32_t i = 0; i < ratioLen; ++i) {
            auto ratioVal = observerParam->GetValue("ratios")->GetArrayItem(i);
            if (ratioVal->IsNumber()) {
                ratios.emplace_back(ratioVal->GetDouble());
            }
        }
    }
    auto listener = v8::Object::New(ctx->GetIsolate());
    listener->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "__nodeId").ToLocalChecked(),
        v8::Int32::New(ctx->GetIsolate(), nodeId)).ToChecked();
    listener->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "__type").ToLocalChecked(),
        v8::String::NewFromUtf8(ctx->GetIsolate(), type.c_str()).ToLocalChecked()).ToChecked();
    for (int32_t j = 0; j < static_cast<int32_t>(ratios.size()); ++j) {
        std::string ratioName = "__ratio" + std::to_string(j);
        listener->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), ratioName.c_str()).ToLocalChecked(),
            v8::Number::New(ctx->GetIsolate(), ratios[j])).ToChecked();
    }
    listener->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "__ratioLen").ToLocalChecked(),
                  v8::Number::New(ctx->GetIsolate(), static_cast<int32_t>(ratios.size()))).ToChecked();
    listener->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "observe").ToLocalChecked(),
        v8::Function::New(ctx, JsVisibleListenerOn, v8::Local<v8::Value>(), 1).ToLocalChecked()).ToChecked();
    listener->Set(ctx, v8::String::NewFromUtf8(ctx->GetIsolate(), "unobserve").ToLocalChecked(),
        v8::Function::New(ctx, JsVisibleListenerOff, v8::Local<v8::Value>(), 0).ToLocalChecked()).ToChecked();
    return listener;
}

void V8ComponentApiBridge::JsVisibleListenerOn(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto observerValue = args.Holder();
    NodeId id = observerValue->Get(context, v8::String::NewFromUtf8(isolate, "__nodeId").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    auto ratioLength = observerValue->Get(context, v8::String::NewFromUtf8(isolate, "__ratioLen").ToLocalChecked())
        .ToLocalChecked()->NumberValue(context).ToChecked();
    std::vector<double> ratios;
    for (int32_t i = 0; i < ratioLength; ++i) {
        auto ratioName = "__ratio" + std::to_string(i);
        auto ratio = observerValue->Get(context, v8::String::NewFromUtf8(isolate, ratioName.c_str()).ToLocalChecked())
            .ToLocalChecked()->NumberValue(context).ToChecked();
        if (LessOrEqual(ratio, 0.0)) {
            ratio = 0.001;
        }
        if (GreatOrEqual(ratio, 1.0)) {
            ratio = 0.999;
        }
        ratios.emplace_back(ratio);
    }

    if (!args[0]->IsFunction()) {
        LOGW("args is not function");
        return;
    }
    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(args[0]);
    auto callbackObject = AceType::MakeRefPtr<VisibleListenerCallback>(context, isolate, jsFunc, id);
    auto stagingPage = static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8EngineInstance::STAGING_PAGE));
    int32_t instanceId = (*stagingPage)->GetPageId();
    auto jsCallback = [callbackObj = callbackObject, isolate, instanceId](bool visible, double ratio) {
        v8::HandleScope handleScope(isolate);
        auto context = isolate->GetCurrentContext();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Context::Scope contextScope(context);
        v8::TryCatch tryCatch(isolate);
        v8::Local<v8::Function> jsFunc = callbackObj->GetJsObject();

        v8::Local<v8::Boolean> visibleJsVal = v8::Boolean::New(context->GetIsolate(), visible);
        v8::Local<v8::Number> ratioJsVal = v8::Number::New(context->GetIsolate(), ratio);
        v8::Local<v8::Value> argv[] = { visibleJsVal, ratioJsVal };
        int32_t len = sizeof(argv) / sizeof(argv[0]);
        v8::Local<v8::Value> res;
        bool succ = jsFunc->Call(context, context->Global(), len, argv).ToLocal(&res);
        if (!succ) {
            LOGE("call jsFunc failed!");
            V8Utils::JsStdDumpErrorAce(isolate, &tryCatch, JsErrorType::COMPILE_ERROR, instanceId);
        }
    };

    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    for (int32_t j = 0; j < static_cast<int32_t>(ratios.size()); ++j) {
        (*delegate)->PushJsCallbackToRenderNode(id, ratios[j], jsCallback);
    }
}

void V8ComponentApiBridge::JsVisibleListenerOff(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto observerValue = args.Holder();
    NodeId id = observerValue->Get(context, v8::String::NewFromUtf8(isolate, "__nodeId").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    auto delegate = static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8EngineInstance::FRONTEND_DELEGATE));
    (*delegate)->RemoveVisibleChangeNode(id);
}

} // namespace OHOS::Ace::Framework