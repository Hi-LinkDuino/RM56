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

#include "hiview_plugin_platform_module_test.h"

#include "string_util.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;

namespace {
std::string GetCmdResultFromPopen(const std::string& cmd)
{
    if (cmd.empty()) {
        return "";
    }

    FILE* fp = popen(cmd.c_str(), "r");
    if (fp == nullptr) {
        return "";
    }
    const int bufSize = 128;
    char buffer[bufSize];
    std::string result = "";
    while (!feof(fp)) {
        if (fgets(buffer, bufSize - 1, fp) != NULL) {
            result += buffer;
        }
    }
    pclose(fp);
    return result;
}

int GetServicePid(const std::string& serviceName)
{
    std::string cmd = "pidof " + serviceName;
    std::string pidStr = GetCmdResultFromPopen(cmd);
    int32_t pid = 0;
    OHOS::HiviewDFX::StringUtil::ConvertStringTo<int32_t>(pidStr, pid);
    printf("the pid of service(%s) is %s \n", serviceName.c_str(), pidStr.c_str());
    return pid;
}

void WaitForServiceReady(const std::string& serviceName)
{
    int pid = GetServicePid(serviceName);
    if (pid <= 0) {
        std::string cmd = "start " + serviceName;
        GetCmdResultFromPopen(cmd);
        const int sleepTime = 10; // 10 seconds
        sleep(sleepTime);
        pid = GetServicePid(serviceName);
    }
    ASSERT_GT(pid, 0);
}
}

/**
 * @tc.name: HiviewStatusTest001
 * @tc.desc: check hiview running status and ensure it has been started
 * @tc.type: FUNC
 * @tc.require: SR000DPTSQ
 */
HWTEST_F(HiviewPluginPlatformModuleTest, HiviewStatusTest001, TestSize.Level0)
{
    WaitForServiceReady("hiview");
}
