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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MULTIMODAL_FLUTTER_RENDER_MULTIMODAL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MULTIMODAL_FLUTTER_RENDER_MULTIMODAL_H

#include "flutter/third_party/txt/src/txt/paragraph.h"

#include "core/components/multimodal/render_multimodal.h"

namespace OHOS::Ace {

class FlutterRenderMultimodal : public RenderMultimodal {
    DECLARE_ACE_TYPE(FlutterRenderMultimodal, RenderMultimodal);

public:
    void Paint(RenderContext& context, const Offset& offset) override;

private:
    void UpdateParagraph(const Offset& offset, const std::string& text);

    std::unique_ptr<txt::Paragraph> paragraph_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MULTIMODAL_FLUTTER_RENDER_MULTIMODAL_H
