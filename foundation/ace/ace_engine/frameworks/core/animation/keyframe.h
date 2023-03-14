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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_KEYFRAME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_KEYFRAME_H

#include "base/memory/ace_type.h"
#include "core/animation/curve.h"

namespace OHOS::Ace {

template<typename T>
class Keyframe : public AceType {
public:
    // keyTime is normalized time.
    Keyframe(float keyTime, const T& keyValue) : keyTime_(keyTime), keyValue_(keyValue) {}

    ~Keyframe() override = default;

    bool IsValid() const
    {
        if (keyTime_ < NORMALIZED_DURATION_MIN || keyTime_ > NORMALIZED_DURATION_MAX) {
            LOGE("invalid normalized key time: %{public}f", keyTime_);
            return false;
        }
        return true;
    }

    float GetKeyTime() const
    {
        return keyTime_;
    }

    void SetKeyValue(const T& keyValue)
    {
        keyValue_ = keyValue;
    }

    const T& GetKeyValue() const
    {
        return keyValue_;
    }

    void SetCurve(const RefPtr<Curve>& curve)
    {
        if (!curve) {
            LOGE("set curve failed, input curve is null.");
            return;
        }
        curve_ = curve;
    }

    const RefPtr<Curve>& GetCurve() const
    {
        return curve_;
    }

private:
    const float keyTime_ { 0.0f };
    T keyValue_;
    RefPtr<Curve> curve_ { Curves::EASE };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_KEYFRAME_H
