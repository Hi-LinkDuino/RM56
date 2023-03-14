/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef TEST_PROCESS_MSG_H
#define TEST_PROCESS_MSG_H

#include <cerrno>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#include "distributed_kv_data_manager.h"

typedef enum {
    CTRL_CODE_SOFTBUS_TYPE = 1000,
    CTRL_CODE_SOFTBUS_OPEN_SESSION,
    CTRL_CODE_DATAMGR_TYPE = 2000,
    CTRL_CODE_DATAMGR_GET_DATA,
    CTRL_CODE_DATAMGR_GET_DATA_REPLY,
    CTRL_CODE_DATAMGR_CREATE_KV,
    CTRL_CODE_DATAMGR_DELETE_KV,
    CTRL_CODE_DATAMGR_PUT_DATA,
    CTRL_CODE_DATAMGR_DELETE_DATA,
    CTRL_CODE_DM_TYPE = 3000,
    CTRL_CODE_FILEMGR_TYPE = 4000,
    CTRL_CODE_DP_TYPE = 5000,
    CTRL_CODE_SEC_TYPE = 6000,
    CTRL_CODE_MEDIA_TYPE = 7000,
    CTRL_CODE_RESULT_TYPE = 9999,
} CtrlCodeType;

#define LOG(format, ...)                                                              \
    do {                                                                              \
        time_t timeSec;                                                               \
        time(&timeSec);                                                               \
        struct tm tmRst;                                                              \
        localtime_r(&timeSec, &tmRst);                                                \
        char strTime[10];                                                             \
        strftime(strTime, sizeof(strTime), "%H:%M:%S", &tmRst);                       \
        fprintf(stdout, "[process-dataMsg] %s " format "\n", strTime, ##__VA_ARGS__); \
    } while (0)

int ProcessSoftBus(int code, char* recvData);
int ProcessDataMgr(int code, char* recvData);
int ProcessDP(int code, char* recvData);
int ProcessDM(int code, char* recvData);
int ProcessFileMgr(int code, char* recvData);
int ProcessSecMgr(int code, char* recvData);
int ProcessMediaMgr(int code, char* recvData);

int processDeletetData(char* putData);
int processGetData(char* putData);
int processPutData(char* putData);
int processCreateKv(char* putData);
int processDeleteKv(char* putData);
char* getRealData(char* str, char* delims);
void getParam(char* putData, char ret[][1024]);
void initKvstoreId(void);
#endif