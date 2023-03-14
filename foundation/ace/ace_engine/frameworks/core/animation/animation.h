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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATION_H

#include "base/utils/listener.h"
#include "core/animation/curve.h"
#include "core/animation/interpolator.h"

namespace OHOS::Ace {

template<typename T>
class Animation : public Interpolator, public ValueListenable<T> {
    DECLARE_ACE_TYPE(Animation, Interpolator);

public:
    virtual const T& GetValue() const = 0;

    void OnInitNotify(float normalizedTime, bool reverse) override
    {
        // If the user sets the initial value, the animation stay at the initial value.
        if (inited_) {
            ValueListenable<T>::NotifyListener(initValue_);
        } else {
            OnNormalizedTimestampChanged(normalizedTime, reverse);
        }
    }

    void SetInitValue(const T& initValue)
    {
        initValue_ = initValue;
        inited_ = true;
    }

    bool HasInitValue() const
    {
        return inited_;
    }

    virtual void SetCurve(const RefPtr<Curve>& curve) {}

private:
    T initValue_ {};
    bool inited_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATION_H
