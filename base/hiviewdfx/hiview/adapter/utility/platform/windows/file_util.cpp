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
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>

namespace OHOS {
namespace HiviewDFX {
namespace FileUtil {
using namespace std;
namespace fs = std::filesystem;
constexpr int PATH_MAX = 4096;
constexpr int MODE_MAX = 07777;
constexpr int MAX_FILE_LENGTH = 32 * 1024 * 1024; // 32MB

bool SaveStringToFd(int fd, const std::string& content)
{
    return true;
}

std::string IncludeTrailingPathDelimiter(const std::string& path)
{
    return "";
}

std::string ExcludeTrailingPathDelimiter(const std::string& path)
{
    return "";
}


mode_t Umask(const mode_t& mode)
{
    return mode;
}

int Open(const std::string& path, const int flags, const mode_t mode)
{
    const mode_t defaultMode = 0;
    return open(path.c_str(), flags, defaultMode);
}

void CreateDirWithDefaultPerm(const std::string& path, uid_t aidRoot, uid_t aidSystem)
{
    FileUtil::ForceCreateDirectory(path);
}

void FormatPath2UnixStyle(std::string &path)
{
    replace(path.begin(), path.end(), '\\', '/');
}

void RemoveFolderBeginWith(const std::string &path, const std::string &folderName)
{
    if (!fs::exists(path)) {
        return;
    }

    for (auto& cur : fs::recursive_directory_iterator(path)) {
        if (cur.is_directory() &&
            strncmp(cur.path().filename().string().c_str(), folderName.c_str(), folderName.length()) == 0) {
            ForceRemoveDirectory(cur.path().string());
        }
    }
}

bool WriteBufferToFd(int fd, const char* buffer, size_t size)
{
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