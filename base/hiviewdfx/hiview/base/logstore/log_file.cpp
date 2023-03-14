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
#include "log_file.h"

#include <list>
#include <stack>
#include <string>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "file_util.h"

namespace OHOS {
namespace HiviewDFX {
LogFile::LogFile(const std::string& fullPath) : path_(fullPath)
{
    struct stat sb;
    if ((fullPath.length() == 0) || (stat(fullPath.c_str(), &sb) == -1)) {
        isValid_ = false;
    } else {
        mtime_ = sb.st_mtime;
        isDir_ = (S_ISDIR(sb.st_mode) != 0);
        if (!isDir_) {
            size_ = sb.st_size;
        } else {
            size_ = FileUtil::GetFolderSize(fullPath);
        }
    }
    name_ = FileUtil::ExtractFileName(fullPath);
}
} // namespace HiviewDFX
} // namespace OHOS