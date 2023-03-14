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

#include "wifi_hal_crpc_base.h"
#include "wifi_hal_base_interface.h"
#include "wifi_hal_define.h"

int *ReadIntArray(Context *context, int size)
{
    if (size <= 0) {
        return NULL;
    }

    int *pArray = (int *)calloc(size, sizeof(int));
    if (pArray == NULL) {
        return NULL;
    }
    for (int i = 0; i < size; ++i) {
        if (ReadInt(context, pArray + i) < 0) {
            free(pArray);
            pArray = NULL;
            return NULL;
        }
    }
    return pArray;
}

char **ReadCharArray(Context *context, int size)
{
    char **pArray = NULL;
    if (size > 0) {
        pArray = (char **)calloc(size, sizeof(char *));
    }
    if (pArray == NULL) {
        return NULL;
    }
    int i = 0;
    for (; i < size; ++i) {
        int len = 0;
        if (ReadInt(context, &len) < 0) {
            break;
        }
        pArray[i] = (char *)calloc(len + 1, sizeof(char));
        if (pArray[i] == NULL) {
            break;
        }
        if (ReadStr(context, pArray[i], len + 1) < 0) {
            break;
        }
    }
    if (i < size) {
        for (int j = 0; j < size; ++j) {
            free(pArray[j]);
            pArray[j] = NULL;
        }
        free(pArray);
        pArray = NULL;
        return NULL;
    }
    return pArray;
}

int RpcGetName(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int size = 0;
    if (ReadInt(context, &size) < 0 || size <= 0) {
        return -1;
    }
    char *ifname = (char *)calloc(size, sizeof(char));
    if (ifname == NULL) {
        return -1;
    }
    WifiErrorNo err = GetName(ifname, size);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteStr(context, ifname);
    }
    WriteEnd(context);
    free(ifname);
    ifname = NULL;
    return 0;
}

int RpcGetType(RpcServer *server, Context *context)
{
    if (server == NULL || context == NULL) {
        return -1;
    }
    int type = 0;
    WifiErrorNo err = GetType(&type);
    WriteBegin(context, 0);
    WriteInt(context, err);
    if (err == WIFI_HAL_SUCCESS) {
        WriteInt(context, type);
    }
    WriteEnd(context);
    return 0;
}