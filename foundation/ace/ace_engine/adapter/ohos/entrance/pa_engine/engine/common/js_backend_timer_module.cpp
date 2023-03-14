/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "js_backend_timer_module.h"

#include "base/log/log.h"

namespace OHOS::Ace {
namespace {
template<class T>
inline T* ConvertNativeValueTo(NativeValue* value)
{
    return (value != nullptr) ? static_cast<T*>(value->GetInterface(T::INTERFACE_ID)) : nullptr;
}

void BindNativeFunction(NativeEngine& engine, NativeObject& object, const char* name, NativeCallback func)
{
    object.SetProperty(name, engine.CreateFunction(name, strlen(name), func, nullptr));
}

NativeValue* SetCallbackTimer(NativeEngine& engine, NativeCallbackInfo& info, bool isInterval)
{
    // Parameter check, must have at least 2 params
    if (info.argc < 2 || info.argv[0]->TypeOf() != NATIVE_FUNCTION || info.argv[1]->TypeOf() != NATIVE_NUMBER) {
        LOGE("Set callback timer failed with invalid parameter.");
        return engine.CreateUndefined();
    }

    // Parse parameter
    std::shared_ptr<NativeReference> func(engine.CreateReference(info.argv[0], 1));
    int64_t delayTime = *ConvertNativeValueTo<NativeNumber>(info.argv[1]);
    std::vector<std::shared_ptr<NativeReference>> params;
    for (size_t index = 2; index < info.argc; ++index) {
        params.emplace_back(std::shared_ptr<NativeReference>(engine.CreateReference(info.argv[index], 1)));
    }

    // Get callbackId
    uint32_t callbackId = JsBackendTimerModule::GetInstance()->AddCallBack(func, params);

    // Post task
    JsBackendTimerModule::GetInstance()->PostTimerCallback(callbackId, delayTime, isInterval);

    return engine.CreateNumber(callbackId);
}

NativeValue* ClearCallbackTimer(NativeEngine& engine, NativeCallbackInfo& info)
{
    // parameter check, must have at least 1 param
    if (info.argc < 1 || info.argv[0]->TypeOf() != NATIVE_NUMBER) {
        LOGE("Clear callback timer failed with invalid parameter.");
        return engine.CreateUndefined();
    }

    uint32_t callbackId = *ConvertNativeValueTo<NativeNumber>(info.argv[0]);
    JsBackendTimerModule::GetInstance()->RemoveTimerCallback(callbackId);
    return engine.CreateUndefined();
}

NativeValue* SetTimeout(NativeEngine* engine, NativeCallbackInfo* info)
{
    if (engine == nullptr || info == nullptr) {
        LOGE("Set timeout failed with engine or callback info is nullptr.");
        return nullptr;
    }

    return SetCallbackTimer(*engine, *info, false);
}

NativeValue* SetInterval(NativeEngine* engine, NativeCallbackInfo* info)
{
    if (engine == nullptr || info == nullptr) {
        LOGE("Set interval failed with engine or callback info is nullptr.");
        return nullptr;
    }

    return SetCallbackTimer(*engine, *info, true);
}

NativeValue* ClearTimeoutOrInterval(NativeEngine* engine, NativeCallbackInfo* info)
{
    if (engine == nullptr || info == nullptr) {
        LOGE("Clear timer failed with engine or callback info is nullptr.");
        return nullptr;
    }

    return ClearCallbackTimer(*engine, *info);
}
} // namespace

void JsBackendTimerModule::TimerCallback(uint32_t callbackId, int64_t delayTime, bool isInterval)
{
    if (!nativeEngine_) {
        LOGE("nativeEngine_ is nullptr.");
    }

    std::shared_ptr<NativeReference> func;
    std::vector<std::shared_ptr<NativeReference>> params;
    if (!GetCallBackById(callbackId, func, params)) {
        return;
    }

    std::vector<NativeValue*> argc;
    argc.reserve(params.size());
    for (auto arg : params) {
        argc.emplace_back(arg->Get());
    }

    nativeEngine_->CallFunction(nativeEngine_->CreateUndefined(), func->Get(), argc.data(), argc.size());

    if (isInterval) {
        PostTimerCallback(callbackId, delayTime, isInterval);
    } else {
        RemoveTimerCallback(callbackId);
    }
}

void JsBackendTimerModule::PostTimerCallback(uint32_t callbackId, int64_t delayTime, bool isInterval)
{
    auto taskNode = callbackNodeMap_.find(callbackId);
    if (taskNode == callbackNodeMap_.end()) {
        LOGE("Post timer callback failed, callbackId %{public}u not found.", callbackId);
    }

    // CancelableCallback class can only be executed once.
    CancelableCallback<void()> cancelableTimer;
    cancelableTimer.Reset([callbackId, delayTime, isInterval] {
        JsBackendTimerModule::GetInstance()->TimerCallback(callbackId, delayTime, isInterval);
    });
    taskNode->second.callback = cancelableTimer;

    delegate_->PostDelayedJsTask(cancelableTimer, delayTime);
}

void JsBackendTimerModule::RemoveTimerCallback(uint32_t callbackId)
{
    auto taskNode = callbackNodeMap_.find(callbackId);
    if (taskNode == callbackNodeMap_.end()) {
        LOGE("Remove timer callback failed, callbackId %{public}u not found.", callbackId);
        return;
    }

    taskNode->second.callback.Cancel();
    callbackNodeMap_.erase(taskNode);
}

uint32_t JsBackendTimerModule::AddCallBack(const std::shared_ptr<NativeReference>& func,
    const std::vector<std::shared_ptr<NativeReference>>& params)
{
    ++callbackId_;
    callbackNodeMap_[callbackId_].func = func;
    callbackNodeMap_[callbackId_].params = params;
    return callbackId_;
}

bool JsBackendTimerModule::GetCallBackById(uint32_t callbackId, std::shared_ptr<NativeReference>& func,
    std::vector<std::shared_ptr<NativeReference>>& params)
{
    auto taskNode = callbackNodeMap_.find(callbackId);
    if (taskNode == callbackNodeMap_.end()) {
        LOGE("Get callback failed, callbackId %{public}u not found.", callbackId);
        return false;
    }

    func = taskNode->second.func;
    params = taskNode->second.params;
    return true;
}

JsBackendTimerModule* JsBackendTimerModule::GetInstance()
{
    static JsBackendTimerModule instance;
    return &instance;
}

void JsBackendTimerModule::InitTimerModule(NativeEngine* engine, const RefPtr<BackendDelegate>& delegate)
{
    if (engine == nullptr || delegate == nullptr) {
        LOGE("InitTimerModule failed with engine or delegate is nullptr.");
        return;
    }

    nativeEngine_ = engine;
    delegate_ = delegate;

    NativeObject* globalObject = ConvertNativeValueTo<NativeObject>(nativeEngine_->GetGlobal());
    if (!globalObject) {
        LOGE("Failed to get global object.");
        return;
    }

    BindNativeFunction(*engine, *globalObject, "setTimeout", SetTimeout);
    BindNativeFunction(*engine, *globalObject, "setInterval", SetInterval);
    BindNativeFunction(*engine, *globalObject, "clearTimeout", ClearTimeoutOrInterval);
    BindNativeFunction(*engine, *globalObject, "clearInterval", ClearTimeoutOrInterval);
}
} // namespace OHOS::Ace