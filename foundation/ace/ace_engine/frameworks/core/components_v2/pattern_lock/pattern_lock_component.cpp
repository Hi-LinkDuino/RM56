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
#include "core/components_v2/pattern_lock/pattern_lock_component.h"

#include "core/components_v2/pattern_lock/pattern_lock_element.h"
#include "core/components_v2/pattern_lock/render_pattern_lock.h"
namespace OHOS::Ace::V2 {
PatternLockComponent::PatternLockComponent()
{
    patternLockController_ = AceType::MakeRefPtr<V2::PatternLockController>();
}
RefPtr<RenderNode> PatternLockComponent::CreateRenderNode()
{
    return RenderPatternLock::Create();
}

RefPtr<Element> PatternLockComponent::CreateElement()
{
    return AceType::MakeRefPtr<PatternLockElement>();
}
void PatternLockComponent::SetRegularColor(const Color& regularColor)
{
    regularColor_ = regularColor;
}
void PatternLockComponent::SetSelectedColor(const Color& selectedColor)
{
    selectedColor_ = selectedColor;
}
void PatternLockComponent::SetActiveColor(const Color& activeColor)
{
    activeColor_ = activeColor;
}
void PatternLockComponent::SetPathColor(const Color& pathColor)
{
    pathColor_ = pathColor;
}
const Dimension& PatternLockComponent::GetCircleRadius() const
{
    return circleRadius_;
}
const Dimension& PatternLockComponent::GetSideLength() const
{
    return sideLength_;
}
void PatternLockComponent::SetCircleRadius(const Dimension& circleRadius)
{
    circleRadius_ = circleRadius;
}
void PatternLockComponent::SetSideLength(const Dimension& sideLength)
{
    sideLength_ = sideLength;
}
const Color& PatternLockComponent::GetRegularColor() const
{
    return regularColor_;
}
const Color& PatternLockComponent::GetActiveColor() const
{
    return activeColor_;
}
const Color& PatternLockComponent::GetPathColor() const
{
    return pathColor_;
}
const Color& PatternLockComponent::GetSelectedColor() const
{
    return selectedColor_;
}
void PatternLockComponent::SetStrokeWidth(const Dimension& lineWidth)
{
    strokeWidth_ = lineWidth;
}
const Dimension& PatternLockComponent::GetStrokeWidth() const
{
    return strokeWidth_;
}
void PatternLockComponent::SetAutoReset(bool isAutoReset)
{
    autoReset_ = isAutoReset;
}
bool PatternLockComponent::GetAutoReset() const
{
    return autoReset_;
}
RefPtr<V2::PatternLockController> PatternLockComponent::GetPatternLockController() const
{
    return patternLockController_;
}
} // namespace OHOS::Ace::V2