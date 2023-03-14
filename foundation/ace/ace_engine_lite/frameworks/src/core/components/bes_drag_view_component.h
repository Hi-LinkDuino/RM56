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

#ifndef OHOS_ACELITE_BES_DRAG_VIEW_COMPNENT_H
#define OHOS_ACELITE_BES_DRAG_VIEW_COMPNENT_H

#include "component.h"
#include "non_copyable.h"
#include "ui_bes_drag_view.h"

namespace OHOS {
namespace ACELite {

class BesDragViewComponent final : public Component {
public:
    BesDragViewComponent() = delete;
    BesDragViewComponent(jerry_value_t options, jerry_value_t children, AppStyleManager* styleManager);
    ~BesDragViewComponent() override {}

protected:
    bool CreateNativeViews() override;
    void ReleaseNativeViews() override;
   
    UIView *GetComponentRootView() const override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;
   

private:

   
    
    UIBesDragView *besDragView_;

};

} // namespace ACELite
} // namespace OHOS

#endif // OHOS_ACELITE_IMAGE_ANIMATOR_COMPNENT_H
