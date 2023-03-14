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

#include <stdio.h>
#include "ipp_algo.h"
#include "securec.h"

#define MAX_BUFFER_COUNT 100

int Init(const IppAlgoMeta *meta)
{
    printf("ipp algo example Init ...\n");
    return 0;
}

int Start(void)
{
    printf("ipp algo example Start ...\n");
    return 0;
}

int Flush(void)
{
    printf("ipp algo example Flush ...\n");
    return 0;
}

int Process(IppAlgoBuffer *inBuffer[], int inBufferCount, IppAlgoBuffer *outBuffer, const IppAlgoMeta *meta)
{
    printf("ipp algo example Process ...\n");
    if (inBuffer == NULL || inBufferCount > MAX_BUFFER_COUNT) {
        printf("inBuffer is NULL\n");
        return -1;
    }

    for (int i = 0; i < inBufferCount; i++) {
        if (inBuffer[i] != NULL) {
            printf("in buffer addr = %p, width = %u, height = %u, stride = %u, size = %u, id = %d\n", inBuffer[i]->addr,
                   inBuffer[i]->width, inBuffer[i]->height, inBuffer[i]->stride, inBuffer[i]->size, inBuffer[i]->id);
        } else {
            printf("in buffer %d is NULL\n", i);
        }
    }

    if (outBuffer != NULL && outBuffer->addr != NULL) {
        printf("out buffer addr = %p, size = %u, id = %d\n", outBuffer->addr, outBuffer->size, outBuffer->id);
    } else {
        printf("out buffer is NULL\n");
    }

    if (inBufferCount == 1) {
        if (inBuffer[0] == NULL || outBuffer == NULL || inBuffer[0]->addr == NULL || outBuffer->addr == NULL) {
            return 0;
        }
        char *in = (char*)(inBuffer[0]->addr);
        char *out = (char*)(outBuffer->addr);
        if (memcpy_s(out, outBuffer->size, in, outBuffer->size) != 0) {
            printf("memcpy_s failed.");
        }
        return 0;
    }

    if (inBufferCount == 2) { // 2:2 inports
        if (inBuffer[0] == NULL || inBuffer[1] == NULL || inBuffer[0]->addr == NULL || inBuffer[1]->addr == NULL) {
            return -1;
        }
        printf("example algo merge 2 camera images\n");
        // format is yuv422
        char *startBuffer1 = (char*)(inBuffer[0]->addr) + inBuffer[0]->stride * inBuffer[0]->height;
        char *startBuffer2 = (char*)(inBuffer[1]->addr) + inBuffer[1]->stride * inBuffer[1]->height;
        if (memcpy_s(startBuffer1, inBuffer[0]->size - inBuffer[0]->stride * inBuffer[0]->height,
            startBuffer2, inBuffer[0]->stride * inBuffer[0]->height) != 0) {
                printf("memcpy_s failed.");
            }
        return 0;
    }
    return 0;
}

int Stop(void)
{
    printf("ipp algo example Stop ...\n");
    return 0;
}
