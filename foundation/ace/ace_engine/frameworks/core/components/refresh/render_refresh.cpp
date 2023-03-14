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

#include "core/components/refresh/render_refresh.h"

#include <chrono>
#include <ctime>

#include "base/i18n/localization.h"
#include "base/utils/string_utils.h"
#include "base/utils/utils.h"
#include "core/animation/curve_animation.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t ANIMATION_DURATION = 350; // Default animation duration
constexpr int32_t MAX_ALPHA = 255;
constexpr int32_t BASE_YEAR = 1900;
constexpr double DEFAULT_TIME_BOX_BOTTOM_SIZE = 8.0; // Default box height for time
constexpr double PERCENT = 0.01;                     // Percent
constexpr double HALF = 0.5;
const char REFRESH_LAST_UPDATED[] = "refresh.last_updated"; // I18n for last updated
const char LAST_UPDATE_FORMAT[] = "yyyy/M/d HH:mm";         // Date format for last updated
const char PULL_DOWN_START[] = "start";                     // Pull-down event, state is start
const char PULL_DOWN_END[] = "end";                         // Pull-down event, state is end

} // namespace

RenderRefresh::RenderRefresh()
{
    loadingComponent_ = AceType::MakeRefPtr<LoadingProgressComponent>();
    loading_ = AceType::DynamicCast<RenderLoadingProgress>(loadingComponent_->CreateRenderNode());
    loading_->SetLoadingMode(MODE_DRAG);

    loadingBackgroundBoxComponent_ = AceType::MakeRefPtr<BoxComponent>();

    decoration_ = AceType::MakeRefPtr<Decoration>();
    loadingBackgroundBox_ = AceType::DynamicCast<RenderBox>(loadingBackgroundBoxComponent_->CreateRenderNode());

    loadingBoxComponent_ = AceType::MakeRefPtr<BoxComponent>();
    loadingBoxComponent_->SetFlex(BoxFlex::FLEX_X);

    loadingBox_ = AceType::DynamicCast<RenderBox>(loadingBoxComponent_->CreateRenderNode());

    loadingBackgroundBox_->AddChild(loading_);
    loadingBox_->AddChild(loadingBackgroundBox_);

    timeBoxComponent_ = AceType::MakeRefPtr<BoxComponent>();
    timeBoxComponent_->SetFlex(BoxFlex::FLEX_X);
    timeBox_ = AceType::DynamicCast<RenderBox>(timeBoxComponent_->CreateRenderNode());

    lastTimeText_ = Localization::GetInstance()->GetEntryLetters(REFRESH_LAST_UPDATED);
    timeText_ = StringUtils::FormatString(lastTimeText_.c_str(), "");
    timeComponent_ = AceType::MakeRefPtr<TextComponent>(timeText_);

    displayComponent_ = AceType::MakeRefPtr<DisplayComponent>();

    display_ = AceType::DynamicCast<RenderDisplay>(displayComponent_->CreateRenderNode());
    time_ = AceType::DynamicCast<RenderText>(timeComponent_->CreateRenderNode());
    display_->AddChild(timeBox_);
    timeBox_->AddChild(time_);
}

void RenderRefresh::Update(const RefPtr<Component>& component)
{
    const RefPtr<RefreshComponent> refresh = AceType::DynamicCast<RefreshComponent>(component);
    if (!refresh) {
        LOGW("RefreshComponent is null");
        return;
    }
    if (refresh->GetOnStateChange()) {
        onStateChange_ = *refresh->GetOnStateChange();
    }
    if (refresh->GetOnRefreshing()) {
        onRefreshing_ = *refresh->GetOnRefreshing();
    }

    refreshComponent_ = AceType::WeakClaim(AceType::RawPtr(component));
    refreshing_ = refresh->IsRefreshing();
    showLastTime_ = refresh->IsShowLastTime();
    isUseOffset_ = refresh->IsUseOffset();
    refreshType_ = refresh->GetRefreshType();
    progressColor_ = refresh->GetProgressColor();
    backgroundColor_ = refresh->GetBackgroundColor();
    frictionRatio_ = refresh->GetFriction() * PERCENT;
    isRefresh_ = refresh->GetIsRefresh();

    loadingComponent_->SetProgressColor(progressColor_);
    loadingComponent_->SetDiameter(Dimension(GetLoadingDiameter()));

    // Set the progress background color
    decoration_->SetBackgroundColor(backgroundColor_);

    if (refresh->GetTextDirection() == TextDirection::LTR) {
        SetTextDirection(TextDirection::LTR);
        time_->SetTextDirection(TextDirection::LTR);
    } else {
        SetTextDirection(TextDirection::RTL);
        time_->SetTextDirection(TextDirection::RTL);
    }

    if (!isInitialized_) {
        loading_->Attach(GetContext());
        loadingBackgroundBox_->Attach(GetContext());
        loadingBox_->Attach(GetContext());
        AddChild(loadingBox_);

        timeBox_->Attach(GetContext());
        display_->Attach(GetContext());
        time_->Attach(GetContext());
        timeText_ = StringUtils::FormatString(lastTimeText_.c_str(), GetFormatDateTime().c_str());
        timeComponent_->SetData(timeText_);
        timeComponent_->SetTextStyle(refresh->GetTextStyle());
        time_->Update(timeComponent_);
        display_->Update(displayComponent_);
        timeBox_->Update(timeBoxComponent_);
        AddChild(display_);

        // Just run on the first time
        isInitialized_ = true;
    }

    loading_->Update(loadingComponent_);
    loadingBackgroundBox_->Update(loadingBackgroundBoxComponent_);
    loadingBox_->Update(loadingBoxComponent_);

    refreshEvent_ = AceAsyncEvent<void(const std::string&)>::Create(refresh->GetRefreshEventId(), context_);
    pullDownEvent_ = AceAsyncEvent<void(const std::string&)>::Create(refresh->GetPulldownEventId(), context_);
    CalcLoadingParams(component);
    Initialize();
    MarkNeedLayout();
}

void RenderRefresh::CalcLoadingParams(const RefPtr<Component>& component)
{
    auto refresh = AceType::DynamicCast<RefreshComponent>(component);
    if (refresh == nullptr) {
        LOGW("RefreshComponent is null");
        return;
    }
    auto context = context_.Upgrade();
    if (context == nullptr) {
        LOGW("context is nullptr!");
        return;
    }
    scale_ = context->GetDipScale();
    triggerLoadingDistance_ = NormalizeToPx(refresh->GetLoadingDistance());
    triggerShowTimeDistance_ = NormalizeToPx(refresh->GetShowTimeDistance());
    if (showLastTime_) {
        timeDistance_ = NormalizeToPx(Dimension(DEFAULT_TIME_BOX_BOTTOM_SIZE, DimensionUnit::VP));
        triggerRefreshDistance_ = triggerShowTimeDistance_;
    } else {
        triggerRefreshDistance_ = NormalizeToPx(refresh->GetRefreshDistance());
        inspectorOffset_ = refresh->GetRefreshDistance();
    }
    loadingDiameter_ = NormalizeToPx(refresh->GetProgressDiameter());
    maxScrollOffset_ = NormalizeToPx(refresh->GetMaxDistance());
    indicatorOffset_ = NormalizeToPx(refresh->GetIndicatorOffset());
    timeOffset_ = NormalizeToPx(refresh->GetTimeOffset());
    loading_->SetDiameter(refresh->GetProgressDiameter());
    loading_->SetDragRange(triggerLoadingDistance_, triggerRefreshDistance_);
    loadingBox_->SetHeight(loadingDiameter_);
    decoration_->SetBorderRadius(Radius(loadingDiameter_ * HALF));
    loadingBackgroundBox_->SetBackDecoration(decoration_);
    loadingBackgroundBox_->SetWidth(loadingDiameter_);
}

void RenderRefresh::Initialize()
{
    LOGI("RenderRefresh Initialize state:%{public}d", refreshing_);
    dragDetector_ = AceType::MakeRefPtr<VerticalDragRecognizer>();
    dragDetector_->SetOnDragUpdate([weakFresh = AceType::WeakClaim(this)](const DragUpdateInfo& info) {
        auto refresh = weakFresh.Upgrade();
        if (refresh) {
            refresh->HandleDragUpdate(info.GetMainDelta());
        }
    });
    dragDetector_->SetOnDragEnd([weakFresh = AceType::WeakClaim(this)](const DragEndInfo& info) {
        auto refresh = weakFresh.Upgrade();
        if (refresh) {
            refresh->HandleDragEnd();
        }
    });

    dragDetector_->SetOnDragCancel([weakFresh = AceType::WeakClaim(this)]() {
        auto refresh = weakFresh.Upgrade();
        if (refresh) {
            refresh->HandleDragCancel();
        }
    });

    animator_ = AceType::MakeRefPtr<Animator>(GetContext());
    refreshController_ = AceType::MakeRefPtr<RefreshController>();
    refreshController_->SetRefresh(AceType::WeakClaim(this));
    InitAccessibilityEventListener();
}

void RenderRefresh::InitAccessibilityEventListener()
{
    auto refNode = accessibilityNode_.Upgrade();
    if (!refNode) {
        return;
    }
    refNode->AddSupportAction(AceAction::ACTION_SCROLL_FORWARD);
    auto weakPtr = AceType::WeakClaim(this);
    refNode->SetActionScrollForward([weakPtr]() {
        auto refresh = weakPtr.Upgrade();
        if (refresh) {
            refresh->SetRefreshStatus(true);
            refresh->MarkNeedLayout();
        }
        return true;
    });
}

void RenderRefresh::UpdateTouchRect()
{
    touchRect_.SetSize(viewPort_);
    touchRect_.SetOffset(GetPosition());
    touchRectList_.emplace_back(touchRect_);
    SetTouchRectList(touchRectList_);
}

void RenderRefresh::HandleDragUpdate(double delta)
{
    if (isRefresh_) {
        return;
    }
    LOGD("RenderRefresh HandleDragUpdate delta is %{public}lf, offset is %{public}lf", delta, scrollableOffset_.GetY());
    if (NearZero(delta)) {
        LOGD("Delta is near zero!");
        return;
    }
    if (refreshStatus_ == RefreshStatus::REFRESH && delta > 0.0) {
        LOGD("The refresh status is refreshing!");
        return;
    }
    Offset deltaOffset(0, delta);
    if (refreshStatus_ == RefreshStatus::DRAG || refreshStatus_ == RefreshStatus::OVER_DRAG ||
        refreshStatus_ == RefreshStatus::DONE) {
        deltaOffset.SetY(GetOffset(delta));
    }
    scrollableOffset_ += deltaOffset;
    scrollableOffset_.SetY(std::clamp(scrollableOffset_.GetY(), 0.0, maxScrollOffset_));
    MarkNeedLayout();
}

void RenderRefresh::HandleDragEnd()
{
    LOGD("RenderRefresh HandleDragEnd");
    if (refreshStatus_ == RefreshStatus::INACTIVE) {
        return;
    }
    if (refreshStatus_ == RefreshStatus::DRAG || refreshStatus_ == RefreshStatus::DONE) {
        double start = scrollableOffset_.GetY();
        double end = 0.0;
        StartAnimation(start, end, false);
        return;
    }
    if (refreshStatus_ == RefreshStatus::OVER_DRAG) {
        double start = scrollableOffset_.GetY();
        double end = triggerRefreshDistance_;
        loading_->SetLoadingMode(MODE_LOOP);
        StartAnimation(start, end, false);
    }
}

void RenderRefresh::HandleDragCancel()
{
    LOGD("RenderRefresh HandleDragCancel");
    refreshing_ = false;
    refreshStatus_ = RefreshStatus::INACTIVE;
    scrollableOffset_.Reset();
    loading_->SetLoadingMode(MODE_DRAG);
    loading_->SetDragDistance(scrollableOffset_.GetY());
    MarkNeedLayout();
}

void RenderRefresh::UpdateScrollOffset(double value)
{
    scrollableOffset_.SetY(value);
    MarkNeedLayout();
}

void RenderRefresh::FireRefreshEvent() const
{
    if (refreshEvent_) {
        LOGI("RefreshEvent, refreshing = %{public}d.", refreshing_);
        std::string param =
            std::string(R"("refresh",{"refreshing":)").append(refreshing_ ? "true" : "false").append("},null");
        refreshEvent_(param);
    }
    if (onRefreshing_) {
        onRefreshing_();
    }
}

void RenderRefresh::FirePullDownEvent(const std::string& state) const
{
    if (pullDownEvent_) {
        LOGI("PullDown Event, state is %{public}s", state.c_str());
        std::string param = std::string(R"("pulldown",{"state":")").append(state).append("\"},null");
        pullDownEvent_(param);
    }
}

void RenderRefresh::StartAnimation(double start, double end, bool isFinished)
{
    animator_->ClearInterpolators();
    animator_->ClearStopListeners();
    translate_ = AceType::MakeRefPtr<CurveAnimation<double>>(start, end, Curves::FRICTION);
    auto weak = AceType::WeakClaim(this);
    translate_->AddListener(Animation<double>::ValueCallback([weak](double value) {
        auto scroll = weak.Upgrade();
        if (scroll) {
            scroll->UpdateScrollOffset(value);
        }
    }));
    animator_->SetDuration(ANIMATION_DURATION);
    animator_->AddInterpolator(translate_);

    animator_->AddStopListener([weak, isFinished]() {
        auto refresh = weak.Upgrade();
        if (refresh) {
            refresh->HandleStopListener(isFinished);
        }
    });

    animator_->Play();
}

void RenderRefresh::HandleStopListener(bool isFinished)
{
    // Update the last loading time
    if (isFinished) {
        timeComponent_->SetData(timeText_);
        time_->Update(timeComponent_);
    }
    if (NearEqual(scrollableOffset_.GetY(), triggerRefreshDistance_)) {
        if (refreshing_) {
            loading_->SetLoadingMode(MODE_LOOP);
        }
        refreshing_ = true;
        FireRefreshEvent();
    } else {
        loading_->SetLoadingMode(MODE_DRAG);
    }
}

RefreshStatus RenderRefresh::GetNextStatus()
{
    RefreshStatus nextStatus;
    auto context = context_.Upgrade();
    switch (refreshStatus_) {
        case RefreshStatus::INACTIVE:
            if (refreshing_) {
                StartAnimation(0.0, triggerRefreshDistance_, false);
                nextStatus = RefreshStatus::REFRESH;
                break;
            }
            if (LessOrEqual(scrollableOffset_.GetY(), 0.0)) {
                nextStatus = RefreshStatus::INACTIVE;
                break;
            }
            // No break here, continue next case
            nextStatus = RefreshStatus::DRAG;
            FirePullDownEvent(PULL_DOWN_START);
            [[fallthrough]];
        case RefreshStatus::DRAG:
            if (LessOrEqual(scrollableOffset_.GetY(), 0.0)) {
                FirePullDownEvent(PULL_DOWN_END);
                nextStatus = RefreshStatus::INACTIVE;
            } else if (scrollableOffset_.GetY() < triggerRefreshDistance_) {
                nextStatus = RefreshStatus::DRAG;
            } else {
                nextStatus = RefreshStatus::OVER_DRAG;
            }
            break;
        case RefreshStatus::OVER_DRAG:
            if (!refreshEvent_ && !context->GetIsDeclarative()) {
                nextStatus = RefreshStatus::DONE;
                break;
            }
            if (scrollableOffset_.GetY() > triggerRefreshDistance_) {
                nextStatus = RefreshStatus::OVER_DRAG;
                break;
            }
            // No break here, continue get next status.
            nextStatus = RefreshStatus::REFRESH;
            [[fallthrough]];
        case RefreshStatus::REFRESH:
            if (!refreshing_) {
                timeText_ = StringUtils::FormatString(lastTimeText_.c_str(), GetFormatDateTime().c_str());
                StartAnimation(scrollableOffset_.GetY(), 0.0, true);
                nextStatus = RefreshStatus::DONE;
                break;
            }
            nextStatus = RefreshStatus::REFRESH;
            break;
        case RefreshStatus::DONE:
            if (scrollableOffset_.GetY() > 0.0) {
                nextStatus = RefreshStatus::DONE;
            } else {
                FirePullDownEvent(PULL_DOWN_END);
                nextStatus = RefreshStatus::INACTIVE;
            }
            break;
        default:
            nextStatus = RefreshStatus::INACTIVE;
            break;
    }
    if (onStateChange_ && (refreshStatus_ != nextStatus)) {
        onStateChange_(static_cast<int>(nextStatus));
    }
    return nextStatus;
}

double RenderRefresh::GetFriction(double percentage) const
{
    if (NearEqual(percentage, 1.0)) {
        return 0.0;
    } else {
        return frictionRatio_ * std::pow(1.0 - percentage, SQUARE);
    }
}

double RenderRefresh::GetOffset(double delta) const
{
    double height = GetLayoutSize().Height();
    if (!NearZero(height)) {
        double friction = GetFriction(std::abs(scrollableOffset_.GetY() / height));
        return friction * delta;
    }
    return delta;
}

double RenderRefresh::MaxScrollableHeight() const
{
    return GetLayoutParam().GetMaxSize().Height();
}

void RenderRefresh::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!dragDetector_) {
        return;
    }
    dragDetector_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(dragDetector_);
}

double RenderRefresh::GetLoadingDiameter() const
{
    double diameter = 0.0;
    if (scrollableOffset_.GetY() < triggerLoadingDistance_) {
        return diameter;
    } else if (scrollableOffset_.GetY() < triggerRefreshDistance_) {
        double maxDistance = triggerRefreshDistance_ - triggerLoadingDistance_;
        double actualDistance = scrollableOffset_.GetY() - triggerLoadingDistance_;

        // Get the diameter by actual distance
        diameter = ((actualDistance * loadingDiameter_ * HALF) / maxDistance) + loadingDiameter_ * HALF;
    } else {
        diameter = loadingDiameter_;
    }
    return diameter;
}

Offset RenderRefresh::GetLoadingOffset() const
{
    auto pipelineContext = GetContext().Upgrade();
    if (!pipelineContext) {
        LOGE("pipelineContext update with nullptr");
        return Offset::Zero();
    }

    if (scrollableOffset_.GetY() < triggerLoadingDistance_) {
        return Offset::Zero();
    }
    if (!isUseOffset_) {
        return scrollableOffset_ * HALF - Offset(0.0, GetLoadingDiameter() * HALF);
    }
    double factor =
        (scrollableOffset_.GetY() - triggerLoadingDistance_) / (triggerRefreshDistance_ - triggerLoadingDistance_);
    return Offset(0.0, indicatorOffset_ * factor);
}

Offset RenderRefresh::GetShowTimeOffset() const
{
    auto pipelineContext = GetContext().Upgrade();
    if (!pipelineContext) {
        LOGE("pipelineContext update with nullptr");
        return Offset::Zero();
    }

    double bottomOffset = timeBox_->GetLayoutSize().Height() +
                          pipelineContext->NormalizeToPx(Dimension(DEFAULT_TIME_BOX_BOTTOM_SIZE, DimensionUnit::VP));
    return scrollableOffset_ - Offset(0.0, bottomOffset + timeOffset_);
}

double RenderRefresh::GetOpacity() const
{
    double factor = 0.0;
    if (scrollableOffset_.GetY() < triggerRefreshDistance_ - timeDistance_) {
        return factor;
    } else if (scrollableOffset_.GetY() < triggerRefreshDistance_) {
        double actualDistance = scrollableOffset_.GetY() - triggerRefreshDistance_ + timeDistance_;

        // Get the factor, timeDistance_ never be zero
        if (!NearZero(timeDistance_)) {
            factor = actualDistance / timeDistance_;
        }
    } else {
        factor = 1.0;
    }
    return factor;
}

std::string RenderRefresh::GetFormatDateTime()
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto local = std::localtime(&now);
    if (local == nullptr) {
        LOGE("Get localtime failed.");
        return "";
    }

    // This is for i18n date time
    DateTime dateTime;
    dateTime.year = static_cast<uint32_t>(local->tm_year + BASE_YEAR);
    dateTime.month = static_cast<uint32_t>(local->tm_mon);
    dateTime.day = static_cast<uint32_t>(local->tm_mday);
    dateTime.hour = static_cast<uint32_t>(local->tm_hour);
    dateTime.minute = static_cast<uint32_t>(local->tm_min);
    std::string time = Localization::GetInstance()->FormatDateTime(dateTime, LAST_UPDATE_FORMAT);
    LOGD("Last update refresh time is %{public}s", time.c_str());
    return time;
}

void RenderRefresh::UpdateScrollableOffset(double delta)
{
    LOGD("Update offset is %{public}lf", delta);
    if (NearZero(delta)) {
        LOGW("Delta is near zero!");
        return;
    }
    if (refreshStatus_ == RefreshStatus::REFRESH) {
        LOGW("The refresh status is refreshing!");
        return;
    }
    Offset offset = Offset(0.0, GetOffset(delta));
    scrollableOffset_ = scrollableOffset_ + offset;
    scrollableOffset_.SetY(std::min(scrollableOffset_.GetY(), maxScrollOffset_));
    MarkNeedLayout();
}

void RenderRefresh::OnHiddenChanged(bool hidden)
{
    if (!hidden) {
        refreshing_ = false;
        refreshStatus_ = RefreshStatus::INACTIVE;
        scrollableOffset_.Reset();
        loading_->SetLoadingMode(MODE_DRAG);
        loading_->SetDragDistance(scrollableOffset_.GetY());
        MarkNeedLayout();
    }
}

void RenderRefresh::PerformLayout()
{
    const auto& children = GetChildren();
    if (children.empty()) {
        LOGW("Refresh has no child!");
        return;
    }
    auto context = context_.Upgrade();
    if (context == nullptr) {
        LOGW("context is nullptr!");
        return;
    }

    RefreshStatus nextState = GetNextStatus();
    if (nextState != RefreshStatus::REFRESH && refreshStatus_ == RefreshStatus::REFRESH) {
        loading_->SetLoadingMode(MODE_EXIT);
    }
    refreshStatus_ = nextState;
    LayoutParam innerLayout = GetLayoutParam();
    innerLayout.SetMinSize(Size(0.0, 0.0));
    if (!NearEqual(scale_, context->GetDipScale())) {
        // Notify loading to updated when window size changed.
        CalcLoadingParams(refreshComponent_.Upgrade());
        loading_->Layout(innerLayout);
    }

    loading_->SetDragDistance(scrollableOffset_.GetY());
    loadingBox_->SetPosition(GetLoadingOffset());

    display_->UpdateOpacity(GetOpacity() * MAX_ALPHA);
    display_->SetPosition(GetShowTimeOffset());

    loadingBox_->SetHidden(scrollableOffset_.GetY() < triggerLoadingDistance_);

    columnChild_ = children.back();
    columnChild_->Layout(innerLayout);
    if (refreshType_ == RefreshType::PULL_DOWN && GreatNotEqual(scrollableOffset_.GetY(), 0.0)) {
        columnChild_->SetPosition(scrollableOffset_);
    } else {
        columnChild_->SetPosition(Offset::Zero());
    }

    display_->Layout(innerLayout);
    loadingBox_->Layout(innerLayout);
    timeBox_->Layout(innerLayout);
    SetLayoutSize(GetLayoutParam().GetMaxSize());
}

} // namespace OHOS::Ace
