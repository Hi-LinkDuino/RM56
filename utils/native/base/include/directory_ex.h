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

#ifndef DIRECTORY_EX_H
#define DIRECTORY_EX_H

#include <string>
#include <vector>
#include <sys/stat.h>

namespace OHOS {

/**
 * The GetCurrentProcFullFileName function get the current process exe name.
 */
std::string GetCurrentProcFullFileName();

/**
 * The GetCurrentProcPath function get the current process exe path.
 */
std::string GetCurrentProcPath();

/**
 * The ExtractFilePath function extract the input file path.
 */
std::string ExtractFilePath(const std::string& fileFullName);

/**
 * The ExtractFilePath function extract the input file name.
 */
std::string ExtractFileName(const std::string& fileFullName);

/**
 * The ExtractFileExt function extract the input file name type.
 */
std::string ExtractFileExt(const std::string& fileName);

/**
 * The ExcludeTrailingPathDelimiter function exclude the end '/' from the strPath,
 * return the path without the end '/'.
 */
std::string ExcludeTrailingPathDelimiter(const std::string& path);

/**
 * The IncludeTrailingPathDelimiter function include the end '/' from the strPath,
 * return the path with the end '/'.
 */
std::string IncludeTrailingPathDelimiter(const std::string& path);

/**
 * The GetDirFiles function get all files in the path.
 */
void GetDirFiles(const std::string& path, std::vector<std::string>& files);

/**
 * The IsEmptyFolder function judge the path is empty,
 * return true if is empty, else false.
 */
bool IsEmptyFolder(const std::string& path);

/**
 * The ForceCreateDirectory function is force create the dir with subdir,
 * return true if create succ, else false.
 */
bool ForceCreateDirectory(const std::string& path);

/**
 * The ForceRemoveDirectory function is force delete the dir with subdir and files,
 * return true if remove succ, else false.
 */
bool ForceRemoveDirectory(const std::string& path);

/**
 * The RemoveFile function is remove the input strFileName,
 * return true if remove succ, else false.
 */
bool RemoveFile(const std::string& fileName);

/**
 * The GetFolderSize function is get the folder size(bytes).
 */
uint64_t GetFolderSize(const std::string& path);

/**
 * The ChangeModeFile function is change the input file authority,
 * return true if change succ,  else false.
 */
bool ChangeModeFile(const std::string& fileName, const mode_t& mode);

/**
 * The ChangeModeDirectory function is change the input Directory authority, include subdir,
 * return true if change succ,  else false.
 */
bool ChangeModeDirectory(const std::string& path, const mode_t& mode);

/**
* The PathToRealPath function is get real path from relative path,
* return true if change succ, else false.
*/
bool PathToRealPath(const std::string& path, std::string& realPath);
} // OHOS
#endif
