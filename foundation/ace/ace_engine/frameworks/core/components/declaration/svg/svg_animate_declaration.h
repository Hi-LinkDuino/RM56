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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_ANIMATE_DECLARATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_ANIMATE_DECLARATION_H

#include "core/components/common/properties/svg_paint_state.h"
#include "core/components/declaration/common/declaration.h"
#include "frameworks/bridge/common/dom/dom_type.h"

namespace OHOS::Ace {

struct SvgAnimateAttribute : Attribute {
    std::string attributeName;
    int32_t begin = 0;
    int32_t dur = 0;
    int32_t end = 0;
    int32_t repeatCount = 1;
    std::string fillMode;
    std::string calcMode;
    std::vector<std::string> values;
    std::vector<double> keyTimes;
    std::vector<std::string> keySplines;
    std::string from;
    std::string to;
    std::vector<std::string> keyPoints;
    std::string path;
    std::string rotate;
    std::string transformType;
};

class SvgAnimateDeclaration : public Declaration {
    DECLARE_ACE_TYPE(SvgAnimateDeclaration, Declaration);

public:
    SvgAnimateDeclaration() = default;
    ~SvgAnimateDeclaration() override = default;

    void SetAttributeName(const std::string& attributeName)
    {
        auto& attribute = MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.attributeName = attributeName;
    }

    const std::string& GetAttributeName() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.attributeName;
    }

    void SetBegin(int32_t begin)
    {
        auto& attribute = MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.begin = begin;
    }

    int32_t GetBegin() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.begin;
    }

    void SetDur(int32_t dur)
    {
        auto& attribute = MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.dur = dur;
    }

    int32_t GetDur() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.dur;
    }

    void SetEnd(int32_t end)
    {
        auto& attribute = MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.end = end;
    }

    int32_t GetEnd() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.end;
    }

    void SetRepeatCount(int32_t repeatCount)
    {
        auto& attribute = MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.repeatCount = repeatCount;
    }

    int32_t GetRepeatCount() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.repeatCount;
    }

    void SetFillMode(const std::string& fillMode)
    {
        auto& attribute = MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.fillMode = fillMode;
    }

    const std::string& GetFillMode() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.fillMode;
    }

    void SetCalcMode(const std::string& calcMode)
    {
        auto& attribute = MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.calcMode = calcMode;
    }

    const std::string& GetCalcMode() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.calcMode;
    }

    const std::vector<std::string>& GetValues() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.values;
    }

    const std::vector<double>& GetKeyTimes() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.keyTimes;
    }

    const std::vector<std::string>& GetKeySplines() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.keySplines;
    }

    const std::string& GetFrom() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.from;
    }

    void SetFrom(const std::string& from)
    {
        auto& attribute = MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.from = from;
    }

    const std::string& GetTo() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.to;
    }

    void SetTo(const std::string& to)
    {
        auto& attribute = MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.to = to;
    }

    const std::string& GetPath() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.path;
    }

    void SetPath(const std::string& path)
    {
        auto& attribute = MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.path = path;
    }

    const std::string& GetRotate() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.rotate;
    }

    void SetRotate(const std::string& rotate)
    {
        auto& attribute = MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.rotate = rotate;
    }

    const std::vector<std::string>& GetKeyPoints() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.keyPoints;
    }

    const std::string& GetTransformType() const
    {
        auto& attribute = static_cast<SvgAnimateAttribute&>(GetAttribute(AttributeTag::SPECIALIZED_ATTR));
        return attribute.transformType;
    }

    void SetTransformType(const std::string& transformType)
    {
        auto& attribute = MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
        attribute.transformType = transformType;
    }

protected:
    void InitSpecialized() override;
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetAnimateAttr(const std::pair<std::string, std::string>& attr);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DECLARATION_SVG_SVG_ANIMATE_DECLARATION_H
