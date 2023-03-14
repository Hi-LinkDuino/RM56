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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_EXTENSIONS_EVENTS_EVENT_EXTENSIONS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_EXTENSIONS_EVENTS_EVENT_EXTENSIONS_H

#include "core/components_v2/extensions/events/on_area_change_extension.h"
#include "core/components_v2/extensions/extension.h"

namespace OHOS::Ace::V2 {

class EventExtensions : public Extension {
    DECLARE_ACE_TYPE(EventExtensions, Extension);

public:
    EventExtensions() = default;
    ~EventExtensions() override = default;

    const RefPtr<OnAreaChangeExtension>& GetOnAreaChangeExtension()
    {
        if (!onAreaChangeExtension_) {
            onAreaChangeExtension_ = MakeRefPtr<OnAreaChangeExtension>();
        }
        return onAreaChangeExtension_;
    }

    bool HasOnAreaChangeExtension() const
    {
        return onAreaChangeExtension_;
    }

private:
    RefPtr<OnAreaChangeExtension> onAreaChangeExtension_;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_EXTENSIONS_EVENTS_EVENT_EXTENSIONS_H