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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_ENGINE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_ENGINE_H

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "third_party/v8/include/libplatform/libplatform.h"
#include "third_party/v8/include/v8.h"

#include "base/memory/ace_type.h"
#include "base/utils/noncopyable.h"
#include "core/common/ace_application_info.h"
#include "core/common/ace_page.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"
#include "native_engine/impl/v8/v8_native_engine.h"

namespace OHOS::Ace::Framework {

class V8EngineInstance final : public AceType, public JsEngineInstance {
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

    explicit V8EngineInstance(const RefPtr<FrontendDelegate>& delegate, int32_t instanceId)
        : frontendDelegate_(delegate), instanceId_(instanceId)
    {}
    ~V8EngineInstance() override;

    void FlushCommandBuffer(void* context, const std::string& command) override;

    bool InitJsEnv();

    void InitJsConsoleObject(v8::Local<v8::Context>& context, v8::Isolate* isolate);
    void InitJsDocumentObject(v8::Local<v8::Context>& context, v8::Isolate* isolate);

    void InitJsPerfUtilObject(v8::Local<v8::Context>& context);

    void InitJsContext();

    void InitGlobalObjectTemplate();

    void CallJs(const std::string& callbackId, const std::string& args, bool keepAlive = false, bool isGlobal = false);

    v8::Isolate* GetV8Isolate() const
    {
        return isolate_;
    }

    v8::Local<v8::Context> GetV8Context() const
    {
        return context_.Get(isolate_);
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

    WeakPtr<JsMessageDispatcher> GetJsMessageDispatcher() const
    {
        return dispatcher_;
    }

    int32_t GetInstanceId() const
    {
        return instanceId_;
    }

    RefPtr<FrontendDelegate> GetDelegate() const
    {
        return frontendDelegate_;
    }

    void SetV8NativeEngine(std::shared_ptr<V8NativeEngine> nativeEngine)
    {
        nativeEngine_ = nativeEngine;
    }

    std::shared_ptr<V8NativeEngine> GetV8NativeEngine() const
    {
        return nativeEngine_.lock();
    }

private:
    v8::Isolate* isolate_ = nullptr;
    v8::Isolate::CreateParams create_params_;
    v8::Persistent<v8::Context> context_;
    v8::Persistent<v8::ObjectTemplate> globalObjectTemplate_;
    RefPtr<FrontendDelegate> frontendDelegate_;
    int32_t instanceId_ = 0;

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
    std::weak_ptr<V8NativeEngine> nativeEngine_;
    mutable std::mutex mutex_;

    ACE_DISALLOW_COPY_AND_MOVE(V8EngineInstance);
};

class V8Engine : public JsEngine {
public:
    explicit V8Engine(int32_t instanceId);
    ~V8Engine() override;

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

    // Fire external event on JS thread
    void FireExternalEvent(const std::string& componentId, const uint32_t nodeId) override;

    // Timer callback
    void TimerCallback(const std::string& callbackId, const std::string& delay, bool isInterval) override;

    // Destroy page instance
    void DestroyPageInstance(int32_t pageId) override;

    // Destroy application instance according to packageName
    void DestroyApplication(const std::string& packageName) override {}

    void UpdateApplicationState(const std::string& packageName, Frontend::State state) override;

    void MediaQueryCallback(const std::string& callbackId, const std::string& args) override;

    void RequestAnimationCallback(const std::string& callbackId, uint64_t timeStamp) override;

    void JsCallback(const std::string& callbackId, const std::string& args) override;

    void RunGarbageCollection() override;

    RefPtr<GroupJsBridge> GetGroupJsBridge() override;

    FrontendDelegate* GetFrontend() override
    {
        return AceType::RawPtr(engineInstance_->GetDelegate());
    }

    void RunNativeEngineLoop() override
    {
        if (nativeEngine_ != nullptr) {
            nativeEngine_->Loop(LOOP_NOWAIT, false);
        }
    }

    static std::unique_ptr<v8::Platform>& GetPlatform();

private:
    void GetLoadOptions(std::string& optionStr, bool isMainPage, const RefPtr<JsAcePage>& page);
    void RegisterWorker();
    void RegisterInitWorkerFunc();
    void RegisterAssetFunc();
    void RegisterOffWorkerFunc();
    void SetPostTask();

    RefPtr<V8EngineInstance> engineInstance_;
    int32_t instanceId_ = 0;
    std::shared_ptr<V8NativeEngine> nativeEngine_;

    ACE_DISALLOW_COPY_AND_MOVE(V8Engine);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_V8_ENGINE_H
