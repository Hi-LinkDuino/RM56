/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "dump_syspara.h"

typedef struct {
    char *serviceName;
    int (*serviceHandler)();
} ServiceItem;

static const ServiceItem SERVICE_LIST[] = {
    {"syspara", QuerySysparaCmd},
};

static void Usage()
{
    printf("Usage:os_dump [--help | -l | SERVICE]\n"
        "         --help: shows this help\n"
        "         -l: only list services, do not dump them\n"
        "         SERVICE: dumps only service SERVICE\n");
};

int main(int argc, char **argv)
{
    static struct option longOptions[] = {
        {"help", no_argument, 0, 0},
        {0, 0, 0, 0}
    };
    int serviceNum = (sizeof(SERVICE_LIST) / sizeof(ServiceItem));
    int c;
    int optionIndex = 0;
    if (optind == argc) {
        Usage();
        return 0;
    }

    optind = 1;
    while (1) {
        c = getopt_long(argc, argv, "l", longOptions, &optionIndex);
        if (c == -1) {
            break;
        }
        switch (c) {
            case 0:
                if (!strcmp(longOptions[optionIndex].name, "help")) {
                    Usage();
                }
                break;
            case 'l':
                printf("OptionList:\n");
                for (int i = 0; i < serviceNum; i = i + 1) {
                    printf("%s\n", SERVICE_LIST[i].serviceName);
                }
                break;
            default:
                Usage();
                break;
        }
    }

    for (int i = optind; i < argc; i++) {
        for (int j = 0; j < serviceNum; j++) {
            if (!strcmp(argv[i], SERVICE_LIST[j].serviceName)) {
                int ret = SERVICE_LIST[j].serviceHandler();
                return ret;
            }
        }
        printf("Service %s not registered! Try os_dump --help\n", argv[i]);
        break;
    }
    return 0;
}
