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

#include "core/animation/svg_animate.h"

#include "base/log/log.h"
#include "base/utils/string_utils.h"
#include "frameworks/core/components/svg/svg_transform.h"

namespace OHOS::Ace {

bool SvgAnimate::GetValuesRange(std::vector<float>& from, std::vector<float>& to, std::string& type)
{
    if (to_.empty() || from_ == to_) {
        return false;
    }

    char tag = (from_.find(',') != std::string::npos) ? ',' : ' ';
    StringUtils::StringSpliter(from_, tag, from);
    tag = (to_.find(',') != std::string::npos) ? ',' : ' ';
    StringUtils::StringSpliter(to_, tag, to);
    if (to.empty()) {
        return false;
    }

    if (!SvgTransform::AlignmentValues(transformType_, from, to)) {
        return false;
    }

    type = transformType_;
    return true;
}

bool SvgAnimate::GetFrames(std::vector<std::vector<float>>& frames, std::string& type)
{
    type = transformType_;
    std::vector<float> frame;
    for (const auto& value : values_) {
        char tag = (value.find(',') != std::string::npos) ? ',' : ' ';
        StringUtils::StringSpliter(value, tag, frame);
        if (!SvgTransform::AlignmentFrame(type, frame)) {
            return false;
        }
        frames.push_back(frame);
    }

    return true;
}

template<typename T>
void SvgAnimate::CreateKeyframe(
    const RefPtr<KeyframeAnimation<T>>& animation, const T& value, float time, const RefPtr<Curve>& curve)
{
    if (!animation) {
        LOGE("create discrete calcMode animate failed, animation is null");
        return;
    }
    if (!curve) {
        LOGE("Create keyframe failed, curve is null");
        return;
    }
    auto keyframe = AceType::MakeRefPtr<Keyframe<T>>(time, value);
    keyframe->SetCurve(curve);
    animation->AddKeyframe(keyframe);
}

template<typename T>
void SvgAnimate::CreateFirstKeyframe(const RefPtr<KeyframeAnimation<T>>& animation, const T& value)
{
    if (!animation) {
        LOGE("create discrete calcMode animate failed, animation is null");
        return;
    }
    auto keyframe = AceType::MakeRefPtr<Keyframe<T>>(0.0f, value);
    animation->AddKeyframe(keyframe);
}

template<typename T>
bool SvgAnimate::CreatePropertyAnimate(
    std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator)
{
    bool ret = false;
    switch (calcMode_) {
        case CalcMode::DISCRETE:
            ret = CreateDiscreteAnimate(std::move(callback), originalValue, animator);
            break;
        case CalcMode::LINEAR:
            ret = CreateLinearAnimate(std::move(callback), originalValue, animator);
            break;
        case CalcMode::PACED:
            ret = CreatePacedAnimate(std::move(callback), originalValue, animator);
            break;
        case CalcMode::SPLINE:
            ret = CreateSplineAnimate(std::move(callback), originalValue, animator);
            break;
        default:
            LOGE("invalid calcMode");
            break;
    }
    return ret;
}

bool SvgAnimate::CreateMotionAnimate(std::function<void(double)>&& callback, const RefPtr<Animator>& animator)
{
    if (keyPoints_.empty() || calcMode_ == CalcMode::PACED) {
        auto animation = AceType::MakeRefPtr<CurveAnimation<double>>(0.0, 1.0, GetCurve());
        animation->AddListener(callback);
        animator->AddInterpolator(animation);
        animator->SetDuration(GetDur());
        animator->SetFillMode(GetFillMode());
        animator->SetIteration(repeatCount_);
        animator->SetStartDelay(GetBegin());
        animator->Play();
        return true;
    }
    bool ret = false;
    double originalValue = 0.0;
    switch (calcMode_) {
        case CalcMode::DISCRETE:
            ret = CreateDiscreteAnimate(std::move(callback), originalValue, animator);
            break;
        case CalcMode::LINEAR:
            ret = CreateLinearAnimate(std::move(callback), originalValue, animator);
            break;
        case CalcMode::SPLINE:
            ret = CreateSplineAnimate(std::move(callback), originalValue, animator);
            break;
        default:
            LOGE("invalid calcMode");
            break;
    }
    return ret;
}

template<typename T>
bool SvgAnimate::CreateDiscreteAnimate(
    std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator)
{
    if (calcMode_ != CalcMode::DISCRETE) {
        LOGW("invalid calcMode");
        return false;
    }
    RefPtr<KeyframeAnimation<T>> animation = AceType::MakeRefPtr<KeyframeAnimation<T>>();
    const auto& values = GetKeyValues();
    if (values.empty()) {
        T startValue = GetStartValue(originalValue);
        T endValue = GetEndValue(startValue);
        if (!DiscreteAnimate(animation, originalValue, startValue, endValue)) {
            LOGW("discreteAnimate, create discrete animate failed");
            return false;
        }
    } else {
        if (!DiscreteAnimate(animation, originalValue)) {
            LOGW("discreteAnimate with value, create discrete animate failed");
            return false;
        }
    }
    animation->SetInitValue(originalValue);
    animation->AddListener(std::move(callback));
    animator->AddInterpolator(animation);
    animator->SetDuration(GetDur());
    animator->SetFillMode(GetFillMode());
    animator->SetIteration(repeatCount_);
    animator->SetStartDelay(GetBegin());
    animator->Play();
    return true;
}

template<typename T>
bool SvgAnimate::DiscreteAnimate(
    const RefPtr<KeyframeAnimation<T>>& animation, const T& originalValue, const T& startValue, const T& endValue)
{
    if (startValue == endValue && startValue == originalValue) {
        LOGW("the start value and end value are the same as the original value");
        return false;
    }
    CreateFirstKeyframe(animation, originalValue);
    CreateKeyframe(animation, startValue, 0.5f, GetCurve());
    CreateKeyframe(animation, endValue, 1.0f, GetCurve());
    return true;
}

template<typename T>
bool SvgAnimate::DiscreteAnimate(const RefPtr<KeyframeAnimation<T>>& animation, const T& originalValue)
{
    const auto& values = GetKeyValues();
    if (values.empty()) {
        LOGW("create discrete calcMode animate failed, values is null");
        return false;
    }
    if (values.size() == 1) {
        CreateFirstKeyframe(animation, originalValue);
        CreateKeyframe(animation, GetValue<T>(values.front()), 1.0f, GetCurve());
        return true;
    } else if (values.size() == 2) {
        return DiscreteAnimate(animation, originalValue, GetValue<T>(values.front()), GetValue<T>(values.back()));
    } else {
        if (!keyTimes_.empty()) {
            return DiscreteWithValues(animation, originalValue);
        } else {
            return DiscreteWithKeyTimes(animation, originalValue);
        }
    }
}

template<typename T>
bool SvgAnimate::DiscreteWithValues(const RefPtr<KeyframeAnimation<T>>& animation, const T& originalValue)
{
    if (!animation) {
        LOGE("create discrete calcMode animate failed, animation is null");
        return false;
    }
    const auto& values = GetKeyValues();
    if (values.size() < 3) {
        LOGW("create discrete calcMode animate failed, values is null");
        return false;
    }
    // In discrete calcMode, if without keyTimes, the first value is the original value
    // For example: values = {2, 3, 4} and originalValue=1, the keyframe values will be {1, 2, 3, 4}
    float keyTime = ((int)(100.0f / (values.size()))) / 100.0f;
    CreateFirstKeyframe(animation, originalValue);
    auto valueIter = values.begin();
    while (valueIter != (values.end() - 1)) {
        CreateKeyframe(animation, GetValue<T>(*valueIter), keyTime, GetCurve());
        keyTime += keyTime;
        ++valueIter;
    }
    CreateKeyframe(animation, GetValue<T>(values.back()), 1.0f, GetCurve());
    return true;
}

template<typename T>
bool SvgAnimate::DiscreteWithKeyTimes(const RefPtr<KeyframeAnimation<T>>& animation, const T& originalValue)
{
    if (!animation) {
        LOGE("create discrete calcMode animate failed, animation is null");
        return false;
    }
    const auto& values = GetKeyValues();
    if (values.size() < 3 || keyTimes_.size() != values.size()) {
        LOGW("create discrete calcMode animate failed, values or keyTimes invalid");
        return false;
    }
    // In discrete calcMode, if has keyTimes, the first value is the original value and the last value of
    // values is invalid. For example: values = {2, 3, 4} and originalValue=1, the keyframe values will be {1, 2, 3}
    CreateFirstKeyframe(animation, originalValue);
    auto valueIter = values.begin();
    auto keyTimeIter = keyTimes_.begin() + 1;
    while (valueIter != (values.end() - 2)) {
        CreateKeyframe(animation, GetValue<T>(*valueIter), *keyTimeIter, GetCurve());
        ++valueIter;
        ++keyTimeIter;
    }
    CreateKeyframe(animation, GetValue<T>(*(values.end() - 2)), 1.0f, GetCurve());
    return true;
}

template<typename T>
bool SvgAnimate::CreateLinearAnimate(
    std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator)
{
    if (calcMode_ != CalcMode::LINEAR) {
        LOGW("invalid calcMode");
        return false;
    }
    const auto& values = GetKeyValues();
    if (!values.empty()) {
        RefPtr<KeyframeAnimation<T>> animation = AceType::MakeRefPtr<KeyframeAnimation<T>>();
        if (!LinearAnimate(animation)) {
            LOGW("create linear animate failed");
            return false;
        }
        animation->SetInitValue(originalValue);
        animation->AddListener(std::move(callback));
        animator->AddInterpolator(animation);
    } else {
        if (!LinearAnimate(std::move(callback), originalValue, animator)) {
            LOGW("create linear animate failed");
            return false;
        }
    }
    animator->SetDuration(GetDur());
    animator->SetFillMode(GetFillMode());
    animator->SetIteration(repeatCount_);
    animator->SetStartDelay(GetBegin());
    animator->Play();
    return true;
}

template<typename T>
bool SvgAnimate::LinearAnimate(
    std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator)
{
    if (!animator) {
        LOGE("create linear calcMode animate failed, animation is null");
        return false;
    }
    T startValue = GetStartValue(originalValue);
    T endValue = GetEndValue(startValue);
    if (startValue == endValue) {
        if (startValue == originalValue) {
            LOGW("the start value and end value are the same as the original value, startValue=%{public}s",
                from_.c_str());
            return false;
        }
        startValue = originalValue;
    }
    auto animation = AceType::MakeRefPtr<CurveAnimation<T>>(startValue, endValue, GetCurve());
    animation->SetInitValue(originalValue);
    animation->AddListener(std::move(callback));
    animator->AddInterpolator(animation);
    return true;
}

template<typename T>
bool SvgAnimate::LinearAnimate(const RefPtr<KeyframeAnimation<T>>& animation)
{
    const auto& values = GetKeyValues();
    if (values.empty()) {
        LOGW("create linear calcMode animate failed, values is invalid");
        return false;
    }
    if (values.size() <= 2) {
        CreateFirstKeyframe(animation, GetValue<T>(values.front()));
        CreateKeyframe(animation, GetValue<T>(values.back()), 1.0f, GetCurve());
        return true;
    }
    if (!keyTimes_.empty()) {
        return LinearWithKeyTimes(animation);
    } else {
        return LinearWithValues(animation);
    }
}

template<typename T>
bool SvgAnimate::LinearWithValues(const RefPtr<KeyframeAnimation<T>>& animation)
{
    if (!animation) {
        LOGE("create linear calcMode animate failed, animation is null");
        return false;
    }
    const auto& values = GetKeyValues();
    if (values.size() < 3) {
        LOGW("create linear calcMode animate failed, values is invalid");
        return false;
    }
    CreateFirstKeyframe(animation, GetValue<T>(values.front()));
    float keyTime = ((int)(100.0f / (values.size() - 1))) / 100.0f;
    auto valueIter = values.begin() + 1;
    while (valueIter != (values.end() - 1)) {
        CreateKeyframe(animation, GetValue<T>(*valueIter), keyTime, GetCurve());
        keyTime += keyTime;
        ++valueIter;
    }
    CreateKeyframe(animation, GetValue<T>(values.back()), 1.0f, GetCurve());
    return true;
}

template<typename T>
bool SvgAnimate::LinearWithKeyTimes(const RefPtr<KeyframeAnimation<T>>& animation)
{
    if (!animation) {
        LOGE("create linear calcMode animate failed, animation is null");
        return false;
    }
    const auto& values = GetKeyValues();
    if (values.size() < 3 || keyTimes_.size() != values.size()) {
        LOGW("create linear calcMode animate failed, values is invalid");
        return false;
    }
    CreateFirstKeyframe(animation, GetValue<T>(values.front()));
    auto valueIter = values.begin() + 1;
    auto keyTimeIter = keyTimes_.begin() + 1;
    while (valueIter != (values.end() - 1)) {
        CreateKeyframe(animation, GetValue<T>(*valueIter), *keyTimeIter, GetCurve());
        ++valueIter;
        ++keyTimeIter;
    }
    CreateKeyframe(animation, GetValue<T>(values.back()), 1.0f, GetCurve());
    return true;
}

template<typename T>
bool SvgAnimate::CreatePacedAnimate(
    std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator)
{
    if (calcMode_ != CalcMode::PACED) {
        LOGW("invalid calcMode");
        return false;
    }
    const auto& values = GetKeyValues();
    if (!values.empty()) {
        RefPtr<KeyframeAnimation<T>> animation = AceType::MakeRefPtr<KeyframeAnimation<T>>();
        if (values.size() <= 2) {
            CreateFirstKeyframe(animation, GetValue<T>(values.front()));
            CreateKeyframe(animation, GetValue<T>(values.back()), 1.0f, GetCurve());
        } else {
            if (!LinearWithValues(animation)) {
                LOGW("linearWithValues, create paced animate failed");
                return false;
            }
        }
        animation->SetInitValue(originalValue);
        animation->AddListener(std::move(callback));
        animator->AddInterpolator(animation);
    } else {
        if (!LinearAnimate(std::move(callback), originalValue, animator)) {
            LOGW("create linear animate failed");
            return false;
        }
    }
    animator->SetDuration(GetDur());
    animator->SetFillMode(GetFillMode());
    animator->SetIteration(repeatCount_);
    animator->SetStartDelay(GetBegin());
    animator->Play();
    return true;
}

template<typename T>
bool SvgAnimate::CreateSplineAnimate(
    std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator)
{
    if (calcMode_ != CalcMode::SPLINE) {
        LOGW("invalid calcMode");
        return false;
    }
    const auto& values = GetKeyValues();
    if (values.empty()) {
        if (!SplineAnimate(std::move(callback), originalValue, animator)) {
            LOGW("create spline animate failed");
            return false;
        }
    } else {
        RefPtr<KeyframeAnimation<T>> animation = AceType::MakeRefPtr<KeyframeAnimation<T>>();
        if (!SplineAnimate(animation)) {
            LOGW("create spline animate failed");
            return false;
        }
        animation->SetInitValue(originalValue);
        animation->AddListener(std::move(callback));
        animator->AddInterpolator(animation);
    }
    animator->SetDuration(GetDur());
    animator->SetFillMode(GetFillMode());
    animator->SetIteration(repeatCount_);
    animator->SetStartDelay(GetBegin());
    animator->Play();
    return true;
}

template<typename T>
bool SvgAnimate::SplineAnimate(
    std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator)
{
    if (!animator) {
        LOGE("create linear calcMode animate failed, animation is null");
        return false;
    }
    if (keySplines_.empty()) {
        LOGW("create linear calcMode animate failed, keySplines is invalid");
        return false;
    }
    T startValue = GetStartValue(originalValue);
    T endValue = GetEndValue(startValue);
    if (startValue == endValue) {
        if (startValue == originalValue) {
            return false;
        }
        startValue = originalValue;
    }
    auto animation = AceType::MakeRefPtr<CurveAnimation<T>>(startValue, endValue, GetCurve(keySplines_[0]));
    animation->SetInitValue(originalValue);
    animation->AddListener(std::move(callback));
    animator->AddInterpolator(animation);
    return true;
}

template<typename T>
bool SvgAnimate::SplineAnimate(const RefPtr<KeyframeAnimation<T>>& animation)
{
    const auto& values = GetKeyValues();
    if (values.size() < 2) {
        LOGW("animation invalid, values is invalid");
        return false;
    }
    if (values.size() == 2) {
        if (keySplines_.size() != 1) {
            LOGW("animation invalid, the curve is not defined.");
            return false;
        }
        CreateFirstKeyframe(animation, GetValue<T>(values.front()));
        CreateKeyframe(animation, GetValue<T>(values.back()), 1.0f, CubicCurveCreator(keySplines_.front()));
        return true;
    }
    if (keyTimes_.empty()) {
        return SplineWithKeySplines(animation);
    } else {
        return SplineWithKeyTimes(animation);
    }
}

template<typename T>
bool SvgAnimate::SplineWithKeySplines(const RefPtr<KeyframeAnimation<T>>& animation)
{
    if (!animation) {
        LOGE("create spline calcMode animate failed, animation is null");
        return false;
    }
    const auto& values = GetKeyValues();
    if (values.size() < 3 || keySplines_.size() != (values.size() - 1)) {
        LOGW("create spline calcMode animate failed, keySplines_ is invalid");
        return false;
    }
    float keyTime = ((int)(100.0f / values.size() - 1)) / 100.0f;
    CreateFirstKeyframe(animation, GetValue<T>(values.front()));
    auto valueIter = values.begin() + 1;
    auto keySplineIter = keySplines_.begin();
    while (valueIter != (values.end() - 1)) {
        CreateKeyframe(animation, GetValue<T>(*valueIter), keyTime, CubicCurveCreator(*keySplineIter));
        ++valueIter;
        ++keySplineIter;
        keyTime += keyTime;
    }
    CreateKeyframe(animation, GetValue<T>(values.back()), 1.0f, CubicCurveCreator(keySplines_.back()));
    return true;
}

template<typename T>
bool SvgAnimate::SplineWithKeyTimes(const RefPtr<KeyframeAnimation<T>>& animation)
{
    if (!animation) {
        LOGE("create spline calcMode animate failed, animation is null");
        return false;
    }
    const auto& values = GetKeyValues();
    if (values.size() < 3 || keySplines_.size() != (values.size() - 1) || values.size() != keyTimes_.size()) {
        LOGW("create spline calcMode animate failed");
        return false;
    }
    CreateFirstKeyframe(animation, GetValue<T>(values.front()));
    auto valueIter = values.begin() + 1;
    auto keySplineIter = keySplines_.begin() + 1;
    auto keyTimeIter = keyTimes_.begin() + 1;
    while (valueIter != (values.end() - 1)) {
        CreateKeyframe(animation, GetValue<T>(*valueIter), *keyTimeIter, CubicCurveCreator(*keySplineIter));
        ++valueIter;
        ++keySplineIter;
        ++keyTimeIter;
    }
    CreateKeyframe(animation, GetValue<T>(values.back()), 1.0f, CubicCurveCreator(keySplines_.back()));
    return true;
}

template bool SvgAnimate::CreatePropertyAnimate(
    std::function<void(double)>&& callback, const double& originalValue, const RefPtr<Animator>& animator);
template bool SvgAnimate::CreatePropertyAnimate(
    std::function<void(Dimension)>&& callback, const Dimension& originalValue, const RefPtr<Animator>& animator);
template bool SvgAnimate::CreatePropertyAnimate(
    std::function<void(Color)>&& callback, const Color& originalValue, const RefPtr<Animator>& animator);

} // namespace OHOS::Ace