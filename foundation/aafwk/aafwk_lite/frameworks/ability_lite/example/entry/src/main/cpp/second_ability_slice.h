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

#ifndef OHOS_SECOND_ABILITY_SLICE_H
#define OHOS_SECOND_ABILITY_SLICE_H

#include "ability_loader.h"
#include "event_listener.h"

namespace OHOS {
class SecondAbilitySlice : public AbilitySlice {
protected:
    void OnStart(const Want &want) override;
    void OnStop() override;

private:
    RootView *rootView_ { nullptr };
};
}

#endif // OHOS_SECOND_ABILITY_SLICE_H
