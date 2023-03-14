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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_QRCODE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_QRCODE_COMPONENT_H

#include "base/geometry/dimension.h"
#include "core/components/common/layout/constants.h"
#include "core/pipeline/base/sole_child_component.h"
#include "core/components/declaration/qrcode/qrcode_declaration.h"

namespace OHOS::Ace {

class ACE_EXPORT QrcodeComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(QrcodeComponent, SoleChildComponent);

public:
    QrcodeComponent();
    ~QrcodeComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    const std::string& GetValue() const;
    const Color& GetBackgroundColor() const;
    const Color& GetQrcodeColor() const;
    QrcodeType GetType() const;
    const Dimension& GetQrcodeWidth() const;
    const Dimension& GetQrcodeHeight() const;
    bool IsWidthDefined() const;
    bool IsHeightDefined() const;
    void SetValue(const std::string& value);
    void SetBackgroundColor(const Color& backgroundColor);
    void SetQrcodeColor(const Color& qrcodeColor);
    void SetQrcodeType(QrcodeType qrcodeType);
    void SetQrcodeWidth(const Dimension& qrcodeWidth);
    void SetQrcodeHeight(const Dimension& qrcodeHeight);
    void SetWidthDefined(bool isWidthDefined);
    void SetHeightDefined(bool isHeightDefined);

    void SetDeclaration(const RefPtr<QrcodeDeclaration>& declaration);

private:
    RefPtr<QrcodeDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_QRCODE_COMPONENT_H
