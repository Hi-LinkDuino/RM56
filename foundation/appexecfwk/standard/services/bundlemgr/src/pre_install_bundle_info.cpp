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

#include "pre_install_bundle_info.h"

#include "common_profile.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string BUNDLE_NAME = "bundleName";
const std::string VERSION_CODE = "versionCode";
const std::string BUNDLE_PATHS = "bundlePaths";
const std::string APP_TYPE = "appType";
}  // namespace

void PreInstallBundleInfo::ToJson(nlohmann::json &jsonObject) const
{
    jsonObject[BUNDLE_NAME] = bundleName_;
    jsonObject[VERSION_CODE] = versionCode_;
    jsonObject[BUNDLE_PATHS] = bundlePaths_;
    jsonObject[APP_TYPE] = appType_;
}

int32_t PreInstallBundleInfo::FromJson(const nlohmann::json &jsonObject)
{
    const auto &jsonObjectEnd = jsonObject.end();
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        BUNDLE_NAME,
        bundleName_,
        JsonType::STRING,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<uint32_t>(jsonObject,
        jsonObjectEnd,
        VERSION_CODE,
        versionCode_,
        JsonType::NUMBER,
        true,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        BUNDLE_PATHS,
        bundlePaths_,
        JsonType::ARRAY,
        true,
        ProfileReader::parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<Constants::AppType>(jsonObject,
        jsonObjectEnd,
        APP_TYPE,
        appType_,
        JsonType::NUMBER,
        false,
        ProfileReader::parseResult,
        ArrayType::NOT_ARRAY);
    int32_t ret = ProfileReader::parseResult;
    // need recover parse result to ERR_OK
    ProfileReader::parseResult = ERR_OK;
    return ret;
}

std::string PreInstallBundleInfo::ToString() const
{
    nlohmann::json j;
    j[BUNDLE_NAME] = bundleName_;
    j[VERSION_CODE] = versionCode_;
    j[BUNDLE_PATHS] = bundlePaths_;
    j[APP_TYPE] = appType_;
    return j.dump();
}
}  // namespace AppExecFwk
}  // namespace OHOS
