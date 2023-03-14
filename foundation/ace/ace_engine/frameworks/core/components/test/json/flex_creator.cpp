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

#include "core/components/test/json/flex_creator.h"

#include "base/log/log.h"
#include "base/utils/macros.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/flex/flex_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

const FlexAlign FLEX_ALIGNS[] = { FlexAlign::AUTO, FlexAlign::FLEX_START, FlexAlign::CENTER, FlexAlign::FLEX_END,
    FlexAlign::STRETCH, FlexAlign::BASELINE, FlexAlign::SPACE_AROUND, FlexAlign::SPACE_BETWEEN };
const MainAxisSize MAIN_AXIS_SIZES[] = { MainAxisSize::MAX, MainAxisSize::MIN };
const TextBaseline BASELINES[] = { TextBaseline::ALPHABETIC, TextBaseline::IDEOGRAPHIC };

RefPtr<Component> CreateFlexFromJson(
    const JsonValue& componentJson, const JsonComponentFactory& factory, FlexDirection direction)
{
    FlexAlign mainAlign = FlexAlign::FLEX_START;
    FlexAlign crossAlign = FlexAlign::FLEX_START;
    MainAxisSize mainAxisSize = MainAxisSize::MAX;
    TextBaseline textBaseline = TextBaseline::ALPHABETIC;
    auto children = factory.CreateChildComponent(componentJson);

    if (componentJson.Contains(MAIN_AXIS_ALIGN) && componentJson.GetValue(MAIN_AXIS_ALIGN)->IsNumber()) {
        mainAlign =
            ConvertIntToEnum(componentJson.GetValue(MAIN_AXIS_ALIGN)->GetInt(), FLEX_ALIGNS, FlexAlign::FLEX_START);
    }
    if (componentJson.Contains(CROSS_AXIS_ALIGN) && componentJson.GetValue(CROSS_AXIS_ALIGN)->IsNumber()) {
        crossAlign =
            ConvertIntToEnum(componentJson.GetValue(CROSS_AXIS_ALIGN)->GetInt(), FLEX_ALIGNS, FlexAlign::FLEX_START);
    }
    RefPtr<FlexComponent> flex;
    if (direction == FlexDirection::ROW) {
        flex = AceType::MakeRefPtr<RowComponent>(mainAlign, crossAlign, children);
    } else {
        flex = AceType::MakeRefPtr<ColumnComponent>(mainAlign, crossAlign, children);
    }
    if (componentJson.Contains(MAIN_AXIS_SIZE) && componentJson.GetValue(MAIN_AXIS_SIZE)->IsNumber()) {
        mainAxisSize =
            ConvertIntToEnum(componentJson.GetValue(MAIN_AXIS_SIZE)->GetInt(), MAIN_AXIS_SIZES, MainAxisSize::MAX);
    }
    flex->SetMainAxisSize(mainAxisSize);
    if (componentJson.Contains(TEXT_BASE_LINE) && componentJson.GetValue(TEXT_BASE_LINE)->IsNumber()) {
        textBaseline =
            ConvertIntToEnum(componentJson.GetValue(TEXT_BASE_LINE)->GetInt(), BASELINES, TextBaseline::ALPHABETIC);
    }
    flex->SetBaseline(textBaseline);
    return flex;
}

} // namespace

RefPtr<Component> RowCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson Row");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != ROW_NAME) {
        LOGE("Create row err: not a row json.");
        return nullptr;
    }
    return CreateFlexFromJson(componentJson, factory, FlexDirection::ROW);
}

RefPtr<Component> ColumnCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson Column");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != COLUMN_NAME) {
        LOGE("Create column err: not a column json.");
        return nullptr;
    }

    return CreateFlexFromJson(componentJson, factory, FlexDirection::COLUMN);
}

} // namespace OHOS::Ace
