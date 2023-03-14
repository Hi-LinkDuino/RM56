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

#include "bundle_extractor.h"

#include "app_log_wrapper.h"
#include "bundle_constants.h"

namespace OHOS {
namespace AppExecFwk {
BundleExtractor::BundleExtractor(const std::string &source) : BaseExtractor(source)
{
    APP_LOGI("BundleExtractor is created");
}

BundleExtractor::~BundleExtractor()
{
    APP_LOGI("BundleExtractoris destroyed");
}

bool BundleExtractor::ExtractProfile(std::ostream &dest) const
{
    if (IsNewVersion()) {
        APP_LOGD("profile is module.json");
        return ExtractByName(Constants::MODULE_PROFILE_NAME, dest);
    }
    APP_LOGD("profile is config.json");
    return ExtractByName(Constants::BUNDLE_PROFILE_NAME, dest);
}

bool BundleExtractor::ExtractPackFile(std::ostream &dest) const
{
    APP_LOGD("start to parse pack.info");
    return ExtractByName(Constants::BUNDLE_PACKFILE_NAME, dest);
}
}  // namespace AppExecFwk
}  // namespace OHOS
