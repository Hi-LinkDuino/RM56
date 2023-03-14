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

#include "core/components/declaration/qrcode/qrcode_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/components/qrcode/qrcode_theme.h"

namespace OHOS::Ace {

using namespace Framework;

void QrcodeDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_QRCODE_ATTR);
    AddSpecializedStyle(DeclarationConstants::DEFAULT_QRCODE_STYLE);
}

void QrcodeDeclaration::InitializeStyle()
{
    auto qrcodeTheme = GetTheme<QrcodeTheme>();
    if (!qrcodeTheme) {
        return;
    }
    SetQrcodeColor(qrcodeTheme->GetQrcodeColor());
    SetBackgroundColor(qrcodeTheme->GetBackgroundColor());
    SetQrcodeType(qrcodeTheme->GetQrcodeType());
    SetQrcodeWidth(qrcodeTheme->GetQrcodeWidth());
    SetQrcodeHeight(qrcodeTheme->GetQrcodeHeight());
}

bool QrcodeDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(QrcodeDeclaration&, const std::string&)> qrcodeAttrOperators[] = {
        { DOM_QRCODE_TYPE,
            [](QrcodeDeclaration& declaration, const std::string& value) {
                declaration.SetQrcodeType(ConvertStrToQrcodeType(value));
            } },
        { DOM_QRCODE_VALUE,
            [](QrcodeDeclaration& declaration, const std::string& value) { declaration.SetValue(value); } },
    };
    auto operatorIter = BinarySearchFindIndex(qrcodeAttrOperators, ArraySize(qrcodeAttrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        qrcodeAttrOperators[operatorIter].value(*this, attr.second);
        return true;
    }
    return false;
}

bool QrcodeDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    const static LinearMapNode<void (*)(QrcodeDeclaration&, const std::string&)> qrcodeOperators[] = {
        { DOM_QRCODE_BACKGROUND_COLOR,
            [](QrcodeDeclaration& declaration, const std::string& value) {
                declaration.SetBackgroundColor(declaration.ParseColor(value));
            } },
        { DOM_QRCODE_COLOR,
            [](QrcodeDeclaration& declaration, const std::string& value) {
                declaration.SetQrcodeColor(declaration.ParseColor(value));
            } },
        { DOM_QRCODE_HEIGHT,
            [](QrcodeDeclaration& declaration, const std::string& value) {
                declaration.SetQrcodeHeight(declaration.ParseDimension(value));
            } },
        { DOM_QRCODE_WIDTH,
            [](QrcodeDeclaration& declaration, const std::string& value) {
                declaration.SetQrcodeWidth(declaration.ParseDimension(value));
            } },
    };
    auto operatorIter = BinarySearchFindIndex(qrcodeOperators, ArraySize(qrcodeOperators), style.first.c_str());
    if (operatorIter != -1) {
        qrcodeOperators[operatorIter].value(*this, style.second);
        return true;
    }
    return false;
}

} // namespace OHOS::Ace
