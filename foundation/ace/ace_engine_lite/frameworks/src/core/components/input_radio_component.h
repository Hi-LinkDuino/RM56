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
#ifndef OHOS_ACELITE_INPUT_RADIO_COMPONENT_H
#define OHOS_ACELITE_INPUT_RADIO_COMPONENT_H
#include "component.h"
#include "key_parser.h"
#include "non_copyable.h"
#include "ui_radio_button.h"

namespace OHOS {
namespace ACELite {
class InputRadioComponent final : public Component {
public:
    ACE_DISALLOW_COPY_AND_MOVE(InputRadioComponent);
    InputRadioComponent() = delete;
    InputRadioComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *manager)
        : Component(options, children, manager),
          changeListener_(nullptr),
          clickListener_(nullptr),
          normalBackGroundImg_(nullptr),
          pressedBackGroundImg_(nullptr)
    {
        SetComponentName(K_INPUT);
    }
    ~InputRadioComponent() override {}
    void ReleaseNativeViews() override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
    bool RegisterPrivateEventListener(uint16_t eventTypeId, jerry_value_t funcValue, bool isStopPropagation) override;
    bool ApplyPrivateStyle(const AppStyleItem *style) override;
    void PostRender() override;

protected:
    UIView *GetComponentRootView() const override
    {
        return const_cast<UIRadioButton *>(&radioButton_);
    }

private:
    void DealEvent();
    UIRadioButton radioButton_;
    StateChangeListener *changeListener_;
    ViewOnClickListener *clickListener_;
    char *normalBackGroundImg_;
    char *pressedBackGroundImg_;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_INPUT_RADIO_COMPONENT_H
