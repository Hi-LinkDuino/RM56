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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_FLUTTER_RENDER_WEB_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_FLUTTER_RENDER_WEB_H

#include "core/components/common/properties/color.h"
#include "core/components/web/render_web.h"
#include "flutter/fml/memory/ref_counted.h"

namespace OHOS::Ace {

class FlutterRenderWeb final : public RenderWeb {
    DECLARE_ACE_TYPE(FlutterRenderWeb, RenderWeb);

public:
    void DumpTree(int32_t depth) override;
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM) and defined(OHOS_STANDARD_SYSTEM)
    void OnPaintFinish() override;
#endif

private:
    void PerformLayout() override;
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM) and defined(OHOS_STANDARD_SYSTEM)
    bool isCreateWebView_ = false;
#endif
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_FLUTTER_RENDER_WEB_H
