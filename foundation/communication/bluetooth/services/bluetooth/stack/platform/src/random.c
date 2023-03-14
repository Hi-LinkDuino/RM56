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

#include "platform/include/random.h"
#include <fcntl.h>
#include <unistd.h>
#include "platform/include/platform_def.h"

static int g_fd = -1;

static int GetRandomFd()
{
    if (g_fd >= 0) {
        return g_fd;
    }

    g_fd = open("/dev/random", O_RDONLY | O_NONBLOCK);
    if (g_fd == -1) {
        LOG_ERROR("Open random-device failed... error no: %{public}d.", errno);
        return -1;
    }

    return g_fd;
}

static void CloseRandomFd()
{
    if (g_fd == -1) {
        return;
    }
    (void)close(g_fd);
    g_fd = -1;
}

uint32_t RandomGenerate()
{
    uint32_t val = 0;
    int rfd = GetRandomFd();
    if (rfd != -1) {
        if (read(rfd, &val, sizeof(val)) == -1) {
            LOG_ERROR("Read random-device failed... error no: %{public}d.", errno);
        }
        CloseRandomFd();
    }
    return val;
}