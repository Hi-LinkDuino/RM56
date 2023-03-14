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
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#include "securec.h"
#include "dhcp_client.h"
#include "dhcp_function.h"

#undef LOG_TAG
#define LOG_TAG "WifiDhcpMain"

/* Static declared. */
static int Usage(void);
static int RunChildProcess(void);
static int CreateDaemon(void);
static int CheckClientProParam(int argc, char *argv[]);
static int GetClientOption(int argc, char *argv[]);
static int InitSpecifiedClientCfg(int argc, char *argv[]);
static int ExecClientProAction(const char *action);
static int GetClientNetworkInfo(void);

static struct DhcpClientCfg *g_cltCfg;

/* Static defined. */
static int Usage(void)
{
    printf("\nUsage:  dhcp_client_service ACTION INTERFACE [OPTION]\n");
    printf("  ACTION = start|stop|status|help|ver\n");
    printf("    start     - start INTERFACE's network interface service\n");
    printf("    stop      - stop INTERFACE's network interface service\n");
    printf("    status    - show INTERFACE's service status and exit\n");
    printf("    help      - displays usage info, INTERFACE is optional\n");
    printf("    ver       - displays client version, INTERFACE is optional\n");
    printf("  INTERFACE = ifname, eg: wlan0\n");
    printf("  OPTION = -w <directory>|-a or -4\n");
    printf("    -w <directory> - specify the client's working directory\n");
    printf("    -a - handle dhcp v4 and v6\n");
    printf("    -4 - only handle dhcp v4\n");
    printf("\n========== running process operate ==========\n");
    printf("  send dhcp release packet  =  kill -USR1 <pid>  \n");
    printf("  send dhcp renew   packet  =  kill -USR2 <pid>  \n\n");
    return DHCP_OPT_SUCCESS;
}

static int RunChildProcess(void)
{
    if (setpgrp() == -1) {
        LOGE("RunChildProcess() can not change process group, return!");
        return DHCP_OPT_FAILED;
    }

    if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
        LOGE("RunChildProcess() signal SIGHUP SIG_ERR, return!");
        return DHCP_OPT_FAILED;
    }

    /* Ensure we can never get a controlling terminal. */
    pid_t childPid;
    switch (childPid = fork()) {
        case -1:
            LOGE("RunChildProcess() fork grandchild failed, return!");
            return DHCP_OPT_FAILED;
        case 0:
            /* Grandchild process continue run. */
            break;
        default:
            /* Child process exit now. */
            exit(EXIT_SUCCESS);
    }
    LOGI("RunChildProcess() child: fork grand suc, childPid:%{public}d,pid():%{public}d,ppid():%{public}d.",
        childPid, getpid(), getppid());
    return DHCP_OPT_SUCCESS;
}

static int CreateDaemon(void)
{
    LOGI("CreateDaemon() enter, pid:%{public}d, ppid:%{public}d.", getpid(), getppid());

    /* A daemon process need close all open files. */
    if (fclose(stdin) != 0) {
        LOGE("CreateDaemon() fclose stdin error:%{public}d!", errno);
        return DHCP_OPT_FAILED;
    }
    if (fclose(stdout) != 0) {
        LOGE("CreateDaemon() fclose stdout error:%{public}d!", errno);
        return DHCP_OPT_FAILED;
    }
    if (fclose(stderr) != 0) {
        LOGE("CreateDaemon() fclose stderr error:%{public}d!", errno);
        return DHCP_OPT_FAILED;
    }

    /* Ensure that the process is not a fork subprocess, init process id is 1. */
    if (getppid() == 1) {
        LOGI("CreateDaemon() getppid() == 1, the process's parent is already init process!");
        /* Set default permissions for files and directories. */
        umask(DEFAULT_UMASK);
        return DHCP_OPT_SUCCESS;
    }

    pid_t pid;
    switch (pid = fork()) {
        case -1:
            LOGE("CreateDaemon() fork first child failed, return!");
            return DHCP_OPT_FAILED;
        case 0:
            if (RunChildProcess() != DHCP_OPT_SUCCESS) {
                LOGE("CreateDaemon() RunChildProcess failed, return!");
                return DHCP_OPT_FAILED;
            };
            /* Child process continue run. */
            break;
        default:
            /* Parent process exit now. */
            exit(EXIT_SUCCESS);
    }

    /* We have forked, setpgrp, forked once more, from now on, we are a daemon process. */
    LOGI("CreateDaemon() grandchild continue run, pid:%{public}d,getpid():%{public}d,getppid():%{public}d.",
        pid, getpid(), getppid());
    umask(DEFAULT_UMASK);
    return DHCP_OPT_SUCCESS;
}

static int CheckClientProParam(int argc, char *argv[])
{
    if (argc <= NUMBER_ONE) {
        printf("CheckClientProParam() argc:%d error, please input valid ACTION!\n", argc);
        Usage();
        return -1;
    }

    /* Check client process param ACTION "help","ver". */
    const char *cmdParam = argv[NUMBER_ONE];
    if (strncasecmp(cmdParam, "help", NUMBER_FOUR) == 0) {
        Usage();
        return 1;
    } else if (strncasecmp(cmdParam, "ver", NUMBER_THREE) == 0) {
        printf("%s, version %s\n", DHCPC_NAME, DHCPC_VERSION);
        return 1;
    } else {
        /* Check client process param ACTION "start","stop","status". */
        if ((strncasecmp(cmdParam, "start", NUMBER_FIVE) != 0) && (strncasecmp(cmdParam, "stop", NUMBER_FOUR) != 0) &&
            (strncasecmp(cmdParam, "status", NUMBER_FIVE) != 0)) {
            printf("CheckClientProParam() argv[1]:%s error, please input valid ACTION!\n", cmdParam);
            Usage();
            return -1;
        }
        /* Check client process running argc. */
        if (argc <= NUMBER_TWO) {
            printf("CheckClientProParam() argc:%d error, please input valid INTERFACE!\n", argc);
            Usage();
            return -1;
        }
    }
    return 0;
}

static int GetClientOption(int argc, char *argv[])
{
    int ch;
    while ((ch = getopt(argc - NUMBER_TWO, argv + NUMBER_TWO, "w:a46")) != -1) {
        switch (ch) {
            case 'w':   /* Specify the client's working directory. */
                LOGI("GetClientOption() cur workDir:%{public}s, optarg:%{public}s", g_cltCfg->workDir, optarg);
                if (strncpy_s(g_cltCfg->workDir, sizeof(g_cltCfg->workDir), optarg, DIR_MAX_LEN - 1) != EOK) {
                    return -1;
                }
                break;
            case 'a':    /* Handle dhcp v4 and v6. */
                g_cltCfg->getMode = DHCP_IP_TYPE_ALL;
                break;
            case '4':    /* Only handle dhcp v4. */
                LOGI("GetClientOption() cur getMode:%{public}u, optarg:%{public}s", g_cltCfg->getMode, optarg);
                g_cltCfg->getMode = DHCP_IP_TYPE_V4;
                break;
            case '6':    /* Only handle dhcp v6. */
                g_cltCfg->getMode = DHCP_IP_TYPE_V6;
                break;
            default:
                printf("GetClientOption() please input valid OPTION!\n");
                Usage();
                return -1;
        }
    }
    return 0;
}

static int InitSpecifiedClientCfg(int argc, char *argv[])
{
    g_cltCfg = GetDhcpClientCfg();
    if ((strncpy_s(g_cltCfg->workDir, sizeof(g_cltCfg->workDir), WORKDIR, DIR_MAX_LEN - 1) != EOK) ||
        (strncpy_s(g_cltCfg->ifaceName, sizeof(g_cltCfg->ifaceName), argv[NUMBER_TWO], INFNAME_SIZE - 1) != EOK)) {
        return DHCP_OPT_FAILED;
    }
    g_cltCfg->getMode = DHCP_IP_TYPE_ALL;
    if ((argc > NUMBER_THREE) && (GetClientOption(argc, argv) != 0)) {
        LOGE("InitSpecifiedClientCfg() GetClientOption failed!");
        return DHCP_OPT_FAILED;
    }

    if (strlen(g_cltCfg->workDir) == 0) {
        LOGE("InitSpecifiedClientCfg() g_cltCfg->workDir:%{public}s error!", g_cltCfg->workDir);
        return DHCP_OPT_FAILED;
    }
    if (CreateDirs(g_cltCfg->workDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != DHCP_OPT_SUCCESS) {
        LOGE("InitSpecifiedClientCfg() CreateDirs %{public}s failed!", g_cltCfg->workDir);
        return DHCP_OPT_FAILED;
    }

    int n = snprintf_s(g_cltCfg->confFile, DIR_MAX_LEN, DIR_MAX_LEN - 1, "%s%s", g_cltCfg->workDir, DHCPC_CONF);
    if (n < 0) {
        return DHCP_OPT_FAILED;
    }
    n = snprintf_s(g_cltCfg->pidFile, DIR_MAX_LEN, DIR_MAX_LEN - 1, "%s%s.pid", g_cltCfg->workDir, g_cltCfg->ifaceName);
    if (n < 0) {
        return DHCP_OPT_FAILED;
    }
    n = snprintf_s(
        g_cltCfg->resultFile, DIR_MAX_LEN, DIR_MAX_LEN - 1, "%s%s.result", g_cltCfg->workDir, g_cltCfg->ifaceName);
    if (n < 0) {
        return DHCP_OPT_FAILED;
    }
    n = snprintf_s(
        g_cltCfg->leaseFile, DIR_MAX_LEN, DIR_MAX_LEN - 1, "%s"DHCPC_LEASE, g_cltCfg->workDir, g_cltCfg->ifaceName);
    if (n < 0) {
        return DHCP_OPT_FAILED;
    }
    LOGI("InitSpecifiedClientCfg() "
         "g_cltCfg->workDir:%{public}s,confFile:%{public}s,pidFile:%{public}s,resultFile:%{public}s, "
         "getMode:%{public}d",
        g_cltCfg->workDir,
        g_cltCfg->confFile,
        g_cltCfg->pidFile,
        g_cltCfg->resultFile,
        g_cltCfg->getMode);
    return DHCP_OPT_SUCCESS;
}

static int ExecClientProAction(const char *action)
{
    /* Stop the specified network interface service. */
    if (strncasecmp(action, "stop", NUMBER_FOUR) == 0) {
        if (StopProcess(g_cltCfg->pidFile) != DHCP_OPT_SUCCESS) {
            LOGI("ExecClientProAction() StopProcess pidFile:%{public}s not success.", g_cltCfg->pidFile);
        } else {
            LOGI("ExecClientProAction() StopProcess pidFile:%{public}s success.", g_cltCfg->pidFile);
        }
        return 1;
    }

    /* Get the specified client process running status. */
    int proStatus = GetProStatus(g_cltCfg->pidFile);
    if (strncasecmp(action, "status", NUMBER_FIVE) == 0) {
        LOGI("ExecClientProAction() action:%{public}s GetProStatus proStatus:%{public}d.", action, proStatus);
        return 1;
    }

    /* Check the specified client process param ACTION. */
    if (strncasecmp(action, "start", NUMBER_FIVE) != 0) {
        LOGE("ExecClientProAction() argv[1]:%{public}s error, please input valid ACTION!", action);
        Usage();
        return -1;
    }
    if (proStatus == 1) {
        LOGI("ExecClientProAction() the specified client process is already started!");
        return 1;
    }

    /* Create a daemon process. */
    if (CreateDaemon() != DHCP_OPT_SUCCESS) {
        LOGE("ExecClientProAction() CreateDaemon failed!");
        return -1;
    }

    /* Init the specified client process id info. */
    if (InitPidfile(g_cltCfg->workDir, g_cltCfg->pidFile, getpid()) != DHCP_OPT_SUCCESS) {
        LOGE("ExecClientProAction() InitPidfile failed, ifaceName:%{public}s.", g_cltCfg->ifaceName);
        return -1;
    }

    return 0;
}

static int GetClientNetworkInfo(void)
{
    if (GetLocalInterface(g_cltCfg->ifaceName, &g_cltCfg->ifaceIndex, g_cltCfg->ifaceMac, NULL) != DHCP_OPT_SUCCESS) {
        LOGE("GetClientNetworkInfo() GetLocalInterface failed, ifaceName:%{public}s.", g_cltCfg->ifaceName);
        return DHCP_OPT_FAILED;
    }
    char macAddr[MAC_ADDR_LEN * MAC_ADDR_CHAR_NUM];
    if (memset_s(macAddr, sizeof(macAddr), 0, sizeof(macAddr)) != EOK) {
        return DHCP_OPT_FAILED;
    }
    MacChConToMacStr(g_cltCfg->ifaceMac, MAC_ADDR_LEN, macAddr, sizeof(macAddr));
    LOGI("GetClientNetworkInfo() g_cltCfg->ifaceName:%{public}s -> ifaceIndex:%{private}d,ifaceMac:%{private}s.",
        g_cltCfg->ifaceName, g_cltCfg->ifaceIndex, macAddr);

    if (GetLocalIp(g_cltCfg->ifaceName, &g_cltCfg->ifaceIpv4) != DHCP_OPT_SUCCESS) {
        LOGE("GetClientNetworkInfo() failed, g_cltCfg->ifaceName:%{public}s.", g_cltCfg->ifaceName);
        return DHCP_OPT_FAILED;
    }
    char *cIp = Ip4IntConToStr(g_cltCfg->ifaceIpv4, true);
    if (cIp == NULL) {
        LOGE("GetClientNetworkInfo() Ip4IntConToStr g_cltCfg->ifaceIpv4 failed!");
        return DHCP_OPT_FAILED;
    }
    LOGI("GetClientNetworkInfo() GetLocalIp ifaceName:%{public}s -> ifaceIpv4:%{private}u - %{private}s.",
        g_cltCfg->ifaceName, g_cltCfg->ifaceIpv4, cIp);
    free(cIp);
    cIp = NULL;

    /* Generate clientid for the specified client process interface. */
    if (g_cltCfg->pOptClientId == NULL) {
        g_cltCfg->pOptClientId = malloc(DHCP_OPT_CODE_BYTES + DHCP_OPT_LEN_BYTES + MAC_ADDR_LEN + 1);
        if (g_cltCfg->pOptClientId == NULL) {
            LOGE("GetClientNetworkInfo() g_cltCfg->pOptClientId malloc failed!");
            return DHCP_OPT_FAILED;
        }
        g_cltCfg->pOptClientId[DHCP_OPT_CODE_INDEX] = CLIENT_IDENTIFIER_OPTION;
        g_cltCfg->pOptClientId[DHCP_OPT_LEN_INDEX] = MAC_ADDR_LEN + 1;
        /* Generate format: 1 + ifaceMac. */
        g_cltCfg->pOptClientId[DHCP_OPT_DATA_INDEX] = NUMBER_ONE;
        if (memcpy_s(g_cltCfg->pOptClientId + DHCP_OPT_DATA_INDEX + 1,
            MAC_ADDR_LEN,
            g_cltCfg->ifaceMac,
            MAC_ADDR_LEN) != EOK) {
            return DHCP_OPT_FAILED;
        }
    }

    return DHCP_OPT_SUCCESS;
}

int main(int argc, char *argv[])
{
    /* Check client process running param ACTION. */
    int nCheck = CheckClientProParam(argc, argv);
    if (nCheck != 0) {
        return (nCheck == 1) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    /* Init the specified client process config. */
    if (InitSpecifiedClientCfg(argc, argv) != DHCP_OPT_SUCCESS) {
        LOGE("main() InitSpecifiedClientCfg failed!");
        return EXIT_FAILURE;
    }

    /* Exec the specified client process ACTION. */
    const char *cmdParam = argv[NUMBER_ONE];
    int nExec = ExecClientProAction(cmdParam);
    if (nExec != 0) {
        return (nExec == 1) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    /* Get the specified client process interface network info. */
    if (GetClientNetworkInfo() != DHCP_OPT_SUCCESS) {
        LOGE("main() GetClientNetworkInfo failed!");
        return EXIT_FAILURE;
    }

    /* Start the specified network interface service. */
    if (StartProcess() != DHCP_OPT_SUCCESS) {
        LOGE("main() StartProcess failed!");
        return EXIT_FAILURE;
    }

    LOGI("main() end, argc:%{public}d.", argc);
    return EXIT_SUCCESS;
}
