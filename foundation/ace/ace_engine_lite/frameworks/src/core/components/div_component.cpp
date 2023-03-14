/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "div_component.h"
#include "ace_log.h"
#include "key_parser.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {
DivComponent::DivComponent(jerry_value_t options,
                           jerry_value_t children,
                           AppStyleManager* styleManager)
    : Component(options, children, styleManager),
      isSecondaryAxisAlignSet_(false), isVerticalLayout_(false)
{
    SetComponentName(K_DIV);
    nativeView_.SetStyle(STYLE_BACKGROUND_OPA, 0);
}


inline UIView *DivComponent::GetComponentRootView() const
{
    return const_cast<FlexLayout *>(&nativeView_);
}

bool DivComponent::ApplyPrivateStyle(const AppStyleItem* style)
{
    // Set default value
    if (!isSecondaryAxisAlignSet_) {
        nativeView_.SetSecondaryAxisAlign(OHOS::ALIGN_START);
    }
    uint16_t stylePropNameId = GetStylePropNameId(style);
    if (!KeyParser::IsKeyValid(stylePropNameId)) {
        return false;
    }
    const char * const strValue = GetStyleStrValue(style);
    if (strValue == nullptr) {
        return false;
    }

    bool applyResult = true;
    uint16_t valueId = KeyParser::ParseKeyId(strValue, GetStyleStrValueLen(style));
    switch (stylePropNameId) {
        case K_FLEX_DIRECTION: {
            switch (valueId) {
                case K_COLUMN:
                    nativeView_.SetLayoutDirection(LAYOUT_VER);
                    isVerticalLayout_ = true;
                    break;
                case K_ROW:
                    nativeView_.SetLayoutDirection(LAYOUT_HOR);
                    break;
                case K_ROW_REVERSE:
                    nativeView_.SetLayoutDirection(LAYOUT_HOR_R);
                    break;
                case K_COLUMN_REVERSE:
                    nativeView_.SetLayoutDirection(LAYOUT_VER_R);
                    break;
                default:
                    applyResult = false;
                    break;
            }
            break;
        }
        case K_JUSTIFY_CONTENT: {
            switch (valueId) {
                case K_FLEX_START:
                    nativeView_.SetMajorAxisAlign(OHOS::ALIGN_START);
                    break;
                case K_FLEX_END:
                    nativeView_.SetMajorAxisAlign(OHOS::ALIGN_END);
                    break;
                case K_CENTER:
                    nativeView_.SetMajorAxisAlign(OHOS::ALIGN_CENTER);
                    break;
                case K_SPACE_BETWEEN:
                    nativeView_.SetMajorAxisAlign(OHOS::ALIGN_BETWEEN);
                    break;
                case K_SPACE_AROUND:
                    nativeView_.SetMajorAxisAlign(OHOS::ALIGN_AROUND);
                    break;
                case K_SPACE_EVENLY:
                    nativeView_.SetMajorAxisAlign(OHOS::ALIGN_EVENLY);
                    break;
                default:
                    applyResult = false;
                    break;
            }
            break;
        }
        case K_ALIGN_ITEMS: {
            switch (valueId) {
                case K_FLEX_START:
                    nativeView_.SetSecondaryAxisAlign(OHOS::ALIGN_START);
                    break;
                case K_FLEX_END:
                    nativeView_.SetSecondaryAxisAlign(OHOS::ALIGN_END);
                    break;
                case K_CENTER:
                    nativeView_.SetSecondaryAxisAlign(OHOS::ALIGN_CENTER);
                    break;
                default:
                    applyResult = false;
                    break;
            }
            if (applyResult)
                isSecondaryAxisAlignSet_ = true;
            break;
        }
        case K_FLEX_WRAP: {
            if (valueId == K_WRAP) {
                nativeView_.SetFlexWrap(FlexLayout::WRAP);
            } else {
                nativeView_.SetFlexWrap(FlexLayout::NOWRAP);
            }
            break;
        }
        default:
            applyResult = false;
            break;
    }

    return applyResult;
}

bool DivComponent::ProcessChildren()
{
    // add all children to this container
    AppendChildren(this);

    return true;
}

void DivComponent::PostUpdate(uint16_t attrKeyId)
{
    nativeView_.LayoutChildren();
}

void DivComponent::AttachView(const Component *child)
{
    if (child == nullptr) {
        return;
    }
    if (!isSecondaryAxisAlignSet_) {
        ConstrainedParameter param;
        child->GetConstrainedParam(param);
        DimensionType type;
        Component *component = const_cast<Component *>(child);
        if (isVerticalLayout_) {
            type = child->GetDimension(K_WIDTH).type;
            if (type == DimensionType::TYPE_UNKNOWN) {
                const int16_t width = this->GetWidth();
                component->SetWidth(width);
                param.maxWidth = width;
                component->AlignDimensions(param);
                child->AdaptBoxSizing();
            }
        } else {
            type = child->GetDimension(K_HEIGHT).type;
            if (type == DimensionType::TYPE_UNKNOWN) {
                const int16_t height = this->GetHeight();
                component->SetHeight(height);
                param.maxHeight = height;
                component->AlignDimensions(param);
                child->AdaptBoxSizing();
            }
        }
    }
    nativeView_.Add(child->GetComponentRootView());
}

void DivComponent::LayoutChildren()
{
    nativeView_.LayoutChildren();
}
} // namespace ACELite
} // namespace OHOS
