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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_DECLARATIVE_ENGINE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_DECLARATIVE_ENGINE_H

#include <mutex>
#include <string>
#include <vector>

#include "ecmascript/napi/include/jsnapi.h"
#include "native_engine/impl/ark/ark_native_engine.h"

#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "base/subwindow/subwindow_manager.h"
#include "base/utils/noncopyable.h"
#include "core/common/ace_application_info.h"
#include "core/common/ace_page.h"
#include "core/components/xcomponent/native_interface_xcomponent_impl.h"
#include "frameworks/bridge/js_frontend/engine/jsi/js_runtime.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine.h"
#include "frameworks/bridge/js_frontend/js_ace_page.h"

namespace OHOS::Ace::Framework {

class JsiDeclarativeEngineInstance final : public AceType, public JsEngineInstance {
    DECLARE_ACE_TYPE(JsiDeclarativeEngineInstance, AceType)
public:
    JsiDeclarativeEngineInstance(const RefPtr<FrontendDelegate>& delegate) : frontendDelegate_(delegate)
    {}
    ~JsiDeclarativeEngineInstance() override;

    void FlushCommandBuffer(void* context, const std::string& command) override;

    bool InitJsEnv(bool debuggerMode, const std::unordered_map<std::string, void*>& extraNativeObject,
        const shared_ptr<JsRuntime>& runtime = nullptr);

    bool FireJsEvent(const std::string& eventStr);

    // add Console object to worker
    void InitConsoleModule(ArkNativeEngine* engine);

    static void RootViewHandle(panda::Local<panda::ObjectRef> value);
    void DestroyRootViewHandle(int32_t pageId);
    void DestroyAllRootViewHandle();

    static std::unique_ptr<JsonValue> GetI18nStringResource(
        const std::string& targetStringKey, const std::string& targetStringValue);
    static std::string GetMediaResource(const std::string& targetFileName);

    static RefPtr<JsAcePage> GetRunningPage(int32_t instanceId);
    static RefPtr<JsAcePage> GetStagingPage(int32_t instanceId);
    static shared_ptr<JsRuntime> GetCurrentRuntime();
    static void PostJsTask(const shared_ptr<JsRuntime>&, std::function<void()>&& task);
    static void TriggerPageUpdate(const shared_ptr<JsRuntime>&);
    static RefPtr<PipelineContext> GetPipelineContext(const shared_ptr<JsRuntime>& runtime);
    static void PreloadAceModule(void* runtime);

    WeakPtr<JsMessageDispatcher> GetJsMessageDispatcher() const
    {
        return dispatcher_;
    }

    void SetRunningPage(const RefPtr<JsAcePage>& page)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        runningPage_ = page;
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
    }

    RefPtr<JsAcePage> GetStagingPage() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return stagingPage_;
    }

    void ResetStagingPage(const RefPtr<JsAcePage>& page)
    {
        stagingPage_ = page;
    }

    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher)
    {
        dispatcher_ = dispatcher;
    }

    RefPtr<FrontendDelegate> GetDelegate() const
    {
        return frontendDelegate_;
    }

    shared_ptr<JsRuntime> GetJsRuntime()
    {
        return runtime_;
    }

    void SetDebugMode(bool isDebugMode)
    {
        isDebugMode_ = isDebugMode;
    }

    void SetRootView(int32_t pageId, panda::Global<panda::ObjectRef> value)
    {
        rootViewMap_.emplace(pageId, value);
    }

private:
    void InitGlobalObjectTemplate();
    void InitConsoleModule();  // add Console object to global
    void InitAceModule();      // add ace object to global
    void InitPerfUtilModule(); // add perfutil object to global
    void InitJsExportsUtilObject();
    void InitJsNativeModuleObject();
    void InitJsContextModuleObject();
    void InitGroupJsBridge();

    std::unordered_map<int32_t, panda::Global<panda::ObjectRef>> rootViewMap_;
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

    shared_ptr<JsRuntime> runtime_;
    RefPtr<FrontendDelegate> frontendDelegate_;
    WeakPtr<JsMessageDispatcher> dispatcher_;
    mutable std::mutex mutex_;
    bool isDebugMode_ = true;
    bool usingSharedRuntime_ = false;
    static bool isModulePreloaded_;
    static bool isModuleInitialized_;
    static shared_ptr<JsRuntime> globalRuntime_;

    ACE_DISALLOW_COPY_AND_MOVE(JsiDeclarativeEngineInstance);
};

class JsiDeclarativeEngine : public JsEngine {
    DECLARE_ACE_TYPE(JsiDeclarativeEngine, JsEngine)
public:
    JsiDeclarativeEngine(int32_t instanceId, void* runtime) : instanceId_(instanceId), runtime_(runtime) {}
    explicit JsiDeclarativeEngine(int32_t instanceId) : instanceId_(instanceId) {}
    ~JsiDeclarativeEngine() override;

    bool Initialize(const RefPtr<FrontendDelegate>& delegate) override;

    void Destroy() override;

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

    void OnActive() override;

    void OnInactive() override;

    bool OnStartContinuation() override;

    void OnCompleteContinuation(int32_t code) override;

    void OnRemoteTerminated() override;

    void OnSaveData(std::string& data) override;

    bool OnRestoreData(const std::string& data) override;

    // Destroy application instance according to packageName
    void DestroyApplication(const std::string& packageName) override;

    void UpdateApplicationState(const std::string& packageName, Frontend::State state) override;

    void OnWindowDisplayModeChanged(bool isShownInMultiWindow, const std::string& data) override;

    void MediaQueryCallback(const std::string& callbackId, const std::string& args) override;

    void RequestAnimationCallback(const std::string& callbackId, uint64_t timeStamp) override;

    void JsCallback(const std::string& callbackId, const std::string& args) override;

    void RunGarbageCollection() override;

    std::string GetStacktraceMessage() override;

    void SetLocalStorage(int32_t instanceId, NativeReference* storage) override;

    void SetContext(int32_t instanceId, NativeReference* context) override;

    RefPtr<GroupJsBridge> GetGroupJsBridge() override;

    FrontendDelegate* GetFrontend() override
    {
        return AceType::RawPtr(engineInstance_->GetDelegate());
    }

    RefPtr<JsiDeclarativeEngineInstance> GetEngineInstance()
    {
        return engineInstance_;
    }

    void RunNativeEngineLoop() override
    {
        if (nativeEngine_ != nullptr) {
            nativeEngine_->Loop(LOOP_NOWAIT, false);
        }
    }

    const shared_ptr<JsValue>& GetRenderContext() const
    {
        return renderContext_;
    }

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void ReplaceJSContent(const std::string& url, const std::string componentName) override;
#endif

private:
    bool CallAppFunc(const std::string& appFuncName);

    bool CallAppFunc(const std::string& appFuncName, std::vector<shared_ptr<JsValue>>& argv);

    void SetPostTask(NativeEngine* nativeEngine);

    void TimerCallJs(const std::string& callbackId) const;

    void InitXComponent();

    void RegisterWorker();
    void RegisterInitWorkerFunc();
    void RegisterAssetFunc();

    RefPtr<JsiDeclarativeEngineInstance> engineInstance_;

    RefPtr<NativeXComponentImpl> nativeXComponentImpl_;

    OH_NativeXComponent *nativeXComponent_ = nullptr;

    int32_t instanceId_ = 0;
    void* runtime_ = nullptr;
    shared_ptr<JsValue> renderContext_;

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    std::string preContent_ = "";
#endif

    ACE_DISALLOW_COPY_AND_MOVE(JsiDeclarativeEngine);
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_JSI_JSI_DECLARATIVE_ENGINE_H
