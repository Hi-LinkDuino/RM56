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

#include "hiview_lite_command.h"

#include <ctype.h>

#include "securec.h"
#include "ohos_types.h"
#include "hiview_util.h"
#include "hiview_config.h"
#include "hiview_log.h"

#define CMD_MIN_LEN     2
#define CMD_MAX_LEN     32
#define CMD_HIEVENT     "hievent"
#define OPTION_TAG      '-'
#define OPTION_SET      'c'
#define OPTION_HELP     'h'
#define STR_MAX_LEN     128

static boolean CheckCmdStr(const char *cmd);
static void HieventHelpProc(void);
static void HieventSetProc(const char *cmd);

/* Command does not contain the "hievent". */
void HieventCmdProc(const char *cmd)
{
    if (cmd == NULL) {
        return;
    }

    int32 len = strnlen(cmd, CMD_MAX_LEN + 1);
    if ((len < CMD_MIN_LEN) || (len > CMD_MAX_LEN) || (CheckCmdStr(cmd) == FALSE)) {
        HIVIEW_UartPrint("Invalid command.\n");
        return;
    }

    if (*cmd != OPTION_TAG) {
        HIVIEW_UartPrint("Invalid command.\n");
        return;
    }

    switch (*(++cmd)) {
        case OPTION_HELP:
            HieventHelpProc();
            break;
        case OPTION_SET:
            HieventSetProc(++cmd);
            break;
        default:
            HIVIEW_UartPrint("Invalid command.\n");
            break;
    }
}

static void HieventHelpProc(void)
{
    HIVIEW_UartPrint("hievent [-h] [-c]\n");
    HIVIEW_UartPrint(" -h            Help\n");
    HIVIEW_UartPrint(" -c            Enable or disable event function\n");
}

static void HieventSetProc(const char *cmd)
{
    if (*cmd != '\0') {
        HIVIEW_UartPrint("Invalid command.\n");
        return;
    }

    if (g_hiviewConfig.eventSwitch == HIVIEW_FEATURE_ON) {
        SwitchEvent(HIVIEW_FEATURE_OFF);
        HIVIEW_UartPrint("Close event function success.\n");
    } else {
        SwitchEvent(HIVIEW_FEATURE_ON);
        HIVIEW_UartPrint("Open event function success.\n");
    }
}

static boolean CheckCmdStr(const char *cmd)
{
    while (*cmd != '\0') {
        if (!(isalnum(*cmd) || (*cmd == ' ') || (*cmd == '\n') ||
            (*cmd == '=') || (*cmd == '-'))) {
            return FALSE;
        }
        cmd++;
    }
    return TRUE;
}

void SwitchEvent(uint8 flag)
{
    g_hiviewConfig.eventSwitch = (flag == HIVIEW_FEATURE_ON) ? HIVIEW_FEATURE_ON : HIVIEW_FEATURE_OFF
}
