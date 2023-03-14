/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "hks_test_log.h"

#include "securec.h"

#include "stdlib.h"

#include "time.h"

#define MAX_LOG_BUFF_LEN    512
#define WAIT_TO_LOG_DONE    100000

void HksTestLog(uint32_t logLevel, const char *funcName, int32_t lineNo, const char *format, ...)
{
    char buf[MAX_LOG_BUFF_LEN] = { 0 };

    va_list ap;
    va_start(ap, format);
    int32_t ret = vsnprintf_s(buf, MAX_LOG_BUFF_LEN, MAX_LOG_BUFF_LEN - 1, format, ap);
    va_end(ap);
    if (ret < 0) {
        printf("hks log concatenate error.");
        return;
    }

    switch (logLevel) {
        case HKS_LOG_LEVEL_I:
            printf("[INFO] %s [%d]: %s\n", funcName, lineNo, buf);
            break;
        case HKS_LOG_LEVEL_E:
            printf("[ERROR] %s [%d]: %s\n", funcName, lineNo, buf);
            break;
        case HKS_LOG_LEVEL_W:
            printf("[WARNING] %s [%d]: %s\n", funcName, lineNo, buf);
            break;
        case HKS_LOG_LEVEL_D:
            printf("[DEBUG] %s [%d]: %s\n", funcName, lineNo, buf);
            break;
        default:
            return;
    }

    usleep(WAIT_TO_LOG_DONE);
}

void HksAssertLog(bool test)
{
    if (!(test)) {
        HKS_TEST_LOG_E("[ASSERT] :fail\n");
    }
}
