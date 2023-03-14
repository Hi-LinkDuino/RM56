/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "bundle_state_storage.h"

#include <fstream>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_util.h"
#include "installd_client.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string::size_type EXPECT_SPLIT_SIZE = 2;

void NameAndUserIdToKey(
    const std::string &bundleName, int32_t userId, std::string &key)
{
    key.append(bundleName);
    key.append(Constants::FILE_UNDERLINE);
    key.append(std::to_string(userId));
    APP_LOGD("bundleName = %{public}s", bundleName.c_str());
}

bool KeyToNameAndUserId(
    const std::string &key, std::string &bundleName, int32_t &userId)
{
    bool ret = false;
    std::vector<std::string> splitStrs;
    OHOS::SplitStr(key, Constants::FILE_UNDERLINE, splitStrs);
    if (splitStrs.size() == EXPECT_SPLIT_SIZE) {
        bundleName = splitStrs[0];
        userId = atoi(splitStrs[1].c_str());
        ret = true;
    }

    APP_LOGD("bundleName = %{public}s", bundleName.c_str());
    return ret;
}
}

bool BundleStateStorage::HasBundleUserInfoJsonDb()
{
    APP_LOGD("HasBundleUserInfoJsonDb start");
    if (BundleUtil::IsExistFile(Constants::BUNDLE_USER_INFO_PATH)) {
        APP_LOGD("Json db exist");
        return true;
    }

    APP_LOGD("Json db not exist, and create it");
    bool isDirExist = BundleUtil::IsExistDir(Constants::BUNDLE_MANAGER_SERVICE_PATH);
    if (!isDirExist) {
        ErrCode result = InstalldClient::GetInstance()->CreateBundleDir(
            Constants::BUNDLE_MANAGER_SERVICE_PATH);
        if (result != ERR_OK) {
            APP_LOGE("fail to create dir, error is %{public}d", result);
            return false;
        }
    }

    auto file = fopen(Constants::BUNDLE_USER_INFO_PATH.c_str(), "at++");
    if (file == nullptr) {
        APP_LOGE("create json db failed");
        return false;
    }

    auto ret = fclose(file);
    if (ret != 0) {
        APP_LOGE("ret: %{public}d", ret);
    }

    return false;
}

bool BundleStateStorage::LoadAllBundleStateData(
    std::map<std::string, std::map<int32_t, BundleUserInfo>> &infos)
{
    APP_LOGD("load all bundle state data to map");
    std::lock_guard<std::mutex> lock(bundleStateMutex_);
    std::fstream i(Constants::BUNDLE_USER_INFO_PATH);
    nlohmann::json jParse;
    if (!i.is_open()) {
        APP_LOGE("failed to open bundle database file");
        return false;
    }

    APP_LOGD("Open bundle state db file success");
    i.seekg(0, std::ios::end);
    int len = static_cast<int>(i.tellg());
    if (len <= 0) {
        APP_LOGD("Tellg failed.");
        i.close();
        return false;
    }

    i.seekg(0, std::ios::beg);
    jParse = nlohmann::json::parse(i, nullptr, false);
    i.close();
    return LoadAllBundleStateDataFromJson(jParse, infos);
}

bool BundleStateStorage::LoadAllBundleStateDataFromJson(
    nlohmann::json &jParse, std::map<std::string, std::map<int32_t, BundleUserInfo>> &infos)
{
    if (jParse.is_discarded()) {
        APP_LOGE("Bad json due to jParse is discarded");
        return false;
    }

    for (auto &item : jParse.items()) {
        std::string bundleName;
        int32_t userId;
        if (!KeyToNameAndUserId(item.key(), bundleName, userId)) {
            continue;
        }

        BundleUserInfo bundleUserInfo;
        nlohmann::json& jsonObject = item.value();
        if (jsonObject.is_discarded()) {
            APP_LOGE("Load failed due to data is discarded");
            continue;
        }

        bundleUserInfo = jsonObject.get<BundleUserInfo>();
        if (infos.find(bundleName) == infos.end()) {
            std::map<int32_t, BundleUserInfo> tempUser;
            tempUser.try_emplace(userId, bundleUserInfo);
            infos.try_emplace(bundleName, tempUser);
            continue;
        }

        auto& bundleUserInfoMaps = infos.at(bundleName);
        if (bundleUserInfoMaps.find(userId) == bundleUserInfoMaps.end()) {
            bundleUserInfoMaps.try_emplace(userId, bundleUserInfo);
            continue;
        }

        bundleUserInfoMaps.at(userId) = bundleUserInfo;
    }

    return !infos.empty();
}

bool BundleStateStorage::SaveBundleStateStorage(
    const std::string bundleName, int32_t userId, const BundleUserInfo &bundleUserInfo)
{
    APP_LOGD("Save bundle state to json db");
    if (bundleName.empty() || userId < 0) {
        APP_LOGE("Save bundle state failed due to param invalid.");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleStateMutex_);
    std::string appName;
    NameAndUserIdToKey(bundleName, userId, appName);
    std::fstream f(Constants::BUNDLE_USER_INFO_PATH);
    if (!f.is_open()) {
        APP_LOGE("failed to open bundle database file");
        return false;
    }

    f.seekg(0, std::ios::end);
    int len = static_cast<int>(f.tellg());
    if (len == 0) {
        nlohmann::json appRoot;
        appRoot[appName] = bundleUserInfo;
        f << std::setw(Constants::DUMP_INDENT) << appRoot << std::endl;
    } else {
        f.seekg(0, std::ios::beg);
        nlohmann::json jsonFile;
        f >> jsonFile;
        jsonFile[appName] = bundleUserInfo;
        f.seekp(0, std::ios::beg);
        f << std::setw(Constants::DUMP_INDENT) << jsonFile << std::endl;
    }

    f.close();
    return true;
}

bool BundleStateStorage::GetBundleStateStorage(
    const std::string bundleName, int32_t userId, BundleUserInfo &bundleUserInfo)
{
    if (bundleName.empty() || userId < 0) {
        APP_LOGE("Get bundle state data failed due to param invalid.");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleStateMutex_);
    std::string appName;
    NameAndUserIdToKey(bundleName, userId, appName);
    std::fstream f(Constants::BUNDLE_USER_INFO_PATH);
    if (!f.is_open()) {
        APP_LOGE("failed to open bundle database file");
        return false;
    }

    f.seekg(0, std::ios::end);
    int len = static_cast<int>(f.tellg());
    if (len == 0) {
        f.close();
        APP_LOGE("failed to open bundle database file due to tellg invalid");
        return false;
    }

    f.seekg(0, std::ios::beg);
    nlohmann::json jsonFile;
    f >> jsonFile;
    if (jsonFile.is_discarded()) {
        f.close();
        APP_LOGE("Get failed due to data is discarded");
        return false;
    }

    bundleUserInfo = jsonFile.get<BundleUserInfo>();
    f.close();
    return true;
}

bool BundleStateStorage::DeleteBundleState(
    const std::string bundleName, int32_t userId)
{
    APP_LOGD("Delete bundle state data");
    if (bundleName.empty() || userId < 0) {
        APP_LOGE("Delete bundle state data failed due to param invalid.");
        return false;
    }

    std::lock_guard<std::mutex> lock(bundleStateMutex_);
    bool isEmpty = false;
    std::string appName;
    NameAndUserIdToKey(bundleName, userId, appName);
    std::ifstream i(Constants::BUNDLE_USER_INFO_PATH);
    nlohmann::json jParse;
    if (!i.is_open()) {
        APP_LOGE("failed to open bundle database file");
        return false;
    }

    i.seekg(0, std::ios::end);
    int len = static_cast<int>(i.tellg());
    if (len == 0) {
        i.close();
        APP_LOGE("file is empty appName = %{private}s", appName.c_str());
        return true;
    }

    i.seekg(0, std::ios::beg);
    i >> jParse;
    if (jParse.find(appName) == jParse.end()) {
        i.close();
        APP_LOGD("not find appName = %{public}s", appName.c_str());
        return true;
    }

    jParse.erase(appName);
    isEmpty = (jParse.size() == 0) ? true : false;
    i.close();
    std::ofstream o(Constants::BUNDLE_USER_INFO_PATH);
    if (!o.is_open()) {
        APP_LOGE("failed to open bundle database file");
        return false;
    }

    if (!isEmpty) {
        o << std::setw(Constants::DUMP_INDENT) << jParse;
    }

    o.close();
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
