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

#include "ace_event_error_code.h"
#include "ace_log.h"
#include "fatal_handler.h"
#include "js_page_state.h"
#include "js_profiler.h"

#include "graphic_config.h"

namespace OHOS {
namespace ACELite {
void PageInitState::Handle(StateMachine &sm)
{
    int currState = sm.GetCurrentState();
    if (currState == UNDEFINED_STATE) {
        sm.SetCurrentState(INIT_STATE);
        HILOG_INFO(HILOG_MODULE_ACE, "page[%p] undefined state -> init state", &sm);
        sm.BindParameters();
        // do this state's action: eval user js code and invoke life cycle callback
        sm.EvalPage();
        START_TRACING(PAGE_ON_INIT);
        sm.InvokePageLifeCycleCallback(PAGE_LIFECYCLE_ON_INIT);
        STOP_TRACING();
        sm.ChangeState(READY_STATE);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "current page[%p] state(%{public}d) is invalid when changing undefined state to init state.", &sm, currState);
    }
}

void PageReadyState::Handle(StateMachine &sm)
{
    int currState = sm.GetCurrentState();
    if (currState == INIT_STATE) {
        HILOG_INFO(HILOG_MODULE_ACE, "page[%p] init state -> ready state", &sm);
        // do this state's action:call render to setup user interface
        sm.RenderPage();
        START_TRACING(PAGE_ON_READY);
        sm.InvokePageLifeCycleCallback(PAGE_LIFECYCLE_CALLBACK_ON_READY);
        STOP_TRACING();
        sm.SetCurrentState(READY_STATE);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "current page[%p] state(%{public}d) is invalid when changing init state to ready state.",
                    &sm, currState);
    }
}

void PageShowState::Handle(StateMachine &sm)
{
    int currState = sm.GetCurrentState();
    if ((currState == READY_STATE) || (currState == BACKGROUND_STATE)) {
        HILOG_INFO(HILOG_MODULE_ACE, "current page[%p] state(%{public}d) -> show state", &sm, currState);
        // do this state's action: call show to display user interface
        sm.ShowPage();
        START_TRACING(PAGE_ON_SHOW);
        sm.InvokePageLifeCycleCallback(PAGE_LIFECYCLE_CALLBACK_ON_SHOW);
        STOP_TRACING();
        sm.SetCurrentState(SHOW_STATE);

#if (BES_FRATURE_PAGE_TRANSITION == 1)
        sm.StartPageTransition();
    } else {
        sm.ReleasePageTransition();
#else
    } else {
#endif
        HILOG_ERROR(HILOG_MODULE_ACE, "current page[%p] state(%{public}d) is invalid when changing ready state to show state.",
                    &sm, currState);
    }
}

void PageBackgroundState::Handle(StateMachine &sm)
{
    int currState = sm.GetCurrentState();

#if (BES_FRATURE_PAGE_TRANSITION == 1)
    if ((currState == READY_STATE) || (currState == SHOW_STATE)) {
        if (!sm.IsFinishing()) {
            START_TRACING(PAGE_ON_BACKGROUND);
            sm.InvokePageLifeCycleCallback(PAGE_LIFECYCLE_CALLBACK_ON_HIDE);
            STOP_TRACING();
        } else {
            HILOG_INFO(HILOG_MODULE_ACE, "current page[%p] state(%{public}d) -> background state", &sm, currState);
            sm.HidePage();
            sm.SetCurrentState(BACKGROUND_STATE);
        }
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "current page[%p] state(%{public}d) is invalid when changing show state to background state.", &sm, currState);
    }
#else
    if ((currState == READY_STATE) || (currState == SHOW_STATE)) {
        HILOG_INFO(HILOG_MODULE_ACE, "current page[%p] state(%{public}d) -> background state", &sm, currState);
        sm.HidePage();
        START_TRACING(PAGE_ON_BACKGROUND);
        sm.InvokePageLifeCycleCallback(PAGE_LIFECYCLE_CALLBACK_ON_HIDE);
        STOP_TRACING();
        sm.SetCurrentState(BACKGROUND_STATE);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "current page[%p] state(%{public}d) is invalid when changing show state to background state.", &sm, currState);
    }
#endif //BES_FRATURE_PAGE_TRANSITION
}

void PageDestroyState::Handle(StateMachine &sm)
{
    int currState = sm.GetCurrentState();
    // any normal state can jump to destroy state
    if ((currState >= INIT_STATE) || (FatalHandler::GetInstance().IsFatalErrorHitted())) {
        HILOG_INFO(HILOG_MODULE_ACE, "current page[%p] state(%{public}d) -> destroy state", &sm, currState);
        ACE_EVENT_PRINT(MT_ACE_RELEASE_HISTORY_PAGE, 0);
        START_TRACING(PAGE_ON_DESTROY);
        sm.InvokePageLifeCycleCallback(PAGE_LIFECYCLE_CALLBACK_ON_DESTROY);
        STOP_TRACING();
        sm.ReleaseHistoryPageResource();
        sm.SetCurrentState(DESTROY_STATE);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "current state(%{public}d) is invalid when changing show state to destroy state.",
                    currState);
    }
}
} // namespace ACELite
} // namespace OHOS
