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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_INSPECTOR_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_INSPECTOR_COMPOSED_ELEMENT_H

#include <tuple>

#include "core/components/box/render_box.h"
#include "core/components/display/display_component.h"
#include "core/components_v2/inspector/inspector_node.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT InspectorComposedElement : public ComposedElement, public InspectorNode {
    DECLARE_ACE_TYPE(InspectorComposedElement, ComposedElement, InspectorNode)

public:
    explicit InspectorComposedElement(const ComposeId& id);
    ~InspectorComposedElement() override;

    void Prepare(const WeakPtr<Element>& weakParent) override;
    void Update() override;
    bool CanUpdate(const RefPtr<Component>& newComponent) override;
    template<class T>
    RefPtr<T> GetInspectorElement(IdType typeId) const
    {
        auto child = children_.empty() ? nullptr : children_.front();
        while (child) {
            auto inspectorComposedElement = AceType::DynamicCast<InspectorComposedElement>(child);
            if (inspectorComposedElement) {
                return nullptr;
            }
            if (AceType::TypeId(child) == typeId) {
                return AceType::DynamicCast<T>(child->GetRenderNode());
            }

            child = child->GetChildren().empty() ? nullptr : child->GetChildren().front();
        }
        return nullptr;
    }
    void AddComposedComponentId();
    void RemoveInspectorNode(int32_t id);
    RefPtr<RenderNode> GetInspectorNode(IdType typeId, bool isForward = false) const;

    void OnInactive() override;
    void OnActive() override;

    template<class T>
    RefPtr<T> GetContentElement(IdType typeId, bool isFindAll = true) const
    {
        auto child = children_.empty() ? nullptr : children_.front();
        while (child) {
            LOGD("GetContentElement: child = %{public}s", AceType::TypeName(child));
            auto inspectorComposedElement = AceType::DynamicCast<InspectorComposedElement>(child);
            if (inspectorComposedElement && !isFindAll) {
                return nullptr;
            }
            if (AceType::TypeId(child) == typeId) {
                return AceType::DynamicCast<T>(child);
            }
            child = child->GetChildren().empty() ? nullptr : child->GetChildren().front();
        }
        return nullptr;
    }

    template<class T>
    RefPtr<T> GetContentRender(IdType typeId) const
    {
        auto child = children_.empty() ? nullptr : children_.front();
        while (child) {
            if (AceType::TypeId(child) == typeId) {
                return AceType::DynamicCast<T>(child->GetRenderNode());
            }
            child = child->GetChildren().empty() ? nullptr : child->GetChildren().front();
        }
        return nullptr;
    }

    RefPtr<Element> GetInspectorComposedElement()
    {
        auto child = children_.empty() ? nullptr : children_.front();
        while (child) {
            auto inspectorComposedElement = AceType::DynamicCast<InspectorComposedElement>(child);
            if (inspectorComposedElement) {
                return child;
            }
            child = child->GetChildren().empty() ? nullptr : child->GetChildren().front();
        }
        return nullptr;
    }

    RefPtr<Element> GetElementChildBySlot(const RefPtr<Element>& element, int32_t& slot) const;
    RefPtr<Element> GetInspectorComposedElementParent(const RefPtr<Element>& element) const;

    // dimension settings
    std::string GetWidth() const override;
    std::string GetHeight() const override;
    std::unique_ptr<JsonValue> GetSize() const override;
    std::unique_ptr<JsonValue> GetPadding() const override;
    Dimension GetMargin(OHOS::Ace::AnimatableType type) const override;
    std::unique_ptr<JsonValue> GetAllMargin() const override;
    std::string GetConstraintSize() const override;
    int32_t GetLayoutPriority() const override;
    int32_t GetLayoutWeight() const override;

    // position settings
    std::string GetAlign() const override;
    std::string GetDirectionStr() const override;
    TextDirection GetDirection() const override;
    std::unique_ptr<JsonValue> GetPosition() const override;
    std::unique_ptr<JsonValue> GetMarkAnchor() const override;
    std::unique_ptr<JsonValue> GetOffset() const override;
    std::string GetRect() override;
    Rect GetParentRect() const override;

    // layout constraint
    double GetAspectRatio() const override;
    int32_t GetDisplayPriority() const override;

    // flex layout
    std::string GetFlexBasis() const override;
    double GetFlexGrow() const override;
    double GetFlexShrink() const override;
    std::string GetAlignSelf() const override;

    // border settings
    Border GetBorder() const override;
    std::unique_ptr<JsonValue> GetUnifyBorder() const override;
    std::string GetBorderStyle() const override;
    std::string GetBorderWidth() const override;
    std::string GetBorderColor() const override;
    std::string GetBorderRadius() const override;

    // background settings
    RefPtr<Decoration> GetBackDecoration() const override;
    std::string GetBackgroundImage() const override;
    std::string GetBackgroundColor() const override;
    std::unique_ptr<JsonValue> GetBackgroundImageSize() const override;
    std::unique_ptr<JsonValue> GetBackgroundImagePosition() const override;
    std::unique_ptr<JsonValue> GetAlignmentType(double width, double height) const override;

    // front decoration settings
    RefPtr<Decoration> GetFrontDecoration() const override;

    // opacity settings
    double GetOpacity() const override;

    // visibility settings
    std::string GetVisibility() const override;

    // enable settings
    bool GetEnabled() const override;

    // zindex settings
    int32_t GetZIndex() const override;

    // graphical transformation
    DimensionOffset GetOriginPoint() const override;
    std::unique_ptr<JsonValue> GetRotate() const override;
    std::unique_ptr<JsonValue> GetScale() const override;
    std::unique_ptr<JsonValue> GetTransform() const override;
    std::unique_ptr<JsonValue> GetTranslate() const override;

    double GetBlur() const override;
    double GetBackDropBlur() const override;
    std::unique_ptr<JsonValue> GetWindowBlur() const override;
    std::unique_ptr<JsonValue> GetShadow() const override;
    double GetBrightness() const override;
    double GetSaturate() const override;
    double GetContrast() const override;
    double GetInvert() const override;
    double GetSepia() const override;
    double GetGrayScale() const override;
    double GetHueRotate() const override;

    // shape clip
    std::string GetClip() const override;
    bool GetClipFlag() const;
    std::unique_ptr<JsonValue> GetMask() const override;

    // grid setting
    int32_t GetGridSpan() const override;
    int32_t GetGridOffset() const override;
    std::unique_ptr<JsonValue> GetUseSizeType() const override;
    RefPtr<GridColumnInfo> GetGridColumnInfo() const override;

    // useAlign seeting
    std::unique_ptr<JsonValue> GetUseAlign() const override;
    std::unique_ptr<JsonValue> ToJsonObject() const override;

    std::unique_ptr<JsonValue> GetOverlay() const override;

    // color gradient
    std::unique_ptr<JsonValue> GetLinearGradient() const override;
    std::unique_ptr<JsonValue> GetSweepGradient() const override;
    std::unique_ptr<JsonValue> GetRadialGradient() const override;
    void GetColorsAndRepeating(std::unique_ptr<JsonValue>& resultJson, const Gradient& gradient) const;

    // bindpopup
    std::string GetBindPopup() const override;
    virtual AceType::IdType GetTargetTypeId() const
    {
        return AceType::TypeId(this);
    }

    // bindcontextmenu
    std::string GetBindContextMenu() const override;

    virtual void AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
    {
        LOGW("inspector AddChildWithSlot");
    }

    virtual void UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
    {
        LOGW("inspector UpdateChildWithSlot");
    }

    virtual void DeleteChildWithSlot(int32_t slot)
    {
        LOGW("inspector DeleteChildWithSlot");
    }

    void UpdateEventTarget(BaseEventInfo& info) const override;

    std::pair<Rect, Offset> GetCurrentRectAndOrigin() const override;

    bool IsRectValid() const
    {
        return isRectValid_;
    }

    void SetKey(const std::string& key)
    {
        key_ = key;
    }

    const std::string& GetKey() const
    {
        return key_;
    }

    const std::string& GetTag() const;
    bool GetClickable() const override;
    bool GetCheckable() const override;
    bool GetFocusable() const override;
    bool GetScrollable() const override;
    bool GetLongClickable() const override;
    bool IsSelected() const override;
    bool IsPassword() const override;
    bool IsChecked() const override;
    bool IsFocused() const override;

protected:
    RefPtr<RenderBox> GetRenderBox() const;

    RefPtr<AccessibilityNode> accessibilityNode_;
    bool accessibilityEnabled_ = false;
    RefPtr<AccessibilityNode> GetAccessibilityNode() const;

private:
    bool isRectValid_;
    std::string key_;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_INSPECTOR_COMPOSED_ELEMENT_H
