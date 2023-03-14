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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_INSPECTOR_NODE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_INSPECTOR_NODE_H

#include <tuple>

#include "base/geometry/dimension_offset.h"
#include "base/geometry/matrix4.h"
#include "base/json/json_util.h"
#include "core/components/box/mask.h"
#include "core/components/common/layout/grid_column_info.h"
#include "core/components/common/properties/clip_path.h"

namespace OHOS::Ace::V2 {

class InspectorNode;

using DoubleJsonFunc = std::function<double(const InspectorNode&)>;
using StringJsonFunc = std::function<std::string(const InspectorNode&)>;
using BoolJsonFunc = std::function<bool(const InspectorNode&)>;
using IntJsonFunc = std::function<int32_t(const InspectorNode&)>;
using JsonValueJsonFunc = std::function<std::unique_ptr<JsonValue>(const InspectorNode&)>;

struct RotateParam {
    float x;
    float y;
    float z;
    float angle;
    Dimension centerX;
    Dimension centerY;
};

struct ScaleParam {
    float x;
    float y;
    float z;
    Dimension centerX;
    Dimension centerY;
};

class ACE_EXPORT InspectorNode : public virtual AceType {
    DECLARE_ACE_TYPE(InspectorNode, AceType)

public:
    InspectorNode() = default;
    ~InspectorNode() override = default;

    int32_t GetinspectorId() const
    {
        return inspectorId_;
    }

    void SetInspectorTag(const std::string& inspectorTag)
    {
        inspectorTag_ = inspectorTag;
    }

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void SetDebugLine(const std::string& debugLine)
    {
        debugLine_ = debugLine;
    }

    std::string GetDebugLine()
    {
        return debugLine_;
    }
#endif

    // dimension settings
    virtual std::string GetWidth() const = 0;
    virtual std::string GetHeight() const = 0;
    virtual std::unique_ptr<JsonValue> GetSize() const = 0;
    virtual std::unique_ptr<JsonValue> GetPadding() const = 0;
    virtual Dimension GetMargin(OHOS::Ace::AnimatableType type) const = 0;
    virtual std::unique_ptr<JsonValue> GetAllMargin() const = 0;
    virtual std::string GetConstraintSize() const = 0;
    virtual int32_t GetLayoutPriority() const = 0;
    virtual int32_t GetLayoutWeight() const = 0;

    // position settings
    virtual std::string GetAlign() const = 0;
    virtual std::string GetDirectionStr() const = 0;
    virtual TextDirection GetDirection() const = 0;
    virtual std::unique_ptr<JsonValue> GetPosition() const = 0;
    virtual std::unique_ptr<JsonValue> GetMarkAnchor() const = 0;
    virtual std::unique_ptr<JsonValue> GetOffset() const = 0;
    virtual std::string GetRect() = 0;
    virtual Rect GetParentRect() const = 0;

    // layout constraint
    virtual double GetAspectRatio() const = 0;
    virtual int32_t GetDisplayPriority() const = 0;

    // flex layout
    virtual std::string GetFlexBasis() const = 0;
    virtual double GetFlexGrow() const = 0;
    virtual double GetFlexShrink() const = 0;
    virtual std::string GetAlignSelf() const = 0;

    // border settings
    virtual Border GetBorder() const = 0;
    virtual std::unique_ptr<JsonValue> GetUnifyBorder() const = 0;
    virtual std::string GetBorderStyle() const = 0;
    virtual std::string GetBorderWidth() const = 0;
    virtual std::string GetBorderColor() const = 0;
    virtual std::string GetBorderRadius() const = 0;

    // background settings
    virtual RefPtr<Decoration> GetBackDecoration() const = 0;
    virtual std::string GetBackgroundImage() const = 0;
    virtual std::string GetBackgroundColor() const = 0;
    virtual std::unique_ptr<JsonValue> GetBackgroundImageSize() const = 0;
    virtual std::unique_ptr<JsonValue> GetBackgroundImagePosition() const = 0;
    virtual std::unique_ptr<JsonValue> GetAlignmentType(double width, double height) const = 0;

    // front decoration settings
    virtual RefPtr<Decoration> GetFrontDecoration() const = 0;

    // opacity settings
    virtual double GetOpacity() const = 0;

    // visibility settings
    virtual std::string GetVisibility() const = 0;

    // enable settings
    virtual bool GetEnabled() const = 0;

    // zindex settings
    virtual int32_t GetZIndex() const = 0;

    // graphical transformation
    virtual DimensionOffset GetOriginPoint() const = 0;
    virtual std::unique_ptr<JsonValue> GetRotate() const = 0;
    virtual std::unique_ptr<JsonValue> GetScale() const = 0;
    virtual std::unique_ptr<JsonValue> GetTransform() const = 0;
    virtual std::unique_ptr<JsonValue> GetTranslate() const = 0;

    virtual double GetBlur() const = 0;
    virtual double GetBackDropBlur() const = 0;
    virtual std::unique_ptr<JsonValue> GetWindowBlur() const = 0;
    virtual std::unique_ptr<JsonValue> GetShadow() const = 0;
    virtual double GetBrightness() const = 0;
    virtual double GetSaturate() const = 0;
    virtual double GetContrast() const = 0;
    virtual double GetInvert() const = 0;
    virtual double GetSepia() const = 0;
    virtual double GetGrayScale() const = 0;
    virtual double GetHueRotate() const = 0;

    // shape clip
    virtual std::string GetClip() const = 0;
    virtual std::unique_ptr<JsonValue> GetMask() const = 0;

    // grid setting
    virtual int32_t GetGridSpan() const = 0;
    virtual int32_t GetGridOffset() const = 0;
    virtual std::unique_ptr<JsonValue> GetUseSizeType() const = 0;
    virtual RefPtr<GridColumnInfo> GetGridColumnInfo() const = 0;

    // useAlign seeting
    virtual std::unique_ptr<JsonValue> GetUseAlign() const = 0;
    virtual std::unique_ptr<JsonValue> ToJsonObject() const = 0;

    virtual std::unique_ptr<JsonValue> GetOverlay() const = 0;

    virtual void UpdateEventTarget(BaseEventInfo& info) const = 0;

    virtual std::pair<Rect, Offset> GetCurrentRectAndOrigin() const = 0;

    // color gradient
    virtual std::unique_ptr<JsonValue> GetLinearGradient() const = 0;
    virtual std::unique_ptr<JsonValue> GetSweepGradient() const = 0;
    virtual std::unique_ptr<JsonValue> GetRadialGradient() const = 0;

    // bindpopup
    virtual std::string GetBindPopup() const = 0;

    // bindcontextmenu
    virtual std::string GetBindContextMenu() const = 0;

    // auto test
    virtual bool GetClickable() const = 0;
    virtual bool GetCheckable() const = 0;
    virtual bool GetFocusable() const = 0;
    virtual bool GetScrollable() const = 0;
    virtual bool GetLongClickable() const = 0;
    virtual bool IsSelected() const = 0;
    virtual bool IsPassword() const = 0;
    virtual bool IsChecked() const = 0;
    virtual bool IsFocused() const = 0;

protected:
    int32_t inspectorId_ = 0;
    int32_t inspectorParentId_ = -1;
    std::string inspectorTag_;
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    std::string debugLine_;
#endif
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_INSPECTOR_NODE_H
