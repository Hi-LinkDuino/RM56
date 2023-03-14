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

#include "core/components/clip/render_clip.h"

#include "base/log/dump_log.h"
#include "base/log/event_report.h"
#include "base/utils/utils.h"
#include "core/components/box/render_box.h"
#include "core/components/clip/clip_component.h"
#include "core/components/display/render_display.h"
#include "core/components/transform/render_transform.h"

namespace OHOS::Ace {

void RenderClip::Update(const RefPtr<Component>& component)
{
    const RefPtr<ClipComponent> clip = AceType::DynamicCast<ClipComponent>(component);
    if (!clip) {
        return;
    }
    clipWithShadow_ = clip->IsClipWithShadow();
    followChildSize_ = clip->IsFollowChild();
    width_ = NormalizeToPx(clip->GetWidth());
    height_ = NormalizeToPx(clip->GetHeight());
    offsetX_ = NormalizeToPx(clip->GetOffsetX());
    offsetY_ = NormalizeToPx(clip->GetOffsetY());

    topLeftRadius_ = clip->GetTopLeftRadius();
    topRightRadius_ = clip->GetTopRightRadius();
    bottomLeftRadius_ = clip->GetBottomLeftRadius();
    bottomRightRadius_ = clip->GetBottomRightRadius();

    MarkNeedLayout();
}

void RenderClip::PerformLayout()
{
    if (GetChildren().empty()) {
        LOGE("RenderClip: no child found in RenderClip!");
        EventReport::SendRenderException(RenderExcepType::CLIP_ERR);
        return;
    }
    auto child = GetChildren().front();
    child->Layout(GetLayoutParam());
    child->SetPosition(Offset::Zero());
    if (followChildSize_) {
        SetLayoutSize(child->GetLayoutSize());
    } else {
        SetLayoutSize(GetLayoutParam().GetMaxSize());
    }
}

FloatPropertyAnimatable::SetterMap RenderClip::GetFloatPropertySetterMap()
{
    FloatPropertyAnimatable::SetterMap map;
    auto weak = AceType::WeakClaim(this);
    map[PropertyAnimatableType::PROPERTY_WIDTH] = [weak](float value) {
        auto clip = weak.Upgrade();
        if (!clip) {
            LOGE("set width failed. clip is null.");
            return;
        }
        clip->SetWidth(value);
    };
    map[PropertyAnimatableType::PROPERTY_HEIGHT] = [weak](float value) {
        auto clip = weak.Upgrade();
        if (!clip) {
            LOGE("set height failed. clip is null.");
            return;
        }
        clip->SetHeight(value);
    };
    map[PropertyAnimatableType::PROPERTY_OFFSET_X] = [weak](float value) {
        auto clip = weak.Upgrade();
        if (!clip) {
            LOGE("set offsetX failed. clip is null.");
            return;
        }
        clip->SetOffsetX(value);
        clip->UpdateBoxForShadowAnimation();
    };
    map[PropertyAnimatableType::PROPERTY_OFFSET_Y] = [weak](float value) {
        auto clip = weak.Upgrade();
        if (!clip) {
            LOGE("set offsetY failed. clip is null.");
            return;
        }
        clip->SetOffsetY(value);
        clip->UpdateBoxForShadowAnimation();
    };
    map[PropertyAnimatableType::PROPERTY_BORDER_RADIUS] = [weak](float value) {
        auto clip = weak.Upgrade();
        if (!clip) {
            LOGE("set border radius failed. clip is null.");
            return;
        }
        clip->SetClipRadius(Radius(value));
    };
    return map;
};

FloatPropertyAnimatable::GetterMap RenderClip::GetFloatPropertyGetterMap()
{
    FloatPropertyAnimatable::GetterMap map;
    auto weak = AceType::WeakClaim(this);
    map[PropertyAnimatableType::PROPERTY_WIDTH] = [weak]() -> float {
        auto clip = weak.Upgrade();
        if (!clip) {
            LOGE("get width failed. clip is null.");
            return 0.0;
        }
        return clip->width_;
    };
    map[PropertyAnimatableType::PROPERTY_HEIGHT] = [weak]() -> float {
        auto clip = weak.Upgrade();
        if (!clip) {
            LOGE("get height failed. clip is null.");
            return 0.0;
        }
        return clip->height_;
    };
    map[PropertyAnimatableType::PROPERTY_OFFSET_X] = [weak]() -> float {
        auto clip = weak.Upgrade();
        if (!clip) {
            LOGE("get offsetX failed. clip is null.");
            return 0.0;
        }
        return clip->offsetX_;
    };
    map[PropertyAnimatableType::PROPERTY_OFFSET_Y] = [weak]() -> float {
        auto clip = weak.Upgrade();
        if (!clip) {
            LOGE("get offsetY failed. clip is null.");
            return 0.0;
        }
        return clip->offsetY_;
    };

    map[PropertyAnimatableType::PROPERTY_BORDER_RADIUS] = [weak]() -> float {
        auto clip = weak.Upgrade();
        if (!clip) {
            LOGE("get borer radius failed. clip is null.");
            return 0.0f;
        }
        return clip->topLeftRadius_.GetX().Value();
    };
    return map;
};

Rect RenderClip::GetClipRect(const Offset& offset) const
{
    double clipWidth = 0.0;
    if (!NearZero(width_)) {
        clipWidth = width_;
    } else {
        clipWidth = GetTransitionPaintRect().Width();
    }
    double clipHeight = 0.0;
    if (!NearZero(height_)) {
        clipHeight = height_;
    } else {
        clipHeight = GetTransitionPaintRect().Height();
    }
    return Rect(offset.GetX() + offsetX_, offset.GetY() + offsetY_, clipWidth, clipHeight);
}

void RenderClip::UpdateBoxForShadowAnimation()
{
    if (!clipWithShadow_) {
        return;
    }

    // If clip is used for animation, then there will be existed display and transform.
    auto renderTransform = AceType::DynamicCast<RenderTransform>(GetParent().Upgrade());
    if (!renderTransform) {
        return;
    }
    auto renderDisplay = AceType::DynamicCast<RenderDisplay>(renderTransform->GetParent().Upgrade());
    if (!renderDisplay) {
        return;
    }
    // There must be box for shadow animation.
    auto renderBox = DynamicCast<RenderBox>(renderDisplay->GetParent().Upgrade());
    if (renderBox) {
        renderBox->SetPaintSize(GetLayoutSize() - Size(offsetX_, offsetY_));
        renderBox->SetPosition(shadowBoxOffset_ + Offset(offsetX_, offsetY_));
        renderBox->MarkNeedRender();
        SetPosition(Offset(-offsetX_, -offsetY_));
    }
}

void RenderClip::Dump()
{
    DumpLog::GetInstance().AddDesc(std::string("Clip HW: ").append(Size(width_, height_).ToString()));
    DumpLog::GetInstance().AddDesc(std::string("Clip Offset: ").append(Offset(offsetX_, offsetY_).ToString()));
}

} // namespace OHOS::Ace
