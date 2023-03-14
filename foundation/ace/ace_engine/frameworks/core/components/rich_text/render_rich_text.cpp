/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/rich_text/render_rich_text.h"

#include <iomanip>
#include <sstream>

#include "base/log/log.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

RenderRichText::RenderRichText() : RenderNode(true)
{
    LOGI("[richtext] recognizer init");
    touchRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
    touchRecognizer_->SetOnTouchDown([wp = AceType::WeakClaim(this)](const TouchEventInfo& info) {
        auto sp = wp.Upgrade();
        if (sp) {
            sp->prevPos_ = info.GetTouches().front().GetLocalLocation().GetY();
        }
    });
    touchRecognizer_->SetOnTouchUp([wp = AceType::WeakClaim(this)](const TouchEventInfo& info) {
        // do nothing
    });
    touchRecognizer_->SetOnTouchMove([wp = AceType::WeakClaim(this)](const TouchEventInfo& info) {
        auto sp = wp.Upgrade();
        if (!sp) {
            LOGE("[richtext] process move, could not get render node");
            return;
        }

        sp->PorcessMove(info.GetTouches().front().GetLocalLocation().GetY());
    });

    // register this listener for consuming the drag events.
    dragRecognizer_ = AceType::MakeRefPtr<VerticalDragRecognizer>();
    dragRecognizer_->SetOnDragStart([](const DragStartInfo& info) {});
    dragRecognizer_->SetOnDragEnd([](const DragEndInfo& info) {});
}

void RenderRichText::PorcessMove(double posY)
{
    auto diff = prevPos_ - posY;
    double scale = 1.0f;
    auto context = GetContext().Upgrade();
    if (context) {
        scale = context->GetViewScale();
    }
    prevPos_ = posY;

    // downside or upside
    if (diff > 0) {
        if (!startSelfScroll_ && currentScrollLength_ == couldScrollLength_) {
            // do nothing
        } else {
            if (currentScrollLength_ > couldScrollLength_) {
                startSelfScroll_ = false;
                currentScrollLength_ = couldScrollLength_;
            } else {
                startSelfScroll_ = true;
                currentScrollLength_ += std::round(scale * diff);
            }
        }
    } else {
        if (!startSelfScroll_ && currentScrollLength_ == 0) {
            // do nothing
        } else {
            if (currentScrollLength_ < 0) {
                startSelfScroll_ = false;
                currentScrollLength_ = 0;
            } else {
                startSelfScroll_ = true;
                currentScrollLength_ += std::round(scale * diff);
            }
        }
    }

    // let window scroll
    if (startSelfScroll_ && delegate_) {
        delegate_->UpdateContentScroll(0, currentScrollLength_);
    }
}

void RenderRichText::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (canSelfScroll_) {
        // control self scroll, true only self scroll, false outside or parent render node scroll
        if (startSelfScroll_) {
            if (dragRecognizer_) {
                dragRecognizer_->SetCoordinateOffset(coordinateOffset);
                result.emplace_back(dragRecognizer_);
            }
        }

        if (touchRecognizer_) {
            touchRecognizer_->SetCoordinateOffset(coordinateOffset);
            result.emplace_back(touchRecognizer_);
        }
    }
}

void RenderRichText::Update(const RefPtr<Component>& component)
{
    if (!component || !delegate_) {
        return;
    }

    const RefPtr<RichTextComponent> richText = AceType::DynamicCast<RichTextComponent>(component);
    if (!richText) {
        LOGE("RenderRichText update dynamicCast to nullptr error");
        return;
    }

    auto& declaration = richText->GetDeclaration();
    if (declaration && declaration->HasDisplayStyle()) {
        bool visible = true;
        auto& style = static_cast<CommonVisibilityStyle&>(declaration->GetStyle(StyleTag::COMMON_VISIBILITY_STYLE));
        if (style.IsValid()) {
            visible = style.visibility == VisibilityType::VISIBLE ? true : false;
        }

        if (isVisible_ != visible) {
            isVisible_ = visible;
            if (hasCreateWeb_) {
                delegate_->ChangeRichTextVisibility(isVisible_);
            }
        }
    }
    if (!hasCreateWeb_) {
        CreateRealWeb(0, 0, isVisible_);
    }

    drawSize_.SetWidth(webContentWidth_);
    drawSize_.SetHeight(webContentHeight_);

    MarkNeedLayout(true, true);
}

void RenderRichText::CreateRealWeb(int32_t top, int32_t left, bool visible, bool reCreate)
{
    if (reCreate) {
        delegate_->ReleasePlatformResource();
        delegate_->CreatePlatformResource(context_, top, left, visible);
        return;
    }

    hasCreateWeb_ = true;
    delegate_->CreatePlatformResource(context_, top, left, visible);
}

void RenderRichText::UpdateLayoutParams(const int32_t width, const int32_t height, const int32_t contentHeight)
{
    float scale = 1.0f;
    auto pipelineContext = context_.Upgrade();
    if (pipelineContext) {
        scale = pipelineContext->GetViewScale();
    }
    if (!NearZero(scale)) {
        webContentWidth_ = width / scale;
        webContentHeight_ = height / scale;
    } else {
        webContentWidth_ = width;
        webContentHeight_ = height;
    }
    drawSize_.SetWidth(webContentWidth_);
    drawSize_.SetHeight(webContentHeight_);

    auto diff = contentHeight - height;
    LOGI("richtext update layout h:%{public}d c-h:%{public}d", height, contentHeight);
    // content height should more than layout height 2 pixel.
    if (diff > 2) {
        canSelfScroll_ = true;
        couldScrollLength_ = diff;
    }

    MarkNeedLayout(false, true);
}

void RenderRichText::OnGlobalPositionChanged()
{
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext || !delegate_) {
        LOGE("update rich text position, but context or delegate is null");
        return;
    }
    Offset offset = GetGlobalOffset();
    double top = offset.GetY() * pipelineContext->GetViewScale();
    double left = offset.GetX() * pipelineContext->GetViewScale();

    if (!initPositionSet_) {
        initPositionSet_ = true;
        CreateRealWeb((int32_t)top, (int32_t)left, isVisible_, true);
    } else {
        delegate_->UpdateWebPostion((int32_t)top, (int32_t)left);
    }
}

void RenderRichText::PerformLayout()
{
    if (!NeedLayout()) {
        LOGI("Render Rich Text PerformLayout No Need to Layout");
        return;
    }
    drawSize_ = Size(webContentWidth_, webContentHeight_);
    SetLayoutSize(drawSize_);
    SetNeedLayout(false);
    MarkNeedRender();
}

void RenderRichText::MarkNeedRender(bool overlay)
{
    RenderNode::MarkNeedRender(overlay);
}

} // namespace OHOS::Ace
