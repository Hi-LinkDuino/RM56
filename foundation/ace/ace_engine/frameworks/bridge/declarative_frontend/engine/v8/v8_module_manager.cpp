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

#include "frameworks/bridge/declarative_frontend/engine/v8/v8_module_manager.h"

#include "base/log/log.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_declarative_engine.h"
#include "frameworks/bridge/declarative_frontend/engine/v8/v8_js_matrix4.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/core/animation/cubic_curve.h"
#include "frameworks/core/animation/spring_curve.h"

namespace OHOS::Ace::Framework {
ModuleManager* ModuleManager::GetInstance()
{
    static ModuleManager instance;
    return &instance;
}

void PagePush(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("context is empty!");
        return;
    }
    v8::Local<v8::String> value = v8::JSON::Stringify(context, args[0]).ToLocalChecked();
    v8::String::Utf8Value argsJsStr(isolate, value);
    if (!(*argsJsStr)) {
        LOGE("PushPage method args is invalid");
        return;
    }

    std::string argsStr(*argsJsStr);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(*argsJsStr);

    std::string uri;
    if (argsPtr != nullptr && argsPtr->GetValue(ROUTE_KEY_URI)->IsString()) {
        uri = argsPtr->GetValue(ROUTE_KEY_URI)->GetString();
    }

    std::string params;
    if (argsPtr != nullptr && argsPtr->GetValue(ROUTE_KEY_PARAMS)->IsObject()) {
        params = argsPtr->GetValue(ROUTE_KEY_PARAMS)->ToString();
    }

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    (*delegate)->Push(uri, params);
}

void PageReplace(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("context is empty!");
        return;
    }
    v8::Local<v8::String> value = v8::JSON::Stringify(context, args[0]).ToLocalChecked();
    v8::String::Utf8Value argsJsStr(isolate, value);
    if (!(*argsJsStr)) {
        LOGE("PageReplace method args is invalid");
        return;
    }

    std::string argsStr(*argsJsStr);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(*argsJsStr);

    std::string uri;
    if (argsPtr != nullptr && argsPtr->GetValue(ROUTE_KEY_URI)->IsString()) {
        uri = argsPtr->GetValue(ROUTE_KEY_URI)->GetString();
    }

    std::string params;
    if (argsPtr != nullptr && argsPtr->GetValue(ROUTE_KEY_PARAMS)->IsObject()) {
        params = argsPtr->GetValue(ROUTE_KEY_PARAMS)->ToString();
    }

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    (*delegate)->Replace(uri, params);
}

void PageBack(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("context is empty!");
        return;
    }
    v8::Local<v8::String> value = v8::JSON::Stringify(context, args[0]).ToLocalChecked();
    v8::String::Utf8Value argsJsStr(isolate, value);
    if (!(*argsJsStr)) {
        LOGE("PageBack method args is invalid");
        return;
    }

    std::string argsStr(*argsJsStr);
    std::unique_ptr<JsonValue> argsPtr = JsonUtil::ParseJsonString(*argsJsStr);

    std::string uri;
    if (argsPtr != nullptr && argsPtr->GetValue(ROUTE_KEY_URI)->IsString()) {
        uri = argsPtr->GetValue(ROUTE_KEY_URI)->GetString();
    }
    std::string params;
    if (argsPtr != nullptr && argsPtr->GetValue(ROUTE_KEY_PARAMS)->IsObject()) {
        params = argsPtr->GetValue(ROUTE_KEY_PARAMS)->ToString();
    }

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    (*delegate)->Back(uri, params);
}

void PageClear(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    (*delegate)->Clear();
}

void PageGetLength(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    int32_t routeLength = (*delegate)->GetStackSize();
    std::string indexLength = std::to_string(routeLength);
    v8::Local<v8::String> lenStr =
        v8::String::NewFromUtf8(isolate, indexLength.c_str()).ToLocalChecked();
    args.GetReturnValue().Set(lenStr);
}

void PageGetState(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);

    int32_t routeIndex = 0;
    std::string routeName;
    std::string routePath;
    ACE_DCHECK(isolate);
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    (*delegate)->GetState(routeIndex, routeName, routePath);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("get router state, context is empty!");
        return;
    }
    v8::Local<v8::Object> res = v8::Object::New(isolate);
    v8::Local<v8::Integer> index = v8::Integer::New(isolate, routeIndex);
    v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, routeName.c_str()).ToLocalChecked();
    v8::Local<v8::String> path = v8::String::NewFromUtf8(isolate, routePath.c_str()).ToLocalChecked();
    res->Set(context, v8::String::NewFromUtf8(isolate, "index").ToLocalChecked(), index).ToChecked();
    res->Set(context, v8::String::NewFromUtf8(isolate, "name").ToLocalChecked(), name).ToChecked();
    res->Set(context, v8::String::NewFromUtf8(isolate, "path").ToLocalChecked(), path).ToChecked();
    args.GetReturnValue().Set(res);
}

void PageGetParams(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    std::string paramsStr = (*delegate)->GetParams();
    if (paramsStr.empty()) {
        LOGI("PageGetParams params is null");
        args.GetReturnValue().Set(v8::Null(isolate));
        return;
    }

    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("get router state, context is empty!");
        return;
    }
    v8::Local<v8::String> paramsRes = v8::String::NewFromUtf8(isolate, paramsStr.c_str()).ToLocalChecked();
    v8::Local<v8::Value> res = v8::JSON::Parse(context, paramsRes).ToLocalChecked();
    args.GetReturnValue().Set(res);
}

void PostponePageTransition(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("context is empty!");
        return;
    }
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    (*delegate)->PostponePageTransition();
}

void LaunchPageTransition(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("context is empty!");
        return;
    }
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    (*delegate)->LaunchPageTransition();
}

void InitRouterModule(v8::Local<v8::Object> moduleObj, v8::Isolate* isolate)
{
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("context is empty!");
        return;
    }
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, ROUTE_PAGE_PUSH).ToLocalChecked(),
        v8::Function::New(context, PagePush).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, ROUTE_PAGE_REPLACE).ToLocalChecked(),
        v8::Function::New(context, PageReplace).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, ROUTE_PAGE_BACK).ToLocalChecked(),
        v8::Function::New(context, PageBack).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, ROUTE_PAGE_CLEAR).ToLocalChecked(),
        v8::Function::New(context, PageClear).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, ROUTE_PAGE_GET_LENGTH).ToLocalChecked(),
        v8::Function::New(context, PageGetLength).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, ROUTE_PAGE_GET_STATE).ToLocalChecked(),
        v8::Function::New(context, PageGetState).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, ROUTE_PAGE_GET_PARAMS).ToLocalChecked(),
        v8::Function::New(context, PageGetParams).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, ROUTE_POSTPONE).ToLocalChecked(),
                   v8::Function::New(context, PostponePageTransition).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, ROUTE_LAUNCH).ToLocalChecked(),
                   v8::Function::New(context, LaunchPageTransition).ToLocalChecked()).ToChecked();
}

void AppGetInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    LOGD("Enter GetAppInfo");
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("get app info, context is empty!");
        return;
    }

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    v8::Local<v8::String> appID =
        v8::String::NewFromUtf8(isolate, (*delegate)->GetAppID().c_str()).ToLocalChecked();
    v8::Local<v8::String> appName =
        v8::String::NewFromUtf8(isolate, (*delegate)->GetAppName().c_str()).ToLocalChecked();
    v8::Local<v8::String> versionName =
        v8::String::NewFromUtf8(isolate, (*delegate)->GetVersionName().c_str()).ToLocalChecked();
    v8::Local<v8::Integer> versionCode = v8::Integer::New(isolate, (*delegate)->GetVersionCode());

    // return the result as an object
    v8::Local<v8::Object> res = v8::Object::New(isolate);
    res->Set(context, v8::String::NewFromUtf8(isolate, "appID").ToLocalChecked(), appID).ToChecked();
    res->Set(context, v8::String::NewFromUtf8(isolate, "appName").ToLocalChecked(), appName).ToChecked();
    res->Set(context, v8::String::NewFromUtf8(isolate, "versionName").ToLocalChecked(), versionName).ToChecked();
    res->Set(context, v8::String::NewFromUtf8(isolate, "versionCode").ToLocalChecked(), versionCode).ToChecked();
    args.GetReturnValue().Set(res);
}

void AppTerminate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);

    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    WeakPtr<PipelineContext> pipelineContextWeak = (*delegate)->GetPipelineContext();
    auto uiTaskExecutor = (*delegate)->GetUiTask();
    uiTaskExecutor.PostTask([pipelineContextWeak]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            pipelineContext->Finish();
        }
    });
}

void AppSetImageCacheCount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (args.Length() != 1 || !args[0]->IsNumber()) {
        LOGE("The arguments must be one int number.");
        return;
    }
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    WeakPtr<PipelineContext> pipelineContextWeak = (*delegate)->GetPipelineContext();
    auto uiTaskExecutor = (*delegate)->GetUiTask();
    int32_t size = args[0]->Int32Value(context).ToChecked();
    size = size > 0 ? size : 0;
    uiTaskExecutor.PostTask([pipelineContextWeak, size]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            auto imageCache = pipelineContext->GetImageCache();
            if (imageCache) {
                imageCache->SetCapacity(size);
            } else {
                LOGW("image cache is null");
            }
        }
    });
}

void AppSetImageRawDataCacheSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (args.Length() != 1 || !args[0]->IsNumber()) {
        LOGE("The arguments must be one int number.");
        return;
    }
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    WeakPtr<PipelineContext> pipelineContextWeak = (*delegate)->GetPipelineContext();
    auto uiTaskExecutor = (*delegate)->GetUiTask();
    int32_t size = args[0]->Int32Value(context).ToChecked();
    size = size > 0 ? size : 0;
    uiTaskExecutor.PostTask([pipelineContextWeak, size]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            auto imageCache = pipelineContext->GetImageCache();
            if (imageCache) {
                imageCache->SetDataCacheLimit(size);
            } else {
                LOGW("image cache is null");
            }
        }
    });
}

void AppSetImageFileCacheSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (args.Length() != 1 || !args[0]->IsNumber()) {
        LOGE("The arguments must be one int number.");
        return;
    }
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    WeakPtr<PipelineContext> pipelineContextWeak = (*delegate)->GetPipelineContext();
    auto uiTaskExecutor = (*delegate)->GetUiTask();
    int32_t size = args[0]->Int32Value(context).ToChecked();
    size = size > 0 ? size : 0;
    uiTaskExecutor.PostTask([pipelineContextWeak, size]() mutable {
        auto pipelineContext = pipelineContextWeak.Upgrade();
        if (pipelineContext) {
            auto imageCache = pipelineContext->GetImageCache();
            if (imageCache) {
                imageCache->SetCacheFileLimit(size);
            } else {
                LOGW("image cache is null");
            }
        }
    });
}

void InitAppModule(v8::Local<v8::Object> moduleObj, v8::Isolate* isolate)
{
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("context is empty!");
        return;
    }
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, APP_GET_INFO).ToLocalChecked(),
                   v8::Function::New(context, AppGetInfo).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, APP_TERMINATE).ToLocalChecked(),
                   v8::Function::New(context, AppTerminate).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, APP_SET_IMAGE_CACHE_COUNT).ToLocalChecked(),
                   v8::Function::New(context, AppSetImageCacheCount).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, APP_SET_IMAGE_RAWDATA_CACHE_SIZE).ToLocalChecked(),
                   v8::Function::New(context, AppSetImageRawDataCacheSize).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, APP_SET_IMAGE_FILE_CACHE_SIZE).ToLocalChecked(),
                   v8::Function::New(context, AppSetImageFileCacheSize).ToLocalChecked()).ToChecked();
}

void CurvesInterpolate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (args.Length() != 1) {
        return;
    }
    double time = args[0]->NumberValue(context).ToChecked();
    int32_t pageId = args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__pageId").ToLocalChecked())
        .ToLocalChecked()->Int32Value(context).ToChecked();
    v8::Local<v8::Value> value = args.Holder()->Get(context, v8::String::NewFromUtf8(isolate, "__curveString").
        ToLocalChecked()).ToLocalChecked();
    v8::String::Utf8Value curve(isolate, value->ToString(context).ToLocalChecked());
    if (!(*curve)) {
        return;
    }
    std::string curveString(*curve);
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    auto page = (*delegate)->GetPage(pageId);
    if (!page) {
        LOGE("get page failed, page is nullptr");
        return;
    }
    auto animationCurve = page->GetCurve(curveString);
    if (!animationCurve) {
        return;
    }
    double curveValue = animationCurve->Move(time);
    args.GetReturnValue().Set(v8::Number::New(isolate, curveValue));
}

void CurvesInit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto curveContext = v8::Object::New(isolate);
    curveContext->Set(context, v8::String::NewFromUtf8(isolate, "interpolate").ToLocalChecked(),
        v8::Function::New(context, CurvesInterpolate).ToLocalChecked()).ToChecked();
    if (args.Length() != 0 && args.Length() != 1) {
        return;
    }
    RefPtr<Curve> curve;
    std::string curveString;
    if (args.Length() == 1 && args[0]->IsString()) {
        v8::String::Utf8Value arg(isolate, args[0]->ToString(context).ToLocalChecked());
        if (!*arg) {
            return;
        }
        curveString = *arg;
    } else {
        curveString = "linear";
    }
    curve = CreateCurve(curveString);

    auto page =
        static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8DeclarativeEngineInstance::STAGING_PAGE));
    if ((*page) == nullptr) {
        LOGE("page is nullptr");
        return;
    }
    (*page)->AddCurve(curveString, curve);
    int32_t pageId = (*page)->GetPageId();
    curveContext->Set(context, v8::String::NewFromUtf8(isolate, "__pageId").ToLocalChecked(),
        v8::Int32::New(isolate, pageId)).ToChecked();
    curveContext->Set(context, v8::String::NewFromUtf8(isolate, "__curveString").ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, curveString.c_str()).ToLocalChecked()).ToChecked();
    args.GetReturnValue().Set(curveContext);
}

void ParseCurves(const v8::FunctionCallbackInfo<v8::Value>& args, std::string& curveString)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    auto curveContext = v8::Object::New(isolate);
    curveContext->Set(context, v8::String::NewFromUtf8(isolate, "interpolate").ToLocalChecked(),
        v8::Function::New(context, CurvesInterpolate).ToLocalChecked()).ToChecked();
    if (args.Length() != 4) {
        LOGE("the number of parameters is illegal");
        return;
    }
    double x0 = args[0]->NumberValue(context).ToChecked();
    double y0 = args[1]->NumberValue(context).ToChecked();
    double x1 = args[2]->NumberValue(context).ToChecked();
    double y1 = args[3]->NumberValue(context).ToChecked();
    RefPtr<Curve> curve;
    if (curveString == "spring") {
        curve = AceType::MakeRefPtr<SpringCurve>(x0, y0, x1, y1);
    } else if (curveString == "cubic-bezier") {
        curve = AceType::MakeRefPtr<CubicCurve>(x0, y0, x1, y1);
    } else {
        LOGE("curve params: %{public}s is illegal", curveString.c_str());
        return;
    }
    auto customCurve = curve->ToString();
    auto page =
        static_cast<RefPtr<JsAcePage>*>(isolate->GetData(V8DeclarativeEngineInstance::STAGING_PAGE));
    if ((*page) == nullptr) {
        LOGE("page is nullptr");
        return;
    }
    (*page)->AddCurve(customCurve, curve);
    int32_t pageId = (*page)->GetPageId();

    curveContext->Set(context, v8::String::NewFromUtf8(isolate, "__pageId").ToLocalChecked(),
        v8::Int32::New(isolate, pageId)).ToChecked();
    curveContext->Set(context, v8::String::NewFromUtf8(isolate, "__curveString").ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, customCurve.c_str()).ToLocalChecked()).ToChecked();
    args.GetReturnValue().Set(curveContext);
}

void CurvesBezier(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::string curveString("cubic-bezier");
    ParseCurves(args, curveString);
}

void CurvesSpring(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::string curveString("spring");
    ParseCurves(args, curveString);
}

void InitCurvesModule(v8::Local<v8::Object> moduleObj, v8::Isolate* isolate)
{
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("context is empty!");
        return;
    }
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, "init").ToLocalChecked(),
                   v8::Function::New(context, CurvesInit).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, "cubicBezier").ToLocalChecked(),
                   v8::Function::New(context, CurvesBezier).ToLocalChecked()).ToChecked();
    moduleObj->Set(context, v8::String::NewFromUtf8(isolate, "spring").ToLocalChecked(),
                   v8::Function::New(context, CurvesSpring).ToLocalChecked()).ToChecked();
}

void InitMatrix(v8::Local<v8::Object> moduleObj, v8::Isolate* isolate)
{
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("context is empty!");
        return;
    }

    JSMatrix4::CreateMatrix4Object(moduleObj, isolate);
}

void SetTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ModuleManager::GetInstance()->SetWaitTimer(args, false);
}

void SetInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ModuleManager::GetInstance()->SetWaitTimer(args, true);
}

void ModuleManager::SetWaitTimer(const v8::FunctionCallbackInfo<v8::Value>& args, bool isInterval)
{
    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("set time out or interval, context is empty!");
        return;
    }

    LOGI("Enter SetWaitTimer %{private}d %{private}d", isInterval, args.Length());

    if (!args[0]->IsFunction()) {
        LOGE("args[0] is not function");
        return;
    }
    v8::Local<v8::Function> jsFunc = v8::Local<v8::Function>::Cast(args[0]);

    int index = 1;
    uint32_t delay = 0;
    if (args[1]->IsNumber()) {
        LOGE("args[1] is number");
        delay = args[1]->ToInt32(context).ToLocalChecked()->Value();
        index = 2;
    }

    CopyablePersistent<v8::Function> pFunction;
    pFunction.Reset(isolate, jsFunc);

    std::vector<v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>>> callbackArray;
    while (index < args.Length()) {
        CopyablePersistent<v8::Value> pValue;
        pValue.Reset(isolate, args[index]);
        callbackArray.emplace_back(pValue);
        ++index;
    }

    uint32_t callbackId = ModuleManager::GetInstance()->AddCallback(pFunction, callbackArray, isolate, isInterval);

    args.GetReturnValue().Set(v8::Integer::New(isolate, callbackId));
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    (*delegate)->WaitTimer(std::to_string(callbackId), std::to_string(delay), isInterval, true);
}

void ClearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ModuleManager::GetInstance()->ClearWaitTimer(args, false);
}

void ClearInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ModuleManager::GetInstance()->ClearWaitTimer(args, true);
}

void ModuleManager::ClearWaitTimer(const v8::FunctionCallbackInfo<v8::Value>& args, bool isInterval)
{
    LOGD("Enter ClearWaitTimer %{private}d", isInterval);

    v8::Isolate* isolate = args.GetIsolate();
    ACE_DCHECK(isolate);
    v8::HandleScope handleScope(isolate);
    auto context = isolate->GetCurrentContext();
    if (context.IsEmpty()) {
        LOGE("set timeout or interval, context is empty!");
        return;
    }

    if (!args[0]->IsNumber()) {
        LOGE("args[0] is not number");
        return;
    }

    uint32_t callbackId = args[0]->ToInt32(context).ToLocalChecked()->Value();
    ModuleManager::GetInstance()->RemoveCallbackFunc(callbackId, isInterval);
    auto delegate =
        static_cast<RefPtr<FrontendDelegate>*>(isolate->GetData(V8DeclarativeEngineInstance::FRONTEND_DELEGATE));
    (*delegate)->ClearTimer(std::to_string(callbackId));
}

uint32_t ModuleManager::AddCallback(CopyablePersistent<v8::Function> callbackFunc,
    std::vector<CopyablePersistent<v8::Value>> callbackArray, v8::Isolate* isolate, bool isInterval)
{
    if (isInterval) {
        return AddCallback(intervalCallbackFuncMap_, intervalCallbackArrayMap_, intervalCallbackIsolateMap_,
            callbackFunc, callbackArray, isolate);
    } else {
        return AddCallback(
            callbackFuncMap_, callbackArrayMap_, callbackIsolateMap_, callbackFunc, callbackArray, isolate);
    }
}

uint32_t ModuleManager::AddCallback(std::unordered_map<uint32_t, CopyablePersistent<v8::Function>>& callbackFuncMap,
    std::unordered_map<uint32_t, std::vector<CopyablePersistent<v8::Value>>>& callbackArrayMap,
    std::unordered_map<uint32_t, v8::Isolate*>& callbackIsolateMap, CopyablePersistent<v8::Function> callbackFunc,
    std::vector<CopyablePersistent<v8::Value>> callbackArray, v8::Isolate* isolate)
{
    ++callbackId_;
    callbackFuncMap[callbackId_] = callbackFunc;
    if (!callbackArray.empty()) {
        callbackArrayMap[callbackId_] = callbackArray;
    }
    callbackIsolateMap[callbackId_] = isolate;
    return callbackId_;
}

void ModuleManager::RemoveCallbackFunc(uint32_t callbackId, bool isInterval)
{
    if (isInterval) {
        RemoveCallbackFunc(
            intervalCallbackFuncMap_, intervalCallbackArrayMap_, intervalCallbackIsolateMap_, callbackId);
    } else {
        RemoveCallbackFunc(callbackFuncMap_, callbackArrayMap_, callbackIsolateMap_, callbackId);
    }
}

void ModuleManager::RemoveCallbackFunc(std::unordered_map<uint32_t, CopyablePersistent<v8::Function>>& callbackFuncMap,
    std::unordered_map<uint32_t, std::vector<CopyablePersistent<v8::Value>>>& callbackArrayMap,
    std::unordered_map<uint32_t, v8::Isolate*>& callbackIsolateMap, uint32_t callbackId)
{
    callbackFuncMap[callbackId].Reset();
    callbackFuncMap.erase(callbackId);

    if (callbackArrayMap.find(callbackId) != callbackArrayMap.end()) {
        uint32_t index = 0;
        while (index < callbackArrayMap[callbackId].size()) {
            callbackArrayMap[callbackId][index].Reset();
            ++index;
        }
        callbackArrayMap.erase(callbackId);
    }
    callbackIsolateMap.erase(callbackId);
}

CopyablePersistent<v8::Function> ModuleManager::GetCallbackFunc(uint32_t callbackId, bool isInterval)
{
    if (isInterval) {
        return intervalCallbackFuncMap_[callbackId];
    } else {
        return callbackFuncMap_[callbackId];
    }
}

std::vector<CopyablePersistent<v8::Value>> ModuleManager::GetCallbackArray(uint32_t callbackId, bool isInterval)
{
    if (isInterval) {
        return GetCallbackArray(intervalCallbackArrayMap_, callbackId);
    } else {
        return GetCallbackArray(callbackArrayMap_, callbackId);
    }
}

std::vector<CopyablePersistent<v8::Value>> ModuleManager::GetCallbackArray(
    std::unordered_map<uint32_t, std::vector<CopyablePersistent<v8::Value>>>& callbackArrayMap, uint32_t callbackId)
{
    if (callbackArrayMap.find(callbackId) != callbackArrayMap.end()) {
        return callbackArrayMap[callbackId];
    }
    std::vector<CopyablePersistent<v8::Value>> emptyRet;
    return emptyRet;
}

v8::Isolate* ModuleManager::GetCallbackIsolate(uint32_t callbackId, bool isInterval)
{
    if (isInterval) {
        return intervalCallbackIsolateMap_[callbackId];
    } else {
        return callbackIsolateMap_[callbackId];
    }
}

void ModuleManager::ClearTimerIsolate(v8::Isolate* isolate)
{
    for (auto it = callbackIsolateMap_.begin(); it != callbackIsolateMap_.end();) {
        if (it->second == isolate) {
            uint32_t index = it->first;
            callbackFuncMap_.erase(index);
            callbackArrayMap_.erase(index);
            callbackIsolateMap_.erase(it++);
        } else {
            it++;
        }
    }
    for (auto it = intervalCallbackIsolateMap_.begin(); it != intervalCallbackIsolateMap_.end();) {
        if (it->second == isolate) {
            uint32_t index = it->first;
            intervalCallbackFuncMap_.erase(index);
            intervalCallbackArrayMap_.erase(index);
            intervalCallbackIsolateMap_.erase(it++);
        } else {
            it++;
        }
    }
}

void ModuleManager::InitTimerModule(v8::Local<v8::Context>& localContext)
{
    v8::Isolate* isolate = localContext->GetIsolate();
    v8::Local<v8::Object> global = localContext->Global();
    global
        ->Set(localContext, v8::String::NewFromUtf8(isolate, SET_TIMEOUT).ToLocalChecked(),
              v8::Function::New(localContext, SetTimeout).ToLocalChecked()).ToChecked();
    global
        ->Set(localContext, v8::String::NewFromUtf8(isolate, SET_INTERVAL).ToLocalChecked(),
              v8::Function::New(localContext, SetInterval).ToLocalChecked()).ToChecked();
    global
        ->Set(localContext, v8::String::NewFromUtf8(isolate, CLEAR_TIMEOUT).ToLocalChecked(),
              v8::Function::New(localContext, ClearTimeout).ToLocalChecked()).ToChecked();
    global
        ->Set(localContext, v8::String::NewFromUtf8(isolate, CLEAR_INTERVAL).ToLocalChecked(),
              v8::Function::New(localContext, ClearInterval).ToLocalChecked()).ToChecked();
}

bool ModuleManager::InitModule(v8::Local<v8::Object> moduleObj, const std::string& moduleName, v8::Isolate* isolate)
{
    static const std::unordered_map<std::string, void (*)(v8::Local<v8::Object>, v8::Isolate*)> MODULE_LIST = {
        { "system.router",
            [](v8::Local<v8::Object> moduleObj, v8::Isolate* isolate) { InitRouterModule(moduleObj, isolate); } },
        { "ohos.router",
            [](v8::Local<v8::Object> moduleObj, v8::Isolate* isolate) { InitRouterModule(moduleObj, isolate); } },
        { "system.app",
            [](v8::Local<v8::Object> moduleObj, v8::Isolate* isolate) { InitAppModule(moduleObj, isolate); } },
        { "ohos.app",
            [](v8::Local<v8::Object> moduleObj, v8::Isolate* isolate) { InitAppModule(moduleObj, isolate); } },
        { "system.curves",
            [](v8::Local<v8::Object> moduleObj, v8::Isolate* isolate) { InitCurvesModule(moduleObj, isolate); } },
        { "ohos.curves",
            [](v8::Local<v8::Object> moduleObj, v8::Isolate* isolate) { InitCurvesModule(moduleObj, isolate); } },
        { "system.matrix4",
            [](v8::Local<v8::Object> moduleObj, v8::Isolate* isolate) { InitMatrix(moduleObj, isolate); } },
        { "ohos.matrix4",
            [](v8::Local<v8::Object> moduleObj, v8::Isolate* isolate) { InitMatrix(moduleObj, isolate); } },
    };
    auto iter = MODULE_LIST.find(moduleName);
    if (iter != MODULE_LIST.end()) {
        iter->second(moduleObj, isolate);
        return true;
    } else {
        LOGE("register module is not found");
        return false;
    }
}
} // namespace OHOS::Ace::Framework
