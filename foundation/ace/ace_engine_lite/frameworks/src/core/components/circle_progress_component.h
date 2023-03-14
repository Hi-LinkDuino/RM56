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

#ifndef OHOS_ACELITE_CIRCLE_PROGRESS_COMPONENT_H
#define OHOS_ACELITE_CIRCLE_PROGRESS_COMPONENT_H


#include "component.h"
#include "non_copyable.h"
#include "ui_circle_progress.h"

#ifdef OH_UI_PROGRESS_TOUCHABLE
#include "abstract_progress_component.h"
#endif

namespace OHOS {
namespace ACELite {
constexpr int16_t DEFAULT_START_ANGLE = 240;
constexpr int16_t DEFAULT_TOTAL_ANGLE = 240;
constexpr int16_t DEFAULT_STROKE_WIDTH = 32;

#ifdef OH_UI_PROGRESS_TOUCHABLE
class CircleProgressComponent final : public AbstractProgressComponent
#else
class CircleProgressComponent final : public Component
#endif
{
public:
    ACE_DISALLOW_COPY_AND_MOVE(CircleProgressComponent);
    CircleProgressComponent() = delete;
    CircleProgressComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager);
    ~CircleProgressComponent() override {}

protected:
    UIView *GetComponentRootView() const override;
#ifndef OH_UI_PROGRESS_TOUCHABLE
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
#endif
    bool ApplyPrivateStyle(const AppStyleItem* style) override;
    void PostUpdate(uint16_t attrKeyId) override;
    void OnViewAttached() override;
    void HandleExtraUpdate();
    bool CreateNativeViews() override;

private:
    void SetAngles();
    bool SetArcProgressStyle(const AppStyleItem* style);
    void SetStartAngle(const AppStyleItem *style);
    void SetTotalAngle(const AppStyleItem *style);
    bool SetArcColor(const AppStyleItem *style);
    bool SetArcBackgroundColor(const AppStyleItem *style);
    UICircleProgress progressView_;
    int16_t centerX_;
    int16_t centerY_;
    int16_t radius_;
    int16_t startAngle_;
    int16_t totalAngle_;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_CIRCLE_PROGRESS_COMPONENT_H
