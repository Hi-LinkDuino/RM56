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

#include "image_component.h"
#include "ace_log.h"
#include "ace_mem_base.h"
#include "js_app_context.h"
#include "js_fwk_common.h"
#include "key_parser.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {
ImageComponent::ImageComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager)
    : Component(options, children, styleManager),
      fitOriginalSize_(0),
      hasSetWidth_(false),
      hasSetHeight_(false)
{
    SetComponentName(K_IMAGE);
}

bool ImageComponent::CreateNativeViews()
{
    // set default value
    imageView_.SetAutoEnable(false);
    imageView_.SetResizeMode(resizeMode_);
    const int16_t defaultOpacity = 0;
    imageView_.SetStyle(STYLE_BACKGROUND_OPA, defaultOpacity);
    return true;
}

inline UIView *ImageComponent::GetComponentRootView() const
{
    return const_cast<UIImageView *>(&imageView_);
}

bool ImageComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    bool setResult = true;
    switch (attrKeyId) {
        case K_SRC: {
            char *src = const_cast<char *>(ParseImageSrc(attrValue));
            imageView_.SetSrc(src);
            ACE_FREE(src);
            break;
        }
        default:
            setResult = false;
            break;
    }

    return setResult;
}

void ImageComponent::UpdateWidgetFitMode()
{
    // update the imageView
    if (fitOriginalSize_ && !(hasSetWidth_ || hasSetHeight_)) {
        imageView_.SetAutoEnable(true);
    } else {
        imageView_.SetAutoEnable(false);
    }
    imageView_.SetResizeMode(resizeMode_);
}

bool ImageComponent::ApplyPrivateStyle(const AppStyleItem *style)
{
    uint16_t styleKey = style->GetPropNameId();
    bool setResult = true;
    switch (styleKey) {
        case K_OBJECT_FIT: {
            const char * const strValue = GetStyleStrValue(style);
            if (strValue == nullptr) {
                return false;
            }
            uint16_t mode = KeyParser::ParseKeyId(strValue, GetStyleStrValueLen(style));
            UpdateResizeMode(mode);
            break;
        }
        case K_FIT_ORIGINAL_SIZE: {
            fitOriginalSize_ = style->GetBoolValue();
            break;
        }
        case K_HEIGHT: {
            hasSetHeight_ = true;
            setResult = false;
            break;
        }
        case K_WIDTH: {
            hasSetWidth_ = true;
            setResult = false;
            break;
        }
        default:
            return false;
    }
    UpdateWidgetFitMode();
    return setResult;
}

void ImageComponent::UpdateResizeMode(uint16_t mode)
{
    switch (mode) {
        case K_COVER:
            resizeMode_ = UIImageView::ImageResizeMode::COVER;
            break;
        case K_CONTAIN:
            resizeMode_ = UIImageView::ImageResizeMode::CONTAIN;
            break;
        case K_FILL:
            resizeMode_ = UIImageView::ImageResizeMode::FILL;
            break;
        case K_NONE:
            resizeMode_ = UIImageView::ImageResizeMode::CENTER;
            break;
        case K_SCALE_DOWN:
            resizeMode_ = UIImageView::ImageResizeMode::SCALE_DOWN;
            break;
        default:
            break;
    }
}
} // namespace ACELite
} // namespace OHOS
