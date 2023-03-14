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

#include "core/components/test/json/wrap_creator.h"

#include <list>

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/box/box_component.h"
#include "core/components/wrap/wrap_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

const WrapDirection WRAP_DIRECTION[] = { WrapDirection::HORIZONTAL, WrapDirection::VERTICAL };
const WrapAlignment WRAP_ALIGNMENT[] = { WrapAlignment::START, WrapAlignment::END, WrapAlignment::CENTER };
const WrapAlignment WRAP_MAIN_ALIGNMENT[] = { WrapAlignment::START, WrapAlignment::END, WrapAlignment::CENTER,
    WrapAlignment::SPACE_AROUND, WrapAlignment::SPACE_BETWEEN };
const WrapAlignment WRAP_CROSS_ALIGNMENT[] = { WrapAlignment::START, WrapAlignment::END, WrapAlignment::CENTER,
    WrapAlignment::STRETCH };

} // namespace

RefPtr<Component> WrapCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    WrapDirection direction = WrapDirection::HORIZONTAL;
    double space = 0.0;
    double contentSpace = 0.0;
    WrapAlignment alignment = WrapAlignment::START;
    WrapAlignment mainAlignment = WrapAlignment::SPACE_AROUND;
    WrapAlignment crossAlignment = WrapAlignment::CENTER;
    auto children = factory.CreateChildComponent(componentJson);

    if (componentJson.Contains(SPACING) && componentJson.GetValue(SPACING)->IsNumber()) {
        space = componentJson.GetValue(SPACING)->GetDouble();
    }
    if (componentJson.Contains(CONTENT_SPACING) && componentJson.GetValue(CONTENT_SPACING)->IsNumber()) {
        contentSpace = componentJson.GetValue(CONTENT_SPACING)->GetDouble();
    }
    if (componentJson.Contains(ALIGNMENT) && componentJson.GetValue(ALIGNMENT)->IsNumber()) {
        alignment = ConvertIntToEnum(componentJson.GetValue(ALIGNMENT)->GetInt(), WRAP_ALIGNMENT, WrapAlignment::START);
    }
    if (componentJson.Contains(CROSS_ALIGNMENT) && componentJson.GetValue(CROSS_ALIGNMENT)->IsNumber()) {
        crossAlignment = ConvertIntToEnum(
            componentJson.GetValue(CROSS_ALIGNMENT)->GetInt(), WRAP_CROSS_ALIGNMENT, WrapAlignment::CENTER);
    }
    if (componentJson.Contains(MAIN_ALIGNMENT) && componentJson.GetValue(MAIN_ALIGNMENT)->IsNumber()) {
        mainAlignment = ConvertIntToEnum(
            componentJson.GetValue(MAIN_ALIGNMENT)->GetInt(), WRAP_MAIN_ALIGNMENT, WrapAlignment::SPACE_AROUND);
    }
    if (componentJson.Contains(DIRECTION) && componentJson.GetValue(DIRECTION)->IsNumber()) {
        direction =
            ConvertIntToEnum(componentJson.GetValue(DIRECTION)->GetInt(), WRAP_DIRECTION, WrapDirection::HORIZONTAL);
    }
    RefPtr<WrapComponent> wrap = AceType::MakeRefPtr<WrapComponent>(space, contentSpace, children);
    wrap->SetMainAlignment(mainAlignment);
    wrap->SetAlignment(alignment);
    wrap->SetCrossAlignment(crossAlignment);
    wrap->SetDirection(direction);
    return wrap;
}

} // namespace OHOS::Ace