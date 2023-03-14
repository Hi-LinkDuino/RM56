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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INSTALLD_OPERATOR_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INSTALLD_OPERATOR_H

#include <mutex>
#include <string>
#include <vector>

#include "nocopyable.h"
#include "appexecfwk_errors.h"
#include "bundle_extractor.h"

namespace OHOS {
namespace AppExecFwk {
class InstalldOperator {
public:
    /**
     * @brief Check whether a file exist.
     * @param path Indicates the file path to be checked.
     * @return Returns true if the file exist; returns false otherwise.
     */
    static bool IsExistFile(const std::string &path);
    /**
     * @brief Check whether a directory exist.
     * @param path Indicates the directory path to be checked.
     * @return Returns true if the directory exist; returns false otherwise.
     */
    static bool IsExistDir(const std::string &path);
    /**
     * @brief Make a new directory including the parent path if not exist.
     * @param path Indicates the directory path to be checked.
     * @param isReadByOthers Indicates the directory whether read by other users.
     * @return Returns true if the directory make successfully; returns false otherwise.
     */
    static bool MkRecursiveDir(const std::string &path, bool isReadByOthers);
    /**
     * @brief Delete a directory.
     * @param path Indicates the directory path to be deleted.
     * @return Returns true if the directory deleted successfully; returns false otherwise.
     */
    static bool DeleteDir(const std::string &path);
    /**
     * @brief Extract the files of a compressed package to a specific directory.
     * @param srcModulePath Indicates the package file path.
     * @param targetPath normal files decompression path.
     * @param targetSoPath so files decompression path.
     * @param cpuAbi cpuAbi.
     * @return Returns true if the package extracted successfully; returns false otherwise.
     */
    static bool ExtractFiles(const std::string &sourcePath, const std::string &targetPath,
        const std::string &targetSoPath, const std::string &cpuAbi);

    static bool isNativeSo(const std::string &entryName, const std::string &targetSoPath, const std::string &cpuAbi);

    static void ExtractSo(const BundleExtractor &extractor, const std::string &entryName,
        const std::string &targetSoPath, const std::string &cpuAbi);

    /**
     * @brief Rename a directory from old path to new path.
     * @param oldPath Indicates the old path name.
     * @param newPath Indicates the new path name.
     * @return Returns true if the directory renamed successfully; returns false otherwise.
     */
    static bool RenameDir(const std::string &oldPath, const std::string &newPath);
    /**
     * @brief Change the owner and group ID of a file or directory.
     * @param filePath Indicates the file or directory path.
     * @param uid Indicates the uid.
     * @param uid Indicates the gid.
     * @return Returns true if changed successfully; returns false otherwise.
     */
    static bool ChangeFileAttr(const std::string &filePath, const int uid, const int gid);
    /**
     * @brief Rename a file from old path to new path.
     * @param oldPath Indicates the old path name.
     * @param newPath Indicates the new path name.
     * @return Returns true if the file renamed successfully; returns false otherwise.
     */
    static bool RenameFile(const std::string &oldPath, const std::string &newPath);
    /**
     * @brief Check whether a path is valid under a root path.
     * @param rootDir Indicates the root path name.
     * @param path Indicates the path to be checked.
     * @return Returns true if the path is valid successfully; returns false otherwise.
     */
    static bool IsValidPath(const std::string &rootDir, const std::string &path);
    /**
     * @brief Check whether a path is valid code path.
     * @param codePath Indicates the path to be checked.
     * @return Returns true if the file renamed successfully; returns false otherwise.
     */
    static bool IsValideCodePath(const std::string &codePath);
    /**
     * @brief Get the parent directory path of a file.
     * @param codePath Indicates the file path.
     * @return Returns the parent directory if get successfully; returns empty string otherwise.
     */
    static std::string GetPathDir(const std::string &path);
    /**
     * @brief Delete files in a directory.
     * @param path Indicates the directory path of the files to be deleted.
     * @return Returns true if the files deleted successfully; returns false otherwise.
     */
    static bool DeleteFiles(const std::string &dataPath);
    /**
     * @brief Make a directory and change the owner and group ID of it.
     * @param path Indicates the directory path to be made.
     * @param isReadByOthers Indicates the directory whether read by other users.
     * @param uid Indicates the uid.
     * @param uid Indicates the gid.
     * @return Returns true if directory made successfully; returns false otherwise.
     */
    static bool MkOwnerDir(const std::string &path, bool isReadByOthers, const int uid, const int gid);
    /**
     * @brief Make a directory and change the owner and group ID of it.
     * @param path Indicates the directory path to be made.
     * @param mode Indicates the directory mode.
     * @param uid Indicates the uid.
     * @param uid Indicates the gid.
     * @return Returns true if directory made successfully; returns false otherwise.
     */
    static bool MkOwnerDir(const std::string &path,  int mode, const int uid, const int gid);
    /**
     * @brief Get disk usage for dir.
     * @param dir Indicates the directory.
     * @param size Indicates the disk size.
     * @return Returns true if successfully; returns false otherwise.
     */
    static int64_t GetDiskUsage(const std::string &dir);
    /**
     * @brief Traverse all cache directories.
     * @param currentPath Indicates the current path.
     * @param cacheDirs Indicates the cache directories.
     * @return Returns true if successfully; returns false otherwise.
     */
    static void TraverseCacheDirectory(const std::string &currentPath, std::vector<std::string> &cacheDirs);
    /**
     * @brief Get disk usage from path.
     * @param path Indicates the current path.
     * @return Returns disk size.
     */
    static int64_t GetDiskUsageFromPath(const std::vector<std::string> &path);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_INSTALLD_OPERATOR_H