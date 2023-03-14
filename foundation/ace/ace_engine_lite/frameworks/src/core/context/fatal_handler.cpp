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

#include "fatal_handler.h"
#include "ace_event_error_code.h"
#include "ace_log.h"
#include "async_task_manager.h"
#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
#include "cmsis_os2.h"
#endif // OHOS_ACELITE_PRODUCT_WATCH
#include "jerryscript-port-default.h"
#include "js_app_context.h"
#include "js_fwk_common.h"
#include "presets/console_log_impl.h"
#include "presets/feature_ability_module.h"
#include "product_adapter.h"
#include "root_view.h"
#include "task_manager.h"

namespace OHOS {
namespace ACELite {
/**
 * Global instance for saving fatal error code and handling fatal errors
 */
FatalHandler &FatalHandler::GetInstance()
{
    static FatalHandler instance;
    return instance;
}

bool FatalHandler::IsErrorHittedWrapper()
{
    return FatalHandler::GetInstance().IsFatalErrorHitted();
}

bool FatalHandler::IsAppExitingWrapper()
{
    return FatalHandler::GetInstance().IsAppExiting();
}

static void HandleFatal(int errorCode)
{
#if (FEATURE_FATAL_ERROR_HANDLING == 1)
    HILOG_ERROR(HILOG_MODULE_ACE, "hitted by fatal error[%{public}d]", errorCode);
    if (FatalHandler::GetInstance().IsFatalErrorHitted()) {
        // fatal hitted again during one fatal handing, return to avoid dead loop
        return;
    }
    // record error code
    FatalHandler::GetInstance().SetFatalError(errorCode);
    // send message to AMS to tear down us
    // try to recycling resource first
    FatalHandler::GetInstance().HandleFatalInternal();
    // inform AMS we are facing fatal error
    ProductAdapter::SendTerminatingRequest(JsAppContext::GetInstance()->GetCurrentAbilityToken(), true);
    // sleep to stuck engien to make ANR for triggering force-stop
    const uint32_t sleepTime = 3000;
    uint8_t maxRetryCount = 20;
    while (maxRetryCount > 0) {
#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
        osDelay(sleepTime);
#endif // OHOS_ACELITE_PRODUCT_WATCH
        maxRetryCount--;
    }
    HILOG_ERROR(HILOG_MODULE_ACE, "the JS task is not killed after sleep very long time");
#else
    FatalHandler::GetInstance().DumpFatalTrace(errorCode);
#endif // FEATURE_FATAL_ERROR_HANDLING
}

void FatalHandler::DumpFatalTrace(int errorCode) const
{
    LogString(LogLevel::LOG_LEVEL_ERR, "[JS Exception]: ");
    LogString(LogLevel::LOG_LEVEL_ERR, GetErrorStr(errorCode));
    LogString(LogLevel::LOG_LEVEL_ERR, "\n");
}

void FatalHandler::RegisterFatalHandler(JSAbility *ability)
{
    jsAbility_ = ability;
    // set callback into jerry
    jerry_port_default_set_fatal_handler(HandleFatal);
}

bool FatalHandler::IsErrorSupported(int errorCode) const
{
    switch (errorCode) {
        case ERR_NATIVE_OUT_OF_MEMORY:
            // fall through
        case ERR_READ_FWK_FILE_FAILED:
            // fall through
        case ERR_EVAL_FWK_FAILED:
            // fall through
        case ERR_READ_JS_FILE_FAILED:
            // fall through
        case ERR_EVAL_JS_FAILED:
            // fall through
        case ERR_OUT_OF_MEMORY: // JS heap overflow
            // fall through
        case ERR_REF_COUNT_LIMIT:
            // fall through
        case ERR_DISABLED_BYTE_CODE:
            // fall through
        case ERR_FAILED_INTERNAL_ASSERTION:
            return true;
        default:
            break;
    }
    return false;
}

void FatalHandler::HandleFatalInternal()
{
    // detach the root component from root view
    RootView *rootView = RootView::GetInstance();
    if ((rootView != nullptr) && (pageRootView_ != nullptr)) {
        rootView->Remove(pageRootView_);
    }
    // do the recycling
    HandleFatalError(FatalHandler::ERR_INVALID);
    // transfer ability to destroy
    if (jsAbility_ != nullptr) {
        jsAbility_->TransferToDestroy();
    }
    // reset locals
    CleanUpFatalResource();
}

void FatalHandler::HandleFatalError(int errorCode)
{
    if (errorCode == ERR_INVALID && fatalErrorCode_ == ERR_INVALID) {
        // no fatal error happens
        return;
    }

    if (errorCode != ERR_INVALID) {
        // update fatal error to new one if it's valid
        fatalErrorCode_ = errorCode;
    }
    isFatalHandled_ = false;
    if (!isRecycling_) {
        // recycle any remain components first
        RecycleComponents();
    } else {
        HILOG_WARN(HILOG_MODULE_ACE, "hitted again when handling fatal error");
    }

    DumpFatalTrace(fatalErrorCode_);

    AsyncTaskManager::GetInstance().SetFront(false);
    // reset the low layer rendering flag if needed
    FatalHandler::GetInstance().ResetRendering();
    FeaAbilityModule::Release();
    componentNodes_.Clear();
    isFatalHandled_ = true;
}

/*
 * Reset render flag or task handler mutex if needed
 */
void FatalHandler::ResetRendering()
{
    if (IsTEHandling()) {
        // release the render flag as we are going into sleep and will be force killed
        TaskManager::GetInstance()->ResetTaskHandlerMutex();
        SetTEHandlingFlag(false);
    }
#if (OHOS_ACELITE_PRODUCT_WATCH == 1)
    ProductAdapter::NotifyRenderEnd();
#endif
}

const char *FatalHandler::GetErrorStr(int errorCode) const
{
    switch (errorCode) {
        case ERR_NATIVE_OUT_OF_MEMORY:
            return "NATIVE OOM";
        case ERR_READ_FWK_FILE_FAILED:
            return "READ FWK FAILED";
        case ERR_EVAL_FWK_FAILED:
            return "EVAL FWK FAILED";
        case ERR_READ_JS_FILE_FAILED:
            return "READ JS FAILED";
        case ERR_EVAL_JS_FAILED:
            return "EVAL JS FAILED";
        case ERR_OUT_OF_MEMORY:
            return "JS HEAP OOM";
        case ERR_REF_COUNT_LIMIT:
            return "JS REF LIMIT";
        case ERR_DISABLED_BYTE_CODE:
            return "JS DISABLED BYTE CODE";
        case ERR_FAILED_INTERNAL_ASSERTION:
            return "JS ASSERTION FAILED";
        default:
            break;
    }
    return "";
}

void FatalHandler::SetFatalError(int errorCode)
{
    switch (errorCode) {
        case ERR_OUT_OF_MEMORY:
            ACE_ERROR_CODE_PRINT(EXCE_ACE_ENGINE_RUNTIME_ERROR, EXCE_ACE_JS_HEAP_OOM);
            break;
        case ERR_REF_COUNT_LIMIT:
            ACE_ERROR_CODE_PRINT(EXCE_ACE_ENGINE_RUNTIME_ERROR, EXCE_ACE_JS_REF_COUNT_LIMIT);
            break;
        case ERR_DISABLED_BYTE_CODE:
            ACE_ERROR_CODE_PRINT(EXCE_ACE_ENGINE_RUNTIME_ERROR, EXCE_ACE_JS_DISABLED_BYTE_CODE);
            break;
        case ERR_FAILED_INTERNAL_ASSERTION:
            ACE_ERROR_CODE_PRINT(EXCE_ACE_ENGINE_RUNTIME_ERROR, EXCE_ACE_JS_FAILED_INTERNAL_ASSERTION);
            break;
        default:
            break;
    }

    if (fatalErrorCode_ == ERR_INVALID && IsErrorSupported(errorCode)) {
        // reset handling flag
        isFatalHandled_ = false;
        fatalErrorCode_ = errorCode;
    }
}

void FatalHandler::RecycleComponents()
{
    isRecycling_ = true;
    ListNode<Component *> *node = componentNodes_.Begin();
    while (node != componentNodes_.End()) {
        if (node->data_ != nullptr) {
            Component *component = node->data_;
            component->Release();
            delete component;
            component = nullptr;
            node->data_ = nullptr;
        }
        node = node->next_;
    }
    isRecycling_ = false;
}

void FatalHandler::CleanUpFatalResource()
{
    // make sure we have released all nodes
    componentNodes_.Clear();
    isFatalHandled_ = false;
    // reset error code when exitting the whole application
    fatalErrorCode_ = ERR_INVALID;
    isExiting_ = false;
    pageRootView_ = nullptr;
}

bool FatalHandler::IsJSRuntimeFatal() const
{
    bool result = false;
    switch (fatalErrorCode_) {
        case ERR_OUT_OF_MEMORY: // JS heap overflow
            // fall through
        case ERR_REF_COUNT_LIMIT:
            // fall through
        case ERR_DISABLED_BYTE_CODE:
            // fall through
        case ERR_FAILED_INTERNAL_ASSERTION:
            result = true;
            break;
        default:
            result = false;
            break;
    }
    return result;
}

bool FatalHandler::IsJSHeapOverflow() const
{
    return (fatalErrorCode_ == ERR_OUT_OF_MEMORY);
}

/**
 * As the exception can interrupt the tree rationship building, so we can't recycle the Components as normal,
 * because we even not get the root Component object when exception happen. So we record all Components when
 * they are created out, and remove them when they are released normally, if exception happens, if any Componets
 * still in our list, that means there is no chance to release them through the normal way, we must release them
 * manually.
 */
void FatalHandler::AttachComponentNode(Component *component)
{
    if (component == nullptr) {
        return;
    }
    componentNodes_.PushBack(component);
}

void FatalHandler::DetachComponentNode(const Component *component)
{
    if ((component == nullptr) || isRecycling_) {
        return;
    }
    ListNode<Component *> *node = componentNodes_.Begin();
    while (node != componentNodes_.End()) {
        if (node->data_ == component) {
            componentNodes_.Remove(node);
            break;
        }
        node = node->next_;
    }
}

bool FatalHandler::IsFatalErrorHitted() const
{
    return (fatalErrorCode_ != ERR_INVALID);
}

bool FatalHandler::IsFatalErrorHandling() const
{
    return isRecycling_;
}

bool FatalHandler::IsFatalErrorHandleDone() const
{
    return isFatalHandled_;
}

void FatalHandler::SetTEHandlingFlag(bool flag)
{
    isTEHandling_ = flag;
}

bool FatalHandler::IsTEHandling() const
{
    return isTEHandling_;
}

void FatalHandler::SetExitingFlag(bool flag)
{
    isExiting_ = flag;
}

bool FatalHandler::IsAppExiting() const
{
    return isExiting_;
}

void FatalHandler::SetCurrentPageRootView(UIView *pageRoot)
{
    pageRootView_ = pageRoot;
}

uint16_t FatalHandler::GetComponentCount() const
{
    return componentNodes_.Size();
}

void FatalHandler::NotifyVisibleStatusChange(bool isVisible) const
{
    ListNode<Component *> *node = componentNodes_.Begin();
    while (node != componentNodes_.End()) {
        if (node->data_ != nullptr) {
            node->data_->OnVisibilityChanged(isVisible);
        }
        node = node->next_;
    }
}
} // namespace ACELite
} // namespace OHOS
