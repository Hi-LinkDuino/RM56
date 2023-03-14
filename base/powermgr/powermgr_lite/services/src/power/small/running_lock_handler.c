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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hilog_wrapper.h"
#include "power/suspend_ops.h"

static int32_t g_lockFd = -1;
static int32_t g_unlockFd = -1;

static int32_t OpenLockFile(const char* path)
{
    int32_t fd = open(path, O_RDWR);
    if (fd < 0) {
        POWER_HILOGE("Failed to open %{public}s, err: %{public}s", path, strerror(errno));
    }
    return fd;
}

static int32_t WriteLock(int32_t fd, const char *name)
{
    if ((fd < 0) || (name == NULL)) {
        POWER_HILOGE("Invalid parameter");
        return -1;
    }

    int32_t ret = write(fd, name, strlen(name));
    if (ret < 0) {
        POWER_HILOGE("Failed to write lock: %{public}s, error: %{public}s", name, strerror(errno));
        return -1;
    }
    return 0;
}

static void RunningLockAcquire(const char *name)
{
    int32_t ret = WriteLock(g_lockFd, name);
    POWER_HILOGD("Acquire runninglock: %{public}s, ret: %{public}d", name, ret);
}

static void RunningLockRelease(const char *name)
{
    int32_t ret = WriteLock(g_unlockFd, name);
    POWER_HILOGD("Release runninglock: %{public}s, ret: %{public}d", name, ret);
}

static struct RunningLockOps g_ops = {
    .Acquire = RunningLockAcquire,
    .Release = RunningLockRelease,
};

struct RunningLockOps* RunningLockOpsInit()
{
    g_lockFd = OpenLockFile("/proc/power/power_lock");
    if (g_lockFd < 0) {
        return NULL;
    }

    g_unlockFd = OpenLockFile("/proc/power/power_unlock");
    if (g_unlockFd < 0) {
        close(g_lockFd);
        g_lockFd = -1;
        return NULL;
    }

    return &g_ops;
}
