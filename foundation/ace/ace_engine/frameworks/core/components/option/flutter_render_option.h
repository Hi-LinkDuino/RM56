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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_OPTION_FLUTTER_RENDER_OPTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_OPTION_FLUTTER_RENDER_OPTION_H

#include "core/components/option/render_option.h"

namespace OHOS::Ace {

class FlutterRenderOption final : public RenderOption {
    DECLARE_ACE_TYPE(FlutterRenderOption, RenderOption);

public:
    FlutterRenderOption() = default;
    ~FlutterRenderOption() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;

    void PaintLine(RenderContext& context, const Offset& offset);
    void PaintBackground(RenderContext& context, const Offset& offset);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_OPTION_FLUTTER_RENDER_OPTION_H
