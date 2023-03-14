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

#include "core/animation/anticipate_curve.h"

namespace OHOS::Ace {

AnticipateCurve::AnticipateCurve(const float tension) : tension_(tension)
{}

float AnticipateCurve::MoveInternal(float time)
{
    // a(t) = t * t * ((tension + 1) * t - tension)
    return time * time * ((tension_ + 1.0f) * time - tension_);
}

} // namespace OHOS::Ace