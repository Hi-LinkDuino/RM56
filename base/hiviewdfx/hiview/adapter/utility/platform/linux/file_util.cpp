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

namespace OHOS {
namespace HiviewDFX {
namespace FileUtil {
using namespace std;

std::string IncludeTrailingPathDelimiter(const std::string& path)
{
    return "";
}

std::string ExcludeTrailingPathDelimiter(const std::string& path)
{
    return "";
}

bool ChangeModeFile(const string& fileName, const mode_t& mode)
{
    if (access(fileName.c_str(), F_OK) != 0) {
        return false;
    }

    return ChangeMode(fileName, mode);
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

bool SaveStringToFd(int fd, const std::string& content)
{
    if (fd <= 0) {
        return false;
    }

    if (content.empty()) {
        return true;
    }

    const long len = write(fd, content.c_str(), content.length());
    if (len < 0) {
        return false;
    }

    if ((unsigned long)len != content.length()) {
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