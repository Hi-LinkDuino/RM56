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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_TRANSFORM_CONVERTOR_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_TRANSFORM_CONVERTOR_H

#include <unordered_map>
#include <list>

#include "base/geometry/transform_util.h"
#include "core/animation/animation_pub.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/common/properties/tween_option.h"

namespace OHOS::Ace {

class TransformConvertor {
public:
    static const std::vector<std::string> TransformKey;

    template<class T>
    using TransformUnorderedMap = std::unordered_map<AnimationType, RefPtr<KeyframeAnimation<T>>>;

    template<class T>
    using TransformList = std::list<RefPtr<KeyframeAnimation<T>>>;

    TransformConvertor() = default;
    ~TransformConvertor() = default;
    void Convert(const std::string& key, const std::string& value, double time);
    void InsertIdentityKeyframe(double time);
    void AddAnimationToTweenOption(TweenOption& option) const;
    void ApplyCurve(const RefPtr<Curve>& curve);
    void ClearAnimations();

private:
    void AddKeyframe(AnimationType type, const RefPtr<Keyframe<TransformOperation>>& keyframe);
    void AddKeyframe(AnimationType type, double time, const TranslateOperation& translate);
    void AddKeyframe(AnimationType type, double time, const RotateOperation& rotate);
    void AddKeyframe(AnimationType type, double time, const SkewOperation& skew);
    void AddKeyframe(AnimationType type, double time, const ScaleOperation& scale);
    void AddKeyframe(AnimationType type, double time, const Matrix4& matrix);
    void AddKeyframe(AnimationType type, double time, const PerspectiveOperation& distance);

private:
    TransformUnorderedMap<TransformOperation> operationMap_;
    TransformList<TransformOperation> operationList_;
    std::list<float> noneKeyframeTimes_;

    static const std::unordered_map<std::string, void (*)(const std::string&, const double&, TransformConvertor&)>
        TransformConvertorMap;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_TRANSFORM_CONVERTOR_H
