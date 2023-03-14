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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RICHTEXT_RICHTEXT_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RICHTEXT_RICHTEXT_ELEMENT_H

#include "core/pipeline/base/render_element.h"
#include "core/components/rich_text/resource/rich_text_delegate.h"

namespace OHOS::Ace {

class RichTextElement : public RenderElement {
    DECLARE_ACE_TYPE(RichTextElement, RenderElement);

public:
    ~RichTextElement() override;
    void SetNewComponent(const RefPtr<Component>& newComponent) override;

    void Prepare(const WeakPtr<Element>& parent) override;
    void SetDelegate(const RefPtr<RichTextDelegate>& delegate)
    {
        delegate_ = delegate;
    }

    const RefPtr<RichTextDelegate>& GetDelegate() const
    {
        return delegate_;
    }
private:
    std::string richTextData_;
    RefPtr<RichTextDelegate> delegate_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RICHTEXT_RICHTEXT_ELEMENT_H
