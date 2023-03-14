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

#include "core/components/overlay/overlay_element.h"

namespace OHOS::Ace {
void OverlayElement::Clear()
{
    LOGD("Clear Overlay. current child size: %zu", children_.size());
    while (children_.size() > 0) {
        PopInstant();
    }
}

} // namespace OHOS::Ace
