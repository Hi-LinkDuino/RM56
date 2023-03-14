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

#ifndef OHOS_ACELITE_BES_MENU_COMPNENT_H
#define OHOS_ACELITE_BES_MENU_COMPNENT_H

#include "component.h"
#include "non_copyable.h"
#include "ui_bes_menu.h"

namespace OHOS {
namespace ACELite {

class BesMenuComponent final : public Component {
public:
    BesMenuComponent() = delete;
    BesMenuComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager);
    ~BesMenuComponent() override {}

protected:
    bool CreateNativeViews() override;
    void ReleaseNativeViews() override;
   
    UIView *GetComponentRootView() const override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
    
    void PostUpdate(uint16_t attrKeyId) override;
    bool RegisterPrivateEventListener(uint16_t eventTypeId, jerry_value_t funcValue, bool isStopPropagation) override;
   

private:
    void ReleaseFrame();
    bool ParseToFrames(jerry_value_t value);
    
    UIBesMenu *besMenu_;

    MenuItemInfo *frames_;
    uint8_t framesSize_;

};

} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_IMAGE_ANIMATOR_COMPNENT_H
