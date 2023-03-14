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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_MOTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_MOTION_H

#include "base/utils/listener.h"
#include "core/animation/time_event.h"

namespace OHOS::Ace {

class Motion : public TimeEvent, public ValueListenable<double> {
    DECLARE_ACE_TYPE(Motion, TimeEvent);

public:
    virtual double GetCurrentPosition() = 0;
    virtual double GetCurrentVelocity() = 0;
    virtual bool IsCompleted() = 0;

    // Each subclass should override this method to perform motion in each timestamp.
    virtual void Move(float offsetTime) = 0;

    // Motion do not have duration, normalizedTime always zero
    void OnTimestampChanged(float timestamp, float normalizedTime, bool reverse) final
    {
        Move(timestamp);
        NotifyListener(GetCurrentPosition());
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_MOTION_H
