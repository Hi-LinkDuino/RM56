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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_SPAN_RENDER_TEXT_SPAN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_SPAN_RENDER_TEXT_SPAN_H

#include "core/components/common/properties/text_style.h"
#include "core/components/text_span/text_span_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderTextSpan : public RenderNode {
    DECLARE_ACE_TYPE(RenderTextSpan, RenderNode);

public:
    ~RenderTextSpan() override;

    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    // The layout of RenderTextSpan is performed by the RenderText parent node.
    void PerformLayout() override {};

    bool IsCallbackCalled() const;
    const TextStyle& GetSpanStyle() const;
    const std::string& GetSpanData() const;

protected:
    TextStyle spanStyle_;
    bool isCallbackCalled_ = false;
    RefPtr<TextSpanComponent> spanComponent_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_SPAN_RENDER_TEXT_SPAN_H
