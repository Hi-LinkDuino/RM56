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

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <getopt.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "dhcp_server.h"
#include "dhcp_define.h"
#include "dhcp_logger.h"
#include "dhcp_ipv4.h"
#include "hash_table.h"
#include "address_utils.h"
#include "securec.h"
#include "dhcp_config.h"
#include "dhcp_argument.h"

#undef LOG_TAG
#define LOG_TAG "DhcpServerMain"

#define DEFAUTL_NET_MASK "255.255.255.0"

static DhcpConfig g_dhcpConfig;

static PDhcpServerContext g_dhcpServer = 0;


enum SignalEvent {
    EXIT = 0,
    RELOAD,
    RESTART,
};

typedef struct DhcpOptionField {
    const char *field;
    int code;
    const char *explain;
    const char *format;
    int supported;
    int (*parseOption)(DhcpOption *, int, char *);

} DhcpOptionField;

void LoadLocalConfig(DhcpAddressPool *pool)
{
    LOGD("loading local configure ...");
}

void ReloadLocalConfig(DhcpAddressPool *pool)
{
    LOGD("reloading local configure ...");
}

static int InitNetworkAbout(DhcpConfig *config)
{
    ArgumentInfo *arg = GetArgument("netmask");
    if (arg) {
        LOGI("subnet mask:%s", arg->value);
        uint32_t argNetmask = ParseIpAddr(arg->value);
        if (argNetmask) {
            config->netmask = argNetmask;
        } else {
            LOGW("error netmask argument.");
            return RET_FAILED;
        }
    } else {
        if (!config->netmask) {
            config->netmask = ParseIpAddr(DEFAUTL_NET_MASK);
        }
    }
    arg = GetArgument("gateway");
    if (arg) {
        LOGI("gateway:%s", arg->value);
        uint32_t argGateway = ParseIpAddr(arg->value);
        if (argGateway) {
            config->gateway = argGateway;
        } else {
            LOGE("error gateway argument.");
            return RET_FAILED;
        }
    }
    return RET_SUCCESS;
}

static int PareseAddreesRange(DhcpConfig *config)
{
    ArgumentInfo *arg = GetArgument("pool");
    if (arg) {
        LOGD("pool info:%s", arg->value);
        int index = 0;
        char *src = arg->value;
        char *delim = ",";
        char *pSave = NULL;
        char *poolPartArg;
        poolPartArg = strtok_r(src, delim, &pSave);
        while (poolPartArg) {
            if (index == 0) {
                config->pool.beginAddress = ParseIpAddr(poolPartArg);
                LOGD("address range begin of: %s", poolPartArg);
            } else if (index == 1) {
                config->pool.endAddress = ParseIpAddr(poolPartArg);
                LOGD("address range end of: %s", poolPartArg);
            }
            index++;
            poolPartArg = strtok_r(NULL, delim, &pSave);
        }
        if (!config->pool.beginAddress || !config->pool.endAddress) {
            LOGE("'pool' argument format error.");
            return RET_FAILED;
        }
        return RET_SUCCESS;
    }
    LOGW("failed to get 'pool' argument.");
    return RET_FAILED;
}

static int InitAddressRange(DhcpConfig *config)
{
    if (HasArgument("pool")) {
        if (PareseAddreesRange(config) != RET_SUCCESS) {
            LOGW("dhcp range config error.");
            return RET_FAILED;
        }
    } else {
        if (!config->pool.beginAddress || !config->pool.endAddress) {
            config->pool.beginAddress = FirstIpAddress(config->serverId, config->netmask);
            config->pool.endAddress = LastIpAddress(config->serverId, config->netmask);
        }
    }
    return RET_SUCCESS;
}

static int InitDomainNameServer(DhcpConfig *config)
{
    ArgumentInfo *arg = GetArgument("dns");
    if (arg) {
        DhcpOption argOpt = {DOMAIN_NAME_SERVER_OPTION, 0, {0}};

        char *pSave = NULL;
        char *pTok = strtok_r(arg->value, ",", &pSave);
        if ((pTok == NULL) || (strlen(pTok) == 0)) {
            LOGE("strtok_r pTok NULL or len is 0!");
            return RET_FAILED;
        }
        uint32_t dnsAddress;
        while (pTok != NULL) {
            if ((dnsAddress = ParseIpAddr(pTok)) == 0) {
                LOGE("ParseIpAddr %s failed, code:%d", pTok, argOpt.code);
                return RET_FAILED;
            }
            if (AppendAddressOption(&argOpt, dnsAddress) != RET_SUCCESS) {
                LOGW("failed to append dns option.");
            };
            pTok = strtok_r(NULL, ",", &pSave);
        }

        if (GetOption(&config->options, argOpt.code) != NULL) {
            RemoveOption(&config->options, DOMAIN_NAME_SERVER_OPTION);
        }
        PushBackOption(&config->options, &argOpt);
    }
    return RET_SUCCESS;
}

static int InitServerId(DhcpConfig *config)
{
    ArgumentInfo *arg = GetArgument("server");
    if (arg) {
        LOGI("server id:%s", arg->value);
        uint32_t argServerId = ParseIpAddr(arg->value);
        if (argServerId) {
            config->serverId = argServerId;
        } else {
            LOGE("error server argument.");
            return RET_FAILED;
        }
    } else {
        if (!config->serverId) {
            LOGE("failed to get 'server' argument or config item.");
            return RET_FAILED;
        }
    }
    return RET_SUCCESS;
}

static int InitLeaseTime(DhcpConfig *config)
{
    ArgumentInfo *arg = GetArgument("lease");
    if (arg) {
        config->leaseTime = atoi(arg->value);
    } else {
        if (!config->leaseTime) {
            config->leaseTime = DHCP_LEASE_TIME;
        }
    }
    return RET_SUCCESS;
}

static int InitRenewalTime(DhcpConfig *config)
{
    ArgumentInfo *arg = GetArgument("renewal");
    if (arg) {
        config->renewalTime = atoi(arg->value);
    } else {
        if (!config->rebindingTime) {
            config->rebindingTime = DHCP_RENEWAL_TIME;
        }
        config->renewalTime = DHCP_RENEWAL_TIME;
    }
    return RET_SUCCESS;
}

static int InitRebindingTime(DhcpConfig *config)
{
    ArgumentInfo *arg = GetArgument("rebinding");
    if (arg) {
        config->rebindingTime = atoi(arg->value);
    } else {
        if (!config->rebindingTime) {
            config->rebindingTime =  DHCP_REBINDING_TIME;
        }
    }
    return RET_SUCCESS;
}
static int InitConfigByArguments(DhcpConfig *config)
{
    if (!config) {
        LOGE("dhcp configure pointer is null.");
        return RET_FAILED;
    }
    if (InitServerId(config) != RET_SUCCESS) {
        return RET_FAILED;
    }
    if (InitNetworkAbout(config) != RET_SUCCESS) {
        return RET_FAILED;
    }
    if (InitAddressRange(config) != RET_SUCCESS) {
        return RET_FAILED;
    }
    if (InitLeaseTime(config) != RET_SUCCESS) {
        return RET_FAILED;
    }
    if (InitRenewalTime(config) != RET_SUCCESS) {
        return RET_FAILED;
    }
    if (InitRebindingTime(config) != RET_SUCCESS) {
        return RET_FAILED;
    }
    if (InitDomainNameServer(config) != RET_SUCCESS) {
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

int ServerActionCallback(int state, int code, const char *ifname)
{
    int ret = 0;
    switch (state) {
        case ST_STARTING: {
            if (code == 0) {
                LOGD(" callback[%s] ==> server starting ...", ifname);
            } else if (code == 1) {
                LOGD(" callback[%s] ==> server started.", ifname);
            } else if (code == NUM_TWO) {
                LOGD(" callback[%s] ==> server start failed.", ifname);
            }
            break;
        }
        case ST_RELOADNG: {
            LOGD(" callback[%s] ==> reloading ...", ifname);
            break;
        }
        case ST_STOPED: {
            LOGD(" callback[%s] ==> server stoped.", ifname);
            break;
        }
        default:
            break;
    }
    return ret;
}

static void SignalHandler(int signal)
{
    switch (signal) {
        case SIGTERM: {
            exit(0);
            break;
        }
        case SIGUSR1:
            break;
        default:
            break;
    }
}

static int RegisterSignalHandle(void)
{
    if (signal(SIGTERM, SignalHandler) == SIG_ERR) {
        LOGE("RegisterSignalHandle() failed, signal SIGTERM err:%d!", errno);
        return RET_FAILED;
    }

    if (signal(SIGUSR1, SignalHandler) == SIG_ERR) {
        LOGE("RegisterSignalHandle() failed, signal SIGUSR1 err:%d!", errno);
        return RET_FAILED;
    }

    return RET_SUCCESS;
}

static int InitializeDhcpConfig(const char *ifname, DhcpConfig *config)
{
    if (!config) {
        LOGE("dhcp configure pointer is null.");
        return RET_FAILED;
    }
    if (InitOptionList(&config->options) != RET_SUCCESS) {
        LOGE("failed to initialize options.");
        return RET_FAILED;
    }
    char *configFile = DHCPD_CONFIG_FILE;
    if (HasArgument("conf")) {
        ArgumentInfo *configArg = GetArgument("conf");
        if (configArg) {
            configFile = configArg->value;
        } else {
            LOGE("failed to get config file name.");
            return RET_FAILED;
        }
    }
    LOGD("load local config file:%s", configFile);
    if (LoadConfig(configFile, ifname, config) != RET_SUCCESS) {
        LOGE("faile to load configure file.");
        return RET_FAILED;
    }
    if (InitConfigByArguments(config) != RET_SUCCESS) {
        LOGE("faile to parse arguments.");
        return RET_FAILED;
    }

    return RET_SUCCESS;
}

static void FreeLocalConfig(void)
{
    FreeOptionList(&g_dhcpConfig.options);
}

void FreeSeverResources(void)
{
    FreeArguments();
    FreeLocalConfig();
    FreeServerContext(g_dhcpServer);
}

static void BeforeExit(void)
{
    if (g_dhcpServer) {
        LOGD("saving lease recoder...");
        if (SaveLease(g_dhcpServer) != RET_SUCCESS) {
            LOGD("failed to save lease recoder.");
        }
    }
    FreeSeverResources();
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        PrintRequiredArguments();
        return 1;
    }
    if (strcmp("-h", argv[argc - 1]) == 0 || strcmp("--help", argv[argc - 1]) == 0) {
        ShowHelp(argc);
        return 0;
    }
    if (InitArguments() != RET_SUCCESS) {
        LOGE("failed to init arguments table.");
        return 1;
    }
    int ret = ParseArguments(argc, argv);
    if (ret != RET_SUCCESS) {
        FreeArguments();
        return 1;
    }
    ArgumentInfo *ifaceName = GetArgument("ifname");
    if (!ifaceName || strlen(ifaceName->value) == 0) {
        printf("missing required parameters:\"ifname\"\n");
        FreeArguments();
        return 1;
    }
    if (InitializeDhcpConfig(ifaceName->value, &g_dhcpConfig) != RET_SUCCESS) {
        LOGW("failed to initialize config.");
    }
    g_dhcpServer = InitializeServer(&g_dhcpConfig);
    if (g_dhcpServer == NULL) {
        LOGE("failed to initialize dhcp server.");
        FreeArguments();
        return 1;
    }
    if (RegisterSignalHandle() != RET_SUCCESS) {
        FreeSeverResources();
        return 1;
    }
    if (atexit(BeforeExit) != 0) {
        LOGW("failed to register exit process function.");
    }
    RegisterDhcpCallback(g_dhcpServer, ServerActionCallback);
    if (StartDhcpServer(g_dhcpServer) != RET_SUCCESS) {
        FreeSeverResources();
        return 1;
    }
    return 0;
}