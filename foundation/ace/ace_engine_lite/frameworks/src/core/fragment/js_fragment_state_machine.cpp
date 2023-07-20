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

#include "js_fragment_state_machine.h"
#include "ace_log.h"
#include "js_app_environment.h"
#include "js_fragment_state.h"

namespace OHOS{
namespace ACELite{

FragmentStateMachine::FragmentStateMachine()
{
    currentState_ = FRAGMENT_UNDEFINED_STATE;
    viewModel_ = UNDEFINED;
    appContext_ = nullptr;
    rootComponent_ = nullptr;
    appRootPath_ = nullptr;
    jsFragemntPath_ = nullptr;

}


FragmentStateMachine::~FragmentStateMachine()
{
    for (int i = 0; i < FRAGMENT_STATE_SIZE; i++) {
        if (stateMap_[i] != nullptr) {
            delete stateMap_[i];
            stateMap_[i] = nullptr;
        }
    }

    if(jsFragemntPath_ != nullptr){
        ace_free(jsFragemntPath_);
        jsFragemntPath_ = nullptr;
    }

    jerry_release_value(viewModel_);
}

bool FragmentStateMachine::Init(const char *path)
{
    appContext_ = JsAppContext::GetInstance();
    appRootPath_ = const_cast<char *>(appContext_->GetCurrentAbilityPath());

    if ((appRootPath_ == nullptr) || (strlen(appRootPath_) == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "fragment statemachine init failed as this app's root path is invalid.");
        return false;
    }

    size_t len = strlen(path);
    const char * const sourceFileSuffix = (JsAppEnvironment::GetInstance()->IsSnapshotMode()) ? ".bc" : ".js";
    len = len + strlen(sourceFileSuffix) + 1;

    jsFragemntPath_ = static_cast<char *>(ace_malloc(len));
    if(jsFragemntPath_ == nullptr){
        return false;
    }

    jsFragemntPath_[0] = '\0';
    strcpy_s(jsFragemntPath_, len, path);
    strcat_s(jsFragemntPath_, len, sourceFileSuffix);

    stateMap_[FRAGMENT_INIT_STATE] = new FragmentInitState();
    stateMap_[FRAGMENT_READY_STATE] = new FragmentReadyState();
    stateMap_[FRAGMENT_SHOW_STATE] = new FragmentShowState();
    stateMap_[FRAGMENT_HIDE_STATE] = new FragmentHideState();
    stateMap_[FRAGMENT_DESTROY_STATE] = new FragmentDestroyState();

    return true;
}

void FragmentStateMachine::ChangeState(int newState)
{
    if((newState <= FRAGMENT_UNDEFINED_STATE) || (newState >= FRAGMENT_END_STATE)){
        HILOG_ERROR(HILOG_MODULE_ACE, "fargment state error input state:%{public}d", newState);
        return;
    }

    FragmentState *state = stateMap_[newState];
    if(state != nullptr){
        state->Handle(*this);
    }
}

void FragmentStateMachine::SetCurrentState(int8_t newState)
{
    if(newState <= FRAGMENT_UNDEFINED_STATE || newState >= FRAGMENT_END_STATE){
        HILOG_ERROR(HILOG_MODULE_ACE, "fargment state error input state:%d", newState);
        return;
    }

    currentState_ = newState;
}

void FragmentStateMachine::EvalFragment()
{
    char *pageFilePath = RelocateJSSourceFilePath(appRootPath_, jsFragemntPath_);
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
    ace_free(pageFilePath);
    pageFilePath = nullptr;
}

void FragmentStateMachine::RenderFragment()
{
    jerry_value_t element = appContext_->Render(viewModel_);
    rootComponent_ = ComponentUtils::GetComponentFromBindingObject(element);

}

void FragmentStateMachine::InvokeLifeCycleCallback(const char *name)
{
    if ((name == nullptr) || (strlen(name) == 0)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "input parameter is invalid when invoking fragment life cycle callback.");
        return;
    }

    jerry_value_t function = jerryx_get_property_str(viewModel_, name);
    if (IS_UNDEFINED(function)) {
        return;
    }
    CallJSFunctionAutoRelease(function, viewModel_, nullptr, 0);
    jerry_release_value(function);
}

} // ACELite
} // OHOS
