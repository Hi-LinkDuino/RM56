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

#include "frameworks/bridge/js_frontend/engine/common/base_animation_bridge.h"

#include <functional>
#include <unordered_map>
#include <utility>

#include "base/json/json_util.h"
#include "base/utils/linear_map.h"
#include "base/utils/string_utils.h"
#include "base/utils/utils.h"
#include "core/animation/keyframe_animation.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/transform_convertor.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t MIN_SIZE = 2;
constexpr Dimension HALF = 0.5_pct;
constexpr Dimension FULL = 1.0_pct;
constexpr Dimension ZERO = 0.0_pct;

void JsParseAnimationFramesInternal(
    const std::unique_ptr<JsonValue>& argsPtrAnimation, std::unordered_map<std::string, std::string>& animationFrames)
{
    if (!argsPtrAnimation || argsPtrAnimation->IsNull()) {
        LOGE("args is null");
        return;
    }

    for (auto i = 0; i < argsPtrAnimation->GetArraySize(); i++) {
        auto item = argsPtrAnimation->GetArrayItem(i);
        if (!item || item->IsNull()) {
            continue;
        }
        auto key = item->GetKey();
        auto value = item->IsString() ? item->GetString() : item->ToString();

        if (!key.empty() && !value.empty()) {
            // in js api offset in range in [0, 1], but keyframe time range in range [0, 100]
            if (key == DOM_ANIMATION_OFFSET) {
                auto time = StringUtils::StringToDouble(value);
                value = std::to_string(time * 100.0);
            }
            animationFrames[key] = value;
        }
    }
}

int32_t JsParseIterations(const std::unique_ptr<JsonValue>& argsPtrIterations)
{
    if (!argsPtrIterations) {
        LOGE("Parse animation iterations failed, argsPtrIterations is null");
        return 0;
    }
    int32_t iterations = 0;
    if (argsPtrIterations->IsString()) {
        std::string iterationsString = argsPtrIterations->GetString();
        if (iterationsString == BaseAnimationBridgeUtils::ITERATIONS_INFINITY) {
            iterations = ANIMATION_REPEAT_INFINITE;
        } else {
            iterations = StringToInt(iterationsString);
        }
    } else if (argsPtrIterations->IsNumber()) {
        iterations = argsPtrIterations->GetInt();
    } else {
        iterations = 1;
    }
    return iterations;
}

void JsParseDoubleParams(const std::unique_ptr<JsonValue>& argsPtrDuration,
    const std::unique_ptr<JsonValue>& argsPtrDelay, std::unordered_map<std::string, double>& animationDoubleParams)
{
    if (argsPtrDelay) {
        double delay = 0.0;
        if (argsPtrDelay->IsString()) {
            delay = StringToDouble(argsPtrDelay->GetString());
        } else {
            delay = argsPtrDelay->GetDouble();
        }
        animationDoubleParams[DOM_ANIMATION_DELAY_API] = delay;
    }
    if (argsPtrDuration) {
        double duration = 0.0;
        if (argsPtrDuration->IsString()) {
            duration = StringToDouble(argsPtrDuration->GetString());
        } else {
            duration = argsPtrDuration->GetDouble();
        }
        animationDoubleParams[DOM_ANIMATION_DURATION_API] = duration;
    }
}

} // namespace

std::vector<Dimension> BaseAnimationBridgeUtils::HandleTransformOrigin(
    const std::vector<std::unordered_map<std::string, std::string>>& animationFrames)
{
    std::string transformOrigin;
    if (animationFrames.size() >= MIN_SIZE) {
        auto iterFrom = animationFrames.front().find(DOM_TRANSFORM_ORIGIN);
        if (iterFrom != animationFrames.front().end()) {
            transformOrigin = iterFrom->second;
        }
        if (transformOrigin.empty()) {
            auto iterTo = animationFrames.back().find(DOM_TRANSFORM_ORIGIN);
            if (iterTo != animationFrames.back().end()) {
                transformOrigin = iterTo->second;
            }
        }
    }

    std::vector<Dimension> transformOriginValue;
    if (transformOrigin.empty()) {
        return transformOriginValue;
    }

    static const LinearMapNode<std::vector<Dimension>> transformOriginMap[] = {
        { DOM_TRANSFORM_ORIGIN_CENTER_BOTTOM, { HALF, FULL } },
        { DOM_TRANSFORM_ORIGIN_CENTER_CENTER, { HALF, HALF } },
        { DOM_TRANSFORM_ORIGIN_CENTER_TOP, { HALF, ZERO } },
        { DOM_TRANSFORM_ORIGIN_LEFT_BOTTOM, { ZERO, FULL } },
        { DOM_TRANSFORM_ORIGIN_LEFT_CENTER, { ZERO, HALF } },
        { DOM_TRANSFORM_ORIGIN_LEFT_TOP, { ZERO, ZERO } },
        { DOM_TRANSFORM_ORIGIN_RIGHT_BOTTOM, { FULL, FULL } },
        { DOM_TRANSFORM_ORIGIN_RIGHT_CENTER, { FULL, HALF } },
        { DOM_TRANSFORM_ORIGIN_RIGHT_TOP, { FULL, ZERO } },
    };

    int64_t idx = BinarySearchFindIndex(transformOriginMap, ArraySize(transformOriginMap), transformOrigin.c_str());
    if (idx < 0) {
        auto pos = transformOrigin.find(' ', 0);
        if (pos != std::string::npos) {
            transformOriginValue.emplace_back(StringToDimension(transformOrigin.substr(0, pos)));
            transformOriginValue.emplace_back(StringToDimension(transformOrigin.substr(pos + 1)));
        }
    } else {
        transformOriginValue = transformOriginMap[idx].value;
    }
    return transformOriginValue;
}

void BaseAnimationBridgeUtils::SetTweenComponentParams(const RefPtr<Curve>& curve,
    const std::vector<std::unordered_map<std::string, std::string>>& animationFrames,
    RefPtr<TweenComponent>& tweenComponent, TweenOption& tweenOption)
{
    tweenComponent->SetCustomTweenOption(tweenOption);
    tweenComponent->SetCustomAnimationOperation(AnimationOperation::NONE);
}

void BaseAnimationBridgeUtils::JsParseAnimationFrames(
    const std::string& content, std::vector<std::unordered_map<std::string, std::string>>& animationFrames)
{
    auto argsPtr = JsonUtil::ParseJsonString(content);
    if (!argsPtr) {
        return;
    }
    auto argsPtrItem = argsPtr->GetArrayItem(0);
    if (!argsPtrItem) {
        LOGE("Animation frames are null.");
        return;
    }
    // Parse the arguments to each item in the frame
    auto size = argsPtrItem->GetArraySize();
    for (int32_t idx = 0; idx < size; ++idx) {
        auto argsPtrAnimation = argsPtrItem->GetArrayItem(idx);
        if (!argsPtrAnimation) {
            continue;
        }
        std::unordered_map<std::string, std::string> animationFrame;
        JsParseAnimationFramesInternal(argsPtrAnimation, animationFrame);
        if (idx == 0) {
            animationFrame[DOM_ANIMATION_OFFSET] = ANIMATION_FROM;
        } else if (idx == size - 1) {
            animationFrame[DOM_ANIMATION_OFFSET] = ANIMATION_TO;
        }
        animationFrames.emplace_back(animationFrame);
    }
}

void BaseAnimationBridgeUtils::JsParseAnimationOptions(const std::string& content, int32_t& iterations,
    std::unordered_map<std::string, double>& animationDoubleOptions,
    std::unordered_map<std::string, std::string>& animationStringOptions)
{
    auto argsPtr = JsonUtil::ParseJsonString(content);
    if (!argsPtr) {
        LOGE("Js Parse AnimationOption failed. argsPtr is null.");
        return;
    }

    auto argsPtrItem = argsPtr->GetArrayItem(1);
    if (!argsPtrItem) {
        LOGE("Js Parse AnimationOption failed. argsPtrItem is null.");
        return;
    }

    auto argsPtrItemIterations = argsPtrItem->GetValue(DOM_ANIMATION_ITERATIONS);
    auto argsPtrItemDelay = argsPtrItem->GetValue(DOM_ANIMATION_DELAY_API);
    auto argsPtrItemDuration = argsPtrItem->GetValue(DOM_ANIMATION_DURATION_API);
    auto argsPtrItemEasing = argsPtrItem->GetValue(DOM_ANIMATION_EASING);
    auto argsPtrItemFill = argsPtrItem->GetValue(DOM_ANIMATION_FILL);
    auto argsPtrItemDirection = argsPtrItem->GetValue(DOM_ANIMATION_DIRECTION_API);

    iterations = JsParseIterations(argsPtrItemIterations);
    JsParseDoubleParams(argsPtrItemDuration, argsPtrItemDelay, animationDoubleOptions);
    if (argsPtrItemEasing) {
        animationStringOptions[DOM_ANIMATION_EASING] = argsPtrItemEasing->GetString();
    }
    if (argsPtrItemFill) {
        animationStringOptions[DOM_ANIMATION_FILL] = argsPtrItemFill->GetString();
    }
    if (argsPtrItemDirection) {
        animationStringOptions[DOM_ANIMATION_DIRECTION_API] = argsPtrItemDirection->GetString();
    }
}

void BaseAnimationBridgeUtils::JsParseAnimatorParams(const std::string& content, int32_t& iterations,
    std::unordered_map<std::string, double>& animationDoubleParams,
    std::unordered_map<std::string, std::string>& animationStringParams)
{
    auto argsPtr = JsonUtil::ParseJsonString(content);
    if (!argsPtr) {
        LOGE("Js Parse animator params failed. argsPtr is null.");
        return;
    }

    auto argsPtrIterations = argsPtr->GetValue(DOM_ANIMATION_ITERATIONS);
    auto argsPtrDelay = argsPtr->GetValue(DOM_ANIMATION_DELAY_API);
    auto argsPtrDuration = argsPtr->GetValue(DOM_ANIMATION_DURATION_API);
    auto argsPtrEasing = argsPtr->GetValue(DOM_ANIMATION_EASING);
    auto argsPtrFill = argsPtr->GetValue(DOM_ANIMATION_FILL);
    auto argsPtrFrom = argsPtr->GetValue(DOM_ANIMATION_BEGIN);
    auto argsPtrTo = argsPtr->GetValue(DOM_ANIMATION_END);
    auto argsPtrDirection = argsPtr->GetValue(DOM_ANIMATION_DIRECTION_API);

    iterations = JsParseIterations(argsPtrIterations);
    JsParseDoubleParams(argsPtrDuration, argsPtrDelay, animationDoubleParams);
    if (argsPtrEasing) {
        animationStringParams[DOM_ANIMATION_EASING] = argsPtrEasing->GetString();
    }
    if (argsPtrFill) {
        animationStringParams[DOM_ANIMATION_FILL] = argsPtrFill->GetString();
    }
    if (argsPtrDirection) {
        animationStringParams[DOM_ANIMATION_DIRECTION_API] = argsPtrDirection->GetString();
    }
    if (argsPtrFrom) {
        double from = 0.0;
        from = argsPtrFrom->GetDouble();
        animationDoubleParams[DOM_ANIMATION_BEGIN] = from;
    }
    if (argsPtrTo) {
        double to = 1.0;
        to = argsPtrTo->GetDouble();
        animationDoubleParams[DOM_ANIMATION_END] = to;
    }
}
} // namespace OHOS::Ace::Framework
