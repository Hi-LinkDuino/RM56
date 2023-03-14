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

#ifndef CRPC_CLIENT_H
#define CRPC_CLIENT_H

#include "context.h"
#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RpcClient RpcClient;

/*
 * RPC CLIENT
 * RPC client sends a request and wait for a response from the server,
 * and process the callback function initiated by the server.
 * so we start a thread to get the server's reply message, judge message type,
 * and to deal reply or callback.
 * the client may process like this:
 * 1. thread: poll read ---> server reply msg ---> notify RemoteCall
 *    client: request functions --->RemoteCall ---> Wait Reply ---> Get Return
 * 2. thread: poll read ---> server callback msg ---> OnTransact
 *    client: OnTransact ---> deal event callback functions
 */

struct RpcClient {
    Context *context;
    int threadRunFlag;
    pthread_t threadId;
    int waitReply;
    pthread_mutex_t mutex;
    pthread_cond_t condW;
    int callLockFlag;
    pthread_mutex_t lockMutex;
    pthread_cond_t lockCond;
};

/**
 * @Description Create a Rpc Client object
 *
 * @param path - Unix domain communication file
 * @return RpcClient* - return client pointer or NULL
 */
RpcClient *CreateRpcClient(const char *path);

/**
 * @Description Release RpcClient object
 *
 * @param client - RpcClient object's pointer
 */
void ReleaseRpcClient(RpcClient *client);

/**
 * @Description Lock the RPC Client
 *
 * @param client - RpcClient object's pointer
 */
void LockRpcClient(RpcClient *client);

/**
 * @Description Unlock the RPC Client
 *
 * @param client - RpcClient object's pointer
 */
void UnlockRpcClient(RpcClient *client);

/**
 * @Description Send request message to server and wait response message
 *
 * @param client - RpcClient object's pointer
 * @return int 0 success, other is failed
 */
int RemoteCall(RpcClient *client);

/**
 * @Description End dealing response message
 *
 * @param client - RpcClient object's pointer
 */
void ReadClientEnd(RpcClient *client);

/**
 * @Description Deal callback messages. This function must be implemented when the RPC client is implemented
 *
 * @param context - pointer to the rpc context
 * @return int - 0 success; -1 deal message failed
 */
int OnTransact(Context *context);

#ifdef __cplusplus
}
#endif
#endif