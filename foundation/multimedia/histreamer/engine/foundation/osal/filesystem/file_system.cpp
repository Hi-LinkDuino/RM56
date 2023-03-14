/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#define HST_LOG_TAG "FileSystem"

#include "file_system.h"
#ifdef _WIN32
#include <direct.h>
#include <fcntl.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif
#include <cstring>
#include <cerrno>
#include <dirent.h>
#include "foundation/log.h"

#ifndef _WIN32
mode_t umask(mode_t __mask);
#endif

namespace OHOS {
namespace Media {
namespace OSAL {
constexpr uint32_t MAX_FILE_PATH = 50;
bool FileSystem::IsRegularFile(const std::string& path)
{
    struct stat s {};
    return (stat(path.c_str(), &s) == 0) && S_ISREG(s.st_mode);
}

bool FileSystem::IsFdValid(int32_t fd, struct stat& s)
{
    return fstat(fd, &s) == 0;
}

bool FileSystem::IsRegularFile(int32_t fd)
{
    struct stat s {};
    return IsFdValid(fd, s) && S_ISREG(s.st_mode);
}

bool FileSystem::IsSocketFile(int32_t fd)
{
    struct stat s {};
    return IsFdValid(fd, s) && S_ISSOCK(s.st_mode);
}

bool FileSystem::IsSeekable(int32_t fd)
{
    return IsRegularFile(fd) && lseek(fd, 0, SEEK_CUR) != -1;
}

bool FileSystem::IsDirectory(const std::string& path)
{
    struct stat s {};
    return (stat(path.c_str(), &s) == 0) && S_ISDIR(s.st_mode);
}

// judge regular file, directory, symbolic link file path exists
bool FileSystem::IsExists(const std::string& path)
{
    return access(path.c_str(), 0) != -1;
}

bool FileSystem::MakeDir(const std::string& path)
{
#ifdef _WIN32
    if (mkdir(path.c_str()) == -1) {
        MEDIA_LOG_E("Fail to create dir " PUBLIC_LOG_S " due to " PUBLIC_LOG_S, path.c_str(), std::strerror(errno));
        return false;
    }
#else
#ifndef OHOS_LITE
    auto oldMask = umask(0);
#endif
    if (mkdir(path.c_str(), 755) == -1) { // 755 directory access permissions
        MEDIA_LOG_E("Fail to create dir " PUBLIC_LOG_S " due to " PUBLIC_LOG_S, path.c_str(), std::strerror(errno));
#ifndef OHOS_LITE
        umask(oldMask);
#endif
        return false;
    }
#ifndef OHOS_LITE
    umask(oldMask);
#endif
#endif
    return true;
}

bool FileSystem::MakeMultipleDir(const std::string& path)
{
    FALSE_RETURN_V(!IsExists(path), true);

    // pos is 1, not 0  example: D:/a/b, /local/tmp/
    // Avoid Linux root path before is empty string, which makes it impossible to judge whether the path exists
    auto index = path.find('/', 1);
    while (index != std::string::npos) {
        std::string tPath = path.substr(0, index);
        FALSE_RETURN_V(IsExists(tPath) || MakeDir(tPath), false);
        index = path.find('/', index + 1);
    }
    return path[path.size() - 1] == '/' || MakeDir(path);
}

void FileSystem::RemoveFilesInDir(const std::string& path)
{
    DIR *directory;
    struct dirent *info;
    if ((directory = opendir(path.c_str())) != nullptr) {
        while ((info = readdir(directory)) != nullptr) {
            if (strcmp(info->d_name, ".") == 0 || strcmp(info->d_name, "..") == 0) {
                continue;
            }
            std::string fullPath = path + "/" + info->d_name;
            MEDIA_LOG_D("Remove file : " PUBLIC_LOG_S, fullPath.c_str());
            NZERO_LOG(remove(fullPath.c_str()));
        }
        closedir(directory);
    }
}

std::string FileSystem::GetTmpFileName()
{
    char tempFileName[MAX_FILE_PATH] = "/tmp/hstTmp.XXXXXX";
#ifdef _WIN32
    char tempPath[MAX_FILE_PATH];
    auto pathLength = GetTempPath(MAX_FILE_PATH, tempPath);
    FALSE_RETURN_V_MSG_E(pathLength < MAX_FILE_PATH && pathLength > 0, tempFileName, "get temp path failed");
    auto ret = GetTempFileName(tempPath, "hstTmp", 0, tempFileName);
    FALSE_RETURN_V_MSG_E(ret != 0, tempFileName, "get temp file name failed");
#else
    mktemp(tempFileName);
#endif
    return tempFileName;
}
} // namespace OSAL
} // namespace Media
} // namespace OHOS