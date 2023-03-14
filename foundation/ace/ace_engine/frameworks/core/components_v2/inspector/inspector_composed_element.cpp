/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components_v2/inspector/inspector_composed_element.h"

#include <atomic>

#include "base/log/dump_log.h"
#include "base/utils/system_properties.h"
#include "core/common/ace_application_info.h"
#include "core/components/box/box_element.h"
#include "core/components/common/layout/constants.h"
#include "core/components/coverage/render_coverage.h"
#include "core/components/display/render_display.h"
#include "core/components/flex/flex_item_element.h"
#include "core/components/flex/render_flex_item.h"
#include "core/components/focusable/focusable_element.h"
#include "core/components/popup/popup_element_v2.h"
#include "core/components/text/render_text.h"
#include "core/components/text/text_element.h"
#include "core/components/transform/transform_element.h"
#include "core/components_v2/inspector/inspector_composed_component.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/components_v2/inspector/utils.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/composed_component.h"

namespace OHOS::Ace::V2 {

namespace {

std::atomic<int32_t> g_currentInspectorId(0);

int32_t GetCurrentInspectorId()
{
    return g_currentInspectorId.fetch_add(1, std::memory_order_relaxed) + 1;
}

constexpr uint32_t WINDOW_BLUR_STYLE_ENUM_OFFSET = 100;

const char* VISIBLE_TYPE[] = { "Visibility.Visible", "Visibility.Hidden", "Visibility.None" };

const char* ITEM_ALIGN[] = { "ItemAlign.Auto", "ItemAlign.Start", "ItemAlign.Center", "ItemAlign.End",
    "ItemAlign.Stretch", "ItemAlign.Baseline" };

// NONE translate to Solid
const char* BORDER_STYLE[] = {
    "BorderStyle.Solid",
    "BorderStyle.Dashed",
    "BorderStyle.Dotted",
    "BorderStyle.Solid",
};

const char* WINDOW_BLUR_STYLE[] = { "BlurStyle.SmallLight", "BlurStyle.MediumLight", "BlurStyle.LargeLight",
    "BlurStyle.XlargeLight", "BlurStyle.SmallDark", "BlurStyle.MediumDark", "BlurStyle.LargeDark",
    "BlurStyle.XlargeDark" };

const char* ALIGNMENT_TYPE[3][3] = { { "Alignment.TopStart", "Alignment.Start", "Alignment.BottomStart" },
    { "Alignment.Top", "Alignment.Center", "Alignment.Bottom" },
    { "Alignment.TopEnd", "Alignment.End", "Alignment.BottomEnd" } };

const char* GRID_SIZE_TYPE[] = { "default", "sx", "sm", "md", "lg" };

constexpr const char* TEXT_DIRECTION[] = { "Direction.Ltr", "Direction.Rtl", "Direction.Inherit", "Direction.Auto" };

constexpr const char* BASIC_SHAPE_TYPE[] { "None", "Inset", "Circle", "Ellipse", "Polygon", "Path", "Rect" };

const std::unordered_map<std::string, DoubleJsonFunc> CREATE_JSON_DOUBLE_MAP {
    { "opacity", [](const InspectorNode& inspector) { return inspector.GetOpacity(); } },
    { "flexGrow", [](const InspectorNode& inspector) { return inspector.GetFlexGrow(); } },
    { "flexShrink", [](const InspectorNode& inspector) { return inspector.GetFlexShrink(); } },
    { "gridOffset", [](const InspectorNode& inspector) { return inspector.GetGridOffset(); } },
    { "blur", [](const InspectorNode& inspector) { return inspector.GetBlur(); } },
    { "backdropBlur", [](const InspectorNode& inspector) { return inspector.GetBackDropBlur(); } },
    { "aspectRatio", [](const InspectorNode& inspector) { return inspector.GetAspectRatio(); } },
    { "brightness", [](const InspectorNode& inspector) { return inspector.GetBrightness(); } },
    { "saturate", [](const InspectorNode& inspector) { return inspector.GetSaturate(); } },
    { "contrast", [](const InspectorNode& inspector) { return inspector.GetContrast(); } },
    { "invert", [](const InspectorNode& inspector) { return inspector.GetInvert(); } },
    { "sepia", [](const InspectorNode& inspector) { return inspector.GetSepia(); } },
    { "grayscale", [](const InspectorNode& inspector) { return inspector.GetGrayScale(); } },
    { "hueRotate", [](const InspectorNode& inspector) { return inspector.GetHueRotate(); } },
};

const std::unordered_map<std::string, StringJsonFunc> CREATE_JSON_STRING_MAP {
    { "visibility", [](const InspectorNode& inspector) { return inspector.GetVisibility(); } },
    { "alignSelf", [](const InspectorNode& inspector) { return inspector.GetAlignSelf(); } },
    { "clip", [](const InspectorNode& inspector) { return inspector.GetClip(); } },
    { "constraintSize", [](const InspectorNode& inspector) { return inspector.GetConstraintSize(); } },
    { "borderColor", [](const InspectorNode& inspector) { return inspector.GetBorderColor(); } },
    { "borderStyle", [](const InspectorNode& inspector) { return inspector.GetBorderStyle(); } },
    { "borderWidth", [](const InspectorNode& inspector) { return inspector.GetBorderWidth(); } },
    { "borderRadius", [](const InspectorNode& inspector) { return inspector.GetBorderRadius(); } },
    { "backgroundImage", [](const InspectorNode& inspector) { return inspector.GetBackgroundImage(); } },
    { "backgroundColor", [](const InspectorNode& inspector) { return inspector.GetBackgroundColor(); } },
    { "flexBasis", [](const InspectorNode& inspector) { return inspector.GetFlexBasis(); } },
    { "width", [](const InspectorNode& inspector) { return inspector.GetWidth(); } },
    { "height", [](const InspectorNode& inspector) { return inspector.GetHeight(); } },
    { "align", [](const InspectorNode& inspector) { return inspector.GetAlign(); } },
    { "direction", [](const InspectorNode& inspector) { return inspector.GetDirectionStr(); } },
    { "bindPopup", [](const InspectorNode& inspector) { return inspector.GetBindPopup(); } },
    { "bindContextMenu", [](const InspectorNode& inspector) { return inspector.GetBindContextMenu(); } }
};

const std::unordered_map<std::string, BoolJsonFunc> CREATE_JSON_BOOL_MAP {
    { "enabled", [](const InspectorNode& inspector) { return inspector.GetEnabled(); } },
    { "clickable", [](const InspectorNode& inspector) { return inspector.GetClickable(); } },
    { "checkable", [](const InspectorNode& inspector) { return inspector.GetCheckable(); } },
    { "focusable", [](const InspectorNode& inspector) { return inspector.GetFocusable(); } },
    { "scrollable", [](const InspectorNode& inspector) { return inspector.GetScrollable(); } },
    { "long-clickable", [](const InspectorNode& inspector) { return inspector.GetLongClickable(); } },
    { "selected", [](const InspectorNode& inspector) { return inspector.IsSelected(); } },
    { "password", [](const InspectorNode& inspector) { return inspector.IsPassword(); } },
    { "checked", [](const InspectorNode& inspector) { return inspector.IsChecked(); } },
    { "focused", [](const InspectorNode& inspector) { return inspector.IsFocused(); } },
};

const std::unordered_map<std::string, IntJsonFunc> CREATE_JSON_INT_MAP {
    { "zIndex", [](const InspectorNode& inspector) { return inspector.GetZIndex(); } },
    { "gridSpan", [](const InspectorNode& inspector) { return inspector.GetGridSpan(); } },
    { "layoutPriority", [](const InspectorNode& inspector) { return inspector.GetLayoutPriority(); } },
    { "layoutWeight", [](const InspectorNode& inspector) { return inspector.GetLayoutWeight(); } },
    { "displayPriority", [](const InspectorNode& inspector) { return inspector.GetDisplayPriority(); } },
};

const std::unordered_map<std::string, JsonValueJsonFunc> CREATE_JSON_JSON_VALUE_MAP {
    { "windowBlur", [](const InspectorNode& inspector) { return inspector.GetWindowBlur(); } },
    { "shadow", [](const InspectorNode& inspector) { return inspector.GetShadow(); } },
    { "position", [](const InspectorNode& inspector) { return inspector.GetPosition(); } },
    { "offset", [](const InspectorNode& inspector) { return inspector.GetOffset(); } },
    { "padding", [](const InspectorNode& inspector) { return inspector.GetPadding(); } },
    { "margin", [](const InspectorNode& inspector) { return inspector.GetAllMargin(); } },
    { "size", [](const InspectorNode& inspector) { return inspector.GetSize(); } },
    { "backgroundImageSize", [](const InspectorNode& inspector) { return inspector.GetBackgroundImageSize(); } },
    { "backgroundImagePosition",
        [](const InspectorNode& inspector) { return inspector.GetBackgroundImagePosition(); } },
    { "useSizeType", [](const InspectorNode& inspector) { return inspector.GetUseSizeType(); } },
    { "rotate", [](const InspectorNode& inspector) { return inspector.GetRotate(); } },
    { "scale", [](const InspectorNode& inspector) { return inspector.GetScale(); } },
    { "transform", [](const InspectorNode& inspector) { return inspector.GetTransform(); } },
    { "translate", [](const InspectorNode& inspector) { return inspector.GetTranslate(); } },
    { "markAnchor", [](const InspectorNode& inspector) { return inspector.GetMarkAnchor(); } },
    { "mask", [](const InspectorNode& inspector) { return inspector.GetMask(); } },
    { "useAlign", [](const InspectorNode& inspector) { return inspector.GetUseAlign(); } },
    { "overlay", [](const InspectorNode& inspector) { return inspector.GetOverlay(); } },
    { "border", [](const InspectorNode& inspector) { return inspector.GetUnifyBorder(); } },
    { "linearGradient", [](const InspectorNode& inspector) { return inspector.GetLinearGradient(); } },
    { "sweepGradient", [](const InspectorNode& inspector) { return inspector.GetSweepGradient(); } },
    { "radialGradient", [](const InspectorNode& inspector) { return inspector.GetRadialGradient(); } },
};

}; // namespace

InspectorComposedElement::InspectorComposedElement(const ComposeId& id) : ComposedElement(id) {}

InspectorComposedElement::~InspectorComposedElement()
{
    if (inspectorId_ == -1) {
        return;
    }
    accessibilityNode_.Reset();
    RemoveInspectorNode(inspectorId_);
}

void InspectorComposedElement::OnInactive()
{
    accessibilityNode_.Reset();
    RemoveInspectorNode(inspectorId_);
    inspectorId_ = -1;
}

void InspectorComposedElement::OnActive()
{
    inspectorId_ = GetCurrentInspectorId();
}

RefPtr<Element> InspectorComposedElement::GetElementChildBySlot(const RefPtr<Element>& element, int32_t& slot) const
{
    if (!element) {
        return nullptr;
    }
    auto child = element->GetChildBySlot(slot);
    if (!child) {
        slot = DEFAULT_ELEMENT_SLOT;
        child = element->GetChildBySlot(slot);
    }
    return child;
}

RefPtr<Element> InspectorComposedElement::GetInspectorComposedElementParent(const RefPtr<Element>& element) const
{
    if (!element) {
        return nullptr;
    }
    for (const auto& child : element->GetChildren()) {
        auto inspectorComposedElement = AceType::DynamicCast<InspectorComposedElement>(child);
        if (inspectorComposedElement) {
            return element;
        }
        auto element_ = GetInspectorComposedElementParent(child);
        if (element_) {
            return element_;
        }
    }
    return nullptr;
}

std::unique_ptr<JsonValue> InspectorComposedElement::ToJsonObject() const
{
    auto resultJson = JsonUtil::Create(true);
    for (const auto& value : CREATE_JSON_DOUBLE_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }
    for (const auto& value : CREATE_JSON_STRING_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    for (const auto& value : CREATE_JSON_BOOL_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }
    for (const auto& value : CREATE_JSON_INT_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }

    for (const auto& value : CREATE_JSON_JSON_VALUE_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }
    return resultJson;
}

void InspectorComposedElement::Prepare(const WeakPtr<Element>& weakParent)
{
    accessibilityEnabled_ = false;
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    accessibilityEnabled_ = true;
#else
    if (AceApplicationInfo::GetInstance().IsAccessibilityEnabled() || SystemProperties::GetAccessibilityEnabled()) {
        accessibilityEnabled_ = true;
    }
#endif
    if (accessibilityEnabled_) {
        auto parent = weakParent.Upgrade();
        RefPtr<InspectorComposedElement> inspectorParent;
        while (parent) {
            inspectorParent = DynamicCast<InspectorComposedElement>(parent);
            if (inspectorParent) {
                break;
            }
            parent = parent->GetElementParent().Upgrade();
        }
        if (inspectorParent) {
            inspectorParentId_ = inspectorParent->inspectorId_;
        }
    }
    AddComposedComponentId();
}

void InspectorComposedElement::Update()
{
    ComposedElement::Update();
    auto component = DynamicCast<V2::InspectorComposedComponent>(component_);
    if (!component) {
        LOGE("fail to update due to component is null");
        return;
    }
    auto inspectorFunctionImpl = component->GetInspectorFunctionImpl();
    inspectorFunctionImpl->SetUpdateEventInfoImpl([weak = WeakClaim(this)](BaseEventInfo& info) {
        auto composedElement = weak.Upgrade();
        if (composedElement) {
            composedElement->UpdateEventTarget(info);
        }
    });
    if (accessibilityNode_) {
        accessibilityNode_->SetAccessible(component->IsAccessibilityGroup());
        accessibilityNode_->SetAccessibilityLabel(component->GetAccessibilitytext());
        accessibilityNode_->SetAccessibilityHint(component->GetAccessibilityDescription());
        accessibilityNode_->SetImportantForAccessibility(component->GetAccessibilityImportance());
        accessibilityNode_->SetFocusChangeEventMarker(component->GetAccessibilityEvent());
    }
}

bool InspectorComposedElement::CanUpdate(const RefPtr<Component>& newComponent)
{
    return Element::CanUpdate(newComponent);
}

void InspectorComposedElement::AddComposedComponentId()
{
    auto context = context_.Upgrade();
    if (context == nullptr) {
        LOGW("get context failed");
        return;
    }
    auto accessibilityManager = context->GetAccessibilityManager();
    if (!accessibilityManager) {
        LOGW("get AccessibilityManager failed");
        return;
    }
    accessibilityManager->AddComposedElement(std::to_string(inspectorId_), AceType::Claim(this));
    if (accessibilityEnabled_) {
        accessibilityNode_ =
            InspectorComposedComponent::CreateAccessibilityNode(inspectorTag_, inspectorId_, inspectorParentId_, -1);
        accessibilityNode_->SetJsComponentId(key_);
    }
}

void InspectorComposedElement::RemoveInspectorNode(int32_t id)
{
    auto context = context_.Upgrade();
    if (context == nullptr) {
        LOGW("get context failed");
        return;
    }
    auto accessibilityManager = context->GetAccessibilityManager();
    if (!accessibilityManager) {
        LOGW("get AccessibilityManager failed");
        return;
    }
    accessibilityManager->RemoveComposedElementById(std::to_string(id));
    if (accessibilityEnabled_) {
        accessibilityManager->RemoveAccessibilityNodeById(id);
    }
}

RefPtr<RenderNode> InspectorComposedElement::GetInspectorNode(IdType typeId, bool isForward) const
{
    if (isForward) {
        auto parent = GetElementParent().Upgrade();
        while (parent) {
            if (AceType::TypeId(parent) == typeId) {
                return parent->GetRenderNode();
            }
            parent = parent->GetElementParent().Upgrade();
        }
        return nullptr;
    }
    auto child = children_.empty() ? nullptr : children_.front();
    while (child) {
        if (AceType::TypeId(child) == typeId) {
            return child->GetRenderNode();
        }
        child = child->GetChildren().empty() ? nullptr : child->GetChildren().front();
    }
    return nullptr;
}

RefPtr<AccessibilityNode> InspectorComposedElement::GetAccessibilityNode() const
{
    auto context = context_.Upgrade();
    if (context == nullptr) {
        LOGW("get context failed");
        return nullptr;
    }
    auto accessibilityManager = context->GetAccessibilityManager();
    if (!accessibilityManager) {
        LOGW("get AccessibilityManager failed");
        return nullptr;
    }
    return accessibilityManager->GetAccessibilityNodeById(std::stoi(id_));
}

RefPtr<RenderBox> InspectorComposedElement::GetRenderBox() const
{
    auto node = GetInspectorNode(BoxElement::TypeId());
    if (!node) {
        return nullptr;
    }
    return AceType::DynamicCast<RenderBox>(node);
}

std::string InspectorComposedElement::GetWidth() const
{
    auto render = GetRenderBox();
    if (render) {
        Dimension value = render->GetWidthDimension();
        if (value.Value() == -1) {
            return "-";
        }
        return value.ToString();
    }
    return "-";
}

std::string InspectorComposedElement::GetHeight() const
{
    auto render = GetRenderBox();
    if (render) {
        Dimension value = render->GetHeightDimension();
        if (value.Value() == -1) {
            return "-";
        }
        return value.ToString();
    }
    return "-";
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetSize() const
{
    auto jsonValue = JsonUtil::Create(false);
    jsonValue->Put("width", GetWidth().c_str());
    jsonValue->Put("height", GetHeight().c_str());
    return jsonValue;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetPadding() const
{
    auto render = GetRenderBox();
    auto jsonValue = JsonUtil::Create(false);
    if (render) {
        auto top = render->GetPadding(DimensionHelper(&Edge::SetTop, &Edge::Top));
        auto right = render->GetPadding(DimensionHelper(&Edge::SetRight, &Edge::Right));
        auto bottom = render->GetPadding(DimensionHelper(&Edge::SetBottom, &Edge::Bottom));
        auto left = render->GetPadding(DimensionHelper(&Edge::SetLeft, &Edge::Left));
        if (top == right && right == bottom && bottom == left) {
            auto temp = JsonUtil::Create(false);
            temp->Put("padding", top.ToString().c_str());
            jsonValue = temp->GetValue("padding");
        } else {
            jsonValue->Put("top", top.ToString().c_str());
            jsonValue->Put("right", right.ToString().c_str());
            jsonValue->Put("bottom", bottom.ToString().c_str());
            jsonValue->Put("left", left.ToString().c_str());
        }
        return jsonValue;
    }
    return jsonValue;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetAllMargin() const
{
    auto render = GetRenderBox();
    auto jsonValue = JsonUtil::Create(false);
    if (render) {
        auto top = render->GetMargin(DimensionHelper(&Edge::SetTop, &Edge::Top));
        auto right = render->GetMargin(DimensionHelper(&Edge::SetRight, &Edge::Right));
        auto bottom = render->GetMargin(DimensionHelper(&Edge::SetBottom, &Edge::Bottom));
        auto left = render->GetMargin(DimensionHelper(&Edge::SetLeft, &Edge::Left));
        if (top == right && right == bottom && bottom == left) {
            auto temp = JsonUtil::Create(false);
            temp->Put("margin", top.ToString().c_str());
            jsonValue = temp->GetValue("margin");
        } else {
            jsonValue->Put("top", top.ToString().c_str());
            jsonValue->Put("right", right.ToString().c_str());
            jsonValue->Put("bottom", bottom.ToString().c_str());
            jsonValue->Put("left", left.ToString().c_str());
        }
        return jsonValue;
    }
    return jsonValue;
}

Dimension InspectorComposedElement::GetMargin(OHOS::Ace::AnimatableType type) const
{
    auto render = GetRenderBox();
    if (render) {
        if (type == AnimatableType::PROPERTY_MARGIN_LEFT) {
            return render->GetMargin(DimensionHelper(&Edge::SetLeft, &Edge::Left));
        } else if (type == AnimatableType::PROPERTY_MARGIN_TOP) {
            return render->GetMargin(DimensionHelper(&Edge::SetTop, &Edge::Top));
        } else if (type == AnimatableType::PROPERTY_MARGIN_RIGHT) {
            return render->GetMargin(DimensionHelper(&Edge::SetRight, &Edge::Right));
        } else if (type == AnimatableType::PROPERTY_MARGIN_BOTTOM) {
            return render->GetMargin(DimensionHelper(&Edge::SetBottom, &Edge::Bottom));
        }
    }
    return Dimension();
}

std::string InspectorComposedElement::GetConstraintSize() const
{
    LayoutParam layoutParam = LayoutParam(Size(), Size());
    auto render = GetRenderBox();
    if (render) {
        layoutParam = render->GetConstraints();
    }
    auto jsonStr = JsonUtil::Create(false);
    Dimension minWidth = Dimension(SystemProperties::Px2Vp(layoutParam.GetMinSize().Width()), DimensionUnit::VP);
    Dimension minHeight = Dimension(SystemProperties::Px2Vp(layoutParam.GetMinSize().Height()), DimensionUnit::VP);
    Dimension maxWidth = Dimension(SystemProperties::Px2Vp(layoutParam.GetMaxSize().Width()), DimensionUnit::VP);
    Dimension maxHeight = Dimension(SystemProperties::Px2Vp(layoutParam.GetMaxSize().Height()), DimensionUnit::VP);
    jsonStr->Put("minWidth", minWidth.ToString().c_str());
    jsonStr->Put("minHeight", minHeight.ToString().c_str());
    jsonStr->Put("maxWidth", maxWidth.ToString().c_str());
    jsonStr->Put("maxHeight", maxHeight.ToString().c_str());
    return jsonStr->ToString();
}

int32_t InspectorComposedElement::GetLayoutPriority() const
{
    auto render = GetRenderBox();
    if (render) {
        return render->GetDisplayIndex();
    }
    return 0;
}

int32_t InspectorComposedElement::GetLayoutWeight() const
{
    auto node = GetInspectorNode(FlexItemElement::TypeId());
    if (!node) {
        return 0;
    }
    auto render = AceType::DynamicCast<RenderFlexItem>(node);
    if (render) {
        return render->GetFlexWeight();
    }
    return 0;
}

std::string InspectorComposedElement::GetAlign() const
{
    auto render = GetRenderBox();
    if (render) {
        auto align = render->GetAlign();
        int32_t h = align.GetHorizontal() + 1;
        int32_t v = align.GetVertical() + 1;
        return ALIGNMENT_TYPE[h][v];
    }
    return ALIGNMENT_TYPE[1][1];
}

std::string InspectorComposedElement::GetDirectionStr() const
{
    auto render = GetRenderBox();
    if (!render) {
        return TEXT_DIRECTION[3];
    }
    auto value = static_cast<int32_t>(render->GetInspectorDirection());
    auto length = static_cast<int32_t>(sizeof(TEXT_DIRECTION) / sizeof(TEXT_DIRECTION[0]));
    if (value < length) {
        return TEXT_DIRECTION[value];
    }
    return TEXT_DIRECTION[3];
}

TextDirection InspectorComposedElement::GetDirection() const
{
    auto render = GetRenderBox();
    if (render) {
        return render->GetTextDirection();
    }
    return TextDirection::AUTO;
}

std::string InspectorComposedElement::GetBorderRadius() const
{
    auto value = GetBorder().TopLeftRadius().GetX().Value();
    if (value == 0.0) {
        return "0.0vp";
    }
    return GetBorder().TopLeftRadius().GetX().ToString();
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetUnifyBorder() const
{
    auto jsonValue = JsonUtil::Create(false);
    jsonValue->Put("width", GetBorderWidth().c_str());
    jsonValue->Put("color", GetBorderColor().c_str());
    jsonValue->Put("radius", GetBorderRadius().c_str());
    jsonValue->Put("style", GetBorderStyle().c_str());
    return jsonValue;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetPosition() const
{
    auto jsonValue = JsonUtil::Create(false);
    auto node = GetInspectorNode(FlexItemElement::TypeId());
    if (!node) {
        jsonValue->Put("x", "0.0px");
        jsonValue->Put("y", "0.0px");
        return jsonValue;
    }
    auto render = AceType::DynamicCast<RenderFlexItem>(node);
    if (render) {
        PositionType type = render->GetPositionType();
        if (type == PositionType::ABSOLUTE) {
            jsonValue->Put("x", render->GetLeft().ToString().c_str());
            jsonValue->Put("y", render->GetTop().ToString().c_str());
            return jsonValue;
        }
    }
    jsonValue->Put("x", "0.0px");
    jsonValue->Put("y", "0.0px");
    return jsonValue;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetMarkAnchor() const
{
    auto jsonValue = JsonUtil::Create(false);
    auto node = GetInspectorNode(FlexItemElement::TypeId());
    if (!node) {
        jsonValue->Put("x", "0.0px");
        jsonValue->Put("y", "0.0px");
        return jsonValue;
    }
    auto render = AceType::DynamicCast<RenderFlexItem>(node);
    if (render) {
        jsonValue->Put("x", render->GetAnchorX().ToString().c_str());
        jsonValue->Put("y", render->GetAnchorY().ToString().c_str());
        return jsonValue;
    }
    jsonValue->Put("x", "0.0px");
    jsonValue->Put("y", "0.0px");
    return jsonValue;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetOffset() const
{
    auto jsonValue = JsonUtil::Create(false);
    auto node = GetInspectorNode(FlexItemElement::TypeId());
    if (!node) {
        jsonValue->Put("x", "0.0px");
        jsonValue->Put("y", "0.0px");
        return jsonValue;
    }
    auto render = AceType::DynamicCast<RenderFlexItem>(node);
    if (render) {
        PositionType type = render->GetPositionType();
        if (type == PositionType::OFFSET) {
            jsonValue->Put("x", render->GetLeft().ToString().c_str());
            jsonValue->Put("y", render->GetTop().ToString().c_str());
            return jsonValue;
        }
    }
    jsonValue->Put("x", "0.0px");
    jsonValue->Put("y", "0.0px");
    return jsonValue;
}

std::string InspectorComposedElement::GetRect()
{
    std::string strRec;
    Rect rect = GetRenderRect();

    if (accessibilityNode_ && accessibilityNode_->GetParentNode()) {
        auto parent = accessibilityNode_->GetParentNode();
        if (parent->GetClipFlag()) {
            rect = rect.Constrain(parent->GetRect());
        }
    }
    if (accessibilityNode_ && GetClipFlag()) {
        accessibilityNode_->SetClipFlagToChild(true);
    }
    isRectValid_ = rect.IsValid();
    strRec = std::to_string(rect.Left())
                 .append(",")
                 .append(std::to_string(rect.Top()))
                 .append(",")
                 .append(std::to_string(rect.Width()))
                 .append(",")
                 .append(std::to_string(rect.Height()));
    return strRec;
}

Rect InspectorComposedElement::GetParentRect() const
{
    auto parent = GetElementParent().Upgrade();
    if (!parent) {
        return Rect();
    }
    Rect parentRect = parent->GetRenderRect();
    return parentRect;
}

double InspectorComposedElement::GetAspectRatio() const
{
    auto render = GetRenderBox();
    if (render) {
        return render->GetAspectRatio();
    }
    return 0.0;
}

int32_t InspectorComposedElement::GetDisplayPriority() const
{
    auto node = GetInspectorNode(FlexItemElement::TypeId());
    if (!node) {
        return 1;
    }
    auto render = AceType::DynamicCast<RenderFlexItem>(node);
    if (render) {
        return render->GetDisplayIndex();
    }
    return 1;
}

std::string InspectorComposedElement::GetFlexBasis() const
{
    auto render = AceType::DynamicCast<RenderFlexItem>(GetInspectorNode(FlexItemElement::TypeId()));
    if (render) {
        auto flexBasis = render->GetFlexBasis();
        return flexBasis.IsValid() ? render->GetFlexBasis().ToString() : "auto";
    }
    return "auto";
}

double InspectorComposedElement::GetFlexGrow() const
{
    auto render = AceType::DynamicCast<RenderFlexItem>(GetInspectorNode(FlexItemElement::TypeId()));
    if (render) {
        return render->GetFlexGrow();
    }
    return 0.0;
}

double InspectorComposedElement::GetFlexShrink() const
{
    auto render = AceType::DynamicCast<RenderFlexItem>(GetInspectorNode(FlexItemElement::TypeId()));
    if (render) {
        return render->GetFlexShrink();
    }
    return 0.0;
}

std::string InspectorComposedElement::GetAlignSelf() const
{
    auto render = AceType::DynamicCast<RenderFlexItem>(GetInspectorNode(FlexItemElement::TypeId()));
    if (render) {
        return ITEM_ALIGN[static_cast<int32_t>(render->GetAlignSelf())];
    }
    return ITEM_ALIGN[0];
}

Border InspectorComposedElement::GetBorder() const
{
    auto render = GetRenderBox();
    if (!render) {
        return Border();
    }
    auto decoration = render->GetBackDecoration();
    if (decoration) {
        return decoration->GetBorder();
    }
    return Border();
}

std::string InspectorComposedElement::GetBorderStyle() const
{
    auto border = GetBorder();
    int32_t style = static_cast<int32_t>(border.Left().GetBorderStyle());
    return BORDER_STYLE[style];
}

std::string InspectorComposedElement::GetBorderWidth() const
{
    auto border = GetBorder();
    return border.Left().GetWidth().ToString();
}

std::string InspectorComposedElement::GetBorderColor() const
{
    auto border = GetBorder();
    return border.Left().GetColor().ColorToString();
}

RefPtr<Decoration> InspectorComposedElement::GetBackDecoration() const
{
    auto render = GetRenderBox();
    if (!render) {
        return nullptr;
    }
    return render->GetBackDecoration();
}

std::string InspectorComposedElement::GetBackgroundImage() const
{
    auto backDecoration = GetBackDecoration();
    if (!backDecoration) {
        return "NONE";
    }
    auto image = backDecoration->GetImage();
    if (!image) {
        return "NONE";
    }
    auto imageRepeat = image->GetImageRepeat();
    if (imageRepeat == ImageRepeat::REPEATX) {
        return image->GetSrc() + ", ImageRepeat.X";
    } else if (imageRepeat == ImageRepeat::REPEATY) {
        return image->GetSrc() + ", ImageRepeat.Y";
    } else if (imageRepeat == ImageRepeat::REPEAT) {
        return image->GetSrc() + ", ImageRepeat.XY";
    }
    return image->GetSrc() + ", ImageRepeat.NoRepeat";
}

std::string InspectorComposedElement::GetBackgroundColor() const
{
    auto jsonValue = JsonUtil::Create(false);
    auto backDecoration = GetBackDecoration();
    if (!backDecoration) {
        return "NONE";
    }
    auto color = backDecoration->GetBackgroundColor();
    return color.ColorToString();
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetBackgroundImageSize() const
{
    auto jsonValue = JsonUtil::Create(false);
    auto backDecoration = GetBackDecoration();
    if (!backDecoration) {
        jsonValue->Put("width", "ImageSize.Auto");
        return jsonValue->GetValue("width");
    }
    auto image = backDecoration->GetImage();
    if (!image) {
        jsonValue->Put("width", "ImageSize.Auto");
        return jsonValue->GetValue("width");
    }
    auto widthType = image->GetImageSize().GetSizeTypeX();
    if (widthType == BackgroundImageSizeType::CONTAIN) {
        jsonValue->Put("width", "ImageSize.Contain");
        return jsonValue->GetValue("width");
    } else if (widthType == BackgroundImageSizeType::COVER) {
        jsonValue->Put("width", "ImageSize.Cover");
        return jsonValue->GetValue("width");
    } else if (widthType == BackgroundImageSizeType::AUTO) {
        jsonValue->Put("width", "ImageSize.Auto");
        return jsonValue->GetValue("width");
    }

    Dimension width = Dimension((image->GetImageSize().GetSizeValueX()), DimensionUnit::VP);
    Dimension height = Dimension((image->GetImageSize().GetSizeValueY()), DimensionUnit::VP);
    jsonValue->Put("width", width.ToString().c_str());
    jsonValue->Put("height", height.ToString().c_str());
    return jsonValue;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetBackgroundImagePosition() const
{
    auto jsonValue = JsonUtil::Create(false);
    auto backDecoration = GetBackDecoration();
    if (!backDecoration) {
        jsonValue->Put("x", 0.0);
        jsonValue->Put("y", 0.0);
        return jsonValue;
    }
    auto image = backDecoration->GetImage();
    if (!image) {
        jsonValue->Put("x", 0.0);
        jsonValue->Put("y", 0.0);
        return jsonValue;
    }
    if (image->GetImagePosition().GetSizeTypeX() == BackgroundImagePositionType::PX) {
        auto width = image->GetImagePosition().GetSizeValueX();
        auto height = image->GetImagePosition().GetSizeValueY();
        jsonValue->Put("x", width);
        jsonValue->Put("y", height);
        return jsonValue;
    } else {
        auto width = image->GetImagePosition().GetSizeValueX();
        auto height = image->GetImagePosition().GetSizeValueY();
        return GetAlignmentType(width, height);
    }
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetAlignmentType(double width, double height) const
{
    auto jsonValue = JsonUtil::Create(false);
    if (NearZero(width)) {
        if (NearZero(height)) {
            jsonValue->Put("x", "Alignment.TopStart");
            return jsonValue->GetValue("x");
        } else if (NearEqual(height, 50.0)) { // Determine whether the vertical element is centered
            jsonValue->Put("x", "Alignment.Start");
            return jsonValue->GetValue("x");
        } else {
            jsonValue->Put("x", "Alignment.BottomStart");
            return jsonValue->GetValue("x");
        }
    } else if (NearEqual(width, 50.0)) {  // Judge whether the horizontal element is centered
        if (NearZero(height)) {
            jsonValue->Put("x", "Alignment.Top");
            return jsonValue->GetValue("x");
        } else if (NearEqual(height, 50)) {
            jsonValue->Put("x", "Alignment.Center");
            return jsonValue->GetValue("x");
        } else {
            jsonValue->Put("x", "Alignment.Bottom");
            return jsonValue->GetValue("x");
        }
    } else {
        if (NearZero(height)) {
            jsonValue->Put("x", "Alignment.TopEnd");
            return jsonValue->GetValue("x");
        } else if (NearEqual(height, 50.0)) {
            jsonValue->Put("x", "Alignment.End");
            return jsonValue->GetValue("x");
        } else {
            jsonValue->Put("x", "Alignment.BottomEnd");
            return jsonValue->GetValue("x");
        }
    }
}

RefPtr<Decoration> InspectorComposedElement::GetFrontDecoration() const
{
    auto render = GetRenderBox();
    if (!render) {
        return nullptr;
    }
    return render->GetFrontDecoration();
}

double InspectorComposedElement::GetOpacity() const
{
    auto node = GetInspectorNode(DisplayElement::TypeId());
    if (!node) {
        return 1.0;
    }
    auto render = AceType::DynamicCast<RenderDisplay>(node);
    if (!render) {
        return 1.0;
    }
    return render->GetTransitionOpacity();
}

std::string InspectorComposedElement::GetVisibility() const
{
    auto node = GetInspectorNode(DisplayElement::TypeId());
    if (!node) {
        return VISIBLE_TYPE[static_cast<int32_t>(VisibleType::VISIBLE)];
    }
    auto render = AceType::DynamicCast<RenderDisplay>(node);
    if (!render) {
        return VISIBLE_TYPE[static_cast<int32_t>(VisibleType::VISIBLE)];
    }
    return VISIBLE_TYPE[static_cast<int32_t>(render->GetVisibleType())];
}

std::string InspectorComposedElement::GetClip() const
{
    auto render = GetRenderBox();
    if (!render) {
        return "false";
    }
    auto clipPath = render->GetClipPath();
    auto jsonValue = JsonUtil::Create(false);
    if (clipPath && clipPath->GetBasicShape()) {
        int32_t shapeType = static_cast<int32_t>(clipPath->GetBasicShape()->GetBasicShapeType());
        int32_t size = static_cast<int32_t>(sizeof(BASIC_SHAPE_TYPE) / sizeof(BASIC_SHAPE_TYPE[0]));
        if (shapeType < size) {
            jsonValue->Put("shape", BASIC_SHAPE_TYPE[shapeType]);
        }
    } else {
        if (render->GetBoxClipFlag() == true) {
            return "true";
        } else {
            return "false";
        }
    }
    return jsonValue->ToString();
}

bool InspectorComposedElement::GetClipFlag() const
{
    auto render = GetRenderBox();
    if (!render) {
        return false;
    }

    return render->GetBoxClipFlag();
}

bool InspectorComposedElement::GetEnabled() const
{
    auto node = GetInspectorNode(GetTargetTypeId());
    if (!node) {
        return true;
    }
    return !node->IsDisabled();
}

int32_t InspectorComposedElement::GetZIndex() const
{
    auto node = GetInspectorNode(GetTargetTypeId());
    if (!node) {
        return 0;
    }
    return node->GetZIndex();
}

DimensionOffset InspectorComposedElement::GetOriginPoint() const
{
    auto node = GetInspectorNode(TransformElement::TypeId());
    if (!node) {
        return DimensionOffset(OHOS::Ace::HALF_PERCENT, OHOS::Ace::HALF_PERCENT);
    }
    auto render = AceType::DynamicCast<RenderTransform>(node);
    if (!render) {
        return DimensionOffset(OHOS::Ace::HALF_PERCENT, OHOS::Ace::HALF_PERCENT);
    }
    return render->GetTransformOrigin();
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetRotate() const
{
    auto render = AceType::DynamicCast<RenderTransform>(GetInspectorNode(TransformElement::TypeId()));
    auto jsonValue = JsonUtil::Create(false);
    if (!render) {
        return jsonValue;
    }
    for (const auto& operation : render->GetTransformEffects().GetOperations()) {
        if (operation.type_ == TransformOperationType::ROTATE) {
            const auto& rotate = operation.rotateOperation_;
            jsonValue->Put("x", std::to_string(rotate.dx).c_str());
            jsonValue->Put("y", std::to_string(rotate.dy).c_str());
            jsonValue->Put("z", std::to_string(rotate.dz).c_str());
            jsonValue->Put("angle", std::to_string(rotate.angle).c_str());
            jsonValue->Put("centerX", render->GetOriginX().ToString().c_str());
            jsonValue->Put("centerY", render->GetOriginY().ToString().c_str());
            break;
        }
    }
    return jsonValue;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetScale() const
{
    auto render = AceType::DynamicCast<RenderTransform>(GetInspectorNode(TransformElement::TypeId()));
    auto jsonValue = JsonUtil::Create(false);
    if (!render) {
        return jsonValue;
    }
    for (const auto& operation : render->GetTransformEffects().GetOperations()) {
        if (operation.type_ == TransformOperationType::SCALE) {
            const auto& scale = operation.scaleOperation_;
            jsonValue->Put("x", std::to_string(scale.scaleX).c_str());
            jsonValue->Put("y", std::to_string(scale.scaleY).c_str());
            jsonValue->Put("z", std::to_string(scale.scaleZ).c_str());
            jsonValue->Put("centerX", render->GetOriginX().ToString().c_str());
            jsonValue->Put("centerY", render->GetOriginY().ToString().c_str());
            break;
        }
    }
    return jsonValue;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetTransform() const
{
    auto render = AceType::DynamicCast<RenderTransform>(GetInspectorNode(TransformElement::TypeId()));
    auto jsonValue = JsonUtil::Create(false);
    if (!render) {
        return jsonValue;
    }
    for (const auto& operation : render->GetTransformEffects().GetOperations()) {
        if (operation.type_ == TransformOperationType::MATRIX) {
            const auto& matrix = operation.matrix4_;
            jsonValue->Put("type", "matrix");
            auto matrixString = matrix.ToString();
            while (matrixString.find("\n") != std::string::npos) {
                auto num = matrixString.find("\n");
                matrixString.replace(num, 1, "");
            }
            jsonValue->Put("matrix", matrixString.c_str());
            break;
        }
    }
    return jsonValue;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetTranslate() const
{
    auto render = AceType::DynamicCast<RenderTransform>(GetInspectorNode(TransformElement::TypeId()));
    auto jsonValue = JsonUtil::Create(false);
    if (!render) {
        return jsonValue;
    }
    for (const auto& operation : render->GetTransformEffects().GetOperations()) {
        if (operation.type_ == TransformOperationType::TRANSLATE) {
            const auto& translate = operation.translateOperation_;
            jsonValue->Put("x", translate.dx.ToString().c_str());
            jsonValue->Put("y", translate.dy.ToString().c_str());
            jsonValue->Put("z", translate.dz.ToString().c_str());
            break;
        }
    }
    return jsonValue;
}

double InspectorComposedElement::GetBlur() const
{
    auto render = AceType::DynamicCast<RenderBox>(GetInspectorNode(BoxElement::TypeId()));
    if (!render) {
        return 0.0;
    }
    return render->GetBlurRadius().Value();
}

double InspectorComposedElement::GetBackDropBlur() const
{
    auto render = AceType::DynamicCast<RenderBox>(GetInspectorNode(BoxElement::TypeId()));
    if (!render) {
        return 0.0;
    }
    return render->GetBackdropRadius().Value();
}

double InspectorComposedElement::GetBrightness() const
{
    auto render = GetRenderBox();
    if (render) {
        return render->GetBrightness();
    }
    return 1.0;
}

double InspectorComposedElement::GetSaturate() const
{
    auto render = GetRenderBox();
    if (render) {
        return render->GetSaturate();
    }
    return 1.0;
}

double InspectorComposedElement::GetContrast() const
{
    auto render = GetRenderBox();
    if (render) {
        return render->GetContrast();
    }
    return 1.0;
}

double InspectorComposedElement::GetInvert() const
{
    auto render = GetRenderBox();
    if (render) {
        return render->GetInvert();
    }
    return 0.0;
}

double InspectorComposedElement::GetSepia() const
{
    auto render = GetRenderBox();
    if (render) {
        return render->GetSepia();
    }
    return 0.0;
}

double InspectorComposedElement::GetGrayScale() const
{
    auto render = GetRenderBox();
    if (render) {
        return render->GetGrayScale();
    }
    return 0.0;
}

double InspectorComposedElement::GetHueRotate() const
{
    auto render = GetRenderBox();
    if (render) {
        return render->GetHueRotate();
    }
    return 0.0;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetWindowBlur() const
{
    auto render = AceType::DynamicCast<RenderBox>(GetInspectorNode(BoxElement::TypeId()));
    auto jsonValue = JsonUtil::Create(false);
    if (!render) {
        return jsonValue;
    }
    jsonValue->Put("percent", std::to_string(render->GetWindowBlurProgress()).c_str());
    jsonValue->Put(
        "style", WINDOW_BLUR_STYLE[static_cast<int32_t>(render->GetWindowBlurStyle()) - WINDOW_BLUR_STYLE_ENUM_OFFSET]);
    return jsonValue;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetShadow() const
{
    auto render = AceType::DynamicCast<RenderBox>(GetInspectorNode(BoxElement::TypeId()));
    auto jsonValue = JsonUtil::Create(false);
    if (!render) {
        return jsonValue;
    }
    Shadow shadow = render->GetShadow();
    jsonValue->Put("radius", std::to_string(shadow.GetBlurRadius()).c_str());
    jsonValue->Put("color", ConvertColorToString(shadow.GetColor()).c_str());
    jsonValue->Put("offsetX", std::to_string(shadow.GetOffset().GetX()).c_str());
    jsonValue->Put("offsetY", std::to_string(shadow.GetOffset().GetY()).c_str());
    return jsonValue;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetOverlay() const
{
    auto jsonValue = JsonUtil::Create(false);
    // Since CoverageComponent is inherited from ComponentGroup, but Coverage does not have Element,
    // ComponentGroupElement is called.
    auto coverage = GetInspectorElement<RenderCoverage>(ComponentGroupElement::TypeId());
    if (!coverage) {
        jsonValue->Put("options", "{align: Alignment.Center, offset: {x: 0, y: 0}}");
        return jsonValue;
    }
    auto title = coverage->GetTextVal();
    auto alignment = coverage->GetAlignment();
    auto jsonAlign = JsonUtil::Create(false);
    if (alignment == Alignment::TOP_LEFT) {
        jsonAlign->Put("align", "Alignment.TopStart");
    } else if (alignment == Alignment::TOP_CENTER) {
        jsonAlign->Put("align", "Alignment.Top");
    } else if (alignment == Alignment::TOP_RIGHT) {
        jsonAlign->Put("align", "Alignment.TopEnd");
    } else if (alignment == Alignment::CENTER_LEFT) {
        jsonAlign->Put("align", "Alignment.Start");
    } else if (alignment == Alignment::CENTER_RIGHT) {
        jsonAlign->Put("align", "Alignment.End");
    } else if (alignment == Alignment::BOTTOM_LEFT) {
        jsonAlign->Put("align", "Alignment.BottomStart");
    } else if (alignment == Alignment::BOTTOM_CENTER) {
        jsonAlign->Put("align", "Alignment.Bottom");
    } else if (alignment == Alignment::BOTTOM_RIGHT) {
        jsonAlign->Put("align", "Alignment.BottomEnd");
    } else {
        jsonAlign->Put("align", "Alignment.Center");
    }
    auto offsetJson = JsonUtil::Create(false);
    offsetJson->Put("x", coverage->GetX().ToString().c_str());
    offsetJson->Put("y", coverage->GetY().ToString().c_str());
    jsonAlign->Put("offset", offsetJson);
    jsonValue->Put("title", title.c_str());
    jsonValue->Put("options", jsonAlign);
    return jsonValue;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetMask() const
{
    auto render = GetRenderBox();
    auto jsonValue = JsonUtil::Create(false);
    if (!render) {
        return jsonValue;
    }
    auto mask = render->GetMask();
    if (mask && mask->GetMaskPath() && mask->GetMaskPath()->GetBasicShape()) {
        auto shape = mask->GetMaskPath()->GetBasicShape();
        int32_t shapeType = static_cast<int32_t>(shape->GetBasicShapeType());
        int32_t size = static_cast<int32_t>(sizeof(BASIC_SHAPE_TYPE) / sizeof(BASIC_SHAPE_TYPE[0]));
        if (shapeType < size) {
            jsonValue->Put("shape", BASIC_SHAPE_TYPE[shapeType]);
        }
    }
    return jsonValue;
}

RefPtr<GridColumnInfo> InspectorComposedElement::GetGridColumnInfo() const
{
    auto render = GetRenderBox();
    if (!render) {
        return nullptr;
    }
    auto columnInfo = render->GetGridColumnInfo();
    if (!columnInfo) {
        return nullptr;
    }
    return columnInfo;
}

int32_t InspectorComposedElement::GetGridSpan() const
{
    auto columnInfo = GetGridColumnInfo();
    if (columnInfo) {
        return columnInfo->GetColumns();
    }
    return 1;
}

int32_t InspectorComposedElement::GetGridOffset() const
{
    auto columnInfo = GetGridColumnInfo();
    if (columnInfo) {
        if (columnInfo->GetOffset(GridSizeType::UNDEFINED) == -1) {
            return 0;
        }
        return columnInfo->GetOffset(GridSizeType::UNDEFINED);
    }
    return 0;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetUseSizeType() const
{
    auto columnInfo = GetGridColumnInfo();
    auto jsonRoot = JsonUtil::Create(false);
    if (!columnInfo) {
        return jsonRoot;
    }
    int32_t index = static_cast<int32_t>(GridSizeType::XS);
    for (; index < static_cast<int32_t>(GridSizeType::XL); index++) {
        auto jsonValue = JsonUtil::Create(false);
        GridSizeType type = static_cast<GridSizeType>(index);
        jsonValue->Put("span", static_cast<int32_t>(columnInfo->GetColumns(type)));
        jsonValue->Put("offset", columnInfo->GetOffset(type));
        jsonRoot->Put(GRID_SIZE_TYPE[index], jsonValue);
    }
    return jsonRoot;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetUseAlign() const
{
    auto render = GetRenderBox();
    auto jsonValue = JsonUtil::Create(false);
    if (!render) {
        return jsonValue;
    }
    jsonValue->Put("edge", ConvertSideToString(render->GetUseAlignSide()).c_str());
    jsonValue->Put("offset", render->GetUseAlignOffset().ToString().c_str());
    return jsonValue;
}

std::string InspectorComposedElement::GetBindPopup() const
{
    auto resultJson = JsonUtil::Create(false);
    auto coverageElement = GetContentElement<ComponentGroupElement>(ComponentGroupElement::TypeId(), false);
    RefPtr<PopupElementV2> popupElement = nullptr;
    if (coverageElement) {
        for (const auto& element : coverageElement->GetChildren()) {
            if (AceType::DynamicCast<PopupElementV2>(element)) {
                popupElement = AceType::DynamicCast<PopupElementV2>(element);
            }
        }
    }
    if (!popupElement) {
        return "";
    }
    std::string show;
    if (popupElement->IsShow()) {
        show = "true";
    } else {
        show = "false";
    }
    auto popupJson = JsonUtil::Create(false);
    popupJson->Put("message", popupElement->GetMessage().c_str());
    popupJson->Put("placementOnTop", popupElement->GetPlacementOnTop());
    auto primaryButtonJson = JsonUtil::Create(false);
    primaryButtonJson->Put("value", popupElement->GetPrimaryButtonValue().c_str());
    auto secondaryButtonJson = JsonUtil::Create(false);
    secondaryButtonJson->Put("value", popupElement->GetSecondaryButtonValue().c_str());

    popupJson->Put("primaryButton", primaryButtonJson);
    popupJson->Put("secondaryButton", secondaryButtonJson);
    return show + ", " + popupJson->ToString();
}

std::string InspectorComposedElement::GetBindContextMenu() const
{
    auto node = GetInspectorNode(BoxElement::TypeId());
    if (!node) {
        return "-";
    }
    auto responseType = AceType::DynamicCast<RenderBox>(node);
    if (responseType) {
        if (responseType->GetOnMouseId()) {
            return "ResponseType.RightClick";
        } else if (responseType->GetOnLongPress()) {
            return "ResponseType.Longpress";
        } else {
            return "-";
        }
    }
    return "-";
}

void InspectorComposedElement::UpdateEventTarget(BaseEventInfo& info) const
{
    auto area = GetCurrentRectAndOrigin();
    auto& target = info.GetTargetWichModify();
    target.area.SetOffset(area.first.GetOffset());
    target.area.SetHeight(Dimension(area.first.GetSize().Height()));
    target.area.SetWidth(Dimension(area.first.GetSize().Width()));
    target.origin = DimensionOffset(area.second);
}

std::pair<Rect, Offset> InspectorComposedElement::GetCurrentRectAndOrigin() const
{
    auto rectInLocal = GetRenderRectInLocal();
    auto rectInGlobal = GetRenderRect();
    auto marginLeft = GetMargin(AnimatableType::PROPERTY_MARGIN_LEFT).ConvertToPx();
    auto marginRight = GetMargin(AnimatableType::PROPERTY_MARGIN_RIGHT).ConvertToPx();
    auto marginTop = GetMargin(AnimatableType::PROPERTY_MARGIN_TOP).ConvertToPx();
    auto marginBottom = GetMargin(AnimatableType::PROPERTY_MARGIN_BOTTOM).ConvertToPx();
    auto Localoffset = rectInLocal.GetOffset();
    auto offset = Offset(Localoffset.GetX() + marginLeft, Localoffset.GetY() + marginTop);
    auto size = Size(rectInLocal.Width() - marginLeft - marginRight, rectInLocal.Height() - marginTop - marginBottom);
    auto globalOffset = rectInGlobal.GetOffset();
    return { { offset, size }, { globalOffset.GetX() - Localoffset.GetX(), globalOffset.GetY() - Localoffset.GetY() } };
}

void InspectorComposedElement::GetColorsAndRepeating(
    std::unique_ptr<JsonValue>& resultJson, const Gradient& gradient) const
{
    auto jsoncolorArray = JsonUtil::CreateArray(false);
    auto colors = gradient.GetColors();
    for (size_t i = 0; i < colors.size(); ++i) {
        auto temp = JsonUtil::CreateArray(false);
        auto value = std::to_string(colors[i].GetDimension().Value() / 100.0);
        auto color = colors[i].GetColor().ColorToString();
        temp->Put("0", color.c_str());
        temp->Put("1", value.c_str());
        auto index = std::to_string(i);
        jsoncolorArray->Put(index.c_str(), temp);
    }
    resultJson->Put("colors", jsoncolorArray);
    auto repeat = ConvertBoolToString(gradient.GetRepeat());
    resultJson->Put("repeating", repeat.c_str());
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetLinearGradient() const
{
    auto resultJson = JsonUtil::Create(false);
    auto node = GetRenderBox();
    if (!node) {
        return resultJson;
    }
    auto decoration = node->GetBackDecoration();
    if (decoration) {
        auto lineGradient = decoration->GetGradient();
        if (GradientType::LINEAR != lineGradient.GetType()) {
            return resultJson;
        }
        if (lineGradient.GetLinearGradient().angle) {
            resultJson->Put("angle", lineGradient.GetLinearGradient().angle->ToString().c_str());
        }

        auto linearX = lineGradient.GetLinearGradient().linearX;
        auto linearY = lineGradient.GetLinearGradient().linearY;
        if (linearX == GradientDirection::LEFT) {
            if (linearY == GradientDirection::TOP) {
                resultJson->Put("direction", "GradientDirection.LeftTop");
            } else if (linearY == GradientDirection::BOTTOM) {
                resultJson->Put("direction", "GradientDirection.LeftBottom");
            } else {
                resultJson->Put("direction", "GradientDirection.Left");
            }
        } else if (linearX == GradientDirection::RIGHT) {
            if (linearY == GradientDirection::TOP) {
                resultJson->Put("direction", "GradientDirection.RightTop");
            } else if (linearY == GradientDirection::BOTTOM) {
                resultJson->Put("direction", "GradientDirection.RightBottom");
            } else {
                resultJson->Put("direction", "GradientDirection.Right");
            }
        } else {
            if (linearY == GradientDirection::TOP) {
                resultJson->Put("direction", "GradientDirection.Top");
            } else if (linearY == GradientDirection::BOTTOM) {
                resultJson->Put("direction", "GradientDirection.Bottom");
            } else {
                resultJson->Put("direction", "GradientDirection.None");
            }
        }
        GetColorsAndRepeating(resultJson, lineGradient);
    }
    return resultJson;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetSweepGradient() const
{
    auto resultJson = JsonUtil::Create(false);
    auto node = GetRenderBox();
    if (!node) {
        return resultJson;
    }
    auto decoration = node->GetBackDecoration();
    if (decoration) {
        auto sweepGradient = decoration->GetGradient();
        if (GradientType::SWEEP != sweepGradient.GetType()) {
            return resultJson;
        }
        auto radialCenterX = sweepGradient.GetSweepGradient().centerX;
        auto radialCenterY = sweepGradient.GetSweepGradient().centerY;
        if (radialCenterX && radialCenterY) {
            auto jsPoint = JsonUtil::CreateArray(false);
            jsPoint->Put("0", radialCenterX->ToString().c_str());
            jsPoint->Put("1", radialCenterY->ToString().c_str());
            resultJson->Put("center", jsPoint);
        }

        auto startAngle = sweepGradient.GetSweepGradient().startAngle;
        auto endAngle = sweepGradient.GetSweepGradient().endAngle;
        if (startAngle) {
            resultJson->Put("start", startAngle->ToString().c_str());
        }
        if (endAngle) {
            resultJson->Put("end", endAngle->ToString().c_str());
        }

        GetColorsAndRepeating(resultJson, sweepGradient);
    }
    return resultJson;
}

std::unique_ptr<JsonValue> InspectorComposedElement::GetRadialGradient() const
{
    auto resultJson = JsonUtil::Create(false);
    auto node = GetRenderBox();
    if (!node) {
        return resultJson;
    }
    auto decoration = node->GetBackDecoration();
    if (decoration) {
        auto radialGradient = decoration->GetGradient();
        if (GradientType::RADIAL != radialGradient.GetType()) {
            return resultJson;
        }

        auto radialCenterX = radialGradient.GetRadialGradient().radialCenterX;
        auto radialCenterY = radialGradient.GetRadialGradient().radialCenterY;
        if (radialCenterX && radialCenterY) {
            auto jsPoint = JsonUtil::CreateArray(false);
            jsPoint->Put("0", radialCenterX->ToString().c_str());
            jsPoint->Put("1", radialCenterY->ToString().c_str());
            resultJson->Put("center", jsPoint);
        }

        auto radius = radialGradient.GetRadialGradient().radialVerticalSize;
        if (radius) {
            resultJson->Put("radius", radius->ToString().c_str());
        }

        GetColorsAndRepeating(resultJson, radialGradient);
    }
    return resultJson;
}

const std::string& InspectorComposedElement::GetTag() const
{
    auto iter = COMPONENT_TAG_TO_ETS_TAG_MAP.find(name_);
    return iter != COMPONENT_TAG_TO_ETS_TAG_MAP.end() ? iter->second : name_;
}

bool InspectorComposedElement::GetClickable() const
{
    auto node = GetAccessibilityNode();
    if (!node) {
        return false;
    }
    return node->GetClickableState();
}
bool InspectorComposedElement::GetCheckable() const
{
    auto node = GetAccessibilityNode();
    if (!node) {
        return false;
    }
    return node->GetCheckableState();
}
bool InspectorComposedElement::GetFocusable() const
{
    auto focusableElement = GetContentElement<FocusableElement>(FocusableElement::TypeId(), false);
    if (!focusableElement) {
        return false;
    }
    return focusableElement->IsFocusable();
}
bool InspectorComposedElement::GetScrollable() const
{
    auto node = GetAccessibilityNode();
    if (!node) {
        return false;
    }
    return node->GetScrollableState();
}
bool InspectorComposedElement::GetLongClickable() const
{
    auto node = GetAccessibilityNode();
    if (!node) {
        return false;
    }
    return node->GetLongClickableState();
}
bool InspectorComposedElement::IsSelected() const
{
    auto node = GetAccessibilityNode();
    if (!node) {
        return false;
    }
    return node->GetSelectedState();
}
bool InspectorComposedElement::IsPassword() const
{
    auto node = GetAccessibilityNode();
    if (!node) {
        return false;
    }
    return node->GetIsPassword();
}
bool InspectorComposedElement::IsChecked() const
{
    auto node = GetAccessibilityNode();
    if (!node) {
        return false;
    }
    return node->GetCheckedState();
}
bool InspectorComposedElement::IsFocused() const
{
    auto node = GetAccessibilityNode();
    if (!node) {
        return false;
    }
    return node->GetFocusedState();
}

} // namespace OHOS::Ace::V2
