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

#include "core/components/qrcode/render_qrcode.h"

namespace OHOS::Ace {
namespace {

constexpr uint32_t QRCODE_START_INDEX = 0;
constexpr uint32_t QRCODE_VALUE_MAX_LENGTH = 256;

}

void RenderQrcode::Update(const RefPtr<Component>& component)
{
    qrcode_ = AceType::DynamicCast<QrcodeComponent>(component);
    if (!qrcode_) {
        LOGE("Update error, qrcode component is null");
        return;
    }
    value_ = qrcode_->GetValue();
    if (value_.empty()) {
        return;
    }
    if (value_.size() > QRCODE_VALUE_MAX_LENGTH) {
        value_ = value_.substr(QRCODE_START_INDEX, QRCODE_VALUE_MAX_LENGTH);
    }
    color_ = qrcode_->GetQrcodeColor();
    backgroundColor_ = qrcode_->GetBackgroundColor();
}

void RenderQrcode::PerformLayout()
{
    if (!qrcode_) {
        return;
    }
    width_ = NormalizePercentToPx(qrcode_->GetQrcodeWidth(), false);
    height_ = NormalizePercentToPx(qrcode_->GetQrcodeHeight(), true);
    auto isWidthDefined = qrcode_->IsWidthDefined();
    auto isHeightDefined = qrcode_->IsHeightDefined();
    if (isWidthDefined && !isHeightDefined) {
        height_ = width_;
    } else if (isHeightDefined && !isWidthDefined) {
        width_ = height_;
    } else if (isHeightDefined && isWidthDefined) {
        if (LessOrEqual(width_, height_)) {
            height_ = width_;
        } else {
            width_ = height_;
        }
    }

    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        LayoutParam layoutParam = GetLayoutParam();
        Size maxSize = layoutParam.GetMaxSize();
        if (maxSize.IsInfinite()) {
            width_ = 0.0;
            height_ = 0.0;
        } else {
            width_ = maxSize.Width();
            height_ = maxSize.Height();
            if (LessOrEqual(width_, height_)) {
                height_ = width_;
            } else {
                width_ = height_;
            }
        }
    }
    SetLayoutSize(Size(width_, height_));
}

} // namespace OHOS::Ace
