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


#ifndef OHOS_ACELITE_JS_FRAGMENT_STATE_MACHINE_H
#define OHOS_ACELITE_JS_FRAGMENT_STATE_MACHINE_H

#include "component.h"
#include "non_copyable.h"
#include "js_app_context.h"



namespace OHOS{
namespace ACELite{

enum {
    FRAGMENT_UNDEFINED_STATE = -1,
    FRAGMENT_INIT_STATE, // 0
    FRAGMENT_READY_STATE,
    FRAGMENT_SHOW_STATE,
    FRAGMENT_HIDE_STATE,
    FRAGMENT_DESTROY_STATE,
    FRAGMENT_END_STATE
};
#define FRAGMENT_STATE_SIZE FRAGMENT_END_STATE

constexpr char FRAGMENT_LIFECYCLE_ON_INIT[] = "onInit";
constexpr char FRAGMENT_LIFECYCLE_CALLBACK_ON_READY[] = "onReady";
constexpr char FRAGMENT_LIFECYCLE_CALLBACK_ON_SHOW[] = "onShow";
constexpr char FRAGMENT_LIFECYCLE_CALLBACK_ON_HIDE[] = "onHide";
constexpr char FRAGMENT_LIFECYCLE_CALLBACK_ON_DESTROY[] = "onDestroy";

class FragmentState;
class FragmentStateMachine final : public MemoryHeap{
public:
    FragmentStateMachine();
    ~FragmentStateMachine();

    bool Init(const char *path);

    void ChangeState(int newState);

    int8_t GetCurrentState() const
    {
        return currentState_;
    }
    void SetCurrentState(int8_t state);

    void EvalFragment();
    void RenderFragment();
    void ShowFragment();
    void InvokeLifeCycleCallback(const char *name);

    Component *GetRootComponent()
    {
        return rootComponent_;
    }

private:
    int8_t currentState_;
    FragmentState *stateMap_[FRAGMENT_STATE_SIZE];
    JsAppContext *appContext_;
    jerry_value_t viewModel_;     // the object evaled from user JS code
    Component *rootComponent_;
    char *appRootPath_;
    char *jsFragemntPath_;
};

} // ACELite
} // OHOS

#endif // OHOS_ACELITE_JS_FRAGMENT_STATE_MACHINE_H
