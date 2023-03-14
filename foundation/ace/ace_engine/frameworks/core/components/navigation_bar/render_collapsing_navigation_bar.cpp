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

#include "core/components/navigation_bar/render_collapsing_navigation_bar.h"

#include "core/animation/curve_animation.h"
#include "core/components/display/display_component.h"
#include "core/components/navigation_bar/navigation_bar_component.h"
#include "core/components/navigation_bar/render_navigation_container.h"
#include "core/components/transform/transform_component.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t COLLAPSING_ANIMATION_DURATION = 300;
constexpr double SPRING_DRAG_DELTA_OFFSET_RATIO = 7;
constexpr double SPRING_RESTORE_DELTA_OFFSET_RATIO = 5;
constexpr double FIX_TITLE_BAR_OFFSET_RATIO = 1.5;
constexpr double BIGGER_TITLE_SIZE = 1.0;
constexpr double BIGGER_TITLE_SIZE_MULTIPLE = 1.1;
constexpr double MAX_ALPHA_VALUE = 255.0;
constexpr double TRANSPARENT = 0.0;
constexpr Dimension HIDE_TITLE_MARGIN_TOP = 12.0_vp;
constexpr Dimension SHOW_TITLE_MARGIN_TOP = 8.0_vp;
constexpr Dimension BIGGER_TITLE_POSITION_OFFSET = 30.0_vp;

} // namespace

RefPtr<RenderNode> RenderCollapsingNavigationBar::Create()
{
    return AceType::MakeRefPtr<RenderCollapsingNavigationBar>();
}

void RenderCollapsingNavigationBar::Update(const RefPtr<Component>& component)
{
    Initialize(GetContext());

    auto collapsingComponent = AceType::DynamicCast<CollapsingNavigationBarComponent>(component);
    ACE_DCHECK(collapsingComponent);
    auto pipelineContext = GetContext();
    auto titleComposed = collapsingComponent->GetTitleComposed();
    if (titleComposed) {
        titleChangedCallback_ = [titleComposed, pipelineContext, weakBar = AceType::WeakClaim(this)](double fontSize) {
            auto boxComponent = AceType::DynamicCast<BoxComponent>(titleComposed->GetChild());
            if (!boxComponent) {
                return;
            }
            auto transformComponent = AceType::DynamicCast<TransformComponent>(boxComponent->GetChild());
            if (!transformComponent) {
                return;
            }
            auto bar = weakBar.Upgrade();
            if (!bar) {
                return;
            }
            double scale = fontSize / bar->lastTitleScale_;
            transformComponent->Scale(scale, scale);
            bar->lastTitleScale_ = scale * bar->lastTitleScale_;
            pipelineContext.Upgrade()->ScheduleUpdate(titleComposed);
        };
    }

    auto subtitleComposed = collapsingComponent->GetSubtitleComposed();
    if (subtitleComposed) {
        subtitleChangedCallback_ = [subtitleComposed, pipelineContext](double opacity) {
            auto boxComponent = AceType::DynamicCast<BoxComponent>(subtitleComposed->GetChild());
            if (!boxComponent) {
                return;
            }
            auto displayComponent = AceType::DynamicCast<DisplayComponent>(boxComponent->GetChild());
            if (!displayComponent) {
                return;
            }
            displayComponent->SetOpacity(opacity);
            pipelineContext.Upgrade()->ScheduleUpdate(subtitleComposed);
        };
    }
    minHeight_ = collapsingComponent->GetMinHeight();
    auto theme = GetTheme<NavigationBarTheme>();
    titleSize_ = ChangedKeyframe(theme->GetTitleFontSize().Value() / theme->GetTitleFontSizeBig().Value(),
        BIGGER_TITLE_SIZE, BIGGER_TITLE_SIZE_MULTIPLE);
    double subtitleOpacity = theme->GetSubTitleColor().GetAlpha() / MAX_ALPHA_VALUE;
    subtitleOpacity_ = ChangedKeyframe(TRANSPARENT, subtitleOpacity, subtitleOpacity);
    changeEvent_ = AceAsyncEvent<void(const std::shared_ptr<BaseEventInfo>&)>::Create(
        collapsingComponent->GetTitleModeChangedEvent(), context_);
}

void RenderCollapsingNavigationBar::PerformLayout()
{
    auto layoutParam = GetLayoutParam();
    SetLayoutSize(layoutParam.GetMaxSize());
    scrollableHeight_ = GetLayoutSize().Height() - NormalizeToPx(minHeight_);
    auto dipScale = GetContext().Upgrade()->GetDipScale();
    if (!NearEqual(dipScale_, dipScale)) {
        dipScale_ = dipScale;
        positionY_.Update(-scrollableHeight_, 0.0, BIGGER_TITLE_POSITION_OFFSET.ConvertToPx(dipScale_));
        titlePositionY_.Update(scrollableHeight_ + NormalizeToPx(HIDE_TITLE_MARGIN_TOP),
            NormalizeToPx(SHOW_TITLE_MARGIN_TOP + minHeight_),
            NormalizeToPx(SHOW_TITLE_MARGIN_TOP + minHeight_ + BIGGER_TITLE_POSITION_OFFSET));
    }

    double titlePositionY;
    double fixedToolBarPos = -positionY_.value;
    if (GreatNotEqual(positionY_.value, positionY_.expand)) {
        titlePositionY = titlePositionY_.expand + positionY_.value;
        fixedToolBarPos += positionY_.value / FIX_TITLE_BAR_OFFSET_RATIO;
    } else {
        titlePositionY = titlePositionY_.expand - positionY_.value * titlePositionY_.expandDis / positionY_.collapse;
    }

    auto fixedToolBar = GetFirstChild();
    fixedToolBar->Layout(layoutParam);
    fixedToolBar->SetPosition(Offset(0.0, fixedToolBarPos));
    auto titleZone = GetChildren().back();
    titleZone->Layout(layoutParam);
    titleZone->SetPosition(Offset(0, titlePositionY));
}

void RenderCollapsingNavigationBar::OnRelatedStart()
{
    relateEvent_ = true;
}

void RenderCollapsingNavigationBar::OnRelatedPreScroll(const Offset& delta, Offset& consumed)
{
    double dy = delta.GetY();
    if (!NeedHidden(dy)) {
        return;
    }

    ScrollBy(dy, positionY_.bigger);
    consumed.SetY(dy);
    if (!barIsMini_ && NearEqual(positionY_.value, positionY_.collapse)) {
        barIsMini_ = true;

        if (changeEvent_) {
            changeEvent_(std::make_shared<NavigationTitleModeChangeEvent>(barIsMini_));
        }
    }
}

void RenderCollapsingNavigationBar::OnRelatedScroll(const Offset& delta, Offset& consumed)
{
    double dy = -delta.GetY();
    if (!NeedShow(dy)) {
        if (!relateEvent_ && LessNotEqual(positionY_.value, positionY_.expand)) {
            PrepareTitleSizeTranslate(titleSize_.value, titleSize_.expand);
            PrepareSubtitleSizeTranslate(subtitleOpacity_.value, subtitleOpacity_.expand);
            PreparePositionTranslate(positionY_.value, positionY_.expand);
            controller_->Forward();
        }
        return;
    }
    if (barIsMini_) {
        barIsMini_ = false;
        if (changeEvent_) {
            changeEvent_(std::make_shared<NavigationTitleModeChangeEvent>(barIsMini_));
        }
    }

    if (!relateEvent_ && LessNotEqual(dy, 0.0)) {
        dy = dy / SPRING_RESTORE_DELTA_OFFSET_RATIO;
    } else if (GreatOrEqual(positionY_.value, positionY_.expand)) {
        dy = dy / SPRING_DRAG_DELTA_OFFSET_RATIO;
    }
    ScrollBy(dy, positionY_.bigger);
    if (LessOrEqual(positionY_.value, positionY_.expand) && relateEvent_) {
        consumed.SetY(dy);
    }
}

void RenderCollapsingNavigationBar::OnRelatedEnd()
{
    relateEvent_ = false;
}

void RenderCollapsingNavigationBar::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    dragRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(dragRecognizer_);
}

void RenderCollapsingNavigationBar::Initialize(const WeakPtr<PipelineContext>& context)
{
    dragRecognizer_ = AceType::MakeRefPtr<VerticalDragRecognizer>();
    dragRecognizer_->SetOnDragStart([weakScroll = AceType::WeakClaim(this)](const DragStartInfo& info) {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            scroll->HandleDragStart(info);
        }
    });
    dragRecognizer_->SetOnDragUpdate([weakScroll = AceType::WeakClaim(this)](const DragUpdateInfo& info) {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            scroll->HandleDragUpdate(info);
        }
    });
    dragRecognizer_->SetOnDragEnd([weakScroll = AceType::WeakClaim(this)](const DragEndInfo&) {
        auto scroll = weakScroll.Upgrade();
        if (scroll) {
            scroll->HandleDragEnd();
        }
    });
    controller_ = AceType::MakeRefPtr<Animator>(context);
    controller_->SetDuration(COLLAPSING_ANIMATION_DURATION);

    InitRelatedParent(GetParent());
    if (IsRelatedEventEnable()) {
        auto navigationContainer = AceType::DynamicCast<RenderNavigationContainer>(relatedParent_.Upgrade());
        if (navigationContainer) {
            navigationContainer->SetCollapsingNavigationBar(AceType::Claim(this));
        }
    }
}

void RenderCollapsingNavigationBar::ScrollBy(double dy, double maxPosition)
{
    lastUpScroll_ = GreatNotEqual(dy, 0.0) ? false : true;
    positionY_.value += dy;
    if (LessNotEqual(positionY_.value, -scrollableHeight_)) {
        positionY_.value = -scrollableHeight_;
    } else if (GreatNotEqual(positionY_.value, maxPosition)) {
        positionY_.value = maxPosition;
    }

    if (titleChangedCallback_) {
        if (GreatNotEqual(positionY_.value, positionY_.expand)) {
            titleSize_.value = titleSize_.expand + positionY_.value * titleSize_.biggerDis / positionY_.bigger;
        } else {
            titleSize_.value = titleSize_.expand - positionY_.value * titleSize_.expandDis / positionY_.collapse;
        }
        titleChangedCallback_(titleSize_.value);
    }
    if (subtitleChangedCallback_ && LessNotEqual(positionY_.value, positionY_.expand)) {
        subtitleOpacity_.value =
            subtitleOpacity_.expand + positionY_.value * subtitleOpacity_.expandDis / scrollableHeight_;
        subtitleChangedCallback_(subtitleOpacity_.value);
    }
    MarkNeedLayout();
}

void RenderCollapsingNavigationBar::HandleDragStart(const DragStartInfo& info) {}

void RenderCollapsingNavigationBar::HandleDragUpdate(const DragUpdateInfo& info)
{
    double mainDelta = info.GetMainDelta();
    bool canExpand = GreatNotEqual(mainDelta, 0.0) && LessNotEqual(positionY_.value, positionY_.expand);
    if (!NeedHidden(mainDelta) && !canExpand) {
        return;
    }
    ScrollBy(mainDelta, positionY_.expand);
}

void RenderCollapsingNavigationBar::HandleDragEnd()
{
    if (GreatNotEqual(positionY_.value, positionY_.expand)) {
        PrepareTitleSizeTranslate(titleSize_.value, titleSize_.expand);
        PrepareSubtitleSizeTranslate(subtitleOpacity_.value, subtitleOpacity_.expand);
        PreparePositionTranslate(positionY_.value, positionY_.expand);
        controller_->Forward();
    } else if (GreatNotEqual(positionY_.value, positionY_.collapse) && !NearZero(positionY_.value)) {
        if (lastUpScroll_) {
            PrepareTitleSizeTranslate(titleSize_.value, titleSize_.collapse);
            PrepareSubtitleSizeTranslate(subtitleOpacity_.value, subtitleOpacity_.collapse);
            PreparePositionTranslate(positionY_.value, positionY_.collapse);
            controller_->Forward();
        } else {
            PrepareTitleSizeTranslate(titleSize_.value, titleSize_.expand);
            PrepareSubtitleSizeTranslate(subtitleOpacity_.value, subtitleOpacity_.expand);
            PreparePositionTranslate(positionY_.value, positionY_.expand);
            controller_->Forward();
        }
    }
}

void RenderCollapsingNavigationBar::PrepareTitleSizeTranslate(double expand, double collapse)
{
    if (!titleChangedCallback_) {
        return;
    }
    titleSizeTranslate_ = AceType::MakeRefPtr<CurveAnimation<double>>(expand, collapse, Curves::FRICTION);
    auto weak = AceType::WeakClaim(this);
    titleSizeTranslate_->AddListener(Animation<double>::ValueCallback([weak](double value) {
        auto bar = weak.Upgrade();
        if (bar) {
            bar->titleChangedCallback_(value);
        }
    }));
    controller_->AddInterpolator(titleSizeTranslate_);
}

void RenderCollapsingNavigationBar::PrepareSubtitleSizeTranslate(double expand, double collapse)
{
    if (!subtitleChangedCallback_) {
        return;
    }
    subtitleSizeTranslate_ = AceType::MakeRefPtr<CurveAnimation<double>>(expand, collapse, Curves::FRICTION);
    auto weak = AceType::WeakClaim(this);
    subtitleSizeTranslate_->AddListener(Animation<double>::ValueCallback([weak](double value) {
        auto bar = weak.Upgrade();
        if (bar) {
            bar->subtitleChangedCallback_(value);
        }
    }));
    controller_->AddInterpolator(subtitleSizeTranslate_);
}

void RenderCollapsingNavigationBar::PreparePositionTranslate(double expand, double collapse)
{
    positionTranslate_ = AceType::MakeRefPtr<CurveAnimation<double>>(expand, collapse, Curves::FRICTION);
    auto weak = AceType::WeakClaim(this);
    positionTranslate_->AddListener(Animation<double>::ValueCallback([weak](double value) {
        auto bar = weak.Upgrade();
        if (bar) {
            bar->positionY_.value = value;
            bar->MarkNeedLayout();
        }
    }));
    controller_->AddInterpolator(positionTranslate_);
}

} // namespace OHOS::Ace
