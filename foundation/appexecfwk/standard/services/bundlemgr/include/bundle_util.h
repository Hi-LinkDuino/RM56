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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_UTIL_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_UTIL_H

#include <string>
#include <vector>

#include "appexecfwk_errors.h"

namespace OHOS {
namespace AppExecFwk {
class BundleUtil {
public:
    /**
     * @brief Check whether a file is valid HAP file.
     * @param bundlePath Indicates the HAP file path.
     * @return Returns ERR_OK if the file checked successfully; returns error code otherwise.
     */
    static ErrCode CheckFilePath(const std::string &bundlePath, std::string &realPath);
    /**
     * @brief Check whether an array of files are valid HAP files.
     * @param bundlePaths Indicates the HAP file paths.
     * @param realPaths Indicates the real paths of HAP files.
     * @return Returns ERR_OK if the file checked successfully; returns error code otherwise.
     */
    static ErrCode CheckFilePath(const std::vector<std::string> &bundlePaths, std::vector<std::string> &realPaths);
    /**
     * @brief Check whether a file is the specific type file.
     * @param fileName Indicates the file path.
     * @param extensionName Indicates the type to be checked.
     * @return Returns true if the file type checked successfully; returns false otherwise.
     */
    static bool CheckFileType(const std::string &fileName, const std::string &extensionName);
    /**
     * @brief Check whether a file name is valid.
     * @param fileName Indicates the file path.
     * @return Returns true if the file name checked successfully; returns false otherwise.
     */
    static bool CheckFileName(const std::string &fileName);
    /**
     * @brief Check whether a Hap size is valid.
     * @param fileName Indicates the file path.
     * @return Returns true if the file size checked successfully; returns false otherwise.
     */
    static bool CheckFileSize(const std::string &bundlePath, const int64_t fileSize);
    /**
     * @brief Check whether the disk path memory is available for installing the hap.
     * @param bundlePath Indicates the file path.
     * @param diskPath Indicates disk path in the system.
     * @return Returns true if the file size checked successfully; returns false otherwise.
     */
    static bool CheckSystemSize(const std::string &bundlePath, const std::string &diskPath);
    /**
     * @brief to obtain the hap paths of the input bundle path.
     * @param currentBundlePath Indicates the current bundle path.
     * @param hapFileList Indicates the hap paths.
     * @return Returns true if the hap path obtained successfully; returns false otherwise.
     */
    static bool GetHapFilesFromBundlePath(const std::string& currentBundlePath, std::vector<std::string>& hapFileList);
    /**
     * @brief to obtain the current time.
     * @return Returns current time.
     */
    static int64_t GetCurrentTime();
    /**
     * @brief key combination of deviceId and bundleName.
     * @param deviceId Indicates the deviceId.
     * @param bundleName Indicates the bundle name.
     * @param key Indicates the key.
     */
    static void DeviceAndNameToKey(
        const std::string &deviceId, const std::string &bundleName, std::string &key);
    /**
     * @brief The key is parsed into deviceId and bundleName.
     * @param key Indicates the key.
     * @param deviceId Indicates the deviceId.
     * @param bundleName Indicates the bundle name.
     * @return Returns result.
     */
    static bool KeyToDeviceAndName(
        const std::string &key, std::string &deviceId, std::string &bundleName);
    /**
     * @brief get userId by callinguid.
     * @return Returns userId.
     */
    static int32_t GetUserIdByCallingUid();
    /**
     * @brief get userId by uid.
     * @param uid Indicates uid.
     * @return Returns userId.
     */
    static int32_t GetUserIdByUid(int32_t uid);
    /**
     * @brief Is file exist.
     * @param path Indicates path.
     * @return Returns result.
     */
    static bool IsExistFile(const std::string &path);
    /**
     * @brief Is dir exist.
     * @param path Indicates path.
     * @return Returns result.
     */
    static bool IsExistDir(const std::string &path);
    /**
     * @brief Rename file from oldPath to newPath.
     * @param oldPath Indicates oldPath.
     * @param newPath Indicates newPath.
     * @return Returns result.
     */
    static bool RenameFile(const std::string &oldPath, const std::string &newPath);
    /**
     * @brief Copy file from oldPath to newPath.
     * @param oldPath Indicates oldPath.
     * @param newPath Indicates newPath.
     * @return Returns result.
     */
    static bool CopyFile(
        const std::string &oldPath, const std::string &newPath);
    /**
     * @brief Delete all dir or file.
     * @param path Indicates sourceStr.
     * @return Returns result.
     */
    static bool DeleteDir(const std::string &path);

    static void MakeHmdfsConfig(const std::string &bundleName, int32_t bundleId);

    static void RemoveHmdfsConfig(const std::string &bundleName);
    static void CloseFileDescriptor(std::vector<int32_t> &fdVec);
    static std::string CreateInstallTempDir(uint32_t installerId);
    static int32_t CreateFileDescriptor(const std::string &bundlePath, long long offset);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_UTIL_H
