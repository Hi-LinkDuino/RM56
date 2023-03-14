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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_APPINFO_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_APPINFO_H

#include <string>

#include "base/json/json_util.h"
#include "base/log/log.h"
#include "base/memory/referenced.h"

namespace OHOS::Ace::Framework {

class ManifestAppInfo : public Referenced {
public:
    ManifestAppInfo() = default;
    ~ManifestAppInfo() override = default;

    // JS application description.
    const std::string& GetAppID() const;
    const std::string& GetAppName() const;
    const std::string& GetIcon() const;
    const std::string& GetVersionName() const;
    uint32_t GetVersionCode() const;
    int32_t GetMinPlatformVersion() const
    {
        return minPlatformVersion_;
    }

    // Global config information.
    const std::string& GetLogLevel() const;

    void AppInfoParse(const std::unique_ptr<JsonValue>& root);

    void ParseI18nJsonInfo();

private:
    std::string appName_;
    std::string versionName_;
    uint32_t versionCode_ = 0;
    std::string logLevel_;
    std::string icon_;
    std::string appID_;
    int32_t minPlatformVersion_ = 0;

    ACE_DISALLOW_COPY_AND_MOVE(ManifestAppInfo);
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_MANIFEST_MANIFEST_APPINFO_H
