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

#include "frameworks/bridge/common/manifest/manifest_window.h"

#include "base/log/log.h"

namespace OHOS::Ace::Framework {

void ManifestWindow::WindowParse(const std::unique_ptr<JsonValue>& root)
{
    if (!root) {
        LOGE("the root manifest is nullptr");
        return;
    }
    auto window = root->GetObject("window");
    if (!window || window->IsNull()) {
        LOGD("No window config found.");
        return;
    }
    auto designWidth = window->GetInt("designWidth", DEFAULT_DESIGN_WIDTH);
    if (designWidth <= 0) {
        designWidth = DEFAULT_DESIGN_WIDTH;
    }
    windowConfig_.designWidth = designWidth;
    windowConfig_.autoDesignWidth = window->GetBool("autoDesignWidth", false);
}

void ManifestWindow::PrintInfo()
{
    LOGD("window: {");
    LOGD("  designWidth: %{private}d", windowConfig_.designWidth);
    LOGD("  autoDesignWidth: %{private}d", windowConfig_.autoDesignWidth);
    LOGD("}");
}

} // namespace OHOS::Ace::Framework
