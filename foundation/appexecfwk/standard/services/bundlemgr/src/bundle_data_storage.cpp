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

#include "bundle_data_storage.h"

#include <fstream>
#include <iomanip>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_profile.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
bool BundleDataStorage::LoadAllData(std::map<std::string, InnerBundleInfo> &infos)
{
    APP_LOGI("load all installed bundle data to map");
    std::fstream i(Constants::BUNDLE_DATA_BASE_FILE);
    nlohmann::json jParse;
    if (!i.is_open()) {
        APP_LOGE("failed to open bundle database file");
        // if file not exist, should create file here
        std::ofstream o(Constants::BUNDLE_DATA_BASE_FILE);
        o.close();
        return false;
    }
    APP_LOGI("open bundle database file success");
    i.seekg(0, std::ios::end);
    int len = static_cast<int>(i.tellg());
    if (len > 0) {
        i.seekg(0, std::ios::beg);
        jParse = nlohmann::json::parse(i, nullptr, false);
        if (jParse.is_discarded()) {
            APP_LOGE("bad bundle database file");
            i.close();
            return false;
        }
        for (auto &item : jParse.items()) {
            InnerBundleInfo innerBundleInfo;
            if (!innerBundleInfo.FromJson(item.value())) {
                infos.try_emplace(item.key(), innerBundleInfo);
            }
        }
    }
    i.close();
    return true;
}

bool BundleDataStorage::SaveStorageBundleInfo(const InnerBundleInfo &innerBundleInfo)
{
    APP_LOGI("save bundle data");
    bool ret = true;
    std::string appName = innerBundleInfo.GetBundleName();
    std::fstream f(Constants::BUNDLE_DATA_BASE_FILE);
    bool isExist = f.good();
    if (isExist) {
        nlohmann::json innerInfo;
        innerBundleInfo.ToJson(innerInfo);
        f.seekg(0, std::ios::end);
        int len = static_cast<int>(f.tellg());
        if (len == 0) {
            nlohmann::json appRoot;
            appRoot[appName] = innerInfo;
            f << std::setw(Constants::DUMP_INDENT) << appRoot << std::endl;
        } else {
            f.seekg(0, std::ios::beg);
            nlohmann::json jsonFile;
            f >> jsonFile;
            jsonFile[appName] = innerInfo;
            f.seekp(0, std::ios::beg);
            f << std::setw(Constants::DUMP_INDENT) << jsonFile << std::endl;
        }
    } else {
        APP_LOGI("bundle database file not exist");
        ret = false;
    }
    f.close();
    return ret;
}

bool BundleDataStorage::DeleteStorageBundleInfo(const InnerBundleInfo &innerBundleInfo)
{
    APP_LOGI("delete bundle data");
    bool ret = false;
    bool isEmpty = false;
    std::string appName = innerBundleInfo.GetBundleName();
    std::ifstream i(Constants::BUNDLE_DATA_BASE_FILE);
    nlohmann::json jParse;
    if (!i.is_open()) {
        APP_LOGE("failed to open bundle database file");
        return false;
    } else {
        i.seekg(0, std::ios::end);
        int len = static_cast<int>(i.tellg());
        if (len != 0) {
            i.seekg(0, std::ios::beg);
            i >> jParse;
            if (jParse.find(appName) != jParse.end()) {
                jParse.erase(appName);
                isEmpty = (jParse.size() == 0) ? true : false;
                ret = true;
            } else {
                APP_LOGE("not find appName = %{public}s", appName.c_str());
            }
        } else {
            APP_LOGE("file is empty appName = %{private}s", appName.c_str());
        }
    }
    i.close();

    std::ofstream o(Constants::BUNDLE_DATA_BASE_FILE);
    if (!o.is_open()) {
        APP_LOGE("failed to open bundle database file");
        ret = false;
    } else {
        if (!isEmpty) {
            o << std::setw(Constants::DUMP_INDENT) << jParse;
        }
    }
    o.close();
    return ret;
}
}  // namespace AppExecFwk
}  // namespace OHOS
