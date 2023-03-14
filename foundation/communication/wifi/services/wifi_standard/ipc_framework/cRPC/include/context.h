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

#ifndef CRPC_CONTEXT_H
#define CRPC_CONTEXT_H

#include "common.h"
#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONTEXT_BUFFER_MIN_SIZE (1024)
#define CONTEXT_BUFFER_MAX_SIZE (1024000)

typedef struct Context Context;
struct Context {
    int fd;
    char *szRead;        /* read buffer */
    unsigned int rCapacity;       /* read buffer size */
    unsigned int rBegin;          /* current read pos */
    unsigned int rEnd;            /* current read end pos */
    char *szWrite;       /* write buffer */
    unsigned int wCapacity;       /* write buffer size */
    unsigned int wBegin;          /* current write pos */
    unsigned int wEnd;            /* current write end pos */
    char *oneProcess;    /* when deal message, copy message into here */
    unsigned int nPos;            /* deal message, read pos */
    unsigned int nSize;           /* deal message's total size */
    char cSplit;         /* message split character flag */
    const char *cMsgEnd; /* message end characters flag */
};

/**
 * @Description Create a Context object
 *
 * @param capacity - Context buffer size
 * @return Context* - context pointer or NULL if failed
 */
Context *CreateContext(int capacity);

/**
 * @Description Release Context
 *
 * @param context - Context object's pointer
 */
void ReleaseContext(Context *context);

/**
 * @Description Read message
 *
 * @param context - Context object's pointer
 * @return int - >= 0 success; other read failed
 */
int ContextReadNet(Context *context);

/**
 * @Description Write message
 *
 * @param context - Context object's pointer
 * @return int - Number of records which is written.
 */
int ContextWriteNet(Context *context);

/**
 * @Description Append buff into context's write cache
 *
 * @param context - Context object's pointer
 * @param buf - input message buffer
 * @param len - message size
 * @return int - 0 success; -1 append failed
 */
int ContextAppendWrite(Context *context, const char *buf, int len);

/**
 * @Description Get message from context read cache. When context read from network,
 *              it first appends read message into it's read cache, and then judge if
 *              has read a complete message. This function returns the message.
 *
 * @param context - Context object's pointer
 * @return char* - a complete message or NULL if not exist
 */
char *ContextGetReadRecord(Context *context);

#ifdef __cplusplus
}
#endif
#endif