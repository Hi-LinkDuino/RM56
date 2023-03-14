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

#include "bundle_parser.h"

#include <fstream>
#include <sstream>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_extractor.h"
#include "bundle_profile.h"
#include "default_permission_profile.h"
#include "module_profile.h"
#include "pre_bundle_profile.h"
#include "rpcid_decode/syscap_tool.h"
#include "securec.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string INSTALL_ABILITY_CONFIGS = "install_ability_configs";

bool ParseStr(const char *buf, const int itemLen, int totalLen, std::vector<std::string> &sysCaps)
{
    APP_LOGD("Parse rpcid output start, itemLen:%{public}d  totalLen:%{public}d.", itemLen, totalLen);
    if (buf == nullptr || itemLen <= 0 || totalLen <= 0) {
        APP_LOGE("param invalid.");
        return false;
    }

    int index = 0;
    while (index + itemLen <= totalLen) {
        char item[itemLen];
        if (strncpy_s(item, sizeof(item), buf + index, itemLen) != 0) {
            APP_LOGE("Parse rpcid failed due to strncpy_s error.");
            return false;
        }

        sysCaps.emplace_back((std::string)item, 0, itemLen);
        index += itemLen;
    }

    return true;
}

bool ReadFileIntoJson(const std::string &filePath, nlohmann::json &jsonBuf)
{
    if (access(filePath.c_str(), F_OK) != 0) {
        APP_LOGE("can not access the file: %{public}s", filePath.c_str());
        return false;
    }

    std::fstream in;
    char errBuf[256];
    errBuf[0] = '\0';
    in.open(filePath, std::ios_base::in);
    if (!in.is_open()) {
        strerror_r(errno, errBuf, sizeof(errBuf));
        APP_LOGE("the file cannot be open due to  %{public}s", errBuf);
        return false;
    }

    in.seekg(0, std::ios::end);
    int64_t size = in.tellg();
    if (size <= 0) {
        APP_LOGE("the file is an empty file");
        in.close();
        return false;
    }

    in.seekg(0, std::ios::beg);
    jsonBuf = nlohmann::json::parse(in, nullptr, false);
    in.close();
    if (jsonBuf.is_discarded()) {
        APP_LOGE("bad profile file");
        return false;
    }

    return true;
}
} // namespace

ErrCode BundleParser::Parse(const std::string &pathName, InnerBundleInfo &innerBundleInfo) const
{
    APP_LOGI("parse from %{private}s", pathName.c_str());
    BundleExtractor bundleExtractor(pathName);
    if (!bundleExtractor.Init()) {
        APP_LOGE("bundle extractor init failed");
        return ERR_APPEXECFWK_PARSE_UNEXPECTED;
    }

    // to extract config.json
    std::ostringstream outStream;
    if (!bundleExtractor.ExtractProfile(outStream)) {
        APP_LOGE("extract profile file failed");
        return ERR_APPEXECFWK_PARSE_NO_PROFILE;
    }

    if (bundleExtractor.IsNewVersion()) {
        APP_LOGD("module.json transform to InnerBundleInfo");
        innerBundleInfo.SetIsNewVersion(true);
        ModuleProfile moduleProfile;
        return moduleProfile.TransformTo(outStream, bundleExtractor, innerBundleInfo);
    }
    APP_LOGD("config.json transform to InnerBundleInfo");
    innerBundleInfo.SetIsNewVersion(false);
    BundleProfile bundleProfile;
    ErrCode ret = bundleProfile.TransformTo(outStream, bundleExtractor, innerBundleInfo);
    if (ret != ERR_OK) {
        APP_LOGE("transform stream to innerBundleInfo failed %{public}d", ret);
        return ret;
    }
    auto& abilityInfos = innerBundleInfo.FetchAbilityInfos();
    for (auto& info : abilityInfos) {
        info.second.isStageBasedModel = bundleExtractor.IsStageBasedModel(info.second.name);
        auto iter = innerBundleInfo.FetchInnerModuleInfos().find(info.second.package);
        if (iter != innerBundleInfo.FetchInnerModuleInfos().end()) {
            iter->second.isStageBasedModel = info.second.isStageBasedModel;
        }
    }

    return ERR_OK;
}

ErrCode BundleParser::ParsePackInfo(const std::string &pathName, BundlePackInfo &bundlePackInfo) const
{
    APP_LOGI("parse from %{private}s", pathName.c_str());
    BundleExtractor bundleExtractor(pathName);
    if (!bundleExtractor.Init()) {
        APP_LOGE("bundle extractor init failed");
        return ERR_APPEXECFWK_PARSE_UNEXPECTED;
    }

    // to extract pack.info
    if (!bundleExtractor.HasEntry(Constants::BUNDLE_PACKFILE_NAME)) {
        APP_LOGW("cannot find pack.info in the hap file");
        return ERR_OK;
    }
    std::ostringstream outStreamForPackInfo;
    if (!bundleExtractor.ExtractPackFile(outStreamForPackInfo)) {
        APP_LOGE("extract profile file failed");
        return ERR_APPEXECFWK_PARSE_NO_PROFILE;
    }
    BundleProfile bundleProfile;
    ErrCode ret = bundleProfile.TransformTo(outStreamForPackInfo, bundlePackInfo);
    if (ret != ERR_OK) {
        APP_LOGE("transform stream to bundlePackinfo failed %{public}d", ret);
        return ret;
    }
    return ERR_OK;
}

ErrCode BundleParser::ParseSysCap(const std::string &pathName, std::vector<std::string> &sysCaps) const
{
    APP_LOGD("Parse sysCaps from %{private}s", pathName.c_str());
    BundleExtractor bundleExtractor(pathName);
    if (!bundleExtractor.Init()) {
        APP_LOGE("Bundle extractor init failed");
        return ERR_APPEXECFWK_PARSE_UNEXPECTED;
    }

    if (!bundleExtractor.HasEntry(Constants::SYSCAP_NAME)) {
        APP_LOGD("Rpcid.sc is not exist, and do not need verification sysCaps.");
        return ERR_OK;
    }

    std::stringstream rpcidStream;
    if (!bundleExtractor.ExtractByName(Constants::SYSCAP_NAME, rpcidStream)) {
        APP_LOGE("Extract rpcid file failed");
        return ERR_APPEXECFWK_PARSE_RPCID_FAILED;
    }

    int32_t rpcidLen = rpcidStream.tellp();
    if (rpcidLen < 0) {
        return ERR_APPEXECFWK_PARSE_UNEXPECTED;
    }
    char rpcidBuf[rpcidLen];
    rpcidStream.read(rpcidBuf, rpcidLen);
    uint32_t outLen;
    char *outBuffer;
    int result = RPCIDStreamDecodeToBuffer(rpcidBuf, rpcidLen, &outBuffer, &outLen);
    if (result != 0) {
        APP_LOGE("Decode syscaps failed");
        return ERR_APPEXECFWK_PARSE_RPCID_FAILED;
    }

    if (!ParseStr(outBuffer, SINGLE_SYSCAP_LENGTH, outLen, sysCaps)) {
        APP_LOGE("Parse syscaps str failed");
        free(outBuffer);
        return ERR_APPEXECFWK_PARSE_RPCID_FAILED;
    }

    APP_LOGD("Parse sysCaps str success");
    free(outBuffer);
    return ERR_OK;
}

ErrCode BundleParser::ParsePreInstallConfig(
    const std::string &configFile, std::set<PreScanInfo> &scanInfos) const
{
    APP_LOGD("Parse preInstallConfig from %{public}s", configFile.c_str());
    nlohmann::json jsonBuf;
    if (!ReadFileIntoJson(configFile, jsonBuf)) {
        APP_LOGE("Parse preInstallConfig file failed");
        return ERR_APPEXECFWK_PARSE_FILE_FAILED;
    }

    PreBundleProfile preBundleProfile;
    return preBundleProfile.TransformTo(jsonBuf, scanInfos);
}

ErrCode BundleParser::ParsePreUnInstallConfig(
    const std::string &configFile, std::set<std::string> &bundleNames) const
{
    APP_LOGD("Parse PreUnInstallConfig from %{public}s", configFile.c_str());
    nlohmann::json jsonBuf;
    if (!ReadFileIntoJson(configFile, jsonBuf)) {
        APP_LOGE("Parse preUnInstallConfig file failed");
        return ERR_APPEXECFWK_PARSE_FILE_FAILED;
    }

    PreBundleProfile preBundleProfile;
    return preBundleProfile.TransformTo(jsonBuf, bundleNames);
}

ErrCode BundleParser::ParsePreInstallAbilityConfig(
    const std::string &configFile, std::set<PreBundleConfigInfo> &preBundleConfigInfos) const
{
    APP_LOGD("Parse PreInstallAbilityConfig from %{public}s", configFile.c_str());
    nlohmann::json jsonBuf;
    if (!ReadFileIntoJson(configFile, jsonBuf)) {
        APP_LOGE("Parse preInstallAbilityConfig file failed");
        return ERR_APPEXECFWK_PARSE_FILE_FAILED;
    }

    PreBundleProfile preBundleProfile;
    return preBundleProfile.TransformTo(jsonBuf, preBundleConfigInfos);
}

ErrCode BundleParser::ParseDefaultPermission(std::vector<DefaultPermission> &defaultPermissions) const
{
    APP_LOGD("Parse DefaultPermission from %{private}s", Constants::INSTALL_LIST_PERMISSIONS_FILE_PATH.c_str());
    nlohmann::json jsonBuf;
    if (!ReadFileIntoJson(Constants::INSTALL_LIST_PERMISSIONS_FILE_PATH, jsonBuf)) {
        APP_LOGE("Parse default-permissions file failed");
        return ERR_APPEXECFWK_PARSE_FILE_FAILED;
    }
    DefaultPermissionProfile profile;
    return profile.TransformTo(jsonBuf, defaultPermissions);
}
}  // namespace AppExecFwk
}  // namespace OHOS
