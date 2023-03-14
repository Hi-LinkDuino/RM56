/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "dhcp_client.h"

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "securec.h"
#include "dhcp_function.h"
#include "dhcp_ipv4.h"

#undef LOG_TAG
#define LOG_TAG "WifiDhcpClient"

/* Default options. */
static struct DhcpClientCfg g_clientCfg = {"", "", "", "", "", 0, 0, DHCP_IP_TYPE_NONE, {'\0'}, NULL, false, "", "", 0};

struct DhcpClientCfg *GetDhcpClientCfg(void)
{
    return &g_clientCfg;
}

int StartProcess(void)
{
    LOGI("StartProcess() begin.");
    if (InitSignalHandle() != DHCP_OPT_SUCCESS) {
        return DHCP_OPT_FAILED;
    }

    if ((g_clientCfg.getMode == DHCP_IP_TYPE_ALL) || (g_clientCfg.getMode == DHCP_IP_TYPE_V4)) {
        /* Handle dhcp v4. */
        StartIpv4();
    }
    return DHCP_OPT_SUCCESS;
}

int StopProcess(const char *pidFile)
{
    LOGI("StopProcess() begin, pidFile:%{public}s.", pidFile);
    pid_t pid = GetPID(pidFile);
    if (pid <= 0) {
        LOGW("StopProcess() GetPID pidFile:%{public}s, pid == -1!", pidFile);
        return DHCP_OPT_SUCCESS;
    }

    LOGI("StopProcess() sending signal SIGTERM to process:%{public}d.", pid);
    if (-1 == kill(pid, SIGTERM)) {
        if (ESRCH == errno) {
            LOGW("StopProcess() pidFile:%{public}s,pid:%{public}d is not exist.", pidFile, pid);
            unlink(pidFile);
            return DHCP_OPT_SUCCESS;
        }
        LOGE("StopProcess() cmd: [kill %{public}d] failed, kill error:%{public}d!", pid, errno);
        return DHCP_OPT_FAILED;
    }

    unlink(pidFile);
    return DHCP_OPT_SUCCESS;
}

int GetProStatus(const char *pidFile)
{
    pid_t pid = GetPID(pidFile);
    if (pid <= 0) {
        LOGI("GetProStatus() %{public}s pid:%{public}d, %{public}s is not running.", pidFile, pid, DHCPC_NAME);
        return 0;
    }
    LOGI("GetProStatus() %{public}s pid:%{public}d, %{public}s is running.", pidFile, pid, DHCPC_NAME);
    return 1;
}
