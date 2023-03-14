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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_TEST_MODULETEST_COMMON_TOOL_H
#define FOUNDATION_APPEXECFWK_SERVICES_TEST_MODULETEST_COMMON_TOOL_H

#include <string>
#include <vector>

namespace OHOS {
namespace AppExecFwk {
class CommonTool {
public:
    int32_t ExcuteCmd(const std::string &cmd, std::vector<std::string> &resvec, const int size);
    bool ExcuteCmd(const std::string &cmd);
    bool CheckFilePathISExist(const std::string &filePath);
    bool CopyTmpFileToSystemPath(const std::string &srcPath, const std::string &dstPath);
    bool DeleteFile(const std::string &filePath);
    long GetFileBuildTime(const std::string &path);
    int32_t GetPid(const std::string &processName);
    std::string VectorToStr(const std::vector<std::string> &strVector);
    bool StartExecutable(const std::string &serviceName, const std::string &args);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_TEST_MODULETEST_COMMON_TOOL_H