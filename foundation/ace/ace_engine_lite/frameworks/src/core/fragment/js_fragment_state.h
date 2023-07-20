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

#ifndef OHOS_ACELITE_JS_FRAGMENT_STATE_H
#define OHOS_ACELITE_JS_FRAGMENT_STATE_H

#include "js_fragment_state_machine.h"

namespace OHOS{
namespace ACELite{
class FragmentState: public MemoryHeap{
public:
    FragmentState() {}
    virtual ~FragmentState() {}
    virtual void Handle(FragmentStateMachine& fragmentSM) = 0;
};

class FragmentInitState final: public FragmentState{
    void Handle(FragmentStateMachine& fragmentSM) override;
};

class FragmentReadyState final: public FragmentState{
    void Handle(FragmentStateMachine& fragmentSM) override;
};

class FragmentShowState final: public FragmentState{
    void Handle(FragmentStateMachine& fragmentSM) override;
};

class FragmentHideState final: public FragmentState{
    void Handle(FragmentStateMachine& fragmentSM) override;
};

class FragmentDestroyState final: public FragmentState{
    void Handle(FragmentStateMachine& fragmentSM) override;
};

} // namespace ACELite
} // OHOS

#endif //OHOS_ACELITE_JS_FRAGMENT_STATE_H