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

#include "common_tool.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>

#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
int32_t CommonTool::ExcuteCmd(const std::string &cmd, std::vector<std::string> &resvec, const int32_t size)
{
    if (size <= 0) {
        return 0;
    }
    resvec.clear();
    FILE *file = popen(cmd.c_str(), "r");
    if (file == nullptr) {
        return 0;
    }
    char tmp[size];
    tmp[0] = '\0';
    while (fgets(tmp, sizeof(tmp), file) != nullptr) {
        if (tmp[strlen(tmp) - 1] == '\n') {
            tmp[strlen(tmp) - 1] = '\0';
        }
        std::string iem(tmp);
        resvec.push_back(iem);
    }
    pclose(file);
    return resvec.size();
}

bool CommonTool::ExcuteCmd(const std::string &cmd)
{
    FILE *file = popen(cmd.c_str(), "r");
    if (file == nullptr) {
        return false;
    }
    pclose(file);
    return true;
}

bool CommonTool::CheckFilePathISExist(const std::string &filePath)
{
    if (access(filePath.c_str(), F_OK) == 0) {
        return true;
    }
    return false;
}

bool CommonTool::CopyTmpFileToSystemPath(const std::string &srcPath, const std::string &dstPath)
{
    if (access(srcPath.c_str(), F_OK) != 0) {
        return false;
    }
    FILE *srcFile = fopen(srcPath.c_str(), "r");
    if (srcFile == nullptr) {
        return false;
    }
    FILE *dstFile = fopen(dstPath.c_str(), "w");
    if (dstFile == nullptr) {
        fclose(srcFile);
        return false;
    }

    constexpr int READ_SIZE = 1024;
    std::string buff;
    buff.reserve(READ_SIZE);
    buff.resize(READ_SIZE - 1);
    size_t count = 0;
    while ((count = fread(&(buff[0]), 1, READ_SIZE - 1, srcFile)) != 0) {
        fwrite(&(buff[0]), 1, count, dstFile);
    }

    fclose(srcFile);
    fclose(dstFile);
    return true;
}

long CommonTool::GetFileBuildTime(const std::string &path)
{
    long buildTime = 0;

    if (!CheckFilePathISExist(path)) {
        return buildTime;
    }

    struct stat buf;
    FILE *file = fopen(path.c_str(), "r");
    if (file == nullptr) {
        return buildTime;
    }
    int fd = fileno(file);
    if (fd == -1) {
        fclose(file);
        return buildTime;
    }
    fstat(fd, &buf);
    buildTime = buf.st_ctime;
    fclose(file);
    return buildTime;
}

int32_t CommonTool::GetPid(const std::string &processName)
{
    std::string cmd = "ps -A | grep " + processName;
    int32_t size = 1024;
    std::vector<std::string> echoContents;
    int32_t queryResult = ExcuteCmd(cmd, echoContents, size);
    if (queryResult == 0) {
        return 0;
    }

    for (std::string echoContent : echoContents) {
        auto index = echoContent.find(processName);
        if (index != std::string::npos) {
            int32_t pid = 0;
            std::vector<std::string> strsRet;
            OHOS::SplitStr(echoContent, " ", strsRet);
            for (std::string item : strsRet) {
                if (OHOS::StrToInt(item, pid)) {
                    break;
                }
            }
            return pid;
        }
    }
    return 0;
}

std::string CommonTool::VectorToStr(const std::vector<std::string> &strVector)
{
    std::string str;
    for (auto it = strVector.begin(); it != strVector.end(); it++) {
        str.append(*it);
    }
    return str;
}

bool CommonTool::StartExecutable(const std::string &serviceName, const std::string &args)
{
    std::string cmd = "chmod +x /system/bin/" + serviceName;
    int res = system(cmd.c_str());
    if (res != 0) {
        return false;
    }

    int32_t pid = GetPid(serviceName);
    if (pid != 0) {
        return true;
    }

    std::string exeCmd = "/system/bin/" + serviceName + " " + args + "&";
    int cmdRes = system(exeCmd.c_str());
    if (cmdRes != 0) {
        return false;
    }

    std::vector<std::string> resvec;
    int32_t size = 1024;
    int32_t contentSize = ExcuteCmd("ps -A |grep " + serviceName, resvec, size);
    if (contentSize == 0) {
        return false;
    }
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS