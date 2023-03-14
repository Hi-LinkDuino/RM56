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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RICHTEXT_RICHTEXT_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RICHTEXT_RICHTEXT_COMPONENT_H

#include <string>

#include "base/geometry/size.h"
#include "base/utils/utils.h"
#include "core/components/declaration/web/web_client.h"
#include "core/components/declaration/richtext/rich_text_declaration.h"
#include "core/pipeline/base/element.h"

namespace OHOS::Ace {

class RichTextDelegate;
// A component can show HTML rich-text.
class RichTextComponent : public RenderComponent {
    DECLARE_ACE_TYPE(RichTextComponent, RenderComponent);

public:
    using ErrorCallback = std::function<void(const std::string&, const std::string&)>;

    RichTextComponent() = default;
    explicit RichTextComponent(const std::string& type);
    ~RichTextComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void SetType(const std::string& type)
    {
        type_ = type;
    }

    const std::string& GetType() const
    {
        return type_;
    }

    const std::string& GetData() const
    {
        return declaration_->GetData();
    }

    const EventMarker& GetPageStartedEventId() const
    {
        return declaration_->GetPageStartedEventId();
    }

    const EventMarker& GetPageFinishedEventId() const
    {
        return declaration_->GetPageFinishedEventId();
    }

    const EventMarker& GetPageErrorEventId() const
    {
        return declaration_->GetPageErrorEventId();
    }

    void SetDeclaration(const RefPtr<RichTextDeclaration>& declaration)
    {
        if (declaration) {
            declaration_ = declaration;
        }
    }

    const RefPtr<RichTextDeclaration>& GetDeclaration() const
    {
        return declaration_;
    }

private:
    RefPtr<RichTextDeclaration> declaration_;
    ErrorCallback errorCallback_ = nullptr;
    RefPtr<RichTextDelegate> delegate_;

    std::string type_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RICHTEXT_RICHTEXT_COMPONENT_H
