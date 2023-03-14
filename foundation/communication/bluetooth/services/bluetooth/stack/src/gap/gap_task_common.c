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

#include "gap_def.h"
#include "gap_task_internal.h"

#include "allocator.h"

#include "btm/btm_thread.h"

void GapBlockInTaskProcess(void *ctx)
{
    GapRunTaskBlockInfo *info = ctx;
    if (info->func != NULL) {
        info->func(info->ctx);
    }
    if (info->event != NULL) {
        EventSet(info->event);
    }
}

NO_SANITIZE("cfi") void GapUnBlockInTaskProcess(void *ctx)
{
    GapRunTaskUnBlockInfo *info = ctx;
    if (info != NULL) {
        if (info->func != NULL) {
            info->func(info->ctx);
        }
        if (info->free != NULL) {
            info->free(info->ctx);
        }
        if (info->ctx != NULL) {
            MEM_MALLOC.free(info->ctx);
        }
        MEM_MALLOC.free(info);
    }
}

int GapRunTaskBlockProcess(void (*func)(void *), void *ctx)
{
    GapRunTaskBlockInfo *info = MEM_MALLOC.alloc(sizeof(GapRunTaskBlockInfo));
    if (info == NULL) {
        return BT_NO_MEMORY;
    }

    info->event = EventCreate(true);
    info->ctx = ctx;
    info->func = func;

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_GAP, GapBlockInTaskProcess, info);
    if (ret == BT_NO_ERROR) {
        ret = EventWait(info->event, WAIT_TIME);
        if (ret == EVENT_WAIT_TIMEOUT_ERR) {
            ret = BT_TIMEOUT;
        } else if (ret == EVENT_WAIT_OTHER_ERR) {
            ret = BT_OS_ERROR;
        }
    }

    EventDelete(info->event);
    MEM_MALLOC.free(info);
    return ret;
}

int GapRunTaskUnBlockProcess(void (*const func)(void *), void *ctx, void (*const free)(void *))
{
    GapRunTaskUnBlockInfo *info = MEM_MALLOC.alloc(sizeof(GapRunTaskUnBlockInfo));
    if (info == NULL) {
        return BT_NO_MEMORY;
    }

    info->ctx = ctx;
    info->func = func;
    info->free = free;

    int ret = BTM_RunTaskInProcessingQueue(PROCESSING_QUEUE_ID_GAP, GapUnBlockInTaskProcess, info);

    return ret;
}
