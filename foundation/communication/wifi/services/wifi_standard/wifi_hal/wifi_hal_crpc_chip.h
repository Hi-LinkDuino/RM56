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

#ifndef OHOS_WIFI_HAL_CRPC_CHIP_H
#define OHOS_WIFI_HAL_CRPC_CHIP_H

#include "server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              GetWifiChip and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcGetWifiChip(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              GetWifiChipIds and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcGetWifiChipIds(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              GetChipId and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcGetChipId(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              CreateIface and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcCreateIface(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              GetIface and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcGetIface(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              GetIfaceNames and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcGetIfaceNames(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              RemoveIface and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcRemoveIface(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              GetCapabilities and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcGetCapabilities(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              GetSupportedComboModes and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcGetSupportedComboModes(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              ConfigComboModes and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcConfigComboModes(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              GetComboModes and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcGetComboModes(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              RequestFirmwareDebugDump and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcRequestFirmwareDebugDump(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              SetPowerMode and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcSetPowerMode(RpcServer *server, Context *context);

#ifdef __cplusplus
}
#endif
#endif