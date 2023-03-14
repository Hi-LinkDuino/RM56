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

#ifndef OHOS_WIFI_HAL_CRPC_BASE_H
#define OHOS_WIFI_HAL_CRPC_BASE_H

#include "server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Read int array
 *
 * @param context - read context
 * @param size - the array's size
 * @return int* - return int array
 */
int *ReadIntArray(Context *context, int size);

/**
 * @Description Read string array
 *
 * @param context - read context
 * @param size - the array's size
 * @return char** - return string array
 */
char **ReadCharArray(Context *context, int size);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              GetName and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcGetName(RpcServer *server, Context *context);

/**
 * @Description Parse the context to obtain data. Call the corresponding function
 *              GetType and assemble the function to obtain data.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcGetType(RpcServer *server, Context *context);

#ifdef __cplusplus
}
#endif
#endif