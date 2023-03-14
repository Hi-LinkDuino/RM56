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

#include "core/components/qrcode/qrcode_component.h"

#include "core/components/qrcode/qrcode_element.h"
#include "core/components/qrcode/render_qrcode.h"

namespace OHOS::Ace {

QrcodeComponent::QrcodeComponent() : SoleChildComponent()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<QrcodeDeclaration>();
        declaration_->Init();
    }
}

RefPtr<RenderNode> QrcodeComponent::CreateRenderNode()
{
    return RenderQrcode::Create();
}

RefPtr<Element> QrcodeComponent::CreateElement()
{
    return AceType::MakeRefPtr<QrcodeElement>();
}

const std::string& QrcodeComponent::GetValue() const
{
    return declaration_->GetValue();
}

const Color& QrcodeComponent::GetBackgroundColor() const
{
    return declaration_->GetBackgroundColor();
}

const Color& QrcodeComponent::GetQrcodeColor() const
{
    return declaration_->GetQrcodeColor();
}

QrcodeType QrcodeComponent::GetType() const
{
    return declaration_->GetType();
}

const Dimension& QrcodeComponent::GetQrcodeWidth() const
{
    return declaration_->GetQrcodeWidth();
}

const Dimension& QrcodeComponent::GetQrcodeHeight() const
{
    return declaration_->GetQrcodeHeight();
}

bool QrcodeComponent::IsWidthDefined() const
{
    return declaration_->IsWidthDefined();
}

bool QrcodeComponent::IsHeightDefined() const
{
    return declaration_->IsHeightDefined();
}

void QrcodeComponent::SetValue(const std::string& value)
{
    declaration_->SetValue(value);
}

void QrcodeComponent::SetBackgroundColor(const Color& backgroundColor)
{
    declaration_->SetBackgroundColor(backgroundColor);
}

void QrcodeComponent::SetQrcodeColor(const Color& qrcodeColor)
{
    declaration_->SetQrcodeColor(qrcodeColor);
}

void QrcodeComponent::SetQrcodeType(QrcodeType qrcodeType)
{
    declaration_->SetQrcodeType(qrcodeType);
}

void QrcodeComponent::SetQrcodeWidth(const Dimension& qrcodeWidth)
{
    declaration_->SetQrcodeWidth(qrcodeWidth);
}

void QrcodeComponent::SetQrcodeHeight(const Dimension& qrcodeHeight)
{
    declaration_->SetQrcodeHeight(qrcodeHeight);
}

void QrcodeComponent::SetWidthDefined(bool isWidthDefined)
{
    declaration_->SetWidthDefined(isWidthDefined);
}

void QrcodeComponent::SetHeightDefined(bool isHeightDefined)
{
    declaration_->SetHeightDefined(isHeightDefined);
}

void QrcodeComponent::SetDeclaration(const RefPtr<QrcodeDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
