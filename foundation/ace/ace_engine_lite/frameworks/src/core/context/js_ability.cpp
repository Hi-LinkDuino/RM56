/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "js_ability.h"

#include <cstring>
#include "ace_event_error_code.h"
#include "ace_log.h"
#include "acelite_config.h"
#include "async_task_manager.h"
#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
#include "dft_impl.h"
#endif // OHOS_ACELITE_PRODUCT_WATCH
#include "fatal_handler.h"
#include "js_ability_impl.h"
#include "js_async_work.h"
#include "js_profiler.h"
#include "product_adapter.h"
#if (defined(__LINUX__) || defined(__LITEOS_A__))
#include "ace_ability.h"
#endif
#include "module_manager.h"

namespace OHOS {
namespace ACELite {
/**
 * This is a helper function to cast void* to JSAbilityImpl*, for header separating purpose.
 */
static JSAbilityImpl *CastAbilityImpl(void *abilityImpl)
{
    if (abilityImpl == nullptr) {
        return nullptr;
    }

    return static_cast<JSAbilityImpl *>(abilityImpl);
}

static void DumpNativeMemoryUsage()
{
#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
    NativeMemInfo memInfo;
    ProductAdapter::GetNativeMemInfo(&memInfo);
    HILOG_DEBUG(HILOG_MODULE_ACE, "available free size: %{public}d", memInfo.freeSize);
#endif // OHOS_ACELITE_PRODUCT_WATCH
}

#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
extern "C" void RestoreSystemWrapper(const char *crashMessage);
#endif
JSAbility::~JSAbility()
{
    // check the status
    if (jsAbilityImpl_ != nullptr) {
#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
        // the JSAbility instance can only be destroied after transfering the app to DESTROY state
        RestoreSystemWrapper("AMS is deleting app task but without normal lifecycle transition!");
#endif
    }
}

void JSAbility::Launch(const char * const abilityPath, const char * const bundleName, uint16_t token,
                       const char *pageInfo)
{
    if (jsAbilityImpl_ != nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Launch only can be triggered once");
        ACE_ERROR_CODE_PRINT(EXCE_ACE_FWK_LAUNCH_FAILED, EXCE_ACE_APP_ALREADY_LAUNCHED);
        return;
    }

    if ((abilityPath == nullptr) || (strlen(abilityPath) == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "invalid app path");
        ACE_ERROR_CODE_PRINT(EXCE_ACE_FWK_LAUNCH_FAILED, EXCE_ACE_INVALID_APP_PATH);
        return;
    }

    if ((bundleName == nullptr) || (strlen(bundleName) == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "invalid bundle name");
        ACE_ERROR_CODE_PRINT(EXCE_ACE_FWK_LAUNCH_FAILED, EXCE_ACE_INVALID_BUNDLE_NAME);
        return;
    }

    HILOG_INFO(HILOG_MODULE_ACE, "LIFECYCLE: JS Ability is launching");

#if (MOCK_JS_ASYNC_WORK != 1)
    JsAsyncWork::SetFatalHandleFunc(FatalHandler::IsErrorHittedWrapper, FatalHandler::IsAppExitingWrapper);
#endif
#if (defined(__LINUX__) || defined(__LITEOS_A__))
    JsAsyncWork::SetPostUITaskFunc(AceAbility::PostUITask);
#endif
    ModuleManager::GetInstance()->SetBundleNameGetter(JSAbility::GetPackageName);
    DumpNativeMemoryUsage();
    jsAbilityImpl_ = new JSAbilityImpl();
    if (jsAbilityImpl_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Create JSAbilityRuntime failed");
        return;
    }
    START_TRACING(LAUNCH);
    // mark the flag in advance to make sure we can take over render tick as soon as possible
    ProductAdapter::UpdateRenderTickAcceptable(true);
    JSAbilityImpl *jsAbilityImpl = CastAbilityImpl(jsAbilityImpl_);
#if (MOCK_JS_ASYNC_WORK != 1)
    // simulator uses the self-implementation for async work, this interface is not included
    JsAsyncWork::SetEnvStatus(true);
#endif
    jsAbilityImpl->InitEnvironment(abilityPath, bundleName, token);
    ACE_EVENT_PRINT(MT_ACE_FWK_LAUNCHING, 0);
    FatalHandler::GetInstance().RegisterFatalHandler(this);
    jsAbilityImpl->DeliverCreate(pageInfo);
    STOP_TRACING();
    OUTPUT_TRACE();
}

void JSAbility::Show()
{
    if (jsAbilityImpl_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Must trigger Launch first");
        return;
    }

    HILOG_INFO(HILOG_MODULE_ACE, "LIFECYCLE: JS Ability will be shown");
    ACE_EVENT_PRINT(MT_ACE_FWK_ACTIVING, 0);
    JSAbilityImpl *jsAbilityImpl = CastAbilityImpl(jsAbilityImpl_);
    jsAbilityImpl->Show();
    AsyncTaskManager::GetInstance().SetFront(true);
    ProductAdapter::UpdateRenderTickAcceptable(true);
    isActived_ = true;
}

void JSAbility::Hide()
{
    if (jsAbilityImpl_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Must trigger Launch first");
        return;
    }
    HILOG_INFO(HILOG_MODULE_ACE, "LIFECYCLE: JS Ability will be hidden");
    ACE_EVENT_PRINT(MT_ACE_FWK_HIDING, 0);
    JSAbilityImpl *jsAbilityImpl = CastAbilityImpl(jsAbilityImpl_);
    jsAbilityImpl->Hide();
    AsyncTaskManager::GetInstance().SetFront(false);
    ProductAdapter::UpdateRenderTickAcceptable(false);
    isActived_ = false;
}

void JSAbility::TransferToDestroy()
{
    if (jsAbilityImpl_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Must trigger Launch first");
        return;
    }

#if (MOCK_JS_ASYNC_WORK != 1)
    // simulator uses the self-implementation for async work, this interface is not included
    JsAsyncWork::SetEnvStatus(false);
#endif
    HILOG_INFO(HILOG_MODULE_ACE, "LIFECYCLE: JS Ability is exiting");
    ACE_EVENT_PRINT(MT_ACE_FWK_DESTROYING, 0);
    JSAbilityImpl *jsAbilityImpl = CastAbilityImpl(jsAbilityImpl_);
    jsAbilityImpl->CleanUp();
    // Reset render flag or low layer task mutex in case we are during the rendering process,
    // this situation might happen if the destroy function is called outside of JS thread, such as AMS.
    ProductAdapter::UpdateRenderTickAcceptable(false);
    FatalHandler::GetInstance().ResetRendering();
    FatalHandler::GetInstance().SetExitingFlag(false);
#if (FEATURE_SCREEN_ON_VISIBLE == 1)
    if (ProductAdapter::SetScreenOnVisible(false) == false) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Fail to recover screen visible property");
    }
#endif
#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
    JsAsyncWork::SetAppQueueHandler(nullptr);
    DftImpl::GetInstance()->RegisterPageReplaced(nullptr);
#endif // OHOS_ACELITE_PRODUCT_WATCH
    delete reinterpret_cast<JSAbilityImpl *>(jsAbilityImpl_);
    jsAbilityImpl_ = nullptr;
    DumpNativeMemoryUsage();
}

void JSAbility::BackPressed()
{
    if (jsAbilityImpl_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Must trigger Launch first");
        return;
    }

    JSAbilityImpl *jsAbilityImpl = CastAbilityImpl(jsAbilityImpl_);
    jsAbilityImpl->NotifyBackPressed();
}

const char *JSAbility::GetPackageName()
{
    return JsAppContext::GetInstance()->GetCurrentBundleName();
}

// this public interface will be deprecated, only fatal scenario can trigger force destroy
void JSAbility::ForceDestroy()
{
    HILOG_ERROR(HILOG_MODULE_ACE, "ForceDestroy interface is deprecated as JS engine can not run on other task");
}

bool JSAbility::IsRecycled()
{
    return (jsAbilityImpl_ == nullptr);
}

LazyLoadManager *GetLazyLoadManager()
{
    JsAppContext *context = JsAppContext::GetInstance();
    return const_cast<LazyLoadManager *>(context->GetLazyLoadManager());
}

LazyLoadState GetLazyLoadManagerState()
{
    LazyLoadManager *lazyLoadManager = GetLazyLoadManager();
    return lazyLoadManager->GetState();
}

void JSAbility::LazyLoadHandleRenderTick(void *data)
{
    UNUSED(data);
    // double check, if state reseted, break
    LazyLoadState state = GetLazyLoadManagerState();
    if (state == LazyLoadState::INIT || state == LazyLoadState::DONE) {
        return;
    }

    GetLazyLoadManager()->RenderLazyLoadWatcher();
}

void JSAbility::HandleRenderTick()
{
    if (!isActived_) {
        // skip the TE tick if we are not forground
        ProductAdapter::NotifyRenderEnd();
        errorTickCount_++;
        if ((errorTickCount_ % ERR_TICK_COUNT_TRACE_CTRL) == 1) {
            HILOG_WARN(HILOG_MODULE_ACE, "skip one render tick process since not activated, count[%{public}d]",
                       errorTickCount_);
        }
        if (errorTickCount_ == UINT32_MAX) {
            errorTickCount_ = 0;
        }
        return;
    }

    // reset error tick tracing count
    errorTickCount_ = 0;

    if ((ProductAdapter::IsTEHandlersRegisted()) && !(FatalHandler::GetInstance().IsAppExiting())) {
        FatalHandler::GetInstance().SetTEHandlingFlag(true);
        ProductAdapter::ProcessOneTE();
        // check if state is ready
        if (GetLazyLoadManagerState() == LazyLoadState::READY) {
            JsAsyncWork::DispatchAsyncWork(LazyLoadHandleRenderTick, nullptr);
        }
        FatalHandler::GetInstance().SetTEHandlingFlag(false);
    }
}
} // namespace ACELite
} // namespace OHOS
