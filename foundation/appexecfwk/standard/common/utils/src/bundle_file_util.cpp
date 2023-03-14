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

#include "bundle_file_util.h"

#include <cinttypes>
#include <dirent.h>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "directory_ex.h"
#include "string_ex.h"

namespace {
const std::string INSTALL_FILE_SUFFIX = ".hap";
const std::string PATH_SEPARATOR = "/";
constexpr int64_t ONE_GB = 1024 * 1024 * 1024;
constexpr int64_t MAX_HAP_SIZE = ONE_GB * 4;  // 4GB
constexpr int PATH_MAX_SIZE = 256;
const char FILE_SEPARATOR_CHAR = '/';
constexpr uint8_t MAX_HAP_NUMBER = 128;
} // namespace

namespace OHOS {
namespace AppExecFwk {
bool BundleFileUtil::CheckFilePath(const std::string &bundlePath, std::string &realPath)
{
    if (!CheckFileName(bundlePath)) {
        APP_LOGE("bundle file path invalid");
        return false;
    }
    if (!CheckFileType(bundlePath, INSTALL_FILE_SUFFIX)) {
        APP_LOGE("file is not hap");
        return false;
    }
    if (!PathToRealPath(bundlePath, realPath)) {
        APP_LOGE("file is not real path");
        return false;
    }
    if (access(realPath.c_str(), F_OK) != 0) {
        APP_LOGE("can not access the bundle file path: %{public}s", realPath.c_str());
        return false;
    }
    if (!CheckFileSize(realPath, MAX_HAP_SIZE)) {
        APP_LOGE("file size is larger than max hap size Max size is: %{public}" PRId64, MAX_HAP_SIZE);
        return false;
    }
    return true;
}

bool BundleFileUtil::CheckFilePath(const std::vector<std::string> &bundlePaths, std::vector<std::string> &realPaths)
{
    // there are three cases for bundlePaths:
    // 1. one bundle direction in the bundlePaths, some hap files under this bundle direction.
    // 2. one hap direction in the bundlePaths.
    // 3. some hap file directions in the bundlePaths.
    APP_LOGD("check file path");
    if (bundlePaths.empty()) {
        APP_LOGE("bundle file paths invalid");
        return false;
    }

    if (bundlePaths.size() == 1) {
        APP_LOGD("bundlePaths only has one element");
        std::string bundlePath = bundlePaths.front();
        std::string realPath = "";
        // it is a file
        if (CheckFilePath(bundlePaths.front(), realPath)) {
            APP_LOGD("path is a file");
            realPaths.emplace_back(realPath);
            return true;
        }
        if (!GetHapFilesFromBundlePath(bundlePath, realPaths)) {
            APP_LOGE("GetHapFilesFromBundlePath failed with bundlePath:%{public}s", bundlePaths.front().c_str());
            return false;
        }
        return true;
    } else {
        APP_LOGD("bundlePaths has more than one element");
        for (const std::string& bundlePath : bundlePaths) {
            std::string realPath = "";
            if (!CheckFilePath(bundlePath, realPath)) {
                return false;
            }
            realPaths.emplace_back(realPath);
        }
    }
    APP_LOGD("finish check file path");
    return true;
}

bool BundleFileUtil::CheckFileType(const std::string &fileName, const std::string &extensionName)
{
    APP_LOGD("path is %{public}s, support suffix is %{public}s", fileName.c_str(), extensionName.c_str());
    if (!CheckFileName(fileName)) {
        return false;
    }

    auto position = fileName.rfind('.');
    if (position == std::string::npos) {
        APP_LOGE("filename no extension name");
        return false;
    }

    std::string suffixStr = fileName.substr(position);
    return LowerStr(suffixStr) == extensionName;
}

bool BundleFileUtil::CheckFileName(const std::string &fileName)
{
    if (fileName.empty()) {
        APP_LOGE("the file name is empty");
        return false;
    }
    if (fileName.size() > PATH_MAX_SIZE) {
        APP_LOGE("bundle file path length %{public}zu too long", fileName.size());
        return false;
    }
    return true;
}

bool BundleFileUtil::CheckFileSize(const std::string &bundlePath, const int64_t fileSize)
{
    struct stat fileInfo = { 0 };
    if (stat(bundlePath.c_str(), &fileInfo) != 0) {
        APP_LOGE("call stat error");
        return false;
    }
    if (fileInfo.st_size > fileSize) {
        return false;
    }
    return true;
}

bool BundleFileUtil::GetHapFilesFromBundlePath(const std::string& currentBundlePath,
    std::vector<std::string>& hapFileList)
{
    APP_LOGD("GetHapFilesFromBundlePath with path is %{public}s", currentBundlePath.c_str());
    if (currentBundlePath.empty()) {
        return false;
    }
    DIR* dir = opendir(currentBundlePath.c_str());
    if (dir == nullptr) {
        APP_LOGE("GetHapFilesFromBundlePath open bundle dir:%{public}s is failure", currentBundlePath.c_str());
        return false;
    }
    std::string bundlePath = currentBundlePath;
    if (bundlePath.back() != FILE_SEPARATOR_CHAR) {
        bundlePath.append(PATH_SEPARATOR);
    }
    struct dirent *entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        const std::string hapFilePath = bundlePath + entry->d_name;
        std::string realPath = "";
        if (!CheckFilePath(hapFilePath, realPath)) {
            APP_LOGE("find invalid hap path %{public}s", hapFilePath.c_str());
            closedir(dir);
            return false;
        }
        hapFileList.emplace_back(realPath);
        APP_LOGD("find hap path %{public}s", realPath.c_str());

        if (!hapFileList.empty() && (hapFileList.size() > MAX_HAP_NUMBER)) {
            APP_LOGE("reach the max hap number 128, stop to add more.");
            closedir(dir);
            return false;
        }
    }
    closedir(dir);
    return true;
}
} // AppExecFwk
} // OHOS