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
#include "root_view.h"
#include "securec.h"

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
}

StateMachine::~StateMachine()
{
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
    if (!jerry_value_is_undefined(object_)) {
        jerry_release_value(object_);
    }
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
    // jump to new State
    State *state = stateMap_[newState];
    if (state != nullptr) {
        state->Handle(*this);
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
    Component::HandlerAnimations();
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
        scrollLayer_->Show();
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
        appContext_->ReleaseStyles();
        appContext_->ReleaseLazyLoadManager();
    }
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

    ReleaseRootObject();

    // release current page's viewModel js object
    jerry_release_value(viewModel_);

    // release animations
    Component::ReleaseAnimations();

    // clean up native module objects required
    ModuleManager::GetInstance()->CleanUpModule();
    // check components leak
    uint16_t remainComponentCount = FatalHandler::GetInstance().GetComponentCount();
    if (remainComponentCount != 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "[%{public}d] components leaked!", remainComponentCount);
    }
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

#ifdef TDD_ASSERTIONS
void StateMachine::SetViewModel(jerry_value_t viewModel)
{
    DeleteViewModelProperties();
    jerry_release_value(viewModel_);
    viewModel_ = viewModel;
    // should add all router param to new view model again?
}
#endif // TDD_ASSERTIONS
} // namespace ACELite
} // namespace OHOS
