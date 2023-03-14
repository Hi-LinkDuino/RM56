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

#include "file_util.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/syscall.h>

#include "directory_ex.h"
#include "file_ex.h"
namespace OHOS {
namespace HiviewDFX {
namespace FileUtil {
using namespace std;
bool LoadStringFromFile(const std::string& filePath, std::string& content)
{
    return OHOS::LoadStringFromFile(filePath, content);
}

bool LoadStringFromFd(int fd, std::string& content)
{
    return OHOS::LoadStringFromFd(fd, content);
}

bool SaveStringToFile(const std::string& filePath, const std::string& content, bool truncated)
{
    return OHOS::SaveStringToFile(filePath, content, truncated);
}

bool SaveStringToFd(int fd, const std::string& content)
{
    return OHOS::SaveStringToFd(fd, content);
}

bool LoadBufferFromFile(const std::string& filePath, std::vector<char>& content)
{
    return OHOS::LoadBufferFromFile(filePath, content);
}

bool SaveBufferToFile(const std::string& filePath, const std::vector<char>& content, bool truncated)
{
    return OHOS::SaveBufferToFile(filePath, content, truncated);
}

bool FileExists(const std::string& fileName)
{
    return OHOS::FileExists(fileName);
}

std::string ExtractFilePath(const std::string& fileFullName)
{
    return OHOS::ExtractFilePath(fileFullName);
}

std::string ExtractFileName(const std::string& fileFullName)
{
    return OHOS::ExtractFileName(fileFullName);
}

std::string IncludeTrailingPathDelimiter(const std::string& path)
{
    return OHOS::IncludeTrailingPathDelimiter(path);
}

std::string ExcludeTrailingPathDelimiter(const std::string& path)
{
    return OHOS::ExcludeTrailingPathDelimiter(path);
}

void GetDirFiles(const std::string& path, std::vector<std::string>& files)
{
    return OHOS::GetDirFiles(path, files);
}

bool ForceCreateDirectory(const std::string& path)
{
    return OHOS::ForceCreateDirectory(path);
}

bool ForceCreateDirectory(const string& path, mode_t mode)
{
    string::size_type index = 0;
    do {
        index = path.find('/', index + 1);
        string subPath = (index == string::npos) ? path : path.substr(0, index);
        if (access(subPath.c_str(), F_OK) != 0) {
            if (mkdir(subPath.c_str(), mode) != 0) {
                return false;
            }
        }
    } while (index != string::npos);
    return access(path.c_str(), F_OK) == 0;
}

bool ForceRemoveDirectory(const std::string& path, bool isNeedDeleteGivenDirSelf)
{
    return OHOS::ForceRemoveDirectory(path);
}

uint64_t GetFileSize(const std::string& path)
{
    struct stat st;
    return stat(path.c_str(), &st) ? 0 : static_cast<uint64_t>(st.st_size);
}

bool RemoveFile(const std::string& fileName)
{
    return OHOS::RemoveFile(fileName);
}

uint64_t GetFolderSize(const std::string& path)
{
    return OHOS::GetFolderSize(path);
}

// inner function, and param is legitimate
bool ChangeMode(const string& fileName, const mode_t& mode)
{
    return (chmod(fileName.c_str(), mode) == 0);
}

bool ChangeModeFile(const string& fileName, const mode_t& mode)
{
    if (access(fileName.c_str(), F_OK) != 0) {
        return false;
    }

    return ChangeMode(fileName, mode);
}

bool ChangeModeDirectory(const std::string& path, const mode_t& mode)
{
    return OHOS::ChangeModeDirectory(path, mode);
}

bool PathToRealPath(const std::string& path, std::string& realPath)
{
    return OHOS::PathToRealPath(path, realPath);
}

mode_t Umask(const mode_t& mode)
{
    return umask(mode);
}

int Open(const std::string& path, const int flags, const mode_t mode)
{
    return open(path.c_str(), flags, mode);
}

void CreateDirWithDefaultPerm(const std::string& path, uid_t aidRoot, uid_t aidSystem)
{
    FileUtil::ForceCreateDirectory(path);
    chown(path.c_str(), aidRoot, aidSystem);
}

void FormatPath2UnixStyle(std::string &path)
{
    // unimplemented
}

void RemoveFolderBeginWith(const std::string &path, const std::string &folderName)
{
    // unimplemented
}

bool WriteBufferToFd(int fd, const char* buffer, size_t size)
{
    if (fd < 0) {
        return false;
    }

    if (buffer == nullptr) {
        return false;
    }

    ssize_t writeSize = size;
    if (writeSize != TEMP_FAILURE_RETRY(write(fd, buffer, size))) {
        return false;
    }

    return true;
}

int CreateFile(const std::string &path, mode_t mode)
{
    if (FileExists(path)) {
        return 0;
    } else {
        std::ofstream fout(path);
        if (!fout.is_open()) {
            return -1;
        }
        fout.flush();
        fout.close();
        if (ChangeMode(path, mode) != 0) {
            return -1;
        }
    }
    return 0;
}

int CopyFile(const std::string &src, const std::string &des)
{
    std::ifstream fin(src, ios::binary);
    std::ofstream fout(des, ios::binary);
    if (!fin.is_open()) {
        return -1;
    }
    if (!fout.is_open()) {
        return -1;
    }
    fout << fin.rdbuf();
    if (fout.fail()) {
        fout.clear();
    }
    fout.flush();
    return 0;
}

bool IsDirectory(const std::string &path)
{
    struct stat statBuffer;
    if (stat(path.c_str(), &statBuffer) == 0 && S_ISDIR(statBuffer.st_mode)) {
        return true;
    }
    return false;
}

bool GetLastLine(std::istream &fin, std::string &line, int maxLen)
{
    if (fin.tellg() <= 0) {
        return false;
    } else {
        fin.seekg(-1, fin.cur);
    }
    uint32_t count = 0;
    while (fin.good() && fin.peek() == fin.widen('\n') && fin.tellg() > 0 && count < maxLen) {
        fin.seekg(-1, fin.cur);
        count++;
    }
    if (!fin.good() || count >= maxLen) {
        return false;
    }
    if (fin.tellg() == 0) {
        return true;
    }
    count = 0;
    while (fin.good() && fin.peek() != fin.widen('\n') && fin.tellg() > 0 && count < maxLen) {
        fin.seekg(-1, fin.cur);
        count++;
    }
    if (!fin.good() || count >= maxLen) {
        return false;
    }
    if (fin.tellg() != 0) {
        fin.seekg(1, fin.cur);
    }
    auto oldPos = fin.tellg();
    getline(fin, line);
    fin.seekg(oldPos);
    return true;
}

std::string GetParentDir(const std::string &path)
{
    string str = ExtractFilePath(path);
    if (str.empty()) {
        return "";
    }
    return str.substr(0, str.size() - 1);
}

bool IsLegalPath(const std::string& path)
{
    if (path.find("./") != std::string::npos ||
        path.find("../") != std::string::npos) {
        return false;
    }
    return true;
}

bool RenameFile(const std::string& src, const std::string& dest)
{
    if (std::rename(src.c_str(), dest.c_str()) == 0) {
        return true;
    }
    return false;
}
} // namespace FileUtil
} // namespace HiviewDFX
} // namespace OHOS
