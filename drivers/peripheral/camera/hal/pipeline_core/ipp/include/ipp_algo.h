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

#ifndef HOS_CAMERA_IPP_ALGO_H
#define HOS_CAMERA_IPP_ALGO_H

#ifdef __cplusplus
extern "C" {
#endif

enum IppAlgoMode {
    IPP_ALGO_MODE_BEGIN,
    IPP_ALGO_MODE_NORMAL = IPP_ALGO_MODE_BEGIN,
    IPP_ALGO_MODE_BEAUTY,
    IPP_ALGO_MODE_HDR,
    IPP_ALGO_MODE_END
};

typedef struct IppAlgoMeta {
    int mode;
    int face_rect[4][2];
    char reserved[64];
} IppAlgoMeta;

typedef struct IppAlgoBuffer {
    void* addr;
    unsigned int width;
    unsigned int height;
    unsigned int stride;
    unsigned int size;
    int id;
} IppAlgoBuffer;

typedef struct IppAlgoFunc {
    int (*Init)(IppAlgoMeta* meta);
    int (*Start)();
    int (*Flush)();
    int (*Process)(IppAlgoBuffer* inBuffer[], int inBufferCount, IppAlgoBuffer* outBuffer, IppAlgoMeta* meta);
    int (*Stop)();
} IppAlgoFunc;

typedef int (*AlgoFuncInit)(IppAlgoMeta *);
typedef int (*AlgoFuncStart)();
typedef int (*AlgoFuncFlush)();
typedef int (*AlgoFuncProcess)(IppAlgoBuffer* inBuffer[],
    int inBufferCount, IppAlgoBuffer* outBuffer, IppAlgoMeta* meta);
typedef int (*AlgoFuncStop)();

#ifdef __cplusplus
}
#endif

#endif
