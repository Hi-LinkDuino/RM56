/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "preferences_helper.h"

#include <climits>
#include <cstdlib>

#include <cerrno>
#include <utility>

#include "logger.h"
#include "preferences.h"
#include "preferences_errno.h"
#include "preferences_impl.h"
#include "securec.h"

namespace OHOS {
namespace NativePreferences {
std::map<std::string, std::shared_ptr<Preferences>> PreferencesHelper::prefsCache_;
std::mutex PreferencesHelper::prefsCacheMutex_;

static bool IsFileExist(const std::filesystem::path &path)
{
    FILE *file = std::fopen(path.c_str(), "r");
    if (file != nullptr) {
        std::fclose(file);
        return true;
    }
    return false;
}

std::string PreferencesHelper::GetRealPath(const std::string &path, int &errorCode)
{
    if (path.empty()) {
        LOG_ERROR("The path can not be empty.");
        errorCode = E_EMPTY_FILE_PATH;
        return "";
    }
    if (path.front() != '/') {
        LOG_ERROR("The path can not be relative path.");
        errorCode = E_RELATIVE_PATH;
        return "";
    }
    if (strlen(path.c_str()) > PATH_MAX) {
        LOG_ERROR("The path exceeds max length.");
        errorCode = E_PATH_EXCEED_MAX_LENGTH;
        return "";
    }
    std::string::size_type pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        LOG_ERROR("path can not be relative path.");
        errorCode = E_RELATIVE_PATH;
        return "";
    }
    std::string filePath = path.substr(0, pos);
    char canonicalPath[PATH_MAX + 1] = { 0 };
    if (realpath(filePath.c_str(), canonicalPath) == nullptr) {
        LOG_ERROR("Failed to obtain real path, errno:%{public}d", errno);
        errorCode = E_INVALID_FILE_PATH;
        return "";
    }
    std::string fileName = path.substr(pos + 1, path.length());
    if (fileName.empty()) {
        LOG_ERROR("file name can not be empty.");
        errorCode = E_EMPTY_FILE_NAME;
        return "";
    }
    errorCode = E_OK;
    std::string realFilePath(canonicalPath);
    return realFilePath.append("/").append(fileName);
}

std::shared_ptr<Preferences> PreferencesHelper::GetPreferences(const std::string &path, int &errCode)
{
    std::string realPath = GetRealPath(path, errCode);
    if (realPath == "" || errCode != E_OK) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(prefsCacheMutex_);
    std::map<std::string, std::shared_ptr<Preferences>>::iterator it = prefsCache_.find(realPath);
    if (it != prefsCache_.end()) {
        return it->second;
    }

    std::filesystem::path filePath = realPath.c_str();
    std::shared_ptr<PreferencesImpl> pref = std::make_shared<PreferencesImpl>(filePath);
    errCode = pref->Init();
    if (errCode != E_OK) {
        return nullptr;
    }
    prefsCache_.insert(make_pair(realPath, pref));
    return pref;
}

int PreferencesHelper::DeletePreferences(const std::string &path)
{
    int errCode = E_OK;
    std::string realPath = GetRealPath(path, errCode);
    if (realPath == "" || errCode != E_OK) {
        return errCode;
    }

    std::lock_guard<std::mutex> lock(prefsCacheMutex_);
    std::map<std::string, std::shared_ptr<Preferences>>::iterator it = prefsCache_.find(realPath);
    if (it != prefsCache_.end()) {
        prefsCache_.erase(it);
    }

    std::filesystem::path filePath = realPath.c_str();
    std::filesystem::path backupPath = PreferencesImpl::MakeBackupPath(filePath);
    std::filesystem::path brokenPath = PreferencesImpl::MakeBrokenPath(filePath);

    std::remove(filePath.c_str());
    std::remove(backupPath.c_str());
    std::remove(brokenPath.c_str());

    if (IsFileExist(filePath) || IsFileExist(backupPath) || IsFileExist(brokenPath)) {
        return E_DELETE_FILE_FAIL;
    }
    return E_OK;
}

int PreferencesHelper::RemovePreferencesFromCache(const std::string &path)
{
    int errCode = E_OK;
    std::string realPath = GetRealPath(path, errCode);
    if (realPath == "" || errCode != E_OK) {
        return errCode;
    }

    std::lock_guard<std::mutex> lock(prefsCacheMutex_);
    std::map<std::string, std::shared_ptr<Preferences>>::iterator it = prefsCache_.find(realPath);
    if (it == prefsCache_.end()) {
        return E_OK;
    }
    prefsCache_.erase(it);
    return E_OK;
}
} // End of namespace NativePreferences
} // End of namespace OHOS
