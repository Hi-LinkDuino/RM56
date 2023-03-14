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

#include <cstring>
#include <iostream>
#include <queue>
#include <vector>
#include <regex>
#include <unordered_map>
#include "hilog/log.h"
#include "format.h"
#include "log_controller.h"
#include "properties.h"
#include "log_display.h"

namespace OHOS {
namespace HiviewDFX {
using namespace std;

using hash_t = std::uint64_t;
unordered_map<uint16_t, std::string> errorMsg
{
    {RET_FAIL, "Unexpected error"},
    {ERR_LOG_LEVEL_INVALID, "Invalid log level, the valid log levels include D/I/W/E/F"},
    {ERR_LOG_TYPE_INVALID, "Invalid log type, the valid log types include app/core/init/kmsg"},
    {ERR_QUERY_TYPE_INVALID, "Query condition on both types and excluded types is undefined or\
    queryTypes can not contain app/core/init and kmsg at the same time"},
    {ERR_QUERY_LEVEL_INVALID, "Query condition on both levels and excluded levels is undefined"},
    {ERR_QUERY_DOMAIN_INVALID, "Invalid domain format, a hexadecimal number is needed"},
    {ERR_QUERY_TAG_INVALID, "Query condition on both tags and excluded tags is undefined"},
    {ERR_QUERY_PID_INVALID, "Query condition on both pid and excluded pid is undefined"},
    {ERR_BUFF_SIZE_EXP, "Buffer resize exception"},
    {ERR_LOG_PERSIST_FILE_SIZE_INVALID, "Invalid log persist file size, file size should be not less than "
    + to_string(MAX_PERSISTER_BUFFER_SIZE)},
    {ERR_LOG_PERSIST_FILE_NAME_INVALID, "Invalid log persist file name, file name should not contain [\\/:*?\"<>|]"},
    {ERR_LOG_PERSIST_COMPRESS_BUFFER_EXP, "Invalid Log persist compression buffer"},
    {ERR_LOG_PERSIST_FILE_PATH_INVALID, "Invalid persister file path or persister directory does not exist"},
    {ERR_LOG_PERSIST_COMPRESS_INIT_FAIL, "Log persist compression initialization failed"},
    {ERR_LOG_PERSIST_FILE_OPEN_FAIL, "Log persist open file failed"},
    {ERR_LOG_PERSIST_MMAP_FAIL, "Log persist mmap failed"},
    {ERR_LOG_PERSIST_JOBID_FAIL, "Log persist jobid not exist"},
    {ERR_LOG_PERSIST_TASK_FAIL, "Log persist task is existed"},
    {ERR_DOMAIN_INVALID, "Invalid domain, domain should not be  more than 0 and less than "
    + to_string(DOMAIN_MAX_SCOPE)},
    {ERR_MEM_ALLOC_FAIL, "Alloc memory failed"},
    {ERR_MSG_LEN_INVALID, "Invalid message length, message length should be not more than "
    + to_string(MSG_MAX_LEN)},
    {ERR_PRIVATE_SWITCH_VALUE_INVALID, "Invalid private switch value, valid:on/off"},
    {ERR_FLOWCTRL_SWITCH_VALUE_INVALID, "Invalid flowcontrl switch value, valid:pidon/pidoff/domainon/domainoff"},
    {ERR_LOG_PERSIST_JOBID_INVALID, "Invalid jobid, jobid should be more than 0"},
    {ERR_LOG_CONTENT_NULL, "Log content NULL"},
    {ERR_COMMAND_NOT_FOUND, "Command not found"},
    {ERR_FORMAT_INVALID, "Invalid format parameter"},
    {ERR_BUFF_SIZE_INVALID, "Invalid buffer size, buffer size should be more than 0 and less than "
    + to_string(MAX_BUFFER_SIZE)},
    {ERR_COMMAND_INVALID, "Invalid command, only one control command can be executed each time"},
    {ERR_KMSG_SWITCH_VALUE_INVALID, "Invalid kmsg switch value, valid:on/off"},
    {ERR_LOG_FILE_NUM_INVALID, "Invalid log number, log number should be more than " + to_string(MIN_LOG_FILE_NUM)
    + " and less than " + to_string(MAX_LOG_FILE_NUM)},
};

string ParseErrorCode(ErrorCode errorCode)
{
    if (errorMsg.count(errorCode) == 0) {
        cout << "ERR_CODE not exist" << endl;
    }
    string errorMsgStr = "[ERR_CODE:" + to_string(errorCode) + "], " + errorMsg[errorCode];
    return errorMsgStr;
}

hash_t Hash(char const *str)
{
    hash_t ret {BASIS};
    if (str == nullptr) {
        return ret;
    }
    while (*str) {
        ret ^= *str;
        ret *= PRIME;
        str++;
    }
    return ret;
}
constexpr hash_t HashCompileTime(char const *str, hash_t lastValue = BASIS)
{
    if (str == nullptr) {
        return lastValue;
    }
    return *str ? HashCompileTime(str + 1, (*str ^ lastValue) * PRIME) : lastValue;
}
constexpr unsigned long long operator "" _hash(char const *p, size_t)
{
    return HashCompileTime(p);
}

string GetLogTypeStr(uint16_t logType)
{
    string logTypeStr = "invalid";
    if (logType == LOG_INIT) {
        logTypeStr = "init";
    }
    if (logType == LOG_CORE) {
        logTypeStr = "core";
    }
    if (logType == LOG_APP) {
        logTypeStr = "app";
    }
    if (logType == LOG_KMSG) {
        logTypeStr = "kmsg";
    }
    return logTypeStr;
}

string GetOrigType(uint16_t shiftType)
{
    string logType = "";
    if (((1 << LOG_INIT) & shiftType) != 0) {
        logType += "init,";
    }
    if (((1 << LOG_CORE) & shiftType) != 0) {
        logType += "core,";
    }
    if (((1 << LOG_APP) & shiftType) != 0) {
        logType += "app,";
    }
    if (((1 << LOG_KMSG) & shiftType) != 0) {
        logType += "kmsg,";
    }
    logType.erase(logType.end() - 1);
    return logType;
}

string GetPressAlgStr(uint16_t pressAlg)
{
    string pressAlgStr;
    if (pressAlg == COMPRESS_TYPE_ZSTD) {
        pressAlgStr = "zstd";
    }
    if (pressAlg == COMPRESS_TYPE_ZLIB) {
        pressAlgStr = "zlib";
    }
    return pressAlgStr;
}

string GetByteLenStr(uint64_t buffSize)
{
    string buffSizeStr;
    if (buffSize < ONE_KB) {
        buffSizeStr += to_string(buffSize);
        buffSizeStr += "B";
    } else if (buffSize < ONE_MB) {
        buffSize = buffSize / ONE_KB;
        buffSizeStr += to_string(buffSize);
        buffSizeStr += "K";
    } else if (buffSize < ONE_GB) {
        buffSize = buffSize / ONE_MB;
        buffSizeStr += to_string(buffSize);
        buffSizeStr += "M";
    } else if (buffSize < ONE_TB) {
        buffSize = buffSize / ONE_GB;
        buffSizeStr += to_string(buffSize);
        buffSizeStr += "G";
    } else {
        buffSize = buffSize / ONE_TB;
        buffSizeStr += to_string(buffSize);
        buffSizeStr += "T";
    }
    return buffSizeStr;
}

/*
 * print control command operation result
 */
int32_t ControlCmdResult(const char* message)
{
    if (message == nullptr) {
        return RET_FAIL;
    }
    MessageHeader* msgHeader =
        reinterpret_cast<MessageHeader*>(const_cast<char*>(message));
    uint8_t msgCmd = msgHeader->msgType;
    uint16_t msgLen = msgHeader->msgLen;
    string outputStr = "";
    uint32_t resultLen = 0;
    switch (msgCmd) {
        case MC_RSP_BUFFER_SIZE: {
            BufferSizeResponse* pBuffSizeRsp =
                reinterpret_cast<BufferSizeResponse*>(const_cast<char*>(message));
            if (!pBuffSizeRsp) {
                return RET_FAIL;
            }
            BuffSizeResult* pBuffSizeRst =
                reinterpret_cast<BuffSizeResult*>(&pBuffSizeRsp->buffSizeRst);
            while (pBuffSizeRst && resultLen < msgLen) {
                if (pBuffSizeRst->result < 0) {
                    outputStr += GetLogTypeStr(pBuffSizeRst->logType);
                    outputStr += " buffer size fail\n";
                    outputStr += ParseErrorCode((ErrorCode)pBuffSizeRst->result);
                    outputStr += "\n";
                } else {
                    outputStr += GetLogTypeStr(pBuffSizeRst->logType);
                    outputStr += " buffer size is ";
                    outputStr += GetByteLenStr(pBuffSizeRst->buffSize);
                    outputStr += "\n";
                }
                pBuffSizeRst++;
                resultLen += sizeof(BuffSizeResult);
            }
            break;
        }
        case MC_RSP_BUFFER_RESIZE: {
            BufferResizeResponse* pBuffResizeRsp =
                reinterpret_cast<BufferResizeResponse*>(const_cast<char*>(message));
            if (!pBuffResizeRsp) {
                return RET_FAIL;
            }
            BuffResizeResult* pBuffResizeRst =
                reinterpret_cast<BuffResizeResult*>(&pBuffResizeRsp->buffResizeRst);
            while (pBuffResizeRst && resultLen < msgLen) {
                if (pBuffResizeRst->result < 0) {
                    outputStr += GetLogTypeStr(pBuffResizeRst->logType);
                    outputStr += " buffer resize fail\n";
                    outputStr += ParseErrorCode((ErrorCode)pBuffResizeRst->result);
                    outputStr += "\n";
                } else {
                    outputStr += GetLogTypeStr(pBuffResizeRst->logType);
                    outputStr += " buffer size is ";
                    outputStr += GetByteLenStr(pBuffResizeRst->buffSize);
                    outputStr += "\n";
                    SetBufferSize((LogType)pBuffResizeRst->logType, true, (size_t)pBuffResizeRst->buffSize);
                }
                pBuffResizeRst++;
                resultLen += sizeof(BuffSizeResult);
            }
            break;
        }
        case MC_RSP_STATISTIC_INFO_QUERY: {
            StatisticInfoQueryResponse* staInfoQueryRsp =
                reinterpret_cast<StatisticInfoQueryResponse*>(const_cast<char*>(message));
            string logOrDomain;
            if (!staInfoQueryRsp) {
                return RET_FAIL;
            }
            if (staInfoQueryRsp->domain != 0xffffffff) {
                logOrDomain = to_string(staInfoQueryRsp->domain);
            } else {
                logOrDomain = GetLogTypeStr(staInfoQueryRsp->logType);
            }
            if (staInfoQueryRsp->result == RET_SUCCESS) {
                outputStr += logOrDomain;
                outputStr += " print log length is ";
                outputStr += GetByteLenStr(staInfoQueryRsp->printLen);
                outputStr += "\n";
                outputStr += logOrDomain;
                outputStr += " cache log length is ";
                outputStr += GetByteLenStr(staInfoQueryRsp->cacheLen);
                outputStr += "\n";
                outputStr += logOrDomain;
                outputStr += " dropped log lines is ";
                outputStr += GetByteLenStr(staInfoQueryRsp->dropped);
            } else if (staInfoQueryRsp->result < 0) {
                outputStr += logOrDomain;
                outputStr += " statistic info query fail\n";
                outputStr += ParseErrorCode((ErrorCode)staInfoQueryRsp->result);
            }
            break;
        }
        case MC_RSP_STATISTIC_INFO_CLEAR: {
            StatisticInfoClearResponse* staInfoClearRsp =
                reinterpret_cast<StatisticInfoClearResponse*>(const_cast<char*>(message));
            string logOrDomain;
            if (!staInfoClearRsp) {
                return RET_FAIL;
            }
            if (staInfoClearRsp->domain != 0xffffffff) {
                logOrDomain = to_string(staInfoClearRsp->domain);
            } else {
                logOrDomain = GetLogTypeStr(staInfoClearRsp->logType);
            }
            if (staInfoClearRsp->result == RET_SUCCESS) {
                outputStr += logOrDomain;
                outputStr += " statistic info clear success ";
            } else if (staInfoClearRsp->result < 0) {
                outputStr += logOrDomain;
                outputStr += " statistic info clear fail\n";
                outputStr += ParseErrorCode((ErrorCode)staInfoClearRsp->result);
            }
            break;
        }
        case MC_RSP_LOG_CLEAR: {
            LogClearResponse* pLogClearRsp =
                reinterpret_cast<LogClearResponse*>(const_cast<char*>(message));
            if (!pLogClearRsp) {
                return RET_FAIL;
            }
            LogClearResult* pLogClearRst = reinterpret_cast<LogClearResult*>(&pLogClearRsp->logClearRst);
            while (pLogClearRst && resultLen < msgLen) {
                if (pLogClearRst->result < 0) {
                    outputStr += GetLogTypeStr(pLogClearRst->logType);
                    outputStr += " log clear fail\n";
                    outputStr += ParseErrorCode((ErrorCode)pLogClearRst->result);
                    outputStr += "\n";
                } else {
                    outputStr += GetLogTypeStr(pLogClearRst->logType);
                    outputStr += " log clear success ";
                    outputStr += "\n";
                }
                pLogClearRst++;
                resultLen += sizeof(LogClearResult);
            }
            break;
        }
        case MC_RSP_LOG_PERSIST_START: {
            LogPersistStartResponse* pLogPersistStartRsp =
                reinterpret_cast<LogPersistStartResponse*>(const_cast<char*>(message));
            if (!pLogPersistStartRsp) {
                return RET_FAIL;
            }
            LogPersistStartResult* pLogPersistStartRst =
                reinterpret_cast<LogPersistStartResult*>(&pLogPersistStartRsp->logPersistStartRst);
            while (pLogPersistStartRst && resultLen < msgLen) {
                if (pLogPersistStartRst->result < 0) {
                    outputStr += "Persist task [jobid:";
                    outputStr += to_string(pLogPersistStartRst->jobId);
                    outputStr += "] start failed\n";
                    outputStr += ParseErrorCode((ErrorCode)pLogPersistStartRst->result);
                } else {
                    outputStr += "Persist task [jobid:";
                    outputStr += to_string(pLogPersistStartRst->jobId);
                    outputStr += "] started successfully\n";
                }
                pLogPersistStartRst++;
                resultLen += sizeof(LogPersistStartResult);
            }
            break;
        }
        case MC_RSP_LOG_PERSIST_STOP: {
            LogPersistStopResponse* pLogPersistStopRsp =
                reinterpret_cast<LogPersistStopResponse*>(const_cast<char*>(message));
            if (!pLogPersistStopRsp) {
                return RET_FAIL;
            }
            LogPersistStopResult* pLogPersistStopRst =
                reinterpret_cast<LogPersistStopResult*>(&pLogPersistStopRsp->logPersistStopRst);
            while (pLogPersistStopRst && resultLen < msgLen) {
                if (pLogPersistStopRst->result < 0) {
                    outputStr += "Persist task [jobid:";
                    outputStr += to_string(pLogPersistStopRst->jobId);
                    outputStr += "] stop failed\n";
                    outputStr += ParseErrorCode((ErrorCode)pLogPersistStopRst->result);
                } else {
                    outputStr += "Persist task [jobid:";
                    outputStr += to_string(pLogPersistStopRst->jobId);
                    outputStr += "] stopped successfully\n";
                }
                pLogPersistStopRst++;
                resultLen += sizeof(LogPersistStopResult);
            }
            break;
        }
        case MC_RSP_LOG_PERSIST_QUERY: {
            LogPersistQueryResponse* pLogPersistQueryRsp =
                reinterpret_cast<LogPersistQueryResponse*>(const_cast<char*>(message));
            if (!pLogPersistQueryRsp) {
                return RET_FAIL;
            }
            LogPersistQueryResult* pLogPersistQueryRst =
                reinterpret_cast<LogPersistQueryResult*>(&pLogPersistQueryRsp->logPersistQueryRst);
            while (pLogPersistQueryRst && resultLen < msgLen) {
                if (pLogPersistQueryRst->result < 0) {
                    outputStr = "Persist task [logtype:";
                    outputStr += GetLogTypeStr(pLogPersistQueryRst->logType);
                    outputStr += "] query failed\n";
                    outputStr += ParseErrorCode((ErrorCode)pLogPersistQueryRst->result);
                } else {
                    outputStr += to_string(pLogPersistQueryRst->jobId);
                    outputStr += " ";
                    outputStr += GetOrigType(pLogPersistQueryRst->logType);
                    outputStr += " ";
                    outputStr += GetPressAlgStr(pLogPersistQueryRst->compressAlg);
                    outputStr += " ";
                    outputStr += pLogPersistQueryRst->filePath;
                    outputStr += " ";
                    outputStr += to_string(pLogPersistQueryRst->fileSize);
                    outputStr += " ";
                    outputStr += to_string(pLogPersistQueryRst->fileNum);
                    outputStr += "\n";
                }
                pLogPersistQueryRst++;
                resultLen += sizeof(LogPersistQueryResult);
            }
            break;
        }
        default:
            break;
    }
    cout << outputStr << endl;
    return 0;
}

HilogShowFormat HilogFormat(const char* formatArg)
{
    static HilogShowFormat format;
    if (formatArg == nullptr) {
        exit(1);
    }
    switch (Hash(formatArg)) {
        case "color"_hash:
            format = COLOR_SHOWFORMAT;
            break;
        case "colour"_hash:
            format = COLOR_SHOWFORMAT;
            break;
        case "time"_hash:
            format = TIME_SHOWFORMAT;
            break;
        case "usec"_hash:
            format = TIME_USEC_SHOWFORMAT;
            break;
        case "nsec"_hash:
            format = TIME_NSEC_SHOWFORMAT;
            break;
        case "year"_hash:
            format = YEAR_SHOWFORMAT;
            break;
        case "zone"_hash:
            format = ZONE_SHOWFORMAT;
            break;
        case "epoch"_hash:
            format = EPOCH_SHOWFORMAT;
            break;
        case "monotonic"_hash:
            format = MONOTONIC_SHOWFORMAT;
            break;
        default:
            cout << ParseErrorCode(ERR_FORMAT_INVALID) << endl;
            exit(1);
    }
    return format;
}

/*
 * Match the logs according to the regular expression
*/
bool HilogMatchByRegex(string context, string regExpArg)
{
    smatch regExpSmatch;
    regex regExp(regExpArg);
    return regex_search(context, regExpSmatch, regExp);
}

void HilogShowLog(uint32_t showFormat, HilogDataMessage* data, const HilogArgs* context,
    vector<string>& tailBuffer)
{
    if (data == nullptr) {
        return;
    }
    if (data->sendId == SENDIDN) {
        return;
    }
    if (data->length == 0) {
        std::cout << ParseErrorCode(ERR_LOG_CONTENT_NULL) << endl;
        return;
    }
    static int printHeadCnt = 0;
    HilogShowFormatBuffer showBuffer;
    const char* content = data->data + data->tag_len;

    if (context == nullptr) {
        return;
    }
    if (context->headLines) {
        if (printHeadCnt++ >= context->headLines) {
            exit(1);
        }
    }
    if (context->regexArgs != "") {
        string str = content;
        if (!HilogMatchByRegex(str, context->regexArgs)) {
            return;
        }
    }

    char buffer[MAX_LOG_LEN + MAX_LOG_LEN];
    showBuffer.level = data->level;
    showBuffer.pid = data->pid;
    showBuffer.tid = data->tid;
    showBuffer.domain = data->domain;
    showBuffer.tag_len = data->tag_len;
    showBuffer.tv_sec = data->tv_sec;
    showBuffer.tv_nsec = data->tv_nsec;
    auto offset = data->tag_len;
    const char *dataBegin = data->data + offset;
    char *dataPos = data->data + offset;
    while (*dataPos != 0) {
        if (*dataPos == '\n') {
            if (dataPos != dataBegin) {
                *dataPos = 0;
                showBuffer.tag_len = offset;
                showBuffer.data = data->data;
                HilogShowBuffer(buffer, MAX_LOG_LEN + MAX_LOG_LEN, showBuffer, showFormat);
                if (context->tailLines) {
                    tailBuffer.emplace_back(buffer);
                    return;
                } else {
                    cout << buffer << endl;
                }
                offset += dataPos - dataBegin + 1;
            } else {
                offset++;
            }
            dataBegin = dataPos + 1;
        }
        dataPos++;
    }
    if (dataPos != dataBegin) {
        showBuffer.data = data->data;
        HilogShowBuffer(buffer, MAX_LOG_LEN + MAX_LOG_LEN, showBuffer, showFormat);
        if (context->tailLines) {
            tailBuffer.emplace_back(buffer);
            return;
        } else {
            cout << buffer << endl;
        }
    }
    return;
}
} // namespace HiviewDFX
} // namespace OHOS
