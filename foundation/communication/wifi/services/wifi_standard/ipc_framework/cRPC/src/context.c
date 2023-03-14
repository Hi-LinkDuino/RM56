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

#include "context.h"

Context *CreateContext(int capacity)
{
    if (capacity < CONTEXT_BUFFER_MIN_SIZE) {
        capacity = CONTEXT_BUFFER_MIN_SIZE;
    } else if (capacity > CONTEXT_BUFFER_MAX_SIZE) {
        capacity = CONTEXT_BUFFER_MAX_SIZE;
    }
    Context *context = (Context *)calloc(1, sizeof(Context));
    if (context == NULL) {
        return NULL;
    }
    context->rCapacity = capacity;
    context->wCapacity = capacity;
    context->szRead = (char *)calloc(capacity, sizeof(char));
    if (context->szRead == NULL) {
        free(context);
        context = NULL;
        return NULL;
    }
    context->szWrite = (char *)calloc(capacity, sizeof(char));
    if (context->szWrite == NULL) {
        free(context->szRead);
        free(context);
        context = NULL;
        return NULL;
    }
    context->cSplit = '\t';
    context->cMsgEnd = "$$$$$$";
    return context;
}

void ReleaseContext(Context *context)
{
    if (context == NULL) {
        return;
    }

    if (context != NULL) {
        free(context->szRead);
        free(context->szWrite);
        free(context);
        context = NULL;
    }
}

/**
 * when context's read/write buffer is less then request len
 * expand the buffer size
 */
static int ExpandReadCache(Context *context, int len)
{
    if (context == NULL) {
        return -1;
    }

    int left = (context->rBegin <= context->rEnd) ? (context->rCapacity - 1 - context->rEnd + context->rBegin)
                                                  : (context->rBegin - context->rEnd - 1);
    if (left < len) {
        int capacity = context->rCapacity;
        while (left < len) {
            capacity += context->rCapacity;
            left += context->rCapacity;
        }
        char *p = (char *)calloc(capacity, sizeof(char));
        if (p == NULL) {
            return -1;
        }
        if (memmove_s(p, capacity, context->szRead, context->rCapacity) != EOK) {
            free(p);
            p = NULL;
            return -1;
        }
        if (context->rBegin > context->rEnd &&
            memmove_s(p + context->rCapacity, context->rCapacity, p, context->rEnd) != EOK) {
            free(p);
            p = NULL;
            return -1;
        }
        char *pFree = context->szRead;
        context->szRead = p;
        if (context->rBegin > context->rEnd) {
            context->rEnd += context->rCapacity;
        }
        context->rCapacity = capacity;
        free(pFree);
        pFree = NULL;
    }
    return 0;
}

static int ExpandWriteCache(Context *context, int len)
{
    if (context == NULL) {
        return -1;
    }

    int left = (context->wBegin <= context->wEnd) ? (context->wCapacity - 1 - context->wEnd + context->wBegin)
                                                  : (context->wBegin - context->wEnd - 1);
    if (left < len) {
        int capacity = context->wCapacity;
        while (left < len) {
            capacity += context->wCapacity;
            left += context->wCapacity;
        }
        char *p = (char *)calloc(capacity, sizeof(char));
        if (p == NULL) {
            return -1;
        }
        if (context->wCapacity < 0) {
            free(p);
            p = NULL;
            return -1;
        }
        if (memmove_s(p, capacity, context->szWrite, context->wCapacity) != EOK) {
            free(p);
            p = NULL;
            return -1;
        }
        if (context->wBegin > context->wEnd &&
            memmove_s(p + context->wCapacity, context->wCapacity, p, context->wEnd) != EOK) {
            free(p);
            p = NULL;
            return -1;
        }
        char *pFree = context->szWrite;
        context->szWrite = p;
        if (context->wBegin > context->wEnd) {
            context->wEnd += context->wCapacity;
        }
        context->wCapacity = capacity;
        free(pFree);
        pFree = NULL;
    }
    return 0;
}

static int ContextAppendRead(Context *context, const char *buf, int len)
{
    if (context == NULL) {
        return -1;
    }

    if (ExpandReadCache(context, len) < 0) {
        return -1;
    }
    if (context->rEnd + len < context->rCapacity) {
        if (memmove_s(context->szRead + context->rEnd, context->rCapacity - context->rEnd, buf, len) != EOK) {
            return -1;
        }
        context->rEnd += len;
    } else {
        int tmp = context->rCapacity - context->rEnd;
        if (tmp > 0 && memmove_s(context->szRead + context->rEnd, tmp, buf, tmp) != EOK) {
            return -1;
        }
        if (tmp < len && memmove_s(context->szRead, len - tmp, buf + tmp, len - tmp) != EOK) {
            return -1;
        }
        context->rEnd = len - tmp;
    }
    return 0;
}

int ContextAppendWrite(Context *context, const char *buf, int len)
{
    if (context == NULL) {
        return -1;
    }

    if (ExpandWriteCache(context, len) < 0) {
        return -1;
    }
    if (context->wEnd + len < context->wCapacity) {
        if (memmove_s(context->szWrite + context->wEnd, context->wCapacity - context->wEnd, buf, len) != EOK) {
            return -1;
        }
        context->wEnd += len;
    } else {
        int tmp = context->wCapacity - context->wEnd;
        if (tmp > 0 && memmove_s(context->szWrite + context->wEnd, tmp, buf, tmp) != EOK) {
            return -1;
        }
        if (tmp < len && memmove_s(context->szWrite, len - tmp, buf + tmp, len - tmp) != EOK) {
            return -1;
        }
        context->wEnd = len - tmp;
    }
    return 0;
}

char *ContextGetReadRecord(Context *context)
{
    if (context == NULL) {
        return NULL;
    }

    if (context->rBegin == context->rEnd) {
        return NULL;
    }
    int len = (context->rBegin <= context->rEnd) ? (context->rEnd - context->rBegin)
                                                 : (context->rCapacity - context->rBegin + context->rEnd);
    char *buf = (char *)calloc(len + 1, sizeof(char));
    if (buf == NULL) {
        return NULL;
    }
    if (context->rBegin < context->rEnd) {
        if (memmove_s(buf, len, context->szRead + context->rBegin, len) != EOK) {
            free(buf);
            return NULL;
        }
    } else {
        int tmp = context->rCapacity - context->rBegin;
        if (tmp > 0 && memmove_s(buf, len + 1, context->szRead + context->rBegin, tmp) != EOK) {
            free(buf);
            return NULL;
        }
        if (context->rEnd > 0 && memmove_s(buf + tmp, len + 1 - tmp, context->szRead, context->rEnd) != EOK) {
            free(buf);
            return NULL;
        }
    }
    buf[len] = 0;
    char *p = strstr(buf, context->cMsgEnd);
    if (p == NULL) {
        free(buf);
        return NULL;
    }
    *p = 0;
    int num = p - buf + strlen(context->cMsgEnd);
    context->rBegin += num;
    if (context->rBegin >= context->rCapacity) {
        context->rBegin -= context->rCapacity;
    }
    return buf;
}

int ContextReadNet(Context *context)
{
    if (context == NULL) {
        return -1;
    }

    char line[MAX_ONE_LINE_SIZE] = {0};
    int ret = MyRead(context->fd, line, sizeof(line) - 1);
    if (ret == SOCK_ERR) {
        return SOCK_ERR;
    }
    int len = strlen(line);
    if (len > 0 && ContextAppendRead(context, line, len) < 0) {
        return -1;
    }
    return ret;
}

int ContextWriteNet(Context *context)
{
    if (context == NULL) {
        return -1;
    }

    if (context->wBegin == context->wEnd) {
        return 0;
    }
    if (context->wBegin < context->wEnd) {
        int ret = MyWrite(context->fd, context->szWrite + context->wBegin, context->wEnd - context->wBegin);
        if (ret > 0) {
            context->wBegin += ret;
        }
        return ret;
    }
    int len = context->wCapacity - context->wBegin;
    int ret = MyWrite(context->fd, context->szWrite + context->wBegin, len);
    if (ret < 0) {
        return ret;
    }
    if (ret < len) {
        context->wBegin += ret;
        return ret;
    }
    context->wBegin = 0;
    ret = MyWrite(context->fd, context->szWrite, context->wEnd);
    if (ret > 0) {
        context->wBegin = ret;
    }
    return ret;
}
