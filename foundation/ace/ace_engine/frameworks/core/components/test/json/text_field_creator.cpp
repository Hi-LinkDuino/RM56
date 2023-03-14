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

#include "core/components/test/json/text_field_creator.h"

#include "base/json/json_util.h"
#include "core/components/test/json/color_creator.h"
#include "core/components/test/json/decoration_creator.h"
#include "core/components/test/json/text_style_creator.h"
#include "core/components/text_field/text_field_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> TextFieldCreator::CreateFromJson(const JsonValue& json, const JsonComponentFactory&)
{
    LOGD("CreateFromJson TextField");
    std::string classType = json.GetValue(CLASS_NAME)->GetString();
    if (classType != TEXT_FIELD_NAME) {
        LOGE("Create TextField err: not a textField json.");
        return nullptr;
    }

    auto textField = AceType::MakeRefPtr<TextFieldComponent>();

    int32_t align = json.GetInt(TEXT_ALIGN, static_cast<int32_t>(TextAlign::START));
    textField->SetTextAlign(static_cast<TextAlign>(align));

    int32_t direction = json.GetInt(TEXT_OVERFLOW, static_cast<int32_t>(TextDirection::INHERIT));
    textField->SetTextDirection(static_cast<TextDirection>(direction));

    textField->SetObscure(json.GetBool(OBSCURE_TEXT, false));
    textField->SetEnabled(json.GetBool(TEXT_FIELD_ENABLED, true));
    textField->SetAutoFocus(json.GetBool(TEXT_FIELD_AUTO_FOCUS, false));
    textField->SetTextMaxLines(json.GetUInt(TEXT_FIELD_MAX_LINES, 1));
    const uint32_t DEFAULT_LENGTH_LIMIT = std::numeric_limits<uint32_t>::max();
    uint32_t maxLength = json.GetUInt(TEXT_FIELD_MAX_LENGTH, DEFAULT_LENGTH_LIMIT);
    if (maxLength != DEFAULT_LENGTH_LIMIT) {
        textField->SetMaxLength(maxLength);
    }
    textField->SetTextInputType(
        CastToTextInputType(json.GetInt(TEXT_FIELD_TYPE, static_cast<int32_t>(TextInputType::TEXT))));
    textField->SetAction(
        CastToTextInputAction(json.GetInt(TEXT_FIELD_ACTION, static_cast<int32_t>(TextInputAction::UNSPECIFIED))));
    textField->SetActionLabel(json.GetString(TEXT_FIELD_ACTION_LABEL));

    auto temp = json.GetValue(TEXT_STYLE);
    if (temp && temp->IsObject()) {
        textField->SetTextStyle(TextStyleCreator::CreateFromJson(*temp));
    }

    temp = json.GetValue(DECORATION_NAME);
    if (temp && temp->IsObject()) {
        textField->SetDecoration(DecorationCreator::CreateFromJson(*temp));
    }

    temp = json.GetValue(CURSOR_COLOR);
    if (temp && temp->IsObject()) {
        textField->SetCursorColor(ColorCreator::CreateFromJson(*temp));
    }

    textField->SetPlaceholder(json.GetString(TEXT_FIELD_PLACEHOLDER));
    temp = json.GetValue(TEXT_FIELD_PLACEHOLDER_COLOR);
    if (temp && temp->IsObject()) {
        textField->SetPlaceholderColor(ColorCreator::CreateFromJson(*temp));
    }

    textField->SetExtend(json.GetBool(TEXT_FIELD_EXTEND, false));
    textField->SetIconImage(json.GetString(TEXT_FIELD_ICON_IMAGE));

    textField->SetOnTextChange(EventMarker { json.GetString(TEXT_FIELD_ON_TEXT_CHANGE) });
    textField->SetOnFinishInput(EventMarker { json.GetString(TEXT_FIELD_ON_FINISH_INPUT) });
    textField->SetOnTap(EventMarker { json.GetString(TEXT_FIELD_ON_TAP) });
    return textField;
}

} // namespace OHOS::Ace
