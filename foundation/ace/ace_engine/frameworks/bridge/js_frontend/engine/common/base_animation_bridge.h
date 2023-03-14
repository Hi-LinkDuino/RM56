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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_COMMON_BASE_ANIMATION_BRIDGE_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_COMMON_BASE_ANIMATION_BRIDGE_H

#include <string>
#include <unordered_map>

#include "base/geometry/dimension.h"
#include "core/components/tween/tween_component.h"

namespace OHOS::Ace::Framework {

enum class AnimatorOperation {
    NONE,
    PLAY,
    FINISH,
    PAUSE,
    CANCEL,
    REVERSE,
};

class ACE_EXPORT BaseAnimationBridgeUtils {
public:
    static constexpr int32_t TRANSFORM_ORIGIN_DEFAULT_SIZE = 2;
    static constexpr const char ITERATIONS_INFINITY[] = "Infinity";
    static constexpr const char ANIMATION_FROM[] = "0.0";
    static constexpr const char ANIMATION_TO[] = "100.0";
    // prefix id of TweenComponent, for differentiation from id of ComposedComponent
    static constexpr const char COMPONENT_PREFIX[] = "FrontendTween";

    static std::vector<Dimension> HandleTransformOrigin(
        const std::vector<std::unordered_map<std::string, std::string>>& animationFrames);

    static void SetTweenComponentParams(const RefPtr<Curve>& curve,
        const std::vector<std::unordered_map<std::string, std::string>>& animationFrames,
        RefPtr<TweenComponent>& tweenComponent, TweenOption& tweenOption);
    static void JsParseAnimationFrames(
        const std::string& content, std::vector<std::unordered_map<std::string, std::string>>& animationFrames);

    static void JsParseAnimationOptions(const std::string& content, int32_t& iterations,
        std::unordered_map<std::string, double>& animationDoubleOptions,
        std::unordered_map<std::string, std::string>& animationStringOptions);

    static void JsParseAnimatorParams(const std::string& content, int32_t& iterations,
        std::unordered_map<std::string, double>& animationDoubleParams,
        std::unordered_map<std::string, std::string>& animationStringParams);
};

class BaseAnimationBridge : public virtual AceType {
    DECLARE_ACE_TYPE(BaseAnimationBridge, AceType);
public:
    virtual void OnJsEngineDestroy() {}

    virtual RefPtr<Animator> JsGetAnimator()
    {
        return nullptr;
    };
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_COMMON_BASE_ANIMATION_BRIDGE_H
