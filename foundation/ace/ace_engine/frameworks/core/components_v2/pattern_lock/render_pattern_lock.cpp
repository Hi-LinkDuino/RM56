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

#include "core/components_v2/pattern_lock/render_pattern_lock.h"

#include "base/log/event_report.h"
#include "core/common/font_manager.h"
#include "core/components/common/properties/alignment.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace::V2 {
RenderPatternLock::RenderPatternLock()
{
    touchRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
    auto weak = AceType::WeakClaim(this);
    touchRecognizer_->SetOnTouchDown([weak](const TouchEventInfo& info) {
        auto patternLock = weak.Upgrade();
        if (patternLock && !info.GetTouches().empty()) {
            const auto& locationInfo = info.GetTouches().front();
            double moveDeltaX = locationInfo.GetLocalLocation().GetX();
            double moveDeltaY = locationInfo.GetLocalLocation().GetY();
            Offset touchPoint;
            touchPoint.SetX(moveDeltaX);
            touchPoint.SetY(moveDeltaY);
            patternLock->HandleCellTouchDown(touchPoint);
        }
    });
    touchRecognizer_->SetOnTouchMove([weak](const TouchEventInfo& info) {
        auto patternLock = weak.Upgrade();
        if (patternLock && !info.GetTouches().empty()) {
            const auto& locationInfo = info.GetTouches().front();
            double moveDeltaX = locationInfo.GetLocalLocation().GetX();
            double moveDeltaY = locationInfo.GetLocalLocation().GetY();
            Offset touchPoint;
            touchPoint.SetX(moveDeltaX);
            touchPoint.SetY(moveDeltaY);
            patternLock->HandleCellTouchMove(touchPoint);
        }
    });
    touchRecognizer_->SetOnTouchUp([weak](const TouchEventInfo& info) {
        auto patternLock = weak.Upgrade();
        if (patternLock) {
            patternLock->HandleCellTouchUp();
        }
    });
    touchRecognizer_->SetOnTouchCancel([weak](const TouchEventInfo& info) {
        auto patternLock = weak.Upgrade();
        if (patternLock) {
            patternLock->HandleCellTouchCancel();
        }
    });
}
void RenderPatternLock::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    touchRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(touchRecognizer_);
}
void RenderPatternLock::HandleCellTouchDown(const Offset& offset)
{
    if (!CheckAutoReset()) {
        return;
    }
    HandleReset();
    cellCenter_ = offset;
    bool isAdd = false;
    for (int16_t i = 0; i < COL_COUNT && !isAdd; i++) {
        for (int16_t j = 0; j < COL_COUNT && !isAdd; j++) {
            isAdd = AddChoosePoint(offset, i + 1, j + 1);
        }
    }
    MarkNeedRender();
    isMoveEventValid_ = true;
}
void RenderPatternLock::HandleCellTouchMove(const Offset& offset)
{
    if (!isMoveEventValid_) {
        return;
    }
    cellCenter_ = offset;
    bool isAdd = false;
    for (int16_t i = 0; i < COL_COUNT && !isAdd; i++) {
        for (int16_t j = 0; j < COL_COUNT && !isAdd; j++) {
            isAdd = AddChoosePoint(offset, i + 1, j + 1);
        }
    }
    MarkNeedRender();
}
void RenderPatternLock::HandleCellTouchUp()
{
    if (!CheckAutoReset()) {
        return;
    }
    animator_->Finish();
    isMoveEventValid_ = false;
    std::vector<int> chooseCellVec;
    for (auto it = choosePoint_.begin(); it != choosePoint_.end(); it++) {
        chooseCellVec.push_back((*it).GetCode());
    }
    if (callbackForJS_) {
        auto event = std::make_shared<PatternCompleteEvent>(chooseCellVec);
        if (event) {
            callbackForJS_(event);
        }
    }
    animator_->SetDuration(DOWN_DURATION);
    animator_->Forward();
    MarkNeedRender();
}

void RenderPatternLock::HandleCellTouchCancel()
{
    HandleCellTouchUp();
}
void RenderPatternLock::UpdateAttr(const RefPtr<Component>& component)
{
    const auto& patternLockComponent = AceType::DynamicCast<PatternLockComponent>(component);
    if (!patternLockComponent) {
        LOGE("PatternLockComponent is null!");
        return;
    }
    sideLength_ = patternLockComponent->GetSideLength().Value() < 0.0 ? 0.0_vp : patternLockComponent->GetSideLength();
    double cSideLength = patternLockComponent->GetSideLength().ConvertToVp();
    double cCircleRadius = patternLockComponent->GetCircleRadius().ConvertToVp();
    const int16_t radiusCount = COL_COUNT * RADIUS_TO_DIAMETER;
    double handleCircleRadius = cCircleRadius > cSideLength / SCALE_SELECTED_CIRCLE_RADIUS / radiusCount
                                    ? cSideLength / SCALE_SELECTED_CIRCLE_RADIUS / radiusCount
                                    : cCircleRadius;
    circleRadius_ = Dimension(handleCircleRadius < 0 ? 0 : handleCircleRadius, DimensionUnit::VP);
    double cStrokeWidth = patternLockComponent->GetStrokeWidth().ConvertToVp();
    double handleStrokeWidth = cStrokeWidth > cSideLength / COL_COUNT ? cSideLength / COL_COUNT : cStrokeWidth;
    strokeWidth_ = Dimension(handleStrokeWidth < 0 ? 0 : handleStrokeWidth, DimensionUnit::VP);
    circleRadiusAnimatorToIncrease_ = circleRadius_;
    circleRadiusAnimatorToDecrease_ = circleRadius_ * SCALE_ACTIVE_CIRCLE_RADIUS;
    regularColor_ = patternLockComponent->GetRegularColor();
    selectedColor_ = patternLockComponent->GetSelectedColor();
    activeColor_ = patternLockComponent->GetActiveColor();
    pathColor_ = patternLockComponent->GetPathColor();
    autoReset_ = patternLockComponent->GetAutoReset();
    callbackForJS_ = AceAsyncEvent<void(const std::shared_ptr<PatternCompleteEvent>&)>::Create(
        patternLockComponent->GetPatternCompleteEvent(), context_);
    const auto& patternLockController = patternLockComponent->GetPatternLockController();
    if (patternLockController) {
        auto weak = AceType::WeakClaim(this);
        patternLockController->SetResetImpl([weak]() {
            auto patternLock = weak.Upgrade();
            if (patternLock) {
                patternLock->HandleReset();
            }
        });
    }
}
void RenderPatternLock::Update(const RefPtr<Component>& component)
{
    UpdateAttr(component);
    // animator
    if (!animator_) {
        animator_ = AceType::MakeRefPtr<Animator>(GetContext());
        auto touchAnimation = AceType::MakeRefPtr<CurveAnimation<double>>(0.0, 1.0, Curves::SHARP);
        touchAnimation->AddListener([weak = AceType::WeakClaim(this)](double value) {
            auto patternLock = weak.Upgrade();
            if (patternLock) {
                Dimension radiusFir { patternLock->circleRadius_.ConvertToVp() *
                                          (1 + (SCALE_SELECTED_CIRCLE_RADIUS - 1) * value),
                    DimensionUnit::VP };
                patternLock->circleRadiusAnimatorToIncrease_ = radiusFir;
                double decreaseRate = 1 + (1 - value) * (SCALE_DECREASE - 1);
                Dimension radiusSec { patternLock->circleRadius_.ConvertToVp() * SCALE_ACTIVE_CIRCLE_RADIUS *
                                          decreaseRate,
                    DimensionUnit::VP };
                patternLock->circleRadiusAnimatorToDecrease_ = radiusSec;
                patternLock->MarkNeedRender();
            }
        });
        animator_->ClearInterpolators();
        animator_->AddInterpolator(touchAnimation);
        animator_->SetFillMode(FillMode::FORWARDS);
    }
    MarkNeedLayout();
}
void RenderPatternLock::PerformLayout()
{
    Size layoutSizeAfterConstrain =
        GetLayoutParam().Constrain(Size(NormalizeToPx(sideLength_), NormalizeToPx(sideLength_)));
    SetLayoutSize(layoutSizeAfterConstrain);
}
bool RenderPatternLock::AddChoosePoint(Offset offset, int16_t x, int16_t y)
{
    const int16_t scale = RADIUS_TO_DIAMETER;
    double offsetX = NormalizeToPx(sideLength_) / COL_COUNT / scale * (scale * x - 1);
    double offsetY = NormalizeToPx(sideLength_) / COL_COUNT / scale * (scale * y - 1);
    Offset centerOffset;
    centerOffset.SetX(offsetX);
    centerOffset.SetY(offsetY);
    double distance = (offset - centerOffset).GetDistance();
    if (distance <= (NormalizeToPx(circleRadius_) * SCALE_SELECTED_CIRCLE_RADIUS)) {
        if (!CheckChoosePoint(x, y)) {
            AddPassPoint(offset, x, y);
            animator_->SetDuration(DOWN_DURATION);
            animator_->Forward();
            choosePoint_.push_back(PatternLockCell(x, y));
        }
        return true;
    }
    return false;
}
void RenderPatternLock::HandleReset()
{
    isMoveEventValid_ = false;
    choosePoint_.clear();
    cellCenter_ = Offset::Zero();
    MarkNeedRender();
}
Offset RenderPatternLock::GetCircleCenterByXY(const Offset& offset, int16_t x, int16_t y)
{
    const int16_t scale = RADIUS_TO_DIAMETER;
    Offset cellCenter;
    cellCenter.SetX(offset.GetX() + NormalizeToPx(sideLength_) / COL_COUNT / scale * (x * scale - 1));
    cellCenter.SetY(offset.GetY() + NormalizeToPx(sideLength_) / COL_COUNT / scale * (y * scale - 1));
    return cellCenter;
}
bool RenderPatternLock::CheckChoosePoint(int16_t x, int16_t y) const
{
    for (auto it = choosePoint_.begin(); it != choosePoint_.end(); it++) {
        if ((*it).GetColumn() == x && (*it).GetRow() == y) {
            return true;
        }
    }
    return false;
}
bool RenderPatternLock::CheckChoosePointIsLastIndex(int16_t x, int16_t y, int16_t index) const
{
    if (!choosePoint_.empty() && static_cast<int16_t>(choosePoint_.size()) >= index) {
        if (choosePoint_.at(choosePoint_.size() - static_cast<uint32_t>(index)).GetColumn() == x &&
            choosePoint_.at(choosePoint_.size() - static_cast<uint32_t>(index)).GetRow() == y) {
            return true;
        }
    }
    return false;
}
bool RenderPatternLock::CheckAutoReset() const
{
    if (!autoReset_ && !choosePoint_.empty() && !isMoveEventValid_) {
        return false;
    }
    return true;
}
void RenderPatternLock::AddPassPoint(Offset offset, int16_t x, int16_t y)
{
    if (choosePoint_.empty()) {
        return;
    }
    passPointCount_ = 0;
    PatternLockCell lastCell = choosePoint_.back();
    int16_t lastX = lastCell.GetColumn();
    int16_t lastY = lastCell.GetRow();
    int16_t lastCode = lastCell.GetCode();
    int16_t nowCode = COL_COUNT * (y - 1) + (x - 1);
    std::vector<PatternLockCell> passPointVec;
    for (int16_t i = 1; i <= COL_COUNT; i++) {
        for (int16_t j = 1; j <= COL_COUNT; j++) {
            PatternLockCell passPoint = PatternLockCell(i, j);
            if ((passPoint.GetCode() >= nowCode && passPoint.GetCode() >= lastCode) ||
                (passPoint.GetCode() <= nowCode && passPoint.GetCode() <= lastCode)) {
                continue;
            }
            if ((j != y) && (j != lastY) &&
                ((double(lastX - i) / (lastY - j) == double(i - x) / (j - y)) && !CheckChoosePoint(i, j))) {
                passPointVec.push_back(passPoint);
            }
            if ((j == lastY) && (j == y) && !CheckChoosePoint(i, j)) {
                passPointVec.push_back(passPoint);
            }
        }
    }
    size_t passPointLength = passPointVec.size();
    if (passPointLength == 0) {
        return;
    }
    passPointCount_ = static_cast<int16_t>(passPointLength);
    if (nowCode > lastCode) {
        choosePoint_.push_back(passPointVec.front());
        if (passPointLength > 1) {
            choosePoint_.push_back(passPointVec.back());
        }
    } else {
        choosePoint_.push_back(passPointVec.back());
        if (passPointLength > 1) {
            choosePoint_.push_back(passPointVec.front());
        }
    }
}
} // namespace OHOS::Ace::V2
