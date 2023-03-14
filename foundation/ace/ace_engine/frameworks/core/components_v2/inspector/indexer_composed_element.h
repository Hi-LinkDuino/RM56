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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_INDEXER_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_INDEXER_COMPOSED_ELEMENT_H

#include "core/components_v2/indexer/render_indexer.h"
#include "core/components_v2/indexer/render_indexer_item.h"
#include "core/components_v2/indexer/indexer_element.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {
class ACE_EXPORT IndexerComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(IndexerComposedElement, InspectorComposedElement)

public:
    explicit IndexerComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~IndexerComposedElement() override = default;

    void Dump() override;
    std::unique_ptr<JsonValue> ToJsonObject() const override;

    std::string GetSelectedColor() const;
    std::string GetPopupColor() const;
    std::string GetSelectedBackgroundColor() const;
    std::string GetItemSize() const;
    std::string GetAlignStyle() const;
    std::string GetSelectedFont() const;
    std::string GetPopupFont() const;
    std::string GetFont() const;
    std::string ConvertAlignStyleToString(AlignStyle alignStyle) const;
    std::string GetPopupBackground() const;
    std::string GetSelected() const;
    std::string GetArrayValue() const;
    bool GetUsingPopup() const;

    AceType::IdType GetTargetTypeId() const override
    {
        return IndexerElement::TypeId();
    }

    std::string ConvertFontFamily(const std::vector<std::string>& fontFamily) const;
};
} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_INDEXER_COMPOSED_ELEMENT_H
