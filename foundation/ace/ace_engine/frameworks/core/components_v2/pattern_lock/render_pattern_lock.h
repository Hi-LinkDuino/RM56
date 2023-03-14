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
#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_PATTERN_LOCK_RENDER_PATTERN_LOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_PATTERN_LOCK_RENDER_PATTERN_LOCK_H
#include "base/utils/system_properties.h"
#include "core/components/common/properties/color.h"
#include "core/components_v2/pattern_lock/pattern_lock_component.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace::V2 {
using CallbackForJS = std::function<void(const std::string&)>;
using PatternCompleteFunc = std::function<void(std::shared_ptr<PatternCompleteEvent>&)>;
constexpr int16_t COL_COUNT = 3;
constexpr double SCALE_SELECTED_CIRCLE_RADIUS = 26.00 / 14.00;
constexpr double SCALE_ACTIVE_CIRCLE_RADIUS = 16.00 / 14.00;
constexpr double SCALE_DECREASE = 26.00 / 16.00;
constexpr double GRADUAL_CHANGE_POINT = 0.5;
constexpr int32_t DOWN_DURATION = 150;
constexpr Dimension DEFAULT_LINE_WIDTH = 34.0_vp;
constexpr int32_t MAX_ALPHA = 255;
constexpr int32_t RADIUS_TO_DIAMETER = 2;
class RenderPatternLock : public RenderNode {
    DECLARE_ACE_TYPE(RenderPatternLock, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

protected:
    bool AddChoosePoint(Offset offset, int16_t x, int16_t y);
    void AddPassPoint(Offset offset, int16_t x, int16_t y);
    bool CheckChoosePoint(int16_t x, int16_t y) const;
    bool CheckChoosePointIsLastIndex(int16_t x, int16_t y, int16_t index) const;
    Offset GetCircleCenterByXY(const Offset& offset, int16_t x, int16_t y);
    void HandleCellTouchDown(const Offset& offset);
    void HandleCellTouchMove(const Offset& offset);
    void HandleCellTouchUp();
    void HandleCellTouchCancel();
    void HandleReset();
    bool CheckAutoReset() const;
    void UpdateAttr(const RefPtr<Component>& component);

protected:
    class PatternLockCell {
    public:
        PatternLockCell(int16_t column, int16_t row)
        {
            column_ = column;
            row_ = row;
            code_ = COL_COUNT * (row - 1) + (column - 1);
        };
        ~PatternLockCell() = default;
        int16_t GetColumn() const
        {
            return column_;
        }
        int16_t GetRow() const
        {
            return row_;
        }
        int16_t GetCode() const
        {
            return code_;
        }

    private:
        int16_t column_;
        int16_t row_;
        int16_t code_;
    };
    RenderPatternLock();
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    Dimension circleRadius_;
    Dimension circleRadiusAnimatorToIncrease_;
    Dimension circleRadiusAnimatorToDecrease_;
    Dimension sideLength_;
    Color regularColor_;
    Color selectedColor_;
    Color activeColor_;
    Color pathColor_;
    Dimension strokeWidth_;
    PatternCompleteFunc callbackForJS_;
    RefPtr<RawRecognizer> touchRecognizer_;
    bool isMoveEventValid_ = false;
    Offset cellCenter_ = Offset::Zero();
    std::vector<PatternLockCell> choosePoint_;
    RefPtr<Animator> animator_;
    bool autoReset_ = true;
    int16_t passPointCount_ = 0;
};
} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_PATTERN_LOCK_RENDER_PATTERN_LOCK_H