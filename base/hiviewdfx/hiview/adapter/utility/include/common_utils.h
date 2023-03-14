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
#ifndef UTILS_H
#define UTILS_H

#include <sys/stat.h>

#include <string>

#include "errors.h"
#include "file_util.h"

namespace OHOS {
namespace HiviewDFX {
static constexpr mode_t DEFAULT_FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH; // -rw-rw-r--
static constexpr mode_t FILE_PERM_755 = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
static constexpr mode_t FILE_PERM_775 = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
static constexpr mode_t FILE_PERM_770 = S_IRWXU | S_IRWXG;
static constexpr uint32_t DEFAULT_DURATION_SECONDS = 60;
static constexpr uint32_t BUF_SIZE_64 = 64;
static constexpr uint32_t BUF_SIZE_128 = 128;
static constexpr uint32_t BUF_SIZE_256 = 256;
static constexpr uint32_t BUF_SIZE_512 = 512;
static constexpr uint32_t BUF_SIZE_4096 = 4096;
static constexpr uint64_t NS_PER_SECOND = 1000000000;
static constexpr uint32_t MAX_LINE_LEN = 1024;
namespace CommonUtils {
std::string GetProcNameByPid(int32_t pid);
pid_t GetPidByName(const std::string& processName);
int32_t ExecCommand(const std::string &cmd, const std::vector<std::string> &args);
bool IsSpecificCmdExist(const std::string& fullPath);
bool WriteCommandResultToFile(int fd, const std::string& cmd);
};
} // namespace HiviewDFX
} // namespace OHOS
#endif
