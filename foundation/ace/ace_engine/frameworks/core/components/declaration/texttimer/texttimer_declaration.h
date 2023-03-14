/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_TEXTTIMER_TEXTTIMER_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_TEXTTIMER_TEXTTIMER_DECLARATION_H

#include "core/components/common/properties/text_style.h"
#include "core/components/declaration/common/declaration.h"
#include "core/components/text/text_component.h"
#include "core/components/texttimer/texttimer_controller.h"

namespace OHOS::Ace {

inline constexpr double DEFAULT_COUNT = 60000.0;
inline const char DEFAULT_FORMAT[] = "HH:mm:ss.S";

struct TextTimerAttribute : Attribute {
    double inputCount = DEFAULT_COUNT;
    bool isCountDown_ = false;
    std::string format_ = DEFAULT_FORMAT;
};

struct TextTimerStyle : Style {
    TextStyle textStyle;
};

struct TextTimerEvent : Event {};

struct TextTimerMethod : Method {};

class TextTimerDeclaration : public Declaration {
    DECLARE_ACE_TYPE(TextTimerDeclaration, Declaration);

public:
    TextTimerDeclaration();
    ~TextTimerDeclaration() override = default;

    const RefPtr<TextTimerController>& GetTextTimerController() const
    {
        return textTimerController_;
    }

    double GetInputCount() const
    {
        auto& attribute = static_cast<TextTimerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.inputCount;
    }

    void SetInputCount(double value)
    {
        auto& attribute = MaybeResetAttribute<TextTimerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.inputCount = value;
    }

    bool GetIsCountDown() const
    {
        auto& attribute = static_cast<TextTimerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.isCountDown_;
    }

    void SetIsCountDown(bool value)
    {
        auto& attribute = MaybeResetAttribute<TextTimerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.isCountDown_ = value;
    }

    const std::string& GetFormat() const
    {
        auto& attribute = static_cast<TextTimerAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.format_;
    }

    void SetFormat(const std::string& value)
    {
        auto& attribute = MaybeResetAttribute<TextTimerAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.format_ = value;
    }

    void SetTextStyle(const TextStyle& textstyle)
    {
        auto& style = MaybeResetStyle<TextTimerStyle>(StyleTag::SPECIALIZED_STYLE);
        style.textStyle = textstyle;
    }

    const TextStyle& GetTextStyle() const
    {
        auto& style = static_cast<TextTimerStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.textStyle;
    }

protected:
    void InitSpecialized() override;

private:
    RefPtr<TextTimerController> textTimerController_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_TEXTTIMER_TEXTTIMER_DECLARATION_H
