/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "core/components/refresh/refresh_element.h"

#include "core/components/refresh/refresh_component.h"

namespace OHOS::Ace {

void RefreshElement::PerformBuild()
{
    LOGD("RefreshElement::PerformBuild");
    RefPtr<RefreshComponent> component = AceType::DynamicCast<RefreshComponent>(component_);
    if (!component) {
        LOGE("Component is null");
        return;
    }
    SoleChildElement::PerformBuild();
}

} // namespace OHOS::Ace
