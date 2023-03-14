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

#include <cstring>

#include "client_executor/include/i_aie_client.inl"
#include "utils/aie_client_const.h"
#include "utils/log/aie_log.h"
#include "utils/utils.h"

using namespace OHOS::AI;

namespace {
    const int INT_1 = 1;
    const int INT_2 = 2;
    const int INT_1024 = 1024;
    const int INT_65535 = 65535;
}

/**
 * Constructs ConfigInfo parameters.
 */
void GetConfigInfo(ConfigInfo &configInfo)
{
    configInfo = {.description = CONFIG_DESCRIPTION};
}

/**
 * Constructs ConfigInfo parameters，including specified description.
 */
void GetConfigInfo2(ConfigInfo &configInfo, const char *description)
{
    configInfo = {.description = description};
}

/**
 * Constructs ClientInfo parameters.
 */
void GetClientInfo(ClientInfo &clientInfo)
{
    const char *str = CLIENT_EXTEND_MSG;
    char *extendMsg = const_cast<char*>(str);
    int len = strlen(str) + 1;

    clientInfo = {
        .clientVersion = CLIENT_VERSION_VALID,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = INVALID_SESSION_ID,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = len,
        .extendMsg = (unsigned char*)extendMsg,
    };
}

/**
 * Constructs AlgorithmInfo parameters.
 */
void GetSyncAlgorithmInfo(AlgorithmInfo &algorithmInfo, bool isAsync, int algorithmType)
{
    const char *str = ALGORITHM_EXTEND_MSG;
    char *extendMsg = const_cast<char*>(str);
    int extendLen = strlen(str) + 1;

    algorithmInfo = {
        .clientVersion = CLIENT_VERSION_VALID,
        .isAsync = isAsync,
        .algorithmType = algorithmType,
        .algorithmVersion = ALGORITHM_VERSION_VALID,
        .isCloud = GetRandomBool(),
        .operateId = GetRandomInt(INT_65535),
        .requestId = GetRandomInt(INT_65535),
        .extendLen = extendLen,
        .extendMsg = (unsigned char*)extendMsg,
    };
}

/**
 * Constructs DataInfo.
 */
DataInfo GetDataInfo(bool isDataInfoNull = true, const char *dataString = DATA_INFO_DEFAULT)
{
    // Sets default dataInfo to null.
    DataInfo dataInfo = {
        .data = nullptr,
        .length = 0,
    };

    // Sets dataInfo to specified value.
    if (!isDataInfoNull) {
        const char *str = dataString;
        char *data = const_cast<char*>(str);
        int length = strlen(str) + 1;

        dataInfo = {
            .data = reinterpret_cast<unsigned char *>(data),
            .length = length,
        };
    }

    return dataInfo;
}

/**
 * Constructs DataInfo with a big length data.
 */
DataInfo GetBigDataInfo(bool isDataInfoNull = true)
{
    // Sets default dataInfo to null.
    DataInfo dataInfo = {
        .data = nullptr,
        .length = 0,
    };

    // Sets dataInfo to specified value.
    if (!isDataInfoNull) {
#ifdef __LINUX__
        int length = INT_2 * INT_1024 * INT_1024; // 2 MB long data, the unit is Byte here.
#else // liteos device has no enough remaining memory to contain 2MB long data.
        int length = INT_1 * INT_1024 * INT_1024; // 1 MB long data, the unit is Byte here.
#endif

        char *data = reinterpret_cast<char *>(malloc(length));

        dataInfo = {
            .data = reinterpret_cast<unsigned char *>(data),
            .length = length,
        };
    }

    return dataInfo;
}

/**
 * Release DataInfo.
 */
void FreeDataInfo(DataInfo &dataInfo)
{
    if (dataInfo.data != nullptr) {
        free(dataInfo.data);
        dataInfo.data = nullptr;
        dataInfo.length = 0;
    }
}