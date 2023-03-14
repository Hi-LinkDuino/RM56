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

#include "frameworks/bridge/common/manifest/manifest_widget.h"

namespace OHOS::Ace::Framework {

const std::string& ManifestWidget::GetWidgetName() const
{
    return widgetName_;
}

const std::string& ManifestWidget::GetWidgetPath() const
{
    return widgetPath_;
}

void ManifestWidgetGroup::WidgetParse(const std::unique_ptr<JsonValue>& root)
{
    if (!root) {
        return;
    }

    auto widgetInfo = root->GetValue("widgets");
    if (widgetInfo->IsArray()) {
        for (int32_t index = 0; index < widgetInfo->GetArraySize(); ++index) {
            const auto& widget = widgetInfo->GetArrayItem(index);
            const auto& widgetName = widget->GetString("name");
            const auto& widgetPath = widget->GetString("path");

            if (widgetName.empty()) {
                LOGE("Invalid widget name. Need to be a non-empty string.");
                continue;
            } else if (widgetPath.empty()) {
                LOGE("Invalid widget path. Need to be a non-empty string.");
                continue;
            }

            auto result = widgets_.try_emplace(widgetName, MakeRefPtr<ManifestWidget>(widgetName, widgetPath));
            if (!result.second) {
                LOGE("Duplicate Widget Names: %{public}s", widgetName.c_str());
            }
        }
    } else {
        LOGW("Widget Format is invalid");
    }
}

} // namespace OHOS::Ace::Framework
