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

#include "bundle_util.h"

#include <chrono>
#include <cinttypes>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <thread>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "hitrace_meter.h"
#include "directory_ex.h"
#include "ipc_skeleton.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string::size_type EXPECT_SPLIT_SIZE = 2;
static std::string g_deviceUdid;
static std::mutex g_mutex;
}

ErrCode BundleUtil::CheckFilePath(const std::string &bundlePath, std::string &realPath)
{
    if (!CheckFileName(bundlePath)) {
        APP_LOGE("bundle file path invalid");
        return ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID;
    }
    if (!CheckFileType(bundlePath, Constants::INSTALL_FILE_SUFFIX)) {
        APP_LOGE("file is not hap");
        return ERR_APPEXECFWK_INSTALL_INVALID_HAP_NAME;
    }
    if (!PathToRealPath(bundlePath, realPath)) {
        APP_LOGE("file is not real path");
        return ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID;
    }
    if (access(realPath.c_str(), F_OK) != 0) {
        APP_LOGE("can not access the bundle file path: %{private}s", realPath.c_str());
        return ERR_APPEXECFWK_INSTALL_INVALID_BUNDLE_FILE;
    }
    if (!CheckFileSize(realPath, Constants::MAX_HAP_SIZE)) {
        APP_LOGE("file size is larger than max hap size Max size is: %{public}" PRId64, Constants::MAX_HAP_SIZE);
        return ERR_APPEXECFWK_INSTALL_INVALID_HAP_SIZE;
    }
    return ERR_OK;
}

ErrCode BundleUtil::CheckFilePath(const std::vector<std::string> &bundlePaths, std::vector<std::string> &realPaths)
{
    HITRACE_METER_NAME(HITRACE_TAG_APP, __PRETTY_FUNCTION__);
    // there are three cases for bundlePaths:
    // 1. one bundle direction in the bundlePaths, some hap files under this bundle direction.
    // 2. one hap direction in the bundlePaths.
    // 3. some hap file directions in the bundlePaths.
    APP_LOGD("check file path");
    if (bundlePaths.empty()) {
        APP_LOGE("bundle file paths invalid");
        return ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID;
    }
    ErrCode ret = ERR_OK;

    if (bundlePaths.size() == 1) {
        struct stat s;
        std::string bundlePath = bundlePaths.front();
        std::string realPath = "";
        if (stat(bundlePath.c_str(), &s) == 0) {
            // it is a direction
            if ((s.st_mode & S_IFDIR) && !GetHapFilesFromBundlePath(bundlePath, realPaths)) {
                APP_LOGE("GetHapFilesFromBundlePath failed with bundlePath:%{private}s", bundlePaths.front().c_str());
                return ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID;
            }
            // it is a file
            if ((s.st_mode & S_IFREG) && (ret = CheckFilePath(bundlePaths.front(), realPath)) == ERR_OK) {
                realPaths.emplace_back(realPath);
            }
            return ret;
        } else {
            APP_LOGE("bundlePath is not existed with :%{private}s", bundlePaths.front().c_str());
            return ERR_APPEXECFWK_INSTALL_FILE_PATH_INVALID;
        }
    } else {
        for (const std::string& bundlePath : bundlePaths) {
            std::string realPath = "";
            ret = CheckFilePath(bundlePath, realPath);
            if (ret != ERR_OK) {
                return ret;
            }
            realPaths.emplace_back(realPath);
        }
    }
    APP_LOGD("finish check file path");
    return ret;
}

bool BundleUtil::CheckFileType(const std::string &fileName, const std::string &extensionName)
{
    APP_LOGD("path is %{private}s, support suffix is %{public}s", fileName.c_str(), extensionName.c_str());
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

bool BundleUtil::CheckFileName(const std::string &fileName)
{
    if (fileName.empty()) {
        APP_LOGE("the file name is empty");
        return false;
    }
    if (fileName.size() > Constants::PATH_MAX_SIZE) {
        APP_LOGE("bundle file path length %{public}zu too long", fileName.size());
        return false;
    }
    return true;
}

bool BundleUtil::CheckFileSize(const std::string &bundlePath, const int64_t fileSize)
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

bool BundleUtil::CheckSystemSize(const std::string &bundlePath, const std::string &diskPath)
{
    struct statfs diskInfo = { 0 };
    if (statfs(diskPath.c_str(), &diskInfo) != 0) {
        APP_LOGE("call statfs error");
        return false;
    }
    int64_t freeSize = diskInfo.f_bfree * diskInfo.f_bsize;
    APP_LOGD("left free size in the disk path is %{public}" PRId64, freeSize / Constants::ONE_GB);

    return CheckFileSize(bundlePath, freeSize);
}

bool BundleUtil::GetHapFilesFromBundlePath(const std::string& currentBundlePath, std::vector<std::string>& hapFileList)
{
    APP_LOGD("GetHapFilesFromBundlePath with path is %{private}s", currentBundlePath.c_str());
    if (currentBundlePath.empty()) {
        return false;
    }
    DIR* dir = opendir(currentBundlePath.c_str());
    if (dir == nullptr) {
        APP_LOGE("GetHapFilesFromBundlePath open bundle dir:%{private}s is failure", currentBundlePath.c_str());
        return false;
    }
    std::string bundlePath = currentBundlePath;
    if (bundlePath.back() != Constants::FILE_SEPARATOR_CHAR) {
        bundlePath.append(Constants::PATH_SEPARATOR);
    }
    struct dirent *entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        const std::string hapFilePath = bundlePath + entry->d_name;
        std::string realPath = "";
        if (CheckFilePath(hapFilePath, realPath) != ERR_OK) {
            APP_LOGE("find invalid hap path %{private}s", hapFilePath.c_str());
            closedir(dir);
            return false;
        }
        hapFileList.emplace_back(realPath);
        APP_LOGD("find hap path %{private}s", realPath.c_str());

        if (!hapFileList.empty() && (hapFileList.size() > Constants::MAX_HAP_NUMBER)) {
            APP_LOGE("reach the max hap number 128, stop to add more.");
            closedir(dir);
            return false;
        }
    }
    closedir(dir);
    return true;
}

int64_t BundleUtil::GetCurrentTime()
{
    int64_t time =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
        .count();
    APP_LOGD("the current time is %{public}" PRId64, time);
    return time;
}

void BundleUtil::DeviceAndNameToKey(
    const std::string &deviceId, const std::string &bundleName, std::string &key)
{
    key.append(deviceId);
    key.append(Constants::FILE_UNDERLINE);
    key.append(bundleName);
    APP_LOGD("bundleName = %{public}s", bundleName.c_str());
}

bool BundleUtil::KeyToDeviceAndName(
    const std::string &key, std::string &deviceId, std::string &bundleName)
{
    bool ret = false;
    std::vector<std::string> splitStrs;
    OHOS::SplitStr(key, Constants::FILE_UNDERLINE, splitStrs);
    // the expect split size should be 2.
    // key rule is <deviceId>_<bundleName>
    if (splitStrs.size() == EXPECT_SPLIT_SIZE) {
        deviceId = splitStrs[0];
        bundleName = splitStrs[1];
        ret = true;
    }
    APP_LOGD("bundleName = %{public}s", bundleName.c_str());
    return ret;
}

int32_t BundleUtil::GetUserIdByCallingUid()
{
    int32_t uid = IPCSkeleton::GetCallingUid();
    APP_LOGI("get calling uid(%{public}d)", uid);
    return GetUserIdByUid(uid);
}

int32_t BundleUtil::GetUserIdByUid(int32_t uid)
{
    if (uid <= Constants::INVALID_UID) {
        APP_LOGE("uid is illegal: %{public}d", uid);
        return Constants::INVALID_USERID;
    }

    return uid / Constants::BASE_USER_RANGE;
}

void BundleUtil::MakeHmdfsConfig(const std::string &bundleName, int32_t bundleId)
{
    std::string bundleDir = Constants::HMDFS_CONFIG_PATH + Constants::PATH_SEPERATE + bundleName;
    if (access(bundleDir.c_str(), F_OK) != 0) {
        if (mkdir(bundleDir.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
            APP_LOGE("make bundle dir error");
            return;
        }
    }

    std::string realBundleDir;
    if (!PathToRealPath(bundleDir, realBundleDir)) {
        APP_LOGE("bundleIdFile is not real path");
        return;
    }

    realBundleDir += (Constants::PATH_SEPERATE + Constants::BUNDLE_ID_FILE);

    int32_t bundleIdFd = open(realBundleDir.c_str(), O_WRONLY | O_TRUNC);
    if (bundleIdFd > 0) {
        std::string bundleIdStr = std::to_string(bundleId);
        if (write(bundleIdFd, bundleIdStr.c_str(), bundleIdStr.size()) < 0) {
            APP_LOGE("write bundleId error");
        }
    }
    close(bundleIdFd);
}

void BundleUtil::RemoveHmdfsConfig(const std::string &bundleName)
{
    std::string bundleDir = Constants::HMDFS_CONFIG_PATH + Constants::PATH_SEPERATE + bundleName;
    std::string realBundleDir;
    if (!PathToRealPath(bundleDir, realBundleDir)) {
        APP_LOGE("bundleDir is not real path");
        return;
    }
    if (rmdir(realBundleDir.c_str()) != 0) {
        APP_LOGE("remove hmdfs bundle dir error");
    }
}

std::string BundleUtil::CreateInstallTempDir(uint32_t installerId)
{
    std::time_t curTime = std::time(0);
    std::string tempDir = Constants::HAP_COPY_PATH + Constants::PATH_SEPARATOR + std::to_string(curTime) +
        std::to_string(installerId) + Constants::PATH_SEPARATOR;
    if (!OHOS::ForceCreateDirectory(tempDir)) {
        APP_LOGE("mkdir %{public}s failed", tempDir.c_str());
        return "";
    }
    if (chown(tempDir.c_str(), Constants::BMS_UID, Constants::BMS_GID) != 0) {
        APP_LOGE("fail to change %{private}s ownership", tempDir.c_str());
        return "";
    }
    mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    if (!OHOS::ChangeModeFile(tempDir, mode)) {
        APP_LOGE("change mode failed, temp install dir : %{public}s", tempDir.c_str());
        return "";
    }
    return tempDir;
}

void BundleUtil::CloseFileDescriptor(std::vector<int32_t> &fdVec)
{
    for_each(fdVec.begin(), fdVec.end(), [](const auto &fd) {
        if (fd > 0) {
            close(fd);
        }
    });
    fdVec.clear();
}

int32_t BundleUtil::CreateFileDescriptor(const std::string &bundlePath, long long offset)
{
    int fd = -1;
    if (bundlePath.length() > Constants::PATH_MAX_SIZE) {
        APP_LOGE("the length of the bundlePath exceeds maximum limitation");
        return fd;
    }
    if ((fd = open(bundlePath.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH)) < 0) {
        APP_LOGE("stream installer open bundlePath %{public}s failed", bundlePath.c_str());
        return fd;
    }
    if (offset > 0) {
        lseek(fd, offset, SEEK_SET);
    }
    return fd;
}

bool BundleUtil::IsExistFile(const std::string &path)
{
    if (path.empty()) {
        return false;
    }

    struct stat buf = {};
    if (stat(path.c_str(), &buf) != 0) {
        return false;
    }

    return S_ISREG(buf.st_mode);
}

bool BundleUtil::IsExistDir(const std::string &path)
{
    if (path.empty()) {
        return false;
    }

    struct stat buf = {};
    if (stat(path.c_str(), &buf) != 0) {
        return false;
    }

    return S_ISDIR(buf.st_mode);
}

bool BundleUtil::RenameFile(const std::string &oldPath, const std::string &newPath)
{
    if (oldPath.empty() || newPath.empty()) {
        APP_LOGE("oldPath or newPath is empty");
        return false;
    }

    if (!DeleteDir(newPath)) {
        APP_LOGE("delete newPath failed");
        return false;
    }

    return rename(oldPath.c_str(), newPath.c_str()) == 0;
}

bool BundleUtil::DeleteDir(const std::string &path)
{
    if (IsExistFile(path)) {
        return OHOS::RemoveFile(path);
    }

    if (IsExistDir(path)) {
        return OHOS::ForceRemoveDirectory(path);
    }

    return true;
}

bool BundleUtil::CopyFile(
    const std::string &sourceFile, const std::string &destinationFile)
{
    if (sourceFile.empty() || destinationFile.empty()) {
        APP_LOGE("Copy file failed due to sourceFile or destinationFile is empty");
        return false;
    }

    std::ifstream in(sourceFile);
    if (!in.is_open()) {
        APP_LOGE("Copy file failed due to open sourceFile failed");
        return false;
    }

    std::ofstream out(destinationFile);
    if (!out.is_open()) {
        APP_LOGE("Copy file failed due to open destinationFile failed");
        in.close();
        return false;
    }

    out << in.rdbuf();
    in.close();
    out.close();
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
