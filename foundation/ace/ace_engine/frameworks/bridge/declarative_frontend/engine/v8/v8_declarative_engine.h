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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_DECLARATIVE_ENGINE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_DECLARATIVE_ENGINE_H

#include <mutex>
#include <string>
#include <vector>

#include "third_party/v8/include/libplatform/libplatform.h"
#include "third_party/v8/include/v8.h"

#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "base/utils/noncopyable.h"
#include "core/common/ace_application_info.h"
#include "core/common/ace_page.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"
#include "frameworks/core/components/xcomponent/native_interface_xcomponent_impl.h"

namespace OHOS::Ace::Framework {

template<typename T>
using CopyablePersistent = v8::Persistent<T, v8::CopyablePersistentTraits<T>>;

// IsolateWrapper make sure isolate disposed with thread.
class IsolateWrapper {
public:
    IsolateWrapper() = default;
    explicit IsolateWrapper(v8::Isolate* isolate) : isolate_(isolate) {}
    ~IsolateWrapper()
    {
        LOG_DESTROY();
        if (isolate_) {
            LOGI("Dispose thread isolate.");
            isolate_->Dispose();
        }
    }

    void Wrap(v8::Isolate* isolate)
    {
        if (isolate_ && isolate_ != isolate) {
            isolate_->Dispose();
        }
        isolate_ = isolate;
    }

    void Invalidate()
    {
        valid_ = false;
    }

    bool IsEmpty() const
    {
        return isolate_ == nullptr || !valid_;
    }

    v8::Isolate* operator->() const
    {
        return isolate_;
    }

    v8::Isolate* Get()
    {
        return valid_ ? isolate_ : nullptr;
    }

private:
    bool valid_ = true;
    v8::Isolate* isolate_ = nullptr;
};

class V8DeclarativeEngineInstance final : public AceType, public JsEngineInstance {
    DECLARE_ACE_TYPE(V8DeclarativeEngineInstance, AceType)
public:
    enum IsolateData {
        FRONTEND_DELEGATE = 0,
        RUNNING_PAGE = 1,
        STAGING_PAGE = 2,
        DISPATCHER = 3,
    };

    static void CallJs(v8::Isolate* isolate,
        v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> pcontext, const std::string& callbackId,
        const std::string& args, bool keepAlive = false, bool isGlobal = false);

    explicit V8DeclarativeEngineInstance(const RefPtr<FrontendDelegate>& delegate) : frontendDelegate_(delegate) {}
    ~V8DeclarativeEngineInstance() override;

    void FlushCommandBuffer(void* context, const std::string& command) override;

    static void PersistRootViewHandle(v8::Isolate* isolate, v8::Local<v8::Object> obj);
    void DestroyPersistRootViewHandle(int32_t pageId);
    void DestroyAllPersistRootViewHandle();

    bool InitJSEnv();

    static void InitJsConsoleObject(v8::Local<v8::Context>& context, v8::Isolate* isolate);

    void InitJsPerfUtilObject(v8::Local<v8::Context>& context);

    void InitJsNativeModuleObject(v8::Local<v8::Context>& context);

    void InitJsExportsUtilObject(v8::Local<v8::Context>& context);

    void InitJSContext();

    static void InitAceModules(const char* start, const char* end, v8::Isolate* isolate);

    void InitGlobalObjectTemplate();

    static v8::Isolate* GetV8Isolate()
    {
        return isolate_.Get();
    }

    v8::Local<v8::Context> GetV8Context() const
    {
        return context_.Get(isolate_.Get());
    }

    v8::Persistent<v8::Context>& GetContext()
    {
        return context_;
    }

    bool FireJsEvent(const std::string& param);

    void SetRunningPage(const RefPtr<JsAcePage>& page)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        runningPage_ = page;
        isolate_->SetData(RUNNING_PAGE, static_cast<void*>(&runningPage_));
    }

    static RefPtr<JsAcePage> GetCurrentPage();
    static RefPtr<JsAcePage> GetRunningPage(v8::Isolate* isolate);
    static RefPtr<JsAcePage> GetStagingPage(v8::Isolate* isolate);

    RefPtr<JsAcePage> GetRunningPage() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return runningPage_;
    }

    void SetStagingPage(const RefPtr<JsAcePage>& page)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stagingPage_ = page;
        isolate_->SetData(STAGING_PAGE, static_cast<void*>(&stagingPage_));
    }

    RefPtr<JsAcePage> GetStagingPage() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return stagingPage_;
    }

    void ResetStagingPage(const RefPtr<JsAcePage>& page)
    {
        stagingPage_ = page;
        isolate_->SetData(STAGING_PAGE, static_cast<void*>(&stagingPage_));
    }

    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher)
    {
        dispatcher_ = dispatcher;
        isolate_->SetData(DISPATCHER, static_cast<void*>(&dispatcher_));
    }

    const RefPtr<FrontendDelegate>& GetDelegate() const
    {
        return frontendDelegate_;
    }

    void* GetDelegateForV8Data()
    {
        return static_cast<void*>(&frontendDelegate_);
    }

    void* GetJsMessageDispatcherForV8Data()
    {
        return static_cast<void*>(&dispatcher_);
    }


    void SetDelegate(v8::Isolate* isolate)
    {
        isolate->SetData(FRONTEND_DELEGATE, static_cast<void*>(&frontendDelegate_));
    }

    void SetDispatcher(v8::Isolate* isolate)
    {
        isolate->SetData(DISPATCHER, static_cast<void*>(&dispatcher_));
    }

    static void TriggerPageUpdate();
    static void PostJsTask(std::function<void()>&& task);
    static std::unique_ptr<JsonValue> GetI18nStringResource(const std::string& targetStringKey,
        const std::string& targetStringValue);
    static std::string GetMediaResource(const std::string& targetFileName);
    static RefPtr<PipelineContext> GetPipelineContext();

private:
    static thread_local IsolateWrapper isolate_;
    v8::Isolate::CreateParams create_params_;
    v8::Persistent<v8::Context> context_;
    v8::Persistent<v8::ObjectTemplate> globalObjectTemplate_;
    RefPtr<FrontendDelegate> frontendDelegate_;
    static std::unique_ptr<JsonValue> currentConfigResourceData_;
    static std::map<std::string, std::string> mediaResourceFileMap_;

    // runningPage_ is the page that is loaded and rendered successfully, while stagingPage_ is to
    // handle all page routing situation, which include two stages:
    // - Loading stage: when a new page is loaded by qjs engine but not rendered, stagingPage_ point to
    //   a new created page, which is different with runningPage_, the DOM build operations should call
    //   this one, such as domCreateBody, domAddElement.
    // - Running stage: If the stagingPage_ rendered successfully, the runningPage_ will update to stagingPage_.
    //   If the stagingPage_ render failed, it will reset to runningPage_. So in running stage, runningPage_
    //   and stagingPage_ point to the same page. But it's better to use runningPage_ in dom update tasks,
    //   such as removeElement, updateElementAttrs and updateElementStyles.
    RefPtr<JsAcePage> runningPage_;
    RefPtr<JsAcePage> stagingPage_;

    WeakPtr<JsMessageDispatcher> dispatcher_;
    mutable std::mutex mutex_;

    static thread_local std::unordered_map<int32_t, CopyablePersistent<v8::Object>> persistentRootViewMap_;

    ACE_DISALLOW_COPY_AND_MOVE(V8DeclarativeEngineInstance);
};

class V8DeclarativeEngine : public JsEngine {
    DECLARE_ACE_TYPE(V8DeclarativeEngine, JsEngine)
public:
    explicit V8DeclarativeEngine(int32_t instanceId) : instanceId_(instanceId) {}
    ~V8DeclarativeEngine() override;

    bool Initialize(const RefPtr<FrontendDelegate>& delegate) override;

    // Load and initialize a JS bundle into the JS Framework
    void LoadJs(const std::string& url, const RefPtr<JsAcePage>& page, bool isMainPage) override;

    // Update running page
    void UpdateRunningPage(const RefPtr<JsAcePage>& page) override;

    // Update staging page
    void UpdateStagingPage(const RefPtr<JsAcePage>& page) override;

    // Reset staging page
    void ResetStagingPage() override;

    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher) override;

    // Fire AsyncEvent on JS
    void FireAsyncEvent(const std::string& eventId, const std::string& param) override;

    // Fire SyncEvent on JS
    void FireSyncEvent(const std::string& eventId, const std::string& param) override;

    void FireExternalEvent(const std::string& componentId, const uint32_t nodeId) override;

    // Timer callback
    void TimerCallback(const std::string& callbackId, const std::string& delay, bool isInterval) override;

    // Destroy page instance
    void DestroyPageInstance(int32_t pageId) override;

    // Destroy application instance according to packageName
    void DestroyApplication(const std::string& packageName) override;

    void UpdateApplicationState(const std::string& packageName, Frontend::State state) override;

    void OnWindowDisplayModeChanged(bool isShownInMultiWindow, const std::string& data) override;

    void OnNewWant(const std::string& data) override;

    void OnSaveAbilityState(std::string& data) override;

    void OnRestoreAbilityState(const std::string& data) override;

    void OnConfigurationUpdated(const std::string& data) override;

    bool OnStartContinuation() override;

    void OnRemoteTerminated() override;

    void OnActive() override;

    void OnInactive() override;

    void OnMemoryLevel(const int32_t level) override;

    void OnCompleteContinuation(const int32_t code) override;

    bool OnRestoreData(const std::string& data) override;

    void OnSaveData(std::string& saveData) override;

    void MediaQueryCallback(const std::string& callbackId, const std::string& args) override;

    void RequestAnimationCallback(const std::string& callbackId, uint64_t timeStamp) override;

    void JsCallback(const std::string& callbackId, const std::string& args) override;

    void RunGarbageCollection() override;

    void NotifyAppStorage(const std::string& key, const std::string& value) override;

    RefPtr<GroupJsBridge> GetGroupJsBridge() override;

    FrontendDelegate* GetFrontend() override
    {
        return AceType::RawPtr(engineInstance_->GetDelegate());
    }

    static std::unique_ptr<v8::Platform>& GetPlatform();

    v8::Local<v8::Object> GetRenderContext() const
    {
        return renderContextXComp_.Get(isolateXComp_);
    }

private:
    void CallAppFunc(v8::Isolate* isolate, const v8::Local<v8::Context>& context, std::string appFuncName);

    bool CallAppFunc(v8::Isolate* isolate, const v8::Local<v8::Context>& context, std::string appFuncName,
        int argc, v8::Local<v8::Value>* argv);

    void RegisterWorker();

    void RegisterInitWorkerFunc();

    void RegisterAssetFunc();

    void RegisterOffWorkerFunc();

    void SetPostTask(NativeEngine* nativeEngine);

    void TimerCallJs(const std::string& callbackId, bool isInterval);

    void InitXComponent();

    RefPtr<V8DeclarativeEngineInstance> engineInstance_;

    RefPtr<NativeXComponentImpl> nativeXComponentImpl_;

    OH_NativeXComponent *nativeXComponent_ = nullptr;

    int32_t instanceId_ = 0;
    v8::Isolate* isolateXComp_ = nullptr;
    v8::Persistent<v8::Object> renderContextXComp_;
    v8::Persistent<v8::Context> ctxXComp_;

    ACE_DISALLOW_COPY_AND_MOVE(V8DeclarativeEngine);
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_DECLARATIVE_ENGINE_H
