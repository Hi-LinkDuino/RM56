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

#ifndef AIE_CLIENT_COMMON_H
#define AIE_CLIENT_COMMON_H

#include <cstring>

#include "client_executor/include/i_aie_client.inl"
#include "utils/aie_client_const.h"
#include "utils/log/aie_log.h"
#include "utils/utils.h"

/**
 * Constructs ConfigInfo parameters.
 */
void GetConfigInfo(ConfigInfo &configInfo);

/**
 * Constructs ConfigInfo parameters，including specified description.
 */
void GetConfigInfo2(ConfigInfo &configInfo, const char *description);

/**
 * Constructs ClientInfo parameters.
 */
void GetClientInfo(ClientInfo &clientInfo);

/**
 * Constructs AlgorithmInfo parameters.
 */
void GetSyncAlgorithmInfo(AlgorithmInfo &algorithmInfo, bool isAsync, int algorithmType);

/**
 * Constructs DataInfo.
 */
DataInfo GetDataInfo(bool isDataInfoNull = true, const char *dataString = DATA_INFO_DEFAULT);

/**
 * Constructs DataInfo with a big length data.
 */
DataInfo GetBigDataInfo(bool isDataInfoNull = true);

/**
 * Release DataInfo.
 */
void FreeDataInfo(DataInfo &dataInfo);

#endif