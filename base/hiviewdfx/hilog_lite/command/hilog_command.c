/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "hilog_command.h"

#include <securec.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>

#define OPTION_HELP 'h'
#define OPTION_LEVEL 'L'
#define OPTION_DOMAIN 'D'
#define OPTION_SILENCE 'S'

#define PARA_AUTO "auto"
#define PARA_AUTO_LEN 4

#define APPHILOGCAT_OFF 0

#define HIVIEW_FEATURE_ON 1
#define HIVIEW_FEATURE_OFF 0

#define HILOG_MODULE_MAX "FFFFF"

char g_logLevelInfo[HILOG_LEVEL_MAX] = {
    'N', // "NONE"
    'N', // "NONE"
    'N', // "NONE"
    'D', // "DEBUG"
    'I', // "INFO"
    'W', // "WARN"
    'E', // "ERROR"
    'F'  // "FATAL"
};

HiviewConfig g_hiviewConfig = {
    .outputOption = OUTPUT_OPTION_FLOW,
    .level = LOG_DEBUG,
    .logSwitch = HIVIEW_FEATURE_ON,
    .dumpSwitch = HIVIEW_FEATURE_OFF,
    .silenceMod = SILENT_MODE_OFF,
    .eventSwitch = HIVIEW_FEATURE_OFF,
    .logOutputModule = HILOG_MODULE_MAX,
};

int HilogHelpProc(const char* tag)
{
    printf("Usage: %s [-h | --help] [-L <level> | --level=<level>] [--silence]\n", tag);
    printf("          [-D <domain> | --domain=<domain>]\n");
    printf("\n");
    printf("Options:\n");
    printf("-h, --help                         Help\n");
    printf("-L <level>, --level=<level>        Outputs logs at a specific level\n");
    printf("                                   Values of level :\n");
    printf("                                        D, stands for Debug\n");
    printf("                                        I, stands for Info\n");
    printf("                                        W, stands for Warning\n");
    printf("                                        E, stands for Error\n");
    printf("                                        F, stands for Fatal\n");
    printf("                                        auto, set log level with predefined macro\n");
    printf("-D <domain>, --domain=<domain>     Outputs logs at a specific domain\n");
    printf("--silence                          Silent mode, not output logs to the serial port\n");
    return -1;
}

bool SetLogLevel(unsigned char level)
{
    if (level >= LOG_DEBUG && level < HILOG_LEVEL_MAX) {
        g_hiviewConfig.level = level;
        printf("Set log level : %d \n", level);
        return true;
    }
    return false;
}

int SetOutputDomain(const char *mod)
{
    if ((mod == NULL) || (strlen(mod) == 1)) {
        printf("Invalid command.\n");
        return -1;
    }
    int len = strlen(mod);
    int modSize = sizeof(g_hiviewConfig.logOutputModule);

    (void)memset_s(g_hiviewConfig.logOutputModule, modSize, '0', modSize);

    int destStart = ((DOMAIN_ID_LENGTH - len) > 0) ? (DOMAIN_ID_LENGTH - len) : 0;
    int sourceStart = ((len - DOMAIN_ID_LENGTH) > 0) ? (len - DOMAIN_ID_LENGTH) : 0;
    int copyLen = (len < DOMAIN_ID_LENGTH) ? len : DOMAIN_ID_LENGTH;

    if (strncpy_s(g_hiviewConfig.logOutputModule + destStart, modSize - destStart, mod + sourceStart, copyLen) != 0) {
        printf("Copy log domain fail : %s \n", mod);
        return -1;
    }

    printf("Set log domain : %s \n", g_hiviewConfig.logOutputModule);
    return 0;
}

int SetOutputLevel(char cmd)
{
    unsigned char level = LOG_DEBUG;
    if (cmd == g_logLevelInfo[LOG_DEBUG]) {
        level = LOG_DEBUG;
    } else if (cmd == g_logLevelInfo[LOG_INFO]) {
        level = LOG_INFO;
    } else if (cmd == g_logLevelInfo[LOG_WARN]) {
        level = LOG_WARN;
    } else if (cmd == g_logLevelInfo[LOG_ERROR]) {
        level = LOG_ERROR;
    } else if (cmd == g_logLevelInfo[LOG_FATAL]) {
        level = LOG_FATAL;
    } else {
        printf("Invalid level : %c\n", cmd);
        return -1;
    }

    if (SetLogLevel(level)) {
        printf("Set the log output level success.\n");
        return 0;
    }
    printf("Set the log output level failure level=%d.\n", level);
    return -1;
}

int ProcAutoSet(void)
{
#ifdef OHOS_RELEASE
#if APPHILOGCAT_STATUS_RELEASE == APPHILOGCAT_OFF
    return -1;
#else
    SetLogLevel(CONFIG_LOG_LEVEL_RELEASE);
    printf("Default log level: %d \n", CONFIG_LOG_LEVEL_RELEASE);
#endif
#else  // OHOS_DEBUG
#if APPHILOGCAT_STATUS_DEBUG == APPHILOGCAT_OFF
    return -1;
#else
    SetLogLevel(CONFIG_LOG_LEVEL_DEBUG);
    printf("Default log level: %d \n", CONFIG_LOG_LEVEL_DEBUG);
#endif
#endif
    return 0;
}

int HilogLevelProc(const char* arg)
{
    if (arg == NULL) {
        printf("Invalid command.\n");
        return -1;
    }
    if (strncmp(arg, PARA_AUTO, PARA_AUTO_LEN) == 0) {
        return ProcAutoSet();
    }
    char level = arg[0];
    return SetOutputLevel(level);
}

int HilogDomainProc(const char* arg)
{
    return SetOutputDomain(arg);
}

int HilogSilenceProc()
{
    g_hiviewConfig.silenceMod = SILENT_MODE_ON;
    return 0;
}

int HilogCmdProc(const char *tag, int argc, char *argv[])
{
    int ret = -1;
    int optionIndex = 0;
    int opt = 0;

    struct option longOptions[] = {
        {"help", no_argument, NULL, OPTION_HELP},
        {"level", required_argument, NULL, OPTION_LEVEL},
        {"domain", required_argument, NULL, OPTION_DOMAIN},
        {"silence", no_argument, NULL, OPTION_SILENCE},
        {0, 0, 0, 0}
    };
    const char *shortOptions = "hL:D:";

    while ((opt = getopt_long(argc, argv, shortOptions, longOptions, &optionIndex)) != -1) {
        switch (opt) {
            case OPTION_HELP:
                ret = HilogHelpProc(tag);
                break;
            case OPTION_LEVEL:
                ret = HilogLevelProc(optarg);
                break;
            case OPTION_DOMAIN:
                ret = HilogDomainProc(optarg);
                break;
            case OPTION_SILENCE:
                ret = HilogSilenceProc();
                break;
            default:
                ret = -1;
                printf("Invalid command.\n");
                break;
        }
        if (ret == -1) {
            return -1;
        }
    }
    return ret;
}

bool FilterLevelLog(unsigned char setLevel, unsigned char logLevel)
{
    int logMinLevel;

    for (logMinLevel = LOG_DEBUG; logMinLevel < HILOG_LEVEL_MAX; logMinLevel++) {
        if (logLevel == g_logLevelInfo[logMinLevel]) {
            break;
        }
    }
    // Loglevel >= set level, may print log
    if (logMinLevel >= setLevel) {
        return true;
    }
    return false;
}

bool FilterModuleLog(const char *setModule, const char *logModule)
{
    if (strncmp(setModule, HILOG_MODULE_MAX, DOMAIN_ID_LENGTH) == 0) {
        return true;
    }
    return strncmp(logModule, g_hiviewConfig.logOutputModule, DOMAIN_ID_LENGTH) == 0;
}