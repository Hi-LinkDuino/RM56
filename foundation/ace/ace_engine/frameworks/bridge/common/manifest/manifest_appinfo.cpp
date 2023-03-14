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

#include "frameworks/bridge/common/manifest/manifest_appinfo.h"

#include "core/common/container.h"
#include "core/components/theme/theme_manager.h"

namespace OHOS::Ace::Framework {

namespace {

const std::string APP_NAME_PREFIX = "$string:";

std::string ParseI18nAppName(std::string& rawAppName)
{
    if (!StringUtils::StartWith(rawAppName, APP_NAME_PREFIX) || rawAppName.size() <= APP_NAME_PREFIX.size()) {
        LOGW("not i18n appName");
        return rawAppName;
    }

    auto container = Container::Current();
    if (!container) {
        LOGW("container is null");
        return rawAppName;
    }
    auto pipelineContext = container->GetPipelineContext();
    if (!pipelineContext) {
        LOGE("pipelineContext is null!");
        return rawAppName;
    }
    auto themeManager = pipelineContext->GetThemeManager();
    if (!themeManager) {
        LOGE("themeManager is null!");
        return rawAppName;
    }

    auto resourceName = rawAppName.substr(APP_NAME_PREFIX.size());
    auto themeConstants = themeManager->GetThemeConstants();
    uint32_t resId = 0;
    auto ret = themeConstants->GetResourceIdByName(resourceName, "string", resId);
    if (!ret) {
        LOGW("GetResourceIdByName failed");
        return rawAppName;
    }
    return themeConstants->GetString(resId);
}

} // namespace

const std::string& ManifestAppInfo::GetAppID() const
{
    return appID_;
}

const std::string& ManifestAppInfo::GetAppName() const
{
    return appName_;
}

const std::string& ManifestAppInfo::GetIcon() const
{
    return icon_;
}

const std::string& ManifestAppInfo::GetVersionName() const
{
    return versionName_;
}

uint32_t ManifestAppInfo::GetVersionCode() const
{
    return versionCode_;
}

const std::string& ManifestAppInfo::GetLogLevel() const
{
    return logLevel_;
}

void ManifestAppInfo::AppInfoParse(const std::unique_ptr<JsonValue>& root)
{
    if (!root) {
        return;
    }

    appName_ = root->GetString("appName");
    versionName_ = root->GetString("versionName");
    versionCode_ = root->GetUInt("versionCode");
    logLevel_ = root->GetString("logLevel");
    icon_ = root->GetString("icon");
    appID_ = root->GetString("appID");
    minPlatformVersion_ = root->GetInt("minPlatformVersion", 0);
}

void ManifestAppInfo::ParseI18nJsonInfo()
{
    appName_ = ParseI18nAppName(appName_);
}

} // namespace OHOS::Ace::Framework
