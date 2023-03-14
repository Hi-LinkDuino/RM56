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

#include "core/components/test/json/text_creator.h"

#include <regex>

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/text_style.h"
#include "core/components/test/json/text_span_creator.h"
#include "core/components/test/json/text_style_creator.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

constexpr TextAlign TEXT_ALIGN_METHODS[] = { TextAlign::LEFT, TextAlign::RIGHT, TextAlign::CENTER, TextAlign::JUSTIFY,
    TextAlign::START, TextAlign::END };
constexpr TextOverflow TEXT_OVERFLOW_METHODS[] = { TextOverflow::CLIP, TextOverflow::ELLIPSIS };
constexpr TextDirection TEXT_DIRECTION_METHODS[] = { TextDirection::LTR, TextDirection::RTL, TextDirection::INHERIT };

} // namespace

RefPtr<Component> TextCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson Text");
    auto data = componentJson.GetString(TEXT_DATA);
    auto text = AceType::MakeRefPtr<TextComponent>(data);
    TextStyle textStyle;
    if (componentJson.Contains(TEXT_STYLE) && componentJson.GetValue(TEXT_STYLE)->IsObject()) {
        textStyle = TextStyleCreator::CreateFromJson(*componentJson.GetValue(TEXT_STYLE));
    }
    if (componentJson.Contains(TEXT_ALIGN) && componentJson.GetValue(TEXT_ALIGN)->IsNumber()) {
        textStyle.SetTextAlign(
            ConvertIntToEnum(componentJson.GetValue(TEXT_ALIGN)->GetInt(), TEXT_ALIGN_METHODS, TextAlign::START));
    }
    if (componentJson.Contains(TEXT_OVERFLOW) && componentJson.GetValue(TEXT_OVERFLOW)->IsNumber()) {
        textStyle.SetTextOverflow(ConvertIntToEnum(
            componentJson.GetValue(TEXT_OVERFLOW)->GetInt(), TEXT_OVERFLOW_METHODS, TextOverflow::CLIP));
    }
    if (componentJson.Contains(TEXT_DIRECTION) && componentJson.GetValue(TEXT_DIRECTION)->IsNumber()) {
        text->SetTextDirection(ConvertIntToEnum(
            componentJson.GetValue(TEXT_DIRECTION)->GetInt(), TEXT_DIRECTION_METHODS, TextDirection::LTR));
    }
    if (componentJson.Contains(TEXT_MAX_LINES) && componentJson.GetValue(TEXT_MAX_LINES)->IsNumber()) {
        textStyle.SetMaxLines(componentJson.GetValue(TEXT_MAX_LINES)->GetUInt());
    }
    text->SetTextStyle(textStyle);
    auto children = factory.CreateChildComponent(componentJson);
    for (const auto& child : children) {
        text->AppendChild(child);
    }
    return text;
}

} // namespace OHOS::Ace