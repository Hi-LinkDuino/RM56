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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_PARSER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_PARSER_H

#include <string>

#include "base/log/log.h"
#include "base/memory/referenced.h"
#include "base/utils/noncopyable.h"
#include "frameworks/bridge/common/manifest/manifest_appinfo.h"
#include "frameworks/bridge/common/manifest/manifest_router.h"
#include "frameworks/bridge/common/manifest/manifest_widget.h"
#include "frameworks/bridge/common/manifest/manifest_window.h"

namespace OHOS::Ace::Framework {

class ACE_EXPORT ManifestParser : public Referenced {
public:
    ManifestParser();
    ~ManifestParser() override = default;

    const RefPtr<ManifestAppInfo>& GetAppInfo() const;
    const RefPtr<ManifestRouter>& GetRouter() const;
    const RefPtr<ManifestWidgetGroup>& GetWidget() const;
    WindowConfig& GetWindowConfig();

    bool IsWebFeature() const
    {
        return webFeature_;
    }
    bool IsUseLiteStyle() const
    {
        return useLiteStyle_;
    }
    int32_t GetMinPlatformVersion() const
    {
        return manifestAppInfo_->GetMinPlatformVersion();
    }

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    void SetPagePath(const std::string& pagePath)
    {
        if (manifestRouter_) {
            manifestRouter_->InsertPageList(pagePath);
        }
    }
#endif

    void Parse(const std::string& url);
    void Printer();

private:
    RefPtr<ManifestAppInfo> manifestAppInfo_;
    RefPtr<ManifestRouter> manifestRouter_;
    RefPtr<ManifestWidgetGroup> manifestWidget_;
    RefPtr<ManifestWindow> manifestWindow_;
    bool webFeature_ = false;
    bool useLiteStyle_ = false;

    ACE_DISALLOW_COPY_AND_MOVE(ManifestParser);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_PARSER_H
