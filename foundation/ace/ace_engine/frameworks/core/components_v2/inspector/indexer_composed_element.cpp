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

#include "core/components_v2/inspector/indexer_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/text/text_element.h"
#include "core/components_v2/indexer/indexer_element.h"
#include "core/components_v2/indexer/render_indexer_item.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {
const std::unordered_map<std::string, std::function<std::string(const IndexerComposedElement&)>> CREATE_JSON_MAP {
    { "selectedColor", [](const IndexerComposedElement& inspector) { return inspector.GetSelectedColor(); } },
    { "popupColor", [](const IndexerComposedElement& inspector) { return inspector.GetPopupColor(); } },
    { "selectedBackgroundColor",
        [](const IndexerComposedElement& inspector) { return inspector.GetSelectedBackgroundColor(); } },
    { "popupBackground", [](const IndexerComposedElement& inspector) { return inspector.GetPopupBackground(); } },
    { "itemSize", [](const IndexerComposedElement& inspector) { return inspector.GetItemSize(); } },
    { "alignStyle", [](const IndexerComposedElement& inspector) { return inspector.GetAlignStyle(); } },
    { "selectedFont", [](const IndexerComposedElement& inspector) { return inspector.GetSelectedFont(); } },
    { "popupFont", [](const IndexerComposedElement& inspector) { return inspector.GetPopupFont(); } },
    { "font", [](const IndexerComposedElement& inspector) { return inspector.GetFont(); } },
    { "selected", [](const IndexerComposedElement& inspector) { return inspector.GetSelected(); } },
    { "arrayValue", [](const IndexerComposedElement& inspector) { return inspector.GetArrayValue(); } }
};

const std::unordered_map<std::string, std::function<bool(const IndexerComposedElement&)>> CREATE_JSON_BOOL_MAP {
    { "usingPopup", [](const IndexerComposedElement& inspector) { return inspector.GetUsingPopup(); } },
};
}; // namespace

void IndexerComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    for (const auto& value : CREATE_JSON_MAP) {
        DumpLog::GetInstance().AddDesc(std::string(value.first + ":").append(value.second(*this)));
    }
}

std::unique_ptr<JsonValue> IndexerComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    for (const auto& value : CREATE_JSON_BOOL_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }
    return resultJson;
}

std::string IndexerComposedElement::GetSelectedColor() const
{
    auto node = GetInspectorNode(IndexerElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderIndexer>(node)->GetSpecificItem(0);
    if (!render) {
        return "";
    }
    auto selectedColor = render ? render->GetActiveTextStyle().GetTextColor() : Color::BLACK;
    return selectedColor.ColorToString();
}

std::string IndexerComposedElement::GetPopupColor() const
{
    auto node = GetInspectorNode(IndexerElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderIndexer>(node);
    auto popupColor = render ? render->GetBubbleText()->GetTextStyle().GetTextColor() : Color::BLACK;
    return popupColor.ColorToString();
}

std::string IndexerComposedElement::GetSelectedBackgroundColor() const
{
    auto node = GetInspectorNode(IndexerElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderIndexer>(node)->GetSpecificItem(0);
    auto selectedBackgroundColor = render ? render->GetSelectedBackgroundColor() : Color::BLACK;
    return selectedBackgroundColor.ColorToString();
}

std::string IndexerComposedElement::GetPopupBackground() const
{
    auto node = GetInspectorNode(IndexerElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderIndexer>(node);
    auto popupBackground = render ? render->GetBubbleBackgroundColor() : Color::BLACK;
    return popupBackground.ColorToString();
}

std::string IndexerComposedElement::GetItemSize() const
{
    auto node = GetInspectorNode(IndexerElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderIndexer>(node)->GetSpecificItem(0);
    auto size = render ? render->GetItemSize().Value() : 0;
    return std::to_string(size);
}

std::string IndexerComposedElement::GetAlignStyle() const
{
    auto node = GetInspectorNode(IndexerElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderIndexer>(node);
    auto style = render ? render->GetAlignStyle() : AlignStyle::RIGHT;
    return ConvertAlignStyleToString(style);
}

std::string IndexerComposedElement::GetSelectedFont() const
{
    auto node = GetInspectorNode(IndexerElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderIndexer>(node)->GetSpecificItem(0);
    auto selectedFontJson = JsonUtil::Create(true);
    auto fontSize = render ? render->GetActiveTextStyle().GetFontSize() : Dimension();
    auto fontStyle = render ? render->GetActiveTextStyle().GetFontStyle() : FontStyle::NORMAL;
    auto fontWeight = render ? render->GetActiveTextStyle().GetFontWeight() : FontWeight::NORMAL;
    auto fontFamily = render ? render->GetActiveTextStyle().GetFontFamilies() : std::vector<std::string>();
    selectedFontJson->Put("size", fontSize.Value());
    selectedFontJson->Put("weight", ConvertWrapFontWeightToStirng(fontWeight).c_str());
    selectedFontJson->Put("family", ConvertFontFamily(fontFamily).c_str());
    selectedFontJson->Put("style", ConvertWrapFontStyleToStirng(fontStyle).c_str());
    return selectedFontJson->ToString();
}

std::string IndexerComposedElement::GetPopupFont() const
{
    auto node = GetInspectorNode(IndexerElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderIndexer>(node);
    auto popupFontJson = JsonUtil::Create(true);
    auto fontSize = render ? render->GetBubbleText()->GetTextStyle().GetFontSize() : Dimension();
    auto fontStyle = render ? render->GetBubbleText()->GetTextStyle().GetFontStyle() : FontStyle::NORMAL;
    auto fontWeight = render ? render->GetBubbleText()->GetTextStyle().GetFontWeight() : FontWeight::NORMAL;
    auto fontFamily = render ? render->GetBubbleText()->GetTextStyle().GetFontFamilies() : std::vector<std::string>();
    popupFontJson->Put("size", fontSize.Value());
    popupFontJson->Put("weight", ConvertWrapFontWeightToStirng(fontWeight).c_str());
    popupFontJson->Put("family", ConvertFontFamily(fontFamily).c_str());
    popupFontJson->Put("style", ConvertWrapFontStyleToStirng(fontStyle).c_str());
    return popupFontJson->ToString();
}

std::string IndexerComposedElement::GetFont() const
{
    auto node = GetInspectorNode(IndexerElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderIndexer>(node)->GetSpecificItem(0);
    auto fontJson = JsonUtil::Create(true);
    auto fontSize = render ? render->GetNormalTextStyle().GetFontSize() : Dimension();
    auto fontStyle = render ? render->GetNormalTextStyle().GetFontStyle() : FontStyle::NORMAL;
    auto fontWeight = render ? render->GetNormalTextStyle().GetFontWeight() : FontWeight::NORMAL;
    auto fontFamily = render ? render->GetNormalTextStyle().GetFontFamilies() : std::vector<std::string>();
    fontJson->Put("size", fontSize.Value());
    fontJson->Put("weight", ConvertWrapFontWeightToStirng(fontWeight).c_str());
    fontJson->Put("family", ConvertFontFamily(fontFamily).c_str());
    fontJson->Put("style", ConvertWrapFontStyleToStirng(fontStyle).c_str());
    return fontJson->ToString();
}

std::string IndexerComposedElement::GetSelected() const
{
    auto node = GetInspectorNode(IndexerElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderIndexer>(node);
    auto selected = render->GetFocusIndex();
    return std::to_string(selected);
}

std::string IndexerComposedElement::GetArrayValue() const
{
    auto node = GetInspectorNode(IndexerElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderIndexer>(node);
    auto jsonValueArray = JsonUtil::CreateArray(true);
    auto value = render->GetArrayValue();
    uint32_t length = value.size();
    if (length <= 0) {
        return "";
    }
    for (uint32_t i = 0; i < length; i++) {
        auto index = std::to_string(i);
        jsonValueArray->Put(index.c_str(), value[i].c_str());
    }
    return jsonValueArray->ToString();
}

bool IndexerComposedElement::GetUsingPopup() const
{
    auto node = GetInspectorNode(IndexerElement::TypeId());
    if (!node) {
        return false;
    }
    auto render = AceType::DynamicCast<RenderIndexer>(node);
    auto usingPopup = render ? render->GetBubbleEnabled() : false;
    return usingPopup;
}

std::string IndexerComposedElement::ConvertAlignStyleToString(AlignStyle alignStyle) const
{
    std::string result = "";
    switch (alignStyle) {
        case AlignStyle::RIGHT:
            result = "AlignStyle.Right";
            break;
        case AlignStyle::LEFT:
            result = "AlignStyle.Left";
            break;
        default:
            LOGD("input do not match any AlignStyle");
    }
    return result;
}

std::string IndexerComposedElement::ConvertFontFamily(const std::vector<std::string>& fontFamily) const
{
    std::string result = "";
    for (const auto& item : fontFamily) {
        result += item;
        result += ",";
    }
    result = result.substr(0, result.size() - 1);
    return result;
}
} // namespace OHOS::Ace::V2