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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SPAN_SPAN_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SPAN_SPAN_DECLARATION_H

#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {

struct SpanAttribute : Attribute {
    std::string spanData;
    bool isShow = true;
};

struct SpanStyle : Style {
    TextStyle spanStyle;
};

class ACE_EXPORT SpanDeclaration : public Declaration {
    DECLARE_ACE_TYPE(SpanDeclaration, Declaration);

public:
    SpanDeclaration() = default;
    ~SpanDeclaration() override = default;

    const std::string& GetSpanData() const
    {
        auto& attribute = static_cast<SpanAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.spanData;
    }

    void SetSpanData(const std::string& spanData)
    {
        auto& attribute = MaybeResetAttribute<SpanAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.spanData = spanData;
    }

    const TextStyle& GetSpanStyle()
    {
        auto& style = static_cast<SpanStyle&>(GetStyle(StyleTag::SPECIALIZED_STYLE));
        return style.spanStyle;
    }

    void SetTextStyle(const TextStyle& spanStyle)
    {
        auto& style = MaybeResetStyle<SpanStyle>(StyleTag::SPECIALIZED_STYLE);
        style.spanStyle = spanStyle;
    }

    bool IsShow() const
    {
        auto& attribute = static_cast<SpanAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.isShow;
    }

    void SetIsShow(bool isShow)
    {
        auto& attribute = MaybeResetAttribute<SpanAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.isShow = isShow;
    }

    void SetHasSetFontStyle(bool isSetFontStyle)
    {
        isSetFontStyle_ = isSetFontStyle;
    }
    bool HasSetFontStyle() const
    {
        return isSetFontStyle_;
    }

    void SetHasSetFontColor(bool isSetFontColor)
    {
        isSetFontColor_ = isSetFontColor;
    }
    bool HasSetFontColor() const
    {
        return isSetFontColor_;
    }

    void SetHasSetFontWeight(bool isSetFontWeight)
    {
        isSetFontWeight_ = isSetFontWeight;
    }
    bool HasSetFontWeight() const
    {
        return isSetFontWeight_;
    }

    void SetHasSetFontSize(bool isSetFontSize)
    {
        isSetFontSize_ = isSetFontSize;
    }
    bool HasSetFontSize() const
    {
        return isSetFontSize_;
    }

    void SetHasSetFontFamily(bool isSetFontFamily)
    {
        isSetFontFamily_ = isSetFontFamily;
    }
    bool HasSetFontFamily() const
    {
        return isSetFontFamily_;
    }

    void SetHasSetTextDecoration(bool isSetTextDecoration)
    {
        isSetTextDecoration_ = isSetTextDecoration;
    }
    bool HasSetTextDecoration() const
    {
        return isSetTextDecoration_;
    }

    void SetHasSetAllowScale(bool isSetAllowScale)
    {
        isSetAllowScale_ = isSetAllowScale;
    }
    bool HasSetAllowScale() const
    {
        return isSetAllowScale_;
    }

    void SetHasSetFontFeatures(bool isSetFontFeatures)
    {
        isSetFontFeatures_ = isSetFontFeatures;
    }
    bool HasSetFontFeatures() const
    {
        return isSetFontFeatures_;
    }

    void SetHasSetLetterSpacing(bool isSetLetterSpacing)
    {
        isSetLetterSpacing_ = isSetLetterSpacing;
    }
    bool HasSetLetterSpacing() const
    {
        return isSetLetterSpacing_;
    }

    void SetHasSetTextCase(bool isSetTextCase)
    {
        isSetTextCase_ = isSetTextCase;
    }
    bool HasSetTextCase() const
    {
        return isSetTextCase_;
    }

protected:
    void InitCommonEvent() override;
    void InitCommonAttribute() override {}
    void InitCommonStyle() override {}
    void InitCommonMethod() override {}
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;

    bool isSetFontStyle_ = false;
    bool isSetFontColor_ = false;
    bool isSetFontWeight_ = false;
    bool isSetFontSize_ = false;
    bool isSetFontFamily_ = false;
    bool isSetTextDecoration_ = false;
    bool isSetAllowScale_ = false;
    bool isSetFontFeatures_ = false;
    bool isSetLetterSpacing_ = false;
    bool isSetTextCase_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SPAN_SPAN_DECLARATION_H
