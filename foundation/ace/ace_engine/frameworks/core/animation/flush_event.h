/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_FLUSH_EVENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_FLUSH_EVENT_H

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

class FlushEvent : public virtual AceType {
    DECLARE_ACE_TYPE(FlushEvent, AceType);

public:
    // Will be called before flush every vsync.
    virtual void OnPreFlush() {}

    // Will be called after flush build in every vsync.
    virtual void OnPostAnimationFlush() {}

    // Will be called after flush every vsync.
    virtual void OnPostFlush() = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_FLUSH_EVENT_H
