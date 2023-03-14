/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "DMSTestBase.h"

#include <unistd.h>
#include <string>
#include <sys/timeb.h>

#include "semaphore.h"
#include "want.h"
#include "bundle_manager.h"
#include "bundle_info.h"

extern "C" void __attribute__((weak)) HOS_SystemInit(void)
{
    SAMGR_Bootstrap();
};

static const int INIT_TIME = 1000;
static BOOL isInited = FALSE;

static sem_t g_sem;

BOOL SystemInitProxy()
{
    if (isInited == FALSE) {
        printf("[hcpptest]samgr start init.\n");
        HOS_SystemInit();

        usleep(INIT_TIME * INIT_TIME);
        isInited = TRUE;
    } else {
        printf("[hcpptest]samgr already init. \n");
    }
}

long long GetSystemTime()
{
    timeb t;
    ftime(&t);
    return t.time * MS2US + t.millitm;
}

std::string GetStringByLen(int len)
{
    std::string tempStr = "";
    for (int i = 0; i < len; i++) {
        tempStr = tempStr + "x";
    }
    return tempStr;
}

/* callback */
static void TestBundleStateCallback(const uint8_t resultCode, const void *resultMessage)
{
    printf("TestBundleStateCallback resultCode: %d \n", resultCode);
    if (resultCode != 0) {
        printf("[hcpptest]W install failed.\n");
    }
    sem_post(&g_sem);
}

BOOL InstallHap()
{
    printf("[hcpptest]hap start to install!\n");
        
    sem_init(&g_sem, 0, 0);
    Install("/test_root/distributedschedule/helloworld_release.hap", nullptr, TestBundleStateCallback);
    sem_wait(&g_sem);

    usleep(INIT_TIME * INIT_TIME);
}

BOOL UninstallHap()
{
    sem_init(&g_sem, 0, 0);
    Uninstall("com.ohos.helloworld", nullptr, TestBundleStateCallback);
    sem_wait(&g_sem);
}
