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
#include <inttypes.h>
#include "serial.h"

const int TMP_CHAR_LEN = 64;
const int HIGH_TWO_BIT = 16;
const int ALPHA_NUM_MAX = 10;

int WriteBegin(Context *context, int type)
{
    if (context == NULL) {
        return -1;
    }

    char szTmp[TMP_CHAR_LEN] = {0};
    if (snprintf_s(szTmp, sizeof(szTmp), sizeof(szTmp) - 1, "%s%c", ((type == 0) ? "N" : "C"), context->cSplit) < 0) {
        return -1;
    }
    int len = strlen(szTmp);
    return ContextAppendWrite(context, szTmp, len);
}

int WriteFunc(Context *context, const char *funcName)
{
    if (context == NULL || funcName == NULL) {
        return -1;
    }

    int len = strlen(funcName) + 1;
    if (len <= 0) {
        return -1;
    }
    char *buf = (char *)calloc(len + 1, sizeof(char));
    if (buf == NULL) {
        return -1;
    }
    if (snprintf_s(buf, len + 1, len, "%s%c", funcName, context->cSplit) < 0) {
        free(buf);
        buf = NULL;
        return -1;
    }
    int ret = ContextAppendWrite(context, buf, len);
    free(buf);
    buf = NULL;
    return ret;
}

int WriteInt(Context *context, int iData)
{
    if (context == NULL) {
        return -1;
    }

    char szTmp[TMP_CHAR_LEN] = {0};
    if (snprintf_s(szTmp, sizeof(szTmp), sizeof(szTmp) - 1, "%d%c", iData, context->cSplit) < 0) {
        return -1;
    }
    return ContextAppendWrite(context, szTmp, strlen(szTmp));
}

int WriteLong(Context *context, long lData)
{
    if (context == NULL) {
        return -1;
    }

    char szTmp[TMP_CHAR_LEN] = {0};
    if (snprintf_s(szTmp, sizeof(szTmp), sizeof(szTmp) - 1, "%ld%c", lData, context->cSplit) < 0) {
        return -1;
    }
    return ContextAppendWrite(context, szTmp, strlen(szTmp));
}

int WriteInt64(Context *context, int64_t iData)
{
    if (context == NULL) {
        return -1;
    }

    char szTmp[TMP_CHAR_LEN] = {0};
    if (snprintf_s(szTmp, sizeof(szTmp), sizeof(szTmp) - 1, "%" PRIu64 "%c", iData, context->cSplit) < 0) {
        return -1;
    }
    return ContextAppendWrite(context, szTmp, strlen(szTmp));
}

int WriteDouble(Context *context, double dData)
{
    if (context == NULL) {
        return -1;
    }

    char szTmp[TMP_CHAR_LEN] = {0};
    if (snprintf_s(szTmp, sizeof(szTmp), sizeof(szTmp) - 1, "%.6lf%c", dData, context->cSplit) < 0) {
        return -1;
    }
    return ContextAppendWrite(context, szTmp, strlen(szTmp));
}

int WriteChar(Context *context, char cData)
{
    if (context == NULL) {
        return -1;
    }

    char szTmp[TMP_CHAR_LEN] = {0};
    if (snprintf_s(szTmp, sizeof(szTmp), sizeof(szTmp) - 1, "%c%c", cData, context->cSplit) < 0) {
        return -1;
    }
    return ContextAppendWrite(context, szTmp, strlen(szTmp));
}

int WriteStr(Context *context, const char *pStr)
{
    if (context == NULL || pStr == NULL) {
        return -1;
    }

    int len = strlen(pStr) + 1;
    if (len <= 0) {
        return -1;
    }
    char *buf = (char *)calloc(len + 1, sizeof(char));
    if (buf == NULL) {
        return -1;
    }
    if (snprintf_s(buf, len + 1, len, "%s%c", pStr, context->cSplit) < 0) {
        free(buf);
        buf = NULL;
        return -1;
    }
    int ret = ContextAppendWrite(context, buf, len);
    free(buf);
    buf = NULL;
    return ret;
}

int WriteUStr(Context *context, const unsigned char *uStr, unsigned int len)
{
    if (context == NULL || uStr == NULL) {
        return -1;
    }

    int inLen = (len << 1) + 1;
    char *buf = (char *)calloc(inLen + 1, sizeof(char));
    if (buf == NULL) {
        return -1;
    }
    int pos = 0;
    for (unsigned int i = 0; i < len; ++i) {
        pos = (i << 1);
        if (snprintf_s(buf + pos, inLen - pos, inLen - pos - 1, "%02x", uStr[i]) < 0) {
            free(buf);
            buf = NULL;
            return -1;
        }
    }
    buf[inLen - 1] = context->cSplit;
    buf[inLen] = 0;
    int ret = ContextAppendWrite(context, buf, inLen);
    free(buf);
    buf = NULL;
    return ret;
}

int WriteEnd(Context *context)
{
    if (context == NULL) {
        return -1;
    }

    return ContextAppendWrite(context, context->cMsgEnd, strlen(context->cMsgEnd));
}

static int ReadNext(Context *context)
{
    if (context == NULL) {
        return -1;
    }

    if (context->oneProcess == NULL || context->nPos >= context->nSize) {
        return -1;
    }
    char *p = context->oneProcess + context->nPos;
    char *q = p;
    while (*p != context->cSplit && *p != '\0') {
        ++p;
    }
    if (*p != context->cSplit) {
        return -1;
    }
    return p - q;
}

int ReadFunc(Context *context, char *funcName, int count)
{
    if (context == NULL) {
        return -1;
    }

    int len = ReadNext(context);
    if (len < 0) {
        return len;
    }
    if (len >= count) {
        return len;
    }
    if (strncpy_s(funcName, count, context->oneProcess + context->nPos, len) != EOK) {
        return -1;
    }
    context->nPos += len + 1;
    return 0;
}

static int IsValidNumberChar(const char *szNum, int size)
{
    int dotNum = 0; /* '.' num */
    for (int i = 0; i < size; ++i) {
        if (szNum[i] == '.') {
            ++dotNum;
        } else if ((szNum[i] < '0' || szNum[i] > '9') && szNum[i] != '+' && szNum[i] != '-') {
            return -1;
        }
    }
    if (dotNum > 1) {
        return -1;
    }
    return 0;
}

static int ReadNextNum(Context *context, char *szNum, int size)
{
    if (context == NULL) {
        return -1;
    }

    int len = ReadNext(context);
    if (len < 0) {
        return len;
    }
    if (len >= size) {
        return SERIAL_READ_TOO_LONG;
    }
    if (strncpy_s(szNum, size, context->oneProcess + context->nPos, len) != EOK) {
        return -1;
    }
    if (IsValidNumberChar(szNum, len) != 0) {
        return SERIAL_READ_INVALID_DIGIT;
    }
    return len;
}

int ReadInt(Context *context, int *iData)
{
    if (context == NULL) {
        return -1;
    }

    char szTmp[TMP_CHAR_LEN] = {0};
    int len = ReadNextNum(context, szTmp, sizeof(szTmp));
    if (len < 0) {
        return len;
    }
    *iData = atoi(szTmp);
    context->nPos += len + 1;
    return 0;
}

int ReadLong(Context *context, long *pLong)
{
    if (context == NULL || pLong == NULL) {
        return -1;
    }

    char szTmp[TMP_CHAR_LEN] = {0};
    int len = ReadNextNum(context, szTmp, sizeof(szTmp));
    if (len < 0) {
        return len;
    }
    *pLong = atol(szTmp);
    context->nPos += len + 1;
    return 0;
}

int ReadInt64(Context *context, int64_t *pInt64)
{
    if (context == NULL) {
        return -1;
    }

    char szTmp[TMP_CHAR_LEN] = {0};
    int len = ReadNextNum(context, szTmp, sizeof(szTmp));
    if (len < 0) {
        return len;
    }
    *pInt64 = atoll(szTmp);
    context->nPos += len + 1;
    return 0;
}

int ReadDouble(Context *context, double *dData)
{
    if (context == NULL || dData == NULL) {
        return -1;
    }

    char szTmp[TMP_CHAR_LEN] = {0};
    int len = ReadNextNum(context, szTmp, sizeof(szTmp));
    if (len < 0) {
        return len;
    }
    *dData = atof(szTmp);
    context->nPos += len + 1;
    return 0;
}

int ReadChar(Context *context, char *cData)
{
    if (context == NULL) {
        return -1;
    }

    int len = ReadNext(context);
    if (len < 0) {
        return len;
    }
    if (len != 1) {
        return SERIAL_READ_TOO_LONG;
    }
    *cData = *(context->oneProcess + context->nPos);
    context->nPos += len + 1;
    return 0;
}

int ReadStr(Context *context, char *str, int count)
{
    if (context == NULL) {
        return -1;
    }

    int len = ReadNext(context);
    if (len < 0) {
        return len;
    }
    if (str == NULL || len >= count) {
        return len;
    }
    if (strncpy_s(str, count, context->oneProcess + context->nPos, len) != EOK) {
        return -1;
    }
    context->nPos += len + 1;
    return 0;
}

int ReadUStr(Context *context, unsigned char *uStr, int count)
{
    if (context == NULL || uStr == NULL) {
        return -1;
    }

    int len = ReadNext(context);
    if (len < 0) {
        return len;
    }
    if ((len & 1) != 0) { /* format invalid: uStr message len must be an even number */
        return -1;
    }
    if ((len >> 1) >= count) {
        return (len >> 1);
    }
    int i = 0;
    int j = 0;
    char *q = context->oneProcess + context->nPos;
    for (; i < len; ++i) {
        unsigned char byte = 0;
        if (q[i] >= '0' && q[i] <= '9') {
            byte += HIGH_TWO_BIT * (q[i] - '0');
        } else {
            byte += HIGH_TWO_BIT * (q[i] - 'a' + ALPHA_NUM_MAX); /* WriteUStr uses lowercase letter, so use 'a' */
        }
        ++i;
        if (q[i] >= '0' && q[i] <= '9') {
            byte += q[i] - '0';
        } else {
            byte += q[i] - 'a' + ALPHA_NUM_MAX;
        }
        uStr[j++] = byte;
    }
    uStr[j] = 0;
    context->nPos += len + 1;
    return 0;
}
