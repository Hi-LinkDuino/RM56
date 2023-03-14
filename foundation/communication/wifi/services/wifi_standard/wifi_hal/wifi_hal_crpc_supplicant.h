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

#ifndef OHOS_WIFI_HAL_CRPC_SUPPLICANT_H
#define OHOS_WIFI_HAL_CRPC_SUPPLICANT_H

#include "server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              StartSupplicant and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcStartSupplicant(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              StopSupplicant and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcStopSupplicant(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              ConnectSupplicant and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcConnectSupplicant(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              DisconnectSupplicant and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcDisconnectSupplicant(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              RequestToSupplicant and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcRequestToSupplicant(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              SetPowerSave and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcSetPowerSave(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              WpaSetCountryCode and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcWpaSetCountryCode(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              WpaGetCountryCode and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcWpaGetCountryCode(RpcServer *server, Context *context);

#ifdef __cplusplus
}
#endif
#endif