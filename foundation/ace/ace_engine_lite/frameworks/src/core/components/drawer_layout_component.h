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

#ifndef OHOS_ACELITE_DRAWER_LAYOUT_COMPONENT_H
#define OHOS_ACELITE_DRAWER_LAYOUT_COMPONENT_H

#include "component.h"
#include "non_copyable.h"
#include "ui_drawer_layout.h"

namespace OHOS{
namespace ACELite{

class DrawerLayoutComponent final : public Component {
public:

    ACE_DISALLOW_COPY_AND_MOVE(DrawerLayoutComponent);
    DrawerLayoutComponent() = delete;
    DrawerLayoutComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager);
    ~DrawerLayoutComponent() override {}

protected:


    UIView *GetComponentRootView() const override;
    bool ProcessChildren() override;
    void AttachView(const Component *child) override;


private:
    UIDrawerLayout drawerLayout_;

};

} // namespace ACELite
} // namespace OHOS

#endif
