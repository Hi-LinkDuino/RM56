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

#ifndef UTILITY_FILE_UTIL_H
#define UTILITY_FILE_UTIL_H

#ifdef _WIN32
using mode_t = unsigned short;
using pid_t = int;
using tid_t = int;
using uid_t = int;
#define S_IRWXU 00700
#define S_IXGRP 00010
#define S_IXOTH 00001
#define S_IRWXG 00070

#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IROTH 00004
#define F_OK    0
#ifndef S_ISDIR
# define S_ISDIR(a) (((a) & S_IFMT) == S_IFDIR)
#include <io.h>
# define access _access
#endif
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#include <sstream>
#include <string>
#include <vector>

namespace OHOS {
namespace HiviewDFX {
namespace FileUtil {
constexpr mode_t DEFAULT_FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH; // -rw-rw-r--
constexpr mode_t FILE_PERM_755 = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
constexpr mode_t FILE_PERM_775 = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
constexpr mode_t FILE_PERM_770 = S_IRWXU | S_IRWXG;
constexpr mode_t FILE_PERM_660 = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
// file_ex.h
bool LoadStringFromFile(const std::string& filePath, std::string& content);
bool LoadStringFromFd(int fd, std::string& content);
bool SaveStringToFile(const std::string& filePath, const std::string& content, bool truncated = true);
bool SaveStringToFd(int fd, const std::string& content);
bool LoadBufferFromFile(const std::string& filePath, std::vector<char>& content);
bool SaveBufferToFile(const std::string& filePath, const std::vector<char>& content, bool truncated = true);
bool FileExists(const std::string& fileName);
bool FileExistsA(const std::string& fileName);
bool WriteBufferToFd(int fd, const char* buffer, size_t size);
int32_t CreateFile(const std::string &path, mode_t mode = DEFAULT_FILE_MODE);

// directory_ex.h
std::string ExtractFilePath(const std::string& fileFullName);
std::string ExtractFileName(const std::string& fileFullName);
std::string IncludeTrailingPathDelimiter(const std::string& path);
std::string ExcludeTrailingPathDelimiter(const std::string& path);
void GetDirFiles(const std::string& path, std::vector<std::string>& files);
bool ForceCreateDirectory(const std::string& path);
bool ForceCreateDirectory(const std::string& path, mode_t mode);
void RemoveFolderBeginWith(const std::string &path, const std::string &folderName);
bool ForceRemoveDirectory(const std::string& path, bool isNeedDeleteGivenDirSelf = true);
bool RemoveFile(const std::string& fileName);
uint64_t GetFolderSize(const std::string& path);
uint64_t GetFileSize(const std::string& path);
bool ChangeMode(const std::string& fileName, const mode_t& mode);
bool ChangeModeDirectory(const std::string& path, const mode_t& mode);
bool PathToRealPath(const std::string& path, std::string& realPath);
mode_t Umask(const mode_t& mode);
int Open(const std::string& path, const int flags, const mode_t mode);
void FormatPath2UnixStyle(std::string &path);
void CreateDirWithDefaultPerm(const std::string& path, uid_t aidRoot, uid_t aid_system);
int CopyFile(const std::string &src, const std::string &des);
bool IsDirectory(const std::string &path);
bool GetLastLine(std::istream &fin, std::string &line, uint32_t maxLen = 10240); // 10240 : max line len
std::string GetParentDir(const std::string &path);
bool IsLegalPath(const std::string& path);
bool RenameFile(const std::string& src, const std::string& dest);
} // namespace FileUtil
} // namespace HiviewDFX
} // namespace OHOS
#endif // UTILITY_FILE_UTIL_H
