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
#ifndef HISTREAMER_FOUNDATION_OSAL_FILESYSTEM_FILE_SYSTEM_H
#define HISTREAMER_FOUNDATION_OSAL_FILESYSTEM_FILE_SYSTEM_H

#include <string>
#include <sys/stat.h>

namespace OHOS {
namespace Media {
namespace OSAL {
class FileSystem {
public:
    static bool IsRegularFile(const std::string& path);
    static bool IsFdValid(int32_t fd, struct stat& s);
    static bool IsRegularFile(int32_t fd);
    static bool IsSocketFile(int32_t fd);
    static bool IsSeekable(int32_t fd);
    static bool IsDirectory(const std::string& path);
    static bool IsExists(const std::string& path);
    static bool MakeDir(const std::string& path);
    static bool MakeMultipleDir(const std::string& path);
    static void RemoveFilesInDir(const std::string& path);
    static std::string GetTmpFileName();
};
} // namespace OSAL
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_FOUNDATION_OSAL_FILESYSTEM_FILE_SYSTEM_H
