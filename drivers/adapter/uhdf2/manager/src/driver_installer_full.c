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

#include "driver_installer_full.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "hdf_base.h"
#include "hdf_driver_installer.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"
#include "service_control.h"

#define HDF_LOG_TAG driver_installer_full
#define MAX_CMD_LEN 256
#define PARAM_CNT 2

static struct DriverInstaller *g_fullInstaller = NULL;
int DriverInstallerFullStartDeviceHost(uint32_t devHostId, const char* devHostName, bool dynamic)
{
    if (dynamic) {
        int ret = ServiceControlWithExtra(devHostName, START, NULL, 0);
        HDF_LOGD("%{public}s %{public}s %{public}d %{public}d", __func__, devHostName, devHostId, ret);
    }

    return HDF_SUCCESS;
}

int DriverInstallerFullStopDeviceHost(uint32_t devHostId, const char* devHostName)
{
    int ret;
    ret = ServiceControlWithExtra(devHostName, STOP, NULL, 0);
    HDF_LOGI("%{public}s %{public}s %{public}d %{public}d", __func__, devHostName, devHostId, ret);
    return ret;
}
static void DriverInstallerFullConstruct(struct DriverInstaller *inst)
{
    struct IDriverInstaller *pvtbl = (struct IDriverInstaller *)inst;
    pvtbl->StartDeviceHost = DriverInstallerFullStartDeviceHost;
    pvtbl->StopDeviceHost = DriverInstallerFullStopDeviceHost;
}

struct HdfObject *DriverInstallerFullCreate(void)
{
    if (g_fullInstaller == NULL) {
        g_fullInstaller = (struct DriverInstaller *)OsalMemCalloc(sizeof(struct DriverInstaller));
        if (g_fullInstaller != NULL) {
            DriverInstallerFullConstruct(g_fullInstaller);
        }
    }

    return (struct HdfObject *)g_fullInstaller;
}

void DriverInstallerFullRelease(struct HdfObject *object)
{
    struct DriverInstaller *installer = (struct DriverInstaller *)object;
    if (g_fullInstaller == installer) {
        g_fullInstaller = NULL;
    }
    if (installer != NULL) {
        OsalMemFree(installer);
    }
}
