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
#include "acelite_config.h"

#if (FEATURE_COMPONENT_QRCODE == 1)
#include "qrcode_component.h"
#include "ace_log.h"
#include "ace_mem_base.h"
#include "js_app_context.h"
#include "js_fwk_common.h"
#include "key_parser.h"
#include "keys.h"

namespace OHOS {
namespace ACELite {
QrcodeComponent::QrcodeComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *manager)
    : Component(options, children, manager), qrcodeValue_(nullptr), qrcodeBackGroundColor_(Color::White()),
      qrcodeColor_(Color::Black()),codeType_(1)
{
    SetComponentName(K_QRCODE);
}

void QrcodeComponent::ReleaseNativeViews()
{
    ACE_FREE(qrcodeValue_);
}

bool QrcodeComponent::SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue)
{
    bool setResult = true;
    switch (attrKeyId) {
        case K_VALUE: {
            ACE_FREE(qrcodeValue_);
            qrcodeValue_ = MallocStringOf(attrValue);
            break;
        }
        case K_BES_TYPE: {
            codeType_ = IntegerOf(attrValue);
            qrcode_.SetQrcodeRange(codeType_);
            break;
        }
        default: {
            setResult = false;
            break;
        }
    }
    return setResult;
}

void QrcodeComponent::PostUpdate(uint16_t attrKeyId)
{
    switch (attrKeyId) {
        case K_BACKGROUND_COLOR:
        // same as below
        case K_BORDER_WIDTH:
        // same as below
        case K_COLOR:
        // same as below
        case K_HEIGHT:
        // same as below
        case K_WIDTH:
        // same as below
        case K_VALUE: {
            DrawQrcode();
            break;
        }
        default:
            break;
    }
}

bool QrcodeComponent::ApplyPrivateStyle(const AppStyleItem *style)
{
    bool result = true;
    uint16_t styleKey = GetStylePropNameId(style);
    switch (styleKey) {
        case K_BACKGROUND_COLOR: {
            result = SetColor(*style, qrcodeBackGroundColor_);
            break;
        }
        case K_COLOR: {
            result = SetColor(*style, qrcodeColor_);
            break;
        }
        default: {
            result = false;
            break;
        }
    }
    return result;
}

void QrcodeComponent::OnViewAttached()
{
    DrawQrcode();
}

bool QrcodeComponent::SetColor(const AppStyleItem &style, ColorType &colortype)
{
    uint32_t color = 0;
    uint8_t alpha = 255;
    if (!GetStyleColorValue(&style, color, alpha)) {
        return false;
    }
    colortype = GetRGBColor(color);
    return true;
}

void QrcodeComponent::DrawQrcode()
{
    const uint16_t defaultWidth = GetWidth();
    const uint16_t defaultHeight = GetHeight();
    uint16_t length = static_cast<uint16_t>(strlen(qrcodeValue_));
    if (length > QRCODE_VAL_MAX || length == 0) {
        HILOG_ERROR(HILOG_MODULE_ACE, "Invalid value. target size[%{public}d]", length);
        return;
    }

    qrcode_.SetQrcodeInfo(qrcodeValue_, qrcodeBackGroundColor_, qrcodeColor_);
    qrcode_.SetWidth(defaultWidth);
    qrcode_.SetHeight(defaultHeight);
}
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_COMPONENT_QRCODE
