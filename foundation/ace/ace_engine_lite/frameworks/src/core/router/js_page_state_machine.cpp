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

#include "js_page_state_machine.h"
#include "ace_log.h"
#include "component.h"
#include "directive/descriptor_utils.h"
#include "fatal_handler.h"
#include "js_app_environment.h"
#include "js_async_work.h"
#include "js_page_state.h"
#include "js_profiler.h"
#include "jsi.h"
#include "lazy_load_manager.h"
#include "mem_proc.h"
#include "module_manager.h"
#include "securec.h"

#if (BES_FRATURE_PAGE_TRANSITION == 1)
#include "screen.h"

#include "bestechnic/bes_transition_parser.h"

namespace {
/** @brief 页面转场的最小时长（毫秒） */
constexpr const int16_t TRANSITION_DURATION_MIN = 100;
/** @brief 页面转场的最大时长（毫秒） */
constexpr const int16_t TRANSITION_DURATION_MAX = 60000;

/** @brief 线性（加速） */
constexpr const char* VAL_EASE_IN = "ease-in";
/** @brief 线性（减速） */
constexpr const char* VAL_EASE_OUT = "ease-out";
/** @brief 线性（加速后减速） */
constexpr const char* VAL_EASE_IN_OUT = "ease-in-out";

/** @brief 淡入 */
constexpr const char* VAL_FADE_IN = "fade-in";
/** @brief 淡出 */
constexpr const char* VAL_FADE_OUT = "fade-out";

/** @brief 中间 */
constexpr const char* VAL_PIVOT_CENTER = "center";
/** @brief 左上位置 */
constexpr const char* VAL_PIVOT_LT = "left_top";
/** @brief 右上位置 */
constexpr const char* VAL_PIVOT_RT = "right_top";
/** @brief 左下位置 */
constexpr const char* VAL_PIVOT_LB = "left_bottom";
/** @brief 右下位置 */
constexpr const char* VAL_PIVOT_RB = "right_bottom";
/** @brief 左侧（居中）位置 */
constexpr const char* VAL_PIVOT_LC = "left_center";
/** @brief 右侧（居中）位置 */
constexpr const char* VAL_PIVOT_RC = "right_center";
/** @brief 顶部（居中）位置 */
constexpr const char* VAL_PIVOT_TC = "top_center";
/** @brief 底部（居中）位置 */
constexpr const char* VAL_PIVOT_BC = "bottom_center";
} // namespace
#endif

namespace OHOS {
namespace ACELite {
StateMachine::StateMachine()
{
    currentState_ = UNDEFINED_STATE;
    for (int i = 0; i < PAGE_STATE_SIZE; i++) {
        stateMap_[i] = nullptr;
    }
    jsPagePath_ = nullptr;
    appRootPath_ = nullptr;
    appContext_ = nullptr;
    uri_ = nullptr;
    rootComponent_ = nullptr;
    viewModel_ = UNDEFINED;
    object_ = UNDEFINED;
    hasParams_ = false;
    isEntireHidden_ = false;
    watchersHead_ = nullptr;
    scrollLayer_ = nullptr;
    fragmentList_ = nullptr;

#if (BES_FRATURE_PAGE_TRANSITION == 1)
    isFinishing_ = false;
#endif
}

StateMachine::~StateMachine()
{
#if (BES_FRATURE_PAGE_TRANSITION == 1)
    //force background state.
    if (!IsFinishing()) {
        SetFinishing(true);
        if (currentState_ >= INIT_STATE) ChangeState(BACKGROUND_STATE);
    }

    SetPageTransitionParam(nullptr);

    //无论如何，认定页面转场已经结束
    if (IsEnterTransitionPage()) RootView::GetInstance()->SetPageTransitionState(false);

    if (scrollLayer_ != nullptr) {
        scrollLayer_->StopPageTransition();
    }
    if (pageTransCallback_ != nullptr) {
        delete pageTransCallback_;
        pageTransCallback_ = nullptr;
    }
#endif

    // release this page's all resource
    // if error happens, statemachine must force to jump to destroy state for releasing resource.
    if ((currentState_ >= INIT_STATE) || FatalHandler::GetInstance().IsFatalErrorHitted()) {
        ChangeState(BACKGROUND_STATE);
        ChangeState(DESTROY_STATE);
    }
    for (int i = 0; i < PAGE_STATE_SIZE; i++) {
        if (stateMap_[i] != nullptr) {
            delete stateMap_[i];
            stateMap_[i] = nullptr;
        }
    }
    if (uri_ != nullptr) {
        ace_free(uri_);
        uri_ = nullptr;
    }
    if (jsPagePath_ != nullptr) {
        ace_free(jsPagePath_);
        jsPagePath_ = nullptr;
    }
    if(fragmentList_ != nullptr){
        delete fragmentList_;
        fragmentList_ = nullptr;
    }

    jerry_release_value(object_);
}

void StateMachine::RegisterParams(jerry_value_t params)
{
    if(viewModel_ == UNDEFINED){
        HILOG_ERROR(HILOG_MODULE_ACE, "register params failed view model is undefined");
        return;
    }

    if(jerry_value_is_undefined(params)){
        HILOG_ERROR(HILOG_MODULE_ACE, "register params failed params is undefined");
        return; 
    }

    jerry_value_t keys = jerry_get_object_keys(params);
    uint16_t size = jerry_get_array_length(keys);
    for(uint16_t idx = 0; idx < size; ++idx){
        jerry_value_t key = jerry_get_property_by_index(keys, idx);
        jerry_value_t value = jerry_get_property(params, key);
        jerry_release_value(jerry_set_property(viewModel_, key, value));
        ReleaseJerryValue(value, key, VA_ARG_END_FLAG);
    }

    ReleaseJerryValue(keys, VA_ARG_END_FLAG);
}

void StateMachine::SetCurrentState(int8_t newState)
{
    if (newState <= UNDEFINED_STATE || newState >= END_STATE) {
        HILOG_ERROR(HILOG_MODULE_ACE, "error input state:%{public}d", newState);
        return;
    }
    currentState_ = newState;
}

int StateMachine::GenerateJsPagePath(const char * const uri)
{
    size_t uriLen = strlen(uri);
    if (uriLen >= PATH_LENGTH_MAX) {
        return ERROR_INPUT_PARAMETER;
    }
    size_t len = uriLen;
    // if path is "/", load default page of app.(appRootPath/index/index.js)
#if JS_PAGE_SPECIFIC
    if (jsPageSpecific.jsIndexFilePath == nullptr) {
        jsPageSpecific.jsIndexFilePath = const_cast<char *>(JS_INDEX_FILE_PATH);
    }
    len = strlen(jsPageSpecific.jsIndexFilePath);
    if (len < uriLen) {
        len = uriLen;
    }
#else
    if ((uriLen == 1) && (uri[0] == PATH_DEFAULT[0])) {
        len = strlen(JS_INDEX_FILE_PATH);
    }
#endif
    const char * const sourceFileSuffix = (JsAppEnvironment::GetInstance()->IsSnapshotMode()) ? ".bc" : ".js";
    len += strlen(sourceFileSuffix);
    // add ending character:'\0'
    len += 1;
    jsPagePath_ = static_cast<char *>(ace_malloc(len));
    if (jsPagePath_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "malloc path memory heap failed.");
        return ERROR_MALLOC;
    }
    jsPagePath_[0] = '\0';
    errno_t err = 0;
    if ((uriLen == 1) && (uri[0] == PATH_DEFAULT[0])) {
#if JS_PAGE_SPECIFIC
        err = strcpy_s(jsPagePath_, len, jsPageSpecific.jsIndexFilePath);
#else
        err = strcpy_s(jsPagePath_, len, JS_INDEX_FILE_PATH);
#endif
    } else {
        err = strcpy_s(jsPagePath_, len, uri);
    }
    JS_PAGE_RETURN_IF_ERROR(err, jsPagePath_);
    err = strcat_s(jsPagePath_, len, sourceFileSuffix);
    JS_PAGE_RETURN_IF_ERROR(err, jsPagePath_);
    return SUCCESS;
}

void StateMachine::RegisterUriAndParamsToPage(const char * const uri, jerry_value_t params)
{
    jerry_value_t globalObject = jerry_get_global_object();
    if (!IS_UNDEFINED(params)) {
        // add $page.path property
        JerrySetStringProperty(params, ROUTER_PAGE_PATH, uri);
        JerrySetNamedProperty(globalObject, ROUTER_PAGE, params);
        hasParams_ = true;
    } else {
        jerry_value_t pagePropName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(ROUTER_PAGE));
        bool success = jerry_delete_property(globalObject, pagePropName);
        if (!success) {
            HILOG_ERROR(HILOG_MODULE_ACE, "delete $page property failed from global object.");
        }
        jerry_release_value(pagePropName);
    }
    jerry_release_value(globalObject);
}

bool StateMachine::Init(jerry_value_t object, jerry_value_t &jsRes)
{
    appContext_ = JsAppContext::GetInstance();
    appRootPath_ = const_cast<char *>(appContext_->GetCurrentAbilityPath());
    if ((appRootPath_ == nullptr) || (strlen(appRootPath_) == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "statemachine init failed as this app's root path is invalid.");
        return false;
    }
    // create new references to object's value,otherwise jerry engine will crash.
    object_ = jerry_acquire_value(object);
    stateMap_[INIT_STATE] = new PageInitState();
    stateMap_[READY_STATE] = new PageReadyState();
    stateMap_[SHOW_STATE] = new PageShowState();
    stateMap_[BACKGROUND_STATE] = new PageBackgroundState();
    stateMap_[DESTROY_STATE] = new PageDestroyState();
    return BindUri(jsRes);
}

bool StateMachine::BindUri(jerry_value_t &jsRes)
{
    // check1:object undefined or type error
    if (!jerry_value_is_object(object_)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "statemachine init failed as object is invalid.");
        jsRes = jerry_create_error(JERRY_ERROR_TYPE,
                                   reinterpret_cast<const jerry_char_t *>("param of router should be object."));
        return false;
    }
    // check2:object's uri is undefined or is not string
    jerry_value_t uriValue = jerryx_get_property_str(object_, ROUTER_PAGE_URI);
    if (!jerry_value_is_string(uriValue)) {
        jerry_release_value(uriValue);
        HILOG_ERROR(HILOG_MODULE_ACE, "statemachine init failed as uri is invalid.");
        jsRes = jerry_create_error(JERRY_ERROR_TYPE,
                                   reinterpret_cast<const jerry_char_t *>("uri value type should be string."));
        return false;
    }
    uri_ = MallocStringOf(uriValue);
    jerry_release_value(uriValue);
    if (uri_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "statemachine init failed as uri is invalid.");
        return false;
    }
    // check3:object's uri is empty
    if (strlen(uri_) == 0) {
        ace_free(uri_);
        uri_ = nullptr;
        HILOG_ERROR(HILOG_MODULE_ACE, "statemachine init failed as uri is empty.");
        jsRes =
            jerry_create_error(JERRY_ERROR_URI, reinterpret_cast<const jerry_char_t *>("uri value can't be empty."));
        return false;
    }
    int result = GenerateJsPagePath(uri_);
    // check4:generate js file's path failed
    if (result != SUCCESS) {
        ace_free(uri_);
        uri_ = nullptr;
        HILOG_ERROR(HILOG_MODULE_ACE, "statemachine init failed as generating js file's path failed.");
        jsRes = jerry_create_error(JERRY_ERROR_URI, reinterpret_cast<const jerry_char_t *>("uri value path error."));
        return false;
    }
    // check5:object's uri is not existed, need to move
    appContext_->SetCurrentJsPath(jsPagePath_);
    if (!CheckJSSourceFile()) {
        ace_free(uri_);
        uri_ = nullptr;
        HILOG_ERROR(HILOG_MODULE_ACE, "statemachine init failed as js file isn't existed.");
        jsRes = jerry_create_error(JERRY_ERROR_URI,
                                   reinterpret_cast<const jerry_char_t *>("route target doesn't existed."));
        return false;
    }
    return true;
}

bool StateMachine::CheckJSSourceFile() const
{
    char *fullPath = RelocateJSSourceFilePath(appRootPath_, jsPagePath_);
    if (fullPath == nullptr) {
        return false;
    }

    bool result = false;
    do {
        size_t pathLength = strlen(fullPath);
        const uint8_t fileSuffixLength = 3;
        if ((pathLength == 0) || (pathLength > PATH_LENGTH_MAX) || (pathLength < fileSuffixLength)) {
            break;
        }

        result = (GetFileSize(fullPath) > 0);
        if (result) {
            // try first one mode successfully
            break;
        }

        const char * const anotherSuffix = (JsAppEnvironment::GetInstance()->IsSnapshotMode()) ? ".js" : ".bc";
        // change file suffix to another mode file
        if (strcpy_s((fullPath + (pathLength - fileSuffixLength)), (fileSuffixLength + 1), anotherSuffix) != EOK) {
            break;
        }
        result = (GetFileSize(fullPath) > 0);
    } while (0);

    ace_free(fullPath);
    fullPath = nullptr;

    return result;
}

void StateMachine::BindParameters()
{
    if (uri_ == nullptr) {
        return;
    }
    jerry_value_t params = jerryx_get_property_str(object_, ROUTER_PAGE_PARAMS);
    RegisterUriAndParamsToPage(uri_, params);
    jerry_release_value(params);
    ace_free(uri_);
    uri_ = nullptr;
}

void StateMachine::ChangeState(int newState)
{
    if ((newState <= UNDEFINED_STATE) || (newState >= END_STATE)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "error input state:%{public}d", newState);
        return;
    }

    //多页面场景的动画修正
    if (newState == SHOW_STATE) {
        Component::HandlerAnimations(rootComponent_->GetComponentRootView());
    }

#if (BES_FRATURE_PAGE_TRANSITION == 1)
    switch (newState) {
        case BACKGROUND_STATE:
            if (!IsFinishing()) {
                Component::StopAnimations(rootComponent_->GetComponentRootView());
                if (appContext_ != nullptr) appContext_->ReleaseLazyLoadManager();
                ReleaseRootObject();
            }
            break;
        case SHOW_STATE:
            //as soon as possible.
            LazyLoadManager *llm = const_cast<LazyLoadManager *>(appContext_->GetLazyLoadManager());
            if (llm->GetState() == LazyLoadState::READY) llm->RenderLazyLoadWatcher();
            break;
    }

    // jump to new State
    State *state = stateMap_[newState];
    if (state != nullptr) {
        state->Handle(*this);
    }

    if (newState == BACKGROUND_STATE) {
        //状态机状态变更后，置位，使得下一次 background-state 到来时，执行真正的进入后台处理逻辑
        SetFinishing(true);
    } else if (newState == SHOW_STATE) {
        SetFinishing(false);
    }
#else
    //页面进入后台，总是停止动画
    if (newState == BACKGROUND_STATE) {
        Component::StopAnimations(rootComponent_->GetComponentRootView());
    }

    // jump to new State
    State *state = stateMap_[newState];
    if (state != nullptr) {
        state->Handle(*this);
    }
#endif

    if(newState == SHOW_STATE || newState == BACKGROUND_STATE) {
        HandleFragmentState(newState);
    }
}

void StateMachine::EvalPage()
{
    // load user's js code and eval it, rootPath/jsPagePath_.
    // for example, router.replace("/"), should load rootPath/index/index.js
    // router.replace("pages/detail/detail"), should load rootPath/pages/detail/detail.js
    char *pageFilePath = RelocateJSSourceFilePath(appRootPath_, jsPagePath_);
    if (pageFilePath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "relocat page JS file failed");
        return;
    }

    jerry_value_t evalResult = appContext_->Eval(pageFilePath, strlen(pageFilePath), false);
    if (IS_UNDEFINED(evalResult)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Eval JS file failed");
        ace_free(pageFilePath);
        pageFilePath = nullptr;
        return;
    }
    viewModel_ = evalResult;

    jerry_value_t params = jerryx_get_property_str(object_, ROUTER_PAGE_PARAMS);
    jerry_value_t keys = jerry_get_object_keys(params);
    uint16_t size = jerry_get_array_length(keys);
    for (uint16_t idx = 0; idx < size; ++idx) {
        jerry_value_t key = jerry_get_property_by_index(keys, idx);
        jerry_value_t value = jerry_get_property(params, key);
        jerry_release_value(jerry_set_property(viewModel_, key, value));
        ReleaseJerryValue(value, key, VA_ARG_END_FLAG);
    }
    ReleaseJerryValue(keys, params, VA_ARG_END_FLAG);

    ace_free(pageFilePath);
    pageFilePath = nullptr;
}

static void ForceGC(void *data)
{
    static_cast<void>(data);
    jerry_gc(jerry_gc_mode_t::JERRY_GC_PRESSURE_HIGH);
#if IS_ENABLED(JS_PROFILER)
    if (JSProfiler::GetInstance()->IsEnabled()) {
        // dump the JS heap status
        JSHeapStatus heapStatus;
        if (JSI::GetJSHeapStatus(heapStatus)) {
            HILOG_DEBUG(HILOG_MODULE_ACE, "JS Heap allocBytes[%{public}d], peakAllocBytes[%{public}d]",
                        heapStatus.allocBytes, heapStatus.peakAllocBytes);
        }
    }
#endif
}

void StateMachine::RenderPage()
{
    START_TRACING(RENDER);
    // if not in init state, reset all watchers of previous page at first
    LazyLoadManager *lazy = const_cast<LazyLoadManager *>(appContext_->GetLazyLoadManager());
    if (lazy != nullptr) {
        lazy->ResetWatchers();
    }
    // Note: do not release the returned value by Render function, it will be released by component
    jerry_value_t element = appContext_->Render(viewModel_);

    rootComponent_ = ComponentUtils::GetComponentFromBindingObject(element);
    // append scroll layer to the outermost view
    scrollLayer_ = new ScrollLayer();
    if (scrollLayer_ != nullptr) {
        scrollLayer_->AppendScrollLayer(rootComponent_);
    }

    //Move to ChangeState()
    //Component::HandlerAnimations(rootComponent_->GetComponentRootView());
    //Component::HandlerAnimations();

    // trigger an async full GC after completing the heavy work, which will
    // be executed after the whole page showing process
    JsAsyncWork::DispatchAsyncWork(ForceGC, nullptr);
    STOP_TRACING();
}

void StateMachine::ShowPage() const
{
    if (isEntireHidden_) {
        HILOG_WARN(HILOG_MODULE_ACE, "showpage: the whole application already in background, do not operate rootview");
        return;
    }
    START_TRACING(ADD_TO_ROOT_VIEW);
    if (scrollLayer_ != nullptr) {

#if (BES_FRATURE_PAGE_TRANSITION == 1)
        bool forceBackground = false;
        if (IsEnterTransitionPage() && pageTransParam_ != nullptr) {
            ////立即改变透明度，消除画面抖动
            // if (!strcmp(pageTransParam_->animName, BES_ANIM_TRANSLATE_X)
            //         || !strcmp(pageTransParam_->animName, BES_ANIM_TRANSLATE_Y)
            //         || !strcmp(pageTransParam_->animName, BES_ANIM_SCALE)) {
                scrollLayer_->GetLayerRootView().SetOpaScale(OPA_TRANSPARENT);
            // }
            forceBackground = pageTransParam_->forceBackground;
        }

        //应用转场强调
        scrollLayer_->Show(forceBackground);
#else
        scrollLayer_->Show();
#endif
    }
    STOP_TRACING();
    SYS_MEMORY_TRACING();
    JERRY_MEMORY_TRACING();
}

void StateMachine::HidePage() const
{
    if (isEntireHidden_) {
        HILOG_WARN(HILOG_MODULE_ACE, "hidepage: the whole application already in background, do not operate rootview");
        return;
    }
    if (scrollLayer_ != nullptr) {
        scrollLayer_->Hide();
    }
    // trigger an async full GC after hiding the page
    JsAsyncWork::DispatchAsyncWork(ForceGC, nullptr);
}

void StateMachine::InvokePageLifeCycleCallback(const char * const name) const
{
    if (FatalHandler::GetInstance().IsJSRuntimeFatal()) {
        // can not continue to involve any JS object creating on engine in case runtime fatal
        return;
    }
    if ((name == nullptr) || (strlen(name) == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "input parameter is invalid when invoking page life cycle callback.");
        return;
    }

    jerry_value_t function = jerryx_get_property_str(viewModel_, name);
    if (IS_UNDEFINED(function)) {
        // user does not set onInit method
        return;
    }
    CallJSFunctionAutoRelease(function, viewModel_, nullptr, 0);
    jerry_release_value(function);
}

void StateMachine::ReleaseRootObject() const
{
    if (FatalHandler::GetInstance().IsJSRuntimeFatal()) {
        // can not continue to involve any JS object creating on engine in case runtime fatal
        return;
    }
    // delete params properties from global object
    jerry_value_t globalObject = jerry_get_global_object();
    if (hasParams_) {
        jerry_value_t pagePropName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(ROUTER_PAGE));
        bool success = jerry_delete_property(globalObject, pagePropName);
        if (!success) {
            HILOG_ERROR(HILOG_MODULE_ACE, "delete $page property failed from global object.");
        }
        jerry_release_value(pagePropName);
    }

    // delete "$root" attribute from global object
    jerry_value_t appPropName = jerry_create_string(reinterpret_cast<const jerry_char_t *>(ATTR_ROOT));
    bool success = jerry_delete_property(globalObject, appPropName);
    if (!success) {
        HILOG_ERROR(HILOG_MODULE_ACE, "delete $root property failed from global object.");
    }
    ReleaseJerryValue(globalObject, appPropName, VA_ARG_END_FLAG);
}

void StateMachine::ReleaseHistoryPageResource()
{
    // remove all native views and release components styles.
    if (appContext_ != nullptr) {
        appContext_->ReleaseStyles(jsPagePath_);

        //remove all native views and release components styles.
        appContext_->RemovePageWatchers(rootComponent_->GetComponentRootView());
    }

    // release animations
    Component::ReleaseAnimations(rootComponent_->GetComponentRootView());

    // release scroll layer object.
    if (scrollLayer_ != nullptr) {
        if (!isEntireHidden_) {
            // if the whole application is in background, avoid operating the rootview
            scrollLayer_->DetachFromRootView();
        }
        delete (scrollLayer_);
        scrollLayer_ = nullptr;
    }
    // if some fatal error happens and is hanled by FatalHandler, the resource is already
    // recycled by it, do not repeat the recycling
    if (!FatalHandler::GetInstance().IsFatalErrorHandleDone()) {
        // release all native views and their binding js objects
        ComponentUtils::ReleaseComponents(rootComponent_);
        rootComponent_ = nullptr;
    }

    // release current page's viewModel js object
    jerry_release_value(viewModel_);

#if (BES_FRATURE_PAGE_TRANSITION == 1)
    //Skip check the components leak.
#else
    ReleaseRootObject();

    // check components leak
    uint16_t remainComponentCount = FatalHandler::GetInstance().GetComponentCount();
    if (remainComponentCount != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "[%{public}d] components leaked!", remainComponentCount);
    }
#endif

    //TODO: 是否必须前置？ 这些全局 module 会主动复用
    // clean up native module objects required
    ModuleManager::GetInstance()->CleanUpModule();
}

void StateMachine::DeleteViewModelProperties() const
{
    if (IS_UNDEFINED(viewModel_)) {
        return;
    }
    jerry_value_t keys = jerry_get_object_keys(viewModel_);
    uint32_t size = jerry_get_array_length(keys);
    for (uint32_t index = 0; index < size; index++) {
        jerry_value_t key = jerry_get_property_by_index(keys, index);
        jerry_delete_property(viewModel_, key);
        jerry_release_value(key);
    }
    jerry_release_value(keys);
}

void StateMachine::SetHiddenFlag(bool flag)
{
    isEntireHidden_ = flag;
}

void StateMachine::HandleFragmentState(int newState)
{
    if(fragmentList_ == nullptr || fragmentList_->size() == 0){
        return;
    }

    auto it = fragmentList_->begin();

    while (it != fragmentList_->end()){
        if(newState == SHOW_STATE){
            (*it)->ChangeState(FRAGMENT_SHOW_STATE);
        }else if(newState == BACKGROUND_STATE){
            (*it)->ChangeState(FRAGMENT_HIDE_STATE);
        }
        it++;
    }

}

void StateMachine::AddFragment(FragmentStateMachine * fragmentSM)
{
    if(fragmentSM == nullptr){
        return;
    }

    if(fragmentList_ == nullptr){
        fragmentList_ = new std::list<FragmentStateMachine*>(); 
    }

    fragmentList_->push_back(fragmentSM);


}

void StateMachine::RemoveFragment(FragmentStateMachine * fragmentSM)
{
    if(fragmentList_ == nullptr || fragmentSM == nullptr  ){
        return;
    }

    fragmentList_->remove(fragmentSM);


}

#ifdef TDD_ASSERTIONS
void StateMachine::SetViewModel(jerry_value_t viewModel)
{
    DeleteViewModelProperties();
    jerry_release_value(viewModel_);
    viewModel_ = viewModel;
    // should add all router param to new view model again?
}
#endif // TDD_ASSERTIONS

#if (BES_FRATURE_PAGE_TRANSITION == 1)
BasePageTransCallback::~BasePageTransCallback()
{
    if (cb_ != nullptr) {
        delete cb_;
        cb_ = nullptr;
    }
}

void BasePageTransCallback::SetExtCallback(PageTransCallback* cb)
{
    if (cb_ != nullptr) {
        delete cb_;
        cb_ = nullptr;
    }
    cb_ = cb;
}

void BasePageTransCallback::OnStart()
{
    if (sm_->IsEnterTransitionPage()) RootView::GetInstance()->SetPageTransitionState(true);
    if (cb_ != nullptr) cb_->OnStart(*sm_);
}

void BasePageTransCallback::Callback(UIView* view)
{
    //check.
    if (sm_->IsEnterTransitionPage()) {
        if (view->GetOpaScale() == OPA_TRANSPARENT) {
            view->SetOpaScale(OPA_TRANSPARENT + 1);
        } else if (view->GetOpaScale() != OPA_OPAQUE) {
            view->SetOpaScale(OPA_OPAQUE);
        }

        PageTransitionParam *param = sm_->pageTransParam_;

        if (cb_ != nullptr) cb_->Callback(view, *sm_, param);

        do {
            if (view == nullptr || param == nullptr) break;
            if (param->animDelay <= 0 || delayTime_ == UINT32_MAX) break;
            if (delayTime_ == 0) {
                delayTime_ = HALTick::GetInstance().GetTime() + param->animDelay;
            }

            if (HALTick::GetInstance().GetTime() > delayTime_) {
                view->SetOpaScale(OPA_OPAQUE);
                //view->Invalidate();
                delayTime_ = UINT32_MAX;
            } else {
                view->SetOpaScale(OPA_TRANSPARENT);
                //view->Invalidate();
            }
        } while (false);
    }
}

void BasePageTransCallback::OnStop(UIView& view)
{
    bool isEnterPage = sm_->IsEnterTransitionPage();
    //recheck.
    if (view.GetOpaScale() != OPA_OPAQUE && isEnterPage) {
        view.SetOpaScale(OPA_OPAQUE);
        view.Invalidate();
    }

    UIView *pageRootView = sm_->rootComponent_->GetComponentRootView();
    if (pageRootView->GetOpaScale() != OPA_OPAQUE && isEnterPage) {
        pageRootView->SetOpaScale(OPA_OPAQUE);
        pageRootView->Invalidate();
    }

    if (isEnterPage) {
        RootView::GetInstance()->SetPageTransitionState(false);
    }
    if (cb_ != nullptr) cb_->OnStop(view, *sm_, sm_->GetPrevSm());
}

bool StateMachine::IsFinishing() const
{
    return isFinishing_;
}

void StateMachine::ReversePageTransition(jerry_value_t paramObj)
{
    if (!IsEnterTransitionPage()) return;

    StateMachine *enterSm = prevSm_;
    prevSm_->prevSm_ = this;
    prevSm_ = nullptr;

    //清除退场动画回调
    if (pageTransCallback_ != nullptr) {
        delete pageTransCallback_;
        pageTransCallback_ = nullptr;
    }

    ReversePageTransitionParam();
    enterSm->ReversePageTransitionParam();
}

StateMachine* StateMachine::GetPrevSm()
{
    return prevSm_;
}

void StateMachine::SetFinishing(bool isFinishing)
{
    isFinishing_ = isFinishing;
}

void StateMachine::RecordPageTransition(StateMachine* prevSm, jerry_value_t paramObj)
{
    if (prevSm == nullptr || IS_UNDEFINED(paramObj)) return;

    //2023年3月10日：旧页面总是需要被延迟销毁，记录旧页面状态机
    SetPrevSm(prevSm);

    jerry_value_t transField = jerryx_get_property_str(paramObj, BES_FIELD_PAGE_TRANSITION);

    do {
        if (IS_UNDEFINED(transField)) break;

        //1. perfer to preset anim.
        uint16_t len = 0;
        char* presetAnim = JerryMallocStringProperty(transField, BES_FIELD_PRESET_ANIM, len);
        if (presetAnim != nullptr) {
            ParsePresetAnim(presetAnim);
            if (pageTransParam_ != nullptr) {
                prevSm->ParsePresetAnim(presetAnim);
            }
            ACE_FREE(presetAnim);
            break;
        }

        //2. ensure the transition param is clear.
        SetPageTransitionParam(nullptr);
        prevSm->SetPageTransitionParam(nullptr);

        //3. transition field.
        ParseCommonParam(transField);
        prevSm->ParseCommonParam(transField);

        //4. enter and exit transition field.

        jerry_value_t enterField = jerryx_get_property_str(transField, BES_FIELD_ENTER);
        
        if (IS_UNDEFINED(enterField)) {
            jerry_release_value(enterField);
            break;
        }
        SetPageTransitionParam(&enterField);
        jerry_release_value(enterField);

        //当且仅当存在入场动画，才去关注退场动画
        jerry_value_t exitField = jerryx_get_property_str(transField, BES_FIELD_EXIT);
        if (!IS_UNDEFINED(exitField)) prevSm->SetPageTransitionParam(&exitField);
        jerry_release_value(exitField);
    } while (false);
    jerry_release_value(transField);

    jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(BES_FIELD_PAGE_TRANSITION));
    jerry_delete_property(paramObj, key);
    jerry_release_value(key);
}

void StateMachine::StartPageTransition()
{
    if (scrollLayer_ == nullptr) return;

    if (pageTransCallback_ != nullptr) pageTransCallback_->OnStart();

    if (pageTransParam_ == nullptr) {
        HILOG_INFO(HILOG_MODULE_ACE, "Skip the page transition anim, except the param");
        if (pageTransCallback_ != nullptr) {
            pageTransCallback_->OnStop(scrollLayer_->GetLayerRootView());
        }
        return;
    }

    //新页面先入场，旧页面后退场
    scrollLayer_->StartPageTransition(pageTransParam_, pageTransCallback_);

    if (prevSm_ != nullptr) {
        prevSm_->StartPageTransition();
    }
}

void StateMachine::ReleasePageTransition()
{
    if (IsEnterTransitionPage()) {
        scrollLayer_->GetLayerRootView().SetOpaScale(OPA_OPAQUE);
    }

    if (pageTransCallback_ != nullptr) {
        delete pageTransCallback_;
        pageTransCallback_ = nullptr;
    }

    SetPageTransitionParam(nullptr);

    SetPrevSm(nullptr);
}

bool StateMachine::SetPageTransCallback(PageTransCallback* cb)
{
    if (RootView::GetInstance()->IsPageTransitionNow()) return false;

    if (pageTransCallback_ == nullptr) {
        pageTransCallback_ = new BasePageTransCallback(this);
        if (pageTransCallback_ == nullptr) {
            return false;
        }
    }
    pageTransCallback_->SetExtCallback(cb);

    return true;
}

bool StateMachine::IsEnterTransitionPage() const
{
    return prevSm_ != nullptr;
}

void StateMachine::SetPageTransitionParam(const jerry_value_t* animField)
{
    if (animField == nullptr) {
        if (pageTransParam_ != nullptr) {
            delete pageTransParam_;
            pageTransParam_ = nullptr;
        }
        return;
    }

    if (pageTransParam_ == nullptr) {
        pageTransParam_ = new PageTransitionParam();
        if (pageTransParam_ == nullptr) return;
    }

    do {
        uint16_t len = 0;
        
        //name
        char* nameStr = JerryMallocStringProperty(*animField, BES_FIELD_ANIM_NAME, len);
        if (nameStr != nullptr) {
            char* animName = nullptr;
            if (!strcmp(nameStr, BES_ANIM_TRANSLATE_X)) {
                animName = BES_ANIM_TRANSLATE_X;
            } else if (!strcmp(nameStr, BES_ANIM_TRANSLATE_Y)) {
                animName = BES_ANIM_TRANSLATE_Y;
            } else if (!strcmp(nameStr, BES_ANIM_OPACITY)) {
                animName = BES_ANIM_OPACITY;
            } else if (!strcmp(nameStr, BES_ANIM_SCALE)) {
                animName = BES_ANIM_SCALE;
            } else {
                ACE_FREE(nameStr);
                break;
            }
            pageTransParam_->animName = animName;
            ACE_FREE(nameStr);
        } else {
            if (pageTransParam_->animName == nullptr) break;
        }

        //interpolation
        char* interpolation = JerryMallocStringProperty(*animField, BES_FIELD_ANIM_INTERPOLATION, len);
        if (interpolation != nullptr) {
            EasingType type = EasingType::LINEAR;
            if (!strcmp(interpolation, VAL_EASE_IN)) {
                type = EasingType::EASE_IN;
            } else if (!strcmp(interpolation, VAL_EASE_OUT)) {
                type = EasingType::EASE_OUT;
            } else if (!strcmp(interpolation, VAL_EASE_IN_OUT)) {
                type = EasingType::EASE_IN_OUT;
            }
            pageTransParam_->animInterpolation = type;

            ACE_FREE(interpolation);
        }

        //duration
        jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(BES_FIELD_ANIM_DURATION));
        if (JerryHasProperty(*animField, key)) {
            int16_t duration = JerryGetIntegerProperty(*animField, BES_FIELD_ANIM_DURATION);
            if (duration > 0) {
                pageTransParam_->animDuration = MATH_MIN(MATH_MAX(TRANSITION_DURATION_MIN, duration), TRANSITION_DURATION_MAX);
            }
        }
        jerry_release_value(key);
        if (pageTransParam_->animDuration <= 0) break;

        //values
        jerry_value_t valuesField = jerryx_get_property_str(*animField, BES_FIELD_ANIM_VALUES);
        if (IS_UNDEFINED(valuesField)) break;
        if (jerry_value_is_array(valuesField)) {
            uint32_t len = jerry_get_array_length(valuesField);
            if (len < 2) {
                HILOG_WARN(HILOG_MODULE_ACE, "Except anim value of page transition! get %lu(less than 2)", len);
            } else {
                jerry_value_t sValueField = jerry_get_property_by_index(valuesField, 0);
                jerry_value_t eValueField = jerry_get_property_by_index(valuesField, 1);
                if (!IS_UNDEFINED(sValueField)) pageTransParam_->animValues[0] = IntegerOf(sValueField);
                if (!IS_UNDEFINED(eValueField)) pageTransParam_->animValues[1] = IntegerOf(eValueField);
                ReleaseJerryValue(sValueField, eValueField, VA_ARG_END_FLAG);

                //extra values.
                ParseExtraAnimValues(pageTransParam_->animName, valuesField, len);
            }
        }
        jerry_release_value(valuesField);

        //fade
        char* fadeField = JerryMallocStringProperty(*animField, BES_FIELD_ANIM_FADE, len);
        if (fadeField != nullptr) {
            BesAnimFade animFade = BesAnimFade::FADE_NONE;
            if (!strcmp(fadeField, VAL_FADE_IN)) {
                animFade = BesAnimFade::FADE_IN;
            } else if (!strcmp(fadeField, VAL_FADE_OUT)) {
                animFade = BesAnimFade::FADE_OUT;
            }
            pageTransParam_->animFade = animFade;
        }
        ACE_FREE(fadeField);

        //pivot
        char* pivotField = JerryMallocStringProperty(*animField, BES_FIELD_ANIM_PIVOT, len);
        pageTransParam_->animPivot = ParseAnimPivot(pivotField);
        ACE_FREE(pivotField);

        //delay
        int16_t delayField = JerryGetIntegerProperty(*animField, BES_FIELD_ANIM_DELAY);
        pageTransParam_->animDelay = MATH_MIN(MATH_MAX(delayField, 0), pageTransParam_->animDuration - 1);

        if (pageTransParam_->animValues[0] == pageTransParam_->animValues[1]) break;

        //successfully.
        return;
    } while (false);

    //failure.
    if (pageTransParam_ != nullptr) {
        delete pageTransParam_;
        pageTransParam_ = nullptr;
    }
}

void StateMachine::ParseExtraAnimValues(const char* const anim, jerry_value_t animValuesField, uint32_t len)
{
    if (anim == nullptr || IS_UNDEFINED(animValuesField)) return;

    if (!strcmp(anim, BES_ANIM_SCALE)) {
        bool xEnable = true, yEnable = true;
        if (len >= 4) {
            jerry_value_t xEnableField = jerry_get_property_by_index(animValuesField, 2);
            jerry_value_t yEnableField = jerry_get_property_by_index(animValuesField, 3);
            if (!IS_UNDEFINED(xEnableField) && !IS_UNDEFINED(yEnableField)) {
                xEnable = IntegerOf(xEnableField) != 0;
                yEnable = IntegerOf(yEnableField) != 0;
            }
            ReleaseJerryValue(xEnableField, yEnableField, VA_ARG_END_FLAG);
        }
        BesTransitionImpl::SetScaleXYEnable(pageTransParam_->animValues[0], xEnable, yEnable);
        BesTransitionImpl::SetScaleXYEnable(pageTransParam_->animValues[1], xEnable, yEnable);
    }
}

BesAnimPivot StateMachine::ParseAnimPivot(const char* const pivotName)
{
    BesAnimPivot pivot = BesAnimPivot::CENTER;
    if (pivotName == nullptr)  return BesAnimPivot::CENTER;

    if (!strcmp(pivotName, VAL_PIVOT_LT)) {
        pivot = BesAnimPivot::LEFT_TOP;
    } else if (!strcmp(pivotName, VAL_PIVOT_RT)) {
        pivot = BesAnimPivot::RIGHT_TOP;
    } else if (!strcmp(pivotName, VAL_PIVOT_LB)) {
        pivot = BesAnimPivot::LEFT_BOTTOM;
    } else if (!strcmp(pivotName, VAL_PIVOT_RB)) {
        pivot = BesAnimPivot::RIGHT_BOTTOM;
    } else if (!strcmp(pivotName, VAL_PIVOT_LC)) {
        pivot = BesAnimPivot::LEFT_CENTER;
    } else if (!strcmp(pivotName, VAL_PIVOT_RC)) {
        pivot = BesAnimPivot::RIGHT_CENTER;
    } else if (!strcmp(pivotName, VAL_PIVOT_TC)) {
        pivot = BesAnimPivot::TOP_CENTER;
    } else if (!strcmp(pivotName, VAL_PIVOT_BC)) {
        pivot = BesAnimPivot::BOTTOM_CENTER;
    }

    return pivot;
}

void StateMachine::SetPrevSm(StateMachine* prevSm)
{
    if (prevSm != nullptr) {
        prevSm->prevSm_ = nullptr;
    }

    if (prevSm_ != nullptr) {
        HILOG_WARN(HILOG_MODULE_ACE, "Prev page[%p] is already exist! release forever", prevSm_);
        delete prevSm_;
        prevSm_ = nullptr;
    }
    prevSm_ = prevSm;
}

void StateMachine::ReversePageTransitionParam()
{
    //反转淡入淡出
    if (pageTransParam_ != nullptr && pageTransParam_->animFade != BesAnimFade::FADE_NONE) {
        pageTransParam_->animFade = pageTransParam_->animFade == BesAnimFade::FADE_IN ? BesAnimFade::FADE_OUT : BesAnimFade::FADE_IN;
    }

    if (pageTransParam_ != nullptr) {
        //反转动画数值
        char *animName = pageTransParam_->animName;
        int16_t *animValues = pageTransParam_->animValues;
        if (!strcmp(animName, BES_ANIM_TRANSLATE_X) 
            || !strcmp(animName, BES_ANIM_TRANSLATE_Y)) {
            int16_t tmp = animValues[0];
            animValues[0] = 0;
            animValues[1] = tmp - animValues[1];
        } else {
            animValues[0] ^= animValues[1];
            animValues[1] ^= animValues[0];
            animValues[0] ^= animValues[1];
        }
    }

    if (!IsEnterTransitionPage()) return;

    PageTransitionParam *exitParam = prevSm_->pageTransParam_;
    if (pageTransParam_ != nullptr) {
        //交换延时
        uint16_t delay = exitParam == nullptr ? 0 : exitParam->animDelay;
        if (delay > 0) {
            pageTransParam_->animDelay = delay;
            pageTransParam_->animDuration += delay;

            if (exitParam != nullptr) {
                exitParam->animDelay = 0;
                exitParam->animDuration -= delay;
            }
        }
    } else {
        //转场补偿
        pageTransParam_ = new PageTransitionParam();
        pageTransParam_->Clone(PRESET_ANIM_OPACITY);
        pageTransParam_->animValues[0] = OPA_TRANSPARENT;
        pageTransParam_->animValues[1] = OPA_OPAQUE;
        pageTransParam_->forceBackground = true;

        if (prevSm_->pageTransParam_ != nullptr) {
            pageTransParam_->animDuration = prevSm_->pageTransParam_->animDuration;
        }
    }

    //交换明暗关系
    if (!pageTransParam_->forceBackground && (exitParam != nullptr && !exitParam->forceBackground)) {
        pageTransParam_->forceBackground = true;
        exitParam->forceBackground = false;
    }
}

void StateMachine::ParseCommonParam(jerry_value_t commonField)
{
    if (IS_UNDEFINED(commonField)) return;

    if (pageTransParam_ == nullptr) {
        pageTransParam_ = new PageTransitionParam();
        if (pageTransParam_ == nullptr) return;
    }

    uint16_t len = 0;

    //perfer
    char *perferValue = JerryMallocStringProperty(commonField, BES_FIELD_ANIM_PERFER, len);
    if (perferValue != nullptr) {
        if (!strcmp(perferValue, BES_FIELD_EXIT)) {
            pageTransParam_->forceBackground = IsEnterTransitionPage();
        }
    }
    ACE_FREE(perferValue);

    //name
    char* str = JerryMallocStringProperty(commonField, BES_FIELD_ANIM_NAME, len);
    if (str != nullptr) {
        char* name = nullptr;
        if (!strcmp(str, BES_ANIM_TRANSLATE_X)) {
            name = BES_ANIM_TRANSLATE_X;
        } else if (!strcmp(str, BES_ANIM_TRANSLATE_Y)) {
            name = BES_ANIM_TRANSLATE_Y;
        } else if (!strcmp(str, BES_ANIM_OPACITY)) {
            name = BES_ANIM_OPACITY;
        } else if (!strcmp(str, BES_ANIM_SCALE)) {
            name = BES_ANIM_SCALE;
        }
        pageTransParam_->animName = name;
        ACE_FREE(str);
    }

    //duration
    jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t *>(BES_FIELD_ANIM_DURATION));
    if (JerryHasProperty(commonField, key)) {
        int16_t duration = JerryGetIntegerProperty(commonField, BES_FIELD_ANIM_DURATION);
        pageTransParam_->animDuration = duration;
    }
    jerry_release_value(key);

    //interpolation
    char* interpolation = JerryMallocStringProperty(commonField, BES_FIELD_ANIM_INTERPOLATION, len);
    if (interpolation != nullptr) {
        EasingType ease = EasingType::LINEAR;
        if (!strcmp(interpolation, VAL_EASE_IN)) {
            ease = EasingType::EASE_IN;
        } else if (!strcmp(interpolation, VAL_EASE_OUT)) {
            ease = EasingType::EASE_OUT;
        } else if (!strcmp(interpolation, VAL_EASE_IN_OUT)) {
            ease = EasingType::EASE_IN_OUT;
        }
        pageTransParam_->animInterpolation = ease;
    }
    ACE_FREE(interpolation);
}

void StateMachine::ParsePresetAnim(const char* const presetAnim)
{
    bool isEnterPage = IsEnterTransitionPage();
    if (pageTransParam_ == nullptr) pageTransParam_ = new PageTransitionParam();

    pageTransParam_ = BesTransitionParser::ParsePresetTransitionAnim(presetAnim, isEnterPage, pageTransParam_);
}
#endif //BES_FRATURE_PAGE_TRANSITION
} // namespace ACELite
} // namespace OHOS
