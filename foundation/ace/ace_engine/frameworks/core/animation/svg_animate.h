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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SVG_ANIMATE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SVG_ANIMATE_H

#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "frameworks/core/animation/animator.h"
#include "frameworks/core/animation/curve_animation.h"
#include "frameworks/core/animation/keyframe_animation.h"

namespace OHOS::Ace {

enum class Fill {
    FREEZE,
    REMOVE,
};

enum class CalcMode {
    LINEAR,
    PACED,
    DISCRETE,
    SPLINE,
};

enum class SvgAnimateType {
    ANIMATE,
    MOTION,
    TRANSFORM,
};

class SvgAnimate : public AceType {
    DECLARE_ACE_TYPE(SvgAnimate, AceType);

public:
    SvgAnimate() = default;
    ~SvgAnimate() override = default;

    template<typename T>
    bool CreatePropertyAnimate(
        std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator);
    bool CreateMotionAnimate(std::function<void(double)>&& callback, const RefPtr<Animator>& animator);

    RefPtr<Curve> GetCurve(const std::string& param = "") const
    {
        if (calcMode_ == CalcMode::DISCRETE) {
            return AceType::MakeRefPtr<StepsCurve>(1, StepsCurvePosition::START);
        } else if (calcMode_ == CalcMode::PACED) {
            return Curves::LINEAR;
        } else if (calcMode_ == CalcMode::SPLINE) {
            return CubicCurveCreator(param);
        } else {
            return Curves::LINEAR;
        }
    }

    FillMode GetFillMode() const
    {
        if (fillMode_ == Fill::FREEZE) {
            return FillMode::FORWARDS;
        } else {
            return FillMode::NONE;
        }
    }

    RefPtr<Curve> CubicCurveCreator(const std::string& paramStr) const
    {
        std::vector<std::string> params;
        StringUtils::SplitStr(paramStr, " ", params);
        if (params.size() != 4) {
            LOGE("cubic curve accept only 4 parameters");
            return Curves::LINEAR;
        }
        double x1 = StringUtils::StringToDouble(params.at(0));
        double y1 = StringUtils::StringToDouble(params.at(1));
        double x2 = StringUtils::StringToDouble(params.at(2));
        double y2 = StringUtils::StringToDouble(params.at(3));
        return AceType::MakeRefPtr<CubicCurve>(x1, y1, x2, y2);
    }

    template<typename T>
    T GetStartValue(const T& originalValue) const
    {
        if (svgAnimateType_ == SvgAnimateType::TRANSFORM) {
            return 0.0;
        }

        if (!from_.empty()) {
            return StringUtils::StringToDouble(from_);
        } else {
            return originalValue;
        }
    }

    template<>
    Color GetStartValue(const Color& originalValue) const
    {
        if (!from_.empty()) {
            return Color::FromString(from_);
        } else {
            return originalValue;
        }
    }

    template<>
    Dimension GetStartValue(const Dimension& originalValue) const
    {
        if (!from_.empty()) {
            return StringUtils::StringToDimension(from_);
        } else {
            return originalValue;
        }
    }

    template<typename T>
    T GetEndValue(const T& startValue) const
    {
        if (svgAnimateType_ == SvgAnimateType::TRANSFORM) {
            return 1.0;
        }

        if (!to_.empty()) {
            return StringUtils::StringToDouble(to_);
        } else if (!by_.empty()) {
            return startValue + StringUtils::StringToDouble(by_);
        } else {
            return startValue;
        }
    }

    template<>
    Color GetEndValue(const Color& startValue) const
    {
        if (!to_.empty()) {
            return Color::FromString(to_);
        } else if (!by_.empty()) {
            return startValue + Color::FromString(by_);
        } else {
            return startValue;
        }
    }

    template<>
    Dimension GetEndValue(const Dimension& startValue) const
    {
        if (!to_.empty()) {
            return StringUtils::StringToDimension(to_);
        } else if (!by_.empty()) {
            return startValue + StringUtils::StringToDimension(by_);
        } else {
            return startValue;
        }
    }

    template<typename T>
    T GetValue(const std::string& value) const
    {
        return StringUtils::StringToDouble(value);
    }

    template<>
    Dimension GetValue(const std::string& value) const
    {
        return StringUtils::StringToDimension(value);
    }

    template<>
    Color GetValue(const std::string& value) const
    {
        return Color::FromString(value);
    }

    void SetAttributeName(const std::string& attributeName)
    {
        attributeName_ = attributeName;
    }

    void SetBegin(int32_t begin)
    {
        begin_ = begin;
    }

    void SetDur(int32_t dur)
    {
        dur_ = dur;
    }

    void SetEnd(int32_t end)
    {
        end_ = end;
    }

    void SetRepeatCount(int32_t repeatCount)
    {
        repeatCount_ = repeatCount;
    }

    void SetFillMode(Fill fillMode)
    {
        fillMode_ = fillMode;
    }

    void SetCalcMode(CalcMode calcMode)
    {
        calcMode_ = calcMode;
    }

    void SetValues(const std::vector<std::string>& values)
    {
        if (!values.empty()) {
            values_.clear();
            values_.assign(values.begin(), values.end());
        }
    }

    void SetKeyTimes(const std::vector<double>& keyTimes)
    {
        if (!keyTimes.empty()) {
            keyTimes_.clear();
            keyTimes_.assign(keyTimes.begin(), keyTimes.end());
        }
    }

    void SetKeySplines(const std::vector<std::string>& keySplines)
    {
        if (!keySplines.empty()) {
            keySplines_.clear();
            keySplines_.assign(keySplines.begin(), keySplines.end());
        }
    }

    void SetFrom(const std::string& from)
    {
        from_ = from;
    }

    void SetTo(const std::string& to)
    {
        to_ = to;
    }

    void SetBy(const std::string& by)
    {
        by_ = by;
    }

    int32_t GetBegin() const
    {
        return std::max(begin_, 0);
    }

    int32_t GetDur() const
    {
        if (dur_ > 0) {
            return dur_;
        }
        return std::max((GetEnd() - GetBegin()), 0);
    }

    int32_t GetEnd() const
    {
        return std::max(end_, 0);
    }

    const std::string& GetFrom() const
    {
        return from_;
    }

    const std::string& GetTo() const
    {
        return to_;
    }

    const std::string& GetBy() const
    {
        return by_;
    }

    const std::string GetAttributeName() const
    {
        if (svgAnimateType_ == SvgAnimateType::TRANSFORM) {
            return attributeName_ + "_" + transformType_;
        }
        return attributeName_;
    }

    const std::vector<std::string>& GetValues() const
    {
        return values_;
    }

    const std::vector<double>& GetKeyTimes() const
    {
        return keyTimes_;
    }

    const std::vector<std::string>& GetKeySplines() const
    {
        return keySplines_;
    }

    int32_t GetRepeatCount() const
    {
        return repeatCount_;
    }

    CalcMode GetCalcMode() const
    {
        return calcMode_;
    }

    void SetKeyPoints(const std::vector<std::string>& keyPoints)
    {
        if (!keyPoints.empty()) {
            keyPoints_.clear();
            keyPoints_.assign(keyPoints.begin(), keyPoints.end());
        }
    }

    void SetPath(const std::string& path)
    {
        path_ = path;
    }

    void SetRotate(const std::string& rotate)
    {
        rotate_ = rotate;
    }

    const std::vector<std::string>& GetKeyPoints() const
    {
        return keyPoints_;
    }

    const std::string& GetPath() const
    {
        return path_;
    }

    const std::string& GetRotate() const
    {
        return rotate_;
    }

    void SetTransformType(const std::string& transformType)
    {
        transformType_ = transformType;
    }

    const std::string& GetTransformType() const
    {
        return transformType_;
    }

    SvgAnimateType GetSvgAnimateType() const
    {
        return svgAnimateType_;
    }

    void SetSvgAnimateType(SvgAnimateType svgAnimateType)
    {
        svgAnimateType_ = svgAnimateType;
    }

    const std::vector<std::string>& GetKeyValues() const
    {
        if (svgAnimateType_ == SvgAnimateType::MOTION) {
            return keyPoints_;
        }
        return values_;
    }

    void Copy(RefPtr<SvgAnimate>& svgAnimate)
    {
        svgAnimate->SetAttributeName(attributeName_);
        svgAnimate->SetBegin(begin_);
        svgAnimate->SetDur(dur_);
        svgAnimate->SetEnd(end_);
        svgAnimate->SetRepeatCount(repeatCount_);
        svgAnimate->SetFillMode(fillMode_);
        svgAnimate->SetCalcMode(calcMode_);
        svgAnimate->SetValues(values_);
        svgAnimate->SetKeyTimes(keyTimes_);
        svgAnimate->SetKeySplines(keySplines_);
        svgAnimate->SetFrom(from_);
        svgAnimate->SetTo(to_);
        svgAnimate->SetBy(by_);
        svgAnimate->SetSvgAnimateType(svgAnimateType_);
        svgAnimate->SetTransformType(transformType_);
    }

    bool GetValuesRange(std::vector<float>& from, std::vector<float>& to, std::string& type);

    bool GetFrames(std::vector<std::vector<float>>& frames, std::string& type);

protected:
    std::string attributeName_;
    int32_t begin_ = 0;
    int32_t dur_ = 0;
    int32_t end_ = 0;
    int32_t repeatCount_ = 1;
    Fill fillMode_ = Fill::REMOVE;
    CalcMode calcMode_ = CalcMode::LINEAR;
    std::vector<std::string> values_;
    std::vector<double> keyTimes_;
    std::vector<std::string> keySplines_;
    std::string from_;
    std::string to_;
    std::string by_;

    std::vector<std::string> keyPoints_;
    std::string path_;
    std::string rotate_;

    std::string transformType_;
    SvgAnimateType svgAnimateType_ = SvgAnimateType::ANIMATE;

private:
    template<typename T>
    void CreateKeyframe(
        const RefPtr<KeyframeAnimation<T>>& animation, const T& value, float time, const RefPtr<Curve>& curve);
    template<typename T>
    void CreateFirstKeyframe(const RefPtr<KeyframeAnimation<T>>& animation, const T& value);

    template<typename T>
    bool CreateDiscreteAnimate(
        std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator);
    template<typename T>
    bool DiscreteAnimate(
        const RefPtr<KeyframeAnimation<T>>& animation, const T& originalValue, const T& startValue, const T& endValue);
    template<typename T>
    bool DiscreteAnimate(const RefPtr<KeyframeAnimation<T>>& animation, const T& originalValue);
    template<typename T>
    bool DiscreteWithValues(const RefPtr<KeyframeAnimation<T>>& animation, const T& originalValue);
    template<typename T>
    bool DiscreteWithKeyTimes(const RefPtr<KeyframeAnimation<T>>& animation, const T& originalValue);

    template<typename T>
    bool CreateLinearAnimate(
        std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator);
    template<typename T>
    bool LinearAnimate(std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator);
    template<typename T>
    bool LinearAnimate(const RefPtr<KeyframeAnimation<T>>& animation);
    template<typename T>
    bool LinearWithKeyTimes(const RefPtr<KeyframeAnimation<T>>& animation);
    template<typename T>
    bool LinearWithValues(const RefPtr<KeyframeAnimation<T>>& animation);

    template<typename T>
    bool CreatePacedAnimate(
        std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator);

    template<typename T>
    bool CreateSplineAnimate(
        std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator);
    template<typename T>
    bool SplineAnimate(std::function<void(T)>&& callback, const T& originalValue, const RefPtr<Animator>& animator);
    template<typename T>
    bool SplineAnimate(const RefPtr<KeyframeAnimation<T>>& animation);
    template<typename T>
    bool SplineWithKeySplines(const RefPtr<KeyframeAnimation<T>>& animation);
    template<typename T>
    bool SplineWithKeyTimes(const RefPtr<KeyframeAnimation<T>>& animation);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_SVG_ANIMATE_H
