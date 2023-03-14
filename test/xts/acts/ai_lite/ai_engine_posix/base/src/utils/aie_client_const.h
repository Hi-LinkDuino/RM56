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

#ifndef AIE_CLIENT_CONST_H
#define AIE_CLIENT_CONST_H

#include <string>

#include "protocol/plugin_config/aie_algorithm_type.h"

/**
 *  ConfigInfo
 */
const char * const CONFIG_DESCRIPTION = "Config information";

/**
 *  ClientInfo
 */
const int CLIENT_VERSION_VALID = 1;
const int CLIENT_VERSION_INVALID = 2;
const int CLIENT_ID_INIT = -1;
const int SESSION_ID_INIT = -1;
const char * const CLIENT_EXTEND_MSG = "Client extend message";

/**
 *  AlgorithmInfo
 */
// 0, sync demo plugin defined in aie_algorithm_type.h
const int ALGORITHM_TYPE_SYNC = OHOS::AI::ALGORITHM_TYPE_SAMPLE_PLUGIN_1;
// 1, async demo plugin defined in aie_algorithm_type.h
const int ALGORITHM_TYPE_ASYNC = OHOS::AI::ALGORITHM_TYPE_SAMPLE_PLUGIN_2;
const int ALGORITHM_TYPE_INVALID = 66;
const int ALGORITHM_VERSION_VALID = 1;
const char * const ALGORITHM_EXTEND_MSG = "Algorithm extend message";

/**
 *  DataInfo
 */
const char * const DATA_INFO_DEFAULT = "Default data info";
const char * const INPUT_INFO_PREPARE = "Prepare input data";
const char * const INPUT_INFO_SYNC_PROCESS = "Sync process input data";
const char * const INPUT_INFO_ASYNC_PROCESS = "Async process input data";
const char * const INPUT_INFO_RELEASE = "Release input data";
const char * const INPUT_INFO_SET_OPTION = "Set option input data";
const char * const INPUT_INFO_GET_OPTION = "Get option input data";

/**
 *  Consts for Performance Test of AieClient
 */
const int EXECUTE_TIMES = 100;
const std::string INTERFACE_GROUP_TAG = "AieClient";
const std::string AIE_CLIENT_PERF_RESULT_FILEPATH = "AieClientPerformanceTest.txt";
const std::string AIE_CLIENT_PERF_RESULT_FILEPATH_BAK = "AieClientPerformanceTestBak.txt";
const bool isSavePerfResult = true;

#endif // AIE_CLIENT_CONST_H