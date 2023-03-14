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

#include "dhcp_argument.h"
#include "dhcp_option.h"
#include "dhcp_config.h"
#include "address_utils.h"
#include "securec.h"
#include "dhcp_logger.h"
#include <getopt.h>
#undef LOG_TAG
#define LOG_TAG "DhcpArgument"

static HashTable g_argumentsTable;

static int PutIpArgument(const char *argument, const char *val)
{
    if (!ParseIpAddr(val)) {
        LOGE("%s format error.", argument);
        return RET_FAILED;
    }
    return PutArgument(argument, val);
}

static int PutPoolArgument(const char *argument, const char *val)
{
    if (!val) {
        return 0;
    }
    if (strchr(val, ',') == NULL) {
        LOGE("too few pool option arguments.");
        return RET_FAILED;
    }
    return PutArgument(argument, val);
}

static int ShowVersion(const char *argument, const char *val)
{
    printf("version:%s\n", DHCPD_VERSION);
    return RET_SUCCESS;
}

static int DefaultArgument(const char *argument, const char *val)
{
    LOGD("Input argument is: [%s], value is [%s]", (argument == NULL) ? "" : argument,
        (val == NULL) ? "" : val);
    return RET_SUCCESS;
}

const char *optionString = "i:c:d:g:s:n:P:S:Bp:o:lb:rvhD";

static struct option longOptions[] = {
    {"ifname", REQUIRED_ARG, 0, 'i'},
    {"conf", REQUIRED_ARG, 0, 'c'},
    {"dns", REQUIRED_ARG, 0, 'd'},
    {"gateway", REQUIRED_ARG, 0, 'g'},
    {"server", REQUIRED_ARG, 0, 's'},
    {"netmask", REQUIRED_ARG, 0, 'n'},
    {"pool", REQUIRED_ARG, 0, 'P'},
    {"lease", REQUIRED_ARG, 0, 0},
    {"renewal", REQUIRED_ARG, 0, 0},
    {"rebinding", REQUIRED_ARG, 0, 0},
    {"version", NO_ARG, 0, 'v'},
    {"help", NO_ARG, 0, 'h'},
    {0, 0, 0, 0},
};

static DhcpUsage usages[] = {
    {&longOptions[NUM_ZERO], "<interface>", "network interface name.", "--ifname eth0", 1, PutArgument},
    {&longOptions[NUM_ONE], "<file>", "configure file name.", "--conf /etc/conf/dhcp_server.conf", 0, PutArgument},
    {&longOptions[NUM_TWO], "<dns1>[,dns2][,dns3][...]", "domain name server IP address list.", "", 0, PutArgument},
    {&longOptions[NUM_THREE], "<gateway>", "gateway option.", "", 0, PutIpArgument},
    {&longOptions[NUM_FOUR], "<server>", "server identifier.", "", 1, PutIpArgument},
    {&longOptions[NUM_FIVE], "<netmask>", "default subnet mask.", "", 1, PutIpArgument},
    {&longOptions[NUM_SIX], "<beginip>,<endip>", "pool address range.", "", 0,
        PutPoolArgument},
    {&longOptions[NUM_SEVEN], "<leaseTime>", "set lease time value, the value is in units of seconds.", "", 0,
        PutArgument},
    {&longOptions[NUM_EIGHT], "<renewalTime>", "set renewal time value, the value is in units of seconds.", "", 0,
        PutArgument},
    {&longOptions[NUM_NINE], "<rebindingTime>", "set rebinding time value, the value is in units of seconds.", "", 0,
        PutArgument},
    {&longOptions[NUM_TEN], "", "show version information.", "", 0, ShowVersion},
    {&longOptions[NUM_ELEVEN], "", "show help information.", "", 0, DefaultArgument},
    {0, "", "", ""},
};

int HasArgument(const char *argument)
{
    char name[ARGUMENT_NAME_SIZE] = {'\0'};
    if (!argument) {
        return 0;
    }
    size_t ssize = strlen(argument);
    if (ssize > ARGUMENT_NAME_SIZE) {
        ssize = ARGUMENT_NAME_SIZE;
    }
    if (memcpy_s(name, ARGUMENT_NAME_SIZE, argument, ssize) != EOK) {
        LOGE("failed to set argument name.");
        return 0;
    }
    if (ContainsKey(&g_argumentsTable, (uintptr_t)name)) {
        return 1;
    }
    return 0;
}

static void ShowUsage(const DhcpUsage *usage)
{
    if (!usage) {
        return;
    }
    if (usage->opt->val) {
        printf("-%c,--%s ", usage->opt->val, usage->opt->name);
    } else {
        printf("   --%s ", usage->opt->name);
    }
    if (usage->params[0] == '\0') {
        printf("\t\t%s\n", usage->desc);
    } else {
        int plen = strlen(usage->params) + strlen(usage->params);
        if (plen < USAGE_DESC_MAX_LENGTH) {
            printf("\t\t%s\t\t%s\n", usage->params, usage->desc);
        } else {
            printf("\t\t%s\n", usage->params);
            printf("\t\t\t%s\n\n", usage->desc);
        }
    }
}

void PrintRequiredArguments(void)
{
    size_t argc = sizeof(usages) / sizeof(DhcpUsage);
    printf("required parameters:");
    int idx = 0;
    for (size_t i = 0; i < argc; i++) {
        DhcpUsage usage = usages[i];
        if (!usage.opt) {
            break;
        }
        if (usage.required) {
            if (idx == 0) {
                printf("\"%s\"", usage.opt->name);
            } else {
                printf(", \"%s\"", usage.opt->name);
            }
            idx++;
        }
    }
    printf(".\n\n");
    printf("Usage: dhcp_server [options] \n");
    printf("e.g: dhcp_server -i eth0 -c /data/dhcp/dhcp_server.conf \n");
    printf("     dhcp_server --help \n\n");
}

static void PrintUsage(void)
{
    printf("Usage: dhcp_server [options] \n\n");

    size_t argc = sizeof(usages) / sizeof(DhcpUsage);
    for (size_t i = 0; i < argc; i++) {
        DhcpUsage usage = usages[i];
        if (!usage.opt) {
            break;
        }
        ShowUsage(&usage);
    }
    printf("\n");
}

void ShowHelp(int argc)
{
    if (argc == NUM_TWO) {
        PrintUsage();
        return;
    }
}

int InitArguments(void)
{
    if (CreateHashTable(&g_argumentsTable, ARGUMENT_NAME_SIZE, sizeof(ArgumentInfo), INIT_ARGS_SIZE) != HASH_SUCCESS) {
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

ArgumentInfo *GetArgument(const char *name)
{
    char argName[ARGUMENT_NAME_SIZE] = {'\0'};
    size_t ssize = strlen(name);
    if (ssize > ARGUMENT_NAME_SIZE) {
        ssize = ARGUMENT_NAME_SIZE;
    }
    if (memcpy_s(argName, ARGUMENT_NAME_SIZE, name, ssize) != EOK) {
        LOGE("failed to set argument name.");
        return NULL;
    }
    if (ContainsKey(&g_argumentsTable, (uintptr_t)argName)) {
        ArgumentInfo *arg = (ArgumentInfo *)At(&g_argumentsTable, (uintptr_t)argName);
        return arg;
    }
    return NULL;
}

int PutArgument(const char *argument, const char *val)
{
    if (!argument) {
        return RET_FAILED;
    }
    if (!val) {
        return RET_FAILED;
    }

    if (HasArgument(argument)) {
        return RET_FAILED;
    }

    ArgumentInfo arg;
    size_t ssize = strlen(argument);
    if (ssize >= ARGUMENT_NAME_SIZE) {
        ssize = ARGUMENT_NAME_SIZE -1;
    }
    size_t vlen = strlen(val);
    if (memset_s(arg.name, ARGUMENT_NAME_SIZE, '\0', ARGUMENT_NAME_SIZE) != EOK) {
        LOGE("failed to reset argument name.");
        return RET_ERROR;
    }
    if (memcpy_s(arg.name, ARGUMENT_NAME_SIZE, argument, ssize) != EOK) {
        LOGE("failed to set argument name.");
        return RET_ERROR;
    }
    if (vlen >= ARGUMENT_VALUE_SIZE) {
        LOGE("value string too long.");
        return RET_ERROR;
    }
    if (memset_s(arg.value, ARGUMENT_VALUE_SIZE, '\0', ARGUMENT_NAME_SIZE) != EOK) {
        LOGE("failed to reset argument value.");
        return RET_ERROR;
    }
    if (memcpy_s(arg.value, ARGUMENT_VALUE_SIZE, val, vlen) != EOK) {
        LOGE("failed to set argument value.");
        return RET_ERROR;
    }
    int ret = Insert(&g_argumentsTable, (uintptr_t)arg.name, (uintptr_t)&arg);
    if (ret == HASH_INSERTED) {
        return RET_SUCCESS;
    }
    return RET_FAILED;
}

int findIndex(int c)
{
    int size = sizeof(longOptions) / sizeof(longOptions[0]);
    for (int i = 0; i < size; ++i) {
        if (longOptions[i].val == c) {
            return i;
        }
    }
    return -1;
}

int ParseArguments(int argc, char *argv[])
{
    int ret;
    opterr = 0;
    size_t optsc = sizeof(usages) / sizeof(DhcpUsage);
    int index = -1;
    int rst = RET_SUCCESS;
    while ((ret = getopt_long(argc, argv, optionString, longOptions, &index)) != -1) {
        if (ret == '?') {
            LOGW("unknown input arguments! ret = ?");
            index = -1;
            continue;
        }
        if (index < 0) {
            index = findIndex(ret);
        }
        if (index < 0 || index >= (int)optsc) {
            LOGD("unknown input arguments! ret = %c, index = %d", ret, index);
            index = -1;
            continue;
        }
        DhcpUsage *usage = &usages[index];
        rst = usage->dealOption(usage->opt->name, optarg);
        if (rst != RET_SUCCESS) {
            break;
        }
        index = -1;
    }

    return rst;
}

void FreeArguments(void)
{
    if (!Initialized(&g_argumentsTable)) {
        return;
    }
    DestroyHashTable(&g_argumentsTable);
}
