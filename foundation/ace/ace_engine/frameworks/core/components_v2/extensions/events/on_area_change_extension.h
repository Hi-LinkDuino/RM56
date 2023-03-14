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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_EXTENSIONS_EVENTS_ON_AREA_CHANGE_EXTENSION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_EXTENSIONS_EVENTS_ON_AREA_CHANGE_EXTENSION_H

#include <functional>
#include <list>

#include "base/geometry/offset.h"
#include "base/geometry/rect.h"
#include "core/components_v2/extensions/extension.h"

namespace OHOS::Ace::V2 {

using OnAreaChangeEvent = std::function<void(const Rect&, const Offset&, const Rect&, const Offset&)>;

class OnAreaChangeExtension : public Extension {
    DECLARE_ACE_TYPE(OnAreaChangeExtension, Extension);

public:
    OnAreaChangeExtension() = default;
    ~OnAreaChangeExtension() override = default;

    void AddOnAreaChangeEvent(const OnAreaChangeEvent& event)
    {
        if (!event) {
            return;
        }
        callbacks_.emplace_back(event);
    }
    void AddOnAreaChangeEvent(OnAreaChangeEvent&& event)
    {
        if (!event) {
            return;
        }
        callbacks_.emplace_back(std::move(event));
    }

    void SetBase(const Rect& rect, const Offset& origin)
    {
        rect_ = rect;
        origin_ = origin;
    }

    void UpdateArea(const Rect& rect, const Offset& origin)
    {
        if ((rect_ == rect) && (origin_ == origin)) {
            return;
        }
        for (const auto& callback : callbacks_) {
            callback(rect_, origin_, rect, origin);
        }
        rect_ = rect;
        origin_ = origin;
    }

private:
    std::list<OnAreaChangeEvent> callbacks_;
    Rect rect_;
    Offset origin_;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_EXTENSIONS_EVENTS_ON