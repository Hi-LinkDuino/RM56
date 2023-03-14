/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "hc_log.h"
#include "securec.h"

#define LOG_PRINT_MAX_LEN 256

static void DevAuthOutPrint(const char *buf, DevAuthLogLevel level)
{
#ifdef DEV_AUTH_DEBUG_PRINTF
    printf("[DEVAUTH]: %s\n", buf);
    return;
#endif
    switch (level) {
        case DEV_AUTH_LOG_LEVEL_DEBUG:
            DEV_AUTH_LOG_DEBUG(buf);
            break;
        case DEV_AUTH_LOG_LEVEL_INFO:
            DEV_AUTH_LOG_INFO(buf);
            break;
        case DEV_AUTH_LOG_LEVEL_WARN:
            DEV_AUTH_LOG_WARN(buf);
            break;
        case DEV_AUTH_LOG_LEVEL_ERROR:
            DEV_AUTH_LOG_ERROR(buf);
            break;
        default:
            break;
    }
}

void DevAuthLogPrint(DevAuthLogLevel level, const char *funName, const char *fmt, ...)
{
    int32_t ulPos = 0;
    char outStr[LOG_PRINT_MAX_LEN] = {0};
    int32_t ret = sprintf_s(outStr, sizeof(outStr), "%s: ", funName);
    if (ret < 0) {
        return;
    }
    ulPos = strlen(outStr);
    va_list arg;
    va_start(arg, fmt);
    ret = vsprintf_s(&outStr[ulPos], sizeof(outStr) - ulPos, fmt, arg);
    va_end(arg);
    if (ret < 0) {
        return;
    }
    DevAuthOutPrint(outStr, level);
}