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

#include "frameworks/bridge/common/manifest/manifest_parser.h"

namespace OHOS::Ace::Framework {

ManifestParser::ManifestParser()
    : manifestAppInfo_(Referenced::MakeRefPtr<ManifestAppInfo>()),
      manifestRouter_(Referenced::MakeRefPtr<ManifestRouter>()),
      manifestWidget_(Referenced::MakeRefPtr<ManifestWidgetGroup>()),
      manifestWindow_(Referenced::MakeRefPtr<ManifestWindow>())
{}

const RefPtr<ManifestAppInfo>& ManifestParser::GetAppInfo() const
{
    return manifestAppInfo_;
}

const RefPtr<ManifestRouter>& ManifestParser::GetRouter() const
{
    return manifestRouter_;
}

const RefPtr<ManifestWidgetGroup>& ManifestParser::GetWidget() const
{
    return manifestWidget_;
}

WindowConfig& ManifestParser::GetWindowConfig()
{
    return manifestWindow_->GetWindowConfig();
}

void ManifestParser::Parse(const std::string& contents)
{
    auto rootJson = JsonUtil::ParseJsonString(contents);
    if (!rootJson || !rootJson->IsValid()) {
        LOGE("the manifest is illegal");
        return;
    }
    manifestAppInfo_->AppInfoParse(rootJson);
    manifestRouter_->RouterParse(rootJson);
    manifestWidget_->WidgetParse(rootJson);
    manifestWindow_->WindowParse(rootJson);
    webFeature_ = rootJson->GetBool("webFeature", false);
    auto deviceTypes = rootJson->GetValue("deviceType");
    if (deviceTypes && deviceTypes->IsArray()) {
        for (int32_t index = 0; index < deviceTypes->GetArraySize(); ++index) {
            auto device = deviceTypes->GetArrayItem(index);
            if (device && device->IsString() && device->GetString() == "liteWearable") {
                useLiteStyle_ = true;
                break;
            }
        }
    }
}

void ManifestParser::Printer()
{
#ifdef ACE_DEBUG_LOG
    LOGD("appinfo:{");
    LOGD("  Icon:%{private}s", manifestAppInfo_->GetIcon().c_str());
    LOGD("  LogLevel:%{public}s", manifestAppInfo_->GetLogLevel().c_str());
    LOGD("  Name:%{public}s", manifestAppInfo_->GetAppName().c_str());
    LOGD("  AppID:%{public}s", manifestAppInfo_->GetAppID().c_str());
    LOGD("  VersionCode:%{public}d", manifestAppInfo_->GetVersionCode());
    LOGD("  VersionName:%{public}s", manifestAppInfo_->GetVersionName().c_str());
    LOGD("  minPlatformVersion:%{public}d", manifestAppInfo_->GetMinPlatformVersion());
    LOGD("}");

    LOGD("router:{");
    LOGD("  entry:%{private}s", manifestRouter_->GetEntry().c_str());
    LOGD("  pages:{");
    for (const auto& page : manifestRouter_->GetPageList()) {
        LOGD("    %{private}s", page.c_str());
    }
    LOGD("}");

    manifestWindow_->PrintInfo();
#endif
}

} // namespace OHOS::Ace::Framework
