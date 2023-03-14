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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_STOP_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_STOP_COMPONENT_H

#include "core/components/declaration/svg/svg_stop_declaration.h"
#include "frameworks/core/pipeline/base/composed_component.h"

namespace OHOS::Ace {

class SvgStopComponent : public ComposedComponent {
    DECLARE_ACE_TYPE(SvgStopComponent, ComposedComponent);

public:
    SvgStopComponent(const ComposeId& id, const std::string& name) : ComposedComponent(id, name)
    {
        declaration_ = AceType::MakeRefPtr<SvgStopDeclaration>();
        declaration_->Init();
        declaration_->InitializeStyle();
    }

    ~SvgStopComponent() override = default;


    const GradientColor& GetGradientColor() const
    {
        return declaration_->GetGradientColor();
    }

    const RefPtr<SvgStopDeclaration>& GetDeclaration() const
    {
        return declaration_;
    }

private:
    RefPtr<SvgStopDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_STOP_COMPONENT_H
