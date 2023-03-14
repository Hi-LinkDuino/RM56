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

#ifndef HILOGTOOL_MSG_H
#define HILOGTOOL_MSG_H

#include <cstdint>
#include <string>
#include <stdint.h>
#include <time.h>
#include "hilog_common.h"

#define FILE_PATH_MAX_LEN 100
#define JOB_ID_ALL 0xffffffff
using OperationCmd = enum {
    LOG_QUERY_REQUEST = 0x01,
    LOG_QUERY_RESPONSE,
    NEXT_REQUEST,
    NEXT_RESPONSE,
    NEW_DATA_NOTIFY,
    MC_REQ_LOG_PERSIST_START,
    MC_RSP_LOG_PERSIST_START,
    MC_REQ_LOG_PERSIST_STOP,
    MC_RSP_LOG_PERSIST_STOP,
    MC_REQ_LOG_PERSIST_QUERY,
    MC_RSP_LOG_PERSIST_QUERY,
    MC_REQ_BUFFER_RESIZE,        // set buffer request
    MC_RSP_BUFFER_RESIZE,        // set buffer response
    MC_REQ_BUFFER_SIZE,          // query buffer size
    MC_RSP_BUFFER_SIZE,          // query buffer size
    MC_REQ_STATISTIC_INFO_QUERY, // statistic info query request
    MC_RSP_STATISTIC_INFO_QUERY, // statistic info query response
    MC_REQ_STATISTIC_INFO_CLEAR, // statistic info clear request
    MC_RSP_STATISTIC_INFO_CLEAR, // statistic info clear response
    MC_REQ_FLOW_CONTROL,         // set flow control request
    MC_RSP_FLOW_CONTROL,         // set flow control response
    MC_REQ_LOG_CLEAR,            // clear log request
    MC_RSP_LOG_CLEAR             // clear log response
};

/*
 * property operation, such as private switch ,log level, flow switch
 */
using OperateType = enum {
    OT_PRIVATE_SWITCH = 0x01,
    OT_KMSG_SWITCH,
    OT_LOG_LEVEL,
    OT_FLOW_SWITCH,
};

using PersisterResponse = enum {
    CREATE_SUCCESS = 1,
    CREATE_DUPLICATE = 2,
    CREATE_DENIED = 3,
    QUERY_SUCCESS = 4,
    QUERY_NOT_EXIST = 5,
    DELETE_SUCCESS = 6,
    DELETE_DENIED = 7,
};

using CompressAlg = enum {
    COMPRESS_TYPE_NONE = 0,
    COMPRESS_TYPE_ZSTD,
    COMPRESS_TYPE_ZLIB,
};

using HilogShowFormat = enum {
    OFF_SHOWFORMAT = 0,
    COLOR_SHOWFORMAT,
    TIME_SHOWFORMAT,
    TIME_USEC_SHOWFORMAT,
    YEAR_SHOWFORMAT,
    ZONE_SHOWFORMAT,
    EPOCH_SHOWFORMAT,
    MONOTONIC_SHOWFORMAT,
    TIME_NSEC_SHOWFORMAT,
};

using MessageHeader = struct {
    uint8_t version;
    uint8_t msgType;  // hilogtool-hilogd message type
    uint16_t msgLen;  // message length
};

using LogQueryRequest = struct {
    MessageHeader header;
    uint8_t nPid;
    uint8_t nNoPid;
    uint8_t nDomain;
    uint8_t nNoDomain;
    uint8_t nTag;
    uint8_t nNoTag;
    uint8_t levels;
    uint16_t types;
    uint32_t pids[MAX_PIDS];
    uint32_t domains[MAX_DOMAINS];
    char tags[MAX_TAGS][MAX_TAG_LEN];
    uint16_t logCount;
    uint8_t noLevels;
    uint16_t noTypes;
    uint32_t noPids[MAX_PIDS];
    uint32_t noDomains[MAX_DOMAINS];
    char noTags[MAX_TAGS][MAX_TAG_LEN];
};

using HilogDataMessage = struct {
    uint16_t sendId;
    uint16_t length; /* data len, equals tag_len plus content length, include '\0' */
    uint16_t level;
    uint16_t type;
    uint16_t tag_len; /* include '\0' */
    uint32_t pid;
    uint32_t tid;
    uint32_t domain;
    uint32_t tv_sec;
    uint32_t tv_nsec;
    char data[]; /* tag and content, include '\0' */
};

using LogQueryResponse = struct {
    MessageHeader header;
    HilogDataMessage data;
};

using NewDataNotify = struct {
    MessageHeader header;
};

using NextRequest = struct {
    MessageHeader header;
    uint16_t sendId;
};

using NextResponce = struct {
    MessageHeader header;
    uint16_t sendId;
    char data[];
};

using BuffSizeMsg = struct {
    uint16_t logType;
};

using BufferSizeRequest = struct {
    MessageHeader msgHeader;
    BuffSizeMsg buffSizeMsg[];
};

using BuffSizeResult = struct {
    uint16_t logType;
    uint64_t buffSize;
    int32_t result;
};

using BufferSizeResponse = struct {
    MessageHeader msgHeader;
    BuffSizeResult buffSizeRst[];
};

using BuffResizeMsg = struct {
    uint16_t logType;
    uint64_t buffSize;
};

using BufferResizeRequest = struct {
    MessageHeader msgHeader;
    BuffResizeMsg buffResizeMsg[];
};

using BuffResizeResult = struct {
    uint16_t logType;
    uint64_t buffSize;
    int32_t result;
};

using BufferResizeResponse = struct {
    MessageHeader msgHeader;
    BuffResizeResult buffResizeRst[];
};

using StatisticInfoQueryRequest = struct {
    MessageHeader msgHeader;
    uint16_t logType;
    uint32_t domain;
};

using StatisticInfoQueryResponse = struct {
    MessageHeader msgHeader;
    int32_t result;
    uint16_t logType;
    uint32_t domain;
    uint64_t printLen;
    uint64_t cacheLen;
    int32_t dropped;
};

using StatisticInfoClearRequest = struct {
    MessageHeader msgHeader;
    uint16_t logType;
    uint32_t domain;
};

using StatisticInfoClearResponse = struct {
    MessageHeader msgHeader;
    int32_t result;
    uint16_t logType;
    uint32_t domain;
};

using LogClearMsg = struct {
    uint16_t logType;
};

using LogClearRequest = struct {
    MessageHeader msgHeader;
    LogClearMsg logClearMsg[];
};

using LogClearResult = struct {
    uint16_t logType;
    int32_t result;
};

using LogClearResponse = struct {
    MessageHeader msgHeader;
    LogClearResult logClearRst[];
};

using LogPersistParam = struct {
    std::string logTypeStr;
    std::string compressAlgStr;
    std::string fileSizeStr;
    std::string fileNumStr;
    std::string fileNameStr;
    std::string jobIdStr;
};

using LogPersistStartMsg = struct {
    uint16_t logType; // union logType
    uint16_t compressAlg;
    char filePath[FILE_PATH_MAX_LEN];
    uint32_t fileSize;
    uint32_t fileNum;
    uint32_t jobId;
};

using LogPersistStartRequest = struct {
    MessageHeader msgHeader;
    LogPersistStartMsg logPersistStartMsg;
};

using LogPersistStartResult = struct {
    int32_t result;
    uint32_t jobId;
};

using LogPersistStartResponse = struct {
    MessageHeader msgHeader;
    LogPersistStartResult logPersistStartRst;
};

using LogPersistStopMsg = struct {
    uint32_t jobId;
};

using LogPersistStopRequest = struct {
    MessageHeader msgHeader;
    LogPersistStopMsg logPersistStopMsg[];
};

using LogPersistStopResult = struct {
    int32_t result;
    uint32_t jobId;
};

using LogPersistStopResponse = struct {
    MessageHeader msgHeader;
    LogPersistStopResult logPersistStopRst[];
};

using LogPersistQueryMsg = struct {
    uint16_t logType;
};

using LogPersistQueryRequest = struct {
    MessageHeader msgHeader;
    LogPersistQueryMsg logPersistQueryMsg;
};

using LogPersistQueryResult = struct {
    int32_t result;
    uint32_t jobId;
    uint16_t logType;
    uint16_t compressAlg;
    char filePath[FILE_PATH_MAX_LEN];
    uint32_t fileSize;
    uint32_t fileNum;
};

using LogPersistQueryResponse = struct {
    MessageHeader msgHeader;
    LogPersistQueryResult logPersistQueryRst[];
};

using SetPropertyParam = struct {
    std::string privateSwitchStr;
    std::string kmsgSwitchStr;
    std::string flowSwitchStr;
    std::string logLevelStr;
    std::string domainStr;
    std::string tagStr;
    std::string pidStr;
};

#endif /* HILOGTOOL_MSG_H */
