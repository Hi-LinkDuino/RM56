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

#include "core/animation/curves.h"

namespace OHOS::Ace {

const RefPtr<DecelerationCurve> Curves::DECELE = AceType::MakeRefPtr<DecelerationCurve>();
const RefPtr<LinearCurve> Curves::LINEAR = AceType::MakeRefPtr<LinearCurve>();
const RefPtr<SineCurve> Curves::SINE = AceType::MakeRefPtr<SineCurve>();
const RefPtr<CubicCurve> Curves::EASE = AceType::MakeRefPtr<CubicCurve>(0.25f, 0.1f, 0.25f, 1.0f);
const RefPtr<CubicCurve> Curves::EASE_IN = AceType::MakeRefPtr<CubicCurve>(0.42f, 0.0f, 1.0f, 1.0f);
const RefPtr<CubicCurve> Curves::EASE_OUT = AceType::MakeRefPtr<CubicCurve>(0.0f, 0.0f, 0.58f, 1.0f);
const RefPtr<CubicCurve> Curves::EASE_IN_OUT = AceType::MakeRefPtr<CubicCurve>(0.42f, 0.0f, 0.58f, 1.0f);
const RefPtr<CubicCurve> Curves::FAST_OUT_SLOW_IN = AceType::MakeRefPtr<CubicCurve>(0.4f, 0.0f, 0.2f, 1.0f);
const RefPtr<CubicCurve> Curves::LINEAR_OUT_SLOW_IN = AceType::MakeRefPtr<CubicCurve>(0.0f, 0.0f, 0.2f, 1.0f);
const RefPtr<CubicCurve> Curves::FAST_OUT_LINEAR_IN = AceType::MakeRefPtr<CubicCurve>(0.4f, 0.0f, 1.0f, 1.0f);
const RefPtr<CubicCurve> Curves::FRICTION = AceType::MakeRefPtr<CubicCurve>(0.2f, 0.0f, 0.2f, 1.0f);
const RefPtr<CubicCurve> Curves::EXTREME_DECELERATION = AceType::MakeRefPtr<CubicCurve>(0.0f, 0.0f, 0.0f, 1.0f);
const RefPtr<CubicCurve> Curves::SHARP = AceType::MakeRefPtr<CubicCurve>(0.33f, 0.0f, 0.67f, 1.0f);
const RefPtr<CubicCurve> Curves::RHYTHM = AceType::MakeRefPtr<CubicCurve>(0.7f, 0.0f, 0.2f, 1.0f);
const RefPtr<CubicCurve> Curves::SMOOTH = AceType::MakeRefPtr<CubicCurve>(0.4f, 0.0f, 0.4f, 1.0f);
const RefPtr<AnticipateCurve> Curves::ANTICIPATE = AceType::MakeRefPtr<AnticipateCurve>(2.0f);
const RefPtr<CubicCurve> Curves::MAGNETIC = AceType::MakeRefPtr<CubicCurve>(0.8f, 0.0f, 1.0f, 0.6f);
const RefPtr<ElasticsCurve> Curves::ELASTICS = AceType::MakeRefPtr<ElasticsCurve>(2.0f);

} // namespace OHOS::Ace
