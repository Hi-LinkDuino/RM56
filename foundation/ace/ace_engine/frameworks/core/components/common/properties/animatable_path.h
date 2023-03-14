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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_ANIMATABLE_PATH_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_ANIMATABLE_PATH_H

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/common/properties/animation_option.h"

namespace OHOS::Ace {

using RenderNodeAnimationCallback = std::function<void()>;

class ACE_EXPORT AnimatablePath final {
public:
    AnimatablePath() = default;
    explicit AnimatablePath(std::string path, const AnimationOption& option = AnimationOption())
        : path_(path), animationOption_(option) {}
    ~AnimatablePath() = default;

    void SetContextAndCallback(const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
    {
        context_ = context;
        animationCallback_ = callback;
    }

    std::string GetValue() const
    {
        return path_;
    }

    void SetValue(std::string path)
    {
        path_ = path;
    }

    const AnimationOption& GetAnimationOption() const
    {
        return animationOption_;
    }

    void SetAnimationOption(const AnimationOption& option)
    {
        animationOption_ = option;
    }

    AnimatablePath& operator=(const AnimatablePath& newPath);
private:
    void AnimateTo(std::string endValue);
    void ResetController();
    void OnAnimationCallback(double value);
    std::string FormatPathString(const std::string& path);

    bool isFirstAssign_ = true;
    std::string path_;
    std::string pathFrom_;
    std::string pathTo_;
    AnimationOption animationOption_;
    RefPtr<Animator> animationController_;
    WeakPtr<PipelineContext> context_;
    RenderNodeAnimationCallback animationCallback_ = nullptr;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_PROPERTIES_ANIMATABLE_PATH_H