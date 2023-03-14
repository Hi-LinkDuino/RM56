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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_WIDGET_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_WIDGET_H

#include <string>
#include <unordered_map>

#include "base/json/json_util.h"
#include "base/log/log.h"
#include "base/memory/referenced.h"

namespace OHOS::Ace::Framework {

class ManifestWidget : public Referenced {
public:
    ManifestWidget(const std::string& name, const std::string& path) : widgetName_(name), widgetPath_(path) {}
    ~ManifestWidget() override = default;

    // widget description.
    const std::string& GetWidgetName() const;
    const std::string& GetWidgetPath() const;

private:
    std::string widgetName_;
    std::string widgetPath_;

    ACE_DISALLOW_COPY_AND_MOVE(ManifestWidget);
};

class ManifestWidgetGroup : public Referenced {
public:
    ManifestWidgetGroup() = default;
    ~ManifestWidgetGroup() override = default;

    void WidgetParse(const std::unique_ptr<JsonValue>& root);

    uint32_t GetWidgetNum() const
    {
        return static_cast<uint32_t>(widgets_.size());
    }

    const std::unordered_map<std::string, RefPtr<ManifestWidget>>& GetWidgetList() const
    {
        return widgets_;
    }

private:
    std::unordered_map<std::string, RefPtr<ManifestWidget>> widgets_;

    ACE_DISALLOW_COPY_AND_MOVE(ManifestWidgetGroup);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_WIDGET_H
