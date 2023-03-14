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

#ifndef HOS_LITE_HIVIEW_COMMAND_H
#define HOS_LITE_HIVIEW_COMMAND_H

#include <stdbool.h>

#include "log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define DOMAIN_ID_LENGTH 5
#define SILENT_MODE_OFF 0
#define SILENT_MODE_ON 1

typedef struct {
    const unsigned char outputOption : 4; /* Control log output mode. Cannot be modified during running. */
    unsigned char hiviewInited : 1;       /* Indicates whether the hiview service is inited. */
    unsigned char level : 3;              /* Control log output level. HILOG_LV_XXX */
    unsigned char logSwitch : 1;          /* Indicates whether to enable the log component. */
    unsigned char eventSwitch : 1;        /* Indicates whether to enable the event component. */
    unsigned char dumpSwitch : 1;         /* Indicates whether to enable the dump component. */
    unsigned char silenceMod : 1;         /* Silent mode, 0 for mode off, 1 for mdoe on. */
    char logOutputModule[DOMAIN_ID_LENGTH + 1]; /* Control log output module (including '\0' byte). */
    unsigned short writeFailureCount;
} HiviewConfig;

typedef enum {
    OUTPUT_OPTION_DEBUG = 0, /* Output to the UART without buffer. Commercial versions are forbidden. */
    OUTPUT_OPTION_FLOW,      /* Output to UART via SAMR */
    OUTPUT_OPTION_TEXT_FILE,
    OUTPUT_OPTION_BIN_FILE,
    OUTPUT_OPTION_MAX
} HiviewOutputOption;

extern HiviewConfig g_hiviewConfig;

int HilogCmdProc(const char *tag, int argc, char *argv[]);
bool FilterLevelLog(unsigned char setLevel, unsigned char logLevel);
bool FilterModuleLog(const char *setModule, const char *logModule);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef HOS_LITE_HIVIEW_COMMAND_H */
