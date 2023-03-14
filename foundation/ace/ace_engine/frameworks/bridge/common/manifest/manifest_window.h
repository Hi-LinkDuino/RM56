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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_WINDOW_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_WINDOW_H

#include "base/json/json_util.h"
#include "base/memory/referenced.h"
#include "core/common/frontend.h"

namespace OHOS::Ace::Framework {

class ManifestWindow : public Referenced {
public:
    ManifestWindow() = default;
    ~ManifestWindow() override = default;

    void WindowParse(const std::unique_ptr<JsonValue>& root);
    void PrintInfo();

    WindowConfig& GetWindowConfig()
    {
        return windowConfig_;
    }

private:
    WindowConfig windowConfig_ = { .designWidth = DEFAULT_DESIGN_WIDTH, .autoDesignWidth = false };

    ACE_DISALLOW_COPY_AND_MOVE(ManifestWindow);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_WINDOW_H
