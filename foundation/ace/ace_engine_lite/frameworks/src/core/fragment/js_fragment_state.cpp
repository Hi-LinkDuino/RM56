/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "js_fragment_state.h"
#include "ace_log.h"

namespace OHOS{
namespace ACELite{

void FragmentInitState::Handle(FragmentStateMachine& fragmentSM)
{
    int currState = fragmentSM.GetCurrentState();
    if(currState == FRAGMENT_UNDEFINED_STATE){
        fragmentSM.SetCurrentState(FRAGMENT_INIT_STATE);
        HILOG_INFO(HILOG_MODULE_ACE, "fragment state undefined state -> init state");
        fragmentSM.EvalFragment();
        fragmentSM.InvokeLifeCycleCallback(FRAGMENT_LIFECYCLE_ON_INIT);
        fragmentSM.ChangeState(FRAGMENT_READY_STATE);
    } else{
        HILOG_ERROR(HILOG_MODULE_ACE, "current state(%{public}d) is invalid when changing to init state.",
                    currState);
    }

}


void FragmentReadyState::Handle(FragmentStateMachine& fragmentSM)
{
    int currState = fragmentSM.GetCurrentState();
    if (currState == FRAGMENT_INIT_STATE) {
        HILOG_INFO(HILOG_MODULE_ACE, "fragment init state -> ready state");
        // do this state's action:call render to setup user interface
        fragmentSM.RenderFragment();
        fragmentSM.InvokeLifeCycleCallback(FRAGMENT_LIFECYCLE_CALLBACK_ON_READY);
        fragmentSM.SetCurrentState(FRAGMENT_READY_STATE);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "fragment current state(%{public}d) is invalid when changing to ready state.",
                    currState);
    }

}


void FragmentShowState::Handle(FragmentStateMachine& fragmentSM)
{
    int currState = fragmentSM.GetCurrentState();
    if ((currState == FRAGMENT_READY_STATE) || (currState == FRAGMENT_HIDE_STATE)) {
        HILOG_INFO(HILOG_MODULE_ACE, "fragment current state(%{public}d) -> show state", currState);
        // do this state's action: call show to display user interface
        fragmentSM.InvokeLifeCycleCallback(FRAGMENT_LIFECYCLE_CALLBACK_ON_SHOW);
        fragmentSM.SetCurrentState(FRAGMENT_SHOW_STATE);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "fragment current state(%{public}d) is invalid when changing to show state.",
                    currState);
    }

}

void FragmentHideState::Handle(FragmentStateMachine& fragmentSM)
{
    int currState = fragmentSM.GetCurrentState();
    if ((currState == FRAGMENT_READY_STATE)|| (currState == FRAGMENT_SHOW_STATE)) {
        HILOG_INFO(HILOG_MODULE_ACE, "fragment current state(%{public}d) -> hide state", currState);
        fragmentSM.InvokeLifeCycleCallback(FRAGMENT_LIFECYCLE_CALLBACK_ON_HIDE);
        fragmentSM.SetCurrentState(FRAGMENT_HIDE_STATE);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "fragment current state(%{public}d) is invalid when changing to hide state.",
                    currState);
    }

}

void FragmentDestroyState::Handle(FragmentStateMachine& fragmentSM)
{
    int currState = fragmentSM.GetCurrentState();
    if ((currState >= FRAGMENT_INIT_STATE)) {
        HILOG_INFO(HILOG_MODULE_ACE, "fragment current state(%{public}d) -> destroy state", currState);
        fragmentSM.InvokeLifeCycleCallback(FRAGMENT_LIFECYCLE_CALLBACK_ON_DESTROY);
        fragmentSM.SetCurrentState(FRAGMENT_DESTROY_STATE);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "fragment current state(%{public}d) is invalid when changing to destory state.",
                    currState);
    }

}


} // ACELite
} // OHOS