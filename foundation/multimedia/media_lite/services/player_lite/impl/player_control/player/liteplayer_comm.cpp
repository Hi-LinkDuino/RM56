/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "liteplayer_comm.h"
#include <sys/time.h>
#include <unistd.h>
#ifdef __LITE__
#include <los_ld_elflib.h>
#else
#include <dlfcn.h>
#endif
#include "liteplayer.h"

using OHOS::Media::PlayerControl;

const long long AV_NS2MS_SCALE = 1000000ll;
const long long AV_SEC2MS_SCALE = 1000ll;

int32_t PlayerControlOnEvent(void* priv, EventCbType event, int32_t ext1, int32_t ext2)
{
    PlayerControl *player = reinterpret_cast<PlayerControl *>(priv);
    if (player == nullptr) {
        return -1;
    }
    return player->OnPlayControlEvent(priv, event);
}

uint64_t PlayerControlGetCurRelativeTime()
{
    struct timespec ts = { 0, 0 };
    (void)clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t curTime = ((uint64_t)ts.tv_sec) * AV_SEC2MS_SCALE + ((uint64_t)ts.tv_nsec) / AV_NS2MS_SCALE;
    return curTime;
}

