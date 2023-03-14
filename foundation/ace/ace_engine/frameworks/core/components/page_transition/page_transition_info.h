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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_TRANSITION_PAGE_TRANSITION_INFO_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_TRANSITION_PAGE_TRANSITION_INFO_H

#include <memory>
#include <optional>

#include "base/memory/ace_type.h"
#include "core/animation/curves.h"

namespace OHOS::Ace {

enum class PageTransitionType {
    ENTER,      // current page enter not specify route type.
    EXIT,       // current page exit not specify route type.
    ENTER_POP,  // top page pop from route stack, current page on top.
    ENTER_PUSH, // current page push into route stack.
    EXIT_POP,   // current page pop from route stack.
    EXIT_PUSH,  // another page push info route stack.
};

enum class RouteType {
    NONE,
    PUSH,
    POP,
};

enum class SlideEffect {
    NONE,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
};

using PageTransitionEventFunc = std::function<void(RouteType, const float&)>;

class ACE_EXPORT PageTransition : public AceType {
    DECLARE_ACE_TYPE(PageTransition, AceType);

public:
    static void ProcessPageTransitionType(const RefPtr<PageTransition>& pageTransition)
    {
        if (pageTransition->type_ == PageTransitionType::ENTER) {
            switch (pageTransition->routeType_) {
                case RouteType::POP:
                    pageTransition->type_ = PageTransitionType::ENTER_POP;
                    break;
                case RouteType::PUSH:
                    pageTransition->type_ = PageTransitionType::ENTER_PUSH;
                    break;
                case RouteType::NONE:
                default:
                    break;
            }
        } else if (pageTransition->type_ == PageTransitionType::EXIT) {
            switch (pageTransition->routeType_) {
                case RouteType::POP:
                    pageTransition->type_ = PageTransitionType::EXIT_POP;
                    break;
                case RouteType::PUSH:
                    pageTransition->type_ = PageTransitionType::EXIT_PUSH;
                    break;
                case RouteType::NONE:
                default:
                    break;
            }
        }
    }

    PageTransition(PageTransitionType type = PageTransitionType::ENTER) : type_(type) {}
    virtual ~PageTransition() = default;

    void SetRouteType(RouteType routeType)
    {
        routeType_ = routeType;
    }

    void SetDuration(int32_t duration)
    {
        tweenOption_.SetDuration(duration);
    }

    void SetDelay(int32_t delay)
    {
        tweenOption_.SetDelay(delay);
    }

    void SetCurve(const RefPtr<Curve>& curve)
    {
        tweenOption_.SetCurve(curve);
    }

    void SetEffect(SlideEffect effect)
    {
        effect_ = effect;
    }

    PageTransitionType GetType() const
    {
        return type_;
    }

    int32_t GetDuration() const
    {
        return tweenOption_.GetDuration();
    }

    int32_t GetDelay() const
    {
        return tweenOption_.GetDelay();
    }

    const RefPtr<Curve>& GetCurve() const
    {
        return tweenOption_.GetCurve();
    }

    const TweenOption& GetTweenOption() const
    {
        return tweenOption_;
    }

    RouteType GetRouteType() const
    {
        return routeType_;
    }

    SlideEffect GetSlideEffect() const
    {
        return effect_;
    }

    const PageTransitionEventFunc& GetOnEnterHandler() const
    {
        return OnEnterHandler_;
    }

    const PageTransitionEventFunc& GetOnExitHandler() const
    {
        return OnExitHandler_;
    }

    void AddTranslateAnimation(const Dimension& dx, const Dimension& dy, const Dimension& dz)
    {
        TransformOperation init;
        init.type_ = TransformOperationType::TRANSLATE;
        init.translateOperation_ = TranslateOperation(Dimension {}, Dimension {}, Dimension {});
        TransformOperation target;
        target.type_ = TransformOperationType::TRANSLATE;
        target.translateOperation_ = TranslateOperation(dx, dy, dz);

        if (type_ == PageTransitionType::ENTER) {
            std::swap(init, target);
        }
        auto animation = AceType::MakeRefPtr<CurveAnimation<TransformOperation>>(init, target, tweenOption_.GetCurve());
        tweenOption_.AddTransformAnimation(animation);
    }

    void AddScaleAnimation(float scaleX, float scaleY, float scaleZ, const Dimension& centerX, const Dimension& centerY)
    {
        TransformOperation init;
        init.type_ = TransformOperationType::SCALE;
        init.scaleOperation_ = ScaleOperation(1.0, 1.0, 1.0);

        TransformOperation target;
        target.type_ = TransformOperationType::SCALE;
        target.scaleOperation_ = ScaleOperation(scaleX, scaleY, scaleZ);

        if (type_ == PageTransitionType::ENTER) {
            std::swap(init, target);
        }
        auto animation = AceType::MakeRefPtr<CurveAnimation<TransformOperation>>(init, target, tweenOption_.GetCurve());

        tweenOption_.SetTransformOrigin(centerX, centerY);
        tweenOption_.AddTransformAnimation(animation);
    }

    void AddOpacityAnimation(float targetValue)
    {
        float initValue = 1.0f;
        if (type_ == PageTransitionType::ENTER) {
            std::swap(initValue, targetValue);
        }
        auto animation = AceType::MakeRefPtr<CurveAnimation<float>>(initValue, targetValue, tweenOption_.GetCurve());
        tweenOption_.SetOpacityAnimation(animation);
    }

    void SetOnEnterHandler(PageTransitionEventFunc&& handler)
    {
        OnEnterHandler_ = std::move(handler);
    }

    void SetOnExitHandler(PageTransitionEventFunc&& handler)
    {
        OnExitHandler_ = std::move(handler);
    }

private:
    RouteType routeType_ = RouteType::NONE;
    PageTransitionType type_ = PageTransitionType::ENTER;
    SlideEffect effect_ = SlideEffect::NONE;
    TweenOption tweenOption_;
    PageTransitionEventFunc OnEnterHandler_;
    PageTransitionEventFunc OnExitHandler_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PAGE_TRANSITION_PAGE_TRANSITION_INFO_H
