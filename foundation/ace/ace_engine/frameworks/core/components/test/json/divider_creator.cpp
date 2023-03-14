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
#include "core/components/test/json/divider_creator.h"

#include "core/components/divider/divider_component.h"
#include "core/components/test/json/color_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> DividerCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    auto dividerComponent = AceType::MakeRefPtr<DividerComponent>();

    dividerComponent->SetStrokeWidth(Dimension(componentJson.GetDouble(DIVIDER_STROKE_WIDTH, 1.0)));
    dividerComponent->SetVertical(componentJson.GetBool(DIVIDER_VERTICAL, false));

    if (componentJson.Contains(DIVIDER_COLOR) && componentJson.GetValue(DIVIDER_COLOR)->IsObject()) {
        auto colorJson = componentJson.GetValue(DIVIDER_COLOR);
        dividerComponent->SetDividerColor(ColorCreator::CreateFromJson(*colorJson));
    }

    return dividerComponent;
}

} // namespace OHOS::Ace
