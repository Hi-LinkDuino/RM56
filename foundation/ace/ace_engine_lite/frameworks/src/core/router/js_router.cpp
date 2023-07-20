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

#include "js_router.h"

#include "ace_log.h"
#if ((OHOS_ACELITE_PRODUCT_WATCH == 1) || (FEATURE_CUSTOM_ENTRY_PAGE == 1))
#include "dft_impl.h"
#endif
#include "async_task_manager.h"
#include "js_page_state_machine.h"
#include "js_profiler.h"
#include "js_page_record.h"
#include "jsi.h"

namespace OHOS {
namespace ACELite {
static void ReplaceAsync(void *data)
{
    if (data == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "replace async failed with null input parameter");
        return;
    }
    // void* can not be dynamically_casted from
    auto router = static_cast<Router *>(data);
    router->ReplaceSync();
    OUTPUT_TRACE();
}

static void PushAsync(void *data)
{
    if(data == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE,"push async failed with null input parameter");
        return;
    }

    auto router = static_cast<Router *>(data);
    router->PushSync();
}

jerry_value_t Router::Replace(jerry_value_t object, bool async)
{
    if (newSm_ != nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "router is replacing, can not handle the new request");
        return UNDEFINED;
    }
    StateMachine *newSm = new StateMachine();
    if (newSm == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc state machine memory heap failed.");
        return UNDEFINED;
    }

#if (BES_FRATURE_PAGE_TRANSITION == 1)
    if (currentSm_ != nullptr) {
        ReplacePageTransCallback *callback = new ReplacePageTransCallback();
        if (!newSm->SetPageTransCallback(callback)) {
            delete callback;
            callback = nullptr;
        }
        OnPrePageTransition(newSm, currentSm_, object);
    }
#endif

    // init new state machine
    jerry_value_t jsRes = jerry_create_undefined();
    bool res = newSm->Init(object, jsRes);
    if (!res) {
        delete newSm;
        return jsRes;
    }
    newSm_ = newSm;
    // dispatch the new page rendering to the async handling as the current context of
    // router.replace need to be released, which need to return out from the scope
    if (async) {
        if (DISPATCH_FAILURE == AsyncTaskManager::GetInstance().Dispatch(ReplaceAsync, this)) {
            // request replacing failed, no chance to do it, release the new state machine
            HILOG_ERROR(HILOG_MODULE_ACE, "dispatch replacing request failed");
            delete newSm_;
            newSm_ = nullptr;
        }
    } else {
        // for the first startup of application, no need to do the async replace
        ReplaceSync();
    }
    return jsRes;
}

void Router::ReplaceSync()
{
    if (newSm_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "replace sync failed, new sm should be prepared");
        return;
    }
    START_TRACING(ROUTER_REPLACE);

#if (BES_FRATURE_PAGE_TRANSITION == 1)
    //It's necessary to delete the old page, but not now.
#else
    // new state machine run successfully to show new page, then need to release old state machine.
    if (currentSm_ != nullptr) {
        delete currentSm_;
        currentSm_ = nullptr;
    }
#endif

    // new state machine should to be current.
    currentSm_ = newSm_;
    newSm_ = nullptr;
    currentSm_->SetHiddenFlag(hidden_);
    // run state machine and start to jump to init state.
    currentSm_->ChangeState(INIT_STATE);
    if (hidden_) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "the whole application is in background, move to HIDE state directly");
        // the whole app is in background, move to HIDE state immediately
        currentSm_->ChangeState(BACKGROUND_STATE);
    } else {
        // above call will move sm into ready state, than let the page show
        currentSm_->ChangeState(SHOW_STATE);
    }
#if ((OHOS_ACELITE_PRODUCT_WATCH == 1) || (FEATURE_CUSTOM_ENTRY_PAGE == 1))
    DftImpl::GetInstance()->CallbackPageReplaced(currentSm_->GetCurrentState());
#endif
    STOP_TRACING();

    //1ms is consumed.
    JSHeapStatus heapStatus;
    JSI::GetJSHeapStatus(heapStatus);
    HILOG_INFO(HILOG_MODULE_ACE, "ReplaceSync. Heap status totalBytes: %d allocBytes: %d peakAllocBytes: %d ",
         heapStatus.totalBytes,heapStatus.allocBytes,heapStatus.peakAllocBytes);
}

jerry_value_t Router::Push(jerry_value_t object)
{
    if(newSm_ != nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "router is working, can not handle this new request");
        return UNDEFINED;
    }

    StateMachine *newSm = new StateMachine();
    if(newSm == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc state machine memory heap failed.");
        return UNDEFINED;
    }

#if (BES_FRATURE_PAGE_TRANSITION == 1)
    PushPageTransCallback *callback = new PushPageTransCallback();
    if (!newSm->SetPageTransCallback(callback)) {
        delete callback;
        callback = nullptr;
    }
    OnPrePageTransition(newSm, currentSm_, object);
#endif

    jerry_value_t jsRes = jerry_create_undefined();
    bool res = newSm->Init(object, jsRes);
    if(!res){
        HILOG_ERROR(HILOG_MODULE_ACE, "StateMachine Init failed.");
        delete newSm;
        return jsRes;
    }

    newSm_ = newSm;

    if(DISPATCH_FAILURE == AsyncTaskManager::GetInstance().Dispatch(PushAsync,this)) {
        delete newSm_;
        newSm_ = nullptr;
    }

    return jsRes;
}

void Router::PushSync()
{
    if (newSm_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "push sync failed, new sm should be prepared");
        return;
    }

    if(currentSm_ != nullptr) {

#if (BES_FRATURE_PAGE_TRANSITION == 1)
        //Already executed in OnPrePageTransition().
#else
        int  state = currentSm_->GetCurrentState();
        if(state == SHOW_STATE){
            currentSm_->ChangeState(BACKGROUND_STATE);
        }
#endif

        pageRecord_->PushPage(currentSm_);
    }

    currentSm_ = newSm_;
    newSm_ = nullptr;
    currentSm_->SetHiddenFlag(hidden_);
    currentSm_->ChangeState(INIT_STATE);

    if (hidden_) {
        HILOG_DEBUG(HILOG_MODULE_ACE, "the whole application is in background, move to HIDE state directly");
        currentSm_->ChangeState(BACKGROUND_STATE);
    } else {
        currentSm_->ChangeState(SHOW_STATE);
    }
}

jerry_value_t Router::Back(jerry_value_t object)
{
    if(newSm_ != nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "router is working, can not handle this new request");
        return UNDEFINED;
    }

    if (currentSm_ == nullptr) return UNDEFINED;

    char* uri = nullptr;
    if(!jerry_value_is_undefined(object)) {
        jerry_value_t uriValue = jerryx_get_property_str(object, ROUTER_PAGE_URI);
        if(jerry_value_is_string(uriValue)) {
            uri = MallocStringOf(uriValue);
        }
        jerry_release_value(uriValue);
    }

    //2023年5月5日：不需要跳转到正在显示的页面
    if (currentSm_->IsCurrentPageUri(uri)) {
        ACE_FREE(uri);
        return UNDEFINED;
    }

    StateMachine* newSm = pageRecord_->BackPage(uri);

    ACE_FREE(uri);

    if (newSm == nullptr){
        HILOG_ERROR(HILOG_MODULE_ACE, "router back failed back page is null");
        return UNDEFINED;
    }

    newSm_ = newSm;

#if (BES_FRATURE_PAGE_TRANSITION == 1)
    //2023年5月16日：不允许用户配置
    // //优先使用用户指定的页面转场参数
    // bool applyUserProfile = false;
    // if (!IS_UNDEFINED(object)) {
    //     jerry_value_t transField = jerryx_get_property_str(object, BES_FIELD_PAGE_TRANSITION);
    //     if (!IS_UNDEFINED(transField)) {
    //         applyUserProfile = true;
    //     }
    //     jerry_release_value(transField);
    // }
    // if (!applyUserProfile) {
    //     currentSm_->ReversePageTransitionParam();
    //     newSm_->ReversePageTransitionParam();
    // }
    //重新编排页面转场信息
    currentSm_->ReversePageTransition(object);

    ReplacePageTransCallback *callback = new ReplacePageTransCallback();
    if (newSm_->SetPageTransCallback(callback)) callback = nullptr;

    //Take a notification to js to release their resource.
    auto state = currentSm_->GetCurrentState();
    if (state == READY_STATE || state == SHOW_STATE) {
        if (!currentSm_->IsFinishing()) currentSm_->ChangeState(BACKGROUND_STATE);
    }
#else
    if (currentSm_ != nullptr) {
        delete currentSm_;
        currentSm_ = nullptr;
    }
#endif

    currentSm_ = newSm_;
    newSm_ = nullptr;

    JsAppContext::GetInstance()->SetCurrentJsPath(currentSm_->GetJsPagePath());

    if (!IS_UNDEFINED(object)) {
        jerry_value_t params = jerryx_get_property_str(object, ROUTER_PAGE_PARAMS);
        if (!IS_UNDEFINED(params)) { //page params maybe undefined.
            currentSm_->RegisterParams(params);
            jerry_release_value(params);
        }
    }

    currentSm_->SetHiddenFlag(false);
    currentSm_->ChangeState(SHOW_STATE);

    //1ms is consumed
    JSHeapStatus heapStatus;
    JSI::GetJSHeapStatus(heapStatus);
    HILOG_INFO(HILOG_MODULE_ACE, "Back. Heap status totalBytes: %d allocBytes: %d peakAllocBytes: %d ",
         heapStatus.totalBytes,heapStatus.allocBytes,heapStatus.peakAllocBytes);

    return UNDEFINED;
}

void Router::Show()
{
    if (currentSm_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "no SM when performing show");
        return;
    }

    hidden_ = false;
    currentSm_->SetHiddenFlag(hidden_);
    currentSm_->ChangeState(SHOW_STATE);
}

void Router::Hide()
{
    if (currentSm_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "no SM when performing background");
        return;
    }

    currentSm_->ChangeState(BACKGROUND_STATE);
    hidden_ = true;
    currentSm_->SetHiddenFlag(hidden_);
}

#if (BES_FRATURE_PAGE_TRANSITION == 1)
void Router::OnPrePageTransition(StateMachine* newSm, StateMachine* currentSm, jerry_value_t object)
{
    if (currentSm == nullptr) return;

    newSm->RecordPageTransition(currentSm, object);

    if (!newSm->IsEnterTransitionPage()) {
        newSm->ReleasePageTransition();
    }

    // Take a notification to js to release their resource.
    auto state = currentSm->GetCurrentState();
    if (state == READY_STATE || state == SHOW_STATE) {
        if (!currentSm->IsFinishing()) {
            currentSm->ChangeState(BACKGROUND_STATE);
        }
    }
}
#endif
} // namespace ACELite
} // namespace OHOS
