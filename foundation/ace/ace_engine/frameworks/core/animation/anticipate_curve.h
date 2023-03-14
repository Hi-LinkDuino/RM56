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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANTICIPATE_CURVE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANTICIPATE_CURVE_H

#include "core/animation/curve.h"

namespace OHOS::Ace {

class AnticipateCurve : public Curve {
    DECLARE_ACE_TYPE(AnticipateCurve, Curve);

public:
    explicit AnticipateCurve(float tension);
    ~AnticipateCurve() override = default;

    float MoveInternal(float time) override;

private:
    // Default amount of anticipation. When tension equals 0.0f, there is no anticipation and the interpolator
    // becomes a simple acceleration interpolator.
    const float tension_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANTICIPATE_CURVE_H
