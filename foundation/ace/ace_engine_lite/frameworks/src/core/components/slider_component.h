/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_SLIDER_COMPONENT_H
#define OHOS_ACELITE_SLIDER_COMPONENT_H

#include "component.h"
#include "event_listener.h"
#include "non_copyable.h"
#include "ui_slider.h"

namespace OHOS {
namespace ACELite {
class SliderComponent final : public Component {
public:
    ACE_DISALLOW_COPY_AND_MOVE(SliderComponent);
    SliderComponent() = delete;
    SliderComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager);
    ~SliderComponent() override {}

protected:
    UIView *GetComponentRootView() const override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
    bool RegisterPrivateEventListener(uint16_t eventTypeId, jerry_value_t funcValue, bool isStopPropagation) override;
    bool ApplyPrivateStyle(const AppStyleItem* style) override;
    void OnViewAttached() override;
    void PostUpdate(uint16_t attrKeyId) override;

private:
    static const int16_t DEFAULT_VALUE = 100;
    static const int8_t SLIDER_WIDTH = 104;          // default slider visible width
    static const int8_t DEFAULT_PADDING = 32;           // default padding
    void SetKnobValue(jerry_value_t attrValue);
    void SetDirection(const char * const directionValue);
    void SetWidthAndHeight();
    int16_t minValue_;
    int16_t maxValue_;
    int16_t value_;
    Point startPosition_;
    UISlider sliderView_;
    SliderEventListener eventListener_;
    UISlider::Direction direction_;
};
} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_SLIDER_COMPONENT_H
